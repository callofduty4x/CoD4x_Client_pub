#include "q_shared.h"
#include "qcommon.h"
#include "xassets/simple.h"
#include "xassets/material.h"

const char *StringTable_Lookup(StringTable *table, const int comparisonColumn, const char *value, const int valueColumn);
void __cdecl StringTable_GetAsset(const char *filename, StringTable **tablePtr);

cvar_t* cl_ranktablename;

void __cdecl CL_InitRanktableVars()
{
    cl_ranktablename = Cvar_RegisterString("g_ranktablename", "", CVAR_CHEAT, "Changes name of ranktable when set by server");
}

void __cdecl CL_GetRankIcon(int rank, int prestige, Material **handle)
{
  StringTable *table;
  const char *rankIconName;
  char id[5];

  if ( com_useFastFiles->boolean )
  {
    assert(rank >= 0);
    assert(prestige >= 0);
    assert(handle);

    char ranktablepath[128];

    if(cl_ranktablename == NULL)
    {
        CL_InitRanktableVars();
    }

    Com_sprintf(ranktablepath, sizeof(ranktablepath), "mp/rankIconTable%s.csv", cl_ranktablename->string);

    StringTable_GetAsset(ranktablepath, &table);
    assert(table);

    Com_sprintf(id, sizeof(id), "%i", rank);
    rankIconName = StringTable_Lookup(table, 0, id, prestige + 1);
    *handle = Material_RegisterHandle(rankIconName, 7);
    if ( Material_IsDefault(*handle) )
    {
      *handle = 0;
    }
  }else{
  }
}


const char *__cdecl CL_GetRankData(int rank)
{
  StringTable *table;
  char llevel[5];

  char ranktablepath[128];

  if(cl_ranktablename == NULL)
  {
    CL_InitRanktableVars();
  }

  Com_sprintf(ranktablepath, sizeof(ranktablepath), "mp/rankTable%s.csv", cl_ranktablename->string);

  StringTable_GetAsset(ranktablepath, &table);
  assert(table);
  Com_sprintf(llevel, sizeof(llevel), "%i", rank);
  return StringTable_Lookup(table, 0, llevel, 14);
}
