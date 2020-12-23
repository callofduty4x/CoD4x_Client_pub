#ifndef __UI_SHARED__
#define __UI_SHARED__

#include "q_shared.h"
#include "xassets/menudef.h"
#include "xassets/font.h"
#include "r_shared.h"
#include <stdbool.h>


#define uiMem (*((uiMem_t*)(0xCAE5C48)))

#define MAX_MAPS 128

#pragma pack(push, 1)
typedef struct
{
  char* gameType;
  int gtEnum;
}gameTypeInfo_t;

typedef struct
{
  char *mapName;
  char field_5000[156];
}mapInfo_t;

typedef struct
{
  int modName;
  int modDescr;
}modInfo_t;

typedef struct
{
  int refreshtime;
  int field_04;
  int sortKey;
  int sortDir;
  int field_8;
  int refreshActive;
  int currentServer;
  int displayServers[20000]; //MAX_GLOBAL_SERVERS got splitted to 10000
  int numDisplayServers;
  int totalServers;//field_13890;
  int numPlayersOnServers; //totalPlayers;
  int nextDisplayRefresh;
  int field_138AC;
  int motdLen;
  int motdWidth;
  int field_138B8;
  int field_138BC;
  int field_138C0;
  int field_138C4;
  char motd[1024];
}uiServerStatus_t;



typedef enum
{
  UILOCALVAR_INT = 0x0,
  UILOCALVAR_FLOAT = 0x1,
  UILOCALVAR_STRING = 0x2,
}UILocalVarType_t;


typedef union
{
  int integer;
  float value;
  const char *string;
}UILocalVarValue_t;


typedef struct UILocalVar_s
{
  UILocalVarType_t type;
  const char *name;
  UILocalVarValue_t value;
}UILocalVar_t;


typedef struct UILocalVarContext_s
{
  UILocalVar_t table[256];
}UILocalVarContext_t;


typedef struct UiContext_s
{
  int localClientNum;
  float bias;
  int realTime;
  int frameTime;
  int cursorx;
  int cursory;
  int debug;
  int screenWidth;
  int screenHeight;
  float screenAspect;
  float FPS;
  float blurRadiusOut;
  menuDef_t *Menus[640];
  int menuCount;
  menuDef_t *menuStack[16];
  int openMenuCount;
  UILocalVarContext_t localVars;
}UiContext_t;


typedef enum
{
  UIMENU_NONE = 0x0,
  UIMENU_MAIN = 0x1,
  UIMENU_INGAME = 0x2,
  UIMENU_PREGAME = 0x3,
  UIMENU_POSTGAME = 0x4,
  UIMENU_WM_QUICKMESSAGE = 0x5,
  UIMENU_SCRIPT_POPUP = 0x6,
  UIMENU_SCOREBOARD = 0x7,
  UIMENU_SPLITSCREENGAMESETUP = 0x8,
  UIMENU_SYSTEMLINKJOINGAME = 0x9,
  UIMENU_PARTY = 0xA,
  UIMENU_GAMELOBBY = 0xB,
  UIMENU_PRIVATELOBBY = 0xC,
}uiMenuCommand_t;


typedef struct uiInfo_s
{
  UiContext_t uiDC;
  int myTeamCount;
  int playerRefresh;
  int playerIndex;
  int numPlayerProfiles;
  const char *profilenames[64];
  int playerProfilesSorted;
  int sortedProfiles[64];
  int timeIndex;
  int previousTimes[4];
  uiMenuCommand_t currentMenuType;
  byte allowScriptMenuResponse;
  byte pad[3];
}uiInfo_t;




typedef struct keywordHash_s
{
  const char *keyword;
  int func;
}keywordHash_t;

typedef struct
{
	const char* name;
	const char* type_name;
}joingamtype_t;


#pragma pack(push, 1)
typedef struct uiMem_s
{
  char field_0[1288];
  cvar_t *ui_showList;
  cvar_t *ui_customClassName;
  char gap_510[44];
  int field_53C;
  char gap_540[16];
  int field_550;
  cvar_t *ui_playerProfileCount;
  cvar_t *ui_browserHardcore;
  cvar_t *ui_browserShowPunkBuster;
  cvar_t *ui_netSource;
  cvar_t *ui_netGametypeName;
  cvar_t *ui_showMenuOnly;
  cvar_t *ui_bigFont;
  cvar_t *ui_cinematicsTimestamp;
  cvar_t *ui_connectScreenTextGlowColor;
  cvar_t *ui_extraBigFont;
  cvar_t *ui_browserShowPure;
  cvar_t *ui_drawCrosshair;
  cvar_t *ui_currentNetMap;
  cvar_t *ui_buildSize;
  cvar_t *ui_browserKillcam;
  cvar_t *ui_allow_classchange;
  cvar_t *ui_browserOldSchool;
  cvar_t *ui_hud_hardcore;
  cvar_t *ui_gametype;
  int field_5A0;
  cvar_t *ui_currentMap;
  int field_5A8;
  cvar_t *ui_netGametype;
  cvar_t *ui_joinGametype;
  cvar_t *ui_browserShowEmpty;
  char gap_5B8[20];
  int field_5CC;
  int field_5D0;
  char gap_5D4[12];
  int field_5E0;
  char gap_5E4[28];
  int field_600;
  int field_604;
  char field_608[32688];
  uiInfo_t uiInfo;
  byte unk99[3076];
  int nextFindPlayerRefresh;
  cvar_t *ui_browserShowDedicatedOnly;
  cvar_t *ui_dedicated;
  cvar_t *ui_playerProfileSelected;
  cvar_t *ui_allow_teamchange;
  cvar_t *ui_buildLocation;
  cvar_t *ui_smallFont;
  cvar_t *ui_customModeEditName;
  cvar_t *ui_uav_axis;
  char gap_AA88[40];
  Font_t *bigFont;
  Font_t *extraSmallFont;
  Font_t *font1;
  Font_t *font3;
  Font_t *smallFont;
  Font_t *extraBigFont;
  Font_t *font2;
  char gap_AACC[4];
  int playerCount;
  char playerNames[64][32];
  char gap_2D1C[2048];
  int uiClients[64];
  int dword_CAF181C;
  gameTypeInfo_t gameTypes[32];
  char gap_3720[2308];
  int numJoinGameTypes;
  joingamtype_t joinGameTypes[32];
  int mapCount;
  int dword_CAF232C;
  mapInfo_t mapInfo[128];
  char gap_116E8[36];
  modInfo_t modInfo[64];
  int field_9354;
  int modIndex;
  char gap_935c[1104];
  uiServerStatus_t serverStatus;
  char serverStatusAddress[64];
  char gap_1D4b4[3332];
  int nextServerStatusRefresh;
  char gap_26774[2244];
  cvar_t *ui_serverStatusTimeOut;
  int field_2703C;
  cvar_t *ui_browserFriendlyfire;
  cvar_t *ui_uav_allies;
  cvar_t *ui_browserMod;
  cvar_t *ui_browserShowFull;
  char gap_27050[60];
  int field_2708C;
  cvar_t *ui_uav_client;
  cvar_t *ui_playerProfileNameNew;
  cvar_t *ui_browserShowPassword;
  cvar_t *ui_borderLowLightScale;
  cvar_t *ui_showEndOfGame;
  cvar_t *ui_customModeName;
  char gap_270A8[32];
  keywordHash_t *itemParseKeywordHash[256];
  char field_1EF10[32768];
  keywordHash_t *menuParseKeywordHash[128];
  char field_2F6C8[8780];
  char field_935C;
}uiMem_t;
#pragma pack(pop)



typedef struct
{

  const char *name;
  void (__cdecl *handler)(void *uictx, itemDef_t *itemdef, char **args);

}commandDef_t;

#pragma pack(pop)

void UI_OpenMenuOnDvar(const char *cmd, const char *menuName, const char *varName, char *cmpValue);
void UI_CloseMenuOnDvar(const char *cmd, const char *menuName, const char *varName, char *cmpValue);
qboolean UI_GetOpenOrCloseMenuOnDvarArgs(char **args, const char *menuName, char *dvarbuf, int dvarlen, char *testbuf, int testlen, char *menubuf, int menulen);
void Menus_OpenByName(const char* menuStr);
void Menus_CloseByName(const char *menuStr);
qboolean String_Parse(char**p, char* out, int outlen);
void __cdecl Menus_Close(UiContext_t *dc, menuDef_t *menu);
void Menus_Open(UiContext_t *dc, menuDef_t *menu);
void Menus_ShowByName(UiContext_t *dc, const char *windowName);
void Menu_Close(menuDef_t *menu);

menuDef_t* Menus_FindName(UiContext_t *dc, const char* menuname);
const char* UI_SafeTranslateString(const char*);
void UI_LoadArenas();
void sub_546E60();
void sub_547010();
void Menu_SetFeederSelection(uiInfo_t* ui_info, menuDef_t* menu, int, int, const char*);
void Menus_CloseAll(UiContext_t *dc);
void UI_SelectCurrentMap();
void UI_LoadGameInfo();
void UI_LoadMods();
void UI_StartServerRefresh(int netsrc, qboolean);
void UI_BuildServerDisplayList(uiInfo_t*, int);
void UI_StopServerRefresh();
void UI_CloseAllMenus();
void UI_CloseAllMenusInternal(int localclientnum);
void UI_BuildServerDisplayListByNetSource();
void UI_BuildServerStatus(uiInfo_t*, int);
void UI_FeederSelection(float, int);
int UI_ServersQsortCompare(const void*, const void*);
void UI_Update(const char *name);
void UI_VerifyLanguage();
qboolean UI_IsFullscreen(uiInfo_t* inf);
void UI_SetActiveMenu(int, int);
void UI_SetMap(const char* map, const char* gametype);
void Item_RunScript( UiContext_t *dc, itemDef_t *item, const char *s );
void UI_DrawText(const ScreenPlacement *scrPlace, const char *text, int maxChars, Font_t *font, float ix, float iy, int horzAlign, int vertAlign, float scale, const float *color, int style);
void UI_DrawBuildString( );
void UI_AddPlayerProfile();

Font_t * UI_GetFontHandle(ScreenPlacement *place, int fontindex, float fontscale);
int UI_TextWidth(const char* text, int maxChars, Font_t *font, float scale);
int UI_TextHeight(Font_t *font, float scale);
int __cdecl sub_554400(UiContext_t *dc, menuDef_t *menu);
void __cdecl sub_54C110(int localClientNum, menuDef_t *menu, const char *a3, int a4);
void UI_OpenAutoUpdateConfirmMenu();
void UI_OpenInstallConfirmMenu();
void UI_DrawHandlePic(ScreenPlacement *place, float x, float y, float w, float h, int horzAlign, int vertAlign, float *color, Material *material);
void UI_BinaryServerInsertion(int i);
void UI_DrawTTFText(const ScreenPlacement *scrPlace, const char *text, int maxChars, Font_t *font, float ix, float iy, int horzAlign, int vertAlign, float scale, const float *color, int style);
void UI_TTFTextHeightWidth(const char *text, int maxChars, Font_t *font, float scale, int* outwidth, int* outheight);
void UI_SetActiveMenuByName(const char* menuname);
uiMenuCommand_t UI_GetActiveMenu();
void Menu_Open(menuDef_t *menu);


#endif
