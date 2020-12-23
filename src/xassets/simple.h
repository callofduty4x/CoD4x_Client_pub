#ifndef __XASSET_SIMPLE_H__
#define __XASSET_SIMPLE_H__


typedef struct
{
  const char *value;
  const char *name;
}LocalizeEntry;

/* 7064 */
typedef struct 
{
  const char *name;
  int len;
  const char *buffer;
}RawFile;

/* 7065 */
typedef struct 
{
  const char *name;
  int columnCount;
  int rowCount;
  const char **values;
}StringTable;

#endif