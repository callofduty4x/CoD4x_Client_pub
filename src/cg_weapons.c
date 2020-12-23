#include "cg_weapons.h"
#include "xassets/weapondef.h"
#include "xzone.h"
#include "sys_patch.h"
#include "ui_shared.h"

//sub_4BB8E0 128

WeaponDef *bg_weaponDefs[ASSET_TYPE_WEAPON_COUNT];
WeaponDef *bg_sharedAmmoCaps[ASSET_TYPE_WEAPON_COUNT];
WeaponDef *bg_weapAmmoTypes[ASSET_TYPE_WEAPON_COUNT];
WeaponDef *bg_weapClips[ASSET_TYPE_WEAPON_COUNT];

int bg_itemlist[MAX_ITEMS + 16];
unsigned int itemRegistered[MAX_ITEMS + 16];
weaponInfo_t cg_weaponsArray[ASSET_TYPE_WEAPON_COUNT];

Material* weaponHudIcons[ASSET_TYPE_WEAPON_COUNT +4];

#define bg_lastParsedWeaponIndex (*(int*)0xD5EC418)

void CG_RegisterItems();
void CG_DrawCursorHint();
void CG_UpdateWeaponViewmodels(int localClientNum);


void CG_ClearWeaponArray()
{

	Com_Memset(cg_weaponsArray, 0, sizeof(cg_weaponsArray));

}

void Patch_CGWeaponAssets()
{

	WriteSymbol(0x402d8c, bg_weaponDefs);
	WriteSymbol(0x405801, bg_weaponDefs);
	WriteSymbol(0x4067fa, bg_weaponDefs);
	WriteSymbol(0x40bb33, bg_weaponDefs);
	WriteSymbol(0x40bb73, bg_weaponDefs);
	WriteSymbol(0x40bbb6, bg_weaponDefs);
	WriteSymbol(0x40bca8, bg_weaponDefs);
	WriteSymbol(0x40bd4c, bg_weaponDefs);
	WriteSymbol(0x40dc5a, bg_weaponDefs);
	WriteSymbol(0x40f1e3, bg_weaponDefs);
	WriteSymbol(0x410ebc, bg_weaponDefs);
	WriteSymbol(0x410f56, bg_weaponDefs);
	WriteSymbol(0x411f76, bg_weaponDefs);
	WriteSymbol(0x414404, bg_weaponDefs);
	WriteSymbol(0x414475, bg_weaponDefs);
	WriteSymbol(0x4145bd, bg_weaponDefs);
	WriteSymbol(0x414644, bg_weaponDefs);
	WriteSymbol(0x416023, bg_weaponDefs);
	WriteSymbol(0x4161a4, bg_weaponDefs);
	WriteSymbol(0x416229, bg_weaponDefs);
	WriteSymbol(0x4162e7, bg_weaponDefs);
	WriteSymbol(0x416364, bg_weaponDefs);
	WriteSymbol(0x4163e4, bg_weaponDefs);
	WriteSymbol(0x416460, bg_weaponDefs);
	WriteSymbol(0x416503, bg_weaponDefs);
	WriteSymbol(0x4165f3, bg_weaponDefs);
	WriteSymbol(0x416633, bg_weaponDefs);
	WriteSymbol(0x416743, bg_weaponDefs);
	WriteSymbol(0x4167c3, bg_weaponDefs);
	WriteSymbol(0x416851, bg_weaponDefs);
	WriteSymbol(0x41687e, bg_weaponDefs);
	WriteSymbol(0x416898, bg_weaponDefs);
	WriteSymbol(0x4168ac, bg_weaponDefs);
	WriteSymbol(0x4168f3, bg_weaponDefs);
	WriteSymbol(0x416907, bg_weaponDefs);
	WriteSymbol(0x416924, bg_weaponDefs);
	WriteSymbol(0x416959, bg_weaponDefs);
	WriteSymbol(0x4169b8, bg_weaponDefs);
	WriteSymbol(0x416a38, bg_weaponDefs);
	WriteSymbol(0x416acd, bg_weaponDefs);
	WriteSymbol(0x416b53, bg_weaponDefs);
	WriteSymbol(0x416bd0, bg_weaponDefs);
	WriteSymbol(0x416db6, bg_weaponDefs);
	WriteSymbol(0x416e23, bg_weaponDefs);
	WriteSymbol(0x416e82, bg_weaponDefs);
	WriteSymbol(0x41700b, bg_weaponDefs);
	WriteSymbol(0x4171dc, bg_weaponDefs);
	WriteSymbol(0x41728d, bg_weaponDefs);
	WriteSymbol(0x417439, bg_weaponDefs);
	WriteSymbol(0x4174e3, bg_weaponDefs);
	WriteSymbol(0x417525, bg_weaponDefs);
	WriteSymbol(0x417542, bg_weaponDefs);
	WriteSymbol(0x41760e, bg_weaponDefs);
	WriteSymbol(0x417639, bg_weaponDefs);
	WriteSymbol(0x4176a3, bg_weaponDefs);
	WriteSymbol(0x417803, bg_weaponDefs);
	WriteSymbol(0x4178bc, bg_weaponDefs);
	WriteSymbol(0x4179a4, bg_weaponDefs);
	WriteSymbol(0x417b75, bg_weaponDefs);
	WriteSymbol(0x417c17, bg_weaponDefs);
	WriteSymbol(0x417e07, bg_weaponDefs);
	WriteSymbol(0x417e59, bg_weaponDefs);
	WriteSymbol(0x417f73, bg_weaponDefs);
	WriteSymbol(0x417ffe, bg_weaponDefs);
	WriteSymbol(0x418057, bg_weaponDefs);
	WriteSymbol(0x418219, bg_weaponDefs);
	WriteSymbol(0x41827b, bg_weaponDefs);
	WriteSymbol(0x418339, bg_weaponDefs);
	WriteSymbol(0x4183c7, bg_weaponDefs);
	WriteSymbol(0x418409, bg_weaponDefs);
	WriteSymbol(0x418517, bg_weaponDefs);
	WriteSymbol(0x41868a, bg_weaponDefs);
	WriteSymbol(0x418780, bg_weaponDefs);
	WriteSymbol(0x4187ed, bg_weaponDefs);
	WriteSymbol(0x41888c, bg_weaponDefs);
	WriteSymbol(0x418c10, bg_weaponDefs);
	WriteSymbol(0x418c4b, bg_weaponDefs);
	WriteSymbol(0x418c9a, bg_weaponDefs);
	WriteSymbol(0x418dca, bg_weaponDefs);
	WriteSymbol(0x418ea8, bg_weaponDefs);
	WriteSymbol(0x4190dc, bg_weaponDefs);
	WriteSymbol(0x41919b, bg_weaponDefs);
	WriteSymbol(0x41938a, bg_weaponDefs);
	WriteSymbol(0x41949c, bg_weaponDefs);
	WriteSymbol(0x4194bf, bg_weaponDefs);
	WriteSymbol(0x419519, bg_weaponDefs);
	WriteSymbol(0x41956d, bg_weaponDefs);
	WriteSymbol(0x4195a0, bg_weaponDefs);
	WriteSymbol(0x4195ec, bg_weaponDefs);
	WriteSymbol(0x419679, bg_weaponDefs);
	WriteSymbol(0x419770, bg_weaponDefs);
	WriteSymbol(0x4198e6, bg_weaponDefs);
	WriteSymbol(0x419a28, bg_weaponDefs);
	WriteSymbol(0x419a69, bg_weaponDefs);
	WriteSymbol(0x419b48, bg_weaponDefs);
	WriteSymbol(0x419b6c, bg_weaponDefs);
	WriteSymbol(0x419c8a, bg_weaponDefs);
	WriteSymbol(0x419d03, bg_weaponDefs);
	WriteSymbol(0x419d5f, bg_weaponDefs);
	WriteSymbol(0x419e5f, bg_weaponDefs);
	WriteSymbol(0x419f70, bg_weaponDefs);
	WriteSymbol(0x41a012, bg_weaponDefs);
	WriteSymbol(0x41a0c3, bg_weaponDefs);
	WriteSymbol(0x41a196, bg_weaponDefs);
	WriteSymbol(0x41a1be, bg_weaponDefs);
	WriteSymbol(0x41a32d, bg_weaponDefs);
	WriteSymbol(0x41a3ad, bg_weaponDefs);
	WriteSymbol(0x41a7da, bg_weaponDefs);
	WriteSymbol(0x41abb8, bg_weaponDefs);
	WriteSymbol(0x41aecd, bg_weaponDefs);
	WriteSymbol(0x41b0f3, bg_weaponDefs);
	WriteSymbol(0x41b319, bg_weaponDefs);
	WriteSymbol(0x41b5c5, bg_weaponDefs);
	WriteSymbol(0x41b7d1, bg_weaponDefs);
	WriteSymbol(0x41b901, bg_weaponDefs);
	WriteSymbol(0x41ba42, bg_weaponDefs);
	WriteSymbol(0x41bc33, bg_weaponDefs);
	WriteSymbol(0x41be1c, bg_weaponDefs);
	WriteSymbol(0x41bfa1, bg_weaponDefs);
	WriteSymbol(0x41c273, bg_weaponDefs);
	WriteSymbol(0x41c2b3, bg_weaponDefs);
	WriteSymbol(0x41c2d4, bg_weaponDefs);
	WriteSymbol(0x422439, bg_weaponDefs);
	WriteSymbol(0x422596, bg_weaponDefs);
	WriteSymbol(0x42265d, bg_weaponDefs);
	WriteSymbol(0x42272d, bg_weaponDefs);
	WriteSymbol(0x42280d, bg_weaponDefs);
	WriteSymbol(0x422908, bg_weaponDefs);
	WriteSymbol(0x422a78, bg_weaponDefs);
	WriteSymbol(0x422baa, bg_weaponDefs);
	WriteSymbol(0x422bf2, bg_weaponDefs);
	WriteSymbol(0x422d8a, bg_weaponDefs);
	WriteSymbol(0x422dff, bg_weaponDefs);
	WriteSymbol(0x423002, bg_weaponDefs);
	WriteSymbol(0x423280, bg_weaponDefs);
	WriteSymbol(0x42328e, bg_weaponDefs);
	WriteSymbol(0x4232ad, bg_weaponDefs);
	WriteSymbol(0x4232b4, bg_weaponDefs);
	WriteSymbol(0x423430, bg_weaponDefs);
	WriteSymbol(0x42360a, bg_weaponDefs);
	WriteSymbol(0x423773, bg_weaponDefs);
	WriteSymbol(0x42385e, bg_weaponDefs);
	WriteSymbol(0x42cd59, bg_weaponDefs);
	WriteSymbol(0x42dd5c, bg_weaponDefs);
	WriteSymbol(0x430141, bg_weaponDefs);
	WriteSymbol(0x4308fc, bg_weaponDefs);
	WriteSymbol(0x43096b, bg_weaponDefs);
	WriteSymbol(0x430a9b, bg_weaponDefs);
	WriteSymbol(0x43122a, bg_weaponDefs);
	WriteSymbol(0x4312bf, bg_weaponDefs);
	WriteSymbol(0x4328d7, bg_weaponDefs);
	WriteSymbol(0x432a4a, bg_weaponDefs);
	WriteSymbol(0x432edd, bg_weaponDefs);
	WriteSymbol(0x435667, bg_weaponDefs);
	WriteSymbol(0x435943, bg_weaponDefs);
	WriteSymbol(0x435dd9, bg_weaponDefs);
	WriteSymbol(0x436201, bg_weaponDefs);
	WriteSymbol(0x4367d9, bg_weaponDefs);
	WriteSymbol(0x43f8c7, bg_weaponDefs);
	WriteSymbol(0x4410aa, bg_weaponDefs);
	WriteSymbol(0x441348, bg_weaponDefs);
	WriteSymbol(0x44181d, bg_weaponDefs);
	WriteSymbol(0x441982, bg_weaponDefs);
	WriteSymbol(0x44223d, bg_weaponDefs);
	WriteSymbol(0x443487, bg_weaponDefs);
	WriteSymbol(0x443944, bg_weaponDefs);
	WriteSymbol(0x444da8, bg_weaponDefs);
	WriteSymbol(0x445bb2, bg_weaponDefs);
	WriteSymbol(0x4479e7, bg_weaponDefs);
	WriteSymbol(0x44c346, bg_weaponDefs);
	WriteSymbol(0x44fe4d, bg_weaponDefs);
	WriteSymbol(0x4503c2, bg_weaponDefs);
	WriteSymbol(0x450847, bg_weaponDefs);
	WriteSymbol(0x45085f, bg_weaponDefs);
	WriteSymbol(0x453675, bg_weaponDefs);
	WriteSymbol(0x4536d9, bg_weaponDefs);
	WriteSymbol(0x453a54, bg_weaponDefs);
	WriteSymbol(0x453ca1, bg_weaponDefs);
	WriteSymbol(0x453dac, bg_weaponDefs);
	WriteSymbol(0x45417b, bg_weaponDefs);
	WriteSymbol(0x45433c, bg_weaponDefs);
	WriteSymbol(0x454765, bg_weaponDefs);
	WriteSymbol(0x4548c1, bg_weaponDefs);
	WriteSymbol(0x454914, bg_weaponDefs);
	WriteSymbol(0x454b04, bg_weaponDefs);
	WriteSymbol(0x454c45, bg_weaponDefs);
	WriteSymbol(0x454d0f, bg_weaponDefs);
	WriteSymbol(0x4552ab, bg_weaponDefs);
	WriteSymbol(0x455597, bg_weaponDefs);
	WriteSymbol(0x4555f9, bg_weaponDefs);
	WriteSymbol(0x455aaa, bg_weaponDefs);
	WriteSymbol(0x456e44, bg_weaponDefs);
	WriteSymbol(0x456f0a, bg_weaponDefs);
	WriteSymbol(0x456f24, bg_weaponDefs);
	WriteSymbol(0x457292, bg_weaponDefs);
	WriteSymbol(0x4572b1, bg_weaponDefs);
	WriteSymbol(0x4575b9, bg_weaponDefs);
	WriteSymbol(0x457662, bg_weaponDefs);
	WriteSymbol(0x457703, bg_weaponDefs);
	WriteSymbol(0x4577c5, bg_weaponDefs);
	WriteSymbol(0x4577e8, bg_weaponDefs);
	WriteSymbol(0x457851, bg_weaponDefs);
	WriteSymbol(0x45789d, bg_weaponDefs);
	WriteSymbol(0x457b8f, bg_weaponDefs);
	WriteSymbol(0x457c0d, bg_weaponDefs);
	WriteSymbol(0x457cb3, bg_weaponDefs);
	WriteSymbol(0x457d86, bg_weaponDefs);
	WriteSymbol(0x457efa, bg_weaponDefs);
	WriteSymbol(0x458913, bg_weaponDefs);
	WriteSymbol(0x458f36, bg_weaponDefs);
	WriteSymbol(0x458f52, bg_weaponDefs);
	WriteSymbol(0x458f6c, bg_weaponDefs);
	WriteSymbol(0x459070, bg_weaponDefs);
	WriteSymbol(0x45a425, bg_weaponDefs);
	WriteSymbol(0x45a448, bg_weaponDefs);
	WriteSymbol(0x45a57d, bg_weaponDefs);
	WriteSymbol(0x45a66d, bg_weaponDefs);
	WriteSymbol(0x45a99c, bg_weaponDefs);
	WriteSymbol(0x45a9dc, bg_weaponDefs);
	WriteSymbol(0x48dd13, bg_weaponDefs);
	WriteSymbol(0x48e40a, bg_weaponDefs);
	WriteSymbol(0x4a7f74, bg_weaponDefs);
	WriteSymbol(0x4a7fe4, bg_weaponDefs);
	WriteSymbol(0x4a8102, bg_weaponDefs);
	WriteSymbol(0x4a8874, bg_weaponDefs);
	WriteSymbol(0x4a89d4, bg_weaponDefs);
	WriteSymbol(0x4ab853, bg_weaponDefs);
	WriteSymbol(0x4ab8d4, bg_weaponDefs);
	WriteSymbol(0x4ab97b, bg_weaponDefs);
	WriteSymbol(0x4abab2, bg_weaponDefs);
	WriteSymbol(0x4abda9, bg_weaponDefs);
	WriteSymbol(0x4abe69, bg_weaponDefs);
	WriteSymbol(0x4ac6d9, bg_weaponDefs);
	WriteSymbol(0x4ac7ee, bg_weaponDefs);
	WriteSymbol(0x4ac97e, bg_weaponDefs);
	WriteSymbol(0x4adf04, bg_weaponDefs);
	WriteSymbol(0x4adf9d, bg_weaponDefs);
	WriteSymbol(0x4aeac1, bg_weaponDefs);
	WriteSymbol(0x4aebe5, bg_weaponDefs);
	WriteSymbol(0x4aed61, bg_weaponDefs);
	WriteSymbol(0x4aeec1, bg_weaponDefs);
	WriteSymbol(0x4aefda, bg_weaponDefs);
	WriteSymbol(0x4af00c, bg_weaponDefs);
	WriteSymbol(0x4b1638, bg_weaponDefs);
	WriteSymbol(0x4b1723, bg_weaponDefs);
	WriteSymbol(0x4b25e3, bg_weaponDefs);
	WriteSymbol(0x4b279c, bg_weaponDefs);
	WriteSymbol(0x4b29a1, bg_weaponDefs);
	WriteSymbol(0x4b29bc, bg_weaponDefs);
	WriteSymbol(0x4b29ee, bg_weaponDefs);
	WriteSymbol(0x4b2a0c, bg_weaponDefs);
	WriteSymbol(0x4b2aa3, bg_weaponDefs);
	WriteSymbol(0x4b2ac1, bg_weaponDefs);
	WriteSymbol(0x4b2af7, bg_weaponDefs);
	WriteSymbol(0x4b2b18, bg_weaponDefs);
	WriteSymbol(0x4b2b4a, bg_weaponDefs);
	WriteSymbol(0x4b2b68, bg_weaponDefs);
	WriteSymbol(0x4b4fa5, bg_weaponDefs);
	WriteSymbol(0x4b52b7, bg_weaponDefs);
	WriteSymbol(0x4bafc5, bg_weaponDefs);
	WriteSymbol(0x4bb05f, bg_weaponDefs);
	WriteSymbol(0x4bb0b0, bg_weaponDefs);
	WriteSymbol(0x4bb132, bg_weaponDefs);
	WriteSymbol(0x4bb151, bg_weaponDefs);
	WriteSymbol(0x4bb1d0, bg_weaponDefs);
	WriteSymbol(0x4bb1e6, bg_weaponDefs);
	WriteSymbol(0x4bb237, bg_weaponDefs);
	WriteSymbol(0x4bb4c7, bg_weaponDefs);
	WriteSymbol(0x4bb58d, bg_weaponDefs);
	WriteSymbol(0x4bb69f, bg_weaponDefs);
	WriteSymbol(0x4bb7ad, bg_weaponDefs);
	WriteSymbol(0x4bb9cc, bg_weaponDefs);
	WriteSymbol(0x4bba89, bg_weaponDefs);
	WriteSymbol(0x4bbbf3, bg_weaponDefs);
	WriteSymbol(0x4bbd06, bg_weaponDefs);
	WriteSymbol(0x4bbec7, bg_weaponDefs);
	WriteSymbol(0x4bbfe5, bg_weaponDefs);
	WriteSymbol(0x4bc2db, bg_weaponDefs);
	WriteSymbol(0x4bc348, bg_weaponDefs);
	WriteSymbol(0x4bc3a1, bg_weaponDefs);
	WriteSymbol(0x4bc402, bg_weaponDefs);
	WriteSymbol(0x4bc48f, bg_weaponDefs);
	WriteSymbol(0x4bc4e0, bg_weaponDefs);
	WriteSymbol(0x4bc56f, bg_weaponDefs);
	WriteSymbol(0x4bcb8e, bg_weaponDefs);
	WriteSymbol(0x4bcc98, bg_weaponDefs);
	WriteSymbol(0x4bcdb5, bg_weaponDefs);
	WriteSymbol(0x4bfd1f, bg_weaponDefs);
	WriteSymbol(0x4bfea1, bg_weaponDefs);
	WriteSymbol(0x4c0fe9, bg_weaponDefs);
	WriteSymbol(0x4c111e, bg_weaponDefs);
	WriteSymbol(0x4c19ac, bg_weaponDefs);
	WriteSymbol(0x4c1a13, bg_weaponDefs);
	WriteSymbol(0x4c2927, bg_weaponDefs);
	WriteSymbol(0x4c39e4, bg_weaponDefs);
	WriteSymbol(0x4c3b03, bg_weaponDefs);
	WriteSymbol(0x4c3c31, bg_weaponDefs);
	WriteSymbol(0x4c409f, bg_weaponDefs);
	WriteSymbol(0x4c4110, bg_weaponDefs);
	WriteSymbol(0x4c4929, bg_weaponDefs);
	WriteSymbol(0x4c4ec1, bg_weaponDefs);
	WriteSymbol(0x4c56e5, bg_weaponDefs);
	WriteSymbol(0x4c5a83, bg_weaponDefs);
	WriteSymbol(0x4c5daa, bg_weaponDefs);
	WriteSymbol(0x4c5ed5, bg_weaponDefs);
	WriteSymbol(0x4c5f55, bg_weaponDefs);
	WriteSymbol(0x4c60da, bg_weaponDefs);
	WriteSymbol(0x4c69d2, bg_weaponDefs);
	WriteSymbol(0x4c6b46, bg_weaponDefs);
	WriteSymbol(0x4c6e73, bg_weaponDefs);
	WriteSymbol(0x4c768f, bg_weaponDefs);
	WriteSymbol(0x4c7aa9, bg_weaponDefs);
	WriteSymbol(0x4c7c44, bg_weaponDefs);
	WriteSymbol(0x4cab25, bg_weaponDefs);
	WriteSymbol(0x4cab75, bg_weaponDefs);
	WriteSymbol(0x4cc545, bg_weaponDefs);
	WriteSymbol(0x4cc558, bg_weaponDefs);
	WriteSymbol(0x4cc604, bg_weaponDefs);
	WriteSymbol(0x4d028e, bg_weaponDefs);
	WriteSymbol(0x4d032e, bg_weaponDefs);
	WriteSymbol(0x4d03ce, bg_weaponDefs);
	WriteSymbol(0x4d046e, bg_weaponDefs);
	WriteSymbol(0x4d04e2, bg_weaponDefs);
	WriteSymbol(0x4d054e, bg_weaponDefs);
	WriteSymbol(0x4d05be, bg_weaponDefs);
	WriteSymbol(0x4d065e, bg_weaponDefs);
	WriteSymbol(0x4d06fe, bg_weaponDefs);
	WriteSymbol(0x4d071e, bg_weaponDefs);
	WriteSymbol(0x4d07b4, bg_weaponDefs);
	WriteSymbol(0x4d0894, bg_weaponDefs);
	WriteSymbol(0x4d0964, bg_weaponDefs);
	WriteSymbol(0x4d3837, bg_weaponDefs);
	WriteSymbol(0x4d843b, bg_weaponDefs);
	WriteSymbol(0x4d8c62, bg_weaponDefs);
	WriteSymbol(0x4d8e30, bg_weaponDefs);
	WriteSymbol(0x4d8fc0, bg_weaponDefs);
	WriteSymbol(0x4dc0fc, bg_weaponDefs);
	WriteSymbol(0x4df3da, bg_weaponDefs);
	WriteSymbol(0x4e65c0, bg_weaponDefs);
	WriteSymbol(0x4e6e22, bg_weaponDefs);
	WriteSymbol(0x4e8a95, bg_weaponDefs);
	WriteSymbol(0x4e90e9, bg_weaponDefs);
	WriteSymbol(0x4ea047, bg_weaponDefs);
	WriteSymbol(0x4ea1b2, bg_weaponDefs);
	WriteSymbol(0x4ea21e, bg_weaponDefs);
	WriteSymbol(0x4ea2be, bg_weaponDefs);
	WriteSymbol(0x4ea350, bg_weaponDefs);
	WriteSymbol(0x4ea3d2, bg_weaponDefs);
	WriteSymbol(0x4eb391, bg_weaponDefs);
	WriteSymbol(0x4eb398, bg_weaponDefs);
	WriteSymbol(0x4eb411, bg_weaponDefs);
	WriteSymbol(0x4eb442, bg_weaponDefs);
	WriteSymbol(0x4eb65a, bg_weaponDefs);
	WriteSymbol(0x4eb6fa, bg_weaponDefs);
	WriteSymbol(0x53cc84, bg_weaponDefs);
	WriteSymbol(0x53d89b, bg_weaponDefs);

	WriteSymbol(0x443058, bg_weaponDefs - 4);

	*(DWORD*)0x0443477 = ASSET_TYPE_WEAPON_COUNT +4;

    WriteSymbol(0x0041647B, &bg_itemlist[1]);
    WriteSymbol(0x00416546, &bg_itemlist[1]);
    WriteSymbol(0x0040BA7A, bg_itemlist);
    WriteSymbol(0x004164BA, bg_itemlist);
    WriteSymbol(0x0045437A, bg_itemlist);
    WriteSymbol(0x004ADB59, bg_itemlist);
    WriteSymbol(0x004BBEAF, bg_itemlist);
    WriteSymbol(0x004BBF41, bg_itemlist);
    WriteSymbol(0x004BC52B, bg_itemlist);
    WriteSymbol(0x004BCD92, bg_itemlist);
    WriteSymbol(0x004D31E5, bg_itemlist);
    WriteSymbol(0x004DFE70, bg_itemlist);

	SetCall(0x43FB5A, CG_ClearWeaponArray);
	SetCall(0x440467, CG_ClearWeaponArray);


	WriteSymbol(0x45aa0c, &cg_weaponsArray[0]);
	WriteSymbol(0x443088, &cg_weaponsArray[0]);
	WriteSymbol(0x4508ec, &cg_weaponsArray[0]);
	WriteSymbol(0x4508f5, &cg_weaponsArray[0]);
	WriteSymbol(0x454352, &cg_weaponsArray[0]);
	WriteSymbol(0x45475e, &cg_weaponsArray[0]);
	WriteSymbol(0x454925, &cg_weaponsArray[0]);
	WriteSymbol(0x456fbd, &cg_weaponsArray[0]);
	WriteSymbol(0x456d95, &cg_weaponsArray[0]);
	WriteSymbol(0x441834, &cg_weaponsArray[0]);
	WriteSymbol(0x457cac, &cg_weaponsArray[0]);
	WriteSymbol(0x441989, &cg_weaponsArray[0]);
	WriteSymbol(0x4403e5, &cg_weaponsArray[1].tree);
	WriteSymbol(0x453a4b, &cg_weaponsArray[0].tree);
	WriteSymbol(0x458de0, &cg_weaponsArray[0].knifeModel);
	WriteSymbol(0x454A2E, &cg_weaponsArray[1].knifeModel);
	WriteSymbol(0x4549b9, &cg_weaponsArray[1].knifeModel);
	WriteSymbol(0x454119, &cg_weaponsArray[1]);
	WriteSymbol(0x44350b, &cg_weaponsArray[0].translatedDisplayName);
	WriteSymbol(0x45434a, &cg_weaponsArray[0].registered);

	WriteSymbol(0x416273, bg_sharedAmmoCaps);
	WriteSymbol(0x41629c, bg_sharedAmmoCaps);
	WriteSymbol(0x4162bb, bg_sharedAmmoCaps);
	WriteSymbol(0x4162f4, bg_sharedAmmoCaps);
	WriteSymbol(0x41646a, bg_sharedAmmoCaps);
	WriteSymbol(0x41696a, bg_sharedAmmoCaps);
	WriteSymbol(0x4169ee, bg_sharedAmmoCaps);
	WriteSymbol(0x416a96, bg_sharedAmmoCaps);

	WriteSymbol(0x4161c3, bg_weapAmmoTypes);
	WriteSymbol(0x416206, bg_weapAmmoTypes);
	WriteSymbol(0x416465, bg_weapAmmoTypes);

	WriteSymbol(0x416383, bg_weapClips);
	WriteSymbol(0x4163c6, bg_weapClips);
	WriteSymbol(0x41646f, bg_weapClips);


	WriteSymbol(0x454578, weaponHudIcons);
	byte patchCG_DrawCursorHint[] =
	{
		PUSH_ESI,
		PUSH_EBX,
		NOP,NOP,NOP,NOP,NOP,
		ADD_ESP 0x18
	};
	memcpy((void*)0x4440BA, patchCG_DrawCursorHint, sizeof(patchCG_DrawCursorHint));
	SetCall(0x4440BC, CG_DrawCursorHint);

	SetCall(0x44E138, CG_UpdateWeaponViewmodels);

	SetCall(0x43EDDD, CG_RegisterItems);
	SetJump(0x44AF69, CG_RegisterItems);


}

//At 0x0443476 it clamps to 132 eg. 128 weapons


WeaponDef* BG_GetWeaponDef(int index)
{
  return bg_weaponDefs[index];
}

int BG_GetNumWeapons()
{
	return bg_lastParsedWeaponIndex + 1;
}

int BG_PlayerWeaponCountPrimaryTypes(playerState_t *ps)
{
  int count;
  signed int i;
  WeaponDef *weapDef;

  count = 0;
  for ( i = 1; i < BG_GetNumWeapons(); ++i )
  {
	weapDef = BG_GetWeaponDef(i);
    if ( weapDef->inventoryType == WEAPINVENTORY_PRIMARY )
    {
      if ( (1 << (i & 0x1F)) & ps->weapons[i >> 5] )
        ++count;
    }
  }
  return count;
}



const char* CG_GetWeaponUseString(int localClientNum, const char **secondaryString)
{
  WeaponDef *weapDef;
  weaponInfo_t *weapInfo;
  const char *translatedString;
  char out[260];

  weapDef = BG_GetWeaponDef(cg.cursorHintIcon -4);
  weapInfo = &cg_weaponsArray[cg.cursorHintIcon - 4];
  if ( weapDef->inventoryType == WEAPINVENTORY_PRIMARY )
  {
    GetKeyBindingLocalizedString(localClientNum, "+activate", out, 0);
    if (BG_PlayerWeaponCountPrimaryTypes(&cg.predictedPlayerState) <= 1 )
        translatedString = UI_SafeTranslateString("PLATFORM_PICKUPNEWWEAPON");
    else
        translatedString = UI_SafeTranslateString("PLATFORM_SWAPWEAPONS");

    *secondaryString = weapInfo->translatedDisplayName;
    return UI_ReplaceConversionString(translatedString, out);
  }
  if ( weapDef->offhandClass != 1 )
  {
    translatedString = UI_SafeTranslateString("PLATFORM_PICKUPNEWWEAPON");
    GetKeyBindingLocalizedString(localClientNum, "+activate", out, 0);
    *secondaryString = weapInfo->translatedDisplayName;
    return UI_ReplaceConversionString(translatedString, out);
  }
  translatedString = UI_SafeTranslateString("PLATFORM_THROWBACKGRENADE");
  GetKeyBindingLocalizedString(localClientNum, "+frag", out, 0);
  return UI_ReplaceConversionString(translatedString, out);
}



void CG_UpdateWeaponViewmodels(int localClientNum)
{
  int i;
  weaponInfo_t* weapInfo;
  char wm;

  for ( i = 1; i < BG_GetNumWeapons(); ++i )
  {
	weapInfo = &cg_weaponsArray[ i ];
    wm = cg.nextSnap->ps.weaponmodels[ i ];

	if ( weapInfo->weapModelIdx != wm )
	{
//		Com_Printf("CG_UpdateWeaponViewmodels() for gun %d\n", i);
		if(weapInfo->handModel == NULL)
		{
			char errorb[1024];
			Com_sprintf(errorb, sizeof(errorb), "CG_UpdateWeaponViewmodels(): weapInfo->handModel must not be NULL - index %d\n", i);
			MessageBoxA(NULL, errorb, "Fatal Error", MB_OK);
			return;
		}

		ChangeViewmodelDObj(localClientNum, i, wm, weapInfo->handModel, weapInfo->gogglesModel, weapInfo->rocketModel, weapInfo->knifeModel, 1u);
	}
  }
}

void __cdecl CG_ProcessClientNoteTracks(cg_t *cga, unsigned int clientNum)
{
  struct XAnimNotify_s *xnotify;
  int *v5;
  int i;

  int* a1 = (int*)cga;

  if(clientNum >= 64)
  {
	return;
  }

	for(i = 0, xnotify = g_notifyList; i < g_notifyListSize; ++i, ++xnotify)
	{
		//Hotfix for null pointer
		if(xnotify->name == NULL)
		{
			Com_PrintError(CON_CHANNEL_ERROR, "CG_ProcessClientNoteTracks(): Illegible Notify list entry. name = NULL\n");
			continue;
		}

        if ( Q_stricmp(xnotify->name, "anim_gunhand = \"left\"") == qfalse )
        {
          v5 = &a1[307 * clientNum];
          v5[240842] = 1;
          v5[240843] = 1;
          continue;
        }
        if ( Q_stricmp(xnotify->name, "anim_gunhand = \"right\"") == qfalse )
        {
          v5 = &a1[307 * clientNum];
          v5[240842] = 0;
          v5[240843] = 1;
          continue;
        }
	}


}

//byte __usercall __spoils<eax,ecx> CG_GetWeapReticleZoom@<al>(float *zoom@<edx>, cg_s *cgameGlob@<esi>)
byte CG_GetWeapReticleZoom(cg_t *cgameGlob, float *zoom)
{
  int v2; // eax@2
  WeaponDef *weap; // ecx@4
  double v5; // st6@6
  byte result; // al@7
  float v7; // [sp+0h] [bp-4h]@4
  float v8; // [sp+0h] [bp-4h]@9
  float v9; // [sp+0h] [bp-4h]@11

  if ( cgameGlob->predictedPlayerState.weapFlags & 2 )
    v2 = cgameGlob->predictedPlayerState.offHandIndex;
  else
    v2 = cgameGlob->predictedPlayerState.weapon;
  weap = BG_GetWeaponDef(v2);
  v7 = cgameGlob->predictedPlayerState.fWeaponPosFrac;
  *zoom = 0.0;
  if ( weap->overlayMaterial == 0 && weap->overlayReticle == WEAPOVERLAYRETICLE_NONE )
    goto LABEL_19;
  v5 = v7;
  if ( v7 == 0.0 )
    goto LABEL_19;
  if ( cgameGlob->playerEntity.bPositionToADS )
  {
    v8 = v5 - (1.0 - weap->fAdsZoomInFrac);
    *zoom = v8;
    if ( v8 > 0.0 )
      *zoom = v8 / weap->fAdsZoomInFrac;
  }
  else
  {
    v9 = v5 - (1.0 - weap->fAdsZoomOutFrac);
    *zoom = v9;
    if ( v9 > 0.0 )
      *zoom = v9 / weap->fAdsZoomOutFrac;
  }
  if ( *zoom <= 0.0099999998 )
  {
LABEL_19:
    result = 0;
  }
  else if ( *zoom <= 1.0 )
  {
    result = 1;
  }
  else
  {
    *zoom = 1.0;
    result = 1;
  }
  return result;
}
