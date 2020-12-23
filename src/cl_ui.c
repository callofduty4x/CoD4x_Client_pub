#include "q_shared.h"
#include "client.h"

#include <stdlib.h>
#include <string.h>

void Key_SetCatcher(int catcher)
{

	if ( clientUIActives.keyCatchers & KEYCATCH_CONSOLE )
		clientUIActives.keyCatchers = catcher | KEYCATCH_CONSOLE;
	else
		clientUIActives.keyCatchers = catcher;

	if ( !(clientUIActives.keyCatchers & 0x10) )
		clientUIActives.unk6 = 0;
}

void Key_RemoveCatcher(int catcher)
{
	clientUIActives.keyCatchers &= catcher;
	if ( !(clientUIActives.keyCatchers & 0x10) )
		clientUIActives.unk6 = 0;
}

/*
====================
LAN_GetLocalServerAddressString
====================
*/
void CLUI_GetServerAddressString( int source, int n, char *buf, int buflen ) {
	switch ( source ) {
		case AS_LOCAL:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				Q_strncpyz( buf, NET_AdrToString( &cls.localServers[n].adr ), buflen );
				return;
			}
			break;
		case AS_GLOBAL:
			if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
				Q_strncpyz( buf, NET_AdrToString( &cls.globalServers[n].adr ), buflen );
				return;
			}
			break;
		case AS_MPLAYER:
			if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
				Q_strncpyz( buf, NET_AdrToString( &cls.mplayerServers[n].adr ), buflen );
				return;
			}
			break;
		case AS_FAVORITES:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				Q_strncpyz( buf, NET_AdrToString( &cls.favoriteServers[n].adr ), buflen );
				return;
			}
			break;
	}
	buf[0] = '\0';
}

signed int CLUI_GetServerPunkBuster(int n, int source)
{
	switch ( source ) {
		case AS_LOCAL:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				return cls.localServers[n].punkbuster;
			}
			return -1;
		case AS_GLOBAL:
			if ( n >= 0 && n < cls.numglobalservers ) {
				return cls.globalServers[n].punkbuster;
			}
			return -1;
		case AS_MPLAYER:
			if ( n >= 0 && n < cls.nummplayerservers ) {
				return cls.mplayerServers[n].punkbuster;
			}
			return -1;
		case AS_FAVORITES:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				return cls.favoriteServers[n].punkbuster;
			}
			return -1;
	}
  	return -1;
}

/*
====================
LAN_GetServerInfo
====================
*/

void LAN_GetServerInfo( int source, int n, char *buf, int buflen ) {

	#define tostring(x) itoa(x, stringbuf, 10)
	int bots;
	char info[MAX_STRING_CHARS];
	char stringbuf[128];
	char tmp[16];
	serverInfo_t *server = NULL;
	info[0] = '\0';
	switch ( source ) {
		case AS_LOCAL:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				server = &cls.localServers[n];
			}
			break;
		case AS_GLOBAL:
			if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
				server = &cls.globalServers[n];
			}
			break;
		case AS_MPLAYER:
			if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
				server = &cls.mplayerServers[n];
			}
			break;
		case AS_FAVORITES:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				server = &cls.favoriteServers[n];
			}
			break;
	}
	if(buf == NULL)
		return;

	buf[0] = '\0';

	if ( server == NULL)
		return;
/*
	Com_sprintf(buf, buflen, "\\hostname\\%s\\mapname\\%s\\clients\\%d\\sv_maxclients\\%d\\ping\\%d\\minping\\%d\\maxping\\%d\\game\\%s"
				"\\gametype\\%s\\nettype\\%d\\addr\\%s\\pswrd\\%d\\con_disabled\\%d\\pure\\%d\\sv_allowAnonymous\\%d\\ff\\%d"
				"\\kc\\%d\\hc\\%d\\od\\%d\\hw\\%d\\dom\\%d\\voice\\%d\\pb\\%d",
				server->hostName, server->mapName, server->clients, server->maxClients,
				server->ping, server->minPing, server->maxPing, server->game, server->gameType,
				server->netType, NET_AdrToString( &server->adr ), server->pswrd, server->con_disabled,
				server->pure, server->allowAnonymous, server->friendlyFire, server->kc, server->hardcore,
				server->od, server->hw, server->dom, server->voice, server->punkbuster);


*/
	/*
	I had a speed problem so that usual way got replaced by that dirty one.
	*/


	FastInfo_SetValueForKey( info, "hostname", server->hostName );
	FastInfo_SetValueForKey( info, "mapname", server->mapName );

	bots = server->clients - server->humanPlayers;
	if(server->humanPlayers >= 0 && server->humanPlayers < 127 && bots)
	{
		Com_sprintf(tmp, sizeof(tmp), "%d(%d)", server->humanPlayers, bots);
	}else{
		Com_sprintf(tmp, sizeof(tmp), "%d", server->clients);
	}
	FastInfo_SetValueForKey( info, "clients", tmp );
	FastInfo_SetValueForKey( info, "sv_maxclients", tostring( server->maxClients ) );
	FastInfo_SetValueForKey( info, "ping", tostring( server->ping ) );
	FastInfo_SetValueForKey( info, "minping", tostring( server->minPing ) );
	FastInfo_SetValueForKey( info, "maxping", tostring( server->maxPing ) );
	FastInfo_SetValueForKey( info, "game", server->game );
	FastInfo_SetValueForKey( info, "gametype", server->gameType );
	FastInfo_SetValueForKey( info, "nettype", tostring( server->netType ) );
	FastInfo_SetValueForKey( info, "addr", NET_AdrToString( &server->adr ) );
	FastInfo_SetValueForKey( info, "pswrd", tostring( server->pswrd) );
	FastInfo_SetValueForKey( info, "con_disabled", tostring( server->con_disabled));
	FastInfo_SetValueForKey( info, "pure", tostring( server->pure));
	FastInfo_SetValueForKey( info, "sv_allowAnonymous", tostring( server->allowAnonymous ) );
	FastInfo_SetValueForKey( info, "ff", tostring( server->friendlyFire ) ); // NERVE - SMF
	FastInfo_SetValueForKey( info, "kc", tostring( server->kc));
	FastInfo_SetValueForKey( info, "hc", tostring( server->hardcore));
	FastInfo_SetValueForKey( info, "od", tostring( server->od));
	if(server->game[0] && Q_stricmp(server->game, "main"))
	{
		FastInfo_SetValueForKey( info, "mod", "1");
	}
	FastInfo_SetValueForKey( info, "hw", tostring( server->hw));
	FastInfo_SetValueForKey( info, "dom", tostring( server->dom));
	FastInfo_SetValueForKey( info, "voice", tostring( server->voice));
	FastInfo_SetValueForKey( info, "pb", tostring( server->punkbuster));
	Q_strncpyz( buf, info, buflen );

}

/*
====================
LAN_RemoveServer
====================
*/

int CLUI_RemoveServer(const char* serveradrstring)
{
	netadr_t address;
	int servernum;

	if(cls.numFavoriteServers <= 0)
		return 0;

	if(NET_StringToAdr(serveradrstring, &address, NA_UNSPEC) == 0)
		return 0;

	servernum = CL_FindServerByAddr( &address, AS_FAVORITES );
	if(servernum < 0)
		return 0;

	--cls.numFavoriteServers;

	Com_Memcpy(&cls.favoriteServers[servernum], &cls.favoriteServers[cls.numFavoriteServers], sizeof(serverInfo_t));
	return 1;
}
/*
====================
LAN_AddServer
====================
*/

int CLUI_AddServer(const char* hostname, const char* serveradrstring)
{
	netadr_t address;

	if(cls.numFavoriteServers >= 128)
		return -1;

	if(NET_StringToAdr(serveradrstring, &address, NA_UNSPEC) == 0)
		return -2;

	if(CL_FindServerByAddr( &address, AS_FAVORITES ) >= 0)
		return 0;

	cls.favoriteServers[cls.numFavoriteServers].adr = address;
	Q_strncpyz(cls.favoriteServers[cls.numFavoriteServers].hostName, hostname, sizeof(cls.favoriteServers[cls.numFavoriteServers].hostName));
	cls.favoriteServers[cls.numFavoriteServers].visible = 1;
	cls.favoriteServers[cls.numFavoriteServers].serveralive = 0;
	cls.numFavoriteServers++;
	return 1;
}

REGPARM(1) int LAN_WaitServerResponse(int updatesource)
{
  int result; // eax@2

  if ( updatesource == AS_GLOBAL || updatesource == AS_MPLAYER)
    result = Sys_Milliseconds() < cls.globalServerRequestTime;
  else
    result = 0;
  return result;
}

int LAN_GetServerCount(int pingupdatesource)
{
	int numServers;

	switch(pingupdatesource)
	{
		case AS_LOCAL:
		    numServers = cls.numlocalservers;
			break;
		case AS_GLOBAL:
	        numServers = cls.numglobalservers;
			break;
		case AS_FAVORITES:
			numServers = cls.numFavoriteServers;
			break;
		case AS_MPLAYER:
			numServers = cls.nummplayerservers;
			break;
		default:
			numServers = 0;
	}
	return numServers;
}

/*
====================
LAN_MarkServerVisible
====================
*/
void LAN_MarkServerVisible( int source, int n, qboolean visible ) {
	if ( n == -1 ) {
		int count = MAX_OTHER_SERVERS;
		serverInfo_t *server = NULL;
		switch ( source ) {
		case AS_LOCAL:
			server = &cls.localServers[0];
			break;
		case AS_MPLAYER:
			server = &cls.mplayerServers[0];
			count = cls.nummplayerservers;
			break;
		case AS_GLOBAL:
			server = &cls.globalServers[0];
			count = cls.numglobalservers;
			break;
		case AS_FAVORITES:
			server = &cls.favoriteServers[0];
			break;
		}
		if ( server ) {
			for ( n = 0; n < count; n++ ) {
				server[n].visible = visible;
			}
		}

	} else {
		switch ( source ) {
		case AS_LOCAL:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				cls.localServers[n].visible = visible;
			}
			break;
		case AS_MPLAYER:
			if ( n >= 0 && n < cls.nummplayerservers ) {
				cls.mplayerServers[n].visible = visible;
			}
			break;
		case AS_GLOBAL:
			if ( n >= 0 && n < cls.numglobalservers ) {
				cls.globalServers[n].visible = visible;
			}
			break;
		case AS_FAVORITES:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				cls.favoriteServers[n].visible = visible;
			}
			break;
		}
	}
}


/*
====================
LAN_ResetPings
====================
*/
void LAN_ResetPings( int source ) {
	int count,i;
	serverInfo_t *servers = NULL;
	count = 0;

	switch ( source ) {
	case AS_LOCAL:
		servers = &cls.localServers[0];
		count = MAX_OTHER_SERVERS;
		break;
	case AS_MPLAYER:
		servers = &cls.mplayerServers[0];
		count = cls.nummplayerservers;
		break;
	case AS_GLOBAL:
		servers = &cls.globalServers[0];
		count = cls.numglobalservers;
		break;
	case AS_FAVORITES:
		servers = &cls.favoriteServers[0];
		count = MAX_OTHER_SERVERS;
		break;
	}
	cls.countPingServers = 0;
	if ( servers ) {
		for ( i = 0; i < count; i++ ) {
			servers[i].ping = -1;
			servers[i].serveralive = qfalse;
			servers[i].queryTime = 0;
		}
	}
}

/*
====================
LAN_GetServerPtr
====================
*/
static serverInfo_t *LAN_GetServerPtr( int source, int n ) {
  switch (source) {
    case AS_LOCAL :
       if (n >= 0 && n < MAX_OTHER_SERVERS) {
        return &cls.localServers[n];
       }
       break;
    case AS_MPLAYER :
       if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
        return &cls.mplayerServers[n];
       }
       break;
    case AS_GLOBAL :
       if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
        return &cls.globalServers[n];
       }
       break;
    case AS_FAVORITES :
       if (n >= 0 && n < MAX_OTHER_SERVERS) {
        return &cls.favoriteServers[n];
       }
       break;
  }
  return NULL;
}


/*
=======================
LAN_ServerIsVisible
=======================
*/
int LAN_ServerIsVisible( int source, int n ) {
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return cls.localServers[n].visible;
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			return cls.mplayerServers[n].visible;
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			return cls.globalServers[n].visible;
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return cls.favoriteServers[n].visible;
		}
		break;
	}
	return qfalse;
}

/*
====================
LAN_GetServerPing
====================
*/
int LAN_GetServerPing( int source, int n ) {
	serverInfo_t *server = NULL;
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.localServers[n];
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			server = &cls.mplayerServers[n];
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			server = &cls.globalServers[n];
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.favoriteServers[n];
		}
		break;
	}
	if ( server ) {
		return server->ping;
	}
	return -1;
}

qboolean LAN_ServerInfoValid(unsigned int server)
{
  if ( server >= MAX_OTHER_SERVERS )
    return qfalse;
  else
    return cls.favoriteServers[server].serveralive == 1;
}

REGPARM(3) serverInfo_t *LAN_GetServerPtr_Stub( int n, int null, int source ) {
  return LAN_GetServerPtr(source, n);
}


void CL_DrawStretchPic(ScreenPlacement *place,
    float x, float y, float w, float h,
    int horzAlign, int vertAlign,
    float s0, float t0, float s1, float t1,
    const float *color, Material *material
)
{
    ScrPlace_ApplyRect(place, &x, &y, &w, &h, horzAlign, vertAlign);
    R_AddCmdDrawStretchPic(x, y, w, h, s0, t0, s1, t1, color, material);
}


void CL_DrawTextPhysicalWithEffects(const char *text, int maxChars, Font_t *font, float x, float y, float xScale, float yScale, const float *color, int style, const float *glowColor, Material *fxMaterial, Material *fxMaterialGlow, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration)
{
	R_AddCmdDrawTextWithEffects(text, maxChars, font, x, y, xScale, yScale, 0.0, color, style, glowColor, fxMaterial, fxMaterialGlow, fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration);
}

void CL_DrawTextPhysical(const char *text, int maxChars, Font_t *font, float x, float y, float xScale, float yScale, const float *color, int style)
{
  R_AddCmdDrawText(text, maxChars, font, x, y, xScale, yScale, 0.0, color, style);
}


void CL_DrawStretchPicPhysical(float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float *color, Material *material)
{
  R_AddCmdDrawStretchPic(x, y, w, h, s0, t0, s1, t1, color, material);
}

void __cdecl AddDebugStringInternal(const float *xyz, const float *color, float scale, const char *text, int duration, struct clientDebugStringInfo_t *info)
{
  struct trDebugString_t *string;

  if ( info->num + 1 <= info->max )
  {
    string = &info->strings[info->num];
		VectorCopy(xyz, string->xyz);
		Vector4Copy(color, string->color);
    string->scale = scale;
    Q_strncpyz(string->text, text, sizeof(string->text));
    info->durations[info->num++] = duration;
  }
}

void __cdecl CL_AddDebugString(const float *xyz, const float *color, float scale, const char *text, qboolean fromServer, int duration)
{
  struct clientDebugStringInfo_t *info;

  if ( cls.rendererStarted )
  {
    if ( CreateDebugStringsIfNeeded() )
    {
			if ( fromServer )
      {
        info = &cls.debug.svStrings;
      }
      else
      {
        info = &cls.debug.clStrings;
      }
      AddDebugStringInternal(xyz, color, scale, text, duration, info);
			if ( fromServer )
      {
				cls.debug.fromServer = 1;
      }
    }
  }
}

void __cdecl AddDebugLineInternal(const float *start, const float *end, const float *color, int depthTest, int duration, struct clientDebugLineInfo_t *info)
{
  struct trDebugLine_t *line;

  if ( info->num + 1 <= info->max )
  {
    line = &info->lines[info->num];

		VectorCopy(start, line->start);
		VectorCopy(end, line->end);
		Vector4Copy(color, line->color);
    line->depthTest = depthTest;
    info->durations[info->num++] = duration;
  }
}

void __cdecl CL_AddDebugLine(const float *start, const float *end, const float *color, int depthTest, int duration, qboolean fromServer)
{
	struct clientDebugLineInfo_t *info;

  if ( cls.rendererStarted )
  {
    if ( CreateDebugLinesIfNeeded() )
    {
      if ( fromServer )
      {
				info = &cls.debug.svLines;
      }
      else
      {
				info = &cls.debug.clLines;
      }
      AddDebugLineInternal(start, end, color, depthTest, duration, info);

      if ( fromServer )
      {
        cls.debug.fromServer = 1;
      }
    }
  }
}


void __cdecl CL_AddDebugStarWithText(const float *point, const float *starColor, const float *textColor, const char *string, float fontsize, int duration, qboolean fromServer)
{
  vec3_t lineEnd;
  vec3_t lineStart;
  float starsize;

  starsize = 5.0 * fontsize;
	VectorCopy(point, lineStart);
	VectorCopy(point, lineEnd);

  lineStart[0] = lineStart[0] + (float)(5.0 * fontsize);
  lineEnd[0] = lineEnd[0] - (float)(5.0 * fontsize);
  CL_AddDebugLine(lineStart, lineEnd, starColor, 0, duration, fromServer);
  lineStart[0] = lineStart[0] - starsize;
  lineEnd[0] = lineEnd[0] + starsize;
  lineStart[1] = lineStart[1] + starsize;
  lineEnd[1] = lineEnd[1] - starsize;
  CL_AddDebugLine(lineStart, lineEnd, starColor, 0, duration, fromServer);
  lineStart[1] = lineStart[1] - starsize;
  lineEnd[1] = lineEnd[1] + starsize;
  lineStart[2] = lineStart[2] + starsize;
  lineEnd[2] = lineEnd[2] - starsize;
  CL_AddDebugLine(lineStart, lineEnd, starColor, 0, duration, fromServer);
  if ( string && *string )
  {
    CL_AddDebugString(point, textColor, fontsize, string, fromServer, duration);
  }
}

