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

//#include "../client/client.h"
//#include "win_local.h"

#include "q_shared.h"
#include "client.h"
#include "cl_input.h"
#include "win_sys.h"
#include "snd_system.h"
#include <windows.h>
#include <Windowsx.h>

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL ( WM_MOUSELAST + 1 )  // message that will be supported by the OS
#endif

#define MSH_MOUSEWHEEL *((UINT*)(MSH_MOUSEWHEEL_ADDR))

// Console variables that we need to access from this module
//cvar_t      *vid_xpos;          // X coordinate of window position
//cvar_t      *vid_ypos;          // Y coordinate of window position
//cvar_t      *r_fullscreen;


#define VID_NUM_MODES ( sizeof( vid_modes ) / sizeof( vid_modes[0] ) )

cvar_t* r_autopriority;

/*
==================
VID_AppActivate
==================
*/
static void VID_AppActivate( BOOL fActive, BOOL minimize ) {
	g_wv.isMinimized = minimize;

	Com_DPrintf(CON_CHANNEL_SYSTEM, "VID_AppActivate: %i\n", fActive );

	Key_ClearStates(0);  // FIXME!!!

	// we don't want to act like we're active if we're minimized
	if ( fActive && !g_wv.isMinimized ) {
		g_wv.activeApp = qtrue;
	} else
	{
		g_wv.activeApp = qfalse;
	}

	// minimize/restore mouse-capture on demand
	if ( !g_wv.activeApp ) {
		in_appactive = qfalse;
		IN_DeactivateMouse();


	} else
	{
		IN_Activate( qtrue );
	}
}

//==========================================================================

static byte s_scantokey[256][2] =
{{0,0}, {0xc8,0xc8}, {0xc9,0xc9}, {0,0},
{0xca,0xca}, {0xcb,0xcb}, {0xcc,0xcc}, {0,0},
{0x7f,0x7f}, {0x09,0x09}, {0,0}, {0,0},
{0xba,0}, {0x0d,0xbf}, {0,0}, {0,0},
{0xa0,0xa0}, {0x9f,0x9f}, {0x9e,0x9e}, {0x99,0x99},
{0x97,0x97},{0,0}, {0,0}, {0,0},
{0,0}, {0,0}, {0,0}, {0x1b,0x1b},
{0,0}, {0,0}, {0,0}, {0,0},
{0x20,0x20}, {0xb8,0xa4}, {0xbe,0xa3}, {0xbc,0xa6},
{0xb6,0xa5}, {0xb9,0x9c}, {0xb7,0x9a}, {0xbb,0x9d},
{0xbd,0x9b}, {0,0}, {0,0}, {0,0},
{0,0}, {0xc0,0xa1}, {0xc1,0xa2}, {0,0},
{'0','0'}, {'1','1'}, {'2','2'}, {'3','3'},
{'4','4'}, {'5','5'}, {'6','6'}, {'7','7'},
{'8','8'}, {'9','9'}, {0,0}, {0,0},
{0,0}, {0,0}, {0,0}, {0,0},
{0,0}, {'a','A'}, {'b','B'}, {'c','C'},
{'d','D'}, {'e','E'}, {'f','F'}, {'g','G'},
{'h','H'}, {'i','I'}, {'j','J'}, {'k','K'},
{'l','L'}, {'m','M'}, {'n','N'}, {'o','O'},
{'p','P'}, {'q','Q'}, {'r','R'}, {'s','S'},
{'t','T'}, {'u','U'}, {'v','V'}, {'w','W'},
{'x','X'}, {'y','Y'}, {'z','Z'}, {0,0},
{0,0}, {0,0}, {0,0}, {0,0},
{0xc0,0xc0}, {0xbc,0xbc}, {0xbd,0xbd}, {0xbe,0xbe},
{0xb9,0xb9}, {0xba,0xba}, {0xbb,0xbb}, {0xb6,0xb6},
{0xb7,0xb7}, {0xb8,0xb8}, {0xc6,0xc6}, {0xc4,0xc4},
{0,0}, {0xc3,0xc3}, {0xc1,0xc1}, {0xc2,0xc2},
{0xa7,0xa7}, {0xa8,0xa8}, {0xa9,0xa9}, {0xaa,0xaa},
{0xab,0xab}, {0xac,0xac}, {0xad,0xad}, {0xae,0xae},
{0xaf,0xaf}, {0xb0,0xb0}, {0xb1,0xb1}, {0xb2,0xb2},
{0,0}, {0,0}, {0,0}, {0,0} };



static byte s_scantohighkey[21][2] =
{{0xb5,0x80}, {0xbf,0x81}, {0xdf,0x82}, {0xe0,0x83},
{0xe1,0x84}, {0xe4,0x85}, {0xe5,0x86}, {0xe6,0x87},
{0xe7,0x88}, {0xe8,0x89}, {0xe9,0x8a}, {0xec,0x8b},
{0xf1,0x8c}, {0xf2,0x8d}, {0xf3,0x8e}, {0xf6,0x8f},
{0xf8,0x90}, {0xf9,0x91}, {0xfa,0x92}, {0xfc,0x93},
{0,0} };


/*
=======
MapKey

Map from windows to quake keynums
=======
*/

#ifndef MAPVK_VK_TO_CHAR
#define MAPVK_VK_TO_CHAR 2
#endif

static byte MapKey( int key, int v_key ) {

	int result;
	int modified;
	qboolean is_extended;
	char character;
	int i;

	modified = ( key >> 16 ) & 255;

	if(modified == 0x29) //(Spacebar)
		return 0x7e;

	if ( key & ( 1 << 24 ) ) {
		is_extended = qtrue;
	} else
	{
		is_extended = qfalse;
	}

	if(v_key < 144){

		result = s_scantokey[v_key][is_extended];

		if(clientUIActives.keyCatchers & 0x11 && !is_extended){
			if(v_key > 95 && v_key < 106)
				result = 0;

			if(v_key == 110)
				result = 0;
	}

		if(result){
			return result;
		}
	}

	result = MapVirtualKeyA(v_key, MAPVK_VK_TO_CHAR) & 255;

	if(result <= 127){
		return result;
	}

	i = 0;
	character = s_scantohighkey[i][0];
	while(character){
		if(result == character){
			return(s_scantohighkey[i][1]);
		}

		i++;
		character = s_scantohighkey[i][0];
	}
	return result;


}

/*
====================
MainWndProc

main window procedure
====================
*/
LONG WINAPI MainWndProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam ) {

	int key;
	int xPos, yPos;
	int nWidth, nHeight;
	int style;
	int fActive, fMinimized;
	int mousemask;
	RECT r;
	MONITORINFO monitor;

	SetThreadExecutionState(2);

	if ( uMsg == MSH_MOUSEWHEEL ) {
		if ( ( ( int ) wParam ) > 0 ) {
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, 206 /*K_MWHEELUP*/, qtrue, 0, NULL );
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, 206 /*K_MWHEELUP*/, qfalse, 0, NULL );
		} else
		{
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, 205 /*K_MWHEELDOWN*/, qtrue, 0, NULL );
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, 205 /*K_MWHEELDOWN*/, qfalse, 0, NULL );
		}
		return DefWindowProcA( hWnd, uMsg, wParam, lParam );
	}

	switch ( uMsg )
	{
	case WM_CLOSE:
		Cbuf_AddText( "quit\n" );
		break;

	case WM_CREATE:

		g_wv.hWnd = hWnd;

		SND_InitDirectSoundHandle(hWnd);


		if( r_reflectionProbeGenerate->boolean && r_fullscreen->boolean){

			Cvar_SetBool(r_fullscreen, qfalse);
			Cbuf_AddText( "vid_restart\n" );
		}
		r_autopriority = Cvar_RegisterBool("r_autopriority", qtrue, 0, "Automatically set the priority of the windows process when the game is minimized");
		MSH_MOUSEWHEEL = RegisterWindowMessage( "MSWHEEL_ROLLMSG" );
		break;


	case WM_DESTROY:
		// let sound and input know about this?
		g_wv.hWnd = NULL;
		break;

	case WM_MOVE:

		if ( !r_fullscreen->boolean ) {
			xPos = (short) LOWORD( lParam );      // horizontal position
			yPos = (short) HIWORD( lParam );      // vertical position

			r.left   = 0;
			r.top    = 0;
			r.right  = 1;
			r.bottom = 1;

			style = GetWindowLongA( hWnd, GWL_STYLE );
			AdjustWindowRect( &r, style, FALSE );

			Cvar_SetInt( vid_xpos, (int)(xPos + r.left));
			Cvar_SetInt( vid_ypos, (int)(yPos + r.top));
			vid_xpos->modified = qfalse;
			vid_ypos->modified = qfalse;
			if ( g_wv.activeApp ) {
				IN_Activate( qtrue );
			}
		}else{
			IN_ActivateMouse();
		}
	break;

	case WM_ACTIVATE:

		fActive = LOWORD( wParam );
		fMinimized = (BOOL) HIWORD( wParam );

		VID_AppActivate( fActive != WA_INACTIVE, fMinimized );

	break;

	case WM_SETFOCUS:
		if(r_autopriority && r_autopriority->boolean){
			SetPriorityClass(GetCurrentProcess(),0x20);
			return 0;
		}

	break;

	case WM_KILLFOCUS:

		if(r_autopriority && r_autopriority->boolean){
			SetPriorityClass(GetCurrentProcess(),0x40);
			return 0;
		}

	break;

	case WM_DISPLAYCHANGE:


		monitor.cbSize = sizeof(monitor);
		GetMonitorInfoA(MonitorFromWindow(hWnd, 2), &monitor);

		GetWindowRect(hWnd, &r);

		xPos = monitor.rcMonitor.left;
		yPos = monitor.rcMonitor.top;
		nWidth = r.right - r.left;
		nHeight = r.bottom - r.top;

		if( nWidth == monitor.rcMonitor.right - monitor.rcMonitor.left && nHeight == monitor.rcMonitor.bottom - monitor.rcMonitor.top){

			MoveWindow( hWnd, xPos, yPos, nWidth, nHeight, qtrue);
		}

		IN_ActivateWin32Mouse();


	break;

	case WM_SYSKEYDOWN:
		if ( wParam == 13 ) {
			if(clientUIActives.state == CA_LOADING)
				return 0;

			if ( r_fullscreen && r_fullscreen->boolean && !com_developer->integer) {
				Cvar_SetBool( r_fullscreen, !r_fullscreen->boolean );
				Cbuf_AddText( "vid_restart\n" );
			}
			return 0;
		}
		// fall through
	case WM_KEYDOWN:

		key = MapKey( lParam, wParam );
		if(!key)
			return 0;

		Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, key, qtrue, 0, NULL );
		break;


	case WM_SYSKEYUP:
	case WM_KEYUP:

		key = MapKey( lParam, wParam );
		if(!key)
			return 0;

		Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, key, qfalse, 0, NULL );
		break;


	case WM_CHAR:
		Com_QueueEvent( g_wv.sysMsgTime, SE_CHAR, wParam, 0, 0, NULL );
		break;


	case WM_SYSCOMMAND:
		if ( wParam == SC_SCREENSAVE || (wParam == SC_KEYMENU && HIWORD( lParam ) <= 0)) {
			return 0;
		}
		break;

// this is complicated because Win32 seems to pack multiple mouse events into
// one update sometimes, so we always check all states and look for events
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:

		mousemask = 0;

		if ( wParam & MK_LBUTTON ) {
			mousemask |= 1;
		}

		if ( wParam & MK_RBUTTON ) {
			mousemask |= 2;
		}

		if ( wParam & MK_MBUTTON ) {
			mousemask |= 4;
		}

		if ( wParam & MK_XBUTTON1 ) {
			mousemask |= 8;
		}

		if ( wParam & MK_XBUTTON2 ) {
			mousemask |= 16;
		}

		IN_MouseEvent( mousemask );

	break;

	case WM_MOUSEWHEEL:
		//
		//
		// this chunk of code theoretically only works under NT4 and Win98
		// since this message doesn't exist under Win95
		//
		if ( ( short ) HIWORD( wParam ) > 0 ) {
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qtrue, 0, NULL );
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qfalse, 0, NULL );
		} else {
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qtrue, 0, NULL );
			Com_QueueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qfalse, 0, NULL );
		}
		break;


	case WM_POWERBROADCAST:

		if(!wParam || wParam == 1)
			return BROADCAST_QUERY_DENY;

	}
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
