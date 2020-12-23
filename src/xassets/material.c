#include "material.h"
#include "../r_shared.h"
#include "../qcommon.h"

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
