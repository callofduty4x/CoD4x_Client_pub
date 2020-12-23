#ifndef _CALLBACKS_H_20200506_
#define _CALLBACKS_H_20200506_

#include "q_shared.h"

void __cdecl BG_CanItemBeGrabbed_Hook();

char __cdecl WeaponEntCanBeGrabbed(int WeaponIndex_, entityState_t *ItemState_, playerState_t *ps_, int a4);

#endif // _CALLBACKS_H_20200506_
