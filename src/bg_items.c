#include "bg_items.h"

#include "callbacks.h"
#include "cg_weapons.h"

#include "xassets/weapondef.h"


qboolean BG_CanItemBeGrabbed(playerState_t *ps_, entityState_t *ItemState_, int a3)
{
    if (!ps_ || !ItemState_)
        return qfalse;

    if ((ps_->weapFlags & 0xFF) < 0)
        return qfalse;

    clipHandle_t itemClipHandle = ItemState_->index;
    if (itemClipHandle < 1 || itemClipHandle >= MAX_ITEMS)
    {
        Com_Error(ERR_DROP, "BG_CanItemBeGrabbed: index out of range (index is %i, eType is %i)", itemClipHandle, ItemState_->eType);
    }

    if (ItemState_->clientNum == ps_->clientNum || ps_->pm_flags & 0x100000)
        return qfalse;

    int weaponIndex = itemClipHandle % 128;
    WeaponDef *weaponItem = bg_weaponDefs[weaponIndex];
    if (!WeaponEntCanBeGrabbed(weaponIndex, ItemState_, ps_, a3))
    {
        int altWeaponIndex = weaponItem->altWeaponIndex;
        if (!altWeaponIndex || !WeaponEntCanBeGrabbed(altWeaponIndex, ItemState_, ps_, a3))
            return qfalse;
    }
    return qtrue;
}
