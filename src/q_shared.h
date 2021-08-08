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
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
#ifndef __Q_SHARED_H
  #define __Q_SHARED_H

  #include "q_platform.h"

// q_shared.h -- included first by ALL program modules.
// A user mod should never modify this file

  #include <math.h>
  #include <stdio.h>
  #include <stdlib.h>


  #define PRODUCT_NAME				"CoD4 MP"
  #define BASEGAME					"main"
  #define CLIENT_WINDOW_TITLE     	"Call of Duty 4 - Modern Warfare"
  #define LONG_PRODUCT_NAME			"Call of Duty 4 - Modern Warfare"
  #define CLIENT_WINDOW_MIN_TITLE 	"CoD4 MP"
  #define GAMENAME_FOR_MASTER		"CoD4"		// must NOT contain whitespaces
  #define HEARTBEAT_FOR_MASTER		GAMENAME_FOR_MASTER
  #define FLATLINE_FOR_MASTER		GAMENAME_FOR_MASTER "dead"
  #define WINDOW_CLASS_NAME			"CoD4" //Do not change
  #define PRODUCT_VERSION 			"1.8"
  #define UPDATE_VERSION_NUM		"20.3"
  #define DEMO_PROTOCOL_VERSION		1
  #define STEAM_APPID				"7940"
  #define STEAM_GAMEID				"7940"
  #define Q3_VERSION PRODUCT_NAME " " UPDATE_VERSION_NUM

/*
==============================================================

PROTOCOL

==============================================================
*/

#define	LEGACY_PROTOCOL_VERSION	6
#define	PROTOCOL_VERSION (unsigned int)(atof(UPDATE_VERSION) + 0.00001)
// 1.31 - 67

#define MAX_UPDATE_SERVERS  5

// override on command line, config files etc.
#ifndef ATVIMASTER_SERVER_NAME
#define ATVIMASTER_SERVER_NAME	"cod4master.activision.com"
#endif

#ifndef AUTHORIZE_SERVER_NAME
  #define	AUTHORIZE_SERVER_NAME	"cod4master.activision.com"
#endif
#ifndef PORT_AUTHORIZE
  #define	PORT_AUTHORIZE		20800
#endif


#define	PORT_MASTER			20810
#define	PORT_UPDATE			27951
#define	PORT_SERVER			28960
#define	NUM_SERVER_PORTS	4		// broadcast scan this many ports after
									// PORT_SERVER so a single machine can
									// run multiple servers


#define MAX_TEAMNAME			32
#define MAX_MASTER_SERVERS      5	// number of supported master servers

#define DEMOEXT	"dm_"			// standard demo extension

#ifdef _MSC_VER
/*
#pragma warning(disable : 4018)     // signed/unsigned mismatch
#pragma warning(disable : 4032)
#pragma warning(disable : 4051)
#pragma warning(disable : 4057)		// slightly different base types
#pragma warning(disable : 4100)		// unreferenced formal parameter
#pragma warning(disable : 4115)
#pragma warning(disable : 4125)		// decimal digit terminates octal escape sequence
#pragma warning(disable : 4127)		// conditional expression is constant
#pragma warning(disable : 4136)
#pragma warning(disable : 4152)		// nonstandard extension, function/data pointer conversion in expression
//#pragma warning(disable : 4201)
//#pragma warning(disable : 4214)
#pragma warning(disable : 4244)
#pragma warning(disable : 4142)		// benign redefinition
//#pragma warning(disable : 4305)		// truncation from const double to float
//#pragma warning(disable : 4310)		// cast truncates constant value
//#pragma warning(disable:  4505) 	// unreferenced local function has been removed
#pragma warning(disable : 4514)
#pragma warning(disable : 4702)		// unreachable code
#pragma warning(disable : 4711)		// selected for automatic inline expansion
#pragma warning(disable : 4220)		// varargs matches remaining parameters
//#pragma intrinsic( memset, memcpy )*/
#endif

//Ignore __attribute__ on non-gcc platforms

#ifndef __GNUC__
 #ifndef __attribute__
  #define __attribute__(x)
 #endif
#endif

#if (defined _MSC_VER)
#define Q_EXPORT __declspec(dllexport)
#elif (defined __SUNPRO_C)
#define Q_EXPORT __global
#elif ((__GNUC__ >= 3) && (!__EMX__) && (!sun))
#define Q_EXPORT __attribute__((visibility("default")))
#else
#define Q_EXPORT
#endif



/*
=====================================================================

 Macros for error reporting/handling
 Code used from: http://c.learncodethehardway.org/book/ex44.html
=====================================================================
*/


#ifndef DEBUG
#define zed_debug(M, ...)
#else
#define zed_debug(M, ...) Com_DPrintf(CON_CHANNEL_SYSTEM, "%s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) Com_PrintError(CON_CHANNEL_ERROR, "(%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) Com_PrintWarning(CON_CHANNEL_SYSTEM,"(%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) Com_DPrintf(CON_CHANNEL_SYSTEM, "(%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) if(!(A)) { zed_debug(M, ##__VA_ARGS__); errno=0; goto error; }


/**********************************************************************
  VM Considerations

  The VM can not use the standard system headers because we aren't really
  using the compiler they were meant for.  We use bg_lib.h which contains
  prototypes for the functions we define for our own use in bg_lib.c.

  When writing mods, please add needed headers HERE, do not start including
  stuff like <stdio.h> in the various .c files that make up each of the VMs
  since you will be including system headers files can will have issues.

  Remember, if you use a C library function that is not defined in bg_lib.c,
  you will have to add your own version for support in the VM.

 **********************************************************************/


#ifdef _MSC_VER
 // #include <io.h>

  typedef __int64 int64_t;
  typedef __int32 int32_t;
  typedef __int16 int16_t;
  typedef __int8 int8_t;
  typedef unsigned __int64 uint64_t;
  typedef unsigned __int32 uint32_t;
  typedef unsigned __int16 uint16_t;
  typedef unsigned __int8 uint8_t;

#else
  #include <stdint.h>
#endif

#ifndef _WIN32
	#define Q_vsnprintf vsnprintf
#else
  	// vsnprintf is ISO/IEC 9899:1999
  	// abstracting this to make it portable
  	int Q_vsnprintf(char *str, size_t size, const char *format, va_list ap);	
#endif

#include "q_platform.h"

//=============================================================

typedef unsigned char 		byte;

typedef enum {qfalse, qtrue}	qboolean;

typedef union {
	float f;
	int i;
	unsigned int ui;
} floatint_t;

typedef int		qhandle_t;
typedef int		sfxHandle_t;
typedef int		fileHandle_t;
typedef int		clipHandle_t;

#define PAD(x,y) (((x)+(y)-1) & ~((y)-1))

#ifdef __GNUC__
#define QALIGN(x) __attribute__((aligned(x)))
#else
#define QALIGN(x)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef __stdcall
#define __stdcall __attribute__((stdcall))
#endif

#ifndef QDECL
#define QDECL
#endif

#ifndef __fastcall
#define __fastcall __attribute__((fastcall))
#endif

#ifndef __regparm1
#define __regparm1 __attribute__((regparm(1)))
#endif

#ifndef __regparm2
#define __regparm2 __attribute__((regparm(2)))
#endif

#ifndef __regparm3
#define __regparm3 __attribute__((regparm(3)))
#endif

#define REGPARM(X)   __attribute__ ((regparm( X )))

#define STRING(s)			#s
// expand constants before stringifying them
#define XSTRING(s)			STRING(s)

#define	MAX_QINT			0x7fffffff
#define	MIN_QINT			(-MAX_QINT-1)

#define ARRAY_LEN(x)			(sizeof(x) / sizeof(*(x)))


// angle indexes
#define	PITCH				0		// up / down
#define	YAW					1		// left / right
#define	ROLL				2		// fall over

// the game guarantees that no string from the network will ever
// exceed MAX_STRING_CHARS
#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_TOKENS	1024	// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		1024	// max length of an individual token

#define	MAX_INFO_STRING		1024
#define	MAX_INFO_KEY		  1024
#define	MAX_INFO_VALUE		1024

#define	BIG_INFO_STRING		8192  // used for system info key only
#define	BIG_INFO_KEY		  8192
#define	BIG_INFO_VALUE		8192


#define	MAX_QPATH			64		// max length of a quake game pathname
/*
#ifdef PATH_MAX
#define MAX_OSPATH			PATH_MAX    //Nonono always 256 because its hardcoded in iw3mp.exe
#else
*/
#define	MAX_OSPATH			256		// max length of a filesystem pathname


#define	MAX_NAME_LENGTH		16		// max length of a client name

#define	MAX_SAY_TEXT	150

// paramters for command buffer stuffing
typedef enum {
	EXEC_NOW,			// don't return until completed, a VM should NEVER use this,
						// because some commands might cause the VM to be unloaded...
	EXEC_INSERT,		// insert at current position, but don't run yet
	EXEC_APPEND			// add to end of the command buffer (normal case)
} cbufExec_t;


//
// these aren't needed by any of the VMs.  put in another header?
//
#define	MAX_MAP_AREA_BYTES		32		// bit vector of area visibility


// print levels from renderer (FIXME: set up for game / cgame?)
typedef enum {
	PRINT_ALL,
	PRINT_DEVELOPER,		// only print when "developer 1"
	PRINT_WARNING,
	PRINT_ERROR
} printParm_t;


typedef struct
{
    byte red;
    byte green;
    byte blue;
    byte alpha;
}ucolor_t;



// parameters to the main Error routine
typedef enum {
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_SERVERDISCONNECT,		// don't kill server
	ERR_DISCONNECT,				// client disconnected from the server
	ERR_SCRIPT,					// script error occured
	ERR_SCRIPT_DROP,
	ERR_LOCALIZATION,
	ERR_MAPLOADERRORSUMMARY
} errorParm_t;

typedef enum
{
  CON_CHANNEL_DONT_FILTER = 0x0,
  CON_CHANNEL_ERROR = 0x1,
  CON_CHANNEL_GAMENOTIFY = 0x2,
  CON_CHANNEL_BOLDGAME = 0x3,
  CON_CHANNEL_SUBTITLE = 0x4,
  CON_CHANNEL_OBITUARY = 0x5,
  CON_CHANNEL_LOGFILEONLY = 0x6,
  CON_CHANNEL_CONSOLEONLY = 0x7,
  CON_CHANNEL_GFX = 0x8,
  CON_CHANNEL_SOUND = 0x9,
  CON_CHANNEL_FILES = 0xA,
  CON_CHANNEL_DEVGUI = 0xB,
  CON_CHANNEL_PROFILE = 0xC,
  CON_CHANNEL_UI = 0xD,
  CON_CHANNEL_CLIENT = 0xE,
  CON_CHANNEL_SERVER = 0xF,
  CON_CHANNEL_SYSTEM = 0x10,
  CON_CHANNEL_PLAYERWEAP = 0x11,
  CON_CHANNEL_AI = 0x12,
  CON_CHANNEL_ANIM = 0x13,
  CON_CHANNEL_PHYS = 0x14,
  CON_CHANNEL_FX = 0x15,
  CON_CHANNEL_LEADERBOARDS = 0x16,
  CON_CHANNEL_LIVE = 0x17,
  CON_CHANNEL_PARSERSCRIPT = 0x18,
  CON_CHANNEL_SCRIPT = 0x19,
  CON_CHANNEL_SPAWNSYSTEM = 0x1A,
  CON_CHANNEL_COOPINFO = 0x1B,
  CON_CHANNEL_SERVERDEMO = 0x1C,
  CON_CHANNEL_DDL = 0x1D,
  CON_CHANNEL_NETWORK = 0x1E,
  CON_CHANNEL_SCHEDULER = 0x1F,
  CON_FIRST_DEBUG_CHANNEL = 0x1F,
  CON_CHANNEL_TASK = 0x20,
  CON_CHANNEL_SPU = 0x21,
  CON_CHANNEL_FILEDL = 0x22,
  CON_BUILTIN_CHANNEL_COUNT = 0x23
}conChannel_t;


// font rendering values used by ui and cgame

#define PROP_GAP_WIDTH			3
#define PROP_SPACE_WIDTH		8
#define PROP_HEIGHT				27
#define PROP_SMALL_SIZE_SCALE	0.75

#define BLINK_DIVISOR			200
#define PULSE_DIVISOR			75

#define UI_LEFT			0x00000000	// default
#define UI_CENTER		0x00000001
#define UI_RIGHT		0x00000002
#define UI_FORMATMASK	0x00000007
#define UI_SMALLFONT	0x00000010
#define UI_BIGFONT		0x00000020	// default
#define UI_GIANTFONT	0x00000040
#define UI_DROPSHADOW	0x00000800
#define UI_BLINK		0x00001000
#define UI_INVERSE		0x00002000
#define UI_PULSE		0x00004000

#if defined(_DEBUG) && !defined(BSPC)
	#define HUNK_DEBUG
#endif

typedef enum {
	h_high,
	h_low,
	h_dontcare
} ha_pref;

#ifdef HUNK_DEBUG
#define Hunk_Alloc( size, preference )				Hunk_AllocDebug(size, preference, #size, __FILE__, __LINE__)
	void *Hunk_AllocDebug( int size, ha_pref preference, char *label, char *file, int line );
#else
	void *Hunk_Alloc( int size, ha_pref preference );
#endif

#define Com_Memset memset
#define Com_Memcpy memcpy

#define CIN_system	1
#define CIN_loop	2
#define	CIN_hold	4
#define CIN_silent	8
#define CIN_shader	16

/*
==============================================================

MATHLIB

==============================================================
*/

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef	int	fixed4_t;
typedef	int	fixed8_t;
typedef	int	fixed16_t;

#ifndef M_PI
#define M_PI		3.14159265358979323846f	// matches value in gcc v2 math.h
#endif

#define NUMVERTEXNORMALS	162
/*extern	vec3_t	bytedirs[NUMVERTEXNORMALS];
*/
// all drawing is done to a 640*480 virtual screen size
// and will be automatically scaled to the real resolution
#define	SCREEN_WIDTH		640
#define	SCREEN_HEIGHT		480

#define TINYCHAR_WIDTH		(SMALLCHAR_WIDTH)
#define TINYCHAR_HEIGHT		(SMALLCHAR_HEIGHT/2)

#define SMALLCHAR_WIDTH		8
#define SMALLCHAR_HEIGHT	16

#define BIGCHAR_WIDTH		16
#define BIGCHAR_HEIGHT		16

#define	GIANTCHAR_WIDTH		32
#define	GIANTCHAR_HEIGHT	48

extern	vec4_t		colorBlack;
extern	vec4_t		colorRed;
extern	vec4_t		colorGreen;
extern	vec4_t		colorBlue;
extern	vec4_t		colorYellow;
extern	vec4_t		colorMagenta;
extern	vec4_t		colorCyan;
extern	vec4_t		colorWhite;
extern	vec4_t		colorLtGrey;
extern	vec4_t		colorMdGrey;
extern	vec4_t		colorDkGrey;
extern  vec4_t		colorWhiteFaded;


#define Q_COLOR_ESCAPE	'^'
#define Q_IsColorString(p)	((p) && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) >= '0' && *((p)+1) <= '@') // ^[0-@]

#define COLOR_BLACK	'0'
#define COLOR_RED	'1'
#define COLOR_GREEN	'2'
#define COLOR_YELLOW	'3'
#define COLOR_BLUE	'4'
#define COLOR_CYAN	'5'
#define COLOR_MAGENTA	'6'
#define COLOR_WHITE	'7'


#define S_COLOR_BLACK	"^0"
#define S_COLOR_RED	"^1"
#define S_COLOR_GREEN	"^2"
#define S_COLOR_YELLOW	"^3"
#define S_COLOR_BLUE	"^4"
#define S_COLOR_CYAN	"^5"
#define S_COLOR_MAGENTA	"^6"
#define S_COLOR_WHITE	"^7"
/*
extern vec4_t	g_color_table[8];
*/
#define	MAKERGB( v, r, g, b ) v[0]=r;v[1]=g;v[2]=b
#define	MAKERGBA( v, r, g, b, a ) v[0]=r;v[1]=g;v[2]=b;v[3]=a

#define DEG2RAD( a ) ( ( (a) * M_PI ) / 180.0F )
#define RAD2DEG( a ) ( ( (a) * 180.0f ) / M_PI )

struct cplane_s;

extern	vec3_t	vec3_origin;
extern	vec3_t	axisDefault[3];

#define	nanmask (255<<23)

#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)

#if idppc

static ID_INLINE float Q_rsqrt( float number ) {
		float x = 0.5f * number;
                float y;
#ifdef __GNUC__
                asm("frsqrte %0,%1" : "=f" (y) : "f" (number));
#else
		y = __frsqrte( number );
#endif
		return y * (1.5f - (x * y * y));
	}

#ifdef __GNUC__
static ID_INLINE float Q_fabs(float x) {
    float abs_x;

    asm("fabs %0,%1" : "=f" (abs_x) : "f" (x));
    return abs_x;
}
#else
#define Q_fabs __fabsf
#endif

#else
float Q_fabs( float f );
float Q_rsqrt( float f );		// reciprocal square root
#endif

#define SQRTFAST( x ) ( (x) * Q_rsqrt( x ) )

signed char ClampChar( int i );
signed short ClampShort( int i );

// this isn't a real cheap function to call!
int DirToByte( vec3_t dir );
void ByteToDir( int b, vec3_t dir );


#if	1

#define DotProduct(x,y)			((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))

#else

#define DotProduct(x,y)			_DotProduct(x,y)
#define VectorSubtract(a,b,c)	_VectorSubtract(a,b,c)
#define VectorAdd(a,b,c)		_VectorAdd(a,b,c)
#define VectorCopy(a,b)			_VectorCopy(a,b)
#define	VectorScale(v, s, o)	_VectorScale(v,s,o)
#define	VectorMA(v, s, b, o)	_VectorMA(v,s,b,o)

#endif








#ifdef Q3_VM
#ifdef VectorCopy
#undef VectorCopy
// this is a little hack to get more efficient copies in our interpreter
typedef struct {
	float	v[3];
} vec3struct_t;
#define VectorCopy(a,b)	(*(vec3struct_t *)b=*(vec3struct_t *)a)
#endif
#endif

#define VectorClear(a)			((a)[0]=(a)[1]=(a)[2]=0)
#define VectorNegate(a,b)		((b)[0]=-(a)[0],(b)[1]=-(a)[1],(b)[2]=-(a)[2])
#define VectorSet(v, x, y, z)	((v)[0]=(x), (v)[1]=(y), (v)[2]=(z))
#define Vector4Copy(a,b)		((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3])

#define	SnapVector(v) {v[0]=((int)(v[0]));v[1]=((int)(v[1]));v[2]=((int)(v[2]));}
// just in case you do't want to use the macros
vec_t _DotProduct( const vec3_t v1, const vec3_t v2 );
void _VectorSubtract( const vec3_t veca, const vec3_t vecb, vec3_t out );
void _VectorAdd( const vec3_t veca, const vec3_t vecb, vec3_t out );
void _VectorCopy( const vec3_t in, vec3_t out );
void _VectorScale( const vec3_t in, float scale, vec3_t out );
void _VectorMA( const vec3_t veca, float scale, const vec3_t vecb, vec3_t vecc );

unsigned ColorBytes3 (float r, float g, float b);
unsigned ColorBytes4 (float r, float g, float b, float a);

float NormalizeColor( const vec3_t in, vec3_t out );

float RadiusFromBounds( const vec3_t mins, const vec3_t maxs );
void ClearBounds( vec3_t mins, vec3_t maxs );
void AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs );

#if !defined( Q3_VM ) || ( defined( Q3_VM ) && defined( __Q3_VM_MATH ) )
static ID_INLINE int VectorCompare( const vec3_t v1, const vec3_t v2 ) {
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2]) {
		return 0;
	}
	return 1;
}

static ID_INLINE vec_t VectorLength( const vec3_t v ) {
	return (vec_t)sqrt (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

static ID_INLINE vec_t VectorLengthSquared( const vec3_t v ) {
	return (v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

static ID_INLINE vec_t Distance( const vec3_t p1, const vec3_t p2 ) {
	vec3_t	v;

	VectorSubtract (p2, p1, v);
	return VectorLength( v );
}

static ID_INLINE vec_t DistanceSquared( const vec3_t p1, const vec3_t p2 ) {
	vec3_t	v;

	VectorSubtract (p2, p1, v);
	return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

// fast vector normalize routine that does not check to make sure
// that length != 0, nor does it return length, uses rsqrt approximation
static ID_INLINE void VectorNormalizeFast( vec3_t v )
{
	float ilength;

	ilength = Q_rsqrt( DotProduct( v, v ) );

	v[0] *= ilength;
	v[1] *= ilength;
	v[2] *= ilength;
}

static ID_INLINE void VectorInverse( vec3_t v ){
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

static ID_INLINE void CrossProduct( const vec3_t v1, const vec3_t v2, vec3_t cross ) {
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

#else
int VectorCompare( const vec3_t v1, const vec3_t v2 );

vec_t VectorLength( const vec3_t v );

vec_t VectorLengthSquared( const vec3_t v );

vec_t Distance( const vec3_t p1, const vec3_t p2 );

vec_t DistanceSquared( const vec3_t p1, const vec3_t p2 );

void VectorNormalizeFast( vec3_t v );

void VectorInverse( vec3_t v );

void CrossProduct( const vec3_t v1, const vec3_t v2, vec3_t cross );

#endif

vec_t VectorNormalize (vec3_t v);		// returns vector length
vec_t VectorNormalize2( const vec3_t v, vec3_t out );
void Vector4Scale( const vec4_t in, vec_t scale, vec4_t out );
void VectorRotate( vec3_t in, vec3_t matrix[3], vec3_t out );
int Q_log2(int val);

float Q_acos(float c);

int		Q_rand( int *seed );
float	Q_random( int *seed );
float	Q_crandom( int *seed );

#define random()	((rand () & 0x7fff) / ((float)0x7fff))
#define crandom()	(2.0 * (random() - 0.5))

void vectoangles( const vec3_t value1, vec3_t angles);
void AxisToAngles( vec3_t axis[3], vec3_t angles );
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );
void UnitQuatToAxis(const float *quat, float (*axis)[3]);

void AxisClear( vec3_t axis[3] );
void AxisCopy( vec3_t in[3], vec3_t out[3] );

void SetPlaneSignbits( struct cplane_s *out );
int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *plane);

qboolean BoundsIntersect(const vec3_t mins, const vec3_t maxs,
		const vec3_t mins2, const vec3_t maxs2);
qboolean BoundsIntersectSphere(const vec3_t mins, const vec3_t maxs,
		const vec3_t origin, vec_t radius);
qboolean BoundsIntersectPoint(const vec3_t mins, const vec3_t maxs,
		const vec3_t origin);

float	AngleMod(float a);
float	LerpAngle (float from, float to, float frac);
float	AngleSubtract( float a1, float a2 );
void	AnglesSubtract( vec3_t v1, vec3_t v2, vec3_t v3 );

float AngleNormalize360 ( float angle );
float AngleNormalize180 ( float angle );
float AngleDelta ( float angle1, float angle2 );

qboolean PlaneFromPoints( vec4_t plane, const vec3_t a, const vec3_t b, const vec3_t c );
void ProjectPointOnPlane( vec3_t dst, const vec3_t p, const vec3_t normal );
void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees );
void RotateAroundDirection( vec3_t axis[3], float yaw );
void MakeNormalVectors( const vec3_t forward, vec3_t right, vec3_t up );
// perpendicular vector could be replaced by this

//int	PlaneTypeForNormal (vec3_t normal);

void MatrixMultiply(float in1[3][3], float in2[3][3], float out[3][3]);
void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
void PerpendicularVector( vec3_t dst, const vec3_t src );
int Q_isnan( float x );


//=============================================

float Com_Clamp( float min, float max, float value );

char	*COM_SkipPath( char *pathname );
const char	*COM_GetExtension( const char *name );
void	COM_StripExtension(const char *in, char *out, int destsize);
void	COM_DefaultExtension( char *path, int maxSize, const char *extension );

void	COM_BeginParseSession( const char *name );
int		COM_GetCurrentParseLine( void );
char	*COM_Parse( char **data_p );
char	*COM_ParseExt( char **data_p, qboolean allowLineBreak );
int		COM_Compress( char *data_p );
void	COM_ParseError( char *format, ... ) __attribute__ ((format (printf, 1, 2)));
void	COM_ParseWarning( char *format, ... ) __attribute__ ((format (printf, 1, 2)));
//int		COM_ParseInfos( char *buf, int max, char infos[][MAX_INFO_STRING] );
const char *__cdecl Com_GetExtensionSubString(const char *filename);

#define MAX_TOKENLENGTH		1024

#ifndef TT_STRING
//token types
#define TT_STRING					1			// string
#define TT_LITERAL					2			// literal
#define TT_NUMBER					3			// number
#define TT_NAME						4			// name
#define TT_PUNCTUATION				5			// punctuation
#endif

typedef struct pc_token_s
{
	int type;
	int subtype;
	int intvalue;
	float floatvalue;
	char string[MAX_TOKENLENGTH];
} pc_token_t;


// data is an in/out parm, returns a parsed out token

void	COM_MatchToken( char**buf_p, char *match );

void SkipBracedSection (char **program);
void SkipRestOfLine ( char **data );

void Parse1DMatrix (char **buf_p, int x, float *m);
void Parse2DMatrix (char **buf_p, int y, int x, float *m);
void Parse3DMatrix (char **buf_p, int z, int y, int x, float *m);
int Com_HexStrToInt( const char *str );

int	QDECL Com_sprintf (char *dest, int size, const char *fmt, ...);
// __attribute__ ((format (printf, 3, 4)));

char *Com_SkipTokens( char *s, int numTokens, char *sep );
char *Com_SkipCharset( char *s, char *sep );

void Com_RandomBytes( byte *string, int len );

// mode parm for FS_FOpenFile
typedef enum {
	FS_READ,
	FS_WRITE,
	FS_APPEND,
	FS_APPEND_SYNC
} fsMode_t;

typedef enum {
	FS_SEEK_CUR,
	FS_SEEK_END,
	FS_SEEK_SET
} fsOrigin_t;

//=============================================

int Q_isprint( int c );
int Q_islower( int c );
int Q_isupper( int c );
int Q_isalpha( int c );
qboolean Q_isanumber( const char *s );
qboolean Q_isintegral( float f );

// portable case insensitive compare
int		Q_stricmp (const char *s1, const char *s2);
int		Q_strncmp (const char *s1, const char *s2, int n);
int		Q_stricmpn (const char *s1, const char *s2, int n);
char	*Q_strlwr( char *s1 );
char	*Q_strupr( char *s1 );
char	*Q_strrchr( const char* string, int c );
const char	*Q_stristr( const char *s, const char *find);

// buffer size safe library replacements
void	Q_strncpyz( char *dest, const char *src, int destsize );
void	Q_strcat( char *dest, int size, const char *src );

// strlen that discounts Quake color sequences
int Q_PrintStrlen( const char *string );
// removes color sequences from string
char *Q_CleanStr( char *string );
// Count the number of char tocount encountered in string
int Q_CountChar(const char *string, char tocount);
unsigned char I_CleanChar(unsigned char in);
qboolean isInteger(const char* string, int size);
void Q_bstrcpy(char* dest, char* src);


//=============================================

// 64-bit integers for global rankings interface
// implemented as a struct for qvm compatibility
typedef struct
{
	byte	b0;
	byte	b1;
	byte	b2;
	byte	b3;
	byte	b4;
	byte	b5;
	byte	b6;
	byte	b7;
} qint64;

//=============================================
/*
short	BigShort(short l);
short	LittleShort(short l);
int		BigLong (int l);
int		LittleLong (int l);
qint64  BigLong64 (qint64 l);
qint64  LittleLong64 (qint64 l);
float	BigFloat (const float *l);
float	LittleFloat (const float *l);

void	Swap_Init (void);
*/
char	* QDECL va(char *format, ...) __attribute__ ((format (printf, 1, 2)));

#define TRUNCATE_LENGTH	64
void Com_TruncateLongString( char *buffer, const char *s );

//=============================================

//
// key / value info strings
//
char *Info_ValueForKey( const char *s, const char *key );
char *Info_ValueForKey_tsInternal( const char *s, const char *key, char* buffer);
char *BigInfo_ValueForKey_tsInternal( const char *s, const char *key, char* buffer);
void Info_RemoveKey( char *s, const char *key );
void Info_RemoveKey_big( char *s, const char *key );
void Info_SetValueForKey( char *s, const char *key, const char *value );
void Info_SetValueForKey_Big( char *s, const char *key, const char *value );
qboolean Info_Validate( const char *s );
void Info_NextPair( const char **s, char *key, char *value );
void FastInfo_SetValueForKey( char *s, const char *key, const char *value );
/*
==========================================================

CVARS (console variables)

Many variables can be used for cheating purposes, so when
cheats is zero, force all unspecified variables to their
default values.
==========================================================
*/
// nothing outside the Cvar_*() functions should modify these fields!
#pragma pack(push, 1)

typedef enum{
    CVAR_BOOL,
    CVAR_FLOAT,
    CVAR_VEC2,
    CVAR_VEC3,
    CVAR_VEC4,
    CVAR_INT,
    CVAR_ENUM,
    CVAR_STRING,
    CVAR_COLOR
}cvarType_t;



typedef union
{
		float floatval;
		int integer;
		const char* string;
		byte boolean;
		vec4_t vec4;
		vec3_t vec3;
		vec2_t vec2;
		ucolor_t color;
}CvarValue_t;


typedef struct{
	union{
		int imin;
		float fmin;
		int enumCount;
	};
	union{
		int imax;
		float fmax;
		const char** enumStrings;
	};
}CvarLimits_t;



typedef struct cvar_s {
	const char *name;
	const char *description;
	short int flags;
	byte type;
	byte modified;
	union{
		float floatval;
		float value;
		int integer;
		const char* string;
		byte boolean;
		vec2_t vec2;
		vec3_t vec3;
		vec4_t vec4;
		ucolor_t color;
	};
	union{
		float latchedFloatval;
		int latchedInteger;
		const char* latchedString;
		byte latchedBoolean;
		vec2_t latchedVec2;
		vec3_t latchedVec3;
		vec4_t latchedVec4;
		ucolor_t latchedColor;
	};
	union{
		float resetFloatval;
		int resetInteger;
		const char* resetString;
		byte resetBoolean;
		vec2_t resetVec2;
		vec3_t resetVec3;
		vec4_t resetVec4;
		ucolor_t resetColor;
	};
	union{
		int imin;
		float fmin;
		int enumCount;
	};
	union{
		int imax;
		float fmax;
		const char** enumStrings;
	};
	byte (__cdecl *domainFunc)(struct cvar_s *, CvarValue_t);
	struct cvar_s *hashNext;
} cvar_t;

#pragma pack(pop)
extern int cvar_modifiedFlags;


//Defines Cvarrelated functions inside executable file
cvar_t* Cvar_RegisterString(const char *var_name, const char *var_value, unsigned short flags, const char *var_description);
cvar_t* Cvar_RegisterBool(const char *var_name, qboolean var_value, unsigned short flags, const char *var_description);
cvar_t* Cvar_RegisterInt(const char *var_name, int var_value, int min_value, int max_value, unsigned short flags, const char *var_description);
cvar_t* Cvar_RegisterEnum(const char *var_name, const char** valnames, int defaultval, unsigned short flags, const char *var_description);
cvar_t* Cvar_RegisterFloat(const char *var_name, float var_value, float min_value, float max_value, unsigned short flags, const char *var_description);
cvar_t* Cvar_RegisterVec2(const char* name, float x, float y, float min, float max, unsigned short flags, const char* description);
cvar_t* Cvar_RegisterVec3(const char* name, float x, float y, float z, float min, float max, unsigned short flags, const char* description);
cvar_t* Cvar_RegisterVec4(const char* name, float x, float y, float z, float imag, float min, float max, unsigned short flags, const char* description);
cvar_t* Cvar_RegisterColor(const char* name, float r, float g, float b, float alpha, unsigned short flags, const char* description);

void Cvar_SetInt(cvar_t* var, int val);
void Cvar_SetBool(cvar_t* var, qboolean val);
void Cvar_SetString(cvar_t* var, char const* string);
void Cvar_SetFloat(cvar_t* var, float val);
void Cvar_SetVec2( cvar_t* cvar, float x, float y);
void Cvar_SetVec3( cvar_t* cvar, float x, float y, float z);
void Cvar_SetVec4( cvar_t* cvar, float x, float y, float z, float imag);
void Cvar_SetColor( cvar_t* cvar, float red, float green, float blue, float alpha);
void Cvar_Set_f(void);
void Cvar_SetS_f(void);
void Cvar_Toggle_f(void);
void Cvar_TogglePrint_f(void);
void Cvar_SetA_f(void);
void Cvar_SetFromCvar_f(void);
void Cvar_SetFromLocalizedStr_f(void);
void Cvar_SetToTime_f(void);
void Cvar_Reset_f(void);
void Cvar_ResetVar(cvar_t* var);
void Cvar_List_f(void);
void Cvar_Dump_f(void);
void Cvar_RegisterBool_f(void);
void Cvar_RegisterInt_f(void);
void Cvar_RegisterFloat_f(void);
void Cvar_SetU_f(void);
qboolean Cvar_Command( void );
int  g_cvar_valueforkey(char* key);
char* Cvar_InfoString(int bit);
char* Cvar_InfoString_IW_Wrapper(int dummy, int bit);
void Cvar_ForEach(void (*callback)(cvar_t const*, void* passedhere), void* passback);
char* Cvar_DisplayableValue(cvar_t const*);
const char* Cvar_GetVariantString(const char* name);
cvar_t* Cvar_FindMalleableVar(const char* name);
void Cvar_Init(void);
void Cvar_CommandCompletion( void(*callback)(const char *s) );
const char *Cvar_VariableString( const char *var_name );
float Cvar_VariableValue( const char *var_name );
int Cvar_VariableIntegerValue( const char *var_name );
qboolean Cvar_VariableBooleanValue( const char *var_name );
cvar_t* Cvar_Set( const char *var_name, const char *value);
void Cvar_SetAllowCheatOnly( const char *var_name, const char *value);
void Cvar_Reset(cvar_t* cvar);
void Cvar_ResetByName(const char* var_name);

void Cvar_SetFloatByName( const char* var_name, float value);
void Cvar_SetIntByName( const char* var_name, int value);
void Cvar_SetBoolByName( const char* var_name, qboolean value);
void Cvar_SetStringByName( const char* var_name, const char* val);
cvar_t *Cvar_FindVar( const char *var_name );
qboolean Cvar_ValidateString( const char *s );
void Cvar_AddFlags(cvar_t* var, unsigned short flags);
void Cvar_AddFlagsByName(const char* var_name, unsigned short flags);
void Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
cvar_t* Cvar_FindMalleable(const char* name);
void Cvar_ClearModified(cvar_t* cvar);
char* Cvar_GetValueAsString(char* name);
const char* __cdecl Dvar_InfoString(int type );
qboolean Dvar_ValueEqualStringToString(const char *varname, const char* ctx, const char * str1, const char *str2);
cvar_t* Cvar_SetFromStringByNameFromSource(const char* var, const char* valstr, int setsrc);
void Cvar_SetStringByName(const char *var_name, const char *valuestr);
void Cvar_SetCheatState();
const char *Cvar_ToString(cvar_t *var);
void Cvar_GetUnpackedColorByName(const char* name, float *expandedColor);
void Cvar_GetUnpackedColor(cvar_t *cvar, float *expandedColor);
#define Cvar_GetInt Cvar_VariableIntegerValue
#define Cvar_GetFloat Cvar_VariableValue
#define Cvar_GetString Cvar_VariableString
#define Cvar_GetBool Cvar_VariableBooleanValue
#define cvar_modifiedFlags *((int*)(0x0CBA73F4))
//#define Dvar_InfoString(X,Y) Cvar_InfoString(Y)
void Cvar_ClearFlags(cvar_t* var, int flags);
int Cvar_GetFlags (cvar_t *var);
qboolean Cvar_HasLatchedValue(cvar_t *cvar);

/*
void __cdecl Cvar_SetInt(cvar_t const* var, int val);
void __cdecl Cvar_SetBool(cvar_t const* var, qboolean val);
void __cdecl Cvar_SetString(cvar_t const* var, char const* string);
void __cdecl Cvar_SetFloat(cvar_t const* var, float val);
void __cdecl Cvar_Set_f(void);
void __cdecl Cvar_SetS_f(void);
void __cdecl Cvar_Toggle_f(void);
void __cdecl Cvar_TogglePrint_f(void);
void __cdecl Cvar_SetA_f(void);
void __cdecl Cvar_SetFromCvar_f(void);
void __cdecl Cvar_SetFromLocalizedStr_f(void);
void __cdecl Cvar_SetToTime_f(void);
void __cdecl Cvar_Reset_f(void);
void __cdecl Cvar_List_f(void);
void __cdecl Cvar_Dump_f(void);
void __cdecl Cvar_RegisterBool_f(void);
void __cdecl Cvar_RegisterInt_f(void);
void __cdecl Cvar_RegisterFloat_f(void);
void __cdecl Cvar_SetU_f(void);
int __cdecl g_cvar_valueforkey(char* key);
char* __cdecl Cvar_InfoString(int unk, int bit);
void __cdecl Cvar_ForEach(void (*callback)(cvar_t const*, void* passedhere), void* passback);
char* __cdecl Cvar_DisplayableValue(cvar_t const*);
char* __cdecl Cvar_GetVariantString(const char* name);
cvar_t* __regparm1 Cvar_FindMalleableVar(const char* name);
void Cvar_Init(void);
*/

//defines Cvarflags
#define	CVAR_ARCHIVE		1	// set to cause it to be saved to vars.rc
								// used for system variables, not for player
								// specific configurations
#define	CVAR_USERINFO		2	// sent to server on connect or change
#define	CVAR_SERVERINFO		4	// sent in response to front end requests
#define	CVAR_SYSTEMINFO		8	// these cvars will be duplicated on all clients
#define	CVAR_INIT		16	// don't allow change from console at all,
								// but can be set from the command line
#define	CVAR_LATCH		32	// will only change when C code next does
								// a Cvar_Get(), so it can't be changed
								// without proper initialization.  modified
								// will be set, even though the value hasn't
								// changed yet
#define	CVAR_ROM		64	// display only, cannot be set by user at all
#define CVAR_CHEAT		128	// can not be changed if cheats are disabled
#define	CVAR_TEMP		256	// can be set even when cheats are disabled, but is not archived
#define CVAR_NORESTART		1024	// do not clear when a cvar_restart is issued
#define	CVAR_USER_CREATED	16384	// created by a set command


#define getcvaradr(adr) ((cvar_t*)(*(int*)(adr)))

//=====================================================================


// in order from highest priority to lowest
// if none of the catchers are active, bound key strings will be executed
#define KEYCATCH_CONSOLE		0x0001
#define	KEYCATCH_UI					0x0002
#define	KEYCATCH_MESSAGE		0x0004
#define	KEYCATCH_CGAME			0x0008


// sound channels
// channel 0 never willingly overrides
// other channels will allways override a playing sound on that channel
typedef enum {
	CHAN_AUTO,
	CHAN_LOCAL,		// menu sounds, etc
	CHAN_WEAPON,
	CHAN_VOICE,
	CHAN_ITEM,
	CHAN_BODY,
	CHAN_LOCAL_SOUND,	// chat messages, etc
	CHAN_ANNOUNCER		// announcer voices, etc
} soundChannel_t;


/*
========================================================================

  ELEMENTS COMMUNICATED ACROSS THE NET

========================================================================
*/

#define	ANGLE2SHORT(x)	((int)((x)*65536.0f/360.0f + 0.5f) & 65535)
#define	SHORT2ANGLE(x)	((x)*(360.0/65536))

#define	SNAPFLAG_RATE_DELAYED	1
#define	SNAPFLAG_NOT_ACTIVE		2	// snapshot used during connection and for zombies
#define SNAPFLAG_SERVERCOUNT	4	// toggled every map_restart so transitions can be detected

//
// per-level limits
//
#define	MAX_CLIENTS			18		// absolute limit
#define MAX_LOCATIONS		64

#define	GENTITYNUM_BITS		10		// don't need to send any more
#define	MAX_GENTITIES		(1<<GENTITYNUM_BITS)

// entitynums are communicated with GENTITY_BITS, so any reserved
// values that are going to be communcated over the net need to
// also be in this range
#define	ENTITYNUM_NONE		(MAX_GENTITIES-1)
#define	ENTITYNUM_WORLD		(MAX_GENTITIES-2)
#define	ENTITYNUM_MAX_NORMAL	(MAX_GENTITIES-2)


#define	MAX_MODELS			512		// these are sent over the net as 8 bits
#define	MAX_SOUNDS			256		// so they cannot be blindly increased


#define	MAX_CONFIGSTRINGS	2442	//

// these are the only configstrings that the system reserves, all the
// other ones are strictly for servergame to clientgame communication
#define	CS_SERVERINFO		0		// an info string with all the serverinfo cvars
#define	CS_SYSTEMINFO		1		// an info string for server system to client system configuration (timescale, etc)

#define	RESERVED_CONFIGSTRINGS	2	// game can't modify below this, only the system can

#define	MAX_GAMESTATE_CHARS	0x20000
typedef struct {
	int			stringOffsets[MAX_CONFIGSTRINGS];
	char		stringData[MAX_GAMESTATE_CHARS];
	int			dataCount;
} gameState_t;

//=========================================================

// bit field limits
#define	MAX_STATS				16
#define	MAX_PERSISTANT			16
#define	MAX_POWERUPS			16
#define	MAX_WEAPONS				128

#define	MAX_PS_EVENTS			2

#define PS_PMOVEFRAMECOUNTBITS	6


#define CONTENTS_BODY           0x2000000   // should never be on a brush, only in game

#define PLAYER_SOLIDMASK	0x00600000


typedef enum {
	TEAM_FREE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SPECTATOR,

	TEAM_NUM_TEAMS
} team_t;


typedef struct clientState_s
{
  int clientIndex;
  team_t team;
  int modelindex;
  int attachModelIndex[6];
  int attachTagIndex[6];
  char name[16];
  float maxSprintTimeMultiplier;
  int rank;
  int prestige;
  int perks;
  int attachedVehEntNum;
  int attachedVehSlotIndex;
}clientState_t;

typedef struct{
	float	yaw;
	int	timer;
	int	transIndex;
	int	flags;
}mantleState_t;

typedef struct{
	int	sprintButtonUpRequired;
	int	sprintDelay;
	int	lastSprintStart;
	int	lastSprintEnd;
	int	sprintStartMaxLength;
}sprintState_t;

typedef enum
{
  PLAYER_OFFHAND_SECONDARY_SMOKE = 0x0,
  PLAYER_OFFHAND_SECONDARY_FLASH = 0x1,
  PLAYER_OFFHAND_SECONDARIES_TOTAL = 0x2,
}OffhandSecondaryClass_t;

typedef enum
{
  PLAYERVIEWLOCK_NONE = 0x0,
  PLAYERVIEWLOCK_FULL = 0x1,
  PLAYERVIEWLOCK_WEAPONJITTER = 0x2,
  PLAYERVIEWLOCKCOUNT = 0x3,
}ViewLockTypes_t;

typedef enum
{
  ACTIONSLOTTYPE_DONOTHING = 0x0,
  ACTIONSLOTTYPE_SPECIFYWEAPON = 0x1,
  ACTIONSLOTTYPE_ALTWEAPONTOGGLE = 0x2,
  ACTIONSLOTTYPE_NIGHTVISION = 0x3,
  ACTIONSLOTTYPECOUNT = 0x4,
}ActionSlotType_t;


typedef struct
{
  unsigned int index;
}ActionSlotParam_SpecifyWeapon_t;


typedef struct
{
  ActionSlotParam_SpecifyWeapon_t specifyWeapon;
}ActionSlotParam_t;

#define MAX_HUDELEMENTS 31

typedef enum
{
  HE_TYPE_FREE = 0x0,
  HE_TYPE_TEXT = 0x1,
  HE_TYPE_VALUE = 0x2,
  HE_TYPE_PLAYERNAME = 0x3,
  HE_TYPE_MAPNAME = 0x4,
  HE_TYPE_GAMETYPE = 0x5,
  HE_TYPE_MATERIAL = 0x6,
  HE_TYPE_TIMER_DOWN = 0x7,
  HE_TYPE_TIMER_UP = 0x8,
  HE_TYPE_TENTHS_TIMER_DOWN = 0x9,
  HE_TYPE_TENTHS_TIMER_UP = 0xA,
  HE_TYPE_CLOCK_DOWN = 0xB,
  HE_TYPE_CLOCK_UP = 0xC,
  HE_TYPE_WAYPOINT = 0xD,
  HE_TYPE_COUNT = 0xE,
}he_type_t;

/* 6853 */
typedef struct
{
  char r;
  char g;
  char b;
  char a;
}hudelem_colorsplit_t;

/* 6854 */
typedef union
{
  hudelem_colorsplit_t split;
  int rgba;
}hudelem_color_t;

typedef struct hudelem_s
{
  he_type_t type;
  float x;
  float y;
  float z;
  int targetEntNum;
  float fontScale;
  int font;
  int alignOrg;
  int alignScreen;
  hudelem_color_t color;
  hudelem_color_t fromColor; //0x28
  int fadeStartTime; //0x2c
  int fadeTime;
  int label;
  int width;
  int height; //0x3C
  int materialIndex;
  int offscreenMaterialIdx; //0x44
  int fromWidth;
  int fromHeight;
  int scaleStartTime;
  int scaleTime;
  float fromX;
  float fromY;
  int fromAlignOrg;
  int fromAlignScreen;
  int moveStartTime;
  int moveTime;
  int time;
  int duration;
  float value;
  int text;
  float sort;
  hudelem_color_t glowColor; //0x84
  int fxBirthTime;
  int fxLetterTime;
  int fxDecayStartTime;
  int fxDecayDuration;
  int soundID;
  int flags;
}hudelem_t;

typedef struct hudElemState_s
{
  hudelem_t current[MAX_HUDELEMENTS];
  hudelem_t archival[MAX_HUDELEMENTS];
}hudElemState_t;


typedef enum
{
  OBJST_EMPTY = 0x0,
  OBJST_ACTIVE = 0x1,
  OBJST_INVISIBLE = 0x2,
  OBJST_DONE = 0x3,
  OBJST_CURRENT = 0x4,
  OBJST_FAILED = 0x5,
  OBJST_NUMSTATES = 0x6,
}objectiveState_t;

typedef struct objective_s
{
  objectiveState_t state;
  float origin[3];
  int entNum;
  int teamNum;
  int icon;
}objective_t;

// playerState_t is the information needed by both the client and server
// to predict player motion and actions
// nothing outside of pmove should modify these, or some degree of prediction error
// will occur

// you can't add anything to this without modifying the code in msg.c

// playerState_t is a full superset of entityState_t as it is used by players,
// so if a playerState_t is transmitted, the entityState_t can be fully derived
// from it.
typedef struct playerState_s {
	int		commandTime;  // 0
	int		pm_type;  // 4
	int		bobCycle;  // 8
	int		pm_flags;  // 12
	int		weapFlags;  // 16
	int		otherFlags;  // 20
	int		pm_time;  // 24
	vec3_t		origin;  // 28

	// http://zeroy.com/script/player/getvelocity.htm
	vec3_t		velocity;  // 40
	vec2_t		oldVelocity;

	int		weaponTime;  // 60
	int		weaponDelay;  // 64
	int		grenadeTimeLeft;  // 68
	int		throwBackGrenadeOwner;  // 72
	int		throwBackGrenadeTimeLeft;  // 76
	int		weaponRestrictKickTime;  // 80
	int		foliageSoundTime;  // 84
	int		gravity;  // 88
	float	leanf;  // 92
	int		speed;  // 96
	vec3_t		delta_angles;  // 100

	/*The ground entity's rotation will be added onto the player's view.  In particular, this will
	* cause the player's yaw to rotate around the entity's z-axis instead of the world z-axis.
	* Any rotation that the reference entity undergoes will affect the player.
	* http://zeroy.com/script/player/playersetgroundreferenceent.htm */
	int		groundEntityNum;  // 112

	vec3_t		vLadderVec;  // 116
	int			jumpTime;  // 128
	float		jumpOriginZ;  // 132

	// Animations as in mp/playeranim.script and animtrees/multiplayer.atr, it also depends on mp/playeranimtypes.txt (the currently used weapon)
	int		legsTimer;  // 136
	int		legsAnim;  // 140
	int		torsoTimer;  // 144
	int		torsoAnim;  // 148

	int		legsAnimDuration; // 152
	int		torsoAnimDuration; // 156

	int		damageTimer;  // 160
	int		damageDuration;  // 164
	int		flinchYawAnim;  // 168
	int		movementDir;  // 172
	int		eFlags;  // 176
	int		eventSequence;  // 180

	int events[4];
	unsigned int eventParms[4];

	int		oldEventSequence;

	int		clientNum;  // 220
	int		offHandIndex;  // 224
	OffhandSecondaryClass_t	offhandSecondary;  // 228
	unsigned int weapon;  // 232
	int		weaponstate;  // 236
	unsigned int weaponShotCount;  // 240
	float	fWeaponPosFrac;  // 244
	int		adsDelayTime;  // 248

	// http://zeroy.com/script/player/resetspreadoverride.htm
	// http://zeroy.com/script/player/setspreadoverride.htm
	int		spreadOverride;  // 252
	int		spreadOverrideState;  // 256

	int		viewmodelIndex;  // 260

	vec3_t		viewangles;  // 264
	int		viewHeightTarget;  // 276
	float	viewHeightCurrent;  // 280
	int		viewHeightLerpTime;  // 284
	int		viewHeightLerpTarget;  // 288
	int		viewHeightLerpDown;  // 292
	vec2_t		viewAngleClampBase;  // 296
	vec2_t		viewAngleClampRange;  // 304

	int		damageEvent;  // 312
	int		damageYaw;  // 316
	int		damagePitch;  // 320
	int		damageCount;  // 324

	int		stats[5]; // 328
	int		ammo[128]; // 348
	int		ammoclip[128]; // 860, 0x35C

	unsigned int	weapons[4]; // 1372 0x55C
	unsigned int	weaponold[4]; // 1388 0x56C
	unsigned int	weaponrechamber[4]; // 1404 0x57C

	float		proneDirection;  // 1420
	float		proneDirectionPitch;  // 1424
	float		proneTorsoPitch;  // 1428
	ViewLockTypes_t		viewlocked;  // 1432
	int		viewlocked_entNum;  // 1436

	int		cursorHint;  // 1440
	int		cursorHintString;  // 1444
	int		cursorHintEntIndex;  // 1448

	int		iCompassPlayerInfo;  // 1452
	int		radarEnabled;  // 1456

	int		locationSelectionInfo;  // 1460
	sprintState_t	sprintState;  // 1464

	// used for leaning?
	float		fTorsoPitch;  // 1484
	float		fWaistPitch;  // 1488

	float		holdBreathScale;  // 1492
	int		holdBreathTimer;  // 1496

	// Scales player movement speed by this amount, ???it's actually a float???
	// http://zeroy.com/script/player/setmovespeedscale.htm
	float		moveSpeedScaleMultiplier;  // 1500

	mantleState_t	mantleState;  // 1504
	float		meleeChargeYaw;  // 1520
	int		meleeChargeDist;  // 1524
	int		meleeChargeTime;  // 1528
	int		perks;  // 1532

	ActionSlotType_t	actionSlotType[4];  // 1536
	ActionSlotParam_t		actionSlotParam[4];  // 1552

	int		entityEventSequence; // 1568

	int		weapAnim;  // 1572 0x624
	float	aimSpreadScale;  // 1576

	// http://zeroy.com/script/player/shellshock.htm
	int		shellshockIndex;  // 1580
	int		shellshockTime;  // 1584
	int		shellshockDuration;  // 1588

	// http://zeroy.com/script/player/setdepthoffield.htm
	float		dofNearStart;  // 1592
	float		dofNearEnd;  // 1596
	float		dofFarStart;  // 1600
	float		dofFarEnd;  // 1604
	float		dofNearBlur;  // 1608
	float		dofFarBlur;  // 1612
	float		dofViewmodelStart;  // 1616
	float		dofViewmodelEnd;  // 1620

	int			hudElemLastAssignedSoundID;  // 1624

	objective_t objective[16];
	char weaponmodels[128];

	int		deltaTime;  // 2204
	int		killCamEntity;  // 2208

	hudElemState_t hud;// 2212
} playerState_t;//Size: 0x2f64



// entityState_t is the information conveyed from the server
// in an update message about entities that the client will
// need to render in some way
// Different eTypes may use the information in different ways
// The messages are delta compressed, so it doesn't really matter if
// the structure size is fairly large

typedef enum {
	TR_STATIONARY,
	TR_INTERPOLATE,				// non-parametric, but interpolate between snapshots
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_SINE,					// value = base + sin( time / duration ) * delta
	TR_GRAVITY
} trType_t;

typedef struct {
	trType_t	trType;
	int		trTime;
	int		trDuration;			// if non 0, trTime + trDuration = stop time
	vec3_t		trBase;
	vec3_t		trDelta;			// velocity, etc
} trajectory_t;



typedef struct LerpEntityStatePhysicsJitter_s
{
  float innerRadius;
  float minDisplacement;
  float maxDisplacement;
}LerpEntityStatePhysicsJitter_t;


/* 6835 */
typedef struct LerpEntityStatePlayer_s
{
  float leanf;
  int movementDir;
}LerpEntityStatePlayer_t;

/* 6836 */
typedef struct LerpEntityStateLoopFx_s
{
  float cullDist;
  int period;
}LerpEntityStateLoopFx_t;

/* 6837 */
typedef struct LerpEntityStateCustomExplode_s
{
  int startTime;
}LerpEntityStateCustomExplode_t;

/* 6838 */
typedef struct LerpEntityStateTurret_s
{
  float gunAngles[3];
}LerpEntityStateTurret_t;

/* 6839 */
typedef struct LerpEntityStateAnonymous_s
{
  int data[7];
}LerpEntityStateAnonymous_t;

/* 6840 */
typedef struct LerpEntityStateExplosion_s
{
  float innerRadius;
  float magnitude;
}LerpEntityStateExplosion_t;

/* 6841 */
typedef struct LerpEntityStateBulletHit_s
{
  float start[3];
}LerpEntityStateBulletHit_t;

/* 6842 */
typedef struct LerpEntityStatePrimaryLight_s
{
  char colorAndExp[4];
  float intensity;
  float radius;
  float cosHalfFovOuter;
  float cosHalfFovInner;
}LerpEntityStatePrimaryLight_t;

/* 6843 */
typedef struct LerpEntityStateMissile_s
{
  int launchTime;
}LerpEntityStateMissile_t;

/* 6844 */
typedef struct LerpEntityStateSoundBlend_s
{
  float lerp;
}LerpEntityStateSoundBlend_t;

/* 6845 */
typedef struct LerpEntityStateExplosionJolt_s
{
  float innerRadius;
  float impulse[3];
}LerpEntityStateExplosionJolt_t;

/* 6831 */
typedef struct LerpEntityStateVehicle_s
{
  float bodyPitch;
  float bodyRoll;
  float steerYaw;
  int materialTime;
  float gunPitch;
  float gunYaw;
  int team;
}LerpEntityStateVehicle_t;

typedef struct LerpEntityStateEarthquake_s
{
  float scale;
  float radius;
  int duration;
}LerpEntityStateEarthquake_t;

typedef union
{
  LerpEntityStateTurret_t turret;
  LerpEntityStateLoopFx_t loopFx;
  LerpEntityStatePrimaryLight_t primaryLight;
  LerpEntityStatePlayer_t player;
  LerpEntityStateVehicle_t vehicle;
  LerpEntityStateMissile_t missile;
  LerpEntityStateSoundBlend_t soundBlend;
  LerpEntityStateBulletHit_t bulletHit;
  LerpEntityStateEarthquake_t earthquake;
  LerpEntityStateCustomExplode_t customExplode;
  LerpEntityStateExplosion_t explosion;
  LerpEntityStateExplosionJolt_t explosionJolt;
  LerpEntityStatePhysicsJitter_t physicsJitter;
  LerpEntityStateAnonymous_t anonymous;
}LerpEntityStateTypeUnion_t;



typedef struct LerpEntityState_s
{
  int eFlags;
  trajectory_t pos;
  trajectory_t apos;
  LerpEntityStateTypeUnion_t u;
}LerpEntityState_t;

// entityState_t is the information conveyed from the server
// in an update message about entities that the client will
// need to render in some way
// Different eTypes may use the information in different ways
// The messages are delta compressed, so it doesn't really matter if
// the structure size is fairly large

typedef struct entityState_s {//Confirmed names and offsets but not types

	int		number;			// entity index	//0x00
	int		eType;			// entityType_t	//0x04

	LerpEntityState_t		lerp;

	int		time2;			//0x70


	int		otherEntityNum;		//0x74 shotgun sources, etc
	int		attackerEntityNum;	//0x78

	int		groundEntityNum;	//0x7c -1 = in air

	int		loopSound;		//0x80 constantly loop this sound
	int		surfType;		//0x84


	clipHandle_t	index;			//0x88
	int		clientNum;		//0x8c 0 to (MAX_CLIENTS - 1), for players and corpses
	int		iHeadIcon;		//0x90
	int		iHeadIconTeam;		//0x94

	int		solid;			//0x98 for client side prediction, trap_linkentity sets this properly	0x98

	int		eventParm;		//0x9c impulse events -- muzzle flashes, footsteps, etc
	int		eventSequence;		//0xa0

	int		events[4];			//0xa4
	int		eventParms[4];		//0xb4

	// for players
	int		weapon;			//0xc4 determines weapon and flash model, etc
	int		weaponModel;		//0xc8
	int		legsAnim;		//0xcc mask off ANIM_TOGGLEBIT
	int		torsoAnim;		//0xd0 mask off ANIM_TOGGLEBIT

	union{
		int scale;
		int eventParm2;
		int	helicopterStage;	//0xd4
	}un1;

	union{
		int hintString;
		int grenadeInPickupRange;
		int vehicleXModel;
	}un2;					//0xd8

	int		fTorsoPitch;		//0xdc
	int		fWaistPitch;		//0xe0
	int		partBits[4];		//0xe4
} entityState_t;	//sizeof(entityState_t): 0xf4



typedef struct clientControllers_s
{
  vec3_t angles[6];
  vec3_t tag_origin_angles;
  vec3_t tag_origin_offset;
}clientControllers_t;



//====================================================================


//
// usercmd_t->button bits, many of which are generated by the client system,
// so they aren't game/cgame only definitions
//
#define	BUTTON_ATTACK		1
#define	BUTTON_TALK			2			// displays talk balloon and disables actions
#define	BUTTON_USE_HOLDABLE	4
#define	BUTTON_GESTURE		8
#define	BUTTON_WALKING		16			// walking can't just be infered from MOVE_RUN
										// because a key pressed late in the frame will
										// only generate a small move value for that frame
										// walking will use different animations and
										// won't generate footsteps
#define BUTTON_AFFIRMATIVE	32
#define	BUTTON_NEGATIVE		64

#define BUTTON_GETFLAG		128
#define BUTTON_GUARDBASE	256
#define BUTTON_PATROL		512
#define BUTTON_FOLLOWME		1024

#define	BUTTON_ANY			2048			// any key whatsoever

#define	MOVE_RUN			120			// if forwardmove or rightmove are >= MOVE_RUN,
										// then BUTTON_WALKING should be set


// usercmd_t is sent to the server each client frame
#pragma pack(push, 1)
typedef struct usercmd_s
{
  int serverTime;
  int buttons;
  int angles[3];
  byte weapon;
  byte offHandIndex;
  byte field_16;
  byte field_17;
  int field_18;
  int field_1C;
}usercmd_t;
#pragma pack(pop)

/*
typedef struct usercmd_s {//Not Known
	int			serverTime;
	int			angles[3];
	int 			buttons;
	byte			weapon;           // weapon
	byte			weapon2;
	signed char	forwardmove, rightmove, upmove;
	byte			unk[7];
} usercmd_t;
*/
// usercmd_t is sent to the server each client frame

/*
=================
PlaneTypeForNormal
=================
*/

#define PlaneTypeForNormal( x ) ( x[0] == 1.0 ? PLANE_X : ( x[1] == 1.0 ? PLANE_Y : ( x[2] == 1.0 ? PLANE_Z : PLANE_NON_AXIAL ) ) )

// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!
typedef struct cplane_s {//Nothing validated
	vec3_t normal;
	float dist;
	byte type;              // for fast side tests: 0,1,2 = axial, 3 = nonaxial
	byte signbits;          // signx + (signy<<1) + (signz<<2), used as lookup during collision
	byte pad[2];
} cplane_t;


// a trace is returned when a box is swept through the world
typedef struct {
	float	fraction;       //0x00 time completed, 1.0 = didn't hit anything
	int	unknown[6];
/*	qboolean allsolid;      // if true, plane is not valid
	qboolean startsolid;    // if true, the initial point was in a solid area
	float fraction;         // time completed, 1.0 = didn't hit anything			//0x00
	vec3_t endpos;          // final position
	cplane_t plane;         // surface normal at impact, transformed to world space
	int surfaceFlags;       // surface hit
	int contents;           // contents on other side of surface hit*/
	int	var_02;		//0x1c
	short	entityNum;      //0x20 entity the contacted sirface is a part of
} trace_t;



typedef enum {
	CA_UNINITIALIZED,
	CA_DISCONNECTED, 	// not talking to a server
	CA_AUTHORIZING,		// not used any more, was checking cd key
	CA_CHALLENGING,		// sending challenge packets to the server
	CA_CONNECTING,		// sending request packets to the server
	CA_CONNECTED,		// netchan_t established, getting gamestate
	CA_STATSSYNC,
	CA_LOADING,			// only during cgame initialization, never during main loop
	CA_PRIMED,			// got gamestate, waiting for first frame
	CA_ACTIVE,			// game views should be displayed
	CA_CINEMATIC		// playing a cinematic or a static pic, not connected to a server
} connstate_t;

// font support

#define GLYPH_START 0
#define GLYPH_END 255
#define GLYPH_CHARSTART 32
#define GLYPH_CHAREND 127
#define GLYPHS_PER_FONT GLYPH_END - GLYPH_START + 1
typedef struct {
  int height;       // number of scan lines
  int top;          // top of glyph in buffer
  int bottom;       // bottom of glyph in buffer
  int pitch;        // width for copying
  int xSkip;        // x adjustment
  int imageWidth;   // width of actual image
  int imageHeight;  // height of actual image
  float s;          // x offset in image where glyph starts
  float t;          // y offset in image where glyph starts
  float s2;
  float t2;
  qhandle_t glyph;  // handle to the shader with the glyph
  char shaderName[32];
} glyphInfo_t;

typedef struct {
  glyphInfo_t glyphs [GLYPHS_PER_FONT];
  float glyphScale;
  char name[MAX_QPATH];
} fontInfo_t;

#define Square(x) ((x)*(x))

// real time
//=============================================


typedef struct qtime_s {
	int tm_sec;     /* seconds after the minute - [0,59] */
	int tm_min;     /* minutes after the hour - [0,59] */
	int tm_hour;    /* hours since midnight - [0,23] */
	int tm_mday;    /* day of the month - [1,31] */
	int tm_mon;     /* months since January - [0,11] */
	int tm_year;    /* years since 1900 */
	int tm_wday;    /* days since Sunday - [0,6] */
	int tm_yday;    /* days since January 1 - [0,365] */
	int tm_isdst;   /* daylight savings time flag */
} qtime_t;


// server browser sources
// TTimo: AS_MPLAYER is no longer used
#define AS_LOCAL		0
#define AS_GLOBAL		1
#define AS_FAVORITES	2
#define AS_MPLAYER		3


// cinematic states
typedef enum {
	FMV_IDLE,
	FMV_PLAY,		// play
	FMV_EOF,		// all other conditions, i.e. stop/EOF/abort
	FMV_ID_BLT,
	FMV_ID_IDLE,
	FMV_LOOPED,
	FMV_ID_WAIT
} e_status;

typedef enum _flag_status {
	FLAG_ATBASE = 0,
	FLAG_TAKEN,			// CTF
	FLAG_TAKEN_RED,		// One Flag CTF
	FLAG_TAKEN_BLUE,	// One Flag CTF
	FLAG_DROPPED
} flagStatus_t;



#define	MAX_GLOBAL_SERVERS			10000
#define	MAX_OTHER_SERVERS			128
#define MAX_PINGREQUESTS			32
#define MAX_SERVERSTATUSREQUESTS	16

#define SAY_ALL		0
#define SAY_TEAM	1
#define SAY_TELL	2

#define CDKEY_LEN 16
#define CDCHKSUM_LEN 2


#define LERP( a, b, w ) ( ( a ) * ( 1.0f - ( w ) ) + ( b ) * ( w ) )
#define LUMA( red, green, blue ) ( 0.2126f * ( red ) + 0.7152f * ( green ) + 0.0722f * ( blue ) )



typedef struct {
	unsigned const char	*start;
	unsigned const char	*end;
} patternseek_t;

/*****************************************************
*** Parsing files ***
*****************************************************/
void Com_ParseReset();
char* Com_ParseGetToken(char* line);
int Com_ParseTokenLength(char* token);

qboolean isNumeric(const char* string, int size);

/*
=====================================================================
 Functions to operate onto a stack in lifo mode
=====================================================================
*/

void stack_init(void *array[], size_t size);
qboolean stack_push(void *array[], int size, void* pointer);
void* stack_pop(void *array[], int size);

/*
=====================================================================
  Writing XML STRINGS
=====================================================================
*/


typedef struct{
    int		parents;
    qboolean	last;
    void *stack[48];
    char *buffer;
    int bufposition;
    char *encoding;
    size_t buffersize;
}xml_t;

void XML_Init( xml_t *base, char *s, int size, char* encoding);
void XML_Escape( char* buffer, size_t size, const char* string);
qboolean QDECL XML_OpenTag( xml_t *base, char* root, int count,... );
void XML_CloseTag(xml_t *base);

/*
=====================================================================
  MD5 Functions
=====================================================================
*/

typedef struct MD5Context {
	uint32_t buf[4];
	uint32_t bits[2];
	unsigned char in[64];
} MD5_CTX;

void MD5Init(struct MD5Context *ctx);
void MD5Update(struct MD5Context *ctx, unsigned char const *buf, unsigned len);
void MD5Final(struct MD5Context *ctx, unsigned char *digest);

typedef enum
{
	CRIT_CONSOLE = 0,
	CRIT_ERRORCHECK = 1,
	CRIT_ERROR = 2,
	CRIT_STATMON = 3,
	CRIT_SCRSTRINGGLOB = 4,
	CRTI_MEMTREE = 5,
	CRIT_REDIRECTPRINT = 6,
	CRIT_EVENTQUEUE = 7,
	CRIT_GPUFENCE = 8,
	CRIT_RENDER = 9,
	CRIT_FILESYSTEM = 10,
	CRIT_PHYSIC = 11,
	CRIT_MISC = 12,
	CRIT_SOUND = 13,
	CRIT_CINAMATIC1 = 14,
	CRIT_CINEMATIC2 = 15,
	CRIT_CBUF = 16,
	CRIT_LOGFILE = 17,
	CRIT_WAITFORMAINTHREADSUSPENDED = 21,
	CRIT_HTTPS = 22,
	CRITSECT_ASSERT = 0x1D,
	CRIT_SIZE
}crit_section_t;

void Q_strchrrepl(char *string, char torepl, char repl);
void Q_strchrreplUni(wchar_t *string, wchar_t torepl, wchar_t repl);
qboolean Q_WIsAnsiString(wchar_t* str);



void Q_strncpyzUni( wchar_t *dest, const wchar_t *src, int destsize );
int Q_StrToWStr(wchar_t* dest, const char* src, size_t len);
void Q_TrimCRLF(char* string);
void Q_TrimCRLFUni(wchar_t* string);
int QDECL Com_sprintfUni(wchar_t *dest, size_t size, const wchar_t *fmt, ...);
void Q_strcatUni( wchar_t *dest, int size, const wchar_t *src );
void CharToHexUni(wchar_t* string, char* hexstring, int size);
void HexToCharUni(char* hexstring, wchar_t* string, int size);
const char* Com_GetFilenameSubString(const char* arg);
void Hunk_FreeTempMemory(void* mem);
void Cmd_Exec_f();



typedef struct
{
  unsigned int sceneWidth;
  unsigned int sceneHeight;
  unsigned int displayWidth;
  unsigned int displayHeight;
  int displayFrequency;
  int isFullscreen;
  float aspectRatioWindow;
  float aspectRatioScenePixel;
  float aspectRatioDisplayPixel;
  unsigned int maxTextureSize;
  unsigned int maxTextureMaps;
  byte deviceSupportsGamma;
  byte pad0[3];
}vidConfig_t;





#ifdef __cplusplus
#define ASSERT_HALT() (std::abort())
#else
#define ASSERT_HALT() (abort())
#endif


//qboolean Assert_MyHandler(const char* exp, const char *filename, int line, const char *function, const char *fmt, ...);
qboolean Assert_MyHandler(const char *filename, int line, int type, const char *fmt, ...);

#define assert ASSERT
#define assertx XASSERT
//#define ASSERT_HANDLER(x, f, l, fu, ...) (Assert_MyHandler(x, f, l, fu, __VA_ARGS__))
#define ASSERT_HANDLER(x, f, l, fu, ...) (Assert_MyHandler(f, l, 0, __VA_ARGS__))

#ifdef NDEBUG
#define XASSERT(x, ...)
#define ASSERT(x)

#else
#define XASSERT(x, ...) (!(x) && ASSERT_HANDLER(#x, __FILE__, __LINE__, __func__, __VA_ARGS__) && (ASSERT_HALT(), 1))
#define ASSERT(x) XASSERT(x, NULL)

#endif

unsigned int __cdecl RandWithSeed(int *seed);




typedef struct {
    byte *buffer;
    unsigned int length;
    unsigned int start;
    unsigned int end;
} RingBuffer;

RingBuffer *RingBuffer_create(int length);
void RingBuffer_destroy(RingBuffer *buffer);
int RingBuffer_read(RingBuffer *buffer, char *target, unsigned int amount);
int RingBuffer_write(RingBuffer *buffer, char *data, unsigned int length);
int RingBuffer_empty(RingBuffer *buffer);
int RingBuffer_full(RingBuffer *buffer);
int RingBuffer_available_data(RingBuffer *buffer);
int RingBuffer_available_space(RingBuffer *buffer);
//bstring RingBuffer_gets(RingBuffer *buffer, int amount);
#define RingBuffer_available_data(B) ((B)->end - (B)->start)
#define RingBuffer_available_space(B) ((B)->length - RingBuffer_available_data(B))
#define RingBuffer_full(B) (RingBuffer_available_data((B)) - (B)->length == 0)
#define RingBuffer_empty(B) (RingBuffer_available_data((B)) == 0)
#define RingBuffer_starts_at(B) ((B)->buffer + (B)->start)
#define RingBuffer_ends_at(B) ((B)->buffer + (B)->end)
#define RingBuffer_commit_read(B, A) ((B)->start = ((B)->start + (A)) % (B)->length)
#define RingBuffer_commit_write(B, A) ((B)->end = ((B)->end + (A)) % (B)->length)

#endif	// __Q_SHARED_H
