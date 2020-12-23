#ifndef __XZONE_H__
#define __XZONE_H__

#include "xassets/weapondef.h"
#include "xassets/font.h"
#include "xassets/fx.h"
#include "xassets/gfximage.h"
#include "xassets/gfxworld.h"
#include "xassets/material.h"
#include "xassets/sounds.h"
#include "xassets/xanim.h"
#include "xassets/xmodel.h"
#include "xassets/menudef.h"
#include "xassets/menulist.h"
#include "xassets/simple.h"

// playerState has to be patched before changing.
// ::weapons, ::weaponold, ::weaponrechamber, ::ammo, ::ammoclip etc.
// usercmd_t too.
// And so much more code must be decompiled before.

//I know this will not work (just stop people from avoiding the new update, Sorry T-Max)
#define ASSET_TYPE_WEAPON_COUNT 144

typedef struct
{
  const char *name;
  int allocFlags;
  int freeFlags;
}XZoneInfo;


typedef struct XZoneInfoInternal
{
  char name[64];
  int flags;
}XZoneInfoInternal_t;

typedef enum{
        ASSET_TYPE_XMODELPIECES,
        ASSET_TYPE_PHYSPRESET,
        ASSET_TYPE_XANIMPARTS,
        ASSET_TYPE_XMODEL,
        ASSET_TYPE_MATERIAL,
        ASSET_TYPE_TECHNIQUE_SET,		// techset
        ASSET_TYPE_IMAGE,		// image
        ASSET_TYPE_SOUND,	// sound
        ASSET_TYPE_SOUND_CURVE,
        ASSET_TYPE_SOUND_LOADED,
        ASSET_TYPE_CLIPMAP, //Col_Map_sp
        ASSET_TYPE_CLIPMAP_PVS, //Col_Map_mp
        ASSET_TYPE_COMWORLD, //Com_Map
        ASSET_TYPE_GAMEWORLD_SP, //Game_Map_sp
        ASSET_TYPE_GAMEWORLD_MP, //Game_Map_mp
        ASSET_TYPE_MAP_ENTS,
        ASSET_TYPE_GFXWORLD, //GfxMaps
        ASSET_TYPE_LIGHT_DEF,
        ASSET_TYPE_UI_MAP,
        ASSET_TYPE_FONT,
        ASSET_TYPE_MENULIST,		//menufile
        ASSET_TYPE_MENU, //menuDef
        ASSET_TYPE_LOCALIZE_ENTRY,
        ASSET_TYPE_WEAPON,
        ASSET_TYPE_SNDDRIVER_GLOBALS,
        ASSET_TYPE_FX,		//fx
        ASSET_TYPE_IMPACT_FX, //FxImpactTable
        ASSET_TYPE_AITYPE,
        ASSET_TYPE_MPTYPE,
        ASSET_TYPE_CHARACTER,
        ASSET_TYPE_XMODELALIAS,
        ASSET_TYPE_RAWFILE,
        ASSET_TYPE_STRINGTABLE,
        ASSET_TYPE_COUNT
}XAssetType;
/*
enum XAssetType
{
  ASSET_TYPE_XMODELPIECES = 0x0,
  ASSET_TYPE_PHYSPRESET = 0x1,
  ASSET_TYPE_XANIMPARTS = 0x2,
  ASSET_TYPE_XMODEL = 0x3,
  ASSET_TYPE_MATERIAL = 0x4,
  ASSET_TYPE_PIXELSHADER = 0x5,
  ASSET_TYPE_TECHNIQUE_SET = 0x6,
  ASSET_TYPE_IMAGE = 0x7,
  ASSET_TYPE_SOUND = 0x8,
  ASSET_TYPE_SOUND_CURVE = 0x9,
  ASSET_TYPE_CLIPMAP = 0xA,
  ASSET_TYPE_CLIPMAP_PVS = 0xB,
  ASSET_TYPE_COMWORLD = 0xC,
  ASSET_TYPE_GAMEWORLD_SP = 0xD,
  ASSET_TYPE_GAMEWORLD_MP = 0xE,
  ASSET_TYPE_MAP_ENTS = 0xF,
  ASSET_TYPE_GFXWORLD = 0x10,
  ASSET_TYPE_LIGHT_DEF = 0x11,
  ASSET_TYPE_UI_MAP = 0x12,
  ASSET_TYPE_FONT = 0x13,
  ASSET_TYPE_MENULIST = 0x14,
  ASSET_TYPE_MENU = 0x15,
  ASSET_TYPE_LOCALIZE_ENTRY = 0x16,
  ASSET_TYPE_WEAPON = 0x17,
  ASSET_TYPE_SNDDRIVER_GLOBALS = 0x18,
  ASSET_TYPE_FX = 0x19,
  ASSET_TYPE_IMPACT_FX = 0x1A,
  ASSET_TYPE_AITYPE = 0x1B,
  ASSET_TYPE_MPTYPE = 0x1C,
  ASSET_TYPE_CHARACTER = 0x1D,
  ASSET_TYPE_XMODELALIAS = 0x1E,
  ASSET_TYPE_RAWFILE = 0x1F,
  ASSET_TYPE_STRINGTABLE = 0x20,
  ASSET_TYPE_COUNT = 0x21,
  ASSET_TYPE_STRING = 0x21,
  ASSET_TYPE_ASSETLIST = 0x22,
};
*/


/* 7066 */
typedef union
{
/*  XModelPieces *xmodelPieces;
  PhysPreset *physPreset;*/
  XAnimParts *parts;
  XModel *model;
  Material *material;
/*  MaterialPixelShader *pixelShader;
  MaterialVertexShader *vertexShader;*/
  struct MaterialTechniqueSet *techniqueSet;
/*  GfxImage *image;*/
  snd_alias_list_t *sound;
/*  SndCurve *sndCurve;
  clipMap_t *clipMap;
  ComWorld *comWorld;
  GameWorldSp *gameWorldSp;
  GameWorldMp *gameWorldMp;
  MapEnts *mapEnts;*/
  GfxWorld *gfxWorld;
  GfxLightDef *lightDef;
  Font_t *font;
  MenuList *menuList;
  menuDef_t *menu;
  LocalizeEntry *localize;
  WeaponDef *weapon;
/*  SndDriverGlobals *sndDriverGlobals;
  FxEffectDef *fx;*/
  FxImpactTable *impactFx;
  RawFile *rawfile;
  StringTable *stringTable;
  void *data;
}XAssetHeader;

typedef union
{
  XAssetType type;
  void* nextHeader;
}XAssetHeaderType_t;


typedef struct
{
  XAssetHeaderType_t this;
  XAssetHeader header;
  unsigned short field_8;
  unsigned short nextListIndex;
  unsigned short field_C;
  unsigned short field_E;
}XAsset_t;

extern XAsset_t* g_assetEntryPool;

void __cdecl DB_LoadXAssets(XZoneInfo *zoneinfo, unsigned int zoneCount, int a3);
void DB_InitXAssets();
void DB_ShutdownXAssets();
void DB_LoadXZoneFromGfxConfig();
byte __cdecl DB_IsXAssetDefault(XAssetType assettype, const char *name);
XAssetHeader __cdecl DB_FindXAssetHeader(XAssetType a2, const char *name);
XAssetHeader Com_FindXAsset(XAssetType type, const char *name);


#define db_hashTable ((uint16_t*)0xe62a80)


/* 7706 */
struct XAsset
{
  XAssetType type;
  XAssetHeader header;
};

/* 7705 */
struct ScriptStringList
{
  int count;
  uint32_t *strings;
};

/* 7707 */
struct XAssetList
{
  struct ScriptStringList stringList;
  int assetCount;
  struct XAsset *assets;
};

struct XBlock
{
  char *data;
  unsigned int size;
};



struct XZoneMemory
{
  struct XBlock blocks[9];
  int allocVertexBuffer;
  int allocIndexBuffer;
  int vertexBufferHandle;
  int indexBufferHandle;
};


#pragma pack(push, 1)
struct XZone
{
  struct XZoneInfoInternal zoneinfo;
  int index;
  struct XZoneMemory zonememory;
  int zoneSize;
  int ff_dir;
};
#pragma pack(pop)


void XAssetLoader_Init();
#endif
