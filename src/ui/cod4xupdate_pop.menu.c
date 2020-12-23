#include "../q_shared.h"
#include "../ui_shared.h"
#include "menudefinition.h"


#include <string.h>


menuDef_t* UI_InstallMenu(const char* connectadr)
{

	static char onclosecmds[512];
	float boxheight = 200;
	float boxwidth = 600;

#define ITEM_COUNT 22

	/* menuDef:cod4xupdate_pop */
	{

		static menuDef_t cod4xupdate_pop;
		int i;
		static itemDef_t *cod4xupdate_pop_items_ptr[ITEM_COUNT];
		static itemDef_t cod4xupdate_pop_items[ITEM_COUNT];
		memset(cod4xupdate_pop_items, 0, sizeof(cod4xupdate_pop_items));
		memset(&cod4xupdate_pop, 0, sizeof(cod4xupdate_pop));

		for(i = 0; i < 7; ++i){
			cod4xupdate_pop_items_ptr[i] = &cod4xupdate_pop_items[i];
		}
		cod4xupdate_pop_items_ptr[7] = &cod4xupdate_pop_items[15];
		for(i = 7; i < 15; ++i){
			cod4xupdate_pop_items_ptr[i+1] = &cod4xupdate_pop_items[i];
		}
		cod4xupdate_pop_items_ptr[16] = &cod4xupdate_pop_items[16];
		cod4xupdate_pop_items_ptr[17] = &cod4xupdate_pop_items[17];
		cod4xupdate_pop_items_ptr[18] = &cod4xupdate_pop_items[18];
		cod4xupdate_pop_items_ptr[19] = &cod4xupdate_pop_items[19];
		cod4xupdate_pop_items_ptr[20] = &cod4xupdate_pop_items[20];
		cod4xupdate_pop_items_ptr[21] = &cod4xupdate_pop_items[21];

		cod4xupdate_pop.items = cod4xupdate_pop_items_ptr;
		cod4xupdate_pop.window.name = "cod4xupdate_pop";
		cod4xupdate_pop.window.rect.x = -boxwidth / 2;
		cod4xupdate_pop.window.rect.y = -boxheight / 2;
		cod4xupdate_pop.window.rect.w = boxwidth;
		cod4xupdate_pop.window.rect.h = boxheight;
		cod4xupdate_pop.window.rect.horzAlign = 2;
		cod4xupdate_pop.window.rect.vertAlign = 2;
		cod4xupdate_pop.window.rectClient.x = 0.000000;
		cod4xupdate_pop.window.rectClient.y = 0.000000;
		cod4xupdate_pop.window.rectClient.w = 0.000000;
		cod4xupdate_pop.window.rectClient.h = 0.000000;
		cod4xupdate_pop.window.rectClient.horzAlign = 0;
		cod4xupdate_pop.window.rectClient.vertAlign = 0;
		cod4xupdate_pop.window.style = 1;
		cod4xupdate_pop.window.border = 1;
		cod4xupdate_pop.window.borderSize = 2.000000;
		cod4xupdate_pop.window.staticFlags = 0x1000000;
		cod4xupdate_pop.window.dynamicFlags[0] = 0x0;
		cod4xupdate_pop.window.foreColor[0] = 1.000000;
		cod4xupdate_pop.window.foreColor[1] = 1.000000;
		cod4xupdate_pop.window.foreColor[2] = 1.000000;
		cod4xupdate_pop.window.foreColor[3] = 1.000000;
		cod4xupdate_pop.window.backColor[0] = 0.000000;
		cod4xupdate_pop.window.backColor[1] = 0.000000;
		cod4xupdate_pop.window.backColor[2] = 0.000000;
		cod4xupdate_pop.window.backColor[3] = 0.000000;
		cod4xupdate_pop.window.borderColor[0] = 0.400000;
		cod4xupdate_pop.window.borderColor[1] = 0.400000;
		cod4xupdate_pop.window.borderColor[2] = 0.420000;
		cod4xupdate_pop.window.borderColor[3] = 1.000000;
		cod4xupdate_pop.window.outlineColor[0] = 0.000000;
		cod4xupdate_pop.window.outlineColor[1] = 0.000000;
		cod4xupdate_pop.window.outlineColor[2] = 0.000000;
		cod4xupdate_pop.window.outlineColor[3] = 0.000000;
		cod4xupdate_pop.itemCount = ITEM_COUNT;
		cod4xupdate_pop.fadeClamp = 0.000000;
		cod4xupdate_pop.fadeAmount = 0.000000;
		cod4xupdate_pop.fadeInAmount = 0.000000;
		cod4xupdate_pop.blurWorld = 0.000000;
		cod4xupdate_pop.onOpen = "\"setLocalVarBool\" \"ui_centerPopup\" 1 ; \"focusfirst\" ; ";
		Com_sprintf(onclosecmds, sizeof(onclosecmds), "\"play\" \"mouse_click\" ; \"setLocalVarBool\" \"ui_centerPopup\" 0 ; exec \"connect %s\"; ", connectadr);
		cod4xupdate_pop.onClose = onclosecmds;
		cod4xupdate_pop.onESC = NULL;//"\"setLocalVarBool\" \"ui_centerPopup\" 0 ; \"close\" \"self\" ; ; ; ";
		cod4xupdate_pop.focusColor[0] = 1.000000;
		cod4xupdate_pop.focusColor[1] = 1.000000;
		cod4xupdate_pop.focusColor[2] = 1.000000;
		cod4xupdate_pop.focusColor[3] = 1.000000;
		cod4xupdate_pop.disableColor[0] = 0.000000;
		cod4xupdate_pop.disableColor[1] = 0.000000;
		cod4xupdate_pop.disableColor[2] = 0.000000;
		cod4xupdate_pop.disableColor[3] = 0.000000;
		cod4xupdate_pop.cursorItem = -1;
		/* itemDef:cod4xupdate_pop_items[0] */
		{
			cod4xupdate_pop_items[0].window.rect.x = -877.000000;
			cod4xupdate_pop_items[0].window.rect.y = -842.000000;
			cod4xupdate_pop_items[0].window.rect.w = 2000.000000;
			cod4xupdate_pop_items[0].window.rect.h = 2000.000000;
			cod4xupdate_pop_items[0].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[0].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[0].window.rectClient.x = -600.000000;
			cod4xupdate_pop_items[0].window.rectClient.y = -800.000000;
			cod4xupdate_pop_items[0].window.rectClient.w = 2000.000000;
			cod4xupdate_pop_items[0].window.rectClient.h = 2000.000000;
			cod4xupdate_pop_items[0].window.rectClient.horzAlign = 0;
			cod4xupdate_pop_items[0].window.rectClient.vertAlign = 0;
			cod4xupdate_pop_items[0].window.borderSize = 1.000000;
			cod4xupdate_pop_items[0].window.dynamicFlags[0] = 0x4;
			cod4xupdate_pop_items[0].window.foreColor[0] = 1.000000;
			cod4xupdate_pop_items[0].window.foreColor[1] = 1.000000;
			cod4xupdate_pop_items[0].window.foreColor[2] = 1.000000;
			cod4xupdate_pop_items[0].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[0].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[0].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[0].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[0].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[0].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[0].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[0].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[0].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[0].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[0].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[0].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[0].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[0].type = 1;
			cod4xupdate_pop_items[0].dataType = 1;
			cod4xupdate_pop_items[0].textFont = 1;
			cod4xupdate_pop_items[0].textAlignX = 0.000000;
			cod4xupdate_pop_items[0].textAlignY = 0.000000;
			cod4xupdate_pop_items[0].textScale = 0.550000;
			cod4xupdate_pop_items[0].text = "";
			cod4xupdate_pop_items[0].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[0].action = "\"close\" \"self\" ; ";
			cod4xupdate_pop_items[0].onFocus = "\"setdvar\" \"ui_popup_close_icon\" 1 ";
			cod4xupdate_pop_items[0].leaveFocus = "\"setdvar\" \"ui_popup_close_icon\" 0 ";
			cod4xupdate_pop_items[0].feeder = 0.000000;
			cod4xupdate_pop_items[0].cursorPos = 0;
			cod4xupdate_pop_items[0].textRect.x = 0.000000;
			cod4xupdate_pop_items[0].textRect.y = 0.000000;
			cod4xupdate_pop_items[0].textRect.w = 0.000000;
			cod4xupdate_pop_items[0].textRect.h = 0.000000;
			cod4xupdate_pop_items[0].textRect.horzAlign = 0;
			cod4xupdate_pop_items[0].textRect.vertAlign = 0;
		}
		/* itemDef:cod4xupdate_pop_items[1] */
		{
			cod4xupdate_pop_items[1].window.rect.x = -281.000000;
			cod4xupdate_pop_items[1].window.rect.y = -46.000000;
			cod4xupdate_pop_items[1].window.rect.w = boxwidth;
			cod4xupdate_pop_items[1].window.rect.h = boxheight;
			cod4xupdate_pop_items[1].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[1].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[1].window.rectClient.x = -4.000000;
			cod4xupdate_pop_items[1].window.rectClient.y = -4.000000;
			cod4xupdate_pop_items[1].window.rectClient.w = boxwidth;
			cod4xupdate_pop_items[1].window.rectClient.h = boxheight;
			cod4xupdate_pop_items[1].window.rectClient.horzAlign = 0;
			cod4xupdate_pop_items[1].window.rectClient.vertAlign = 0;
			cod4xupdate_pop_items[1].window.borderSize = 1.000000;
			cod4xupdate_pop_items[1].window.dynamicFlags[0] = 0x4;
			cod4xupdate_pop_items[1].window.foreColor[0] = 1.000000;
			cod4xupdate_pop_items[1].window.foreColor[1] = 1.000000;
			cod4xupdate_pop_items[1].window.foreColor[2] = 1.000000;
			cod4xupdate_pop_items[1].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[1].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[1].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[1].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[1].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[1].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[1].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[1].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[1].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[1].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[1].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[1].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[1].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[1].type = 1;
			cod4xupdate_pop_items[1].dataType = 1;
			cod4xupdate_pop_items[1].textFont = 1;
			cod4xupdate_pop_items[1].textAlignX = 0.000000;
			cod4xupdate_pop_items[1].textAlignY = 0.000000;
			cod4xupdate_pop_items[1].textScale = 0.550000;
			cod4xupdate_pop_items[1].text = "";
			cod4xupdate_pop_items[1].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[1].action = "; ";
			cod4xupdate_pop_items[1].feeder = 0.000000;
			cod4xupdate_pop_items[1].cursorPos = 0;
			cod4xupdate_pop_items[1].textRect.x = 0.000000;
			cod4xupdate_pop_items[1].textRect.y = 0.000000;
			cod4xupdate_pop_items[1].textRect.w = 0.000000;
			cod4xupdate_pop_items[1].textRect.h = 0.000000;
			cod4xupdate_pop_items[1].textRect.horzAlign = 0;
			cod4xupdate_pop_items[1].textRect.vertAlign = 0;
		}
		/* itemDef:cod4xupdate_pop_items[2] */
		{
			cod4xupdate_pop_items[2].window.rect.x = -877.000000;
			cod4xupdate_pop_items[2].window.rect.y = -842.000000;
			cod4xupdate_pop_items[2].window.rect.w = 2000.000000;
			cod4xupdate_pop_items[2].window.rect.h = 2000.000000;
			cod4xupdate_pop_items[2].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[2].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[2].window.rectClient.x = -600.000000;
			cod4xupdate_pop_items[2].window.rectClient.y = -800.000000;
			cod4xupdate_pop_items[2].window.rectClient.w = 2000.000000;
			cod4xupdate_pop_items[2].window.rectClient.h = 2000.000000;
			cod4xupdate_pop_items[2].window.rectClient.horzAlign = 0;
			cod4xupdate_pop_items[2].window.rectClient.vertAlign = 0;
			cod4xupdate_pop_items[2].window.style = 3;
			cod4xupdate_pop_items[2].window.borderSize = 1.000000;
			cod4xupdate_pop_items[2].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[2].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[2].window.foreColor[0] = 0.000000;
			cod4xupdate_pop_items[2].window.foreColor[1] = 0.000000;
			cod4xupdate_pop_items[2].window.foreColor[2] = 0.000000;
			cod4xupdate_pop_items[2].window.foreColor[3] = 0.275000;
			cod4xupdate_pop_items[2].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[2].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[2].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[2].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[2].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[2].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[2].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[2].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[2].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[2].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[2].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[2].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[2].type = 0;
			cod4xupdate_pop_items[2].dataType = 0;
			cod4xupdate_pop_items[2].textAlignX = 0.000000;
			cod4xupdate_pop_items[2].textAlignY = 0.000000;
			cod4xupdate_pop_items[2].textScale = 0.550000;
			cod4xupdate_pop_items[2].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[2].feeder = 0.000000;
			cod4xupdate_pop_items[2].cursorPos = 0;
			cod4xupdate_pop_items[2].textRect.x = 0.000000;
			cod4xupdate_pop_items[2].textRect.y = 0.000000;
			cod4xupdate_pop_items[2].textRect.w = 0.000000;
			cod4xupdate_pop_items[2].textRect.h = 0.000000;
			cod4xupdate_pop_items[2].textRect.horzAlign = 0;
			cod4xupdate_pop_items[2].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item2_visibleExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item2_visibleExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item2_visibleExp_ptr[i] = &cod4xupdate_pop_item2_visibleExp[i];
			}

			memset(cod4xupdate_pop_item2_visibleExp, 0, sizeof(cod4xupdate_pop_item2_visibleExp));
			cod4xupdate_pop_items[2].visibleExp.entries = cod4xupdate_pop_item2_visibleExp_ptr;
			cod4xupdate_pop_item2_visibleExp[0].type = 0;
			cod4xupdate_pop_item2_visibleExp[0].data.op = 16;
			cod4xupdate_pop_item2_visibleExp[1].type = 1;
			cod4xupdate_pop_item2_visibleExp[1].data.operand.dataType = 0;
			cod4xupdate_pop_item2_visibleExp[1].data.operand.internals.intVal = 1;
			cod4xupdate_pop_items[2].visibleExp.numEntries = 2;

			static expressionEntry_t *cod4xupdate_pop_item2_materialExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item2_materialExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item2_materialExp_ptr[i] = &cod4xupdate_pop_item2_materialExp[i];
			}

			memset(cod4xupdate_pop_item2_materialExp, 0, sizeof(cod4xupdate_pop_item2_materialExp));
			cod4xupdate_pop_items[2].materialExp.entries = cod4xupdate_pop_item2_materialExp_ptr;
			cod4xupdate_pop_item2_materialExp[0].type = 0;
			cod4xupdate_pop_item2_materialExp[0].data.op = 16;
			cod4xupdate_pop_item2_materialExp[1].type = 1;
			cod4xupdate_pop_item2_materialExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item2_materialExp[1].data.operand.internals.string = "white";
			cod4xupdate_pop_items[2].materialExp.numEntries = 2;
		}
		/* itemDef:cod4xupdate_pop_items[3] */
		{
			cod4xupdate_pop_items[3].window.rect.x = -(boxwidth -4 / 2);
			cod4xupdate_pop_items[3].window.rect.y = -(boxheight -4 / 2);
			cod4xupdate_pop_items[3].window.rect.w = boxwidth -4;
			cod4xupdate_pop_items[3].window.rect.h = boxheight -4;
			cod4xupdate_pop_items[3].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[3].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[3].window.rectClient.x = 0.000000;
			cod4xupdate_pop_items[3].window.rectClient.y = 0.000000;
			cod4xupdate_pop_items[3].window.rectClient.w = boxwidth - 4;
			cod4xupdate_pop_items[3].window.rectClient.h = boxheight - 4;
			cod4xupdate_pop_items[3].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[3].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[3].window.style = 3;
			cod4xupdate_pop_items[3].window.borderSize = 1.000000;
			cod4xupdate_pop_items[3].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[3].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[3].window.foreColor[0] = 0.200000;
			cod4xupdate_pop_items[3].window.foreColor[1] = 0.200000;
			cod4xupdate_pop_items[3].window.foreColor[2] = 0.220000;
			cod4xupdate_pop_items[3].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[3].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[3].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[3].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[3].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[3].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[3].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[3].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[3].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[3].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[3].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[3].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[3].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[3].window.background = Material_RegisterHandle("white", 0);
			cod4xupdate_pop_items[3].type = 0;
			cod4xupdate_pop_items[3].dataType = 0;
			cod4xupdate_pop_items[3].textAlignX = 0.000000;
			cod4xupdate_pop_items[3].textAlignY = 0.000000;
			cod4xupdate_pop_items[3].textScale = 0.550000;
			cod4xupdate_pop_items[3].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[3].feeder = 0.000000;
			cod4xupdate_pop_items[3].cursorPos = 0;
			cod4xupdate_pop_items[3].textRect.x = 0.000000;
			cod4xupdate_pop_items[3].textRect.y = 0.000000;
			cod4xupdate_pop_items[3].textRect.w = 0.000000;
			cod4xupdate_pop_items[3].textRect.h = 0.000000;
			cod4xupdate_pop_items[3].textRect.horzAlign = 0;
			cod4xupdate_pop_items[3].textRect.vertAlign = 0;
		}
		/* itemDef:cod4xupdate_pop_items[4] */
		{
			cod4xupdate_pop_items[4].window.rect.x = 2.000000;
			cod4xupdate_pop_items[4].window.rect.y = -40.000000;
			cod4xupdate_pop_items[4].window.rect.w = 0.000000;
			cod4xupdate_pop_items[4].window.rect.h = 0.000000;
			cod4xupdate_pop_items[4].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[4].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[4].window.rectClient.x = 6;
			cod4xupdate_pop_items[4].window.rectClient.y = 2.000000;
			cod4xupdate_pop_items[4].window.rectClient.w = 0.000000;
			cod4xupdate_pop_items[4].window.rectClient.h = 0.000000;
			cod4xupdate_pop_items[4].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[4].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[4].window.borderSize = 1.000000;
			cod4xupdate_pop_items[4].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[4].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[4].window.foreColor[0] = 0.690000;
			cod4xupdate_pop_items[4].window.foreColor[1] = 0.690000;
			cod4xupdate_pop_items[4].window.foreColor[2] = 0.690000;
			cod4xupdate_pop_items[4].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[4].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[4].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[4].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[4].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[4].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[4].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[4].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[4].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[4].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[4].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[4].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[4].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[4].type = 1;
			cod4xupdate_pop_items[4].dataType = 1;
			cod4xupdate_pop_items[4].textFont = 1;
			cod4xupdate_pop_items[4].textAlign = ITEM_ALIGN_TOP_LEFT;
			cod4xupdate_pop_items[4].textAlignX = 0.000000;
			cod4xupdate_pop_items[4].textAlignY = 0.000000;
			cod4xupdate_pop_items[4].textScale = 0.36666;
			cod4xupdate_pop_items[4].textStyle = 6;
			cod4xupdate_pop_items[4].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[4].feeder = 0.000000;
			cod4xupdate_pop_items[4].cursorPos = 0;
			cod4xupdate_pop_items[4].textRect.x = 0.000000;
			cod4xupdate_pop_items[4].textRect.y = 0.000000;
			cod4xupdate_pop_items[4].textRect.w = 0.000000;
			cod4xupdate_pop_items[4].textRect.h = 0.000000;
			cod4xupdate_pop_items[4].textRect.horzAlign = 0;
			cod4xupdate_pop_items[4].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item4_textExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item4_textExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item4_textExp_ptr[i] = &cod4xupdate_pop_item4_textExp[i];
			}

			memset(cod4xupdate_pop_item4_textExp, 0, sizeof(cod4xupdate_pop_item4_textExp));
			cod4xupdate_pop_items[4].textExp.entries = cod4xupdate_pop_item4_textExp_ptr;
			cod4xupdate_pop_item4_textExp[0].type = 0;
			cod4xupdate_pop_item4_textExp[0].data.op = 16;
			cod4xupdate_pop_item4_textExp[1].type = 1;
			cod4xupdate_pop_item4_textExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item4_textExp[1].data.operand.internals.string = "The server you have attempted to join requires CoD4 X 18. CoD4 X is a community made CoD4 patch.\nThis patch offers you the following features:";
			cod4xupdate_pop_items[4].textExp.numEntries = 2;
		}
		/* itemDef:cod4xupdate_pop_items[5] */
		{
			cod4xupdate_pop_items[5].window.rect.x = -275.000000;
			cod4xupdate_pop_items[5].window.rect.y = 2.000000;
			cod4xupdate_pop_items[5].window.rect.w = boxwidth - 12.5;
			cod4xupdate_pop_items[5].window.rect.h = 18.000000;
			cod4xupdate_pop_items[5].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[5].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[5].window.rectClient.x = 2.000000;
			cod4xupdate_pop_items[5].window.rectClient.y = boxheight - 4 - 40;
			cod4xupdate_pop_items[5].window.rectClient.w = boxwidth - 12.5;
			cod4xupdate_pop_items[5].window.rectClient.h = 18.000000;
			cod4xupdate_pop_items[5].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[5].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[5].window.style = 3;
			cod4xupdate_pop_items[5].window.borderSize = 1.000000;
			cod4xupdate_pop_items[5].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[5].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[5].window.foreColor[0] = 0.900000;
			cod4xupdate_pop_items[5].window.foreColor[1] = 0.900000;
			cod4xupdate_pop_items[5].window.foreColor[2] = 1.000000;
			cod4xupdate_pop_items[5].window.foreColor[3] = 0.070000;
			cod4xupdate_pop_items[5].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[5].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[5].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[5].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[5].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[5].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[5].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[5].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[5].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[5].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[5].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[5].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[5].type = 0;
			cod4xupdate_pop_items[5].dataType = 0;
			cod4xupdate_pop_items[5].textAlignX = 0.000000;
			cod4xupdate_pop_items[5].textAlignY = 0.000000;
			cod4xupdate_pop_items[5].textScale = 0.550000;
			cod4xupdate_pop_items[5].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[5].feeder = 0.000000;
			cod4xupdate_pop_items[5].cursorPos = 0;
			cod4xupdate_pop_items[5].textRect.x = 0.000000;
			cod4xupdate_pop_items[5].textRect.y = 0.000000;
			cod4xupdate_pop_items[5].textRect.w = 0.000000;
			cod4xupdate_pop_items[5].textRect.h = 0.000000;
			cod4xupdate_pop_items[5].textRect.horzAlign = 0;
			cod4xupdate_pop_items[5].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item5_materialExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item5_materialExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item5_materialExp_ptr[i] = &cod4xupdate_pop_item5_materialExp[i];
			}

			memset(cod4xupdate_pop_item5_materialExp, 0, sizeof(cod4xupdate_pop_item5_materialExp));
			cod4xupdate_pop_items[5].materialExp.entries = cod4xupdate_pop_item5_materialExp_ptr;
			cod4xupdate_pop_item5_materialExp[0].type = 0;
			cod4xupdate_pop_item5_materialExp[0].data.op = 16;
			cod4xupdate_pop_item5_materialExp[1].type = 1;
			cod4xupdate_pop_item5_materialExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item5_materialExp[1].data.operand.internals.string = "gradient_fadein";
			cod4xupdate_pop_items[5].materialExp.numEntries = 2;
		}
		/* itemDef:cod4xupdate_pop_items[6] */
		{
			cod4xupdate_pop_items[6].window.rect.x = 270.500000;
			cod4xupdate_pop_items[6].window.rect.y = 2.000000;
			cod4xupdate_pop_items[6].window.rect.w = 4.500000;
			cod4xupdate_pop_items[6].window.rect.h = 18.000000;
			cod4xupdate_pop_items[6].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[6].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[6].window.rectClient.x = boxwidth - 10.5;
			cod4xupdate_pop_items[6].window.rectClient.y = boxheight - 4 - 40;
			cod4xupdate_pop_items[6].window.rectClient.w = 4.500000;
			cod4xupdate_pop_items[6].window.rectClient.h = 18.000000;
			cod4xupdate_pop_items[6].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[6].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[6].window.style = 3;
			cod4xupdate_pop_items[6].window.borderSize = 1.000000;
			cod4xupdate_pop_items[6].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[6].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[6].window.foreColor[0] = 0.900000;
			cod4xupdate_pop_items[6].window.foreColor[1] = 0.900000;
			cod4xupdate_pop_items[6].window.foreColor[2] = 1.000000;
			cod4xupdate_pop_items[6].window.foreColor[3] = 0.070000;
			cod4xupdate_pop_items[6].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[6].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[6].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[6].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[6].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[6].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[6].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[6].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[6].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[6].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[6].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[6].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[6].window.background = Material_RegisterHandle("button_highlight_end", 0);
			cod4xupdate_pop_items[6].type = 0;
			cod4xupdate_pop_items[6].dataType = 0;
			cod4xupdate_pop_items[6].textAlignX = 0.000000;
			cod4xupdate_pop_items[6].textAlignY = 0.000000;
			cod4xupdate_pop_items[6].textScale = 0.550000;
			cod4xupdate_pop_items[6].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[6].feeder = 0.000000;
			cod4xupdate_pop_items[6].cursorPos = 0;
			cod4xupdate_pop_items[6].textRect.x = 0.000000;
			cod4xupdate_pop_items[6].textRect.y = 0.000000;
			cod4xupdate_pop_items[6].textRect.w = 0.000000;
			cod4xupdate_pop_items[6].textRect.h = 0.000000;
			cod4xupdate_pop_items[6].textRect.horzAlign = 0;
			cod4xupdate_pop_items[6].textRect.vertAlign = 0;
		}
		/* itemDef:cod4xupdate_pop_items[7] */
		{
			cod4xupdate_pop_items[7].window.rect.x = -275.000000;
			cod4xupdate_pop_items[7].window.rect.y = 2.000000;
			cod4xupdate_pop_items[7].window.rect.w = boxwidth - 12.5;
			cod4xupdate_pop_items[7].window.rect.h = 18.000000;
			cod4xupdate_pop_items[7].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[7].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[7].window.rectClient.x = 2.000000;
			cod4xupdate_pop_items[7].window.rectClient.y = boxheight - 4 - 40;
			cod4xupdate_pop_items[7].window.rectClient.w = boxwidth - 12.5;
			cod4xupdate_pop_items[7].window.rectClient.h = 18.000000;
			cod4xupdate_pop_items[7].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[7].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[7].window.style = 3;
			cod4xupdate_pop_items[7].window.borderSize = 2.000000;
			cod4xupdate_pop_items[7].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[7].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[7].window.foreColor[0] = 0.900000;
			cod4xupdate_pop_items[7].window.foreColor[1] = 0.950000;
			cod4xupdate_pop_items[7].window.foreColor[2] = 1.000000;
			cod4xupdate_pop_items[7].window.foreColor[3] = 0.250000;
			cod4xupdate_pop_items[7].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[7].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[7].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[7].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[7].window.borderColor[0] = 0.800000;
			cod4xupdate_pop_items[7].window.borderColor[1] = 0.950000;
			cod4xupdate_pop_items[7].window.borderColor[2] = 1.000000;
			cod4xupdate_pop_items[7].window.borderColor[3] = 0.400000;
			cod4xupdate_pop_items[7].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[7].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[7].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[7].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[7].window.background = Material_RegisterHandle("gradient_fadein", 0);
			cod4xupdate_pop_items[7].type = 0;
			cod4xupdate_pop_items[7].dataType = 0;
			cod4xupdate_pop_items[7].textAlignX = 0.000000;
			cod4xupdate_pop_items[7].textAlignY = 0.000000;
			cod4xupdate_pop_items[7].textScale = 0.550000;
			cod4xupdate_pop_items[7].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[7].feeder = 0.000000;
			cod4xupdate_pop_items[7].cursorPos = 0;
			cod4xupdate_pop_items[7].textRect.x = 0.000000;
			cod4xupdate_pop_items[7].textRect.y = 0.000000;
			cod4xupdate_pop_items[7].textRect.w = 0.000000;
			cod4xupdate_pop_items[7].textRect.h = 0.000000;
			cod4xupdate_pop_items[7].textRect.horzAlign = 0;
			cod4xupdate_pop_items[7].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item7_visibleExp_ptr[12];
			static expressionEntry_t cod4xupdate_pop_item7_visibleExp[12];

			for(i = 0; i < 12; ++i){
				cod4xupdate_pop_item7_visibleExp_ptr[i] = &cod4xupdate_pop_item7_visibleExp[i];
			}

			memset(cod4xupdate_pop_item7_visibleExp, 0, sizeof(cod4xupdate_pop_item7_visibleExp));
			cod4xupdate_pop_items[7].visibleExp.entries = cod4xupdate_pop_item7_visibleExp_ptr;
			cod4xupdate_pop_item7_visibleExp[0].type = 0;
			cod4xupdate_pop_item7_visibleExp[0].data.op = 16;
			cod4xupdate_pop_item7_visibleExp[1].type = 0;
			cod4xupdate_pop_item7_visibleExp[1].data.op = 58;
			cod4xupdate_pop_item7_visibleExp[2].type = 1;
			cod4xupdate_pop_item7_visibleExp[2].data.operand.dataType = 2;
			cod4xupdate_pop_item7_visibleExp[2].data.operand.internals.string = "ui_highlight";
			cod4xupdate_pop_item7_visibleExp[3].type = 0;
			cod4xupdate_pop_item7_visibleExp[3].data.op = 1;
			cod4xupdate_pop_item7_visibleExp[4].type = 0;
			cod4xupdate_pop_item7_visibleExp[4].data.op = 12;
			cod4xupdate_pop_item7_visibleExp[5].type = 1;
			cod4xupdate_pop_item7_visibleExp[5].data.operand.dataType = 0;
			cod4xupdate_pop_item7_visibleExp[5].data.operand.internals.intVal = 3;
			cod4xupdate_pop_item7_visibleExp[6].type = 0;
			cod4xupdate_pop_item7_visibleExp[6].data.op = 14;
			cod4xupdate_pop_item7_visibleExp[7].type = 0;
			cod4xupdate_pop_item7_visibleExp[7].data.op = 61;
			cod4xupdate_pop_item7_visibleExp[8].type = 1;
			cod4xupdate_pop_item7_visibleExp[8].data.operand.dataType = 2;
			cod4xupdate_pop_item7_visibleExp[8].data.operand.internals.string = "ui_choicegroup";
			cod4xupdate_pop_item7_visibleExp[9].type = 0;
			cod4xupdate_pop_item7_visibleExp[9].data.op = 1;
			cod4xupdate_pop_item7_visibleExp[10].type = 0;
			cod4xupdate_pop_item7_visibleExp[10].data.op = 12;
			cod4xupdate_pop_item7_visibleExp[11].type = 1;
			cod4xupdate_pop_item7_visibleExp[11].data.operand.dataType = 2;
			cod4xupdate_pop_item7_visibleExp[11].data.operand.internals.string = "popmenu";
			cod4xupdate_pop_items[7].visibleExp.numEntries = 12;
		}
		/* itemDef:cod4xupdate_pop_items[8] */
		{
			cod4xupdate_pop_items[8].window.rect.x = 270.500000;
			cod4xupdate_pop_items[8].window.rect.y = 2.000000;
			cod4xupdate_pop_items[8].window.rect.w = 4.500000;
			cod4xupdate_pop_items[8].window.rect.h = 18.000000;
			cod4xupdate_pop_items[8].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[8].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[8].window.rectClient.x = boxwidth - 10.5;
			cod4xupdate_pop_items[8].window.rectClient.y = boxheight - 4 - 40;
			cod4xupdate_pop_items[8].window.rectClient.w = 4.500000;
			cod4xupdate_pop_items[8].window.rectClient.h = 18.000000;
			cod4xupdate_pop_items[8].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[8].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[8].window.style = 3;
			cod4xupdate_pop_items[8].window.borderSize = 1.000000;
			cod4xupdate_pop_items[8].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[8].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[8].window.foreColor[0] = 0.900000;
			cod4xupdate_pop_items[8].window.foreColor[1] = 0.950000;
			cod4xupdate_pop_items[8].window.foreColor[2] = 1.000000;
			cod4xupdate_pop_items[8].window.foreColor[3] = 0.250000;
			cod4xupdate_pop_items[8].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[8].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[8].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[8].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[8].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[8].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[8].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[8].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[8].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[8].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[8].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[8].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[8].window.background = Material_RegisterHandle("button_highlight_end", 0);
			cod4xupdate_pop_items[8].type = 0;
			cod4xupdate_pop_items[8].dataType = 0;
			cod4xupdate_pop_items[8].textAlignX = 0.000000;
			cod4xupdate_pop_items[8].textAlignY = 0.000000;
			cod4xupdate_pop_items[8].textScale = 0.550000;
			cod4xupdate_pop_items[8].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[8].feeder = 0.000000;
			cod4xupdate_pop_items[8].cursorPos = 0;
			cod4xupdate_pop_items[8].textRect.x = 0.000000;
			cod4xupdate_pop_items[8].textRect.y = 0.000000;
			cod4xupdate_pop_items[8].textRect.w = 0.000000;
			cod4xupdate_pop_items[8].textRect.h = 0.000000;
			cod4xupdate_pop_items[8].textRect.horzAlign = 0;
			cod4xupdate_pop_items[8].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item8_visibleExp_ptr[12];
			static expressionEntry_t cod4xupdate_pop_item8_visibleExp[12];

			for(i = 0; i < 12; ++i){
				cod4xupdate_pop_item8_visibleExp_ptr[i] = &cod4xupdate_pop_item8_visibleExp[i];
			}

			memset(cod4xupdate_pop_item8_visibleExp, 0, sizeof(cod4xupdate_pop_item8_visibleExp));
			cod4xupdate_pop_items[8].visibleExp.entries = cod4xupdate_pop_item8_visibleExp_ptr;
			cod4xupdate_pop_item8_visibleExp[0].type = 0;
			cod4xupdate_pop_item8_visibleExp[0].data.op = 16;
			cod4xupdate_pop_item8_visibleExp[1].type = 0;
			cod4xupdate_pop_item8_visibleExp[1].data.op = 58;
			cod4xupdate_pop_item8_visibleExp[2].type = 1;
			cod4xupdate_pop_item8_visibleExp[2].data.operand.dataType = 2;
			cod4xupdate_pop_item8_visibleExp[2].data.operand.internals.string = "ui_highlight";
			cod4xupdate_pop_item8_visibleExp[3].type = 0;
			cod4xupdate_pop_item8_visibleExp[3].data.op = 1;
			cod4xupdate_pop_item8_visibleExp[4].type = 0;
			cod4xupdate_pop_item8_visibleExp[4].data.op = 12;
			cod4xupdate_pop_item8_visibleExp[5].type = 1;
			cod4xupdate_pop_item8_visibleExp[5].data.operand.dataType = 0;
			cod4xupdate_pop_item8_visibleExp[5].data.operand.internals.intVal = 3;
			cod4xupdate_pop_item8_visibleExp[6].type = 0;
			cod4xupdate_pop_item8_visibleExp[6].data.op = 14;
			cod4xupdate_pop_item8_visibleExp[7].type = 0;
			cod4xupdate_pop_item8_visibleExp[7].data.op = 61;
			cod4xupdate_pop_item8_visibleExp[8].type = 1;
			cod4xupdate_pop_item8_visibleExp[8].data.operand.dataType = 2;
			cod4xupdate_pop_item8_visibleExp[8].data.operand.internals.string = "ui_choicegroup";
			cod4xupdate_pop_item8_visibleExp[9].type = 0;
			cod4xupdate_pop_item8_visibleExp[9].data.op = 1;
			cod4xupdate_pop_item8_visibleExp[10].type = 0;
			cod4xupdate_pop_item8_visibleExp[10].data.op = 12;
			cod4xupdate_pop_item8_visibleExp[11].type = 1;
			cod4xupdate_pop_item8_visibleExp[11].data.operand.dataType = 2;
			cod4xupdate_pop_item8_visibleExp[11].data.operand.internals.string = "popmenu";
			cod4xupdate_pop_items[8].visibleExp.numEntries = 12;
		}
		/* itemDef:cod4xupdate_pop_items[9] */
		{
			cod4xupdate_pop_items[9].window.rect.x = -275.000000;
			cod4xupdate_pop_items[9].window.rect.y = 2.000000;
			cod4xupdate_pop_items[9].window.rect.w = boxwidth - 0;
			cod4xupdate_pop_items[9].window.rect.h = 18.000000;
			cod4xupdate_pop_items[9].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[9].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[9].window.rectClient.x = 2.000000;
			cod4xupdate_pop_items[9].window.rectClient.y = boxheight - 4 - 40;
			cod4xupdate_pop_items[9].window.rectClient.w = boxwidth - 8;
			cod4xupdate_pop_items[9].window.rectClient.h = 18.000000;
			cod4xupdate_pop_items[9].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[9].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[9].window.borderSize = 1.000000;
			cod4xupdate_pop_items[9].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[9].window.foreColor[0] = 1.0000;
			cod4xupdate_pop_items[9].window.foreColor[1] = 1.0000;
			cod4xupdate_pop_items[9].window.foreColor[2] = 1.0000;
			cod4xupdate_pop_items[9].window.foreColor[3] = 1.0000;
			cod4xupdate_pop_items[9].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[9].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[9].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[9].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[9].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[9].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[9].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[9].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[9].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[9].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[9].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[9].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[9].type = 1;
			cod4xupdate_pop_items[9].dataType = 1;
			cod4xupdate_pop_items[9].textFont = 1;
			cod4xupdate_pop_items[9].textAlign = 10;
			cod4xupdate_pop_items[9].textAlignX = -6.000000;
			cod4xupdate_pop_items[9].textAlignY = 0.000000;
			cod4xupdate_pop_items[9].textScale = 0.4;
			cod4xupdate_pop_items[9].textStyle = 6;
			cod4xupdate_pop_items[9].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[9].action = "\"play\" \"mouse_click\" ; \"close\" \"self\" ; \"uiScript\" \"installcod4x\" ; ";
			cod4xupdate_pop_items[9].onFocus = "\"play\" \"mouse_submenu_over\" ; \"setLocalVarInt\" \"ui_highlight\" 3 ; \"setLocalVarString\" \"ui_choicegroup\" \"popmenu\" ; ; ";
			cod4xupdate_pop_items[9].leaveFocus = "\"setLocalVarInt\" \"ui_highlight\" 0 ; ; ";
			cod4xupdate_pop_items[9].feeder = 0.000000;
			cod4xupdate_pop_items[9].cursorPos = 0;
			cod4xupdate_pop_items[9].textRect.x = 0.000000;
			cod4xupdate_pop_items[9].textRect.y = 0.000000;
			cod4xupdate_pop_items[9].textRect.w = 0.000000;
			cod4xupdate_pop_items[9].textRect.h = 0.000000;
			cod4xupdate_pop_items[9].textRect.horzAlign = 0;
			cod4xupdate_pop_items[9].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item9_textExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item9_textExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item9_textExp_ptr[i] = &cod4xupdate_pop_item9_textExp[i];
			}

			memset(cod4xupdate_pop_item9_textExp, 0, sizeof(cod4xupdate_pop_item9_textExp));
			cod4xupdate_pop_items[9].textExp.entries = cod4xupdate_pop_item9_textExp_ptr;
			cod4xupdate_pop_item9_textExp[0].type = 0;
			cod4xupdate_pop_item9_textExp[0].data.op = 16;
			cod4xupdate_pop_item9_textExp[1].type = 1;
			cod4xupdate_pop_item9_textExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item9_textExp[1].data.operand.internals.string = "Start automatic installation of CoD4 X and connect to server (This takes only a few seconds)";
			cod4xupdate_pop_items[9].textExp.numEntries = 2;
		}
		/* itemDef:cod4xupdate_pop_items[10] */
		{
			cod4xupdate_pop_items[10].window.rect.x = -275.000000;
			cod4xupdate_pop_items[10].window.rect.y = 22.000000;
			cod4xupdate_pop_items[10].window.rect.w = boxwidth - 12.5;
			cod4xupdate_pop_items[10].window.rect.h = 18.000000;
			cod4xupdate_pop_items[10].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[10].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[10].window.rectClient.x = 2.000000;
			cod4xupdate_pop_items[10].window.rectClient.y = boxheight - 4 - 20;
			cod4xupdate_pop_items[10].window.rectClient.w = boxwidth - 12.5;
			cod4xupdate_pop_items[10].window.rectClient.h = 18.000000;
			cod4xupdate_pop_items[10].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[10].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[10].window.style = 3;
			cod4xupdate_pop_items[10].window.borderSize = 1.000000;
			cod4xupdate_pop_items[10].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[10].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[10].window.foreColor[0] = 0.900000;
			cod4xupdate_pop_items[10].window.foreColor[1] = 0.900000;
			cod4xupdate_pop_items[10].window.foreColor[2] = 1.000000;
			cod4xupdate_pop_items[10].window.foreColor[3] = 0.070000;
			cod4xupdate_pop_items[10].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[10].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[10].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[10].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[10].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[10].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[10].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[10].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[10].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[10].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[10].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[10].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[10].type = 0;
			cod4xupdate_pop_items[10].dataType = 0;
			cod4xupdate_pop_items[10].textAlignX = 0.000000;
			cod4xupdate_pop_items[10].textAlignY = 0.000000;
			cod4xupdate_pop_items[10].textScale = 0.550000;
			cod4xupdate_pop_items[10].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[10].feeder = 0.000000;
			cod4xupdate_pop_items[10].cursorPos = 0;
			cod4xupdate_pop_items[10].textRect.x = 0.000000;
			cod4xupdate_pop_items[10].textRect.y = 0.000000;
			cod4xupdate_pop_items[10].textRect.w = 0.000000;
			cod4xupdate_pop_items[10].textRect.h = 0.000000;
			cod4xupdate_pop_items[10].textRect.horzAlign = 0;
			cod4xupdate_pop_items[10].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item10_materialExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item10_materialExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item10_materialExp_ptr[i] = &cod4xupdate_pop_item10_materialExp[i];
			}

			memset(cod4xupdate_pop_item10_materialExp, 0, sizeof(cod4xupdate_pop_item10_materialExp));
			cod4xupdate_pop_items[10].materialExp.entries = cod4xupdate_pop_item10_materialExp_ptr;
			cod4xupdate_pop_item10_materialExp[0].type = 0;
			cod4xupdate_pop_item10_materialExp[0].data.op = 16;
			cod4xupdate_pop_item10_materialExp[1].type = 1;
			cod4xupdate_pop_item10_materialExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item10_materialExp[1].data.operand.internals.string = "gradient_fadein";
			cod4xupdate_pop_items[10].materialExp.numEntries = 2;
		}
		/* itemDef:cod4xupdate_pop_items[11] */
		{
			cod4xupdate_pop_items[11].window.rect.x = 270.500000;
			cod4xupdate_pop_items[11].window.rect.y = 22.000000;
			cod4xupdate_pop_items[11].window.rect.w = 4.500000;
			cod4xupdate_pop_items[11].window.rect.h = 18.000000;
			cod4xupdate_pop_items[11].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[11].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[11].window.rectClient.x = boxwidth - 10.5;
			cod4xupdate_pop_items[11].window.rectClient.y = boxheight - 4 - 20;
			cod4xupdate_pop_items[11].window.rectClient.w = 4.500000;
			cod4xupdate_pop_items[11].window.rectClient.h = 18.000000;
			cod4xupdate_pop_items[11].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[11].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[11].window.style = 3;
			cod4xupdate_pop_items[11].window.borderSize = 1.000000;
			cod4xupdate_pop_items[11].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[11].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[11].window.foreColor[0] = 0.900000;
			cod4xupdate_pop_items[11].window.foreColor[1] = 0.900000;
			cod4xupdate_pop_items[11].window.foreColor[2] = 1.000000;
			cod4xupdate_pop_items[11].window.foreColor[3] = 0.070000;
			cod4xupdate_pop_items[11].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[11].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[11].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[11].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[11].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[11].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[11].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[11].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[11].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[11].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[11].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[11].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[11].window.background = Material_RegisterHandle("button_highlight_end", 0);
			cod4xupdate_pop_items[11].type = 0;
			cod4xupdate_pop_items[11].dataType = 0;
			cod4xupdate_pop_items[11].textAlignX = 0.000000;
			cod4xupdate_pop_items[11].textAlignY = 0.000000;
			cod4xupdate_pop_items[11].textScale = 0.550000;
			cod4xupdate_pop_items[11].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[11].feeder = 0.000000;
			cod4xupdate_pop_items[11].cursorPos = 0;
			cod4xupdate_pop_items[11].textRect.x = 0.000000;
			cod4xupdate_pop_items[11].textRect.y = 0.000000;
			cod4xupdate_pop_items[11].textRect.w = 0.000000;
			cod4xupdate_pop_items[11].textRect.h = 0.000000;
			cod4xupdate_pop_items[11].textRect.horzAlign = 0;
			cod4xupdate_pop_items[11].textRect.vertAlign = 0;
		}
		/* itemDef:cod4xupdate_pop_items[12] */
		{
			cod4xupdate_pop_items[12].window.rect.x = -275.000000;
			cod4xupdate_pop_items[12].window.rect.y = 22.000000;
			cod4xupdate_pop_items[12].window.rect.w = boxwidth - 12.5;
			cod4xupdate_pop_items[12].window.rect.h = 18.000000;
			cod4xupdate_pop_items[12].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[12].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[12].window.rectClient.x = 2.000000;
			cod4xupdate_pop_items[12].window.rectClient.y = boxheight - 4 - 20;
			cod4xupdate_pop_items[12].window.rectClient.w = boxwidth - 12.5;
			cod4xupdate_pop_items[12].window.rectClient.h = 18.000000;
			cod4xupdate_pop_items[12].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[12].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[12].window.style = 3;
			cod4xupdate_pop_items[12].window.borderSize = 2.000000;
			cod4xupdate_pop_items[12].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[12].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[12].window.foreColor[0] = 0.900000;
			cod4xupdate_pop_items[12].window.foreColor[1] = 0.950000;
			cod4xupdate_pop_items[12].window.foreColor[2] = 1.000000;
			cod4xupdate_pop_items[12].window.foreColor[3] = 0.250000;
			cod4xupdate_pop_items[12].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[12].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[12].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[12].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[12].window.borderColor[0] = 0.800000;
			cod4xupdate_pop_items[12].window.borderColor[1] = 0.950000;
			cod4xupdate_pop_items[12].window.borderColor[2] = 1.000000;
			cod4xupdate_pop_items[12].window.borderColor[3] = 0.400000;
			cod4xupdate_pop_items[12].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[12].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[12].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[12].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[12].window.background = Material_RegisterHandle("gradient_fadein", 0);
			cod4xupdate_pop_items[12].type = 0;
			cod4xupdate_pop_items[12].dataType = 0;
			cod4xupdate_pop_items[12].textAlignX = 0.000000;
			cod4xupdate_pop_items[12].textAlignY = 0.000000;
			cod4xupdate_pop_items[12].textScale = 0.550000;
			cod4xupdate_pop_items[12].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[12].feeder = 0.000000;
			cod4xupdate_pop_items[12].cursorPos = 0;
			cod4xupdate_pop_items[12].textRect.x = 0.000000;
			cod4xupdate_pop_items[12].textRect.y = 0.000000;
			cod4xupdate_pop_items[12].textRect.w = 0.000000;
			cod4xupdate_pop_items[12].textRect.h = 0.000000;
			cod4xupdate_pop_items[12].textRect.horzAlign = 0;
			cod4xupdate_pop_items[12].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item12_visibleExp_ptr[12];
			static expressionEntry_t cod4xupdate_pop_item12_visibleExp[12];

			for(i = 0; i < 12; ++i){
				cod4xupdate_pop_item12_visibleExp_ptr[i] = &cod4xupdate_pop_item12_visibleExp[i];
			}

			memset(cod4xupdate_pop_item12_visibleExp, 0, sizeof(cod4xupdate_pop_item12_visibleExp));
			cod4xupdate_pop_items[12].visibleExp.entries = cod4xupdate_pop_item12_visibleExp_ptr;
			cod4xupdate_pop_item12_visibleExp[0].type = 0;
			cod4xupdate_pop_item12_visibleExp[0].data.op = 16;
			cod4xupdate_pop_item12_visibleExp[1].type = 0;
			cod4xupdate_pop_item12_visibleExp[1].data.op = 58;
			cod4xupdate_pop_item12_visibleExp[2].type = 1;
			cod4xupdate_pop_item12_visibleExp[2].data.operand.dataType = 2;
			cod4xupdate_pop_item12_visibleExp[2].data.operand.internals.string = "ui_highlight";
			cod4xupdate_pop_item12_visibleExp[3].type = 0;
			cod4xupdate_pop_item12_visibleExp[3].data.op = 1;
			cod4xupdate_pop_item12_visibleExp[4].type = 0;
			cod4xupdate_pop_item12_visibleExp[4].data.op = 12;
			cod4xupdate_pop_item12_visibleExp[5].type = 1;
			cod4xupdate_pop_item12_visibleExp[5].data.operand.dataType = 0;
			cod4xupdate_pop_item12_visibleExp[5].data.operand.internals.intVal = 4;
			cod4xupdate_pop_item12_visibleExp[6].type = 0;
			cod4xupdate_pop_item12_visibleExp[6].data.op = 14;
			cod4xupdate_pop_item12_visibleExp[7].type = 0;
			cod4xupdate_pop_item12_visibleExp[7].data.op = 61;
			cod4xupdate_pop_item12_visibleExp[8].type = 1;
			cod4xupdate_pop_item12_visibleExp[8].data.operand.dataType = 2;
			cod4xupdate_pop_item12_visibleExp[8].data.operand.internals.string = "ui_choicegroup";
			cod4xupdate_pop_item12_visibleExp[9].type = 0;
			cod4xupdate_pop_item12_visibleExp[9].data.op = 1;
			cod4xupdate_pop_item12_visibleExp[10].type = 0;
			cod4xupdate_pop_item12_visibleExp[10].data.op = 12;
			cod4xupdate_pop_item12_visibleExp[11].type = 1;
			cod4xupdate_pop_item12_visibleExp[11].data.operand.dataType = 2;
			cod4xupdate_pop_item12_visibleExp[11].data.operand.internals.string = "popmenu";
			cod4xupdate_pop_items[12].visibleExp.numEntries = 12;
		}
		/* itemDef:cod4xupdate_pop_items[13] */
		{
			cod4xupdate_pop_items[13].window.rect.x = 270.500000;
			cod4xupdate_pop_items[13].window.rect.y = 22.000000;
			cod4xupdate_pop_items[13].window.rect.w = 4.500000;
			cod4xupdate_pop_items[13].window.rect.h = 18.000000;
			cod4xupdate_pop_items[13].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[13].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[13].window.rectClient.x = boxwidth - 10.5;
			cod4xupdate_pop_items[13].window.rectClient.y = boxheight - 4 - 20;
			cod4xupdate_pop_items[13].window.rectClient.w = 4.500000;
			cod4xupdate_pop_items[13].window.rectClient.h = 18.000000;
			cod4xupdate_pop_items[13].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[13].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[13].window.style = 3;
			cod4xupdate_pop_items[13].window.borderSize = 1.000000;
			cod4xupdate_pop_items[13].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[13].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[13].window.foreColor[0] = 0.900000;
			cod4xupdate_pop_items[13].window.foreColor[1] = 0.950000;
			cod4xupdate_pop_items[13].window.foreColor[2] = 1.000000;
			cod4xupdate_pop_items[13].window.foreColor[3] = 0.250000;
			cod4xupdate_pop_items[13].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[13].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[13].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[13].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[13].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[13].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[13].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[13].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[13].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[13].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[13].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[13].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[13].window.background = Material_RegisterHandle("button_highlight_end", 0);
			cod4xupdate_pop_items[13].type = 0;
			cod4xupdate_pop_items[13].dataType = 0;
			cod4xupdate_pop_items[13].textAlignX = 0.000000;
			cod4xupdate_pop_items[13].textAlignY = 0.000000;
			cod4xupdate_pop_items[13].textScale = 0.550000;
			cod4xupdate_pop_items[13].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[13].feeder = 0.000000;
			cod4xupdate_pop_items[13].cursorPos = 0;
			cod4xupdate_pop_items[13].textRect.x = 0.000000;
			cod4xupdate_pop_items[13].textRect.y = 0.000000;
			cod4xupdate_pop_items[13].textRect.w = 0.000000;
			cod4xupdate_pop_items[13].textRect.h = 0.000000;
			cod4xupdate_pop_items[13].textRect.horzAlign = 0;
			cod4xupdate_pop_items[13].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item13_visibleExp_ptr[12];
			static expressionEntry_t cod4xupdate_pop_item13_visibleExp[12];

			for(i = 0; i < 12; ++i){
				cod4xupdate_pop_item13_visibleExp_ptr[i] = &cod4xupdate_pop_item13_visibleExp[i];
			}

			memset(cod4xupdate_pop_item13_visibleExp, 0, sizeof(cod4xupdate_pop_item13_visibleExp));
			cod4xupdate_pop_items[13].visibleExp.entries = cod4xupdate_pop_item13_visibleExp_ptr;
			cod4xupdate_pop_item13_visibleExp[0].type = 0;
			cod4xupdate_pop_item13_visibleExp[0].data.op = 16;
			cod4xupdate_pop_item13_visibleExp[1].type = 0;
			cod4xupdate_pop_item13_visibleExp[1].data.op = 58;
			cod4xupdate_pop_item13_visibleExp[2].type = 1;
			cod4xupdate_pop_item13_visibleExp[2].data.operand.dataType = 2;
			cod4xupdate_pop_item13_visibleExp[2].data.operand.internals.string = "ui_highlight";
			cod4xupdate_pop_item13_visibleExp[3].type = 0;
			cod4xupdate_pop_item13_visibleExp[3].data.op = 1;
			cod4xupdate_pop_item13_visibleExp[4].type = 0;
			cod4xupdate_pop_item13_visibleExp[4].data.op = 12;
			cod4xupdate_pop_item13_visibleExp[5].type = 1;
			cod4xupdate_pop_item13_visibleExp[5].data.operand.dataType = 0;
			cod4xupdate_pop_item13_visibleExp[5].data.operand.internals.intVal = 4;
			cod4xupdate_pop_item13_visibleExp[6].type = 0;
			cod4xupdate_pop_item13_visibleExp[6].data.op = 14;
			cod4xupdate_pop_item13_visibleExp[7].type = 0;
			cod4xupdate_pop_item13_visibleExp[7].data.op = 61;
			cod4xupdate_pop_item13_visibleExp[8].type = 1;
			cod4xupdate_pop_item13_visibleExp[8].data.operand.dataType = 2;
			cod4xupdate_pop_item13_visibleExp[8].data.operand.internals.string = "ui_choicegroup";
			cod4xupdate_pop_item13_visibleExp[9].type = 0;
			cod4xupdate_pop_item13_visibleExp[9].data.op = 1;
			cod4xupdate_pop_item13_visibleExp[10].type = 0;
			cod4xupdate_pop_item13_visibleExp[10].data.op = 12;
			cod4xupdate_pop_item13_visibleExp[11].type = 1;
			cod4xupdate_pop_item13_visibleExp[11].data.operand.dataType = 2;
			cod4xupdate_pop_item13_visibleExp[11].data.operand.internals.string = "popmenu";
			cod4xupdate_pop_items[13].visibleExp.numEntries = 12;
		}
		/* itemDef:cod4xupdate_pop_items[14] */
		{
			cod4xupdate_pop_items[14].window.rect.x = -275.000000;
			cod4xupdate_pop_items[14].window.rect.y = 22.000000;
			cod4xupdate_pop_items[14].window.rect.w = boxwidth - 8;
			cod4xupdate_pop_items[14].window.rect.h = 18.000000;
			cod4xupdate_pop_items[14].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[14].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[14].window.rectClient.x = 2.000000;
			cod4xupdate_pop_items[14].window.rectClient.y = boxheight - 4 - 20;
			cod4xupdate_pop_items[14].window.rectClient.w = boxwidth - 8;
			cod4xupdate_pop_items[14].window.rectClient.h = 18.000000;
			cod4xupdate_pop_items[14].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[14].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[14].window.borderSize = 1.000000;
			cod4xupdate_pop_items[14].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[14].window.foreColor[0] = 1.000000;
			cod4xupdate_pop_items[14].window.foreColor[1] = 1.000000;
			cod4xupdate_pop_items[14].window.foreColor[2] = 1.000000;
			cod4xupdate_pop_items[14].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[14].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[14].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[14].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[14].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[14].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[14].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[14].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[14].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[14].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[14].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[14].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[14].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[14].type = 1;
			cod4xupdate_pop_items[14].dataType = 1;
			cod4xupdate_pop_items[14].textFont = 1;
			cod4xupdate_pop_items[14].textAlign = 10;
			cod4xupdate_pop_items[14].textAlignX = -6.000000;
			cod4xupdate_pop_items[14].textAlignY = 0.000000;
			cod4xupdate_pop_items[14].textScale = 0.400000;
			cod4xupdate_pop_items[14].textStyle = 6;
			cod4xupdate_pop_items[14].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[14].action = "\"play\" \"mouse_click\" ; \"close\" \"self\" ; ";
			cod4xupdate_pop_items[14].onFocus = "\"play\" \"mouse_submenu_over\" ; \"setLocalVarInt\" \"ui_highlight\" 4 ; \"setLocalVarString\" \"ui_choicegroup\" \"popmenu\" ; ; ";
			cod4xupdate_pop_items[14].leaveFocus = "\"setLocalVarInt\" \"ui_highlight\" 0 ; ; ";
			cod4xupdate_pop_items[14].feeder = 0.000000;
			cod4xupdate_pop_items[14].cursorPos = 0;
			cod4xupdate_pop_items[14].textRect.x = 0.000000;
			cod4xupdate_pop_items[14].textRect.y = 0.000000;
			cod4xupdate_pop_items[14].textRect.w = 0.000000;
			cod4xupdate_pop_items[14].textRect.h = 0.000000;
			cod4xupdate_pop_items[14].textRect.horzAlign = 0;
			cod4xupdate_pop_items[14].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item14_textExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item14_textExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item14_textExp_ptr[i] = &cod4xupdate_pop_item14_textExp[i];
			}

			memset(cod4xupdate_pop_item14_textExp, 0, sizeof(cod4xupdate_pop_item14_textExp));
			cod4xupdate_pop_items[14].textExp.entries = cod4xupdate_pop_item14_textExp_ptr;
			cod4xupdate_pop_item14_textExp[0].type = 0;
			cod4xupdate_pop_item14_textExp[0].data.op = 16;
			cod4xupdate_pop_item14_textExp[1].type = 1;
			cod4xupdate_pop_item14_textExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item14_textExp[1].data.operand.internals.string = "Abort";
			cod4xupdate_pop_items[14].textExp.numEntries = 2;
		}
				/* itemDef:cod4xupdate_pop_items[15] */
		{
			cod4xupdate_pop_items[15].window.rect.x = 0.000000;
			cod4xupdate_pop_items[15].window.rect.y = 0.000000;
			cod4xupdate_pop_items[15].window.rect.w = 2600.000000;
			cod4xupdate_pop_items[15].window.rect.h = 2600.000000;
			cod4xupdate_pop_items[15].window.rect.horzAlign = 1;
			cod4xupdate_pop_items[15].window.rect.vertAlign = 0;
			cod4xupdate_pop_items[15].window.rectClient.x = 0.000000;
			cod4xupdate_pop_items[15].window.rectClient.y = 0.000000;
			cod4xupdate_pop_items[15].window.rectClient.w = 2600.000000;
			cod4xupdate_pop_items[15].window.rectClient.h = 2600.000000;
			cod4xupdate_pop_items[15].window.rectClient.horzAlign = 1;
			cod4xupdate_pop_items[15].window.rectClient.vertAlign = 0;
			cod4xupdate_pop_items[15].window.borderSize = 1.000000;
			cod4xupdate_pop_items[15].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[15].window.foreColor[0] = 0.690000;
			cod4xupdate_pop_items[15].window.foreColor[1] = 0.690000;
			cod4xupdate_pop_items[15].window.foreColor[2] = 0.690000;
			cod4xupdate_pop_items[15].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[15].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[15].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[15].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[15].window.backColor[3] = 1.000000;
			cod4xupdate_pop_items[15].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[15].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[15].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[15].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[15].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[15].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[15].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[15].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[15].type = 1;
			cod4xupdate_pop_items[15].dataType = 1;
			cod4xupdate_pop_items[15].textFont = 1;
			cod4xupdate_pop_items[15].textAlign = 10;
			cod4xupdate_pop_items[15].textAlignX = 0.000000;
			cod4xupdate_pop_items[15].textAlignY = 0.000000;
			cod4xupdate_pop_items[15].textScale = 0.400000;
			cod4xupdate_pop_items[15].textStyle = 6;
			cod4xupdate_pop_items[15].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[15].action = NULL;
			cod4xupdate_pop_items[15].onFocus = NULL;
			cod4xupdate_pop_items[15].leaveFocus = NULL;
			cod4xupdate_pop_items[15].feeder = 0.000000;
			cod4xupdate_pop_items[15].cursorPos = 0;
			cod4xupdate_pop_items[15].textRect.x = 0.000000;
			cod4xupdate_pop_items[15].textRect.y = 0.000000;
			cod4xupdate_pop_items[15].textRect.w = 0.000000;
			cod4xupdate_pop_items[15].textRect.h = 0.000000;
			cod4xupdate_pop_items[15].textRect.horzAlign = 0;
			cod4xupdate_pop_items[15].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item15_textExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item15_textExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item15_textExp_ptr[i] = &cod4xupdate_pop_item15_textExp[i];
			}

			memset(cod4xupdate_pop_item15_textExp, 0, sizeof(cod4xupdate_pop_item15_textExp));
			cod4xupdate_pop_items[15].textExp.entries = cod4xupdate_pop_item15_textExp_ptr;
			cod4xupdate_pop_item15_textExp[0].type = 0;
			cod4xupdate_pop_item15_textExp[0].data.op = 16;
			cod4xupdate_pop_item15_textExp[1].type = 1;
			cod4xupdate_pop_item15_textExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item15_textExp[1].data.operand.internals.string = "";
			cod4xupdate_pop_items[15].textExp.numEntries = 2;
		}
		int j = 0;
		#define TEXT_GAPSPACE 16.0
		#define TEXT_SCALE 0.33333
		{
			int k = j + 16;
			cod4xupdate_pop_items[k].window.rect.x = 2.000000;
			cod4xupdate_pop_items[k].window.rect.y = -40.000000;
			cod4xupdate_pop_items[k].window.rect.w = 0.000000;
			cod4xupdate_pop_items[k].window.rect.h = 0.000000;
			cod4xupdate_pop_items[k].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.x = 6;
			cod4xupdate_pop_items[k].window.rectClient.y = 44.000000 + TEXT_GAPSPACE * j;
			cod4xupdate_pop_items[k].window.rectClient.w = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.h = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[k].window.borderSize = 1.000000;
			cod4xupdate_pop_items[k].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[k].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[k].window.foreColor[0] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[1] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[2] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[k].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[k].type = 1;
			cod4xupdate_pop_items[k].dataType = 1;
			cod4xupdate_pop_items[k].textFont = 1;
			cod4xupdate_pop_items[k].textAlign = ITEM_ALIGN_TOP_LEFT;
			cod4xupdate_pop_items[k].textAlignX = 0.000000;
			cod4xupdate_pop_items[k].textAlignY = 0.000000;
			cod4xupdate_pop_items[k].textScale = TEXT_SCALE;
			cod4xupdate_pop_items[k].textStyle = 6;
			cod4xupdate_pop_items[k].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[k].feeder = 0.000000;
			cod4xupdate_pop_items[k].cursorPos = 0;
			cod4xupdate_pop_items[k].textRect.x = 0.000000;
			cod4xupdate_pop_items[k].textRect.y = 0.000000;
			cod4xupdate_pop_items[k].textRect.w = 0.000000;
			cod4xupdate_pop_items[k].textRect.h = 0.000000;
			cod4xupdate_pop_items[k].textRect.horzAlign = 0;
			cod4xupdate_pop_items[k].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item_textExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item_textExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item_textExp_ptr[i] = &cod4xupdate_pop_item_textExp[i];
			}

			memset(cod4xupdate_pop_item_textExp, 0, sizeof(cod4xupdate_pop_item_textExp));
			cod4xupdate_pop_items[k].textExp.entries = cod4xupdate_pop_item_textExp_ptr;
			cod4xupdate_pop_item_textExp[0].type = 0;
			cod4xupdate_pop_item_textExp[0].data.op = 16;
			cod4xupdate_pop_item_textExp[1].type = 1;
			cod4xupdate_pop_item_textExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item_textExp[1].data.operand.internals.string = "Backup masterserver which provides an additional serverlist without fake-servers";
			cod4xupdate_pop_items[k].textExp.numEntries = 2;
		}
		++j;
		{
			int k = j + 16;
			cod4xupdate_pop_items[k].window.rect.x = 2.000000;
			cod4xupdate_pop_items[k].window.rect.y = -40.000000;
			cod4xupdate_pop_items[k].window.rect.w = 0.000000;
			cod4xupdate_pop_items[k].window.rect.h = 0.000000;
			cod4xupdate_pop_items[k].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.x = 6;
			cod4xupdate_pop_items[k].window.rectClient.y = 44.000000 + TEXT_GAPSPACE * j;
			cod4xupdate_pop_items[k].window.rectClient.w = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.h = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[k].window.borderSize = 1.000000;
			cod4xupdate_pop_items[k].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[k].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[k].window.foreColor[0] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[1] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[2] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[k].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[k].type = 1;
			cod4xupdate_pop_items[k].dataType = 1;
			cod4xupdate_pop_items[k].textFont = 1;
			cod4xupdate_pop_items[k].textAlign = ITEM_ALIGN_TOP_LEFT;
			cod4xupdate_pop_items[k].textAlignX = 0.000000;
			cod4xupdate_pop_items[k].textAlignY = 0.000000;
			cod4xupdate_pop_items[k].textScale = TEXT_SCALE;
			cod4xupdate_pop_items[k].textStyle = 6;
			cod4xupdate_pop_items[k].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[k].feeder = 0.000000;
			cod4xupdate_pop_items[k].cursorPos = 0;
			cod4xupdate_pop_items[k].textRect.x = 0.000000;
			cod4xupdate_pop_items[k].textRect.y = 0.000000;
			cod4xupdate_pop_items[k].textRect.w = 0.000000;
			cod4xupdate_pop_items[k].textRect.h = 0.000000;
			cod4xupdate_pop_items[k].textRect.horzAlign = 0;
			cod4xupdate_pop_items[k].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item_textExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item_textExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item_textExp_ptr[i] = &cod4xupdate_pop_item_textExp[i];
			}

			memset(cod4xupdate_pop_item_textExp, 0, sizeof(cod4xupdate_pop_item_textExp));
			cod4xupdate_pop_items[k].textExp.entries = cod4xupdate_pop_item_textExp_ptr;
			cod4xupdate_pop_item_textExp[0].type = 0;
			cod4xupdate_pop_item_textExp[0].data.op = 16;
			cod4xupdate_pop_item_textExp[1].type = 1;
			cod4xupdate_pop_item_textExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item_textExp[1].data.operand.internals.string = "Full backward compatibility to CoD4 v1.7 servers";
			cod4xupdate_pop_items[k].textExp.numEntries = 2;
		}
		++j;
		{
			int k = j + 16;
			cod4xupdate_pop_items[k].window.rect.x = 2.000000;
			cod4xupdate_pop_items[k].window.rect.y = -40.000000;
			cod4xupdate_pop_items[k].window.rect.w = 0.000000;
			cod4xupdate_pop_items[k].window.rect.h = 0.000000;
			cod4xupdate_pop_items[k].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.x = 6;
			cod4xupdate_pop_items[k].window.rectClient.y = 44.000000 + TEXT_GAPSPACE * j;
			cod4xupdate_pop_items[k].window.rectClient.w = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.h = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[k].window.borderSize = 1.000000;
			cod4xupdate_pop_items[k].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[k].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[k].window.foreColor[0] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[1] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[2] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[k].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[k].type = 1;
			cod4xupdate_pop_items[k].dataType = 1;
			cod4xupdate_pop_items[k].textFont = 1;
			cod4xupdate_pop_items[k].textAlign = ITEM_ALIGN_TOP_LEFT;
			cod4xupdate_pop_items[k].textAlignX = 0.000000;
			cod4xupdate_pop_items[k].textAlignY = 0.000000;
			cod4xupdate_pop_items[k].textScale = TEXT_SCALE;
			cod4xupdate_pop_items[k].textStyle = 6;
			cod4xupdate_pop_items[k].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[k].feeder = 0.000000;
			cod4xupdate_pop_items[k].cursorPos = 0;
			cod4xupdate_pop_items[k].textRect.x = 0.000000;
			cod4xupdate_pop_items[k].textRect.y = 0.000000;
			cod4xupdate_pop_items[k].textRect.w = 0.000000;
			cod4xupdate_pop_items[k].textRect.h = 0.000000;
			cod4xupdate_pop_items[k].textRect.horzAlign = 0;
			cod4xupdate_pop_items[k].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item_textExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item_textExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item_textExp_ptr[i] = &cod4xupdate_pop_item_textExp[i];
			}

			memset(cod4xupdate_pop_item_textExp, 0, sizeof(cod4xupdate_pop_item_textExp));
			cod4xupdate_pop_items[k].textExp.entries = cod4xupdate_pop_item_textExp_ptr;
			cod4xupdate_pop_item_textExp[0].type = 0;
			cod4xupdate_pop_item_textExp[0].data.op = 16;
			cod4xupdate_pop_item_textExp[1].type = 1;
			cod4xupdate_pop_item_textExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item_textExp[1].data.operand.internals.string = "Closing of known and critical security vulnerabilities";
			cod4xupdate_pop_items[k].textExp.numEntries = 2;
		}
		++j;
		{
			int k = j + 16;
			cod4xupdate_pop_items[k].window.rect.x = 2.000000;
			cod4xupdate_pop_items[k].window.rect.y = -40.000000;
			cod4xupdate_pop_items[k].window.rect.w = 0.000000;
			cod4xupdate_pop_items[k].window.rect.h = 0.000000;
			cod4xupdate_pop_items[k].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.x = 6;
			cod4xupdate_pop_items[k].window.rectClient.y = 44.000000 + TEXT_GAPSPACE * j;
			cod4xupdate_pop_items[k].window.rectClient.w = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.h = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[k].window.borderSize = 1.000000;
			cod4xupdate_pop_items[k].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[k].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[k].window.foreColor[0] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[1] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[2] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[k].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[k].type = 1;
			cod4xupdate_pop_items[k].dataType = 1;
			cod4xupdate_pop_items[k].textFont = 1;
			cod4xupdate_pop_items[k].textAlign = ITEM_ALIGN_TOP_LEFT;
			cod4xupdate_pop_items[k].textAlignX = 0.000000;
			cod4xupdate_pop_items[k].textAlignY = 0.000000;
			cod4xupdate_pop_items[k].textScale = TEXT_SCALE;
			cod4xupdate_pop_items[k].textStyle = 6;
			cod4xupdate_pop_items[k].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[k].feeder = 0.000000;
			cod4xupdate_pop_items[k].cursorPos = 0;
			cod4xupdate_pop_items[k].textRect.x = 0.000000;
			cod4xupdate_pop_items[k].textRect.y = 0.000000;
			cod4xupdate_pop_items[k].textRect.w = 0.000000;
			cod4xupdate_pop_items[k].textRect.h = 0.000000;
			cod4xupdate_pop_items[k].textRect.horzAlign = 0;
			cod4xupdate_pop_items[k].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item_textExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item_textExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item_textExp_ptr[i] = &cod4xupdate_pop_item_textExp[i];
			}

			memset(cod4xupdate_pop_item_textExp, 0, sizeof(cod4xupdate_pop_item_textExp));
			cod4xupdate_pop_items[k].textExp.entries = cod4xupdate_pop_item_textExp_ptr;
			cod4xupdate_pop_item_textExp[0].type = 0;
			cod4xupdate_pop_item_textExp[0].data.op = 16;
			cod4xupdate_pop_item_textExp[1].type = 1;
			cod4xupdate_pop_item_textExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item_textExp[1].data.operand.internals.string = "Quick joining of CoD4 X 1.8 enabled servers";
			cod4xupdate_pop_items[k].textExp.numEntries = 2;
		}
		++j;
		{
			int k = j + 16;
			cod4xupdate_pop_items[k].window.rect.x = 2.000000;
			cod4xupdate_pop_items[k].window.rect.y = -40.000000;
			cod4xupdate_pop_items[k].window.rect.w = 0.000000;
			cod4xupdate_pop_items[k].window.rect.h = 0.000000;
			cod4xupdate_pop_items[k].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.x = 6;
			cod4xupdate_pop_items[k].window.rectClient.y = 44.000000 + TEXT_GAPSPACE * j;
			cod4xupdate_pop_items[k].window.rectClient.w = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.h = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[k].window.borderSize = 1.000000;
			cod4xupdate_pop_items[k].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[k].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[k].window.foreColor[0] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[1] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[2] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[k].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[k].type = 1;
			cod4xupdate_pop_items[k].dataType = 1;
			cod4xupdate_pop_items[k].textFont = 1;
			cod4xupdate_pop_items[k].textAlign = ITEM_ALIGN_TOP_LEFT;
			cod4xupdate_pop_items[k].textAlignX = 0.000000;
			cod4xupdate_pop_items[k].textAlignY = 0.000000;
			cod4xupdate_pop_items[k].textScale = TEXT_SCALE;
			cod4xupdate_pop_items[k].textStyle = 6;
			cod4xupdate_pop_items[k].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[k].feeder = 0.000000;
			cod4xupdate_pop_items[k].cursorPos = 0;
			cod4xupdate_pop_items[k].textRect.x = 0.000000;
			cod4xupdate_pop_items[k].textRect.y = 0.000000;
			cod4xupdate_pop_items[k].textRect.w = 0.000000;
			cod4xupdate_pop_items[k].textRect.h = 0.000000;
			cod4xupdate_pop_items[k].textRect.horzAlign = 0;
			cod4xupdate_pop_items[k].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item_textExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item_textExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item_textExp_ptr[i] = &cod4xupdate_pop_item_textExp[i];
			}

			memset(cod4xupdate_pop_item_textExp, 0, sizeof(cod4xupdate_pop_item_textExp));
			cod4xupdate_pop_items[k].textExp.entries = cod4xupdate_pop_item_textExp_ptr;
			cod4xupdate_pop_item_textExp[0].type = 0;
			cod4xupdate_pop_item_textExp[0].data.op = 16;
			cod4xupdate_pop_item_textExp[1].type = 1;
			cod4xupdate_pop_item_textExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item_textExp[1].data.operand.internals.string = "Measurements against cheaters on supported servers";
			cod4xupdate_pop_items[k].textExp.numEntries = 2;
		}
		++j;
		{
			int k = j + 16;
			cod4xupdate_pop_items[k].window.rect.x = 2.000000;
			cod4xupdate_pop_items[k].window.rect.y = -40.000000;
			cod4xupdate_pop_items[k].window.rect.w = 0.000000;
			cod4xupdate_pop_items[k].window.rect.h = 0.000000;
			cod4xupdate_pop_items[k].window.rect.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rect.vertAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.x = 6;
			cod4xupdate_pop_items[k].window.rectClient.y = 44.000000 + TEXT_GAPSPACE * j;
			cod4xupdate_pop_items[k].window.rectClient.w = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.h = 0.000000;
			cod4xupdate_pop_items[k].window.rectClient.horzAlign = 2;
			cod4xupdate_pop_items[k].window.rectClient.vertAlign = 2;
			cod4xupdate_pop_items[k].window.borderSize = 1.000000;
			cod4xupdate_pop_items[k].window.staticFlags = 0x100000;
			cod4xupdate_pop_items[k].window.dynamicFlags[0] = 0x10004;
			cod4xupdate_pop_items[k].window.foreColor[0] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[1] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[2] = 0.690000;
			cod4xupdate_pop_items[k].window.foreColor[3] = 1.000000;
			cod4xupdate_pop_items[k].window.backColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.backColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.borderColor[3] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[0] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[1] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[2] = 0.000000;
			cod4xupdate_pop_items[k].window.outlineColor[3] = 0.000000;
			cod4xupdate_pop_items[k].type = 1;
			cod4xupdate_pop_items[k].dataType = 1;
			cod4xupdate_pop_items[k].textFont = 1;
			cod4xupdate_pop_items[k].textAlign = ITEM_ALIGN_TOP_LEFT;
			cod4xupdate_pop_items[k].textAlignX = 0.000000;
			cod4xupdate_pop_items[k].textAlignY = 0.000000;
			cod4xupdate_pop_items[k].textScale = TEXT_SCALE;
			cod4xupdate_pop_items[k].textStyle = 6;
			cod4xupdate_pop_items[k].parent = &cod4xupdate_pop;
			cod4xupdate_pop_items[k].feeder = 0.000000;
			cod4xupdate_pop_items[k].cursorPos = 0;
			cod4xupdate_pop_items[k].textRect.x = 0.000000;
			cod4xupdate_pop_items[k].textRect.y = 0.000000;
			cod4xupdate_pop_items[k].textRect.w = 0.000000;
			cod4xupdate_pop_items[k].textRect.h = 0.000000;
			cod4xupdate_pop_items[k].textRect.horzAlign = 0;
			cod4xupdate_pop_items[k].textRect.vertAlign = 0;

			static expressionEntry_t *cod4xupdate_pop_item_textExp_ptr[2];
			static expressionEntry_t cod4xupdate_pop_item_textExp[2];

			for(i = 0; i < 2; ++i){
				cod4xupdate_pop_item_textExp_ptr[i] = &cod4xupdate_pop_item_textExp[i];
			}

			memset(cod4xupdate_pop_item_textExp, 0, sizeof(cod4xupdate_pop_item_textExp));
			cod4xupdate_pop_items[k].textExp.entries = cod4xupdate_pop_item_textExp_ptr;
			cod4xupdate_pop_item_textExp[0].type = 0;
			cod4xupdate_pop_item_textExp[0].data.op = 16;
			cod4xupdate_pop_item_textExp[1].type = 1;
			cod4xupdate_pop_item_textExp[1].data.operand.dataType = 2;
			cod4xupdate_pop_item_textExp[1].data.operand.internals.string = "Visit http://cod4x.me for further information";
			cod4xupdate_pop_items[k].textExp.numEntries = 2;
		}
		return &cod4xupdate_pop;
	}

}
