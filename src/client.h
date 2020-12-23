#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "q_shared.h"
#include "qcommon.h"
#include "xassets/material.h"
#include "r_shared.h"


#define CLIENT_STATIC_ADDR 0x956d80
#define CLC_BASE_ADDR 0x8F4CE0
#define clc (*((clientConnection_t*)(CLC_BASE_ADDR)))
#define cls (*((clientStatic_t*)(CLIENT_STATIC_ADDR)))
#define clientUIActives (*((clientUIActives_t*)(0xc5f8f4)))
#define cl (*((clientActive_t*)(0x0c5f930)))
#define cluidlstatus (*((clDownloadUI_status_t*)(0x14B8C30)))
#define statData (*((clStatData_t*)(0x0cc18c90)))
#define frame_msec *((int*)(0xC5F8D0))
#define g_console_field_width *((int*)(0x726554))
#define RETRANSMIT_TIMEOUT				2000
#define STATSPACKET_TRANSMITWAIT		100

#define cl_nodelta getcvaradr(0x956cfc)
#define nextmap getcvaradr(0x1435D50)

#define AUTOUPDATEINSTALLER "cod4update.dl_"

extern char autoupdateFilename[MAX_STRING_CHARS];
extern qboolean autoupdateDownloaded;



struct field_t
{
  int cursor;
  int scroll;
  int drawWidth;
  int widthInPixels;
  float charHeight;
  int fixedSize;
  char buffer[256];
};

#define g_consoleField (*((struct field_t*)(0x8F1B88)))

typedef struct
{
  int magic;
  int saveTime;
  char digest[16];
  char statFilePath[260];
  stats_t stats;
}saveStatData_t;

typedef struct
{
  stats_t stats;
  byte dataValid;
  byte writeFlag;
}clStatData_t;

typedef struct
{
	byte unk1;
	byte cl_running;
	byte unk3;
	byte unk4;
	int  keyCatchers;
	int  unk6;
	connstate_t state;
}clientUIActives_t;

typedef struct clVoices_s
{
  char data[256];
  int len;
}clVoices_t;

/*
=============================================================================

the clientConnection_t structure is wiped when disconnecting from a server,
either to go to a full screen console, play a demo, or connect to a different server

A connection can be to either a server through the network layer or a
demo through a file.

=============================================================================
*/

#define NETCHAN_UNSENTBUFFER_SIZE 0x800
#define NETCHAN_FRAGMENTBUFFER_SIZE 0x20000

typedef struct
{
	char command[MAX_TOKEN_CHARS];
}clReliableCommands_t;

typedef struct {
	int			net_qport;					//(0x0A1E878)
	int			clientNum;					//(0x0A1E87C)
	int			lastPacketSentTime;			// for retransmits during connection
	int			lastPacketTime;				// for timeouts

	netadr_t	serverAddress;				//(0x0A1E888)

	int			connectTime;				// for connection retransmits
	int			connectPacketCount;			// for display on connection dialog
	char		serverMessage[MAX_OSPATH];	// for display on connection dialog

	int			challenge;					// from the server to use for connecting
	int			checksumFeed;				// from the server for checksum calculations

	// these are our reliable messages that go to the server
	int			reliableSequence;
	int			reliableAcknowledge;		// the last one the server has executed
	clReliableCommands_t reliableCommands[MAX_RELIABLE_COMMANDS];

	// server message (unreliable) and command (reliable) sequence
	// numbers are NOT cleared at level changes, but continue to
	// increase as long as the connection is valid

	// message sequence is used by both the network layer and the
	// delta compression layer
	int			serverMessageSequence;

	// reliable messages received from server
	int			serverCommandSequence;
	int			lastExecutedServerCommand;		// last server command grabbed or executed with CL_GetServerCommand
	char		serverCommands[MAX_RELIABLE_COMMANDS][MAX_TOKEN_CHARS];

	byte		isServerRestarting;
	byte		padding[3];
	int			demoLastArchiveIndex;
	// demo information
	char		demoName[MAX_QPATH];
	qboolean	demorecording;
	qboolean	demoplaying;
	int			timedemo;
	//int			exitdemo;
	qboolean	demowaiting;	// don't record until a non-delta message is received
	qboolean	firstDemoFrameSkipped;
	fileHandle_t demofile;
/*#ifdef MAC OS
#ifndef _WIN32*/
	fileHandle_t timedemofile;
/*#endif
#endif*/
	int			timeDemoFrames;		// counter of rendered frames
	int			timeDemoStart;		// cls.realtime before first frame
	int			timeDemoBaseTime;	// each frame will be at this time + frameNum * 50

	int			unk_01;

	// big stuff at end of structure so most offsets are 15 bits or less
	netchan_t	netchan;
	byte		unsentBuffer[NETCHAN_UNSENTBUFFER_SIZE];
	byte		fragmentBuffer[NETCHAN_FRAGMENTBUFFER_SIZE];
	byte		net_profiling_buffer[1504];
	byte		statPacketSyncState;
	byte		dummy[3];
	union
	{
		int		statPacketSentTime[7];
		int		serverConfigDataSequence;
	};
	union{
		clVoices_t	voice[10];
		byte authdata[64];
	};
	int			numVoiceData;
}clientConnection_t;



typedef struct{
  netadr_t adr;
  char allowAnonymous;
  char pswrd;
  char pure;
  char con_disabled;
  char netType;
  byte clients;
  byte maxClients;
  char visible;
  char friendlyFire;
  char kc;
  char hardcore;
  char od;
  char hw;
  char dom;
  char voice;
  char punkbuster;
  char field_24;
  char mod;
  __int16 minPing;
  __int16 maxPing;
  __int16 ping;
  unsigned __int32 queryTime;
  char serveralive;
  char hostName[32];
  char mapName[32];
  char game[24];
  char gameType[16];
  byte humanPlayers;
  char padding[2];
}serverInfo_t;



typedef struct
{
  int startTime;
  int duration;
  int fadein;
  int fadeout;
  Material_t *material[2];
}clientLogo_t;

// "updates" shifted from -7
#define AUTOUPDATE_DIR "ni]Zm^l"
#define AUTOUPDATE_DIR_SHIFT 7

struct trDebugString_t
{
  float xyz[3];
  float color[4];
  float scale;
  char text[96];
};


struct clientDebugStringInfo_t
{
  int max;
  int num;
  struct trDebugString_t *strings;
  int *durations;
};


struct trDebugLine_t
{
  vec3_t start;
  vec3_t end;
  vec4_t color;
  int depthTest;
};


struct clientDebugLineInfo_t
{
  int max;
  int num;
  struct trDebugLine_t *lines;
  int *durations;
};


struct clientDebug_t
{
  int prevFromServer;
  int fromServer;
  struct clientDebugStringInfo_t clStrings;
  struct clientDebugStringInfo_t svStringsBuffer;
  struct clientDebugStringInfo_t svStrings;
  struct clientDebugLineInfo_t clLines;
  struct clientDebugLineInfo_t svLinesBuffer;
  struct clientDebugLineInfo_t svLines;
};



typedef struct {
	qboolean	quit;
	qboolean	hunkUsersStarted;

	char		servername[MAX_OSPATH];		// name of server from original connect (used by reconnect)

	qboolean	rendererStarted;
	qboolean	cgameStarted;

	int			framecount;
	int			frametime;			// msec since last frame

	int			realtime;			// ignores pause
	int			realFrametime;		// ignoring pause, so console always works

	clientLogo_t logo;

	vec3_t mapCenter;

	int			numlocalservers;
	serverInfo_t	localServers[MAX_OTHER_SERVERS];


	int currentPingServer; //Index of the current server which gets send a ping request
	int countPingServers; //Count how many servers got pinged
	int totalSeenServers;
	int globalServerRequestTime;
	int numglobalservers;
	serverInfo_t globalServers[MAX_GLOBAL_SERVERS];
	serverInfo_t mplayerServers[MAX_GLOBAL_SERVERS];
	int numFavoriteServers;
	serverInfo_t favoriteServers[MAX_OTHER_SERVERS];

	int pingUpdateSource;		// source currently pinging or updating


	// update server info
	int			nummplayerservers;
	int			masterNum;
	int			downloadFilesCompleted; //Is required to really go to next file when WWW download failed once and FS is not yet restarted
	qboolean	wwwdlCheckedHead;
	int			updateServerChallenge;
	char		updateChallenge[MAX_TOKEN_CHARS];
	char		updateInfoString[MAX_INFO_STRING];

	netadr_t	authorizeServer;
  Material* whiteMaterial;
  Material* consoleMaterial;
  Font_t *consoleFont;

	// DHM - Nerve :: Auto-update Info
	char 		updateServerNames[MAX_UPDATE_SERVERS][MAX_QPATH];
	netadr_t 	updateServer;

	vidConfig_t vidConfig;
  struct clientDebug_t debug;

	fileHandle_t download;
	char downloadTempName[MAX_OSPATH];
	char downloadName[MAX_OSPATH];
	int downloadNumber;
	int downloadBlock;
	int downloadCount;
	int downloadSize;
	char downloadList[MAX_INFO_STRING];
	qboolean downloadRestart;
	int field_30491C;
	int disconnectForWWWdl;
	int field_304924;
	int cl_wwwDlDisconnected;
	char wwwDownloadName[MAX_QPATH];
	vec3_t debugRenderPos;
}clientStatic_t;

#pragma pack(push, 1)


typedef struct {
	int p_cmdNumber; // cl.cmdNumber when packet was sent
	int p_serverTime; // usercmd->serverTime when packet was sent
	int p_realtime; // cls.realtime when packet was sent
} outPacket_t;


typedef struct
{
  int valid;
  int snapFlags;
  int serverTime;
  int messageNum;
  int deltaNum;
  int ping;
  int field_18;
  playerState_t ps;
  int numEntities;
  int numClients;
  int parseEntitiesNum;
  int parseClientsNum;
  int serverCommandNum;
}clSnapshot_t;

typedef struct
{
  int serverTime;
  vec3_t origin;
  vec3_t velocity;
  int bobCycle;
  int movementDir;
  vec3_t viewAngles;
}ClientArchiveData_t;

/* 742 */
typedef enum
{
  CL_STANCE_STAND = 0x0,
  CL_STANCE_CROUCH = 0x1,
  CL_STANCE_PRONE = 0x2,
}StanceState;

typedef struct clientActive_s
{
  char usingAds;
  char pad[3];
  int timeoutcount;
  clSnapshot_t snap;

  int field_2F9C;
  int serverTime;
  int oldServerTime;
  int field_2FA8;
  int serverTimeDelta;
  int field_2FB0;
  int field_2FB4;
  int field_2FB8;

  gameState_t gameState;

  char mapname[64];
  int parseEntitiesNum;
  int parseClientsNum;
  int mouseDx[2];
  int mouseDy[2];
  int mouseIndex;
  byte stanceHeld;
  byte pad2[3];
  StanceState stance;
  StanceState stancePosition;
  int stanceTime;
  int cgameUserCmdWeapon;
  int cgameUserCmdOffHandIndex;
  float cgameFOVSensitivityScale;
  float cgameMaxPitchSpeed;
  float cgameMaxYawSpeed;
  vec3_t cgameKickAngles;
  vec3_t cgameOrigin;
  vec3_t cgameVelocity;
  vec3_t cgameViewangles;
  int cgameBobCycle;
  int cgameMovementDir;
  int cgameExtraButtons;
  int cgamePredictedDataServerTime;
  float viewangles[3];
  int serverId;
  int dword_C84FE8;
  char byte_C84FEC[262144];
  char gap_656BC[4];
  int dword_CC4FF0;
  char anonymous_0[4];
  usercmd_t cmds[128];
  int cmdNumber;
  ClientArchiveData_t clientArchive[256];
  int clientArchiveIndex;
  outPacket_t outPackets[32];
  clSnapshot_t snapshots[32];
  entityState_t entityBaselines[1024];
  entityState_t parseEntities[2048];
  clientState_t parseClients[2048];
  char gap_1B1AD0[264];
  int field_1B1BD8;
}clientActive_t;


typedef struct {
	netadr_t adr;
	int start;
	int time;
	char info[MAX_INFO_STRING];
} ping_t;


typedef struct
{
	int cl_downloadSize;
	int cl_downloadCount;
	int cl_downloadTime;
	char cl_downloadRemoteName[64];
}clDownloadUI_status_t;
#pragma pack(pop)

struct statmonitor_s
{
  int endtime;
  Material *material;
};


 void CL_InitOnceForAllClients();
 void __cdecl CL_ForwardToServer_f();
 void __cdecl CL_Configstrings_f();
 void __cdecl CL_Clientinfo_f();
 void __cdecl CL_Vid_Restart_f();
 void __cdecl CL_Snd_Restart_f();
 void __cdecl CL_Disconnect_f();
 void __cdecl CL_Record_f();
 void __cdecl CL_StopRecord_f();
 void __cdecl CL_PlayDemo_f();
 void __cdecl CL_PlayCinematic_f();
 void __cdecl CL_PlayUnskippableCinematic_f();
 void __cdecl CL_PlayLogo_f();
 void __cdecl CL_Connect_f();
 void __cdecl CL_Reconnect_f();
 void __cdecl CL_LocalServers_f();
 void __cdecl CL_GlobalServers_f();
 void __cdecl CL_Rcon_f();
 void __cdecl CL_Ping_f();
 void __cdecl CL_ServerStatus_f();
 void __cdecl CL_Setenv_f();
 void __cdecl CL_ShowIP_f();
 void __cdecl CL_ToggleMenu_f();
 void __cdecl CL_OpenedIWDList_f();
 void __cdecl CL_ReferencedIWDList_f();
 void __cdecl CL_UpdateLevelHunkUsage();
 void __cdecl CL_startSingleplayer_f();
 void __cdecl CL_CubemapShot_f();
 void __cdecl CL_OpenScriptMenu_f();
 void __cdecl Com_WriteLocalizedSoundAliasFiles();
 void __cdecl UI_CloseMenu_f();
 void __cdecl CL_SelectStringTableEntryInDvar_f();
 void __cdecl Con_Init();
 void __cdecl CL_InitInput();
 void __cdecl Ragdoll_InitCvars();
 void __cdecl R_Init();
 void __cdecl CG_RegisterCvars();
 void __cdecl Ragdoll_InitCommands();
 void __cdecl CL_AddFav_f();
 void __cdecl CL_ReplayDemo_f();


void __cdecl CL_ConsolePrint(int unk1, int output, const char* text, int unk3, int unk4, int type);
void __cdecl CL_CharEvent( int key );
void __cdecl SND_Init();
void __cdecl CL_Init( );
void R_InitOnce();
void __cdecl Run_RendererThread(void);
void __cdecl CL_InitRenderer(void);
qboolean __cdecl Init_RendererThread(void);
void CL_PacketEvent( netadr_t *from, msg_t *msg );
void CL_LoadCachedServers(void);
void CL_SaveServersToCache();
int CLUI_AddServer(const char* hostname, const char* serveradrstring);
int CL_RemoveServer(const char* serveradrstring);
void CL_ServerInfoPacket( netadr_t *from, msg_t *msg);
int CL_FindServerByAddr( netadr_t *netadr, int pingupdatesource );
int REGPARM(1) CL_ServerStatus( char *serverAddress, char *serverStatusString, int maxLen );
void CL_ServerStatusResponse( netadr_t *from, msg_t *msg );
void CL_LocalServers_f( void );
void CL_CheckForResend( int dummy );
const char* CL_RequestAuthorization( void );
void CL_BuildMd5StrFromCDKey(char*);
const char* CL_GetConfigString(int index);
qboolean LiveStorage_DoWeHaveStats();
stats_t* LiveStorage_GetStatsBuffer();
void LiveStorage_ReadStatsFromDir(const char* dir);
int CL_GetStatPacketIndex(void);
extern void (*CL_Netchan_Decode)(byte *data, int length);
extern void (*CL_Netchan_Encode)(byte *data, int cursize);
void CL_ParseServerMessage( msg_t *msg );
void CL_WriteDemoMessage( msg_t* msg, int headerBytes );
void CL_WriteDemoArchive(  );
void CL_ConnectionlessPacket( netadr_t* from, msg_t* msg );
//void REGPARM(1) CL_ConnectionlessPacket( msg_t* msg, netadr_t from );
void REGPARM(1) CL_ProcessPbPacket( msg_t* msg );
void CL_ConnectionlessPacket2( netadr_t *from, msg_t* msg);
void CL_ChallengeResponse(netadr_t *from);
void CL_VoicePacket( msg_t* msg );
void CL_ConnectResponse(netadr_t *from);
void CL_StatsResponse(netadr_t *from);
void CL_DisconnectPacket( netadr_t *from, const char* message );
void CL_DisconnectError(const char* message);
void CL_Disconnect( );
void CL_PrintPacket( netadr_t *from, msg_t *msg );
void CL_AwaitingAuthPacket( netadr_t *from );
void CL_ServerErrorPacket(netadr_t *from, msg_t *msg);
void CL_LoadingNewMapPacket( netadr_t *from, msg_t* msg );
void CL_SetupForNewServerMap(const char* newmap, const char* newgametype);
void CL_StatsRequest( netadr_t *from );
void CL_FastRestartMap( netadr_t *from );
void CL_RequestMotd( void );
const char* CL_GetServerIPAddress();
void CL_SavePredictedOriginForServerTime(struct clientActive_s *clactive, int serverTime, float *predictedViewAngles, float *predictedOrigin, float *predictedVelocity, int bobCycle, int movementDir);
qboolean CL_Netchan_TransmitNextFragment( netchan_t *chan );
qboolean CL_Netchan_Transmit( netchan_t *chan, int outlen, byte* outdata );
void CL_DownloadsComplete( );
void CL_NextDownload( );
void CL_WWWDownloadStop( );
void CL_BeginDownload( const char *localName, const char *remoteName );
void CL_InitDownloads( void );
void CL_UpdateInfoPacket( netadr_t *from );
ping_t* CL_GetFreePing();
void CL_Ping_f( void );
void CL_SendPbPacket(int len, void *data);
qboolean REGPARM(1) CL_UpdateDirtyPings(unsigned int source);
void CL_GetUpdateInfo();
void LiveStorage_UploadStats();
void LiveStorage_Init();
void CL_MapLoading();
void CL_Reconnect_f( void );
void CL_WritePacket( void );
void CL_GetCDKey(char* key, char* checksum, qboolean ismenu);
void CL_SetCDKey(char* key, char* checksum);
qboolean CL_CDKeyValidate(char* key1234, char* key5);
void Key_SetCatcher(int catcher);
void Key_RemoveCatcher(int catcher);
void Com_SortPlayerProfiles(int);
int Com_GetPlayerProfile(const char*);
void Com_SelectPlayerProfile(int);
void Com_CreatePlayerProfile();
void UI_DeletePlayerProfile();
void CLUI_GetServerAddressString( int source, int n, char *buf, int buflen );
signed int CLUI_GetServerPunkBuster(int n, int source);
int Key_ClearStates(int a1);
void LAN_GetServerInfo( int source, int n, char *buf, int buflen );
int LAN_GetServerPing( int source, int n );
int LAN_ServerIsVisible( int source, int n );
qboolean LAN_ServerInfoValid(unsigned int server);
int CLUI_RemoveServer(const char* serveradrstring);
void UI_AddServerToFavorites(const char *hostname, const char *serveradrstring);
void CLUI_SetPbClStatus(qboolean enable);
void CL_MutePlayer(int clnum);
int __cdecl LiveStorage_DecodeStatsData(saveStatData_t *data, const char *fsgamevar);
byte __cdecl LiveStorage_DecodeStatsData2(saveStatData_t *data, const char *fsgamevar);
void LiveStorage_WriteStatsChecksums(saveStatData_t *data);
void __cdecl CL_WriteVoicePacket();
void CL_RunOncePerClientFrame(int msec);
void CL_Frame( );
qboolean CL_ReadyToSendPacket();
void CL_ResendAuthorization( );
void CL_SendCmd();
void sub_57AEB0();
void sub_57AE10();
void sub_45C740();
void sub_46C680();
void sub_46C590( );
void sub_4729C0();
void sub_46C700( );
void sub_45C440( );
void sub_463E00();
void CL_ShutdownRef();
void CL_Shutdown(int);
void CL_InitDedicated();
void CL_AddReliableCommand( const char *cmd );
const char* CL_GetConfigString( int index );
void CL_InitCGame( netsrc_t src );
void CL_SendPureChecksums();
void REGPARM(1) LoadMapLoadScreen(const char* mapname);
void CL_ShutdownAll();
void REGPARM(1) CL_ParseDownload( msg_t *msg );
void CL_ParseWWWDownload( msg_t *msg);
int CL_WWWDownloadLoop( );
void CL_WWWDownload();
void CL_ParseSnapshot( msg_t *msg );
void CL_ParseGamestate( int, msg_t *msg );
void CL_GetAutoUpdate( void );
void CL_PlayDemo( const char* arg, qboolean timedemo, qboolean longpath );
void CL_ReadDemoMessage( );
void CL_ReadDemoData( int );
void CL_ReadDemoMessage( );
void CL_ReadDemoArchive( );
void CL_DemoCompleted( );
void CL_SystemInfoChanged( );
void LiveStorage_StatGetInCvarCmd( );
void LiveStorage_UploadStatsCmd();
void LiveStorage_ReadStatsCmd();
void LiveStorage_StatSetCmd();
void LiveStorage_StatsInit();
void __cdecl CL_SelectPlayerProfile(int);
void sub_45C970();
void sub_48D300(int);
byte sub_4686C0();
char* CL_GetLegacyServerAddress(char* outstring, int len);
void SCR_DrawScreenField(int);
void R_BeginFrame();
int sub_474DA0();
void SND_PlayFXSounds();
void SND_UpdateLoopingSounds();
void SND_Update();
void CG_DrawFullScreenDebugOverlay();
void sub_42C010();
void R_EndFrame();
void R_IssueRenderCommands(int);
void CL_ShutdownSteamApi();
void CL_DiscordShutdown();
void CL_ProcessSteamCommands(msg_t* msg);
void CL_ProcessSteamAuthorizeRequest(const char* s, uint64_t serversteamid);
void CL_ParseDownloadX( msg_t *msg );
void CL_ClearStaticDownload();
void CL_AutoUpdateDoneDownload();
void Con_InitCvars();
void CL_KevinosResetInfection();
qboolean CL_ServerInFilter(netadr_t* adr, int type);
qboolean Key_IsActive(int keynum);
qboolean IN_IsTalkKeyHeld();
int CL_Record_WriteGameState(byte* CompressBuf, qboolean isfirst );
void CL_DrawStretchPic(ScreenPlacement *place, float x, float y, float w, float h, int horzAlign, int vertAlign, float s0, float t0, float s1, float t1, const float *color, Material *material);
void CL_DrawTextPhysicalWithEffects(const char *text, int maxChars, Font_t *font, float x, float y, float xScale, float yScale, const float *color, int style, const float *glowColor, Material *fxMaterial, Material *fxMaterialGlow, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration);
void CL_DrawTextPhysical(const char *text, int maxChars, Font_t *font, float x, float y, float xScale, float yScale, const float *color, int style);
void CL_DrawStretchPicPhysical(float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float *color, Material *material);
int CL_GetKeyBinding(int localClientNum, char *search, char *outstring);
void CL_SetStance(StanceState stance);
const char* __cdecl CL_GetRankData(int);
qboolean CL_IsPlayerMuted(int num);
qboolean CL_IsPlayerTalking(int num);
void CL_SetUserCmdOrigin(float *viewangles, float *velocity, float *origin, int bobCycle, int movementDir);
REGPARM(1) int LAN_WaitServerResponse(int updatesource);
int LAN_GetServerCount(int pingupdatesource);
void LAN_ResetPings( int source );
void LAN_MarkServerVisible( int source, int n, qboolean visible );
void R_SyncGpuPF();
void IN_Frame();
void CL_GetMapCenter(float* vec3);
int CL_GetShowNetStatus();
int CL_ReceiveContentFromServerInBuffer(const char* url, byte* updateinfodata, int bufferlen);
void Con_ClearNotify(int localClientNum);
const char* CL_CGameNeedsServerCommand(int localClientNum, int commandsequence);
void __cdecl CL_DrawText(ScreenPlacement *scrPlace, const char *text, int maxChars, struct Font_s *font, float x, float y, int horzAlign, int vertAlign, float xScale, float yScale, const float *color, int style);
void __cdecl CL_AddDebugString(const float *xyz, const float *color, float scale, const char *text, qboolean fromServer, int duration);
void __cdecl CL_AddDebugStarWithText(const float *point, const float *starColor, const float *textColor, const char *string, float fontsize, int duration, qboolean fromServer);
void __cdecl CL_AddDebugLine(const float *start, const float *end, const float *color, int depthTest, int duration, qboolean fromServer);
void CL_NetAddDebugData( );
qboolean CreateDebugStringsIfNeeded();
qboolean CreateDebugLinesIfNeeded();
void __cdecl Con_TimeJumped(int localClientNum, int serverTime);
void REGPARM(1) LoadMapLoadScreenInternal(const char* name);
qboolean CL_CheckAnticheat();
void CL_SendReliableClientCommand(msg_t* msg);
void Sys_RunAnticheatNetDataCallback(msg_t* msg);
void Con_InitClientAssets();
qboolean CL_IsConnected();
void CL_RestartAndReconnect();

#define CS_BASICANTICHEATCFG 4860

#endif
