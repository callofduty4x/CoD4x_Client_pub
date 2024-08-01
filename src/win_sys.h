/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//c5f900 clc.state


#ifndef __WIN_SYS_H__
#define __WIN_SYS_H__

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <setjmp.h>

#include "q_shared.h"
#include "game.h"

#define SERVER_STATIC_ADDR 0x1cbfc80
#define SVS_TIME_ADDR 0x90b4f84
#define SERVER_STRUCT_ADDR 0x17fc7c8
#define g_entities_ADDR 0x1288500
#define sys_criticalSections_ADDR 0xcc11f00
#define g_wv_ADDR 0xcc1b6f4
#define splashScreen_HWND_ADDR 0xcc18c88
#define sys_cmdline_ADDR 0xcc157e0
#define cmd_functions_ADDR 0x1410b3c
#define sv_cmd_functions_ADDR 0x14099DC
#define db_init_ADDR 0xe7f8c4
#define sys_timeBase_ADDR 0xcc18c84
#define sys_timeInitialized_ADDR 0xd8b3018
#define CM_TraceBox_ADDR 0x4f6d00
#define com_frameTime_ADDR 0x1476efc
#define com_fullyInitialized_ADDR 0x1476f18
#define MSH_MOUSEWHEEL_ADDR 0xcc1b6ec
#define in_appactive_ADDR 0xcc147c0
#define window_center_x_ADDR 0xcc147bc
#define window_center_y_ADDR 0xcc147b8
#define s_wmv_ADDR 0xcc147c4
#define in_mouse_cvar_ADDR ((cvar_t**)0xcc120e4)
#define org_rd_buffer_ADDR 0x1476F10
#define org_rd_flush_ADDR 0x1435D74
#define org_rd_buffersize_ADDR 0x1435D34
#define cdkeyCvarStringBase_ADDR 0x6EAF2C
#define dsoundFailedStr_ADDR 0x6e04e8
#define msg_printEntityNums_ADDR 0x14e88c0
#define msg_dumpEnts_ADDR 0x14e88a8

#include <windows.h>
#include <wincrypt.h>

#define g_wv (*((WinVars_t*)(g_wv_ADDR)))

typedef struct{
	HINSTANCE reflib_library;           // Handle to refresh DLL
	qboolean reflib_active;
	HWND hWnd;							//0xcc1b6fc
	HINSTANCE hInstance;				//0xcc1b700
	qboolean activeApp;
	qboolean isMinimized;				//0xcc1b708
	qboolean recenterMouse;
	// when we get a windows message, we store the time off so keyboard processing
	// can know the exact time of an event
	unsigned sysMsgTime;				//0xcc1b710
} WinVars_t;

typedef struct{
	void (__stdcall	*AIL_set_DirectSound_HWND_int)(int, HWND);
}MilesSoundSystem_t;

extern MilesSoundSystem_t mss;
//This defines Cvars directly related to executable file
#define getcvaradr(adr) ((cvar_t*)(*(int*)(adr)))



#define vid_xpos getcvaradr(0xd569674)
#define vid_ypos getcvaradr(0xd5695f4)
#define r_reflectionProbeGenerate getcvaradr(0xcc9f5f8)
#define r_fullscreen getcvaradr(0xd569598)




#define IDI_ICON1 108



void __cdecl SV_ClipMoveToEntity(trace_t *trace);

qboolean Sys_RandomBytes( byte *string, int len );

void Sys_EventLoop(void);

void IN_Init(void);

void Sys_Init(void);

void Sys_ShowConsole( int visLevel, qboolean quitOnClose );
void Sys_ClearConsole( void );

void Conbuf_AppendText( const char *pMsg );

byte Sys_IsMainThread(void);

void Sys_ShowConsole( int visLevel, qboolean quitOnClose );
void __cdecl PbCaptureConsoleOutput(const char* text);
void Sys_CreateConsole( qboolean show );
LONG WINAPI MainWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
void RestoreMainFunction();
void Patch_Dedicated();
void Patch_Other();
void Patch_WinMainEntryPoint();
void Scr_InitVariables();


#define CM_TransformedBoxTrace_ADDR 0x4f54a0
clipHandle_t REGPARM(1) SV_ClipHandleForEntity(const sharedEntity_t *ent);
void __cdecl Sys_InitMainThread(void);
void __cdecl Sys_InitLocalization(void);
qboolean REGPARM(2) Sys_571a90(int, char* str1, char* string);
void __cdecl GetApplicationLockFileName(void);
qboolean __cdecl Sys_CheckCrashOrRerun(void);
void __cdecl Com_InitParse(void);
float __cdecl InitTiming(void);
void __cdecl Sys_GetHardwareInfo(void);
void __cdecl Sys_CreateSplashWindow(void);
void __cdecl Sys_RegisterWindow(void);
void __cdecl getcodcwd(char* buffer, size_t buffersize);
byte REGPARM(1) PbClientInitialize(HINSTANCE);
byte __cdecl PbServerInitialize(void);
void __cdecl Com_CheckSetRecommend(void);
void __cdecl SL_Init();
void __cdecl Swap_Init();
void __cdecl Cbuf_Init();
void __cdecl Cmd_Init();
void __cdecl Com_InitDvars(void);
void __cdecl CCS_InitConstantConfigStrings();
qboolean __cdecl Sys_SpawnDatabaseThread(void);
void __cdecl CL_InitKeyCommands();
void __cdecl FS_InitFilesystem( void );
void __cdecl Con_InitChannels(void);
void __cdecl StatsCommands_Init(void);
void REGPARM(1) Init_PlayerProfile(int);
void __cdecl Sub_4f9280(int, int);
void __cdecl SEH_UpdateLanguageInfo(void);
void __cdecl Sub_46cd60(void);
void __cdecl Com_InitHunkMemory(void);
void __cdecl Sys_OutOfMemory(const char*, int);
void __cdecl Com_Quit_f(void);
void __cdecl Com_WriteConfig_f(void);
void __cdecl Com_WriteDefaults_f(void);
void __cdecl Scr_Init(void);
void __cdecl Scr_DefaultSettings(void);
void __cdecl XAnim_Init(void);
void __cdecl SV_Init(void);
void __cdecl Com_StartupVariable(const char *match);
void __cdecl SND_InitDriver(void);
//int __cdecl _setjmp3(jmp_buf env, int count,...);
void __cdecl Play_Intro();
void __cdecl Cvar_Dump(void);
void __cdecl CL_KeyEvent( int zero, int key, qboolean down, unsigned time );

int __cdecl Com_EventLoop_org(void);
void __cdecl Com_ClientPacketEvent(void);
void __cdecl Com_ServerPacketEvent(void);
void __cdecl Sys_RegisterInfoDvars(void);

void __cdecl Cvar_Init(void);

qboolean Patch_MainModule(void(patch_func)());

void __stdcall PbClientProcessEvents();
void __stdcall PbServerProcessEvents(int);
void PbClientConnecting(int arg_1, char* string, int* size);

void Sys_RegisterCoD4Protocol( );
void Sys_RegisterDemoExt( );

void sub_477210();
void Sys_GetLastErrorAsString(char* errbuf, int len);

typedef DWORD threadid_t;
int _wrename(const wchar_t *oldname, const wchar_t *newname);
void Sys_SetEvent(HANDLE hEvent);
void Sys_In_Restart_f( void );

#define rgRegisteredEvent (*(HANDLE*)(0x014E890C))
#endif
