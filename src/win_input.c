/*
===========================================================================

Return to Castle Wolfenstein multiplayer GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Return to Castle Wolfenstein multiplayer GPL Source Code (RTCW MP Source Code).  

RTCW MP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW MP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW MP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW MP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW MP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

// win_input.c -- win32 mouse and joystick code
// 02/21/97 JCB Added extended DirectInput code to support external controllers.

//#include "../client/client.h"
//#include "win_local.h"
//#include "dinput.h"

#include "q_shared.h"
#include "win_sys.h"
#include "qcommon.h"
#include "cl_input.h"
#include "client.h"
#include <windows.h>

cvar_t * in_mouse;
cvar_t* raw_input;
bool g_showCursor;


typedef struct {
	int oldButtonState;
	struct tagPOINT oldPos;
	byte mouseActive;
	byte mouseInitialized;
	byte rawmouseinitialized;
} WinMouseVars_t;

static int window_center_x;
static int window_center_y;
qboolean in_appactive;
WinMouseVars_t s_wmv;

/*
============================================================

WIN32 MOUSE CONTROL

============================================================
*/
void IN_MouseMove();
/*
================
IN_RecenterMouse
================
*/

void IN_RecenterMouse( void ) {
	RECT window_rect;

	GetWindowRect( g_wv.hWnd, &window_rect );

	window_center_x = ( window_rect.right + window_rect.left ) / 2;
	window_center_y = ( window_rect.top + window_rect.bottom ) / 2;

	SetCursorPos( window_center_x, window_center_y );
}


/*
============================================================

DIRECT INPUT MOUSE CONTROL

============================================================
*/


#ifndef DOOMSOUND   ///// (SA) DOOMSOUND
#undef DEFINE_GUID
/*
#define DEFINE_GUID( name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8 ) \
	EXTERN_C const GUID name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
*/
#define DEFINE_GUID( name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8 ) \
	const GUID name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

DEFINE_GUID( GUID_SysMouse,   0x6F1D2B60,0xD5A0,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 );
DEFINE_GUID( GUID_XAxis,   0xA36D02E0,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 );
DEFINE_GUID( GUID_YAxis,   0xA36D02E1,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 );
DEFINE_GUID( GUID_ZAxis,   0xA36D02E2,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 );

/*
#define DINPUT_BUFFERSIZE           16
#define iDirectInputCreate( a,b,c,d ) pDirectInputCreate( a,b,c,d )

HRESULT ( WINAPI * pDirectInputCreate )( HINSTANCE hinst, DWORD dwVersion,
										 LPDIRECTINPUT * lplpDirectInput, LPUNKNOWN punkOuter );

*/
#endif ///// (SA) DOOMSOUND

//static HINSTANCE hInstDI;

typedef struct MYDATA {
	LONG lX;                    // X axis goes here
	LONG lY;                    // Y axis goes here
	LONG lZ;                    // Z axis goes here
	BYTE bButtonA;              // One button goes here
	BYTE bButtonB;              // Another button goes here
	BYTE bButtonC;              // Another button goes here
	BYTE bButtonD;              // Another button goes here
} MYDATA;
/*
static DIOBJECTDATAFORMAT rgodf[] = {
	{ &GUID_XAxis,    FIELD_OFFSET( MYDATA, lX ),       DIDFT_AXIS | DIDFT_ANYINSTANCE,   0,},
	{ &GUID_YAxis,    FIELD_OFFSET( MYDATA, lY ),       DIDFT_AXIS | DIDFT_ANYINSTANCE,   0,},
	{ &GUID_ZAxis,    FIELD_OFFSET( MYDATA, lZ ),       0x80000000 | DIDFT_AXIS | DIDFT_ANYINSTANCE,   0,},
	{ 0,              FIELD_OFFSET( MYDATA, bButtonA ), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0,},
	{ 0,              FIELD_OFFSET( MYDATA, bButtonB ), DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0,},
	{ 0,              FIELD_OFFSET( MYDATA, bButtonC ), 0x80000000 | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0,},
	{ 0,              FIELD_OFFSET( MYDATA, bButtonD ), 0x80000000 | DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0,},
};

#define NUM_OBJECTS ( sizeof( rgodf ) / sizeof( rgodf[0] ) )

static DIDATAFORMAT df = {
	sizeof( DIDATAFORMAT ),       // this structure
	sizeof( DIOBJECTDATAFORMAT ), // size of object data format
	DIDF_RELAXIS,               // absolute axis coordinates
	sizeof( MYDATA ),             // device data size
	NUM_OBJECTS,                // number of objects
	rgodf,                      // and here they are
};

static LPDIRECTINPUT g_pdi;
static LPDIRECTINPUTDEVICE g_pMouse;
*/
/*
============================================================

  MOUSE CONTROL

============================================================
*/


bool IN_IsForegroundWindow()
{
  return GetForegroundWindow() == g_wv.hWnd;
}

/*
===========
IN_ActivateMouse

Called when the window gains focus or changes in some way
===========
*/
void IN_ActivateMouse( qboolean force ) {
	if ( !s_wmv.mouseInitialized ) {
		return;
	}
	if ( !in_mouse->boolean ) {
		s_wmv.mouseActive = qfalse;
		return;
	}
	if ( !force && s_wmv.mouseActive ) {
		return;
	}
	s_wmv.mouseActive = IN_IsForegroundWindow() != 0;
}

void IN_ShowSystemCursor(int show)
{
	
  int actualShow; // [esp+0h] [ebp-8h]
  int desiredShow; // [esp+4h] [ebp-4h]

  g_showCursor = show;
  desiredShow = (show != 0) - 1;
  for ( actualShow = ShowCursor(show); actualShow != desiredShow; actualShow = ShowCursor(actualShow < desiredShow) )
  {
    ;
  }
}

void IN_DeactivateWin32Mouse()
{
  IN_ShowSystemCursor(1);
}


/*
===========
IN_DeactivateMouse

Called when the window loses focus
===========
*/
void IN_DeactivateMouse( void ) {


	if ( !s_wmv.mouseInitialized ) {
		return;
	}
	if ( !s_wmv.mouseActive ) {
		return;
	}
	s_wmv.mouseActive = qfalse;
	IN_DeactivateWin32Mouse();

}


/*
===========
IN_MouseEvent
===========
*/
void IN_MouseEvent( int mstate ) {
	int button;
	int diff;
	
	if ( !s_wmv.mouseInitialized ) {
		return;
	}

	diff = s_wmv.oldButtonState ^ mstate;

	if(!diff){
		return;
	}
	
// perform button actions
	for  ( button = 0 ; button < 5 ; button++ )
	{
		if(!(diff & ( 1 << button )))
			continue;
		
		if(mstate & ( 1 << button )){
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, K_MOUSE1 + button, qtrue, 0, NULL );
		}else{
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, K_MOUSE1 + button, qfalse, 0, NULL );
		}
	}
	s_wmv.oldButtonState = mstate;
}

/*
=========================================================================

=========================================================================
*/

void IN_StartupMouse()
{
	s_wmv.mouseInitialized = 0;
	if(s_wmv.rawmouseinitialized == false)
	{
		Cvar_SetBool(raw_input, false);
		Cvar_ClearModified(raw_input);
	}
	if ( in_mouse->boolean )
	{
		//Call it so mouse pointer gets drawn properly even in RAW input mode
		
		IN_RecenterMouse();
        s_wmv.oldPos.x = window_center_x;
        s_wmv.oldPos.y = window_center_y;
		
		s_wmv.mouseInitialized = 1;

	}
	else
	{
		Com_Printf(CON_CHANNEL_SYSTEM, "Mouse control not active.\n");
	}

}


void IN_Startup()
{
  IN_StartupMouse();
  //IN_StartupGamepads();
  Cvar_ClearModified(in_mouse);

}

/*
===========
IN_Init
===========
*/
void IN_Init( void ) {
	Com_Printf (CON_CHANNEL_SYSTEM, "\n------- Input Initialization -------\n");

	s_wmv.mouseInitialized = qfalse;

	raw_input = Cvar_RegisterBool("raw_input", qfalse, CVAR_LATCH | CVAR_ARCHIVE, "Use raw input for input devices");
	in_mouse = Cvar_RegisterInt("in_mouse", 1, 0, 1, CVAR_LATCH | CVAR_ARCHIVE, "Initialize the mouse");
	IN_Startup();

	Com_Printf (CON_CHANNEL_SYSTEM, "------------------------------------\n");

}

/*
===========
IN_Shutdown
===========
*/
void IN_Shutdown( void ) {
	IN_DeactivateMouse();
}


/*
===========
IN_Activate

Called when the main window gains or loses focus.
The window may have been destroyed and recreated
between a deactivate and an activate.
===========
*/
void __cdecl IN_Activate(qboolean active)
{
  in_appactive = active;
  if ( active )
  {
    IN_ActivateMouse(1);
  }
  else
  {
    IN_DeactivateMouse();
  }
}


void IN_ClampMouseMove(struct tagPOINT *curPos)
{
  bool isClamped; 
  struct tagRECT rc; 

  GetWindowRect(g_wv.hWnd, &rc);
  isClamped = false;
  if ( curPos->x >= rc.left )
  {
    if ( curPos->x >= rc.right )
    {
      curPos->x = rc.right - 1;
      isClamped = true;
    }
  }
  else
  {
    curPos->x = rc.left;
    isClamped = true;
  }
  if ( curPos->y >= rc.top )
  {
    if ( curPos->y >= rc.bottom )
    {
      curPos->y = rc.bottom - 1;
      isClamped = true;
    }
  }
  else
  {
    curPos->y = rc.top;
    isClamped = true;
  }
  if ( isClamped )
  {
    SetCursorPos(curPos->x, curPos->y);
  }
}


void IN_MouseMove()
{
  int dx;
  struct tagPOINT curPos;
  int dy;


  assert(s_wmv.mouseInitialized);
  assert(r_fullscreen);

  if ( IN_IsForegroundWindow() )
  {
    GetCursorPos(&curPos);
    if ( r_fullscreen->boolean )
    {
      IN_ClampMouseMove(&curPos);
    }
    dx = curPos.x - s_wmv.oldPos.x;
    dy = curPos.y - s_wmv.oldPos.y;

    s_wmv.oldPos = curPos;
    ScreenToClient(g_wv.hWnd, &curPos);
    g_wv.recenterMouse = CL_MouseEvent(curPos.x, curPos.y, dx, dy);
    if ( g_wv.recenterMouse )
    {
      if ( dx || dy )
      {
        IN_RecenterMouse();
        s_wmv.oldPos.x = window_center_x;
        s_wmv.oldPos.y = window_center_y;
      }
    }
  }
}


void __cdecl IN_Frame()
{
  if ( Cvar_GetBool("ClickToContinue") )
  {
    PostMessageA(g_wv.hWnd, 0x201u, 1u, 0);
  }

  if ( s_wmv.mouseInitialized )
  {
    if ( in_appactive )
    {

      IN_ActivateMouse(0);
	  if(!raw_input->boolean)
  	  {
	    IN_MouseMove();
	  }

      /*
	  if ( IN_IsForegroundWindow() )
      {
        IN_GamepadsMove();
      }*/
    }
    else
    {
      IN_DeactivateMouse();
    }
  }
}

void IN_SetCursorPos(int x, int y)
{
  struct tagPOINT curPos;

  curPos.x = x;
  curPos.y = y;
  ClientToScreen(g_wv.hWnd, &curPos);
  SetCursorPos(curPos.x, curPos.y);
  s_wmv.oldPos = curPos;
}



#define HID_USAGE_PAGE_GENERIC 1
#define HID_USAGE_GENERIC_MOUSE 2

static RAWINPUT rawinput;

void IN_RawMouseInit()
{
	RAWINPUTDEVICE rimouse[1];

	rimouse[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rimouse[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	rimouse[0].dwFlags = RIDEV_INPUTSINK;
	rimouse[0].hwndTarget = g_wv.hWnd;

	if(!RegisterRawInputDevices(rimouse, 1, sizeof(RAWINPUTDEVICE)))
	{
		Com_PrintError(CON_CHANNEL_SYSTEM,"Raw input initialization failed. Will use Win32 mouse. Error code 0x%x\n", (unsigned int)GetLastError());
		s_wmv.rawmouseinitialized = false;
	}else{
		s_wmv.rawmouseinitialized = true;
	}

	memset(&rawinput, 0, sizeof(RAWINPUT));

}

void IN_RawEvent(LPARAM lParam)
{
	int dx;
	struct tagPOINT curPos;
	int dy;

	if ( raw_input->boolean && IN_IsForegroundWindow() )
	{

		assert(s_wmv.mouseInitialized);
		assert(r_fullscreen);

		UINT size = sizeof(RAWINPUT);
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawinput, &size, sizeof(RAWINPUTHEADER));
		
		if (rawinput.header.dwType != RIM_TYPEMOUSE) {
			return;
		}

		dx = rawinput.data.mouse.lLastX;
		dy = rawinput.data.mouse.lLastY;

		if ( r_fullscreen->boolean )
		{
			curPos.x = s_wmv.oldPos.x + dx;
			curPos.y = s_wmv.oldPos.y + dy;
			IN_ClampMouseMove(&curPos);
		}else{
			GetCursorPos(&curPos);
		}

		s_wmv.oldPos = curPos;
		ScreenToClient(g_wv.hWnd, &curPos);
		g_wv.recenterMouse = CL_MouseEvent(curPos.x, curPos.y, dx, dy);
		if ( g_wv.recenterMouse )
		{
			if ( dx || dy )
			{
				IN_RecenterMouse();
				s_wmv.oldPos.x = window_center_x;
				s_wmv.oldPos.y = window_center_y;
			}
		}
	}

}

bool IN_AppActive()
{
	return in_appactive;
}

void IN_MenuResetMouse()
{
	g_wv.recenterMouse = CL_MouseEvent(s_wmv.oldPos.x, s_wmv.oldPos.y, 0, 0);
	if ( g_wv.recenterMouse )
	{
		IN_RecenterMouse();
		s_wmv.oldPos.x = window_center_x;
		s_wmv.oldPos.y = window_center_y;
	}
}


bool IN_RestartNeeded()
{
	return Cvar_HasLatchedValue(raw_input);
}