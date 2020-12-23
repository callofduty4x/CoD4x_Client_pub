#include <stdint.h>
#include "steam_api/steam_shared.h"

int Com_SteamServerApi_Load(uint32_t unIP, uint16_t usSteamPort, uint16_t usGamePort, uint16_t usQueryPort, const char *pchVersionString);
void Com_SteamServerApi_RunFrame( );
void Com_SteamServerApi_Shutdown( );
uint64_t Com_SteamServerApi_GetHostSteamID( );
void Com_SteamServerApi_EndAuthSession(uint64_t steamid);
int Com_SteamServerApi_StartAuthentication(uint64_t steamid, const void* pToken, unsigned int uTokenLength);
void Com_SteamServerApi_SteamIDToString(uint64_t steamid, char* string, int length);

int Com_SteamClientApi_Load( const wchar_t* dllpath, int appID, int gameId, const char* modName, int overlayrenderer );
uint64_t Com_SteamClientApi_GetSteamID( );
void Com_SteamClientApi_Shutdown();
void Com_SteamClientApi_RunFrame( );
void Com_SteamClientApi_AdvertiseServer(uint32_t unIPServer, uint16_t usPortServer);
void Com_SteamClientApi_StopAdvertisingServer( );
void Com_SteamClientApi_CancelAuthTicket( );
uint32_t Com_SteamClientApi_GetAuthTicket(unsigned char* data, int buflen);
void Com_SteamClientApi_SteamIDToString(uint64_t steamid, char* string, int length);
void Com_SteamClientApi_GetLocalPlayerName(char* playername, int len);
int Com_SteamClientApi_IsLoaded();
int Com_SteamClientApi_GetClans(clan_t* clans, int maxcount);


