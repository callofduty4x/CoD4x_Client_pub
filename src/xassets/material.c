#include "material.h"
#include "../r_shared.h"
#include "../qcommon.h"
#include "../xzone.h"

qboolean Material_IsDefault(Material *a1)
{
  Material *v1;

  v1 = rgp.defaultMaterial;

  return a1->textureTable == v1->textureTable && a1->constantTable == v1->constantTable && a1->techniqueSet == v1->techniqueSet;
}

const char * Material_GetName(Material *a1)
{
  return a1->info.name;
}

void Font_InitTTF();


typedef struct
{
  const char* materialname;
  Material** m;
}material_loadstr;

material_loadstr *default_materials = (material_loadstr *)0x6B25A0;

void __cdecl Material_Init()
{
  int i;

  for(i = 0; i < 50; ++i)
  {
    *default_materials[i].m = Material_RegisterHandle(default_materials[i].materialname,0);
    if ( !*default_materials[i].m )
    {
      Com_Error(0, "Could not find material '%s'", default_materials[i].materialname);
    }
  }

  Font_InitTTF();
}


MaterialTechniqueSet *__cdecl Material_GetTechniqueSet(Material *material)
{
//  assert(material);
//  assertx(material->techniqueSet, "material '%s' missing techset. If you are building fastfile, check Launcher for error messages.", material->info.name);
  return material->techniqueSet;
}

bool MaterialValidate()
{
  bool status = false;
  int i;
  for(i = 0; i < rgp.materialCount; ++i)
  {
    MaterialTechniqueSet * techset = Material_GetTechniqueSet(rgp.sortedMaterials[i]);
    if(techset->remappedTechniqueSet == NULL)
    {
      char dbgtxt[1024];
      //sprintf(dbgtxt, "Bad Material at index %d missing remapped technique set \'%s\'\n", i, Material_GetName(rgp.sortedMaterials[i]));
      //MessageBoxA(NULL, dbgtxt, Material_GetName(rgp.sortedMaterials[i]), MB_OK);
      Com_Printf(0, "Bad Material at Ref %p missing remapped technique set \'%s\'\n", rgp.sortedMaterials[i] , Material_GetName(rgp.sortedMaterials[i]));
      status = true;
    }
  }
  
  return status;
}

void Material_SortHook()
{
  //MessageBoxA(NULL, "Joke", "Joke", MB_OK);

  Com_Printf(0, "MatCount: %d\n", rgp.materialCount);
  if(MaterialValidate() == false)
  {
    return;
  }
  Com_Printf(0, "Error MaterialValidate is bad\n");
  //Material_SortInternal();
/*  __asm__("movl $0x621600, %%eax\n"
            "push %%edx\n"
            "call *%%eax\n"
            "add $4, %%esp\n"
            ::"edi" (rgp.materialCount), "edx" (rgp.sortedMaterials)
            :"eax");
*/
}

Material*** varMaterialHandlePtr = (Material***)0xE34598;
void Load_Material();
void Load_MaterialHook()
{
  Load_Material();
/*
  Material* mat = **varMaterialHandlePtr;
  if(Q_stricmp(Material_GetName(mat), "mc/mtl_desertpalmfronds") == 0)
  {
    Com_Printf(0, "^3Found Material %s Ref %p\n", Material_GetName(mat), mat);
    if(mat->techniqueSet == NULL)
    {
      Com_Printf(0, "Material %s without techset\n", Material_GetName(mat));
    }
    else if(mat->techniqueSet->remappedTechniqueSet == NULL)
    {
      Com_Printf(0, "Material %s without techset remapping\n", Material_GetName(mat));
    }
  }
*/  

}


XAssetHeader DB_AddXAsset_Material_Hook(XAssetHeader header)
{
  XAssetHeader rheader = DB_AddXAsset(4, header);
  
  Material* mat = rheader.material;

  if(Q_stricmp(Material_GetName(mat), "mc/mtl_desertpalmfronds") == 0)
  {
    Com_Printf(0, "^3Found Material %s Ref %p\n", Material_GetName(mat), mat);
    if(mat->techniqueSet == NULL)
    {
      Com_Printf(0, "Material %s without techset\n", Material_GetName(mat));
    }
    else if(mat->techniqueSet->remappedTechniqueSet == NULL)
    {
      Com_Printf(0, "Material %s without techset remapping\n", Material_GetName(mat));
    }
    __asm__("int $3");
  }
  
  
  
  return rheader;
}


struct XAssetEntry
{
  struct XAsset asset;
  char zoneIndex;
  bool inuse;
  uint16_t nextHash;
  uint16_t nextOverride;
  uint16_t usageFrame;
};
#define DB_GetXAssetSizeHandler_ADDR 0x726a10
#define DB_DynamicCloneXAssetHandler_ADDR 0x7267A8

static int (__cdecl **DB_GetXAssetSizeHandler)() = (int (__cdecl **)())DB_GetXAssetSizeHandler_ADDR;
void (__cdecl **DB_DynamicCloneXAssetHandler)(XAssetHeader, XAssetHeader, int) = (void (__cdecl **)(XAssetHeader, XAssetHeader, int))DB_DynamicCloneXAssetHandler_ADDR;


static int __cdecl DB_GetXAssetTypeSize(int type)
{
  //assert( DB_GetXAssetSizeHandler[type] );

  return DB_GetXAssetSizeHandler[type]();
}

void __cdecl DB_CloneXAssetInternal(struct XAsset *from, struct XAsset *to)
{
  int size;
  assert ( from->type == to->type );

  size = DB_GetXAssetTypeSize(from->type);
  memcpy(to->header.data, from->header.data, size);
}

char __cdecl DB_DynamicCloneXAsset(XAssetHeader from, XAssetHeader to, XAssetType type, int cloneMethod)
{
  if ( !DB_DynamicCloneXAssetHandler[type] )
  {
    return 0;
  }
  DB_DynamicCloneXAssetHandler[type](from, to, cloneMethod);
  return 1;
}

void __cdecl DB_CloneXAsset(struct XAsset *from, struct XAsset *to, int cloneMethod)
{
  //assert ( from->type == to->type );

  DB_DynamicCloneXAsset(from->header, to->header, to->type, cloneMethod);
  DB_CloneXAssetInternal(from, to);
}

void __cdecl DB_CloneXAssetEntry(struct XAssetEntry *from, struct XAssetEntry *to)
{
  DB_CloneXAsset(&from->asset, &to->asset, to->zoneIndex == 0);
  to->zoneIndex = from->zoneIndex;
}

