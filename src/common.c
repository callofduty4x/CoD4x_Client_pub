#include "q_shared.h"
#include "qcommon.h"
#include "client.h"
#include "server.h"
#include "win_sys.h"
#include "ui_shared.h"
#include "sec_common.h"

#include <stdarg.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

#define	MAX_CONSOLE_LINES	32
#define com_errorMessage (char*)(0x01475ED0)
#define com_errorEntered (*(qboolean*)(0x1476F1C))
#define	com_numConsoleLines *((int*)(0x1476ed0))
#define com_consoleLines ((char**)(0x1475D80))
#define com_animcheckEnabled *(qboolean*)(0x14E8C10)
#define db_init *((int*)(db_init_ADDR))

static qboolean com_fullyInitialized;
#define scr_initialized *((qboolean*)(0xE31808))
#define inUpdateFrame *((byte*)(0xD5EC422))
#define s_lastUpdateScreenTime (*(unsigned int*)(0xD5EC428))
#define s_hunkTotal *(int*)(0xCB224E8)
#define s_hunkData *(void**)(0xD5EC4F0)
#define s_origHunkData *(void**)(0xD5EC4F4)
#define hunk_low (*((struct hunkUsed_t*)(0xCBA24FC)))
#define hunk_high (*((struct hunkUsed_t*)(0xCBA24F4)))
#define g_mem (*((struct PhysicalMemory*)(0x0CBF98A0)))
#define dword_CBF99B8 (*(DWORD*)(0x0CBF99B8))

cvar_t	*com_version;
cvar_t	*com_shortversion;
#define com_recommendedSet (*(cvar_t**)0x1476f00)
cvar_t	*com_net_dropsim;
cvar_t	*com_legacyProtocol;
cvar_t	*com_demoplayer;
cvar_t	*com_PMemMegs;
#define dvar_modifiedFlags (*(int*)0x0CBA73F4)

int com_codeTimeScale;

qboolean Com_IsFullyInitialized()
{
	return com_fullyInitialized;
}

//============================================================================
//	Console messages and logfile printing
//============================================================================

static char	*rd_buffer;
static int	rd_buffersize;
static void	(*rd_flush)( char *buffer );

fileHandle_t logfile;


/*

void Com_BeginRedirect (char *buffer, int buffersize, void (*flush)( char *) )
{
	if (!buffer || !buffersize || !flush)
		return;
	rd_buffer = buffer;
	rd_buffersize = buffersize;
	rd_flush = flush;

	*rd_buffer = 0;
}

void Com_EndRedirect (void)
{
	if ( rd_flush ) {
		rd_flush(rd_buffer);
	}

	rd_buffer = NULL;
	rd_buffersize = 0;
	rd_flush = NULL;
}

void Com_StopRedirect (void)
{
	rd_flush = NULL;
}
*/
void Com_PrintMessage( conChannel_t channel, char *msg, msgtype_t type) {

	static qboolean com_isPrinting = qfalse;

	PbCaptureConsoleOutput(msg);

	if ( rd_buffer && type != MSG_NORDPRINT) {

		if(channel == CON_CHANNEL_LOGFILEONLY)
			return;

		Sys_EnterCriticalSection(6);

		if(!rd_flush){
			Sys_LeaveCriticalSection(6);
			return;
		}
		if ((strlen (msg) + strlen(rd_buffer)) > (rd_buffersize - 1)) {
			rd_flush(rd_buffer);
			*rd_buffer = 0;
		}
		Q_strcat(rd_buffer, rd_buffersize, msg);
                // TTimo nooo .. that would defeat the purpose
		//rd_flush(rd_buffer);
		//*rd_buffer = 0;
		Sys_LeaveCriticalSection(6);
		return;
	}
	// echo to dedicated console and early console
	if(channel != CON_CHANNEL_LOGFILEONLY && com_dedicated && !com_dedicated->integer){
		CL_ConsolePrint(0, channel, msg, 0, 0, type);
	}

	if(msg[0] == '^' && msg[1])
		msg += 2;

	if(channel != CON_CHANNEL_LOGFILEONLY)
		Sys_Print( msg );

	if(channel == CON_CHANNEL_CONSOLEONLY)
		return;

	// logfile
	if ( com_logfile && com_logfile->integer ) {

		Sys_EnterCriticalSection(0);
		// TTimo: only open the qconsole.log if the filesystem is in an initialized state
		//   also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)

		if(FS_Initialized() && !com_isPrinting){
			com_isPrinting = qtrue;
			if ( !logfile && Sys_IsMainThread()) {
				struct tm *newtime;
				char openstring[128];
				__time64_t aclock;

				_time64( &aclock );
				newtime = _localtime64( &aclock );

				Com_sprintf(openstring, sizeof(openstring), "\nLogfile opened on %s\n", asctime( newtime ));

				logfile = FS_FOpenFileWrite( "console_mp.log" );
				if ( logfile )
				{
					FS_Write(openstring, strlen(openstring), logfile);
				}else{
					Cvar_SetInt(com_logfile, 0);
				}
			}
			if ( logfile ) {
				FS_Write(msg, strlen(msg), logfile);
				if( com_logfile->integer > 1 )
				{
					FS_Flush(logfile);
				}
			}
			com_isPrinting = qfalse;
		}

		Sys_LeaveCriticalSection(0);
	}
}


/*
=============
Com_Printf

Both client and server can use this, and it will output
to the apropriate place.

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void QDECL Com_Printf(conChannel_t channel, const char *fmt, ... ) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage(channel, msg, MSG_DEFAULT);
}

/*
================
Com_DPrintf

A Com_Printf that only shows up if the "developer" cvar is set
================
*/
void QDECL Com_DPrintf( conChannel_t channel, const char *fmt, ...) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	if ( !com_developer || !com_developer->integer ) {
		return;			// don't confuse non-developers with techie stuff...
	}

	msg[0] = '^';
	msg[1] = '2';

	va_start (argptr,fmt);
	Q_vsnprintf (&msg[2], (sizeof(msg)-3), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage( channel, msg, MSG_DEFAULT);
}

/*
=============
Com_PrintError

Server can use this, and it will output
to the apropriate place.

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void QDECL Com_PrintError( conChannel_t channel, const char *fmt, ... ) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	memcpy(msg,"^1Error: ", 10);

	va_start (argptr,fmt);
	Q_vsnprintf (&msg[9], (sizeof(msg)-10), fmt, argptr);
	va_end (argptr);

        Com_PrintMessage( channel, msg, MSG_ERROR);
}

/*
=============
Com_PrintWarning

Server can use this, and it will output
to the apropriate place.

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void QDECL Com_PrintWarning( conChannel_t channel, const char *fmt, ... ) {
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	memcpy(msg,"^3Warning: ", 12);

	va_start (argptr,fmt);
	Q_vsnprintf (&msg[11], (sizeof(msg)-12), fmt, argptr);
	va_end (argptr);

    Com_PrintMessage( channel, msg, MSG_WARNING);
}

//============================================================================
/*
========================================================================

EVENT LOOP

========================================================================
*/

#define MAX_QUEUED_EVENTS  256
#define MASK_QUEUED_EVENTS ( MAX_QUEUED_EVENTS - 1 )

static sysEvent_t  eventQueue[ MAX_QUEUED_EVENTS ];
static int         eventHead = 0;
static int         eventTail = 0;


/*
================
Com_QueueEvent

A time of 0 will get the current time
Ptr should either be null, or point to a block of data that can
be freed by the game later.
================
*/
void Com_QueueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr )
{
	sysEvent_t  *ev;

	ev = &eventQueue[ eventHead & MASK_QUEUED_EVENTS ];

	if ( eventHead - eventTail >= MAX_QUEUED_EVENTS )
	{
		Com_Printf(CON_CHANNEL_SYSTEM, "Com_QueueEvent: overflow\n");
		// we are discarding an event, but don't leak memory
		if ( ev->evPtr )
		{
			Z_Free( ev->evPtr );
		}
		eventTail++;
	}

	eventHead++;

	if ( time == 0 )
	{
		time = Sys_Milliseconds();
	}

	ev->evTime = time;
	ev->evType = type;
	ev->evValue = value;
	ev->evValue2 = value2;
	ev->evPtrLength = ptrLength;
	ev->evPtr = ptr;
}

/*
================
Com_GetEvent

================
*/

sysEvent_t Com_GetEvent( void )
{
	sysEvent_t  ev;
	char        *s;

	Sys_EnterCriticalSection(9);


	// return if we have data
	if ( eventHead > eventTail )
	{
		eventTail++;
		Sys_LeaveCriticalSection(9);
		return eventQueue[ ( eventTail - 1 ) & MASK_QUEUED_EVENTS ];
	}


	Sys_EventLoop();
	//Calls MainWndProc()

	// check for console commands
	s = Sys_ConsoleInput();
	if ( s )
	{
		char  *b;
		int   len;

		len = strlen( s ) + 1;
		b = Z_Malloc( len );
		strcpy( b, s );
		Com_QueueEvent( 0, SE_CONSOLE, 0, 0, len, b );
	}

	// return if we have data
	if ( eventHead > eventTail )
	{
		eventTail++;
		Sys_LeaveCriticalSection(9);
		return eventQueue[ ( eventTail - 1 ) & MASK_QUEUED_EVENTS ];
	}

	// create an empty event to return
	memset( &ev, 0, sizeof( ev ) );
	ev.evTime = Sys_Milliseconds();
	Sys_LeaveCriticalSection(9);
	return ev;
}

/*
====================
NET_Event

Called from Com_EventLoop
====================
*/

void Com_GetPacketLoop( )
{
	byte bufData[MAX_MSGLEN + 1];
	netadr_t from;
	msg_t netmsg;
	netadrtype_t type;
	type = NA_IP;

	while(1)
	{
		MSG_Init(&netmsg, bufData, sizeof(bufData));

		if(NET_GetPacket(&from, &netmsg, type))
		{

			if(com_net_dropsim->integer > 0 && com_net_dropsim->integer <= 100)
			{
				// com_dropsim->integer percent of incoming packets get dropped.
				if(rand() % 101 <= com_net_dropsim->integer)
					continue;          // drop this packet
            }

			if(com_sv_running->boolean)
				SV_PacketEventWrapper(&from, &netmsg);
			else{
				CL_PacketEvent(&from, &netmsg);
			}
		}
		else
		{
			type++;
			if(type > NA_MULTICAST6)
			{
				break;
			}
		}
	}

	while(NET_GetLoopPacket(NS_CLIENT, &from, &netmsg))
	{
		CL_PacketEvent(&from, &netmsg);
	}

	while(NET_GetLoopPacket(NS_SERVER, &from, &netmsg))
	{
		if(com_sv_running->boolean)
		{
			SV_PacketEventWrapper(&from, &netmsg);
		}
	}
}

void ScreenshotTakeOnKeyEvent();

/*
=================
Com_EventLoop

Returns last event time
=================
*/
int Com_EventLoop( void ) {
	sysEvent_t	ev;

	while ( 1 ) {
		ev = Com_GetEvent();

		switch(ev.evType)
		{
			// if no more events are available
			case SE_NONE:

//				Com_ClientPacketEvent();
//				Com_ServerPacketEvent();

				Com_GetPacketLoop();
				return ev.evTime;

			case SE_KEY:
				CL_KeyEvent( 0, ev.evValue, ev.evValue2, ev.evTime );
				break;

			case SE_CHAR:
				CL_CharEvent(ev.evValue);
				break;

			case SE_CONSOLE:
				Cbuf_AddText((char *)ev.evPtr );
				Z_Free( ev.evPtr );
				Cbuf_AddText("\n");
				break;

			default:
				Com_Error( ERR_FATAL, "Com_EventLoop: bad event type %i", ev.evType );
				break;
		}
	}
	return 0;	// never reached
}



/*
==================
Com_RandomBytes

fills string array with len radom bytes, peferably from the OS randomizer
==================
*/
void Com_RandomBytes( byte *string, int len )
{
	int i;

	if( Sys_RandomBytes( string, len ) )
		return;

	Com_Printf(CON_CHANNEL_SYSTEM, "Com_RandomBytes: using weak randomization\n" );
	for( i = 0; i < len; i++ )
		string[i] = (unsigned char)( rand() % 255 );
}

/*
============
Com_HashKey
============
*/
int Com_HashKey( char *string, int maxlen ) {
	int register hash, i;

	hash = 0;
	for ( i = 0; i < maxlen && string[i] != '\0'; i++ ) {
		hash += string[i] * ( 119 + i );
	}
	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	return hash;
}

void __cdecl PMem_BeginAlloc(const char *name, unsigned int allocType)
{
	struct PhysicalMemoryAllocation *allocEntry;

	allocEntry = &g_mem.prim[allocType].allocList[g_mem.prim[allocType].allocListCount++];
    g_mem.prim[allocType].allocName = "$init";
    allocEntry->name = "$init";
    allocEntry->pos = g_mem.prim[allocType].pos;
}


void __cdecl PMem_EndAlloc(const char *name, int allocType)
{
  g_mem.prim[allocType].allocName = 0;
}

void PMem_FreeInPrim(struct PhysicalMemoryPrim *prim, const char *name);

void PMem_Free(const char* name, unsigned int alloctype)
{
	struct PhysicalMemoryPrim *prim = &g_mem.prim[alloctype];
	PMem_FreeInPrim(prim, name);
}


#define SYS_MEMORY 0x8000000
qboolean Com_HunkInitialized()
{
    return g_mem.buf ? qtrue : qfalse;
}

void PMem_Init()
{
    com_PMemMegs = Cvar_RegisterInt("com_PMemMegs", 230, 128, 600, CVAR_LATCH, "Number of megabytes allocated for the PMem memory");
    DWORD memsize = 1024 * 1024 * (com_PMemMegs->integer);

    char *allocResult = (char *)VirtualAlloc(0, memsize, 0x1000, 4);
    Com_Memset(&g_mem, 0, 0x21c);
    Com_Printf(CON_CHANNEL_SYSTEM, "begin $init\n");
    g_mem.buf = allocResult;
    g_mem.prim[1].pos = memsize;
}

void __cdecl Com_InitHunkMemory()
{
  if ( FS_Started() )
  {
    Com_Error(ERR_FATAL, "Hunk initialization failed. File system load stack not zero");
  }
  s_hunkTotal = 0xA000000;

  if ( com_useFastFiles->boolean )
  {
    s_hunkTotal = 0x2000000;
  }
  R_ReflectionProbeRegisterDvars();
  if ( r_reflectionProbeGenerate->boolean )
  {
    s_hunkTotal = 0x20000000;
  }
  s_hunkData = VirtualAlloc(0, s_hunkTotal, 0x2000u, 4u);
  if ( !s_hunkData )
    Sys_OutOfMemory(".\\universal\\com_memory.cpp", 1318);

  s_origHunkData = s_hunkData;
  hunk_low.permanent = 0;
  hunk_low.temp = 0;
  hunk_high.permanent = 0;
  hunk_high.temp = 0;
  Hunk_ClearData();
  VirtualFree(s_hunkData, s_hunkTotal, 0x4000u);
  Cmd_AddCommand("meminfo", Com_Meminfo_f);
}


void DB_InitThread(){
	if(!Sys_SpawnDatabaseThread()){
		Sys_Error("Failed to create database thread");
	}
}

void Hunk_InitDebugMemory(){
	void* var_01;
	void* var_02;


	var_01 = VirtualAlloc(0, 0x1000000, 0x2000, 4);
	var_02 = VirtualAlloc(var_01, 32, 0x1000, 4);
	if(!var_02){
		Sys_OutOfMemory(".\\universal\\com_memory.cpp" ,0x1aa);
		return;
	}
__asm__ __volatile__(
"	fld1   \n"
"	andl   $0xfffffffe,0xcba73f4\n"
"	fstps  0x1435d2c\n"
"	lea    0x1000000(%%eax),%%edx\n"
"	lea    0x20(%%eax),%%ecx\n"
"	mov    %%edx,0xc(%%eax)\n"
"	mov    %%ecx,0x10(%%eax)\n"
"	movl   $0x1000000,0x8(%%eax)\n"
"	mov    %%eax,(%%eax)\n"
"	movb   $0x0,0x18(%%eax)\n"
"	movl   $0x6f21dc,0x14(%%eax)\n"
"	movb   $0x0,0x19(%%eax)\n"
"	movl   $0x0,0x1c(%%eax)\n"
"	mov    %%eax,0xcb224e4\n"
	::"eax" (var_01): );
}

/*
void Com_CheckSetRecommend(){

	if(!com_recommendedSet->boolean || Com_LoadRecommendedConfiguration()){
		Com_SetRecommend();
		Cvar_SetBool();

	}

	if(Sys_HasConfigureChecksumChanged()){
		Com_SetRecommend();
	}
}
*/

void Com_ParseURLCommand_f( )
{
	char command[MAX_STRING_CHARS];
	char password[32];
	char execstring[MAX_STRING_CHARS];
	char const* cmd;
	char *split;
	password[0] = 0;

	if(Cmd_Argc() != 2)
	{
		Com_Printf(CON_CHANNEL_DONT_FILTER, "usage: openurl command\n");
	}
	cmd = Cmd_Argv(1);

	if(Q_stricmpn("cod4://", cmd, 7))
	{
		Com_PrintWarning(CON_CHANNEL_DONT_FILTER, "Invalid URL passed. CoD4 URLs starting always with cod4://\nFormat is: cod4://address:port/pw:password");
		return;
	}
	cmd += 7;
	Q_strncpyz(command, cmd, sizeof(command));

	split = strchr(command, '/');
	if(split != NULL)
	{
		*split = '\0';
		split += 1;
		if(*(split))
		{
			if(!Q_stricmpn("pw:", split, 3) && split[4])
			{
				split += 3;
				Q_strncpyz(password, split, sizeof(password));
				split = strchr(password, ';');
				if(split != NULL)
				{
					*split = '\0';
				}
			}
		}

	}

	split = strchr(command, ';');
	if(split != NULL)
	{
		*split = '\0';
	}
	if(password[0])
	{
		Com_sprintf(execstring, sizeof(execstring), "set password \"%s\"; connect \"%s\"\n", password, command);
	}else{
		Com_sprintf(execstring, sizeof(execstring), "connect \"%s\"\n", command);
	}
	Cbuf_AddText(execstring);
}

void Com_ExecCommandlineArgs()
{
  int i;
  char buf[MAX_STRING_CHARS];

  for ( i = 0; i < com_numConsoleLines; ++i )
  {
    if ( *com_consoleLines[i] )
    {
      Com_sprintf(buf, sizeof(buf), "%s\n", com_consoleLines[i]);
      Cbuf_ExecuteBuffer(0, 0, buf);
    }
  }
}

void Com_BuildVersionString(char* buf, int size)
{
	Com_sprintf(buf, size ,"%s %s %s", Q3_VERSION, PLATFORM_STRING, __DATE__);
}

void Dev_UtilsInit();

#include "cg_shared.h"


static void InitConsoleLines(char *CommandLine_)
{
    signed int count; // edx

    com_consoleLines[0] = CommandLine_;
    count = 1;
    com_numConsoleLines = 1;
    if (*CommandLine_)
    {
        do
        {
            if (*CommandLine_ == '+' || *CommandLine_ == '\n')
            {
                if (count == ' ')
                    break;
                com_consoleLines[count++] = CommandLine_ + 1;
                *CommandLine_ = 0;
            }
            ++CommandLine_;
        } while (*CommandLine_);
        com_numConsoleLines = count;
    }
}

void __cdecl Com_Init_Try_Block(char* commandLine){

	int msec;
	char* s;
	int qport;
	char updatestring[256];

	Com_BuildVersionString(updatestring, sizeof(updatestring));
	Com_Printf(CON_CHANNEL_SYSTEM, "%s\n", updatestring);
	Com_Printf(CON_CHANNEL_SYSTEM, "Commandline is: %s\n", commandLine);
	InitConsoleLines(commandLine);

	//Com_Printf( "cg.predictedPlayerState.Ammo %p, Ammoclip %p, Weaponmodels %p\n", cg.predictedPlayerState.ammo ,cg.predictedPlayerState.ammoclip, cg.predictedPlayerState.weaponmodels);
	//cg.predictedPlayerState.Ammo 007945d0, Ammoclip 007947d0, Weaponmodels 00794c90
	//Patching: ResetWeaponAnimTrees() BG_GetTotalAmmoReserve() BG_ClipForWeapon()

	SL_Init();
	Swap_Init();
	Cbuf_Init();
	Cmd_Init();
	Com_StartupVariable(0);
	Com_InitDvars();
	Sec_Init();

	CCS_InitConstantConfigStrings();

	msec = 0;
	if(com_useFastFiles->boolean)
    {
		PMem_Init();
		db_init = qtrue;
		Com_Printf(CON_CHANNEL_SYSTEM, "begin $init\n");
		msec = Sys_Milliseconds();
		PMem_BeginAlloc("$init", 1);
		DB_InitThread();
	}


	CL_InitKeyCommands();
	FS_InitFilesystem();
	Sys_SetupCrashReporter();
	Con_InitChannels();
	LiveStorage_Init();

	Cvar_AddFlags(com_dedicated, CVAR_ROM); //Shall fix bug where players start dedicated server but not client because of their config

	Init_PlayerProfile(0);
	Cbuf_Execute(0, 0);

	Cvar_ClearFlags(com_dedicated, CVAR_ROM);

	if(dvar_modifiedFlags & 0x20)
		Com_InitDvars();

	com_recommendedSet = Cvar_RegisterBool("com_recommendedSet", qtrue, CVAR_ARCHIVE, "Use recommended settings");
	if(strstr(commandLine, "+save"))
		Com_CheckSetRecommend();
	else
		Sys_RegisterInfoDvars();

	Com_StartupVariable(0);

	if(!com_useFastFiles->boolean)
		SEH_UpdateLanguageInfo();

	if(com_dedicated->integer)
		CL_InitDedicated();

	Com_InitHunkMemory();
	Hunk_InitDebugMemory();
	com_codeTimeScale = 1.0;
#ifdef DEBUG
	if(com_developer && com_developer->integer)
		Dev_UtilsInit();
#endif
	Cmd_AddCommand ("quit", Com_Quit_f);
	Cmd_AddCommand ("writeconfig", Com_WriteConfig_f);
	Cmd_AddCommand ("writedefaults", Com_WriteDefaults_f);
	Cmd_AddCommand ("openurl", Com_ParseURLCommand_f);
	s = va("%s %s %s", Q3_VERSION, PLATFORM_STRING, __DATE__ );
	com_version = Cvar_RegisterString("version", s, CVAR_ROM|CVAR_SERVERINFO, "Game version");
	com_shortversion = Cvar_RegisterString("shortversion", "1.7", CVAR_ROM|CVAR_SERVERINFO, "Short game version");
	com_legacyProtocol = Cvar_RegisterBool("legacyProtocol", qfalse, CVAR_ROM, "Is true when connecting to a server with protocol version 6");
	com_demoplayer = Cvar_RegisterBool("demoplayer", qfalse, CVAR_INIT, "If this is true this client is a demo player only");

	Sys_Init();
	Com_RandomBytes( (byte*)&qport, sizeof(int) );
	Netchan_Init( qport );

	Scr_InitVariables();
	Scr_Init();
	Scr_DefaultSettings();
	XAnim_Init();

	SV_Init();

	com_net_dropsim = Cvar_RegisterInt( "net_dropsim", 0, 0, 100, CVAR_CHEAT, "Network packetloss simulation - Percentage for packets getting dropped");
	Huffman_InitMain();
	NET_Init();

	Cvar_ClearModified(com_dedicated);
	if(!com_dedicated->integer){
		//CL_InitOnceForAllClients_org();
		CL_InitOnceForAllClients();
		CL_Init(0);
	}

	com_frameTime = Sys_Milliseconds();
	Com_StartupVariable(0);

	if(!com_dedicated->integer){
		IN_Init();

		SND_InitDriver();

		R_InitOnce();
		CL_InitRenderer();
		*(int*)0x956e8c = 1;
		SND_Init();
		//Sys_ShowConsole(0 , qfalse);
	}

	if(!Sys_IsTempInstall())
	{
		Sys_RegisterCoD4Protocol();
		Sys_RegisterDemoExt( );
	}

	Play_Intro();

	if(com_useFastFiles->boolean){
		PMem_EndAlloc("$init", 1);
		db_init = qfalse;
		Com_Printf(CON_CHANNEL_SYSTEM, "end $init %d ms\n", Sys_Milliseconds() - msec);
	}

	//XAssetLoader_Init();


	Com_Printf(CON_CHANNEL_SYSTEM, "--- Common Initialization Complete ---\n");
	com_fullyInitialized = qtrue;

	//CL_KevinosResetInfection();

//	Cvar_Dump();  //Stop this unnecessary console spam

}

qboolean Com_IsLegacyServer()
{
	return com_legacyProtocol->boolean;
}


void __cdecl Com_Init(char* cmdLine)
{
	char errormsg[MAX_STRING_CHARS];
	jmp_buf *abortframe;

	abortframe = (jmp_buf *)Sys_GetValue(2);

	if ( _setjmp3(*abortframe, 0))
	{
		Com_sprintf(errormsg, sizeof(errormsg), "Error during initialization:\n%s\n", com_errorMessage);
		Sys_Error(errormsg);
		return;
	}

	Com_Init_Try_Block(cmdLine);

	abortframe = (jmp_buf *)Sys_GetValue(2);

	if ( !_setjmp3(*abortframe, 0) )
	{
		Com_ExecCommandlineArgs();
	}
	if ( com_errorEntered )
	{
		Com_ErrorCleanUp();
	}
	if ( com_sv_running->boolean || com_dedicated->integer )
	{
		return;
	}

  	abortframe = (jmp_buf *)Sys_GetValue(2);

	if ( _setjmp3(*abortframe, 0) )
	{
	    Com_sprintf(errormsg, sizeof(errormsg), "Error during initialization:\n%s\n", com_errorMessage);
		Sys_Error(errormsg);
		return;

    }
    if ( !cls.rendererStarted )
	{
		CL_InitRenderer( );
    }
	sub_5F78A0();


	CL_StartHunkUsers();



	R_SyncRenderThread();


}

void Com_StartHunkUsers();

void Com_StartHunkUsers()
{
  jmp_buf *abortframe;
  abortframe = (jmp_buf *)Sys_GetValue(2);

  if ( _setjmp3(*abortframe, 0))
  {
    Sys_Error("Error during initialization:\n%s\n", com_errorMessage);
  }
  Com_AssetLoadUI();
  UI_SetActiveMenu(0, UIMENU_MAIN);
  IN_Frame();
  Com_EventLoop();
}

int Com_ModifyMsec(signed int msec)
{
  qboolean a;
  int clampTime, oldMsec;

  oldMsec = msec;

  a = 1;
  if ( com_fixedtime->integer )
  {
    msec = com_fixedtime->integer;
  }
  else
  {
    if ( 1.0 != com_timescale->value || 1.0 != com_codeTimeScale || 1.0 != dev_timescale->value)
    {
		msec = floorf((com_codeTimeScale * (double)msec * com_timescale->value * dev_timescale->value) + 0.5);
	}else{
		a = 0;
	}
  }

  if ( msec < 1 )
  {
    msec = 1;
  }
  if ( com_dedicated->integer )
  {
    if ( msec > 500 )
	{
      Com_PrintWarning(CON_CHANNEL_SYSTEM, "Hitch warning: %i msec frame time\n", msec);
	}
    clampTime = 5000;
  }
  else
  {
    if ( com_sv_running->boolean )
      clampTime = com_maxFrameTime->integer;
    else
      clampTime = 5000;
  }
  if ( msec > clampTime )
  {
    msec = clampTime;
  }

  if ( a && oldMsec )
  {
    com_timescaleValue = (double)msec / (double)oldMsec;
  }
  else
  {
    com_timescaleValue = 1.0;
  }

  return msec;
}


void Com_Frame_Try_Block()
{
	int minMsec;
	int msec;
	static int lastTime;

	Com_WriteConfiguration();
	com_animcheckEnabled = com_animCheck->boolean;

	if ( com_maxfps->integer > 0 && !com_dedicated->integer )
	{
		minMsec = 1000 / com_maxfps->integer;
	}else{
		minMsec = 1;
	}

	if ( *(qboolean*)0x14E89A0 != 1 )
	{
		if ( !sys_lockThreads->integer && *(int*)0xCC9D058 == 1 )
		{
			Win_UpdateThreadLock( 1 );

		}else{
			Win_UpdateThreadLock( sys_lockThreads->integer );

		}
	}
	else
	{
		*(int*)0xD5EC490 = 2;
	}

	if ( com_dedicated->integer )
	{
		while ( qtrue )
		{
			Com_EventLoop();
			com_frameTime = Sys_Milliseconds();
			if ( lastTime > com_frameTime ){
				lastTime = com_frameTime;
			}
			msec = com_frameTime - lastTime;
			if ( msec >= minMsec )
				break;
			Sleep( 1 );
		}
		lastTime = com_frameTime;
	}
	else
	{
		do
		{
			Com_EventLoop();
			com_frameTime = Sys_Milliseconds();;
			if ( lastTime > com_frameTime ){
				lastTime = com_frameTime;
			}
			msec = com_frameTime - lastTime;
			if ( msec > 0 )
				break;
			usleep( 50 );
		}while(1);

		if (msec < minMsec )
		{
			msec = minMsec;
		}
		lastTime = msec + lastTime;
		if ( !msec )
		{
			msec = 1;
		}
	}
	Cbuf_Execute( );

	msec = Com_ModifyMsec(msec);
	SV_Frame(msec);
	Com_SetDedicatedMode( );

	if ( com_dedicated->integer )
	{
		return;
	}

	CL_RunOncePerClientFrame( msec );

	Com_EventLoop( );

	Cbuf_Execute( );


	CL_Frame(0);

	/* reset cvar_changedflags maybe */
	*(int*)0xCBA73F4 &= 0xFFFFFFFDu;

	if ( !UI_IsFullscreen(&uiMem.uiInfo) && !clientUIActives.state )
	{
		UI_SetActiveMenu(0, 1);
	}

	SCR_UpdateScreen( );

	Ragdoll_Update( msec );
	StatMon_Frame( );


	R_WaitEndTime( lastTime );

}


void __cdecl R_WaitEndTime(unsigned int frameTime)
{
  unsigned int time;

  while ( 1 )
  {
	time = Sys_Milliseconds() - frameTime;
    if ( (time & 0x80000000u) == 0 )
      break;
    Sleep(1u);
  }
}


void SCR_UpdateScreen()
{
	unsigned int updateMsec;
	unsigned int now;

    if ( inUpdateFrame )
	{
		return;
	}

	if ( clientUIActives.state <= 8 )
	{
		if( clientUIActives.state < 7 )
		{
			updateMsec = 16;
		}else{
			updateMsec = 33;
		}
		now = Sys_Milliseconds();
		if ( updateMsec > now - s_lastUpdateScreenTime )
		{
			return;
		}
		s_lastUpdateScreenTime = now;
	}

    if ( clientUIActives.state == 7 )
	{
		Sys_LoadingKeepAlive();
	}

    if ( !scr_initialized )
    {
		return;
	}

	if ( !com_errorEntered )
    {
		if(!R_GfxWorldLoaded() && clientUIActives.state == 9)
		{
			return;
		}
        inUpdateFrame = 1;
        Scr_UpdateFrame();
        inUpdateFrame = 0;
    }
}

#define databaseObjHandle (*(HANDLE*)(0x14E89A4))
#define databaseObjHandle2 (*(HANDLE*)(0x14E89E4))

void Com_Frame()
{
	static int com_frameCount;

	jmp_buf *abortframe;

	abortframe = (jmp_buf *)Sys_GetValue(2);

	if ( !_setjmp3(*abortframe, 0))
	{
		if ( WaitForSingleObject(databaseObjHandle2, 0) && !WaitForSingleObject(databaseObjHandle, 0) )
		{
			DB_PostLoadXZone();
		}
		Com_Frame_Try_Block();
		++com_frameCount;
	}

	Sys_EnterCriticalSection( 2 );
	if( !com_errorEntered )
	{
		Sys_LeaveCriticalSection( 2 );
		return;
	}

	Com_ErrorCleanUp();
	Sys_LeaveCriticalSection( 2 );

	if ( !com_dedicated->integer )
	{
		CL_InitRenderer();
		Com_StartHunkUsers();
	}

}


qboolean Com_HasPlayerProfile()
{

	if(com_playerProfile->string[0])
		return qtrue;
	return qfalse;
}


void Com_LoadPlayerProfile(const char *profilename)
{
	char profile[64];

	if ( !Q_stricmp(profilename, com_playerProfile->string) )
	{
		return;
	}
  Q_strncpyz(profile, profilename, sizeof(profile));

  if ( Com_PlayerProfileExists(profile) == qfalse)
  {
		return;
	}
	FS_SV_WriteFileToSavePath( "players/profiles/active.txt", (void*)profilename, strlen(profilename) );

	Cmd_ExecuteSingleCommand(0, 0, "disconnect");
  Cbuf_AddText("quit\n");

	Sys_SetRestartParams( "+nosplash +set ui_playerProfileAlreadyChosen 1" );
}


qboolean Com_PlayerProfileExists( const char* buf )
{
	char profileqpath[MAX_OSPATH];

	Com_sprintf(profileqpath, sizeof(profileqpath), "players/profiles/%s", buf);

	return FS_SV_DirExists(profileqpath);
}

qboolean __cdecl Com_CheckPlayerProfile( )
{
  fileHandle_t f;
  char buf[MAX_QPATH];
  int len;

  len = FS_SV_FOpenFileRead( "players/profiles/active.txt", &f );
  if(len < 1)
  {
	return qfalse;
  }
  len = FS_Read( buf, sizeof(buf) -1, f );
  FS_FCloseFile( f );
  if(len < 1)
  {
	return qfalse;
  }
  buf[len] = '\0';
  Q_TrimCRLF( buf );

  if ( !Com_PlayerProfileExists( buf ))
  {
	return qfalse;
  }
  Com_SetPlayerProfile(0, buf );
  return qtrue;
}

void Com_Quit_f()
{
    Com_Printf(CON_CHANNEL_DONT_FILTER, "quitting...\n");
    while ( *(int*)0xCC9D148 )
	{
		R_SyncRenderThread();
	}
    R_WaitWorkerCmds();
    *(byte*)0x15CA618 = 0;
    Sys_EnterCriticalSection( 2 );
    if ( *(short*)0x13EE4C0 )
    {
        sub_518940();
        *(short*)0x13EE4C0 = 0;
    }

	if ( !com_errorEntered )
    {
        Hunk_Clear();
		Sys_DestroySplashWindow();
        CL_Shutdown(0);
		CL_ShutdownSteamApi();
		CL_DiscordShutdown();
        SV_Shutdown("EXE_SERVERQUIT");
        CL_ShutdownRef(); //Also destroy window
        Com_Close();
        if ( logfile )
        {
            FS_FCloseFile(logfile);
            logfile = 0;
        }
        FS_Shutdown();
		FS_ShutdownIwdPureCheckReferences();

		FS_ShutdownServerIwdNames( );
		FS_ShutdownServerReferencedIwds( );
		FS_ShutdownServerReferencedFFs( );
    }
	Sys_Quit();
}


void Com_SetDedicatedMode()
{
	static const char* g_dedicatedEnumNames[] = { "listen server", "dedicated LAN server", "dedicated internet server", NULL };

	if ( com_dedicated->flags & 0x40 || com_dedicated->latchedInteger == com_dedicated->integer)
	{
		return;
	}

    com_dedicated = Cvar_RegisterEnum("dedicated", g_dedicatedEnumNames, 0, 0x20u, "True if this is a dedicated server");

    if ( com_dedicated->integer )
    {
      Cvar_RegisterEnum("dedicated", g_dedicatedEnumNames, 0, 0x40u, "True if this is a dedicated server");
    }
    Cvar_ClearModified( com_dedicated );
    CL_Shutdown(0);
	CL_ShutdownSteamApi();
	CL_DiscordShutdown();
    CL_ShutdownRef();
    CL_InitDedicated();
    SV_AddDedicatedCommands();
}


void Com_SetErrorEntered()
{

	com_errorEntered = 1;

}



typedef struct
{
    short word_0;
    byte byte_2;
    byte numbytes;
    byte datastart;
}slHeader_t;

typedef union
{
    slHeader_t header;
    char data[12];
}slTree_t;

slTree_t** memTreePub = (slTree_t**)0x14E8A04;


char* SL_ConvertToString(unsigned int location)
{
    char* string;

	slTree_t* slmem = *memTreePub;
    string = (char*)&(slmem[location].header.datastart);
    return string;
}

int SL_GetString(const char *string, int unk)
{
	int memlen = strlen(string) + 1;
	return SL_GetStringOfSize(string, unk, memlen);
}

void FreeString(const char* string)
{
	unsigned int index;
    index = SL_FindString(string);
    if(index)
	{
		SL_RemoveRefToString(index);
	}
}


int SL_FindString(const char *string)
{
    return FindStringOfSize(string, strlen(string) + 1);
}


int SL_FindLowercaseString(const char *upperstring)
{
	unsigned int size;
	char lwrstr[8192];
	int i;

	size = strlen(upperstring) + 1;

	if ( (signed int)size >= sizeof(lwrstr) )
	{
		return 0;
	}

	for(i = 0; i < size; ++i)
	{
		lwrstr[i] = tolower(upperstring[i]);
	}
    return FindStringOfSize(lwrstr, size);
}

const char* CopyString(const char* string)
{
	int location;
	const char* storedString;

	location = SL_GetString(string, 0);
	storedString = SL_ConvertToString(location);
	return storedString;

}

unsigned Com_BlockChecksumKey32(void* buffer, int length, int key)
{
    int i, j;
    unsigned int q = ~key;
    byte* val = buffer;

    for(i = 0; i < length; i++)
    {
        q = val[i] ^ q;

        for(j = 0; j < 8; j++)
        {
            if(q & 1)
                q = (q >> 1) ^ 0xEDB88320;
            else
                q = (q >> 1) ^ 0;
        }
    }
    return ~q;
}

void Com_BuildPlayerProfilePath(char* path, int pathlen, char *dir, char* file)
{

  if ( !com_playerProfile->string[0] )
  {
    Com_Error(0, "Tried to use a player profile before it was set.  This is probably a menu bug.\n");
		return;
  }

  if(dir && file)
  {
		Com_sprintf(path, pathlen, "players/profiles/%s/%s/%s", com_playerProfile->string, dir, file);
  }else{
		if(dir)
		{
			file = dir;
		}

		Com_sprintf(path, pathlen, "players/profiles/%s/%s", com_playerProfile->string, file);
  }
}

void Com_WriteConfigToFile(char *filename)
{
  fileHandle_t f;
  char bindwbuf[8192];

  f = FS_SV_FOpenFileWriteSavePath(filename);


  if ( !f )
  {
    Com_Printf(CON_CHANNEL_SYSTEM, "Couldn't write %s.\n", filename);
  }
  FS_Printf(f, "// generated by Call of Duty, do not modify\n");
  FS_Printf(f, "unbindall\n");
  Key_WriteBindings(bindwbuf);
  FS_Printf(f, "%s", bindwbuf);
  Cvar_WriteVariables(&f, (void (__cdecl *)(cvar_t *, void *))Cvar_WriteSingleVariable);
  Con_WriteFilterConfigString(f);
  FS_FCloseFile(f);
}

void __cdecl Com_WriteConfiguration()
{
  char filename[64];

  if ( !Com_IsFullyInitialized() || !(cvar_modifiedFlags & 1) )
  {
	return;
  }

  cvar_modifiedFlags &= 0xFFFFFFFEu;

  if ( !com_playerProfile->string[0] )
  {
		return;
  }
  Com_BuildPlayerProfilePath(filename, sizeof(filename), "config_mp.cfg", NULL);
  Com_WriteConfigToFile(filename);
}


/*
===============
Com_WriteConfig_f

Write the config file to a specific name
===============
*/
void Com_WriteConfig_f( void )
{
	char filename[MAX_QPATH];
	char wfilename[MAX_QPATH];

	if ( Cmd_Argc() != 2 ) {
		Com_Printf(CON_CHANNEL_DONT_FILTER, "Usage: writeconfig <filename>\n" );
		return;
	}

	Q_strncpyz( filename, Cmd_Argv( 1 ), sizeof( filename ) );
	COM_DefaultExtension( filename, sizeof( filename ), ".cfg" );
	Com_sprintf(wfilename, sizeof(wfilename), "players/%s", filename);
	Com_Printf(CON_CHANNEL_DONT_FILTER, "Writing %s.\n", wfilename );
	Com_WriteConfigToFile( wfilename );
}


int Com_NewPlayerProfile(const char *profname)
{

  wchar_t ospath[MAX_OSPATH];
  wchar_t ospathout[MAX_OSPATH];


  if ( Com_PlayerProfileExists(profname) )
  {
    Com_Printf(CON_CHANNEL_DONT_FILTER, "Profile '%s' already exists\n", profname);
    return 0;
  }

  FS_BuildOSPathForThreadUni( FS_GetSavePath(), "players/profiles", profname, ospath, 0);
  Com_sprintfUni(ospathout, sizeof(ospathout), L"%s%c", ospath, PATH_SEPUNI);

  if ( FS_CreatePathUni(ospathout) )
  {
      Com_Printf(CON_CHANNEL_DONT_FILTER, "Unable to create new profile path: players/profiles/%s\n", profname);
      return 0;
  }
  return 1;
}

qboolean __cdecl Com_DeletePlayerProfile(const char *profname)
{
  char profdir[MAX_QPATH];

  if ( !Com_PlayerProfileExists(profname))
  {
	return 0;
  }

  Com_sprintf(profdir, sizeof(profdir), "players/profiles/%s", profname);

  if(FS_SV_RemoveDir(profdir))
  {
    if ( !Q_stricmp(profname, com_playerProfile->string) )
	{
      Cvar_SetString(com_playerProfile, "");
    }
	return 1;
  }
  return 0;
}



void Com_ReadCDKey()
{
	HKEY hKey;
	DWORD cbData;
	DWORD Type;
	char querydata[21];
	qboolean opened;
	char* cl_cdkey = (char*)0x724b84;
	char* cl_cdkey_checksum = (char*)0x724ba8;

	opened = (RegOpenKeyA(HKEY_CURRENT_USER, "SOFTWARE\\Activision\\Call of Duty 4", &hKey) == 0);
	if(opened == qfalse)
	{
		opened = (RegOpenKeyA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Activision\\Call of Duty 4", &hKey) == 0);
	}


	if (opened == qtrue )
	{
		Type = 1;
		cbData = 21;
		if ( RegQueryValueExA(hKey, "codkey", 0, &Type, (LPBYTE)querydata, &cbData) == 0 && cbData == 21)
		{
			Q_strncpyz(cl_cdkey, querydata, 16 +1);
			Q_strncpyz(cl_cdkey_checksum, querydata + 16, 4 +1);

			if(CL_CDKeyValidate(cl_cdkey, cl_cdkey_checksum))
			{
				RegCloseKey(hKey);
				return;
			}
		}
		RegCloseKey(hKey);
	}

	memset(cl_cdkey, ' ', 16);
	cl_cdkey[16] = '\0';
	memset(cl_cdkey_checksum, ' ', 4);
	cl_cdkey_checksum[4] = '\0';
}


typedef struct
{
	const char *language;
	const char *translations;
}localization_t;

#define localization (*((localization_t*)(0xCC147D4)))

const char* Win_GetLanguage()
{
	return localization.language;
}



/*
============
Com_StringContains
============
*/
char *Com_StringContains( char *str1, char *str2, int casesensitive ) {
	int len, i, j;

	len = strlen( str1 ) - strlen( str2 );
	for ( i = 0; i <= len; i++, str1++ ) {
		for ( j = 0; str2[j]; j++ ) {
			if ( casesensitive ) {
				if ( str1[j] != str2[j] ) {
					break;
				}
			} else {
				if ( toupper( str1[j] ) != toupper( str2[j] ) ) {
					break;
				}
			}
		}
		if ( !str2[j] ) {
			return str1;
		}
	}
	return NULL;
}

wchar_t *Com_StringContainsW( wchar_t *str1, wchar_t *str2, int casesensitive ) {
	int len, i, j;

	len = wcslen( str1 ) - wcslen( str2 );
	for ( i = 0; i <= len; i++, str1++ ) {
		for ( j = 0; str2[j]; j++ ) {
			if ( casesensitive ) {
				if ( str1[j] != str2[j] ) {
					break;
				}
			} else {
				if ( towupper( str1[j] ) != towupper( str2[j] ) ) {
					break;
				}
			}
		}
		if ( !str2[j] ) {
			return str1;
		}
	}
	return NULL;
}




/*
============
Com_Filter
============
*/
int Com_Filter( char *filter, char *name, int casesensitive ) {
	char buf[MAX_TOKEN_CHARS];
	char *ptr;
	int i, found;

	while ( *filter ) {
		if ( *filter == '*' ) {
			filter++;
			for ( i = 0; *filter; i++ ) {
				if ( *filter == '*' || *filter == '?' ) {
					break;
				}
				buf[i] = *filter;
				filter++;
			}
			buf[i] = '\0';
			if ( strlen( buf ) ) {
				ptr = Com_StringContains( name, buf, casesensitive );
				if ( !ptr ) {
					return qfalse;
				}
				name = ptr + strlen( buf );
			}
		} else if ( *filter == '?' )      {
			filter++;
			name++;
		} else if ( *filter == '[' && *( filter + 1 ) == '[' )           {
			filter++;
		} else if ( *filter == '[' )      {
			filter++;
			found = qfalse;
			while ( *filter && !found ) {
				if ( *filter == ']' && *( filter + 1 ) != ']' ) {
					break;
				}
				if ( *( filter + 1 ) == '-' && *( filter + 2 ) && ( *( filter + 2 ) != ']' || *( filter + 3 ) == ']' ) ) {
					if ( casesensitive ) {
						if ( *name >= *filter && *name <= *( filter + 2 ) ) {
							found = qtrue;
						}
					} else {
						if ( toupper( *name ) >= toupper( *filter ) &&
							 toupper( *name ) <= toupper( *( filter + 2 ) ) ) {
							found = qtrue;
						}
					}
					filter += 3;
				} else {
					if ( casesensitive ) {
						if ( *filter == *name ) {
							found = qtrue;
						}
					} else {
						if ( toupper( *filter ) == toupper( *name ) ) {
							found = qtrue;
						}
					}
					filter++;
				}
			}
			if ( !found ) {
				return qfalse;
			}
			while ( *filter ) {
				if ( *filter == ']' && *( filter + 1 ) != ']' ) {
					break;
				}
				filter++;
			}
			filter++;
			name++;
		} else {
			if ( casesensitive ) {
				if ( *filter != *name ) {
					return qfalse;
				}
			} else {
				if ( toupper( *filter ) != toupper( *name ) ) {
					return qfalse;
				}
			}
			filter++;
			name++;
		}
	}
	return qtrue;
}

int Com_FilterW( wchar_t *filter, wchar_t *name, int casesensitive ) {
	wchar_t buf[MAX_TOKEN_CHARS];
	wchar_t *ptr;
	int i, found;

	while ( *filter ) {
		if ( *filter == '*' ) {
			filter++;
			for ( i = 0; *filter; i++ ) {
				if ( *filter == '*' || *filter == '?' ) {
					break;
				}
				buf[i] = *filter;
				filter++;
			}
			buf[i] = '\0';
			if ( wcslen( buf ) ) {
				ptr = Com_StringContainsW( name, buf, casesensitive );
				if ( !ptr ) {
					return qfalse;
				}
				name = ptr + wcslen( buf );
			}
		} else if ( *filter == '?' )      {
			filter++;
			name++;
		} else if ( *filter == '[' && *( filter + 1 ) == '[' )           {
			filter++;
		} else if ( *filter == '[' )      {
			filter++;
			found = qfalse;
			while ( *filter && !found ) {
				if ( *filter == ']' && *( filter + 1 ) != ']' ) {
					break;
				}
				if ( *( filter + 1 ) == '-' && *( filter + 2 ) && ( *( filter + 2 ) != ']' || *( filter + 3 ) == ']' ) ) {
					if ( casesensitive ) {
						if ( *name >= *filter && *name <= *( filter + 2 ) ) {
							found = qtrue;
						}
					} else {
						if ( towupper( *name ) >= towupper( *filter ) &&
							 towupper( *name ) <= towupper( *( filter + 2 ) ) ) {
							found = qtrue;
						}
					}
					filter += 3;
				} else {
					if ( casesensitive ) {
						if ( *filter == *name ) {
							found = qtrue;
						}
					} else {
						if ( towupper( *filter ) == towupper( *name ) ) {
							found = qtrue;
						}
					}
					filter++;
				}
			}
			if ( !found ) {
				return qfalse;
			}
			while ( *filter ) {
				if ( *filter == ']' && *( filter + 1 ) != ']' ) {
					break;
				}
				filter++;
			}
			filter++;
			name++;
		} else {
			if ( casesensitive ) {
				if ( *filter != *name ) {
					return qfalse;
				}
			} else {
				if ( towupper( *filter ) != towupper( *name ) ) {
					return qfalse;
				}
			}
			filter++;
			name++;
		}
	}
	return qtrue;
}


/*
============
Com_FilterPath
============
*/
int Com_FilterPath( char *filter, char *name, int casesensitive ) {
	int i;
	char new_filter[MAX_QPATH];
	char new_name[MAX_QPATH];

	for ( i = 0; i < MAX_QPATH - 1 && filter[i]; i++ ) {
		if ( filter[i] == '\\' || filter[i] == ':' ) {
			new_filter[i] = '/';
		} else {
			new_filter[i] = filter[i];
		}
	}
	new_filter[i] = '\0';
	for ( i = 0; i < MAX_QPATH - 1 && name[i]; i++ ) {
		if ( name[i] == '\\' || name[i] == ':' ) {
			new_name[i] = '/';
		} else {
			new_name[i] = name[i];
		}
	}
	new_name[i] = '\0';
	return Com_Filter( new_filter, new_name, casesensitive );
}

int Com_FilterPathW( wchar_t *filter, wchar_t *name, int casesensitive ) {
	int i;
	wchar_t new_filter[MAX_QPATH];
	wchar_t new_name[MAX_QPATH];

	for ( i = 0; i < MAX_QPATH - 1 && filter[i]; i++ ) {
		if ( filter[i] == '\\' || filter[i] == ':' ) {
			new_filter[i] = '/';
		} else {
			new_filter[i] = filter[i];
		}
	}
	new_filter[i] = '\0';
	for ( i = 0; i < MAX_QPATH - 1 && name[i]; i++ ) {
		if ( name[i] == '\\' || name[i] == ':' ) {
			new_name[i] = '/';
		} else {
			new_name[i] = name[i];
		}
	}
	new_name[i] = '\0';
	return Com_FilterW( new_filter, new_name, casesensitive );
}

/*
================
Com_RealTime
================
*/
int Com_RealTime( qtime_t *qtime ) {
	__time64_t t;
	struct tm *tms;

	t = _time64( NULL );
	if ( !qtime ) {
		return t;
	}
	tms = _localtime64( &t );
	if ( tms ) {
		qtime->tm_sec = tms->tm_sec;
		qtime->tm_min = tms->tm_min;
		qtime->tm_hour = tms->tm_hour;
		qtime->tm_mday = tms->tm_mday;
		qtime->tm_mon = tms->tm_mon;
		qtime->tm_year = tms->tm_year;
		qtime->tm_wday = tms->tm_wday;
		qtime->tm_yday = tms->tm_yday;
		qtime->tm_isdst = tms->tm_isdst;
	}
	return t;
}


const char *__cdecl Com_GetBuildDisplayName()
{
	return "Call of Duty 4 X";
}
