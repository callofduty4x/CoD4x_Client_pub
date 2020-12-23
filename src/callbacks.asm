%macro bin_import 2

	SECTION .text
		global %1
		%1: jmp dword [o%1]

	SECTION .rodata
		o%1 dd %2
%endmacro



; Prologue for usercall function hook.
; Callee assumed to be _cdecl function.
; %1 - extern _cdecl function supposed to call.
; %2 - hook function name.
%macro USERCALL_WRAPPER_PROLOGUE 2
SECTION .text
extern %1
global %2
%2:
	push ebp
	mov ebp, esp
	sub esp, 4 ; For return value.
	pushfd
	pushad
%endmacro



; Epilogue for usercall function hook.
; Callee assumed to be _cdecl.
; %1 - register for call result.
%macro USERCALL_WRAPPER_EPILOGUE 1
	mov dword [esp + 32 + 4], eax ; + pushad + pushfd
	popad
	popfd
	pop %1
	pop ebp
	ret
%endmacro


; Prologue for usercall function caller.
; This is _cdecl function.
; %1 - name of this function.
; %2 - name of address in %3.
; %3 - address of usercall function.
%macro USERCALL_CALLER_PROLOGUE 3
SECTION .rodata
	%2 dd %3
SECTION .text
global %1
%1:
	push ebp
	mov ebp, esp
%endmacro



; Epilogue for usercall function caller.
%macro USERCALL_CALLER_EPILOGUE 0
	pop ebp
	ret
%endmacro


;bin_import _fopen, 0x671E8F
;bin_import _fclose, 0x671D4F
;bin_import _fread, 0x67213E
;bin_import _fwrite, 0x6722B8
;bin_import _ftell, 0x6724D2
;bin_import _fseek, 0x6725C7
;bin_import CL_ReadDemoData, 0x468F90
;bin_import CL_ReadDemoArchive, 0x468EA0
;bin_import CL_SystemInfoChanged, 0x473AB0
bin_import BG_PlayerStateToEntityState_internal, 0x40C6E0
bin_import flrand, 0x5628F0
bin_import CG_PredictPlayerState_Internal, 0x447260
bin_import Ragdoll_GetRootOrigin, 0x5BA060
bin_import Ragdoll_CreateRagdollForDObj, 0x5BA270
bin_import CG_CreatePhysicsObject, 0x434C70
bin_import CG_PreProcess_GetDObj, 0x435130
bin_import Phys_ObjGetInterpolatedState, 0x5A6EC0
bin_import BG_EvaluateTrajectory_internal, 0x40BD70
bin_import Com_ClientDObjClearAllSkel, 0x500f70
bin_import SV_ClipHandleForEntity, 0x536cb0
bin_import Sys_InitMainThread, 0x50b230
bin_import Sys_571a90, 0x571a90
bin_import GetApplicationLockFileName, 0x577320
bin_import Sys_CheckCrashOrRerun, 0x577390
bin_import Com_InitParse, 0x5705c0
bin_import InitTiming, 0x572ac0
bin_import Sys_GetHardwareInfo, 0x576590
bin_import Sys_CreateSplashWindow, 0x579030
bin_import Sys_RegisterWindow, 0x5774c0
bin_import getcodcwd, 0x67401c
bin_import PbClientInitialize, 0x5c0290
bin_import PbServerInitialize, 0x5c1540
bin_import Com_CheckSetRecommend, 0x4fe930
bin_import SL_Init, 0x517f10
bin_import Swap_Init, 0x5719B0
bin_import Cbuf_Init, 0x4f8d40
bin_import Cmd_Init, 0x4f9dd0
bin_import Com_InitDvars, 0x4fea80
bin_import CCS_InitConstantConfigStrings, 0x55a310
bin_import Sys_SpawnDatabaseThread, 0x50b480
bin_import CL_InitKeyCommands, 0x467d20
bin_import FS_InitFilesystem, 0x55ebf0
bin_import Con_InitChannels, 0x476fc0
bin_import StatsCommands_Init, 0x579f80
bin_import Init_PlayerProfile, 0x4fae30
bin_import Cbuf_Execute_stub, 0x4f9280
bin_import Cbuf_SV_Execute, 0x4F8FD0
bin_import SEH_UpdateLanguageInfo, 0x538860
;bin_import Com_InitHunkMemory, 0x5639f0
bin_import Com_Meminfo_f, 0x81CD2D8
bin_import Sys_OutOfMemory, 0x576880
bin_import R_ReflectionProbeRegisterDvars, 0x60FCC0
;bin_import Com_WriteConfig_f, 0x4ffc00
bin_import Com_WriteDefaults_f, 0x4ffc70
bin_import Scr_Init, 0x51d190
bin_import Scr_DefaultSettings, 0x4fe9b0
bin_import XAnim_Init, 0x580d00
bin_import SV_Init, 0x52fed0
bin_import Com_StartupVariable, 0x4fd740
bin_import SND_InitDriver, 0x573840
bin_import Init_RendererThread, 0x50b3a0
bin_import Run_RendererThread, 0x629a70
;bin_import CL_InitRenderer, 0x46caf0
bin_import Play_Intro, 0x4fea00
bin_import Cvar_Dump, 0x502380
bin_import Z_Free, 0x670da6
bin_import CL_KeyEvent, 0x467eb0
bin_import Z_Malloc, 0x5c8ea0
bin_import Com_EventLoop_org, 0x4fdc80
bin_import Com_ClientPacketEvent, 0x4fda30
bin_import Com_ServerPacketEvent, 0x4fdb40
bin_import Sys_RegisterInfoDvars, 0x576370
bin_import Cmd_ExecuteSingleCommand, 0x4f9ab0
;bin_import R_AddCmdClearScreen, 0x5f7710
;bin_import FS_FOpenFileReadForThread, 0x55b960
bin_import FS_FOpenFileWriteForThread, 0x4ba530
bin_import Com_Error, 0x4fd330
bin_import PbClientProcessEvents@0, 0x5bfb40
bin_import PbServerProcessEvents@4, 0x5c13c0
bin_import Sys_Main, 0x577550
;bin_import FS_Read, 0x55c120
;bin_import FS_SV_FOpenFileRead, 0x502cf0
bin_import FS_SV_FOpenFileWrite, 0x502bf0
bin_import Dvar_InfoString, 0x502680
;bin_import CL_RequestAuthorization, 0x469b20
;bin_import CL_BuildMd5StrFromCDKey, 0x4699a0
bin_import PbClLoad, 0x5bf990
bin_import PbSvLoad, 0x5c1230
bin_import PbClUnload, 0x5bf950
bin_import PbSvUnload, 0x5c11f0
bin_import CL_WriteDemoArchive, 0x468800
;bin_import CL_ParseServerMessage, 0x474650
;bin_import CL_ConnectionlessPacket, 0x46c0d0
bin_import CL_ProcessPbPacket, 0x46c0d0
;bin_import CL_ConnectionlessPacket2, 0x46b5e0
bin_import SEH_LocalizeTextMessage, 0x538d30
;bin_import UI_CloseAllMenusInternal, 0x54b060
bin_import CL_InitOnceForAllClients_org, 0x46d4e0
bin_import Com_Frame_o, 0x5002e0
bin_import Com_Init_o, 0x4ff9d0
;bin_import sub_578C50, 0x578C50
;bin_import sub_578EC0, 0x578EC0
bin_import Scr_UpdateLoadScreen2, 0x475120
bin_import Cbuf_ExecuteBuffer, 0x4f91c0
bin_import SND_StopSounds, 0x5c6220
bin_import CL_GetFreePing, 0x471D90
;bin_import LiveStorage_UploadStats, 0x579B40
bin_import EnablePbCl, 0x5c0400
bin_import DisablePbCl, 0x5c0450
bin_import isPbClEnabled, 0x5c03b0
;bin_import Menus_Close, 0x54c420
;bin_import Menus_FindName, 0x54c230
bin_import UI_LoadArenas, 0x541210
bin_import sub_546E60, 0x546E60
bin_import sub_547010, 0x547010
bin_import UI_SelectCurrentMap, 0x547060
bin_import UI_LoadGameInfo, 0x548d40
bin_import UI_LoadMods, 0x544ec0
;bin_import UI_StartServerRefresh, 0x54aaf0
bin_import Com_CreatePlayerProfile, 0x545210
bin_import UI_DeletePlayerProfile, 0x5453b0
bin_import Com_SortPlayerProfiles, 0x545070
bin_import Com_GetPlayerProfile, 0x544f80
bin_import Com_SelectPlayerProfile, 0x545020
bin_import DB_SyncXAssets, 0x48a290
bin_import UI_ServersQsortCompare, 0x544e90
;bin_import Key_ClearStates, 0x468500
bin_import UI_VerifyLanguage, 0x5456b0
bin_import Com_SetRecommended, 0x4fe7a0
bin_import LiveStorage_DecodeStatsData2, 0x579540
bin_import SND_LoadEntChannelFile, 0x5c6b20
bin_import SND_SetEnvironmentEffects_f, 0x5c1bc0
bin_import SND_DeactivateEnvironmentEffects_f, 0x5c1d60
bin_import SND_PlayLocal_f, 0x5c6510
bin_import SND_SetEq_f, 0x5c1f70
bin_import SND_SetEqFreq_f, 0x5c2110
bin_import SND_SetEqGain_f, 0x5c22a0
bin_import SND_SetEqQ_f, 0x5c2340
bin_import SND_SetEqType_f, 0x5c2200
bin_import SND_DeactivateEq_f, 0x5c2410
bin_import sub_57B350, 0x57B350
bin_import sub_57B5C0, 0x57B5C0
bin_import sub_57B070, 0x57B070
;bin_import sub_57B1D0, 0x57B1D0
bin_import sub_57B860, 0x57B860
bin_import sub_4ED0B0, 0x4ED0B0
bin_import sub_4ECA60, 0x4ECA60
bin_import sub_4EC880, 0x4EC880
bin_import sub_4ECB70, 0x4ECB70
bin_import DirectSoundCaptureCreate@12, 0x670296
bin_import Com_ErrorCleanUp, 0x04FD030
bin_import sub_5F78A0, 0x5F78A0
bin_import CL_StartHunkUsers, 0x046CC80
bin_import R_SyncRenderThread, 0x05F78F0
bin_import DB_PostLoadXZone, 0x048A120
bin_import Com_WriteLegacyConfiguration, 0x4FFBB0
bin_import UI_SetActiveMenu, 0x549540
bin_import Ragdoll_Update, 0x5BECA0
bin_import StatMon_Frame, 0x4FFE90
bin_import Sys_LoadingKeepAlive, 0x576F80
bin_import Sub_5C6B90, 0x5C6B90
bin_import sub_57AEB0, 0x57AEB0
;bin_import sub_57AE10, 0x57AE10
bin_import sub_45C740, 0x45C740
bin_import sub_46C680, 0x46C680
bin_import sub_46C590, 0x46C590
bin_import sub_4729C0, 0x4729C0
bin_import sub_46C700, 0x46C700
bin_import sub_4ED380, 0x4ED380
;bin_import Com_PlayerProfileExists, 0x4FABB0
bin_import FS_WriteFileToDir, 0x55C4E0 ;REGPARM(1)
bin_import CL_ShutdownRef, 0x46CA40
bin_import Com_Close, 0x500380
bin_import FS_Shutdown, 0x55E140
bin_import FS_ShutdownReferencedFiles, 0x55E0E0
bin_import R_WaitWorkerCmds, 0x500740
bin_import Hunk_Clear, 0x4FCE30
bin_import Hunk_ClearData, 0x563C90
bin_import SV_Shutdown, 0x530E10
bin_import sub_477210, 0x477210
bin_import doexit, 0x67334D ;it is equal with exit() but using the MSVC variant here is safer here considering multithreading
bin_import Key_Shutdown, 0x4685D0
bin_import Dvar_Shutdown, 0x56B7D0
bin_import SV_Say_f, 0x5284F0
bin_import SV_Tell_f, 0x528560
bin_import Sys_SuspendOtherThreads, 0x50B610
bin_import sub_55A170, 0x55A170
bin_import FS_Restart, 0x55ED10
;bin_import FS_ReferencedPakChecksums, 0x504480
bin_import CL_InitCGame, 0x45BEF0
bin_import Com_Restart, 0x5004C0
bin_import CL_ShutdownAll, 0x469430
bin_import __iw3mp_security_init_cookie, 0x67f189
bin_import __iw3mp_tmainCRTStartup, 0x67475c
bin_import Cvar_SetCheatState, 0x56D220
bin_import SL_GetStringOfSize, 0x518290
bin_import FS_ComparePaks, 0x5035F0
bin_import FS_AddIwdFilesForGameDirectory, 0x55D8B0
bin_import Com_SetPlayerProfile, 0x4FAC60
bin_import LiveStorage_StatGetInCvarCmd, 0x579E20
bin_import LiveStorage_StatSetCmd, 0x579D50
bin_import LiveStorage_StatsInit, 0x4FC550
bin_import Cvar_WriteSingleVariable, 0x501FF0
bin_import CL_SelectPlayerProfile, 0x545020
bin_import sub_5F4720, 0x5F4720
bin_import R_InitHardware, 0x5F47F0
;bin_import sub_5F4C60, 0x5F4C60
bin_import R_FatalError, 0x5F3300
bin_import R_InitSystems, 0x5F3DB0
;bin_import R_TextWidth, 0x5f1ee0
bin_import sub_60D7D0, 0x60D7D0
bin_import sub_5F3FD0, 0x5F3FD0
bin_import DB_ShutdownXAssetsInternal, 0x48b200
bin_import sub_4686C0, 0x4686C0
bin_import sub_48D300, 0x48D300
bin_import Asset_Parse, 0x558130
bin_import PC_SourceError, 0x5552e0
bin_import PC_LoadSourceHandleA, 0x420e90
bin_import Con_DrawConsoleInternal, 0x0461CD0
;bin_import SCR_DrawSmallStringExt, 0x0474C30
bin_import Con_CheckResize, 0x045CEF0
bin_import R_BeginFrame, 0x5F75A0
bin_import sub_474DA0, 0x474DA0
bin_import SND_PlayFXSounds, 0x5C3D90
bin_import SND_UpdateLoopingSounds, 0x5C4C60
bin_import SND_Update, 0x5C60C0
bin_import CG_DrawFullScreenDebugOverlay, 0x42C5A0
bin_import sub_42C010, 0x42C010
bin_import R_EndFrame, 0x5F7680
bin_import R_IssueRenderCommands, 0x5F6210
bin_import SCR_DrawScreenField, 0x474E40
bin_import Material_Register_FastFile, 0x5f2a80
bin_import Material_Register_LoadObj, 0x5f2a00
bin_import Voice_IncomingVoiceData, 0x57af60
bin_import sub_576280, 0x576280
bin_import CG_DrawFPSDebugInfo, 0x42B890
bin_import CG_DrawSnapshotDebugInfo, 0x42BEB0
bin_import UI_ReplaceConversionString, 0x54ADB0
;bin_import D3DErrorToString, 0x685F98
bin_import R_ErrorDescription, 0x685F98
;bin_import R_GetAdapterInfo, 0x5F46D0
bin_import sub_5F4140, 0x5F4140
bin_import Direct3DCreate9, 0x670284
bin_import FindStringOfSize, 0x0517F70
bin_import Con_InitCvars, 0x45DD50
bin_import DB_LoadXAssets, 0x48A2B0
bin_import DB_InitXAssets, 0x46CCB0
bin_import sub_554400, 0x554400
bin_import sub_54C110, 0x54C110
;bin_import Com_FindSoundAlias_FastFile, 0x565590
bin_import Com_PickSoundAliasFromList, 0x5654F0
bin_import _Com_PickSoundAliasFromList, 0x565C70
bin_import SND_PauseSounds, 0x5C4210
bin_import SND_UnpauseSounds, 0x5C4370
bin_import DB_FindXAssetHeader, 0x489570
bin_import DB_IsXAssetDefault, 0x4898A0
bin_import CG_RegisterWeapon, 0x454320
bin_import SE_GetString_LoadObj, 0x53A1E0
bin_import SV_SetConfigString, 0x52E6C0
bin_import AimAssist_ApplyMeleeCharge, 0x402B60
bin_import R_RenderScene, 0x5FAF00
bin_import R_WarnOncePerFrame, 0x6264B0
bin_import UI_BinaryServerInsertion, 0x5471E0
bin_import RBCmd_6136E0, 0x6136E0
bin_import RBCmd_612420, 0x612420
bin_import RBCmd_612480, 0x612480
bin_import RBCmd_612340, 0x612340
bin_import RBCmd_613730, 0x613730
bin_import RBCmd_6115F0, 0x6115F0
bin_import RBCmd_611650, 0x611650
bin_import RBCmd_6116B0, 0x6116B0
bin_import RBCmd_6119C0, 0x6119C0
bin_import RBCmd_612020, 0x612020
bin_import RBCmd_611CC0, 0x611CC0
bin_import RBCmd_611EC0, 0x611EC0
bin_import RBCmd_614FB0, 0x614FB0
bin_import RBCmd_615080, 0x615080
bin_import RBCmd_6124C0, 0x6124C0
bin_import RBCmd_612610, 0x612610
bin_import RBCmd_612CF0, 0x612CF0
bin_import RBCmd_6134B0, 0x6134B0
bin_import RBCmd_613680, 0x613680
bin_import RBCmd_6150C0, 0x6150C0
bin_import RB_EndTessSurface, 0x61A2F0
bin_import IN_Frame, 0x576100
bin_import R_SyncGpuPF, 0x5F5170
bin_import CreateDebugStringsIfNeeded, 0x461EC0
bin_import CreateDebugLinesIfNeeded, 0x462080
bin_import CG_RegisterSounds, 0x43E130
bin_import RB_DrawStretchPicRotate, 0x6111B0
;bin_import __cinit, 0x6732BB
;bin_import __heap_init, 0x678933
;bin_import __mtinit, 0x676CB5
;bin_import FS_LoadZipFile, 0x55c6f0
bin_import Com_AssetLoadUI, 0x500200
bin_import ScrPlace_SetupFloatViewport, 0x477420
bin_import R_BeginRegistration, 0x5F4EE0

bin_import FS_InitCvars, 0x55e390
bin_import FS_AddCommands, 0x503c70
bin_import FS_DisplayPath, 0x55d510
bin_import FS_AddGameDirectory_real, 0x55dd80

;bin_import DrawText2D, 0x613EA0
bin_import sub_4ECF20, 0x4ecf20
bin_import CG_ConfigStringModified, 0x44AF10
bin_import LoadMapLoadScreenInternal, 0x46A800
bin_import StringTable_LookupRowNumForValue, 0x569AA0
;SECTION .text
;global MSG_WriteEntityIndex
;MSG_WriteEntityIndex:
;	push esi
;	push edi
;	mov eax, [esp+12]
;	mov esi, [esp+16]
;	mov edi, [esp+20]
;	call dword [oMSG_WriteEntityIndex]
;	pop edi
;	pop esi
;	ret
;
;SECTION .rodata
;oMSG_WriteEntityIndex dd 0x5094f0

bin_import Cvar_Init, 0x56d2b0

SECTION .text
global Cbuf_AddText
Cbuf_AddText:
	xor ecx, ecx
	mov eax, [esp+4]
	jmp dword [oCbuf_AddText]

SECTION .rodata
oCbuf_AddText dd 0x4f8d90

SECTION .text
global Cvar_FindMalleable
Cvar_FindMalleable:
	push edi
	mov edi, [esp+8]
	call dword [oCvar_FindMalleable]
	pop edi
	ret

SECTION .rodata
oCvar_FindMalleable dd 0x56b5d0

bin_import Cvar_Registernew_stub, 0x56c130

SECTION .text
global Cvar_SetVariant
Cvar_SetVariant:

	mov eax, esp
	mov ecx, [eax+24]
	push ecx
	mov ecx, [eax+20]
	push ecx
	mov ecx, [eax+16]
	push ecx
	mov ecx, [eax+12]
	push ecx
	mov ecx, [eax+8]
	push ecx
	mov eax, [eax+4]
	call dword [oCvar_SetVariant]
	add esp, 20
	ret

SECTION .rodata
oCvar_SetVariant dd 0x56b1b0

SECTION .text
global PbCaptureConsoleOutput
PbCaptureConsoleOutput:
	push esi
	mov esi, [esp+8]
	call dword [oPbCaptureConsoleOutput]
	pop esi
	ret

SECTION .rodata
oPbCaptureConsoleOutput dd 0x5c16d0


SECTION .text
global CL_ConsolePrint
CL_ConsolePrint:
	push esi
	mov eax, esp
	mov edx, [eax+0x1C] ; type (arg6)
	push edx
	mov edx, [eax+0x18]
	push edx
	mov edx, [eax+0x14]
	push edx
	mov edx, [eax+0x10] ; text -> (arg3)
	push edx
	mov esi, [eax+0xC] ; output -> esi (arg2)
	mov edx, [eax+0x8]
	push edx
	call dword [oCL_ConsolePrint]
	add esp, 0x14
	pop esi
	ret

SECTION .rodata
oCL_ConsolePrint dd 0x45ef40


SECTION .text
global CL_CharEvent
CL_CharEvent:
	mov ecx, [esp+4]
	jmp dword [oCL_CharEvent]

SECTION .rodata
oCL_CharEvent dd 0x468490


global CM_TraceBox
CM_TraceBox:
	mov eax, [esp+8]
	mov ecx, [esp+4]
	add esp, 12
	call dword [oCM_TraceBox]
	sub esp, 12
	ret

SECTION .rodata
oCM_TraceBox dd 0x4f6d00


SECTION .text
global CM_TransformedBoxTrace
CM_TransformedBoxTrace:

	push ebp
	mov ebp, esp
	mov eax, [ebp+0x28]

	mov edx, [ebp+0x24]
	push edx

	mov edx, [ebp+0x20]
	push edx

	mov edx, [ebp+0x1C]
	push edx

	mov edx, [ebp+0x18]
	push edx

	mov ecx, [ebp+0x14]

	mov edx, [ebp+0x10]
	push edx

	mov edx, [ebp+0xC]
	push edx

	mov edx, [ebp+0x8]
	push edx

	call dword [oCM_TransformedBoxTrace]
	add esp, 0x1c
	pop ebp
	ret

SECTION .rodata
oCM_TransformedBoxTrace dd 0x4f54a0


SECTION .text
global NET_SendPacket_Stub
extern NET_SendPacketWrapper
NET_SendPacket_Stub:
	push ebp
	mov ebp, esp
	lea eax, [ebp+12]
	push eax  ;netadr_t*
	push edi  ;data
	push esi  ;length
	mov eax, [ebp+0x8] ;sock
	push eax
	call dword [O_NET_SendPacket]
	pop esi
	pop esi
	pop edi
	pop eax
	pop ebp
	ret

SECTION .rodata
O_NET_SendPacket dd NET_SendPacketWrapper

SECTION .text
global Sys_PBSendUdpPacket_Stub
extern Sys_PBSendUdpPacket
Sys_PBSendUdpPacket_Stub:
	mov edx, esp
	mov eax, [edx+12]
	push eax  ;data*
	mov eax, [edx+8]
	push eax  ;len
	mov eax, [edx+4]
	push eax  ;netport
	push esi  ;netstring
	call dword [oSys_PBSendUdpPacket]
	pop esi
	add esp, 12
	ret

SECTION .rodata
oSys_PBSendUdpPacket dd Sys_PBSendUdpPacket


SECTION .text
global NET_CompareAdr_Stub
extern NET_CompareAdrSigned
NET_CompareAdr_Stub:
	push eax
	push ecx
	call dword [oNET_CompareAdrSigned]
	add esp, 8
	ret

SECTION .rodata
oNET_CompareAdrSigned dd NET_CompareAdrSigned

SECTION .text
global Mantle_CapView_Stub
extern Mantle_CapView
Mantle_CapView_Stub:
	push esi
	call dword [oMantle_CapView]
	add esp, 4
	ret

SECTION .rodata
oMantle_CapView dd Mantle_CapView


;SECTION .text
;global CL_PacketEvent
;CL_PacketEvent:
;	xor edx, edx
;	mov eax, esp
;	push esi
;	mov ecx, [eax+20]
;	push ecx
;	mov ecx, [eax+16]
;	push ecx
;	mov ecx, [eax+12]
;	push ecx
;	mov ecx, [eax+8]
;	push ecx
;	mov ecx, [eax+4]
;	push ecx
;	mov esi, [eax+24]
;	call dword [oCL_PacketEvent]
;	add esp, 20
;	pop esi
;	ret

;SECTION .rodata
;oCL_PacketEvent dd 0x46c320


SECTION .text
global SV_PacketEvent
SV_PacketEvent:
	mov eax, esp
	push esi
	mov ecx, [eax+20]
	push ecx
	mov ecx, [eax+16]
	push ecx
	mov ecx, [eax+12]
	push ecx
	mov ecx, [eax+8]
	push ecx
	mov ecx, [eax+4]
	push ecx
	mov esi, [eax+24]
	call dword [oSV_PacketEvent]
	add esp, 20
	pop esi
	ret

SECTION .rodata
oSV_PacketEvent dd 0x532a40


;SECTION .text
;global unzCloseCurrentFile
;unzCloseCurrentFile:
;	push edi
;	mov edi, [esp+8]
;	call dword [ounzCloseCurrentFile]
;	pop edi
;	ret

;SECTION .rodata
;ounzCloseCurrentFile dd 0x596a50

;SECTION .text
;global FS_Write
;FS_Write:
;	mov ecx, [esp+4]
;	mov eax, [esp+8]
;	mov edx, [esp+12]
;	push edx
;	push eax
;	call dword [oFS_Write]
;	add esp, 8
;	ret

;SECTION .rodata
;oFS_Write dd 0x55c200

;SECTION .text
;global CL_WriteDemoMessage
;CL_WriteDemoMessage:
;	push edi
;	mov edx, [esp+8]
;	mov edi, [esp+12]
;	push edx
;	call dword [oCL_WriteDemoMessage]
;	pop edx
;	pop edi
;	ret

SECTION .rodata
oCL_WriteDemoMessage dd 0x468850

SECTION .text
global Cmd_TokenizeStringGeneric
Cmd_TokenizeStringGeneric:

	push esi
	mov ecx, [esp+8]
	mov esi, [esp+12]
	mov eax, [esp+16]
	mov edx, [esp+20]
	push edx
	push eax
	call dword [oCmd_TokenizeStringGeneric]
	add esp, 8
	pop esi
	ret

SECTION .rodata
oCmd_TokenizeStringGeneric dd 0x4f9840

;SECTION .text
;global CL_VoicePacket
;CL_VoicePacket:

;	mov eax, [esp+4]
;	jmp dword [oCL_VoicePacket]

;SECTION .rodata
;oCL_VoicePacket dd 0x4767b0

;SECTION .text
;global LiveStorage_ReadStatsFromDir
;LiveStorage_ReadStatsFromDir:

;	mov ecx, [esp+4]
;	jmp dword [oLiveStorage_ReadStatsFromDir]

;SECTION .rodata
;oLiveStorage_ReadStatsFromDir dd 0x579900

SECTION .text
global CL_DisconnectError
CL_DisconnectError:

	mov eax, [esp+4]
	jmp dword [oCL_DisconnectError]

SECTION .rodata
oCL_DisconnectError dd 0x46b4e0

SECTION .text
global CL_SetupForNewServerMap
CL_SetupForNewServerMap:

	push edi
	mov edi, [esp+8]
	mov edx, [esp+12]
	push edx
	call dword [oCL_SetupForNewServerMap]
	pop edx
	pop edi
	ret

SECTION .rodata
oCL_SetupForNewServerMap dd 0x470580

SECTION .text
global Con_Close
Con_Close:

	push esi
	mov esi, [esp+8]
	call dword [oCon_Close]
	pop esi
	ret

SECTION .rodata
oCon_Close dd 0x461da0

SECTION .text
global SV_Frame
SV_Frame:

	push esi
	mov esi, [esp+8]
	call dword [oSV_Frame]
	pop esi
	ret

SECTION .rodata
oSV_Frame dd 0x5335d0

SECTION .text
global MSG_WriteDeltaUsercmdKey
MSG_WriteDeltaUsercmdKey:

	mov eax, esp
	push esi
	mov esi, [eax+16]
	push esi
	mov esi, [eax+12]
	push esi
	mov esi, [eax+8]
	push esi
	mov esi, [eax+4]
	call dword [oMSG_WriteDeltaUsercmdKey]
	add esp, 12
	pop esi
	ret

SECTION .rodata
oMSG_WriteDeltaUsercmdKey dd 0x505C60


SECTION .text
global LAN_GetServerInfo_Stub
extern LAN_GetServerInfo
LAN_GetServerInfo_Stub:

	push 1024
	push edi
	push ecx
	push eax
	call dword [oLAN_GetServerInfo]
	add esp, 16
	ret

SECTION .rodata
oLAN_GetServerInfo dd LAN_GetServerInfo

SECTION .text
global RB_BeginSurface_Stub
extern RB_BeginSurface
RB_BeginSurface_Stub:

	push edi
	push esi
	call dword [oRB_BeginSurface]
	add esp, 8
	ret

SECTION .rodata
oRB_BeginSurface dd RB_BeginSurface

SECTION .text
global CLUI_GetServerAddressString_Stub
extern CLUI_GetServerAddressString
CLUI_GetServerAddressString_Stub:

	push edi
	push esi
	push eax
	push ecx
	call dword [oCLUI_GetServerAddressString]
	add esp, 16
	ret

SECTION .rodata
oCLUI_GetServerAddressString dd CLUI_GetServerAddressString


SECTION .rodata
oCL_DownloadsComplete dd 0x46A8D0



SECTION .text
global Com_SetErrorMessage
Com_SetErrorMessage:

	mov eax, [esp+4]
	jmp dword [oCom_SetErrorMessage]

SECTION .rodata
oCom_SetErrorMessage dd 0x4fcfe0

SECTION .text
global String_Parse
String_Parse:

	mov eax, [esp+4]
	mov edx, [esp+8]
	mov ecx, [esp+12]
	push ecx
	push edx
	call dword [oString_Parse]
	add esp,8
	ret

SECTION .rodata
oString_Parse dd 0x54b510

SECTION .text
global UI_SafeTranslateString
UI_SafeTranslateString:

	mov eax, [esp+4]
	jmp dword [oUI_SafeTranslateString]

SECTION .rodata
oUI_SafeTranslateString dd 0x54aca0



SECTION .text
global Cvar_SetFromStringByNameFromSource
Cvar_SetFromStringByNameFromSource:

	mov eax, [esp+4]
	mov edx, [esp+8]
	mov ecx, [esp+12]
	push ecx
	push edx
	call dword [oCvar_SetFromStringByNameFromSource]
	add esp, 8
	ret

SECTION .rodata
oCvar_SetFromStringByNameFromSource dd 0x56d0a0


SECTION .text
global Menu_SetFeederSelection
Menu_SetFeederSelection:
	mov edx, esp
	push edi
	mov eax, [edx+16]
	push eax
	mov eax, [edx+12]
	push eax
	mov eax, [edx+8]
	push eax
	mov eax, [edx+20]
	mov edi, [edx+4]
	call dword [oMenu_SetFeederSelection]
	add esp, 12
	pop edi
	ret

SECTION .rodata
oMenu_SetFeederSelection dd 0x5542c0

;SECTION .text
;global UI_BuildServerDisplayList
;UI_BuildServerDisplayList:

;	mov eax, [esp+8]
;	jmp dword [oUI_BuildServerDisplayList]

;SECTION .rodata
;oUI_BuildServerDisplayList dd 0x5472b0

SECTION .text
global UI_BuildServerStatus
UI_BuildServerStatus:

	mov eax, [esp+4]
	mov edx, [esp+8]
	push edx
	call dword [oUI_BuildServerStatus]
	pop edx
	ret

SECTION .rodata
oUI_BuildServerStatus dd 0x548160


SECTION .text
global UI_FeederSelection
UI_FeederSelection:

	mov ecx, [esp+8]
	jmp dword [oUI_FeederSelection]

SECTION .rodata
oUI_FeederSelection dd 0x548900

;SECTION .text
;global Menus_CloseAll
;Menus_CloseAll:

;	push esi
;	mov esi, [esp+8]
;	call dword [oMenus_CloseAll]
;	pop esi
;	ret

;SECTION .rodata
;oMenus_CloseAll dd 0x54c540

SECTION .text
global Int_Parse
Int_Parse:

	push edi
	mov edi, [esp+8]
	mov eax, [esp+12]
	push eax
	call dword [oInt_Parse]
	add esp, 4
	pop edi
	ret

SECTION .rodata
oInt_Parse dd 0x558f40



SECTION .text
global CL_MutePlayer
CL_MutePlayer:

	mov ecx, [esp+4]
	jmp dword [oCL_MutePlayer]

SECTION .rodata
oCL_MutePlayer dd 0x472940

;SECTION .text
;global CL_Frame
;CL_Frame:

;	mov eax, [esp+4]
;	jmp dword [oCL_Frame]

;SECTION .rodata
;oCL_Frame dd 0x46C9A0

;SECTION .text
;global CL_RunOncePerClientFrame
;CL_RunOncePerClientFrame:

;	mov eax, [esp+4]
;	jmp dword [oCL_RunOncePerClientFrame]

SECTION .rodata
oCL_RunOncePerClientFrame dd 0x46C8D0



SECTION .text
global Win_UpdateThreadLock
Win_UpdateThreadLock:

	mov eax, [esp+4]
	jmp dword [oWin_UpdateThreadLock]

SECTION .rodata
oWin_UpdateThreadLock dd 0x50B710


SECTION .text
global UI_IsFullscreen
UI_IsFullscreen:

	mov ecx, [esp+4]
	jmp dword [oUI_IsFullscreen]

SECTION .rodata
oUI_IsFullscreen dd 0x5543B0

SECTION .text
global sub_463E00
sub_463E00:

	xor ecx, ecx
	jmp dword [osub_463E00]

SECTION .rodata
osub_463E00 dd 0x463E00

SECTION .text
global sub_45C440
sub_45C440:

	push esi
	xor esi, esi
	call dword [osub_45C440]
	pop esi
	ret

SECTION .rodata
osub_45C440 dd 0x45C440

SECTION .text
global sub_4ED610
sub_4ED610:

	push esi
	mov esi, [esp+8]
	call dword [osub_4ED610]
	pop esi
	ret

SECTION .rodata
osub_4ED610 dd 0x4ED610


SECTION .text
global sub_518940
sub_518940:

	mov eax, [esp+4]
	jmp dword [osub_518940]

SECTION .rodata
osub_518940 dd 0x518940


SECTION .text
global CL_Shutdown
CL_Shutdown:

	mov eax, [esp+4]
	jmp dword [oCL_Shutdown]

SECTION .rodata
oCL_Shutdown dd 0x46FDF0


SECTION .text
global R_MakeDedicated
R_MakeDedicated:

	mov eax, [esp+4]
	jmp dword [oR_MakeDedicated]

SECTION .rodata
oR_MakeDedicated dd 0x5F3D10


SECTION .text
global UI_SetMap
UI_SetMap:

	push esi
	mov esi, [esp+8]
	mov eax, [esp+12]
	push eax
	call dword [oUI_SetMap]
	add esp, 4
	pop esi
	ret

SECTION .rodata
oUI_SetMap dd 0x5442F0

SECTION .text
global FS_SV_Rename
FS_SV_Rename:

	mov eax, [esp+4]
	mov edx, [esp+8]
	push edx
	call dword [oFS_SV_Rename]
	pop edx
	ret

SECTION .rodata
oFS_SV_Rename dd 0x502EF0

SECTION .text
global CL_ParseSnapshot
CL_ParseSnapshot:

	mov eax, [esp+4]
	jmp dword [oCL_ParseSnapshot]

SECTION .rodata
oCL_ParseSnapshot dd 0x473710

;SECTION .text
;global UI_DrawText
;UI_DrawText:

;	push ebp
;	mov ebp, esp
;	mov edx, [ebp+48]
;	push edx
;	mov edx, [ebp+44]
;	push edx
;	mov edx, [ebp+40]
;	push edx
;	mov eax, [ebp+36]
;	mov ecx, [ebp+32]
;	mov edx, [ebp+28]
;	push edx
;	push edx
;	mov edx, [ebp+20]
;	push edx
;	mov edx, [esp+16]
;	push edx
;	mov edx, [ebp+12]
;	push edx
;	mov edx, [ebp+8]
;	push edx
;	call dword [oUI_DrawText]
;	add esp, 36
;	pop ebp
;	ret

;SECTION .rodata
;oUI_DrawText dd 0x542F50


SECTION .text
global MSG_ReadDeltaEntity
MSG_ReadDeltaEntity:

	push ebp
	mov ebp, esp
	mov ecx, [ebp+24]
	push ecx			;int number
	mov eax, [ebp+20]	;entityState_t *to
	mov ecx, [ebp+16]
	push ecx			;entityState_t *from
	mov ecx, [ebp+12]
	push ecx			;int
	mov ecx, [ebp+8]	;msg_t *msg
	call dword [oMSG_ReadDeltaEntity]
	add esp, 12
	pop ebp
	ret

SECTION .rodata
oMSG_ReadDeltaEntity dd 0x506E20



;SECTION .text
;global FS_PureServerSetLoadedIwds
;FS_PureServerSetLoadedIwds:

;	mov ecx, [esp+4]
;	mov edx, [esp+8]
;	push edx
;	call dword [oFS_PureServerSetLoadedIwds]
;	pop edx
;	ret

;SECTION .rodata
;oFS_PureServerSetLoadedIwds dd 0x504650


SECTION .text
global FS_CompareFastFiles
FS_CompareFastFiles:

	push esi
	push edi
	mov eax, [esp+20]
	mov edi, [esp+16]
	mov esi, [esp+12]
	push eax
	call dword [oFS_CompareFastFiles]
	pop edi
	pop edi
	pop esi
	ret

SECTION .rodata
oFS_CompareFastFiles dd 0x5038F0


SECTION .text
global FS_AddUserMapDirIWD_Stub
extern FS_AddUserMapDirIWD
FS_AddUserMapDirIWD_Stub:
	push edi
	call dword [oFS_AddUserMapDirIWD]
	pop edi
	ret
SECTION .rodata
oFS_AddUserMapDirIWD dd FS_AddUserMapDirIWD



SECTION .text
global LiveStorage_ReadStatsFromDir_Stub
extern LiveStorage_ReadStatsFromDir
LiveStorage_ReadStatsFromDir_Stub:
	push ecx
	call dword [oLiveStorage_ReadStatsFromDir]
	pop ecx
	ret
SECTION .rodata
oLiveStorage_ReadStatsFromDir dd LiveStorage_ReadStatsFromDir


SECTION .text
global Key_WriteBindings
Key_WriteBindings:

	xor eax, eax
	jmp dword [oKey_WriteBindings]

SECTION .rodata
oKey_WriteBindings dd 0x467C00

SECTION .text
global Cvar_WriteVariables
Cvar_WriteVariables:

	push edi
	mov edi, [esp+8]
	mov eax, [esp+12]
	push eax
	call dword [oCvar_WriteVariables]
	add esp, 4
	pop edi
	ret

SECTION .rodata
oCvar_WriteVariables dd 0x569CC0

SECTION .text
global Con_WriteFilterConfigString
Con_WriteFilterConfigString:

	push edi
	mov edi, [esp+8]
	call dword [oCon_WriteFilterConfigString]
	pop edi
	ret

SECTION .rodata
oCon_WriteFilterConfigString dd 0x476D50

SECTION .text
global Hunk_FreeTempMemory
Hunk_FreeTempMemory:

	push esi
	mov esi, [esp+8]
	call dword [oHunk_FreeTempMemory]
	pop esi
	ret

SECTION .rodata
oHunk_FreeTempMemory dd 0x564240

SECTION .text
global Hunk_AllocateTempMemory
Hunk_AllocateTempMemory:

	mov eax, [esp+4]
	jmp dword [oHunk_AllocateTempMemory]

SECTION .rodata
oHunk_AllocateTempMemory dd 0x564140



SECTION .text
global FS_ReadFile
FS_ReadFile:

	mov edx, [esp+8]
	mov eax, [esp+4]
	push edx
	call dword [oFS_ReadFile]
	pop edx
	ret

SECTION .rodata
oFS_ReadFile dd 0x55C440

SECTION .text
global Cmd_ExecFastfile
Cmd_ExecFastfile:

	push edi
	mov edi, [esp+8]
	push 0
	push 0
	call dword [oCmd_ExecFastfile]
	add esp, 8
	pop edi
	ret

SECTION .rodata
oCmd_ExecFastfile dd 0x4F93E0


SECTION .text
global Com_NewPlayerProfile_Stub
extern Com_NewPlayerProfile
Com_NewPlayerProfile_Stub:
	push esi
	call dword [oCom_NewPlayerProfile]
	pop esi
	ret
SECTION .rodata
oCom_NewPlayerProfile dd Com_NewPlayerProfile


SECTION .text
global String_Alloc
String_Alloc:

	mov eax, [esp+4]
	jmp dword [oString_Alloc]

SECTION .rodata
oString_Alloc dd 0x558E40


SECTION .text
global R_SetupWindowSettings
R_SetupWindowSettings:

	push esi
	mov esi, [esp+8]
	call dword [oR_SetupWindowSettings]
	pop esi
	ret

SECTION .rodata
oR_SetupWindowSettings dd 0x5F4B10


SECTION .text
global sub_45C970
sub_45C970:

	xor eax, eax
	jmp dword [osub_45C970]

SECTION .rodata
osub_45C970 dd 0x45C970


SECTION .text
global PC_ReadTokenHandleA
PC_ReadTokenHandleA:

	push edi
	mov eax, [esp+8]
	mov edi, [esp+12]
	call dword [oPC_ReadTokenHandleA]
	pop edi
	ret

SECTION .rodata
oPC_ReadTokenHandleA dd 0x420fb0


SECTION .text
global Menu_New
Menu_New:

	push edi
	mov edi, [esp+8]
	mov eax, [esp+12]
	push eax
	call dword [oMenu_New]
	add esp, 4
	pop edi
	ret

SECTION .rodata
oMenu_New dd 0x558450

SECTION .text
global R_GenerateReflections
R_GenerateReflections:

	mov ecx, [esp+4]
	mov edx, [esp+8]
	mov eax, [esp+12]
	push eax
	push edx
	call dword [oR_GenerateReflections]
	add esp, 8
	ret

SECTION .rodata
oR_GenerateReflections dd 0x0610230

;SECTION .text
;global CL_CGameNeedsServerCommand
;CL_CGameNeedsServerCommand:

;	mov edx, [esp+4]
;	mov eax, [esp+8]
;	push edx
;	call dword [oCL_CGameNeedsServerCommand]
;	add esp, 4
;	ret

;SECTION .rodata
;oCL_CGameNeedsServerCommand dd 0x045AFC0

SECTION .text
global CG_DeployServerCommand
CG_DeployServerCommand:

	xor eax, eax
	jmp dword [oCG_DeployServerCommand]

SECTION .rodata
oCG_DeployServerCommand dd 0x044BE40

SECTION .text
global SEH_SafeTranslateString
SEH_SafeTranslateString:
	push esi
	mov esi, [esp+8]
	call dword [oSEH_SafeTranslateString]
	pop esi
	ret

SECTION .rodata
oSEH_SafeTranslateString dd 0x538B60

SECTION .text
global sub_5F39C0
sub_5F39C0:
	push esi
	mov esi, [esp+8]
	call dword [osub_5F39C0]
	pop esi
	ret

SECTION .rodata
osub_5F39C0 dd 0x5F39C0


SECTION .text
global SL_RemoveRefToString
SL_RemoveRefToString:

	mov eax, [esp+4]
	jmp dword [oSL_RemoveRefToString]

SECTION .rodata
oSL_RemoveRefToString dd 0x0518940


SECTION .text
global Cvar_MakeExplicitType
Cvar_MakeExplicitType:

	push ebp
	mov ebp, esp
	push esi
;cvarlimits
	mov edx, [ebp+40]
	push edx
	mov edx, [ebp+36]
	push edx
;cvarvalue
	mov edx, [ebp+32]
	push edx
	mov edx, [ebp+28]
	push edx
	mov edx, [ebp+24]
	push edx
	mov edx, [ebp+20]
	push edx
;cvarflags
	mov edx, [ebp+16]
	push edx
;cvartype
	mov edx, [ebp+12]
	push edx
;cvar
	mov esi, [ebp+8]
	call dword [oCvar_MakeExplicitType]
	add esp, 32
	pop esi
	pop ebp
	ret

SECTION .rodata
oCvar_MakeExplicitType dd 0x56BCD0


;SECTION .text
;global Cvar_ConvertToStringType
;Cvar_ConvertToStringType:

;	mov eax, [esp+4]
;	jmp dword [oCvar_ConvertToStringType]

;SECTION .rodata
;oCvar_ConvertToStringType dd 0x056B920


SECTION .text
global Cvar_SetLatchedValue
Cvar_SetLatchedValue:

	push ebp
	mov ebp, esp
;cvarvalue
	mov edx, [ebp+24]
	push edx
	mov edx, [ebp+20]
	push edx
	mov edx, [esp+16]
	push edx
	mov edx, [ebp+12]
	push edx
;cvar
	mov eax, [ebp+8]
	call dword [oCvar_SetLatchedValue]
	add esp, 16
	pop ebp
	ret

SECTION .rodata
oCvar_SetLatchedValue dd 0x56B060

SECTION .text
global Cvar_Reregister_Stub
extern Cvar_Reregister
Cvar_Reregister_Stub:
	mov edx, esp
;description
	mov ecx, [edx+12]
	push ecx
;limits
	mov ecx, [edx+36]
	push ecx
	mov ecx, [edx+32]
	push ecx
;value
	mov ecx, [edx+28]
	push ecx
	mov ecx, [edx+24]
	push ecx
	mov ecx, [edx+20]
	push ecx
	mov ecx, [edx+16]
	push ecx
;flags
	push edi
;type
	mov ecx, [edx+8]
	push ecx
;cvarname
	mov ecx, [edx+4]
	push ecx
;cvar_t*
	push eax
	call dword [OCvar_Reregister]
	add esp,44
	ret

SECTION .rodata
OCvar_Reregister dd Cvar_Reregister


SECTION .text
global Cvar_SetCommand
Cvar_SetCommand:

	mov edx, [esp+4]
	mov eax, [esp+8]
	push edx
	call dword [oCvar_SetCommand]
	add esp,4
	ret

SECTION .rodata
oCvar_SetCommand dd 0x56D110


SECTION .text
global DB_LoadXZoneForModLevel_Stub
extern DB_LoadXZoneForModLevel
DB_LoadXZoneForModLevel_Stub:
	push esi
	call dword [O_DB_LoadXZoneForModLevel]
	pop esi
	ret
SECTION .rodata
O_DB_LoadXZoneForModLevel dd DB_LoadXZoneForModLevel


SECTION .text
global SND_PlayLocalSoundAliasByName
SND_PlayLocalSoundAliasByName:
	push esi
	mov esi, [esp+8]
	mov ecx, [esp+12]
	call dword [oSND_PlayLocalSoundAliasByName]
	pop esi
	ret

SECTION .rodata
oSND_PlayLocalSoundAliasByName dd 0x554FC0



;SECTION .text
;global unzClose
;unzClose:

;	push edi
;	mov edi, [esp+8]
;	call dword [ounzClose]
;	pop edi
;	ret

;SECTION .rodata
;ounzClose dd 0x596A50


SECTION .text
global SND_StopBackground
SND_StopBackground:
	mov edx, [esp+4]
	mov eax, [esp+8]
	push eax
	call dword [oSND_StopBackground]
	add esp, 4
	ret

SECTION .rodata
oSND_StopBackground dd 0x5C4770

SECTION .text
global SND_PlayAmbientAlias
SND_PlayAmbientAlias:
	mov eax, [esp+4]
	mov edx, [esp+8]
	mov ecx, [esp+12]
	push ecx
	push edx
	call dword [oSND_PlayAmbientAlias]
	add esp, 8
	ret



SECTION .rodata
oSND_PlayAmbientAlias dd 0x5C4850



SECTION .text
global R_TestFunc
R_TestFunc:

	push esi
	mov esi, [esp+8]
	call dword [oR_TestFunc]
	pop esi
	ret

SECTION .rodata
oR_TestFunc dd 0x5F4910



SECTION .text
global CG_EntityEvent_Stub
extern CG_EntityEvent
CG_EntityEvent_Stub:
	mov edx, [esp+4]
	push eax  ;event
	push edx  ;centity
	push ecx  ;localclientnum
	call dword [oCG_EntityEvent]
	add esp, 12
	ret

SECTION .rodata
oCG_EntityEvent dd CG_EntityEvent




SECTION .text
global CG_EntityEvent_Internal
CG_EntityEvent_Internal:

	mov ecx, [esp+4]  ;localclientnum
	mov edx, [esp+8] ;centity
	mov eax, [esp+12]  ;event
	push edx
	call dword [oCG_EntityEvent_Internal]
	add esp, 4
	ret

SECTION .rodata
oCG_EntityEvent_Internal dd 0x435CE0



SECTION .text
global CG_ExecuteNewServerCommands_Stub
extern CG_ExecuteNewServerCommands
CG_ExecuteNewServerCommands_Stub:
	push edi  ;serverCommandNum
	push esi  ;localClientNum
	call dword [oCG_ExecuteNewServerCommands]
	add esp, 8
	ret

SECTION .rodata
oCG_ExecuteNewServerCommands dd CG_ExecuteNewServerCommands



;SECTION .text
;global MSG_WriteDeltaEntity
;MSG_WriteDeltaEntity:

;	push ebp
;	mov ebp, esp
;force
;	mov edx, [ebp+24]
;	push edx
;to
;	mov ecx, [ebp+20]
;from
;	mov eax, [ebp+16]
;time
;	mov edx, [ebp+12]
;	push edx
;msg
;	mov edx, [ebp+8]
;	push edx
;	call dword [oMSG_WriteDeltaEntity]
;	add esp, 12
;	pop ebp
;	ret

;SECTION .rodata
;oMSG_WriteDeltaEntity dd 0x50A280


SECTION .text
global AimAssist_UpdateMouseInput_Stub
extern AimAssist_UpdateMouseInput
AimAssist_UpdateMouseInput_Stub:
	push edi  ;AimOutput
	push esi  ;AimInput
	call AimAssist_UpdateMouseInput
	add esp, 8
	ret


SECTION .text
global DObjGetBoneIndex
DObjGetBoneIndex:
	mov ecx, [esp+4]
	mov edx, [esp+8]
	mov eax, [esp+12]
	push eax
	push edx
	call dword [oDObjGetBoneIndex]
	add esp, 8
	ret

SECTION .rodata
oDObjGetBoneIndex dd 0x57F2B0


SECTION .text
global CG_DObjGetLocalBoneMatrix
CG_DObjGetLocalBoneMatrix:
	mov ecx, [esp+4]
	mov eax, [esp+8]
	mov edx, [esp+12]
	push edx
	push ecx
	call dword [oCG_DObjGetLocalBoneMatrix]
	add esp, 8
	ret

SECTION .rodata
oCG_DObjGetLocalBoneMatrix dd 0x4352E0



;SECTION .text
;global R_GetCharacterGlyph
;R_GetCharacterGlyph:

;	push edi
;	mov edi, [esp+12]
;	mov eax, [esp+8]
;	push eax
;	call dword [oR_GetCharacterGlyph]
;	add esp, 4
;	pop edi
;	ret

;SECTION .rodata
;oR_GetCharacterGlyph dd 0x5F1D60



SECTION .text
global ChangeViewmodelDObj
ChangeViewmodelDObj:

	push ebp
	mov ebp, esp
;updateClientInfo
	mov edx, [ebp+36]
	push edx
;newKnife
	mov edx, [ebp+32]
	push edx
;newRocket
	mov edx, [ebp+28]
	push edx
;newGoggles
	mov edx, [ebp+24]
	push edx
;newHands
	mov edx, [ebp+20]
	push edx
;weaponModel
	mov ecx, [ebp+16]
;weaponNum
	mov eax, [ebp+12]
;localClientNum
	mov edx, [ebp+8]
	push edx
	call dword [oChangeViewmodelDObj]
	add esp, 24
	pop ebp
	ret

SECTION .rodata
oChangeViewmodelDObj dd 0x454740


SECTION .text
global CG_GetClientName_Stub
extern CG_GetClientNameIW
CG_GetClientName_Stub:
	mov eax, [esp+4]
	push eax
	push esi
	call dword [O_CG_GetClientName]
	add esp, 8
	ret
SECTION .rodata
O_CG_GetClientName dd CG_GetClientNameIW


SECTION .text
global UI_DrawGameTypeString_Stub
extern UI_DrawGameTypeString
UI_DrawGameTypeString_Stub:
	mov edx, esp
;style
	mov ecx, [edx+16]
	push ecx
;color
	mov ecx, [edx+12]
	push ecx
;fontscale
	mov ecx, [edx+8]
	push ecx
;font
	mov ecx, [edx+4]
	push ecx
;rect
	push esi
	call dword [OUI_DrawGameTypeString]
	add esp, 20
	ret

SECTION .rodata
OUI_DrawGameTypeString dd UI_DrawGameTypeString

SECTION .text
global MSG_ReadOriginFloat_Stub
extern MSG_ReadOriginFloat
MSG_ReadOriginFloat_Stub:
	mov edx, [esp+4] ;bits
	mov ecx, [esp+8] ;oldValue
	push ecx
	push edx
	push eax
	call dword [O_MSG_ReadOriginFloat]
	add esp, 12
	ret
SECTION .rodata
O_MSG_ReadOriginFloat dd MSG_ReadOriginFloat

;SECTION .text
;global unzSkipDataInCurrentFile_Stub
;extern unzSkipDataInCurrentFile
;unzSkipDataInCurrentFile_Stub:
;	push ecx
;	push eax
;	call dword [O_unzSkipDataInCurrentFile]
;	add esp, 8
;	ret
;SECTION .rodata
;O_unzSkipDataInCurrentFile dd unzSkipDataInCurrentFile

;SECTION .text
;global unzReadCurrentFile
;unzReadCurrentFile:
;	mov eax, [esp+4]
;	mov edx, [esp+8]
;	mov ecx, [esp+12]
;	push edx
;	call dword [ounzReadCurrentFile]
;	add esp, 4
;	ret

;SECTION .rodata
;ounzReadCurrentFile dd 0x596900


SECTION .text
global Con_TimeJumped
Con_TimeJumped:
	mov eax, [esp+8]
	mov edx, [esp+4]
	push edx
	call dword [oCon_TimeJumped]
	add esp, 4
	ret

SECTION .rodata
oCon_TimeJumped dd 0x45CC80


SECTION .text
global RB_DrawHudIcon_Stub
extern RB_DrawHudIcon
RB_DrawHudIcon_Stub:
	mov edx, esp
	push esi
	mov esi, ecx
;color
	mov ecx, [edx+28]
	push ecx
;yScale
	mov ecx, [edx+24]
	push ecx
;xScale
	mov ecx, [edx+20]
	push ecx
;Font
	push eax
;cosAngle
	mov ecx, [edx+16]
	push ecx
;sinAngle
	mov ecx, [edx+12]
	push ecx
;y
	mov ecx, [edx+8]
	push ecx
;x
	mov ecx, [edx+4]
	push ecx
;text
	push esi
	call dword [ORB_DrawHudIcon]
	add esp, 36
	pop esi
	ret

SECTION .rodata
ORB_DrawHudIcon dd RB_DrawHudIcon



USERCALL_WRAPPER_PROLOGUE BG_CanItemBeGrabbed, BG_CanItemBeGrabbed_Hook
	push dword [ebp + 8]
	push esi
	push edi
	call BG_CanItemBeGrabbed
	add esp, 12
USERCALL_WRAPPER_EPILOGUE eax



USERCALL_CALLER_PROLOGUE WeaponEntCanBeGrabbed, oWeaponEntCanBeGrabbed, 0x0040BBB0
	push dword [ebp + 20]
	push dword [ebp + 16]
	mov ecx, dword [ebp + 12]
	mov eax, dword [ebp + 8]
	call dword [oWeaponEntCanBeGrabbed]
	add esp, 8
USERCALL_CALLER_EPILOGUE


SECTION .text
global PMem_FreeInPrim
PMem_FreeInPrim:

	push esi
	mov eax, [esp+8]
	mov esi, [esp+12]
	call dword [oPMem_FreeInPrim]
	pop esi
	ret

SECTION .rodata
oPMem_FreeInPrim dd 0x570350

SECTION .text
global DB_FreeXZoneMemory
DB_FreeXZoneMemory:

	push edi
	mov edi, [esp+8]
	call dword [oDB_FreeXZoneMemory]
	pop edi
	ret

SECTION .rodata
oDB_FreeXZoneMemory dd 0x488A40
