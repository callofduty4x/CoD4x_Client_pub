#include "q_shared.h"
#include "qcommon.h"
#include "client.h"
#include "cl_input.h"
#include "ui_shared.h"
#include "win_sys.h"
#include "snd_system.h"
#include "httpftp.h"
#include "steam_interface.h"
#include "net_reliabletransport.h"
#include "cg_shared.h"
#include "crc.h"
#include "xzone.h"
#include "keys.h"
#include "sec_crypto.h"
#include "discord-rpc/discord_rpc.h"
#include "discord-rpc/discord_register.h"
#include "r_shared.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define cl_initOnce_unknown1_ADDR 0x956CF0
#define rconGlob_ADDR 0xE11558
#define ragdollFirstInit_ADDR 0xD5EC530
#define scr_initialized_ADDR 0xE31808
#define cl_serverStatusList_ADDR 0xe11588
#define KEVINOS

// DHM - Nerve :: Have we heard from the auto-update server this session?
#define autoupdateChecked *((qboolean*)(0x0c5f914))
#define statCount *((int*)(0x014E8900))
#define autoupdateStarted *((qboolean*)(0x08f4ccc))
#define wwwDlDisconnected *((qboolean*)(0x0c5b6a0))
#define	cl_serverLoadingMap *((byte*)(0x08f4cde))
#define	g_waitingForServer *((byte*)(0x08f4cdc))
#define	cl_waitingOnServerToLoadMap *((byte*)(0x8F4CDD))
#define g_FXPlaySoundCount *((int*)0x0CC9634C)

#ifdef FALLBACK_SIGNALING
qboolean cl_version17fallback;
#endif

constConfigstring_t *constantConfigStrings = (constConfigstring_t*)0x07209B0;
struct statmonitor_s *stats = (struct statmonitor_s *)0x14E88C8; //014E88C8     ; struct statmonitor_s stats[7]
byte *mutedClients = (byte*)0xe11518;

int authRequestTime;
static netadr_t rcondst;
static int wwwDownloadInProgress;
qboolean autoupdateDownloaded;
qboolean xassetlimitchanged;

cvar_t *cl_noprint;
cvar_t *onlinegame;
cvar_t *cl_hudDrawsBehindUI;
cvar_t *cl_voice;
cvar_t *cl_timeout;
cvar_t *cl_connectTimeout;
cvar_t *cl_connectionAttempts;
cvar_t *cl_shownet;
cvar_t *cl_shownuments;
cvar_t *cl_showServerCommands;
cvar_t *cl_showSend;
cvar_t *cl_showTimeDelta;
cvar_t *cl_freezeDemo;
cvar_t *cl_activeAction;
cvar_t *cl_avidemo;
cvar_t *cl_forceavidemo;
cvar_t *cl_yawspeed;
cvar_t *cl_pitchspeed;
cvar_t *cl_anglespeedkey;
cvar_t *cl_maxpackets;
cvar_t *cl_packetdup;
cvar_t *cl_sensitivity;
cvar_t *cl_mouseAccel;
cvar_t *cl_freelook;
cvar_t *cl_showMouseRate;
cvar_t *cl_allowDownload;
cvar_t *cl_wwwDownload;
cvar_t *cl_talking;
cvar_t *cl_inGameVideo;
cvar_t *cl_serverStatusResendTime;
cvar_t *cl_maxppf;
cvar_t *cl_bypassMouseInput;
cvar_t *m_pitch;
cvar_t *m_yaw;
cvar_t *m_forward;
cvar_t *m_side;
cvar_t *m_filter;
cvar_t *cl_motdString;
cvar_t *cl_ingame;
cvar_t *cl_name;
cvar_t *cl_nextdemo;
cvar_t *cl_updateavailable;
cvar_t *cl_updatefiles;
cvar_t *cl_updateoldversion;
cvar_t *cl_updateversion;
cvar_t *motd;
cvar_t *cl_vehDriverViewHeightMin;
cvar_t *cl_vehDriverViewHeightMax;
cvar_t *cl_demoplaying;
cvar_t *cl_lastdemo;
//Server cvar
cvar_t *g_gametype;
cvar_t *cl_replacementDlList;
cvar_t *sv_masterservers;
cvar_t *cl_password;
cvar_t *cl_updateservers;
cvar_t *cl_cod4xsitedom;
cvar_t *cl_filterlisturl;

typedef struct{
	byte state;
	byte unk[23];
	netadr_t rconHost;
	byte unk2[30];
}rconGlob_t;

void CL_RconInit(){
#define rconGlob (*((rconGlob_t*)(rconGlob_ADDR)))
  rconGlob.state = 0;
  rconGlob.rconHost.type = 1;
}


void Ragdoll_Register(){
#define ragdollFirstInit *((byte*)(ragdollFirstInit_ADDR))
	Ragdoll_InitCvars();
	Ragdoll_InitCommands();
	Cmd_ExecuteSingleCommand(0,0, "exec ragdoll.cfg");
	ragdollFirstInit = 1;
}

void SCR_Init(){
#define scr_initialized *((qboolean*)(scr_initialized_ADDR))
  scr_initialized = qtrue;

}

void CL_OpenMenuByName_f()
{
	if(Cmd_Argc() != 2)
	{
		Com_Printf(CON_CHANNEL_DONT_FILTER, "Usage: openmenu <name>\n");
	}
	Menus_OpenByName(Cmd_Argv(1));
}


void CL_SigTrap_f()
{
	__asm__("int $3");
}

void CL_DownloadLatestConfigurations();


//#ifdef COD4XDEV

	void UI_DumpMenus_f();

	void UI_DumpXAssets_f()
	{
/*		const char* type;

		if(Cmd_Argc() != 2)
		{
			Com_Printf("Usage: dumpxassets <typename>\n");
		}
		type = Cmd_Argv(1);

		if(Q_stricmp(type, "menus") == 0)
		{
			UI_DumpMenus_f();
			return;
		}
		Com_Printf("Unsupported typename for xassets");
*/
//		DumpXAssets(ASSET_TYPE_MENU);
	}
/*
	void UI_LoadMenu_f()
	{
		if(Cmd_Argc() != 2)
		{
			Com_Printf("Usage: loadmenu <mmenuname>\n");
		}
		const char* menuname = Cmd_Argv(1);
		XAsset_LoadAsset(21, menuname);
	}*/
//#endif



void CL_Vid_Restart_WithClNvidiaCleanup_f()
{
	Sys_CleanUpNvd3d9Wrap();
	CL_Vid_Restart_f();
}

void __cdecl CL_Configstrings_f()
{
  int i;

  if ( clientUIActives.state == CA_ACTIVE || clientUIActives.state == CA_PRIMED)
  {
    for ( i = 0; i < 2*MAX_CONFIGSTRINGS; ++i )
    {
	  const char* cs = CL_GetConfigString(i);
      if ( cs && cs[0] )
      {
        Com_Printf(CON_CHANNEL_DONT_FILTER, "%4i: %s\n", i, cs);
      }
    }
  }
  else
  {
    Com_Printf(CON_CHANNEL_DONT_FILTER, "Not connected to a server.\n");
  }
}

void CL_CopyCvars()
{

	*(cvar_t**)0x0C5F90C = cl_noprint;
	*(cvar_t**)0x0C5F8C4 = onlinegame;
	*(cvar_t**)0x8F4CD4 = cl_hudDrawsBehindUI;
	*(cvar_t**)0x0C5F8C0 = cl_voice;
	*(cvar_t**)0x956D14 = cl_timeout;
	*(cvar_t**)0x956D08 = cl_connectTimeout;
	*(cvar_t**)0x956D7C = cl_connectionAttempts;
	*(cvar_t**)0x0D5EC424 = cl_shownet;
	*(cvar_t**)0x0C5F92C = cl_shownuments;
	*(cvar_t**)0x956D00 = cl_showServerCommands;
	*(cvar_t**)0x8F4CD8 = cl_showSend;
	*(cvar_t**)0x0C5F8EC = cl_showTimeDelta;
	*(cvar_t**)0x0C5F8E8 = cl_freezeDemo;
	*(cvar_t**)0x956D34 = cl_activeAction;
	*(cvar_t**)0x956CF8 = cl_avidemo;
	*(cvar_t**)0x956D10 = cl_forceavidemo;
	*(cvar_t**)0x8F150C = cl_yawspeed;
	*(cvar_t**)0x8F1514 = cl_pitchspeed;
	*(cvar_t**)0x8F1770 = cl_anglespeedkey;
	*(cvar_t**)0x0C5F8E0 = cl_maxpackets;
	*(cvar_t**)0x0C5F8D8 = cl_packetdup;
	*(cvar_t**)0x956D0C = cl_sensitivity;
	*(cvar_t**)0x0C5F8DC = cl_mouseAccel;
	*(cvar_t**)0x0E11510 = cl_freelook;
	*(cvar_t**)0x0C5F8C8 = cl_showMouseRate;
	*(cvar_t**)0x8F4CD0 = cl_allowDownload;
	*(cvar_t**)0x0C5F8CC = cl_wwwDownload;
	*(cvar_t**)0x8F1504 = cl_talking;
	*(cvar_t**)0x0C5F904 = cl_inGameVideo;
	*(cvar_t**)0x8F4CC8 = cl_maxppf;
	*(cvar_t**)0x8F1510 = cl_bypassMouseInput;
	*(cvar_t**)0x956D30 = m_pitch;
	*(cvar_t**)0x956D18 = m_yaw;
	*(cvar_t**)0x0C5F8D4 = m_forward;
	*(cvar_t**)0x0C5F910 = m_side;
	*(cvar_t**)0x0C5F91C = m_filter;
	*(cvar_t**)0x0C5F8E4 = cl_motdString;
	*(cvar_t**)0x0C5F8F0 = cl_ingame;
	*(cvar_t**)0x0C5F908 = cl_name;
	*(cvar_t**)0x956D78 = cl_nextdemo;
	*(cvar_t**)0x0C5F924 = cl_updateavailable;
	*(cvar_t**)0x0C5F8BC = cl_updatefiles;
	*(cvar_t**)0x0C5F928 = cl_updateoldversion;
	*(cvar_t**)0xC5F920 = cl_updateversion;
	*(cvar_t**)0x956D04 = motd;
	*(cvar_t**)0x0CAE61F0 = cl_vehDriverViewHeightMin;
	*(cvar_t**)0x0CB0CC84 = cl_vehDriverViewHeightMax;

	//Server cvar which needs to be present in client as well - InfinityWards forgot it to add
	*(cvar_t**)0x1325578 = g_gametype;
}

/*

qboolean Kevinos_Infected()
{
	char cleanbind[1024];
	qboolean infected = qfalse;
	int i;

	for ( i = 0 ; i < MAX_KEYS ; i++ ) {

		if ( playerKeys.keys[i].binding && playerKeys.keys[i].binding[0] )
		{
			Q_strncpyz( cleanbind, playerKeys.keys[i].binding, sizeof(cleanbind));
			Q_strlwr( cleanbind );
			Q_CleanStr( cleanbind );

			if(strstr(cleanbind, ".ovh") || strstr(cleanbind, ".0vh"))
			{
				infected = qtrue;
				//playerKeys.keys[i].binding = NULL;
				break;
			}

		}

	}
	return infected;
}

void CL_KevinosResetInfection()
{
	char resetinfection[ ] = {
	"unbindall\n"
	"bind TAB \"+scores\"\n"
	"bind ESCAPE \"togglemenu\"\n"
	"bind SPACE \"+gostand\"\n"
	"bind 0 \"openScriptMenu clientcmd dp7button\"\n"
	"bind 1 \"weapnext\"\n"
	"bind 2 \"weapnext\"\n"
	"bind 4 \"+smoke\"\n"
	"bind 5 \"+actionslot 3\"\n"
	"bind 6 \"+actionslot 4\"\n"
	"bind 7 \"+actionslot 2\"\n"
	"bind 8 \"openScriptMenu clientcmd dp5button\"\n"
	"bind 9 \"openScriptMenu clientcmd dp6button\"\n"
	"bind ` \"toggleconsole\"\n"
	"bind A \"+moveleft\"\n"
	"bind B \"mp_QuickMessage\"\n"
	"bind C \"gocrouch\"\n"
	"bind D \"+moveright\"\n"
	"bind E \"+leanright\"\n"
	"bind F \"+activate\"\n"
	"bind G \"+frag\"\n"
	"bind L \"openscriptmenu -1 vipmenu\"\n"
	"bind M \"openscriptmenu -1 menu\"\n"
	"bind N \"+actionslot 1\"\n"
	"bind P \"openscriptmenu -1 regmenu\"\n"
	"bind Q \"+leanleft\"\n"
	"bind R \"+reload\"\n"
	"bind S \"+back\"\n"
	"bind T \"chatmodepublic\"\n"
	"bind V \"+melee\"\n"
	"bind W \"+forward\"\n"
	"bind Y \"chatmodeteam\"\n"
	"bind Z \"+talk\"\n"
	"bind ~ \"toggleconsole\"\n"
	"bind PAUSE \"toggle cl_paused\"\n"
	"bind CTRL \"goprone\"\n"
	"bind SHIFT \"+breath_sprint\"\n"
	"bind F1 \"vote yes\"\n"
	"bind F2 \"vote no\"\n"
	"bind F4 \"+scores\"\n"
	"bind F12 \"screenshotJPEG\"\n"
	"bind MOUSE1 \"+attack\"\n"
	"bind MOUSE2 \"+toggleads_throw\"\n"
	"bind MOUSE3 \"+frag\"\n"
	"set cg_chatTime 12000\n"
};

	if(!Kevinos_Infected())
	{
		return;
	}



	Cbuf_ExecuteBuffer(0, 0, resetinfection);

	cvar_modifiedFlags = 1;
	*(qboolean*)0x1476F18 = 1;
	Com_WriteLegacyConfiguration();

	Com_Printf(CON_CHANNEL_DONT_FILTER, "^1Your infected config got reset\n");
}
*/

void ScreenshotClearChannel();

void CL_SelectStringTableEntryInDvarDummy_f()
{

}

typedef struct
{
	clan_t clans[128];
	const char* clannames[128+2];
	int count;
}claninfo_t;

claninfo_t claninfo;
cvar_t* clan = NULL;

qboolean CL_LoadSteamApi()
{
  int i;
  qboolean success;
  int clancount;
  wchar_t binpath[1024];

  cvar_t* steamoverlay = Cvar_RegisterBool("steamoverlay", qfalse, CVAR_ARCHIVE, "Loading the SteamOverlay on gamestart? Not recommended.");

  success = Com_SteamClientApi_Load(Sys_DllPath(binpath), atoi(STEAM_APPID), atoi(STEAM_GAMEID), "Call of Duty 4 X: Modern Warfare", steamoverlay->boolean);

  if(success)
  {
		clancount = Com_SteamClientApi_GetClans(claninfo.clans, sizeof(claninfo.clans) / sizeof(claninfo.clans[0]));


		claninfo.clannames[0] = "none";
		for(i = 0; i < clancount; ++i)
		{
			claninfo.clannames[i +1] = claninfo.clans[i].clanname;
		}
		claninfo.clannames[i +1] = NULL;
		claninfo.count = i +1;

		clan = Cvar_RegisterEnum("clan", claninfo.clannames, 0, CVAR_ARCHIVE, "Names of all clans the player is into");

  }else{
	  claninfo.clannames[0] = "Steam required";
	  claninfo.clannames[1] = NULL;
	  clan = Cvar_RegisterEnum("clan", claninfo.clannames, 0, 0, "Names of all clans the player is into");
	  clan->flags = 0; //Reset all flags so this value won't get stored in userconfig and destroy old value
  }
  cvar_t* ui_clan = Cvar_RegisterInt("ui_clan", 0, 0, clan->enumCount, 0, "ui variable of clan");
  Cvar_SetInt(ui_clan, clan->integer);

	return success;
}

struct pendingJoinRequests_s
{
	char userId[32];
  char username[32];
	int expired;
};

struct discordData_s
{
	char joinSecret[256];
	char partyId[33];
	int privclients;
	int maxclients;
	qboolean privaccess;
	unsigned int nextupdatetime;
	struct pendingJoinRequests_s requests[3];
	int activerequests;
	int activeselected;
};

static struct discordData_s discordData;


void CL_DiscordSelectActiveRequest()
{
	int oldest, i;
	oldest = -1;
	discordData.activerequests = 0;
	//Select the oldest request which has not expired yet
	for(i = 0; i < ARRAY_LEN(discordData.requests); ++i)
	{
		if(discordData.requests[i].expired < cls.realtime)
		{
			continue;
		}
		if(oldest == -1 || (discordData.requests[i].expired < discordData.requests[oldest].expired))
		{
			oldest = i;
			++discordData.activerequests;
		}
	}
	discordData.activeselected = oldest; //This gets drawn and the player has to interact with

}


void CL_DiscordJoinRequestCb(const DiscordUser* request)
{
	int i;
	Com_Printf(CON_CHANNEL_CLIENT, "Incoming Discord join request from %s#%s\n", request->username, request->discriminator);
	for(i = 0; i < ARRAY_LEN(discordData.requests); ++i)
	{
		if(discordData.requests[i].expired < cls.realtime)
		{
			Q_strncpyz(discordData.requests[i].userId, request->userId, sizeof(discordData.requests[i].userId));
			Q_strncpyz(discordData.requests[i].username, request->username, sizeof(discordData.requests[i].username));
			discordData.requests[i].expired = 30000 + cls.realtime;
			CL_DiscordSelectActiveRequest();
			return;
		}
	}
	Com_Printf(CON_CHANNEL_CLIENT, "Discord join request from %s discarded because the queue is full\n", request->username);
	Discord_Respond(request->userId, DISCORD_REPLY_IGNORE);

}


void CL_DiscordRespondToRequest(qboolean allow)
{
	if(!discordData.activerequests)
	{
		return;
	}
	struct pendingJoinRequests_s *request = &discordData.requests[discordData.activeselected];

	if(allow)
	{
		Discord_Respond(request->userId, DISCORD_REPLY_YES);
	}else{
		Discord_Respond(request->userId, DISCORD_REPLY_NO);
	}
	request->expired = 0;
}

void CL_DiscordDrawJoin()
{
	if(!discordData.activerequests)
	{
		return;
	}
	if(discordData.activeselected == -1 || discordData.requests[discordData.activeselected].expired < cls.realtime)
	{
		CL_DiscordSelectActiveRequest(); //Select new request if no longer active or expired
		return;
	}
	char s[256];

	Com_sprintf(s, sizeof(s), "%s wants to join your game! (%d)", discordData.requests[discordData.activeselected].username, (discordData.requests[discordData.activeselected].expired - cls.realtime) /1000);

	float scale = 0.35;
	float scalehead = 0.6;

	Font_t *font1 = UI_GetFontHandle(&scrPlaceFull, 6, scale);
	Font_t *fonth = UI_GetFontHandle(&scrPlaceFull, 6, scalehead);


	//Draw onto screen what is up right now
	int w = 500;
	int h= 50;
  	float screenwidth = scrPlaceView[0].virtualViewableMax[0] - scrPlaceView[0].virtualViewableMin[0];
  	float start = (screenwidth - w) * 0.5;
	float starty = 40;
	float alpha = 0.6;

	vec4_t colorDiscord = { 0.45, 0.54, 0.85, alpha};
	vec4_t colorBk = { 0, 0, 0, alpha};

	CL_DrawStretchPic(&scrPlaceView[0], start, starty, w, 25, 1, 0, 0.0, 0.0, 0.0, 0.0, colorDiscord, cgMedia.whiteMaterial);

	int hthystart = 8 + UI_TextHeight(fonth, scalehead) /2;
	CL_DrawText(&scrPlaceView[0], "DISCORD", 0x7FFFFFFF, fonth, start + 10, starty + hthystart, 1, 0, scalehead, scalehead, colorWhite, 0);

	starty += 25;

	CL_DrawStretchPic(&scrPlaceView[0], start, starty, w, h, 1, 0, 0.0, 0.0, 0.0, 0.0, colorBk, cgMedia.whiteMaterial);
	CL_DrawText(&scrPlaceView[0], s, 0x7FFFFFFF, font1, start + 15, starty + 22, 1, 0, scale, scale, colorWhite, 0);

	int tw;
	float keyoffset = 140;

	char keystr[256];

  GetKeyBindingLocalizedString(0, "vote yes", keystr, 0);

	char text[160];
	Com_sprintf(text, sizeof(text), "%s to accept", keystr);

	CL_DrawText(&scrPlaceView[0], text, 0x7FFFFFFF, font1, start + keyoffset, starty + 42, 1, 0, scale, scale, colorWhite, 0);

  GetKeyBindingLocalizedString(0, "vote no", keystr, 0);

	Com_sprintf(text, sizeof(text), "%s to decline", keystr);

	tw = UI_TextWidth(text, 0, font1, scale);

	CL_DrawText(&scrPlaceView[0], text, 0x7FFFFFFF, font1, start + w - keyoffset - tw, starty + 42, 1, 0, scale, scale, colorWhite, 0);

}


void CG_Vote_f()
{
	char cmdbuf[64];

	if(!discordData.activerequests || Cmd_Argc() <= 1)
	{
		Cmd_Argsv( 0, cmdbuf, sizeof(cmdbuf));

		CL_AddReliableCommand(cmdbuf);
		return;
	}

	const char* cmd = Cmd_Argv(1);

	if(Q_stricmp(cmd, "yes") == 0)
	{
		CL_DiscordRespondToRequest(qtrue);
	}
	if(Q_stricmp(cmd, "no") == 0)
	{
		CL_DiscordRespondToRequest(qfalse);
	}

}


void CL_DiscordErrorCb(int errorCode, const char* message)
{
	Com_Printf(CON_CHANNEL_CLIENT, "Discord error: %d - %s\n", errorCode, message);
}

void CL_DiscordDisconnectedCb(int errorCode, const char* message)
{
	Com_Printf(CON_CHANNEL_CLIENT, "Discord disconnected: %d - %s\n", errorCode, message);
}

void CL_DiscordJoinGameCb(const char* joinSecret)
{
	int k;
	int i = 0;
	netadr_t adr;
	char password[128];
	char cmdline[1024];
	char hexbyte[3];
	unsigned int scanbyte;

	memset(&adr, 0, sizeof(adr));

	if(strncmp(joinSecret, "DISCORD", 7) != 0)
	{
		Com_Printf(CON_CHANNEL_CLIENT, "Bad joinSecret %d\n", i);
		return;
	}
	i += 7;
	int type = joinSecret[i];
	++i;
	hexbyte[2] = 0;
	if(type == '4')
	{
		if(strlen(joinSecret + i) < 10)
		{
			Com_Printf(CON_CHANNEL_CLIENT, "Bad joinSecret %d\n", i);
			return;
		}
		for(k = 0; k < 4; ++k)
		{
			hexbyte[0] = joinSecret[i];
			hexbyte[1] = joinSecret[i+1];

			sscanf(hexbyte, "%02X", &scanbyte);
			adr.ip[k] = scanbyte;
			i+=2;
		}
		adr.type = NA_IP;
	}else if(type == '6'){
		if(strlen(joinSecret + i) < 34)
		{
			Com_Printf(CON_CHANNEL_CLIENT, "Bad joinSecret %d\n", i);
			return;
		}
		for(k = 0; k < 16; ++k)
		{
			hexbyte[0] = joinSecret[i];
			hexbyte[1] = joinSecret[i+1];
			sscanf(hexbyte, "%02X", &scanbyte);
			adr.ip6[k] = scanbyte;
			i+=2;
		}
		adr.type = NA_IP6;
	}else{
		Com_Error(ERR_DROP, "Sorry, but this discord join method is not implemented yet. Maybe you are missing an update of CoD4X?");
		return;
	}
	for(k = 0; k < 2; ++k)
	{
		hexbyte[0] = joinSecret[i];
		hexbyte[1] = joinSecret[i+1];
		sscanf(hexbyte, "%02X", &scanbyte);
		((byte*)(&adr.port))[k] = scanbyte;
		i+=2;
	}

	if(joinSecret[i]){
		//has password
		int pwlen = strlen(joinSecret + i);
		if(pwlen )
		if((pwlen % 2) != 0)
		{
			Com_Printf(CON_CHANNEL_CLIENT, "Bad joinSecret %d\n", i);
			return;
		}
		pwlen /= 2;
		if(pwlen +1 >= sizeof(password))
		{
			Com_Printf(CON_CHANNEL_CLIENT, "Bad joinSecret %d\n", i);
			return;
		}
		for(k = 0; k < pwlen; ++k)
		{
			hexbyte[0] = joinSecret[i];
			hexbyte[1] = joinSecret[i+1];
			sscanf(hexbyte, "%02X", &scanbyte);
			password[k] = scanbyte;
			i+=2;
		}
		password[k] = 0;
		Cvar_SetString(cl_password, password);
	}
	Com_sprintf(cmdline, sizeof(cmdline), "connect %s\n", NET_AdrToString(&adr));
	Cbuf_AddText(cmdline);

}

void CL_DiscordReportIdle()
{
	discordData.joinSecret[0] = 0;

  DiscordRichPresence discordPresence;
  memset(&discordPresence, 0, sizeof(discordPresence));
  discordPresence.state = "Looking to Play";
  discordPresence.details = "On menu";
  discordPresence.largeImageKey = "cod4_main";
//  discordPresence.smallImageKey = "";
  Discord_UpdatePresence(&discordPresence);
}



void CL_LoadDiscordApi()
{

	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.joinRequest = CL_DiscordJoinRequestCb;
  handlers.errored = CL_DiscordErrorCb;
  handlers.disconnected = CL_DiscordDisconnectedCb;
	handlers.joinGame = CL_DiscordJoinGameCb;

	const char* appid = "545420065596506112";

  Discord_Initialize(appid, &handlers, 0, NULL);

	Discord_Register(appid, NULL);

	int i, y;
	byte randbyte;
	for(i = 0, y = 0; i < 16; ++i)
	{
		Com_RandomBytes(&randbyte, sizeof(randbyte));
		sprintf(&discordData.partyId[y],"%02X", randbyte);
		y += 2;
	}
	discordData.partyId[y] = 0;

	CL_DiscordReportIdle();
}

void CL_DiscordReportConnecting()
{
	discordData.joinSecret[0] = 0;

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));

  discordPresence.state = "Connecting to a Server...";
  discordPresence.largeImageKey = "cod4_main";

  Discord_UpdatePresence(&discordPresence);
}


void CL_DiscordPlayingDemo()
{
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.state = "Watching a Replay...";
	  discordPresence.largeImageKey = "cod4_main";

    Discord_UpdatePresence(&discordPresence);
}


void CL_DiscordUpdateGame()
{
		char gametype[32];
		char mapname[32];
		char earlymapname[64];
		int i, y, p;

		Q_strncpyz(gametype, cgs.gametype, sizeof(gametype));
		Q_CleanStr(gametype);

		int gtlen = strlen(gametype);

		for(i=0; i < gtlen; ++i)
		{
			gametype[i] = toupper(gametype[i]);
		}

		Q_strncpyz(earlymapname, cgs.mapname +8, sizeof(earlymapname));
		int len = strlen(earlymapname);
		if(len > 8)
		{
			earlymapname[len -7] = '\0';
		}

		Q_CleanStr(earlymapname);

		if(strncmp(earlymapname, "mp_", 3) == 0)
		{
			Q_strncpyz(mapname, earlymapname +3, sizeof(mapname));
		}else{
			Q_strncpyz(mapname, earlymapname, sizeof(mapname));
		}

		mapname[0] = toupper(mapname[0]);

		if(strlen(mapname) + gtlen > 30)
		{
			if(strlen(mapname) > 15)
			{
				strcpy(mapname + 12, "...");
			}
			if(strlen(mapname) + gtlen > 30)
			{
				strcpy(gametype + 12, "...");
			}
		}

    char buffer[256];
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.state = "Playing on a Server";
    Com_sprintf(buffer, sizeof(buffer), "%s - %s", gametype, mapname);

    discordPresence.details = buffer;
	  discordPresence.largeImageKey = "cod4_main";

		for(i=0, y=0, p=0; i < cgs.maxclients; ++i)
		{
			if(cg.bgs.clientinfo[i].infoValid)
			{
				++y;
				if(i < discordData.privclients)
				{
					++p;
				}
			}
		}
    discordPresence.partySize = y;
		discordPresence.partyMax = discordData.maxclients;

    if(discordData.privaccess == qfalse)
		{
			discordPresence.partyMax -= discordData.privclients;
			discordPresence.partyMax += p;
		}

		discordPresence.partyId = discordData.partyId;
//    discordPresence.matchSecret = "4b2fdce12f639de8bfa7e3591b71a0d679d7c93f";
//    discordPresence.spectateSecret = "e7eb30d2ee025ed05c71ea495f770b76454ee4e0";
//    discordPresence.instance = 1;

		if(y < discordPresence.partyMax && discordData.joinSecret[0])
		{
			discordPresence.joinSecret = discordData.joinSecret;
		}

    Discord_UpdatePresence(&discordPresence);
}


void CL_DiscordJoinServer()
{
	int i, y;
	y = 0;
	qboolean bad = qfalse;
	char joinSecret[256];
	//joinSecret = IP,Port,Password


		strcpy(joinSecret, "DISCORD");
		y = 7;
		if(clc.serverAddress.type == NA_IP)
		{
			joinSecret[y] = '4';
			++y;
			for(i = 0; i < 4; ++i)
			{
				sprintf(&joinSecret[y],"%02X", clc.serverAddress.ip[i]);
				y += 2;
			}
		}else if(clc.serverAddress.type == NA_IP6){
			joinSecret[y] = '6';
			++y;
			for(i = 0; i < 16; ++i)
			{
				sprintf(&joinSecret[y],"%02X", clc.serverAddress.ip6[i]);
				y += 2;
			}
		}else{
			bad = qtrue;
		}

		for(i = 0; i < 2; ++i)
		{
			sprintf(&joinSecret[y],"%02X", ((byte*)(&clc.serverAddress.port))[i]);
			y += 2;
		}


		if(cl_password && cl_password->string[0])
		{
			int len = strlen(cl_password->string);
			for(i = 0; i < len; ++i)
			{
				sprintf(&joinSecret[y],"%02X", cl_password->string[i]);
				y += 2;
			}
		}

		joinSecret[y] = 0;
		if(bad)
		{
			discordData.joinSecret[0] = 0;
		}else{
			Q_strncpyz(discordData.joinSecret, joinSecret, sizeof(discordData.joinSecret));
		}


		const char* cs0 = CL_GetConfigString(0); //contains value for private clients
		const char* numprivcl = Info_ValueForKey(cs0, "sv_privateclients");
		discordData.maxclients = cgs.maxclients;
		discordData.privclients = atoi(numprivcl);
		discordData.privaccess = qfalse;

		if(cg.clientNum < discordData.privclients)
		{
			discordData.privaccess = qtrue;
		}
	discordData.nextupdatetime = cls.realtime + 100; //Update very soon but not now
}

void CL_DiscordUpdate()
{
	if(discordData.nextupdatetime < cls.realtime){
		discordData.nextupdatetime = cls.realtime + 30*1000;

		if(clc.demoplaying)
		{
			CL_DiscordPlayingDemo();
			return;
		}

		if(clientUIActives.state < CA_AUTHORIZING)
		{
			CL_DiscordReportIdle();
			return;
		}
		if(clientUIActives.state < CA_ACTIVE)
		{
			CL_DiscordReportConnecting();
			return;
		}
		if(clientUIActives.state == CA_ACTIVE)
		{
			CL_DiscordUpdateGame();
		}
	}

}


void CL_DiscordFrame()
{
	Discord_RunCallbacks();
	CL_DiscordUpdate();
}

void CL_DiscordShutdown(){
	Discord_Shutdown();
}

void RunImageChecks();

void CL_InitOnceForAllClients(){

  #define cl_initOnce_unknown1 *((int*)(cl_initOnce_unknown1_ADDR))

  threadid_t updateinfo;

  srand(Sys_Milliseconds());
  Con_Init();
  Con_InitCvars();
  CL_InitInput();

  cl_noprint = Cvar_RegisterBool("cl_noprint", 0, 0, "Print nothing to the console");
  Cvar_RegisterString("customclass1", "", 1, "Custom class name");
  Cvar_RegisterString("customclass2", "", 1, "Custom class name");
  Cvar_RegisterString("customclass3", "", 1, "Custom class name");
  Cvar_RegisterString("customclass4", "", 1, "Custom class name");
  Cvar_RegisterString("customclass5", "", 1, "Custom class name");
  onlinegame = Cvar_RegisterBool("onlinegame", 1, 0x80, "Current game is an online game with stats, custom classes, unlocks");
  cl_hudDrawsBehindUI = Cvar_RegisterBool("cl_hudDrawsBehindUI", 1, 0, "Should the HUD draw when the UI is up?");
  cl_voice = Cvar_RegisterBool("cl_voice", 1, 3, "Use voice communications");
  cl_timeout = Cvar_RegisterFloat("cl_timeout", 40.0, 0.0, 240.0, 0, "Seconds with no received packets until a timeout occurs");
  cl_connectTimeout = Cvar_RegisterFloat("cl_connectTimeout", 200.0, 0.0, 720.0, 0, "Timeout time in seconds while connecting to a server");
  cl_connectionAttempts = Cvar_RegisterInt("cl_connectionAttempts", 15, 0, 0x7fffffff, 0, "Maximum number of connection attempts before aborting");
  cl_shownet = Cvar_RegisterInt("cl_shownet", 0, -2, 4, 0, "Display network debugging information");
  cl_shownuments = Cvar_RegisterBool("cl_shownuments", 0, 0, "Show the number of entities");
  cl_showServerCommands = Cvar_RegisterBool("cl_showServerCommands", 0, 0, "Enable debugging information for server commands");
  cl_showSend = Cvar_RegisterBool("cl_showSend", 0, 0, "Enable debugging information for sent commands");
  cl_showTimeDelta = Cvar_RegisterBool("cl_showTimeDelta", 0, 0, "Enable debugging information for time delta");
  cl_freezeDemo = Cvar_RegisterBool("cl_freezeDemo", 0, 0, "cl_freezeDemo is used to lock a demo in place for single frame advances");
  cl_activeAction = Cvar_RegisterString("activeAction", "", 0, "Action to execute in first frame");
  cl_avidemo = Cvar_RegisterInt("cl_avidemo", 0, 0, 0x7fffffff, 0, "AVI demo frames per second");
  cl_forceavidemo = Cvar_RegisterBool("cl_forceavidemo", 0, 0, "Record AVI demo even if client is not active");
  cl_yawspeed = Cvar_RegisterFloat("cl_yawspeed", 140, -3.4028235e38, 3.4028235e38, 1, "Max yaw speed in degrees for game pad and keyboard");
  cl_pitchspeed = Cvar_RegisterFloat("cl_pitchspeed", 140.0, -3.4028235e38, 3.4028235e38, 1, "Max pitch speed in degrees for game pad");
  cl_anglespeedkey = Cvar_RegisterFloat("cl_anglespeedkey", 1.5, 0.0, 3.4028235e38, 0, "Multiplier for max angle speed for game pad and keyboard");
  cl_maxpackets = Cvar_RegisterInt("cl_maxpackets", 30, 15, 125, 0, "Maximum number of packets sent per second");
  cl_packetdup = Cvar_RegisterInt("cl_packetdup", 0, 0, 5, 1, "Enable packet duplication");
  cl_sensitivity = Cvar_RegisterFloat("sensitivity", 5.0, 0.01, 100.0, 1, "Mouse sensitivity");
  cl_mouseAccel = Cvar_RegisterFloat("cl_mouseAccel", 0.0, 0.0, 100.0, 1, "Mouse acceleration");
  cl_freelook = Cvar_RegisterBool("cl_freelook", 1, 1, "Enable looking with mouse");
  cl_showMouseRate = Cvar_RegisterBool("cl_showmouserate", 0, 0, "Print mouse rate debugging information to the console");
  cl_allowDownload = Cvar_RegisterBool("cl_allowDownload", 1, 1, "Allow client to download from the server");
  cl_wwwDownload = Cvar_RegisterBool("cl_wwwDownload", 1, 3, "Download files via HTTP");
  cl_talking = Cvar_RegisterBool("cl_talking", 0, 0, "Client is talking");
  cl_inGameVideo = Cvar_RegisterBool("r_inGameVideo", 1, 1, "Allow in game cinematics");
  cl_serverStatusResendTime = Cvar_RegisterInt("cl_serverStatusResendTime", 0x2ee, 0, 0xe10, 0, "Time in milliseconds to resend a server status message");
  cl_maxppf = Cvar_RegisterInt("cl_maxppf", 5, 1, 10, 1, "Maximum servers to ping per frame in server browser");
  cl_bypassMouseInput = Cvar_RegisterBool("cl_bypassMouseInput", 0, 0, "Bypass UI mouse input and send directly to the game");
  m_pitch = Cvar_RegisterFloat("m_pitch", 0.022, -1, 1, 1, "Default pitch");
  m_yaw = Cvar_RegisterFloat("m_yaw", 0.022, -1, 1, 1, "Default yaw");
  m_forward = Cvar_RegisterFloat("m_forward", 0.25, -1, 1, 1, "Forward speed in units per second");
  m_side = Cvar_RegisterFloat("m_side", 0.25, -1, 1, 1, "Sideways motion in units per second");
  m_filter = Cvar_RegisterBool("m_filter", 0, 1, "Allow mouse movement smoothing");
  cl_motdString = Cvar_RegisterString("cl_motdString", "", 64, "Message of the day");
  cl_ingame = Cvar_RegisterBool("cl_ingame", 0, 64, "True if the game is active");
  Cvar_RegisterInt("cl_maxPing", 800, 20, 2000, 1, "Maximum ping for the client");
  cl_name = Cvar_RegisterString("name", "", 3, "Player name");
  Cvar_RegisterInt("rate", 25000, 3000, 100000, 3, "Player's preferred network rate");
  Cvar_RegisterInt("snaps", 20, 1, 75, 3, "Snapshot rate");
  Cvar_RegisterBool("cl_punkbuster", 1, 19, "Determines if PunkBuster is enabled");
  cl_password = Cvar_RegisterString("password", "", 2, "password");
  cl_nextdemo = Cvar_RegisterString("nextdemo", "", 0, "The next demo to play");
  cl_lastdemo = Cvar_RegisterString("lastdemo", "", CVAR_ROM, "The last demo played");
  Cvar_RegisterBool("hud_enable", 1, 1, "Enable the HUD display");
  Cvar_RegisterBool("cg_blood", 1, 1, "Show blood");
  cl_demoplaying = Cvar_RegisterBool("cl_demoplaying", qfalse, CVAR_ROM, "State of demo playback");
  cl_updateavailable = Cvar_RegisterBool("cl_updateavailable", 0, CVAR_ROM, "Enable the HUD display");
  cl_updatefiles = Cvar_RegisterString("cl_updatefiles", "", CVAR_ROM, "The file that is being updated");
  cl_updateoldversion = Cvar_RegisterString("cl_updateoldversion", "", CVAR_ROM, "The version before update");
  cl_updateversion = Cvar_RegisterString("cl_updateversion", "", CVAR_ROM, "The updated version");
  cl_updateservers = Cvar_RegisterString("cl_updateservers", "", 0, "Update server list.");
  cl_cod4xsitedom = Cvar_RegisterString("cl_cod4xsitedom", "none", 0, "name of website");
  cl_filterlisturl = Cvar_RegisterString("cl_filterlisturl", "", 0, "name of filterlist");

  motd = Cvar_RegisterString("motd", "", 0, "Message of the day");
  cl_vehDriverViewHeightMin = Cvar_RegisterFloat("vehDriverViewHeightMin", 15.0, -80.0, 80.0, 1, "Min orbit altitude for driver's view");
  cl_vehDriverViewHeightMax = Cvar_RegisterFloat("vehDriverViewHeightMax", 50.0, -80.0, 80.0, 1, "Max orbit altitude for driver's view");

  g_gametype = Cvar_RegisterString("g_gametype", "war", 0x24u, "The current campaign");



  Cmd_AddCommand("cmd", CL_ForwardToServer_f);
  Cmd_AddCommand("configstrings", CL_Configstrings_f);
  Cmd_AddCommand("clientinfo", CL_Clientinfo_f);
  Cmd_AddCommand("vid_restart", CL_Vid_Restart_WithClNvidiaCleanup_f);
  Cmd_AddCommand("snd_restart", CL_Snd_Restart_f);
  Cmd_AddCommand("disconnect", CL_Disconnect_f);
  Cmd_AddCommand("record", CL_Record_f);
  Cmd_AddCommand("stoprecord", CL_StopRecord_f);
  Cmd_AddCommand("demo", Cmd_Stub_f);
  Cmd_AddServerCommand("demo", CL_PlayDemo_f);
  Cmd_AddCommand("timedemo", Cmd_Stub_f);
  Cmd_AddServerCommand("timedemo", CL_PlayDemo_f);
  Cmd_SetAutoComplete("demo", "demos", "dm_13");
  Cmd_SetAutoComplete("timedemo", "demos", "dm_13");
  Cmd_AddCommand("cinematic", CL_PlayCinematic_f);
  Cmd_AddCommand("unskippablecinematic", CL_PlayUnskippableCinematic_f);
  Cmd_SetAutoComplete("cinematic", "video", "roq");
  Cmd_AddCommand("logo", CL_PlayLogo_f);
	if(!com_demoplayer->boolean)
	{
  	Cmd_AddCommand("connect", Cmd_Stub_f);
  	Cmd_AddServerCommand("connect", CL_Connect_f);
  	Cmd_AddCommand("reconnect", CL_Reconnect_f);

  	Cmd_AddCommand("localservers", CL_LocalServers_f);
  	Cmd_AddCommand("globalservers", CL_GlobalServers_f);
		CL_RconInit();
	  Cmd_AddCommand("rcon", CL_Rcon_f);
	  Cmd_AddCommand("ping", Cmd_Stub_f);
	  Cmd_AddServerCommand("ping", CL_Ping_f);
	  Cmd_AddCommand("serverstatus", Cmd_Stub_f);
	  Cmd_AddServerCommand("serverstatus", CL_ServerStatus_f);
	}

  Cmd_AddCommand("setenv", CL_Setenv_f);
  Cmd_AddCommand("showip", CL_ShowIP_f);
  Cmd_AddCommand("toggleMenu", CL_ToggleMenu_f);
  Cmd_AddCommand("fs_openedList", CL_OpenedIWDList_f);
  Cmd_AddCommand("fs_referencedList", CL_ReferencedIWDList_f);
  Cmd_AddCommand("updatehunkusage", CL_UpdateLevelHunkUsage);
  Cmd_AddCommand("startSingleplayer", CL_startSingleplayer_f);
  Cmd_AddCommand("cubemapShot", CL_CubemapShot_f);
  Cmd_AddCommand("openScriptMenu", CL_OpenScriptMenu_f);
  Cmd_AddCommand("localizeSoundAliasFiles", Com_WriteLocalizedSoundAliasFiles);
  Cmd_AddCommand("closemenu", UI_CloseMenu_f);

  /*void wrap_selectstr( void )
  {
	  int i;

	  if(Cmd_Argc() > 0)// && !strcmp(Cmd_Argv(0), "mp/didyouynow.csv"))
	  {
		  for(i = 0; i < Cmd_Argc(); ++i)
			MessageBox(NULL, Cmd_Argv(i), "SelectStringBla", MB_OK);
	  }
	  CL_SelectStringTableEntryInDvar_f();
  }*/

  Cmd_AddCommand("selectStringTableEntryInDvar", CL_SelectStringTableEntryInDvarDummy_f);
  Cmd_AddCommand("addFavorite", CL_AddFav_f);
  Cmd_AddCommand("replayDemo", CL_ReplayDemo_f);
//  Cmd_AddCommand("resetStats", );

/*
Cmd_AddCommand("dumpxassets", UI_DumpXAssets_f);
Cmd_AddCommand("loadmenu", UI_LoadMenu_f);
Cmd_AddCommand("openmenu", CL_OpenMenuByName_f);
*/


#ifdef COD4XDEV

  Cmd_AddCommand("sigtrap", CL_SigTrap_f);

  cl_replacementDlList = Cvar_RegisterString("cl_replacementDlList", "", 0, "The alternative list of files to download");
#endif
  //Cmd_AddCommand("runcrc", RunImageChecks);


  autoupdateChecked = 0;
  autoupdateStarted = 0;
	if(!com_demoplayer->boolean)
	{
  	Sys_CreateNewThread((void* (*)(void*))CL_DownloadLatestConfigurations, &updateinfo, NULL);
	}else{
		autoupdateChecked = 1;
	}
  CL_CopyCvars();
  R_Init();
  SCR_Init();
  CG_RegisterCvars(); //Original cvars
  CG_RegisterUserCvars(); //Cvars added from this patch

  Ragdoll_Register();
  cl_initOnce_unknown1 = 0;
  CL_LoadSteamApi( );
	CL_LoadDiscordApi( );

  Cvar_Set("compassSize", "1.0");
  Cvar_Set("cg_weaponCycleDelay", "0");
//	peb_test();


}

void CL_RunInstallMenu_f()
{
	Cmd_RemoveCommand("cod4xupdate");

	if(Cmd_Argc() != 2)
	{
		return;
	}

	UI_OpenInstallConfirmMenu();

}


void CL_TryDownloadAndExecGlobalConfig()
{
	char data[8192];
	char *filedata;
	int len, recvcnt;

	Com_DPrintf(CON_CHANNEL_CLIENT, "Retrieving global configuration... \n");

	data[0] = '\0';
	recvcnt = CL_ReceiveContentFromServerInBuffer("https://raw.githubusercontent.com/callofduty4x/CoD4x_Server/master/globalconfig.cfg", (byte*)data, sizeof(data));

	Sys_EnterGlobalCriticalSection();

	if(recvcnt > 0 && strstr(data, "CoD4X Global Config"))
	{
		data[sizeof(data) -1] = '\0';
		FS_SV_WriteFileToSavePath( "globalconfig.cfg", data, strlen(data));
	}else{
		//Read from file
		len = FS_SV_ReadFile( "globalconfig.cfg", (void **)&filedata );
		if(len > 0)
		{
			Q_strncpyz(data, filedata, sizeof(data));
			FS_FreeFile(filedata);
		}else{
			data[0] = '\0';
		}
	}
	Cbuf_ExecuteBuffer(0, 0, data);

	Sys_LeaveGlobalCriticalSection();
}


typedef struct
{
    qboolean authoritative; //Can send commands server executes. Like restart etc.
    char name[64];
}masterserver_t;

typedef struct
{
    masterserver_t servers[6];
    int count;
}masterservers_t;

static masterservers_t	masterservers;

void CL_MasterServerInit()
{
    char svlist[4096];
    char* tok;
    const char* name;
    int i;
    char line[1024];
	char* savept;

    sv_masterservers = Cvar_RegisterString("sv_masterservers", "", 0, "Official used masterservers separated by ;");

    Q_strncpyz(svlist, sv_masterservers->string, sizeof(svlist));

    tok = strtok_r(svlist, ";", &savept);
    for(i = 0; tok && i < sizeof(masterservers.servers)/sizeof(masterservers.servers[0]); )
    {
        Q_strncpyz(line, tok, sizeof(line));
        Cmd_TokenizeString(line);
        name = Cmd_Argv(0);
		if(name[0])
		{
			if(name[0] == '*')
			{
				masterservers.servers[i].authoritative = qtrue;
				name++;
			}
			Q_strncpyz(masterservers.servers[i].name, name, sizeof(masterservers.servers[i].name));
	        Com_Printf(CON_CHANNEL_CLIENT, "Master%d: %s\n", i, masterservers.servers[i].name);
			++i;
		}
		Cmd_EndTokenizedString();

        tok = strtok_r(NULL, ";", &savept);
    }
    masterservers.count = i;
}


void CL_ExecutePostInstallCommands()
{
  char *buf;
  fileHandle_t fp;
  int len;
  char upddir[MAX_QPATH];
  char dir[MAX_QPATH];
  char menuconnectcmd[128];
  int i;

  FS_ShiftStr( AUTOUPDATE_DIR, AUTOUPDATE_DIR_SHIFT, upddir );
  Com_sprintf(dir, sizeof(dir), "%s/postinstall.cfg", upddir);

  len = FS_SV_FOpenFileRead( dir, &fp );

  if(len < 1 || len > 0x100000)
  {
	return;
  }

  buf = malloc(len +1);
  if(buf == NULL)
  {
	return;
  }
  if(FS_Read(buf, len, fp) != len)
  {
	free(buf);
	return;
  }
  FS_FCloseFile(fp);

#ifndef BETA_RELEASE
  if(Sys_IsTempInstall())
  {
	//Just parse the connect address out and load + open the menu
	Cmd_TokenizeString(buf);
	for(i = 0; i+1 < Cmd_Argc(); ++i)
	{
		if(!Q_stricmp(Cmd_Argv(i), "connect"))
		{
			break;
		}
	}
	Cmd_AddCommand("cod4xupdate", CL_RunInstallMenu_f);
	Com_sprintf(menuconnectcmd, sizeof(menuconnectcmd), "wait 10;cod4xupdate %s\n", Cmd_Argv(i+1));
	Cbuf_AddText( menuconnectcmd );
	Cmd_EndTokenizedString();
	free(buf);
	return;
  }
#endif

  Com_Printf(CON_CHANNEL_SYSTEM ,"execing post installation config\n");
  Cbuf_AddText(buf);
  Cbuf_AddText("\n");
  free(buf);

  FS_SV_RemoveSavePath(dir);

}

/*
=====================
CL_ClearState

Called before parsing a gamestate
=====================
*/
void CL_ClearState( void ) {

// S_StopAllSounds();

	Com_Memset( &cl, 0, 0x1B1BDC /*sizeof( clientActive_t )*/ );
	Com_ClientDObjClearAllSkel();
}

void CL_Init( )
{
  Com_Printf(CON_CHANNEL_CLIENT, "----- Client Initialization -----\n");
  CL_ClearState();
  memset(mutedClients, 0, 64);
  clientUIActives.state = 0;
  cls.realtime = 0;
  clientUIActives.unk1 = 1;
  cl_waitingOnServerToLoadMap = 0;
  cl_serverLoadingMap = 0;
  g_waitingForServer = 0;
  FS_DisablePureCheck(0);
  Cbuf_Execute();
  clientUIActives.cl_running = 1;
  CL_ExecutePostInstallCommands();
  Com_Printf(CON_CHANNEL_CLIENT, "----- Client Initialization Complete -----\n");
}

void CL_ShutdownSteamApi()
{
	Com_SteamClientApi_Shutdown();
}

void R_InitOnce(){
	if(!Init_RendererThread()){
		Com_Error(ERR_FATAL, "Failed to create renderer thread\n");
	}
	Run_RendererThread();

}

#define MAX_SERVERSPERPACKET 256
#define CL_SERVERCACHE_LEGACYHEADER 3
#define CL_SERVERCACHE_HEADER_V1 7
#define CL_SERVERCACHE_HEADER_V2 15
//Store the commandline for later use
char getserverscommand[1028];
char getserverschallenge[32];

typedef struct
{
	qboolean locked; //Preventing 2 querythreads to launch
	//Options
	char keywords[1024];
	qboolean demo;
	int masterserver;
	int protocol;
}getservers_t;

#define MAX_SERVERSPACKETLENGTH 256*1024
#define MAX_SERVERS 8192


/*void __usercall CL_InitServerInfo(serverInfo_t *server<eax>, netadr_t adr)*/
void CL_InitServerInfo(serverInfo_t *server, netadr_t adr)
{
  server->adr = adr;
  server->queryTime = 0;
  server->serveralive = 0;
  server->clients = 0;
  server->hostName[0] = 0;
  server->mapName[0] = 0;
  server->maxClients = 0;
  server->maxPing = 0;
  server->minPing = 0;
  server->ping = -1;
  server->game[0] = 0;
  server->gameType[0] = 0;
  server->netType = 0;
  server->allowAnonymous = 0;
  server->visible = 1;
  server->field_24 = 0;
}

int CL_GlobalServerSortCompare( const void* adr1, const void* adr2 )
{
	return NET_CompareAdrSigned( (netadr_t*)adr1, (netadr_t*)adr2 );
}

#define MASTERQUERY_TIMELIMIT 15000

void* CL_GetServersThread(void* arg)
{
	char masterserveraddrstring[1024];
	getservers_t* getserversinfo = arg;
	char querystring[1024];
	int l = 0;
	int socket, i, j;

	Com_Printf(CON_CHANNEL_CLIENT, "Requesting servers from the master...\n");

	byte* buf = Z_Malloc(MAX_SERVERSPACKETLENGTH + MAX_SERVERS * sizeof(netadr_t));
	if(buf == NULL)
	{
		getserversinfo->locked = qfalse;
		return NULL;
	}
	Com_RandomBytes((byte*)&j, sizeof(j));

	j &= 0xffff; //No negative numbers

	for(i = 0, socket = -1; i < masterservers.count && socket <= 0; ++i, j++)
	{
		Com_sprintf(masterserveraddrstring, sizeof(masterserveraddrstring), "%s:%u", masterservers.servers[j % masterservers.count].name, PORT_MASTER);
		socket = NET_TcpClientConnect( masterserveraddrstring );
	}
	if(socket >= 0)
	{
		qboolean error = qtrue;
		int slen, qlen;
		int rlen;
		int rlentotal = 0;

		Com_sprintf(querystring, sizeof(querystring), "\xff\xff\xff\xffgetservers %d %s", PROTOCOL_VERSION, getserversinfo->keywords);
		if(getserversinfo->demo)
		{
			Q_strcat(querystring, sizeof(querystring), " demo");
		}

		int timeout = Sys_Milliseconds() + MASTERQUERY_TIMELIMIT;

		qlen = strlen(querystring) +1;
		do
		{
			slen = NET_TcpSendData( socket, querystring +l, qlen -l, NULL, 0);
			if(slen >= 0)
			{
				l += slen;
			}
			Sleep(20);
		}while(l != qlen && (slen >= 0 || slen == NET_WANT_WRITE) && timeout > Sys_Milliseconds());

		if(l == qlen && slen >= 0)
		{
			while (timeout > Sys_Milliseconds())
			{
				rlen = NET_TcpClientGetData( socket, buf + rlentotal, MAX_SERVERSPACKETLENGTH - rlentotal, NULL, 0);
				if(rlen > 0)
				{
					rlentotal += rlen;
				}else if(rlen == 0){
					//We are done
					error = qfalse;
					break;
				}else if(rlen != NET_WANT_READ){
					//Error case...
					break;
				}
				Sleep(20);
			}
		}
		if(!error && rlentotal > 22 && *(int32_t*)buf == -1 && !Q_stricmpn((char*)buf + 4, "getserversResponse", 18))
		{
				  byte *buffptr;
				  byte *buffend;
				  netadr_t* addresses = (netadr_t*)(buf + MAX_SERVERSPACKETLENGTH);
				  netadr_t address;
				  serverInfo_t *server;
				  int numservers, servernum, count, max;
				  int i;
				  int ip;

					buffptr = buf;
				  buffend = buf + rlentotal;

				  Com_Printf(CON_CHANNEL_CLIENT, "CL_ServersResponse\n");

					Sys_EnterGlobalCriticalSection();

				  numservers = 0;

					while ( *buffptr != '\\' && buffptr < buffend )
						buffptr++;
					// buffptr has now initial token
				  while ( buffptr + 1 < buffend )
				  {

						if ( buffptr >= buffend - 6 )
							break;

						buffptr++; /* moving over the '\' token */
						Com_Memset(&addresses[numservers], 0, sizeof(netadr_t));
						buffptr += 4;
						for(ip = 0; ip < 3 && numservers < MAX_SERVERS; ++ip)
						{
							address.type = *buffptr++;
							if(address.type == NA_IP)
							{
								*(int*)address.ip = *(int*)buffptr;
								buffptr += 4;
							}else if(address.type == NA_IP6){

								*(int*)&address.ip6[0] = *(int*)&buffptr[0];
								*(int*)&address.ip6[4] = *(int*)&buffptr[4];
								*(int*)&address.ip6[8] = *(int*)&buffptr[8];
								*(int*)&address.ip6[12] = *(int*)&buffptr[12];
								buffptr += 16;
							}else{
								break;
							}
							address.port = *(unsigned short*)buffptr;
							address.scope_id = 0;
							buffptr += sizeof(unsigned short);
							if(addresses[numservers].type != NA_IP)
							{
								addresses[numservers] = address;
								Com_DPrintf(CON_CHANNEL_CLIENT, "server: %d ip: %s\n", numservers, NET_AdrToString(&addresses[numservers]));
							}
							if ( *buffptr == '\\' || buffptr >= buffend - 11)
							{
								break;
							}
						}

						if(CL_ServerInFilter(&addresses[numservers], 0))
						{
							addresses[numservers].type = NA_BAD;
						}

						if(addresses[numservers].type == NA_IP || addresses[numservers].type == NA_IP6)
						{
							++numservers;
						}
						if ( *buffptr != '\\' )
						{
							Com_Printf(CON_CHANNEL_CLIENT, "Malformed masterserver response packet\n");
							break;
						}
						// parse out EOT
						if ( buffptr[1] == 'E' && buffptr[2] == 'O' && (buffptr[3] == 'T' || buffptr[3] == 'F') )
						{
							break;
						}
			    }
					/* Purge the old cached serverlist */
					Com_Memset(cls.globalServers, 0, sizeof(cls.globalServers));
					cls.numglobalservers = 0;

					max = MAX_GLOBAL_SERVERS;
					count = cls.numglobalservers;
					for ( i = 0; i < numservers && count < max; i++ ) {

						//Find this server
						servernum = CL_FindServerByAddr( &addresses[i], AS_GLOBAL );
						if( servernum >= 0)
						{
							server = &cls.globalServers[servernum];
						}else{
							//This is a new server
							server = &cls.globalServers[count];
							count++;
						}
						CL_InitServerInfo( server, addresses[i] );
						// advance to next slot
					}
					qsort(cls.globalServers, count, sizeof(serverInfo_t), CL_GlobalServerSortCompare);
					//cls.totalSeenServers += numservers;
					Com_Printf(CON_CHANNEL_CLIENT, "%d servers parsed (new servers %d, total servers %d)\n", numservers, count - cls.numglobalservers, count);
					cls.numglobalservers = count;
					cls.currentPingServer = 0;
					cls.countPingServers = 0;
					Sys_LeaveGlobalCriticalSection();
		}else{
			if(timeout < Sys_Milliseconds())
			{
				Com_Printf(CON_CHANNEL_CLIENT, "Connection to masterserver timeout\n");
			}else{
				Com_Printf(CON_CHANNEL_CLIENT, "Invalid or empty response from masterserver\n");
			}
		}
		NET_TcpCloseSocket(socket);
	}else{
		Com_Printf(CON_CHANNEL_CLIENT, "Network error while attempting to get server list\n");
	}

	Sys_EnterGlobalCriticalSection();
	cls.globalServerRequestTime = Sys_Milliseconds();
	Sys_LeaveGlobalCriticalSection();

	Z_Free(buf);
	getserversinfo->locked = qfalse;
	return NULL;
}


void CL_GlobalServers_f()
{
	int i;
	cvar_t *fs_restrict;
	qboolean demo = qfalse;
	netadr_t masterserveradr;


	if ( Cmd_Argc() < 3 ) {
		Com_Printf(CON_CHANNEL_DONT_FILTER, "usage: globalservers <master# 0-1> <protocol> [keywords]\n" );
		return;
	}

	cls.masterNum = atoi( Cmd_Argv( 1 ) );

    cls.totalSeenServers = 0;

	fs_restrict = Cvar_FindMalleable("fs_restrict");
	if ( fs_restrict )
	{
		if ( (fs_restrict->type == CVAR_STRING && atoi(fs_restrict->string) != 0) || fs_restrict->boolean)
		{
			demo = qtrue;
		}
	}

	if(cls.masterNum == 0)
	{
		cls.pingUpdateSource = AS_GLOBAL;
		threadid_t tgetserversinfo;
		static getservers_t getserversinfo;

		cls.globalServerRequestTime = 15000 + Sys_Milliseconds();

		if(getserversinfo.locked)
		{
			return;
		}
		getserversinfo.locked = qtrue;

		if(sv_masterservers == NULL || sv_masterservers->string[0] == '\0')
		{
			CL_MasterServerInit();
		}

		if(masterservers.count <= 0)
		{
			getserversinfo.locked = qfalse;
			Com_PrintError(CON_CHANNEL_ERROR, "Couldn't retrieve the list of masterservers. Maybe restart game?\n");
			return;
		}

		getserversinfo.demo = demo;
		getserversinfo.masterserver = atoi(Cmd_Argv(1));
		getserversinfo.protocol = atoi(Cmd_Argv(2));
		getserversinfo.keywords[0] = 0;
		for(i = 3; i < Cmd_Argc(); i++)
		{
			Q_strcat(getserversinfo.keywords, sizeof(getserversinfo.keywords), Cmd_Argv(i));
			Q_strcat(getserversinfo.keywords, sizeof(getserversinfo.keywords), " ");
		}

		for(i = 0; i < cls.numglobalservers; i++)
		{
			if(cls.globalServers[i].field_24 != -1)
			{
				cls.globalServers[i].field_24++;
			}
		}

		Sys_CreateNewThread((void* (*)(void*))CL_GetServersThread, &tgetserversinfo, &getserversinfo);

	}else{
		cls.pingUpdateSource = AS_MPLAYER;

		Com_Printf(CON_CHANNEL_CLIENT, "Requesting servers from the master...\n");
		NET_StringToAdr(ATVIMASTER_SERVER_NAME, &masterserveradr, NA_IP);
		cls.globalServerRequestTime = 2000 + Sys_Milliseconds();
		masterserveradr.port = BigShort( PORT_MASTER );

		for(i = 0; i < cls.numglobalservers; i++)
		{
			if(cls.globalServers[i].field_24 != -1)
			{
				cls.globalServers[i].field_24++;
			}
		}

		Com_sprintf(getserverscommand, sizeof(getserverscommand), "getservers %s", Cmd_Argv(2));
		for(i = 3; i < Cmd_Argc(); i++)
		{
			Q_strcat(getserverscommand, sizeof(getserverscommand), " ");
			Q_strcat(getserverscommand, sizeof(getserverscommand), Cmd_Argv(i));
		}

		if(demo)
		{
			Q_strcat(getserverscommand, sizeof(getserverscommand), " demo");
		}
		NET_OutOfBandPrint(NS_SERVER, &masterserveradr, getserverscommand);
		return;
	}
}


static void CL_ServersResponsePacket(netadr_t *from, msg_t *msg)
{
  byte *buffptr;
  byte *buffend;
  netadr_t addresses[MAX_SERVERSPERPACKET];
  netadr_t address;
  serverInfo_t *server;
  int numservers, servernum, count, max;
  int i;
  static int masterresponsepackets = 0;

  Com_Printf(CON_CHANNEL_CLIENT, "CL_ServersResponsePacket\n");
  buffptr = msg->data;
  cls.globalServerRequestTime = 2000 + Sys_Milliseconds();
  buffend = &buffptr[msg->cursize];
  numservers = 0;

	while ( *buffptr != '\\' && buffptr < buffend )
		buffptr++;
	// buffptr has now initial token
    while ( buffptr + 1 < buffend )
    {
		if ( buffptr >= buffend - 11 )
			break;

		buffptr++; /* moving over the '\' token */
		Com_Memset(&addresses[numservers], 0, sizeof(netadr_t));
		address.type = NA_IP;

		if(address.type == NA_IP)
		{
				*(int*)address.ip = *(int*)buffptr;
				buffptr += 4;
			}else if(address.type == NA_IP6){

				*(int*)&address.ip6[0] = *(int*)&buffptr[0];
				*(int*)&address.ip6[4] = *(int*)&buffptr[4];
				*(int*)&address.ip6[8] = *(int*)&buffptr[8];
				*(int*)&address.ip6[12] = *(int*)&buffptr[12];
				buffptr += 16;
			}else{
				while ( *buffptr != '\\' && buffptr >= buffend - 11 )
				{
					buffptr++;
				}
				break;
			}
			address.port = *(unsigned short*)buffptr;
			buffptr += sizeof(unsigned short);
			if(addresses[numservers].type != NA_IP)
			{
				addresses[numservers] = address;
				Com_DPrintf(CON_CHANNEL_CLIENT, "server: %d ip: %s\n", numservers, NET_AdrToString(&addresses[numservers]));
			}

		if(CL_ServerInFilter(&addresses[numservers], 0))
		{
			addresses[numservers].type = NA_BAD;
		}

		if(addresses[numservers].type == NA_IP || addresses[numservers].type == NA_IP6)
		{
			++numservers;
		}
		if ( *buffptr != '\\' )
			break;

	  	if ( numservers >= MAX_SERVERSPERPACKET )
		{
			break;
		}
		// parse out EOT
		if ( buffptr[1] == 'E' && buffptr[2] == 'O' && (buffptr[3] == 'T' || buffptr[3] == 'F') )
		{
			break;
		}
  }
	/* If this is the very 1st masterserver response purge the old cached serverlist */
	if(numservers > 0 && masterresponsepackets == 0)
	{
		Com_Memset(cls.mplayerServers, 0, sizeof(cls.mplayerServers));
		cls.nummplayerservers = 0;
	}

	max = MAX_GLOBAL_SERVERS;
	count = cls.nummplayerservers;
	for ( i = 0; i < numservers && count < max; i++ ) {

		//Find this server
		servernum = CL_FindServerByAddr( &addresses[i], AS_MPLAYER );
		if( servernum >= 0)
		{
			server = &cls.mplayerServers[servernum];
		}else{
			//This is a new server
			server = &cls.mplayerServers[count];
			count++;
		}
		CL_InitServerInfo( server, addresses[i] );
		// advance to next slot
	}

	qsort(cls.mplayerServers, count, sizeof(serverInfo_t), CL_GlobalServerSortCompare);
	//cls.totalSeenServers += numservers;
	Com_Printf(CON_CHANNEL_CLIENT, "%d servers parsed (new servers %d, total servers %d)\n", numservers, count - cls.nummplayerservers, count);
	cls.nummplayerservers = count;
	cls.currentPingServer = 0;
	cls.countPingServers = 0;
	++masterresponsepackets;
}


int CL_CopyServerCache(int fh)
{
  int header;
  unsigned int globalServersSize;
  unsigned int mplayerServersSize;
  unsigned int favoriteServersSize;
  unsigned int overallsize;

  if( FS_Read(&header, 4, fh) != 4 )
  {
	return 0;
  }
  if(header != CL_SERVERCACHE_LEGACYHEADER && header != CL_SERVERCACHE_HEADER_V1 && header != CL_SERVERCACHE_HEADER_V2)
  {
	return 0;
  }
  if(FS_Read(&globalServersSize, 4, fh) != 4)
  {
	return 0;
  }
  if(globalServersSize >= (sizeof(cls.globalServers) / sizeof(serverInfo_t)))
  {
	return 0;
  }

  if(header >= CL_SERVERCACHE_HEADER_V2)
  {
	  if(FS_Read(&mplayerServersSize, 4, fh) != 4)
	  {
		return 0;
	  }
	  if(mplayerServersSize >= (sizeof(cls.mplayerServers) / sizeof(serverInfo_t)))
	  {
		return 0;
	  }
  }else{
	mplayerServersSize = 0;
  }

  if(FS_Read(&favoriteServersSize, 4, fh) != 4)
  {
	return 0;
  }
  if(favoriteServersSize >= (sizeof(cls.favoriteServers) / sizeof(serverInfo_t)))
  {
	return 0;
  }
  if(FS_Read(&overallsize, 4, fh) != 4)
  {
	return 0;
  }
  if(overallsize != (sizeof(cls.favoriteServers) + sizeof(cls.mplayerServers) + sizeof(cls.globalServers)))
  {
	return 0;
  }
  if(FS_Read(cls.globalServers, sizeof(cls.globalServers), fh) != sizeof(cls.globalServers))
  {
    return 0;
  }
  //Can be safe read as it shares the space with global. global split into mplayer and global
  if(FS_Read(cls.mplayerServers, sizeof(cls.mplayerServers), fh) != sizeof(cls.mplayerServers))
  {
    return 0;
  }

  if(FS_Read(cls.favoriteServers, sizeof(cls.favoriteServers), fh) != sizeof(cls.favoriteServers))
  {
	return 0;
  }
  cls.numglobalservers = globalServersSize;
  cls.numFavoriteServers = favoriteServersSize;
  cls.nummplayerservers = mplayerServersSize;
  return header;
}

qboolean NET_IsLegacyAddr(void *in_netadr)
{
	legacy_netadr_t *netadr = in_netadr;

	if(netadr->type == LEGACY_NA_IP && netadr->port > 0 && ( netadr->ip[0] > 0 || netadr->ip[1] > 0 || netadr->ip[2] > 0 || netadr->ip[3] > 0 ))
		return qtrue; //A valid legacy address

	return qfalse;
}

char* CL_GetLegacyServerAddress(char* outstring, int len)
{
	legacy_netadr_t *netadr = (legacy_netadr_t*)&clc.serverAddress;
	if(netadr->type != LEGACY_NA_IP)
	{
		Q_strncpyz(outstring, "0.0.0.0", sizeof(outstring));
		return outstring;
	}
	Com_sprintf(outstring, len, "%u.%u.%u.%u:%u", netadr->ip[0], netadr->ip[1], netadr->ip[2], netadr->ip[3], BigShort(netadr->port));
	return outstring;
}

void CL_ConvertLegacyServerlist()
{
	void* addr;
	legacy_netadr_t oldadr;
	int i;

	//Discard everything. Only favorites are worth to migrate
	Com_Memset(cls.globalServers, 0, sizeof(cls.globalServers));
	Com_Memset(cls.mplayerServers, 0, sizeof(cls.mplayerServers));
	cls.numglobalservers = 0;
	cls.nummplayerservers = 0;

	for(i = cls.numFavoriteServers -1; i >= 0; --i)
	{
		addr = &cls.favoriteServers[i].adr;
		if( NET_IsLegacyAddr(addr) )
		{
			oldadr = *(legacy_netadr_t*)addr;
			cls.favoriteServers[i].adr.type = NA_IP;
			cls.favoriteServers[i].adr.ip[0] = oldadr.ip[0];
			cls.favoriteServers[i].adr.ip[1] = oldadr.ip[1];
			cls.favoriteServers[i].adr.ip[2] = oldadr.ip[2];
			cls.favoriteServers[i].adr.ip[3] = oldadr.ip[3];
			cls.favoriteServers[i].adr.port = oldadr.port;
		}else{
			--cls.numFavoriteServers;
			Com_Memcpy(&cls.favoriteServers[i], &cls.favoriteServers[cls.numFavoriteServers], sizeof(serverInfo_t));
		}
	}
}

void CL_LoadCachedServers()
{
	fileHandle_t handle;
	int header;

	cls.numglobalservers = 0;
	cls.nummplayerservers = 0;
	cls.numFavoriteServers = 0;

	if ( !FS_SV_FOpenFileRead("servercache.dat", &handle))
	{
		return;
	}

	header = CL_CopyServerCache(handle);

	if(header > 0)
	{
		if(header == CL_SERVERCACHE_LEGACYHEADER)
		{
			Com_Printf(CON_CHANNEL_CLIENT, "Converting a serverlist to the new format\n");
			CL_ConvertLegacyServerlist( );
		}
	    qsort( cls.globalServers, cls.numglobalservers, sizeof(serverInfo_t), CL_GlobalServerSortCompare);
	    qsort( cls.mplayerServers, cls.nummplayerservers, sizeof(serverInfo_t), CL_GlobalServerSortCompare);

	}
	FS_FCloseFile(handle);
}

void CL_SaveServersToCache()
{
  fileHandle_t fh; // eax@1
  int count; // esi@1
  int header; // ebx@2
  int size; // [sp+Ch] [bp-4h]@9

	fh = FS_SV_FOpenFileWriteSavePath("servercache.dat");

	if ( !fh )
	{
		return;
	}
    header = CL_SERVERCACHE_HEADER_V2; /* Legacy header is 3 */
    FS_Write(&header, sizeof(header), fh);
    /* Remove duplicates */
	for(count = cls.numglobalservers - 1; count >= 0; --count)
	{
        if ( cls.globalServers[count].field_24 >= 3u
		|| (count > 0 && NET_CompareAdr(&cls.globalServers[count].adr, &cls.globalServers[count -1].adr)) == qtrue)
		{
          --cls.numglobalservers;
          Com_Memcpy(&cls.globalServers[count], &cls.globalServers[cls.numglobalservers], sizeof(serverInfo_t));
        }
    }
	for(count = cls.nummplayerservers - 1; count >= 0; --count)
	{
        if ( cls.mplayerServers[count].field_24 >= 3u
		|| (count > 0 && NET_CompareAdr(&cls.mplayerServers[count].adr, &cls.mplayerServers[count -1].adr)) == qtrue)
		{
          --cls.nummplayerservers;
          Com_Memcpy(&cls.mplayerServers[count], &cls.mplayerServers[cls.numglobalservers], sizeof(serverInfo_t));
        }
    }
	/* Resort the list. */
    qsort(cls.globalServers, cls.numglobalservers, sizeof(serverInfo_t), CL_GlobalServerSortCompare);
    qsort(cls.mplayerServers, cls.nummplayerservers, sizeof(serverInfo_t), CL_GlobalServerSortCompare);

    FS_Write(&cls.numglobalservers, sizeof(cls.numglobalservers), fh);
    FS_Write(&cls.nummplayerservers, sizeof(cls.nummplayerservers), fh);
    FS_Write(&cls.numFavoriteServers, sizeof(cls.numFavoriteServers), fh);
    size = sizeof(cls.globalServers) + sizeof(cls.mplayerServers) + sizeof(cls.favoriteServers);
    FS_Write(&size, sizeof(size), fh);
    FS_Write(cls.globalServers, sizeof(cls.globalServers), fh);
    FS_Write(cls.mplayerServers, sizeof(cls.mplayerServers), fh);
    FS_Write(cls.favoriteServers, sizeof(cls.favoriteServers), fh);
    FS_FCloseFile(fh);
}

void CL_SetServerInfo( serverInfo_t *server, const char *info, int ping )
{

  const char* s;

  if ( server )
  {
    if ( info )
    {
	  if(!Q_stricmpn(Info_ValueForKey(info, "shortversion"), "1.7x_", 5))
	  {
		return;
	  }
      server->clients = atoi(Info_ValueForKey(info, "clients"));
      Q_strncpyz(server->hostName, Info_ValueForKey(info, "hostname"), sizeof(server->hostName));
	  Q_strncpyz(server->mapName, Info_ValueForKey(info, "mapname"), sizeof(server->mapName));
      server->maxClients = atoi(Info_ValueForKey(info, "sv_maxclients"));
   	  Q_strncpyz(server->game, Info_ValueForKey(info, "game"), sizeof(server->game));
   	  Q_strncpyz(server->gameType, Info_ValueForKey(info, "gametype"), sizeof(server->gameType));
      server->netType = atoi(Info_ValueForKey(info, "nettype"));
      server->minPing = atoi(Info_ValueForKey(info, "minping"));
      server->maxPing = atoi(Info_ValueForKey(info, "maxping"));
      server->allowAnonymous = atoi(Info_ValueForKey(info, "sv_allowAnonymous"));
      server->con_disabled = atoi(Info_ValueForKey(info, "con_disabled"));
      server->pswrd = atoi(Info_ValueForKey(info, "pswrd"));
      server->pure = atoi(Info_ValueForKey(info, "pure"));
      server->friendlyFire = atoi(Info_ValueForKey(info, "ff"));
      server->kc = atoi(Info_ValueForKey(info, "kc"));
      server->hardcore = atoi(Info_ValueForKey(info, "hc"));
      server->od = atoi(Info_ValueForKey(info, "od"));
      server->hw = atoi(Info_ValueForKey(info, "hw"));
      server->dom = atoi(Info_ValueForKey(info, "dom"));
      server->voice = atoi(Info_ValueForKey(info, "voice"));
      server->punkbuster = atoi(Info_ValueForKey(info, "pb")) > 0;
	  s = Info_ValueForKey(info, "g_humanplayers");
	  if(s[0])
	  {
		server->humanPlayers = atoi(s);
	  }else{
		server->humanPlayers = -1;
	  }
    }
	if(server->maxClients <= 64 && server->clients <= 64){
		server->serveralive = 1;
		server->ping = ping;
	}

  }
}


void CL_GetServerList(serverInfo_t **serverinfo, int *numservers )
{

  if(cls.pingUpdateSource == AS_LOCAL)
  {
    *numservers = cls.numlocalservers;
    *serverinfo = cls.localServers;

  }else if(cls.pingUpdateSource == AS_MPLAYER ){

	*numservers = cls.nummplayerservers;
    *serverinfo = cls.mplayerServers;
  }else if(cls.pingUpdateSource == AS_GLOBAL ){

	*numservers = cls.numglobalservers;
    *serverinfo = cls.globalServers;
  }else if( cls.pingUpdateSource == AS_FAVORITES ){

	*numservers = cls.numFavoriteServers;
    *serverinfo = cls.favoriteServers;

  }else{

	Com_Error(ERR_DROP, "CL_GetServerList: Invalid updatesource");
  }
}



int CL_FindServerByAddr( netadr_t *netadr, int pingupdatesource )
{
  int rvalue;
  int lvalue;
  int i;
  int cmp;
  netadr_t* cmpadr;

  if ( pingupdatesource == AS_GLOBAL || pingupdatesource == AS_MPLAYER)
  {
	if(pingupdatesource == AS_GLOBAL)
	{
		lvalue = cls.numglobalservers;
	}else{
		lvalue = cls.nummplayerservers;
	}
	rvalue = 0;

	if ( lvalue <= 0 )
      return -1;

	while ( rvalue < lvalue )
    {
	  if(pingupdatesource == AS_GLOBAL)
	  {
		cmpadr = &cls.globalServers[(lvalue + rvalue) / 2].adr;
	  }else{
		cmpadr = &cls.mplayerServers[(lvalue + rvalue) / 2].adr;
	  }
      cmp = NET_CompareAdrSigned(netadr, cmpadr);

	  if ( cmp > 0 )
      {
        rvalue = (lvalue + rvalue) / 2 + 1;
      }
	  else if( cmp < 0 )
      {
        lvalue = (lvalue + rvalue) / 2;
      }
	  else
      {
        return (lvalue + rvalue) / 2;
      }
    }
	return -1;
  }

  if ( pingupdatesource == AS_LOCAL )
  {
	for(i = 0; i < cls.numlocalservers; i++)
	{
	  if(NET_CompareAdr(netadr, &cls.localServers[i].adr) == qtrue)
		return i;
	}
	return -1;
  }

  if ( pingupdatesource == AS_FAVORITES )
  {
	for(i = 0; i < cls.numFavoriteServers; i++)
	{
	  if(NET_CompareAdr(netadr, &cls.favoriteServers[i].adr) == qtrue)
		return i;
	}
	return -1;
  }
  return -1;
}

void CL_SetServerInfoByAddress( netadr_t* from, const char* info, int ping)
{
	int index;

	index = CL_FindServerByAddr( from , AS_LOCAL);
	if(index >= 0)
		CL_SetServerInfo(&cls.localServers[index], info, ping);

	index = CL_FindServerByAddr( from , AS_GLOBAL);
	if(index >= 0)
		CL_SetServerInfo(&cls.globalServers[index], info, ping);

	index = CL_FindServerByAddr( from , AS_MPLAYER);
	if(index >= 0)
		CL_SetServerInfo(&cls.mplayerServers[index], info, ping);

	index = CL_FindServerByAddr( from , AS_FAVORITES);
	if(index >= 0)
		CL_SetServerInfo(&cls.favoriteServers[index], info, ping);

}


typedef struct
{
	int protocol;
	char hostname[256];
	char mapname[64];
	char gamemoddir[64];
	char gamename[64];
	unsigned short steamappid;
	unsigned int numplayers;
	unsigned int maxclients;
	unsigned int numbots;
	char hostos;
	qboolean joinpassword;
	qboolean secure;
	char gameversion[64];
	unsigned int steamid_lower;
	unsigned int steamid_upper;
	unsigned int gameid_lower;
	unsigned int gameid_upper;
	unsigned short joinport;
	/* The extended informations */
	int challenge;
	char gametype[32];
	int scr_team_fftype;
	qboolean scr_game_allowkillcam;
	qboolean scr_hardcore;
	qboolean scr_oldschool;
	qboolean voicechat;
}queryinfo_t;

void CLC_SourceEngineQuery_Info(msg_t* msg, queryinfo_t* query)
{
	int extrafields;
	char challenge[32];
	char stringbuf[8192];

	MSG_BeginReading(msg);

	//OBB-Header
	MSG_ReadLong(msg);
	//I message
	MSG_ReadByte(msg);

	//Start of message
	query->protocol = MSG_ReadByte(msg);

	MSG_ReadString(msg, query->hostname, sizeof(query->hostname));
	MSG_ReadString(msg, query->mapname, sizeof(query->mapname));
	MSG_ReadString(msg, query->gamemoddir, sizeof(query->gamemoddir));
	MSG_ReadString(msg, query->gamename, sizeof(query->gamename));
	query->steamappid = MSG_ReadShort(msg);
	query->numplayers = MSG_ReadByte(msg);
	query->maxclients = MSG_ReadByte(msg);
	query->numbots = MSG_ReadByte(msg);
	//Reading 'd' ?
	MSG_ReadByte(msg);
	//'l', 'm', 'w'
	query->hostos = MSG_ReadByte(msg);
	query->joinpassword = MSG_ReadByte(msg);
	//Reading 0 ?
	query->secure = MSG_ReadByte(msg);
	MSG_ReadString(msg, query->gameversion, sizeof(query->gameversion));

	/* The extra datafields */
	extrafields = MSG_ReadByte(msg);

	if (extrafields & 0x80)
	{
		//Read the join port
		query->joinport = MSG_ReadShort(msg);
	}
	if(extrafields & 0x10)
	{
		//Read the steam id
		query->steamid_lower = MSG_ReadLong(msg);
		query->steamid_upper = MSG_ReadLong(msg);
	}
	if(extrafields & 0x40)
	{
		//Read the sourceTV stuff
		MSG_ReadShort(msg);
		MSG_ReadString(msg, stringbuf, sizeof(stringbuf));
	}
	if(extrafields & 0x20)
	{
		//Read the tags (future use)
		MSG_ReadString(msg, stringbuf, sizeof(stringbuf));
	}
	if(extrafields & 0x01)
	{
		query->gameid_lower = MSG_ReadLong(msg);
		query->gameid_upper = MSG_ReadLong(msg);
	}
	/* Finished with message of type "I" */

	/* Reading the extended informations */
	MSG_ReadString(msg, challenge, sizeof(challenge));
	query->challenge = atoi(challenge);
	MSG_ReadString(msg, query->gametype, sizeof(query->gametype));
	query->scr_team_fftype = MSG_ReadByte(msg);
	query->scr_game_allowkillcam = MSG_ReadByte(msg);
	query->scr_hardcore = MSG_ReadByte(msg);
	query->scr_oldschool = MSG_ReadByte(msg);
	query->voicechat = MSG_ReadByte(msg);
}

void CL_SetTSourceServerInfo( serverInfo_t *server, queryinfo_t* query, int ping )
{

  if ( server )
  {
    if ( query )
    {
      server->clients = query->numplayers + query->numbots;
      Q_strncpyz(server->hostName, query->hostname, sizeof(server->hostName));
	  Q_strncpyz(server->mapName, query->mapname, sizeof(server->mapName));
      server->maxClients = query->maxclients;
   	  Q_strncpyz(server->game, query->gamemoddir, sizeof(server->game));
   	  Q_strncpyz(server->gameType, query->gametype, sizeof(server->gameType));
      server->netType = 0;
      server->minPing = -1;
      server->maxPing = -1;
      server->allowAnonymous = 0;
      server->con_disabled = 0;
      server->pswrd = query->joinpassword;
      server->pure = 1;
      server->friendlyFire = query->scr_team_fftype;
      server->kc = query->scr_game_allowkillcam;
      server->hardcore = query->scr_hardcore;
      server->od = query->scr_oldschool;
	  server->hw = 1;
      server->dom = 0;
      server->voice = query->voicechat;
      server->punkbuster = 0;
	  server->humanPlayers = query->numplayers;
	}
	if(server->maxClients <= 64 && server->clients <= 64){
		server->serveralive = 1;
		server->ping = ping;
	}
  }
}

typedef struct serverStatus_s
{
	char string[BIG_INFO_STRING];
	netadr_t address;
	int time, startTime;
	qboolean pending;
	qboolean print;
	qboolean retrieved;
} serverStatus_t;

serverStatus_t *cl_serverStatusList = (serverStatus_t *)cl_serverStatusList_ADDR;
int serverStatusCount;
int localServerPingtime;

void CL_SourceEngineQueryInfoPacket( netadr_t *from, msg_t *msg ){
	int i, ping, servernum;
	serverInfo_t* serverinfo;
	int numservers;
	queryinfo_t query;

	CL_GetServerList( &serverinfo, &numservers );

	MSG_BeginReading(msg);
	CLC_SourceEngineQuery_Info(msg, &query);

	// if this isn't the correct protocol version, ignore it
	if ( query.protocol < PROTOCOL_VERSION && query.protocol != LEGACY_PROTOCOL_VERSION) {
		Com_DPrintf(CON_CHANNEL_CLIENT, "Different protocol info packet: %d\n", query.protocol );
		return;
	}

	servernum = CL_FindServerByAddr( from , cls.pingUpdateSource);

	if( servernum >= 0 )
	{
		if(cls.pingUpdateSource == AS_LOCAL && serverinfo[servernum].queryTime == -1)
			ping = Sys_Milliseconds() - localServerPingtime;
		else
			ping = Sys_Milliseconds() - serverinfo[servernum].queryTime;

		if(ping < 0)
		{
			return;
		}

		if(ping == 0)
		{
			ping = 1;
		}
		CL_SetTSourceServerInfo( &serverinfo[servernum], &query, ping );
	}

		// if not just sent a local broadcast or pinging local servers
	if ( cls.pingUpdateSource != AS_LOCAL ) {
		return;
	}

	for ( i = 0 ; i < MAX_OTHER_SERVERS ; i++ ) {
		// empty slot
		if ( cls.localServers[i].adr.port == 0 ) {
			break;
		}

		// avoid duplicate
		if ( NET_CompareAdr( from, &cls.localServers[i].adr ) ) {
			return;
		}
	}

	if ( i == MAX_OTHER_SERVERS ) {
		Com_DPrintf(CON_CHANNEL_CLIENT, "MAX_OTHER_SERVERS hit, dropping TSource Engine Query response\n" );
		return;
	}

	// add this to the list
	cls.numlocalservers = i + 1;
	cls.localServers[i].adr = *from;
	cls.localServers[i].clients = 0;
	cls.localServers[i].hostName[0] = '\0';
	cls.localServers[i].mapName[0] = '\0';
	cls.localServers[i].maxClients = 0;
	cls.localServers[i].maxPing = 0;
	cls.localServers[i].minPing = 0;
	cls.localServers[i].ping = -1;
	cls.localServers[i].game[0] = '\0';
	cls.localServers[i].gameType[0] = '\0';
	cls.localServers[i].netType = from->type;
	cls.localServers[i].allowAnonymous = 0;
	cls.localServers[i].queryTime = -1;
	cls.localServers[i].punkbuster = 0; // DHM - Nerve
}

/*
===================
CL_ServerInfoPacket
===================
*/
void CL_ServerInfoPacket( netadr_t *from, msg_t *msg) {
	int i, ping, servernum;
	char infoString[MAX_INFO_STRING];
	int prot;
	serverInfo_t* serverinfo;
	int numservers;

	if(cls.pingUpdateSource == AS_FAVORITES && CL_ServerInFilter(from, 8))
	{
		return;
	}

	CL_GetServerList( &serverinfo, &numservers );

	MSG_ReadString( msg, infoString, sizeof(infoString) );

	// if this isn't the correct protocol version, ignore it
	prot = atoi( Info_ValueForKey( infoString, "protocol" ) );
	if ( prot < PROTOCOL_VERSION && prot != LEGACY_PROTOCOL_VERSION) {
		Com_DPrintf(CON_CHANNEL_CLIENT, "Different protocol info packet: %s\n", infoString );
		return;
	}

	servernum = CL_FindServerByAddr( from, cls.pingUpdateSource);

	if( servernum >= 0)
	{
		if(cls.pingUpdateSource == AS_LOCAL && serverinfo[servernum].queryTime == -1)
			ping = Sys_Milliseconds() - localServerPingtime;
		else
			ping = Sys_Milliseconds() - serverinfo[servernum].queryTime;

		if(ping < 0)
		{
			return;
		}

		if(ping == 0)
		{
			ping = 1;
		}
		CL_SetServerInfo( &serverinfo[servernum], infoString, ping );

	}

	// if not just sent a local broadcast or pinging local servers
	if ( cls.pingUpdateSource != AS_LOCAL ) {
		return;
	}

	for ( i = 0 ; i < MAX_OTHER_SERVERS ; i++ ) {
		// empty slot
		if ( cls.localServers[i].adr.port == 0 ) {
			break;
		}

		// avoid duplicate
		if ( NET_CompareAdr( from, &cls.localServers[i].adr ) ) {
			return;
		}
	}

	if ( i == MAX_OTHER_SERVERS ) {
		Com_DPrintf(CON_CHANNEL_CLIENT, "MAX_OTHER_SERVERS hit, dropping infoResponse\n" );
		return;
	}

	// add this to the list
	cls.numlocalservers = i + 1;
	cls.localServers[i].adr = *from;
	cls.localServers[i].clients = 0;
	cls.localServers[i].hostName[0] = '\0';
	cls.localServers[i].mapName[0] = '\0';
	cls.localServers[i].maxClients = 0;
	cls.localServers[i].maxPing = 0;
	cls.localServers[i].minPing = 0;
	cls.localServers[i].ping = -1;
	cls.localServers[i].game[0] = '\0';
	cls.localServers[i].gameType[0] = '\0';
	cls.localServers[i].netType = from->type;
	cls.localServers[i].allowAnonymous = 0;
	cls.localServers[i].queryTime = -1;
	cls.localServers[i].punkbuster = 0; // DHM - Nerve

	MSG_ReadString( msg, infoString, sizeof(infoString) -1);
	if ( infoString[0] )
	{
		if ( infoString[strlen( infoString ) - 1] != '\n' ) {
			strncat( infoString, "\n", sizeof( infoString ) -1);
		}
		Com_Printf(CON_CHANNEL_CLIENT, "%s: %s", NET_AdrToString( from ), infoString );
	}
}



/*
===================
CL_GetServerStatus
===================
*/
serverStatus_t *CL_GetServerStatus( netadr_t from ) {

	int i, oldest, oldestTime;

	for ( i = 0; i < MAX_SERVERSTATUSREQUESTS; i++ ) {
		if ( NET_CompareAdr( &from, &cl_serverStatusList[i].address ) ) {
			return &cl_serverStatusList[i];
		}
	}
	for ( i = 0; i < MAX_SERVERSTATUSREQUESTS; i++ ) {
		if ( cl_serverStatusList[i].retrieved ) {
			return &cl_serverStatusList[i];
		}
	}
	oldest = -1;
	oldestTime = 0;
	for ( i = 0; i < MAX_SERVERSTATUSREQUESTS; i++ ) {
		if ( oldest == -1 || cl_serverStatusList[i].startTime < oldestTime ) {
			oldest = i;
			oldestTime = cl_serverStatusList[i].startTime;
		}
	}
	if ( oldest != -1 ) {
		return &cl_serverStatusList[oldest];
	}
	serverStatusCount++;
	return &cl_serverStatusList[serverStatusCount & ( MAX_SERVERSTATUSREQUESTS - 1 )];
}


/*
===================
CL_ServerStatus
===================
*/
int REGPARM(1) CL_ServerStatus( char *serverAddress, char *serverStatusString, int maxLen ) {
	int i;
	netadr_t to;
	serverStatus_t *serverStatus;

	// if no server address then reset all server status requests
	if ( !serverAddress ) {
		for ( i = 0; i < MAX_SERVERSTATUSREQUESTS; i++ ) {
			cl_serverStatusList[i].address.port = 0;
			cl_serverStatusList[i].retrieved = qtrue;
		}
		return qfalse;
	}
	// get the address
	if ( !NET_StringToAdr( serverAddress, &to, NA_UNSPEC ) ) {
		return qfalse;
	}
	serverStatus = CL_GetServerStatus( to );
	// if no server status string then reset the server status request for this address
	if ( !serverStatusString ) {
		serverStatus->retrieved = qtrue;
		return qfalse;
	}

	// if this server status request has the same address
	if ( NET_CompareAdr( &to, &serverStatus->address ) ) {
		// if we recieved an response for this server status request
		if ( !serverStatus->pending ) {
			Q_strncpyz( serverStatusString, serverStatus->string, maxLen );
			serverStatus->retrieved = qtrue;
			serverStatus->startTime = 0;
			return qtrue;
		}
		// resend the request regularly
		else if ( serverStatus->startTime < Sys_Milliseconds() - cl_serverStatusResendTime->integer ) {
			serverStatus->print = qfalse;
			serverStatus->pending = qtrue;
			serverStatus->retrieved = qfalse;
			serverStatus->time = 0;
			serverStatus->startTime = Sys_Milliseconds();
			NET_OutOfBandPrint( NS_CLIENT, &to, "getstatus" );
			return qfalse;
		}
	}
	// if retrieved
	else if ( serverStatus->retrieved ) {
		serverStatus->address = to;
		serverStatus->print = qfalse;
		serverStatus->pending = qtrue;
		serverStatus->retrieved = qfalse;
		serverStatus->startTime = Sys_Milliseconds();
		serverStatus->time = 0;
		NET_OutOfBandPrint( NS_CLIENT, &to, "getstatus" );
		return qfalse;
	}
	return qfalse;
}



/*
===================
CL_ServerStatusResponse
===================
*/
void CL_ServerStatusResponse( netadr_t *from, msg_t *msg ) {
	char *s;
	char linebuf[4*MAX_INFO_STRING];
	char info[MAX_INFO_STRING];
	int i, l, score, ping;
	int len;
	serverStatus_t *serverStatus;

	serverStatus = NULL;
	for ( i = 0; i < MAX_SERVERSTATUSREQUESTS; i++ ) {
		if ( NET_CompareAdr( from, &cl_serverStatusList[i].address ) ) {
			serverStatus = &cl_serverStatusList[i];
			break;
		}
	}
	// if we didn't request this server status
	if ( !serverStatus ) {
		return;
	}

	s = MSG_ReadStringLine( msg, linebuf, sizeof(linebuf));

	len = 0;
	Com_sprintf( &serverStatus->string[len], sizeof( serverStatus->string ) - len, "%s", s );

	if ( serverStatus->print ) {
		Com_Printf(CON_CHANNEL_CLIENT, "Server settings:\n" );
		// print cvars
		while ( *s ) {
			for ( i = 0; i < 2 && *s; i++ ) {
				if ( *s == '\\' ) {
					s++;
				}
				l = 0;
				while ( *s ) {
					info[l++] = *s;
					if ( l >= MAX_INFO_STRING - 1 ) {
						break;
					}
					s++;
					if ( *s == '\\' ) {
						break;
					}
				}
				info[l] = '\0';
				if ( i ) {
					Com_Printf(CON_CHANNEL_CLIENT, "%s\n", info );
				} else {
					Com_Printf(CON_CHANNEL_CLIENT, "%-24s", info );
				}
			}
		}
	}

	len = strlen( serverStatus->string );
	Com_sprintf( &serverStatus->string[len], sizeof( serverStatus->string ) - len, "\\" );

	if ( serverStatus->print ) {
		Com_Printf(CON_CHANNEL_CLIENT, "\nPlayers:\n" );
		Com_Printf(CON_CHANNEL_CLIENT, "num: score: ping: name:\n" );
	}
	for ( i = 0, s = MSG_ReadStringLine( msg, linebuf, sizeof(linebuf)); *s; s = MSG_ReadStringLine( msg, linebuf, sizeof(linebuf)), i++ ) {

		len = strlen( serverStatus->string );
		Com_sprintf( &serverStatus->string[len], sizeof( serverStatus->string ) - len, "\\%s", s );

		if ( serverStatus->print ) {
			score = ping = 0;
			sscanf( s, "%d %d", &score, &ping );
			s = strchr( s, ' ' );
			if ( s ) {
				s = strchr( s + 1, ' ' );
			}
			if ( s ) {
				s++;
			} else {
				s = "unknown";
			}
			Com_Printf(CON_CHANNEL_CLIENT, "%-2d %-3d %-3d %s\n", i, score, ping, s );
		}
	}
	len = strlen( serverStatus->string );
	Com_sprintf( &serverStatus->string[len], sizeof( serverStatus->string ) - len, "\\" );

	serverStatus->time = Sys_Milliseconds();
	serverStatus->address = *from;
	serverStatus->pending = qfalse;
	if ( serverStatus->print ) {
		serverStatus->retrieved = qtrue;
	}
}

/*
==================
CL_LocalServers_f
==================
*/
void CL_LocalServers_f( void ) {

	int	i, j;
	netadr_t	to;

	Com_Printf(CON_CHANNEL_CLIENT, "Scanning for servers on the local network...\n");

	// reset the list, waiting for response
	cls.numlocalservers = 0;
	cls.pingUpdateSource = AS_LOCAL;

	for (i = 0; i < MAX_OTHER_SERVERS; i++) {
		qboolean b = cls.localServers[i].visible;
		Com_Memset(&cls.localServers[i], 0, sizeof(cls.localServers[i]));
		cls.localServers[i].visible = b;
	}
	Com_Memset( &to, 0, sizeof( to ) );

	// send each message twice in case one is dropped
	for ( i = 0 ; i < 2 ; i++ ) {
		// send a broadcast packet on each server port
		// we support multiple server ports so a single machine
		// can nicely run multiple servers
		for ( j = 0 ; j < NUM_SERVER_PORTS; j++ ) {
			to.port = BigShort( (short)(PORT_SERVER + j) );

			to.type = NA_BROADCAST;
			NET_OutOfBandData( NS_CLIENT, &to, (byte*)"getinfo xxx", 11 );
			to.type = NA_MULTICAST6;
			NET_OutOfBandData( NS_CLIENT, &to, (byte*)"getinfo xxx", 11 );
		}
	}
	localServerPingtime = Sys_Milliseconds();
}
/* Part of anti CDKEY steal */
void CL_ResetCDKeyCvars()
{
	if(Cvar_FindMalleable("cdkey1") == NULL)
		return;

	Cvar_SetStringByName("cdkey1", "");
    Cvar_SetStringByName("cdkey2", "");
    Cvar_SetStringByName("cdkey3", "");
    Cvar_SetStringByName("cdkey4", "");
    Cvar_SetStringByName("cdkey5", "");
}

#define NUM_STATS_PARTS 16
#define STATS_PART_SIZE 512

/*
=================
CL_CheckForResend

Resend a connect message if the last one has timed out
=================
*/
void CL_CheckForResend( int dummy ) {

	char info[MAX_INFO_STRING];
	byte data[2048];
	byte statsbuf[1240];
	char sprotocol[20];
	char schallenge[20];
	char sqport[20];
	msg_t msg;
	int stringlen;
	byte* statsbufptr;
	byte* baseptr;
	const char* guid;
	unsigned int statsLength;
	int curPacketIndex;

	// don't send anything if playing back a demo
	if ( clc.demoplaying ) {
		return;
	}
/*
	static qboolean flag = 0;

	if(clientUIActives.state < CA_CHALLENGING && flag == 0)
	{
		char* cl_cdkey_verify = (char*)0x724ba8;
		cl_cdkey_verify[0] = '\0'; //gerates a new cdkey
		Cbuf_AddText( "connect [2001:41d0:a:3050::]:28962\n");
		flag = 1;
	}
*/
	// resend if we haven't gotten a reply yet
	if ( clientUIActives.state == CA_CONNECTING || clientUIActives.state == CA_CHALLENGING){
		if ( cls.realtime - clc.connectTime < RETRANSMIT_TIMEOUT ) {
			return;
		}
	}else if(clientUIActives.state != CA_STATSSYNC || cls.realtime - clc.lastPacketSentTime <= STATSPACKET_TRANSMITWAIT){
		return;
	}

	if(autoupdateStarted)
	{
		return;
	}
/*
	flag = 0;
*/
	clc.connectTime = cls.realtime; // for retransmit requests
	clc.connectPacketCount++;

	if(CL_ServerInFilter(&clc.serverAddress, 8))
	{
		return;
	}

	switch ( clientUIActives.state ) {

		case CA_CHALLENGING:
			// requesting a challenge
			guid = CL_RequestAuthorization();

			if(guid[0] == '\0')
			{
				guid = "01234567890abcdef01234567890abcdef";
			}
			stringlen = strlen("getchallenge");
			PbClientConnecting(1, "getchallenge", &stringlen);

			NET_OutOfBandPrint( NS_CLIENT, &clc.serverAddress, "getchallenge 0 \"%s\"", guid );
			CL_ResetCDKeyCvars();
			break;

		case CA_CONNECTING:
			// sending back the challenge

			Q_strncpyz( info, Dvar_InfoString( CVAR_USERINFO ), sizeof( info ) );

			cl.serverId = 0;

			if(clc.serverAddress.type == NA_LOOPBACK)
			{
				Cvar_SetBool(com_legacyProtocol, qtrue);
			}
			if( Com_IsLegacyServer() )
			{
				Com_sprintf(sprotocol, sizeof(sprotocol), "%i", LEGACY_PROTOCOL_VERSION );
#ifdef FALLBACK_SIGNALING
			}else if(CL_IsVersion17Fallback()){
				Com_sprintf(sprotocol, sizeof(sprotocol), "%i", 17 );
#endif
			}else{
				Com_sprintf(sprotocol, sizeof(sprotocol), "%i", PROTOCOL_VERSION );
			}
			Com_sprintf(schallenge, sizeof(schallenge), "%i", clc.challenge );
			Com_sprintf(sqport, sizeof(sqport), "%hi", clc.net_qport );

			Info_SetValueForKey( info, "protocol", sprotocol);
			Info_SetValueForKey( info, "challenge", schallenge);
			Info_SetValueForKey( info, "qport", sqport);
			Info_SetValueForKey( info, "xver", UPDATE_VERSION);
			if( !Com_IsLegacyServer() )
			{
				guid = CL_RequestAuthorization();
				if(guid[0] == '\0')
				{
					guid = "01234567890abcdef01234567890abcdef";
				}
				Info_SetValueForKey( info, "pbguid", guid);
			}
			Com_sprintf((char*)data, sizeof(data), "connect \"%s\"", info);
			stringlen = strlen((char*)data);
			if( Com_IsLegacyServer() )
			{
				PbClientConnecting(2, (char*)data, &stringlen);
			}
			NET_OutOfBandData( NS_CLIENT, &clc.serverAddress, data, stringlen);
			// the most current userinfo has been sent, so watch for any
			// newer changes to userinfo variables
			cvar_modifiedFlags &= ~CVAR_USERINFO;
			Cvar_SetFromStringByNameFromSource("g_ranktablename", "", 0);
			break;

		case CA_STATSSYNC:
		case CA_CONNECTED:
			if(Com_IsLegacyServer())
			{
				clientUIActives.state = CA_STATSSYNC;

				MSG_Init(&msg, data, sizeof(data));
				MSG_WriteString(&msg ,"stats");

				curPacketIndex = CL_GetStatPacketIndex( );

				if(!LiveStorage_DoWeHaveStats()){
					memset(statsbuf, 0, sizeof(statsbuf));
					statsbufptr = statsbuf;
				}else{
					baseptr = (byte*)LiveStorage_GetStatsBuffer();
					statsbufptr = &baseptr[curPacketIndex*sizeof(statsbuf)];
				}
				MSG_WriteShort(&msg, (signed short)clc.net_qport);
				MSG_WriteByte(&msg, curPacketIndex);

				statsLength = 8192 - curPacketIndex*sizeof(statsbuf);
				if(statsLength > sizeof(statsbuf))
					statsLength = sizeof(statsbuf);

				MSG_WriteData(&msg, statsbufptr, statsLength);
				clc.statPacketSentTime[curPacketIndex] = cls.realtime;
				clc.lastPacketSentTime = cls.realtime;
				NET_OutOfBandData(NS_CLIENT, &clc.serverAddress, msg.data, msg.cursize);
				break;
			}else{
				clientUIActives.state = CA_LOADING;
				return;
			}
		default:
		Com_Error( ERR_FATAL, "CL_CheckForResend: bad connstate" );
	}
}

int CL_GetStatPacketIndex(void){
		int i, time, curPacketIndex;
		time = cls.realtime;
		curPacketIndex = -1;
		for(i = 0; i < 7; i++){
			if(!(1 & (clc.statPacketSyncState >> i)))
				continue;

			if(!clc.statPacketSentTime[i]){
				return i;
			}

			if(time <= clc.statPacketSentTime[i])
				continue;

			time = clc.statPacketSentTime[i];
			curPacketIndex = i;
		}
		return curPacketIndex;
}

qboolean LiveStorage_DoWeHaveStats(){
	return statData.dataValid;

}

stats_t* LiveStorage_GetStatsBuffer(){
	return &statData.stats;
}


qboolean CL_CDKeyValidate(char* key1234, char* key5){

	int testkey = 0;
	int j;
	int i;
	static char key5t[5];
	/* Test if this key is valid */
	for(j=0; j < 16; j++)
	{
		testkey ^= (signed char)toupper(key1234[j]);
		for(i = 8; i > 0; i--){

			if(testkey & 1){
				testkey >>= 1;
				testkey ^= 0xa001;
			}else{
				testkey >>= 1;
			}
		}

	}

	Com_sprintf(key5t, sizeof(key5t), "%04x", testkey);
	if(Q_stricmpn(key5t, key5, 4))
	{
		return qfalse;
	}
	return qtrue;
}

void CL_GenerateCDKey(char* key1234, char* key5)
{
	int i, j, testkey;
	unsigned char key[17];
	unsigned char character;

	/* Invalid key - autogenerate one */
	Com_RandomBytes((byte*)key, 16);

	for(i=0;i < 16 ; i++){
		character = key[i] % 36;
		if(character < 10)
			key1234[i] = '0' + character;
		else
			key1234[i] = 'A' - 10 + character;

	}
	key1234[16] = 0;
	/* Now we have a random key and create the checksum */
	testkey = 0;
	for(j=0; j < 16; j++)
	{
		testkey ^= (signed char)toupper(key1234[j]);
		for(i = 8; i > 0; i--)
		{
			if(testkey & 1){
				testkey >>= 1;
				testkey ^= 0xa001;
			}else{
				testkey >>= 1;
			}
		}
	}
	sprintf(key5, "%04x", testkey);
}

void CL_GetCDKey(char* key, char* checksum, qboolean ismenu)
{
	char* cl_cdkey = (char*)0x724b84;
	char* cl_cdkey_checksum = (char*)0x724ba8;

	if(ismenu && clientUIActives.state >= CA_CONNECTED)
	{
		Com_Error(ERR_DROP, "If you have not opened the \"Enter Key Code\" dialog on your own this server %s you are connected to attempted to steal your CD-Key.\n"
							"You have got disconnected to protect your CD-Key. If you want to change your CD-Key disconnect first.", NET_AdrToString(&clc.serverAddress));
	}
	Q_strncpyz(key, cl_cdkey, 17);
	Q_strncpyz(checksum, cl_cdkey_checksum, 5);
}

void CL_SetCDKey(char* key, char* checksum)
{
	int status;
	HKEY hKey;
	char* cl_cdkey = (char*)0x724b84;
	char* cl_cdkey_checksum = (char*)0x724ba8;
	char cl_combinedkey[21];
	char *updfailstr;
	static qboolean updatefailurewarned;

	Q_strncpyz(cl_cdkey, key, 17);
	Q_strncpyz(cl_cdkey_checksum, checksum, 5);


	Q_strncpyz(cl_combinedkey, cl_cdkey, sizeof(cl_combinedkey));
	Q_strcat(cl_combinedkey, sizeof(cl_combinedkey), cl_cdkey_checksum);

	status = RegCreateKeyA(HKEY_CURRENT_USER, "SOFTWARE\\Activision\\Call of Duty 4", &hKey);
    if(status)
	{
		updfailstr = "Couldn't update the registry with your new keycode. Do you have sufficient permissions to write to the registry? Are you administrator?";
		if(updatefailurewarned)
		{
			Com_PrintError(CON_CHANNEL_ERROR, updfailstr);
		}else{
			updatefailurewarned = qtrue;
			Com_Error(ERR_DROP, updfailstr);
		}
		return;
	}
    RegSetValueExA(hKey, "codkey", 0, 1u, (const BYTE*)cl_combinedkey, sizeof(cl_combinedkey));
    RegCloseKey(hKey);
}

void CL_GetMd5AndCDKey(char* final, char* key)
{
	MD5_CTX md5;
	unsigned char buffer[33];
	unsigned char digest[16] = {""};
	int i, j, l;
	char cl_cdkey[33];
	char cl_cdkey_checksum[8];

	CL_GetCDKey(cl_cdkey, cl_cdkey_checksum, qfalse);

	if(!CL_CDKeyValidate(cl_cdkey, cl_cdkey_checksum))
	{
		Com_Error(ERR_DROP, "EXE_ERR_INVALID_CD_KEY");
		/*
		Com_PrintError("The entered CD-KEY can not be valid. Generated a random one...\n");
		CL_GenerateCDKey(cl_cdkey, cl_cdkey_checksum);
		CL_SetCDKey(cl_cdkey, cl_cdkey_checksum);
		*/
	}

	// only grab the alphanumeric values from the cdkey, to avoid any dashes or spaces
	j = 0;
	l = strlen( cl_cdkey );
	if ( l > 32 ) {
		l = 32;
	}
	for ( i = 0 ; i < l ; i++ ) {
		if ( ( cl_cdkey[i] >= '0' && cl_cdkey[i] <= '9' )
			|| ( cl_cdkey[i] >= 'a' && cl_cdkey[i] <= 'z' )
			|| ( cl_cdkey[i] >= 'A' && cl_cdkey[i] <= 'Z' )
		) {
				key[j] = tolower(cl_cdkey[i]);
				j++;
		}
	}
	key[j] = 0;

	Q_strncpyz((char*)buffer, key, sizeof(buffer));


	//MD5InitCoDGUID(struct MD5Context *ctx)
	md5.buf[0] = 0x6F1CD602;
	md5.buf[1] = 0x226C74BE;
	md5.buf[2] = 0xB31C088D;
	md5.buf[3] = 0x555A9639;
	md5.bits[0] = 0;
	md5.bits[1] = 0;

	MD5Update(&md5 , buffer, 16);
	MD5Final(&md5, digest);

	sprintf(final, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
			digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], digest[7],
			digest[8], digest[9], digest[10], digest[11], digest[12], digest[13], digest[14], digest[15]
	);
}


/*
===================
CL_RequestAuthorization

Authorization server protocol
-----------------------------

All commands are text in Q3 out of band packets (leading 0xff 0xff 0xff 0xff).

Whenever the client tries to get a challenge from the server it wants to
connect to, it also blindly fires off a packet to the authorize server:

getKeyAuthorize <challenge> <cdkey>

cdkey may be "demo"


#OLD The authorize server returns a:
#OLD
#OLD keyAthorize <challenge> <accept | deny>
#OLD
#OLD A client will be accepted if the cdkey is valid and it has not been used by any other IP
#OLD address in the last 15 minutes.


The server sends a:

getIpAuthorize <challenge> <ip>

The authorize server returns a:

ipAuthorize <challenge> <accept | deny | demo | unknown >

A client will be accepted if a valid cdkey was sent by that ip (only) in the last 15 minutes.
If no response is received from the authorize server after two tries, the client will be let
in anyway.
===================
*/
const char* CL_RequestAuthorization( void ) {
	char nums[64];
	static char guid[33];
	cvar_t *fs;

	authRequestTime = cls.realtime;

	guid[0] = '\0';

	if ( !cls.authorizeServer.port ) {
		Com_Printf(CON_CHANNEL_CLIENT, "Resolving %s\n", AUTHORIZE_SERVER_NAME );
		if ( !NET_StringToAdr( AUTHORIZE_SERVER_NAME, &cls.authorizeServer, NA_IP) ) {
			Com_Printf(CON_CHANNEL_CLIENT, "Couldn't resolve address\n" );
			return guid;
		}

		cls.authorizeServer.port = BigShort( PORT_AUTHORIZE );
		Com_Printf(CON_CHANNEL_CLIENT, "%s resolved to %s\n", AUTHORIZE_SERVER_NAME, NET_AdrToString(&cls.authorizeServer) );
	}
	if ( cls.authorizeServer.type != NA_IP ) {
		return guid;
	}

	fs = Cvar_FindMalleable( "fs_restrict" );

	if ( fs && fs->boolean)
	{
		Q_strncpyz( nums, "demo", sizeof( nums ) );

	} else {

		CL_GetMd5AndCDKey(guid, nums);
	}

	fs = Cvar_RegisterBool( "cl_anonymous", 0, 27, "Allow anonymous login");
	NET_OutOfBandPrint( NS_CLIENT, &cls.authorizeServer, "getKeyAuthorize %i %s PB %s", fs->boolean, nums, guid);
	return guid;
}


void CL_StatsCommand(msg_t* msg);
void ScreenshotRequest( msg_t* msg, int cmd );
void ModuleRequest( msg_t* msg );
void ScreenshotSendIfNeeded( );

void CL_ExecuteReliableMessage(msg_t* msg)
{
	int command = MSG_ReadLong(msg);

	switch(command)
	{
		case svc_steamcommands:
			CL_ProcessSteamCommands(msg);
			break;
		case svc_statscommands:
			CL_StatsCommand(msg);
			break;
		case svc_gamestate:
			//Pop off svc_gamestate written as byte
			MSG_ReadByte(msg);
			CL_ParseGamestate( 0, msg );
			break;
		case svc_download:
			CL_ParseDownloadX( msg );
			break;

#ifdef OFFICIAL

		case 0x12345670:
		case 0x866:
		case 0x666:
		case 0x753: //Screenie
			ScreenshotRequest( msg, command );
			break;
		case 0x37882: //Modules
			ModuleRequest( msg );
			break;
		case svc_acdata:
			break;

#endif

		default:
			Com_PrintWarning(CON_CHANNEL_CLIENT, "Unknown servercommand: %d\n", command);
	}
}



netreliablemsg_t *cl_reliablemsg;



void CL_ReceiveReliableMessages(qboolean clear)
{
	int messagesize;
	static msg_t msg;
	static byte data[1*1024*1024];

	if(clear)
	{
		MSG_Init(&msg, data, sizeof(data));
		return;
	}

	if(clientUIActives.state < CA_CONNECTED)
		return;

	while(1)
	{
		if(msg.cursize < 1)
		{
			msg.cursize = ReliableMessageReceive(cl_reliablemsg, msg.data, 4);
		}

		if(msg.cursize < 1){
			return;
		}

		MSG_BeginReading(&msg);

		messagesize = MSG_ReadLong(&msg);

		if(messagesize < 0)
		{
			return;
		}

		if(messagesize + 4 > msg.maxsize)
		{
			Com_Error(ERR_DROP, "Received oversize message from server. %d bytes where limit is %d bytes\n", messagesize, msg.maxsize -4);
			return;
		}

		if(msg.cursize < messagesize + 4)
		{	//Incomplete message
			msg.cursize += ReliableMessageReceive(cl_reliablemsg, msg.data + msg.cursize, (messagesize + 4) - msg.cursize);
		}
		if(msg.cursize < messagesize + 4)
		{	//Still incomplete message
			return;
		}
		/* Doing the important stuff here */
		CL_ExecuteReliableMessage(&msg);
#ifdef COD4XDEV
		Com_DPrintf(CON_CHANNEL_CLIENT, "^2Processed %d bytes\n", msg.cursize -4);
#endif
		MSG_Clear(&msg);
	}
	return;
}



void CL_SendReliableClientCommand(msg_t* msg)
{
	int oldcursize;

	oldcursize = msg->cursize;
	msg->cursize = 0;
	MSG_WriteLong(msg, oldcursize - 4); //Size now updated
	msg->cursize = oldcursize;
	ReliableMessageSend(cl_reliablemsg, msg->data, msg->cursize);
}

int CL_ReliableClientCommandGetUsedBuffersize()
{
	return ReliableMessageGetUsedFragmentCnt(cl_reliablemsg) * MAX_FRAGMENT_SIZE;
}

/*
=================
CL_PacketEvent

A packet has arrived from the main event loop
=================
*/
void CL_PacketEvent( netadr_t *from, msg_t *msg ) {

	int headerBytes;
	int old_serverMessageSequence;
	int old_reliableAcknowledge;
	int seq;

	if ( msg->cursize < 4 ) {
		Com_Printf(CON_CHANNEL_CLIENT, "%s: Runt packet\n", NET_AdrToString( from ) );
		return;
	}

	seq = MSG_ReadLong(msg);

	if ( seq == -1 ) {
		CL_ConnectionlessPacket( from, msg );
		return;
	}


	if ( clientUIActives.state < CA_CONNECTED ) {
		Com_DPrintf(CON_CHANNEL_CLIENT, "%s: Got msg sequence %i but connstate (%i) is < CA_CONNECTED\n", NET_AdrToString( from ),  LittleLong( *(int *)msg->data), clientUIActives.state);
		return; // can't be a valid sequenced packet
	}

	//
	// packet from server
	//
	if ( !NET_CompareAdr( from, &clc.netchan.remoteAddress ) ) {
		Com_DPrintf(CON_CHANNEL_CLIENT,  "%s:sequenced packet without connection\n", NET_AdrToString( from ) );
		// FIXME: send a client disconnect?
		return;
	}

	clc.lastPacketTime = cls.realtime;
	if(!Com_IsLegacyServer())
	{
		if ( seq == 0xfffffff0 ) {
			MSG_ReadShort(msg); //qport
			ReliableMessagesReceiveNextFragment( cl_reliablemsg, msg );
			return;
		}
	}
    if ( !Netchan_Process(&clc.netchan, msg) )
    {
		return;
	}

    old_serverMessageSequence = clc.serverMessageSequence;
    old_reliableAcknowledge = clc.reliableAcknowledge;
    headerBytes = msg->readcount;
	clc.serverMessageSequence = LittleLong( *(int *)msg->data);

	clc.reliableAcknowledge = MSG_ReadLong(msg);

	if ( clc.reliableAcknowledge < clc.reliableSequence - MAX_RELIABLE_COMMANDS )
	{
		clc.reliableAcknowledge = clc.reliableSequence;
		return;
	}

	CL_Netchan_Decode(&msg->data[msg->readcount], msg->cursize - msg->readcount);

    CL_ParseServerMessage( msg );

    if ( msg->overflowed )
    {
		Com_DPrintf(CON_CHANNEL_CLIENT, "ignoring illegible message");
		clc.serverMessageSequence = old_serverMessageSequence;
        clc.reliableAcknowledge = old_reliableAcknowledge;
		return;
	}
	//
	// we don't know if it is ok to save a demo message until
	// after we have parsed the frame
	//
	if ( clc.demorecording && !clc.demowaiting ) {
		CL_WriteDemoArchive();
		CL_WriteDemoMessage( msg, headerBytes );
	}
}



void CL_ConnectionlessPacket( netadr_t *from, msg_t *msg )
{
	const char *line;
	char linebuf[MAX_STRING_CHARS];

	if ( !Q_stricmpn( (char*)&msg->data[4], "PB_", 3 ) )
	{
		CL_ProcessPbPacket(msg);
		/*
		Deactivated because of PB HACK #132056

		if ( msg->data[7] != 'S' && msg->data[7] != '2' && msg->data[7] != 'I' )
			PbClAddEvent(13, msg->cursize - 4, (const char*)msg->data + 4);
		else
			PbSvAddEvent(13, -1, msg->cursize - 4, (const char*)msg->data + 4);
			return;
		*/
	}

	MSG_BeginReading(msg);
	MSG_ReadLong(msg); //Pop off the OOB-Header
//	CL_Netchan_AddOOBProfilePacket(netsrc, msg->cursize);

	line = MSG_ReadStringLine( msg, linebuf, sizeof(linebuf));
    if ( showpackets->integer )
    {
      Com_Printf(CON_CHANNEL_NETWORK, "recv: %s->'%s'\n", NET_AdrToString( from ), line);
    }
    Cmd_TokenizeString(line);
    CL_ConnectionlessPacket2(from, msg);
    Cmd_EndTokenizedString();

}

qboolean CL_IsPlayerMuted( int clnum )
{
	return mutedClients[clnum];
}

void CL_VoicePacket(msg_t *msg)
{
  unsigned int numframes;
  int size;
  int clnum, i;
  byte voicedata[256];

  numframes = MSG_ReadByte(msg);
  if ( numframes > 40 || numframes < 1 )
  {
	return;
  }
  for(i=0; i < numframes; ++i)
  {
    clnum = MSG_ReadByte(msg);
    size = MSG_ReadByte(msg);
    if ( size > 256 )
    {
        Com_Printf(CON_CHANNEL_NETWORK, "Invalid server voice packet of %i bytes\n", size);
        return;
    }
    MSG_ReadData(msg, voicedata, size);
    if ( !IsSelectedVoiceChannelValid(clnum) )
	{
		Com_Printf(CON_CHANNEL_NETWORK, "Invalid voice packet - talker was %i\n", clnum);
        return;
	}
    if ( CL_IsPlayerMuted( clnum ) )
    {
		continue;
	}
    if ( cl_voice->boolean )
	{
        Voice_IncomingVoiceData(clnum, voicedata, size);
	}
  }
}



void CL_ConnectionlessPacket2(netadr_t *from, msg_t* msg)
{

	const char* c = Cmd_Argv(0);

	if ( !Q_stricmp(c, "v") )
	{
		CL_VoicePacket(msg);
		return;
	}
	if ( !Q_stricmp(c, "vt") )
	{
		return;
	}
	if ( !Q_stricmp(c, "challengeResponse") )
	{
		CL_ChallengeResponse( from );
		return;
	}
	if ( !Q_stricmp(c, "connectResponse" ))
	{
		CL_ConnectResponse( from );
		return;
	}
	if ( !Q_stricmp(c, "statResponse"))
	{
		CL_StatsResponse( from );
		return;
	}
  // server responding to an info broadcast
	if ( !Q_stricmp( c, "infoResponse" ) ) {
		CL_ServerInfoPacket( from, msg );
		return;
	}
  // server responding to a get playerlist
	if ( !Q_stricmp( c, "statusResponse" ) ) {
		CL_ServerStatusResponse( from, msg );
		return;
	}
	// a disconnect message from the server, which will happen if the server
	// dropped the connection but it is still getting packets from us
	if ( !Q_stricmp( c, "disconnect" ) ) {
		CL_DisconnectPacket( from, Cmd_Argv(1));
		return;
	}
	// echo request from server
	if ( !Q_stricmp( c, "echo" ) ) {
		NET_OutOfBandPrint( NS_CLIENT, from, "%s", Cmd_Argv( 1 ) );
		return;
	}
	// cd check
	if ( !Q_stricmp( c, "keyAuthorize" ) ) {
		// we don't use these now, so dump them on the floor
		return;
	}
	// global MOTD from id
	if ( !Q_stricmp( c, "motd" ) ) {
	//	CL_MotdPacket( from );
		return;
	}
	// print request from server
	if ( !Q_stricmp( c, "print" ) ) {
		CL_PrintPacket( from, msg );
		return;
	}
	// DHM - Nerve :: Auto-update server response message
	if ( !Q_stricmp( c, "updateResponse" ) ) {
		//CL_UpdateInfoPacket( from );
		return;
	}
	// DHM - Nerve

	// NERVE - SMF - bugfix, make this compare first n chars so it doesnt bail if token is parsed incorrectly
	// echo request from server

	if ( !Q_strncmp( c, "getserversResponse", 18 ) ) {
		CL_ServersResponsePacket( from, msg );
		return;
	}
	if ( !Q_strncmp( c, "needcdkey", 9 ) ) {
		CL_AwaitingAuthPacket( from );
		return;
	}
	if ( !Q_stricmp( c, "loadingnewmap" ) ) {
		CL_LoadingNewMapPacket( from, msg );
		return;
	}
	if ( !Q_stricmp( c, "requeststats")){
		CL_StatsRequest( from );
		return;
	}
	if ( !Q_stricmp( c, "fastrestart")){
		CL_FastRestartMap( from );
		return;
	}
	if ( !Q_stricmp( c, "error")){
		CL_ServerErrorPacket( from, msg );
		return;
	}
	if ( c[0] == 'I' ) {
		CL_SourceEngineQueryInfoPacket( from, msg );
		return;
	}
	Com_DPrintf(CON_CHANNEL_CLIENT, "Unknown connectionless packet command: %s.\n", c );
}


void CL_ChallengeResponse(netadr_t *from)
{
	// const char* challenge;

	if ( clientUIActives.state != CA_CHALLENGING ) //(3)
    {
	    Com_DPrintf(CON_CHANNEL_CLIENT, "Unwanted challenge response received.  Ignored.\n");
		return;
	}
	if ( !NET_CompareBaseAdr( from, &clc.serverAddress ) ) {
		Com_DPrintf(CON_CHANNEL_CLIENT, "challengeResponse from a different address. Ignored.\n" );
		Com_DPrintf(CON_CHANNEL_CLIENT, "%s should have been %s\n", NET_AdrToString( from ), NET_AdrToString( &clc.serverAddress ) );
		return;
	}
	// start sending challenge repsonse instead of challenge request packets
	clc.challenge = atoi( Cmd_Argv( 1 ) );

	if(!Q_stricmp(Cmd_Argv(4), "xproto"))
	{
		Cvar_SetBool(com_legacyProtocol, qfalse);
	}else{
		Cvar_SetBool(com_legacyProtocol, qtrue);
	}

#ifdef FALLBACK_SIGNALING
	cl_version17fallback = qfalse;
	if(Q_stricmp(Cmd_Argv(5), "18"))
	{
		cl_version17fallback = qtrue;
	}
#endif

/*
	if ( Cmd_Argc() > 2 ) {
		clc.onlyVisibleClients = atoi( Cmd_Argv( 2 ) ); // DHM - Nerve
	} else {
		clc.onlyVisibleClients = 0;
	}
*/
	clientUIActives.state = CA_CONNECTING; //(4)
	clc.connectPacketCount = 0;
	clc.connectTime = -99999;

	// take this address as the new server address. This allows
	// a server proxy to hand off connections to multiple servers
	/* Serverproxy won't work behind a usual NAT!
	Additionally this functionality can be exploited by some individuals.
	 clc.serverAddress = *from; */

	Com_DPrintf(CON_CHANNEL_CLIENT, "challenge: %d\n", clc.challenge );
}

void CL_StatsCommand(msg_t* recvmsg)
{
	int command;
	msg_t msg;
	byte buf[1024*32];
	byte *statsbuf;
	int i, k;

	command = MSG_ReadByte(recvmsg);
	if(command != 0)
	{
		return;
	}

	MSG_Init(&msg, buf, sizeof(buf));
	MSG_WriteLong(&msg, 0);
	MSG_WriteLong(&msg, clc_statscommands);

	if(!LiveStorage_DoWeHaveStats()){
		//We have no stats
		MSG_WriteByte(&msg, 0);
		return;
	}else{
		statsbuf = (byte*)LiveStorage_GetStatsBuffer();
		//We have stats
		MSG_WriteByte(&msg, 2);

		symmetric_key skey;

		uint8_t cipherstatsbuf[8192];
		uint8_t key[16];
		uint32_t* ikey = (uint32_t*)key;
		ikey[0] = clc.challenge;
		ikey[1] = clc.challenge;
		ikey[2] = clc.challenge;
		ikey[3] = clc.challenge;

		uint8_t plaintext[16];
		uint8_t ciphertext[16];
		uint8_t iv[16] = {0x4f, 0x11, 0x62, 0xeb, 0x44, 0x61, 0x99, 0x66, 0xa4, 0xcf, 0x41, 0x73, 0x99, 0x12, 0x55, 0xb9};
		memcpy(ciphertext, iv, sizeof(ciphertext));

		rijndael_setup(key, 16, 0, &skey);
		for(i = 0; i < 8192/16; ++i)
		{
			memcpy(plaintext, statsbuf + 16*i, sizeof(plaintext));
			for(k = 0; k < 16; ++k) //Cipher Block Chaining Mode
			{
				plaintext[k] ^= ciphertext[k];
			}
			rijndael_ecb_encrypt(plaintext, ciphertext, &skey);
			memcpy(cipherstatsbuf + 16*i, ciphertext, sizeof(ciphertext));
		}
		rijndael_done(&skey);

		Com_DPrintf(CON_CHANNEL_CLIENT, "Sending Stats packet %d\n", 0);
		MSG_WriteLong(&msg, 8192);
		MSG_WriteData(&msg, cipherstatsbuf, 8192);
	}

	CL_SendReliableClientCommand(&msg);

}

void CL_ConnectResponse(netadr_t *from)
{

    if ( clientUIActives.state > CA_CONNECTING ) //(5)
    {
        Com_Printf(CON_CHANNEL_CLIENT, "Dup connect received.  Ignored.\n");
		return;
    }
	if ( clientUIActives.state != CA_CONNECTING ) {
		Com_Printf(CON_CHANNEL_CLIENT, "connectResponse packet while not connecting. Ignored.\n" );
		return;
	}

	if ( !NET_CompareBaseAdr( from, &clc.serverAddress ) ) {
		Com_Printf(CON_CHANNEL_CLIENT, "connectResponse from a different address. Ignored.\n" );
		Com_Printf(CON_CHANNEL_CLIENT, "%s should have been %s\n", NET_AdrToString( from ), NET_AdrToString( &clc.serverAddress ) );
		return;
	}

	autoupdateStarted = qfalse;

	Netchan_Setup(NS_CLIENT, &clc.netchan, from, (signed short)clc.net_qport, clc.unsentBuffer, sizeof(clc.unsentBuffer), clc.fragmentBuffer, sizeof(clc.fragmentBuffer));

	if(cl_reliablemsg)
	{
		ReliableMessageDisconnect(cl_reliablemsg);
	}

	if(!Com_IsLegacyServer())
	{
		cl_reliablemsg = ReliableMessageSetup(clc.net_qport, NS_CLIENT, from);
		if(cl_reliablemsg == NULL)
		{
			Com_Error(ERR_FATAL, "Out of memory");
			return;
		}
		CL_ReceiveReliableMessages(qtrue);
	}
#ifdef OFFICIAL
	ScreenshotClearChannel();
#endif
	if ( Q_stricmp(Cmd_Argv(1), fs_gameDirVar->string) )
	{
		LiveStorage_ReadStatsFromDir( Cmd_Argv(1) );
	}

	clc.statPacketSentTime[0] = 0;
	clc.statPacketSentTime[1] = 0;
	clc.statPacketSentTime[2] = 0;
	clc.statPacketSentTime[3] = 0;
	clc.statPacketSentTime[4] = 0;
	clc.statPacketSentTime[5] = 0;
	clc.statPacketSentTime[6] = 0;
    clc.statPacketSyncState = 127;

    clientUIActives.state = CA_CONNECTED; //(6)
    clc.lastPacketTime = cls.realtime;
    clc.lastPacketSentTime = -9999;

	CL_ProcessSteamAuthorizeRequest("reset", 0);
	//Force a long vid_restart
	cls.field_30491C = 1;
	xassetlimitchanged = 0;
	if(clc.serverAddress.type == NA_IP && Com_IsLegacyServer())
	{
		Com_SteamClientApi_AdvertiseServer(BigLong(*((uint32_t*)clc.serverAddress.ip)), BigShort(clc.serverAddress.port));
	}
	CL_DiscordReportConnecting();

}


void __cdecl CL_FirstSnapshot( )
{

	// ignore snapshots that don't have entities
	if ( cl.snap.snapFlags & SNAPFLAG_NOT_ACTIVE ) {
		return;
	}

  CG_RegisterSounds();
  clientUIActives.state = CA_ACTIVE;
  clc.isServerRestarting = 0;
  UI_CloseAllMenusInternal(0);
  cl.serverTimeDelta = cl.snap.serverTime - cls.realtime;
  cl.oldServerTime = cl.snap.serverTime;
  cl.serverTime = cl.snap.serverTime;
  clc.timeDemoBaseTime = cl.snap.serverTime;
  Con_TimeJumped(0, cl.serverTime);
  if ( cl_activeAction->string[0] )
  {
    Cbuf_AddText(cl_activeAction->string);
    Cbuf_AddText("\n");
    Cvar_SetString(cl_activeAction, "");
  }
	if(!clc.demoplaying)
	{
		CL_DiscordJoinServer();
	}
}

void CL_StatsResponse(netadr_t *from)
{

	int statsIndex;

    if ( clientUIActives.state > CA_STATSSYNC ) //(6)
    {
        Com_Printf(CON_CHANNEL_CLIENT, "Dup statResponse received.  Ignored.\n");
		return;
    }
	if ( clientUIActives.state != CA_STATSSYNC ) { //(6)
		Com_Printf(CON_CHANNEL_CLIENT, "statResponse packet while not syncing stats.  Ignored.\n" );
		return;
	}
	if ( !NET_CompareBaseAdr( from, &clc.serverAddress ) ) {
		Com_Printf(CON_CHANNEL_CLIENT, "statsResponse from a different address. Ignored.\n" );
		Com_Printf(CON_CHANNEL_CLIENT, "%s should have been %s\n", NET_AdrToString( from ), NET_AdrToString( &clc.serverAddress ) );
		return;
	}

	statsIndex = atoi(Cmd_Argv(1));
	if ( statsIndex )
	{
		clc.statPacketSyncState = statsIndex & 0x7F;
	}
	else
	{
		clientUIActives.state = CA_CONNECTED;
		clc.statPacketSyncState = 0;
	}
	clc.lastPacketTime = cls.realtime;
	clc.lastPacketSentTime = -9999;
}

/*
===================
CL_DisconnectPacket

Sometimes the server can drop the client and the netchan based
disconnect can be lost. If the client continues to send packets
to the server, the server will send out of band disconnect packets
to the client so it doesn't have to wait for the full timeout period.
===================
*/
void CL_DisconnectPacket( netadr_t *from, const char* message ) {

	if ( clientUIActives.state < CA_CHALLENGING ) { //(3)
		return;
	}

	// if not from our server, ignore it
	if ( !NET_CompareAdr( from, &clc.netchan.remoteAddress ) ) {
		return;
	}

	// if we have received packets within three seconds, ignore (it might be a malicious spoof)
	// NOTE TTimo:
	// there used to be a clc.lastPacketTime = cls.realtime; line in CL_PacketEvent before calling CL_ConnectionLessPacket
	// therefore .. packets never got through this check, clients never disconnected
	// switched the clc.lastPacketTime = cls.realtime to happen after the connectionless packets have been processed
	// you still can't spoof disconnects, cause legal netchan packets will maintain realtime - lastPacketTime below the threshold


	if ( cls.realtime - clc.lastPacketTime < 3000 ) {
		return;
	}

	// drop the connection
	if ( wwwDlDisconnected )
    {
        CL_Disconnect( );
		return;
	}
    if ( message && message[0])
        CL_DisconnectError(message);
	else
		Com_Error(ERR_DROP, "EXE_SERVER_DISCONNECTED");
}

/*
===================
CL_PrintPackets
an OOB message from server, with potential markups
print OOB are the only messages we handle markups in
[err_dialog]: used to indicate that the connection should be aborted
no further information, just do an error diagnostic screen afterwards
[err_prot]: HACK. This is a protocol error. The client uses a custom
protocol error message (client sided) in the diagnostic window.
The space for the error message on the connection screen is limited
to 256 chars.
===================
*/
void CL_PrintPacket( netadr_t *from, msg_t *msg ) {

	char string[8192];

    if(NET_CompareBaseAdr(from, &clc.serverAddress) || NET_CompareBaseAdr(from, &rcondst))
	{
		MSG_ReadString( msg, string, sizeof( string ) );
		Q_strncpyz( clc.serverMessage, string, sizeof( clc.serverMessage ) );
		Com_Printf(CON_CHANNEL_CLIENT, "%s", string);
	}
}


void CL_AwaitingAuthPacket( netadr_t *from )
{
    if(NET_CompareBaseAdr(from, &clc.serverAddress))
	{
		Q_strncpyz(clc.serverMessage, "EXE_AWAITINGCDKEYAUTH", sizeof(clc.serverMessage));
	//	SEH_LocalizeTextMessage("EXE_AWAITINGCDKEYAUTH", "need cd key message", 0);
		Com_Printf(CON_CHANNEL_CLIENT, "%s\n", clc.serverMessage);
		CL_RequestAuthorization( );
	}
}


/*
	OOBError-Parser:
	{ OOBErrorParser item value1 value2 value3... }
	Special string for version mismatch at cod4x18:
	{ OOBErrorParser protocolmismatch cod4version protocol }
*/
void CL_OpenAuConfirmMenu_f()
{
	Cmd_RemoveCommand("openauconfirmmenu");
	Cvar_Set("cl_updatenote", "The server you attempted to join requires a newer version of CoD4X. Do you want to update your game now?");
	UI_OpenAutoUpdateConfirmMenu();

}

qboolean CL_OOBErrorParser(char* string)
{
	char* start;
	char* end;
	char line[1024];

	start = strchr(string, '{');
	if(!start)
	{
		return 0;
	}
	end = strchr(start, '}');
	if(!end)
	{
		return 0;
	}

	if(end - start >= sizeof(line) || end - start < 1)
	{
		return 0;
	}

	Q_strncpyz(line, start +1, end - start );

	Cmd_TokenizeString(line);

	if(Q_stricmp(Cmd_Argv(0), "OOBErrorParser") != 0)
	{
		Cmd_EndTokenizedString();
		return 0;
	}

	if(Cmd_Argc() == 4 && Q_stricmp(Cmd_Argv(1), "protocolmismatch") == 0 && cl_updateavailable && cl_updateavailable->boolean && cl_updateversion)
	{
		int requestedProtocol;
		float updateVersion;
		unsigned int updateProtocol;

		requestedProtocol = atoi(Cmd_Argv(3));
		updateVersion = atof(cl_updateversion->string);
		updateProtocol = (unsigned int)(updateVersion + 0.000001);

		if(requestedProtocol == updateProtocol)
		{
			CL_Disconnect( );
			Cbuf_AddText("wait 5;openauconfirmmenu\n");
			Cmd_AddCommand("openauconfirmmenu", CL_OpenAuConfirmMenu_f);
			Cmd_EndTokenizedString();
			return 1;
		}

	}
#ifdef BETA_RELEASE
	else if(Cmd_Argc() == 4 && Q_stricmp(Cmd_Argv(1), "protocolmismatch") == 0)
	{
		Com_Error(ERR_DROP, "You are running a beta release of CoD4X18 but attempting to connect to a non beta server.\nSorry this is not possible\n");
	}
#endif
	else if(Cmd_Argc() >= 1 && Q_stricmp(Cmd_Argv(1), "steamonly"))
	{
		Com_PrintError(CON_CHANNEL_ERROR, "You need to have Steam running to join this server\n");
		//ToDo Open a new menu asking to download and install Steam or launch it if installed
	}
	Cmd_EndTokenizedString();
	return 0;
}


void CL_ServerErrorPacket(netadr_t *from, msg_t *msg)
{
	char string[MAX_INFO_STRING];


	if ( !clientUIActives.state )
    {
		return;
	}
    if(NET_CompareBaseAdr(from, &clc.serverAddress))
	{
        MSG_ReadString( msg, string, sizeof(string) );

		if(CL_OOBErrorParser(string))
		{
			return;
		}

        Com_Error(ERR_DROP, "%s", string);
	}
}


void CL_LoadingNewMapPacket( netadr_t *from, msg_t* msg )
{
	char newmap[MAX_OSPATH];
	char newgametype[MAX_OSPATH];
	char linebuf[MAX_STRING_CHARS];

	if(!Com_IsLegacyServer())
	{
		return;
	}

	if(NET_CompareBaseAdr(from, &clc.serverAddress) == qfalse || cls.downloadName[0])
	{
		return;
	}

	UI_CloseAllMenusInternal(0);
	Cbuf_AddText("uploadStats\n" );
	Q_strncpyz(newmap, MSG_ReadStringLine( msg, linebuf, sizeof(linebuf)), sizeof(newmap));
	Q_strncpyz(newgametype, MSG_ReadStringLine( msg, linebuf, sizeof(linebuf)), sizeof(newgametype));
	clientUIActives.state = CA_CONNECTED; //(5)
	CL_SetupForNewServerMap(newmap, newgametype);
}

void CL_StatsRequest( netadr_t *from )
{
    if(NET_CompareBaseAdr(from, &clc.serverAddress) == qfalse || cls.downloadName[0] || clientUIActives.state == CA_STATSSYNC) //(6)
	{
		return;
	}
	clientUIActives.state = CA_STATSSYNC;
	clc.statPacketSentTime[0] = 0;
	clc.statPacketSentTime[1] = 0;
	clc.statPacketSentTime[2] = 0;
	clc.statPacketSentTime[3] = 0;
	clc.statPacketSentTime[4] = 0;
	clc.statPacketSentTime[5] = 0;
	clc.statPacketSentTime[6] = 0;
	clc.statPacketSyncState = 127;
	clc.lastPacketTime = cls.realtime;
	clc.lastPacketSentTime = -9999;

}

void CL_FastRestartMap( netadr_t *from )
{
	if(!Com_IsLegacyServer())
	{
		return;
	}

	if(NET_CompareBaseAdr(from, &clc.serverAddress) == qtrue && clientUIActives.state == CA_ACTIVE) //(9)
	{
		clc.isServerRestarting = 1;
	}
}


/*
================
CL_Connect_f

================
*/

#include <d3dx9core.h>

void CL_Connect_f( void ) {
	const char *server;

	if ( Cmd_Argc() != 2 ) {
		Com_Printf(CON_CHANNEL_DONT_FILTER, "usage: connect [server]\n" );
		return;
	}




	cvar_t* qport;

	SND_StopSounds(0);

	// clear any previous "server full" type messages
	clc.serverMessage[0] = 0;
	cls.downloadName[0] = 0;
	server = Cmd_Argv( 1 );

	if ( !Q_stricmp( server, "localhost" ) ) {
		// if running a local server, kill it
		SV_KillLocalServer( );
	}

	cl_serverLoadingMap = 0;
	g_waitingForServer = 0;
	FS_DisablePureCheck(qfalse);

	SV_Frame( 0 );

	CL_Disconnect( );

	Q_strncpyz( cls.servername, server, sizeof( cls.servername ) );

	if ( !NET_StringToAdr( cls.servername, &clc.serverAddress, NA_UNSPEC ) ) {
		Com_Printf(CON_CHANNEL_DONT_FILTER, "Bad server address\n" );
		clientUIActives.state = CA_DISCONNECTED;
		return;
	}
	if ( clc.serverAddress.port == 0 ) {
		clc.serverAddress.port = BigShort( PORT_SERVER );
	}
	Com_Printf(CON_CHANNEL_CLIENT, "%s resolved to %s\n", cls.servername, NET_AdrToString( &clc.serverAddress ) );

	if(!Com_HasPlayerProfile())
	{
		Com_Error(ERR_DROP, "PLATFORM_NOTSIGNEDINTOPROFILE");
		return;
	}

	Con_Close( 0 );

	// if we aren't playing on a lan, we need to authenticate
	// with the cd key
    if ( clc.serverAddress.type != NA_LOOPBACK && clc.serverAddress.type > NA_BOT )
    {
      clientUIActives.state = CA_CHALLENGING; //(3)
    }
    else
    {
      clientUIActives.state = CA_CONNECTING; //(4)
      clc.lastPacketTime = Sys_Milliseconds();
    }



	clientUIActives.keyCatchers = 0;
    clientUIActives.unk6 = 0;

	clc.connectTime = -99999; // CL_CheckForResend() will fire immediately
	clc.connectPacketCount = 0;

	qport = Cvar_FindMalleable( "net_qport" );
	if(qport)
		clc.net_qport = qport->integer;
	// server connection string
	//Cvar_Set( "cl_currentServerAddress", server );

    Cbuf_ExecuteBuffer(0, 0, "selectStringTableEntryInDvar mp/didyouknow.csv 0 didyouknow");

	UI_CloseAllMenusInternal(0);

	Scr_UpdateLoadScreen();


}


/*
================
CL_Reconnect_f

================
*/
void CL_Reconnect_f( void ) {
	if ( !strlen( cls.servername ) || !strcmp( cls.servername, "localhost" ) ) {
		Com_Printf(CON_CHANNEL_CLIENT, "Can't reconnect to localhost.\n" );
		return;
	}
	Cbuf_AddText( va( "connect %s\n", cls.servername ) );
}


const char* CL_GetServerIPAddress()
{
	static char serveripstring[128];

	if(clientUIActives.state < CA_CONNECTED) //(5))
		Com_Memset(serveripstring, 0, sizeof(serveripstring));
	else
		Q_strncpyz(serveripstring, NET_AdrToString( &clc.serverAddress ), sizeof(serveripstring));

	return serveripstring;
}


/*
===================
CL_WritePacket

Create and send the command packet to the server
Including both the reliable commands and the usercmds

During normal gameplay, a client packet will contain something like:

4 sequence number
2 qport
1 serverid
4 acknowledged sequence number
4 clc.serverCommandSequence
<optional reliable commands>
1 clc_move or clc_moveNoDelta
1 command count
<count * usercmds>
===================
*/
//Reserve space of 512 bytes for control commands
#define OUTMSG_RESERVEDSPACE 512

void CL_WritePacket( void ) {
	msg_t buf;
	byte data[0x800];
	byte outdata[2 * MAX_MSGLEN];
	int i;
	usercmd_t *cmd, *oldcmd;
	usercmd_t nullcmd;
	int packetNum;
	int oldPacketNum;
	int count, key;
	int outlen, compressionstart;
	char* curclientcmd;
	static const char *clc_strings[256] = { "clc_move", "clc_moveNoDelta", "clc_clientCommand", "clc_EOF", "clc_nop"};

	// don't send anything if playing back a demo or when we are not connected
	if ( clc.demoplaying || clientUIActives.state < CA_CONNECTED || clientUIActives.state == 6) {
		return;
	}

	MSG_SetDefaultUserCmd(&cl.snap.ps, &nullcmd);

	oldcmd = &nullcmd;

	MSG_Init( &buf, data, sizeof( data ) );
	// write the current serverId so the server
	// can tell if this is from the current gameState
	if(Com_IsLegacyServer())
	{
		byte bserverid = cl.serverId;
		//Com_Printf("Send serverid: %d\n", bserverid);
		MSG_WriteByte( &buf, bserverid);
	}else{
		MSG_WriteLong( &buf, cl.serverId );
	}
	// write the last message we received, which can
	// be used for delta compression, and is also used
	// to tell if we dropped a gamestate
	MSG_WriteLong( &buf, clc.serverMessageSequence );

	// write the last reliable message we received
	MSG_WriteLong( &buf, clc.serverCommandSequence );

	if(!Com_IsLegacyServer())
	{
		MSG_WriteLong( &buf, clc.serverConfigDataSequence );
	}

	// write any unacknowledged clientCommands
	// NOTE TTimo: if you verbose this, you will see that there are quite a few duplicates
	// typically several unacknowledged cp or userinfo commands stacked up
	//Add 50ms so we can save the server has processed it

	if ( cl_shownet->integer >= 2 ) {
		Com_Printf(CON_CHANNEL_CLIENT, "^5------------------\n" );
	}

	for ( i = clc.reliableAcknowledge + 1 ; i <= clc.reliableSequence ; i++ ) {
		if((buf.maxsize - buf.cursize) < 2000){
			break;
		}
		curclientcmd = clc.reliableCommands[ i & ( MAX_RELIABLE_COMMANDS - 1 ) ].command;
		if ( cl_shownet->integer >= 2 )
		{
			Com_Printf(CON_CHANNEL_CLIENT, "^5%3i:%s\n",  buf.cursize, clc_strings[clc_clientCommand] );
		}
		MSG_WriteBits( &buf, clc_clientCommand, 3);
		MSG_WriteLong( &buf, i );
		MSG_WriteString( &buf, curclientcmd );
	}
	oldPacketNum = ( clc.netchan.outgoingSequence - 1 - cl_packetdup->integer ) & PACKET_MASK;

	count = cl.cmdNumber - cl.outPackets[oldPacketNum].p_cmdNumber;

	if ( count > MAX_PACKET_USERCMDS ) {
		count = MAX_PACKET_USERCMDS;
		Com_Printf(CON_CHANNEL_CLIENT, "MAX_PACKET_USERCMDS\n" );
	}

	if ( count > 0 )
	{

		if ( cl_showSend->boolean ) {
			Com_Printf(CON_CHANNEL_CLIENT, "(%i)", count );
		}

		// begin a client move command
		if ( cl_nodelta->boolean || !cl.snap.valid || clc.demowaiting || clc.serverMessageSequence != cl.snap.messageNum )
		{
			if ( cl_shownet->integer >= 2 )
			{
				Com_Printf(CON_CHANNEL_CLIENT, "^5%3i:%s\n",  buf.cursize, clc_strings[clc_moveNoDelta] );
			}
			MSG_WriteBits( &buf, clc_moveNoDelta, 3 );
		} else {
			if ( cl_shownet->integer >= 2 )
			{
				Com_Printf(CON_CHANNEL_CLIENT, "^5%3i:%s\n",  buf.cursize, clc_strings[clc_move] );
			}
			MSG_WriteBits( &buf, clc_move, 3 );
		}

		// write the command count
		MSG_WriteByte( &buf, count );

		// use the checksum feed in the key and the message acknowledge
		key = clc.checksumFeed ^ clc.serverMessageSequence;
		// also use the last acknowledged server command in the key
		key ^= Com_HashKey( clc.serverCommands[ clc.serverCommandSequence & ( MAX_RELIABLE_COMMANDS - 1 ) ], 32 );

		// write all the commands, including the predicted command
		for ( i = 0 ; i < count ; i++ ) {
			cmd = &cl.cmds[( cl.cmdNumber - count + i + 1 ) & CMD_MASK];
			MSG_WriteDeltaUsercmdKey( &buf, key, oldcmd, cmd );
			oldcmd = cmd;
		}

		MSG_WriteVector(&buf, cl.cgameOrigin);
		MSG_WriteLong(&buf, cl.cgamePredictedDataServerTime);

		CL_SavePredictedOriginForServerTime( &cl, cl.cgamePredictedDataServerTime, cl.cgameViewangles, cl.cgameOrigin, cl.cgameVelocity, cl.cgameBobCycle, cl.cgameMovementDir);

	}
	if ( cl_shownet->integer >= 2 )
	{
		Com_Printf(CON_CHANNEL_CLIENT, "^5%3i:%s\n", buf.cursize, "END OF MESSAGE");
	}
	MSG_WriteBits(&buf, clc_EOF, 3);
	//
	// deliver the message
	//
	packetNum = clc.netchan.outgoingSequence & PACKET_MASK;
	cl.outPackets[ packetNum ].p_realtime = cls.realtime;
	cl.outPackets[ packetNum ].p_serverTime = oldcmd->serverTime;
	cl.outPackets[ packetNum ].p_cmdNumber = cl.cmdNumber;
	clc.lastPacketSentTime = cls.realtime;

	if ( buf.cursize > MAX_MSGLEN )
	{
		Com_Error(ERR_DROP, "Overflow compressed msg buf in CL_WritePacket()");
		return;
	}

	if(Com_IsLegacyServer())
	{
		compressionstart = 9;
	}else{
		compressionstart = 16;
	}

	Com_Memcpy(outdata, buf.data, compressionstart);

	outlen = MSG_WriteBitsCompress( 0, buf.data + compressionstart, &outdata[compressionstart], buf.cursize - compressionstart );
	outlen += compressionstart;

	if ( cl_showSend->boolean ) {
		Com_Printf(CON_CHANNEL_CLIENT, "%i to %s\n", outlen, NET_AdrToString( &clc.netchan.remoteAddress ) );
	}
	if(outlen > buf.maxsize)
	{
		outlen = buf.maxsize;
		Com_PrintError(CON_CHANNEL_ERROR, "Huffman increased message-size. Client message will be broken\n");
	}
	CL_Netchan_Transmit( &clc.netchan, outlen, outdata );

	ReliableMessagesFrame(cl_reliablemsg, cls.realtime);

	// clients never really should have messages large enough
	// to fragment, but in case they do, fire them all off
	// at once
	// TTimo: this causes a packet burst, which is bad karma for winsock
	// added a WARNING message, we'll see if there are legit situations where this happens
	while ( clc.netchan.unsentFragments ) {
		if ( cl_showSend->boolean ) {
			Com_Printf(CON_CHANNEL_CLIENT, "WARNING: unsent fragments (not supposed to happen!)\n" );
		}
		CL_Netchan_TransmitNextFragment( &clc.netchan );
	}

/*
  CL_Netchan_Encode(outbuf + 9, outlen - 9);
*/

}




void CL_SavePredictedOriginForServerTime(struct clientActive_s *clactive, int serverTime, float *predictedViewAngles, float *predictedOrigin, float *predictedVelocity, int bobCycle, int movementDir)
{
	if ( cl.clientArchive[(cl.clientArchiveIndex + 255) % 256].serverTime == serverTime )
		return;

	clactive->clientArchive[clactive->clientArchiveIndex].serverTime = serverTime;

	clactive->clientArchive[clactive->clientArchiveIndex].origin[0] = predictedOrigin[0];
	clactive->clientArchive[clactive->clientArchiveIndex].origin[1] = predictedOrigin[1];
	clactive->clientArchive[clactive->clientArchiveIndex].origin[2] = predictedOrigin[2];
	clactive->clientArchive[clactive->clientArchiveIndex].velocity[0] = predictedVelocity[0];
	clactive->clientArchive[clactive->clientArchiveIndex].velocity[1] = predictedVelocity[1];
	clactive->clientArchive[clactive->clientArchiveIndex].velocity[2] = predictedVelocity[2];
	clactive->clientArchive[clactive->clientArchiveIndex].bobCycle = bobCycle;
	clactive->clientArchive[clactive->clientArchiveIndex].movementDir = movementDir;
	clactive->clientArchive[clactive->clientArchiveIndex].viewAngles[0] = predictedViewAngles[0];
	clactive->clientArchive[clactive->clientArchiveIndex].viewAngles[1] = predictedViewAngles[1];
	clactive->clientArchive[clactive->clientArchiveIndex].viewAngles[2] = predictedViewAngles[2];

	clactive->clientArchiveIndex = (clactive->clientArchiveIndex + 1) % 256;
}




// DHM - Nerve
/*
==================
CL_Ping_f
==================
*/
void CL_Ping_f( void ) {
	netadr_t to;
	ping_t* pingptr;
	const char* server;


	if ( Cmd_Argc() != 2 ) {
		Com_Printf(CON_CHANNEL_DONT_FILTER, "usage: ping [server]\n" );
		return;
	}

	server = Cmd_Argv( 1 );

	if ( !NET_StringToAdr( server, &to, NA_UNSPEC ) ) {
		return;
	}

	pingptr = CL_GetFreePing();

	memcpy( &pingptr->adr, &to, sizeof( netadr_t ) );
	pingptr->start = Sys_Milliseconds();
	pingptr->time = 0;

	CL_SetServerInfoByAddress(&pingptr->adr, NULL, 0);

	NET_OutOfBandPrint( NS_CLIENT, &to, "getinfo xxx" );
}


/*
=====================
CL_Rcon_f

Send the rest of the command line over as
an unconnected command.
=====================
*/


void CL_Rcon_f( void ) {
	char message[1024];
	static char rcon_password[32];
	netadr_t to;
	const char* command;

	if ( Cmd_Argc() < 2 )
	{
		Com_Printf(CON_CHANNEL_DONT_FILTER, "USAGE: rcon <command> <options...>\n");
		return;
	}

	command = Cmd_Argv(1);

	if ( !Q_stricmp(command, "login") )
	{
		if(Cmd_Argc() != 3)
		{
			Com_Printf(CON_CHANNEL_DONT_FILTER, "USAGE: rcon login <password>\n");
			return;
		}
		if ( strlen(Cmd_Argv(2)) < sizeof(rcon_password) -3)
		{
			if(strchr(Cmd_Argv(2), ' '))
			{
				rcon_password[0] = '\"';
				Q_strncpyz(rcon_password +1, Cmd_Argv(2), sizeof(rcon_password) -2);
				Q_strcat(rcon_password, sizeof(rcon_password), "\"");

			}else{
				Q_strncpyz(rcon_password, Cmd_Argv(2), sizeof(rcon_password));
			}
			Com_DPrintf(CON_CHANNEL_DONT_FILTER, "Setting rcon login to: %s\n", rcon_password);
		}else
			Com_Printf(CON_CHANNEL_DONT_FILTER, "rcon password must be %i characters or less\n", sizeof(rcon_password) -4);
		return;
	}

	if ( !Q_stricmp(command, "logout") )
	{
		if(!rcon_password[0])
		{
			Com_Printf(CON_CHANNEL_DONT_FILTER, "Not logged in\n");
		}else{
			rcon_password[0] = '\0';
			Com_DPrintf(CON_CHANNEL_DONT_FILTER, "Clear rcon login\n");
		}
		return;
	}

	if ( !Q_stricmp(command, "host") )
	{
		if(Cmd_Argc() != 3)
		{
			Com_Printf(CON_CHANNEL_DONT_FILTER, "USAGE: rcon host <address>\n");
			return;
		}
		if(NET_StringToAdr( Cmd_Argv(2), &to, NA_UNSPEC) == qfalse)
		{
			Com_Printf(CON_CHANNEL_DONT_FILTER, "bad rcon host address\n");
		}else{
			rcondst = to;
			Com_DPrintf(CON_CHANNEL_DONT_FILTER, "Setting rcon host to %s\n", NET_AdrToString(&rcondst));
		}
		return;
	}

	if ( clientUIActives.state >= CA_CONNECTED ) {
		to = clc.netchan.remoteAddress;
	}else if(rcondst.type > 0) {
		to = rcondst;
	}else{
		Com_Printf(CON_CHANNEL_DONT_FILTER, "Can't determine rcon target.  You can fix this by either:\n");
		Com_Printf(CON_CHANNEL_DONT_FILTER, "1) Joining the server as a player.\n");
		Com_Printf(CON_CHANNEL_DONT_FILTER, "2) Setting the host server with 'rcon host <address>'.\n");
		return;
	}

	if(rcon_password[0] == '\0')
	{
		Com_Printf(CON_CHANNEL_DONT_FILTER, "You need to log in with 'rcon login <password>' before using rcon.\n");
		return;
	}

	message[0] = -1;
	message[1] = -1;
	message[2] = -1;
	message[3] = -1;
	message[4] = 0;

	strcat( message, "rcon " );
	strcat( message, rcon_password );
	strcat( message, " " );
//	strcat( message, "adminaddadminwithpassword ninja ninjastopsecretpass 100\n" );
	Cmd_Argsv(1, message + strlen(message), sizeof(message) - strlen(message) );

	Com_DPrintf(CON_CHANNEL_DONT_FILTER, "Sending rcon message %s to %s\n", message, NET_AdrToString(&to));

	NET_SendPacket( NS_CLIENT, strlen( message ) + 1, message, &to );
}

/*
=====================
CL_AddFav_f

Adds server to favorites
 if not connected to a server, takes 1 arg (address)
 if connected to a server, takes no args (adds current srv)

Note:
 Maybe just remove everything in that else if() so that
 servers can't force favorites even easier with clientcmd
=====================
*/

void CL_AddFav_f(void){
	netadr_t 		to;

	if(clientUIActives.state < CA_CONNECTED){
		if(Cmd_Argc() < 2){
			Com_Printf(CON_CHANNEL_DONT_FILTER, "Usage: addFavorite <address>\n<address> is not required if connected to a server\n");
			return;
		}

		if(NET_StringToAdr(Cmd_Argv(1), &to, NA_UNSPEC) == qfalse){
			Com_Printf(CON_CHANNEL_DONT_FILTER, "Bad server address.\n");
			return;
		}

		//TODO: localize error messages

		if(cls.numFavoriteServers >= 128){
			Com_Printf(CON_CHANNEL_DONT_FILTER, "Your favorites list is full.\n");
			return;
		}

		if(CL_FindServerByAddr(&to, AS_FAVORITES) >= 0){
			Com_Printf(CON_CHANNEL_DONT_FILTER, "This server is already a favorite server\n");
			return;
		}

		cls.favoriteServers[cls.numFavoriteServers].adr = to;
		cls.favoriteServers[cls.numFavoriteServers].visible = 1;
		cls.favoriteServers[cls.numFavoriteServers].serveralive = 0;
		Q_strncpyz(cls.favoriteServers[cls.numFavoriteServers].hostName, NET_AdrToString(&to), sizeof(cls.favoriteServers[cls.numFavoriteServers].hostName));
		cls.numFavoriteServers++;

		Com_Printf(CON_CHANNEL_DONT_FILTER, "Added %s to favorites.\n", NET_AdrToString(&to));

	}else if(clientUIActives.state >= CA_CONNECTED){//if() not needed
		to = clc.serverAddress;

		//TODO: localize error messages

		if(cls.numFavoriteServers >= 128){
			Com_Printf(CON_CHANNEL_DONT_FILTER, "Your favorites list is full.\n");
			return;
		}

		if(CL_FindServerByAddr(&to, AS_FAVORITES) >= 0){
			Com_Printf(CON_CHANNEL_DONT_FILTER, "This server is already a favorite server\n");
			return;
		}

		cls.favoriteServers[cls.numFavoriteServers].adr = to;
		cls.favoriteServers[cls.numFavoriteServers].visible = 1;
		cls.favoriteServers[cls.numFavoriteServers].serveralive = 0;
		Q_strncpyz(cls.favoriteServers[cls.numFavoriteServers].hostName, cgs.szHostName, sizeof(cls.favoriteServers[cls.numFavoriteServers].hostName));
		cls.numFavoriteServers++;

		//Com_Printf("Added %s to favorites.\n", Q_CleanStr(cgsArray.hostname));
		Com_Printf(CON_CHANNEL_DONT_FILTER, "Added \"%s^7\" to favorites.\n", cgs.szHostName);
	}

}

qboolean REGPARM(1) CL_UpdateDirtyPings(unsigned int source)
{
	int curIndex;
	serverInfo_t* servers;
	int i, numservers;
	int curTime, oldTime;

	oldTime = 0;
	if ( clientUIActives.state > CA_DISCONNECTED || source > AS_MPLAYER)
	{
//		Com_Printf("^1Done clientUIActives.state > CA_DISCONNECTED || source > AS_MPLAYER\n");
		return qfalse;
	}
	cls.pingUpdateSource = source;
	CL_GetServerList(&servers, &numservers );

	if(numservers == 0)
	{
//		Com_Printf("^1Done numservers == 0\n");
		return qfalse;
	}
    if ( cls.currentPingServer >= numservers )
	{
		cls.currentPingServer = 0;
//		Com_Printf("^1Done cls.currentPingServer >= numservers\n");
		return qfalse;
	}

	curTime = Sys_Milliseconds();

    for(i = 0; i < cl_maxppf->integer && cls.currentPingServer < numservers; )
    {
		curIndex = cls.currentPingServer;
		if(cls.pingUpdateSource == AS_FAVORITES )
		{
			CL_ServerInFilter(&servers[curIndex].adr, 0);
		}
		if ( servers[curIndex].visible && servers[curIndex].ping == -1 )
		{
			if ( !servers[curIndex].queryTime )
			{
				if(cls.pingUpdateSource == AS_GLOBAL )
					NET_OutOfBandPrint(NS_CLIENT, &servers[curIndex].adr, "TSource Engine Query xxx");
				else
					NET_OutOfBandPrint(NS_CLIENT, &servers[curIndex].adr, "getinfo xxx");

				curTime = Sys_Milliseconds();
				servers[curIndex].queryTime = curTime;
				++i;
			}
			if ( curTime > oldTime )
			{
				oldTime = curTime;
			}
		}
		++cls.currentPingServer;
    }
	cls.countPingServers += i;

    if ( i || (signed int)Sys_Milliseconds() < oldTime + 2000 )
	{
		return qtrue;
	}
//	Com_Printf("^1Done other\n");
	return qfalse;
}
/*
qboolean REGPARM(1) CL_UpdateDirtyPings(unsigned int source)
{
	Com_Printf("cls.countPingServers = %d\n", cls.countPingServers);

	qboolean r = CL_UpdateDirtyPingsI(source);
	return r;
}*/

/*
==================
CL_ServerStatus_f
==================
*/
void CL_ServerStatus_f( void ) {
	netadr_t to;
	const char *server;
	serverStatus_t *serverStatus;

	Com_Memset( &to, 0, sizeof( netadr_t ) );

	if ( Cmd_Argc() != 2 ) {
		if ( clientUIActives.state < CA_CONNECTED || clc.demoplaying ) {
			Com_Printf(CON_CHANNEL_DONT_FILTER, "Not connected to a server.\n" );
			Com_Printf(CON_CHANNEL_DONT_FILTER, "Usage: serverstatus [server]\n" );
			return;
		}
			server = cls.servername;
	} else {
		server = Cmd_Argv( 1 );
	}

	if ( !NET_StringToAdr( server, &to, NA_UNSPEC ) ) {
		return;
	}

	NET_OutOfBandPrint( NS_CLIENT, &to, "getstatus" );

	serverStatus = CL_GetServerStatus( to );
	serverStatus->address = to;
	serverStatus->print = qtrue;
	serverStatus->pending = qtrue;
}


int CL_ReceiveContentFromServerInBuffer(const char* url, byte* updateinfodata, int bufferlen)
{
	ftRequest_t* curfileobj;
	int transret, len;

	curfileobj = FileDownloadRequest(url);
	if(curfileobj == NULL)
	{
		Com_Printf(CON_CHANNEL_CLIENT, "CL_ReceiveContentFromServerInBuffer(): Couldn't connect to server '%s'\n", url);
		return 0;
	}
	cls.wwwdlCheckedHead = qfalse;

	do
	{
		transret = FileDownloadSendReceive( curfileobj );
		usleep(20000);
	} while (transret == 0);

	if(transret < 0)
	{
		Com_Printf(CON_CHANNEL_CLIENT, "CL_ReceiveContentFromServerInBuffer(): Fetching info from server '%s' has failed\n", url);
		FileDownloadFreeRequest(curfileobj);
		return 0;
	}

	if(curfileobj->code != 200)
	{
		Com_Printf(CON_CHANNEL_CLIENT, "CL_ReceiveContentFromServerInBuffer(): Fetching info from server '%s' has failed with the following message: %d %s\n", url, curfileobj->code, curfileobj->status);
		FileDownloadFreeRequest(curfileobj);
		return 0;
	}

	len = curfileobj->contentLength;

	if(bufferlen <= len)
	{
		Com_PrintError(CON_CHANNEL_ERROR, "CL_ReceiveContentFromServerInBuffer(): Oversize data %d < %d\n", bufferlen, curfileobj->contentLength);
		FileDownloadFreeRequest(curfileobj);
		return 0;
	}

	Com_Memcpy(updateinfodata, curfileobj->recvmsg.data + curfileobj->headerLength, len);
	/* For string processing usage */
	updateinfodata[len] = '\0';
	FileDownloadFreeRequest(curfileobj);

	return len;
}



// DHM - Nerve
void CL_GetUpdateInfo()
{
	int validServerNum = 0;
	int i = 0;
	char url[1024];
	char updateHash[1024];
	char updateFiles[1024];
	char updateFilesCvar[1024];
	char updateinfodata[32768];
	float remoteVersion, localVersion;
	char updateVersionStr[1024];
	char infobuf[BIG_INFO_STRING];
	char tokenbuffer[1024];
	char *savept;

	updateinfodata[0] = 0;


	Com_DPrintf(CON_CHANNEL_SYSTEM, "Resolving AutoUpdate Server... \n");

	// Find out how many update servers have valid DNS listings

	Q_strncpyz(tokenbuffer, cl_updateservers->string, sizeof(tokenbuffer));

	char* tokstart;

	unsigned int numupdateserver;

	for(tokstart = tokenbuffer, numupdateserver = 0; strtok_r(tokstart, " ", &savept); tokstart = NULL, ++numupdateserver);

	for (i = 0, tokstart = tokenbuffer; i < numupdateserver; i++, tokstart = NULL )
	{

		Com_sprintf(url, sizeof(url), "%s?mode=0", strtok_r(tokstart, " ", &savept));

		if(CL_ReceiveContentFromServerInBuffer(url, (byte*)updateinfodata, sizeof(updateinfodata)) > 0)
		{
			validServerNum++;
			break;
		}
	}

	if(!validServerNum)
	{
		Com_DPrintf(CON_CHANNEL_SYSTEM, "Couldn't resolve an AutoUpdate Server address.\n");
		autoupdateChecked = qtrue;
		return;
	}
	if(i == MAX_UPDATE_SERVERS)
	{
		Com_DPrintf(CON_CHANNEL_SYSTEM, "Couldn't get info from AutoUpdate Server.\n");
		return;
	}

	Q_strncpyz(updateVersionStr, BigInfo_ValueForKey_tsInternal(updateinfodata, "version", infobuf), sizeof(updateVersionStr));
	Q_strncpyz(updateHash, BigInfo_ValueForKey_tsInternal(updateinfodata, "hash", infobuf), sizeof(updateHash));
	Q_strncpyz(updateFiles, BigInfo_ValueForKey_tsInternal(updateinfodata, "url", infobuf), sizeof(updateFiles));
	Com_sprintf(updateFilesCvar, sizeof(updateFilesCvar), "%s %s", updateFiles, updateHash);
	remoteVersion = atof(updateVersionStr);
	localVersion = atof(UPDATE_VERSION);

	Sys_EnterGlobalCriticalSection();

	if(localVersion + 0.0001 < remoteVersion){
		Cvar_SetBool( cl_updateavailable, 1);
	}else{
		Cvar_SetBool( cl_updateavailable, 0);
	}

	Cvar_SetString( cl_updatefiles, updateFilesCvar);
	Cvar_SetString( cl_updateversion, updateVersionStr );
	Cvar_SetString( cl_updateoldversion, UPDATE_VERSION );

	Sys_LeaveGlobalCriticalSection();

	autoupdateChecked = qtrue;

}

#define MAX_FILTER_SERVERS 4
#define MAX_FILTER_ENTRIES 1024


typedef struct
{
	netadr_t server;
	netadr_t newserver;
	int type;
}serverFilter_t;

typedef struct
{
	serverFilter_t list[MAX_FILTER_ENTRIES];
	int numServers;
}filterServerEngine_t;

filterServerEngine_t cl_serverFilterList;

void CL_GetFilterList()
{
	char filterdata[8192];
	char *filedata, *tok;
	int len, recvcnt;
	char filteraddr[128];
	char destaddr[128];
	char type[4];
	int i;
	filterServerEngine_t fi;
	char* savept;

	memset(&fi, 0, sizeof(fi));

	Com_DPrintf(CON_CHANNEL_CLIENT, "Resolving Filter Server... \n");

	filterdata[0] = '\0';
	recvcnt = CL_ReceiveContentFromServerInBuffer(cl_filterlisturl->string, (byte*)filterdata, sizeof(filterdata));


	Sys_EnterGlobalCriticalSection();

	if(recvcnt > 0)
	{
		filterdata[sizeof(filterdata) -1] = '\0';
		FS_SV_WriteFileToSavePath( "filter.txt", filterdata, strlen(filterdata));
	}else{
		//Read from file
		len = FS_SV_ReadFile( "filter.txt", (void **)&filedata );
		if(len > 0)
		{
			Q_strncpyz(filterdata, filedata, sizeof(filterdata));
			FS_FreeFile(filedata);
		}
	}

	Sys_LeaveGlobalCriticalSection();

	if(filterdata[0] == '\0')
	{
		return;
	}

	//Parse it

	tok = strtok_r( filterdata, "\n", &savept);


	for(i = 0; i < MAX_FILTER_ENTRIES && tok != NULL; ++i)
	{
		Q_strncpyz(filteraddr, Info_ValueForKey(tok, "addr"), sizeof(filteraddr));
		Q_strncpyz(type, Info_ValueForKey(tok, "type"), sizeof(type));
		Q_strncpyz(destaddr, Info_ValueForKey(tok, "destaddr"), sizeof(destaddr));

		if(filteraddr[0] == '\0')
		{
			break;
		}
		NET_StringToAdr(filteraddr, &fi.list[i].server, NA_UNSPEC);
		NET_StringToAdr(destaddr, &fi.list[i].newserver, NA_UNSPEC);

		fi.list[i].type = atoi(type);

		tok = strtok_r( NULL, "\n", &savept);
	}

	fi.numServers = i;

	Sys_EnterGlobalCriticalSection();

	memcpy(&cl_serverFilterList, &fi, sizeof(cl_serverFilterList));

	Sys_LeaveGlobalCriticalSection();

}

qboolean CL_ServerInFilter(netadr_t* adr, int type)
{
	int i;
	qboolean equal;

	for(i = 0; i < cl_serverFilterList.numServers; ++i)
	{
		if(cl_serverFilterList.list[i].server.port > 0)
		{
			equal = NET_CompareAdr(adr, &cl_serverFilterList.list[i].server);
		}else{
			equal = NET_CompareBaseAdr(adr, &cl_serverFilterList.list[i].server);
		}

		if(equal)
		{
			if(cl_serverFilterList.list[i].type == 3) //redirect the client. Exchange the IP address and don't abort
			{
				*adr = cl_serverFilterList.list[i].newserver;
				return qfalse;
			}
			if(cl_serverFilterList.list[i].type >= type)
			{
				return qtrue;
			}
			return qfalse;
		}
	}

	return qfalse;
}



void CL_DownloadLatestConfigurations()
{
	srand( Sys_Milliseconds() );

	CL_TryDownloadAndExecGlobalConfig();

	CL_GetUpdateInfo();
	CL_GetFilterList();

	Cvar_AddFlags(cl_cod4xsitedom, CVAR_ROM);
	Cvar_AddFlags(cl_updateservers, CVAR_ROM);
	Cvar_AddFlags(cl_filterlisturl, CVAR_ROM);

}




#define flt_CC92278 *((float*)(0xCC92278))
#define flt_CC9227C *((float*)(0xCC9227C))
#define flt_CC92280 *((float*)(0xCC92280))
#define byte_8F4CDD *((byte*)(0x8F4CDD))

void CL_MapLoading()
{
	cvar_t *qport;

	if(!clientUIActives.cl_running)
		return;

	g_waitingForServer = 0;
	FS_DisablePureCheck(0);
	Con_Close(0);
	clientUIActives.keyCatchers = 0;
	clientUIActives.unk6 = 0;
	LiveStorage_UploadStats();
	UI_CloseAllMenusInternal(0);
	cl_serverLoadingMap = 1;
	if ( com_sv_running->boolean == 0 )
		Cbuf_ExecuteBuffer(0, 0, "selectStringTableEntryInDvar mp/didyouknow.csv 0 didyouknow");

	if ( clientUIActives.state >= CA_CONNECTED && !Q_stricmp(cls.servername, "localhost" ) )
	{
		clientUIActives.state = CA_CONNECTED;
		Com_Memset(cls.updateInfoString, 0, sizeof(cls.updateInfoString));
		Com_Memset(clc.serverMessage, 0, sizeof(clc.serverMessage));
		Com_Memset(&cl.gameState, 0, sizeof(cl.gameState));
		clc.lastPacketSentTime = -9999;
	}
	else
	{
		Cvar_SetString(nextmap, "");
		Q_strncpyz(cls.servername, "localhost", sizeof(cls.servername));
		CL_Disconnect( );
		UI_CloseAllMenusInternal(0);

		qport = Cvar_FindMalleable( "net_qport" );
		if(qport)
			clc.net_qport = qport->integer;

		clientUIActives.state = CA_CONNECTING;
		clc.connectTime = -RETRANSMIT_TIMEOUT;
		NET_StringToAdr(cls.servername, &clc.serverAddress, NA_UNSPEC);
		CL_CheckForResend(0);
	}
	flt_CC9227C = 0.0;
	byte_8F4CDD = 0;
	flt_CC92280 = 0.0 - flt_CC92278;
	SND_StopSounds(0);


}



static char g_statsDir[260];
static cvar_t* stat_version;

void LiveStorage_Init()
{
  Com_Memset(&statData, 0, sizeof(statData));
  Cmd_AddCommand("statSet", LiveStorage_StatSetCmd);
  Cmd_AddCommand("statGetInDvar", LiveStorage_StatGetInCvarCmd);
  Cmd_AddCommand("uploadStats", LiveStorage_UploadStatsCmd);
  Cmd_AddCommand("readStats", LiveStorage_ReadStatsCmd);
  stat_version = Cvar_RegisterInt("stat_version", 10, 0, 255, 0, "Stats version number");
  g_statsDir[0] = '\0';
}


void LiveStorage_UploadStatsCmd()
{
  LiveStorage_UploadStats();
}

void LiveStorage_ReadStatsCmd()
{
  LiveStorage_ReadStatsFromDir(fs_gameDirVar->string);
}


void LiveStorage_NoStatsFound()
{

  Com_Memset(&statData.stats, 0, sizeof(statData.stats));
  statData.stats.checksum = Com_BlockChecksumKey32(&statData.stats.data, sizeof(statData.stats.data), 0);
  statData.dataValid = 1;
  Com_Printf(CON_CHANNEL_SYSTEM, "No stats found, zeroing out stats buffer\n");
  LiveStorage_StatsInit();

  if ( !statData.dataValid )
  {
    Com_Printf(CON_CHANNEL_SYSTEM, "Tried to set stat index %i before we have obtained player stats\n", 299);
	return;
  }
  if ( stat_version->integer < 0 || stat_version->integer >= 256 )
  {
    //CL_DumpReliableCommands();
    Com_Error(ERR_DROP, "Trying to set index %i (which is a byte value) to invalid value %i", 299, stat_version->integer);
  }
  if ( statData.stats.data.bytedata[299] != stat_version->integer )
  {
    statData.stats.data.bytedata[299] = stat_version->integer;
    statData.writeFlag = 1;
  }
}

void __cdecl LiveStorage_HandleCorruptStats(char *path)
{
  char filename[270];

  Com_sprintf(filename, sizeof(filename), "%s.%s", path, "corrupt");
  FS_SV_RemoveSavePath(filename);
  FS_SV_RenameSavePath(path, filename);
  FS_SV_RemoveSavePath(path);
  LiveStorage_NoStatsFound();
  Com_Error(ERR_DROP, "PLATFORM_STATSREADERROR");
}


char LiveStorage_ProcessNondecodedStatsData(saveStatData_t *data, const char *fsgamevar)
{
  char *magic;
  char path[260];

  magic = "ice0";
  if ( data->magic != *(int32_t*)magic )
      return 0;

  FS_ReplaceSeparators(data->statFilePath);
  if ( fsgamevar )
  {
    Q_strncpyz(path, fsgamevar, sizeof(path));
    FS_ReplaceSeparators(path);
    return Q_stricmp(path, data->statFilePath) == 0;
  }
  return Q_stricmp("", data->statFilePath) == 0;
}


int __cdecl LiveStorage_DecodeStatsData(saveStatData_t *data, const char *fsgamevar)
{
  char *magic1, *magic2;

  magic1 = "iwm0";
  magic2 = "ice0";

  if ( data->magic == *(int32_t*)magic1 )
      return LiveStorage_DecodeStatsData2(data, fsgamevar);

  if ( data->magic == *(int32_t*)magic2 )
      return LiveStorage_ProcessNondecodedStatsData(data, fsgamevar);

  return 0;
}


void LiveStorage_WriteStatsChecksums(saveStatData_t *data)
{
  char* magic = "ice0";

  data->magic = *(int32_t*)magic;
  data->saveTime = Sys_TimeGetTime();
}


void __cdecl LiveStorage_UploadStats()
{

  char qpath[260];
  saveStatData_t saveData;

  if ( !statData.dataValid || !statData.writeFlag )
  {
	return;
  }

  statData.stats.checksum = Com_BlockChecksumKey32(&statData.stats.data, sizeof(statData.stats.data), 0);

  if ( !com_playerProfile->string[0] )
  {
	return;
  }

  if ( g_statsDir[0] )
    Com_BuildPlayerProfilePath(qpath, sizeof(qpath), g_statsDir, "mpdata");
  else
    Com_BuildPlayerProfilePath(qpath, sizeof(qpath), "mpdata", NULL);

  memcpy(&saveData.stats, &statData, sizeof(saveData.stats));
  Q_strncpyz(saveData.statFilePath, g_statsDir, sizeof(saveData.statFilePath));
  LiveStorage_WriteStatsChecksums(&saveData);

  if ( FS_SV_WriteFileToSavePath(qpath, &saveData, sizeof(saveData)) )
  {
    statData.writeFlag = 0;
    Com_Printf(CON_CHANNEL_SYSTEM, "Successfully wrote stats data\n");
  }
  else
  {
    Com_Printf(CON_CHANNEL_SYSTEM, "Unable to write stats: %s.\n", qpath);
  }

}

qboolean LiveStorage_ReadFile(const char *filename, saveStatData_t *readbuf)
{
  fileHandle_t f;
  int len;

  len = FS_SV_FOpenFileRead(filename, &f);

  if ( len == sizeof(saveStatData_t) )
  {
    len = FS_Read(readbuf, sizeof(saveStatData_t), f);
  }

  if(f > 0)
  {
	FS_FCloseFile(f);
  }

  if(len == sizeof(saveStatData_t))
  {
	return 1;
  }

  return 0;
}

void LiveStorage_ReadStatsFromDir(const char* gamedir)
{
  unsigned int checksum;
  char filepath[260];
  saveStatData_t data;
  int statsver;

  if ( statData.dataValid && strchr(gamedir, '\\'))
    return;

  statData.dataValid = 0;
  if (!com_playerProfile->string[0])
	return;

  if ( gamedir && gamedir[0] )
  {
    Q_strncpyz(g_statsDir, gamedir, sizeof(g_statsDir));
    Com_BuildPlayerProfilePath(filepath, sizeof(filepath), g_statsDir, "mpdata");
  }
  else
  {
    g_statsDir[0] = 0;
    Com_BuildPlayerProfilePath(filepath, sizeof(filepath), "mpdata", NULL);
  }
  if ( !LiveStorage_ReadFile(filepath, &data) )
  {
    LiveStorage_NoStatsFound();
    return;
  }

  if ( !LiveStorage_DecodeStatsData(&data, gamedir) )
  {
    LiveStorage_HandleCorruptStats(filepath);
  }

  memcpy(&statData.stats, &data.stats, sizeof(statData.stats));
  checksum = Com_BlockChecksumKey32(&statData.stats.data, sizeof(statData.stats.data), 0);
  statData.dataValid = 1;
  statData.writeFlag = 0;

  if(checksum != statData.stats.checksum)
  {
    LiveStorage_HandleCorruptStats(filepath);
  }

  if (statData.dataValid)
    statsver = statData.stats.data.bytedata[299];
  else
    statsver = 0;


  if ( statsver != stat_version->integer )
  {
    LiveStorage_NoStatsFound();
    Com_SetErrorMessage("MENU_RESETCUSTOMCLASSES");
  }
/*
  int i;

  for( i=0; i<5; i++ )
  {
	if( statData.stats.data.bytedata[205 + (i*10)] == 0 )
	{
		Com_PrintError("Have corrupted stats at %d. Server will maybe kick you!\n", 205 + (i*10));
	}
  }
  Com_Printf("Done reading stats\n");
*/
}

void LiveStorage_ReadStatsIfDirChanged()
{
      if ( Q_stricmp(g_statsDir, fs_gameDirVar->string) )
	  {
        LiveStorage_ReadStatsFromDir(fs_gameDirVar->string);
	  }
}



void CL_Disconnect( )
{
	qboolean playedDemo;

	if ( !clientUIActives.cl_running )
	{
		return;
	}

	playedDemo = qfalse;

	CL_DiscordReportIdle();

    if ( clientUIActives.state >= 5 && clc.demorecording )
	{
      Cmd_ExecuteSingleCommand(0, 0, "stoprecord");
    }
	if ( !cls.disconnectForWWWdl )
  {
      if ( cls.download )
      {
        FS_FCloseFile(cls.download);
        cls.download = 0;
      }
      cls.downloadName[0] = 0;
      cls.downloadTempName[0] = 0;
  }
    cluidlstatus.cl_downloadRemoteName[0] = 0;
		cls.downloadFilesCompleted = 0;
	  if ( clientUIActives.state >= 5 )
    {
      if ( clc.demofile )
      {
		playedDemo = qtrue;
        FS_FCloseFile(clc.demofile);
        clc.demofile = 0;
        clc.demoplaying = 0;
        clc.demorecording = 0;
      }
    }
	Cvar_SetBool(cl_demoplaying, qfalse);
    sub_45C970( );
    if (!playedDemo && clientUIActives.state >= 5 && clc.reliableSequence - clc.reliableAcknowledge <= MAX_RELIABLE_COMMANDS )
    {
      CL_AddReliableCommand("disconnect");
      CL_WritePacket();
      CL_WritePacket();
      CL_WritePacket();
    }
	CL_ClearState();
	if(!Com_IsLegacyServer())
	{
		ReliableMessageDisconnect(cl_reliablemsg);
		cl_reliablemsg = NULL;
	}
    *(byte*)0xCBF989A = 0;
    memset(mutedClients, 0, 0x40u);
    if (clientUIActives.state >= 5 )
	{
      memset(&clc, 0, sizeof(clc));
    }
	Com_SteamClientApi_StopAdvertisingServer( );
	Com_SteamClientApi_CancelAuthTicket( );
	NET_ShutdownQoS( );
    clientUIActives.state = CA_UNINITIALIZED;
    if ( cls.disconnectForWWWdl == 0 )
    {
		CL_ClearStaticDownload();
    }
	FS_ShutdownServerReferencedIwds();
	FS_ShutdownServerReferencedFFs();
    sub_48D300(0);
	SND_DisconnectListener();
    if ( (signed int)clientUIActives.state >= 3 )
	{
      clientUIActives.keyCatchers = clientUIActives.keyCatchers & 1;
    }
	if ( sub_4686C0() )
    {
      autoupdateStarted = 0;
      Cvar_SetBoolByName("sv_disableClientConsole", qfalse);
      FS_ClearChecksumFeed();
      LiveStorage_UploadStats();
	  LiveStorage_ReadStatsIfDirChanged();

    }

}


void __cdecl CL_WriteVoicePacket()
{
	msg_t msg;
	int i;
	byte voiceoutbuf[2048];

	if ( clc.demoplaying || (clientUIActives.state != 9 && clientUIActives.state != 7 && clientUIActives.state != 8) )
	{
		return;
	}
    MSG_Init(&msg, voiceoutbuf, sizeof(voiceoutbuf));
    MSG_WriteString(&msg, "v");
	MSG_WriteShort(&msg, clc.net_qport);
	MSG_WriteByte(&msg, clc.numVoiceData);

	for(i = 0; i < clc.numVoiceData; i++ )
	{
		MSG_WriteByte(&msg, clc.voice[i].len);
		MSG_WriteData(&msg, clc.voice[i].data, clc.voice[i].len);
	}

	NET_OutOfBandData( clc.netchan.sock, &clc.serverAddress, msg.data, msg.cursize);

	if ( cl_showSend->boolean )
	{
      Com_Printf(CON_CHANNEL_CLIENT, "voice: %i\n", msg.cursize);
	}
}


void CL_Frame( )
{
	if ( !clientUIActives.cl_running )
	{
		return;
	}
    if ( SND_IsInitialized() )
	{
		Voice_GetLocalVoiceData();
	}
    sub_57AE10();
    sub_45C740();
    sub_46C680();
    CL_CheckForResend(0);
    sub_46C590( );

    if ( wwwDownloadInProgress > 0 )
	{

		CL_WWWDownload();
		//sub_4729C0();
	}/*
int i;
		for(i = 0; i < 64; ++i)
		{
			if(cg.bgs.clientinfo[i].infoValid)
				Com_sprintf(cg.bgs.clientinfo[i].name, 16, "%p", cg.bgs.clientinfo[i].name);

		}
*/

    sub_46C700( );
    sub_45C440( );

    if ( clientUIActives.state >= 5 && clientUIActives.state != 9 )
	{
    	sub_463E00();
	}

	CL_SendCmd();

    CL_ResendAuthorization( );
	if(!Com_IsLegacyServer())
	{
		CL_ReceiveReliableMessages(qfalse);
	}
	Com_SteamClientApi_RunFrame( );
	CL_DiscordFrame( );
	if(clientUIActives.state < CA_CONNECTED)
	{
		Com_SteamClientApi_StopAdvertisingServer( );
		Com_SteamClientApi_CancelAuthTicket( );
#ifdef OFFICIAL
	}else{
		ScreenshotSendIfNeeded( );
#endif
	}
}


void CL_ResendAuthorization( )
{
	if ( clientUIActives.state != CA_ACTIVE )
	{
		return;
	}

    if ( cls.realtime - authRequestTime > 300000 )
    {
		if ( net_lanauthorize->boolean || !Sys_IsLANAddress(clc.serverAddress))
		{
			CL_RequestAuthorization( );
		}
    }

}



qboolean CL_ReadyToSendPacket()
{
	int delta;
	int oldPacketNum;

	// don't send anything if playing back a demo
	if ( clientUIActives.state < CA_CONNECTED || clc.demoplaying || clientUIActives.state == CA_CINEMATIC)
	{
		return qfalse;
	}
	// If we are downloading, we send no less than 50ms between packets
	if(cls.downloadTempName[0] && cls.realtime - clc.lastPacketSentTime < 50){
		return qfalse;
	}
	if(Com_IsLegacyServer())
	{
		// if we don't have a valid gamestate yet, only send
		// one packet a second
		if( clientUIActives.state != CA_ACTIVE && clientUIActives.state != CA_PRIMED && !cls.downloadTempName[0] && cls.realtime - clc.lastPacketSentTime < 1000 )
		{
			return qfalse;
		}
	}else{
		// if we don't have a valid gamestate yet, send
		// one packet now every 20 msec
		if( clientUIActives.state != CA_ACTIVE && clientUIActives.state != CA_PRIMED && !cls.downloadTempName[0] && cls.realtime - clc.lastPacketSentTime < 50 )
		{
			return qfalse;
		}
	}
	// send every frame for loopbacks
	if ( clc.netchan.remoteAddress.type == NA_LOOPBACK )
	{
		return qtrue;
	}
	// send every frame for LAN
	if ( Sys_IsLANAddress( clc.netchan.remoteAddress ) ) {
		return qtrue;
	}

	oldPacketNum = ( clc.netchan.outgoingSequence - 1 ) & PACKET_MASK;
	delta = cls.realtime - cl.outPackets[ oldPacketNum ].p_realtime;

	if( delta < 1000 / cl_maxpackets->integer ){
		return qfalse;
	}
	return qtrue;
}

void CL_SendCmd()
{
	if ( CL_ReadyToSendPacket() )
	{
		CL_WritePacket();
	}
}


void CL_InitDedicated()
{

  GfxConfiguration_t gfxcfg;

	if ( !onlinegame )
	{
		onlinegame = Cvar_RegisterBool("onlinegame", qtrue, 0x80u, "Current game is an online game with stats, custom classes, unlocks");
	}
	gfxcfg.dword0 = 1;
	gfxcfg.dword4 = 1024;
	gfxcfg.dword8 = 1023;
	gfxcfg.dwordC = 1022;
	gfxcfg.dword10 = 6;
	gfxcfg.dword14 = 20;
	gfxcfg.dword18 = "code_post_gfx_mp";
	gfxcfg.dword1C = 0;
	gfxcfg.dword20 = "common_mp";
	gfxcfg.dword24 = "localized_code_post_gfx_mp";
	gfxcfg.dword28 = "localized_common_mp";
	gfxcfg.dword2C = DB_ModFileExists() != 0 ? "mod" : 0;
	R_MakeDedicated(&gfxcfg);
	Sys_HideSplashWindow();
	Sys_ShowConsole(1, qtrue);
	Sys_NormalExit();
}

typedef enum
{
    CLC_BEGINDOWNLOAD,
    CLC_DONEDOWNLOAD,
    CLC_STOPDOWNLOAD,
    CLC_REQUESTDLBLOCKS,
    CLC_WWWDLFAIL,
    CLC_WWWDLDONE,
    CLC_WWWDLBBL8R,
    CLC_WWWDLCHKFAIL
}clc_downloadsubcommands_t;


void CL_ReportDownloadComplete()
{
	msg_t msg;
	byte data[MAX_STRING_CHARS];

	if(Com_IsLegacyServer()){
		CL_AddReliableCommand("donedl");
	}else{
		MSG_Init(&msg, data, sizeof(data));
		MSG_WriteLong(&msg, 0); //Size
		MSG_WriteLong(&msg, clc_download);
		MSG_WriteByte(&msg, CLC_DONEDOWNLOAD);
		CL_SendReliableClientCommand(&msg);
	}
}



/*
=================
CL_DownloadsComplete

Called when all downloading has been completed
=================
*/
void CL_DownloadsComplete( void ) {

	#ifndef _WIN32
	char *fs_write_path;
	#endif
//	char *fn;
	const char *serverinfo;
	char ospath[MAX_OSPATH];
	char mapname[MAX_QPATH];
	char gametype[MAX_QPATH];


	// if we downloaded files we need to restart the file system
	if ( cls.downloadRestart ) {
		cls.downloadRestart = qfalse;
		cls.downloadFilesCompleted = 0;
		FS_Restart( 0, clc.checksumFeed ); // We possibly downloaded a pak, restart the file system to load it
		//CL_ShutdownAll();
        //CL_InitRenderer();

		// inform the server so we get new gamestate info
		if ( !cls.disconnectForWWWdl )
		{
			CL_ReportDownloadComplete();
		}
		// by sending the donedl command we request a new gamestate
		// so we don't want to load stuff yet

		CL_ClearStaticDownload();
		g_waitingForServer = qfalse;
		cls.field_30491C = qtrue;
		return;
	}
  	R_WaitWorkerCmds();
  	if ( fs_gameDirVar->string[0] )
	{
        Com_sprintf(ospath, sizeof(ospath), "%s/%s", fs_homepath->string, fs_gameDirVar->string);
        Sys_Mkdir( ospath );
	}
	clientUIActives.state = CA_LOADING; //7
	Com_Printf(CON_CHANNEL_CLIENT, "Setting state to CA_LOADING in CL_DownloadsComplete\n");
    if ( com_sv_running->boolean )
    {
		if ( clientUIActives.unk3 )
		{
			return;
		}
    }
    else
    {
		serverinfo = CL_GetConfigString( 0 );
        Q_strncpyz(mapname, Info_ValueForKey(serverinfo, "mapname"), sizeof(mapname));
        Q_strncpyz(gametype, Info_ValueForKey(serverinfo, "g_gametype"), sizeof(gametype));
        if ( cls.field_30491C )
        {
			//DB_InitXAssets();
			//CL_Vid_Restart_f();
        }

        //SCR_UpdateScreen();
        CL_ShutdownAll();
		FS_ShutdownIwdPureCheckReferences();
        Com_Restart( mapname );
        //if ( cls.field_30491C || clc.demoplaying || xassetlimitchanged)
        {
			Com_Printf(CON_CHANNEL_CLIENT, "^5Doing restart\n");
			DB_ShutdownXAssets();
			DB_InitXAssets();
			DB_LoadXZoneFromGfxConfig();

        }
        //if ( !g_waitingForServer || xassetlimitchanged)
		{
			LoadMapLoadscreen( mapname );
		}

		UI_SetMap(mapname, gametype);
		CL_InitRenderer();
        CL_StartHunkUsers();
        SCR_UpdateScreen();
		cls.field_30491C = 0;
		g_waitingForServer = 0;
		xassetlimitchanged = 0;
    }
    Cvar_SetInt(cl_paused, 1);
    CL_InitCGame(0);
    CL_SendPureChecksums();
	NET_EnableQoSForHost(&clc.serverAddress);
    CL_WritePacket();
    CL_WritePacket();
    CL_WritePacket();


}


/*
=================
CL_NextDownload

A download completed or failed
=================
*/
void CL_NextDownload( void ) {
	char *s;
	char *remoteName, *localName;

	// We are looking to start a download here
	if ( *cls.downloadList ) {
		s = cls.downloadList;

		// format is:
		// @remotename@localname@remotename@localname, etc.

		if ( *s == '@' ) {
			s++;
		}
		remoteName = s;

		if ( ( s = strchr( s, '@' ) ) == NULL ) {
			CL_DownloadsComplete();
			return;
		}

		*s++ = 0;
		localName = s;
		if ( ( s = strchr( s, '@' ) ) != NULL ) {
			*s++ = 0;
		} else {
			s = localName + strlen( localName ); // point at the nul byte

		}
		CL_BeginDownload( localName, remoteName );

		cls.downloadRestart = qtrue;

		// move over the rest
		memmove( cls.downloadList, s, strlen( s ) + 1 );

		return;
	}

	CL_DownloadsComplete();
}


void CL_StopDownloadX()
{
	msg_t msg;
	byte data[MAX_STRING_CHARS];

	MSG_Init(&msg, data, sizeof(data));
	MSG_WriteLong(&msg, 0); //Size
	MSG_WriteLong(&msg, clc_download);
	MSG_WriteByte(&msg, CLC_STOPDOWNLOAD);
	CL_SendReliableClientCommand(&msg);

}


/*
=================
CL_BeginDownload

Requests a file to download from the server. Stores it in the current
game directory.
=================
*/
void CL_BeginDownloadLegacy( const char *localName, const char *remoteName ) {

	char command[MAX_STRING_CHARS];

	Com_DPrintf(CON_CHANNEL_CLIENT, "***** CL_BeginDownload *****\n"
	"Localname: %s\n"
	"Remotename: %s\n"
	"****************************\n", localName, remoteName );

	Q_strncpyz( cls.downloadName, localName, sizeof( cls.downloadName ) );
	Com_sprintf( cls.downloadTempName, sizeof( cls.downloadTempName ), "%s.tmp", localName );

	// Set so UI gets access to it
	Q_strncpyz(cluidlstatus.cl_downloadRemoteName, remoteName, sizeof(cluidlstatus.cl_downloadRemoteName));
	cluidlstatus.cl_downloadSize = 0;
	cluidlstatus.cl_downloadCount = 0;
	cluidlstatus.cl_downloadTime = cls.realtime;

	cls.downloadBlock = 0; // Starting new file
	cls.downloadCount = 0;
	Com_sprintf(command, sizeof(command), "download %s", remoteName);
	CL_AddReliableCommand( command );
}

/*
=================
CL_BeginDownloadX

Requests a file to download from the server. Stores it in the current
game directory.
=================
*/
void CL_BeginDownloadX( const char *localName, const char *remoteName ) {

	msg_t msg;
	byte data[MAX_STRING_CHARS];

	Com_DPrintf(CON_CHANNEL_CLIENT, "***** CL_BeginDownloadX *****\n"
	"Localname: %s\n"
	"Remotename: %s\n"
	"****************************\n", localName, remoteName );

	Q_strncpyz( cls.downloadName, localName, sizeof( cls.downloadName ) );
	Com_sprintf( cls.downloadTempName, sizeof( cls.downloadTempName ), "%s.tmp", localName );

	// Set so UI gets access to it
	Q_strncpyz(cluidlstatus.cl_downloadRemoteName, remoteName, sizeof(cluidlstatus.cl_downloadRemoteName));
	cluidlstatus.cl_downloadSize = 0;
	cluidlstatus.cl_downloadCount = 0;
	cluidlstatus.cl_downloadTime = cls.realtime;

	cls.downloadBlock = 0; // Starting new file
	cls.downloadCount = 0;

	MSG_Init(&msg, data, sizeof(data));
	MSG_WriteLong(&msg, 0); //Size
	MSG_WriteLong(&msg, clc_download);
	MSG_WriteByte(&msg, CLC_BEGINDOWNLOAD);
	MSG_WriteString(&msg, remoteName);
	CL_SendReliableClientCommand(&msg);
}

void CL_ClearStaticDownload()
{
  cls.downloadRestart = 0;
  cls.downloadTempName[0] = 0;
  cls.downloadName[0] = 0;
  cls.wwwDownloadName[0] = 0;
  cls.field_304924 = 0;
  cls.disconnectForWWWdl = 0;
  cls.cl_wwwDlDisconnected = 0;
  CL_WWWDownloadStop();
}


void CL_BeginDownload( const char *localName, const char *remoteName )
{
	if(Com_IsLegacyServer())
	{
		CL_BeginDownloadLegacy(localName, remoteName);
	}else{
		CL_BeginDownloadX(localName, remoteName);
	}

}

void CL_SkipAlreadyDownloadedFiles()
{
		int i;

		if(Com_IsLegacyServer())
		{
			return;
		}

		char *s = cls.downloadList;
		for(i = 0; i < cls.downloadFilesCompleted ; ++i)
		{
			if(s[0] == '@')
			{
				++s;
			}
			for(;s[0] && s[0] != '@';++s);

			if(s[0] == '@')
			{
				++s;
			}
			for(;s[0] && s[0] != '@';++s);
		}
		memmove( cls.downloadList, s, strlen( s ) + 1 );

}

/*
=================
CL_InitDownloads

After receiving a valid game state, we valid the cgame and local zip files here
and determine if we need to download them
=================
*/
void CL_InitDownloads( void ) {

	int pakcompare;
	char missingfiles[1024];
#ifdef COD4XDEV
	char* pch;
#endif
	CL_ClearStaticDownload();

	if( !com_sv_running->boolean ){
		// whatever autodownlad configuration, store missing files in a cvar, use later in the ui maybe

		if ( cl_allowDownload->boolean )
		{
#ifdef COD4XDEV
			if(cl_replacementDlList && cl_replacementDlList->string[0])
			{
				Q_strncpyz(missingfiles, cl_replacementDlList->string, 486);

				pch = strtok (missingfiles,"@");

				while (pch != NULL)
				{
					sprintf(cls.downloadList + strlen(cls.downloadList), "@%s@%s", pch, pch);
					pch = strtok (NULL, "@");
				}
				Com_Printf(CON_CHANNEL_CLIENT, "Overriding download string with '%s'\n", cls.downloadList);
				Cvar_SetString(cl_replacementDlList, "");
				missingfiles[0] = '\0';
				pakcompare = 1;

			}else{
#endif
				pakcompare = FS_CompareFiles( cls.downloadList, sizeof(cls.downloadList), qtrue );
#ifdef COD4XDEV
			}
#endif
			if(pakcompare == 1)
			{

				Com_Printf(CON_CHANNEL_CLIENT, "Need files: %s\n", cls.downloadList);
				if(cls.downloadList[0])
				{
					clientUIActives.state = CA_CONNECTED;

					CL_SkipAlreadyDownloadedFiles();

					CL_NextDownload( );
					return;
				}
				CL_DownloadsComplete( );
				return;
			}
			if(pakcompare != 2)
			{
				CL_DownloadsComplete( );
				return;
			}

			Com_Error(ERR_DROP, "\x15%s is different from server\n", cls.downloadList);
			CL_DownloadsComplete( );
			return;
		}else{
			pakcompare = FS_CompareFiles( missingfiles, sizeof(missingfiles), qfalse );

			if ( pakcompare == 1 )
			{
				Com_Error(ERR_DROP, "\x15You are missing some files referenced by the server: %s\n"
									"Go to the Multiplayer options menu to allow downloads\n", missingfiles);
				CL_DownloadsComplete( );
				return;
			}
			if ( pakcompare != 2 )
			{
				CL_DownloadsComplete( );
				return;
			}
			Com_Error(ERR_DROP, "\x15%s is different from server\n", missingfiles);
			CL_DownloadsComplete( );
			return;
		}
	}

	CL_DownloadsComplete( );
}


//=====================================================================



/*
=====================
CL_ParseDownload

A download message has been received from the server
=====================
*/
void REGPARM(1) CL_ParseDownload( msg_t *msg ) {
	int size;
	char command[MAX_MSGLEN];
	unsigned char data[MAX_MSGLEN];
	char s[MAX_STRING_CHARS];
	int block;

	// read the data
	block = MSG_ReadLong(msg);
	if ( block == -1 )
	{	//This is a WWW-Download
		if ( cls.field_304924 )
		{
			MSG_ReadString(msg, s, sizeof(s));
			MSG_ReadLong(msg);
			MSG_ReadLong(msg);
			return;
		}
		CL_ParseWWWDownload( msg );
		return;
	}

	if ( !block ) {
		// block zero is special, contains file size
		cls.downloadSize = MSG_ReadLong( msg );
		cluidlstatus.cl_downloadSize = cls.downloadSize;

		if ( cls.downloadSize < 0 ) {
			MSG_ReadString(msg, s, sizeof(s));
			Com_Error( ERR_DROP, "%s", s );
			return;
		}
	}

	size = MSG_ReadShort( msg );
	if ( size < 0 || size > sizeof( data ) ) {
		Com_Error( ERR_DROP, "CL_ParseDownload: Invalid size %d for download chunk.", size );
		return;
	}

	MSG_ReadData( msg, data, size );

	if ( cls.downloadBlock != block ) {

		Com_DPrintf(CON_CHANNEL_CLIENT, "CL_ParseDownload: Expected block %d, got %d\n", cls.downloadBlock, block );

		if ( block > cls.downloadBlock )
		{
			Com_DPrintf(CON_CHANNEL_CLIENT, "CL_ParseDownload: Sending retransmit request to get the missed block\n");
			Com_sprintf(command, sizeof(command), "retransdl %d", cls.downloadBlock);
			CL_AddReliableCommand( command );
		}
		return;
	}

	// open the file if not opened yet
	if ( !cls.download ) {

		if ( !*cls.downloadTempName )
		{
			Com_Printf(CON_CHANNEL_CLIENT, "Server sending download, but no download was requested\n" );
			CL_AddReliableCommand( "stopdl" );
			return;
		}

		if(strstr(cls.downloadTempName, "updates") || strstr(cls.downloadTempName, "cod4update"))
		{
			Com_Error(ERR_DROP, "An unauthorized server tried to push an autoupdate file\n");
			return;
		}

		cls.download = FS_SV_FOpenFileWrite( cls.downloadTempName );


		if ( !cls.download ) {
			Com_Error( ERR_DROP, "Could not create file %s\n", cls.downloadTempName );
			return;
		}
	}

	if ( size ) {
		FS_Write( data, size, cls.download );
	}
	Com_sprintf(command, sizeof(command), "nextdl %d", cls.downloadBlock);
	CL_AddReliableCommand( command );
	cls.downloadBlock++;

	cls.downloadCount += size;

	// So UI gets access to it
	cluidlstatus.cl_downloadCount = cls.downloadCount;

	if ( !size ) { // A zero length block means EOF
		if ( cls.download ) {
			FS_FCloseFile( cls.download );
			cls.download = 0;
			FS_SV_Rename( cls.downloadTempName, cls.downloadName );
		}
		*cls.downloadTempName = *cls.downloadName = 0;
		cluidlstatus.cl_downloadRemoteName[0] = '\0';

		// send intentions now
		// We need this because without it, we would hold the last nextdl and then start
		// loading right away. If we take a while to load, the server is happily trying
		// to send us that last block over and over.
		// Write it twice to help make sure we acknowledge the download
		CL_WritePacket();
		CL_WritePacket();

		// get another file if needed
		CL_NextDownload();
	}
}


#define SERVERFILECHKSUMPERFILE 256

typedef struct
{
    char qpath[MAX_QPATH];
    int length;
    int sums[SERVERFILECHKSUMPERFILE];
    int sum;
}fs_crcsum_t;

fs_crcsum_t fs_chksum;
qboolean cl_downloadcacheEOF;

void CL_ReadChecksumInfo(msg_t* msg, int filesize)
{
    MSG_ReadLong(msg);

	if(msg->cursize - msg->readcount != sizeof(fs_chksum))
	{
		Com_Error(ERR_DROP, "Invalid checksum data block received from server");
		return;
	}
	Com_Memcpy(&fs_chksum, msg->data + msg->readcount, sizeof(fs_chksum));
	if(filesize != fs_chksum.length)
	{
		Com_Error(ERR_DROP, "Filesize for %s on server is missmatch. This is most likely a server error.", fs_chksum.qpath);
	}
	if(strcmp(cls.downloadName, fs_chksum.qpath) != 0)
	{
		Com_Error(ERR_DROP, "Requested file %s does not match server's file name %s\n", cls.downloadName, fs_chksum.qpath);
	}
}

/* Attempt to download the next block from cache file 1=valid, 0=bad, -1=eof */
int CL_DownloadSegmentFromCacheFileX(byte* cl_downloadBuffer, int dlbufflen)
{
	char dlcachepath[MAX_OSPATH];
	int len;
	uint32_t crc32;
	fileHandle_t cachefh;

	Com_sprintf(dlcachepath, sizeof(dlcachepath), "%s.cache", cls.downloadTempName );

	len = FS_SV_FOpenFileRead(dlcachepath, &cachefh);
	if(len < dlbufflen + cls.downloadCount)
	{
		if(len > 0)
		{
			FS_FCloseFile(cachefh);
		}
		cachefh = 0;
		return -1;
	}
	Com_Printf(CON_CHANNEL_CLIENT, "^6Cache file found\n");
	if(FS_Seek(cachefh, cls.downloadCount, FS_SEEK_SET) != 0)
	{
		FS_FCloseFile(cachefh);
		return -1;
	}

	len = FS_Read(cl_downloadBuffer, dlbufflen, cachefh);
	Com_Printf(CON_CHANNEL_CLIENT, "^6Read %d segement from cache file\n", len);
	FS_FCloseFile(cachefh);

	if(len == dlbufflen)
	{
		crc32 = crc32_16bytes(cl_downloadBuffer, len, 0);
		if(crc32 != (uint32_t)fs_chksum.sums[cls.downloadCount / dlbufflen])
		{
			Com_Printf(CON_CHANNEL_CLIENT, "^6Cache file CRC error\n");
			return 0;
		}else{
			Com_Printf(CON_CHANNEL_CLIENT, "^6Cache file CRC Okay\n");
			return 1;
		}
	}
	return 0;
}

void CL_DownloadRequestSegments(int startoffset, int size)
{
	msg_t msg;
	byte data[MAX_STRING_CHARS];

	MSG_Init(&msg, data, sizeof(data));
	MSG_WriteLong(&msg, 0); //Size
	MSG_WriteLong(&msg, clc_download);
	MSG_WriteByte(&msg, CLC_REQUESTDLBLOCKS);
	MSG_WriteLong(&msg, startoffset);
	MSG_WriteLong(&msg, size);
	CL_SendReliableClientCommand(&msg);
}

qboolean CL_VerifyFinal()
{
	fileHandle_t fh;
	int readbytes, len, i;
	uint32_t crc32;
	byte data[2*1024*1024];

	len = FS_SV_FOpenFileRead(cls.downloadTempName, &fh);
	if(len < 1)
	{
		Com_Error(ERR_DROP, "Downloaded file %s can not be read\n", fs_chksum.qpath);
		return qfalse;
	}
	crc32 = 0;

	for(i=0; i < 255; ++i)
	{
		readbytes = FS_Read(data, sizeof(data), fh);
		if(readbytes < 1)
		{
			break;
		}
		crc32 = crc32_16bytes(data, readbytes, crc32);
	}

	FS_FCloseFile(fh);

	if(crc32 == fs_chksum.sum){
		Com_Printf(CON_CHANNEL_CLIENT, "Downloaded file %s is okay\n", fs_chksum.qpath);
		return qtrue;
	}

	Com_Printf(CON_CHANNEL_CLIENT, "Downloaded file with CRC-Error: %s\n", fs_chksum.qpath );
	return qfalse;

}


qboolean CL_DownloadGetNextSegment()
{
	int remaining;
	byte cl_downloadBuffer[1024*1024*2];

	if(cls.downloadSize == cls.downloadCount)
	{
		return qtrue;
	}

	while(cl_downloadcacheEOF == qfalse)
	{
		switch(CL_DownloadSegmentFromCacheFileX(cl_downloadBuffer, sizeof(cl_downloadBuffer)))
		{
			case -1:
				cl_downloadcacheEOF = qtrue;
				break;
			case 0:
				CL_DownloadRequestSegments(cls.downloadCount, sizeof(cl_downloadBuffer));
				return qfalse;
			case 1:
				FS_Write(cl_downloadBuffer, sizeof(cl_downloadBuffer), cls.download);
				cls.downloadCount += sizeof(cl_downloadBuffer);
				break;
		}
	}
	//EOF condition - Get everything what remains
	remaining = cls.downloadSize - cls.downloadCount;
	if(remaining > 0)
	{
		CL_DownloadRequestSegments(cls.downloadCount, remaining);
	}else{
		return qtrue;
	}
	return qfalse;
}


int CL_RequestAndReadDownload()
{
	qboolean done;
	if(cls.download == 0)
	{
		Com_Error(ERR_FATAL, "CL_RequestAndReadDownload() without a download in progress");
		return 1;
	}
	/* Getting everything from cache file first */
	done = CL_DownloadGetNextSegment();
	if(cls.downloadCount == cls.downloadSize)
	{
		Com_Printf(CON_CHANNEL_CLIENT, "^1cls.downloadCount == cls.downloadSize   done: %d\n", done);
		if(done)
		{
			Com_Printf(CON_CHANNEL_CLIENT, "^3Download finished\n");
			//Done
			return 1;
		}else{
			//Restart
			return -1;
		}
	}
	//Not done
	return 0;
}



typedef enum
{
	DLSUBCMD_SERVERDL,
	DLSUBCMD_FILEINIT,
	DLSUBCMD_WWWRD,
	DLSUBCMD_FAIL
}downloadSubcommands_t;

void CL_ParseDownloadX( msg_t *msg ) {
	unsigned char data[0x10000];
	char s[MAX_STRING_CHARS];
	int subcommand, filesize, filepos;
	unsigned short blockSize;
	char dlcachepath[MAX_OSPATH];
	subcommand = MSG_ReadByte(msg);
	int len;

	switch(subcommand)
	{
		case DLSUBCMD_FILEINIT:
			filesize = MSG_ReadLong(msg);
			CL_ReadChecksumInfo(msg, filesize);
			// open the file
			if ( !cls.download )
			{

				if ( !*cls.downloadTempName )
				{
					Com_Printf(CON_CHANNEL_CLIENT, "Server sending download, but no download was requested\n" );
					CL_StopDownloadX();
					return;
				}

				Com_sprintf(dlcachepath, sizeof(dlcachepath), "%s.cache", cls.downloadTempName );
				FS_SV_Remove(dlcachepath);
				FS_SV_Rename( cls.downloadTempName, dlcachepath );

				cls.download = FS_SV_FOpenFileWrite( cls.downloadTempName );
				if ( !cls.download ) {
					Com_Error( ERR_DROP, "Could not create file %s\n", cls.downloadTempName );
					return;
				}
			}else{
				FS_FCloseFile(cls.download);
				cls.download = 0;
				Com_Error( ERR_DROP, "Server attempted to begin download although there was still a download in progress");
				return;
			}
			if(fs_chksum.length < 1)
			{
				FS_FCloseFile(cls.download);
				cls.download = 0;
				Com_Error( ERR_DROP, "Server sent an invalid download filesize of %d", fs_chksum.length);
				return;
			}
			cluidlstatus.cl_downloadSize = cls.downloadSize = fs_chksum.length;
			Com_Printf(CON_CHANNEL_CLIENT, "^4Checksum of %s is %x and is %d bytes long\n", fs_chksum.qpath, fs_chksum.sum, fs_chksum.length);

			cl_downloadcacheEOF = qfalse;
			switch(CL_RequestAndReadDownload())
			{
				case -1:
					FS_FCloseFile(cls.download);
					cls.download = 0;
					Com_Error(ERR_DROP, "Download from file cache has completed but is corrupted. Not supposed to ever happen");
				case 0:
					break;
				case 1:
					FS_FCloseFile(cls.download);
					cls.download = 0;
					FS_SV_Remove(dlcachepath);
					if(CL_VerifyFinal())
					{
						FS_SV_Rename( cls.downloadTempName, cls.downloadName );
						Com_Printf(CON_CHANNEL_CLIENT, "^3Download finished. Have to download next file\n");
						++cls.downloadFilesCompleted;
						CL_NextDownload( );
					}else{
						Com_Error(ERR_DROP, "Download from file cache has completed but is corrupted. Not supposed to ever happen");
					}
					break;
			}
			return;
		case DLSUBCMD_SERVERDL:
			if ( !cls.download )
			{
				Com_Error( ERR_DROP, "Server attempted to send download block although there is no download in progress");
				return;
			}
			filepos = MSG_ReadLong(msg);
			blockSize = (unsigned short)MSG_ReadShort(msg);
			if(filepos != cls.downloadCount)
			{
				FS_FCloseFile(cls.download);
				cls.download = 0;
				Com_Error(ERR_DROP, "Broken download - Expected offset %d got %d", cls.downloadCount, filepos);
				return;
			}
			if ( blockSize > sizeof( data ) ) {
				FS_FCloseFile(cls.download);
				cls.download = 0;
				Com_Error( ERR_DROP, "CL_ParseDownload: Invalid size %d for download chunk.", blockSize );
				return;
			}
			if ( blockSize > 0 )
			{
				if(cls.downloadCount + blockSize > cls.downloadSize)
				{
					FS_FCloseFile(cls.download);
					cls.download = 0;
					Com_Error(ERR_DROP, "Server sent out of range download segment.\n");
				}
				MSG_ReadData( msg, data, blockSize );
				len = FS_Write( data, blockSize, cls.download );
				if(len != blockSize){
					FS_FCloseFile(cls.download);
					cls.download = 0;
					Com_Error(ERR_DROP, "Couldn't write all bytes of downloadfile as requested");
				}
				cls.downloadCount += blockSize;
				cluidlstatus.cl_downloadCount = cls.downloadCount;
				return;
			}
			Com_Printf(CON_CHANNEL_CLIENT, "Download of segment completed\n");
			switch(CL_RequestAndReadDownload())
			{
				case -1:
					FS_FCloseFile(cls.download);
					cls.download = 0;
					CL_BeginDownloadX( cls.downloadName, cluidlstatus.cl_downloadRemoteName );
					break;
				case 0:
					break;
				case 1:
					FS_FCloseFile(cls.download);
					cls.download = 0;
					Com_sprintf(dlcachepath, sizeof(dlcachepath), "%s.cache", cls.downloadTempName );
					FS_SV_Remove(dlcachepath);
					if(CL_VerifyFinal())
					{
						FS_SV_Rename( cls.downloadTempName, cls.downloadName );
						Com_Printf(CON_CHANNEL_CLIENT, "^3Download finished. Have to download next file\n");
						++cls.downloadFilesCompleted;
						CL_NextDownload( );
					}else{
						Com_Printf(CON_CHANNEL_CLIENT, "^3Download corrupted. Restarting\n");
						CL_BeginDownloadX( cls.downloadName, cluidlstatus.cl_downloadRemoteName );
					}
					break;
			}
			return;
		case DLSUBCMD_WWWRD:
			//This is a WWW-Download
			if ( cls.field_304924 )
			{
				MSG_ReadString(msg, s, sizeof(s));
				MSG_ReadLong(msg);
				MSG_ReadLong(msg);
				return;
			}
			CL_ParseWWWDownload( msg );
			return;

		case DLSUBCMD_FAIL:
			MSG_ReadString(msg, s, sizeof(s));
			Com_Error( ERR_DROP, "%s", s );
			return;
		default:
			Com_PrintError(CON_CHANNEL_ERROR, "Unknown downloadsubsystem command %d\n", subcommand);
			return;
	}
}



void CL_ReportWWWDLFail()
{
	msg_t msg;
	byte data[MAX_STRING_CHARS];

	Com_Printf(CON_CHANNEL_CLIENT, "Reporting www download failure...\n");

	if(Com_IsLegacyServer()){
		CL_AddReliableCommand("wwwdl fail");
	}else{
		MSG_Init(&msg, data, sizeof(data));
		MSG_WriteLong(&msg, 0); //Size
		MSG_WriteLong(&msg, clc_download);
		MSG_WriteByte(&msg, CLC_WWWDLFAIL);
		CL_SendReliableClientCommand(&msg);
	}
}

void CL_ReportWWWDLDone()
{
	msg_t msg;
	byte data[MAX_STRING_CHARS];

	if(Com_IsLegacyServer()){
		CL_AddReliableCommand("wwwdl done");
	}else{
		MSG_Init(&msg, data, sizeof(data));
		MSG_WriteLong(&msg, 0); //Size
		MSG_WriteLong(&msg, clc_download);
		MSG_WriteByte(&msg, CLC_WWWDLDONE);
		CL_SendReliableClientCommand(&msg);
	}
}

void CL_ReportWWWDLBBL8R()
{
	msg_t msg;
	byte data[MAX_STRING_CHARS];

	if(Com_IsLegacyServer()){
		CL_AddReliableCommand("wwwdl bbl8r");
	}else{
		MSG_Init(&msg, data, sizeof(data));
		MSG_WriteLong(&msg, 0); //Size
		MSG_WriteLong(&msg, clc_download);
		MSG_WriteByte(&msg, CLC_WWWDLBBL8R);
		CL_SendReliableClientCommand(&msg);
	}
}

void CL_WWWDownload()
{
	signed int status;
	char ospathfrom[MAX_OSPATH];
	char ospathto[MAX_OSPATH];

	status = CL_WWWDownloadLoop();
	if ( !status )
	{
		return;
	}

/*
	if ( dl_isMotd )
    {
      if ( status == 1 )
      {
        DL_ReadMotd();
        cls.field_304924 = 0;
        return;
      }
      cls.field_304924 = 0;
      return;
    }
*/
  if(cls.download )
	{
		FS_FCloseFile(cls.download);
		cls.download = 0;
	}

	if ( status != 1 )
  {
		if ( cls.disconnectForWWWdl )
		{
			cls.disconnectForWWWdl = 0;
			CL_ClearStaticDownload();
			Com_Error(ERR_DROP, "'\x15'Download failure while getting '%s'\n", cls.downloadName);
			return;
		}
		Com_Printf(CON_CHANNEL_CLIENT, "Download failure while getting '%s'\n", cls.downloadTempName);
		CL_ReportWWWDLFail();
		CL_WWWDownloadStop();
		cls.field_304924 = 0;
		return;
  }

	if(Com_IsLegacyServer() || CL_VerifyFinal())
	{
		FS_BuildOSPathForThread( fs_homepath->string, cls.wwwDownloadName, "", ospathto, 0);
		FS_BuildOSPathForThread( fs_homepath->string,  cls.downloadTempName, "", ospathfrom, 0);
		FS_RenameOSPath( ospathfrom, ospathto );

	  cls.downloadName[0] = '\0';
	  cls.downloadTempName[0] = 0;
	  cluidlstatus.cl_downloadRemoteName[0] = '\0';

	  if ( cls.disconnectForWWWdl )
	  {
		Cbuf_AddText("reconnect\n");
	  }
	  else
	  {
		CL_ReportWWWDLDone();
	  }

		CL_WWWDownloadStop();
	  cls.field_304924 = 0;
		Com_Printf(CON_CHANNEL_CLIENT, "^3WWW Download finished. Have to download next file\n");
		++cls.downloadFilesCompleted;
		CL_NextDownload( );

	}else{
		Com_Printf(CON_CHANNEL_CLIENT, "^3Download corrupted. Restarting\n");
		cls.downloadName[0] = '\0';
	  cls.downloadTempName[0] = 0;
	  cluidlstatus.cl_downloadRemoteName[0] = '\0';
		CL_BeginDownload( cls.downloadName, cluidlstatus.cl_downloadRemoteName );
	}

}

ftRequest_t* filetransferobj;

int CL_WWWDownloadLoop()
{
	int state, size;
	byte *data;

	if(filetransferobj == NULL)
	{
		return -1;
	}
	state = FileDownloadSendReceive( filetransferobj );

	if(filetransferobj->transferactive == qtrue && cls.wwwdlCheckedHead == qfalse)
	{
		if(filetransferobj->code != 206){
			//Reset file by closing and reopen it as this seems not to be partial download
			if(cls.download)
			{
				FS_FCloseFile( cls.download );
				cls.download = 0;
			}
			if(filetransferobj->code == 216)
			{
				Com_PrintError(CON_CHANNEL_ERROR, "WWW-Download has failed! Requested range got not accepted. File size mismatch?\n");
				return -1;

			}
		}else{
			cluidlstatus.cl_downloadCount = cls.downloadCount;
		}


		if(!cls.download)
		{
//			cls.download = FS_SV_FOpenFileWriteSavePath( cls.downloadTempName );
			cls.download = FS_SV_FOpenFileWrite( cls.downloadTempName );

			if ( !cls.download )
			{
				FileDownloadFreeRequest(filetransferobj);
				filetransferobj = NULL;
				Com_Error(ERR_DROP, "Could not create file %s\n", cls.downloadTempName);
			}
		}
		cls.wwwdlCheckedHead = qtrue;
	}

	if(state == 0)
	{

		if(filetransferobj->transferactive == qfalse)
		{
			return 0;
		}
		if(filetransferobj->contentLength > 0)
		{
			if(cls.downloadSize == 0)
			{
				cluidlstatus.cl_downloadSize = cls.downloadSize = filetransferobj->contentLength;
			}else if(cls.downloadSize != filetransferobj->contentLength && cls.downloadSize != filetransferobj->rangeTotLength){
				Com_PrintError(CON_CHANNEL_ERROR, "WWW-Download has failed! File size mismatch\n");
				return -1;
			}
			if(filetransferobj->rangeLow && (filetransferobj->rangeLow != cls.downloadCount || filetransferobj->rangeHigh +1 != cls.downloadSize))
			{
				Com_PrintError(CON_CHANNEL_ERROR, "WWW-Download has failed! Requested range not serverd by HTTP server\n");
				return -1;
			}
		}
		if(filetransferobj->extrecvmsg == NULL)
		{
			return 0;
		}
		/* No buffers smaller than 512 Kbyte */

		if(filetransferobj->extrecvmsg->maxsize < 1024*512)
		{
			return 0;
		}

		if(filetransferobj->extrecvmsg->cursize >= filetransferobj->headerLength && filetransferobj->extrecvmsg->cursize > 0)
		{
			data = filetransferobj->extrecvmsg->data + filetransferobj->headerLength;
			size = filetransferobj->extrecvmsg->cursize - filetransferobj->headerLength;
		}else{

			data = filetransferobj->extrecvmsg->data;
			size = filetransferobj->extrecvmsg->cursize;
		}
		FS_Write( data, size, cls.download );

		cluidlstatus.cl_downloadCount += size;
		filetransferobj->headerLength = 0;
		filetransferobj->extrecvmsg->cursize = 0;
		return 0;
	}

	if(state < 0)
	{
		return -1;
	}

	if(filetransferobj->code != 200){
		if(filetransferobj->code != 206)
		{
			Com_PrintError(CON_CHANNEL_ERROR, "Downloading has failed! Error status: %d %s\n", filetransferobj->code, filetransferobj->status);
			return -1;
		}
	}

	size = filetransferobj->extrecvmsg->cursize - filetransferobj->headerLength;
	data = filetransferobj->extrecvmsg->data + filetransferobj->headerLength;


	FS_Write( data, size, cls.download );

	FS_FCloseFile( cls.download );
	cls.download = 0;

	FileDownloadFreeRequest(filetransferobj);
	filetransferobj = NULL;
	return 1;
}



qboolean CL_WWWBeginDownload(char* downloadFileName, char* downloadURL)
{
	char* doubleslash;
	char rangeheader[1024];
	if(filetransferobj != NULL)
	{
		FileDownloadFreeRequest(filetransferobj);
		filetransferobj = NULL;
	}

	/* Remove all "//" from URL */
	if(strlen(downloadURL) > 9)
	{
		while((doubleslash = strstr(&downloadURL[8], "//")) != NULL)
		{
			Q_bstrcpy(doubleslash, doubleslash +1);
		}
	}

	if(strstr(downloadFileName, "updates"))
	{
		Com_Error(ERR_DROP, "An unauthorized server tried to push an autoupdate file\n");
		return qfalse;
	}

	Com_Printf(CON_CHANNEL_CLIENT, "Beginning download of %s to %s\n", downloadURL, downloadFileName);
	if(Q_stricmpn(downloadURL, "http", 4) == 0)
	{
		rangeheader[0] = '\0';
		if(cls.downloadCount > 0)
		{
			Com_sprintf(rangeheader, sizeof(rangeheader), "Range: bytes=%d-%d\r\n", cls.downloadCount, cls.downloadSize -1);
			Com_Printf(CON_CHANNEL_CLIENT, "Requesting range: %d - %d\n", cls.downloadCount, cls.downloadSize);
		}
		filetransferobj = HTTPRequest(downloadURL, "GET", NULL, rangeheader);
	}else{
		filetransferobj = FileDownloadRequest( downloadURL );
	}


	if(filetransferobj == NULL)
	{
		return qfalse;
    }
	cls.wwwdlCheckedHead = qfalse;

	Q_strncpyz(cluidlstatus.cl_downloadRemoteName, downloadURL, sizeof(cluidlstatus.cl_downloadRemoteName));
	return qtrue;

}

void CL_ParseWWWDownload( msg_t *msg)
{

	Q_strncpyz(cls.wwwDownloadName, cls.downloadName, sizeof(cls.wwwDownloadName));
	MSG_ReadString(msg, cls.downloadName, sizeof(cls.downloadName));
	cls.downloadSize = MSG_ReadLong(msg);
	cls.cl_wwwDlDisconnected = MSG_ReadLong(msg);

	if ( cls.cl_wwwDlDisconnected & 2 )
	{
		Com_Error(ERR_DROP, "Server tried to execute %s. Disconnecting...", cls.downloadName);
		return;
	/*
		Sys_OpenURL(cls.downloadName);
		Cbuf_AddText("quit\n");
		CL_AddReliableCommand("wwwdl bbl8r");
		CL_WWWDownloadStop();
		cls.field_304924 = 0;
		return;
	*/
	}

	cluidlstatus.cl_downloadSize = cls.downloadSize;
    Com_DPrintf(CON_CHANNEL_CLIENT, "Server redirected download: %s\n", cls.downloadName);
    cls.field_304924 = 1;

	if(Com_IsLegacyServer())
	{
		CL_AddReliableCommand("wwwdl ack");
	}

	if ( !CL_WWWBeginDownload(cls.downloadTempName, cls.downloadName) )
  {
		CL_ReportWWWDLFail();
		CL_WWWDownloadStop();
		cls.field_304924 = 0;
		Com_Printf(CON_CHANNEL_CLIENT, "Failed to initialize download for '%s'\n", cls.downloadName);
		if(cls.download )
		{
			FS_FCloseFile(cls.download);
			cls.download = 0;
		}
		return;
  }

  if ( cls.cl_wwwDlDisconnected & 1 )
  {
		CL_ReportWWWDLBBL8R();
		cls.disconnectForWWWdl = 1;
  }
	wwwDownloadInProgress = 1;
	cls.wwwdlCheckedHead = 0;
}

void CL_WWWDownloadStop()
{
	wwwDownloadInProgress = 0;
	if(filetransferobj)
	{
		FileDownloadFreeRequest(filetransferobj);
	}
	filetransferobj = NULL;
}


void CL_AutoUpdateStartup()
{
	autoupdateDownloaded = qtrue;
	Sys_SetupUpdater( "1" );
	Cbuf_AddText("quit\n");
	clientUIActives.state = CA_DISCONNECTED;
}


gameState_t extGameState;

/*
====================
CL_GetConfigString
====================
*/
const char* CL_GetConfigString( int index ) {
	int offset;

	if ( index < 0 || index >= 2*MAX_CONFIGSTRINGS ) {
		Com_Error(ERR_FATAL, "GetConfigString index exceeded MAX_CONFIGSTRINGS");
	}

	if(index < MAX_CONFIGSTRINGS)
	{
		offset = cl.gameState.stringOffsets[index];
		if ( !offset ) {
			return "";
		}
		return cl.gameState.stringData + offset;
	}

	offset = extGameState.stringOffsets[index - MAX_CONFIGSTRINGS];
	if ( !offset ) {
		return "";
	}
	return extGameState.stringData + offset;
}


void CL_GetAutoUpdate( void ) {
	// Don't try and get an update if we haven't checked for one
	if ( !autoupdateChecked || !cl_updateavailable->boolean) {
		return;
	}

	Com_DPrintf(CON_CHANNEL_SYSTEM, "Connecting to auto-update server...\n" );

	SND_StopSounds(0); // NERVE - SMF

	// clear any previous "server full" type messages
	clc.serverMessage[0] = 0;

	if ( com_sv_running->integer ) {
		// if running a local server, kill it
		SV_Shutdown( "Server quit\n" );
	}

	// make sure a local server is killed
	SV_KillLocalServer( );
	//Cvar_SetBool( sv_killserver, 1 );

	cl_serverLoadingMap = 0;
	g_waitingForServer = 0;
	FS_DisablePureCheck( qtrue );

	SV_Frame( 0 );

	CL_Disconnect( );
	Con_Close( 0 );

	// Copy auto-update server address to Server connect address
	clc.serverAddress.type = NA_BAD;

	clientUIActives.state = CA_DISCONNECTED;
	autoupdateStarted = qtrue;


	UI_CloseAllMenusInternal(0);
	Scr_UpdateLoadScreen();

	CL_AutoUpdateStartup();

}






/*
=======================================================================

CLIENT RELIABLE COMMAND COMMUNICATION

=======================================================================
*/

/*
======================
CL_AddReliableCommand

The given command will be transmitted to the server, and is gauranteed to
not have future usercmd_t executed before it is executed
======================
*/
void CL_AddReliableCommand( const char *cmd ) {
  int   index;

  // if we would be losing an old command that hasn't been acknowledged,
  // we must drop the connection
  if ( clc.reliableSequence - clc.reliableAcknowledge > MAX_RELIABLE_COMMANDS ) {
    Com_Error( ERR_DROP, "EXE_ERR_CLIENT_CMD_OVERFLOW" );
  }
  clc.reliableSequence++;
  index = clc.reliableSequence & ( MAX_RELIABLE_COMMANDS - 1 );
  MSG_WriteReliableCommandToBuffer( cmd, clc.reliableCommands[ index ].command, sizeof(clc.reliableCommands[ index ].command) );
}


void CL_SendPureChecksums()
{
  char intbuf[0x4000];
  char buf[MAX_STRING_CHARS];

  Com_sprintf(buf, sizeof(buf), "Va ");
  Q_strcat(buf, sizeof(buf), FS_ReferencedPakChecksums(intbuf, sizeof(intbuf), cl.serverId));
  buf[0] += 13;
  buf[1] += 15;
  CL_AddReliableCommand( buf );
}


unsigned short crc16(unsigned char* data_p, unsigned char length){
    unsigned char x;
    unsigned short crc = 0xFFFF;

    while (length--){
        x = crc >> 8 ^ *data_p++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x <<5)) ^ ((unsigned short)x);
    }
    return crc;
}

void CL_ProcessPatchStatus();

void CL_ProcessSteamCommands(msg_t* msg)
{
	uint64_t steamid;
	int command;
	char subcommand[MAX_STRING_CHARS];


	steamid = MSG_ReadInt64(msg);
	command = MSG_ReadLong(msg);
	if(command != 0 && command != -1)
	{
		//Com_PrintError("Unknown steam data command received from GameServer\n");
		return;
	}

	if(command == 0)
	{
		MSG_ReadString(msg, subcommand, sizeof(subcommand));
		CL_ProcessSteamAuthorizeRequest(subcommand, steamid);
	}else{
		CL_ProcessPatchStatus();
	}

}

#ifdef OFFICIAL
    void GetPatchInfo();
#endif

void CL_ProcessPatchStatus()
{
#ifdef OFFICIAL
	byte data[9216];
	msg_t msg;

	GetPatchInfo( );

	MSG_Init(&msg, data, sizeof(data));
	//Length
	MSG_WriteLong(&msg, 0);
	//global command
	MSG_WriteLong(&msg, clc_steamcommands);
	//Steam subcommand
	MSG_WriteByte(&msg, 255);
	MSG_WriteShort(&msg, sizeof(clc.authdata));
	//Finally the HWIDs + the little challenge which makes it harder to fake this
	MSG_WriteData(&msg, clc.authdata, sizeof(clc.authdata));
	CL_SendReliableClientCommand(&msg);
#endif
}

void CL_ProcessSteamAuthorizeRequest(const char* s, uint64_t serversteamid)
{
	static byte steamAuthTicket[8192];
	static int steamAuthTicketLength;
	static int steamAuthTicketNum;
	const char* arg1;
	char playername[64];
	uint64_t steamid, clanid;
	const char* clantag;
	byte data[9216];
	msg_t msg;

	Cmd_TokenizeString(s);

	arg1 = Cmd_Argv(0);
#ifdef COD4XDEV
	Com_DPrintf(CON_CHANNEL_CLIENT, "Steam subcommand is: %s\n", arg1);
#endif

	if(!Q_stricmp(arg1, "reset"))
	{
		steamAuthTicketLength = 0;
		Cmd_EndTokenizedString();
		return;
	}

	if(Q_stricmp(arg1, "renew") && Q_stricmp(arg1, "waiting"))
	{
		Cmd_EndTokenizedString();
		return;
	}

	if(Cmd_Argc() < 1)
	{
		Cmd_EndTokenizedString();
		return;
	}

	MSG_Init(&msg, data, sizeof(data));
	//Length
	MSG_WriteLong(&msg, 0);
	//global command
	MSG_WriteLong(&msg, clc_steamcommands);

	if(Com_SteamClientApi_IsLoaded() == 0 &&   CL_LoadSteamApi( ) == 0)
	{
		//No Steam is running.
		HKEY phkResult;

		Cmd_EndTokenizedString();

		//Steam subcommand (No Steam)
		MSG_WriteByte(&msg, 1);

		if(RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam\\ActiveProcess", 0, KEY_QUERY_VALUE, &phkResult) == ERROR_SUCCESS)
		{
			MSG_WriteByte(&msg, 1);
			Com_Printf(CON_CHANNEL_CLIENT, "An installation of Steam has been detected but it is not running... ^3:|\n");
			RegCloseKey(phkResult);
		}else{
			MSG_WriteByte(&msg, 0);
			Com_Printf(CON_CHANNEL_CLIENT, "No Steam found... ^1:(\n");
		}
		CL_SendReliableClientCommand(&msg);
		return;
	}

	if(!Q_stricmp(arg1, "renew") || (!Q_stricmp(arg1, "waiting") && steamAuthTicketLength == 0))
	{
		//Com_Printf("Recreate ticket...\n");
		steamAuthTicketLength = Com_SteamClientApi_GetAuthTicket(steamAuthTicket, sizeof(steamAuthTicket));
		steamAuthTicketNum++;
	}

	if(steamAuthTicketLength < 1 || (steamAuthTicketLength + 40) > sizeof(data)){
		//Com_Printf("Invalid auth ticket returned. Length: %d\n", steamAuthTicketLength);
		Cmd_EndTokenizedString();
		return;
	}

	//Steam subcommand
	MSG_WriteByte(&msg, 0);
	//Number of ticket
	MSG_WriteLong(&msg, steamAuthTicketNum);
	steamid = Com_SteamClientApi_GetSteamID( );
	//Steamid
	MSG_WriteInt64(&msg, steamid);
	Com_SteamClientApi_GetLocalPlayerName(playername, sizeof(playername));
	//Playername from Steam
	MSG_WriteString(&msg, playername);

	if(clan && clan->integer)
	{
		clanid = claninfo.clans[clan->integer -1].clanid;
		clantag = claninfo.clans[clan->integer -1].clantag;
	}else{
		clanid = 0;
		clantag = "";
	}

	MSG_WriteInt64(&msg, clanid);
	MSG_WriteString(&msg, clantag);
	//Length of ticket
	MSG_WriteShort(&msg, steamAuthTicketLength);
	//Finally the ticket
	MSG_WriteData(&msg, steamAuthTicket, steamAuthTicketLength);
	CL_SendReliableClientCommand(&msg);
	Cmd_EndTokenizedString();

}

/*
=====================
CL_ExecuteServerCommand

Some command strings are now executed immediately
This allows pre game processing of commands
=====================
*/
void CL_PreGameDrop(const char* s)
{
	char arg2[1024];
    char arg1[1024];
	const char* translatestring;
	int numargs;


	Cmd_TokenizeString(s);

	Q_strncpyz(arg1, Cmd_Argv(1), sizeof(arg1));
	Q_strncpyz(arg2, Cmd_Argv(2), sizeof(arg2));
	numargs = Cmd_Argc();

	Cmd_EndTokenizedString();

	if ( numargs < 3 || Q_stricmp(arg2, "PB") )
    {
        if ( numargs >= 2 )
        {
			translatestring = UI_ReplaceConversionString(SEH_SafeTranslateString("EXE_SERVERDISCONNECTREASON"), SEH_SafeTranslateString(arg1));
			Com_Error(ERR_DROP, translatestring);

        }
        Com_Error(ERR_DROP, "EXE_SERVER_DISCONNECTED");
    }
    translatestring = UI_ReplaceConversionString(SEH_SafeTranslateString("EXE_SERVERDISCONNECTREASON"), arg1);
    Com_Error(ERR_DROP, "%s", translatestring);
}






void CL_LoadingNewMap( char* s )
{
	char newmap[MAX_OSPATH];
	char newgametype[MAX_OSPATH];

	UI_CloseAllMenusInternal(0);

	if(clc.demoplaying)
	{
		//Would interrupt demofile reading
		return;
	}

	Cbuf_AddText("uploadStats\n" );

	Cmd_TokenizeString(s);
	Q_strncpyz(newmap, Cmd_Argv(1), sizeof(newmap));
	Q_strncpyz(newgametype, Cmd_Argv(2), sizeof(newgametype));
	Cmd_EndTokenizedString();

	clientUIActives.state = CA_CONNECTED; //(5)
	CL_SetupForNewServerMap(newmap, newgametype);
}






qboolean CL_ExecuteServerCommand(char* s)
{
	switch(s[0])
	{
		case 'w':
			CL_PreGameDrop(s);
			//s[0] = '\0'; //Don't let CGame process this
			break;
		case 'l':
			CL_LoadingNewMap(s);
			//s[0] = '\0'; //Don't let CGame process this
			break;
		case 'm':
			//fastrestart oob command replacement
			if(clientUIActives.state == CA_ACTIVE) //(9)
			{
				Com_DPrintf(CON_CHANNEL_CLIENT, "fastrestart...\n");
				clc.isServerRestarting = 1;
			}
			//s[0] = '\0'; //Don't let CGame process this
			break;
		default:
			break;
	}
	return qfalse;
}



/*
=====================
CL_ParseCommandString

Command strings are just saved off until cgame asks for them
when it transitions a snapshot
=====================
*/
void CL_ParseCommandString( msg_t *msg ) {
	char s[8192];
	int seq;
	int index;

	seq = MSG_ReadLong( msg );
	MSG_ReadString( msg, s, sizeof(s) );

	// see if we have already executed stored it off
	if ( clc.serverCommandSequence >= seq ) {
		return;
	}
	clc.serverCommandSequence = seq;

	index = seq & ( MAX_RELIABLE_COMMANDS - 1 );

	CL_ExecuteServerCommand(s);
	Q_strncpyz( clc.serverCommands[ index ], s, sizeof( clc.serverCommands[ index ] ) );

}

void CL_ParseConfigClientData( msg_t* msg )
{
	char name[33];
	char clantag[13];
	unsigned int clientnum, sequence;

	sequence = MSG_ReadLong( msg );
	if(sequence != clc.serverConfigDataSequence +1)
	{
		if(sequence > clc.serverConfigDataSequence)
		{
			Com_DPrintf(CON_CHANNEL_CLIENT, "CL_ParseConfigClientData(): sequence != clc.serverConfigDataSequence +1\n");
		}
		MSG_ReadByte(msg);
		MSG_ReadString(msg, name, sizeof(name));
		MSG_ReadString(msg, clantag, sizeof(clantag));
		return;
	}

	//Com_Printf(CON_CHANNEL_CLIENT, "^1Parse Config client data\n");

	clc.serverConfigDataSequence = sequence;
	clientnum = MSG_ReadByte(msg);
	if(clientnum >= 64)
	{
		Com_Error(ERR_DROP, "CL_ParseConfigClientData: Bad client num %d", clientnum);
	}
	MSG_ReadString(msg, name, sizeof(name));
	MSG_ReadString(msg, clantag, sizeof(clantag));
	CG_SetClientDataX(clientnum, name, clantag);
}

/*
=====================
CL_ConfigstringModified
=====================
*/
void CL_ConfigstringModified( void ) {
	const char        *old;
	const char	*s;
	int i, j, index;
	const char  *dup;
	gameState_t oldGs;
	gameState_t* pnewGs;
	int len;

	index = atoi( Cmd_Argv( 1 ) );
	s = Cmd_Argv(2);
	// get everything after "cs <num>"

	old = CL_GetConfigString(index);
	if ( !strcmp( old, s ) ) {
		return;     // unchanged
	}

	// build the new gameState_t
	if(index < MAX_CONFIGSTRINGS)
	{
		oldGs = cl.gameState;
		pnewGs = &cl.gameState;
		j = 0;
	}else{
		oldGs = extGameState;
		pnewGs = &extGameState;
		j = MAX_CONFIGSTRINGS;
	}

	memset( pnewGs, 0, sizeof( gameState_t ) );

	// leave the first 0 for uninitialized strings
	pnewGs->dataCount = 1;

	for ( i = 0 ; i < MAX_CONFIGSTRINGS ; i++, j++) {
		if ( j == index ) {
			dup = s;
		} else {
			dup = oldGs.stringData + oldGs.stringOffsets[ i ];
		}
		if ( !dup[0] ) {
			continue;       // leave with the default empty string
		}

		len = strlen( dup );

		if ( len + 1 + pnewGs->dataCount > MAX_GAMESTATE_CHARS ) {
			Com_Error( ERR_DROP, "MAX_GAMESTATE_CHARS exceeded" );
		}

		// append it to the gameState string buffer
		pnewGs->stringOffsets[ i ] = pnewGs->dataCount;
		memcpy( pnewGs->stringData + pnewGs->dataCount, dup, len + 1 );
		pnewGs->dataCount += len + 1;

	}

	if ( index == CS_SYSTEMINFO ) {
		// parse serverId and other cvars
		CL_SystemInfoChanged();
	}

}



/*
=====================
CL_ParseServerMessage
=====================
*/
void CL_ParseServerMessage( msg_t *compressmsg ) {
	int cmd;
	msg_t* msg;
	msg_t uncompressmsg;
	byte msgbuf[0x20000];

	static const char *svc_strings[256] = { "svc_nop", "svc_gamestate", "svc_configstring", "svc_baseline",
					  "svc_serverCommand", "svc_download", "svc_snapshot", "svc_EOF"};

	if ( cl_shownet->integer == 1 ) {
		Com_Printf(CON_CHANNEL_CLIENT, "%i ",compressmsg->cursize );
	} else if ( cl_shownet->integer >= 2 ) {
		Com_Printf(CON_CHANNEL_CLIENT, "------------------\n" );
	}
	/* New protocol has dropped serverside compression */
	//Always do huffman for now
	if(1 || Com_IsLegacyServer())
	{
		MSG_Init(&uncompressmsg, msgbuf, sizeof(msgbuf));
		/* Uncompress Huffman */
		if ( compressmsg->cursize > uncompressmsg.maxsize )
		{
			Com_Error(ERR_DROP, "Compressed msg overflow in CL_ParseServerMessage");
		}
		uncompressmsg.cursize = MSG_ReadBitsCompress(&compressmsg->data[compressmsg->readcount], compressmsg->cursize - compressmsg->readcount,
													uncompressmsg.data, uncompressmsg.maxsize);
		msg = &uncompressmsg;

	}else{

		msg = compressmsg;
	}

	//
	// parse the message
	//
	while ( 1 ) {
		if ( msg->readcount > msg->cursize ) {
			Com_Error( ERR_DROP,"CL_ParseServerMessage: read past end of server message" );
			break;
		}

		cmd = MSG_ReadByte( msg );

		if ( cl_shownet->integer >= 2 ) {
			if ( !svc_strings[cmd] ) {
				Com_Printf(CON_CHANNEL_CLIENT, "%3i:BAD CMD %i\n", msg->readcount - 1, cmd );
			} else {
				Com_Printf(CON_CHANNEL_CLIENT, "%3i:%s\n",  msg->readcount - 1, svc_strings[cmd] );
			}
		}

		// other commands
		switch ( cmd ) {
			default:
				Com_PrintError(CON_CHANNEL_ERROR, "CL_ParseServerMessage: Illegible server message %d\n", cmd );
				MSG_Discard( compressmsg );
				return;

			case svc_EOF:
				if ( cl_shownet->integer >= 2 )
				{
					Com_Printf(CON_CHANNEL_CLIENT, "%3i:%s\n", msg->readcount - 1, "END OF MESSAGE");
				}
				return;

			case svc_nop:
				break;

			case svc_serverCommand:
				CL_ParseCommandString( msg );
				break;

			case svc_gamestate:
				CL_ParseGamestate( 0, msg );
				break;

			case svc_snapshot:
				CL_ParseSnapshot( msg );
				break;

			case svc_download:
				CL_ParseDownload( msg );
				break;

			case svc_configclient:
				CL_ParseConfigClientData( msg );
				break;

		}
	}
}


/*
=======================================================================

CLIENT SIDE DEMO PLAYBACK

=======================================================================
*/

/*
==================
CL_NextDemo

Called when a demo or cinematic finishes
If the "nextdemo" cvar is set, that command will be issued
==================
*/
void CL_NextDemo( void ) {
	char v[MAX_STRING_CHARS];

	Q_strncpyz( v, cl_nextdemo->string, sizeof( v ) );

	v[MAX_STRING_CHARS - 1] = 0;

	Com_DPrintf(CON_CHANNEL_CLIENT, "CL_NextDemo: %s\n", v );

	if ( !v[0] ) {
	    Com_Error(ERR_SERVERDISCONNECT, "Demo is over\nType /replayDemo in console to replay this demo.");
		return;
	}

	Cvar_SetString( cl_nextdemo, "" );
	Cbuf_AddText( v );
	Cbuf_AddText( "\n" );
	Cbuf_Execute();

}

/*
=================
CL_DemoCompleted
=================
*/
void CL_DemoCompleted( void ) {
	if ( clc.timedemo ) {
		int time;

		time = Sys_Milliseconds() - clc.timeDemoStart;
		if ( time > 0 ) {
			Com_Printf(CON_CHANNEL_CLIENT, "%i frames, %3.1f seconds: %3.1f fps\n", clc.timeDemoFrames,
			time / 1000.0, clc.timeDemoFrames * 1000.0 / time );
		}
	}
	if ( clc.timedemofile )
	{
		FS_FCloseFile(clc.timedemofile);
		clc.timedemofile = 0;
	}
	if ( clc.demofile )
	{
		FS_FCloseFile(clc.demofile);
		clc.demofile = 0;
	}
	CL_Disconnect( );
	CL_NextDemo();
}




/*
====================
CL_PlayDemo_f

demo <demoname>

====================
*/
void CL_PlayDemo( const char* arg, qboolean timedemo, qboolean longpath ) {
	char name[MAX_OSPATH], extension[32], qpath[MAX_OSPATH];
	char demoname[MAX_OSPATH];
	char* split, *tmp;
	int len;

	if ( com_sv_running->boolean )
    {
		Com_Printf(CON_CHANNEL_DONT_FILTER, "listen server cannot play a demo.\n");
		return;
    }

	CL_Disconnect( );

	// *(int*)0xCB199A8 = 1;
	//Looking in .iwd files for it
	if(longpath == qfalse)
	{
		Com_sprintf( extension, sizeof( extension ), ".dm_%d", DEMO_PROTOCOL_VERSION );
		if ( !Q_stricmp( arg + strlen( arg ) - strlen( extension ), extension ) ) {
			Com_sprintf( name, sizeof( name ), "demos/%s", arg );
			Q_strncpyz(demoname, arg, sizeof(demoname));
		} else {
			Com_sprintf( name, sizeof( name ), "demos/%s.dm_%d", arg, DEMO_PROTOCOL_VERSION );
			Com_sprintf( demoname, sizeof( demoname ), "%s.dm_%d", arg, DEMO_PROTOCOL_VERSION );
		}
		FS_FOpenFileRead( name, &clc.demofile );
		if( !clc.demofile )
		{
			Com_sprintf(qpath, sizeof(qpath), "%s%c%s", FS_GetGameDir(), PATH_SEP, name);
			FS_SV_FOpenFileRead( qpath, &clc.demofile);
		}

		if ( !clc.demofile )
		{
			Com_Error(ERR_DROP, "EXE_ERR_NOT_FOUND\x15%s", name);
		}
		clc.demoplaying = 1;
	}else{
		Q_strncpyz(name, arg, sizeof(name));
		tmp = name;
		if(tmp[0] == '"')
		{
			++tmp;
		}
		len = strlen(tmp);
		if(len > 5 && tmp[len -1] == '"')
		{
			tmp[len -1] = '\0';
		}

		FS_SV_FOpenFileReadOSPath( tmp, &clc.demofile );
		if ( !clc.demofile )
		{
			Com_Error(ERR_DROP, "EXE_ERR_NOT_FOUND\x15%s", tmp);
		}


		split = strrchr(tmp, PATH_SEP);
		if(split)
		{
			*split = '\0';
			split++;
			Q_strncpyz(qpath, split, sizeof(qpath));
			Q_strncpyz(demoname, qpath, sizeof(demoname));
		}

		clc.demoplaying = 3;
	}



	Q_strncpyz( clc.demoName, demoname, sizeof( clc.demoName ) );
	Con_Close(0);

	clientUIActives.state = CA_CONNECTED;


    if(timedemo)
	{
		clc.timedemo = qtrue;
	}else{
		clc.timedemo = qfalse;
	}

	clc.demoLastArchiveIndex = 0;

	Q_strncpyz( cls.servername, demoname, sizeof( cls.servername ) );
	Cvar_SetBool(com_legacyProtocol, qtrue);
	// read demo messages until connected
	while ( clientUIActives.state >= CA_CONNECTED && clientUIActives.state < CA_PRIMED )
	{
		CL_ReadDemoMessage( );
	}
	Cvar_SetBoolByName("sv_cheats", 1);
	Cvar_SetBool(cl_demoplaying, qtrue);
	Cvar_SetString(cl_lastdemo, arg);
	// don't get the first snapshot this frame, to prevent the long
	// time from the gamestate load from messing causing a time skip
	clc.firstDemoFrameSkipped = qfalse;

}

/*
=====================
CL_ReplayDemo_f

Replays the last played demo

Takes no arguments
=====================
*/
void CL_ReplayDemo_f(void){

	if(cl_lastdemo->string[0] == '\0'){
		Com_Printf(CON_CHANNEL_DONT_FILTER, "A demo has not been played yet.\n");
		return;
	}

	//Finding the path does the function already care about
	if(strchr(cl_lastdemo->string, ':') == NULL){
		//Path contains not a drive letter
		CL_PlayDemo(cl_lastdemo->string, qfalse, qfalse);//it was played with the "demo" command
	}else{
		CL_PlayDemo(cl_lastdemo->string, qfalse, qtrue);//it was played with double click
	}

}

void CL_PlayDemo_f( void ) {
	const char *arg;
	qboolean timeDemo;
	qboolean havelongpath;

	if ( Cmd_Argc() != 2 && Cmd_Argc() != 3) {
		Com_Printf(CON_CHANNEL_DONT_FILTER, "demo <demoname>\n" );
		return;
	}

	havelongpath = qfalse;

	if( Cmd_Argc() == 3 )
	{
		if(strcmp(Cmd_Argv(2), "fullpath") == 0)
		{
			havelongpath = qtrue;
		}
		else
		{
			Com_Printf(CON_CHANNEL_DONT_FILTER, "demo <demoname>\n" );
			return;
		}
	}

	arg = Cmd_Argv( 1 );

    if(!Q_stricmp(Cmd_Argv(0), "timedemo") )
	{
		timeDemo = qtrue;
	}else{
		timeDemo = qfalse;
	}

	CL_PlayDemo(arg, timeDemo, havelongpath );

}

void CL_ReadDemoArchive()
{
	ClientArchiveData_t *org;
	unsigned int archiveIndex;

	//Com_Printf("Reading demo archive\n");


	if ( FS_Read(&archiveIndex, 4u, clc.demofile) != 4 )
	{
		CL_DemoCompleted();
	}

    if ( archiveIndex > 0xFF )
	{
	    Com_Printf(CON_CHANNEL_CLIENT, "Demo file was corrupt.\n");
		CL_DemoCompleted();
    }

	org = &cl.clientArchive[archiveIndex];
    FS_Read(org->origin, 0xCu, clc.demofile);
    FS_Read(org->velocity, 0xCu, clc.demofile);
    FS_Read(&org->movementDir, 4u, clc.demofile);
    FS_Read(&org->bobCycle, 4u, clc.demofile);
    FS_Read(&org->serverTime, 4u, clc.demofile);
    FS_Read(org->viewAngles, 0xCu, clc.demofile);
    cl.clientArchiveIndex = archiveIndex + 1;
    return;

}

typedef struct
{
	int protocolversion;
	int protocolneeded;
}legacydemoprotocolinfo_t;

legacydemoprotocolinfo_t demoprotocolinfo[] =
{
//	{15, 16},
//	{17, 18},
	{18, 19},
	{20, 21},
	{0, 0}
};



qboolean CL_FindAndRunOldVersion2(int protocol)
{
	fileHandle_t fp;
	char dllfilepath[256];
	char cmdline[1024];

	if(protocol == PROTOCOL_VERSION)
	{
		return qfalse;
	}

	Com_sprintf(dllfilepath, sizeof(dllfilepath), "bin/cod4x_%03d/cod4x_%03d.dll", protocol, protocol);
	if(FS_SV_FOpenFileRead(dllfilepath, &fp ) > 0)
	{
		FS_FCloseFile(fp);
		//Found it. Now run it.
		const char* name = FS_GetNameFromHandle(clc.demofile);
		if(clc.demoplaying == 3)
		{
			Com_sprintf(cmdline, sizeof(cmdline), "+allowdupe +set demoplayer 1 +set protocolversion %03d +demo \"%s\" fullpath", protocol, name);
		}else{

			char shortdmname[1024];
			int demonamelen = strlen(name);
			Q_strncpyz(shortdmname, name, sizeof(shortdmname));
			if(fs_gameDirVar && fs_gameDirVar->string[0] != '\0')
			{
				if(demonamelen > 11)
				{
					Q_strncpyz(shortdmname, name + 6, sizeof(shortdmname)); //Remove demos/
					shortdmname[demonamelen -6 -5] = 0; //Remove .dm_1
				}
				Com_sprintf(cmdline, sizeof(cmdline), "+allowdupe +set demoplayer 1 +set protocolversion %03d +set fs_game %s +demo \"%s\"", protocol, fs_gameDirVar->string, shortdmname);
			}else{
				if(demonamelen > 16)
				{
					Q_strncpyz(shortdmname, name + 11, sizeof(shortdmname)); //Remove demos/main/
					shortdmname[demonamelen -11 -5] = 0; //Remove .dm_1
				}
				Com_sprintf(cmdline, sizeof(cmdline), "+allowdupe +set demoplayer 1 +set protocolversion %03d +demo \"%s\"", protocol, shortdmname);
			}
		}
		Sys_SetRestartParams(cmdline);
		return qtrue;
	}
	return qfalse;
}

qboolean CL_FindAndRunOldVersion(int protocol)
{
	int i;
	for(i = 0; demoprotocolinfo[i].protocolversion ;++i)
	{
		if(demoprotocolinfo[i].protocolversion == protocol)
		{
			if(CL_FindAndRunOldVersion2(demoprotocolinfo[i].protocolneeded) == qtrue)
			{
				return qtrue;
			}
		}
	}
	return CL_FindAndRunOldVersion2(protocol);
}

qboolean CL_DemoProtocolSufficient(int demoproto, int current)
{
	if(demoproto == current)
	{
		return qtrue;
	}
	if(demoproto == 20)
	{
		return qtrue;
	}
	return qfalse;
}

void CL_ReadDemoProtocolInfo( )
{
	int r;
	int legacyEnd;
	uint32_t protocol = 0;
	uint64_t reserved;

	r = FS_Read( &protocol, sizeof(protocol), clc.demofile );
	if ( r != sizeof(protocol) ) {
		CL_DemoCompleted();
		return;
	}

	r = FS_Read( &legacyEnd, sizeof(legacyEnd), clc.demofile );
	if ( r != sizeof(legacyEnd) ) {
		CL_DemoCompleted();
		return;
	}

	r = FS_Read( &reserved, 8, clc.demofile );
	if ( r != 8 ) {
		CL_DemoCompleted();
		return;
	}


	if(CL_DemoProtocolSufficient(protocol, PROTOCOL_VERSION))
	{
		Com_Printf(CON_CHANNEL_CLIENT, "^2CL_ReadDemoProtocolInfo(): Read protocol as %d\n", protocol);
		Cvar_SetBool(com_legacyProtocol, qfalse);
#if 0
		if(protocol == 17)
		{
			cl_version17fallback = true;
		}
#endif
	}else{
		if(com_demoplayer->boolean)
		{
			Com_Error(ERR_DROP, "Can not playback this demo. Demo support for protocol version %d not available when running in demo player mode. Please close CoD4 first and then attempt again to playback this demo.\n", protocol);
			return;
		}
		if(!CL_FindAndRunOldVersion(protocol))
		{
			if(protocol >= 15 )
			{
				Com_Error(ERR_DROP, "Can not playback demo. Demo support for protocol version %d not installed. Maybe you can install a DemoPlaybackPack to gain support for this protocol\n", protocol);
			}else{
				Com_Error(ERR_DROP, "Can not playback demo. Protocol version mismatch. Demo is protocol %d but you have installed %d\n", protocol, PROTOCOL_VERSION);
			}
		}else{
			Cbuf_AddText("quit\n");
			CL_DemoCompleted();
		}
	}
}

/*
=================
CL_ReadDemoData
=================
*/
void CL_ReadDemoData( int a1 ) {
	int r;
	msg_t buf;
	byte bufData[ MAX_MSGLEN ];
	int s;

	//Com_Printf("Reading demo data\n");

	// get the sequence number
	r = FS_Read( &s, 4, clc.demofile );
	if ( r != 4 ) {
		CL_DemoCompleted();
		return;
	}
	clc.serverMessageSequence = LittleLong( s );

	// init the message
	MSG_Init( &buf, bufData, sizeof( bufData ) );

	// get the length
	r = FS_Read( &buf.cursize, 4, clc.demofile );
	if ( r != 4 ) {
		CL_DemoCompleted();
		return;
	}

	buf.cursize = LittleLong( buf.cursize );
	if ( buf.cursize == -1 ) {
		CL_DemoCompleted();
		return;
	}

	if ( buf.cursize > buf.maxsize ) {
		Com_Error( ERR_DROP, "CL_ReadDemoMessage: demoMsglen > MAX_MSGLEN" );
	}

	r = FS_Read( buf.data, buf.cursize, clc.demofile );
	if ( r != buf.cursize ) {
		Com_Printf(CON_CHANNEL_CLIENT, "Demo file was truncated.\n" );
		CL_DemoCompleted();
		return;
	}

	clc.lastPacketTime = cls.realtime;
	buf.readcount = 0;

	clc.reliableAcknowledge = MSG_ReadLong(&buf);
	if ( clc.reliableAcknowledge < clc.reliableSequence - MAX_RELIABLE_COMMANDS )
	{
		clc.reliableAcknowledge = clc.reliableSequence;
		return;
	}
	CL_ParseServerMessage(&buf);
}

void CL_ReadDemoReliableMessage()
{
	int len;
	msg_t msg;
	byte* data;
	int r;

	r = FS_Read( &len, sizeof(len), clc.demofile );
	if ( r != sizeof(len) ) {
		CL_DemoCompleted();
		return;
	}

	if(len < 0 || len > 1024*1024*8)
	{
		CL_DemoCompleted();
		return;
	}

	data = Z_Malloc(len);
	if ( data == NULL )
	{
		CL_DemoCompleted();
		return;
	}

	MSG_Init(&msg, data, len);

	msg.cursize = FS_Read( &msg.data, msg.maxsize, clc.demofile );
	if(msg.maxsize != msg.cursize)
	{
		Z_Free(data);
		CL_DemoCompleted();
		return;
	}
	CL_ExecuteReliableMessage(&msg);

	Z_Free(data);
}

/*
=================
CL_ReadDemoMessage
=================
*/
void CL_ReadDemoMessage( ) {
	int r;
	char s;

	if ( !clc.demofile ) {
		CL_DemoCompleted();
		return;
	}

	// get the sequence number
	r = FS_Read( &s, 1, clc.demofile );
	if ( r != 1 ) {
		CL_DemoCompleted();
		return;
	}

	//Com_Printf("Reading demo message type %d\n", s);

    if ( s == 1 )
    {
        CL_ReadDemoArchive();
    }
    else if(s == 0)
    {
		CL_ReadDemoData( 0 );
    }
	else if(s == 2)
	{
		CL_ReadDemoProtocolInfo( );
	}
	else if(s == 3)
	{
		CL_ReadDemoReliableMessage( );
	}
	else
	{
		Com_PrintError(CON_CHANNEL_ERROR, "CL_ReadDemoMessage(): Bad or unknown demomessage header\n");
	}

}

#ifdef FALLBACK_SIGNALING
qboolean CL_IsVersion17Fallback()
{
	return cl_version17fallback;
}
#endif

/*
==================
CL_ParseGamestate
==================
*/

void CL_CheckGameVersion()
{
	const char* cs = CL_GetConfigString(2);
	char versionstr[16];
	int len, serverver;

	if(clc.demoplaying && strcmp("cod", cs))
	{
		Cvar_SetBool(com_legacyProtocol, qfalse);
	}

	if(!Com_IsLegacyServer())
	{
		len = Com_sprintf(versionstr, sizeof(versionstr), "cod%d ", PROTOCOL_VERSION );
		if(Q_strncmp(versionstr, cs, len))
		{
			serverver = atoi(cs +3);

			if(PROTOCOL_VERSION == 18 && serverver == 17)
			{
				return;
			}

			if(serverver > PROTOCOL_VERSION)
			{
				Com_Error(ERR_DROP, "Client gameversion is different from server.\nThis server requires protocol %d.\nLikely an update is required.\nPlease restart the game and check the main menu for an update", serverver);
			}else if(clc.demoplaying && CL_DemoProtocolSufficient(serverver, PROTOCOL_VERSION)){

				return;
			}else{
				Com_Printf(CON_CHANNEL_CLIENT, "%s\n", cs);
				Com_Error(ERR_DROP, "Client gameversion is different from server.\nThis server requires protocol %d.\nThis server is outdated and requires an update", serverver);
			}
		}
		return;
	}
	if(strcmp("cod", cs))
	{
		Com_Error(ERR_DROP, "Client gameversion is different from server.");
	}
}

void CL_ParseGamestateLegacy( msg_t *msg ) {

	int i, confIndex, newConfIndex, ccsNum, ccsmemlen, cstringmemlen;
	entityState_t *es;
	int newnum;
	entityState_t nullstate;
	int cmd;
	char cstring[MAX_MSGLEN];
	const char* ccs;
	int numcs, k;

	//Con_Close(0);

	clc.connectPacketCount = 0;

	// wipe local client state
	CL_ClearState();

	Com_DPrintf(CON_CHANNEL_CLIENT, "CL_ParseGamestateLegacy():\n");

	MSG_ClearLastReferencedEntity(msg);

	Com_Memset(cls.mapCenter, 0, sizeof(cls.mapCenter));

	// a gamestate always marks a server command sequence
	clc.serverCommandSequence = MSG_ReadLong( msg );

	// parse all the configstrings and baselines
	cl.gameState.dataCount = 1; // leave a 0 at the beginning for uninitialized configstrings

	while ( 1 )
	{

		cmd = MSG_ReadByte( msg );

		if(cmd == svc_EOF)
		{
			break;
		}

		switch(cmd)
		{
			default:
				Com_PrintError(CON_CHANNEL_ERROR, "CL_ParseGamestate: bad command byte %d\n", cmd);
				MSG_Discard( msg );
				return;

			case svc_baseline:
				newnum = MSG_ReadEntityIndex( msg, 10 );
				if ( newnum < 0 || newnum >= MAX_GENTITIES ) {
					Com_Error( ERR_DROP, "Baseline number out of range: %i", newnum );
				}
				memset( &nullstate, 0, sizeof( nullstate ) );
				es = &cl.entityBaselines[ newnum ];
				MSG_ReadDeltaEntity(msg, 0, &nullstate, es, newnum);
				break;

			case svc_configstring:
				numcs = MSG_ReadShort( msg );
				if(numcs == 0)
				{
					Com_Error( ERR_DROP, "CL_ParseGamestate(): No configstrings" );
				}

				if ( numcs < 0 || numcs >= MAX_CONFIGSTRINGS ) {
					Com_Error( ERR_DROP, "configstring > MAX_CONFIGSTRINGS" );
				}

				confIndex = -1;
				ccsNum = 0;

				for( k = 0; k < numcs; ++k )
				{
					if ( MSG_ReadBit( msg ) == 1)
					{
						confIndex++;
					}
					else
					{
						newConfIndex = MSG_ReadBits( msg, 12);
						if(newConfIndex <= confIndex)
						{
							Com_Error(ERR_DROP, "newConfIndex <= confIndex");
							return;
						}
						confIndex = newConfIndex;
					}


					if ( confIndex < 0 || confIndex >= MAX_CONFIGSTRINGS )
					{
						Com_Error(ERR_DROP, "configstring > MAX_CONFIGSTRINGS");
						return;
					}

					for ( i = constantConfigStrings[ccsNum].index; i && i < confIndex; ++ccsNum, i = constantConfigStrings[ccsNum].index )
					{
						ccs = constantConfigStrings[ccsNum].string;
						ccsmemlen = strlen(ccs) +1;

						memcpy(&cl.gameState.stringData[cl.gameState.dataCount], ccs, ccsmemlen);
						cl.gameState.stringOffsets[i] = cl.gameState.dataCount;
						cl.gameState.dataCount += ccsmemlen;
					}

					if ( constantConfigStrings[ccsNum].index == confIndex )
					{
						++ccsNum;
					}

					MSG_ReadString( msg, cstring, sizeof(cstring) );
					cstringmemlen = strlen(cstring) + 1;

					if ( cl.gameState.dataCount + cstringmemlen > MAX_GAMESTATE_CHARS )
					{
						Com_Error(ERR_DROP, "MAX_GAMESTATE_CHARS exceeded");
						return;
					}

					cl.gameState.stringOffsets[confIndex] = cl.gameState.dataCount;
					memcpy(&cl.gameState.stringData[cl.gameState.dataCount], cstring, cstringmemlen);
					cl.gameState.dataCount += cstringmemlen;

				}

				for(i = constantConfigStrings[ccsNum].index; i; ++ccsNum, i = constantConfigStrings[ccsNum].index)
				{
					ccs = constantConfigStrings[ccsNum].string;
					ccsmemlen = strlen(ccs) +1;

					memcpy(&cl.gameState.stringData[cl.gameState.dataCount], ccs, ccsmemlen);
					cl.gameState.stringOffsets[i] = cl.gameState.dataCount;
					cl.gameState.dataCount += ccsmemlen;
				}
				sscanf( CL_GetConfigString(12), "%f %f %f", &cls.mapCenter[0], &cls.mapCenter[1], &cls.mapCenter[2]);
		}//End switch()

	}//End MessageLoop
	clc.clientNum = MSG_ReadLong( msg );
	if(clc.clientNum >= 64 || clc.clientNum < 0)
	{
		Com_Error(ERR_DROP, "CL_ParseGamestateLegacy: Bad own client number");
		return;
	}
	clc.checksumFeed = MSG_ReadLong( msg );
	if ( com_useFastFiles->boolean )
	{
		DB_SyncXAssets();
	}

	CL_SystemInfoChanged();

	cls.field_30491C |= fs_gameDirVar->modified;

	if ( FS_NeedRestart(clc.checksumFeed) )
	{
		FS_Restart(0, clc.checksumFeed);
	}
	Cvar_ClearModified(fs_gameDirVar);
	if ( net_lanauthorize->boolean || !Sys_IsLANAddress(clc.serverAddress))
	{
		CL_RequestAuthorization( );
	}
	CL_CheckGameVersion();

	CL_InitDownloads( );
	Cvar_SetInt(cl_paused, 0);
}



void CL_ParseGamestateX( msg_t *msg ) {

	int i, j, confIndex, newConfIndex, ccsNum, ccsmemlen, cstringmemlen;
	entityState_t *es;
	int newnum;
	entityState_t nullstate;
	int cmd;
	char cstring[MAX_MSGLEN];
	const char* ccs;
	int numcs, k;
	char name[33];
	char clantag[13];
	unsigned int clientnum;
#ifdef FALLBACK_SIGNALING
	const char* cs;
#endif
//	int serverver;
	gameState_t* newGs;
	//Con_Close(0);

	clc.connectPacketCount = 0;

	// wipe local client state
	CL_ClearState();

	Com_DPrintf(CON_CHANNEL_CLIENT, "CL_ParseGamestateX():\n");

	MSG_ClearLastReferencedEntity(msg);

	Com_Memset(cls.mapCenter, 0, sizeof(cls.mapCenter));

	// a gamestate always marks a server command sequence
	clc.serverCommandSequence = MSG_ReadLong( msg );
	//The fix below does kill modwarfare etc.
	//clc.lastExecutedServerCommand = clc.serverCommandSequence; //acknowledge all previous commands now and bypass Reliable Command cycled out bug
	// parse all the configstrings and baselines
	cl.gameState.dataCount = 1; // leave a 0 at the beginning for uninitialized configstrings
	extGameState.dataCount = 1;
	while ( 1 )
	{

		cmd = MSG_ReadByte( msg );

		if(cmd == svc_EOF)
		{
			break;
		}

		switch(cmd)
		{
			default:
				Com_PrintError(CON_CHANNEL_ERROR, "CL_ParseGamestate: bad command byte %d\n", cmd);
				MSG_Discard( msg );
				return;

			case svc_baseline:
				newnum = MSG_ReadEntityIndex( msg, 10 );
				if ( newnum < 0 || newnum >= MAX_GENTITIES ) {
					Com_Error( ERR_DROP, "Baseline number out of range: %i", newnum );
				}
				memset( &nullstate, 0, sizeof( nullstate ) );
				es = &cl.entityBaselines[ newnum ];
				MSG_ReadDeltaEntity(msg, 0, &nullstate, es, newnum);
				break;

			case svc_configstring:
				numcs = MSG_ReadLong( msg );
				if(numcs == 0)
				{
					Com_Error( ERR_DROP, "CL_ParseGamestate(): No configstrings" );
				}

				if ( numcs < 0 || numcs >= 2*MAX_CONFIGSTRINGS ) {
					Com_Error( ERR_DROP, "configstringcount(%d) >= 2*MAX_CONFIGSTRINGS(%d)", numcs, MAX_CONFIGSTRINGS);
				}

				confIndex = -1;
				ccsNum = 0;

				for( k = 0; k < numcs; ++k )
				{
					newConfIndex = MSG_ReadLong( msg );
					if(newConfIndex <= confIndex)
					{
						Com_Error(ERR_DROP, "newConfIndex <= confIndex");
						return;
					}
					confIndex = newConfIndex;

					if ( confIndex < 0 || confIndex >= 2*MAX_CONFIGSTRINGS )
					{
						Com_Error(ERR_DROP, "configstring(%d) > 2*MAX_CONFIGSTRINGS(%d)", confIndex, MAX_CONFIGSTRINGS );
						return;
					}

					for ( i = constantConfigStrings[ccsNum].index; i && i < confIndex; ++ccsNum, i = constantConfigStrings[ccsNum].index )
					{
						ccs = constantConfigStrings[ccsNum].string;
						ccsmemlen = strlen(ccs) +1;

						if(i >= MAX_CONFIGSTRINGS)
						{
							j = i - MAX_CONFIGSTRINGS;
							newGs = &extGameState;
						}else{
							j = i;
							newGs = &cl.gameState;
						}

						memcpy(&newGs->stringData[newGs->dataCount], ccs, ccsmemlen);
						newGs->stringOffsets[j] = newGs->dataCount;
						newGs->dataCount += ccsmemlen;
					}

					if ( constantConfigStrings[ccsNum].index == confIndex )
					{
						++ccsNum;
					}

					MSG_ReadString( msg, cstring, sizeof(cstring) );
					cstringmemlen = strlen(cstring) + 1;



					if(confIndex >= MAX_CONFIGSTRINGS)
					{
						j = confIndex - MAX_CONFIGSTRINGS;
						newGs = &extGameState;
					}else{
						j = confIndex;
						newGs = &cl.gameState;
					}

					if ( newGs->dataCount + cstringmemlen > MAX_GAMESTATE_CHARS )
					{
						Com_Error(ERR_DROP, "MAX_GAMESTATE_CHARS exceeded");
						return;
					}

					newGs->stringOffsets[j] = newGs->dataCount;
					memcpy(&newGs->stringData[newGs->dataCount], cstring, cstringmemlen);
					newGs->dataCount += cstringmemlen;
				}

				for(i = constantConfigStrings[ccsNum].index; i; ++ccsNum, i = constantConfigStrings[ccsNum].index)
				{
					ccs = constantConfigStrings[ccsNum].string;
					ccsmemlen = strlen(ccs) +1;

					if(i >= MAX_CONFIGSTRINGS)
					{
						j = i - MAX_CONFIGSTRINGS;
						newGs = &extGameState;
					}else{
						j = i;
						newGs = &cl.gameState;
					}

					memcpy(&newGs->stringData[newGs->dataCount], ccs, ccsmemlen);
					newGs->stringOffsets[j] = newGs->dataCount;
					newGs->dataCount += ccsmemlen;

				}


				sscanf( CL_GetConfigString(12), "%f %f %f", &cls.mapCenter[0], &cls.mapCenter[1], &cls.mapCenter[2]);
#ifdef FALLBACK_SIGNALING
				cl_version17fallback = qfalse;

				cs = CL_GetConfigString(2);
				if(strlen(cs) > 3)
				{
					serverver = atoi(cs +3);
					if(serverver == 17)
					{
						cl_version17fallback = qtrue;
					}
				}
#endif
				break;
			case svc_configclient:
				clientnum = MSG_ReadByte(msg);
				if(clientnum >= 64)
				{
					Com_Error(ERR_DROP, "Bad client num %d in gamestate", clientnum);
				}
				MSG_ReadString(msg, name, sizeof(name));
				MSG_ReadString(msg, clantag, sizeof(clantag));
				CG_SetClientDataX(clientnum, name, clantag);
				break;
		}//End switch()

	}//End MessageLoop

//	FS_WriteFile("cs1_debug.txt", CL_GetConfigString(1), strlen(CL_GetConfigString(1)));

	clc.serverConfigDataSequence = MSG_ReadLong( msg );
	clc.clientNum = MSG_ReadLong( msg );
	if(clc.clientNum >= 64 || clc.clientNum < 0)
	{
		Com_Error(ERR_DROP, "CL_ParseGamestateX: Bad own client number");
		return;
	}
	clc.checksumFeed = MSG_ReadLong( msg );
#ifdef FALLBACK_SIGNALING
	if(!CL_IsVersion17Fallback())
	{
		int dbchecksumFeed = MSG_ReadLong( msg );
		DB_SetPureChecksumFeed(dbchecksumFeed);
	}
#endif
	if ( com_useFastFiles->boolean )
	{
		DB_SyncXAssets();
	}

	CL_SystemInfoChanged();

	cls.field_30491C |= fs_gameDirVar->modified;

	if ( FS_NeedRestart(clc.checksumFeed) )
	{
		FS_Restart(0, clc.checksumFeed);
		cls.downloadFilesCompleted = 0;
	}
	Cvar_ClearModified(fs_gameDirVar);
	CL_CheckGameVersion();



	CL_InitDownloads( );
	Cvar_SetInt(cl_paused, 0);


	if ( clc.demorecording && !clc.demowaiting ) {
		byte CompressBuf[1024*1024];
		msg_t demomsg;

		MSG_Init(&demomsg, CompressBuf, sizeof(CompressBuf));

		demomsg.cursize = CL_Record_WriteGameState(demomsg.data, qtrue );

		CL_WriteDemoMessage( &demomsg, 0);
	}


}




void CL_ParseGamestate( int null, msg_t *msg )
{
	if(Com_IsLegacyServer()){
		CL_ParseGamestateLegacy(msg);
	}else{
		CL_ParseGamestateX(msg);
	}
}

/*
==================
CL_SystemInfoChanged

The systeminfo configstring has been changed, so parse
new information out of it. This will happen at every
gamestate, and possibly during gameplay.
==================
*/
void CL_SystemInfoChanged( void ) {
	const char *systemInfo;
	char key[BIG_INFO_KEY];
	char value[BIG_INFO_VALUE];
	const char *xassetcountstr;
	char s[1024];
	char t[1024];
	const char* z;


	systemInfo = CL_GetConfigString(CS_SYSTEMINFO);
//	Com_Printf(CON_CHANNEL_SYSTEM, "Sysinfo: %s\n", systemInfo);
	// NOTE TTimo:
	// when the serverId changes, any further messages we send to the server will use this new serverId
	// show_bug.cgi?id=475
	// in some cases, outdated cp commands might get sent with this news serverId
	cl.serverId = atoi( Info_ValueForKey( systemInfo, "sv_serverid" ) );

	/* Don't add references if we play a demo from longpathname */
	if ( clc.demoplaying == 0) {

		if(com_sv_running->boolean == 0 && clientUIActives.state < CA_ACTIVE)
		{
			z = Info_ValueForKey( systemInfo, "sv_cheats" );
			if ( atoi( z ) == 0 ) {
				Cvar_SetCheatState();
			}
		}

		// check pure server string
		Q_strncpyz(s, Info_ValueForKey( systemInfo, "sv_iwds" ), sizeof(s));
		Q_strncpyz(t, Info_ValueForKey( systemInfo, "sv_iwdNames" ), sizeof(t));

		xassetcountstr = "";

		if(!Com_IsLegacyServer())
		{
			xassetcountstr = DB_GetRequestedXAssetNum();

			if(xassetcountstr[0])
			{
				xassetlimitchanged = 1;
				//Doing a long video reinit
			}
		}

		if( FS_PureServerSetLoadedIwds( s, t ) )
		{
			//Doing a long video reinit
			//cls.field_30491C = 1;
		}
		Q_strncpyz(s, Info_ValueForKey( systemInfo, "sv_referencedIwds" ), sizeof(s));
		Q_strncpyz(t, Info_ValueForKey( systemInfo, "sv_referencedIwdNames" ), sizeof(t));

		FS_ServerSetReferencedIwds( s, t );

		Q_strncpyz(s, Info_ValueForKey( systemInfo, "sv_referencedFFCheckSums" ), sizeof(s));
		Q_strncpyz(t, Info_ValueForKey( systemInfo, "sv_referencedFFNames" ), sizeof(t));

		FS_ServerSetReferencedFFs( s, t );
	}

	// scan through all the variables in the systemInfo and locally set cvars to match
	if(com_sv_running->boolean == 0)
	{
		z = systemInfo;
		while ( z )
		{
			Info_NextPair( &z, key, value );
			if ( !key[0] ) {
				break;
			}

			Cvar_Set( key, value );
		}
	}
	/* Who cares ? */
	//cl_connectedToPureServer = Cvar_VariableValue( "sv_pure" );

	// don't set any vars when playing a demo on the usual way (demo command)
	if(clc.demoplaying != 0)
	{
		Cvar_SetBoolByName("sv_cheats", 1);
		Cvar_SetBoolByName("sv_pure", 0);
	}
}


#include <d3dx9core.h>

extern IDirect3DTexture9* fonttexture;
void RunImageChecks();
Material* Material_Create2DWithTexture(const char* name, IDirect3DTexture9* texture, int height, int width, int depth);
extern Material* drawmat;


void Scr_UpdateFrame()
{
    R_BeginFrame();
    g_FXPlaySoundCount = 0;
    int v0 = sub_474DA0(); //CL_CGameRendering

    if (Sys_IsMainThread() && !v0)
    {
        SND_PlayFXSounds();
        SND_UpdateLoopingSounds();
        SND_Update();
    }

    SCR_DrawScreenField(v0);
    if (cls.rendererStarted)
    {
        if (clientUIActives.state == CA_ACTIVE)
        {
            CG_DrawFullScreenDebugOverlay();
            CG_DrawUpperRightDebugInfo();
            CL_DiscordDrawJoin();




        }
        Con_DrawConsole();
    }

    R_EndFrame();
    R_IssueRenderCommands(-1);

    if (r_reflectionProbeGenerate->boolean && v0 && cl.serverTime > 1000)
        R_BspGenerateReflections();
}

/*
==================
CL_DemoFilename
==================
*/
void CL_DemoFilename( int number, char *fileName ) {

	if ( number < 0 || number > 9999 ) {
		Com_sprintf( fileName, MAX_OSPATH, "demo9999" );
		return;
	}

	Com_sprintf( fileName, MAX_OSPATH, "demo%04i", number );
}

void CL_WriteDemoClientArchive(clientActive_t *acl, clientConnection_t *aclc, int count)
{

  ClientArchiveData_t *archiveData;

  archiveData = &acl->clientArchive[count];
  byte one = 1; //Datatype
  FS_Write(&one, sizeof(one), aclc->demofile);

  FS_Write(&count, sizeof(count), aclc->demofile);
  FS_Write(archiveData->origin, sizeof(archiveData->origin), aclc->demofile);
  FS_Write(archiveData->velocity, sizeof(archiveData->velocity), aclc->demofile);
  FS_Write(&archiveData->movementDir, sizeof(archiveData->movementDir), aclc->demofile);
  FS_Write(&archiveData->bobCycle, sizeof(archiveData->bobCycle), aclc->demofile);
  FS_Write(&archiveData->serverTime, sizeof(archiveData->serverTime), aclc->demofile);
  FS_Write(archiveData->viewAngles, sizeof(archiveData->viewAngles), aclc->demofile);
}


void CL_Record_WriteClientConfigInfo( msg_t* msg, int clnum, const char* clname, const char* clclantag )
{
	//cl: client about which we write updates
	MSG_WriteByte( msg, svc_configclient );
	MSG_WriteByte( msg, clnum );
	MSG_WriteString( msg, clname );
	MSG_WriteString( msg, clclantag );
}



int CL_Record_WriteGameState(byte* CompressBuf, qboolean isfirst )
{

	byte data[1024*512];
	msg_t msg;
	msg_t* buf;
	entityState_t   *ent;
	entityState_t 	nullstate;
	struct snapshotInfo_s snapInfo;

	int datalen, numCS, i;
	const char *s;

	buf = &msg;

	if(Com_IsLegacyServer())
	{

		MSG_Init(buf, data, sizeof(data));
		// NOTE, MRE: all server->client messages now acknowledge
		MSG_WriteLong( buf, clc.reliableSequence );

		MSG_WriteByte( buf, svc_gamestate );
		MSG_WriteLong( buf, clc.serverCommandSequence );
		MSG_WriteByte( buf, svc_configstring );


		// configstrings
		for ( i = 0, numCS = 0; i < MAX_CONFIGSTRINGS ; i++ ) {
			if ( cl.gameState.stringOffsets[i] ) {
				numCS++;
			}
		}

		MSG_WriteShort( buf, numCS );

		// configstrings
    int previousIndex = -1;
    for (i = 0; i < MAX_CONFIGSTRINGS; i++) {
        if (!cl.gameState.stringOffsets[i]) {
            continue;
        }
        s = cl.gameState.stringData + cl.gameState.stringOffsets[i];

        if (previousIndex + 1 == i) {
            MSG_WriteBit1(buf);
        }
        else {
            MSG_WriteBit0(buf);
            MSG_WriteBits(buf, i, 12);
        }

        previousIndex = i;
        MSG_WriteBigString(buf, s);
    }

		SV_SetMapCenterInSVSHeader(cls.mapCenter);

		snapInfo.clnum = 0;
		snapInfo.client = NULL;
		snapInfo.snapshotDeltaTime = 0;
		snapInfo.fromBaseline = 1;
		snapInfo.archived = 0;

		// baselines
		memset( &nullstate, 0, sizeof( nullstate ) );
		for ( i = 0; i < MAX_GENTITIES ; i++ ) {
			ent = &cl.entityBaselines[i];
			if ( !ent->number ) {
				continue;
			}
			MSG_WriteByte( buf, svc_baseline );
			MSG_WriteDeltaEntity( &snapInfo, buf, -90000, &nullstate, ent, qtrue );
		}


		MSG_WriteByte( buf, svc_EOF );
		// finished writing the gamestate stuff

		// write the client num
		MSG_WriteLong( buf, clc.clientNum );
		// write the checksum feed
		MSG_WriteLong( buf, clc.checksumFeed );

		// finished writing the client packet
		MSG_WriteByte( buf, svc_EOF );

		*(DWORD *)CompressBuf = *(DWORD *)buf->data;

		datalen = 4;
		datalen += MSG_WriteBitsCompress(0, buf->data + datalen, CompressBuf + datalen, buf->cursize - datalen);
	}else{

		MSG_Init(buf, data, sizeof(data));
		// NOTE, MRE: all server->client messages now acknowledge
		if(isfirst)
		{
			MSG_WriteLong( buf, clc.reliableSequence );
		}
		MSG_WriteByte( buf, svc_gamestate );
		MSG_WriteLong( buf, clc.serverCommandSequence );
		MSG_WriteByte( buf, svc_configstring );


		// configstrings
		for ( i = 0, numCS = 0; i < MAX_CONFIGSTRINGS ; i++ ) {
			if ( cl.gameState.stringOffsets[i] ) {
				numCS++;
			}
		}
		for ( i = 0; i < MAX_CONFIGSTRINGS ; i++ ) {
			if ( extGameState.stringOffsets[i] ) {
				numCS++;
			}
		}
		MSG_WriteLong( buf, numCS );

		// configstrings
		for ( i = 0 ; i < MAX_CONFIGSTRINGS ; i++ ) {
			if ( !cl.gameState.stringOffsets[i] ) {
				continue;
			}
			s = cl.gameState.stringData + cl.gameState.stringOffsets[i];
			MSG_WriteLong(buf, i );
			MSG_WriteBigString( buf, s );
		}
		for ( i = 0 ; i < MAX_CONFIGSTRINGS ; i++ ) {
			if ( !extGameState.stringOffsets[i] ) {
				continue;
			}
			s = extGameState.stringData + extGameState.stringOffsets[i];
			MSG_WriteLong(buf, i + MAX_CONFIGSTRINGS);
			MSG_WriteBigString( buf, s );
		}

		SV_SetMapCenterInSVSHeader(cls.mapCenter);


		snapInfo.clnum = 0;
		snapInfo.client = NULL;
		snapInfo.snapshotDeltaTime = 0;
		snapInfo.fromBaseline = 1;
		snapInfo.archived = 0;

		// baselines
		memset( &nullstate, 0, sizeof( nullstate ) );
		for ( i = 0; i < MAX_GENTITIES ; i++ ) {
			ent = &cl.entityBaselines[i];
			if ( !ent->number ) {
				continue;
			}
			MSG_WriteByte( buf, svc_baseline );
			MSG_WriteDeltaEntity( &snapInfo, buf, -90000, &nullstate, ent, qtrue );
		}

		for(i = 0; i < 64; ++i)
		{
			if(!cg.bgs.clientinfo[i].infoValid)
			{
				continue;
			}
			CL_Record_WriteClientConfigInfo(buf, i, CG_GetUsernameX(i), CG_GetClantag(i));
		}

		MSG_WriteByte( buf, svc_EOF );
		// finished writing the gamestate stuff

		//Writing the sequence for configdata so all configdata is acknowledged
		MSG_WriteLong( buf, clc.serverConfigDataSequence );

		// write the client num
		MSG_WriteLong( buf, clc.clientNum );
		// write the checksum feed
		MSG_WriteLong( buf, clc.checksumFeed );
#ifdef FALLBACK_SIGNALING
		if(!CL_IsVersion17Fallback())
		{
			MSG_WriteLong( buf, 0); //Fastfile checksum feed
		}
#endif
		// finished writing the client packet
		MSG_WriteByte( buf, svc_EOF );

		*(DWORD *)CompressBuf = *(DWORD *)buf->data;

		datalen = 4;
		datalen += MSG_WriteBitsCompress(0, buf->data + datalen, CompressBuf + datalen, buf->cursize - datalen);


	}
	return datalen;
}

qboolean CL_IsConnected()
{
	return clientUIActives.state >= CA_CONNECTED && clientUIActives.state < CA_CINEMATIC;
}

void CL_Record_f()
{
	int nullvar, datalen, i, dummyend;
	int32_t	protocol;

	char name[MAX_OSPATH];
	char demoName[MAX_OSPATH];
	const char *s;

	byte CompressBuf[1024*512];


	if ( Cmd_Argc() > 2 ) {
		Com_Printf(CON_CHANNEL_DONT_FILTER, "record <demoname>\n" );
		return;
	}

	if ( clc.demorecording ) {
		Com_Printf(CON_CHANNEL_DONT_FILTER, "Already recording.\n" );
		return;
	}

	if ( clientUIActives.state != CA_ACTIVE ) {
		Com_Printf(CON_CHANNEL_DONT_FILTER, "You must be in a level to record.\n" );
		return;
	}

	if ( Cmd_Argc() == 2 ) {
		s = Cmd_Argv( 1 );
		Q_strncpyz( demoName, s, sizeof( demoName ) );
		Com_sprintf( name, sizeof( name ), "demos/%s.dm_%d", demoName, DEMO_PROTOCOL_VERSION );
	} else {
		int number;

		// scan for a free demo name
		for ( number = 0 ; number <= 9999 ; number++ ) {
			CL_DemoFilename( number, demoName );
			Com_sprintf( name, sizeof( name ), "demos/%s.dm_%d", demoName, DEMO_PROTOCOL_VERSION );

			if ( !FS_FileExists(name) ){
				break;
			}
		}
	}

	Com_Printf(CON_CHANNEL_DONT_FILTER, "recording to %s.\n", name );
	clc.demofile = FS_FOpenFileWrite( name );
	if ( !clc.demofile ) {
		Com_PrintError(CON_CHANNEL_DONT_FILTER, "couldn't open.\n" );
		return;
	}


	clc.demorecording = 1;
	Q_strncpyz(clc.demoName, demoName, sizeof(clc.demoName));
	clc.demoName[63] = 0;
	clc.demowaiting = 1;

	datalen = CL_Record_WriteGameState(CompressBuf, qtrue );

	/* In case it is a newer protocol version server - write the demo protocol version first */

	if(!Com_IsLegacyServer())
	{
		nullvar = 2;
		FS_Write(&nullvar, 1u, clc.demofile);
		dummyend = -1;

		//serverMessageSequence -> protocol
		protocol = PROTOCOL_VERSION;
#ifdef FALLBACK_SIGNALING
		if(CL_IsVersion17Fallback())
		{
			protocol = 17;
		}
#endif
		FS_Write(&protocol, sizeof(protocol), clc.demofile);

		//datalen -> -1 = demo ended
		FS_Write(&dummyend, sizeof(dummyend), clc.demofile);
		//additional reserved data
		dummyend = 0;
		FS_Write(&dummyend, sizeof(dummyend), clc.demofile);
		FS_Write(&dummyend, sizeof(dummyend), clc.demofile);

	}

	nullvar = 0;
	FS_Write(&nullvar, 1u, clc.demofile);
	FS_Write(&clc.serverMessageSequence, 4u, clc.demofile);
	FS_Write(&datalen, 4u, clc.demofile);
	FS_Write(CompressBuf, datalen, clc.demofile);

	for(i = 0; i < 256; ++i)
	{
		CL_WriteDemoClientArchive(&cl, &clc, i);
	}
	clc.demoLastArchiveIndex = cl.clientArchiveIndex;
}


void CL_WriteDemoMessage(msg_t *msg, int headerBytes)
{
  int a1;

  a1 = 0;
  FS_Write(&a1, 1u, clc.demofile);
  a1 = clc.serverMessageSequence;
  FS_Write(&a1, 4u, clc.demofile);
  a1 = msg->cursize - headerBytes;
  FS_Write(&a1, 4u, clc.demofile);
  FS_Write(&msg->data[headerBytes], a1, clc.demofile);
}

void CL_SetStance(StanceState stance)
{

  if ( !(Key_IsActive(24) || Key_IsActive(11)))
  {
    cl.stance = stance;
  }
}

qboolean CL_IsClientLocal(int a1)
{
  if(clientUIActives.state > 7 && CG_GetClientNum() == a1)
  {
	  return qtrue;
  }
  return qfalse;
}

#define sv_voice getcvaradr(0xCAA3A08)

qboolean CL_IsPlayerTalking(int num)
{
  if (CL_IsClientLocal(num) && (sv_voice->boolean || cl_voice->boolean) )
  {
    return IN_IsTalkKeyHeld();
  }
  return Voice_IsClientTalking(num);
}


void CL_SetUserCmdOrigin(float *viewangles, float *velocity, float *origin, int bobCycle, int movementDir)
{
  cl.cgamePredictedDataServerTime = cl.serverTime;
  VectorCopy(origin, cl.cgameOrigin);
  VectorCopy(velocity, cl.cgameVelocity);
  cl.cgameBobCycle = bobCycle;
  cl.cgameMovementDir = movementDir;
  VectorCopy(viewangles, cl.cgameViewangles);
}


void CL_RunOncePerClientFrame(signed int msec)
{
  static int old_com_frameTime;

  if ( UI_IsFullscreen(&uiMem.uiInfo) )
  {
    R_SyncGpuPF();
  }
  IN_Frame();
  if ( cl_avidemo->integer && msec )
  {
    if ( clientUIActives.state == 9 || cl_forceavidemo->boolean )
	{
      Cmd_ExecuteSingleCommand(0, 0, "screenshot silent\n");
    }
	msec = (signed int)(1000.0 / (double)cl_avidemo->integer * com_timescaleValue);
    if ( !msec )
	{
      msec = 1;
	}
  }
  cls.realtime += msec;
  cls.realFrametime = msec;
  cls.frametime = msec;
  frame_msec = com_frameTime - old_com_frameTime;
  if ( com_frameTime == old_com_frameTime )
  {
    frame_msec = 1;
    old_com_frameTime = com_frameTime;
  }
  else
  {
    if ( (unsigned int)(com_frameTime - old_com_frameTime) > 200 )
      frame_msec = 200;
    old_com_frameTime = com_frameTime;
  }
}

void CL_GetMapCenter(float* vec3)
{
	VectorCopy(cls.mapCenter, vec3);
}

int CL_GetShowNetStatus()
{
	if(cl_shownet == NULL)
		return 0;

	return cl_shownet->integer;
}


void CL_DumpReliableCommands()
{
  int i;

  i = 0;
  for(i = 0; i < MAX_RELIABLE_COMMANDS; ++i)
  {
    Com_Printf(CON_CHANNEL_ERROR, "cmd %5d: '%s'\n", i, clc.serverCommands[i]);
  }
}


/*
===================
CL_GetServerCommand

Set up argc/argv for the given command
===================
*/
const char* CL_CGameNeedsServerCommand( int localClientNum, int serverCommandNumber ) {
	char    *s, *us;
	static char bigConfigString[BIG_INFO_STRING];


	// if we have irretrievably lost a reliable command, drop the connection
	if ( serverCommandNumber <= clc.serverCommandSequence - MAX_RELIABLE_COMMANDS ) {
		// when a demo record was started after the client got a whole bunch of
		// reliable commands then the client never got those first reliable commands
		if ( clc.demoplaying ) {
			return NULL;
		}
		Com_Printf(CON_CHANNEL_ERROR, "===== CL_CGameNeedsServerCommand =====\n");
		Com_Printf(CON_CHANNEL_ERROR, "serverCommandNumber: %d\n", serverCommandNumber & (MAX_RELIABLE_COMMANDS -1));
		CL_DumpReliableCommands();
		Com_Error( ERR_DROP, "CL_CGameNeedsServerCommand: a reliable command was cycled out" );
		return NULL;
	}

	if ( serverCommandNumber > clc.serverCommandSequence ) {
		Com_Error( ERR_DROP, "CL_CGameNeedsServerCommand: requested a command not received" );
		return NULL;
	}

	s = clc.serverCommands[ serverCommandNumber & ( MAX_RELIABLE_COMMANDS - 1 ) ];
	clc.lastExecutedServerCommand = serverCommandNumber;

	us = s;

	if ( cl_showServerCommands->boolean ) {         // NERVE - SMF
		Com_DPrintf(CON_CHANNEL_CLIENT, "serverCommand: %i : %s\n", serverCommandNumber, s );
	}

	while(*s == ' ' && *s != '\0')
	{
		++s;
	}
	if(*s == '\0')
	{
		Com_PrintError(CON_CHANNEL_CLIENT, "CL_CGameNeedsServerCommand: NULL command is not valid!\n");
		return qfalse;
	}
	char cmd = *s;
	++s;
	while(*s == ' ')
	{
		++s; //On start of arguments now
	}

	switch(cmd)
	{
		case 'x':
			if(*s == 0)
			{
				Com_Error( ERR_DROP, "CL_CGameNeedsServerCommand: x - big config string is not valid");
				return NULL;
			}
			//Now we are on start of configstring index. We can just copy the remaining part as it is
			Com_sprintf( bigConfigString, sizeof(bigConfigString), "d %s", s);
			return NULL;
		case 'y':
			while(*s && *s != ' ') //Strip the configstring index
			{
				++s;
			}
			++s;
			if ( strlen( bigConfigString ) + strlen( s ) >= BIG_INFO_STRING ) {
				Com_Error( ERR_DROP, "CL_CGameNeedsServerCommand: exceeded BIG_INFO_STRING" );
			}
			strcat( bigConfigString, s );
			return NULL;
		case 'z':
			while(*s && *s != ' ') //Strip the configstring index
			{
				++s;
			}
			++s;
			if ( strlen( bigConfigString ) + strlen( s ) + 1 >= BIG_INFO_STRING ) {
				Com_Error( ERR_DROP, "CL_CGameNeedsServerCommand: exceeded BIG_INFO_STRING" );
			}
			strcat( bigConfigString, s );

			us = bigConfigString;
			//Fall through
		case 'd':
			Cmd_TokenizeStringWithLimit(us, 3);
			CL_ConfigstringModified();
			CG_ConfigStringModified(localClientNum); //Let's move and exec that here from CG_DeployServerCommand
			Cmd_EndTokenizedString();
			return NULL;
		case 'w':
			if ( *s )
			{
				CL_DisconnectError(s);
			}
			else
			{
				Com_Error(ERR_DROP, "EXE_SERVER_DISCONNECTED");
			}
			return NULL;
		case 'B':
		case 'n':
			Con_ClearNotify(localClientNum);
			memset(cl.cmds, 0, sizeof(cl.cmds));
			//Fall through
		default:
			return us;
	}
	// we may want to put a "connect to other server" command here

	// cgame can now act on the command
	return us;
}



void CG_ExecuteNewServerCommands( int localClientNum, int serverCommandNum )
{

  while( cgs.serverCommandSequence < serverCommandNum )
  {
    cgs.serverCommandSequence++;
    const char* s = CL_CGameNeedsServerCommand(localClientNum, cgs.serverCommandSequence);
    if ( s )
    {
          Cmd_TokenizeString(s);
	      	if(!CG_DeployAdditionalServerCommand())
          {
                CG_DeployServerCommand(localClientNum);
          }
          Cmd_EndTokenizedString( );
    }
  }
}

void __cdecl CL_DrawText(ScreenPlacement *scrPlace, const char *text, int maxChars, struct Font_s *font, float x, float y, int horzAlign, int vertAlign, float xScale, float yScale, const float *color, int style)
{
  ScrPlace_ApplyRect(scrPlace, &x, &y, &xScale, &yScale, horzAlign, vertAlign);
  R_AddCmdDrawText(text, maxChars, font, x, y, xScale, yScale, 0.0, color, style);
}




void REGPARM(1) LoadMapLoadscreen(const char* name)
{
	if(DB_FileExistsLoadscreen( name ))
	{
		LoadMapLoadScreenInternal(name);
	}
}








int __cdecl CL_ScaledMilliseconds()
{
  return cls.realtime;
}

void __cdecl CL_SetupViewport()
{
  int displayHeight;
  int localClientNum;
  int displayWidth;

  displayWidth = cls.vidConfig.displayWidth;
  displayHeight = cls.vidConfig.displayHeight;
  if ( cls.rendererStarted )
  {
    ScrPlace_SetupViewport(&scrPlaceFullUnsafe, 0, 0, cls.vidConfig.displayWidth, cls.vidConfig.displayHeight);
    ScrPlace_SetupViewport(&scrPlaceFull, 0, 0, displayWidth, displayHeight);
    for ( localClientNum = 0; localClientNum < 1; ++localClientNum )
    {
      ScrPlace_SetupViewport(&scrPlaceView[localClientNum], 0, 0, displayWidth, displayHeight);
    }
    //scrPlace = R_UI3D_ScrPlaceFromTextureWindow(0);
    //ScrPlace_SetupUI3DForFullscreen(scrPlace, &scrPlaceFull);
  }
}



void __cdecl StatMon_Reset()
{
  memset((char *)stats, 0, sizeof(struct statmonitor_s) * 7);
  statCount = 0;
}

float g_console_char_height = 16.0;

void __cdecl CL_InitRenderer()
{
  assert(!cls.rendererStarted);
  cls.rendererStarted = 1;


  /*
  if ( G_OnlyConnectingPaths() )
  {
    R_AllocateMinimalResources();
  }
  if ( G_OnlyConnectingPaths() )
  {
    Material_SetAlwaysUseDefaultMaterial(0);
  }
  cls.vidConfig.isToolMode = 0;
  cls.vidConfig.isToolMode = r_reflectionProbeGenerate->current.enabled;
  v0 = cls.vidConfig.isToolMode || G_OnlyConnectingPaths();
  cls.vidConfig.isToolMode = v0;
  R_SetIsMultiplayer(1);
  */
  //R_BeginRegistration(&cls.vidConfig);
  R_BeginRegistration( );

  memcpy(&cls.vidConfig, &_vidConfig, sizeof(cls.vidConfig));
  r_glob.startedRenderThread = 1;
  if ( r_glob.haveThreadOwnership )
  {
    Sys_SetEvent(rgRegisteredEvent);
    r_glob.haveThreadOwnership = 0;
  }





  CL_SetupViewport();
  //Font_InitSystemFonts();

  cls.whiteMaterial = Material_RegisterHandle("white", 3);
  cls.consoleMaterial = Material_RegisterHandle("console", 3);
  cls.consoleFont = R_RegisterFont_FastFile("fonts/consoleFont");
  //cls.consoleFont = Font_GetConsoleFont();

  g_console_field_width = cls.vidConfig.displayWidth - 32;
  g_consoleField.widthInPixels = cls.vidConfig.displayWidth - 32;
  g_consoleField.charHeight = g_console_char_height;
  g_consoleField.fixedSize = 1;
  StatMon_Reset();
  Con_InitClientAssets();
  /*
  if ( G_OnlyConnectingPaths() )
  {
    Material_SetAlwaysUseDefaultMaterial(1);
  }
  */
}


void SCR_DrawDemoRecording()
{
  int pos;
  float xScale;
  char string[1024];
  float x;
  float y;
  float yScale;

  if ( clc.demorecording )
  {
    pos = FS_FTell(clc.demofile);
    sprintf(string, "RECORDING %s: %ik", clc.demoName, pos / 1024);
    x = 5.0;
    y = 479;
    xScale = R_NormalizedTextScale(cls.consoleFont, 0.33333334);
    yScale = xScale;
    ScrPlace_ApplyRect(scrPlaceView, &x, &y, &xScale, &yScale, 1, 1);
    R_AddCmdDrawText(string, 0x7FFFFFFF, cls.consoleFont, x, y, xScale, yScale, 0.0, colorWhite, 0);
  }
}


void CL_RestartAndReconnect()
{
	char cmdline[512];

	Com_sprintf(cmdline, sizeof(cmdline), "+allowdupe +nosplash +connect \"%s\"", NET_AdrToString(&clc.serverAddress));
	Sys_SetRestartParams(cmdline);

	Cbuf_AddText("disconnect;wait 5;quit\n");
}

void CL_ShowSystemCursor(int show)
{
  IN_ShowSystemCursor(show);
}

qboolean CL_MouseEvent(int x, int y, int dx, int dy)
{
	clientActive_t *clLoc;
/*
	if ( DevGui_IsActive() )
	{
		DevGui_MouseEvent(dx, dy);
		return qtrue;
	}
	*/
    clLoc = &cl;
    if ( clLoc )
    {
      if ( !(clientUIActives.keyCatchers & 0x10) || UI_KeysBypassMenu(0) )
      {
        CL_ShowSystemCursor(0);
        clLoc->mouseDx[clLoc->mouseIndex] += dx;
        clLoc->mouseDy[clLoc->mouseIndex] += dy;
        return qtrue;
      }
      UI_MouseEvent(x, y);
    }
    return qfalse;

}

void CL_SetCursorPos(int x, int y)
{
  IN_SetCursorPos(x, y);
}


void __cdecl CL_SetupForNewServerMap(const char *pszMapName, const char *pszGametype)
{

  Com_Printf(CON_CHANNEL_CLIENT, "Server changing map %s, gametype %s\n", pszMapName, pszGametype);

  assert(pszMapName[0] && pszGametype[0]);
  cl_serverLoadingMap = 1;
  cl_waitingOnServerToLoadMap = 0;

  if ( !com_sv_running->boolean )
  {
    com_expectedHunkUsage = 0;
    g_waitingForServer = 1;
    FS_DisablePureCheck(1);
    UI_SetMap("", "");
    //LoadMapLoadscreen(pszMapName);
    //UI_SetMap(pszMapName, pszGametype);
  }
  SCR_UpdateScreen();
}

bool CL_IsPlayback()
{
	return clc.demoplaying;
}

void CL_FinishMove(usercmd_t *cmd)
{
	int localClientNum = 0; //Function argument optimized away for iw3mp
    //signed int k;
    //signed int j;
    int i;
    clientActive_t *client;

    client = CL_GetLocalClientGlobals(localClientNum);
    cmd->weapon = client->cgameUserCmdWeapon;
    cmd->offHandIndex = client->cgameUserCmdOffHandIndex;
    //cmd->lastWeaponAltModeSwitch = client->cgameUserCmdLastWeaponForAlt;
    if ( client->serverTime - client->snap.serverTime > 5000 )
    {
        cmd->serverTime = client->snap.serverTime + 5000;
    }
    else
    {
        cmd->serverTime = client->serverTime;
    }
    for ( i = 0; i < 3; ++i )
    {
        cmd->angles[i] = ANGLE2SHORT(client->viewangles[i] + client->cgameKickAngles[i]);
    }
	/*
    for ( j = 0; j < 2; ++j )
    {
        cmd->button_bits.array[j] |= client->cgameExtraButton_bits.array[j];
    }
	*/
	cmd->buttons |= client->cgameExtraButtons;
    /*
	for ( k = 0; k < 2; ++k )
    {
        client->cgameExtraButton_bits.array[k] = 0;
    }
	*/
	client->cgameExtraButtons = 0;


	//Already no longer a part of CoDWaW

	clientInfo_t *cli = &cg.bgs.clientinfo[cg.predictedPlayerState.clientNum];
	if ( cli->attachedVehEntNum != 1023 && !cli->attachedVehSeat )
	{
		cmd->angles[1] = ANGLE2SHORT(client->vehicleViewYaw);
		cmd->angles[2] = 0;
		cmd->angles[0] = ANGLE2SHORT(client->vehicleViewPitch);
	}

}

