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

#ifndef __GAME_H__
#define __GAME_H__

#include "q_shared.h"

#define	MAX_STATS				16
#define	MAX_PERSISTANT			16
#define	MAX_POWERUPS			16

#define g_entities ((gentity_t*)(g_entities_ADDR))
#define g_gametypes ((gametypes_t*)(g_gametypes_ADDR))


// NOTE: we can only use up to 15 in the client-server stream
// SA NOTE: should be 31 now (I added 1 bit in msg.c)
typedef enum {
	WP_NONE,                // 0

	WP_KNIFE,               // 1
	// German weapons
	WP_LUGER,               // 2
	WP_MP40,                // 3
	WP_MAUSER,              // 4
	WP_FG42,                // 5
	WP_GRENADE_LAUNCHER,    // 6
	WP_PANZERFAUST,         // 7
	WP_VENOM,               // 8
	WP_FLAMETHROWER,        // 9
	WP_TESLA,               // 10
	WP_SPEARGUN,            // 11

// weapon keys only go 1-0, so put the alternates above that (since selection will be a double click on the german weapon key)

	// American equivalents
	WP_KNIFE2,              // 12
	WP_COLT,                // 13	equivalent american weapon to german luger
	WP_THOMPSON,            // 14	equivalent american weapon to german mp40
	WP_GARAND,              // 15	equivalent american weapon to german mauser
	WP_BAR,                 // 16	equivalent american weapon to german fg42
	WP_GRENADE_PINEAPPLE,   // 17
	WP_ROCKET_LAUNCHER,     // 18	equivalent american weapon to german panzerfaust

	// secondary fire weapons
	WP_SNIPERRIFLE,         // 19
	WP_SNOOPERSCOPE,        // 20
	WP_VENOM_FULL,          // 21
	WP_SPEARGUN_CO2,        // 22
	WP_FG42SCOPE,           // 23	fg42 alt fire
	WP_BAR2,                // 24

	// more weapons
	WP_STEN,                // 25	silenced sten sub-machinegun
	WP_MEDIC_SYRINGE,       // 26	// JPW NERVE -- broken out from CLASS_SPECIAL per Id request
	WP_AMMO,                // 27	// JPW NERVE likewise
	WP_ARTY,                // 28
	WP_SILENCER,            // 29	// used to be sp5
	WP_AKIMBO,              // 30	//----(SA)	added

// jpw
	WP_CROSS,               // 31
	WP_DYNAMITE,            // 32
	WP_DYNAMITE2,           // 33
	WP_PROX,                // 34

	WP_MONSTER_ATTACK1,     // 35	// generic monster attack, slot 1
	WP_MONSTER_ATTACK2,     // 36	// generic monster attack, slot 2
	WP_MONSTER_ATTACK3,     // 37	// generic monster attack, slot 2

	WP_SMOKETRAIL,          // 38

	WP_GAUNTLET,            // 39

	WP_SNIPER,              // 40
	WP_MORTAR,              // 41
	VERYBIGEXPLOSION,       // 42	// explosion effect for airplanes

	// NERVE - SMF - special weapons are here now
	WP_MEDKIT,              // 43
	WP_PLIERS,              // 44
	WP_SMOKE_GRENADE,       // 45
	// -NERVE - SMF
	WP_BINOCULARS,          // 46

	WP_NUM_WEAPONS          // 47   NOTE: this cannot be larger than 64 for AI/player weapons!

} weapon_t;



// player_state->stats[] indexes
typedef enum {
	STAT_HEALTH,
	STAT_HOLDABLE_ITEM,
//	STAT_WEAPONS,					// 16 bit fields
	STAT_ARMOR,
//----(SA) Keys for Wolf
	STAT_KEYS,                      // 16 bit fields
//----(SA) end
	STAT_DEAD_YAW,                  // look this direction when dead (FIXME: get rid of?)
	STAT_CLIENTS_READY,             // bit mask of clients wishing to exit the intermission (FIXME: configstring?)
	STAT_MAX_HEALTH,                // health / armor limit, changable by handicap
	STAT_PLAYER_CLASS,              // DHM - Nerve :: player class in multiplayer
	STAT_CAPTUREHOLD_RED,           // JPW NERVE - red team score
	STAT_CAPTUREHOLD_BLUE           // JPW NERVE - blue team score
} statIndex_t;


// player_state->persistant[] indexes
// these fields are the only part of player_state that isn't
// cleared on respawn
typedef enum {
	PERS_SCORE,                     // !!! MUST NOT CHANGE, SERVER AND GAME BOTH REFERENCE !!!
	PERS_HITS,                      // total points damage inflicted so damage beeps can sound on change
	PERS_RANK,
	PERS_TEAM,
	PERS_SPAWN_COUNT,               // incremented every respawn
	PERS_REWARD_COUNT,              // incremented for each reward sound
	PERS_REWARD,                    // a reward_t
	PERS_ATTACKER,                  // clientnum of last damage inflicter
	PERS_KILLED,                    // count of the number of times you died
	// these were added for single player awards tracking
	PERS_RESPAWNS_LEFT,             // DHM - Nerve :: number of remaining respawns

	PERS_ACCURACY_SHOTS,
	PERS_ACCURACY_HITS,

	// Rafael - mg42		// (SA) I don't understand these here.  can someone explain?
	PERS_HWEAPON_USE,
	// Rafael wolfkick
	PERS_WOLFKICK
} persEnum_t;



// means of death
// COD4: raw\maps\mp\gametypes\_missions.gsc
typedef enum
{
	MOD_UNKNOWN,
	MOD_PISTOL_BULLET,
	MOD_RIFLE_BULLET,
	MOD_GRENADE,
	MOD_GRENADE_SPLASH,
	MOD_PROJECTILE,
	MOD_PROJECTILE_SPLASH,
	MOD_MELEE,
	MOD_HEAD_SHOT,
	MOD_CRUSH,
	MOD_TELEFRAG,
	MOD_FALLING,
	MOD_SUICIDE,
	MOD_TRIGGER_HURT,
	MOD_EXPLOSIVE,
	MOD_IMPACT,

	MOD_DUMMY

} meansOfDeath_t;

#define MOD_NUM	16
/*
char *modNames[MOD_NUM] =
{
	"MOD_UNKNOWN",
	"MOD_PISTOL_BULLET",
	"MOD_RIFLE_BULLET",
	"MOD_GRENADE",
	"MOD_GRENADE_SPLASH",
	"MOD_PROJECTILE",
	"MOD_PROJECTILE_SPLASH",
	"MOD_MELEE",
	"MOD_HEAD_SHOT",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TRIGGER_HURT",
	"MOD_EXPLOSIVE",
	"MOD_IMPACT"
};

//hit locations
static const char *g_HitLocNames[] =
{
	"none",
	"helmet",
	"head",
	"neck",
	"torso_upper",
	"torso_lower",
	"right_arm_upper",
	"left_arm_upper",
	"right_arm_lower",
	"left_arm_lower",
	"right_hand",
	"left_hand",
	"right_leg_upper",
	"left_leg_upper",
	"right_leg_lower",
	"left_leg_lower",
	"right_foot",
	"left_foot",
	"gun",
};

*/





//---------------------------------------------------------

typedef struct{
    int	score; //0x2f78
    int	deaths; //0x2f7c
    int	kills; //0x2f80
    int	assists; //0x2f84
}clientScoreboard_t;


typedef struct {
	byte		linked;				//0xf4 qfalse if not in any good cluster

	byte		bmodel;				//0xf5 if false, assume an explicit mins / maxs bounding box
							// only set by trap_SetBrushModel
	byte		svFlags;
	byte		pad1;
	int clientMask[2];

	byte		inuse;
	byte		pad2[3];
	int			broadcastTime;
	vec3_t		mins, maxs;		//0x108  //0x114  from SharedEntity_t

	int		contents;		// CONTENTS_TRIGGER, CONTENTS_SOLID, CONTENTS_BODY, etc
									// a non-solid entity should set to 0

	vec3_t		absmin, absmax;		//0x124  //0x130 derived from mins/maxs and origin + rotation

	// currentOrigin will be used for all collision detection and world linking.
	// it will not necessarily be the same as the trajectory evaluation for the current
	// time, because each entity must be moved one at a time after time is advanced
	// to avoid simultanious collision issues
	vec3_t		currentOrigin;		//0x13c
	vec3_t		currentAngles;		//0x148

	// when a trace call is made and passEntityNum != ENTITYNUM_NONE,
	// an ent will be excluded from testing if:
	// ent->s.number == passEntityNum	(don't interact with self)
	// ent->r.ownerNum == passEntityNum	(don't interact with your own missiles)
	// entity[ent->r.ownerNum].r.ownerNum == passEntityNum	(don't interact with other missiles from owner)
	uint16_t	ownerNum;	//0x154
	uint16_t	pad3;
	int			eventTime;
} entityShared_t;


struct entityState_s;

// the server looks at a sharedEntity, which is the start of the game's gentity_t structure
typedef struct {
	entityState_t	s;				// communicated by server to clients
	entityShared_t	r;				// shared by both the server system and game
} sharedEntity_t;






// gentity->flags
#define FL_GODMODE              0x00000010
#define FL_NOTARGET             0x00000020
#define FL_TEAMSLAVE            0x00000400  // not the first on the team
#define FL_NO_KNOCKBACK         0x00000800
#define FL_DROPPED_ITEM         0x00001000
#define FL_NO_BOTS              0x00002000  // spawn point not for bot use
#define FL_NO_HUMANS            0x00004000  // spawn point just for bots
#define FL_AI_GRENADE_KICK      0x00008000  // an AI has already decided to kick this grenade
// Rafael
#define FL_NOFATIGUE            0x00010000  // cheat flag no fatigue

#define FL_TOGGLE               0x00020000  //----(SA)	ent is toggling (doors use this for ex.)
#define FL_KICKACTIVATE         0x00040000  //----(SA)	ent has been activated by a kick (doors use this too for ex.)
#define FL_SOFTACTIVATE         0x00000040  //----(SA)	ent has been activated while 'walking' (doors use this too for ex.)
#define FL_DEFENSE_GUARD        0x00080000  // warzombie defense pose

#define FL_PARACHUTE            0x00100000
#define FL_WARZOMBIECHARGE      0x00200000
#define FL_NO_MONSTERSLICK      0x00400000
#define FL_NO_HEADCHECK         0x00800000

#define FL_NODRAW               0x01000000




// contents flags are seperate bits
// a given brush can contribute multiple content bits
// multiple brushes can be in a single leaf

// these definitions also need to be in q_shared.h!

// lower bits are stronger, and will eat weaker brushes completely
#define CONTENTS_SOLID          1       // an eye is never valid in a solid


#define CONTENTS_PLAYERCLIP     0x10000
#define CONTENTS_MONSTERCLIP    0x20000
#define CONTENTS_VEHICLECLIP	//??
#define CONTENTS_ITEMCLIP
#define CONTENTS_NODROP
#define CONTENTS_NONSOLID



#define CONTENTS_BODY           0x2000000   // should never be on a brush, only in game






/*
#define CONTENTS_WINDOW         2       // translucent, but not watery
#define CONTENTS_AUX            4
#define CONTENTS_LAVA           8
#define CONTENTS_SLIME          16
#define CONTENTS_WATER          32
#define CONTENTS_MIST           64
#define LAST_VISIBLE_CONTENTS   64

// remaining contents are non-visible, and don't eat brushes

#define CONTENTS_AREAPORTAL     0x8000


// currents can be added to any other contents, and may be mixed
#define CONTENTS_CURRENT_0      0x40000
#define CONTENTS_CURRENT_90     0x80000
#define CONTENTS_CURRENT_180    0x100000
#define CONTENTS_CURRENT_270    0x200000
#define CONTENTS_CURRENT_UP     0x400000
#define CONTENTS_CURRENT_DOWN   0x800000

#define CONTENTS_ORIGIN         0x1000000   // removed before bsping an entity

#define CONTENTS_MONSTER        0x2000000   // should never be on a brush, only in game
#define CONTENTS_DEADMONSTER    0x4000000
#define CONTENTS_DETAIL         0x8000000   // brushes to be added after vis leafs
//renamed because it's in conflict with the Q3A translucent contents
#define CONTENTS_Q2TRANSLUCENT  0x10000000  // auto set if any surface has trans
#define CONTENTS_LADDER         0x20000000

*/

/*

// contents flags are seperate bits
// a given brush can contribute multiple content bits

// these definitions also need to be in q_shared.h!

#define CONTENTS_SOLID          1       // an eye is never valid in a solid

#define CONTENTS_LIGHTGRID      4   //----(SA)	added

#define CONTENTS_LAVA           8
#define CONTENTS_SLIME          16
#define CONTENTS_WATER          32
#define CONTENTS_FOG            64


//----(SA) added
#define CONTENTS_MISSILECLIP    128 // 0x80
#define CONTENTS_ITEM           256 // 0x100
//----(SA) end
#define CONTENTS_MOVER          0x4000
#define CONTENTS_AREAPORTAL     0x8000

#define CONTENTS_PLAYERCLIP     0x10000
#define CONTENTS_MONSTERCLIP    0x20000

//bot specific contents types
#define CONTENTS_TELEPORTER     0x40000
#define CONTENTS_JUMPPAD        0x80000
#define CONTENTS_CLUSTERPORTAL  0x100000
#define CONTENTS_DONOTENTER     0x200000
#define CONTENTS_DONOTENTER_LARGE       0x400000


#define CONTENTS_ORIGIN         0x1000000   // removed before bsping an entity

#define CONTENTS_BODY           0x2000000   // should never be on a brush, only in game
#define CONTENTS_CORPSE         0x4000000
#define CONTENTS_DETAIL         0x8000000   // brushes not used for the bsp

#define CONTENTS_STRUCTURAL     0x10000000  // brushes used for the bsp
#define CONTENTS_TRANSLUCENT    0x20000000  // don't consume surface fragments inside
#define CONTENTS_TRIGGER        0x40000000
#define CONTENTS_NODROP         0x80000000  // don't leave bodies or items (death fog, lava)

*/

// g_dmflags->integer flags
#define DF_NO_FALLING           8
#define DF_FIXED_FOV            16
#define DF_NO_FOOTSTEPS         32
#define DF_NO_WEAPRELOAD        64

// content masks
#define MASK_ALL                ( -1 )
#define MASK_SOLID              ( CONTENTS_SOLID )
#define MASK_PLAYERSOLID        ( CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_BODY )
#define MASK_DEADSOLID          ( CONTENTS_SOLID | CONTENTS_PLAYERCLIP )
#define MASK_WATER              ( CONTENTS_WATER | CONTENTS_LAVA | CONTENTS_SLIME )
//#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA)
#define MASK_OPAQUE             ( CONTENTS_SOLID | CONTENTS_LAVA )      //----(SA)	modified since slime is no longer deadly
#define MASK_SHOT               ( CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_CORPSE )
#define MASK_MISSILESHOT        ( MASK_SHOT | CONTENTS_MISSILECLIP )






#define SURF_LIGHT      0x1     // value will hold the light strength

#define SURF_SLICK      0x2     // effects game physics

#define SURF_SKY        0x4     // don't draw, but add to skybox
#define SURF_WARP       0x8     // turbulent water warp
#define SURF_TRANS33    0x10
#define SURF_TRANS66    0x20
#define SURF_FLOWING    0x40    // scroll towards angle
#define SURF_NODRAW     0x80    // don't bother referencing the texture

#define SURF_HINT       0x100   // make a primary bsp splitter
#define SURF_SKIP       0x200   // completely ignore, allowing non-closed brushes

#define SURF_MONSTERSLICK       0x4000000   // slick surf that only affects ai's




// movers are things like doors, plats, buttons, etc
typedef enum {
	MOVER_POS1,
	MOVER_POS2,
	MOVER_POS3,
	MOVER_1TO2,
	MOVER_2TO1,
	// JOSEPH 1-26-00
	MOVER_2TO3,
	MOVER_3TO2,
	// END JOSEPH

	// Rafael
	MOVER_POS1ROTATE,
	MOVER_POS2ROTATE,
	MOVER_1TO2ROTATE,
	MOVER_2TO1ROTATE
} moverState_t;


// door AI sound ranges
#define HEAR_RANGE_DOOR_LOCKED      128 // really close since this is a cruel check
#define HEAR_RANGE_DOOR_KICKLOCKED  512
#define HEAR_RANGE_DOOR_OPEN        256
#define HEAR_RANGE_DOOR_KICKOPEN    768

// DHM - Nerve :: Worldspawn spawnflags to indicate if a gametype is not supported
#define NO_GT_WOLF      1
#define NO_STOPWATCH    2
#define NO_CHECKPOINT   4

//============================================================================

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;


//====================================================================
//
// Scripting, these structure are not saved into savegames (parsed each start)
typedef struct
{
	char    *actionString;
	qboolean ( *actionFunc )( gentity_t *ent, char *params );
} g_script_stack_action_t;
//
typedef struct
{
	//
	// set during script parsing
	g_script_stack_action_t     *action;            // points to an action to perform
	char                        *params;
} g_script_stack_item_t;
//
#define G_MAX_SCRIPT_STACK_ITEMS    64
//
typedef struct
{
	g_script_stack_item_t items[G_MAX_SCRIPT_STACK_ITEMS];
	int numItems;
} g_script_stack_t;
//
typedef struct
{
	int eventNum;                           // index in scriptEvents[]
	char                *params;            // trigger targetname, etc
	g_script_stack_t stack;
} g_script_event_t;
//
typedef struct
{
	char        *eventStr;
	qboolean ( *eventMatch )( g_script_event_t *event, char *eventParm );
} g_script_event_define_t;
//
// Script Flags
#define SCFL_GOING_TO_MARKER    0x1
#define SCFL_ANIMATING          0x2
//
// Scripting Status (NOTE: this MUST NOT contain any pointer vars)
typedef struct
{
	int scriptStackHead, scriptStackChangeTime;
	int scriptEventIndex;       // current event containing stack of actions to perform
	// scripting system variables
	int scriptId;                   // incremented each time the script changes
	int scriptFlags;
	char    *animatingParams;
} g_script_status_t;



#define G_MAX_SCRIPT_ACCUM_BUFFERS  8


struct gentity_s {
	entityState_t s;
	entityShared_t r;               // shared by both the server system and game

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
	//================================

	struct gclient_s    *client;            // NULL if not a client		0x15c

	qboolean inuse;

	byte unknown[0x110]; //0x164


/*
	char        *classname;         // set in QuakeEd
	int spawnflags;                 // set in QuakeEd

	qboolean neverFree;             // if true, FreeEntity will only unlink
									// bodyque uses this

	int flags;                      // FL_* variables

	char        *model;
	char        *model2;
	int freetime;                   // level.time when the object was freed

	int eventTime;                  // events will be cleared EVENT_VALID_MSEC after set
	qboolean freeAfterEvent;
	qboolean unlinkAfterEvent;

	qboolean physicsObject;         // if true, it can be pushed by movers and fall off edges
									// all game items are physicsObjects,
	float physicsBounce;            // 1.0 = continuous bounce, 0.0 = no bounce
	int clipmask;                   // brushes with this content value will be collided against
									// when moving.  items and corpses do not collide against
									// players, for instance

	// movers
	moverState_t moverState;
	int soundPos1;
	int sound1to2;
	int sound2to1;
	int soundPos2;
	int soundLoop;
	// JOSEPH 1-26-00
	int sound2to3;
	int sound3to2;
	int soundPos3;
	// END JOSEPH

	int soundKicked;
	int soundKickedEnd;

	int soundSoftopen;
	int soundSoftendo;
	int soundSoftclose;
	int soundSoftendc;

	gentity_t   *parent;
	gentity_t   *nextTrain;
	gentity_t   *prevTrain;
	// JOSEPH 1-26-00
	vec3_t pos1, pos2, pos3;
	// END JOSEPH

	char        *message;

	int timestamp;              // body queue sinking, etc   //0x1bc

	float angle;                // set in editor, -1 = up, -2 = down
	char        *target;
	char        *targetname;
	char        *team;
	char        *targetShaderName;
	char        *targetShaderNewName;
	gentity_t   *target_ent;

	float speed;
	float closespeed;           // for movers that close at a different speed than they open
	vec3_t movedir;

	int gDuration;
	int gDurationBack;
	vec3_t gDelta;
	vec3_t gDeltaBack;

	int nextthink;
	void ( *think )( gentity_t *self );
	void ( *reached )( gentity_t *self );       // movers call this when hitting endpoint
	void ( *blocked )( gentity_t *self, gentity_t *other );
	void ( *touch )( gentity_t *self, gentity_t *other, trace_t *trace );
	void ( *use )( gentity_t *self, gentity_t *other, gentity_t *activator );
	void ( *pain )( gentity_t *self, gentity_t *attacker, int damage, vec3_t point );
	void ( *die )( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod );

	int pain_debounce_time;
	int fly_sound_debounce_time;            // wind tunnel
	int last_move_time;

	int health;		//0x1A0 ??

	qboolean takedamage;	//0x16b

	int damage;
	int splashDamage;           // quad will increase this without increasing radius
	int splashRadius;
	int methodOfDeath;
	int splashMethodOfDeath;

	int count;

	gentity_t   *chain;
	gentity_t   *enemy;
	gentity_t   *activator;
	gentity_t   *teamchain;     // next entity in team
	gentity_t   *teammaster;    // master of the team

	int watertype;
	int waterlevel;

	int noise_index;

	// timing variables
	float wait;		//
	float random;		//

	// Rafael - sniper variable
	// sniper uses delay, random, radius
	int radius;
	float delay;

	// JOSEPH 10-11-99
	int TargetFlag;
	float duration;
	vec3_t rotate;
	vec3_t TargetAngles;
	// END JOSEPH

	gitem_t     *item;          // for bonus items

	// Ridah, AI fields
	char        *aiAttributes;
	char        *aiName;
	int aiTeam;
	void ( *AIScript_AlertEntity )( gentity_t *ent );
	qboolean aiInactive;
	int aiCharacter;            // the index of the type of character we are (from aicast_soldier.c)
	// done.

	char        *aiSkin;
	char        *aihSkin;

	vec3_t dl_color;
	char        *dl_stylestring;
	char        *dl_shader;
	int dl_atten;


	int key;                    // used by:  target_speaker->nopvs,

	qboolean active;	//0x16c
	qboolean botDelayBegin;

	// Rafael - mg42
	float harc;
	float varc;

	int props_frame_state;

	// Ridah
	int missionLevel;               // mission we are currently trying to complete
									// gets reset each new level
	// done.

	// Rafael
	qboolean is_dead;
	// done

	int start_size;
	int end_size;

	// Rafael props

	qboolean isProp;

	int mg42BaseEnt;

	gentity_t   *melee;

	char        *spawnitem;

	qboolean nopickup;

	int flameQuota, flameQuotaTime, flameBurnEnt;

	int count2;

	int grenadeExplodeTime;         // we've caught a grenade, which was due to explode at this time
	int grenadeFired;               // the grenade entity we last fired

	int mg42ClampTime;              // time to wait before an AI decides to ditch the mg42

	char        *track;

	// entity scripting system
	char                *scriptName;

	int numScriptEvents;
	g_script_event_t    *scriptEvents;  // contains a list of actions to perform for each event type
	g_script_status_t scriptStatus;     // current status of scripting
	// the accumulation buffer
	int scriptAccumBuffer[G_MAX_SCRIPT_ACCUM_BUFFERS];

	qboolean AASblocking;
	float accuracy;

	char        *tagName;       // name of the tag we are attached to
	gentity_t   *tagParent;

	float headshotDamageScale;

	int lastHintCheckTime;                  // DHM - Nerve
	// -------------------------------------------------------------------------------------------
	// if working on a post release patch, new variables should ONLY be inserted after this point
	// DHM - Nerve :: the above warning does not really apply to MP, but I'll follow it for good measure

	int voiceChatSquelch;                   // DHM - Nerve
	int voiceChatPreviousTime;              // DHM - Nerve
	int lastBurnedFrameNumber;              // JPW - Nerve   : to fix FT instant-kill exploit*/
};//Size: 0x274


//====================================================================

typedef enum {
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED
} clientConnected_t;

typedef enum {
	SPECTATOR_NOT,
	SPECTATOR_FREE,
	SPECTATOR_FOLLOW,
	SPECTATOR_SCOREBOARD
} spectatorState_t;

typedef enum {
	TEAM_BEGIN,     // Beginning a team game, spawn at base
	TEAM_ACTIVE     // Now actively playing
} playerTeamStateState_t;

typedef struct {
	playerTeamStateState_t state;

	int location;

	int captures;
	int basedefense;
	int carrierdefense;
	int flagrecovery;
	int fragcarrier;
	int assists;

	float lasthurtcarrier;
	float lastreturnedflag;
	float flagsince;
	float lastfraggedcarrier;
} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define FOLLOW_ACTIVE1  -1
#define FOLLOW_ACTIVE2  -2

// client data that stays across multiple levels or tournament restarts
// this is achieved by writing all the data to cvar strings at game shutdown
// time and reading them back at connection time.  Anything added here
// MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()

typedef struct {
	//Most is not active
	team_t sessionTeam;		//0x3010
	int spectatorTime;              // for determining next-in-line to play
	spectatorState_t spectatorState;
	int spectatorClient_Unknown;            // for chasecam and follow mode
	int wins, losses;               // tournament stats
	int playerType;                 // DHM - Nerve :: for GT_WOLF
	int playerWeapon;               // DHM - Nerve :: for GT_WOLF
//	int playerItem;                 // DHM - Nerve :: for GT_WOLF
	int playerTagIndex;		//0x3030
	int playerSkin;                 // DHM - Nerve :: for GT_WOLF
//	int spawnObjectiveIndex;         // JPW NERVE index of objective to spawn nearest to (returned from UI)
	int latchPlayerType;            // DHM - Nerve :: for GT_WOLF not archived
	int latchPlayerWeapon;          // DHM - Nerve :: for GT_WOLF not archived
	int latchPlayerItem;            // DHM - Nerve :: for GT_WOLF not archived
	int latchPlayerSkin;            // DHM - Nerve :: for GT_WOLF not archived
	char netname[MAX_NAME_LENGTH];	//0x3048
	int lastFollowedClient;
	int rank;			//0x305c
	int prestige;			//0x3060
	int perkIndex;			//0x3064
	int vehicleOwnerNum;		//0x3068  //Mybe vehicletype ?
	int vehicleRideSlot;		//0x306c
	int PSOffsetTime;		//0x3070 ???
	int spectatorClient;           //0x3074 for chasecam and follow mode
} clientSession_t;

#define MAX_NETNAME         16
#define MAX_VOTE_COUNT      3

#define PICKUP_ACTIVATE 0   // pickup items only when using "+activate"
#define PICKUP_TOUCH    1   // pickup items when touched
#define PICKUP_FORCE    2   // pickup the next item when touched (and reset to PICKUP_ACTIVATE when done)

/*
typedef enum {
	STATE_PLAYING,
	STATE_DEAD,
	STATE_SPECTATOR,
	STATE_INTERMISSION
} player_gamestate_t;
*/

// client data that stays across multiple respawns, but is cleared
// on each level change or team change at ClientBegin()
typedef struct {

        enum{	STATE_PLAYING, STATE_DEAD,
		STATE_SPECTATOR, STATE_INTERMISSION
	}playerState;//0x2f64

	int unknownStateVar;		//0x2f68
	int unknownStateVar2;		//0x2f6c
	int unknown[2];			//0x2f70

	clientScoreboard_t	scoreboard;	//0x2f78
	qboolean initialSpawn;          //0x2f88 the first spawn should be at a cool location
	clientConnected_t connected;	//0x2f8c maybe
	usercmd_t cmd;                  //0x2f90 we would lose angles if not persistant
	usercmd_t oldcmd;               //0x2fb0 previous command processed by pmove()
	qboolean localClient;           //0x2fd0 true if "ip" info key is "localhost"

	qboolean predictItemPickup;     //0x2fd4 based on cg_predictItems userinfo
	char netname[MAX_NETNAME];	//0x2fd8

	int maxHealth;                  // 0x2fe8 for handicapping
	int enterTime;                  // 0x2fec level.time the client entered the game
	int connectTime;                // DHM - Nerve :: level.time the client first connected to the server  // N/A
//	playerTeamState_t teamState;    // status in teamplay games
	int voteCount;                  // 0x2ff4 to prevent people from constantly calling votes
	int teamVoteCount;              // to prevent people from constantly calling votes		// N/A

	int moveSpeedScale;		// 0x2ffc

	int viewModel;			// 0x3000 //Model-index

	int clientCanSpectate;		// 0x3004
	int freeaddr1;			// 0x3008

	int clientState;		// 0x300c
} clientPersistant_t;



//	int ping;			// 0x3108 server to game info for scoreboard



typedef struct {
	vec3_t mins;
	vec3_t maxs;

	vec3_t origin;

	int time;
	int servertime;
} clientMarker_t;

#define MAX_CLIENT_MARKERS 10

#define LT_SPECIAL_PICKUP_MOD   3       // JPW NERVE # of times (minus one for modulo) LT must drop ammo before scoring a point
#define MEDIC_SPECIAL_PICKUP_MOD    4   // JPW NERVE same thing for medic




typedef struct {
void* dummy;
}animModelInfo_t; //Dummy


typedef struct {
	qboolean bAutoReload; // do we predict autoreload of weapons
	int blockCenterViewTime; // don't let centerview happen for a little while

	// Arnout: MG42 aiming
	float varc, harc;
	vec3_t centerangles;

} pmoveExt_t;   // data used both in client and server - store it here





// this structure is cleared on each ClientSpawn(),
// except for 'client->pers' and 'client->sess'
struct gclient_s {
	// ps MUST be the first element, because the server expects it
	playerState_t ps;               //0x00 communicated by server to clients

	// the rest of the structure is private to game

	clientPersistant_t pers;	//0x2f64
	clientSession_t sess;		//0x3010

	qboolean noclip;		//0x3078
	qboolean ufo;			//0x307c
	qboolean freezeControls;	//0x3080

	int lastCmdTime;                //0x3084 level.time of last usercmd_t, for EF_CONNECTION
									// we can't just use pers.lastCommand.time, because
									// of the g_sycronousclients case
	//Buttonlogic/exact addresses is not known but scope
	int buttons;			//0x3088
	int oldbuttons;
	int unk1;
	int latched_buttons;		//0x3094

	int wbuttons;
	int oldwbuttons;
	int latched_wbuttons;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int damage_armor;               //0x30a4 damage absorbed by armor
	int damage_blood;               // damage taken out of health
	int damage_knockback;           // impact damage
	vec3_t damage_from;             //0x30b0 origin for vector calculation
	qboolean damage_fromWorld;      //0x30bc if true, don't use the damage_from vector

	int accurateCount;              // for "impressive" reward sound	N/A

	int accuracy_shots;             // total number of shots		N/A
	int accuracy_hits;              // total number of hits			N/A

	//
//	int lastkilled_client;          // last client that this client killed
//	int lasthurt_client;            // last client that damaged this client
//	int lasthurt_mod;               // type of damage the client did

	// timers
//	int respawnTime;                // can respawn when time > this, force after g_forcerespwan
	int inactivityTime;             //0x30cc kick players when time > this
	qboolean inactivityWarning;     //0x30d0 qtrue if the five second warning has been given
	int playerTalkTime;		//0x30d4 ??
	int rewardTime;                 // clear the EF_AWARD_IMPRESSIVE, etc when time > this  N/A
        vec3_t unk;			//0x30dc


	int airOutTime;			//0x30e8 Unknown only reset

	int lastKillTime;               // ???for multiple kill rewards
	int dummy34;

	qboolean fireHeld;              // ???used for hook
	gentity_t   *hook;              //0x30f8 grapple hook if out

	int switchTeamTime;             //0x30fc time the player switched teams



	int IMtooLazy[33];
	//Not anymore resolved

	// timeResidual is used to handle events that happen every second
	// like health / armor countdowns and regeneration
/*	int timeResidual;

	float currentAimSpreadScale;

	int medicHealAmt;

	// RF, may be shared by multiple clients/characters
	animModelInfo_t *modelInfo;

	// -------------------------------------------------------------------------------------------
	// if working on a post release patch, new variables should ONLY be inserted after this point

	gentity_t   *persistantPowerup;
	int portalID;
	int ammoTimes[WP_NUM_WEAPONS];
	int invulnerabilityTime;

	gentity_t   *cameraPortal;              // grapple hook if out
	vec3_t cameraOrigin;

	int dropWeaponTime;         // JPW NERVE last time a weapon was dropped
	int limboDropWeapon;         // JPW NERVE weapon to drop in limbo
	int deployQueueNumber;         // JPW NERVE player order in reinforcement FIFO queue
	int sniperRifleFiredTime;         // JPW NERVE last time a sniper rifle was fired (for muzzle flip effects)
	float sniperRifleMuzzleYaw;       // JPW NERVE for time-dependent muzzle flip in multiplayer
	int lastBurnTime;         // JPW NERVE last time index for flamethrower burn
	int PCSpecialPickedUpCount;         // JPW NERVE used to count # of times somebody's picked up this LTs ammo (or medic health) (for scoring)
	int saved_persistant[MAX_PERSISTANT];           // DHM - Nerve :: Save ps->persistant here during Limbo

	// g_antilag.c
	int topMarker;
	clientMarker_t clientMarkers[MAX_CLIENT_MARKERS];
	clientMarker_t backupMarker;

	gentity_t       *tempHead;  // Gordon: storing a temporary head for bullet head shot detection

	pmoveExt_t pmext;*/
};//Size: 0x3184

// this structure is cleared as each map is entered
//
#define MAX_SPAWN_VARS          64
#define MAX_SPAWN_VARS_CHARS    2048

typedef struct {
	struct gclient_s    *clients;       // [maxclients]

	struct gentity_s    *gentities;




	int gentitySize;

	int num_entities;               // current number, <= MAX_GENTITIES

	struct gentity_s *firstFreeEnt;
	struct gentity_s *lastFreeEnt;
	void *logFile;
	int initializing;
	int clientIsSpawning;
	objective_t objectives[16];

	// store latched cvars here that we want to get at often
	int maxclients;				//0x1e4
	int framenum;
	int time;                           // in msec		0x1ec
	int previousTime;                   // 0x1f0 so movers can back up when blocked
	int frameTime;                      // Gordon: time the frame started, for antilag stuff

	int startTime;                      // level.time the map was started


	int teamScores[TEAM_NUM_TEAMS];		//0x1fc
	int lastTeamLocationTime;               // last time of client team location update

	qboolean bUpdateScoresForIntermission;		//???? Not known 0x210
	byte teamHasRadar[TEAM_NUM_TEAMS];
	int manualNameChange;			//0x218
	int numConnectedClients;              // connected, non-spectators
	int sortedClients[MAX_CLIENTS];		//sorted by rank or score ? 0x220



	// voting state
	char voteString[MAX_STRING_CHARS];		//0x320
	char voteDisplayString[MAX_STRING_CHARS];	//0x720
	int voteTime;                       // level.time vote was called	0xb20
	int voteExecuteTime;                // time the vote is executed
	int voteYes;				//0xb28
	int voteNo;				//0xb2c
	int numVotingClients;			// set by CalculateRanks
/*
	// spawn variables
	qboolean spawning;                  // the G_Spawn*() functions are valid
	int numSpawnVars;
	char        *spawnVars[MAX_SPAWN_VARS][2];  // key / value pairs
	int numSpawnVarChars;
	char spawnVarChars[MAX_SPAWN_VARS_CHARS];

	// intermission state
	int intermissionQueued;             // intermission was qualified, but
										// wait INTERMISSION_DELAY_TIME before
										// actually going there so the last
										// frag can be watched.  Disable future
										// kills during this delay
	int intermissiontime;               // time the intermission was started
	char        *changemap;
	qboolean readyToExit;               // at least one client wants to exit
	int exitTime;
	vec3_t intermission_origin;         // also used for spectator spawns
	vec3_t intermission_angle;

	qboolean locationLinked;            // target_locations get linked
	gentity_t   *locationHead;          // head of the location list
	int bodyQueIndex;                   // dead bodies
	gentity_t   *bodyQue[BODY_QUEUE_SIZE];

	int portalSequence;
	// Ridah
	char        *scriptAI;
	int reloadPauseTime;                // don't think AI/client's until this time has elapsed
	int reloadDelayTime;                // don't start loading the savegame until this has expired

	int lastGrenadeKick;

	int loperZapSound;
	int stimSoldierFlySound;
	int bulletRicochetSound;
	// done.

	int snipersound;

	//----(SA)	added
	int knifeSound[4];
	//----(SA)	end

// JPW NERVE
	int capturetimes[4];         // red, blue, none, spectator for WOLF_MP_CPH
	int redReinforceTime, blueReinforceTime;         // last time reinforcements arrived in ms
	int redNumWaiting, blueNumWaiting;         // number of reinforcements in queue
	vec3_t spawntargets[MAX_MULTI_SPAWNTARGETS];      // coordinates of spawn targets
	int numspawntargets;         // # spawntargets in this map
// jpw

	// RF, entity scripting
	char        *scriptEntity;

	// player/AI model scripting (server repository)
	animScriptData_t animScriptData;

	// NERVE - SMF - debugging/profiling info
	int totalHeadshots;
	int missedHeadshots;
	qboolean lastRestartTime;
	// -NERVE - SMF

	int numFinalDead[2];                // DHM - Nerve :: unable to respawn and in limbo (per team)
	int numOidTriggers;                 // DHM - Nerve

	qboolean latchGametype;             // DHM - Nerve*/
} level_locals_t;



typedef struct {
    int		levelLoadRetries;
    int		LevelExitFrame;
    qboolean	initServer;
} extlevel_locals_t;

extlevel_locals_t extlevel;


typedef struct {
    char	gametypename[64];
    char	gametypereadable[68];
} gametype_t;


typedef struct {
    int		var_01;
    int		var_02;
    int		var_03;
    int		numGametypes;
    gametype_t	gametype[];
} gametypes_t;




#define iDFLAGS_RADIUS					1			// explosive damage
#define iDFLAGS_NO_ARMOR				2			// ???
#define iDFLAGS_NO_KNOCKBACK			4			// players dont get pushed in damage_dir
#define iDFLAGS_PENETRATION				8			// bullets can penetrate walls
#define iDFLAGS_NO_TEAM_PROTECTION		16			// team kills/damage in TDM/SD etc
#define iDFLAGS_NO_PROTECTION			32			// nothing can stop damage
#define iDFLAGS_PASSTHRU				64			// bullet passed through non solid surface (???)






//
// config strings are a general means of communicating variable length strings
// from the server to all connected clients.
//

// CS_SERVERINFO and CS_SYSTEMINFO are defined in q_shared.h
/*

#define CS_MUSIC                2
#define CS_MESSAGE              3       // from the map worldspawn's message field
#define CS_MOTD                 4       // g_motd string for server message of the day
#define CS_WARMUP               5       // server time when the match will be restarted
#define CS_SCORES1              6
#define CS_SCORES2              7
*/

#define CS_VOTE_TIME            13
#define CS_VOTE_STRING          14
#define CS_VOTE_YES             15
#define CS_VOTE_NO              16
/*
#define CS_GAME_VERSION         12
#define CS_LEVEL_START_TIME     13      // so the timer only shows the current level
#define CS_INTERMISSION         14      // when 1, intermission will start in a second or two

*/
// DHM - Nerve :: Wolf Multiplayer information

// TTimo - voting config flags
#define VOTEFLAGS_RESTART           ( 1 << 0 )
#define VOTEFLAGS_GAMETYPE          ( 1 << 1 )
#define VOTEFLAGS_STARTMATCH        ( 1 << 2 )
#define VOTEFLAGS_NEXTMAP           ( 1 << 3 )
#define VOTEFLAGS_SWAP              ( 1 << 4 )
#define VOTEFLAGS_TYPE              ( 1 << 5 )
#define VOTEFLAGS_KICK              ( 1 << 6 )
#define VOTEFLAGS_MAP               ( 1 << 7 )
#define VOTEFLAGS_ANYMAP            ( 1 << 8 )
/*
// entityState_t->eFlags
#define EF_DEAD             0x00000001      // don't draw a foe marker over players with EF_DEAD
#define EF_NONSOLID_BMODEL  0x00000002      // bmodel is visible, but not solid
#define EF_TELEPORT_BIT     0x00000004      // toggled every time the origin abruptly changes
#define EF_MONSTER_EFFECT   0x00000008      // draw an aiChar dependant effect for this character
#define EF_CAPSULE          0x00000010      // use capsule for collisions
#define EF_CROUCHING        0x00000020      // player is crouching
#define EF_MG42_ACTIVE      0x00000040      // currently using an MG42
#define EF_NODRAW           0x00000080      // may have an event, but no model (unspawned items)
#define EF_FIRING           0x00000100      // for lightning gun
#define EF_INHERITSHADER    EF_FIRING       // some ents will never use EF_FIRING, hijack it for "USESHADER"
#define EF_BOUNCE_HEAVY     0x00000200      // more realistic bounce.  not as rubbery as above (currently for c4)
#define EF_SPINNING         0x00000400      // (SA) added for level editor control of spinning pickup items
#define EF_BREATH           EF_SPINNING     // Characters will not have EF_SPINNING set, hijack for drawing character breath

#define EF_MELEE_ACTIVE     0x00000800      // (SA) added for client knowledge of melee items held (chair/etc.)
#define EF_TALK             0x00001000      // draw a talk balloon
#define EF_SMOKING          EF_MONSTER_EFFECT3  // DHM - Nerve :: ET_GENERAL ents will emit smoke if set // JPW switched to this after my code change
#define EF_CONNECTION       0x00002000      // draw a connection trouble sprite
#define EF_MONSTER_EFFECT2  0x00004000      // show the secondary special effect for this character
#define EF_SMOKINGBLACK     EF_MONSTER_EFFECT2  // JPW NERVE -- like EF_SMOKING only darker & bigger
#define EF_HEADSHOT         0x00008000      // last hit to player was head shot
#define EF_MONSTER_EFFECT3  0x00010000      // show the third special effect for this character
#define EF_HEADLOOK         0x00020000      // make the head look around*/

#define EF_VOTED            0x00100000     // already cast a vote

/*

#define EF_STAND_IDLE2      0x00040000      // when standing, play idle2 instead of the default
#define EF_VIEWING_CAMERA   EF_STAND_IDLE2  // NOTE: REMOVE STAND_IDLE2 !!
#define EF_TAGCONNECT       0x00080000      // connected to another entity via tag
#define EF_MOVER_BLOCKED    0x00100000      // mover was blocked dont lerp on the client
#define EF_FORCED_ANGLES    0x00200000  // enforce all body parts to use these angles

#define EF_ZOOMING          0x00400000      // client is zooming
#define EF_NOSWINGANGLES    0x00800000      // try and keep all parts facing same direction


// !! NOTE: only place flags that don't need to go to the client beyond 0x00800000

#define EF_DUMMY_PMOVE      0x01000000
#define EF_BOUNCE           0x04000000      // for missiles
#define EF_BOUNCE_HALF      0x08000000      // for missiles
#define EF_MOVER_STOP       0x10000000      // will push otherwise	// (SA) moved down to make space for one more client flag

*/






/*
// --- COD4: raw\maps\mp\gametypes\_missions.gsc --- //

typedef enum
{
	MOD_UNKNOWN,
	MOD_PISTOL_BULLET,
	MOD_RIFLE_BULLET,
	MOD_GRENADE,
	MOD_GRENADE_SPLASH,
	MOD_PROJECTILE,
	MOD_PROJECTILE_SPLASH,
	MOD_MELEE,
	MOD_HEAD_SHOT,
	MOD_CRUSH,
	MOD_TELEFRAG,
	MOD_FALLING,
	MOD_SUICIDE,
	MOD_TRIGGER_HURT,
	MOD_EXPLOSIVE,
	MOD_IMPACT,

	MOD_BAD

} meansOfDeath_t;

#define MOD_NUM	16
char *modNames[MOD_NUM] =
{
	"MOD_UNKNOWN",
	"MOD_PISTOL_BULLET",
	"MOD_RIFLE_BULLET",
	"MOD_GRENADE",
	"MOD_GRENADE_SPLASH",
	"MOD_PROJECTILE",
	"MOD_PROJECTILE_SPLASH",
	"MOD_MELEE",
	"MOD_HEAD_SHOT",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TRIGGER_HURT",
	"MOD_EXPLOSIVE",
	"MOD_IMPACT",
};

static const char *g_HitLocNames[] =
{
	"none",
	"helmet",
	"head",
	"neck",
	"torso_upper",
	"torso_lower",
	"right_arm_upper",
	"left_arm_upper",
	"right_arm_lower",
	"left_arm_lower",
	"right_hand",
	"left_hand",
	"right_leg_upper",
	"left_leg_upper",
	"right_leg_lower",
	"left_leg_lower",
	"right_foot",
	"left_foot",
	"gun",
};

*/




/*
// --- COD4: raw\maps\mp\gametypes\_hud.gsc --- //

// Edge relative placement values for rect->h_align and rect->v_align
#define HORIZONTAL_ALIGN_SUBLEFT		0	// left edge of a 4:3 screen (safe area not included)
#define HORIZONTAL_ALIGN_LEFT			1	// left viewable (safe area) edge
#define HORIZONTAL_ALIGN_CENTER			2	// center of the screen (reticle)
#define HORIZONTAL_ALIGN_RIGHT			3	// right viewable (safe area) edge
#define HORIZONTAL_ALIGN_FULLSCREEN		4	// disregards safe area
#define HORIZONTAL_ALIGN_NOSCALE		5	// uses exact parameters - neither adjusts for safe area nor scales for screen size
#define HORIZONTAL_ALIGN_TO640			6	// scales a real-screen resolution x down into the 0 - 640 range
#define HORIZONTAL_ALIGN_CENTER_SAFEAREA 7	// center of the safearea
#define HORIZONTAL_ALIGN_MAX			HORIZONTAL_ALIGN_CENTER_SAFEAREA
#define HORIZONTAL_ALIGN_DEFAULT		HORIZONTAL_ALIGN_SUBLEFT

#define VERTICAL_ALIGN_SUBTOP			0	// top edge of the 4:3 screen (safe area not included)
#define VERTICAL_ALIGN_TOP				1	// top viewable (safe area) edge
#define VERTICAL_ALIGN_CENTER			2	// center of the screen (reticle)
#define VERTICAL_ALIGN_BOTTOM			3	// bottom viewable (safe area) edge
#define VERTICAL_ALIGN_FULLSCREEN		4	// disregards safe area
#define VERTICAL_ALIGN_NOSCALE			5	// uses exact parameters - neither adjusts for safe area nor scales for screen size
#define VERTICAL_ALIGN_TO480			6	// scales a real-screen resolution y down into the 0 - 480 range
#define VERTICAL_ALIGN_CENTER_SAFEAREA	7	// center of the save area
#define VERTICAL_ALIGN_MAX				VERTICAL_ALIGN_CENTER_SAFEAREA
#define VERTICAL_ALIGN_DEFAULT			VERTICAL_ALIGN_SUBTOP

static const char *g_he_font[] =
{
	"default",		// HE_FONT_DEFAULT
	"bigfixed",		// HE_FONT_BIGFIXED
	"smallfixed",	// HE_FONT_SMALLFIXED
	"objective",	// HE_FONT_OBJECTIVE
};


// These values correspond to the defines in q_shared.h
static const char *g_he_alignx[] =
{
	"left",   // HE_ALIGN_LEFT
	"center", // HE_ALIGN_CENTER
	"right",  // HE_ALIGN_RIGHT
};


static const char *g_he_aligny[] =
{
	"top",    // HE_ALIGN_TOP
	"middle", // HE_ALIGN_MIDDLE
	"bottom", // HE_ALIGN_BOTTOM
};


// These values correspond to the defines in menudefinition.h
static const char *g_he_horzalign[] =
{
	"subleft",			// HORIZONTAL_ALIGN_SUBLEFT
	"left",				// HORIZONTAL_ALIGN_LEFT
	"center",			// HORIZONTAL_ALIGN_CENTER
	"right",			// HORIZONTAL_ALIGN_RIGHT
	"fullscreen",		// HORIZONTAL_ALIGN_FULLSCREEN
	"noscale",			// HORIZONTAL_ALIGN_NOSCALE
	"alignto640",		// HORIZONTAL_ALIGN_TO640
	"center_safearea",	// HORIZONTAL_ALIGN_CENTER_SAFEAREA
};
//cassert( ARRAY_COUNT( g_he_horzalign ) == HORIZONTAL_ALIGN_MAX + 1 );


static const char *g_he_vertalign[] =
{
	"subtop",			// VERTICAL_ALIGN_SUBTOP
	"top",				// VERTICAL_ALIGN_TOP
	"middle",			// VERTICAL_ALIGN_CENTER
	"bottom",			// VERTICAL_ALIGN_BOTTOM
	"fullscreen",		// VERTICAL_ALIGN_FULLSCREEN
	"noscale",			// VERTICAL_ALIGN_NOSCALE
	"alignto480",		// VERTICAL_ALIGN_TO480
	"center_safearea",	// VERTICAL_ALIGN_CENTER_SAFEAREA
};
//cassert( ARRAY_COUNT( g_he_vertalign ) == VERTICAL_ALIGN_MAX + 1 );

*/

qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 );


#endif
