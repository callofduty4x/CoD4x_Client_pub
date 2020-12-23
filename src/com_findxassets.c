#include "q_shared.h"
#include "qcommon.h"
#include "xassets/sounds.h"
#include "xzone.h"


snd_alias_list_t *__cdecl Com_FindSoundAlias_FastFile(const char *soundname);
snd_alias_list_t *__cdecl Com_PickSoundAliasFromList(const char *soundname);

snd_alias_list_t* Com_FindSoundAlias(const char *name)
{
    if ( com_useFastFiles->boolean )
        {
                return Com_FindSoundAlias_FastFile(name);
        }
        return Com_PickSoundAliasFromList(name);

}

snd_alias_list_t *__cdecl Com_FindSoundAlias_FastFile(const char *name)
{
  XAssetHeader header;

  header = DB_FindXAssetHeader(ASSET_TYPE_SOUND, name);
  
  if ( DB_IsXAssetDefault(ASSET_TYPE_SOUND, name) )
  {
    Com_PrintError(CON_CHANNEL_ERROR, "Missing soundalias \"%s\".\n", name);
    return NULL;
  }
  return header.sound;
}

const char *__cdecl SE_GetString_FastFile(const char *name)
{
  XAssetHeader header;

  header = DB_FindXAssetHeader(ASSET_TYPE_LOCALIZE_ENTRY, name);
  if ( header.localize )
    return header.localize->value;
  else
    return NULL;
}

const char *__cdecl SE_GetString_LoadObj(const char *name);

const char* SE_GetString(const char* findentry)
{
	if ( com_useFastFiles->boolean )
      return SE_GetString_FastFile(findentry);
    else
      return SE_GetString_LoadObj(findentry);
}

XAssetHeader Com_FindXAsset(XAssetType type, const char *name)
{
  XAssetHeader header;
  switch(type)
  {
    case ASSET_TYPE_SOUND:
      header.sound = Com_FindSoundAlias(name);
      break;
    case ASSET_TYPE_MATERIAL:
      header.material = Material_RegisterHandle(name, 0);
      break;
    default:
      Com_Error(ERR_FATAL, "Com_FindXAsset: Invalid xasset type %d", type);
  }

  return header;
}