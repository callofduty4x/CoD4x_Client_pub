#ifndef _CG_WEAPONS_H_20200506_
#define _CG_WEAPONS_H_20200506_

#include "xzone.h"

// 16 items per weapon asset (view/worldmodels in weapon file).
#define MAX_ITEMS (ASSET_TYPE_WEAPON_COUNT*16)

extern int bg_itemlist[MAX_ITEMS + 16];
extern unsigned int itemRegistered[MAX_ITEMS + 16];

#endif // _CG_WEAPONS_H_20200506_
