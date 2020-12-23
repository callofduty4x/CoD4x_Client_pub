// steamcrap.cpp : Defines the entry point for the console application.
//
#include <stdint.h>
//#include <windows.h>
#include <stdio.h>
#include <wchar.h>
//#include <iostream>
//#include <list>
//#include <algorithm>
//#include <mutex>

#define CON_CHANNEL_SYSTEM 0x10


extern "C" {
	void Com_Printf(int channel, const char *fmt, ...);
	void Com_DPrintf(int channel, const char *fmt, ...);
	void Com_Error(int type, const char *fmt, ...);
	void Com_PrintError(int channel, const char *fmt, ...);
	int Com_sprintf(char *dest, int size, const char *fmt, ...);
	void Q_strncpyz( char *dest, const char *src, int destsize );
	void SV_SteamSetUserInfo(uint64_t steamid, int haslicense, int valid);
	void Com_Quit_f();
	int SV_GetSteamUserInfo(uint64_t* steamid, char** playername, int* score);
	const char* Sys_ExeFile(void);
	void Com_LogDebugData(uint8_t* data, int len);
}



#define STEAM_API_NODLL

	  
#undef __cdecl
#define __linux__
#define CALLBACK_GNUFIXUP
#include "steam_api.h"
#include "steam_gameserver.h"
#include "steam_apix.h"
#include "steam_shared.h"

#ifndef __cdecl
		#define __cdecl
#endif
//#define USE_GS_AUTH_API
//csid = gameserver->GetSteamID();

class SteamGameClientBridge;

char accounttype[] = {'I', 'U', 'M', 'G', 'A', 'P', 'C', 'g', 'c', '?', '?', '?', '?', '?', '?', '?'};

extern "C" void Com_SteamClientApi_DebugCallback(int nSeverity, const char *pchDebugText);





class SteamGameClientBridge
{
public:

	//Constructor
	SteamGameClientBridge( );

	// Destructor
	~SteamGameClientBridge();
	static SteamGameClientBridge* Init(  const wchar_t* dllpath, AppId_t parentAppID, AppId_t gameId, const char* modName, bool overlayrenderer);

	void RunFrame( );
	bool isAdvertisingServer;

	uint32 GetAuthTicket(unsigned char* data, int buflen);

	void CancelAuthTicket( );
	
private:
	//
	// Various callback functions that Steam will call to let us know about events related to our
	// connection to the Steam servers for authentication purposes.
	//

	// ipc failure handler
	STEAM_CALLBACK( SteamGameClientBridge, OnIPCFailure, IPCFailure_t, m_IPCFailureCallback );

	// Steam wants to shut down, Game for Windows applications should shutdown too
	STEAM_CALLBACK( SteamGameClientBridge, OnSteamShutdown, SteamShutdown_t, m_SteamShutdownCallback );
	
	// callback for when the lobby game server has started
	STEAM_CALLBACK( SteamGameClientBridge, OnGameJoinRequested, GameRichPresenceJoinRequested_t, m_GameJoinRequested );

	// callbacks for Steam connection state
	STEAM_CALLBACK( SteamGameClientBridge, OnSteamServersConnected, SteamServersConnected_t, m_SteamServersConnected );
	STEAM_CALLBACK( SteamGameClientBridge, OnSteamServersDisconnected, SteamServersDisconnected_t, m_SteamServersDisconnected );
	STEAM_CALLBACK( SteamGameClientBridge, OnSteamServerConnectFailure, SteamServerConnectFailure_t, m_SteamServerConnectFailure );
	STEAM_CALLBACK( SteamGameClientBridge, OnGameOverlayActivated, GameOverlayActivated_t, m_CallbackGameOverlayActivated );
	
	// callback when getting the results of a web call
	STEAM_CALLBACK( SteamGameClientBridge, OnGameWebCallback, GameWebCallback_t, m_CallbackGameWebCallback );
	// Function to tell Steam about our servers details
	
	void SendUpdatedServerDetailsToSteam();

	// Track whether our server is connected to Steam ok (meaning we can restrict who plays based on 
	// ownership and VAC bans, etc...)

	bool m_bConnectedToSteam;
	bool initialized;
	HAuthTicket m_hAuthTicket;

};

//-----------------------------------------------------------------------------
// Purpose: Handles notification of a steam ipc failure
// we may get multiple callbacks, one for each IPC operation we attempted
// since the actual failure, so protect ourselves from alerting more than once.
//-----------------------------------------------------------------------------
void SteamGameClientBridge::OnIPCFailure( IPCFailure_t *failure )
{
	static bool bExiting = false;
	if ( !bExiting )
	{
		Com_Error(0, "Steam IPC Failure, shutting down...\n" );
		bExiting = true;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Steam is asking us to join a game, based on the user selecting
//			'join game' on a friend in their friends list 
//			the string comes from the "connect" field set in the friends' rich presence
//-----------------------------------------------------------------------------
void SteamGameClientBridge::OnGameJoinRequested( GameRichPresenceJoinRequested_t *pCallback )
{
	Com_Printf(CON_CHANNEL_SYSTEM, "Steam want to connect me to: %s\n", pCallback->m_rgchConnect);
}

//-----------------------------------------------------------------------------
// Purpose: Handles notification of a Steam shutdown request since a Windows
// user in a second concurrent session requests to play this game. Shutdown
// this process immediately if possible.
//-----------------------------------------------------------------------------
void SteamGameClientBridge::OnSteamShutdown( SteamShutdown_t *callback )
{
	static bool bExiting = false;
	if ( !bExiting )
	{
		//OutputDebugString( "Steam shutdown request, shutting down\n" );
		Com_Quit_f();
		bExiting = true;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Handles notification that we are now connected to Steam
//-----------------------------------------------------------------------------
void SteamGameClientBridge::OnSteamServersConnected( SteamServersConnected_t *callback )
{
	if ( SteamUser()->BLoggedOn() )
		m_bConnectedToSteam = true;
	else
	{
		//OutputDebugString( "Steam: Got SteamServersConnected_t, but not logged on?\n" );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Handles notification that we are now connected to Steam
//-----------------------------------------------------------------------------
void SteamGameClientBridge::OnSteamServersDisconnected( SteamServersDisconnected_t *callback )
{
	m_bConnectedToSteam = false;
	Com_Printf( CON_CHANNEL_SYSTEM, "Steam: Got SteamServersDisconnected_t\n" );
}


//-----------------------------------------------------------------------------
// Purpose: Handles notification that the Steam overlay is shown/hidden, note, this
// doesn't mean the overlay will or will not draw, it may still draw when not active.
// This does mean the time when the overlay takes over input focus from the game.
//-----------------------------------------------------------------------------
void SteamGameClientBridge::OnGameOverlayActivated( GameOverlayActivated_t *callback )
{
/*	
	if ( callback->m_bActive )	
		Com_Printf( "Steam overlay now active\n" );
	else
		Com_Printf( "Steam overlay now inactive\n" );
	*/
}


//-----------------------------------------------------------------------------
// Purpose: Handle the callback from the user clicking a steam://gamewebcallback/ link in the overlay browser
//	You can use this to add support for external site signups where you want to pop back into the browser
//  after some web page signup sequence, and optionally get back some detail about that.
//-----------------------------------------------------------------------------
void SteamGameClientBridge::OnGameWebCallback( GameWebCallback_t *callback )
{
	//m_bSentWebOpen = false;
	Com_Printf(CON_CHANNEL_SYSTEM, "Steam: User submitted following url: %s\n", callback->m_szURL );
}


//-----------------------------------------------------------------------------
// Purpose: Handles notification that we are failed to connected to Steam
//-----------------------------------------------------------------------------
void SteamGameClientBridge::OnSteamServerConnectFailure( SteamServerConnectFailure_t *callback )
{
	Com_Printf(CON_CHANNEL_SYSTEM, "SteamServerConnectFailure_t: %d\n", callback->m_eResult );
	m_bConnectedToSteam = false;
}



//-----------------------------------------------------------------------------
// Purpose: Function which needs to getting processed on frame
//-----------------------------------------------------------------------------
void SteamGameClientBridge::RunFrame( )
{
	if(!initialized)
	{
		return;
	}

	SteamAPI_RunCallbacks();
}

//-----------------------------------------------------------------------------
// Purpose: Constructor -- note the syntax for setting up Steam API callback handlers
//-----------------------------------------------------------------------------
SteamGameClientBridge::SteamGameClientBridge(  ):
	m_IPCFailureCallback( this, &SteamGameClientBridge::OnIPCFailure ),
	m_SteamShutdownCallback( this, &SteamGameClientBridge::OnSteamShutdown ),
	m_GameJoinRequested( this, &SteamGameClientBridge::OnGameJoinRequested ),
	m_SteamServersConnected( this, &SteamGameClientBridge::OnSteamServersConnected ),
	m_SteamServersDisconnected( this, &SteamGameClientBridge::OnSteamServersDisconnected ),
	m_SteamServerConnectFailure( this, &SteamGameClientBridge::OnSteamServerConnectFailure ),
	m_CallbackGameOverlayActivated( this, &SteamGameClientBridge::OnGameOverlayActivated ),
	m_CallbackGameWebCallback( this, &SteamGameClientBridge::OnGameWebCallback )
{

	m_bConnectedToSteam = false;
	initialized = true;
	m_hAuthTicket = 0;

}



SteamGameClientBridge* SteamGameClientBridge::Init(  const wchar_t* dllpath, AppId_t parentAppID, AppId_t gameId, const char* modName, bool overlayrenderer )
{

	if(SteamAPI_InitX(dllpath, parentAppID, gameId, modName, overlayrenderer ) == false){
		Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed\n");
		return NULL;
	}
	
	SteamClient()->SetWarningMessageHook( &Com_SteamClientApi_DebugCallback );

	SteamGameClientBridge* isteamclientbridge = new SteamGameClientBridge(  );

	return isteamclientbridge;
}



//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
SteamGameClientBridge::~SteamGameClientBridge()
{

	// release our reference to the steam client library
	SteamAPI_Shutdown();
}

uint32 SteamGameClientBridge::GetAuthTicket(unsigned char* data, int buflen)
{
	uint32 writtenlen;
	
	if(m_hAuthTicket != k_HAuthTicketInvalid)
	{
		SteamUser()->CancelAuthTicket( m_hAuthTicket );	
		m_hAuthTicket = k_HAuthTicketInvalid;
	}

	m_hAuthTicket = SteamUser()->GetAuthSessionTicket( data, buflen, &writtenlen );

	//Com_Printf(CON_CHANNEL_SYSTEM, "Ticket %x, len %d\n", (unsigned int)m_hAuthTicket, writtenlen);


	if(writtenlen < 10)
	{
		if ( m_hAuthTicket != k_HAuthTicketInvalid )
		{
			SteamUser()->CancelAuthTicket( m_hAuthTicket );
			m_hAuthTicket = k_HAuthTicketInvalid;
		}
		return 0;
	}
	
	return writtenlen;
}

void SteamGameClientBridge::CancelAuthTicket( )
{
	if(m_hAuthTicket != k_HAuthTicketInvalid)
	{
		SteamUser()->CancelAuthTicket( m_hAuthTicket );	
		m_hAuthTicket = k_HAuthTicketInvalid;
	}
}


extern "C" {
	/* Begin now with client functions */
	static SteamGameClientBridge* steamclientbridge;
	
	int Com_SteamClientApi_Load( const wchar_t* dllpath, AppId_t appID, AppId_t gameId, const char* modName, int overlayrenderer)
	{
		steamclientbridge = SteamGameClientBridge::Init( dllpath, appID, gameId, modName, overlayrenderer);
		if(steamclientbridge == NULL)
		{
			return 0;
		}
		return 1;
	}
	
	void Com_SteamClientApi_Shutdown()
	{
		if(steamclientbridge == NULL)
		{
			return;
		}
		
		delete steamclientbridge;
		
		steamclientbridge = NULL;
	}
	
	int Com_SteamClientApi_IsLoaded()
	{
		if(steamclientbridge == NULL)
		{
			return 0;
		}
		return 1;
	}
	
	void Com_SteamClientApi_RunFrame( )
	{
		if(steamclientbridge == NULL){
			return;
		}
		steamclientbridge->RunFrame( );
	}
	

	
	void Com_SteamClientApi_SteamIDToString(uint64_t steamid, char* string, int length)
	{
		CSteamID *csteamid = new CSteamID(steamid);
		if(csteamid == NULL){
			string[0] = '\0';
		}else{
			Com_sprintf(string, length, "[%c:%u:%u]", accounttype[csteamid->GetEAccountType() % 16], csteamid->GetEUniverse(), csteamid->GetAccountID());
		}
		delete csteamid;
	}

	
	uint64_t Com_SteamClientApi_GetSteamID( )
	{
		if(steamclientbridge == NULL){
			return 0;
		}
		CSteamID csteamid;
		uint64_t steamid64;
		SteamUser()->GetSteamID( &csteamid );
		steamid64 = csteamid.ConvertToUint64();
		return steamid64;
	}
	
	void Com_SteamClientApi_AdvertiseServer(uint32_t unIPServer, uint16_t usPortServer)
	{
		if(steamclientbridge == NULL){
			return;
		}
		SteamUser()->AdvertiseGame( k_steamIDNonSteamGS, unIPServer, usPortServer );
		steamclientbridge->isAdvertisingServer = true;
	}
	void Com_SteamClientApi_StopAdvertisingServer(  )
	{
		if(steamclientbridge == NULL || steamclientbridge->isAdvertisingServer == false){
			return;
		}
		SteamUser()->AdvertiseGame( k_steamIDNonSteamGS, 0, 0 );
		steamclientbridge->isAdvertisingServer = false;
	}
	
	uint32 Com_SteamClientApi_GetAuthTicket(unsigned char* data, int buflen)
	{
		if(steamclientbridge == NULL){
			return 0;
		}
		return steamclientbridge->GetAuthTicket(data, buflen);
	}
	
	void Com_SteamClientApi_CancelAuthTicket( )
	{
		if(steamclientbridge == NULL){
			return;
		}
		steamclientbridge->CancelAuthTicket();
	}
	
	void Com_SteamClientApi_DebugCallback(int nSeverity, const char *pchDebugText)
	{
		Com_DPrintf(CON_CHANNEL_SYSTEM, "Steam: %s\n", pchDebugText);
	}
	
	void Com_SteamClientApi_GetLocalPlayerName(char* playername, int len)
	{
		const char *playerName = SteamFriends()->GetPersonaName();
		Q_strncpyz(playername, playerName, len);
	}

	int Com_SteamClientApi_GetClans(clan_t* clans, int maxcount)
	{
		int i;
		int clancount = SteamFriends()->GetClanCount();
		CSteamID clan;
		
		if(clancount < 0 || clancount > 128)
		{
			return 0;
		}
		
		if(maxcount < clancount)
		{
			clancount = maxcount;
		}
		
		for(i = 0; i < clancount; ++i)
		{
			SteamFriends()->GetClanByIndex( &clan, i );
			clans[i].clanid = clan.ConvertToUint64();
			Q_strncpyz(clans[i].clanname, SteamFriends()->GetClanName( clan ), sizeof(clans[i].clanname));
			Q_strncpyz(clans[i].clantag, SteamFriends()->GetClanTag( clan ), sizeof(clans[i].clantag));
		}
		return i;
	}
	
}
