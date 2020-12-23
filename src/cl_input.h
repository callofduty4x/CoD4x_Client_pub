/*
===========================================================================

Return to Castle Wolfenstein multiplayer GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Return to Castle Wolfenstein multiplayer GPL Source Code (?RTCW MP Source Code?).  

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

#ifndef __CL_INPUT_H__
#define __CL_INPUT_H__

#include "keycodes.h"
#include "q_shared.h"

#define playersKb ((kbutton_t*)(0x8F1518))

//
// cl_input
//
typedef struct {
	int down[2];                // key nums holding it down
	unsigned downtime;          // msec timestamp
	unsigned msec;              // msec down this frame if both a down and up happened
	byte active;            // current state
	byte wasPressed;        // set when down, not cleared when up
	byte pad[2];
} kbutton_t;

typedef enum {
	KB_LEFT,
	KB_RIGHT,
	KB_FORWARD,
	KB_BACK,
	KB_LOOKUP,
	KB_LOOKDOWN,
	KB_MOVELEFT,
	KB_MOVERIGHT,
	KB_STRAFE,
	KB_SPEED,
	KB_UP,
	KB_DOWN,
	KB_BUTTONS0,
	KB_BUTTONS1,
	KB_BUTTONS2,
	KB_BUTTONS3,
	KB_BUTTONS4,
	KB_BUTTONS5,
	KB_BUTTONS6,
	KB_BUTTONS7,
	KB_WBUTTONS0,
	KB_WBUTTONS1,
	KB_WBUTTONS2,
	KB_WBUTTONS3,
	KB_WBUTTONS4,
	KB_WBUTTONS5,
	KB_WBUTTONS6,
	KB_WBUTTONS7,
	KB_MLOOK,
	KB_KICK,
	NUM_BUTTONS
} kbuttons_t;


void IN_Activate( qboolean active );
//void IN_MouseEvent( int mstate );
void IN_ActivateWin32Mouse( void );
void IN_ActivateMouse( void );
void IN_DeactivateMouse( void );
void IN_MouseEvent( int mstate );


typedef struct 
{
  float deltaTime;
  float pitch;
  float pitchAxis;
  float pitchMax;
  float yaw;
  float yawAxis;
  float yawMax;
  float forwardAxis;
  float rightAxis;
  int buttons;
  int localClientNum;
  playerState_t *ps;
}AimInput;


typedef struct 
{
  float pitch;
  float yaw;
  float meleeChargeYaw;
  char meleeChargeDist;
  char pad[3];
}AimOutput;




/* 7871 */
typedef struct
{
  float slowdownRegionWidth;
  float slowdownRegionHeight;
  float autoAimRegionWidth;
  float autoAimRegionHeight;
  float autoMeleeRegionWidth;
  float autoMeleeRegionHeight;
  float lockOnRegionWidth;
  float lockOnRegionHeight;
}AimTweakables;

/* 7872 */
typedef struct
{
  int entIndex;
  float clipMins[2];
  float clipMaxs[2];
  float aimPos[3];
  float velocity[3];
  float distSqr;
  float crosshairDistSqr;
}AimScreenTarget;


/* 7873 */
typedef struct 
{
  byte initialized;
  byte pad[3];
  __attribute__((aligned(4))) AimTweakables tweakables;
  float viewOrigin[3];
  float viewAngles[3];
  float viewAxis[3][3];
  float fovTurnRateScale;
  float fovScaleInv;
  float fWeaponPosFrac;
  float pitchDelta;
  float yawDelta;
  float screenWidth;
  float screenHeight;
  float screenMtx[4][4];
  float invScreenMtx[4][4];
  AimScreenTarget screenTargets[64];
  int screenTargetCount;
  int autoAimTargetEnt;
  byte autoAimPressed;
  byte autoAimActive;
  byte pad2[2];
  __attribute__((aligned(4))) float autoAimPitch;
  float autoAimPitchTarget;
  float autoAimYaw;
  float autoAimYawTarget;
  int autoMeleeTargetEnt;
  byte autoMeleeActive;
  byte autoMeleePressed;
  byte pad3[2];
  __attribute__((aligned(4))) float autoMeleePitch;
  float autoMeleePitchTarget;
  float autoMeleeYaw;
  float autoMeleeYawTarget;
  int unkTragetEnt;
}AimAssistGlobals;

#define aaGlobArray (*((AimAssistGlobals*)(0x72AF08)))

#endif