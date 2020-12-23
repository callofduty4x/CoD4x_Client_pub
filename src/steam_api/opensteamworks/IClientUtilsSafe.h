//==========================  Open Steamworks  ================================
//
// This file is part of the Open Steamworks project. All individuals associated
// with this project do not claim ownership of the contents
// 
// The code, comments, and all related files, projects, resources,
// redistributables included with this project are Copyright Valve Corporation.
// Additionally, Valve, the Valve logo, Half-Life, the Half-Life logo, the
// Lambda logo, Steam, the Steam logo, Team Fortress, the Team Fortress logo,
// Opposing Force, Day of Defeat, the Day of Defeat logo, Counter-Strike, the
// Counter-Strike logo, Source, the Source logo, and Counter-Strike Condition
// Zero are trademarks and or registered trademarks of Valve Corporation.
// All other trademarks are property of their respective owners.
//
//=============================================================================

#ifndef ICLIENTUTILS_H
#define ICLIENTUTILS_H
#ifdef _WIN32
#pragma once
#endif
#ifndef NULL
#define NULL nullptr
#endif

#include "../SteamTypes.h"
#include "IImportStruct.h"
//#include "UtilsCommon.h"


typedef enum ELauncherType
{
	// TODO: Reverse this enum
} ELauncherType;

enum EWindowType
{
	// TODO: Reverse this enum
};

enum EGameLaunchMethod
{
	// TODO: Reverse this enum
};

enum EUIMode
{
	k_EUIModeNormal = 0,
	k_EUIModeTenFoot = 1,
};

enum ESpewGroup
{
	k_ESpewGroupConsole = 0,
	k_ESpewGroupPublish = 1,
	k_ESpewGroupBootstrap = 2,
	k_ESpewGroupStartup = 3,
	k_ESpewGroupService = 4,
	k_ESpewGroupFileop = 5,
	k_ESpewGroupSystem = 6,
	k_ESpewGroupSmtp = 7,
	k_ESpewGroupAccount = 8,
	k_ESpewGroupJob = 9,
	k_ESpewGroupCrypto = 10,
	k_ESpewGroupNetwork = 11,
	k_ESpewGroupVac = 12,
	k_ESpewGroupClient = 13,
	k_ESpewGroupContent = 14,
	k_ESpewGroupCloud = 15,
	k_ESpewGroupLogon = 16,
	k_ESpewGroupClping = 17,
	k_ESpewGroupThreads = 18,
	k_ESpewGroupBsnova = 19,
	k_ESpewGroupTest = 20,
	k_ESpewGroupFiletx = 21,
	k_ESpewGroupStats = 22,
	k_ESpewGroupSrvping = 23,
	k_ESpewGroupFriends = 24,
	k_ESpewGroupChat = 25,
	k_ESpewGroupGuestpass = 26,
	k_ESpewGroupLicense = 27,
	k_ESpewGroupP2p = 28,
	k_ESpewGroupDatacoll = 29,
	k_ESpewGroupDrm = 30,
	k_ESpewGroupSvcm = 31,
	k_ESpewGroupHttpclient = 32,
	k_ESpewGroupHttpserver = 33,
};


#ifdef PARSM
	#undef PARSM
#endif

#define PARSM( x ) # x,(int)&( (struct IClientUtils*)0 )->x / sizeof(int*)



struct IClientUtils
{
	AppId_t CPPCALL (*SetAppIDForCurrentPipe)( InterfaceDesc_t* pclass, AppId_t nAppID, bool bTrackProcess );
	AppId_t CPPCALL (*GetAppID)(InterfaceDesc_t* pclass);
	void CPPCALL (*SetAPIDebuggingActive)( InterfaceDesc_t* pclass, bool bActive, bool bVerbose );
	InterfaceDesc_t* pclass;
};

static safeimportaccess_t iClientUtilsFields[] = 
{
	{ PARSM( SetAppIDForCurrentPipe )},
	{ PARSM( GetAppID )},
	{ PARSM( SetAPIDebuggingActive )},
	{ nullptr, 0 }
};

#endif // ICLIENTUTILS_H
