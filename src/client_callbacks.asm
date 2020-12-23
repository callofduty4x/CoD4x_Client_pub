
%macro bin_import 2

	SECTION .text		
		global %1
		%1: jmp dword [o%1]
		
	SECTION .rodata	
		o%1 dd %2
%endmacro

bin_import CL_ForwardToServer_f, 0x469dd0
bin_import CL_Clientinfo_f, 0x46a760
bin_import CL_Vid_Restart_f, 0x46a180
bin_import CL_Snd_Restart_f, 0x46a550
bin_import CL_Disconnect_f, 0x46a040
;bin_import CL_Record_f, 0x468960
bin_import CL_StopRecord_f, 0x4688c0
bin_import CL_PlayCinematic_f, 0x45c8d0
bin_import CL_PlayUnskippableCinematic_f, 0x45c920
bin_import CL_PlayLogo_f, 0x46d030
;bin_import CL_Connect_f, 0x471050
;bin_import CL_Reconnect_f, 0x46a090
;bin_import CL_LocalServers_f, 0x470220
;bin_import CL_GlobalServers_f, 0x471820
;bin_import CL_Rcon_f, 0x471af0
;bin_import CL_Ping_f, 0x471e70
;bin_import CL_ServerStatus_f, 0x472310
bin_import CL_Setenv_f, 0x469e40
bin_import CL_ShowIP_f, 0x4704f0
bin_import CL_ToggleMenu_f, 0x46d310
bin_import CL_OpenedIWDList_f, 0x46a6b0
bin_import CL_ReferencedIWDList_f, 0x46a6e0
bin_import CL_UpdateLevelHunkUsage, 0x45bab0
bin_import CL_startSingleplayer_f, 0x46ce90
bin_import CL_CubemapShot_f, 0x475200
bin_import CL_OpenScriptMenu_f, 0x46d3f0
bin_import Com_WriteLocalizedSoundAliasFiles, 0x5698d0
bin_import UI_CloseMenu_f, 0x548f20
bin_import CL_SelectStringTableEntryInDvar_f, 0x548dc0
bin_import Con_Init, 0x45d080
bin_import CL_InitInput, 0x4643f0
bin_import Ragdoll_InitCvars, 0x5ba330
bin_import R_Init, 0x46ccb0
bin_import CG_RegisterCvars, 0x43ac60
bin_import Ragdoll_InitCommands, 0x5bb350
