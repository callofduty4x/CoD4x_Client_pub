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

#ifndef ICLIENTUSER_H
#define ICLIENTUSER_H
#ifdef _WIN32
#pragma once
#endif

#include "../steamtypes.h"
//#include "UserCommon.h"
#include "ContentServerCommon.h"
#include "ECurrencyCode.h"
#include "IImportStruct.h"

// Protobuf'ed class
class ClientAppInfo
#ifdef _S4N_
{
	int m_iPadding;
}
#endif
;

enum EParentalFeature
{
	k_EParentalFeatureInvalid = 0,
	k_EParentalFeatureStore = 1,
	k_EParentalFeatureCommunity = 2,
	k_EParentalFeatureProfile = 3,
	k_EParentalFeatureFriends = 4,
	k_EParentalFeatureNews = 5,
	k_EParentalFeatureTrading = 6,
	k_EParentalFeatureSettings = 7,
	k_EParentalFeatureConsole = 8,
	k_EParentalFeatureBrowser = 9,
	k_EParentalFeatureParentalSetup = 10,
	k_EParentalFeatureLibrary= 11,
	k_EParentalFeatureTest = 12,
};

typedef enum ELogonState
{
	k_ELogonStateNotLoggedOn = 0,
	k_ELogonStateLoggingOn = 1,
	k_ELogonStateLoggingOff = 2,
	k_ELogonStateLoggedOn = 3
} ELogonState;


typedef enum ERegistrySubTree
{
	k_ERegistrySubTreeNews = 0,
	k_ERegistrySubTreeApps = 1,
	k_ERegistrySubTreeSubscriptions = 2,
	k_ERegistrySubTreeGameServers = 3,
	k_ERegistrySubTreeFriends = 4,
	k_ERegistrySubTreeSystem = 5,
	k_ERegistrySubTreeAppOwnershipTickets = 6,
	k_ERegistrySubTreeLegacyCDKeys = 7,
} ERegistrySubTree;


typedef enum ESteamUsageEvent
{
	k_ESteamUsageEventMarketingMessageView = 1,
	k_ESteamUsageEventHardwareSurvey = 2,
	k_ESteamUsageEventDownloadStarted = 3,
	k_ESteamUsageEventLocalizedAudioChange = 4,
	k_ESteamUsageEventClientGUIUsage = 5,
	k_ESteamUsageEventCharityChoice = 6,
} ESteamUsageEvent;

typedef enum EClientStat
{
	k_EClientStatP2PConnectionsUDP = 0,
	k_EClientStatP2PConnectionsRelay = 1,
	k_EClientStatP2PGameConnections = 2,
	k_EClientStatP2PVoiceConnections = 3,
	k_EClientStatBytesDownloaded = 4,
	k_EClientStatMax = 5,
} EClientStat;

enum EMicroTxnAuthResponse
{
	k_EMicroTxnAuthResponseInvalid = 0,
	k_EMicroTxnAuthResponseAuthorize = 1,
	k_EMicroTxnAuthResponseDeny = 2,
	k_EMicroTxnAuthResponseAutoDeny = 3,
};


enum ESteamGuardProvider
{
	// TODO: Reverse this enum
};

class CUtlBuffer
#ifdef _S4N_
{
	int m_iPadding;
}
#endif
;


class CAmount
{
public:
	int m_nAmount;
	ECurrencyCode m_eCurrencyCode;
};


typedef enum ENatDiscoveryTypes
{
	eNatTypeUntested = 0,
	eNatTypeTestFailed = 1,
	eNatTypeNoUDP = 2,
	eNatTypeOpenInternet = 3,
	eNatTypeFullCone = 4,
	eNatTypeRestrictedCone = 5,
	eNatTypePortRestrictedCone = 6,
	eNatTypeUnspecified = 7,
	eNatTypeSymmetric = 8,
	eNatTypeSymmetricFirewall = 9,
	eNatTypeCount = 10
} ENatType;

typedef enum EPhysicalSocketConnectionResult
{
	PhysicalSocket_Unknown = 0,
	PhysicalSocket_IsRemoteSide = 1,
	PhysicalSocket_Connected = 2,
	PhysicalSocket_Failed = 3,
	PhysicalSocket_SignalingFailed = 4,
	PhysicalSocket_ResultCount = 5
} EPhysicalSocketConnectionResult;


class CNatTraversalStat
{
public:
	EPhysicalSocketConnectionResult m_eResult;
	ENatDiscoveryTypes m_eLocalNatType;
	ENatDiscoveryTypes m_eRemoteNatType;
	bool m_bMultiUserChat : 1;
	bool m_bRelay : 1;
};


// Protobuf, see steammessages_offline.steamclient.proto
class COffline_OfflineLogonTicket
#ifdef _S4N_
{
	int m_iPadding;
}
#endif
;



#ifdef PARSM
	#undef PARSM
#endif

#define PARSM( x ) # x,(int)&( (struct IClientUser*)0 )->x / sizeof(int*)

struct IClientUser
{
//	bool CPPCALL (*SpawnProcess)( classptr_t pclass, const char *exepath, const char *lpApplicationName, uint32 dwCreationFlags, const char *lpCurrentDirectory, CGameID gameID, const char *pchGameName, AppId_t nAppID, uint32 uUnk, uint32 unk2); Not like this anymore
	bool CPPCALL (*SpawnProcess)( InterfaceDesc_t* pclass, const char *exepath, const char *lpApplicationName, const char *lpCurrentDirectory, CGameID gameID, const char *pchGameName, AppId_t nAppID, uint32 uUnk, uint32 unk2);
	InterfaceDesc_t* pclass;
};

static safeimportaccess_t iClientUserFields[] = 
{
	{ PARSM( SpawnProcess )},
	{ nullptr, 0 }
};

#endif // ICLIENTUSER_H
