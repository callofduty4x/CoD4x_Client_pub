#ifndef __SERVER_H__
#define __SERVER_H__

#include "q_shared.h"
#include "qcommon.h"
#include "game.h"

#define svs_time *((int*)(SVS_TIME_ADDR))
#define svs (*((serverStatic_t*)(0x185C480)))
#define sv (*((server_t*)(SERVER_STRUCT_ADDR)))
#define svsHeader (*((svsHeader_t*)(0xCAE3A80)))
#define MAX_ENT_CLUSTERS 16
#define MAX_BPS_WINDOW 20

typedef enum {
	SS_DEAD,			// no map loaded
	SS_LOADING,			// spawning level entities
	SS_GAME				// actively running
} serverState_t;


typedef enum {
	CS_FREE,		// can be reused for a new connection
	CS_ZOMBIE,		// client has been disconnected, but don't reuse
				// connection for a couple seconds
	CS_CONNECTED,		// has been assigned to a client_t, but no gamestate yet
	CS_PRIMED,		// gamestate has been sent, but client hasn't sent a usercmd
	CS_ACTIVE		// client is fully in game
}clientConnectState_t;

typedef struct{
	char command[MAX_STRING_CHARS];
	int cmdTime;
	int cmdType;
}reliableCommands_t;


typedef struct {//(0x2146c);
	playerState_t	ps;			//0x2146c
	int		num_entities;
	int		num_clients;		// (0x2f68)
	int		first_entity;		// (0x2f6c)into the circular sv_packet_entities[]
	int		first_client;
							// the entities MUST be in increasing state number
							// order, otherwise the delta compression will fail
	unsigned int	messageSent;		// (0x243e0 | 0x2f74) time the message was transmitted
	unsigned int	messageAcked;		// (0x243e4 | 0x2f78) time the message was acked
	int		messageSize;		// (0x243e8 | 0x2f7c)   used to rate drop packets
	int		serverTime;
} clientSnapshot_t;//size: 0x2f84


#pragma pack(push, 1)
typedef struct 
{
  char num;
  char data[256];
  int dataLen;
}voices_t;
#pragma pack(pop)



/* 7487 */
typedef struct archivedSnapshot_s
{
  int start;
  int size;
}archivedSnapshot_t;


/* 7489 */
typedef struct 
{
  netadr_t adr;
  int challenge;
  int time;
  int pingTime;
  int firstTime;
  int firstPing;
  qboolean connected;
  char guid[36];
}challenge_t;


typedef struct 
{
  int banTime;
  char guid[32];
}tempBanSlot_t;

/* 7486 */
typedef struct cachedSnapshot_s
{
  int archivedFrame;
  int time;
  int num_entities;
  int first_entity;
  int num_clients;
  int first_client;
  int usesDelta;
}cachedSnapshot_t;


/* 7488 */
typedef struct cachedClient_s
{
  int playerStateExists;
  clientState_t cs;
  playerState_t ps;
}cachedClient_t;

/* 7472 */
typedef struct 
{
  int svFlags;
  int clientMask[2];
  vec3_t absmin;
  vec3_t absmax;
}archivedEntityShared_t;

/* 7473 */
typedef struct archivedEntity_s
{
  entityState_t s;
  archivedEntityShared_t r;
}archivedEntity_t;

typedef struct svEntity_s {
	uint16_t worldSector;
	uint16_t nextEntityInWorldSector;
	archivedEntity_t		baseline;		// 0x04  for delta compression of initial sighting
	int			numClusters;		// if -1, use headnode instead
	int			clusternums[MAX_ENT_CLUSTERS];
	int			lastCluster;		// if all the clusters don't fit in clusternums
	int			linkcontents;
	float		linkmin[2];
	float		linkmax[2];
}svEntity_t; //size: 0x178



typedef struct {//0x13e78d00
	serverState_t		state;
	int			timeResidual;		// <= 1000 / sv_frame->value
	byte		inFrame;
	byte		smp;
	byte		allowNetPackets;
	byte		pad;
	qboolean	restarting;		// if true, send configstring changes during SS_LOADING
	int			start_frameTime;		//restartedServerId;	serverId before a map_restart
	int			checksumFeed;		// 0x14 the feed key that we use to compute the pure checksum strings

	struct cmodel_s		*models[MAX_MODELS];

	uint16_t			emptyConfigString;		//0x13e79518
	uint16_t			configstrings[MAX_CONFIGSTRINGS]; //(0x13e7951a)

	uint16_t			pad2; //0x13e7a82e
	svEntity_t			svEntities[MAX_GENTITIES]; //0x1b30 (0x13e7a830) size: 0x5e000

	// the game virtual machine will update these on init and changes
	struct gentity_s	*gentities;	//0x5fb30  (0x13ed8830)
	int			gentitySize;	//0x5fb34  (0x13ed8834)
	int			num_entities;		// current number, <= MAX_GENTITIES

	playerState_t		*gameClients;		//0x5fb3c
	int			gameClientSize;		//0x5fb40 (13ed8840)will be > sizeof(playerState_t) due to game private data
	
	int			skelTimeStamp;
	int			skelMemPos;
	int			bpsWindow[MAX_BPS_WINDOW];
	int			bpsWindowSteps;
	int			bpsTotalBytes;
	int			bpsMaxBytes;
	int			ubpsWindow[MAX_BPS_WINDOW];
	int			ubpsTotalBytes;
	int			ubpsMaxBytes;
	float		ucompAve;
	int			ucompNum;
	char		gametype[MAX_QPATH]; //(0x13ed8908)
	byte		killServer; //??? Likely but not 100%
	byte		pad3[3];		//???
	const char *killReason;
} server_t;//Size: 0x5fc50

typedef struct {
//	vec3_t boxmins, boxmaxs;	// enclose the test object along entire move
	vec3_t mins;	//0x00
	vec3_t maxs;	//0x0c size of the moving object
	vec3_t var_01;
	vec3_t start;	//0x24
	vec3_t end;	//0x30
	vec3_t var_02;
	int passEntityNum;  //0x48
	int passOwnerNum;   //0x4c
	int contentmask;    //0x50
	int capsule;
} moveclip_t;


#pragma pack(push, 1)
typedef struct client_s
{
  clientConnectState_t state;
  int sendAsActive;
  int deltaMessage;
  qboolean rateDelayed;
  netchan_t netchan;
  vec3_t predictedOrigin;
  int predictedOriginServerTime;
  const char *dropReason;
  char userinfo[1024];
  reliableCommands_t reliableCommands[128];
  int reliableSequence;
  int reliableAcknowledge;
  int reliableSent;
  int messageAcknowledge;
  int gamestateMessageNum;
  int challenge;
  usercmd_t lastUsercmd;
  int lastClientCommand;
  char lastClientCommandString[1024];
  gentity_t *gentity;
  char shortname[16];
  int wwwDl_var01;
  char downloadName[64];
  fileHandle_t download;
  int downloadSize;
  int downloadCount;
  int downloadClientBlock;
  int downloadCurrentBlock;
  int downloadXmitBlock;
  byte *downloadBlocks[8];
  int downloadBlockSize[8];
  qboolean downloadEOF;
  int downloadSendTime;
  char wwwDownloadURL[256];
  qboolean wwwDownload;
  qboolean wwwDownloadStarted;
  qboolean wwwDl_var02;
  qboolean wwwDl_var03;
  int nextReliableTime;
  int floodprotect;
  int lastPacketTime;
  int lastConnectTime;
  int nextSnapshotTime;
  int timeoutCount;
  clientSnapshot_t frames[32];
  int ping;
  int rate;
  int snapshotMsec;
  int unknown6;
  int pureAuthentic;
  byte unsentBuffer[131072];
  byte fragmentBuffer[2048];
  char pbguid[33];
  byte pad;
  uint16_t clscriptid;
  int canNotReliable;
  int serverId;
  voices_t voicedata[40];
  int unsentVoiceData;
  byte mutedClients[64];
  byte hasVoip;
  stats_t stats;
  byte receivedstats;
  byte pad2[2];
}client_t;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct 
{
  client_t *clients;
  int time;
  int snapFlagServerBit;
  int numSnapshotEntities;
  int numSnapshotClients;
  int nextSnapshotEntities;
  int nextSnapshotClients;
  entityState_t *snapshotEntities;
  clientState_t *snapshotClients;
  svEntity_t *svEntities;
  vec3_t mapCenter;
  archivedEntity_t *cachedSnapshotEntities;
  cachedClient_t *cachedSnapshotClients;
  byte *archiveSnapBuffer;
  cachedSnapshot_t *cachedSnapshotFrames;
  int nextCachedSnapshotFrames;
  int nextArchivedSnapshotFrames;
  int nextCachedSnapshotEntities;
  int nextCachedSnapshotClients;
  int num_entities;
  int maxClients;
  int fps;
  int canArchiveData;
  sharedEntity_t *gentities;
  int gentitySize;
  clientSession_t *gclients_session;
  gclient_t *gclients;
  int gclientSize;
}svsHeader_t;
#pragma pack(pop)

typedef struct serverStatic_s
{
  cachedSnapshot_t cachedSnapshotFrames[512];
  archivedEntity_t cachedSnapshotEntities[0x4000];
  qboolean initialized;
  int time;
  int snapFlagServerBit;
  client_t clients[64];
  int numSnapshotEntities;
  int numSnapshotClients;
  int nextSnapshotEntities;
  int nextSnapshotClients;
  entityState_t snapshotEntities[0x2A000];
  clientState_t snapshotClients[0x20000];
  int nextArchivedSnapshotFrames;
  archivedSnapshot_t archivedSnapshotFrames[1200];
  byte archivedSnapshotBuffer[0x2000000];
  int nextArchivedSnapshotBuffer;
  int nextCachedSnapshotEntities;
  int nextCachedSnapshotClients;
  int nextCachedSnapshotFrames;
  cachedClient_t cachedSnapshotClients[0x1000];
  int nextHeartbeatTime;
  int nextStatusResponseTime;
  challenge_t challenges[1024];
  netadr_t redirectAddress;
  netadr_t authorizeAddress;
  char OOBProf[1504];
  tempBanSlot_t tempBans[16];
  int field_14850;
  vec3_t mapCenter;
  char field_14860[112];
}serverStatic_t;

int SV_NumForGentity( sharedEntity_t *ent );
sharedEntity_t *SV_GentityNum( int num );
playerState_t *SV_GameClientNum( int num );
svEntity_t  *SV_SvEntityForGentity( sharedEntity_t *gEnt );
sharedEntity_t *SV_GEntityForSvEntity( svEntity_t *svEnt );
void __cdecl SV_PacketEvent(legacy_netadr_t, msg_t*);
void SV_PacketEventWrapper(netadr_t *from, msg_t *netmsg);
int __cdecl SV_SetConfigString(signed int index, char *string);
void SV_GetMapCenterFromSVSHeader(float* center);
int SV_GetPredirectedOriginAndTimeForClientNum(int clientNum, float *origin);
#endif

