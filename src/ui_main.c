#include "q_shared.h"
#include "qcommon.h"
#include "ui_shared.h"
#include "client.h"
#include "snd_system.h"
#include "r_shared.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ui/ui_menus.h"

cvar_t	*ui_debug;

void Sys_InstallCoD4X_f();

uiMenuCommand_t UI_GetActiveMenu()
{
	return uiMem.uiInfo.currentMenuType;
}

void UI_SetActiveMenuByName(const char* menuname)
{
	Key_SetCatcher(16);
    Menus_CloseAll(&uiMem.uiInfo.uiDC);
    const char* errmsg = Cvar_GetString("com_errorMessage");
    if ( errmsg[0] && Q_stricmp(errmsg, ";") )
    {
        Menus_OpenByName("error_popmenu");
    }else{
		Menus_OpenByName(menuname);
    }
}

void UI_OpenMenuOnDvar(const char *cmd, const char *menuName, const char *varName, char *cmpValue)
{
  if ( Dvar_ValueEqualStringToString(varName, cmd, cmpValue, "openMenuOnDvar") )
  {
	Menus_OpenByName(menuName);
  }
}

void UI_CloseMenuOnDvar(const char *cmd, const char *menuName, const char *varName, char *cmpValue)
{
  if ( Dvar_ValueEqualStringToString(varName, cmd, cmpValue, "closeMenuOnDvar") )
  {
	Menus_CloseByName(menuName);
  }
}

qboolean UI_GetOpenOrCloseMenuOnDvarArgs(char **args, const char *menuName, char *dvarbuf, int dvarlen, char *testbuf, int testlen, char *menubuf, int menulen)
{
  if ( !String_Parse(args, dvarbuf, dvarlen) )
  {
	Com_Printf(CON_CHANNEL_UI, "%s: invalid dvar name.\n", menuName);
	return qfalse;
  }

  if ( !String_Parse(args, testbuf, testlen) )
  {
    Com_Printf(CON_CHANNEL_UI, "%s: invalid test value.\n", menuName);
    return qfalse;
  }

  if ( !String_Parse(args, menubuf, menulen) )
  {
    Com_Printf(CON_CHANNEL_UI, "%s: invalid menu name.\n", menuName);
    return qfalse;
  }
  return qtrue;
}

menuDef_t* Menus_FindName(UiContext_t *dc, const char* menuname)
{
	int i;

	for (i=0; i < dc->menuCount; ++i)
	{
		if(Q_stricmp(dc->Menus[i]->window.name, menuname) == 0)
		{
			return dc->Menus[i];
		}
	}
	return NULL;
}

void Window_AddDynamicFlags(int localClientNum, windowDef_t *w, const int newFlags)
{
	w->dynamicFlags[localClientNum] |= newFlags;
}

void Menu_Open(menuDef_t *menu)
{
  		Menus_Open(&uiMem.uiInfo.uiDC, menu);
}

void Menus_OpenByName(const char* menuStr)
{
	menuDef_t *menu;
	menu = Menus_FindName(&uiMem.uiInfo.uiDC, menuStr);
	if ( menu )
	{
		Menus_Open(&uiMem.uiInfo.uiDC, menu);
		return;
	}
	Com_PrintWarning(CON_CHANNEL_UI, "Could not find menu '%s'\n", menuStr);
}

void Menu_Close(menuDef_t *menu)
{
	if ( menu )
		Menus_Close(&uiMem.uiInfo.uiDC, menu);
}

void Menus_CloseByName(const char *menuStr)
{
	menuDef_t *menu;

	menu = Menus_FindName(&uiMem.uiInfo.uiDC, menuStr);
	if ( menu )
		Menus_Close(&uiMem.uiInfo.uiDC, menu);
}

void Menus_ShowByName(UiContext_t *dc, const char *windowName)
{
	menuDef_t* menu = Menus_FindName(dc, windowName);
	if(menu)
	{
		Window_AddDynamicFlags(dc->localClientNum, &menu->window, 4);
	}
}

void UI_RunMenuScript(int localClientNum, char **args, const char* actualScript)
{
  signed int profileNum;
  int sortColumn;
  int serverIndex;
  int enablePB;
  char addr[32];
  char name[32];
  char parseStr[MAX_STRING_TOKENS];
  char newfsgame[260];
  char infostring[MAX_INFO_STRING];
  char cdkey[17];
  char cdkeychecksum[5];
  char cdkeyblock[5];
  char dvarname[128];
  char testvalue[256];
  char menuname[128];

  if ( !String_Parse(args, parseStr, sizeof(parseStr)) )
    return;

  localClientNum = 0;

  if ( !Q_stricmp(parseStr, "StartServer") )
  {
    if ( !uiMem.ui_dedicated->boolean )
    {
      if ( Cvar_GetBool("sv_punkbuster") )
      {
        if ( !Cvar_GetBool("cl_punkbuster") )
        {
			Menus_OpenByName("startpb_popmenu");
			return;
        }
      }
    }
    Cvar_SetBoolByName("cg_thirdPerson", 0);
	Com_sprintf(testvalue, sizeof(testvalue),"%i", uiMem.ui_dedicated->integer);
    Cvar_SetFromStringByNameFromSource("dedicated", testvalue, 1);
    Cvar_SetStringByName("g_gametype", uiMem.gameTypes[ uiMem.ui_netGametype->integer ].gameType);

	if(uiMem.ui_currentNetMap->integer < 0 || uiMem.ui_currentNetMap->integer >= MAX_MAPS)
	{
		Cvar_SetInt(uiMem.ui_currentNetMap, 0);
	}
	Com_sprintf( testvalue, sizeof(testvalue) ,"wait ; wait ; map %s\n", uiMem.mapInfo[ uiMem.ui_currentNetMap->integer ].mapName );
    Cbuf_AddText( testvalue );
    return;
  }
  if ( !Q_stricmp(parseStr, "getCDKey") )
  {
    CL_GetCDKey(cdkey, cdkeychecksum, qtrue);
    Cvar_SetStringByName("cdkey1", "");
    Cvar_SetStringByName("cdkey2", "");
    Cvar_SetStringByName("cdkey3", "");
    Cvar_SetStringByName("cdkey4", "");
    Cvar_SetStringByName("cdkey5", "");

    if ( strlen(cdkey) == 16 )
    {
      strncpy(cdkeyblock, &cdkey[0], 4);
      cdkeyblock[4] = 0;
      Cvar_SetStringByName("cdkey1", cdkeyblock);

      strncpy(cdkeyblock, &cdkey[4], 4);
      cdkeyblock[4] = 0;
      Cvar_SetStringByName("cdkey2", cdkeyblock);

	  strncpy(cdkeyblock, &cdkey[8], 4);
      cdkeyblock[4] = 0;
      Cvar_SetStringByName("cdkey3", cdkeyblock);

	  strncpy(cdkeyblock, &cdkey[12], 4);
      cdkeyblock[4] = 0;
      Cvar_SetStringByName("cdkey4", cdkeyblock);
    }

    if ( strlen(cdkeychecksum) == 4 )
    {
      strncpy(cdkeyblock, cdkeychecksum, 4);
      cdkeyblock[4] = 0;
      Cvar_SetStringByName("cdkey5", cdkeyblock);
    }
    return;
  }
  if ( !Q_stricmp(parseStr, "verifyCDKey") )
  {
    cdkey[0] = 0;
    Q_strcat(cdkey, sizeof(cdkey), Cvar_GetVariantString("cdkey1"));
    Q_strcat(cdkey, sizeof(cdkey), Cvar_GetVariantString("cdkey2"));
    Q_strcat(cdkey, sizeof(cdkey), Cvar_GetVariantString("cdkey3"));
    Q_strcat(cdkey, sizeof(cdkey), Cvar_GetVariantString("cdkey4"));

    Q_strncpyz(cdkeychecksum, Cvar_GetVariantString("cdkey5"), sizeof(cdkeychecksum));

    if ( CL_CDKeyValidate(cdkey, cdkeychecksum) )
    {
      Cvar_SetStringByName("ui_cdkeyvalid", UI_SafeTranslateString("EXE_CDKEYVALID"));
      CL_SetCDKey(cdkey, cdkeychecksum);
    }
    else
    {
      Cvar_SetStringByName("ui_cdkeyvalid", UI_SafeTranslateString("EXE_CDKEYINVALID"));
    }
    return;
  }
  if ( !Q_stricmp(parseStr, "loadArenas") )
  {
    UI_LoadArenas();
    sub_546E60();
    sub_547010();
    Menu_SetFeederSelection(&uiMem.uiInfo, NULL, 4, 0, "createserver_maps");
    UI_SelectCurrentMap();
    return;
  }
  if ( !Q_stricmp(parseStr, "loadGameInfo" ) )
  {
    UI_LoadGameInfo();
    return;
  }
  if ( !Q_stricmp(parseStr, "LoadMods") )
  {
    UI_LoadMods();
    return;
  }
  if ( !Q_stricmp(parseStr, "voteTypeMap") )
  {
    Cbuf_AddText( va( "callvote typemap %s %s\n", uiMem.gameTypes[ uiMem.ui_netGametype->integer ].gameType, uiMem.mapInfo[ uiMem.ui_currentNetMap->integer ].mapName));
    return;
  }
  if ( !Q_stricmp(parseStr, "voteMap" ) )
  {
    if ( uiMem.ui_currentNetMap->integer < 0 || uiMem.ui_currentNetMap->integer >= uiMem.mapCount )
      return;

    Cbuf_AddText( va("callvote map %s\n", uiMem.mapInfo[uiMem.ui_currentNetMap->integer].mapName));
    return;
  }
  if ( !Q_stricmp(parseStr, "voteGame") )
  {
	Cbuf_AddText( va("callvote g_gametype %s\n", uiMem.gameTypes[uiMem.ui_netGametype->integer].gameType));
    return;
  }
  if ( !Q_stricmp(parseStr, "clearError") )
  {
    Cvar_SetStringByName("com_errorMessage", "");
    Cvar_SetBoolByName("com_isNotice", 0);
    if ( clientUIActives.state > 0 )
      Key_RemoveCatcher(-17);
    return;
  }
  if ( !Q_stricmp(parseStr, "RefreshServers") )
  {
    UI_StartServerRefresh(localClientNum, qtrue);
    UI_BuildServerDisplayList(&uiMem.uiInfo, 1);
    return;
  }
  if ( !Q_stricmp(parseStr, "RefreshFilter" ) )
  {
    UI_StartServerRefresh(localClientNum, qfalse);
    UI_BuildServerDisplayList(&uiMem.uiInfo, 1);
    return;
  }
  if ( !Q_stricmp(parseStr, "addPlayerProfiles") )
  {
    UI_AddPlayerProfile();
    return;
  }
  if ( !Q_stricmp(parseStr, "sortPlayerProfiles" ) )
  {
    uiMem.uiInfo.playerProfilesSorted = uiMem.uiInfo.playerProfilesSorted == 0;
    Com_SortPlayerProfiles(0);
    return;
  }
  if ( !Q_stricmp(parseStr, "selectActivePlayerProfile" ) )
  {
    profileNum = Com_GetPlayerProfile(com_playerProfile->string);
    if ( profileNum >= 0 && profileNum < uiMem.uiInfo.numPlayerProfiles )
      Com_SelectPlayerProfile(profileNum);
    return;
  }
  if ( !Q_stricmp(parseStr, "createPlayerProfile" ) )
  {
    Com_CreatePlayerProfile();
    return;
  }
  if ( !Q_stricmp(parseStr, "deletePlayerProfile" ) )
  {
    UI_DeletePlayerProfile();
    return;
  }
  if ( !Q_stricmp(parseStr, "loadPlayerProfile" ) )
  {
    if ( uiMem.ui_playerProfileSelected->string[0] )
      Com_LoadPlayerProfile( uiMem.ui_playerProfileSelected->string );
    return;
  }
  if ( !Q_stricmp(parseStr, "RunMod" ) )
  {
    if ( uiMem.modInfo[uiMem.modIndex].modName )
    {
      Com_sprintf(newfsgame, sizeof(newfsgame), "%s/%s", "mods", uiMem.modInfo[uiMem.modIndex].modName);
      if ( com_useFastFiles->boolean )
        DB_SyncXAssets();
      Cvar_SetStringByName("fs_game", newfsgame);
      Cbuf_AddText( "vid_restart\n");
    }
    return;
  }
  if ( !Q_stricmp(parseStr, "ClearMods" ) )
  {
    if ( com_useFastFiles->boolean )
		DB_SyncXAssets();

	Cvar_ResetByName("fs_game");
	Cbuf_AddText( "vid_restart\n");
    return;
  }
  if ( !Q_stricmp(parseStr, "closeJoin") )
  {
    if ( uiMem.serverStatus.refreshActive )
    {
      UI_StopServerRefresh();
      uiMem.serverStatus.nextDisplayRefresh = 0;
      uiMem.nextServerStatusRefresh = 0;
      uiMem.nextFindPlayerRefresh = 0;
      UI_BuildServerDisplayList(&uiMem.uiInfo, 1);
      return;
    }
    Menus_CloseByName("joinserver");
    Menus_OpenByName( "main");
	return;
  }
  if ( !Q_stricmp(parseStr, "StopRefresh") )
  {
    UI_StopServerRefresh();
    uiMem.serverStatus.nextDisplayRefresh = 0;
    uiMem.nextServerStatusRefresh = 0;
    uiMem.nextFindPlayerRefresh = 0;
    return;
  }
  if ( !Q_stricmp(parseStr, "ServerStatus") )
  {
    UI_BuildServerDisplayListByNetSource(&uiMem.uiInfo);
    if ( uiMem.serverStatus.currentServer >= 0 && uiMem.serverStatus.currentServer < uiMem.serverStatus.numDisplayServers )
    {
      CLUI_GetServerAddressString(uiMem.ui_netSource->integer, uiMem.serverStatus.displayServers[uiMem.serverStatus.currentServer], uiMem.serverStatusAddress, sizeof(uiMem.serverStatusAddress));
      UI_BuildServerStatus(&uiMem.uiInfo, 1);
    }
    return;
  }
  if ( !Q_stricmp(parseStr, "UpdateFilter") )
  {
    if ( uiMem.ui_netSource->integer )
      UI_StartServerRefresh(localClientNum, qtrue);
    UI_BuildServerDisplayList( &uiMem.uiInfo, 1);
    UI_FeederSelection(2.0, 0);
    return;
  }
  if ( !Q_stricmp(parseStr, "JoinServer") )
  {
	Cvar_SetBoolByName("cg_thirdPerson", 0);
	UI_BuildServerDisplayListByNetSource(&uiMem.uiInfo);
	if ( CLUI_GetServerPunkBuster( uiMem.serverStatus.displayServers[uiMem.serverStatus.currentServer], uiMem.ui_netSource->integer) == 1 )
	{
		if ( !Cvar_GetBool("cl_punkbuster") )
		{
			Menus_OpenByName("joinpb_popmenu");
			return;
		}

	}
	serverIndex = uiMem.serverStatus.currentServer;
	if ( serverIndex >= 0 && serverIndex < uiMem.serverStatus.numDisplayServers )
	{
		CLUI_GetServerAddressString( uiMem.ui_netSource->integer, uiMem.serverStatus.displayServers[serverIndex], infostring, sizeof(infostring));
		Cbuf_AddText( va("connect %s\n", infostring));
	}
	return;
  }
  if ( !Q_stricmp(parseStr, "Quit") )
  {
	Cmd_ExecuteSingleCommand(localClientNum, localClientNum, "quit");
    return;
  }
  if ( !Q_stricmp(parseStr, "Controls") )
  {
    Cvar_SetIntByName("cl_paused", 1);
    Key_SetCatcher(0x10);
    Menus_CloseAll(&uiMem.uiInfo.uiDC);
    Menus_OpenByName("setup_menu2");
	return;
  }
  if ( !Q_stricmp(parseStr, "Leave") )
  {
    Cbuf_AddText( "disconnect\n");
    Key_SetCatcher(0x10);
    Menus_CloseAll(&uiMem.uiInfo.uiDC);
    Menus_OpenByName("main");
    return;
  }
  if ( !Q_stricmp(parseStr, "ServerSort" ) )
  {
    if ( Int_Parse(args, &sortColumn) )
    {
        if ( sortColumn == uiMem.serverStatus.sortKey )
          uiMem.serverStatus.sortDir = uiMem.serverStatus.sortDir == 0;
        uiMem.serverStatus.sortKey = sortColumn;
        qsort(uiMem.serverStatus.displayServers, uiMem.serverStatus.numDisplayServers, sizeof(uiMem.serverStatus.displayServers[0]), UI_ServersQsortCompare);
    }
    return;
  }
  if ( !Q_stricmp(parseStr, "closeingame") )
  {
    Key_RemoveCatcher(-17);
    Key_ClearStates(localClientNum);
    Cvar_SetIntByName("cl_paused", 0);
    Menus_CloseAll(&uiMem.uiInfo.uiDC);
    return;
  }
  if ( !Q_stricmp(parseStr, "voteKick") )
  {
    if ( uiMem.uiInfo.playerIndex < 0 || uiMem.uiInfo.playerIndex >= uiMem.playerCount )
        return;

    Cbuf_AddText( va("callvote kick \"%s\"\n", uiMem.playerNames[uiMem.uiInfo.playerIndex]));
    return;
  }
  if ( !Q_stricmp(parseStr, "voteTempBan") )
  {
    if ( uiMem.uiInfo.playerIndex < 0 || uiMem.uiInfo.playerIndex >= uiMem.playerCount )
        return;

    Cbuf_AddText( va("callvote tempBanUser \"%s\"\n", uiMem.playerNames[uiMem.uiInfo.playerIndex]));
    return;
  }
  if ( !Q_stricmp(parseStr, "addFavorite") )
  {
	if ( uiMem.ui_netSource->integer != AS_FAVORITES  )
    {
        addr[0] = 0;
        name[0] = 0;
        UI_BuildServerDisplayListByNetSource(&uiMem.uiInfo);
        if ( uiMem.serverStatus.currentServer >= 0 && uiMem.serverStatus.currentServer < uiMem.serverStatus.numDisplayServers )
        {
            LAN_GetServerInfo( uiMem.ui_netSource->integer, uiMem.serverStatus.displayServers[uiMem.serverStatus.currentServer], infostring, sizeof(infostring));
            Q_strncpyz(name, Info_ValueForKey(infostring, "hostname"), sizeof(name));
            Q_strncpyz(addr, Info_ValueForKey(infostring, "addr"), sizeof(addr));
        }
        UI_AddServerToFavorites(name, addr);
    }
    return;
  }
  if ( !Q_stricmp(parseStr, "deleteFavorite") )
  {
    if ( uiMem.ui_netSource->integer == AS_FAVORITES )
    {
        if ( uiMem.serverStatus.currentServer >= 0 && uiMem.serverStatus.currentServer < uiMem.serverStatus.numDisplayServers )
        {
            UI_BuildServerDisplayListByNetSource(&uiMem.uiInfo);
            LAN_GetServerInfo( uiMem.ui_netSource->integer, uiMem.serverStatus.displayServers[uiMem.serverStatus.currentServer], infostring, sizeof(infostring));
            addr[0] = 0;
            Q_strncpyz(addr, Info_ValueForKey(infostring, "addr"), sizeof(addr));
			if(addr[0] != '\0')
				CLUI_RemoveServer(addr);
        }
    }
    return;
  }
  if ( !Q_stricmp(parseStr, "createFavorite") )
  {
    if ( uiMem.ui_netSource->integer == AS_FAVORITES )
    {
        name[0] = 0;
        addr[0] = 0;
        Q_strncpyz(addr, "default", sizeof(addr));
        Q_strncpyz(name, Cvar_GetVariantString("ui_favoriteAddress"), sizeof(name));
        UI_AddServerToFavorites(addr, name);
        UI_StartServerRefresh(localClientNum, qtrue);
    }
    return;
  }
  if ( !Q_stricmp(parseStr, "update") )
  {
    if ( String_Parse(args, infostring, sizeof(infostring)) )
		UI_Update(infostring);
    return;
  }
  if ( !Q_stricmp(parseStr, "setPbClStatus") )
  {
	if ( Int_Parse(args, &enablePB) )
	{
		CLUI_SetPbClStatus(enablePB);
	}
	return;
  }
  if ( !Q_stricmp(parseStr, "startSingleplayer") )
  {
    Cbuf_AddText( "startSingleplayer\n");
    return;
  }
  if ( !Q_stricmp(parseStr, "getLanguage" ) )
  {
      Cvar_SetIntByName("ui_language", Cvar_GetInt("loc_language"));
      UI_VerifyLanguage();
      return;
  }
  if ( !Q_stricmp(parseStr, "verifyLanguage") )
  {
	UI_VerifyLanguage();
	return;
  }
  if ( !Q_stricmp(parseStr, "updateLanguage") )
  {
    Cvar_SetIntByName("loc_language", Cvar_GetInt("ui_language"));
    UI_VerifyLanguage();
    Cbuf_AddText( "vid_restart\n");
    return;
  }
  if ( !Q_stricmp(parseStr, "mutePlayer") )
  {
    if ( uiMem.uiInfo.playerIndex >= 0 && uiMem.uiInfo.playerIndex < uiMem.playerCount )
        CL_MutePlayer(uiMem.uiClients[uiMem.uiInfo.playerIndex]);
    return;
  }
  if ( !Q_stricmp(parseStr, "openMenuOnDvar") || !Q_stricmp(parseStr, "openMenuOnDvarNot") )
  {
    if ( UI_GetOpenOrCloseMenuOnDvarArgs(args, parseStr, dvarname, sizeof(dvarname), testvalue, sizeof(testvalue), menuname, sizeof(menuname)) )
      UI_OpenMenuOnDvar(parseStr, menuname, dvarname, testvalue);
    return;
  }
  if ( !Q_stricmp(parseStr, "closeMenuOnDvar") || !Q_stricmp(parseStr, "closeMenuOnDvarNot") )
  {
    if ( UI_GetOpenOrCloseMenuOnDvarArgs(args, parseStr, dvarname, sizeof(dvarname), testvalue, sizeof(testvalue), menuname, sizeof(menuname)) )
        UI_CloseMenuOnDvar(parseStr, menuname, dvarname, testvalue);
    return;
  }
  if ( !Q_stricmp(parseStr, "setRecommended") )
  {
    Com_SetRecommended(localClientNum, 1);
    return;
  }
  if ( !Q_stricmp(parseStr, "clearLoadErrorsSummary") )
  {
    Menus_CloseAll(&uiMem.uiInfo.uiDC);
    return;
  }
  if ( !Q_stricmp(parseStr, "installcod4x") )
  {
    Menus_CloseAll(&uiMem.uiInfo.uiDC);
    Sys_InstallCoD4X_f();
	return;
  }
  Com_Printf(CON_CHANNEL_UI, "unknown UI script %s in script:\n%s\n", parseStr, actualScript);
}



void UI_StopServerRefresh()
{
	int numServers;
	int numFilterServers;

	if ( !uiMem.serverStatus.refreshActive )
		return;


    uiMem.serverStatus.refreshActive = qfalse;
    if(uiMem.ui_netSource->integer != AS_GLOBAL)
	{
		Com_Printf(CON_CHANNEL_UI,   "%d servers listed in browser with %d players and bots.\n", uiMem.serverStatus.numDisplayServers, uiMem.serverStatus.numPlayersOnServers);
	}else{
		Com_Printf(CON_CHANNEL_UI,   "%d servers listed in browser with %d players.\n", uiMem.serverStatus.numDisplayServers, uiMem.serverStatus.numPlayersOnServers);

	}
	numServers = LAN_GetServerCount(uiMem.ui_netSource->integer);

    numFilterServers = numServers - uiMem.serverStatus.numDisplayServers;
    if ( numFilterServers > 0 )
      Com_Printf(CON_CHANNEL_UI,  "%d servers not listed (filtered out by game browser settings)\n", numFilterServers);

}


void UI_BuildServerDisplayListByNetSource()
{
	int numServers = LAN_GetServerCount(uiMem.ui_netSource->integer);


	if ( uiMem.serverStatus.totalServers != numServers )
	{
		uiMem.serverStatus.totalServers = numServers;
		if ( uiMem.serverStatus.numDisplayServers )
		{
			uiMem.serverStatus.currentServer = -1;
			UI_BuildServerDisplayList(&uiMem.uiInfo, 1);
		}
	}
}


void UI_AddServerToFavorites(const char *hostname, const char *serveradrstring)
{

	if ( hostname[0] == '\0')
	{
		Com_Printf(CON_CHANNEL_UI,  "%s\n", UI_SafeTranslateString("EXE_FAVORITENAMEEMPTY"));
		Cvar_SetStringByName("ui_favorite_message", "@EXE_FAVORITENAMEEMPTY");
		return;
	}

    if ( serveradrstring[0] == '\0' )
    {
		Com_Printf(CON_CHANNEL_UI,  "%s\n", UI_SafeTranslateString("EXE_FAVORITEADDRESSEMPTY"));
		Cvar_SetStringByName("ui_favorite_message", "@EXE_FAVORITEADDRESSEMPTY");
		return;
	}

    switch(CLUI_AddServer(hostname, serveradrstring))
	{
		case 0:
		    Com_Printf(CON_CHANNEL_UI,  "%s\n", UI_SafeTranslateString("EXE_FAVORITEINLIST"));
			Cvar_SetStringByName("ui_favorite_message", "@EXE_FAVORITEINLIST");
			return;
		case -1:
			Com_Printf(CON_CHANNEL_UI,  "%s\n", UI_SafeTranslateString("EXE_FAVORITELISTFULL"));
			Cvar_SetStringByName("ui_favorite_message", "@EXE_FAVORITELISTFULL");
			return;
		case -2:
			Com_Printf(CON_CHANNEL_UI,   "%s\n", UI_SafeTranslateString("EXE_BADSERVERADDRESS"));
            Cvar_SetStringByName("ui_favorite_message", "@EXE_BADSERVERADDRESS");
			return;
		default:
			Com_Printf(CON_CHANNEL_UI,  "%s\n", UI_SafeTranslateString("EXE_FAVORITEADDED"));
            Cvar_SetStringByName("ui_favorite_message", "@EXE_FAVORITEADDED");

	}
}


void UI_Update(const char *name)
{
  float rate;

  if ( !Q_stricmp(name, "ui_SetName") )
  {
    Cvar_SetStringByName("name", Cvar_GetVariantString("ui_Name"));
    return;
  }
  if ( !Q_stricmp(name, "ui_GetName") )
  {
    Cvar_SetStringByName("ui_Name", Cvar_GetVariantString("name"));
    return;
  }
  if ( !Q_stricmp(name, "ui_setRate") )
  {
	rate = Cvar_GetInt( "rate" );
	if ( rate >= 5000 ) {
		Cvar_SetIntByName( "cl_maxpackets", 30 );
		Cvar_SetIntByName( "cl_packetdup", 1 );
	} else if ( rate >= 4000 ) {
		Cvar_SetIntByName( "cl_maxpackets", 15 );
		Cvar_SetIntByName( "cl_packetdup", 2 ); // favor less prediction errors when there's packet loss
	} else {
		Cvar_SetIntByName( "cl_maxpackets", 15 );
		Cvar_SetIntByName( "cl_packetdup", 1 ); // favor lower bandwidth
	}
  }
  if ( !Q_stricmp(name, "ui_mousePitch") )
  {
    if ( Cvar_GetBool(name) == 0)
		Cvar_SetFloatByName("m_pitch", 0.022f);
    else
		Cvar_SetFloatByName("m_pitch", -0.022f);
  }
}


#define commandDefHCast void (__cdecl *)(void *uictx, itemDef_t *item, char **args)


commandDef_t commandList[] =
{
	{"fadein", (commandDefHCast)0x54c610},
	{"fadeout", (commandDefHCast)0x54c670},
	{"show", (commandDefHCast)0x54c570},
	{"hide", (commandDefHCast)0x54c5c0},
	{"showMenu", (commandDefHCast)0x54c6d0},
	{"hideMenu", (commandDefHCast)0x54c720},
	{"setcolor", (commandDefHCast)0x54bdd0},
	{"open", (commandDefHCast)0x54c780},
	{"close", (commandDefHCast)0x54d0f0},
	{"ingameopen", (commandDefHCast)0x54d180},
	{"ingameclose", (commandDefHCast)0x54d1d0},
	{"setbackground", (commandDefHCast)0x54bec0},
	{"setitemcolor", (commandDefHCast)0x54bfa0},
	{"focusfirst", (commandDefHCast)0x54d2d0},
	{"setfocus", (commandDefHCast)0x54d370},
	{"setfocusbydvar", (commandDefHCast)0x54d440},
	{"setdvar", (commandDefHCast)0x54d510},
	{"exec", (commandDefHCast)0x54d5e0},
	{"execnow", (commandDefHCast)0x54d600},
	{"execOnDvarStringValue", (commandDefHCast)0x54d8c0},
	{"execOnDvarIntValue", (commandDefHCast)0x54d8e0},
	{"execOnDvarFloatValue", (commandDefHCast)0x54d900},
	{"execNowOnDvarStringValue", (commandDefHCast)0x54d920},
	{"execNowOnDvarIntValue", (commandDefHCast)0x54d940},
	{"execNowOnDvarFloatValue", (commandDefHCast)0x54d960},
	{"play", (commandDefHCast)0x54dd20},
	{"scriptmenuresponse", (commandDefHCast)0x54dd90},
	{"scriptMenuRespondOnDvarStringValue", (commandDefHCast)0x54d980},
	{"scriptMenuRespondOnDvarIntValue", (commandDefHCast)0x54d9a0},
	{"scriptMenuRespondOnDvarFloatValue", (commandDefHCast)0x54d9c0},
	{"setLocalVarBool", (commandDefHCast)0x54d9e0},
	{"setLocalVarInt", (commandDefHCast)0x54da70},
	{"setLocalVarFloat", (commandDefHCast)0x54db00},
	{"setLocalVarString", (commandDefHCast)0x54db90},
	{"feederTop", (commandDefHCast)0x54dc10},
	{"feederBottom", (commandDefHCast)0x54dc90},
	{"openforgametype", (commandDefHCast)0x54c7f0},
	{"closeforgametype", (commandDefHCast)0x54c8a0},
	{"statclearperknew", (commandDefHCast)0x54cee0},
	{"statsetusingtable", (commandDefHCast)0x54c940},
	{"statclearbitmask", (commandDefHCast)0x54d060},
	{"getautoupdate", (commandDefHCast)CL_GetAutoUpdate}
};


int scriptCommandCount = sizeof( commandList ) / sizeof( commandDef_t );


void Item_RunScript( UiContext_t *dc, itemDef_t *item, const char *s )
{
	char script[5120], *p;
	char command[1024];
	int i;


	Com_Memset( script, 0, sizeof( script ) );

	if ( !item || !s || !s[0] ) {
		return;
	}

	Q_strcat( script, sizeof(script), s );
	p = script;

	// expect command then arguments, ; ends command, NULL ends script
	while ( String_Parse(&p, command, sizeof(command) ) ) {

		if ( command[0] == ';' && command[1] == '\0' ) {
			continue;
		}

		if(ui_debug->boolean)
		{
			Com_Printf(CON_CHANNEL_UI,  "^2Exec menu: %s\n", command);
		}


		for ( i = 0; i < scriptCommandCount; i++ ) {

			if ( Q_stricmp( command, commandList[i].name ) == 0 )
			{
				commandList[i].handler(dc, item, &p);
				break;
			}

		}
		// not in our auto list, pass to handler
		if ( i == scriptCommandCount ) {
			UI_RunMenuScript(dc->localClientNum, &p, s);
		}
	}
}

void UI_DrawBuildString( )
{
  int ctx;
  float size;
  Font_t *font;
  float fontscale;
  float ypos;
  float xpos;
  float advwidth;
  float buildstringwidth;
  char buildString[128];
  const char *advstring = "Visit the official CoD4X website: www.cod4x.me";

  if(uiMem.uiInfo.uiDC.openMenuCount > 2)
  {
	  return;
  }
  if(uiMem.uiInfo.uiDC.openMenuCount == 2 && strcmp(uiMem.uiInfo.uiDC.menuStack[1]->window.name, "main_text") != 0)
  {
	  return;
  }

  ctx = 0;
  size = scrPlaceView[ctx].scaleVirtualToReal[1] * uiMem.ui_buildSize->floatval;

  if ( uiMem.ui_smallFont->floatval >= size ){
	font = uiMem.extraSmallFont;
  }else if ( uiMem.ui_extraBigFont->floatval <= size ){
    font = uiMem.extraBigFont;
  }else if ( uiMem.ui_bigFont->floatval > size ){
	font = uiMem.smallFont;
  }else{
    font = uiMem.bigFont;
  }

  fontscale = uiMem.ui_buildSize->floatval;

  ypos = uiMem.ui_buildLocation->vec2[1];
  xpos = uiMem.ui_buildLocation->vec2[0];
  ypos -= 5.0;
#ifdef BETA_RELEASE
  xpos -= 20.0;
  Com_sprintf(buildString, sizeof(buildString), "%s Beta", UPDATE_VERSION);
#else
  Com_sprintf(buildString, sizeof(buildString), "%s", UPDATE_VERSION);
#endif
  UI_DrawText(&scrPlaceView[ctx], buildString, 64, font, xpos, ypos, 3, 0, fontscale, colorMdGrey, 0);

  ypos += UI_TextHeight(font, fontscale);

  advwidth = (float)UI_TextWidth(advstring, 0, font, fontscale);
  buildstringwidth = (float)UI_TextWidth(buildString, 0, font, fontscale);

  UI_DrawText(&scrPlaceView[ctx], advstring, 64, font, xpos + buildstringwidth - advwidth, ypos, 3, 0, fontscale, colorLtGrey, 0);

  ypos += UI_TextHeight(font, fontscale) - 5;

  UI_DrawText(&scrPlaceView[ctx], "cod4x.me(Call of Duty 4 X) is not affiliated with Activision Publishing", 74, font, xpos + buildstringwidth - advwidth, ypos, 3, 0, fontscale * 0.75, colorLtGrey, 0);

}


int UI_PlayerProfilesQsortCompare(const void *v1, const void *v2)
{
  int result;
  const int *cmp1 = v1;
  const int *cmp2 = v2;

  if ( *cmp1 == *cmp2 )
  {
    return 0;
  }

  result = Q_stricmp(uiMem.uiInfo.profilenames[*cmp1], uiMem.uiInfo.profilenames[*cmp2]);

  if ( !uiMem.uiInfo.playerProfilesSorted )
  {
    result = -result;
  }
  return result;
}


void CL_SortPlayerProfiles(int a1)
{

	int i;

	if ( !uiMem.uiInfo.numPlayerProfiles )
	{
		return;
	}
    for(i = 0; i < uiMem.uiInfo.numPlayerProfiles; i++)
	{
        uiMem.uiInfo.sortedProfiles[i] = i;
    }
    qsort(uiMem.uiInfo.sortedProfiles, uiMem.uiInfo.numPlayerProfiles, 4u, UI_PlayerProfilesQsortCompare);
    CL_SelectPlayerProfile(a1);

}


void __cdecl UI_AddPlayerProfile()
{

  int i;
  //qboolean last;
  uiMem.uiInfo.numPlayerProfiles = 0;
  uiMem.uiInfo.playerProfilesSorted = 1;

  char* dirlist[1024];

  FS_SV_ListDirectories("players/profiles", dirlist, 1024);
  //last = qfalse;
  for ( i = 0; dirlist[i] && i < 64; i++ )
  {
    uiMem.uiInfo.profilenames[i] = String_Alloc(dirlist[i]);
    if(uiMem.uiInfo.profilenames[i] == NULL)
    {
      break;
    }
  }

  uiMem.uiInfo.numPlayerProfiles = i;

  for ( i = 0; dirlist[i]; i++ )
  {
	  free(dirlist[i]);
  }

  CL_SortPlayerProfiles(0);
  Cvar_SetInt(uiMem.ui_playerProfileCount, uiMem.uiInfo.numPlayerProfiles);
}


#if 0

#define MAX_SOURCEFILES 64

int PC_ReadTokenHandleA(int handle, pc_token_t* tok);
int Asset_Parse(int handle);
byte Menu_New(int handle, int imgTrack);
void PC_SourceError(int, const char* fmt,...);

extern source_t** sourceFiles;

int PC_LoadSourceHandleMemory(char* filename, char* ptr, int length)
{
	source_t *source;
	int i;

	for( i = 1; i < MAX_SOURCEFILES; i++)
	{
		if(!sourceFiles[i])
		{
			break;
		}
	}
	if( i >= MAX_SOURCEFILES)
	{
		return 0;
	}
	source = LoadSourceMemory(ptr, length ,filename);
	if(!source)
	{
		return 0;
	}
//	PC_AddDefine(source ,"PC");
	sourceFiles[i] = source;
	return i;
}

int PC_LoadSourceHandleA(char* menuname, char **pc);


qboolean UI_ParseMenuOfMemory(char *menuname, char* buf, int len)
{

	int handle;
	pc_token_t token;
	int a2 = 0;
	char *pc[] = {"PC", NULL};

	Com_Printf(CON_CHANNEL_UI,  "Loading '%s'...\n", menuname);
	handle = PC_LoadSourceHandleA(menuname, pc);
	//handle = PC_LoadSourceHandleMemory(menuname, buf, len);
	if ( !handle )
	{
		Com_PrintError("Couldn't find menu file '%s'\n", menuname);
		return 0;
	}

    while(PC_ReadTokenHandleA(handle, &token))
	{

        if ( token.string[0] == '}' || token.string[0] == '{' )
        {
			continue;
		}

        if ( !Q_stricmp(token.string, "assetGlobalDef") )
        {
		    if ( !Asset_Parse( handle ) )
			{
				break;
			}
			continue;
		}

		if ( !Q_stricmp(token.string, "menudef") )
        {
			if ( !Menu_New(handle, a2) )
			{
                break;
			}
			continue;
        }
        PC_SourceError(handle, "Unknown token %s in menu file.  Expected \"menudef\" or \"assetglobaldef\".\n", token.string);

    }

  //  PC_FreeSourceHandle(handle);
    return 1;
}

#endif

//#ifdef COD4XDEV
	void MenuDumpMenuDef_t(menuDef_t* menu, char* asname, char* outdata, int outdatalen);
	int MenuDumpMenuDefBin_t(menuDef_t* menu, char* outdata, int outdatalen);
/*
	void UI_DumpMenus_f()
	{
		char dump[1024*4096];
		char savepath[MAX_OSPATH];
		int i;
		char* gamedir;

		gamedir = FS_GetGameDir();

		if(gamedir[0] == '\0')
		{
			gamedir = "main";
		}

		for (i = 0; i < uiMem.uiInfo.uiDC.menuCount; ++i)
		{
			Com_sprintf(savepath, sizeof(savepath), "dumps/%s/menus/%s.menu", gamedir, uiMem.uiInfo.uiDC.Menus[i]->window.name);
			int len = MenuDumpMenuDefBin_t(uiMem.uiInfo.uiDC.Menus[i], dump, sizeof(dump));
			FS_SV_WriteFileToSavePath( savepath, dump, len );

		}
		Com_Printf(CON_CHANNEL_UI,  "^2%d Menus dumped :D\n", i);
	}
*/

//#endif


void UI_DumpOpenMenuNames_f()
{
	int i;
	UiContext_t *dc = &uiMem.uiInfo.uiDC;

	for(i = 0; i < dc->openMenuCount; ++i)
	{
		Com_Printf(CON_CHANNEL_UI,  "%d: %s\n", i, dc->menuStack[i]->window.name);
	}
}

void UI_RegisterCvars()
{
  uiMem.ui_customModeName = Cvar_RegisterString("ui_customModeName", "", 0, "Custom game mode name");
  uiMem.ui_customModeEditName = Cvar_RegisterString("ui_customModeEditName", "", 0, "Name to give the currently edited custom game mode when editing is complete");
  uiMem.ui_customClassName = Cvar_RegisterString("ui_customClassName", "", 0, "Custom Class name");
  Cvar_RegisterBool("g_allowvote", qtrue, 1u, 0);
  Cvar_RegisterBool("cg_brass", qtrue, 1u, 0);
  Cvar_RegisterBool("fx_marks", qtrue, 1u, 0);
  Cvar_RegisterString("server1", "", 1u, "Server display");
  Cvar_RegisterString("server2", "", 1u, "Server display");
  Cvar_RegisterString("server3", "", 1u, "Server display");
  Cvar_RegisterString("server4", "", 1u, "Server display");
  Cvar_RegisterString("server5", "", 1u, "Server display");
  Cvar_RegisterString("server6", "", 1u, "Server display");
  Cvar_RegisterString("server7", "", 1u, "Server display");
  Cvar_RegisterString("server8", "", 1u, "Server display");
  Cvar_RegisterString("server9", "", 1u, "Server display");
  Cvar_RegisterString("server10", "", 1u, "Server display");
  Cvar_RegisterString("server11", "", 1u, "Server display");
  Cvar_RegisterString("server12", "", 1u, "Server display");
  Cvar_RegisterString("server13", "", 1u, "Server display");
  Cvar_RegisterString("server14", "", 1u, "Server display");
  Cvar_RegisterString("server15", "", 1u, "Server display");
  Cvar_RegisterString("server16", "", 1u, "Server display");
  uiMem.ui_netSource = Cvar_RegisterInt("ui_netSource", 1, 0, 3, 1u, "The network source where:\n  0:Local\n  1:Internet\n  2:Favourites\n  3:Internet ATVI");
  uiMem.ui_smallFont = Cvar_RegisterFloat("ui_smallFont", 0.25, 0.0, 1.0, 1u, "Small font scale");
  uiMem.ui_bigFont = Cvar_RegisterFloat("ui_bigFont", 0.40000001, 0.0, 1.0, 1u, "Big font scale");
  uiMem.ui_extraBigFont = Cvar_RegisterFloat("ui_extraBigFont", 0.55000001, 0.0, 1.0, 1u, "Extra big font scale");
  uiMem.ui_currentMap = Cvar_RegisterInt("ui_currentMap", 0, 0, 0x7FFFFFFF, 1u, "Current map index");
  uiMem.ui_gametype = Cvar_RegisterInt("ui_gametype", 3, 0, 0x7FFFFFFF, 1u, "Game type");
  uiMem.ui_joinGametype = Cvar_RegisterInt("ui_joinGametype", 0, 0, 31, 1u, "Game join type");
  uiMem.ui_netGametypeName = Cvar_RegisterString("ui_netGametypeName", "", 1u, "Displayed game type name");
  uiMem.ui_dedicated = Cvar_RegisterInt("ui_dedicated", 0, 0, 2, 1u, "True if this is a dedicated server");
  uiMem.ui_currentNetMap = Cvar_RegisterInt("ui_currentNetMap", 0, 0, 0x7FFFFFFF, 1u, "Currently running map");
  uiMem.ui_browserShowFull = Cvar_RegisterBool("ui_browserShowFull", qtrue, 1u, "Show full servers");
  uiMem.ui_browserShowEmpty = Cvar_RegisterBool("ui_browserShowEmpty", qtrue, 1u, "Show empty servers");
  uiMem.ui_browserShowPassword = Cvar_RegisterInt("ui_browserShowPassword", -1, -1, 1, 1u, "Show servers that are password protected");
  uiMem.ui_browserShowPure = Cvar_RegisterBool("ui_browserShowPure", qtrue, 1u, "Show pure servers only");
  uiMem.ui_browserMod = Cvar_RegisterInt("ui_browserMod", -1, -1, 1, 1u, "UI Mod value");
  uiMem.ui_browserShowDedicatedOnly = Cvar_RegisterBool("ui_browserShowDedicated", 0, 1u, "Show dedicated servers only");
  uiMem.ui_browserFriendlyfire = Cvar_RegisterInt("ui_browserFriendlyfire", -1, 0x80000000, 0x7FFFFFFF, 1u, "Friendly fire is active");
  uiMem.ui_browserKillcam = Cvar_RegisterInt("ui_browserKillcam", -1, 0x80000000, 0x7FFFFFFF, 1u, "Kill cam is active");
  uiMem.ui_browserHardcore = Cvar_RegisterInt("ui_browserHardcore", -1, 0x80000000, 0x7FFFFFFF, 1u, "Hardcore mode");
  uiMem.ui_browserOldSchool = Cvar_RegisterInt("ui_browserOldSchool", -1, 0x80000000, 0x7FFFFFFF, 1u, "Oldschool mode");
  uiMem.ui_serverStatusTimeOut = Cvar_RegisterInt("ui_serverStatusTimeOut", 7000, 0, 0x7FFFFFFF, 1u, "Time in milliseconds before a server status request times out");
  uiMem.ui_browserShowPunkBuster = Cvar_RegisterInt("ui_browserShowPunkBuster", -1, 0x80000000, 0x7FFFFFFF, 1u, "Only show PunkBuster servers?");
  uiMem.ui_playerProfileCount = Cvar_RegisterInt("ui_playerProfileCount", 0, 0x80000000, 0x7FFFFFFF, 0x40u, "Number of player profiles");
  uiMem.ui_playerProfileSelected = Cvar_RegisterString("ui_playerProfileSelected", "", 0x40u, "Selected player profile name");
  uiMem.ui_playerProfileNameNew = Cvar_RegisterString("ui_playerProfileNameNew", "", 0, "New player profile name");
  uiMem.ui_buildLocation = Cvar_RegisterVec2("ui_buildLocation", -100.0, 52.0, -10000.0, 10000.0, 0, "Where to draw the build number");
  uiMem.ui_buildSize = Cvar_RegisterFloat("ui_buildSize", 0.30000001, 0.0, 1.0, 0, "Font size to use for the build number");
  uiMem.ui_showList = Cvar_RegisterBool("ui_showList", 0, 0x80u, "Show onscreen list of currently visible menus");
  uiMem.ui_showMenuOnly = Cvar_RegisterString("ui_showMenuOnly", "", 0, "If set, only menus using this name will draw.");
  uiMem.ui_showEndOfGame = Cvar_RegisterBool("ui_showEndOfGame", 0, 0, "Currently showing the end of game menu.");
  uiMem.ui_borderLowLightScale = Cvar_RegisterFloat("ui_borderLowLightScale", 0.60000002, 0.0, 1.0, 0, "Scales the border color for the lowlight color on certain UI borders");
  uiMem.ui_cinematicsTimestamp = Cvar_RegisterBool("ui_cinematicsTimestamp", 0, 0, "Shows cinematics timestamp on subtitle UI elements.");
  uiMem.ui_connectScreenTextGlowColor = Cvar_RegisterVec4("ui_connectScreenTextGlowColor", 0.30000001, 0.60000002, 0.30000001, 1.0, 0.0, 1.0, 0, "Glow color applied to the mode and map name strings on the connect screen.");
  uiMem.ui_drawCrosshair = Cvar_RegisterBool("ui_drawCrosshair", qtrue, 1u, "Whether to draw crosshairs.");
  uiMem.ui_hud_hardcore = Cvar_RegisterBool("ui_hud_hardcore", 0, 0x80u, "Whether the HUD should be suppressed for hardcore mode");
  uiMem.ui_uav_allies = Cvar_RegisterBool("ui_uav_allies", 0, 0x80u, "Whether the UI should show UAV to allies");
  uiMem.ui_uav_axis = Cvar_RegisterBool("ui_uav_axis", 0, 0x80u, "Whether the UI should show UAV to axis");
  uiMem.ui_uav_client = Cvar_RegisterBool("ui_uav_client", 0, 0x80u, "Whether the UI should show UAV to this client");
  uiMem.ui_allow_classchange = Cvar_RegisterBool("ui_allow_classchange", 0, 0x80u, "Whether the UI should allow changing class");
  uiMem.ui_allow_teamchange = Cvar_RegisterBool("ui_allow_teamchange", 0, 0x80u, "Whether the UI should allow changing team");
  ui_debug = Cvar_RegisterBool("ui_debug", qfalse, 0, "Turns debug output of ui on");

  if(ui_debug->boolean)
  {
	Cmd_AddCommand("printOpenMenuNames", UI_DumpOpenMenuNames_f);
  }
  //On possible 1st run reset this variable to -1 from the default 0 setting
  if(Sys_IsTempInstall())
  {
	Cvar_SetInt(uiMem.ui_browserMod, -1);
  }

}


void Menus_RemoveFromStack(UiContext_t *dc, menuDef_t *pMenu)
{
  int v2;
  int v3;
  menuDef_t **v4;
  menuDef_t **v5;

  v2 = dc->openMenuCount - 1;
  v3 = dc->openMenuCount - 1;
  if ( v3 >= 0 )
  {
    v4 = &dc->menuStack[v3];
    while ( *v4 != pMenu )
    {
      --v3;
      --v4;
      if ( v3 < 0 )
        return;
    }
    dc->openMenuCount = v2;
    if ( v3 < v2 )
    {
      v5 = &dc->menuStack[v3];
      do
      {
        *v5 = v5[1];
        ++v3;
        ++v5;
      }
      while ( v3 < dc->openMenuCount );
    }
  }
}

void Menu_CallOnFocusDueToOpen(UiContext_t *dc, menuDef_t *menu)
{
  int v2;
  int v3;
  int v4;
  itemDef_t **v5;
  itemDef_t *v6;

  v2 = menu->itemCount;
  v3 = 0;
  if ( v2 > 0 )
  {
    v4 = dc->localClientNum;
    v5 = menu->items;
    while ( !(((unsigned int)(*v5)->window.dynamicFlags[v4] >> 2) & 1) || !(((unsigned int)(*v5)->window.dynamicFlags[v4] >> 1) & 1) )
    {
      ++v3;
      ++v5;
      if ( v3 >= v2 )
        return;
    }
    if ( menu->items[v3]->onFocus )
    {
      v6 = menu->items[v3];
      Item_RunScript(dc, v6, v6->onFocus);
    }
  }
}


void Menu_LoseFocusDueToOpen(UiContext_t *dc, menuDef_t *menu)
{
  int v3;

  if ( ((unsigned int)menu->window.dynamicFlags[dc->localClientNum] >> 2) & 1 )
  {
    if ( ((unsigned int)menu->window.dynamicFlags[dc->localClientNum] >> 1) & 1 )
    {
      menu->window.dynamicFlags[dc->localClientNum] &= 0xFFFFFFFD;
      v3 = 0;
      if ( menu->itemCount > 0 )
      {
        while ( !(((unsigned int)menu->items[v3]->window.dynamicFlags[dc->localClientNum] >> 2) & 1) || !(((unsigned int)menu->items[v3]->window.dynamicFlags[dc->localClientNum] >> 1) & 1) )
        {
          ++v3;
          if ( v3 >= menu->itemCount )
            return;
        }
        if ( menu->items[v3]->leaveFocus )
        {
          Item_RunScript(dc, menu->items[v3], menu->items[v3]->leaveFocus);
        }
      }
    }
  }
}


void Menus_Open(UiContext_t *dc, menuDef_t *menu)
{
  itemDef_t itemdef;
  int i;

  if(ui_debug->boolean)
  {
	Com_Printf(CON_CHANNEL_UI,  "^5Opening menu: %s\n", menu->window.name);
  }
/*
	if(!Q_stricmp(menu->window.name, "auconfirm"))
	{
		menu->items[2]->text = "Hello 3aGl3. This is a longer test text sentence \nHmm. And now another sentence :D Yes";
	}
*/
  for(i = dc->openMenuCount -1; i >= 0; --i)
  {
	Menu_LoseFocusDueToOpen(dc, dc->menuStack[i]);
  }


  Menus_RemoveFromStack(dc, menu);
  if ( dc->openMenuCount == 16 )
  {
    Com_Error(ERR_DROP, "Too many menus opened (16 menus)");
  }
  dc->menuStack[dc->openMenuCount] = menu;
  ++dc->openMenuCount;
  menu->window.dynamicFlags[dc->localClientNum] |= 6u;
  Menu_CallOnFocusDueToOpen(dc, menu);
  if ( dc->debug )
    sub_554400(dc, menu);

  if ( fs_gameDirVar->string[0] )
    sub_54C110(dc->localClientNum, menu, "mini_map2_overlay", 0);

  if ( menu->onOpen )
  {
    itemdef.parent = menu;
    Item_RunScript(dc, &itemdef, menu->onOpen);
  }
  if ( menu->soundLoop )
    SND_PlayLocalSoundAliasByName(dc->localClientNum, menu->soundLoop);
}


void Menus_Close(UiContext_t *dc, menuDef_t *menu)
{
	int i;
	itemDef_t itemdef;

	if(ui_debug->boolean)
	{
		Com_Printf(CON_CHANNEL_UI,  "^5Closing menu: %s\n", menu->window.name);
	}


	for(i = dc->openMenuCount - 1; i >= 0; --i)
	{
		if(dc->menuStack[i] == menu)
		{
			break;
		}
	}

	if(i < 0)
	{
		menu->window.dynamicFlags[dc->localClientNum] &= 0xFFFFFFF9;
		return;
	}

	if ( ((unsigned int)menu->window.dynamicFlags[dc->localClientNum] >> 2) & 1 && menu->onClose )
    {
		itemdef.parent = menu;
		Item_RunScript(dc, &itemdef, menu->onClose);
    }

	if ( ((unsigned int)menu->window.dynamicFlags[dc->localClientNum] >> 2) & 1 )
	{
        Menus_RemoveFromStack(dc, menu);
	}
    else
	{
        Menus_RemoveFromStack(dc, menu);
		menu->window.dynamicFlags[dc->localClientNum] &= 0xFFFFFFF9;
		return;
	}


	for(i = dc->openMenuCount - 1; i >= 0; --i)
	{
		if ( ((unsigned int)dc->menuStack[i]->window.dynamicFlags[dc->localClientNum] >> 2) & 1 )
		{
            break;
		}
	}

	if(i < 0)
	{
		menu->window.dynamicFlags[dc->localClientNum] &= 0xFFFFFFF9;
		return;
	}

	dc->menuStack[i]->window.dynamicFlags[dc->localClientNum] |= 2u;
    Menu_CallOnFocusDueToOpen(dc, dc->menuStack[i]);
	menu->window.dynamicFlags[dc->localClientNum] &= 0xFFFFFFF9;

}

void Menus_CloseAll(UiContext_t *dc)
{

	int i;

	for(i = 0; i < dc->menuCount; ++i)
	{
		Menus_Close(dc, dc->Menus[i]);
	}

}

void Menus_AddMenu(menuDef_t* menu)
{
	uiInfo_t *info = &uiMem.uiInfo;
	UiContext_t *dc = &uiMem.uiInfo.uiDC;

	if(info->uiDC.menuCount >= 640)
	{
		return;
	}

	dc->Menus[dc->menuCount] = menu;

	++dc->menuCount;

}

void UI_CloseAllMenusInternal(int localclientnum)
{
  Menus_CloseAll(&uiMem.uiInfo.uiDC);
  UI_SetActiveMenu(localclientnum, 0);
}

void UI_CloseAllMenus()
{
  UI_CloseAllMenusInternal(0);

}




void UI_OpenAutoUpdateConfirmMenu()
{
	Menus_Open(&uiMem.uiInfo.uiDC, UI_AutoUpdateConfirmMenu( ));
}

void UI_OpenInstallConfirmMenu()
{
	Menus_Open(&uiMem.uiInfo.uiDC, UI_InstallMenu(Cmd_Argv(1)));
}


void UI_DrawText(const ScreenPlacement *scrPlace, const char *text, int maxChars, Font_t *font, float ix, float iy, int horzAlign, int vertAlign, float scale, const float *color, int style)
{
  long double nScale;
  float x;
  float y;
  float yScale;
  float xScale;

  nScale = R_NormalizedTextScale(font, scale);
  xScale = nScale;
  yScale = nScale;
  ScrPlace_ApplyRect(scrPlace, &ix, &iy, &xScale, &yScale, horzAlign, vertAlign);
  x = floor(ix + 0.5);
  y = floor(iy + 0.5);
  R_AddCmdDrawText(text, maxChars, font, x, y, xScale, yScale, 0.0, color, style);
}

void UI_DrawHandlePic(ScreenPlacement *place, float x, float y, float w, float h, int horzAlign, int vertAlign, float *color, Material *material)
{
    float wi = 0, hi = 0, s0 = 0, t0 = 0, s1 = 0, t1 = 0;

    if (w >= 0.0)
    {
        s0 = 0.0;
        s1 = 1.0;
        wi = w;
    }
    else
    {
        s0 = 1.0;
        s1 = 0.0;
        wi = -w;
    }

    if (h >= 0.0)
    {
        t0 = 0.0;
        t1 = 1.0;
        hi = h;
    }
    else
    {
        t0 = 1.0;
        t1 = 0.0;
        hi = -h;
    }
    CL_DrawStretchPic(place, x, y, wi, hi, horzAlign, vertAlign, s0, t0, s1, t1, color, material);
}

void UI_DrawGameTypeString(rectDef_t *rect, Font_t *font, float fontscale, float *color, int style)
{
  const char *string;
  const char *transString;
  int localClientNum = 0;

  if ( uiMem.ui_joinGametype->integer >= uiMem.numJoinGameTypes )
  {
    Cvar_SetInt(uiMem.ui_joinGametype, 0);
  }
  string = uiMem.joinGameTypes[uiMem.ui_joinGametype->integer].type_name;
  if ( !*string )
  {
    string = "EXE_ALL";
  }
  transString = UI_SafeTranslateString(string);
  UI_DrawText(&scrPlaceView[localClientNum], transString, 0x7FFFFFFF, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, fontscale, color, style);
}


char* ui_updatesourcestrings[] = {
	"EXE_LOCAL",
	"EXE_INTERNET\x14 CoD4X",
	"EXE_FAVORITES",
	"EXE_INTERNET\x14 ATVI",
	"bad allocation"
};
char* MonthAbbrev[] = {
	"EXE_MONTH_ABV_JANUARY",
	"EXE_MONTH_ABV_FEBRUARY",
	"EXE_MONTH_ABV_MARCH",
	"EXE_MONTH_ABV_APRIL",
	"EXE_MONTH_ABV_MAY",
	"EXE_MONTH_ABV_JUN",
	"EXE_MONTH_ABV_JULY",
	"EXE_MONTH_ABV_AUGUST",
	"EXE_MONTH_ABV_SEPTEMBER",
	"EXE_MONTH_ABV_OCTOBER",
	"EXE_MONTH_ABV_NOVEMBER",
	"EXE_MONTH_ABV_DECEMBER"
};

//LAN_GetServerPtr

REGPARM(1) void sub_544D00(int a1)
{
  int v2; // ecx@6
  int updateval; // ecx@8

  if ( a1 != 200 && a1 != 201)
  {
    if ( a1 != 13 && a1 != 191 )
    {
        return;
    }
  }
  if ( a1 != 201 )
  {
    updateval = uiMem.ui_netSource->integer + 1;
    if ( updateval == 4 )
      updateval = 0;
  }else{
	v2 = uiMem.ui_netSource->integer;
	if ( !uiMem.ui_netSource->integer )
	{
		v2 = 4;
	}
	updateval = v2 - 1;
  }

  Cvar_SetInt(uiMem.ui_netSource, updateval);
  if ( uiMem.ui_netSource->integer != 1 )
  {
    UI_StartServerRefresh(0, qtrue);
  }
  UI_BuildServerDisplayList(&uiMem.uiInfo, 1);
}


/*
==================
UI_RemoveServerFromDisplayList
==================
*/
static void UI_RemoveServerFromDisplayList( int num ) {
	int i, j;

	for ( i = 0; i < uiMem.serverStatus.numDisplayServers; i++ ) {
		if ( uiMem.serverStatus.displayServers[i] == num ) {
			uiMem.serverStatus.numDisplayServers--;
			for ( j = i; j < uiMem.serverStatus.numDisplayServers; j++ ) {
				uiMem.serverStatus.displayServers[j] = uiMem.serverStatus.displayServers[j + 1];
			}
			return;
		}
	}
}



static void UI_UpdatePendingPings() {
	LAN_ResetPings( uiMem.ui_netSource->integer );
	uiMem.serverStatus.refreshActive = qtrue;
	uiMem.serverStatus.refreshtime = uiMem.uiInfo.uiDC.realTime + 1000;
}




/*
=================
UI_StartServerRefresh
=================
*/
void UI_StartServerRefresh( int ctx, qboolean full ) {
	const char *ptr;
	int i;

	qtime_t q;
	Com_RealTime( &q );
	char cvarname[1024];
	char cvarval[1024];
	Com_sprintf(cvarname, sizeof(cvarname), "ui_lastServerRefresh_%i", uiMem.ui_netSource->integer);
	Com_sprintf(cvarval, sizeof(cvarval), "%s %i, %i  %i:%i", UI_SafeTranslateString(MonthAbbrev[q.tm_mon]),q.tm_mday, 1900 + q.tm_year,q.tm_hour,q.tm_min);

	Cvar_Set(cvarname, cvarval);

	if ( !full ) {
		UI_UpdatePendingPings();
		return;
	}

	uiMem.serverStatus.refreshActive = qtrue;
	uiMem.serverStatus.nextDisplayRefresh = uiMem.uiInfo.uiDC.realTime + 1000;
	// clear number of displayed servers
	uiMem.serverStatus.numDisplayServers = 0;
	uiMem.serverStatus.numPlayersOnServers = 0;
	uiMem.serverStatus.totalServers = LAN_GetServerCount(uiMem.ui_netSource->integer);
	// mark all servers as visible so we store ping updates for them
	LAN_MarkServerVisible( uiMem.ui_netSource->integer, -1, qtrue );
	// reset all the pings
	LAN_ResetPings( uiMem.ui_netSource->integer );
	//
	if ( uiMem.ui_netSource->integer == AS_LOCAL ) {
		Cbuf_AddText( "localservers\n" );
		uiMem.serverStatus.refreshtime = uiMem.uiInfo.uiDC.realTime + 1000;
		return;
	}

	uiMem.serverStatus.refreshtime = uiMem.uiInfo.uiDC.realTime + 5000;
	if ( uiMem.ui_netSource->integer == AS_GLOBAL || uiMem.ui_netSource->integer == AS_MPLAYER ) {
		if ( uiMem.ui_netSource->integer == AS_GLOBAL ) {
			i = 0;
		} else {
			i = 1;
		}
		ptr = Cvar_GetVariantString( "debug_protocol" );
		if ( *ptr ) {
			Cbuf_AddText( va("globalservers %i %s full empty\n", i, ptr ));
		} else {
			Cbuf_AddText( va("globalservers %i %d full empty\n", i, LEGACY_PROTOCOL_VERSION ));
		}
	}
}




/*
==================
UI_BuildServerDisplayList
==================
*/
void UI_BuildServerDisplayList(uiInfo_t *ui_info, int force ) {
	int i, count, clients, maxClients, ping, len, friendlyFire, punkbuster;
	char info[MAX_STRING_CHARS];
	//qboolean startRefresh = qtrue; // TTimo: unused

	if ( !( force || ui_info->uiDC.realTime > uiMem.serverStatus.nextDisplayRefresh ) ) {
		return;
	}
	// if we shouldn't reset
	if ( force == 2 ) {
		force = 0;
	}

	// do motd updates here too
	Q_strncpyz(uiMem.serverStatus.motd, Cvar_VariableString( "cl_motdString"), sizeof( uiMem.serverStatus.motd ) );
	len = strlen( uiMem.serverStatus.motd );
	if ( len == 0 ) {
		Com_sprintf( uiMem.serverStatus.motd, sizeof(uiMem.serverStatus.motd), "%s - %s", UI_SafeTranslateString("EXE_COD_MULTIPLAYER"), Q3_VERSION );
		len = strlen( uiMem.serverStatus.motd );
	}
	if ( len != uiMem.serverStatus.motdLen ) {
		uiMem.serverStatus.motdLen = len;
		uiMem.serverStatus.motdWidth = -1;
	}

	if ( force ) {
		// clear number of displayed servers
		uiMem.serverStatus.numDisplayServers = 0;
		uiMem.serverStatus.numPlayersOnServers = 0;
		uiMem.serverStatus.totalServers = LAN_GetServerCount(uiMem.ui_netSource->integer);
		// set list box index to zero
		if ( uiMem.serverStatus.currentServer >= 0 )
		{
			Menu_SetFeederSelection(ui_info, 0, 2, 0, 0);
		}
		// mark all servers as visible so we store ping updates for them
		LAN_MarkServerVisible( uiMem.ui_netSource->integer, -1, qtrue );
	}

	// get the server count (comes from the master)
	count = LAN_GetServerCount( uiMem.ui_netSource->integer );
	if(((uiMem.ui_netSource->integer == AS_GLOBAL || uiMem.ui_netSource->integer == AS_MPLAYER) && Sys_Milliseconds() < cls.globalServerRequestTime) || count == -1 || ( uiMem.ui_netSource->integer == AS_LOCAL && count == 0 ))
	{
		uiMem.serverStatus.numDisplayServers = 0;
		uiMem.serverStatus.numPlayersOnServers = 0;
		uiMem.serverStatus.totalServers = LAN_GetServerCount(uiMem.ui_netSource->integer);
		uiMem.serverStatus.nextDisplayRefresh = ui_info->uiDC.realTime + 500;
		return;
	}
	if(uiMem.ui_netSource->integer == AS_FAVORITES)
	{
		uiMem.serverStatus.numPlayersOnServers = 0;
	}
	qsort(uiMem.serverStatus.displayServers, uiMem.serverStatus.numDisplayServers, 4u, UI_ServersQsortCompare);

	for ( i = 0; i < count; i++ ) {

		// if we already got info for this server
		if ( !LAN_ServerIsVisible( uiMem.ui_netSource->integer, i ) ) {
			continue;
		}
		// get the ping for this server
		ping = LAN_GetServerPing( uiMem.ui_netSource->integer, i );
		if ( ping > 0 || uiMem.ui_netSource->integer == AS_FAVORITES ) {

			LAN_GetServerInfo( uiMem.ui_netSource->integer, i, info, sizeof(info) );

			if ( !Q_stricmpn(Info_ValueForKey(info, "addr"), "000.000.000.000", 15) )
			{
				LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
				continue;
			}

			clients = atoi( Info_ValueForKey( info, "clients" ) );
			uiMem.serverStatus.numPlayersOnServers += clients;

			if(ping > 0 || (uiMem.ui_netSource->integer == AS_FAVORITES && LAN_ServerInfoValid(i)))
			{

				if ( uiMem.ui_browserShowEmpty->boolean == 0 ) {
					if ( clients == 0 ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				if ( uiMem.ui_browserShowFull->boolean == 0 ) {
					maxClients = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
					if ( clients == maxClients ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				if ( uiMem.ui_browserShowPassword->integer >= 0 )
				{
					int password = atoi( Info_ValueForKey( info, "pswrd" ) );
					if ( password != uiMem.ui_browserShowPassword->integer )
					{
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				if ( uiMem.ui_browserShowPure->boolean ) {
					int pure = atoi( Info_ValueForKey( info, "pure" ) );
					if ( !pure ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				if ( uiMem.ui_browserShowDedicatedOnly->boolean ) {
					int hw = atoi( Info_ValueForKey( info, "hw" ) );

					if ( hw != 1 && hw != 2 && hw != 3 ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				if ( uiMem.ui_browserMod->integer >= 0 ) {
					int mod = atoi( Info_ValueForKey( info, "mod" ) );

					if ( mod != uiMem.ui_browserMod->integer ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				// NERVE - SMF - friendly fire parsing
				if ( uiMem.ui_browserFriendlyfire->integer >= 0 ) {
					friendlyFire = atoi( Info_ValueForKey(info, "ff") );

					if ( uiMem.ui_browserFriendlyfire->integer != friendlyFire ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				if ( uiMem.ui_browserKillcam->integer >= 0 ) {
					int kc = atoi( Info_ValueForKey(info, "kc") );

					if ( uiMem.ui_browserKillcam->integer != kc ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				if ( uiMem.ui_browserHardcore->integer >= 0 ) {
					int hc = atoi( Info_ValueForKey(info, "hc") );

					if ( uiMem.ui_browserHardcore->integer != hc ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				if ( uiMem.ui_browserOldSchool->integer >= 0 ) {
					int od = atoi( Info_ValueForKey(info, "od") );

					if ( uiMem.ui_browserOldSchool->integer != od ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				if ( uiMem.ui_browserOldSchool->integer >= 0 ) {
					int od = atoi( Info_ValueForKey(info, "od") );

					if ( uiMem.ui_browserOldSchool->integer != od ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				// DHM - Nerve - PunkBuster parsing
				if ( uiMem.ui_browserShowPunkBuster->integer >= 0) {
					punkbuster = atoi( Info_ValueForKey( info, "pb" ) );

					if ( uiMem.ui_browserShowPunkBuster->integer != punkbuster ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				if ( uiMem.joinGameTypes[uiMem.ui_joinGametype->integer].type_name[0] ) {
					const char* gt = Info_ValueForKey( info, "gametype" );

					if ( Q_stricmp(gt, uiMem.joinGameTypes[uiMem.ui_joinGametype->integer].name) ) {
						LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
						continue;
					}
				}

				/*
				if ( ui_serverFilterType.integer > 0 ) {
					if ( Q_stricmp( Info_ValueForKey( info, "game" ), serverFilters[ui_serverFilterType.integer].basedir ) != 0 ) {
						trap_LAN_MarkServerVisible( ui_netSource.integer, i, qfalse );
						continue;
					}
				}
				*/
			}


			// make sure we never add a favorite server twice
			if ( uiMem.ui_netSource->integer == AS_FAVORITES ) {
				UI_RemoveServerFromDisplayList( i );
			}
			// insert the server into the list
			UI_BinaryServerInsertion( i );
			// done with this server
			if ( ping > 0 ) {
				LAN_MarkServerVisible( uiMem.ui_netSource->integer, i, qfalse );
			}
		}
	}


	uiMem.serverStatus.refreshtime = ui_info->uiDC.realTime;
}

REGPARM(1) void UI_BuildServerDisplayList_Stub(int force, uiInfo_t* ui_info)
{
	UI_BuildServerDisplayList(ui_info, force);
}

/*
=================
LerpColor
	lerp and clamp each component of <a> and <b> into <c> by the fraction <t>
=================
*/
void LerpColor( vec4_t a, vec4_t b, vec4_t c, float t ) {
	int i;
	for ( i = 0; i < 4; i++ )
	{
		c[i] = a[i] + t * ( b[i] - a[i] );
		if ( c[i] < 0 ) {
			c[i] = 0;
		} else if ( c[i] > 1.0 ) {
			c[i] = 1.0;
		}
	}
}


void UI_DrawServerRefreshDate( int ctx, rectDef_t *rect, Font_t *font, float scale, vec4_t color, int textStyle ) {
	int serverCount;            // NERVE - SMF

	if ( uiMem.serverStatus.refreshActive ) {
		vec4_t lowLight, newColor;
		lowLight[0] = 0.8 * color[0];
		lowLight[1] = 0.8 * color[1];
		lowLight[2] = 0.8 * color[2];
		lowLight[3] = 0.8 * color[3];
		LerpColor( color,lowLight,newColor,0.5 + 0.5 * sin( uiMem.uiInfo.uiDC.realTime / PULSE_DIVISOR ) );
		// NERVE - SMF
		serverCount = LAN_GetServerCount( uiMem.ui_netSource->integer );
		if ( !LAN_WaitServerResponse(uiMem.ui_netSource->integer) ) {
			char buff[1024],  buff2[64];
			Q_strncpyz(buff, UI_SafeTranslateString("EXE_GETTINGINFOFORSERVERS"), sizeof(buff));
			char* s = strstr(buff, "&&1");
			if(s && strlen(buff) < (sizeof(buff) - 128))
			{
				Q_strncpyz(buff2, s + 3, sizeof(buff2));
				sprintf(s, "%d/%d %s", cls.countPingServers, serverCount, buff2);
			}

			UI_DrawText(&scrPlaceView[ctx], buff, 0x7FFFFFFF, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, newColor, textStyle );
		} else {

			UI_DrawText(&scrPlaceView[ctx], UI_SafeTranslateString("EXE_WAITINGFORMASTERSERVERRESPONSE"), 0x7FFFFFFF, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, newColor, textStyle );
		}
	} else {
		char buff[64];
		Q_strncpyz( buff, Cvar_VariableString( va( "ui_lastServerRefresh_%i", uiMem.ui_netSource->integer ) ), sizeof(buff) );
		const char* rs = UI_SafeTranslateString("EXE_REFRESHTIME");
		const char* text = UI_ReplaceConversionString(rs, buff);

		UI_DrawText(&scrPlaceView[ctx], text, 0x7FFFFFFF, font, rect->x, rect->y, rect->horzAlign, rect->vertAlign, scale, color, textStyle);
	}

}

/*
=================
UI_DoServerRefresh
=================
*/
/* Using this function makes the server refresh insane slow. Idk why... */
void UI_DoServerRefresh( void ) {
	qboolean wait = qfalse;
  static int timelimit;

	if ( !uiMem.serverStatus.refreshActive ) {
		return;
	}
	if ( uiMem.ui_netSource->integer != AS_FAVORITES ) {
		if ( uiMem.ui_netSource->integer == AS_LOCAL ) {
			if ( !LAN_GetServerCount(AS_LOCAL) )
      {
				wait = qtrue;
			}
		} else {
			if ( LAN_WaitServerResponse(uiMem.ui_netSource->integer) )
      {
				wait = qtrue;
			}
		}
	}

  if ( uiMem.uiInfo.uiDC.realTime < uiMem.serverStatus.refreshtime && wait && timelimit == 0)
  {
		  return;
	}

	UI_BuildServerDisplayListByNetSource(&uiMem.uiInfo);

	// if still trying to retrieve pings
	if ( timelimit == 0 && CL_UpdateDirtyPings(uiMem.ui_netSource->integer) ) {
		uiMem.serverStatus.refreshtime = uiMem.uiInfo.uiDC.realTime + 2000;
  } else {
    if(timelimit == 0)
    {
      timelimit = uiMem.uiInfo.uiDC.realTime + 1000;
    }
     if ( !wait && timelimit < uiMem.uiInfo.uiDC.realTime) { //Bugfix applied: Wait 1 more second for all servers to have sent their details
        // get the last servers in the list
        UI_BuildServerDisplayList( &uiMem.uiInfo, 2 );
        // stop the refresh
        UI_StopServerRefresh();
        timelimit = 0;
     }
  }
	//
	UI_BuildServerDisplayList( &uiMem.uiInfo, qfalse );

}

/*
unsigned int UILocalVar_HashName(const char *name)
{
  char v1; // cl@1
  uint16_t v2; // si@1
  int v3; // edx@2
  int v4; // edi@3

  v1 = *name;
  v2 = 0;
  if ( *name )
  {
    v3 = 119 - (uint32_t)name;
    do
    {
      v4 = v1 * (uint32_t)&name[v3];
      v1 = (name++)[1];
      v2 += v4;
    }
    while ( v1 );
  }
  return (uint8_t)(v2 + HIBYTE(v2));
}

byte UILocalVar_FindLocation(const char *name, UILocalVarContext_t *a2, unsigned int *a3)
{
  unsigned int v3; // eax@1
  unsigned int v4; // esi@1
  const char *v5; // ecx@2

  v3 = UILocalVar_HashName(name);
  v4 = v3;
  while ( 1 )
  {
    v5 = a2->table[v4].name;
    if ( !v5 )
    {
      *a3 = v4;
      return 0;
    }
    if ( !strcmp(v5, name) )
      break;
    v4 = (uint8_t)(v4 + 1);
    if ( v4 == v3 )
    {
      *a3 = v4;
      return 0;
    }
  }
  *a3 = v4;
  return 1;
}

UILocalVar* __fastcall GetLocalVar(Operand *op)
{
  unsigned int index;

  if ( op->dataType == VAL_STRING )
  {
    if ( UILocalVar_FindLocation(op->internals.string, &uiMem.uiInfo.uiDC.localVars, &index) )
      return &uiMem.uiInfo.uiDC.localVars.table[index];
    return NULL;
  }
  const char* type;
  switch(op->dataType)
  {
    case VAL_INT:
      type = "Integer";
      break;
    case VAL_FLOAT:
      type = "Float";
      break;
    default:
      type = "Unknown";
  }
  Com_PrintError("Error: Must use a string as the name of a localVar, not a %s\n", type);
  return NULL;
}*/
/*
0053C770*/



bool __cdecl ValidGamePadButtonIcon(unsigned int letter)
{
  if ( letter >= 1 && letter <= 6 )
  {
    return 1;
  }
  if ( letter >= 14 && letter <= 25 )
  {
    return 1;
  }
  if ( letter >= 0x1C && letter <= 0x1F )
  {
    return 1;
  }
  if ( letter == 188 )
  {
    return 1;
  }
  return letter == 189;
}

