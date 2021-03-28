// steamcrap.cpp : Defines the entry point for the console application.
//
#include <cstdlib>
#include <windows.h>
#include <cstdint>
#include <cstdarg>
#include <cstdio>
// #include <wchar.h>
// #include <iostream>
#include <list>
// #include <utility>
//#include <algorithm>
// #include <signal.h>
//#include <mutex>

#define CON_CHANNEL_SYSTEM 0x10


extern "C" {
	void Com_Printf(int channel, const char *fmt, ...);
	void Com_DPrintf(int channel, const char *fmt, ...);
	void Com_Error(int type, const char *fmt, ...);
	void Com_PrintError(int channel, const char *fmt, ...);
	int Com_sprintf(char *dest, int size, const char *fmt, ...);
	void Q_strncpyz( char *dest, const char *src, int destsize );
	void SV_SteamSetUserInfo(uint64_t steamid, int haslicense, int valid);
	void Com_Quit_f();
	int SV_GetSteamUserInfo(uint64_t* steamid, char** playername, int* score);
	const char* Sys_ExeFile(void);
	//void Com_LogDebugData(byte* data, int len);
	int Sys_IsTempInstall();
}

int CheckSteamLibraryW(const wchar_t* szFilename);
int CheckSteamLibraryA(const char* szFilename);

//#define strncpy_s Q_strncpyz
//#define sprintf_s Com_sprintf


#define STEAM_API_NODLL
#define VERSION_SAFE_STEAM_API_INTERFACES

#define STEAMUSER_INTERFACE_VERSION "SteamUser017"
#define STEAMFRIENDS_INTERFACE_VERSION "SteamFriends014"
#define STEAMUTILS_INTERFACE_VERSION "SteamUtils007"
#define STEAMMATCHMAKING_INTERFACE_VERSION "SteamMatchMaking009"
#define STEAMMATCHMAKINGSERVERS_INTERFACE_VERSION "SteamMatchMakingServers002"
#define STEAMUSERSTATS_INTERFACE_VERSION "STEAMUSERSTATS_INTERFACE_VERSION011"
#define STEAMAPPS_INTERFACE_VERSION "STEAMAPPS_INTERFACE_VERSION006"
#define STEAMNETWORKING_INTERFACE_VERSION "SteamNetworking005"
#define STEAMREMOTESTORAGE_INTERFACE_VERSION "STEAMREMOTESTORAGE_INTERFACE_VERSION012"
#define STEAMSCREENSHOTS_INTERFACE_VERSION "STEAMSCREENSHOTS_INTERFACE_VERSION002"
#define STEAMHTTP_INTERFACE_VERSION "STEAMHTTP_INTERFACE_VERSION002"
#define STEAMUNIFIEDMESSAGES_INTERFACE_VERSION "STEAMUNIFIEDMESSAGES_INTERFACE_VERSION001"
#define STEAMCONTROLLER_INTERFACE_VERSION "STEAMCONTROLLER_INTERFACE_VERSION"
#define STEAMUGC_INTERFACE_VERSION "STEAMUGC_INTERFACE_VERSION003"
#define STEAMAPPLIST_INTERFACE_VERSION  "STEAMAPPLIST_INTERFACE_VERSION001"
#define STEAMMUSIC_INTERFACE_VERSION "STEAMMUSIC_INTERFACE_VERSION001"
#define STEAMMUSICREMOTE_INTERFACE_VERSION "STEAMMUSICREMOTE_INTERFACE_VERSION001"
#define STEAMHTMLSURFACE_INTERFACE_VERSION "STEAMHTMLSURFACE_INTERFACE_VERSION_002"


#undef __cdecl
#define __linux__
#define CALLBACK_GNUFIXUP
#include "steam_api.h"
#include "steam_gameserver.h"
#include "opensteamworks/IClientApps.h"
#include "opensteamworks/IClientUtilsSafe.h"
#include "opensteamworks/IClientUserSafe.h"
#include "steam_apix.h"
#include "../libudis86/extern.h"

#ifndef __cdecl
		#define __cdecl
#endif

#ifndef KEY_WOW64_32KEY
	#define KEY_WOW64_32KEY 0x0200
#endif

//#define USE_GS_AUTH_API
//csid = gameserver->GetSteamID();

class SteamGameClientBridge;

extern "C" void Com_SteamClientApi_DebugCallback(int nSeverity, const char *pchDebugText);

static int Com_sprintfUni(wchar_t *dest, size_t size, const wchar_t *fmt, ...)
{

	int			len, numchar;
	va_list		argptr;

	numchar = size / sizeof(wchar_t);

	va_start (argptr, fmt);
	
	len = _vsnwprintf(dest, numchar, fmt, argptr );
	va_end (argptr);

	return len;
}

//Required for the disassembler to find the valid pointer
void GetSectionBaseAndLen(unsigned char* imagestart, const char* sectionname, DWORD* sectiondata, DWORD* sectionlen)
{
	DWORD dwSectionSize;
	DWORD dwSectionBase;
	int i;
	IMAGE_SECTION_HEADER sectionHeader;
	IMAGE_DOS_HEADER dosHeader;
	IMAGE_NT_HEADERS ntHeader;
	int nSections;
	IMAGE_FILE_HEADER imageHeader;
	bool hasDoSHeader = false;

	dosHeader = *(IMAGE_DOS_HEADER*)(imagestart);

	if(dosHeader.e_magic == *(WORD*)"MZ")
	{
		hasDoSHeader = true;
	}else{
		//Error, only object files are without DOS header
		*sectionlen = 0;
		*sectiondata = 0;
		return;
	}

	//if(hasDoSHeader)
	{
		// get nt header
		ntHeader = *(IMAGE_NT_HEADERS*)((DWORD)imagestart + dosHeader.e_lfanew);
		imageHeader = ntHeader.FileHeader;
	}
	//else{
	//	imageHeader = *(IMAGE_FILE_HEADER*)(imagestart);
	//}

	nSections = imageHeader.NumberOfSections;

	// iterate through section headers and search for "sectionname" section
	for ( i = 0; i < nSections; i++)
	{
		if(hasDoSHeader)
		{
			sectionHeader = *(IMAGE_SECTION_HEADER*) ((DWORD)imagestart + dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS) + i*sizeof(IMAGE_SECTION_HEADER));
		}else{
			sectionHeader = *(IMAGE_SECTION_HEADER*) ((DWORD)imagestart + sizeof(IMAGE_FILE_HEADER) + i*sizeof(IMAGE_SECTION_HEADER));
		}
		if (strcmp((char*)sectionHeader.Name, sectionname) == 0)
		{
			// the values you need

			//Would be for pe-file
			//dwSectionBase = (DWORD)imagestart + sectionHeader.PointerToRawData;
			//dwSectionSize = sectionHeader.SizeOfRawData;

			//Using Virtual image
			dwSectionBase = (DWORD)imagestart + sectionHeader.VirtualAddress;
			dwSectionSize = sectionHeader.SizeOfRawData;

			break;
		}
	}

	if(i == nSections)
	{
		*sectionlen = 0;
		*sectiondata = 0;
	}else{
		*sectiondata = dwSectionBase;
		*sectionlen = dwSectionSize;
	}
}



class CCallbackMgr
{
	public:
		CCallbackMgr( );
		void SteamAPI_RegisterCallback( class CCallbackBase *pCallback, int iCallback );
		void SteamAPI_UnregisterCallback( class CCallbackBase *pCallback);
		void SteamAPI_RunCallbacks( class SteamAPI_Main* );
	private:
		std::list<class CCallbackBase*> callbacks;
//		std::recursive_mutex m_mutex;
		bool (__cdecl *Steam_BGetCallback)(HSteamPipe, CallbackMsg_t*);
		void (__cdecl *Steam_FreeLastCallback)(HSteamPipe);
		bool (__cdecl *Steam_GetAPICallResult)(HSteamPipe, SteamAPICall_t, void *, int, int, bool *);
};



class SteamAPI_Main: public CSteamAPIContext
{
public:

	//Constructor
	SteamAPI_Main(  const wchar_t* dllpath, AppId_t parentAppID, AppId_t gameId, const char* modName, bool overlayrenderer);

	// Destructor
	~SteamAPI_Main();

	bool InitAPI(  const wchar_t* dllpath, AppId_t parentAppID, AppId_t gameId, const char* modName);

	void ShutdownAPI( );

	void RunFrame( );
	HSteamUser GetHSteamUser();
	HSteamPipe GetHSteamPipe();
	HMODULE GetHSteamClient();
	const char* GetVtableImportAddress(uint32_t entrypoint);
	bool ParseVtable(InterfaceDesc_t* classptr, safeimportaccess_t* fields, uint32_t* semiclassbase);
	void ImportImageSectionInfo();
	bool LoadGameoverlayRenderer();
	ISteamClient* SteamClient();
	void SteamAPI_RegisterCallback( class CCallbackBase *pCallback, int iCallback );
	void SteamAPI_UnregisterCallback( class CCallbackBase *pCallback);
	bool IsInitialized();
	void SetAppIDToAuthAppID(bool settoauthappid);
	InterfaceDesc_t* GetInterface(const char* interfacename, HSteamPipe pipe, HSteamUser user);
//	bool FindInterfaceOldMethod();
	bool FindInterfaceNewMethod(void*);
	bool Steam3MasterFindClientUtils(const char* codeptr, void**);
	int BasicInitialization(wchar_t *dllfilepath, AppId_t parentAppID, AppId_t gameId);
private:
	// Track whether our server is connected to Steam ok (meaning we can restrict who plays based on
	// ownership and VAC bans, etc...)

	bool m_bConnectedToSteam;
	bool initialized;

	AppId_t appId;

	IClientUtils iClientUtils;
	IClientUser iClientUser;
	ISteamUtils *iSteamUtils;
	ISteamUser *iSteamUserlocal;
	HSteamPipe hSteamPipe;
	HSteamUser hSteamUser;
	ISteamClient* iSteamClient;
	HMODULE hSteamClientGlob;
	HMODULE hSteamOverlayRenderer;
	CCallbackMgr ccbmgr;
	//Text and data section offsets
	uint32_t textstart;
	uint32_t textsize;
	uint32_t rdatastart;
	uint32_t rdatasize;
friend class CCallbackMgr;
};


SteamAPI_Main *steamApiCtx;

//-----------------------------------------------------------------------------
// Purpose: Function which needs to getting processed on frame
//-----------------------------------------------------------------------------
void SteamAPI_Main::RunFrame( )
{
	if(!initialized)
	{
		return;
	}
	SteamUtils()->RunFrame( );
	SteamController()->RunFrame();
	ccbmgr.SteamAPI_RunCallbacks(this);
}

void SteamAPI_Main::ShutdownAPI(void)
{

	if ( hSteamPipe && hSteamUser )
	{
		iSteamClient->ReleaseUser(hSteamPipe, hSteamUser);
	}

	Clear();

	if ( hSteamPipe )
		iSteamClient->BReleaseSteamPipe(hSteamPipe);

	if ( iSteamClient )
		iSteamClient->BShutdownIfAllPipesClosed();

	if ( hSteamClientGlob )
	{
		FreeLibrary(hSteamClientGlob);
	}
	hSteamClientGlob = nullptr;

	initialized = false;
	appId = 0;
	hSteamPipe = 0;
	hSteamUser = 0;

	iSteamClient = nullptr;
	memset(&iClientUtils, 0, sizeof(iClientUtils));
	memset(&iClientUser, 0, sizeof(iClientUser));
	iSteamUtils = nullptr;
	iSteamUserlocal = nullptr;

	hSteamClientGlob = nullptr;
}

SteamAPI_Main::~SteamAPI_Main()
{
	ShutdownAPI();
}

ISteamClient* SteamAPI_Main::SteamClient()
{
	return iSteamClient;
}

HSteamUser SteamAPI_Main::GetHSteamUser()
{
	return hSteamUser;
}
HSteamPipe SteamAPI_Main::GetHSteamPipe()
{
	return hSteamPipe;
}

HMODULE SteamAPI_Main::GetHSteamClient()
{
	return hSteamClientGlob;
}



HKEY __cdecl SteamAPI_RegistryNameToHandle(const char *a1)
{
  if ( !_stricmp(a1, "HKEY_LOCAL_MACHINE") || !_stricmp(a1, "HKLM") )
  {
    return HKEY_LOCAL_MACHINE;
  }
  else if ( !_stricmp(a1, "HKEY_CURRENT_USER") || !_stricmp(a1, "HKCU") )
  {
    return HKEY_CURRENT_USER;
  }
  else if ( !_stricmp(a1, "HKEY_CLASSES_ROOT") || !_stricmp(a1, "HKCR") )
  {
    return HKEY_CLASSES_ROOT;
  }
  return  0;
}

HMODULE __cdecl SteamApi_LoadClientDll(LPCSTR libname)
{
  HMODULE h;
  unsigned int numChar;
  wchar_t wpath[4096];

  numChar = MultiByteToWideChar(0xFDE9u, 0, libname, -1, wpath, sizeof(wpath) / 2);
  if ( numChar < 1 )
  {
		if(!CheckSteamLibraryA(libname))
		{
			return nullptr;
		}
		return LoadLibraryExA(libname, 0, 8u);
  }

	if(!CheckSteamLibraryW(wpath))
	{
		return nullptr;
	}
  
	h = LoadLibraryExW(wpath, 0, 8u);
  if(h)
  {
		return h;
  }
  
	if(!CheckSteamLibraryA(libname))
	{
		return nullptr;
	}
	return LoadLibraryExA(libname, 0, 8u);

}


bool SteamAPI_GetSteamClientDLLPath(wchar_t *steamclientpath, size_t len)
{
	DWORD type;
	DWORD cbData;
	HKEY hkey;
	DWORD state;

    cbData = len;
    hkey = 0;
	steamclientpath[0] = L'\0';

    if(RegOpenKeyExA(SteamAPI_RegistryNameToHandle("HKCU"), "Software\\Valve\\Steam\\ActiveProcess", 0, KEY_READ | KEY_WOW64_32KEY , &hkey) != ERROR_SUCCESS)
    {
		return 0;
    }

	state = RegQueryValueExW(hkey, L"SteamClientDll", 0, &type, (LPBYTE)steamclientpath, &cbData);
	RegCloseKey(hkey);

	if( state != ERROR_SUCCESS)
	{
		return 0;
	}
	steamclientpath[len -1] = '\0';

	wchar_t* find, *nextfind;

	find = nullptr;
	nextfind = wcsstr(steamclientpath, L"steamclient.dll");

	while(nextfind)
	{
		find = nextfind;
		nextfind++;
		nextfind = wcsstr(nextfind, L"steamclient.dll");
	}

	if(find)
	{
		find[-1] = L'\0';
	}else{
		return 0;
	}
	return 1;
}


 bool SteamAPI_Main::LoadGameoverlayRenderer()
{
	wchar_t steamclientpath[1024];
	wchar_t overlayRendererDllPath[1024];

	hSteamOverlayRenderer = GetModuleHandleW(L"gameoverlayrenderer.dll");

	if ( hSteamOverlayRenderer )
	{
		return 1;
	}

	SteamAPI_GetSteamClientDLLPath(steamclientpath, sizeof(steamclientpath) / sizeof(wchar_t));
	Com_sprintfUni(overlayRendererDllPath, sizeof(overlayRendererDllPath), L"%s\\%s", steamclientpath, L"gameoverlayrenderer.dll");
    hSteamOverlayRenderer = LoadLibraryExW(overlayRendererDllPath, 0, 8);

    if ( !hSteamOverlayRenderer )
    {
		return 0;
    }
	return 1;
}

typedef void* (__cdecl *createInterface_t)(const char *, int *) ;


bool SteamAPI_Main::IsInitialized()
{
	return initialized;
}






bool SteamAPI_Main::Steam3MasterFindClientUtils(const char* codeptr, void** interfaceconstructors)
{
//	InterfaceDesc_t* (*funcstart)(HSteamUser, HSteamPipe) ;

	ud_t ud_obj;
	const ud_operand_t* operant1;
	enum ud_mnemonic_code mnemonic;
	
	if(textstart == 0 || rdatastart == 0)
	{
		return false;
	}

	ud_init(&ud_obj);

	ud_set_mode(&ud_obj, 32);
	ud_set_vendor(&ud_obj, UD_VENDOR_ANY);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	ud_set_pc(&ud_obj, (uint64_t)codeptr);

	ud_set_input_buffer(&ud_obj, (uint8_t*)codeptr, 1024);

	int state = 0;
	int gapcounter = 0;
	int createinterfacefunccnt = 0;
	while (ud_disassemble(&ud_obj))
	{

		mnemonic = ud_insn_mnemonic(&ud_obj);

		if (mnemonic == UD_Iret || mnemonic == UD_Iretf || mnemonic == UD_Iint3 || mnemonic == UD_Iinvalid)
		{
			break;
		}
		if(state == 0)
		{
			++state;
			if(mnemonic == UD_Ipush)
			{
				continue;
			}
			return false;
		}
		if(state == 1)
		{
			++state;
			if(mnemonic == UD_Icall)
			{
				continue;
			}
			return false;
		}
		if(state == 2)
		{
			if(mnemonic == UD_Itest)
			{
				++state;
				gapcounter = 0;
			}
			if(gapcounter > 10)
			{ //something seems to be very wrong
				return false;
			}
			++gapcounter;
		}
		if(state == 3)
		{
			if(mnemonic == UD_Ijnz)
			{
				//have to follow jump
				signed int displacement;
				operant1 = ud_insn_opr(&ud_obj, 0);
				if(operant1->size == 32)
				{
					displacement = operant1->lval.sdword;
				}else{
					displacement = operant1->lval.sbyte;
				}
				ud_input_skip(&ud_obj, displacement);
				++state;
				gapcounter = 0;
				continue;
			}
			if(mnemonic == UD_Ijz)
			{
				++state;
				gapcounter = 0;
				continue;
			}
			if(gapcounter > 3)
			{ //something seems to be very wrong
				return false;
			}
			++gapcounter;
		}

		if(state == 4)
		{
			if(createinterfacefunccnt >= 5)
			{
				return true;
			}
			if(mnemonic == UD_Ipush)
			{
				operant1 = ud_insn_opr(&ud_obj, 0);
				if(operant1->type == UD_OP_IMM)
				{
					if(operant1->lval.udword == 0)
					{
						++state;
						gapcounter = 0;
						continue;
					}else{
						return false;
					}
				}
			}

		}
		if(state == 5)
		{
			if(mnemonic == UD_Icall)
			{
				operant1 = ud_insn_opr(&ud_obj, 0);
				if(operant1->type == UD_OP_JIMM && operant1->size == 32)
				{
					interfaceconstructors[createinterfacefunccnt] = (void*)(ud_obj.pc + operant1->lval.sdword);
					++createinterfacefunccnt;
					state = 4;
					continue;
				}
				
			}
		}



/*
			if(mnemonic == UD_Imov)
			{
				operant1 = ud_insn_opr(&ud_obj, 0);
				operant2 = ud_insn_opr(&ud_obj, 1);

				//catch mov dword [eax+12], xxx
				if(operant1->type == UD_OP_MEM && operant1->base == UD_R_EAX && operant1->size == 32 && operant1->lval.udword == 12)
				{
					checkflags |= 8;
				}

				//catch mov dword [eax+8], xxx
				if(operant1->type == UD_OP_MEM && operant1->base == UD_R_EAX && operant1->size == 32 && operant1->lval.udword == 8)
				{
					checkflags |= 4;
				}

				//catch mov dword [eax+4], imm32
				if(operant1->type == UD_OP_MEM && operant1->base == UD_R_EAX && operant1->size == 32 && operant1->lval.udword == 4
					&& operant2->type == UD_OP_IMM && operant2->size == 32)
				{
					checkflags |= 2;
					detectname = (const char*)operant2->lval.udword; //check
				}
				//catch mov dword [eax], imm32
				if(operant1->type == UD_OP_MEM && operant1->base == UD_R_EAX && operant1->size == 32 && operant1->lval.udword == 0
					&& operant2->type == UD_OP_IMM && operant2->size == 32)
				{
					checkflags |= 1;
					detectvtable = (void*)operant2->lval.udword;//check
				}

			}
			if(checkflags == 15) //passed the basic test
			{
				if((uint32_t)detectname >= rdatastart && (uint32_t)detectname < rdatastart + rdatasize)
				{
					if((uint32_t)detectvtable >= rdatastart && (uint32_t)detectvtable < rdatastart + rdatasize)
					{
						if(strcmp(interfacename, detectname) == 0)
						{
							return funcstart(user, pipe);
						}
						break;
					}
				}
			}
*/
	}



	return false;
}


bool SteamAPI_Main::FindInterfaceNewMethod(void* engineInterface)
{
	void* interfacefuncs[10];
	unsigned int i;
	const char* base = (char*)rdatastart;

	for( i = 0; i < rdatasize -13; ++i, ++base)
	{
		if(strcmp(base, "Steam3Master") == 0)
		{
			break;
		}
	}
	if(i == rdatasize -13)
	{
		return false;
	}
	DWORD stringaddress = (DWORD)base;
	base = (char*)textstart;
	char findsignature[6];
	findsignature[0] = 0x68; //push constant
	*(DWORD*)(findsignature +1) = stringaddress;
	findsignature[5] = 0xe8; //call
	memset(interfacefuncs, 0, sizeof(interfacefuncs));
	for(i = 0; i < textsize; ++i, ++base)
	{
		if(memcmp(base, findsignature, sizeof(findsignature)) == 0)
		{
			if(Steam3MasterFindClientUtils(base, interfacefuncs) == true)
			{
				break;
			}
		}
	}
	for(i = 0; i < 6 && interfacefuncs[i]; ++i)
	{
		interfacefuncs[i] = ((void* (*)(HSteamUser, HSteamPipe))interfacefuncs[i])(hSteamUser, hSteamPipe);
		Com_PrintError(CON_CHANNEL_SYSTEM, "Grabbed interface!\n");
	}
	InterfaceDesc_t* ClientUtils_class;

	for(i = 0; i < 6 && interfacefuncs[i]; ++i)
	{
		ClientUtils_class = (InterfaceDesc_t*)interfacefuncs[i];
		if(ParseVtable(ClientUtils_class, iClientUtilsFields, (uint32_t*)&iClientUtils))
		{
			break;
		}
	}
	if(i == 6 || interfacefuncs[i] == NULL)
	{
		ShutdownAPI();
		Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed (no IClientUtils)\n");
		return false;
	}
	return true;

}
/*
bool SteamAPI_Main::FindInterfaceOldMethod()
{
	InterfaceDesc_t* ClientUtils_class = GetInterface("IClientUtils", hSteamPipe, 0);
	
	if(!ParseVtable(ClientUtils_class, iClientUtilsFields, (uint32_t*)&iClientUtils))
	{
		ShutdownAPI();
		Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed (no IClientUtils)\n");
		return false;
	}


	InterfaceDesc_t* ClientUser_class = GetInterface("IClientUser", hSteamPipe, hSteamUser);

	if(!ParseVtable(ClientUser_class, iClientUserFields, (uint32_t*)&iClientUser))
	{
		ShutdownAPI();
		Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed (no IClientUser)\n");
		return false;
	}
	return true;
}
*/


int SteamAPI_Main::BasicInitialization(wchar_t *steamClientDllPath, AppId_t parentAppID, AppId_t gameId)
{
	int retval;
	char envValue[128];
	createInterface_t CreateInterfaceFunction;

	sprintf(envValue, "%u", parentAppID);
	SetEnvironmentVariableA("SteamAppId", envValue);
	sprintf(envValue, "%u", gameId & 0xFFFFFF);
	SetEnvironmentVariableA("SteamGameId", envValue);

	if ( !hSteamClientGlob)
	{
		hSteamClientGlob = LoadLibraryExW(steamClientDllPath, 0, 8);
	}
	if ( !hSteamClientGlob )
	{
		Com_Printf(CON_CHANNEL_SYSTEM, "[S_API FAIL] SteamAPI_Init() failed. Steam is either not running or installed\n");
		return 0;
	}

	CreateInterfaceFunction = (void*  (__cdecl *)(const char *, int *))GetProcAddress(hSteamClientGlob, "CreateInterface");

	if ( !CreateInterfaceFunction )
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed (No CreateInterface function)\n");
		return 0;
	}


	/* Get some of the interfaces */
	/* Official */
	iSteamClient = (ISteamClient*)CreateInterfaceFunction("SteamClient016", &retval);
	if(iSteamClient == nullptr)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed (Interface missing)\n");
		return 0;
	}

	hSteamPipe =  iSteamClient->CreateSteamPipe();

	if ( !hSteamPipe )
	{
		ShutdownAPI();
		Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed (no hSteamPipe)\n");
		return 0;
	}

	hSteamUser = iSteamClient->ConnectToGlobalUser(hSteamPipe);

	if ( !hSteamUser )
	{
		ShutdownAPI();
		return -1;
	}
	return 1;
}



SteamAPI_Main::SteamAPI_Main(  const wchar_t* dllpath, AppId_t parentAppID, AppId_t gameId, const char* productName, bool overlayrenderer)
{
	wchar_t dllfilepath[2048];
	wchar_t steamClientDllPath[2048];

	m_bConnectedToSteam = false;
	initialized = false;
	appId = 0;
	hSteamPipe = 0;
	hSteamUser = 0;
	signed int rstatus;

	iSteamClient = nullptr;
	memset(&iClientUtils, 0, sizeof(iClientUtils));
	memset(&iClientUser, 0, sizeof(iClientUser));

	iSteamUtils = nullptr;
	iSteamUserlocal = nullptr;

	hSteamClientGlob = nullptr;

	if ( !hSteamClientGlob){
		if(!SteamAPI_GetSteamClientDLLPath(dllfilepath, sizeof(dllfilepath) / 2))
		{
			Com_Printf(CON_CHANNEL_SYSTEM, "[S_API FAIL] SteamAPI_Init() failed. Steam appears to be not installed\n");
			return;
		}
		Com_sprintfUni(steamClientDllPath, sizeof(steamClientDllPath), L"%s\\%s", dllfilepath, L"steamclient.dll");
	} 


	switch(BasicInitialization(steamClientDllPath, parentAppID, gameId))
	{
		case 1:
			break;
		case 0:
			return;
		case -1:
			rstatus = BasicInitialization(steamClientDllPath, 42750, gameId);
			if(rstatus != 1)
			{
				if(rstatus == -1){
					Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed (bad hSteamUser)\n");
				}
				return;
			}
			break;
	}


#if 0
	/* Unofficial */
	engineInterface = (void*)CreateInterfaceFunction("CLIENTENGINE_INTERFACE_VERSION005", &retval);
	if(engineInterface == NULL)
	{
		engineInterface = (void*)CreateInterfaceFunction("CLIENTENGINE_INTERFACE_VERSION006", &retval);
	}
	if(engineInterface == NULL)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed (Interface missing)\n");
		return;
	}
#endif

//	ImportImageSectionInfo();



/*
	if(FindInterfaceNewMethod(NULL) == false)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed (bad Interface)\n");
		ShutdownAPI();
		return;
	}

*/
    iSteamUserlocal = iSteamClient->GetISteamUser(hSteamPipe, hSteamUser, "SteamUser017");
	iSteamUtils = iSteamClient->GetISteamUtils(hSteamPipe, "SteamUtils007");


	if(!iSteamUserlocal || !iSteamUtils)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed (no iSteamUserlocal or iSteamUtils)\n");
		ShutdownAPI();
		return;
	}
    /* Init for OpenSteamWorks ClientEngine */


	char ourDirectory[1024];
	char cmdline[1024];
	const char* exefile;
/*
	if(Sys_IsTempInstall())
	{
		ourDirectory[0] = '\0';
		GetSystemDirectoryA(ourDirectory, sizeof(ourDirectory));

		if(ourDirectory[0] && ourDirectory[strlen(ourDirectory) -1] == '\\')
		{
			ourDirectory[strlen(ourDirectory) -1] = '\0';
		}
		Com_sprintf(cmdline, sizeof(cmdline), "%s\\%s", ourDirectory, "rundll32.exe");
		exefile = cmdline;
	}

	AppId_t fakeappid = 42750;
	// create a fake app to hold our gameid
	uint64_t gameID = 0xACCF2DAB01000000 | fakeappid; // crc32 for 'kekking' + mod
	CGameID cgameid(gameID);

	AppId_t returnedappid = iClientUtils.SetAppIDForCurrentPipe(iClientUtils.pclass, parentAppID, 1 ) ;



	if(returnedappid != parentAppID)
	{
//		iClientUser.SpawnProcess(iClientUser.pclass, exefile, exefile, ourDirectory, cgameid, productName, 0, 0, 0);
		returnedappid = iClientUtils.SetAppIDForCurrentPipe(iClientUtils.pclass, fakeappid, 1 ) ; //reserve appid for unlicensed cod4
	}
	if(returnedappid == 0)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Steam: SteamAPI_Init() failed - couldn't initialize Steam api with both appids\n");
		ShutdownAPI();
		return;
	}
*/
	/* End init for OpenSteamWorks */

#if 0
  iSteamClient->Set_SteamAPI_CPostAPIResultInProcess(iSteamClient, sub_3B403800);
#endif

	if(overlayrenderer == true)
	{
		if(LoadGameoverlayRenderer() == false)
		{
			Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't load the gameoverlay\n");
		}
	}
	initialized = true;
	return;
}


CCallbackMgr::CCallbackMgr()
{
	callbacks.empty();
	Steam_BGetCallback = nullptr;
	Steam_FreeLastCallback = nullptr;
	Steam_GetAPICallResult = nullptr;
}


void CCallbackMgr::SteamAPI_RunCallbacks(SteamAPI_Main* sam)
{
//	mutex.lock;
	CallbackMsg_t callbackMsg;

	std::list<class CCallbackBase *>::iterator iter;

	if(Steam_BGetCallback == nullptr)
	{
		Steam_BGetCallback = (bool (__cdecl *)(HSteamPipe, CallbackMsg_t*))GetProcAddress(sam->GetHSteamClient(), "Steam_BGetCallback");
	}
	if(Steam_FreeLastCallback == nullptr)
	{
		Steam_FreeLastCallback = (void (__cdecl *)(HSteamPipe))GetProcAddress(sam->GetHSteamClient(), "Steam_FreeLastCallback");
	}
	if(Steam_GetAPICallResult == nullptr)
	{
		Steam_GetAPICallResult = (bool (__cdecl *)(HSteamPipe, SteamAPICall_t, void *, int, int, bool *))GetProcAddress(sam->GetHSteamClient(), "Steam_GetAPICallResult");
	}

	if(Steam_BGetCallback == nullptr || Steam_FreeLastCallback == nullptr || Steam_GetAPICallResult == nullptr)
	{
		return;
	}

	while(Steam_BGetCallback(sam->GetHSteamPipe(), &callbackMsg))
	{
		for(iter = callbacks.begin(); iter != callbacks.end(); ++iter)
		{

			if((*iter)->m_iCallback == callbackMsg.m_iCallback)
			{

				(*iter)->Run(callbackMsg.m_pubParam);
			}


		}

		Steam_FreeLastCallback(sam->GetHSteamPipe());
	}

//	mutex.unlock;

}

void SteamAPI_Main::SteamAPI_RegisterCallback( class CCallbackBase *pCallback, int iCallback )
{
	ccbmgr.SteamAPI_RegisterCallback(pCallback, iCallback);
}

void CCallbackMgr::SteamAPI_RegisterCallback( class CCallbackBase *pCallback, int iCallback )
{
	class CCallbackBase *fx = pCallback;

	pCallback->m_nCallbackFlags |= 1u;
	pCallback->m_iCallback = iCallback;

//	mutex.lock;

	for (const auto it : callbacks)
		if(it == fx)
		{
			Com_PrintError(CON_CHANNEL_SYSTEM, "Attempted to register callback twice. Aborting...\n");
	//		mutex.unlock;
			return;
		}

	callbacks.empty();
	callbacks.push_front(pCallback);

//	mutex.unlock;

	//Com_Printf("Register callback %d %d\n", iCallback, pCallback->GetICallback());

}

void SteamAPI_Main::SteamAPI_UnregisterCallback( class CCallbackBase *pCallback)
{
	ccbmgr.SteamAPI_UnregisterCallback(pCallback);
}

void CCallbackMgr::SteamAPI_UnregisterCallback( class CCallbackBase *pCallback)
{
//	mutex.lock;
	if(pCallback->m_nCallbackFlags & 1)
	{
		pCallback->m_nCallbackFlags &= 0xFE;
		callbacks.remove(pCallback);
	}
//	mutex.unlock;
}

void SteamAPI_Main::ImportImageSectionInfo()
{
	GetSectionBaseAndLen((unsigned char*)hSteamClientGlob, ".text", (DWORD*)&textstart, (DWORD*)&textsize);
	GetSectionBaseAndLen((unsigned char*)hSteamClientGlob, ".rdata", (DWORD*)&rdatastart, (DWORD*)&rdatasize);
}


bool IsValidFunctionName(const char* name)
{
	int i, j;

	for(i = 0; i < 64; ++i)
	{
		if(name[i] == 0)
		{
			break;
		}
	}

	if(i == 64 || i < 4)
	{
		return false;
	}

	for(j = 0; j < i -1; ++j)
	{
		if(!isalnum(name[j]))
		{
			return false;
		}
	}
	return true;
}


InterfaceDesc_t* SteamAPI_Main::GetInterface(const char* interfacename, HSteamPipe pipe, HSteamUser user)
{
	unsigned int i;
	const unsigned char firstsig[] = {0x55, 0x8b, 0xec, 0x6a, 0x10, 0xe8};
	const unsigned char secondsig[] = {0x83, 0xc4, 0x04, 0x85, 0xc0};
	unsigned int checkflags;
	const char* detectname;
	void* detectvtable;
	InterfaceDesc_t* (*funcstart)(int, HSteamPipe) ;

	ud_t ud_obj;
	const ud_operand_t* operant1;
	const ud_operand_t* operant2;
	enum ud_mnemonic_code mnemonic;
	
	if(textstart == 0 || rdatastart == 0)
	{
		return nullptr;
	}


	for(i = 0; i < textsize; ++i)
	{
		if(((const unsigned char*)textstart + i)[0] != firstsig[0] || memcmp((const unsigned char*)textstart + i, firstsig, sizeof(firstsig)) != 0)
		{
			continue;
		}
		funcstart = (InterfaceDesc_t*(*)(int, HSteamPipe))(textstart + i);
		i += sizeof(firstsig);
		i += sizeof(DWORD);

		if(memcmp((const unsigned char*)textstart + i, secondsig, sizeof(secondsig)) != 0)
		{
			continue;
		}
		i += sizeof(secondsig);
			
		ud_init(&ud_obj);

		ud_set_mode(&ud_obj, 32);
		ud_set_vendor(&ud_obj, UD_VENDOR_ANY);
		ud_set_syntax(&ud_obj, UD_SYN_INTEL);
		ud_set_pc(&ud_obj, textstart + i);

		ud_set_input_buffer(&ud_obj, (uint8_t*)textstart + i, 0x30);

		checkflags = 0;
		detectname = NULL;
		detectvtable = 0;

		while (ud_disassemble(&ud_obj))
		{

			mnemonic = ud_insn_mnemonic(&ud_obj);

			if (mnemonic == UD_Iret || mnemonic == UD_Iretf || mnemonic == UD_Iint3 || mnemonic == UD_Iinvalid)
			{
				break;
			}
			if(mnemonic == UD_Imov)
			{
				operant1 = ud_insn_opr(&ud_obj, 0);
				operant2 = ud_insn_opr(&ud_obj, 1);

				//catch mov dword [eax+12], xxx
				if(operant1->type == UD_OP_MEM && operant1->base == UD_R_EAX && operant1->size == 32 && operant1->lval.udword == 12)
				{
					checkflags |= 8;
				}

				//catch mov dword [eax+8], xxx
				if(operant1->type == UD_OP_MEM && operant1->base == UD_R_EAX && operant1->size == 32 && operant1->lval.udword == 8)
				{
					checkflags |= 4;
				}

				//catch mov dword [eax+4], imm32
				if(operant1->type == UD_OP_MEM && operant1->base == UD_R_EAX && operant1->size == 32 && operant1->lval.udword == 4
					&& operant2->type == UD_OP_IMM && operant2->size == 32)
				{
					checkflags |= 2;
					detectname = (const char*)operant2->lval.udword; //check
				}
				//catch mov dword [eax], imm32
				if(operant1->type == UD_OP_MEM && operant1->base == UD_R_EAX && operant1->size == 32 && operant1->lval.udword == 0
					&& operant2->type == UD_OP_IMM && operant2->size == 32)
				{
					checkflags |= 1;
					detectvtable = (void*)operant2->lval.udword;//check
				}

			}
			if(checkflags == 15) //passed the basic test
			{
				if((uint32_t)detectname >= rdatastart && (uint32_t)detectname < rdatastart + rdatasize)
				{
					if((uint32_t)detectvtable >= rdatastart && (uint32_t)detectvtable < rdatastart + rdatasize)
					{
						if(strcmp(interfacename, detectname) == 0)
						{
							return funcstart(user, pipe);
						}
						break;
					}
				}
			}

		}


	}

	return NULL;
}
 

bool SteamAPI_Main::ParseVtable(InterfaceDesc_t* classptr, safeimportaccess_t* fields, uint32_t* semiclassbase)
{
    int i, j;
	const char* funcname;

	if(classptr == nullptr)
	{
		return false;
	}

	uint32_t* base = (uint32_t*)classptr->vtable;

	for( i = 0; (uint32_t)&base[i] < rdatastart + rdatasize && base[i] >= textstart && base[i] < textstart + textsize; ++i)
	{
		funcname = GetVtableImportAddress(base[i]);
//		Com_Printf("Proc: %s  Entry 0x%x\n", funcname, base[i]);
		if(funcname == nullptr)
		{
			continue;
		}

		for(j = 0; fields[j].name; ++j)
		{
			if(strcmp(fields[j].name, funcname) == 0)
			{
				semiclassbase[fields[j].index] = base[i];
				//Com_Printf("Found %s Offset %d Address 0x%x\n", fields[j].name, fields[j].index, base[i]);
			}
		}
	}

	for(i = 0; fields[i].name; ++i)
	{
		if(semiclassbase[i] == 0)
		{
			return false;
		}
	}
	semiclassbase[i] = (uint32_t)classptr;
	return true;

}

const char* SteamAPI_Main::GetVtableImportAddress(uint32_t entrypoint)
{
	ud_t ud_obj;
	const ud_operand_t* operant;
	enum ud_mnemonic_code mnemonic;

	if(textstart == 0 || rdatastart == 0 || entrypoint < textstart || entrypoint >= textstart + textsize)
	{
		return nullptr;
	}

	ud_init(&ud_obj);

	ud_set_mode(&ud_obj, 32);
	ud_set_vendor(&ud_obj, UD_VENDOR_ANY);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	ud_set_pc(&ud_obj, entrypoint);

	ud_set_input_buffer(&ud_obj, (uint8_t*)entrypoint, 4096);

	while (ud_disassemble(&ud_obj))
	{

		mnemonic = ud_insn_mnemonic(&ud_obj);

		//Com_Printf("%s\n", ud_insn_asm(&ud_obj));

		if (mnemonic == UD_Iret || mnemonic == UD_Iretf || mnemonic == UD_Iint3 || mnemonic == UD_Iinvalid)
		{
			break;
		}

		if(mnemonic == UD_Ipush)
		{
			operant = ud_insn_opr(&ud_obj, 0);
			if(operant && operant->type == UD_OP_IMM && operant->size == 32)
			{
				if(operant->lval.udword >= rdatastart && operant->lval.udword < rdatastart + rdatasize && IsValidFunctionName((const char*)operant->lval.udword))
				{
					const char* value1 = (const char*)operant->lval.udword;
					ud_disassemble(&ud_obj);
					mnemonic = ud_insn_mnemonic(&ud_obj);
					if(operant->lval.udword >= rdatastart && operant->lval.udword < rdatastart + rdatasize && IsValidFunctionName((const char*)operant->lval.udword))
					{
						if(strncmp((const char*)operant->lval.udword, "IClient", 7) == 0)
						{
							return value1;
						}
					}
				}
			}
		}

	}

	return nullptr;
}




extern "C" {

	void SteamAPI_Shutdown( )
	{
		if(steamApiCtx == nullptr)
		{
			return;
		}
		steamApiCtx->ShutdownAPI();
	}


	void SteamAPI_RunCallbacks(  )
	{
		if(steamApiCtx == nullptr)
		{
			return;
		}
		steamApiCtx->RunFrame(  );
	}

	void SteamAPI_RegisterCallback( class CCallbackBase *pCallback, int iCallback )
	{
		if(steamApiCtx == nullptr)
		{
			return;
		}
		steamApiCtx->SteamAPI_RegisterCallback( pCallback, iCallback );
	}

	void SteamAPI_UnregisterCallback( class CCallbackBase *pCallback )
	{
		if(steamApiCtx == nullptr)
		{
			return;
		}
		steamApiCtx->SteamAPI_UnregisterCallback( pCallback );
		Com_Printf(CON_CHANNEL_SYSTEM, "Unregister callback %d\n", 0);


	}

	HANDLE steamInitThread;

	LONG WINAPI Steam_SignalHandler(EXCEPTION_POINTERS *exceptptr)
	{
		steamApiCtx = nullptr;
		TerminateThread(steamInitThread, -1);
		Com_PrintError(CON_CHANNEL_SYSTEM, "SteamAPI init thread has crashed\n");
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	struct threadargs_t
	{
		wchar_t dllpath[1024];
		AppId_t parentAppID;
		AppId_t gameId;
		char modName[256];
		bool overlayrenderer;
	};

	void* SteamAPI_InitInternal( void* a )
	{
		struct threadargs_t* args = (struct threadargs_t*)a;

		steamApiCtx = new SteamAPI_Main( args->dllpath, args->parentAppID, args->gameId, args->modName, args->overlayrenderer);

		if(steamApiCtx && steamApiCtx->IsInitialized() && steamApiCtx->Init())
		{
			return (void*)1;
		}

		delete steamApiCtx;

		steamApiCtx = nullptr;
		return nullptr;
	}

	bool SteamAPI_InitX(const wchar_t* dllpath, AppId_t parentAppID, AppId_t gameId, const char* modName, bool overlayrenderer)
	{
		

		static struct threadargs_t args;

		Com_sprintfUni(args.dllpath, sizeof(args.dllpath), L"%s", dllpath);
		Q_strncpyz(args.modName, modName ,sizeof(args.modName));
		args.gameId = gameId;
		args.parentAppID = parentAppID;
		args.overlayrenderer = overlayrenderer;
#ifdef DEBUG
		SteamAPI_InitInternal(&args);
#else
		LPTOP_LEVEL_EXCEPTION_FILTER oldhandler = SetUnhandledExceptionFilter(Steam_SignalHandler);


		steamInitThread = CreateThread(nullptr, //Choose default security
							1024*1024*2, //stack size
							(LPTHREAD_START_ROUTINE)&SteamAPI_InitInternal, //Routine to execute
							&args, //Thread parameter
							0, //Immediately run the thread
							nullptr //Thread Id
						);

		if(steamInitThread == nullptr)
		{
			Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't run SteamAPI init thread\n");
		}else{
			if(WaitForSingleObject(steamInitThread, 20000) == WAIT_TIMEOUT)
			{
				TerminateThread(steamInitThread, -1);
				steamApiCtx = nullptr;
				Com_PrintError(CON_CHANNEL_SYSTEM, "Timeout hit while waiting for SteamAPI init thread\n");
			}
		}
		SetUnhandledExceptionFilter(oldhandler);
#endif
		if(steamApiCtx)
		{
			return 1;
		}
		return 0;
	}


	ISteamClient* SteamClient(  )
	{
		if(steamApiCtx == nullptr)
		{
			return nullptr;
		}
		return steamApiCtx->SteamClient();
	}

	HSteamUser SteamAPI_GetHSteamUser()
	{
		if(steamApiCtx == nullptr)
		{
			return 0;
		}
		return steamApiCtx->GetHSteamUser();
	}

	HSteamPipe SteamAPI_GetHSteamPipe()
	{
		if(steamApiCtx == nullptr)
		{
			return 0;
		}
		return steamApiCtx->GetHSteamPipe();
	}


	ISteamUser*			SteamUser()							{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamUser(); }
	ISteamFriends*		SteamFriends()						{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamFriends(); }
	ISteamUtils*		SteamUtils()						{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamUtils(); }
	ISteamMatchmaking*	SteamMatchmaking()					{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamMatchmaking(); }
	ISteamUserStats*	SteamUserStats()					{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamUserStats(); }
	ISteamApps*			SteamApps()							{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamApps(); }
	ISteamMatchmakingServers*	SteamMatchmakingServers()	{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamMatchmakingServers(); }
	ISteamNetworking*	SteamNetworking()					{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamNetworking(); }
	ISteamRemoteStorage* SteamRemoteStorage()				{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamRemoteStorage(); }
	ISteamScreenshots*	SteamScreenshots()					{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamScreenshots(); }
	ISteamHTTP*			SteamHTTP()							{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamHTTP(); }
	ISteamUnifiedMessages*	SteamUnifiedMessages()			{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamUnifiedMessages(); }
	ISteamController*	SteamController()					{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamController(); }
	ISteamUGC*			SteamUGC()							{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamUGC(); }
	ISteamAppList*		SteamAppList()						{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamAppList(); }
	ISteamMusic*		SteamMusic()						{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamMusic(); }
	ISteamMusicRemote*	SteamMusicRemote()					{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamMusicRemote(); }
	ISteamHTMLSurface*	SteamHTMLSurface()					{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamHTMLSurface(); }

#ifdef _PS3
	ISteamPS3OverlayRender* SteamPS3OverlayRender()		{ if(steamApiCtx == nullptr){	return nullptr; } return steamApiCtx->SteamPS3OverlayRender(); }
#endif
}


int CheckSteamLibraryW(const wchar_t* szFilename)
{
	
    DWORD dummy;
    DWORD dwSize = GetFileVersionInfoSizeW(szFilename, &dummy);
	
    if (dwSize == 0)
    {
        return 0;
    }
	
	char* data = new char[dwSize];
	
	// load the version info
  if (!GetFileVersionInfoW(szFilename, 0, dwSize, data))
  {
    return 0;
  }

	unsigned int compsize;
	const char* compname;
	
	
	if(!VerQueryValueA(data, "\\StringFileInfo\\040904B0\\CompanyName", (void**)&compname, &compsize))
	{
		delete data;
		return 0;
	}
	
	if(strncmp(compname, "Valve Corporation", 17))
	{
		delete data;
		return 0;
	}
		
	delete data;
	return 1;
}

int CheckSteamLibraryA(const char* szFilename)
{
	
    DWORD dummy;
    DWORD dwSize = GetFileVersionInfoSizeA(szFilename, &dummy);
	
    if (dwSize == 0)
    {
        return 0;
    }
	
	char* data = new char[dwSize];
	
	// load the version info
  if (!GetFileVersionInfoA(szFilename, 0, dwSize, data))
  {
    return 0;
  }

	unsigned int compsize;
	const char* compname;
	
	
	if(!VerQueryValueA(data, "\\StringFileInfo\\040904B0\\CompanyName", (void**)&compname, &compsize))
	{
		delete data;
		return 0;
	}
	
	if(strncmp(compname, "Valve Corporation", 17))
	{
		delete data;
		return 0;
	}
		
	delete data;
	return 1;
}


/*
Only junk here


void SSetCall(DWORD addr, void* destination){

	DWORD callwidth;
	byte* baddr = (byte*)addr;

	callwidth = (DWORD)( (DWORD)destination - (DWORD)baddr - 5);
	*baddr = 0xe8;
	baddr++;

	*(DWORD*)baddr = callwidth;
}

void SSetJump(DWORD addr, void* destination){

	DWORD jmpwidth;
	byte* baddr = (byte*)addr;

	jmpwidth = (DWORD)(  (DWORD)destination -  (DWORD)baddr - 5);
	*baddr = 0xe9;
	baddr++;

	*(DWORD*)baddr = jmpwidth;
}


typedef struct{
	byte gap0[4];
	byte* data;
	byte gap8[8];
	int dataReadBytes;
	int numBytes;
} pipebuffer;



void *write2pipestart = (void*)0x382DF790;
byte pipefncstartbytes[5];

char __thiscall WriteToPipe(void *thiss, pipebuffer *pipeBuf, char a3)
{
	byte firstcmd;
	int numBytes;
	byte* data;
	int lenData;
	char buffer[256];
	byte advertcmd[] = {0x01, 0x01, 0x00 , 0x00 , 0x00 , 0xB4 , 0x01 , 0x00 , 0x00 , 0xFE , 0xA6 , 0x00 , 0x01 , 0xAB , 0x2D , 0x8F , 0xA1 , 0x02 , 0x00 , 0x00 , 0x00 , 'C' , 0x00 , 0x00 , 0x00 , 0x00 , 0xCC , 0xCC , 0xFF , 0xFF , 0xEE , 0xEE};
	byte newdatabuf[1024];


	char (__thiscall *WriteToPipePtr)(void* thiss, pipebuffer *pipeBuf, char a3 ) = (char (__thiscall *)(void* thiss, pipebuffer *pipeBuf, char a3 ) )write2pipestart;

	numBytes = pipeBuf->numBytes;
	firstcmd = pipeBuf->data[pipeBuf->dataReadBytes];

	data = &pipeBuf->data[pipeBuf->dataReadBytes +1];
	lenData = numBytes -1;

	Com_sprintf(buffer, sizeof(buffer), "Header: Length: %d, 1stCmd: %d\n", lenData, firstcmd);
	Com_LogDebugData((byte*)buffer, strlen(buffer));
	Com_LogDebugData(data, lenData);
	Com_LogDebugData((byte*)"\n\n", 2);

	if(lenData == 31 && pipeBuf->dataReadBytes == 0)
	{
		newdatabuf[0] = firstcmd;
		memcpy(&newdatabuf[1], advertcmd, sizeof(advertcmd));
		pipeBuf->data = newdatabuf;
		pipeBuf->numBytes = sizeof(advertcmd) +1;
		Com_Printf("Reassigned pipe buffer\n");
	}

	memcpy(write2pipestart, pipefncstartbytes, 5);

	WriteToPipePtr(thiss, pipeBuf, a3);

	SSetJump((DWORD)write2pipestart, (void*)WriteToPipe);

	return 1;
}


void Write2PipeHookInit()
{
	DWORD oldProtect;
	VirtualProtect(write2pipestart, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(pipefncstartbytes, write2pipestart, 5);
	SSetJump((DWORD)write2pipestart, (void*)WriteToPipe);
}

*/
