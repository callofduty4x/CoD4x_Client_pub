/*
===========================================================================
    Copyright (C) 1999-2005 Id Software, Inc.

    This file is part of CoD4X18-Server source code.

    CoD4X18-Server source code is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    CoD4X18-Server source code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
===========================================================================
*/
#include "win_debugcon.h"
#include <errno.h>
#include <float.h>
#include <fcntl.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>
#include <conio.h>
#include <stdexcept>


#define SYSCON_DEFAULT_WIDTH    540
#define SYSCON_DEFAULT_HEIGHT   450

#define COPY_ID         1
#define CLEAR_ID        3

#define ERRORBOX_ID     10
#define ERRORTEXT_ID    11

#define EDIT_ID         100
#define INPUT_ID        101

#define IDI_ICON1 1

// Next default values for new objects
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NO_MFC                     1
#define _APS_NEXT_RESOURCE_VALUE        130
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1005
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif



//********************************************************

#define Q_IsColorString(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE )
#define Q_COLOR_ESCAPE	'^'	
	

	/*
	** Sys_ShowConsole
	*/
	void DebugConsole::Show( int visLevel, bool quitOnClose ) {
		s_wcd.quitOnClose = quitOnClose;

		if ( visLevel == s_wcd.visLevel ) {
			return;
		}

		s_wcd.visLevel = visLevel;

		if ( !s_wcd.hWnd ) {
			return;
		}

		switch ( visLevel )
		{
		case 0:
			ShowWindow( s_wcd.hWnd, SW_HIDE );
			break;
		case 1:
			ShowWindow( s_wcd.hWnd, SW_SHOWNORMAL );
			SendMessageA( s_wcd.hwndBuffer, EM_LINESCROLL, 0, 0xffff );
			break;
		case 2:
			ShowWindow( s_wcd.hWnd, SW_MINIMIZE );
			break;
		default:
			break;
		}
	}

	/*
	** Sys_ConsoleInput
	*/
	char *DebugConsole::Input( void ) {

		if ( s_wcd.consoleText[0] == 0 ) {
			return NULL;
		}

		strcpy( s_wcd.returnedText, s_wcd.consoleText );
		s_wcd.consoleText[0] = 0;

		return s_wcd.returnedText;
	}

	/*
	** Conbuf_AppendText
	*/
	void DebugConsole::PrintInternal( const char *pMsg ) {
	#define CONSOLE_BUFFER_SIZE     16384

		char buffer[CONSOLE_BUFFER_SIZE * 2];
		char *b = buffer;
		const char *msg;
		int bufLen;
		int i = 0;

		//
		// if the message is REALLY long, use just the last portion of it
		//
		if ( strlen( pMsg ) > CONSOLE_BUFFER_SIZE - 1 ) {
			msg = pMsg + strlen( pMsg ) - CONSOLE_BUFFER_SIZE + 1;
		} else
		{
			msg = pMsg;
		}

		//
		// copy into an intermediate buffer
		//
		while ( msg[i] && ( ( b - buffer ) < (signed)sizeof( buffer ) - 1 ) )
		{
			if ( msg[i] == '\n' && msg[i + 1] == '\r' ) {
				b[0] = '\r';
				b[1] = '\n';
				b += 2;
				i++;
			} else if ( msg[i] == '\r' )     {
				b[0] = '\r';
				b[1] = '\n';
				b += 2;
			} else if ( msg[i] == '\n' )     {
				b[0] = '\r';
				b[1] = '\n';
				b += 2;
			} else if ( Q_IsColorString( &msg[i] ) )   {
				i++;
			} else
			{
				*b = msg[i];
				b++;
			}
			i++;
		}
		*b = 0;
		bufLen = b - buffer;

		s_totalChars += bufLen;

		//
		// replace selection instead of appending if we're overflowing
		//
		if ( s_totalChars > CONSOLE_BUFFER_SIZE ) {
			SendNotifyMessageA( s_wcd.hwndBuffer, EM_SETSEL, 0, -1 );
			s_totalChars = bufLen;
		} else {
			// NERVE - SMF - always append at the bottom of the textbox
			SendNotifyMessageA( s_wcd.hwndBuffer, EM_SETSEL, 0xFFFF, 0xFFFF );
		}

		//
		// put this text into the windows console
		//
		SendNotifyMessageA( s_wcd.hwndBuffer, EM_LINESCROLL, 0, 0xffff );
		SendNotifyMessageA( s_wcd.hwndBuffer, EM_SCROLLCARET, 0, 0 );

		
		SendNotifyMessageA( s_wcd.hwndBuffer, EM_REPLACESEL, 0, (LPARAM) buffer );
		//
		//InvalidateRect(s_wcd.hwndBuffer, NULL, TRUE);
	}

	void DebugConsole::Print( const std::string &pMsg ) {
		PrintInternal(pMsg.c_str());
	}
	void DebugConsole::Print( const char *pMsg ) {
		PrintInternal(pMsg);
	}
	void DebugConsole::DisableDraw() 
	{
		SendMessage(  s_wcd.hwndBuffer, WM_SETREDRAW, (WPARAM) FALSE, 0);
	}

	void DebugConsole::EnableDraw() 
	{
		SendMessageA( s_wcd.hwndBuffer, WM_SETREDRAW,  (WPARAM) TRUE, 0);
	}

	DebugConsole::DebugConsole(HINSTANCE hInstance, const char* title)
	{
		InitInternal( hInstance, title, 0, 0 );
	}

	DebugConsole::DebugConsole(HINSTANCE hInstance, const char* title, int initialwidth, int initialheigth)
	{
		InitInternal( hInstance, title, initialwidth, initialheigth );
	}

	DebugConsole::~DebugConsole()
	{
		Shutdown();
	}

	LONG WINAPI DebugConsole::ConWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
		int cx, cy;
		float sx; /*, sy*/
		float x, y, w, h;

		switch ( uMsg )
		{
		case WM_SIZE:
			// NERVE - SMF
			cx = LOWORD( lParam );
			cy = HIWORD( lParam );
	/*
			if ( cx < SYSCON_DEFAULT_WIDTH )
				cx = SYSCON_DEFAULT_WIDTH;
			if ( cy < SYSCON_DEFAULT_HEIGHT )
				cy = SYSCON_DEFAULT_HEIGHT;
	*/
			sx = (float)cx / SYSCON_DEFAULT_WIDTH;
			//sy = (float)cy / SYSCON_DEFAULT_HEIGHT;

			x = 8;
			y = 6;
			w = cx - 15;
			h = cy - 60;
			SetWindowPos( s_wcd.hwndBuffer, NULL, x, y, w, h, 0 );

			y = y + h + 4;
			h = 20;
			SetWindowPos( s_wcd.hwndInputLine, NULL, x, y, w, h, 0 );

			y = y + h + 4;
			w = 72 * sx;
			h = 24;
			SetWindowPos( s_wcd.hwndButtonCopy, NULL, x, y, w, h, 0 );

			x = cx - 6 - w;
			SetWindowPos( s_wcd.hwndButtonClear, NULL, x, y, w, h, 0 );

			s_wcd.windowWidth = cx;
			s_wcd.windowHeight = cy;
			// -NERVE - SMF
			break;
		case WM_ACTIVATE:
			if ( LOWORD( wParam ) != WA_INACTIVE ) {
				SetFocus( s_wcd.hwndInputLine );
			}
			break;

		case WM_CLOSE:
			return 0;
		case WM_CTLCOLORSTATIC:
			if ( ( HWND ) lParam == s_wcd.hwndBuffer ) {
				SetBkColor( ( HDC ) wParam, RGB( 0, 0, 0 ) );
				SetTextColor( ( HDC ) wParam, RGB( 0xff, 0xff, 0xff ) );
				return ( long ) s_wcd.hbrEditBackground;

			}
			break;

		case WM_COMMAND:
			if ( wParam == COPY_ID ) {
				SendMessageA( s_wcd.hwndBuffer, EM_SETSEL, 0, -1 );
				SendMessageA( s_wcd.hwndBuffer, WM_COPY, 0, 0 );
			} else if ( wParam == CLEAR_ID )   {
				SendMessageA( s_wcd.hwndBuffer, EM_SETSEL, 0, -1 );
				SendMessageA( s_wcd.hwndBuffer, EM_REPLACESEL, FALSE, ( LPARAM ) "" );
				UpdateWindow( s_wcd.hwndBuffer );
			}
			break;
		case WM_CREATE:
	//		s_wcd.hbmLogo = LoadBitmap( g_wv.hInstance, MAKEINTRESOURCE( IDB_BITMAP1 ) );
	//		s_wcd.hbmClearBitmap = LoadBitmap( g_wv.hInstance, MAKEINTRESOURCE( IDB_BITMAP2 ) );
			s_wcd.hbrEditBackground = CreateSolidBrush( RGB( 0, 0, 0 ) );
	//		s_wcd.hbrErrorBackground = CreateSolidBrush( RGB( 0x80, 0x80, 0x80 ) );
			SetTimer( hWnd, 1, 1000, NULL );
			break;
		case WM_TIMER:
			if ( wParam == 1 ) {
				s_timePolarity = !s_timePolarity;
				if ( s_wcd.hwndErrorBox ) {
					InvalidateRect( s_wcd.hwndErrorBox, NULL, FALSE );
				}
			}
			break;
		}
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}

	LONG WINAPI DebugConsole::InputLineWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
		char inputBuffer[1024];

		switch ( uMsg )
		{
		case WM_KILLFOCUS:
			if ( ( HWND ) wParam == s_wcd.hWnd ||
				( HWND ) wParam == s_wcd.hwndErrorBox ) {
				SetFocus( hWnd );
				return 0;
			}
			break;

		case WM_CHAR:
			if ( wParam == 13 ) {
				GetWindowText( s_wcd.hwndInputLine, inputBuffer, sizeof( inputBuffer ) );
				strncat( s_wcd.consoleText, inputBuffer, sizeof( s_wcd.consoleText ) - strlen( s_wcd.consoleText ) - 5 );
				strcat( s_wcd.consoleText, "\n" );
				SetWindowText( s_wcd.hwndInputLine, "" );

				//CON_Print( va( "]%s\n", inputBuffer ) );

				return 0;
			}
		}

		return CallWindowProc( s_wcd.SysInputLineWndProc, hWnd, uMsg, wParam, lParam );
	}

	LONG WINAPI DebugConsole::S_ConWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
		class DebugConsole* cl = (class DebugConsole*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if(cl == NULL)
		{
			if(uMsg == WM_NCCREATE)
			{
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
				cl = (DebugConsole*)(lpcs->lpCreateParams);
				SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)cl);
			}
			return DefWindowProc( hWnd, uMsg, wParam, lParam ); //early callback
		}
		return cl->ConWndProc(hWnd, uMsg, wParam, lParam);
	}



	LONG WINAPI DebugConsole::S_InputLineWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
		class DebugConsole* cl = (class DebugConsole*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if(cl == NULL)
		{
			return DefWindowProc( hWnd, uMsg, wParam, lParam ); //early callback
		}
		return cl->InputLineWndProc(hWnd, uMsg, wParam, lParam);
	}

	/*
	** Sys_CreateConsole
	*/
	void DebugConsole::InitInternal( HINSTANCE hInstance, const char* winname, int initialwidth, int initialheigth) {
		HDC hDC;
		WNDCLASSA wc;
		RECT rect;
		const char *DEDCLASS = winname;
		const char *WINDOWNAME = winname;

		int nHeight;
		int swidth, sheight;
		int DEDSTYLE = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX | WS_SIZEBOX;

		memset( &wc, 0, sizeof( wc ) );

		wc.style         = 0;
		wc.lpfnWndProc   = S_ConWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_ICON1 ) );
		wc.hCursor       = LoadCursor( NULL,IDC_ARROW );
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.lpszMenuName  = 0;
		wc.lpszClassName = DEDCLASS;

		if ( !RegisterClass( &wc ) ) {
			throw std::runtime_error("Couldn't register class.");
		}
		rect.left = 0;
		rect.top = 0;
		if(initialwidth == 0 || initialheigth == 0)
		{
			rect.bottom = SYSCON_DEFAULT_HEIGHT;
			rect.right = SYSCON_DEFAULT_WIDTH;
		}else{
			rect.bottom = initialheigth;
			rect.right = initialwidth;			
		}
		AdjustWindowRect( &rect, DEDSTYLE, FALSE );

		hDC = GetDC( GetDesktopWindow() );
		swidth = GetDeviceCaps( hDC, HORZRES );
		sheight = GetDeviceCaps( hDC, VERTRES );
		ReleaseDC( GetDesktopWindow(), hDC );

		s_wcd.windowWidth = rect.right - rect.left + 1;
		s_wcd.windowHeight = rect.bottom - rect.top + 1;

		s_wcd.hWnd = CreateWindowEx( 0,
									DEDCLASS,
									WINDOWNAME,
									DEDSTYLE,
									( swidth - 600 ) / 2, ( sheight - 450 ) / 2, rect.right - rect.left + 1, rect.bottom - rect.top + 1,
									NULL,
									NULL,
									hInstance,
									this );

		if ( s_wcd.hWnd == NULL ) {
			throw std::runtime_error("Couldn't create window.");
		}

		SetWindowLongPtr(s_wcd.hWnd, GWLP_USERDATA, (LONG_PTR)this);

		//
		// create fonts
		//
		hDC = GetDC( s_wcd.hWnd );
		nHeight = -MulDiv( 8, GetDeviceCaps( hDC, LOGPIXELSY ), 72 );

		s_wcd.hfBufferFont = CreateFont( nHeight,
										0,
										0,
										0,
										FW_LIGHT,
										0,
										0,
										0,
										DEFAULT_CHARSET,
										OUT_DEFAULT_PRECIS,
										CLIP_DEFAULT_PRECIS,
										DEFAULT_QUALITY,
										FF_MODERN | FIXED_PITCH,
										"Courier New" );

		ReleaseDC( s_wcd.hWnd, hDC );






		int cx = s_wcd.windowWidth;
		int cy = s_wcd.windowHeight;
	/*
			if ( cx < SYSCON_DEFAULT_WIDTH )
				cx = SYSCON_DEFAULT_WIDTH;
			if ( cy < SYSCON_DEFAULT_HEIGHT )
				cy = SYSCON_DEFAULT_HEIGHT;
	*/
		int sx = (float)cx / SYSCON_DEFAULT_WIDTH;
			//sy = (float)cy / SYSCON_DEFAULT_HEIGHT;

		int x = 8;
		int y = 6;
		int w = cx - 15;
		int h = cy - 60;

		//
		// create the scrollbuffer
		//
		s_wcd.hwndBuffer = CreateWindow( "edit", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
										ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
										x, y, w, h,
										s_wcd.hWnd,
										( HMENU ) EDIT_ID,             // child window ID
										hInstance, NULL );

		y = y + h + 4;
		h = 20;
		//SetWindowPos( s_wcd.hwndInputLine, NULL, x, y, w, h, 0 );

		//
		// create the input line
		//
		s_wcd.hwndInputLine = CreateWindow( "edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
											ES_LEFT | ES_AUTOHSCROLL,
											x, y, w, h,
											s_wcd.hWnd,
											( HMENU ) INPUT_ID,         // child window ID
											hInstance, NULL );

		SetWindowLongPtr(s_wcd.hwndInputLine, GWLP_USERDATA, (LONG_PTR)this);
		s_wcd.SysInputLineWndProc = ( WNDPROC ) SetWindowLongPtr( s_wcd.hwndInputLine, GWL_WNDPROC, ( LONG_PTR ) S_InputLineWndProc );


		y = y + h + 4;
		w = 72 * sx;
		h = 24;

		//
		// create the buttons
		//
		s_wcd.hwndButtonCopy = CreateWindow( "button", "copy", BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
											x, y, w, h,
											s_wcd.hWnd,
											( HMENU ) COPY_ID,         // child window ID
											hInstance, NULL );
		//SendMessageA( s_wcd.hwndButtonCopy, WM_SETTEXT, 0, ( LPARAM ) "copy" );



		x = cx - 6 - w;


		s_wcd.hwndButtonClear = CreateWindow( "button", "clear", BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
											x, y, w, h,
											s_wcd.hWnd,
											( HMENU ) CLEAR_ID,       // child window ID
											hInstance, NULL );
		//SendMessageA( s_wcd.hwndButtonClear, WM_SETTEXT, 0, ( LPARAM ) "clear" );


		SendMessageA( s_wcd.hwndBuffer, WM_SETFONT, ( WPARAM ) s_wcd.hfBufferFont, 0 );
		SendMessageA( s_wcd.hwndInputLine, WM_SETFONT, ( WPARAM ) s_wcd.hfBufferFont, 0 );

		ShowWindow( s_wcd.hWnd, SW_SHOWDEFAULT );
		UpdateWindow( s_wcd.hWnd );
		SetForegroundWindow( s_wcd.hWnd );
		SetFocus( s_wcd.hwndInputLine );

		s_wcd.visLevel = 1;
	}
		/*
	** Sys_DestroyConsole
	*/
	void DebugConsole::Shutdown( void ) {
		if ( s_wcd.hWnd ) {
			ShowWindow( s_wcd.hWnd, SW_HIDE );
			CloseWindow( s_wcd.hWnd );
			DestroyWindow( s_wcd.hWnd );
			s_wcd.hWnd = 0;
		}
	}

