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

#include "q_shared.h"
#include "qcommon.h"
#include "keys.h"
#include "cl_input.h"
#include <ctype.h>

/*
===================
Key_ClearStates
===================
*/
void Key_ClearStates( int localClientNum ) {
	int i;

	playerKeys.anyKeyDown = qfalse;

	for ( i = 0 ; i < MAX_KEYS ; i++ ) {
		if ( playerKeys.keys[i].down ) {
			CL_KeyEvent(0, i, qfalse, 0 );
		}
		playerKeys.keys[i].down = 0;
		playerKeys.keys[i].repeats = 0;
	}
}

#define specialKeynameTable ((const char**)(0x727548))
#define keynames_translated ((keyname_t*)(0x727248))
#define keynames ((keyname_t*)(0x726F48))

/*
===================
Key_KeynumToString
Returns a string (either a single ascii char, a K_* name, or a 0x11 hex string) for the
given keynum.
===================
*/
const char *Key_KeynumToString( int keynum, qboolean bTranslate ) {
	keyname_t   *kn;
	static char tinystr[5];
	int i, j;
	unsigned int specialKeyEntry;

	if ( keynum == -1 ) {
		return "<KEY NOT FOUND>";
	}

	if ( keynum < 0 || keynum > 255 ) {
		return "<OUT OF RANGE>";
	}

	if ( bTranslate && loc_language->integer == 1)
	{
		specialKeyEntry = keynum - '0';
		if ( specialKeyEntry <= 9 )
		{
			return specialKeynameTable[specialKeyEntry];
		}
	}

	kn = keynames;    //init to english

	// check for printable ascii (don't use quote)
	if ( keynum > 32 && keynum < 127 && keynum != '"' ) {
		tinystr[0] = toupper(keynum);
		tinystr[1] = 0;
		return tinystr;
	}else{

		if ( bTranslate ) {
			kn = keynames_translated;
		}

	}

	// check for a key string
	for ( ; kn->name ; kn++ ) {
		if ( keynum == kn->keynum ) {
			return kn->name;
		}
	}

	// make a hex string
	i = keynum >> 4;
	j = keynum & 15;

	tinystr[0] = '0';
	tinystr[1] = 'x';
	tinystr[2] = i > 9 ? i - 10 + 'a' : i + '0';
	tinystr[3] = j > 9 ? j - 10 + 'a' : j + '0';
	tinystr[4] = 0;

	return tinystr;
}

void Key_KeynumToStringBuf(signed int keynum, char *outstring, int size)
{
  Q_strncpyz(outstring, Key_KeynumToString(keynum, 1), size);
}

/*
============
Key_Bindlist_f
============
*/
void Key_Bindlist_f( void ) {
	int i;

	for ( i = 0 ; i < MAX_KEYS ; i++ ) {
		if ( playerKeys.keys[i].binding && playerKeys.keys[i].binding[0] ) {
			Com_Printf(CON_CHANNEL_DONT_FILTER, "%s \"%s\"\n", Key_KeynumToString( i, qfalse ), playerKeys.keys[i].binding );
		}
	}
}

void REGPARM(1) AimAssist_ApplyMeleeCharge(AimInput *a1, AimOutput *a2);

void AimAssist_UpdateMouseInput(AimInput *input, AimOutput *output)
{

//  playerState_t* ps;

  output->pitch = input->pitch;
  output->yaw = input->yaw;

  output->meleeChargeYaw = 0.0;
  output->meleeChargeDist = 0;
/*
  if ( aaGlobArray.initialized )
  {
    AimAssist_UpdateTweakables(input);
    ps = input->ps;
    aaGlobArray.fWeaponPosFrac = ps->fWeaponPosFrac;
    if ( ps->eFlags & 0x300 )
    {
      if ( input->buttons & 0x800 )
	  {
        aaGlobArray.fWeaponPosFrac = 1.0;
	  }
    }
    AimAssist_ApplyAutoMelee(input, output);
    AimAssist_ApplyMeleeCharge(input, output);

  }
*/
}

int Key_GetCommandAssignment(int localClientNum, char *search, int *twokeys)
{
  int count;
  int i;

  count = 0;
  i = 0;
  twokeys[1] = -1;
  *twokeys = -1;
  qkey_t* keys = playerKeys.keys;

  for(i = 0; i < 256; ++i)
  {
		if ( !keys[i].binding )
		{
			continue;
		}

		if ( Q_stricmp(keys[i].binding, search) == qfalse )
		{
			twokeys[count++] = i;
			if ( count == 2 )
			{
			  break;
			}
		}
  }
  return count;
}

int CL_GetKeyBinding(int localClientNum, char *search, char *outstring)
{
  int result;
  int twokeys[2];

  outstring[128] = 0;
  result = Key_GetCommandAssignment(localClientNum, search, twokeys);
  if ( result )
  {
    Key_KeynumToStringBuf(twokeys[0], outstring, 128);
    if ( result == 2 )
	{
      Key_KeynumToStringBuf(twokeys[1], outstring + 128, 128);
	}
  }
  else
  {
    Q_strncpyz(outstring ,"KEY_UNBOUND", 256);
  }
  return result;
}

qboolean Key_IsActive(int keynum)
{
	return playersKb[keynum].active;
}

qboolean IN_IsTalkKeyHeld()
{
  return Key_IsActive(29);
}
