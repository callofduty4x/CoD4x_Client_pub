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

#ifndef __WIN_DEBUGCON_H__
#define __WIN_DEBUGCON_H__

#include <windows.h>
#include <string>

class DebugConsole
{

public:
	/*
	** Sys_ShowConsole
	*/
	void Show( int visLevel, bool quitOnClose );

	/*
	** Sys_ConsoleInput
	*/
	char *Input( void ) ;

	/*
	** Conbuf_AppendText
	*/
	void Print( const char *pMsg );
	void Print( const std::string &pMsg );

	void DisableDraw();
	void EnableDraw() ;

	DebugConsole(HINSTANCE hInstance, const char* title);
	DebugConsole(HINSTANCE hInstance, const char* title, int initialwidth, int initialheigth);

	virtual ~DebugConsole();

protected:
	void PrintInternal( const char *pMsg );

private:

	LONG WINAPI ConWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) ;
	LONG WINAPI InputLineWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) ;
	static LONG WINAPI S_ConWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) ;
	static LONG WINAPI S_InputLineWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) ;


	/*
	** Sys_CreateConsole
	*/
	void InitInternal( HINSTANCE hInstance, const char* winname, int initialwidth, int initialheigth);
		/*
	** Sys_DestroyConsole
	*/
	void Shutdown( void ) ;

	struct WinConData
	{
		HWND hWnd;
		HWND hwndBuffer;

		HWND hwndButtonClear;
		HWND hwndButtonCopy;
		HWND hwndButtonQuit;

		HWND hwndErrorBox;
		HWND hwndErrorText;

		HBITMAP hbmLogo;
		HBITMAP hbmClearBitmap;

		HBRUSH hbrEditBackground;
		HBRUSH hbrErrorBackground;

		HFONT hfBufferFont;
		HFONT hfButtonFont;

		HWND hwndInputLine;

		char errorString[80];

		char consoleText[512], returnedText[512];
		int visLevel;
		bool quitOnClose;
		int windowWidth, windowHeight;

		WNDPROC SysInputLineWndProc;

	};

	WinConData s_wcd;

	unsigned long s_totalChars;
	bool s_timePolarity;
};


#endif
