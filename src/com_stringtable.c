#include "q_shared.h"
#include "qcommon.h"
#include "xassets/simple.h"
#include "xzone.h"


int __cdecl StringTable_LookupRowNumForValue(StringTable *table, const int comparisonColumn, const char *value);

const char *StringTable_Lookup(StringTable *table, const int comparisonColumn, const char *value, const int valueColumn)
{
  int row;
  int valindex;

  if ( !table )
  {
    Com_PrintError(CON_CHANNEL_SYSTEM, "Unable to find the lookup table in the fastfile, aborting lookup\n");
    return "";
  }
  row = StringTable_LookupRowNumForValue(table, comparisonColumn, value);
  if ( valueColumn >= table->columnCount )
    return "";
  if ( row >= table->rowCount )
    return "";
  if ( row < 0 )
    return "";
  if ( valueColumn < 0 )
    return "";
  valindex = valueColumn + row * table->columnCount;
  if ( !table->values[valindex] )
    return "";
  return table->values[valindex];
}


void __cdecl StringTable_GetAsset_FastFile(const char *filename, StringTable **tablePtr)
{
  *tablePtr = DB_FindXAssetHeader(ASSET_TYPE_STRINGTABLE, filename).stringTable;
}

void __cdecl StringTable_GetAsset(const char *filename, StringTable **tablePtr)
{
  void (__cdecl *f)(const char *, StringTable **);

  if ( com_useFastFiles->boolean )
  {
    f = StringTable_GetAsset_FastFile;
  }
  else
  {
    Com_Error(ERR_FATAL, "Trying to use a string table with fast file loading disabled.");
    return;
  }
  f(filename, tablePtr);
}
