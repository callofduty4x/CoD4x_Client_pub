#include "q_shared.h"
#include "qcommon.h"
#include "client.h"
#include "xassets/material.h"
#include "xzone.h"
#include "sys_patch.h" 
#include <stdlib.h>

#define NUM_ASSETTYPES 33
#define g_assetNames_ADDR 0x726840
#define DB_FreeXAssetHeaderHandler_ADDR 0x0
#define DB_DynamicCloneXAssetHandler_ADDR 0x0
#define DB_GetXAssetSizeHandler_ADDR 0x726a10
#define DB_InitPoolHeaderHandler_ADDR 0x726448
#define g_zoneInitialized (*(byte*)0xd5ec42c)
#define g_freeAssetEntryHead (*(XAsset_t **)0xE34EEC)

#define databaseCompletedEvent (*(volatile HANDLE*)0x14E89A4)
#define resumedDatabaseEvent (*(volatile HANDLE*)0x14E8908)
cvar_t* r_xassetnum;



void *DB_XAssetPool[ASSET_TYPE_COUNT];
int g_poolsize[ASSET_TYPE_COUNT];
int XAssetStdCount[ASSET_TYPE_COUNT];
int XAssetRequestedCount[ASSET_TYPE_COUNT];

int (__cdecl **DB_GetXAssetSizeHandler)() = (int (__cdecl **)())DB_GetXAssetSizeHandler_ADDR;
void (__cdecl **DB_InitPoolHeaderHandler)(void *pool, int size) = (void (__cdecl **)(void*, int))DB_InitPoolHeaderHandler_ADDR;

Material_t* Material_Register_FastFile(const char* name, int track);
Material_t* Material_Register_LoadObj(const char* name, int track);
void DB_PureInfoDestroy();


XAsset_t* XAssetHeads = (XAsset_t*)0xF0D650;
const char** g_assetNames = (const char**)g_assetNames_ADDR;
/* Temp only: */
void **DB_XAssetStdPool = (void **)0x7265e0;
//int *g_poolsize = (int*)0x7263a0;

int DB_GetXAssetTypeSize(XAssetType type)
{
        if(DB_GetXAssetSizeHandler[type] == NULL)
        {
                return 0;
        }
        return DB_GetXAssetSizeHandler[type]();
}

int DB_GetXAssetStdCount(XAssetType type)
{
        return XAssetStdCount[type];
}

const char* DB_GetXAssetTypeName(XAssetType type)
{
        return g_assetNames[type];
}

//Are that headers ? I'm not sure
typedef struct XModelAssetsHeader_s{
        struct XModelAssetsHeader_s*    next;
        char                    data[216];
}XModelAssetsHeader_t;

typedef struct WeaponDefHeader_s{
        struct WeaponDefHeader_s*       next;
        char                    data[2164];
}WeaponDefHeader_t;

typedef struct XAssetsHeaderCommon_s{
        struct XAssetsHeaderCommon_s*   next;
}XAssetsHeaderCommon_t;

qboolean DB_XAssetNoAlloc(XAssetType i)
{
        if(i == ASSET_TYPE_CLIPMAP)
                return qtrue;
        if(i == ASSET_TYPE_CLIPMAP_PVS)
                return qtrue;
        if(i == ASSET_TYPE_COMWORLD)
                return qtrue;
        if(i == ASSET_TYPE_GAMEWORLD_SP)
                return qtrue;
        if(i == ASSET_TYPE_GAMEWORLD_MP)
                return qtrue;
        if(i == ASSET_TYPE_MAP_ENTS)
                return qtrue;
        if(i == ASSET_TYPE_GFXWORLD)
                return qtrue;
        if(i == ASSET_TYPE_UI_MAP)
                return qtrue;
        if(i == ASSET_TYPE_SNDDRIVER_GLOBALS)
                return qtrue;
        if(i == ASSET_TYPE_AITYPE)
                return qtrue;
        if(i == ASSET_TYPE_MPTYPE)
                return qtrue;
        if(i == ASSET_TYPE_CHARACTER)
                return qtrue;
        if(i == ASSET_TYPE_LOCALIZE_ENTRY)
                return qtrue;
        if(i == ASSET_TYPE_XMODELALIAS)
                return qtrue;
        if(i == ASSET_TYPE_RAWFILE)
                return qtrue;
        if(i == ASSET_TYPE_MENU)
                return qtrue;
        if(i == ASSET_TYPE_WEAPON)
                return qtrue;
        if(i == ASSET_TYPE_STRINGTABLE)
                return qtrue;
        if(i == ASSET_TYPE_MATERIAL)
                return qtrue;
        return qfalse;
}

void DB_CustomAllocOnce(XAssetType type)
{
        int count = DB_GetXAssetStdCount(type);
        int typesize = DB_GetXAssetTypeSize(type);

        DB_XAssetPool[type] = Z_Malloc((count+1) * typesize);
        if(DB_XAssetPool[type] == NULL)
        {
                g_poolsize[type] = 0;
        }
}

void DB_InitXAssetMem()
{
        int i;

        Com_Memcpy(DB_XAssetPool ,DB_XAssetStdPool ,sizeof(DB_XAssetPool));

        for(i = 0;  i < ASSET_TYPE_COUNT; ++i)
        {
                g_poolsize[i] = DB_GetXAssetStdCount(i);
        }

        DB_CustomAllocOnce( ASSET_TYPE_MENU );
        DB_CustomAllocOnce( ASSET_TYPE_WEAPON );
        DB_CustomAllocOnce( ASSET_TYPE_STRINGTABLE );
}

void DB_RelocateXAssetMem()
{
        int i, typesize, count;

        void* newmem;

/*
        for(i = 0; i < 32767; ++i)
        {
                if(g_assetEntryPool[i].header)
                {
                        if(g_assetEntryPool[i].this.type > ASSET_TYPE_COUNT || g_assetEntryPool[i].this.type < 0)
                                ;//Com_Printf("^2Not Free XAsset with nextheader at %d\n", i);
                        else
                                Com_Printf("^5Not Free XAsset: %s at %d\n", DB_GetXAssetTypeName(g_assetEntryPool[i].this.type),i);
                }
        }
*/

        for(i = 0;  i < ASSET_TYPE_COUNT; ++i)
        {

                if(DB_XAssetNoAlloc(i))
                {
                        continue;
                }

                if(XAssetRequestedCount[i] <= g_poolsize[i])
                {
                        //Only allocate if we need more than what is already allocated
                        continue;
                }

                count = XAssetRequestedCount[i];
                typesize = DB_GetXAssetTypeSize(i);

                newmem = Z_Malloc((count+1) * typesize);

                if(newmem == NULL)
                {
                        continue;
                }

                if(DB_XAssetPool[i] != DB_XAssetStdPool[i] && DB_XAssetPool[i] != NULL)
                {
                        Z_Free(DB_XAssetPool[i]);
                }
               // Com_Printf("^2Reallocate %d XAssets on request of type: %s\n", count, DB_GetXAssetTypeName(i));
                DB_XAssetPool[i] = newmem;
                g_poolsize[i] = count;
                DB_InitPoolHeaderHandler[i](DB_XAssetPool[i], g_poolsize[i]);
        }

//        Com_Printf("^4Zone relocated\n");
}

void DB_ClearXAssetMem()
{

//      XAssetType i;

        g_zoneInitialized = 0;
 //       Com_Printf("^6XAssets storage clearing...\n");
/*
        for(i = 0;  i < ASSET_TYPE_COUNT; ++i)
        {
                if(DB_XAssetNoAlloc(i))
                {
                        continue;
                }
                if(DB_XAssetPool[i] == NULL)
                {
                        continue;
                }
                Z_Free(DB_XAssetPool[i]);
                DB_XAssetPool[i] = NULL;
        }
*/
}


void DB_ShutdownXAssets()
{
        DB_ShutdownXAssetsInternal();
        DB_ClearXAssetMem();
        DB_PureInfoDestroy();
}


/* How many XAssets we have to allocate if it is undefined */


void XAssetsInitStdCount()
{
        XAssetStdCount[ASSET_TYPE_XMODELPIECES] = 64;
        XAssetStdCount[ASSET_TYPE_PHYSPRESET] = 64;
        XAssetStdCount[ASSET_TYPE_XANIMPARTS] = 4096;
        XAssetStdCount[ASSET_TYPE_XMODEL] = 1000;
        XAssetStdCount[ASSET_TYPE_MATERIAL] = 2048;
        XAssetStdCount[ASSET_TYPE_TECHNIQUE_SET] = 1024;
        XAssetStdCount[ASSET_TYPE_IMAGE] = 2400;
        XAssetStdCount[ASSET_TYPE_SOUND] = 16000;
        XAssetStdCount[ASSET_TYPE_SOUND_CURVE] = 64;
        XAssetStdCount[ASSET_TYPE_SOUND_LOADED] = 1200;
        XAssetStdCount[ASSET_TYPE_CLIPMAP] = 1;
        XAssetStdCount[ASSET_TYPE_CLIPMAP_PVS] = 1;
        XAssetStdCount[ASSET_TYPE_COMWORLD] = 1;
        XAssetStdCount[ASSET_TYPE_GAMEWORLD_SP] = 1;
        XAssetStdCount[ASSET_TYPE_GAMEWORLD_MP] = 1;
        XAssetStdCount[ASSET_TYPE_MAP_ENTS] = 2;
        XAssetStdCount[ASSET_TYPE_GFXWORLD] = 1;
        XAssetStdCount[ASSET_TYPE_LIGHT_DEF] = 32;
        XAssetStdCount[ASSET_TYPE_UI_MAP] = 0;
        XAssetStdCount[ASSET_TYPE_FONT] = 16;
        XAssetStdCount[ASSET_TYPE_MENULIST] = 128;
        XAssetStdCount[ASSET_TYPE_MENU] = 1280;
        XAssetStdCount[ASSET_TYPE_LOCALIZE_ENTRY] = 6144;
        XAssetStdCount[ASSET_TYPE_WEAPON] = ASSET_TYPE_WEAPON_COUNT + 24; //24 extra weapons to mute those... Knowing it does not work
        XAssetStdCount[ASSET_TYPE_SNDDRIVER_GLOBALS] = 1;
        XAssetStdCount[ASSET_TYPE_FX] = 400;
        XAssetStdCount[ASSET_TYPE_IMPACT_FX] = 4;
        XAssetStdCount[ASSET_TYPE_AITYPE] = 0;
        XAssetStdCount[ASSET_TYPE_MPTYPE] = 0;
        XAssetStdCount[ASSET_TYPE_CHARACTER] = 0;
        XAssetStdCount[ASSET_TYPE_XMODELALIAS] = 0;
        XAssetStdCount[ASSET_TYPE_RAWFILE] = 1024;
        XAssetStdCount[ASSET_TYPE_STRINGTABLE] = 100;
}

void XAssets_Patch()
{


        WriteSymbol(0x00488E98, g_poolsize);
        WriteSymbol(0x00488EE3, g_poolsize);
        WriteSymbol(0x00488F0F, g_poolsize);

        WriteSymbol(0x00488E67, DB_XAssetPool);
        WriteSymbol(0x00488F05, DB_XAssetPool);
        WriteSymbol(0x00489D55, DB_XAssetPool);
        WriteSymbol(0x0048AEDF, DB_XAssetPool);
        WriteSymbol(0x0048AF22, DB_XAssetPool);
        WriteSymbol(0x0048B00E, DB_XAssetPool);
        WriteSymbol(0x0048B088, DB_XAssetPool);
        WriteSymbol(0x0048B184, DB_XAssetPool);
        WriteSymbol(0x0048B585, DB_XAssetPool);

        XAssetsInitStdCount();
        //memset((void*)0x488f01, 0xcc, 0x34);
}


void DB_ParseRequestedXAssetNum()
{

        char toparse[1024];
        const char* typename;
        char* scanpos;
        char scanstring[64];
        int i, count;

        Com_Memcpy(XAssetRequestedCount, XAssetStdCount, sizeof(XAssetRequestedCount));
        Com_sprintf(toparse, sizeof(toparse), " %s", r_xassetnum->string);

        for(i = 0;  i < ASSET_TYPE_COUNT; ++i)
        {

                if(DB_XAssetNoAlloc(i) || i == ASSET_TYPE_MENU || i == ASSET_TYPE_WEAPON || i == ASSET_TYPE_STRINGTABLE)
                {
                        continue;
                }

                typename = DB_GetXAssetTypeName( i );

                Com_sprintf(scanstring, sizeof(scanstring), " %s=", typename);

                scanpos = strstr(toparse, scanstring);
                if(scanpos == NULL)
                {
                        continue;
                }

                scanpos += strlen(scanstring);

                count = atoi(scanpos);
                if(count < 1 || count > 65535)
                {
                        continue;
                }

                if(count <= DB_GetXAssetStdCount(i))
                {
                        continue;
                }
                XAssetRequestedCount[i] = count;

        }


}

const char* DB_GetRequestedXAssetNum()
{
        const char* cs = CL_GetConfigString(2);
        char versionstr[16];
        int len;

        len = Com_sprintf(versionstr, sizeof(versionstr), "cod%d ", PROTOCOL_VERSION );
        Cvar_SetString(r_xassetnum, cs + len);
        return cs + len;
}

void __cdecl DB_InitZone()
{
  static qboolean firstinit;

  int i;


  if(firstinit)
  {
        if(!Com_IsLegacyServer())
        {
                DB_GetRequestedXAssetNum();
        }
        DB_ParseRequestedXAssetNum();
        DB_RelocateXAssetMem();
    return;
  }

  firstinit = qtrue;

  r_xassetnum = Cvar_RegisterString("r_xassetnum", "", CVAR_INIT, "The list of xassets with their count in the key=value key=value... format");

  DB_InitXAssetMem();

  for(i = 0; i < ASSET_TYPE_COUNT; ++i)
  {
    if( DB_XAssetPool[i] == NULL)
        {
                continue;
    }
        DB_InitPoolHeaderHandler[i](DB_XAssetPool[i], g_poolsize[i]);
  }

  g_freeAssetEntryHead = XAssetHeads;

  for(i = 1; i < 32767; ++i)
  {
    g_assetEntryPool[i].this.nextHeader = &XAssetHeads[i];
  }
  XAssetHeads[32766].this.nextHeader = NULL;

  //In case we have set this from the commandline...
  if(r_xassetnum->string[0])
  {
        DB_ParseRequestedXAssetNum();
        DB_RelocateXAssetMem();
  }

//  Com_Printf("^4Zone initialized\n");
}


Material_t* Material_RegisterHandle(const char* name, int track)
{
        if(com_useFastFiles->boolean)
        {
                return Material_Register_FastFile(name, track);
        }else{
                return Material_Register_LoadObj(name, track);
        }
}


void DB_LoadXZoneForModLevel(const char* levelname)
{
        XZoneInfo zoneinfo[2];
        int numFastFiles = 1;

    DB_AddUserMapDir(levelname);
    zoneinfo[0].name = levelname;
    zoneinfo[0].allocFlags = 8;
    zoneinfo[0].freeFlags = 8;
        /*
        if(DB_ModFileExists())
        {
                zoneinfo[1].name = "mod";
                zoneinfo[1].allocFlags = 16;
                zoneinfo[1].freeFlags = 0;
                numFastFiles++;
        }
    */
        DB_LoadXAssets(zoneinfo, numFastFiles, 0);
}


void __cdecl DB_LoadXZoneFromGfxConfig()
{
  XZoneInfo zoneinfo[7];

  int index = 0;


  zoneinfo[index].name = gfxCfg.codeFastFileName;
  zoneinfo[index].allocFlags = 2;
  zoneinfo[index].freeFlags = 0;
  ++index;
 

  if ( gfxCfg.localizedCodeFastFileName )
  {
    zoneinfo[index].name = gfxCfg.localizedCodeFastFileName;
    zoneinfo[index].allocFlags = 0;
    zoneinfo[index].freeFlags = 0;
    ++index;
  }

  if ( gfxCfg.modFastFileName )
  {
    zoneinfo[index].name = gfxCfg.modFastFileName;
    zoneinfo[index].allocFlags = 16;
    zoneinfo[index].freeFlags = 0;
    ++index;

    DB_LoadXAssets(zoneinfo, index, 0);
    
    sub_5F78A0();
    WaitForSingleObject(databaseCompletedEvent, -1);
    R_SyncRenderThread();
    
    DB_PostLoadXZone();
    index = 0;
  }else{

    zoneinfo[index].name = "cod4x_patchv2";
    zoneinfo[index].allocFlags = 16;
    zoneinfo[index].freeFlags = 0;
    ++index;
  }

  if ( gfxCfg.uiFastFileName )
  {
    zoneinfo[index].name = gfxCfg.uiFastFileName;
    zoneinfo[index].allocFlags = 8;
    zoneinfo[index].freeFlags = 0;
    ++index;
  }

  zoneinfo[index].name = gfxCfg.commonFastFileName;
  zoneinfo[index].allocFlags = 4;
  zoneinfo[index].freeFlags = 0;
  ++index;

  if ( gfxCfg.localizedCommonFastFileName )
  {
    zoneinfo[index].name = gfxCfg.localizedCommonFastFileName;
    zoneinfo[index].allocFlags = 1;
    zoneinfo[index].freeFlags = 0;
    ++index;
  }

  DB_LoadXAssets(zoneinfo, index, 0);
}




XAsset_t* g_assetEntryPool = (XAsset_t*)0xF0D640;






HANDLE DB_OpenCoD4XZoneFile(const char* zone)
{
        wchar_t path[1024];
        char ffname[64];

        Com_sprintf(ffname, sizeof(ffname), "%s.ff", zone);

      	FS_BuildOSPathForThreadUni(FS_GetSavePath(), "zone", ffname, path, 0);

        return CreateFileW(path, 0x80000000, 0, 0, 3u, 0x60000000u, 0);
}

HANDLE DB_OpenCoD4XZoneFileHandler(const char* zone)
{
        HANDLE h = DB_OpenCoD4XZoneFile(zone);
        if(h == (HANDLE)-1)
        {
                Com_PrintWarning(CON_CHANNEL_FILES, "WARNING: Could not find zone 'zone/%s.ff'\n", zone);
        }
        return h;
}


int REGPARM(1) DB_FileSize(const char* zoneName, int source) //DB_FileSize
{
        HANDLE h = (void*)-1;
	char filename[MAX_OSPATH];

        if ( Q_stricmp(zoneName, "mod") != 0 && Q_stricmp(zoneName, "common_mp") != 0 && 
                Q_stricmp(zoneName, "localized_common_mp") != 0 && Q_stricmp(zoneName, "code_post_gfx_mp") != 0 && 
                Q_stricmp(zoneName, "localized_code_post_gfx_mp") != 0
        ){
                h = DB_OpenCoD4XZoneFile(zoneName);
        }
	if(h == (void*)-1)
	{
                DB_BuildOSPath(zoneName, source, sizeof(filename), filename);
                h = CreateFileA(filename, 0x80000000, 1, 0, 3, 0x60000000, 0);
	}
	if(h == (void*)-1)
	{
		return 0;
	}
	int size = GetFileSize(h, 0);
	CloseHandle(h);
	return size;
}

void DB_ModXFileHandle(const char *zone, HANDLE *h, int *type)
{
  char ospath[256];
  char qpath[256];

  if ( Q_stricmp(zone, "mod") == 0 || Q_stricmp(zone, "common_mp") == 0 || 
        Q_stricmp(zone, "localized_common_mp") == 0 || Q_stricmp(zone, "code_post_gfx_mp") == 0 || 
        Q_stricmp(zone, "localized_code_post_gfx_mp") == 0
  ){
        Com_sprintf(qpath, sizeof(qpath), "%s/%s", fs_gameDirVar->string, zone);
        
        if(!CL_IsConnected() || FS_ExistsInReferencedFFs(qpath) || clc.demoplaying)
        {
                DB_BuildOSPath(zone, 1, sizeof(ospath), ospath);
                *h = CreateFileA(ospath, 0x80000000, 1u, 0, 3u, 0x60000000u, 0);
                if ( *h != (HANDLE)-1 )
                {
                        *type = 1;
                }
        }
        return;
  }
  if ( !DB_FileExists(zone, 0) )
  {
      DB_BuildOSPath(zone, 2, sizeof(ospath), ospath);

      *h = CreateFileA(ospath, 0x80000000, 1u, 0, 3u, 0x60000000u, 0);
      if ( *h != (HANDLE)-1 )
      {
        *type = 2;
      }
  }
}



HANDLE __cdecl DB_OpenCommonXZoneFile(const char* zoneName, int* type)
{
    char ospath[256];
    HANDLE h = (HANDLE)-1;

    if(fs_gameDirVar->string[0])
    {
        DB_ModXFileHandle(zoneName, &h, type);
    }
    DB_BuildOSPath(zoneName, 0, sizeof(ospath), ospath);
    if ( h == (HANDLE)-1 && Q_stricmp(zoneName, "mod") != 0)
    {
      *type = 0;
      h = CreateFileA(ospath, 0x80000000, 1u, 0, 3u, 0x60000000u, 0);
    }

    if ( h == (HANDLE)-1 )
    {
        if ( strstr(ospath, "_load") )
        {
          Com_PrintWarning(CON_CHANNEL_FILES, "WARNING: Could not find zone '%s'\n", ospath);
        }
        else
        {
          SetEvent(databaseCompletedEvent);
          Com_Error(ERR_FATAL, "ERROR: Could not find zone '%s'\n", ospath);
        }
        return 0;
    }
    return h;
}


struct DB_LoadData
{
  void *f;
  const char *filename;
  struct XBlock *blocks;
  int outstandingReads;
  struct _OVERLAPPED overlapped;
  //z_stream_s stream;
  int stream[13];
  char *compressBufferStart;
  char *compressBufferEnd;
  void (__cdecl *interrupt_)();
  int allocType;
};
#define g_load (*((struct DB_LoadData*)(0xE344C8)))




struct XZonePure
{
        char name[28];
        uint32_t sum;
};
#define MAX_PUREZONEFILES 16
struct XZonePureInfo
{
        struct XZonePure data[MAX_PUREZONEFILES];
        struct XZonePure* activeset;
        uint32_t checksumfeed;
};

struct XZonePureInfo xzonepureinfo;

void DB_SetPureChecksumFeed(uint32_t newchecksumfeed)
{
        xzonepureinfo.checksumfeed = newchecksumfeed;
}

void (*DB_PureInfoDestroyEx)();
void (*DB_PureInfoBeginEx)();
void (*DB_PureInfoReadChunkEx)(long unsigned int dwNumberOfBytesTransfered);


void DB_PureInfoSetCallbacks(void (*DB_PureInfoDestroy)(), void (*DB_PureInfoBegin)(), void (*DB_PureInfoReadChunk)(long unsigned int))
{
        DB_PureInfoDestroyEx = DB_PureInfoDestroy;
        DB_PureInfoBeginEx = DB_PureInfoBegin;
        DB_PureInfoReadChunkEx = DB_PureInfoReadChunk;
}

void DB_PureInfoDestroy()
{
        memset(&xzonepureinfo.data, 0, sizeof(xzonepureinfo.data));
        xzonepureinfo.activeset = NULL;
        if(DB_PureInfoDestroyEx)
        {
                DB_PureInfoDestroyEx();
        }
}

void DB_PureInfoRemove(const char* name)
{
        struct XZonePureInfo newpureinfo; 
        memset(&newpureinfo.data, 0, sizeof(newpureinfo.data));

        int i, y;
        for(i = 0, y = 0; i < MAX_PUREZONEFILES; ++i)
        {
                if(xzonepureinfo.data[i].name[0] && Q_stricmp(xzonepureinfo.data[i].name, name) != 0)
                {
                       newpureinfo.data[y] = xzonepureinfo.data[i];
                       ++y;
                }
        }
        newpureinfo.checksumfeed = xzonepureinfo.checksumfeed;
        newpureinfo.activeset = NULL;

        xzonepureinfo = newpureinfo;
}

void DB_PureInfoBegin()
{
        if(Com_IsLegacyServer())
        {
                return;
        }
        int i;
        for(i = 0; i < MAX_PUREZONEFILES; ++i)
        {
                if(xzonepureinfo.data[i].name[0] == 0)
                {
                        Q_strncpyz(xzonepureinfo.data[i].name, g_load.filename, sizeof(xzonepureinfo.data[i].name));
                        break;
                }
                if(Q_stricmp(xzonepureinfo.data[i].name, g_load.filename) == 0)
                {
                        break;
                }
        }
        if(i == MAX_PUREZONEFILES)
        {
                Com_Error(ERR_FATAL, "Too many zone files loaded. %s\n", g_load.filename);
                return;
        }
        xzonepureinfo.activeset = &xzonepureinfo.data[i];
        xzonepureinfo.activeset->sum = 0;
        if(DB_PureInfoBeginEx)
        {
                DB_PureInfoBeginEx();
        }
}

void DB_WritePureInfoString(char* info6, int maxsize, int iwdfeed)
{
        if(Com_IsLegacyServer())
        {
                return;
        }
        int i;
        int checksum; 

        checksum = iwdfeed;

      	Q_strcat(info6, maxsize, "# ");

        for(i = 0; i < MAX_PUREZONEFILES; ++i)
        {
                if(xzonepureinfo.data[i].name[0] == 0)
                {
                        break;
                }
              	Q_strcat(info6, maxsize, va("%s %d ", xzonepureinfo.data[i].name, xzonepureinfo.data[i].sum));
                checksum ^= xzonepureinfo.data[i].sum;
        }

        Q_strcat(info6, maxsize, va("%i", i ^ checksum));
}


void __stdcall DB_FileReadCompletionCallback(long unsigned int dwErrorCode, long unsigned int dwNumberOfBytesTransfered, struct _OVERLAPPED *lpOverlapped)
{
    char *fileBuffer;

    if(xzonepureinfo.activeset != NULL && dwNumberOfBytesTransfered > 0)
    {
        fileBuffer = &g_load.compressBufferStart[g_load.overlapped.Offset & 0x7FFFF];
        xzonepureinfo.activeset->sum = Com_BlockChecksumKey32(fileBuffer, dwNumberOfBytesTransfered, xzonepureinfo.activeset->sum);
    }
    if(DB_PureInfoReadChunkEx)
    {
        DB_PureInfoReadChunkEx(dwNumberOfBytesTransfered);
    }
}


qboolean __cdecl DB_ReadData()
{
  char *fileBuffer;
  BOOL result;

  if ( g_load.interrupt_ )
    g_load.interrupt_();

  if(g_load.overlapped.Offset == 0)
  {
        DB_PureInfoBegin();
  }

  fileBuffer = &g_load.compressBufferStart[g_load.overlapped.Offset & 0x7FFFF];
  WaitForSingleObject(resumedDatabaseEvent, 0xFFFFFFFF);
  result = ReadFileEx((HANDLE)g_load.f, fileBuffer, 0x40000u, &g_load.overlapped, DB_FileReadCompletionCallback);
  if ( result )
  {
    g_load.overlapped.Offset += 0x40000;
    ++g_load.outstandingReads;
    return qtrue;
  }
  return qfalse;
}

void DB_FreeXZoneMemory(struct XZoneMemory *zoneMemory);

//void __usercall DB_UnloadXZoneMemory(XZone *zone@<eax>)
void REGPARM(1) DB_UnloadXZoneMemory(struct XZone *zone)
{
  DB_FreeXZoneMemory(&zone->zonememory);
  memset(&zone->zonememory.blocks, 0, sizeof(zone->zonememory.blocks));
  Com_Printf(CON_CHANNEL_SYSTEM, "Unloaded fastfile %s\n", zone->zoneinfo.name);
  PMem_Free(zone->zoneinfo.name, zone->index);
  DB_PureInfoRemove(zone->zoneinfo.name);
  zone->zoneinfo.name[0] = 0;
}

