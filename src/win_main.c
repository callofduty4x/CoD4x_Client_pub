#include "q_shared.h"
#include "qcommon.h"
#include "win_sys.h"
#include "client.h"
#include "crc.h"
#include "crashrpt/TinyCrashRpt.h"

#include <stdlib.h>
#include <mmsystem.h>
#include <windows.h>
#include <winbase.h>
#include <direct.h>
#include <shellapi.h>
#include <shlobj.h>
#include <psapi.h>
#include <stdbool.h>

#define splashScreen_HWND *((HWND*)(splashScreen_HWND_ADDR))
#define sys_cmdline ((char*)(sys_cmdline_ADDR))
#define sys_timeBase *((int*)(sys_timeBase_ADDR))
#define sys_timeInitialized *((qboolean*)(sys_timeInitialized_ADDR))
#define sys_processSemaphoreFile (char*)(0xCC15FE4)
#define localization (*((localization_t*)(0xCC147D4)))

// Called from launcher
__declspec(dllexport) const char* QDECL GetCoD4xVersion(void) {
    return UPDATE_VERSION_NUM;
}

DWORD *threadHandles = (DWORD *)0x14E89A8; //array of 12

const char *__cdecl Sys_GetCurrentThreadName()
{
	return "Unknown"; //Help
  //return s_threadNames[Sys_GetThreadContext()];
}

void Sys_EventLoop(){
	MSG msg;

	// pump the message loop
	while ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) {
		if ( !GetMessage( &msg, NULL, 0, 0 ) ) {
			Com_Quit_f();
		}
		// save the msg time, because wndprocs don't have access to the timestamp
		g_wv.sysMsgTime = msg.time;

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

}

void Sys_GetLastErrorAsString(char* errbuf, int len)
{
	int lastError = GetLastError();
	if(lastError != 0)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPSTR)errbuf, len -1, NULL);
	}else{
		Q_strncpyz(errbuf, "Unknown Error", len);
	}
}

#define MAX_XCRITICAL_SECTIONS 3

LPCRITICAL_SECTION sys_criticalSection = (LPCRITICAL_SECTION)(sys_criticalSections_ADDR);
CRITICAL_SECTION sys_xCriticalSection[MAX_XCRITICAL_SECTIONS];
void Sys_InitializeCriticalSections()
{
	int i;
	for(i = 0; i < 20; i++)
	{
		InitializeCriticalSection(&sys_criticalSection[i]);
	}
	for(i = 0; i < MAX_XCRITICAL_SECTIONS; i++)
	{
		InitializeCriticalSection(&sys_xCriticalSection[i]);
	}
}

void Sys_EnterCriticalSection(int section)
{
	if(section < 20)
		EnterCriticalSection(&sys_criticalSection[section]);
	else
		EnterCriticalSection(&sys_xCriticalSection[section -20]);
}

void Sys_LeaveCriticalSection(int section)
{
	if(section < 20)
		LeaveCriticalSection(&sys_criticalSection[section]);
	else
		LeaveCriticalSection(&sys_xCriticalSection[section -20]);
}


static qboolean mainthread_lock;
static HANDLE mainThreadSuspended;
static HMODULE dllHandle;

/* Called once on initialization */
qboolean Sys_InitializeGlobalCriticalSection()
{
	mainThreadSuspended = CreateEvent(NULL,FALSE,FALSE,"MainThreadSuspendingOnBeginFrame");
	if(mainThreadSuspended)
	{
		return qtrue;
	}
	return qfalse;
}

/* Called by mainthread just before the usual frame begins */
void Sys_MainThreadCheckForGlobalCriticalSection()
{

	if(!mainthread_lock)
	{
		return;
	}
	SetEvent(mainThreadSuspended);
	WaitForSingleObject(mainThreadSuspended, INFINITE);

}

/* Called by every other thread which wants to do thread unsafe operations the mainthread can usually do only */
/* Never call it twice without prior leaving */
void Sys_EnterGlobalCriticalSection()
{

	Sys_EnterCriticalSection(CRIT_WAITFORMAINTHREADSUSPENDED);
	if(mainthread_lock)
	{
		Com_Error(ERR_FATAL, "Sys_EnterCriticalSectionGlobal called twice without leaving");
	}
	mainthread_lock = qtrue;
	WaitForSingleObject(mainThreadSuspended, INFINITE);
}

void Sys_LeaveGlobalCriticalSection()
{
	mainthread_lock = qfalse;
	SetEvent(mainThreadSuspended);
	Sys_LeaveCriticalSection(CRIT_WAITFORMAINTHREADSUSPENDED);

}


qboolean Sys_CreateNewThread(void* (*ThreadMain)(void*), threadid_t *tid, void* arg)
{
	char errMessageBuf[512];
	DWORD lastError;
	HANDLE thid = CreateThread(	NULL, // LPSECURITY_ATTRIBUTES lpsa,
								0, // DWORD cbStack,
								(LPTHREAD_START_ROUTINE)ThreadMain, // LPTHREAD_START_ROUTINE lpStartAddr,
								arg, // LPVOID lpvThreadParm,
								0, // DWORD fdwCreate,
								tid );
	if(thid == NULL)
	{
		lastError = GetLastError();
		if(lastError != 0)
		{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPSTR)errMessageBuf, sizeof(errMessageBuf) -1, NULL);
			Com_PrintError(CON_CHANNEL_SYSTEM, "Failed to start thread with error: %s\n", errMessageBuf);
		}else{
			Com_PrintError(CON_CHANNEL_SYSTEM, "Failed to start thread!\n");
		}
		return qfalse;
	}
	CloseHandle(thid);
	return qtrue;
}



void preInitError(const char* error){
	MessageBoxA( 0, error, "Call of Duty 4 - Modern Warfare - Fatal Error", 0x10 );
	exit(1);
}

byte Sys_IsMainThread(){

	if(threadHandles[0] == GetCurrentThreadId()){
		return qtrue;
	}
	return qfalse;
}

typedef int (__stdcall *MSGBOXAAPI)(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

int MessageBoxTimeoutA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds)
{
    static MSGBOXAAPI MsgBoxTOA = NULL;

    if (!MsgBoxTOA)
    {
        HMODULE hUser32 = GetModuleHandleA("user32.dll");
        if (hUser32)
        {
            MsgBoxTOA = (MSGBOXAAPI)GetProcAddress(hUser32, "MessageBoxTimeoutA");
            //fall through to 'if (MsgBoxTOA)...'
        }
        else
        {
            //stuff happened, add code to handle it here
            return MessageBoxA(hWnd, lpText, lpCaption, uType);
        }
    }

    if (MsgBoxTOA)
    {
        return MsgBoxTOA(hWnd, lpText, lpCaption,
              uType, wLanguageId, dwMilliseconds);
    }

    return 0;
}


/*
================
Sys_Milliseconds
================
*/

int Sys_Milliseconds(void){
	int sys_curtime;

	if( !sys_timeInitialized){
		sys_timeBase = timeGetTime();
		sys_timeInitialized = qtrue;
	}
	sys_curtime = timeGetTime() - sys_timeBase;
	return sys_curtime;
}


/*
================
Sys_RandomBytes
================
*/
qboolean Sys_RandomBytes( byte *string, int len )
{
	HCRYPTPROV  prov;

	if( !CryptAcquireContext( &prov, NULL, NULL,
		PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )  {

		return qfalse;
	}

	if( !CryptGenRandom( prov, len, (BYTE *)string ) )  {
		CryptReleaseContext( prov, 0 );
		return qfalse;
	}
	CryptReleaseContext( prov, 0 );
	return qtrue;
}

void Sys_CreateMainWindowClass(){
/*
		WNDCLASS wc;

		memset( &wc, 0, sizeof( wc ) );

		wc.style         = 0;
		wc.lpfnWndProc   = MainWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = g_wv.hInstance;
		wc.hIcon         = LoadIconA( g_wv.hInstance, MAKEINTRESOURCE( IDI_ICON1 ) );
		wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
		//wc.hbrBackground = (void *)COLOR_GRAYTEXT;
		wc.lpszMenuName  = 0;
		wc.lpszClassName = WINDOW_CLASS_NAME;

		if ( !RegisterClass( &wc ) )
			Com_Error( ERR_FATAL, "EXE_ERR_COULDNT_REGISTER_WINDOW" );
	*/


	WNDCLASSEXA wndClass;

	memset(&wndClass, 0, sizeof( wndClass ));
	wndClass.cbSize = sizeof( wndClass );
	wndClass.lpfnWndProc = (WNDPROC)MainWndProc;
	wndClass.hInstance = g_wv.hInstance;
	wndClass.hIcon = LoadIconA(g_wv.hInstance, (LPCSTR)1);
	wndClass.hCursor = LoadCursorA(0, (LPCSTR)0x7F00);
	wndClass.hbrBackground = CreateSolidBrush(0);
	wndClass.lpszClassName = "CoD4";
	if ( !RegisterClassExA(&wndClass) )
	{
		Com_Error(0, "EXE_ERR_COULDNT_REGISTER_WINDOW");
	}
}

void Sys_LoadModules(HINSTANCE hInstance){
    HINSTANCE base;
    FARPROC proc;
    int i, copylen;
    char moduledir[1024];
    char mss32path[1024];
    char miles32path[1024];
    static qboolean loaded = qfalse;
    char* find;

    if(loaded == qtrue)
        return;

    copylen = GetModuleFileNameA(hInstance, moduledir, sizeof(moduledir));

    if (copylen < 1 || strrchr(moduledir, '\\') == NULL)
    {
        Q_strncpyz(miles32path, "miles32.dll", sizeof(miles32path));
    } else {
        find = strrchr(moduledir, '\\');
        *find = '\0';
        Com_sprintf(miles32path, sizeof(miles32path), "%s\\miles32.dll", moduledir);
    }


    base = LoadLibraryA(miles32path);

    if(!base){
        preInitError("Error loading module mss32.dll\n");
    }
    proc = GetProcAddress(base, "_AIL_set_DirectSound_HWND@8");
    if(!proc)
        preInitError("No entry point for procedure _AIL_set_DirectSound_HWND\n");

    mss.AIL_set_DirectSound_HWND_int = (void*)proc;
}

//================================================================

static char exeFilename[ MAX_STRING_CHARS ] = { 0 };
static wchar_t dllFilename[ MAX_STRING_CHARS ] = { 0 };

/*
=================
Sys_SetExeFile
=================
*/
void Sys_SetExeFile(const char *filepath)
{
	Q_strncpyz(exeFilename, filepath, sizeof(exeFilename));
}

/*
=================
Sys_SetDllFile
=================
*/
void Sys_SetDllFile(const wchar_t *filepath)
{
	Q_strncpyzUni(dllFilename, filepath, sizeof(dllFilename));
}

/*
=================
Sys_ExeFile
=================
*/
const char* Sys_ExeFile( void )
{
	return exeFilename;
}

/*
=================
Sys_DllFile
=================
*/
const wchar_t* Sys_DllFile( void )
{
	return dllFilename;
}


/*
=================
Sys_Print
=================
*/
void Sys_Print( const char *msg )
{
	if(Sys_IsMainThread())
		Conbuf_AppendText( msg );

}



/*
=================
Sys_In_Restart_f

Restart the input subsystem
=================
*/
void Sys_In_Restart_f( void ) {
	IN_Shutdown();
	IN_Init();
}


/*
=================
Sys_Net_Restart_f

Restart the network subsystem
=================
*/
void Sys_Net_Restart_f( void ) {
	NET_Restart_f();
}

/*
================
Sys_GetCurrentUser
================
*/
const char *Sys_GetCurrentUser( void )
{

	static char s_userName[1024];
	unsigned long size = sizeof( s_userName );

	if( !GetUserName( s_userName, &size ) || !s_userName[0] )
	{
		Q_strncpyz( s_userName, "Unknown Player", sizeof(s_userName) );
	}

	return s_userName;
}

int Sys_TimeGetTime()
{
	return timeGetTime();
}

/*
================
Sys_Init

Called after the common systems (cvars, files, etc)
are initialized
================
*/

void Sys_Init( void ) {
	OSVERSIONINFOA osversion;
	// make sure the timer is high precision, otherwise
	// NT gets 18ms resolution
	timeBeginPeriod( 1 );

	Cmd_AddCommand( "in_restart", Sys_In_Restart_f );
	osversion.dwOSVersionInfoSize = sizeof( osversion );

	if ( !GetVersionEx( &osversion ) ) {
		Sys_Error( "Couldn't get OS info" );
	}

	if ( osversion.dwMajorVersion < 4 ) {
		Sys_Error( va("%s requires Windows version 4 or greater", LONG_PRODUCT_NAME));
	}
	if ( osversion.dwPlatformId == VER_PLATFORM_WIN32s ) {
		Sys_Error( va("%s doesn't run on Win32s", LONG_PRODUCT_NAME));
	}
	//
	// figure out our CPU
	//
	Com_Printf(CON_CHANNEL_SYSTEM, "CPU is: %s\n", Cvar_GetVariantString("sys_cpuName"));
	Com_Printf(CON_CHANNEL_SYSTEM, "Measured CPU speed is %s GHz\n", Cvar_GetVariantString("sys_cpuGHz"));
	Com_Printf(CON_CHANNEL_SYSTEM, "Total CPU performance is estimated as %s\n", Cvar_GetVariantString("sys_configureGHz"));
	Com_Printf(CON_CHANNEL_SYSTEM, "System memory is %s MB (capped at 1 GB)\n", Cvar_GetVariantString("sys_sysMB"));
	Com_Printf(CON_CHANNEL_SYSTEM, "Video card is \"%s\"\n", Cvar_GetVariantString("sys_gpu"));
	Com_Printf(CON_CHANNEL_SYSTEM, "Streaming SIMD Extensions (SSE) support: %s\n", Cvar_GetVariantString("sys_SSE"));

	Cvar_RegisterString( "username", Sys_GetCurrentUser(), CVAR_ROM, "The current username" );
}

char sys_cwd[1024];

void Sys_Cwd(char* path, int len)
{
	Q_strncpyz(path, sys_cwd, len);
}

void Sys_SetupCwd(  )
{
	//char path[1024];
	//char filepath[1024];
	char* cut;

	/* DLL planting vulnerable...
	_getcwd(path, sizeof(path));
	// Test for file: path\main\iw_00.iwd
	FS_BuildOSPathForThread(path, "main", "iw_00.iwd", filepath, 0);
	if(FS_FileExistsOSPath( filepath ))
	{
		Q_strncpyz(sys_cwd, path, sizeof(sys_cwd));
		return;
	}
	*/

	Q_strncpyz(sys_cwd, Sys_ExeFile(), sizeof(sys_cwd));
	cut = strrchr(sys_cwd, PATH_SEP);
	if(cut != NULL)
	{
		*cut = '\0';
		SetCurrentDirectory(sys_cwd);
	}
}

typedef struct
{
	const char* language;
	const char* translations;
}localization_t;


void Sys_InitLocalization()
{
  /* Function does depend on Sys_SetupCwd() */
  char* buf, *split;
  static char language[32];
  char path[1024];
  char cwd[1024];

  localization.language = NULL;
  localization.translations = NULL;

  Sys_Cwd(cwd, sizeof(cwd));

  Com_sprintf(path, sizeof(path), "%s%c%s", cwd, PATH_SEP, "localization.txt");

  if(FS_ReadFileOSPath( path, (void**)&buf ) < 1)
  {
	localization.language = NULL;
	localization.translations = NULL;
	return;
  }
  Q_strncpyz(language, buf, sizeof(language));
  FS_FreeFile(buf);
  localization.language = language;

  split = strchr(language, '\n');
  if(split != NULL)
  {
	*split = '\0';
	localization.translations = split +1;
  }

  split = strchr(language, '\r');
  if(split != NULL)
  {
	*split = '\0';
  }
}

/* Needs 256 wchar_t characters */
const wchar_t* Sys_DllPath(wchar_t* path)
{
	char protocolstring[128];
	Com_sprintf(protocolstring, sizeof(protocolstring), "cod4x_%03u", PROTOCOL_VERSION);
	FS_BuildOSPathForThreadUni(FS_GetSavePath(), "bin", protocolstring, path, 0);
	return path;
}



char UPDATE_VERSION[64];
char sys_restartCmdLine[4096];
void Sys_RestartProcessOnExit( );
qboolean Sys_DetectNvidiaD3d9Wrap();

void Sys_MitigateNvidiaD3d9Wrap();
void CleanKernel32Funcs();

void Sys_SetRestartParams(const char* params)
{
	Q_strncpyz(sys_restartCmdLine, params, sizeof(sys_restartCmdLine));
}
//=======================================================================

void Sys_CleanUpNvd3d9Wrap()
{
	static qboolean nvd3d9wrapcleanedup = 0;

	if(nvd3d9wrapcleanedup == qfalse && Sys_DetectNvidiaD3d9Wrap())
	{
		//MessageBoxA(NULL, "NVD3D9Wrap detected", "NVD3D9Wrap detected", MB_OK);
		Sys_MitigateNvidiaD3d9Wrap();
		nvd3d9wrapcleanedup = qtrue;
	}
}


void Sys_FixUpThirdPartyModules()
{
	//Sys_CleanUpNvd3d9Wrap();
}

qboolean Sys_AlreadyRunning(qboolean destroy)
{
	char varname[1024];
	static HANDLE hMutexOneInstance;
	qboolean alreadyRunning;

	uint32_t crc = crc32_16bytes(Sys_ExeFile(), strlen(Sys_ExeFile()), 0);

	Com_sprintf(varname, sizeof(varname), "Global\\%X-6-BZ34-1337-7331-IW3", crc);

	if(destroy)
	{
		if(hMutexOneInstance == NULL)
		{
			return 1;
		}
		hMutexOneInstance = NULL;
		CloseHandle(hMutexOneInstance);
		return 1;
	}

	hMutexOneInstance = CreateMutexA( NULL, TRUE, varname);
	alreadyRunning = (GetLastError() == ERROR_ALREADY_EXISTS);

	if (hMutexOneInstance != NULL)
    {
        ReleaseMutex(hMutexOneInstance);
    }
	return alreadyRunning;
}

__stdcall void ExcHndlInit();

__declspec( dllexport ) int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){

	char cwd[256];
	char lpFilename[MAX_STRING_CHARS];
	int copylen;
	int choice;
	char cmdline[4096];
	char tchr;
	qboolean pbrequested;

#ifndef NDEBUG
	MessageBoxA(NULL, "Attach debugger now", "Waiting for debugger", MB_OK);
#endif

	copylen = GetModuleFileNameA(hInstance, lpFilename, sizeof(lpFilename));
	if(copylen >= (sizeof(lpFilename) -1))
	{
		Sys_SetExeFile( "" );
	}else{
		Sys_SetExeFile( lpFilename );
	}

	Sys_SetupCwd( );
	SetDllDirectoryA("");

	Sys_InitializeCriticalSections();
	if(!Sys_InitializeGlobalCriticalSection())
	{
		preInitError("Couldn't create GlobalCriticalSection object");
		return -1;
	}

	Patch_MainModule(Patch_Other);

	Sys_InitMainThread();

	Sys_FixUpThirdPartyModules();

	Sys_InitLocalization();

    Sys_LoadModules(hInstance);

	Com_sprintf(UPDATE_VERSION, sizeof(UPDATE_VERSION), "%s", UPDATE_VERSION_NUM);

	if(!strstr(lpCmdLine, "allowdupe") && !strstr(lpCmdLine, "dedicated") && !strstr(lpCmdLine, "installupdatefiles") && !strstr(lpCmdLine, "ui_playerProfileAlreadyChosen"))
	{
		while(Sys_AlreadyRunning(qfalse))
		{
			choice = MessageBoxTimeoutA(NULL, "An copy of CoD4 is already running", "CoD4 is already running",
										MB_ABORTRETRYIGNORE | MB_DEFBUTTON1 | MB_SETFOREGROUND | MB_ICONERROR,
										0, 60000);
			if(choice == IDRETRY)
			{
				Sys_AlreadyRunning(qtrue);
				continue;
			}
			if(choice == IDIGNORE)
			{
				break;
			}
			Sys_AlreadyRunning(qtrue);
			return -1;
		}

		if(strstr(lpCmdLine, "+save")){
			if(!Sys_CheckCrashOrRerun()){
				*((DWORD*)(0xcc147d4)) = 0;
				*((DWORD*)(0xcc147d8)) = 0;
				Sys_AlreadyRunning(qtrue);
				return(0);
			}
		}
	}


	Com_InitParse();
	Cvar_Init();

	*(double*)(0xCC11EF8) = InitTiming() * 1000.0;

	Sys_GetHardwareInfo();

	g_wv.hInstance = hInstance;

	//Clean the commandline from the executable path so no weird error show up
	const char* c = lpCmdLine;
	cmdline[0] = '\0';
	while(*c == ' ' && *c != '\0')
	{
		++c;
	}
	if(*c == '\"')
	{
		++c;
		while(*c != '\"' && *c != '\0')
		{
			++c;
		}
		if(*c != '\0')
		{
			++c;
			if(*c == ' ')
			{
				++c;
			}
		}
		Q_strncpyz( cmdline, c, sizeof(cmdline));
	}else{
		Q_strncpyz( cmdline, lpCmdLine, sizeof(cmdline));
	}

	if(strstr(lpCmdLine, "+nosplash") == NULL)
	{
		Sys_CreateSplashWindow();

		if(splashScreen_HWND){
			ShowWindow(splashScreen_HWND ,SW_SHOW );
			UpdateWindow(splashScreen_HWND);
		}
	}


	Sys_CreateMainWindowClass();




	if(strstr(lpCmdLine, "+sysconsole") != NULL)
	{
		Sys_CreateConsole( qtrue );
	}else{
		Sys_CreateConsole( qfalse );
	}

	SetErrorMode(1);

	Sys_Milliseconds();

	Com_Init( cmdline );


	Sys_FixUpThirdPartyModules();
	if(!com_dedicated || !com_dedicated->integer)
		Cbuf_AddText("readStats\n");


	Sys_Cwd( cwd, sizeof(cwd));

	Com_Printf(CON_CHANNEL_SYSTEM, "Working directory: %s\n", cwd);

	if((!com_dedicated || !com_dedicated->integer))
	{
		pbrequested = Cvar_GetBool("cl_punkbuster");
		PBCL_GlobalConstructor();
		if(!PbClientInitialize(g_wv.hInstance)){
			if(pbrequested)
			{
				Com_SetErrorMessage("MPUI_NOPUNKBUSTER");
			}
		}
	}

	pbrequested = Cvar_GetBool("sv_punkbuster");
	PBSV_GlobalConstructor();
	if(!PbServerInitialize()){
		if(pbrequested)
		{
			Com_PrintError(CON_CHANNEL_SYSTEM, "Unable to initialize PunkBuster. PunkBuster is disabled\n");
			Com_SetErrorMessage("MPUI_NOPUNKBUSTER");
		}
	}

	SetFocus(g_wv.hWnd);

	if(com_dedicated && com_dedicated->integer)
		Patch_MainModule(Patch_Dedicated);

	CleanKernel32Funcs();

	FS_ValidateIwdFiles();


	while(qtrue)
	{
		if((com_dedicated && com_dedicated->integer) || g_wv.isMinimized){
			Sleep(5);
		}
		Sys_MainThreadCheckForGlobalCriticalSection();
		Com_Frame();
		PbServerProcessEvents(0);

		if(!com_dedicated || !com_dedicated->integer)
		{
			PbClientProcessEvents();



		}
	}
	return 0;
}

void __iw3mp_security_init_cookie();
int __iw3mp_tmainCRTStartup();

/* We have a too small stacksize on mainthread.
This trick will replace the mainthread with a new mainthread which has an large enough stack */

int entrypoint()
{
	static qboolean mainThreadCreated;

	if(!mainThreadCreated)
	{
		mainThreadCreated = qtrue;

		if (CreateThread(NULL, //Choose default security
							1024*1024*12, //stack size
							(LPTHREAD_START_ROUTINE)&entrypoint, //Routine to execute
							NULL, //Thread parameter
							0, //Immediately run the thread
							NULL //Thread Id
		) == NULL)
		{
			return -1;
		}
		return 0;
	}

	HINSTANCE hInstance = (HINSTANCE)0x400000;
	HINSTANCE hPrevInstance = (HINSTANCE)0x0;
	char lpCmdLine[1024];

	Q_strncpyz(lpCmdLine, GetCommandLineA(), sizeof(lpCmdLine));

	int r = WinMain(hInstance, hPrevInstance, lpCmdLine, 0);
	exit(r);
}

BOOL WINAPI __declspec(dllexport) DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID pvReserved)
{
	wchar_t lpFilename[MAX_STRING_CHARS];
	int copylen;

	if(dwReason == DLL_PROCESS_ATTACH)
	{
		dllHandle = hInstance;
	}
	if(dwReason == DLL_PROCESS_ATTACH && *(int*)0x690429 != 0 /* Will bbe 1 due to writeprocessmemory() on temp install */)
	{
		copylen = GetModuleFileNameW(hInstance, lpFilename, sizeof(lpFilename) / sizeof(wchar_t));
		if(copylen >= (sizeof(lpFilename) -1))
		{
			Sys_SetDllFile( L"" );
		}else{
			Sys_SetDllFile( lpFilename );
		}

		if(Com_HunkInitialized() == qfalse)
		{
            Patch_MainModule(Patch_WinMainEntryPoint);
		}

	}
	return TRUE;
}

void __cdecl Win_ShutdownLocalization()
{
  *(int*)0xCC147D4 = 0;
  *(int*)0xCC147D8 = 0;
}




void Sys_Quit()
{

    Sys_EnterCriticalSection( 2 );
    timeEndPeriod(1u);
    IN_Shutdown(); //sub_575E90();
    Key_Shutdown(); //sub_4685D0();
	Sys_NormalExit();
	Win_ShutdownLocalization( );
	/* Function does not release all elements properly in some cases */
    //Dvar_Shutdown(); //sub_56B7D0();
    Cmd_Shutdown();
	//Con_Shutdown();
	//Com_ShutdownEvents();
	Sys_DestroyConsole();
    sub_477210();
	Sys_AlreadyRunning(qtrue);
    Sys_RestartProcessOnExit( );
	ExitProcess(0);
  //doexit(0, 0, 0);

}

void Sys_NormalExit()
{
    DeleteFileA(sys_processSemaphoreFile);
}

void Sys_DestroySplashWindow()
{
    if ( splashScreen_HWND )
    {
        ShowWindow(splashScreen_HWND, 0);
        DestroyWindow(splashScreen_HWND);
        splashScreen_HWND = 0;
    }
}

void Sys_HideSplashWindow()
{
	if ( splashScreen_HWND )
	{
		ShowWindow(splashScreen_HWND, 0);
	}
}



void Sys_RestartProcessOnExit()
{
	void* HWND = NULL;
	char displayMessageBuf[1024];
	const char *exefile;
    SHELLEXECUTEINFO sei;
	int pid;

	if ( sys_restartCmdLine[0] == '\0' )
	{
		/* No restart (normal exit) */
		return;
	}


	exefile = Sys_ExeFile();
	if(strlen(exefile) < 9)
	{
		/* Can not restart (normal exit) */
		return;
	}

	memset(&sei, 0, sizeof(sei));
	sei.cbSize = sizeof(sei);
    sei.lpVerb = "open";
    sei.lpFile = exefile;
	sei.lpParameters = sys_restartCmdLine;
    sei.hwnd = HWND;
    sei.nShow = SW_NORMAL;
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;

	if (!ShellExecuteExA(&sei))
	{
		FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						0, GetLastError(), 0x400, displayMessageBuf, sizeof(displayMessageBuf), 0);

		MessageBoxA(HWND, va("ShellExec of commandline: %s %s has failed.\nError: %s\n" , exefile, sys_restartCmdLine, displayMessageBuf), "Call of Duty - Error", MB_OK | MB_ICONERROR);
		return;
	}
	if(sei.hProcess == NULL)
	{
		return;
	}
	pid = GetProcessId(sei.hProcess);
    AllowSetForegroundWindow(pid);
}

void Sys_CreateErrorBox(const char* text)
{
  MessageBoxA(GetActiveWindow(), text, "Error", 0x10u);
}

void __cdecl Sys_Error(const char *fmt, ...)
{
	struct tagMSG Msg; // [sp+Ch] [bp-101Ch]@3
	char		errormsg[0x1000];
	va_list		argptr;

    Sys_EnterCriticalSection( 2 );

	Com_SetErrorEntered();

	Sys_SuspendOtherThreads();

	R_DestroyGameWindows();

	va_start (argptr,fmt);
	Q_vsnprintf (errormsg, sizeof(errormsg), fmt, argptr);
	va_end (argptr);

	sub_55A170();

	if ( com_dedicated->integer && Sys_IsMainThread() )
	{
		Sys_ShowConsole(1, qfalse);
		Conbuf_AppendText("\n\n");
		Conbuf_AppendText(errormsg);
		Conbuf_AppendText("\n");
		Sys_CreateErrorBox(errormsg);
		while ( GetMessageA(&Msg, 0, 0, 0) )
		{
		  TranslateMessage(&Msg);
		  DispatchMessageA(&Msg);
		}
	}else{
		Sys_CreateErrorBox(errormsg);
	}
	Sys_AlreadyRunning(qtrue);
	ExitProcess(0);

//	doexit( 0, 0, 0 );
}

void Sys_Mkdir(const char* dir)
{
	_mkdir(dir);
}

void Sys_MkdirUni(const wchar_t* dir)
{
	_wmkdir(dir);
}

qboolean Sys_CopyFileUni(wchar_t* fromOSPath, wchar_t* toOSPath)
{

	if(CopyFileW( fromOSPath, toOSPath, FALSE) == 0)
	{
		return qfalse;
	}
	return qtrue;
}

#ifndef KF_FLAG_CREATE
	#define KF_FLAG_CREATE 0x00008000
#endif

const GUID FOLDERID_LocalAppData = {0xF1B32785, 0x6FBA, 0x4FCF, {0x9D, 0x55, 0x7B, 0x8E, 0x7F, 0x15, 0x70, 0x91}};

HMODULE LoadSystemLibraryA(char* libraryname)
{
	char system32dir[1024];
	char modulepath[1024];
	int len;

	len = GetSystemDirectoryA(system32dir, sizeof(system32dir));

	if (len < 1 || len >= sizeof(system32dir))
	{
		return LoadLibraryA(libraryname);
	}

	Com_sprintf(modulepath, sizeof(modulepath), "%s\\%s", system32dir, libraryname);
	return LoadLibraryA(libraryname);

}


wchar_t* Sys_GetAppDataDir(wchar_t *basepath, int size)
{



	HMODULE hModuleShl = LoadSystemLibraryA("shell32.dll");
	HMODULE hModuleOle = LoadSystemLibraryA("ole32.dll");
	HRESULT (WINAPI *farproc)(const GUID *rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);
	HRESULT (__cdecl *farprocxp)(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);
	void (__cdecl *CoTaskMemFree_t)( void* mem );
	wchar_t xpPath[MAX_OSPATH];
	LPWSTR wszPath = NULL;


	/* Get the AppData folder */
	basepath[0] = L'\0';

	if(hModuleShl)
	{
		farproc = (void*)GetProcAddress(hModuleShl, "SHGetKnownFolderPath");
		farprocxp = (void*)GetProcAddress(hModuleShl, "SHGetFolderPathW");
		if(farproc)
		{

			HRESULT hr = farproc ( &FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &wszPath );
			if ( SUCCEEDED(hr) )
			{
				Q_strncpyzUni(basepath, wszPath, size);
				Q_strcatUni(basepath, size, L"\\CallofDuty4MW");
			}
			if(hModuleOle)
			{
				CoTaskMemFree_t = (void*)GetProcAddress(hModuleShl, "CoTaskMemFree");
				if(CoTaskMemFree_t)
				{
					CoTaskMemFree_t( wszPath );
				}
			}


		}else if(farprocxp){

			HRESULT hr = farprocxp(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, (LPTSTR)xpPath);
			if ( SUCCEEDED(hr) )
			{
				Q_strncpyzUni(basepath, xpPath, size);
				Q_strcatUni(basepath, size, L"\\CallofDuty4MW");
			}

		}else{
			FreeLibrary(hModuleShl);
			return NULL;
		}
		FreeLibrary(hModuleShl);

	}else{
		return NULL;
	}

	if(hModuleOle)
	{
		FreeLibrary(hModuleOle);
	}

	return basepath;
}

void Sys_RegisterCoD4Protocol( )
{
	int status, len;
	HKEY hKey;
	char openpath[MAX_STRING_CHARS];
	char iconpath[MAX_STRING_CHARS];
	const char *exepath = Sys_ExeFile();
	const char *handler = "URL: CoD4 Connect Handler";
	len = strlen(exepath);

	if( len < 5)
	{
		return;
	}


	status = RegCreateKeyA(HKEY_CURRENT_USER, "Software\\Classes\\cod4", &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with protocol cod4");
		return;
	}
    RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)handler, strlen(handler) +1);
	RegSetValueExA(hKey, "URL Protocol", 0, 1u, NULL, 0);
    RegCloseKey(hKey);

	Com_sprintf(iconpath, sizeof(iconpath), "%s,0", exepath);

	status = RegCreateKeyA(HKEY_CURRENT_USER, "Software\\Classes\\cod4\\DefaultIcon", &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with protocol cod4");
		return;
	}
    RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)iconpath, strlen(iconpath) +1);
    RegCloseKey(hKey);


	status = RegCreateKeyA(HKEY_CURRENT_USER, "Software\\Classes\\cod4\\shell\\open\\command", &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with protocol cod4");
		return;
	}

	Com_sprintf(openpath, sizeof(openpath), "\"%s\" +openurl \"%%1\"", exepath);

	RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)openpath, strlen(openpath));
    RegCloseKey(hKey);
}


void Sys_RegisterDemoExt( )
{
	int status, len, size;
	HKEY hKey;
	HKEY hKey2;
	char openpath[MAX_STRING_CHARS];
	char iconpath[MAX_STRING_CHARS];
	const char *exepath = Sys_ExeFile();
	len = strlen(exepath);
	char value[MAX_STRING_CHARS];
	char typepath[MAX_STRING_CHARS];
	char commandpath[MAX_STRING_CHARS];
	char* productIndentifier = "CoD4X.CallofDuty4X.1";
	char buf[] = { "eOIc\\_" };
    int i;

    for(i = 0; i < 6; ++i)
    {
        buf[i] ^= 44;
    }

	if( len < 5)
	{
		return;
	}

	status = RegCreateKeyA(HKEY_CURRENT_USER, "Software\\Classes\\.dm_1", &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with extension .dm_1");
		return;
	}

	size = sizeof(value);

	status = RegQueryValueExA( hKey, "", NULL, NULL, (BYTE*)value, (DWORD*)&size);

	if(status == ERROR_SUCCESS && strstr(value, buf))
	{
		RegDeleteValue(hKey, NULL);
		if(RegOpenKeyA(HKEY_CURRENT_USER, "Software\\Classes", &hKey2) == ERROR_SUCCESS)
		{
			RegDeleteKeyA(hKey2, va("%s\\shell\\open\\command", value));
			RegDeleteKeyA(hKey2, va("%s\\shell\\open", value));
			RegDeleteKeyA(hKey2, va("%s\\shell", value));
			RegDeleteKeyA(hKey2, value);
			RegCloseKey(hKey2);
		}

	}


	status = RegQueryValueExA( hKey, "", NULL, NULL, (BYTE*)value, (DWORD*)&size);

	if(status != ERROR_SUCCESS || value[0] == '\0')
	{
		size = sizeof(value);
		RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)productIndentifier, strlen(productIndentifier) +1);
		if(RegQueryValueExA( hKey, "", NULL, NULL, (BYTE*)value, (DWORD*)&size) != ERROR_SUCCESS || value[0] == '\0' )
		{
			Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with extension .dm_1");
			RegCloseKey(hKey);
			return;
		}
	}
    RegCloseKey(hKey);

	Com_sprintf(typepath, sizeof(typepath), "Software\\Classes\\%s", value);


	if(RegOpenKeyA(HKEY_CURRENT_USER, typepath, &hKey) == ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		if(Q_stricmpn(value, productIndentifier, strlen(productIndentifier)))
		{
		/* Already registered with some else application. Will stop*/
			return;
		}
	}

	Com_sprintf(iconpath, sizeof(iconpath), "%s,0", exepath);

	status = RegCreateKeyA(HKEY_CURRENT_USER, typepath, &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with extension .dm_1");
		return;
	}
    RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)LONG_PRODUCT_NAME, strlen(LONG_PRODUCT_NAME) +1);
    RegSetValueExA(hKey, "CurVer", 0, 1u, (const BYTE*)value, strlen(value) +1);
	RegSetValueExA(hKey, "DefaultIcon", 0, 1u, (const BYTE*)iconpath, strlen(iconpath) +1);
    RegCloseKey(hKey);


	Com_sprintf(commandpath, sizeof(commandpath), "%s\\shell\\open\\command", typepath);

	status = RegCreateKeyA(HKEY_CURRENT_USER, commandpath, &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with extension .dm_1");
		return;
	}
	Com_sprintf(openpath, sizeof(openpath), "\"%s\" +demo \"%%1\" fullpath", exepath);

	size = sizeof(value);

	if(RegQueryValueExA( hKey, "", NULL, NULL, (BYTE*)value, (DWORD*)&size) == ERROR_SUCCESS && !Q_stricmpn(value, openpath, strlen(openpath)) )
	{
		RegCloseKey(hKey);
		return;
	}
	RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)openpath, strlen(openpath));
	RegCloseKey(hKey);
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	Com_Printf(CON_CHANNEL_SYSTEM, "Updated demo-file association record\n");
}


qboolean Sys_DirectoryExistsUni(wchar_t* path)
{
  DWORD dwAttrib = GetFileAttributesW(path);

  if(dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
  {
	return qfalse;
  }
  return qtrue;
}



int Sys_RemoveDirTreeUni(const wchar_t *dir)
{
  HANDLE dhandle;
  qboolean hassep;
  struct _WIN32_FIND_DATAW FindFileData;
  wchar_t FileName[256];
  wchar_t lastchar;

  lastchar = dir[wcslen(dir) - 1];
  if ( lastchar == L'\\' || lastchar == L'/' )
  {
    hassep = 1;
    Com_sprintfUni(FileName, sizeof(FileName), L"%ls*", dir);
  }
  else
  {
    hassep = 0;
	Com_sprintfUni(FileName, sizeof(FileName), L"%ls\\*", dir);
  }
  dhandle = FindFirstFileW(FileName, &FindFileData);
  if ( dhandle == (HANDLE)-1 )
  {
    return _wrmdir(dir) != -1;
  }

  do
  {

    if ( FindFileData.cFileName[0] != L'.' || (FindFileData.cFileName[1] && (FindFileData.cFileName[1] != L'.' || FindFileData.cFileName[2])) )
    {
	    if(hassep)
		{
		  Com_sprintfUni(FileName, sizeof(FileName), L"%ls%ls", dir, FindFileData.cFileName);
        }else{
		  Com_sprintfUni(FileName, sizeof(FileName), L"%ls\\%ls", dir, FindFileData.cFileName);
		}

        if ( FindFileData.dwFileAttributes & 0x10 ? Sys_RemoveDirTreeUni(FileName) == 0 : _wremove(FileName) == -1 )
		{
			FindClose(dhandle);
			return 0;
		}
    }
  }
  while ( FindNextFileW(dhandle, &FindFileData) != 0 );
  FindClose(dhandle);
  return _wrmdir(dir) != -1;
}


int Sys_ListDirectories(const wchar_t* dir, char** list, int limit)
{
  int count, i;
  HANDLE dhandle;
  struct _WIN32_FIND_DATAW FindFileData;
  char *ansifilename;
  wchar_t searchdir[MAX_OSPATH];
  char errorString[1024];

  list[0] = NULL;

  FS_BuildOSPathForThreadUni( dir, "*", "", searchdir, 0 );

  dhandle = FindFirstFileW(searchdir, &FindFileData);
  if ( dhandle == (HANDLE)-1 )
  {
	Sys_GetLastErrorAsString(errorString, sizeof(errorString));
	Com_DPrintf(CON_CHANNEL_SYSTEM,"Sys_ListDirectories: %s\n", errorString);
    return 0;
  }
  count = 0;

  do
  {

    if ( !FindFileData.cFileName[0])
    {
		continue;
	}
	if(FindFileData.cFileName[0] == L'.' && (FindFileData.cFileName[1] == L'.' || !FindFileData.cFileName[1]))
	{
		continue;
	}

	/* is directory ? */
    if ( FindFileData.dwFileAttributes & 0x10)
	{

		if(Q_WIsAnsiString(FindFileData.cFileName) == qfalse)
		{

			//We won't support non ANSI chars
			continue;
		}

		list[count] = malloc(wcslen(FindFileData.cFileName) +1);

		if(list[count] == NULL)
		{
			break;
		}

		ansifilename = list[count];

		/* String copy as ANSI string */
		for(i = 0; FindFileData.cFileName[i]; ++i)
		{
			ansifilename[i] = FindFileData.cFileName[i];
		}
		ansifilename[i] = '\0';
		count++;
	}

  }
  while ( FindNextFileW(dhandle, &FindFileData) != 0 && count < limit -1);

  FindClose(dhandle);

  list[count] = NULL;

  return count;
}

qboolean PatchEntrypoint(HANDLE hProcess, const char* dllfile);

wchar_t* Sys_GetBinDir1k(wchar_t* bindir)
{
    HMODULE hMods[1024];
	DWORD cbNeeded;
    wchar_t szModName[1024];
	int i;

	HANDLE hProcess = GetCurrentProcess( );

	if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
        {
			szModName[0] = 0;
            // Get the full path to the module's file.
            if ( GetModuleFileNameW( hMods[i], szModName, sizeof(szModName) / sizeof(szModName[0])))
            {
                // Print the module name and handle value.
				wchar_t* f = wcsstr(szModName, L"CallofDuty4MW\\bin\\cod4x_");
				if(f)
				{
					FS_DirNameUni(szModName);
					Q_strncpyzUni(bindir, szModName, 1024);
					return bindir;
				}
            }
        }
    }
	return NULL;

}

/*
void* Steam_FindOverlayHandler(HMODULE hModule){

	int i;
	byte *dataptr, *textptr;
	DWORD dwSectionSize;
	DWORD dwSectionBase;
	IMAGE_SECTION_HEADER sectionHeader;

	const char* searchstr = "hookDirect3DCreate9 called\n";

	if (! hModule)
		return NULL;

	// get dos header
	IMAGE_DOS_HEADER dosHeader = *(IMAGE_DOS_HEADER*)(hModule);

	// get nt header
	IMAGE_NT_HEADERS ntHeader = *(IMAGE_NT_HEADERS*)((DWORD)hModule + dosHeader.e_lfanew);

	// iterate through section headers and search for ".text" section
	int nSections = ntHeader.FileHeader.NumberOfSections;

	dwSectionSize = 0;

	for ( i = 0; i < nSections; i++)
	{
		sectionHeader = *(IMAGE_SECTION_HEADER*) ((DWORD)hModule + dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS) + i*sizeof(IMAGE_SECTION_HEADER));

		if (strcmp((char*)sectionHeader.Name,".rdata") == 0)
		{
			// the values you need
			dwSectionBase = (DWORD)hModule + sectionHeader.VirtualAddress;
			dwSectionSize = sectionHeader.Misc.VirtualSize;
			break;
		}
	}

	if(!dwSectionSize)
	{
		return NULL;
	}

	for(i = 0, dataptr = (byte*)dwSectionBase; i < dwSectionSize; ++i, ++dataptr)
	{
		if(dataptr[0] == searchstr[0] && strcmp((char*)dataptr, searchstr) == 0)
		{
			break;
		}
	}
	if(i >= dwSectionSize)
	{
		return NULL;
	}

	dwSectionSize = 0;

	for ( i = 0; i < nSections; i++)
	{
		sectionHeader = *(IMAGE_SECTION_HEADER*) ((DWORD)hModule + dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS) + i*sizeof(IMAGE_SECTION_HEADER));

		if (strcmp((char*)sectionHeader.Name,".text") == 0)
		{
			// the values you need
			dwSectionBase = (DWORD)hModule + sectionHeader.VirtualAddress;
			dwSectionSize = sectionHeader.Misc.VirtualSize;
			break;
		}
	}

	if(!dwSectionSize){
		return NULL;
	}

	for(i = 0, textptr = (byte*)dwSectionBase; i < dwSectionSize; ++i, ++textptr)
	{
		//Push offset = 0x68
		if(*textptr == 0x68)
		{
			++i;
			++textptr;

			if(*(DWORD*)textptr == (DWORD)dataptr)
			{
				break;
			}
		}
	}

	if(i >= dwSectionSize)
	{
		return NULL;
	}

	//Find begin of function - step backwards
	for(i = 0; i < 15; ++i, --textptr)
	{
		if(textptr[0] == 0x55 && textptr[1] == 0x8b && textptr[2] == 0xec)
		{
			break;
		}
	}

	if(i >= 15)
	{
		//Function start not found
		return NULL;
	}

	return textptr;
}


void Sys_SteamSetOverlayhandler()
{
	void (*farproc)( );
	HMODULE hModule = GetModuleHandleA("GameOverlayRenderer");
	if(hModule == NULL)
	{
		return;
	}

	farproc = Steam_FindOverlayHandler(hModule);

	if(farproc == NULL){
		return;
	}

	R_SetDirect3DCreate9Func(farproc);

}
*/

qboolean Sys_DetectNvidiaD3d9Wrap()
{
	// Get a list of all the modules in this process.
/*
	HMODULE hMods[1024];
	DWORD cbNeeded;
	int i;
	char* sep;
    char szModName[MAX_OSPATH];

	HANDLE hProcess = GetCurrentProcess( );

    if( !EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
		return qfalse;
	}

    for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
    {
        // Get the full path to the module's file.
        if ( GetModuleFileNameA( hMods[i], szModName, sizeof(szModName)))
        {
            // Print the module name and handle value.
			sep = strrchr(szModName, '\\');
			if(sep)
			{
				*sep = '\0';
				sep++;
			}else{
				sep = szModName;
			}

			if(sep)
			{

				if(!Q_stricmpn(sep, "nvd3d9wrap", 10))
				{
					return qtrue;
				}

			}
        }
    }
*/
	if(GetModuleHandleA("nvd3d9wrap"))
	{
		return qtrue;
	}

	return qfalse;
}

qboolean IsJumpInstruction(void* adr)
{
	byte* inst = adr;
	if(inst[0] == 0xe9)
	{
		return qtrue;
	}
	return qfalse;
}


qboolean IsHotpatchEntrypoint(void* adr)
{
	byte* inst = adr;

	if(inst[2] != 0x55){
		return qfalse;
	}
	if(inst[3] != 0x8b || inst[4] != 0xec)
	{
		return qfalse;
	}
	return qtrue;

}

qboolean Sys_RemoveHookFromProc(void* topatchproc, byte* replacebytes, int replacebyteslen)
{
	DWORD oldprotect, dummyoldp;

	/* Detect if a hook was really installed */
	if(IsJumpInstruction(topatchproc) == qfalse)
	{
		//No hook found
		return qtrue;
	}

	if(VirtualProtect(topatchproc, replacebyteslen, PAGE_EXECUTE_READWRITE, &oldprotect) == 0)
	{

		return qfalse;
	}
	memcpy(topatchproc, replacebytes, replacebyteslen);

	VirtualProtect(topatchproc, replacebyteslen, oldprotect, &dummyoldp);

	return qtrue;

}


qboolean Sys_RemoveHooksFromModule(const char* libraryname, const char* cleanprocname, const char **tocleanprocs, int numprocs)
{
	void* topatchproc;
	void* cleancmpproc;
	int i;

	HMODULE hModule = GetModuleHandleA(libraryname);
	byte buffer[5];

	if(hModule == NULL)
	{
		return qfalse;
	}

	cleancmpproc = GetProcAddress(hModule, cleanprocname);
	if(cleancmpproc == NULL)
	{
		return qfalse;
	}

	/* Dirty Hook is detected! Removing it... */
	if(IsHotpatchEntrypoint(cleancmpproc) == qfalse)
	{
		return qfalse;
	}

	memcpy(buffer, cleancmpproc, sizeof(buffer));

	for(i = 0; i < numprocs; ++i)
	{
		/* Get the proc addresses of "Direct3DCreate9" for fixing and "DebugSetLevel" to get bytes to copy from */
		topatchproc = GetProcAddress(hModule, tocleanprocs[i]);
		if(topatchproc == NULL){
			continue;
		}

		if(Sys_RemoveHookFromProc(topatchproc, buffer, sizeof(buffer)) == qfalse)
		{
			return qfalse;
		}
	}
	return qtrue;
}

void CleanKernel32Funcs()
{
	const char *kernelbaseprocs[] = {"GetModuleHandleW", "GetModuleHandleA", "GetModuleHandleExW", "GetModuleHandleExA"};

	Sys_RemoveHooksFromModule("KernelBase.dll", "Sleep", kernelbaseprocs, 4);
	Sys_RemoveHooksFromModule("kernel32.dll", "Sleep", kernelbaseprocs, 4);
}



void Sys_MitigateNvidiaD3d9Wrap()
{
	const char *kernelbaseprocs[] = {"LoadLibraryExW", "FreeLibrary", "GetModuleHandleW",
									"GetModuleHandleExW", "GetModuleFilenameExW", "EnumProcessModulesEx", "GetMappedFileNameW",
									"GetModuleInformation", "RegQueryValueExW", "RegDeleteValueW", "RegSetValueExA", "RegSetValueExW"};

	const char *d3d9procs[] = {"Direct3DCreate9", "Direct3DCreate9Ex"};

	if(Sys_RemoveHooksFromModule("KernelBase.dll", "Sleep", kernelbaseprocs, 12) == qfalse)
	{
	//MessageBoxA(NULL, "Failed to disable nvd3d9wrap from KernelBase.dll. The game does maybe crash when Steam is running and nvd3d9wrap.dll is installed.", "Failed to mitigate nvd3d9wrap", MB_OK | MB_ICONWARNING);
		return;
	}

	if(Sys_RemoveHooksFromModule("d3d9.dll", "DebugSetLevel", d3d9procs, 2) == qfalse)
	{
	//MessageBoxA(NULL, "Failed to disable nvd3d9wrap from d3d9.dll. The game does maybe crash when Steam is running and nvd3d9wrap.dll is installed.", "Failed to mitigate nvd3d9wrap", MB_OK | MB_ICONWARNING);
		return;
	}

	//Sys_SteamSetOverlayhandler();

}


void Sys_LockCriticalSection_v1(critSectionLock_t *section)
{
  while ( 1 )
  {
    if ( section->operand1 )
	{
        Sleep(0);
		continue;
	}

    if ( InterlockedIncrement(&section->operand2) == 1 && !section->operand1 )
	{
      break;
	}

	InterlockedDecrement(&section->operand2);
    Sleep(0);
  }
}

void Sys_UnlockCriticalSection_v1(critSectionLock_t *section)
{
	InterlockedDecrement(&section->operand2);
}

const char* Sys_DefaultInstallPath()
{
  HMODULE hModule;
  DWORD len;
  static char exePath[256];

  if ( !exePath[0] )
  {
    hModule = GetModuleHandleA(0);
    if(hModule == NULL)
	{
		Com_Error(ERR_FATAL, "Sys_DefaultInstallPath: Got no HMODULE");
		return exePath;
	}
	len = GetModuleFileNameA(hModule, exePath, 0x100u);
    if ( len == 256 )
    {
      len = 255;
    }
    else if ( len == 0 )
    {
		Com_Error(ERR_FATAL, "Sys_DefaultInstallPath: Got no ModuleFileName");
		return exePath;
    }

    do
    {
      if ( exePath[len] == '\\' )
        break;
      if ( exePath[len] == '/' )
        break;
      if ( exePath[len] == ':' )
        break;

      --len;
    }
    while ( len );
    exePath[len] = 0;

  }
  return exePath;
}


qboolean Sys_IsClientActive()
{
	return g_wv.activeApp;
}


/*
==============================================================

DIRECTORY SCANNING

==============================================================
*/

#define MAX_FOUND_FILES 0x1000

/*
==============
Sys_ListFilteredFiles
==============
*/
void Sys_ListFilteredFiles( const char *basedir, char *subdirs, char *filter, char **list, int *numfiles )
{
	char	search[MAX_OSPATH], newsubdirs[MAX_OSPATH];
	char	filename[MAX_OSPATH];
	intptr_t	findhandle;
	struct _finddata64i32_t findinfo;

	if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
		return;
	}

	if (strlen(subdirs)) {
		Com_sprintf( search, sizeof(search), "%s\\%s\\*", basedir, subdirs );
	}
	else {
		Com_sprintf( search, sizeof(search), "%s\\*", basedir );
	}

	findhandle = _findfirst64i32 (search, &findinfo);
	if (findhandle == -1) {
		return;
	}

	do {
		if (findinfo.attrib & _A_SUBDIR) {

			if (Q_stricmp(findinfo.name, ".") && Q_stricmp(findinfo.name, "..")) {

				if (strlen(subdirs)) {
					Com_sprintf( newsubdirs, sizeof(newsubdirs), "%s\\%s", subdirs, findinfo.name);
				}
				else {
					Com_sprintf( newsubdirs, sizeof(newsubdirs), "%s", findinfo.name);
				}
				Sys_ListFilteredFiles( basedir, newsubdirs, filter, list, numfiles );
			}
		}
		if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
			break;
		}
		Com_sprintf( filename, sizeof(filename), "%s\\%s", subdirs, findinfo.name );
		if (!Com_FilterPath( filter, filename, qfalse ))
			continue;
		list[ *numfiles ] = (char*)CopyString( filename );
		(*numfiles)++;
	} while ( _findnext64i32 (findhandle, &findinfo) != -1 );

	_findclose (findhandle);
}

void Sys_ListFilteredFilesW( const wchar_t *basedir, wchar_t *subdirs, wchar_t *filter, wchar_t **list, int *numfiles )
{
	wchar_t	search[MAX_OSPATH], newsubdirs[MAX_OSPATH];
	wchar_t	filename[MAX_OSPATH];
	intptr_t	findhandle;
	struct _wfinddata64i32_t findinfo;

	if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
		return;
	}

	if (subdirs[0]) {
		Com_sprintfUni( search, sizeof(search), L"%ls\\%ls\\*", basedir, subdirs );
	}
	else {
		Com_sprintfUni( search, sizeof(search), L"%ls\\*", basedir );
	}

	findhandle = _wfindfirst64i32 (search, &findinfo);
	if (findhandle == -1) {
		return;
	}

	do {
		if (findinfo.attrib & _A_SUBDIR) {

			if (wcscmp(findinfo.name, L".") && wcscmp(findinfo.name, L"..")) {

				if (wcslen(subdirs)) {
					Com_sprintfUni( newsubdirs, sizeof(newsubdirs), L"%ls\\%ls", subdirs, findinfo.name);
				}
				else {
					Com_sprintfUni( newsubdirs, sizeof(newsubdirs), L"%ls", findinfo.name);
				}
				Sys_ListFilteredFilesW( basedir, newsubdirs, filter, list, numfiles );
			}
		}
		if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
			break;
		}
		Com_sprintfUni( filename, sizeof(filename), L"%ls\\%ls", subdirs, findinfo.name );
		if (!Com_FilterPathW( filter, filename, qfalse ))
			continue;
		list[ *numfiles ] = _wcsdup( filename );
		(*numfiles)++;
	} while ( _wfindnext64i32 (findhandle, &findinfo) != -1 );

	_findclose (findhandle);
}



/*
==============
strgtr
==============
*/

static qboolean strgtr(const char *s0, const char *s1)
{
	int l0, l1, i;

	l0 = strlen(s0);
	l1 = strlen(s1);

	if (l1<l0) {
		l0 = l1;
	}

	for(i=0;i<l0;i++) {
		if (s1[i] > s0[i]) {
			return qtrue;
		}
		if (s1[i] < s0[i]) {
			return qfalse;
		}
	}
	return qfalse;
}


static qboolean strgtrw(const wchar_t *s0, const wchar_t *s1)
{
	int l0, l1, i;

	l0 = wcslen(s0);
	l1 = wcslen(s1);

	if (l1<l0) {
		l0 = l1;
	}

	for(i=0;i<l0;i++) {
		if (s1[i] > s0[i]) {
			return qtrue;
		}
		if (s1[i] < s0[i]) {
			return qfalse;
		}
	}
	return qfalse;
}

/*
==============
Sys_ListFiles
==============
*/
char **Sys_ListFiles( const char *directory, const char *extension, char *filter, int *numfiles, qboolean wantsubs )
{
	char	search[MAX_OSPATH];
	int	nfiles;
	char	**listCopy;
	char	*list[MAX_FOUND_FILES];
	struct _finddata64i32_t findinfo;
	intptr_t	findhandle;
	int	flag;
	int	i;

	if (filter) {

		nfiles = 0;
		Sys_ListFilteredFiles( directory, "", filter, list, &nfiles );

		list[ nfiles ] = 0;
		*numfiles = nfiles;

		if (!nfiles)
		return NULL;

		listCopy = Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
		for ( i = 0 ; i < nfiles ; i++ ) {
			listCopy[i] = list[i];
		}
		listCopy[i] = NULL;

		return listCopy;
	}

	if ( !extension) {
		extension = "";
	}

	// passing a slash as extension will find directories
	if ( extension[0] == '/' && extension[1] == 0 ) {
		extension = "";
		flag = 0;
	} else {
		flag = _A_SUBDIR;
	}

	Com_sprintf( search, sizeof(search), "%s\\*%s", directory, extension );

	// search
	nfiles = 0;

	findhandle = _findfirst64i32 (search, &findinfo);
	if (findhandle == -1) {
		*numfiles = 0;
		return NULL;
	}

	do {
		if ( (!wantsubs && flag ^ ( findinfo.attrib & _A_SUBDIR )) || (wantsubs && findinfo.attrib & _A_SUBDIR) ) {
			if ( nfiles == MAX_FOUND_FILES - 1 ) {
				break;
			}
			list[ nfiles ] = (char*)CopyString( findinfo.name );
			nfiles++;
		}
	} while ( _findnext64i32 (findhandle, &findinfo) != -1 );

	list[ nfiles ] = 0;

	_findclose (findhandle);

	// return a copy of the list
	*numfiles = nfiles;

	if ( !nfiles ) {
		return NULL;
	}

	listCopy = Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
	for ( i = 0 ; i < nfiles ; i++ ) {
		listCopy[i] = list[i];
	}
	listCopy[i] = NULL;

	do {
		flag = 0;
		for(i=1; i<nfiles; i++) {
			if (strgtr(listCopy[i-1], listCopy[i])) {
				char *temp = listCopy[i];
				listCopy[i] = listCopy[i-1];
				listCopy[i-1] = temp;
				flag = 1;
			}
		}
	} while(flag);

	return listCopy;
}

/*
==============
Sys_ListFiles
==============
*/
wchar_t **Sys_ListFilesW( const wchar_t *directory, const wchar_t *extension, wchar_t *filter, int *numfiles, qboolean wantsubs )
{
	wchar_t	search[MAX_OSPATH];
	int	nfiles;
	wchar_t	**listCopy;
	wchar_t	*list[MAX_FOUND_FILES];
	struct _wfinddata64i32_t findinfo;
	intptr_t	findhandle;
	int	flag;
	int	i;

	if (filter) {

		nfiles = 0;
		Sys_ListFilteredFilesW( directory, L"", filter, list, &nfiles );

		list[ nfiles ] = 0;
		*numfiles = nfiles;

		if (!nfiles)
		return NULL;

		listCopy = Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
		for ( i = 0 ; i < nfiles ; i++ ) {
			listCopy[i] = list[i];
		}
		listCopy[i] = NULL;

		return listCopy;
	}

	if ( !extension) {
		extension = L"";
	}

	// passing a slash as extension will find directories
	if ( extension[0] == '/' && extension[1] == 0 ) {
		extension = L"";
		flag = 0;
	} else {
		flag = _A_SUBDIR;
	}

	Com_sprintfUni( search, sizeof(search), L"%ls\\*%ls", directory, extension );

	// search
	nfiles = 0;

	findhandle = _wfindfirst64i32 (search, &findinfo);
	if (findhandle == -1) {
		*numfiles = 0;
		return NULL;
	}

	do {
		if ( (!wantsubs && flag ^ ( findinfo.attrib & _A_SUBDIR )) || (wantsubs && findinfo.attrib & _A_SUBDIR) ) {
			if ( nfiles == MAX_FOUND_FILES - 1 ) {
				break;
			}
			list[ nfiles ] = _wcsdup( findinfo.name );
			nfiles++;
		}
	} while ( _wfindnext64i32 (findhandle, &findinfo) != -1 );

	list[ nfiles ] = 0;

	_findclose (findhandle);

	// return a copy of the list
	*numfiles = nfiles;

	if ( !nfiles ) {
		return NULL;
	}

	listCopy = Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
	for ( i = 0 ; i < nfiles ; i++ ) {
		listCopy[i] = list[i];
	}
	listCopy[i] = NULL;

	do {
		flag = 0;
		for(i=1; i<nfiles; i++) {
			if (strgtrw(listCopy[i-1], listCopy[i])) {
				wchar_t *temp = listCopy[i];
				listCopy[i] = listCopy[i-1];
				listCopy[i-1] = temp;
				flag = 1;
			}
		}
	} while(flag);

	return listCopy;
}

/*
==============
Sys_FreeFileList
==============
*/
void Sys_FreeFileList( char **list )
{
	int i;

	if ( !list ) {
		return;
	}

	for ( i = 0 ; list[i] ; i++ ) {
		FreeString( list[i] );
	}

	Z_Free( list );
}

void Sys_FreeFileListW( wchar_t **list )
{
	int i;

	if ( !list ) {
		return;
	}

	for ( i = 0 ; list[i] ; i++ ) {
		free( list[i] );
	}

	Z_Free( list );
}

qboolean REGPARM(1) Sys_DirectoryHasContent(const char *dir)
{
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;

    char searchpath[MAX_OSPATH];

	if(strlen(dir) > MAX_OSPATH - 6 || dir[0] == '\0')
		return qfalse;

    Q_strncpyz(searchpath, dir, sizeof(searchpath));
	if( searchpath[strlen(searchpath) -1] ==  '\\' )
	{
		searchpath[strlen(searchpath) -1] = '\0';
	}
	Q_strcat(searchpath, sizeof(searchpath), "\\*");

    if((hFind = FindFirstFile(searchpath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        return qfalse;
    }

    do
    {
        if(stricmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0)
        {
			FindClose(hFind);
			return qtrue;
        }
    }
    while(FindNextFile(hFind, &fdFile));

    FindClose(hFind);

    return qfalse;
}


//Returns number of successful parsed certs
int Sys_ReadCertificate(void* cacert, int (*store_callback)(void* ca_ctx, const unsigned char* pemderbuf, int lenofpemder))
{
//From: http://stackoverflow.com/questions/9507184/can-openssl-on-windows-use-the-system-certificate-store
    HCERTSTORE hStore;
    PCCERT_CONTEXT pContext = NULL;

    hStore = CertOpenSystemStoreA(0, "ROOT");

    if (!hStore)
	{
        return 0;
	}
	int i = 0;
	while ((pContext = CertEnumCertificatesInStore(hStore, pContext)))
	{
		if(pContext->dwCertEncodingType != X509_ASN_ENCODING)
		{
			continue;
		}
		if(store_callback(cacert, pContext->pbCertEncoded, pContext->cbCertEncoded) >= 0)
		{
			++i;
		}
	}
	CertFreeCertificateContext(pContext);
	CertCloseStore(hStore, 0);
	return i;
}

void Sys_SetEvent(HANDLE hEvent)
{
	SetEvent(hEvent);
}

