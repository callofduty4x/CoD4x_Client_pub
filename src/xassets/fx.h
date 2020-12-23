#ifndef __FX_H__
#define __FX_H__

typedef void FxEffectDef;

/* 6942 */
struct FxImpactEntry
{
  FxEffectDef *nonflesh[29];
  FxEffectDef *flesh[4];
};

/* 6952 */
typedef struct 
{
  const char *name;
  struct FxImpactEntry *table;
}FxImpactTable;

#endif