#ifndef _MENULIST_H_20201605_
#define _MENULIST_H_20201605_

typedef struct MenuList
{
  const char *name;
  int menuCount;
  menuDef_t **menus;
} MenuList;

#endif //_MENULIST_H_20201605_
