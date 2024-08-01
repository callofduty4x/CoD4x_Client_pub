#include "cg_scoreboard.h"

#include "q_shared.h"
#include "qcommon.h"
#include "win_sys.h"
#include "client.h"
#include "ui_shared.h"
#include "sys_patch.h"
#include "callbacks.h"

#include <windows.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#include <ctype.h>

int entrypoint();

void WriteSymbol(DWORD addr, void* symbol)
{
	uint32_t* paddr = (uint32_t*)addr;
	*paddr = (uint32_t)symbol;
}

void WriteSymbolRel(DWORD addr, void* symbol)
{
	DWORD width = (DWORD)( (DWORD)symbol - addr - 4);

	uint32_t* paddr = (uint32_t*)addr;
	*paddr = (uint32_t)width;
}

void Patch_Memset(void * ptr, int value, size_t num)
{
	memset(ptr, value, num);
}


void Sys_PatchImageWithBlock(byte *block, int blocksize)
{

    int startadr;
    byte* startadrasbytes = (byte*)&startadr;

    startadrasbytes[0] = block[0];
    startadrasbytes[1] = block[1];
    startadrasbytes[2] = block[2];
    startadrasbytes[3] = block[3];

// printf("Block Start address is: %X\n", startadr);

    memcpy((void*)startadr, &block[4], blocksize - 4);

}


void SetCall(DWORD addr, void* destination){
	byte* baddr = (byte*)addr;

	*baddr = 0xe8;
	baddr++;

	WriteSymbolRel((DWORD)baddr, destination);
}

void SetJump(DWORD addr, void* destination){

	byte* baddr = (byte*)addr;

	*baddr = 0xe9;
	baddr++;

	WriteSymbolRel((DWORD)baddr, destination);
}

void Patch_WinMainEntryPoint(){

	SetCall(0x6748ce, WinMain);
	SetJump(0x67493c, entrypoint);
}

void RemoveStuckInClientCall(){

	Patch_Memset((void*)0x4a9a76, 0x90, 5);

}

void Patch_Dedicated(){
	//SetCall(0x4f8264, SV_ClipMoveToEntity);
	//Broken
}

void Patch_R_SamplerValidate()
{

	static byte patchblock_A[] = {
	0x4e, 0xc3, 0x64, 0x0, 0x90, 0x90, 0x90, 0x90, 0x90, 0x83, 0xc4, 0x08, 0xeb, 0x17
	};

	static byte patchblock_B[] = {
	0xef, 0xbc, 0x64, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x83, 0xc4, 0x08, 0xeb, 0x19
	};
	Sys_PatchImageWithBlock(patchblock_A, sizeof(patchblock_A));
	Sys_PatchImageWithBlock(patchblock_B, sizeof(patchblock_B));
//	SetCall(0x64c34e, Com_PrintError);
//	SetCall(0x64bcef, Com_PrintError);
}


/*void Patch_sub_60B110()
{
	static byte patchblock_C[] = { 	0x0b, 0xb1, 0x60, 0x00,
									0x55 ,0x8b ,0x6c ,0x24 ,0x08 ,0x56 ,0x8b ,0x35 ,0x14 ,
									0xd1 ,0xc9 ,0x0c ,0x8b ,0xc6 ,0xc1 ,0xe8 ,0x05 ,0x0f ,
									0xaf ,0xc5 ,0x8b ,0x0d ,0x20 ,0xa3 ,0xc9 ,0x0c ,0x85 ,
									0xc9 ,0x74 ,0x4b ,0x8b ,0x89 ,0xf0 ,0x00 ,0x00 ,0x00 };

	Sys_PatchImageWithBlock(patchblock_C, sizeof(patchblock_C));
	SetCall(0x44D56A, (void*)0x60b10b);
}*/

const char * s_aspectRatioNames[] = {"auto", "standard", "wide 16:10", "wide 16:9", "ultrawide 21:9", NULL};

__regparm1 float MSG_ReadOriginZFloat(msg_t *msg, float oldValue);
void CG_HudGetClientName(char* name, int clientnum, int size);
void FS_AddUserMapDirIWD_Stub();
void REGPARM(1) NET_OutOfBandPrint_Stub( const char *format, netsrc_t sock, netadr_t adr, ... );
qboolean NET_CompareAdr_Stub(netadr_t* , netadr_t*);
void NET_SendPacket_Stub();
void Sys_PBSendUdpPacket_Stub(unsigned short netport, int len, char* data);
void REGPARM(1) CL_ServersResponsePacket(msg_t *msg, netadr_t from);
const char *__cdecl PB_Q_Serveraddr();
void LAN_GetServerInfo_Stub();
void CLUI_GetServerAddressString_Stub();
void Sys_Cwd(char* path, int len);
void LiveStorage_ReadStatsFromDir_Stub();
void Com_NewPlayerProfile_Stub();
void R_InitGraphicsApi();
void DB_InitZone();
void XAssets_Patch();
void __cdecl R_ShutdownD3D();
void Con_DrawBuildString();
void Scr_UpdateFrame();
void SND_Init();
void R_DirectXError(char *errorstring);
void CG_ExecuteNewServerCommands_Stub( );
void ApplyAntiHackHooks();
int Cvar_StringToEnum();
void Cvar_Reregister_Stub();
void DB_LoadXZoneForModLevel_Stub(char*);
void UI_RegisterCvars();
void Menus_Open();
void Menus_Close();
void CG_StartAmbient(int localClientNum);
void CL_SystemInfoChanged( void );
qboolean FS_ResetPureServerIwds( );
void CL_Vid_Restart_WithClNvidiaCleanup_f();
void removeObjectFromList();
void Com_FindSoundAlias_FastFile();
void CG_EntityEvent_Stub();
void CL_ConfigstringModified();
void UI_DrawGameTypeString_Stub();
void CG_GetEntityBModelBounds();
void Patch_CGWeaponAssets();
void SV_Patches();
void CG_Respawn();
void CG_DrawActive();
void CG_DrawOverheadNames(int localClientNum, int cent, float alpha);
REGPARM(1) void sub_544D00(int a1); //Setting ui_netsource to 0 when 3 --> inc to 4
REGPARM(3) serverInfo_t *LAN_GetServerPtr_Stub( int n, int null, int source );
void UI_BuildServerDisplayList_Stub(int a1, int a2);
void UI_DrawServerRefreshDate();
void UI_DoServerRefresh();
void UI_StopServerRefresh();
qboolean REGPARM(1) Sys_DirectoryHasContent(const char *dir);
void CG_GetClientName_Stub(int, char*);
void CG_DrawFollow(int);
void __cdecl CG_ProcessClientNoteTracks(int, unsigned int clientNum);
void __cdecl Material_Init();
void Mantle_CapView_Stub(int ps);
void CG_ScoresUp_f();
void CG_ScoresDown_f();
float MSG_ReadOriginFloat_Stub(int msg, int bits, float oldv);
void DB_LoadXZoneFromGfxConfig();
HANDLE DB_OpenCoD4XZoneFileHandler(const char*);
HANDLE DB_OpenCommonXZoneFile(const char*, int*);
const char* Con_LinePrefix();
void R_PatchSortedMaterials();
int __regparm1 __MSG_ReadBitsCompress_Server(const byte* input, byte* outputBuf, int readsize);
int unzSkipDataInCurrentFile_Stub(int filehandle, int len);
void __cdecl CL_FirstSnapshot( );
void RB_DrawHudIcon_Stub();
void REGPARM(1) R_StoreWindowSettings(GfxWindowParms_t *wndParms);
/* Well it is not necessary to return truth here */
qboolean Sys_IsLANAddress_Dummy()
{
	return qfalse;
}
void Gfx_PatchRB_ExecuteRendererCommandsLoop();
void CG_InitConsoleCommandsPatched();
void CompleteDvarArgument();
extern char* ui_updatesourcestrings[];
void __cdecl CL_GetRankIcon(int rank, int prestige, Material **handle);
int REGPARM(1) DB_FileSize(const char* zoneName, int source); //DB_FileSize
qboolean REGPARM(2) FS_IwdIsPureStub(int dummy, void* iwd);
void __cdecl FS_AddIwdPureCheckReference(void *search);
void SCR_DrawDemoRecording();
int __stdcall MSS_FileSeekCallback(fileHandle_t f, int offset, unsigned int org);
void FS_ShutdownSearchPathsCoD4( );
qboolean __cdecl DB_ReadData();
void REGPARM(1) DB_UnloadXZoneMemory(void *zone);
qboolean __cdecl R_CreateDevice(struct GfxWindowParms *wndParms);
void Con_RegisterDvar_Stub();
void Material_SortHook();
void Load_MaterialHook();
void DB_AddXAsset_Material_Hook();
void DB_AddXAsset_Techset_Hook();
void DB_CloneXAssetEntry_Stub();
void __cdecl _Load_MaterialTechniqueSetAsset( );
void R_PickRenderer(D3DCAPS9 *caps);
void REGPARM(1) Dvar_SetVariant( );
cvar_t* REGPARM(1) Cvar_SetFromStringByNameExternal(const char* varname, const char* valueString);
void CL_FinishMove(usercmd_t *cmd);
void CalcViewValuesVehicleDriver();
void CG_CalcViewValues(int localClientNum);


/*
void Com_Error_DebugFunction()
{
	char buf[4096];

	//Sys_ShowConsole(1, qfalse);
	buf[0] = '\0';
	Q_strcat(buf, sizeof(buf), Cvar_GetVariantString("sv_referencedIwdNames"));
	Q_strcat(buf, sizeof(buf), "\n");
	Q_strcat(buf, sizeof(buf), Cvar_GetVariantString("sv_referencedFFNames"));
	MessageBoxA(NULL, buf, "Referenced Files are", MB_OK);

	FS_PathToBuffer( buf, sizeof(buf) );
	MessageBoxA(NULL, buf, "Search Path is", MB_OK);
	__asm__("int $3");
}
*/

/*
void Dvar_MakeExplicitType_StringReset(cvar_t* var);
void Dvar_MakeExplicitType_Fixup()
{

	int start = 0x56be0b;

	memset((void*)0x56be0b, NOP, 0x56BEC4 - 0x56be0b);

	byte patch[] =
		{
			NOP,
			PUSH_ESI,
			NOP,NOP,NOP,NOP,NOP,NOP,
			ADD_ESP 4
		};
	memcpy((void*)0x56be0b, patch, sizeof(patch));
	SetCall(0x56be0b + 2, Dvar_MakeExplicitType_StringReset);
}
*/



void Patch_ConsoleCvarInitRemoval()
{
	int i;
	DWORD addressList[] = {0x43DDC1, 0x43DE21, 0x44DAF6, 0x45B7E2, 0x45EF5D, 0x45EFEE, 0x45F049, 0x45F12D, 0x45F277};

	for(i = 0; i < sizeof(addressList)/sizeof(DWORD); ++i)
	{
		Patch_Memset((void*)addressList[i], NOP, 5);
	}
}

void* calloc_dbg(size_t a, size_t b)
{
  char msg[1204];
  Com_sprintf(msg, sizeof(msg), "calloc_dbg: count %d unitsize %d", a, b);
  MessageBoxA(NULL, msg, "Calloc_dbg()", MB_OK);
  return NULL;
}

void* encodePointer(void* ptr)
{
  return ptr;
}

void* decodePointer(void* ptr)
{
  return ptr;
}


struct _finddata64i32i_t {
	unsigned	attrib;
	__time64_t	time_create;
	__time64_t	time_access;
	__time64_t	time_write;
	_fsize_t	size;
	char		name[260];
};

/*
__cdecl intptr_t _findfirst64i32i(const char* _filename, struct _finddata64i32i_t* _fdata) {
    struct _finddata_t fd;
    intptr_t ret = _findfirst(_filename, &fd);
    if (ret == -1) {
        memset(_fdata, 0, sizeof(struct _finddata64i32i_t));
        return ret;
    }
    _fdata->attrib = fd.attrib;
    _fdata->time_create = (__time64_t)fd.time_create;
    _fdata->time_access = (__time64_t)fd.time_access;
    _fdata->time_write  = (__time64_t)fd.time_write;
    _fdata->size        = fd.size;
    strncpy(_fdata->name, fd.name, sizeof(_fdata->name));
    return ret;
}

__cdecl intptr_t _findnext64i32i(intptr_t _fp, struct _finddata64i32i_t* _fdata) {
    struct _finddata_t fd;
    int ret = _findnext(_fp, &fd);
    if (ret == -1) {
      memset(_fdata, 0, sizeof(struct _finddata64i32i_t));
      return ret;
    }
    _fdata->attrib = fd.attrib;
    _fdata->time_create = (__time64_t)fd.time_create;
    _fdata->time_access = (__time64_t)fd.time_access;
    _fdata->time_write  = (__time64_t)fd.time_write;
    _fdata->size        = fd.size;
    strncpy(_fdata->name, fd.name, sizeof(_fdata->name));
    return ret;
}
*/

int __errno_dummy()
{
	return -1;
}

void empty_stub()
{

}


__cdecl int memmove_s__(void * dst, size_t sizeInBytes, const void * src, size_t count)
{
    if (count == 0)
    {
        /* nothing to do */
        return 0;
    }

	/* validation section */
    if(dst == NULL || src == NULL)
	{
		Com_Error(ERR_FATAL, "memmove_s(): dst == NULL or src == NULL");
		return 0;
	}

    if(sizeInBytes < count)
	{
		Com_Error(ERR_FATAL, "memmove_s(): sizeInBytes < count");
		return 0;
	}

    memmove(dst, src, count);
    return 0;
}

__cdecl int memcpy_s__(char* dst, size_t sizeInBytes, const char* src, size_t count)
{
    if (count == 0)
    {
        /* nothing to do */
        return 0;
    }

    /* validation section */
    if(dst == NULL || src == NULL)
	{
		Com_Error(ERR_FATAL, "memcpy_s(): dst == NULL or src == NULL");
		return 0;
	}

    if (sizeInBytes < count)
    {
		Com_Error(ERR_FATAL, "memcpy_s(): sizeInBytes < count");
		return 0;
    }

    memcpy(dst, src, count);
    return 0;
}

__cdecl void strcpy_s__(char *Dst, size_t SizeInBytes, const char *Src)
{
	Q_strncpyz(Dst, Src, SizeInBytes);
}



static void _amsg_exit()
{
	MessageBoxA(NULL, "_amsg_exit called although it shouldn't", "_amsg_exit", MB_OK);
	__asm__("int $3");
}

static void LocaleUpdate_LocaleUpdate()
{
	MessageBoxA(NULL, "LocaleUpdate_LocaleUpdate called although it shouldn't", "LocaleUpdate_LocaleUpdate", MB_OK);
	__asm__("int $3");
}

static void doexit()
{
	MessageBoxA(NULL, "doexit called although it shouldn't", "doexit", MB_OK);
	__asm__("int $3");
}

static void __crtExitProcess()
{
	MessageBoxA(NULL, "__crtExitProcess called although it shouldn't", "__crtExitProcess", MB_OK);
	__asm__("int $3");
}

static void _mtinitlocknum()
{
	MessageBoxA(NULL, "_mtinitlocknum called although it shouldn't", "_mtinitlocknum", MB_OK);
	__asm__("int $3");
}

static void _unlock()
{
	MessageBoxA(NULL, "_unlock called although it shouldn't", "_unlock", MB_OK);
	__asm__("int $3");
}

static void _lock()
{
	MessageBoxA(NULL, "_lock called although it shouldn't", "_lock", MB_OK);
	__asm__("int $3");
}


static void msvc_atexit()
{
	MessageBoxA(NULL, "msvc_atexit called although it shouldn't", "msvc_atexit", MB_OK);
	__asm__("int $3");
}

static void msvc_sub_673248()
{
	MessageBoxA(NULL, "msvc_sub_673248 called although it shouldn't", "msvc_sub_673248", MB_OK);
	__asm__("int $3");
}

static void msvc_NMSG_WRITE()
{
	MessageBoxA(NULL, "msvc_NMSG_WRITE called although it shouldn't", "msvc_NMSG_WRITE", MB_OK);
	__asm__("int $3");
}

static void msvc__exit()
{
	MessageBoxA(NULL, "msvc__exit called although it shouldn't", "msvc__exit", MB_OK);
	__asm__("int $3");
}

static void _invalid_parameter()
{
	MessageBoxA(NULL, "_invalid_parameter called although it shouldn't", "_invalid_parameter", MB_OK);
	__asm__("int $3");
}

static void _calloc_crt()
{
	MessageBoxA(NULL, "_calloc_crt called although it shouldn't", "_calloc_crt", MB_OK);
	__asm__("int $3");
}

static void msvc_sub_674415()
{
	MessageBoxA(NULL, "msvc_sub_674415 called although it shouldn't", "msvc_sub_674415", MB_OK);
	__asm__("int $3");
}

static void _getptd_noexit()
{
	MessageBoxA(NULL, "_getptd_noexit called although it shouldn't", "_getptd_noexit", MB_OK);
	__asm__("int $3");
}


static void msvc_fptrap()
{
	MessageBoxA(NULL, "msvc_fptrap called although it shouldn't", "msvc_fptrap", MB_OK);
	__asm__("int $3");
}

void _87except()
{
	MessageBoxA(NULL, "_87except called although it shouldn't", "_87except", MB_OK);
	__asm__("int $3");
}

void __unwind_handler4()
{
	MessageBoxA(NULL, "__unwind_handler4 called although it shouldn't", "__unwind_handler4", MB_OK);
	__asm__("int $3");
}
void __unwind_handler()
{
	MessageBoxA(NULL, "__unwind_handler called although it shouldn't", "__unwind_handler", MB_OK);
	__asm__("int $3");
}
void _CatchGuardHandler__YA_AW4_EXCEPTION_DISPOSITION__PAUEHExceptionRecord__PAUCatchGuardRN__PAX2_Z()
{
	MessageBoxA(NULL, "enum _EXCEPTION_DISPOSITION __cdecl CatchGuardHandler(struct EHExceptionRecord *,struct CatchGuardRN *,void *,void *) called although it shouldn't", "enum _EXCEPTION_DISPOSITION __cdecl CatchGuardHandler(struct EHExceptionRecord *,struct CatchGuardRN *,void *,void *)", MB_OK);
	__asm__("int $3");
}
void _TranslatorGuardHandler__YA_AW4_EXCEPTION_DISPOSITION__PAUEHExceptionRecord__PAUTranslatorGuardRN__PAX2_Z()
{
	MessageBoxA(NULL, "enum _EXCEPTION_DISPOSITION __cdecl TranslatorGuardHandler(struct EHExceptionRecord *,struct TranslatorGuardRN *,void *,void *) called although it shouldn't", "enum _EXCEPTION_DISPOSITION __cdecl TranslatorGuardHandler(struct EHExceptionRecord *,struct TranslatorGuardRN *,void *,void *)", MB_OK);
	__asm__("int $3");
}
void __except_handler3()
{
	MessageBoxA(NULL, "__except_handler3 called although it shouldn't", "__except_handler3", MB_OK);
	__asm__("int $3");
}
void sub_6902A8()
{
	MessageBoxA(NULL, "Exception handler sub_6902A8 called although it shouldn't", "sub_6902A8", MB_OK);
	__asm__("int $3");
}
void sub_6902C3()
{
	MessageBoxA(NULL, "Exception handler sub_6902C3 called although it shouldn't", "sub_6902C3", MB_OK);
	__asm__("int $3");
}
void SEH_575A00()
{
	MessageBoxA(NULL, "Exception handler SEH_575A00 called although it shouldn't", "SEH_575A00", MB_OK);
	__asm__("int $3");
}
void SEH_575850()
{
	MessageBoxA(NULL, "Exception handler SEH_575850 called although it shouldn't", "SEH_575850", MB_OK);
	__asm__("int $3");
}
void SEH_53B9B0()
{
	MessageBoxA(NULL, "Exception handler SEH_53B9B0 called although it shouldn't", "SEH_53B9B0", MB_OK);
	__asm__("int $3");
}
void SEH_575A70()
{
	MessageBoxA(NULL, "Exception handler SEH_575A70 called although it shouldn't", "SEH_575A70", MB_OK);
	__asm__("int $3");
}
void SEH_56EF50()
{
	MessageBoxA(NULL, "Exception handler SEH_56EF50 called although it shouldn't", "SEH_56EF50", MB_OK);
	__asm__("int $3");
}
void SEH_53B360()
{
	MessageBoxA(NULL, "Exception handler SEH_53B360 called although it shouldn't", "SEH_53B360", MB_OK);
	__asm__("int $3");
}
void SEH_53B4B0()
{
	MessageBoxA(NULL, "Exception handler SEH_53B4B0 called although it shouldn't", "SEH_53B4B0", MB_OK);
	__asm__("int $3");
}
void SEH_53BA40()
{
	MessageBoxA(NULL, "Exception handler SEH_53BA40 called although it shouldn't", "SEH_53BA40", MB_OK);
	__asm__("int $3");
}
void sub_6899A8()
{
	MessageBoxA(NULL, "Exception handler sub_6899A8 called although it shouldn't", "sub_6899A8", MB_OK);
	__asm__("int $3");
}
void ___DestructExceptionObject()
{
	MessageBoxA(NULL, "Exception handler ___DestructExceptionObject called although it shouldn't", "___DestructExceptionObject", MB_OK);
	__asm__("int $3");
}

void __stdcall __CxxThrowException(int a, int b)
{
	Com_Error(0, "Unhandled exception (__CxxThrowException). Can not continue");
}

void __stdcall  _Xlen()
{
	Com_Error(0, "string too long (_Xlen). Can not continue");
}

void __stdcall  _Xran()
{
	Com_Error(0, "invalid string position (_Xran). Can not continue");
}
void __SEH_prolog4()
{
	MessageBoxA(NULL, "Exception handler __SEH_prolog4 called although it shouldn't", "__SEH_prolog4", MB_OK);
	__asm__("int $3");
}
void __SEH_epilog4()
{
	MessageBoxA(NULL, "Exception handler __SEH_epilog4 called although it shouldn't", "__SEH_epilog4", MB_OK);
	__asm__("int $3");
}
void __except_handler4()
{
	MessageBoxA(NULL, "__except_handler4 called although it shouldn't", "__except_handler4", MB_OK);
	__asm__("int $3");
}
void sub_50B2A0() //Maybe important?
{

}
void sub_67358B()
{
	MessageBoxA(NULL, "sub_67358B called although it shouldn't", "sub_67358B", MB_OK);
	__asm__("int $3");
}

void RB_BeginSurface_Stub();


FILE** stdio_list()
{
	static FILE* list[3];
	list[0] = stdout;
	list[1] = stdin;
	list[2] = stderr;
	return list;
}

static void* operator_new(size_t a1)
{
	void* r = malloc(a1);
	if(r == NULL)
	{
		Com_Error(ERR_FATAL, "operator_new failed");
	}
	return r;
}

static void _purecall()
{
	Com_Error(ERR_FATAL, "_purecall");
}

int _initstub()
{
	return 1;
}

int _initstub0()
{
	return 0;
}


double _CIsqrt();
double _CIpow();
double _CIlog();
double _ftol2_sse();
double _CIexp();
double _CIasin();
double _CIatan();
double _CIatan2();
double _CIsin();
double _CIcos();
double _CIacos();
double _CItan();

qboolean __stdcall Sub_57B5C0_Patch(int a, int b, int c);


void Patch_CoD4XXZone()
{
  Patch_Memset((char*)0x48A923, NOP, 0x48A996 - 0x48A923); //NOP out the file open stuff of mp_patch.ff, we add our own function which will open mp_patch.ff file and return handle or -1
  *(int32_t*)0x48a909 = 5; //Check for "cod4x..." instead "mp_patch"
  WriteSymbol(0x48a904, "cod4x"); //"cod4x" instead "mp_patch"
  *(char*)0x48A923 = PUSH_EBP; //Push zone name onto stack
  SetCall(0x48A924, DB_OpenCoD4XZoneFileHandler);
  *(char*)0x48A929 = 0x83; //Add esp, 4
  *(char*)0x48A92a = 0xc4;
  *(char*)0x48A92b = 4;

  Patch_Memset((char*)0x48A9A5, NOP, 0x48A9B9 - 0x48A9A5); //Wipe error if not found message (do our own)

  Patch_Memset((char*)0x48A9C6, NOP, 0x48AA38 - 0x48A9C6);

  //Has to push zonename and type pointer

  *(char*)0x48A9C6 = 0x8D;  //LEA zonetype pointer
  *(char*)0x48A9C7 = 0x54;
  *(char*)0x48A9C8 = 0x24;
  *(char*)0x48A9C9 = 0x14;
  *(char*)0x48A9CA = PUSH_EDX; //Zonetype
  *(char*)0x48A9CB = PUSH_EBP; //Zonename

  SetCall(0x48A9CC, DB_OpenCommonXZoneFile);
  *(char*)0x48A9D1 = 0x83; //Add esp, 8
  *(char*)0x48A9D2 = 0xc4;
  *(char*)0x48A9D3 = 8;

  Patch_Memset((char*)0x48AA43, NOP, 0x48AA6D - 0x48AA43);
}

float cgfov90 = 90.0f;

void Patch_Other(){
	SetJump(0x576cd0, Com_QueueEvent);
	SetJump(0x4fca50, Com_PrintMessage);

	SetJump(0x508b40, NET_SendPacket_Stub);
	SetJump(0x578080 ,Sys_PBSendUdpPacket_Stub);


	SetJump(0x4764d0, CL_LoadCachedServers);
	SetJump(0x476540, CL_SaveServersToCache);

	SetJump(0x4720f0, CL_ServerStatus);


	SetCall(0x469641, CL_CheckForResend);
	SetCall(0x46c9d2, CL_CheckForResend);

	SetJump(0x469b20, CL_RequestAuthorization);

	Com_Memset((void*)0x46c0e3, NOP, 64); //Patch old CL_ConnectionlessPacket function so that it process only PunkBuster stuff
											//This Dirty trick is needed as we receive otherwise PB HACK #132056 kick if we call cb() directly

	SetJump(0x463F50, CL_WritePacket);
	//SetCall(0x447cbd, CL_GetServerIPAddress);
	SetJump(0x46B470, PB_Q_Serveraddr);
	SetJump(0x475600, LAN_GetServerInfo_Stub);
	SetJump(0x476370, CLUI_GetServerAddressString_Stub);
	SetJump(0x46AC60, CL_InitDownloads);
	SetJump(0x5C0270, CL_SendPbPacket);
	SetJump(0x4703C0, CL_UpdateDirtyPings);
	SetJump(0x4F99E0, Cmd_RemoveCommand);
	SetJump(0x4694E0, CL_MapLoading);
	SetCall(0x5799cf, LiveStorage_DecodeStatsData);
	SetCall(0x579c0f, LiveStorage_WriteStatsChecksums);
	SetJump(0x476660, CL_WriteVoicePacket);
	SetCall(0x474227, Sys_IsLANAddress_Dummy);
	SetJump(0x463E60, CL_ReadyToSendPacket);
	SetJump(0x4FD4E0, Com_Quit_f);
	SetJump(0x576A30, Sys_Error);
	SetJump(0x474440, CL_ParseDownload);
	SetJump(0x500940, CL_WWWDownloadStop);
	SetJump(0x54DE70, Item_RunScript);
	SetJump(0x543410, UI_DrawBuildString);
	SetJump(0x67401c, Sys_Cwd);
//	SetJump(0x579a02, (void*)0x579b29);
	*(byte*)0x4fe930 = 0xc3;
	Patch_R_SamplerValidate();

#ifdef OFFICIAL

	ApplyAntiHackHooks();


#endif

	SetJump(0x671E8F, fopen);
	SetJump(0x671D4F, fclose);
	SetJump(0x67213E, fread);
	SetJump(0x6722B8, fwrite);
	SetJump(0x6724D2, ftell);
	SetJump(0x6725C7, fseek);
	SetJump(0x673b19, fflush);
  SetJump(0x67650F, fputs);
	SetJump(0x675F80, fgets);
	SetJump(0x672DBC, fprintf);
	SetJump(0x6743F9, vfprintf);


  //For Dr. Memory (Original Com_Memset does illegible reads)
  SetJump(0x564ed0, memset);

  SetJump(0x674242, calloc);
  SetJump(0x671B6D, malloc);
  SetJump(0x672F0B, realloc);
  SetJump(0x670da6, free);

//  SetJump(0x676960, encodePointer);
//  SetJump(0x676960, decodePointer);

  SetJump(0x6706f5, vsnprintf);
  SetJump(0x67096C, sprintf);
  SetJump(0x672BD4, asctime);
  SetJump(0x67521A, tmpnam);

  SetJump(0x6766BB, __errno_dummy);
  SetJump(0x6766CE, __errno_dummy); //doserrno
  SetJump(0x670D45, _ctime64); //ctime64!!!
  SetJump(0x6729C2, _localtime64); //localtime64
  SetJump(0x674F62, _gmtime64); //gmtime64
  SetJump(0x674955, abort);

  SetJump(0x673bdb, _findfirst64i32); //findfirst64i32
  SetJump(0x673D0B, _findnext64i32);
  SetJump(0x673E3A, _findclose);
  SetJump(0x670E9C, sscanf);
  SetJump(0x671249, atof);
  SetJump(0x671320, qsort);
  SetJump(0x6718FD, empty_stub); //printf
  SetJump(0x671A43, getenv);
  SetJump(0x671C30, snprintf);
  SetJump(0x674D19, _mktime64); //libwww requires
  SetJump(0x6890AC, putenv);
  SetJump(0x67367D, memmove_s__);
  SetJump(0x6737E6, memcpy_s__);
  SetJump(0x67CBC0, memcpy);
  SetJump(0x67468A, chmod);
  SetJump(0x67546A, strtol);
  SetJump(0x675F63, strftime);
  SetJump(0x6761DD, empty_stub); //stat64i32 libwww
  SetJump(0x67B359, strcpy_s__);
  SetJump(0x67B671, close);
  SetJump(0x67B73E, fileno);
  SetJump(0x67F6D1, access); //libwww
  SetJump(0x680080, stricmp);
  SetJump(0x684B2B, strnicmp);
  SetJump(0x688E3B, strlwr);

  SetJump(0x670615, srand);
  SetJump(0x670622, rand);
  SetJump(0x670945, tolower);
  SetJump(0x670F0F, isalpha);
  SetJump(0x670F89, isupper);
  SetJump(0x671001, isdigit);
  SetJump(0x6710F8, isspace);
  SetJump(0x671175, isalnum);
  SetJump(0x6718D6, toupper);

  SetJump(0x6702B0, memmove);
  SetJump(0x670710, strstr);
  SetJump(0x670796, atol);
  SetJump(0x6707AC, LocaleUpdate_LocaleUpdate);
  SetJump(0x6709F0, strncpy);
  SetJump(0x670B14, strncmp);
  SetJump(0x670C10, strncat);
  SetJump(0x6715D0, ceil);
  SetJump(0x671700, strchr);
  SetJump(0x671AC6, rename);
  SetJump(0x671AF4, remove);
  SetJump(0x67264C, longjmp);
  SetJump(0x672C1B, stdio_list);
  SetJump(0x67319F, _amsg_exit);
  SetJump(0x67334D, doexit);
  SetJump(0x6731E9, __crtExitProcess);
  SetJump(0x678A40, _mtinitlocknum);
  SetJump(0x678A2B, _unlock);
  SetJump(0x678B03, _lock);
  SetJump(0x674678, msvc_atexit);
  SetJump(0x673248, msvc_sub_673248);
  SetJump(0x67B3D8, msvc_NMSG_WRITE);
  SetJump(0x67342F, exit);
  SetJump(0x673440, msvc__exit);
  //Not patched 006734C0     ; public: __thiscall std::bad_alloc::bad_alloc(void)

  SetJump(0x6734D9, operator_new);
  SetJump(0x673550, strrchr);
  SetJump(0x6735D0, memchr);
  SetJump(0x673967, _invalid_parameter);
  SetJump(0x673E5A, mkdir);
  SetJump(0x673E86, rmdir);
  SetJump(0x674065, strtok);
  SetJump(0x67DE5A, _calloc_crt);
  SetJump(0x674281, _purecall);
  SetJump(0x674415, msvc_sub_674415); //libwww
  SetJump(0x676651, ferror);
  SetJump(0x676B05, _getptd_noexit);
  SetJump(0x679830, strlen);
  SetJump(0x6846F6, msvc_fptrap);
  SetJump(0x689131, itoa);

  //Theoretical wrapping:
  /*
  static float CIsqrt();
  __asm__(
    "_CIsqrt:\n"
    "push %edx;\n"
    "sub $20,%esp\n"
    "fstpl 0x0(%esp)\n"
    "call _sqrt\n"
    "add $20,%esp\n"
    "pop %edx\n"
    "ret\n");
  */

  SetJump(0x68AEC0, _CIsqrt);
  SetJump(0x68AFC0, _CIpow);
  SetJump(0x68B250, _CIlog);
  SetJump(0x68B360, (void*)0x68B396); //Map _ftol2_sse to _ftol2
  SetJump(0x68B44C, _CIexp);
  SetJump(0x68B630, _CIasin);
  SetJump(0x68B780, _CIatan);
  SetJump(0x68B86A, _CIatan2);
  SetJump(0x68B8C0, _CIsin);
  SetJump(0x68B9F0, _CIcos);
  SetJump(0x68BB20, _CIacos);
  SetJump(0x68BC70, _CItan);
  SetJump(0x68BD60, floor);
  SetJump(0x68BE84, _setjmp3);
  SetJump(0x68F381, _87except);

  SetJump(0x67BBF8, __unwind_handler4);
  SetJump(0x67D2D4, __unwind_handler);
  SetJump(0x689218, _CatchGuardHandler__YA_AW4_EXCEPTION_DISPOSITION__PAUEHExceptionRecord__PAUCatchGuardRN__PAX2_Z);
  SetJump(0x68931D, _TranslatorGuardHandler__YA_AW4_EXCEPTION_DISPOSITION__PAUEHExceptionRecord__PAUTranslatorGuardRN__PAX2_Z);
  SetJump(0x68B4CC, __except_handler3);
  SetJump(0x6902A8, sub_6902A8);
  SetJump(0x6902C3, sub_6902C3);
  SetJump(0x6902E0, SEH_575A00);
  SetJump(0x6902F0, SEH_575850);
  SetJump(0x690300, SEH_53B9B0);
  SetJump(0x690310, SEH_575A70);
  SetJump(0x690320, SEH_56EF50);
  SetJump(0x690330, SEH_53B360);
  SetJump(0x690340, SEH_53B4B0);
  SetJump(0x690350, SEH_53BA40);
  SetJump(0x6899A8, sub_6899A8);
  SetJump(0x689B90, ___DestructExceptionObject);
  SetJump(0x679634, __SEH_prolog4);
  SetJump(0x679679, __SEH_epilog4);
  SetJump(0x679690, __except_handler4);
  SetJump(0x67E71A, __CxxThrowException);
  SetJump(0x67358B, sub_67358B);
  SetJump(0x685ED8, _Xlen);
  SetJump(0x685F17, _Xran);

  //???
  SetJump(0x50B2A0, sub_50B2A0);




  (*(byte*)0x67df3d) = 0xcc; //_fcloseall


	SetJump(0x468F90, CL_ReadDemoData);
	SetJump(0x4690C0, CL_ReadDemoMessage);

	SetJump(0x468EA0, CL_ReadDemoArchive);
	SetJump(0x468DF0, CL_DemoCompleted);
	Patch_Memset((void*)0x45c5dc, NOP, 14);
	SetCall(0x45c5dc, CL_ReadDemoMessage);

	SetCall(0x46A636, SND_Init);
	SetJump(0x55DD30, FS_AddUserMapDirIWD_Stub);
	SetJump(0x4FADC0, Com_CheckPlayerProfile);
	SetJump(0x579900, LiveStorage_ReadStatsFromDir_Stub);
	SetJump(0x579B40, LiveStorage_UploadStats);
	SetJump(0x4F9470, Cmd_Exec_f);
	SetJump(0x4FAFE0, Com_NewPlayerProfile_Stub);
	SetCall(0x5453CE, Com_DeletePlayerProfile);
	SetJump(0x5F4CC0, R_InitGraphicsApi);
	SetCall(0x48a2cc, DB_InitZone);
	XAssets_Patch();
	SetCall(0x5f508d, R_ShutdownD3D);
	/* Overrides error in CG_Init() about bad client/server game */
	Com_Memset((void*)0x44004c, NOP, 5);
	SetJump(0x4696b0, CL_Disconnect);
	SetCall(0x461C74, Con_DrawBuildString);
	SetJump(0x474ff0, Scr_UpdateFrame);
	SetJump(0x5F3300, R_DirectXError);
	SetJump(0x44C630, CG_ExecuteNewServerCommands_Stub);
	SetJump(0x56BFF0, Cvar_Reregister_Stub);
	SetJump(0x56A3D0, Cvar_StringToEnum);
	Patch_ConsoleCvarInitRemoval();
	SetJump(0x501230, Cvar_Command);
	SetCall(0x548FBA, UI_RegisterCvars);
	SetJump(0x550a50, Menus_Open);
	SetJump(0x54c420, Menus_Close);
//	Patch_sub_60B110();

	SetCall(0x45AF61, CL_SystemInfoChanged);
	SetCall(0x55EDD8, FS_ResetPureServerIwds);
	SetCall(0x4FD0F7, FS_ResetPureServerIwds);
	SetCall(0x46A64B, CL_Vid_Restart_WithClNvidiaCleanup_f);
	//SetCall(0x45BFB4, DB_LoadXZoneForModLevel_Stub);
	//SetJump(0x43F200, CG_StartAmbient);


	Com_Memset((void*)0x5A762E, NOP, 0x29);

	*(byte*)0x5A7630 = 0x57;

	SetCall(0x5A7631, removeObjectFromList);

	*(byte*)0x5A7636 = 0x83;
	*(byte*)0x5A7637 = 0xc4;
	*(byte*)0x5A7638 = 0x04;


	SetJump(0x565590, Com_FindSoundAlias_FastFile);
	SetCall(0x43765F, CG_EntityEvent_Stub);
	SetCall(0x43759B, CG_EntityEvent_Stub);
	SetCall(0x45B3E3, CL_ConfigstringModified);
	WriteSymbol(0x71C7d8, "non impact mark material used for impact mark\n");
	SetCall(0x44DE91, CG_Respawn);
	SetCall(0x44D5F1, CG_Respawn);

	SetCall(0x43DC15, CG_RegisterScoreboardCvars);
	SetCall(0x468225, Scoreboard_HandleInput);
	SetCall(0x42F715, CG_DrawScoreboard);
	SetCall(0x42F109, CG_DrawScoreboard);

	SetJump(0x42D770, CG_DrawOverheadNames);
	SetJump(0x475FB0, CG_GetClientName_Stub);
    SetCall(0x438cad, CG_HudGetClientName);

  Com_Memset((void*)0x438ca6, NOP, 5);
  Com_Memset((void*)0x438c9b, NOP, 6);

	SetCall(0x42F76B, CG_DrawFollow);
	SetJump(0x433970, CG_ProcessClientNoteTracks);
	SetJump(0x543940, UI_DrawGameTypeString_Stub);
  SetJump(0x573200 ,Sys_DirectoryHasContent);
  SetJump(0x544D00, sub_544D00); //ui_netsource
  SetJump(0x4755C0, LAN_WaitServerResponse);
  SetJump(0x475A40 ,LAN_GetServerPtr_Stub);
  SetJump(0x5472B0, UI_BuildServerDisplayList_Stub);
  SetJump(0x545980, UI_BuildServerDisplayListByNetSource);
  SetJump(0x543EF0, UI_DrawServerRefreshDate);
  SetJump(0x54A9C0, UI_StopServerRefresh);
  SetJump(0x5F2B30, Material_Init);
  SetJump(0x409250, Mantle_CapView_Stub);

/*
  SetJump(0x42A9D0, CG_ScoresUp_f);
  SetJump(0x42AAA0, CG_ScoresDown_f);
*/

  SetJump(0x54AA40, UI_DoServerRefresh);

  static byte patchblock_sndsys1[] = {
	   0xe8, 0x00, 0x00, 0x00, 0x00, 0x85, 0xc0, 0x0f, 0x84, 0x22, 0x02, 0x00, 0x00
	};
  static byte patchblock_sndsys2[] = {
     0xe8, 0x00, 0x00, 0x00, 0x00, 0x85, 0xc0, 0x0f, 0x84, 0x88, 0x01, 0x00, 0x00
  };
  Patch_Memset((void*)0x57b613, 0x90, 0x16);
  memcpy((void*)0x57b613, patchblock_sndsys1, sizeof(patchblock_sndsys1));
  SetCall(0x57b613, Sub_57B5C0_Patch);

  Patch_Memset((void*)0x57b3a3, 0x90, 0x16);
  memcpy((void*)0x57b3a3, patchblock_sndsys2, sizeof(patchblock_sndsys2));
  SetCall(0x57b3a3, Sub_57B5C0_Patch);
  WriteSymbol(0x543d1d, ui_updatesourcestrings);
  WriteSymbol(0x543A6C, ui_updatesourcestrings);

  SetCall(0x506C04, MSG_ReadOriginFloat_Stub);
  SetJump(0x506680, MSG_ReadOriginZFloat);

//  SetJump(0x61A220, RB_BeginSurface_Stub); //No reason to hook it

  Patch_CGWeaponAssets();
  SV_Patches();
  Gfx_PatchRB_ExecuteRendererCommandsLoop();


  SetJump(0x5F3DA2 ,DB_LoadXZoneFromGfxConfig);
  SetCall(0x5F4810 ,DB_LoadXZoneFromGfxConfig);

  *(char*)0x467FC2 = JMP_SHORT; //Kill the "Monkeytoy"
  Patch_Memset((char*)0x4683EF, NOP, 6); //Kill the sv_disableClientConsole

  Patch_CoD4XXZone();

  SetCall(0x460613, Con_LinePrefix);
  SetCall(0x52D12D, __MSG_ReadBitsCompress_Server); //BufferOverflow fix

//  R_PatchSortedMaterials();
//  SetCall(0x55C402, unzSkipDataInCurrentFile_Stub);

	SetCall(0x44B565, CL_FirstSnapshot);
	SetCall(0x45C49A, CL_FirstSnapshot);
	SetCall(0x45C515, CL_FirstSnapshot);

	Patch_Memset((char*)0x42B189, NOP, 1033); //Wipe commands
	SetCall(0x42B189, CG_InitConsoleCommandsPatched);
	SetCall(0x614382, RB_DrawHudIcon_Stub);

	SetCall(0x47060C, LoadMapLoadscreen);
	SetCall(0x467073, CompleteDvarArgument);
	SetCall(0x5F4838, R_StoreWindowSettings);
	WriteSymbol(0x62D35E, s_aspectRatioNames);
	SetCall(0x44A671, CL_GetRankIcon);
	SetCall(0x503e65, FS_Startup);
	SetCall(0x55ec5d, FS_Startup);
	SetCall(0x55ed4d, FS_Startup);
	SetCall(0x5039d1, DB_FileSize);
	SetCall(0x55bb63, FS_IwdIsPureStub);
	SetCall(0x55bcc8, FS_AddIwdPureCheckReference);

    SetJump(0x0040BC40, BG_CanItemBeGrabbed_Hook);
	SetCall(0x55DC10, FS_LoadZipFile);
	SetJump(0x55b960, FS_FOpenFileReadForThread);
	SetJump(0x55B3B0, FS_FCloseFile);
	SetCall(0x474F36 ,SCR_DrawDemoRecording);
	SetJump(0x55C120, FS_Read);
	SetJump(0x573310, MSS_FileSeekCallback);
	SetJump(0x502CF0, FS_SV_FOpenFileRead);
	SetCall(0x55E1AD, FS_ShutdownSearchPathsCoD4);
	SetJump(0x4779F0, DB_ReadData);
	SetJump(0x48B0E0, DB_UnloadXZoneMemory);
	//SetCall(0x5F47F5, R_CreateDevice); //broken, needs fix
	SetCall(0x452C8E, CG_DrawActive);

	Patch_Memset((char*)0x45E580, NOP, 0x45E5F1 - 0x45E580);
	SetCall(0x45E5F6, Con_RegisterDvar_Stub);
	WriteSymbol(0x43ad8d, &cgfov90);

	SetCall(0x5417F9, UI_SetSystemCursorPos);
	SetCall(0x54B298, UI_SetSystemCursorPos);
	SetCall(0x452A44, IN_Frame);
//	SetCall(0x6029F6, Material_SortHook);
//	SetCall(0x621740, Material_SortHook);
//	SetCall(0x47BD09, Load_MaterialHook);
//	SetCall(0x47BD1D, DB_AddXAsset_Material_Hook);
//	SetCall(0x488BA1, DB_AddXAsset_Techset_Hook);
	SetCall(0x489D40, DB_CloneXAssetEntry_Stub);
	SetCall(0x489DCF, DB_CloneXAssetEntry_Stub);

	SetCall(0x47B96E, _Load_MaterialTechniqueSetAsset);
	SetCall(0x5F39D6, R_PickRenderer);
	SetJump(0x56B1B0, Dvar_SetVariant);
/*
	SetCall(0x54FBFF, Cvar_SetFromStringByNameExternal);
	SetCall(0x54FDB9, Cvar_SetFromStringByNameExternal);
	SetCall(0x550149, Cvar_SetFromStringByNameExternal);
	SetCall(0x54F98C, Cvar_SetFromStringByNameExternal);
*/
	SetCall(0x54F538, Cvar_SetFromStringByNameExternal);
	SetCall(0x463DEF, CL_FinishMove);

    // Unused for now - breaks killcam entity link
    // Unfinished work from afe7c8f9fc7db2c714b15e7ab412d6e63183eda9
    //SetCall(0x452A70, CG_CalcViewValues);
    //SetCall(0x451E22, CG_CalcViewValues);

}





qboolean Patch_MainModule(void(patch_func)()){

	int i;
	DWORD dwSectionSize = 0;
	DWORD dwSectionBase;
	HMODULE hModule;
	hModule = GetModuleHandleA(NULL);
	if (! hModule)
		return qfalse;

	// get dos header
	IMAGE_DOS_HEADER dosHeader = *(IMAGE_DOS_HEADER*)(hModule);

	// get nt header
	IMAGE_NT_HEADERS ntHeader = *(IMAGE_NT_HEADERS*)((DWORD)hModule + dosHeader.e_lfanew);

	// iterate through section headers and search for ".text" section
	int nSections = ntHeader.FileHeader.NumberOfSections;
	for ( i = 0; i < nSections; i++)
	{
		IMAGE_SECTION_HEADER sectionHeader = *(IMAGE_SECTION_HEADER*) ((DWORD)hModule + dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS) + i*sizeof(IMAGE_SECTION_HEADER));

		if (strcmp((char*)sectionHeader.Name,".text") == 0)
		{
			// the values you need
			dwSectionBase = (DWORD)hModule + sectionHeader.VirtualAddress;
			dwSectionSize = sectionHeader.Misc.VirtualSize;
			break;
		}
	}
	if(!dwSectionSize)
			return qfalse;

	// unprotect the text section
	DWORD oldProtectText;
	VirtualProtect((LPVOID)dwSectionBase, dwSectionSize, PAGE_READWRITE, &oldProtectText);
    patch_func();
	VirtualProtect((LPVOID)dwSectionBase, dwSectionSize, PAGE_EXECUTE_READ, &oldProtectText);

	return qtrue;
}


// time_t time (time_t* timer)
// {
// 	return (__time32_t)_time32((__time32_t*)timer);
// }
