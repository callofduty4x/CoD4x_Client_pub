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
#include <windows.h>

cvar_t **in_mouse_cvar = in_mouse_cvar_ADDR;
#define in_mouse (*in_mouse_cvar)

typedef struct {
	int oldButtonState;

	qboolean unk2;
	int unk1;
	byte mouseActive;
	byte mouseInitialized;
} WinMouseVars_t;

//static int window_center_x, window_center_y;

#define window_center_x *((int*)(window_center_x_ADDR))
#define window_center_y *((int*)(window_center_y_ADDR))
#define s_wmv (*((WinMouseVars_t*)(s_wmv_ADDR)))

/*
============================================================

WIN32 MOUSE CONTROL

============================================================
*/

/*
================
IN_ActivateWin32Mouse
================
*/

void IN_ActivateWin32Mouse( void ) {
	RECT window_rect;

	GetWindowRect( g_wv.hWnd, &window_rect );

	window_center_x = ( window_rect.right + window_rect.left ) / 2;
	window_center_y = ( window_rect.top + window_rect.bottom ) / 2;

	SetCursorPos( window_center_x, window_center_y );
}


/*
================
IN_Win32Mouse
================
*/
void IN_Win32Mouse( int *mx, int *my ) {
	POINT current_pos;

	// find mouse movement
	GetCursorPos( &current_pos );

	// force the mouse to the center, so there's room to move
	SetCursorPos( window_center_x, window_center_y );

	*mx = current_pos.x - window_center_x;
	*my = current_pos.y - window_center_y;
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

/*
===========
IN_ActivateMouse

Called when the window gains focus or changes in some way
===========
*/
void IN_ActivateMouse( void ) {
	if ( !s_wmv.mouseInitialized ) {
		return;
	}
	if ( !in_mouse->boolean ) {
		s_wmv.mouseActive = qfalse;
		return;
	}
	if ( s_wmv.mouseActive ) {
		return;
	}

	if(GetForegroundWindow() == g_wv.hWnd){
		s_wmv.mouseActive = qtrue;
	}
}


/*
===========
IN_DeactivateMouse

Called when the window loses focus
===========
*/
void IN_DeactivateMouse( void ) {

	int bShow;

	if ( !s_wmv.mouseInitialized ) {
		return;
	}
	if ( !s_wmv.mouseActive ) {
		return;
	}
	s_wmv.mouseActive = qfalse;

	bShow = ShowCursor( TRUE );
	while ( bShow )
		bShow = ShowCursor( bShow < 0 );

}


/*
===========
IN_MouseEvent
===========
*/
void IN_MouseEvent( int mstate ) {
	int i;
	int var_01;
	
	if ( !s_wmv.mouseInitialized ) {
		return;
	}

	var_01 = s_wmv.oldButtonState ^ mstate;

	if(!var_01){
		return;
	}
	
// perform button actions
	for  ( i = 0 ; i < 5 ; i++ )
	{
		if(!(var_01 & ( 1 << i )))
			continue;
		
		if(mstate & ( 1 << i )){
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, K_MOUSE1 + i, qtrue, 0, NULL );
		}else{
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, K_MOUSE1 + i, qfalse, 0, NULL );
		}
	}
	s_wmv.oldButtonState = mstate;
}

/*
=========================================================================

=========================================================================
*/

/*
===========
IN_Init
===========
*/
void IN_Init( void ) {
	Com_Printf (CON_CHANNEL_SYSTEM, "\n------- Input Initialization -------\n");

	s_wmv.mouseInitialized = qfalse;

	in_mouse = Cvar_RegisterInt("in_mouse", 1, 0, 1, CVAR_LATCH | CVAR_ARCHIVE, "Initialize the mouse");
	
	if ( in_mouse->integer == 0 ) {
		Com_Printf(CON_CHANNEL_SYSTEM, "Mouse control not active.\n" );
		return;
	}

	s_wmv.mouseInitialized = qtrue;


	Com_Printf (CON_CHANNEL_SYSTEM, "------------------------------------\n");

	in_mouse->modified = qfalse;
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
void IN_Activate( qboolean active ) {
	in_appactive = active;
	
	if(s_wmv.mouseInitialized){
		if(in_mouse->boolean && GetForegroundWindow() == g_wv.hWnd){
			s_wmv.mouseActive = 1;
		}else{
			s_wmv.mouseActive = 0;
		}	
	}
}
