#include "cg_shared.h"
#include "qcommon.h"
#include "r_shared.h"
#include "xassets/sounds.h"
#include "xassets/weapondef.h"
#include "client.h"
#include "snd_system.h"
#ifdef OFFICIAL
#include "private/sometricks.h"
#endif
#include "ui_shared.h"
#include "xzone.h"
#include "bg_shared.h"
#include "cg_local_mp.h"

#include <stdbool.h>

cvar_t* debug_show_viewpos;
cvar_t* cg_zoom_sensitivity_ratio;
//float CG_DrawViewAngles(ScreenPlacement *scrPlace, float y);

void __cdecl CG_DrawUpperRightDebugInfo()
{
  rg.field_21D8 = 0;
  float yoffset = cg_debugInfoCornerOffset->vec2[1];
  if ( debug_show_viewpos->boolean )
  {
    yoffset = CG_DrawViewpos(&scrPlaceFull, yoffset);
  }
  if ( cg_drawFPS->boolean )
  {
    yoffset = CG_DrawFPSDebugInfo(yoffset);
  }
  if ( cg_drawSnapshot->boolean && cg.snap != NULL)
  {
    yoffset = CG_DrawSnapshotDebugInfo(yoffset);
  }
  //yoffset = CG_DrawViewAngles(&scrPlaceFull, yoffset);


}

qboolean CG_DeployAdditionalServerCommand()
{
  const char* cmd1full = Cmd_Argv(0);
  byte cmd = cmd1full[0];

  switch(cmd){
    case 'M':
      CL_NetAddDebugData( );
      break;
    default:
      return qfalse;
  }
  return qtrue;
}

cg_t* CG_GetLocalClientGlobals(int localClientNum)
{
  assert ( (unsigned int)localClientNum >= MAX_LOCAL_CLIENTS);
  //return &cgArray[localClientNum];
  return &cg;
}

cgs_t *__cdecl CG_GetLocalClientStaticGlobals(int localClientNum)
{
  assert ( (unsigned int)localClientNum >= MAX_LOCAL_CLIENTS);

  //return &cgsArray[localClientNum];
  return &cgs;

}

/*
Not used, needed and bugged

void CG_StartAmbient(int localClientNum)
{
  char *soundname;
  int fadetime;
  snd_alias_list_t *list;
  snd_alias_t *sndalias;
  const char* cs;


  cs = CL_GetConfigString(821);
  soundname = Info_ValueForKey(cs, "n");
  fadetime = atol(Info_ValueForKey(cs, "t")) - cgArray.frametime;

  Com_Printf("^6CG_StartAmbient() Fadetime: %d\n", fadetime);

  if ( fadetime < 0 || !cgArray.frametime )
  {
    fadetime = 0;
  }

  if ( strlen(soundname) )
  {
        Com_Printf("CG_StartAmbient(): Play sound %s\n", soundname);

    list = Com_FindSoundAlias(soundname);
        if(list == NULL)
                Com_Printf("CG_StartAmbient(): list=NULL\n");
    sndalias = _Com_PickSoundAliasFromList(list);
        if(sndalias == NULL)
                Com_Printf("CG_StartAmbient(): sndalias=NULL\n");
        SND_PlayAmbientAlias(sndalias, localClientNum, fadetime);
  }
  else
  {
        Com_Printf("CG_StartAmbient(): Stoping all ambient sounds\n");
        SND_StopAmbient(fadetime);
  }
  Com_Printf("^6End of CG_StartAmbient()\n");
}

*/
#ifndef OFFICIAL
void Sec_MonitorGame(){}
#endif

void CG_EntityEvent_Internal(int localClientNum, centity_t *cent, int event);
void CG_EntityEvent(int localClientNum, centity_t *cent, int event)
{

  // const char** eventTypeStrings = (const char**)0x7294F0;
  // Com_Printf(va("CG_EntityEvent %d %d\n", cent->pose.eType, event));

  if(cent->pose.eType == 1 && event == 26) // some entity is shooting
  {

    Sec_MonitorGame();
  }
  CG_EntityEvent_Internal(localClientNum, cent, event);
}
/*
void CG_DrawDebugClientPos()
{
        float* org1, *org2, *org3;

        centity_t* cent = &cgEntities[0];
        if(cent->pose.eType == 0)
        {
                Com_Printf("No Player 0\n");
                return;
        }
        org1 = cent->pose.origin;
        org2 = cent->currentState.pos.trBase;
        org3 = cent->nextState.lerp.pos.trBase;

        Com_Printf("Pose: %f %f %f   currentState: %f %f %f nextState %f %f %f\n", org1[0], org1[1], org1[2], org2[0], org2[1], org2[2], org3[0], org3[1], org3[2]);



}*/


void __cdecl CG_RegisterItems()
{
  int i;
  signed int v4;
  int v5;
  char items[1024];

//Com_Printf("^5Client 0: %p\n", &cgEntities[0].pose.origin);
  Q_strncpyz(items,  CL_GetConfigString(2314), sizeof(items));
//  Com_Printf("ItemConfigString: %s\n", items);
  for(i = 1; i < ASSET_TYPE_WEAPON_COUNT; ++i)
  {
    v4 = items[i / 4];

    if ( v4 > '9' )
    {
        v5 = v4 - 'W';
    }else{
        v5 = v4 - '0';
    }


    if ( (1 << (i & 3)) & v5 )
    {
      //WeaponDef* weapdef = BG_GetWeaponDef(i);
  //		Com_Printf("Register Weapon index: %d Name: %s   -   %s   -   %s   -   %s\n", i, weapdef->szInternalName, weapdef->szDisplayName, weapdef->szOverlayName, weapdef->gunXModel[0] != NULL ? weapdef->gunXModel[0]->name : "none");
      CG_RegisterWeapon(i);
    }
  }

}


void __cdecl CG_DebugStar(const float *point, const float *color, int duration)
{
  CL_AddDebugStar(point, color, duration, qfalse);
}


void CG_UpdateCursorHints(cg_t *cgarg)
{
  if ( !cgarg->renderingThirdPerson )
  {
    if ( cgarg->predictedPlayerState.cursorHint )
    {
      cgarg->cursorHintTime = cgarg->time;
      cgarg->cursorHintFade = cg_hintFadeTime->integer;
      cgarg->cursorHintIcon = cgarg->predictedPlayerState.cursorHint;
      cgarg->cursorHintString = cgarg->predictedPlayerState.cursorHintString;
    }
  }
}

double CG_FadeAlpha(int timeNow, int startMsec, int totalMsec, int fadeMsec)
{
  int t;

  t = timeNow - startMsec;
  if ( fadeMsec <= 0 || totalMsec - t >= fadeMsec )
  {
    return 1.0;
  }
  else
  {
    return (double)(totalMsec - t) * 1.0 / (double)fadeMsec;
  }
}

float * CG_FadeColor(int timeNow, int startMsec, int totalMsec, int fadeMsec)
{
  static vec4_t color;

  if ( !startMsec )
  {
    return 0;
  }
  if ( timeNow - startMsec >= totalMsec )
  {
    return 0;
  }
  color[3] = CG_FadeAlpha(timeNow, startMsec, totalMsec, fadeMsec);
  color[2] = 1.0;
  color[1] = 1.0;
  color[0] = 1.0;
  return color;
}


void CG_DrawCursorHint(int localClientNum, rectDef_t *rect, Font_t *font, float scale, float *color, int style)
{
  const char *v7; // ebp@1
  double v8; // st7@2
  double v9; // st6@5
  double v11; // st5@8
  float v12; // ST44_4@9
  double v14; // st6@12
  float v15; // ST38_4@14
  const char *text; // eax@18
  const char *v17; // ebp@18
  float w; // ST54_4@20
  float v19; // ST38_4@20
  float v20; // ST14_4@20
  float v21; // ST38_4@20
  int v22; // edx@24
  WeaponDef *v23; // ecx@24
  weaponIconRatioType_t v24; // eax@25
  const char *v25; // eax@31
  const char *v26; // eax@33
  const char *v27; // eax@37
  ScreenPlacement *place; // edi@39
  int v29; // eax@43
  float tx; // ST48_4@43
  float ty; // ST34_4@43
  double v40; // st7@43
  float v41; // ST34_4@43
  float v42; // ST14_4@43
  float v43; // ST34_4@43
  float v44; // ST10_4@43
  float v45; // ST34_4@43
  float v46; // ST0C_4@43
  float v47; // ST34_4@43
  float v48; // ST48_4@44
  float v49; // ST3C_4@44
  float v50; // ST34_4@44
  float v51; // ST34_4@44
  float v52; // ST34_4@44
  float v53; // ST34_4@44
  double v54; // st7@44
  float v56; // ST34_4@46
  float v57; // ST34_4@46
  float v59; // ST34_4@46
  float posx; // [sp+8h] [bp-158h]@44
  float posy; // [sp+Ch] [bp-154h]@44
  int fontscale; // [sp+10h] [bp-150h]@44
  int v64; // [sp+14h] [bp-14Ch]@44
  float v69; // [sp+34h] [bp-12Ch]@8
  float v70; // [sp+38h] [bp-128h]@11
  float v71; // [sp+3Ch] [bp-124h]@11
  float v72; // [sp+3Ch] [bp-124h]@41
  int v73; // [sp+40h] [bp-120h]@24
  float v74; // [sp+44h] [bp-11Ch]@8
  float v75; // [sp+48h] [bp-118h]@8
  float v76; // [sp+4Ch] [bp-114h]@41
  const char *v77; // [sp+50h] [bp-110h]@8
  char a2a[260]; // [sp+5Ch] [bp-104h]@37
  int x, y;
  char weaponUseDisplayString[256];

  cg_t* cgameGlob = &cg; //CG_GetLocalClientGlobals(localClientNum);

  v7 = 0;
  if ( !cg_cursorHints->integer )
    return;
  CG_UpdateCursorHints(cgameGlob);
  v8 = 1.0;
  color[3] *= CG_FadeAlpha(cgameGlob->time, cgameGlob->cursorHintTime, cgameGlob->cursorHintFade, 100);

  v9 = 0.0;
  if ( 0.0 == color[3] )
  {
    cg.cursorHintIcon = 0;
    return;
  }
  v75 = 1.0;
  v74 = 1.0;
  v77 = 0;
  v69 = 0.0;
  v11 = 0.5;
  if ( cg_cursorHints->integer == 3 )
  {
    v12 = (double)cg.time / 150.0;
    color[3] = (sin(v12) * 0.5 + 0.5) * color[3];
    v9 = 0.0;
    v11 = 0.5;
    v8 = 1.0;
  }

  if ( cg_cursorHints->integer < 3 )
  {
    v14 = v11;
    if ( cg_cursorHints->integer == 2 )
    {
      v70 = (double)(cg.cursorHintTime % 1000) / 100.0;
    }
    else
    {
      v15 = (double)cg.time / 150.0;
      v70 = (sin(v15) * 0.5 + 0.5) * 10.0;
      v14 = 0.5;
      v8 = 1.0;
    }
    v71 = v14 * v70;
  }
  else
  {
    v71 = v9;
    v70 = v9;
  }
  if ( cg.cursorHintIcon == 1 )
  {
    if ( cg.cursorHintString >= 0 )
    {
      text = CG_GetUseString(localClientNum);
      v17 = text;
      if ( text )
      {
        if ( *text )
        {
          w = (double)UI_TextWidth(text, 0, font, scale);
          v19 = (double)UI_TextHeight(font, scale) * 0.5 + rect->y;
          v20 = v19;
          v21 = (w + v70) * -0.5;
          UI_DrawText((ScreenPlacement *)&scrPlaceView[localClientNum], v17, 0x7FFFFFFF, font, v21, v20, rect->horzAlign, rect->vertAlign, scale, color, style);
        }
      }
    }
    return;
  }

  if(cg.cursorHintIcon >= 4 + ASSET_TYPE_WEAPON_COUNT)
  {
	  return;
  }

  Material* icon;
  if(cg.cursorHintIcon < 5 )
  {
	icon= cgMedia.hintMaterials[cg.cursorHintIcon];
  }else{
	icon= weaponHudIcons[cg.cursorHintIcon -4];
  }

  if ( icon == NULL)
  {
    return;
  }

  if ( cg.cursorHintIcon < 5 || cg.cursorHintIcon > 132 )
  {
    if ( cg.cursorHintString < 0 )
    {
      if ( cg.cursorHintIcon != 3 )
        goto LABEL_39;
      GetKeyBindingLocalizedString(localClientNum, "+activate", a2a, 0);
      v27 = UI_SafeTranslateString("PLATFORM_PICKUPHEALTH");
      v26 = UI_ReplaceConversionString(v27, a2a);
    }
    else
    {
      v26 = CG_GetUseString(localClientNum);
    }
  }
  else
  {
    v22 = cg.cursorHintIcon - 4;
    v23 = bg_weaponDefs[cg.cursorHintIcon - 4];
    v73 = cg.cursorHintIcon - 4;
    if ( v23->hudIcon )
    {
      v24 = v23->hudIconRatio;
      if ( v24 )
      {
        if ( v24 == 1 )
        {
          v75 = 2.0;
          v69 = -0.5 * rect->w;
          v74 = v8;
        }
        else
        {
          v75 = 2.0;
          v69 = -0.5 * rect->w;
          v74 = 0.5;
        }
      }
    }
    if ( v23->weapClass == WEAPCLASS_TURRET )
    {
      if ( cg.cursorHintString >= 0 )
      {
        v25 = CG_GetUseString(localClientNum);
        v22 = v73;
        v7 = v25;
      }
      v77 = cg_weaponsArray[v22].translatedDisplayName;
      goto LABEL_39;
    }
    v26 = CG_GetWeaponUseString(localClientNum, &v77);
  }
  v7 = v26;
LABEL_39:
  place = (ScreenPlacement *)&scrPlaceView[localClientNum];
  if ( v7 && *v7 )
  {
    v76 = (double)UI_TextWidth(v7, 0, font, scale);
	v72 = (double)UI_TextHeight(font, scale);
    if ( v77 && cg_weaponHintsCoD1Style->boolean )
    {
	  Com_sprintf(weaponUseDisplayString, sizeof(weaponUseDisplayString), "%s %s", v7, v77);
	  v29 = UI_TextWidth(weaponUseDisplayString, 0, font, scale);
      tx = ((double)v29) * -0.5;
      ty = 0.5 * v72 + rect->y;
      UI_DrawText(place, weaponUseDisplayString, 0x7FFFFFFF, font, tx, ty, rect->horzAlign, rect->vertAlign, scale, color, style);
      v40 = rect->w * v75 + v70;
      v41 = v70 + v74 * rect->h;
      v42 = v41;
      v43 = v40;
      v44 = v43;
      v45 = v72 * 1.5 + rect->y - rect->h * 0.5 * v74;
      v46 = v45;
      v47 = v40 * -0.5;
      UI_DrawHandlePic(place, v47, v46, v44, v42, rect->horzAlign, rect->vertAlign, color, icon);
      return;
    }
    v48 = (v75 * rect->w + v70 + v76) * -0.5;
    v49 = rect->y - rect->h * 0.5 * v74;
    v50 = 0.5 * v72 + rect->y;
    UI_DrawText(place, v7, 0x7FFFFFFF, font, v48, v50, rect->horzAlign, rect->vertAlign, scale, color, style);
    v64 = rect->vertAlign;
    v51 = v74 * rect->h + v70;
    fontscale = rect->horzAlign;
    posy = v51;
    v52 = v70 + v75 * rect->w;
    posx = v52;
    y = v49;
    v53 = v48 + v76;
    v54 = v53;
  }
  else
  {

    v64 = rect->vertAlign;
    fontscale = rect->horzAlign;
    v56 = rect->h * v74 + v70;
    posy = v56;
    v57 = v70 + v75 * rect->w;
    posx = v57;
    y = rect->y - v74 * v71;
    v59 = rect->x - (v71 + rect->w + v69) * 0.5;
    v54 = v59;
  }
  x = v54;
  UI_DrawHandlePic(place, x, y, posx, posy, fontscale, v64, color, icon);
}



const char * CG_GetUseString(int localClientNum)
{
	char translatedString[256];
	const char* cs;

	cs = CL_GetConfigString(cg.cursorHintString + 277);
	if ( !cs || !*cs )
	{
		  return NULL;
	}
    if ( !GetKeyBindingLocalizedString(localClientNum, "+activate", translatedString, 0) )
    {
      Q_strncpyz(translatedString, UI_SafeTranslateString("KEY_USE"), sizeof(translatedString));
    }
    return UI_ReplaceConversionString(SEH_LocalizeTextMessage((char*)cs, "Hint String", 0), translatedString);

}

void CG_SetEquippedOffHand(/*int localClientNum@<ecx>,*/ int event)
{
  cg.equippedOffHand = event;
  if ( cg.ammoFadeTime < cg.time )
  {
    cg.ammoFadeTime = cg.time;
    Menus_ShowByName(&uiMem.uiInfo.uiDC, "weaponinfo");
    Menus_ShowByName(&uiMem.uiInfo.uiDC, "weaponinfo_lowdef");

  }
  if ( cg.offhandFadeTime < cg.time )
  {
    cg.offhandFadeTime = cg.time;
    Menus_ShowByName(&uiMem.uiInfo.uiDC, "offhandinfo");
  }
}


void CG_ClearCameraShakes(/*int localclient*/)
{
  Com_Memset(s_cameraShakeSet.shakes, 0, sizeof(s_cameraShakeSet.shakes));
}

void CG_Respawn(/*int localClientNum*/)
{
  cg.predictedPlayerState = cg.snap->ps;
  cg.swayViewAngles[0] = 0.0;
  cg.swayViewAngles[1] = 0.0;
  cg.swayViewAngles[2] = 0.0;
  cg.swayAngles[0] = 0.0;
  cg.swayAngles[1] = 0.0;
  cg.swayAngles[2] = 0.0;
  cg.swayOffset[0] = 0.0;
  cg.swayOffset[1] = 0.0;
  cg.weaponSelect = cg.predictedPlayerState.weapon;
  cg.swayOffset[2] = 0.0;
  cg.weaponSelectTime = cg.time;
  cg.cursorHintIcon = 0;
  cg.equippedOffHand = cg.predictedPlayerState.offHandIndex;
  cg.cursorHintTime = 0;

  CG_ClearAngles();

  cg.proneBlockedEndTime = 0;
  cg.xyspeed = 0.0;
  Com_Memset(&cg.playerEntity, 0, sizeof(cg.playerEntity));
  cg.v_dmg_pitch = 0.0;
  cg.v_dmg_roll = 0.0;
  cg.vGunOffset[0] = 0.0;
  cg.vGunOffset[1] = 0.0;
  cg.damageTime = 0;
  cg.vGunOffset[2] = 0.0;
  cg.vGunSpeed[0] = 0.0;
  cg.vGunSpeed[1] = 0.0;
  cg.vGunSpeed[2] = 0.0;
  Com_Memset(cg.viewDamage, 0, sizeof(cg.viewDamage));
  CG_ClearCameraShakes();
  cg.predictedError[0] = 0.0;
  cg.predictedError[1] = 0.0;
  cg.predictedError[2] = 0.0;
  CL_SetStance( 0 );
  cl.usingAds = 0;
  CG_SetEquippedOffHand(/*localClientNum*/cg.predictedPlayerState.offHandIndex);
  cg.healthOverlayHurt = 0;
  cg.healthOverlayToAlpha = hud_healthOverlay_phaseEnd_toAlpha->floatval;
  cg.healthOverlayPulseDuration = 0;
  cg.healthOverlayPulsePhase = 0;
  cg.healthOverlayOldHealth = 1.0;
  cg.healthOverlayPulseIndex = 0;
  cg.weapIdleTime = 0;
  cg.holdBreathInTime = 0;
  cg.playerEntity.fLastIdleFactor = 0.0;
  cg.holdBreathDelay = 0;
  cg.holdBreathTime = -1;
  cg.heightToCeiling = 3.4028235e38;
  cg.holdBreathFrac = 0.0;
}

gameClientDataX_t gameClientDataX[64];

const char* CG_GetUsernameX(int clnum)
{
	return gameClientDataX[clnum].name;
}

const char* CG_GetClantag(int clnum)
{
	return gameClientDataX[clnum].clantag;
}

void CG_SetClientDataX(int clnum, const char* name, const char* clantag)
{
	Q_strncpyz(gameClientDataX[clnum].name, name, sizeof(gameClientDataX[clnum].name));
	Q_strncpyz(gameClientDataX[clnum].clantag, clantag, sizeof(gameClientDataX[clnum].clantag));

}

int CG_GetClientNum()
{
	return cg.clientNum;
}

void sub_42D650(float *vec1, float *vec2, refdef_t *ref)
{
  vec3_t vec;

  vec[0] = *vec1 - ref->vieworg[0];
  vec[1] = vec1[1] - ref->vieworg[1];
  vec[2] = vec1[2] - ref->vieworg[2];
  vec2[0] = ref->viewaxis[0][2] * vec[2] + ref->viewaxis[0][0] * vec[0] + ref->viewaxis[0][1] * vec[1];
  vec2[1] = ref->viewaxis[1][1] * vec[1] + ref->viewaxis[1][0] * vec[0] + ref->viewaxis[1][2] * vec[2];
  vec2[2] = vec[0] * ref->viewaxis[2][0] + vec[1] * ref->viewaxis[2][1] + vec[2] * ref->viewaxis[2][2];
}


char CG_CalcNamePosition(int localClientNum, float *origin, float *xOut, float *yOut)
{
  float v5;
  float v7;
  float v8;
  vec3_t v9;

  sub_42D650(origin, v9, &cg.refdef);
  v5 = v9[0];
  if ( v9[0] >= 0.0 )
  {
    v7 = v9[1] / cg.refdef.tanHalfFovX;
    *xOut = scrPlaceView[localClientNum].realViewportSize[0] * 0.5 * (1.0 - v7 / v5);
    v8 = v9[2] / cg.refdef.tanHalfFovY;
    *yOut = (1.0 - v8 / v5) * (0.5 * scrPlaceView[localClientNum].realViewportSize[1]);
    return qtrue;
  }
  return qfalse;
}

qboolean CG_GetClientClantag(int index, char *buf, int size)
{
	const char* tag;

	buf[0] = '\0';

	if ( !cg.bgs.clientinfo[index].infoValid )
	{
		return qfalse;
	}

	if(Com_IsLegacyServer())
	{
		return qfalse;
	}
	tag = CG_GetClantag(index);
	if(tag[0] == '\0')
	{
		return qfalse;
	}
	Q_strncpyz(buf, tag, size);
	return qtrue;
}

qboolean CG_GetClientName(int index, char *buf, int size)
{
	const char* name;

	*buf = 0;

	if ( !cg.bgs.clientinfo[index].infoValid )
	{
		return qfalse;
	}

	if(Com_IsLegacyServer())
	{
		name = cg.bgs.clientinfo[index].name;
	}else{
		name = CG_GetUsernameX(index);
	}

    Q_strncpyz(buf, name, size);
    return qtrue;
}

qboolean CG_GetClientNameIW(int index, char *buf)
{
	const char* name;

	*buf = 0;

	if ( !cg.bgs.clientinfo[index].infoValid )
	{
		return qfalse;
	}

	if(Com_IsLegacyServer())
	{
		name = cg.bgs.clientinfo[index].name;
	}else{
		name = CG_GetUsernameX(index);
	}

    Q_strncpyz(buf, name, 38);
    return qtrue;

}

void CG_HudGetClientName(char* hudname, int clientnum, int size)
{
  char name[64];
  char clan[64];

  CG_GetClientName(clientnum, name, sizeof(name));
  CG_GetClientClantag(clientnum, clan, sizeof(clan));

  Com_sprintf(hudname, size, "%s %s", clan, name);

}


void CG_DrawOverheadNames(int localClientNum, centity_t *cent, float alpha)
{
	Font_t* font;
	DObj_t* dobj;
	vec3_t org;
	char overheadtext[64];
	char clantag[32];
	vec4_t color, color2;
	float x, y, xn;
	float sqrdist, v30, v29, var_58;
	Material* material;
	float picy;
	const char* dtext;
	float normalizedScale;
	float prescale;
	float width;
	float tagwidth;
	vec4_t clantagcolor;
	int i;

	if ( alpha < 0.001 )
	{
		return;
	}

	font = UI_GetFontHandle(0, cg_overheadNamesFont->integer, 1.0);
    dobj = Com_GetClientDObj(cent->nextState.number);
    if ( dobj && CG_DObjGetWorldTagPos(&cent->pose, dobj, scr_const.j_head_dup, org) )
    {
      org[2] = org[2] + 10.0;
    }
    else
    {
      org[0] = cent->pose.origin[0];
      org[1] = cent->pose.origin[1];
      org[2] = cent->pose.origin[2] + 82.0;
    }

	if ( !CG_GetClientName(cent->nextState.clientNum, overheadtext, sizeof(overheadtext)) )
    {
		Q_strncpyz(overheadtext, "?", sizeof(overheadtext));
	}
	CG_GetClientClantag(cent->nextState.clientNum, clantag, sizeof(clantag));

	CG_RelativeTeamColor(cent->nextState.clientNum, "g_TeamColor", color);
	color[3] = alpha;

    if ( !CG_CalcNamePosition(0, org, &x, &y) )
    {
		return;
	}

    sqrdist = DistanceSquared(cg.refdef.vieworg, org);
    if ( cg_overheadNamesNearDist->floatval * cg_overheadNamesNearDist->floatval <= sqrdist )
    {
        if ( cg_overheadNamesFarDist->floatval * cg_overheadNamesFarDist->floatval >= sqrdist )
        {
            v30 = (sqrt(sqrdist) - cg_overheadNamesNearDist->floatval) / (cg_overheadNamesFarDist->floatval - cg_overheadNamesNearDist->floatval);
            prescale = cg_overheadNamesFarScale->floatval * v30 + 1.0 - v30;
        }
        else
        {
            prescale = cg_overheadNamesFarScale->floatval;
        }
    }
    else
    {
        prescale = 1.0;
    }

	normalizedScale = R_NormalizedTextScale(font, prescale * cg_overheadNamesSize->floatval);
	width = R_TextWidth(overheadtext, 32, font);
    if(clantag[0])
	{
		tagwidth = R_TextWidth(clantag, 32, font);
		width += tagwidth + 6.5;
	}
	width *= normalizedScale;
	x = floor(x - 0.5 * width + 0.5);
    y = floor(y + 0.5);

	Cvar_GetUnpackedColor(cg_overheadNamesGlow, color2);

	color2[3] *= alpha;
	xn = x;
	if(clantag[0])
	{
		for(i = 0; i < 4; ++i)
		{
			clantagcolor[i] = 1.25 * color[i];
			if(clantagcolor[i] > 1.0)
			{
				clantagcolor[i] = 1.0;
			}
		}
		CL_DrawTextPhysicalWithEffects(clantag, 32, font, xn, y, normalizedScale, normalizedScale, clantagcolor, 3, color2, 0, 0, 0, 0, 0, 0);
		xn += (tagwidth + 6.5) * normalizedScale;
	}
    CL_DrawTextPhysicalWithEffects(overheadtext, 32, font, xn, y, normalizedScale, normalizedScale, color, 3, color2, 0, 0, 0, 0, 0, 0);

    CL_GetRankIcon(cg.bgs.clientinfo[cent->nextState.clientNum].rank, cg.bgs.clientinfo[cent->nextState.clientNum].unk1, &material);

	if(!material)
	{
		return;
	}

    v29 = R_TextHeight(font) * normalizedScale;

	var_58 = cg_overheadIconSize->floatval * normalizedScale * R_TextHeight(font);

    normalizedScale = R_NormalizedTextScale(font, prescale * cg_overheadRankSize->floatval);

    dtext = CL_GetRankData(cg.bgs.clientinfo[cent->nextState.clientNum].rank);
    x -= (prescale + prescale + var_58 + R_TextWidth(dtext, 32, font) * normalizedScale);
    color[0] = 1.0;
    color[1] = 1.0;
    color[2] = 1.0;
    color[3] = alpha;

    picy = y - (var_58 + v29) * 0.5;

    CL_DrawStretchPicPhysical(x, picy, var_58, var_58, 0.0, 0.0, 1.0, 1.0, color, material);
    x += var_58;

    y = R_TextHeight(font) * 0.25 + y;
    CL_DrawTextPhysical(dtext, 3, font, x, y, normalizedScale, normalizedScale, color, 3);


}



qboolean CG_DObjGetWorldTagPos(cpose_t *pose, DObj_t* obj, uint32_t tag, float *pos)
{
  char index;
  DObjAnimMat *matBase;
  DObjAnimMat *mat;

  index = -2;

  if ( !DObjGetBoneIndex(obj, tag, &index) )
  {
    return 0;
  }

  CG_DObjGetLocalBoneMatrix(pose, obj, index);

  matBase = DObjGetRotTransArray(obj);

  if ( matBase && (mat = &matBase[(unsigned int)index]) != 0 )
  {
	pos[0] = mat->trans[0] + cg.refdef.viewOffset[0];
    pos[1] = mat->trans[1] + cg.refdef.viewOffset[1];
    pos[2] = mat->trans[2] + cg.refdef.viewOffset[2];
    return 1;
  }
  return 0;
}


void CG_RelativeTeamColor(int clientNum, const char *prefix, float *color)
{
  const char *v3;
  char string[256];

  team_t team = cg.bgs.clientinfo[clientNum].team;

  if(team == 3)
  {
	v3 = "%s_Spectator";
  }else if(clientNum == cg.clientNum || (team && cg.bgs.clientinfo[cg.clientNum].team == team)){
	v3 = "%s_MyTeam";
  }else{
	v3 = "%s_EnemyTeam";
  }

  Com_sprintf(string, sizeof(string), v3, prefix);
  Cvar_GetUnpackedColorByName(string, color);

}


qboolean __cdecl CG_DrawFollow(int localClientNum)
{

	char *locstring;
	char clantag[64];
	Font_t *font;
	float scale;
	float w, tw, nw;
	char string[38];
	qboolean hastag, hasname;

	if ( !(cg.nextSnap->ps.otherFlags & 2) || cg.inKillCam )
	{
		return qfalse;
	}

	hastag = CG_GetClientClantag(cg.nextSnap->ps.clientNum, clantag, sizeof(clantag));
	hasname = CG_GetClientName(cg.nextSnap->ps.clientNum, string, sizeof(string));
	if ( !hasname )
	{
        Com_sprintf(string, sizeof(string), "?");
	}
	locstring = SEH_LocalizeTextMessage("CGAME_FOLLOWING\x15", "spectator follow string", 0);
    font = uiMem.font2;
	scale = 0.33333334;
	w = UI_TextWidth(locstring, 0, font, scale) * -0.5;
    UI_DrawText(&scrPlaceView[localClientNum], locstring, 0x7FFFFFFF, font, w, 20.0, 7, 1, scale, colorWhite, 3);
	if(hastag)
	{
		tw = UI_TextWidth(clantag, 0, font, scale);
		nw = UI_TextWidth(string, 0, font, scale);
		w = (nw + tw + 7.0) * -0.5;
		UI_DrawText(&scrPlaceView[localClientNum], clantag, 0x7FFFFFFF, font, w, 36.0, 7, 1, scale, colorLtGrey, 3);
		UI_DrawText(&scrPlaceView[localClientNum], string, 0x7FFFFFFF, font, w + tw + 7.0, 36.0, 7, 1, scale, colorWhite, 3);
	}else{
		w = UI_TextWidth(string, 0, font, scale) * -0.5;
		UI_DrawText(&scrPlaceView[localClientNum], string, 0x7FFFFFFF, font, w, 36.0, 7, 1, scale, colorWhite, 3);
	}
	return qtrue;
}

float CG_DrawViewposoffset = 50.0;

void CG_LogViewpos(const char* s)
{
  static fileHandle_t fh = 0;
  static vec3_t lastpos = {0, 0, 0};
  static int lastchanged = 0;

  char logstring[256];

  if(fh == -1)
  {
    return;
  }
  if(fh == 0)
  {
    if(cg.refdef.vieworg[0] == lastpos[0] && cg.refdef.vieworg[1] == lastpos[1] && cg.refdef.vieworg[2] == lastpos[2])
    {
      return;
    }
    fh = FS_FOpenFileWrite("view_pos.log");
  }else{
    int now = Sys_Milliseconds();

    if(cg.refdef.vieworg[0] == lastpos[0] && cg.refdef.vieworg[1] == lastpos[1] && cg.refdef.vieworg[2] == lastpos[2])
    {
      if(now - lastchanged > 2000)
      {
        FS_FCloseFile(fh);
        fh = 0;
        return;
      }
    }else{
      lastchanged = now;
    }
  }
  VectorCopy(cg.refdef.vieworg,lastpos);
  if(fh < 1)
  {
    fh = -1;
    return;
  }
  int l = Com_sprintf(logstring, sizeof(logstring), "%s\n", s);
  FS_Write(logstring, l, fh);

}


float CG_DrawViewpos(ScreenPlacement *scrPlace, float y)
{
  char levelname[64];
  int len;

  Q_strncpyz(levelname, cgs.mapname +8, sizeof(levelname));
  len = strlen(levelname);

  if(len > 8)
  {
	  levelname[len -7] = '\0';
  }

  float farRight = (float)((float)(scrPlace->virtualViewableMax[0] - scrPlace->virtualViewableMin[0]) + cg_debugInfoCornerOffset->value) - CG_DrawViewposoffset;
  const char* s = va("%s (%.1f %.1f %.1f) %.1f %.1f", levelname, cg.refdef.vieworg[0], cg.refdef.vieworg[1], cg.refdef.vieworg[2], cg.refdefViewAngles[1], cg.refdefViewAngles[0]);
  y = CG_CornerDebugPrint(scrPlace, farRight, y, 0.0, s, "", colorWhite) + y;
//  CG_LogViewpos(s);

  return y;
}

/*
float CG_DrawViewAngles(ScreenPlacement *scrPlace, float y)
{
  clientActive_t *client;
  float steerYaw;

  client = CL_GetLocalClientGlobals(0);
  int vehEntNum = cg.bgs.clientinfo[cg.predictedPlayerState.clientNum].attachedVehEntNum;
  if(vehEntNum != 1023)
  {
    centity_t *cVehEnt = &cgEntities[vehEntNum];
    if ( cVehEnt->nextValid )
    {
      if(cVehEnt->nextState.eType == ET_VEHICLE)
      {
        steerYaw = cVehEnt->pose.angles[YAW];
      }
    }
  }

  float farRight = (float)((float)(scrPlace->virtualViewableMax[0] - scrPlace->virtualViewableMin[0]) + cg_debugInfoCornerOffset->value) - CG_DrawViewposoffset;
  const char* s = va("%.1f %.1f %.1f vy%.1f t%.1f", client->viewangles[0], client->viewangles[1], client->viewangles[2], cl.vehicleViewYaw, steerYaw);
  y = CG_CornerDebugPrint(scrPlace, farRight, y, 0.0, s, "", colorWhite) + y;
//  CG_LogViewpos(s);

  return y;
}
*/

int __cdecl CG_DrawDevString(ScreenPlacement *scrPlace, float x, float y, float xScale, float yScale, const char *s, const float *color, int align, struct Font_s *font)
{
  int step;

  assert((align & CG_ALIGN_X) == CG_ALIGN_LEFT || (align & CG_ALIGN_X) == CG_ALIGN_RIGHT || (align & CG_ALIGN_X) == CG_ALIGN_CENTER);
  if ( (align & CG_ALIGN_X) == CG_ALIGN_RIGHT )
  {
    x = x - (float)R_TextWidth(s, 0, font) * xScale;
  }
  else if ( (align & CG_ALIGN_X) == CG_ALIGN_CENTER )
  {
    x = x - (float)((float)((float)R_TextWidth(s, 0, font) * xScale) * 0.5);
  }

  assert((align & CG_ALIGN_Y) == CG_ALIGN_TOP || (align & CG_ALIGN_Y) == CG_ALIGN_BOTTOM || (align & CG_ALIGN_Y) == CG_ALIGN_MIDDLE);

  step = R_TextHeight(font);
  if ( (align & CG_ALIGN_Y) == CG_ALIGN_TOP )
  {
    y = y + (float)step * yScale;
  }
  else if ( (align & CG_ALIGN_Y) == CG_ALIGN_MIDDLE )
  {
    y = ((float)step * yScale) * 0.5 + y;
  }
  CL_DrawText(scrPlace, s, 0x7FFFFFFF, font, x, y, 1, 1, xScale, yScale, color, 0);
  return step;
}

double __cdecl CG_CornerDebugPrint(ScreenPlacement *sP, float posX, float posY, float labelWidth, const char *text, const char *label, const float *color)
{
  int yDelta;
  int textDelta;
  int labelDelta;

  if ( cg_drawFPSLabels->boolean )
  {
    textDelta = CG_DrawDevString(sP, posX - labelWidth, posY, 1.0, 1.1, text, color, 6, cgMedia.smallDevFont);
    labelDelta = CG_DrawDevString(sP, posX - labelWidth, posY, 1.0, 1.1, label, colorWhiteFaded, 5, cgMedia.smallDevFont);
    if ( textDelta < labelDelta )
    {
      yDelta = labelDelta;
    }
    else
    {
      yDelta = textDelta;
    }
  }
  else
  {
    yDelta = CG_DrawDevString(sP, posX, posY, 1.0, 1.1, text, color, 6, cgMedia.smallDevFont);
  }
  return (double)yDelta * 0.75;
}

void CG_RegisterUserCvars()
{
  debug_show_viewpos = Cvar_RegisterBool("debug_show_viewpos", 0, 0, "Draw current player view position");
  cg_zoom_sensitivity_ratio = Cvar_RegisterFloat("zoom_sensitivity_ratio", 1.0, 0.0, 10.0, CVAR_ARCHIVE, "Scales the sensitivity when in ADS mode. Defaults to 1.0. A value of 0.0 does disable zoom sensitivity completely.");
}

void __cdecl CG_GameMessage(int localClientNum, const char *msg)
{
  CL_ConsolePrint(localClientNum, CON_CHANNEL_GAMENOTIFY, msg, 0, cg_gameMessageWidth->integer, 0);
}


void CG_DrawActive()
{

  float FOVSensitivityScale = cg.zoomSensitivity;

  if(cg.playerEntity.bPositionToADS == 0)
  {
    FOVSensitivityScale *= cg_zoom_sensitivity_ratio->floatval;
  }


  if ( cg.shellshock.sensitivity != 0.0 )
  {
	  FOVSensitivityScale *= cg.shellshock.sensitivity;
  }
  cl.cgameFOVSensitivityScale = FOVSensitivityScale;
  cl.cgameExtraButtons |= cg.extraButtons;
  cl.cgameUserCmdWeapon = cg.weaponSelect;
  cl.cgameUserCmdOffHandIndex = cg.equippedOffHand;
  cg.extraButtons = 0;
  CG_UpdateViewAngles();
  cls.debugRenderPos[0] = cg.refdef.vieworg[0];
  cls.debugRenderPos[1] = cg.refdef.vieworg[1];
  cls.debugRenderPos[2] = cg.refdef.vieworg[2];
  R_RenderScene(&cg.refdef);
}



void CG_InitConsoleCommandsPatched()
{
  Cmd_AddCommand("mr", NULL);
  Cmd_AddCommand("kill", NULL);
  Cmd_AddCommand("give", NULL);
  Cmd_AddCommand("take", NULL);
  Cmd_AddCommand("god", NULL);
  Cmd_AddCommand("demigod", NULL);
  Cmd_AddCommand("notarget", NULL);
  Cmd_AddCommand("noclip", NULL);
  Cmd_AddCommand("ufo", NULL);
  Cmd_AddCommand("levelshot", NULL);
  Cmd_AddCommand("setviewpos", NULL);
  Cmd_AddCommand("jumptonode", NULL);
  Cmd_AddCommand("stats", NULL);
  Cmd_AddCommand("say", NULL);
  Cmd_AddCommand("say_team", NULL);
  Cmd_AddCommand("team", NULL);
  Cmd_AddCommand("follow", NULL);
  Cmd_AddCommand("callvote", NULL);
  Cmd_AddCommand("vote", CG_Vote_f);
  Cmd_AddCommand("follownext", NULL);
  Cmd_AddCommand("followprev", NULL);
  Cmd_AddCommand("printentities", NULL);
  Cmd_AddCommand("muteplayer", NULL);
  Cmd_AddCommand("unmuteplayer", NULL);
}


void CG_UpdateFov(float fov)
{
  float v3;
  float v4;

  v3 = tan(fov * (M_PI / 360));
  v4 = 0.75 * v3;
  cg.refdef.tanHalfFovX = cgs.viewAspect * v4;
  cg.refdef.tanHalfFovY = v4;
  cg.zoomSensitivity = v3 / (1/(M_PI / 2));
}


void CG_CalcFov(){
  float fov;

  fov = CG_GetViewFov();
  CG_UpdateFov(fov);
}

void CG_VehSphereCoordsToPos(float sphereDistance, float sphereYaw, float sphereAltitude, float *result)
{
  double sin_alt;
  double sin_yaw;
  double cos_alt;
  float alt_rad;
  float yaw_rad;
  double cos_yaw;

  alt_rad = (90.0 - sphereAltitude) * (M_PI/180);
  sincos(alt_rad, &sin_alt, &cos_alt);

  yaw_rad = (sphereYaw - 90.0) * (M_PI/180);
  sincos(yaw_rad, &sin_yaw, &cos_yaw);

  result[0] = cos_yaw * sphereDistance * sin_alt;
  result[1] = sin_yaw * sphereDistance * sin_alt;
  result[2] = sphereDistance * cos_alt;
}

static float oldVehForwardYaw;
static float oldClientViewYaw;

void CG_VehUseInitialize()
{
  clientActive_t *client = CL_GetLocalClientGlobals(0);

  int vehEntNum = cg.bgs.clientinfo[cg.predictedPlayerState.clientNum].attachedVehEntNum;

  centity_t *cVehEnt = &cgEntities[vehEntNum];

  client->vehicleViewYaw = cVehEnt->pose.angles[YAW] - 90.0f;
  client->viewangles[1] = client->vehicleViewYaw;
  oldVehForwardYaw = client->vehicleViewYaw;
  oldClientViewYaw = client->viewangles[1];
}

void CG_VehUseDeinitialize()
{

}

void CalcViewValuesVehicleDriver()
{
  double v1;
  float normalizedPitch;
  vec3_t newLookDir;
  vec3_t lookAtPos;

  clientActive_t *client = CL_GetLocalClientGlobals(0);

  lookAtPos[2] = cg.predictedPlayerState.origin[2] + 55.0;
  lookAtPos[0] = cg.predictedPlayerState.origin[0] + 0.0;
  lookAtPos[1] = cg.predictedPlayerState.origin[1] + 0.0;
  normalizedPitch = client->vehicleViewPitch / 360;


  //client->vehicleViewYaw = client->viewangles[1];
  int vehEntNum = cg.bgs.clientinfo[cg.predictedPlayerState.clientNum].attachedVehEntNum;
  centity_t *cVehEnt = &cgEntities[vehEntNum];
  if ( cVehEnt->nextValid && cVehEnt->nextState.eType == ET_VEHICLE)
  {
    float vehForwardYaw = cVehEnt->pose.angles[YAW] - 90.0f;

    float vehYawDelta = vehForwardYaw - oldVehForwardYaw;
    float clientYawDelta = client->viewangles[1] - oldClientViewYaw;


    client->vehicleViewYaw += (vehYawDelta + clientYawDelta);
/*
    float viewforwardoffset = client->vehicleViewYaw - vehForwardYaw;

    if( viewforwardoffset > 75 )
    {
      client->vehicleViewYaw = vehForwardYaw + 75.0f;
    }else if( viewforwardoffset < 75 ){
      client->vehicleViewYaw = vehForwardYaw - 75.0f;
    }
*/

    oldVehForwardYaw = vehForwardYaw;
    oldClientViewYaw = client->viewangles[1];
    //client->vehicleViewYaw = cVehEnt->pose.angles[YAW] - 90.0f;
  }

  v1 = fabs((normalizedPitch - floorf(normalizedPitch + 0.5)) * 360.0);

  lookAtPos[2] = (vehDriverViewHeightMax->floatval - v1) / vehDriverViewHeightMax->floatval * vehDriverViewFocusRange->floatval + lookAtPos[2];
  if ( vehDebugClient->boolean )
  {
    CG_DebugStar(lookAtPos, colorBlue, 0);
  }
  CG_VehSphereCoordsToPos(vehDriverViewDist->floatval, client->vehicleViewYaw, client->vehicleViewPitch, newLookDir);

  VectorAdd(newLookDir, lookAtPos, cg.refdef.vieworg);

  ThirdPersonViewTrace(&cg, lookAtPos, cg.refdef.vieworg, 0x811, cg.refdef.vieworg);

  VectorSubtract(lookAtPos, cg.refdef.vieworg, newLookDir);

  VectorNormalize(newLookDir);
  vectoangles(newLookDir, cg.refdefViewAngles);
  AnglesToAxis(cg.refdefViewAngles, cg.refdef.viewaxis);

  CG_CalcFov();
}

float BG_GetSpeed(const playerState_t *ps, int time)
{
  if ( (ps->pm_flags & 8) != 0 )
  {
    if ( time - ps->jumpTime >= 500 )
    {
      return ps->velocity[2];
    }
    return 0.0;
  }
  return Vec2Length(ps->velocity);
}

clientInfo_t * ClientInfoForLocalClient(int localClientNum)
{
  cg_t *cgameGlob;
  playerState_t *ps;
  bgs_t *bg;

  assert((unsigned int)localClientNum > MAX_LOCAL_CLIENTS);

  cgameGlob = CG_GetLocalClientGlobals(localClientNum);
  ps = &cgameGlob->predictedPlayerState;
  bg = &cgameGlob->bgs;
  assert(ps->clientNum < MAX_CLIENTS);
  return &bg->clientinfo[ps->clientNum];
}

bool CG_VehLocalClientUsingVehicle(int localClientNum)
{
  clientInfo_t *ci;

  ci = ClientInfoForLocalClient(localClientNum);
  assert(ci);
  return ci->attachedVehEntNum != 1023;
}

void CG_PerturbCamera(cg_t *cgameGlob)
{
  vec3_t rot[3];
  vec3_t axis[3];

  if ( cgameGlob->shellshock.viewDelta[0] != 0.0 || cgameGlob->shellshock.viewDelta[1] != 0.0 )
  {
    rot[2][0] = 0.0;
    rot[2][1] = 0.0;
    rot[2][2] = 1.0;
    rot[0][0] = 1.0;
    rot[0][1] = cgameGlob->shellshock.viewDelta[0];
    rot[0][2] = cgameGlob->shellshock.viewDelta[1];
    Vec3Normalize(rot[0]);
    Vec3Cross(rot[2], rot[0], rot[1]);
    Vec3Normalize(rot[1]);
    Vec3Cross(rot[0], rot[1], rot[2]);
    AxisCopy(cgameGlob->refdef.viewaxis, axis);
    MatrixMultiply(rot, axis, cgameGlob->refdef.viewaxis);
  }
}

void CG_CalcVrect(int localClientNum)
{
  cg_t *cgameGlob;
  cgs_t *_cgs;

  cgameGlob = CG_GetLocalClientGlobals(localClientNum);
  _cgs = CG_GetLocalClientStaticGlobals(localClientNum);
  cgameGlob->refdef.x = _cgs->viewX;
  cgameGlob->refdef.y = _cgs->viewY;
  cgameGlob->refdef.width = _cgs->viewWidth;
  cgameGlob->refdef.height = _cgs->viewHeight;
  cgameGlob->refdef.useScissorViewport = false;
}


void CG_SmoothCameraZ(cg_t *cgameGlob)
{
  int timeSinceStart;
  int smoothingDuration;
  float lerp;

  if ( cgameGlob->stepViewChange != 0.0 && cgameGlob->time - cgameGlob->stepViewStart >= 0 )
  {
    timeSinceStart = cgameGlob->time - cgameGlob->stepViewStart;
    smoothingDuration = (int)(cg_viewZSmoothingTime->floatval * 1000.0);
    if ( timeSinceStart < smoothingDuration ) //clip to bounds between 1.0 and 0.0
    {
      if ( timeSinceStart >= 0 )
      {
        lerp = (float)timeSinceStart / (float)smoothingDuration;
      }
      else
      {
        lerp = 0.0;
      }
    }
    else
    {
      lerp = 1.0;
    }
    cgameGlob->refdef.vieworg[2] = cgameGlob->refdef.vieworg[2] - (1.0 - lerp) * cgameGlob->stepViewChange;
  }
}

int CG_VehLocalClientVehicleSlot(int localClientNum)
{
  clientInfo_t *ci;

  ci = ClientInfoForLocalClient(localClientNum);

  assert(ci);
  assert(ci->attachedVehEntNum != ENTITYNUM_NONE);

  return ci->attachedVehSeat;
}

void CG_VehGunnerPOV(int localClientNum, float *resultOrigin, float *resultAngles)
{
  clientInfo_t *ci;
  float axis[4][3];

  ci = ClientInfoForLocalClient(localClientNum);

  assert(ci);
  assert(ci->attachedVehEntNum != ENTITYNUM_NONE);

  GetTagMatrix(localClientNum, ci->attachedVehEntNum, scr_const.tag_gunner_pov, axis, resultOrigin);
  AxisToAngles(axis, resultAngles);
}

void SeatTransformForSlot(int localClientNum, int vehEntNum, int vehSlotIdx, float *resultOrigin, float *resultAngles)
{
  uint16_t tagName;
  vec3_t tagOrigin;
  vec3_t tagMtx[3];

  tagName = BG_VehiclesGetSlotTagName(vehSlotIdx);
  GetTagMatrix(localClientNum, vehEntNum, tagName, tagMtx, tagOrigin);
  if ( resultAngles )
  {
    AxisToAngles(tagMtx, resultAngles);
  }
  if ( resultOrigin )
  {
    VectorCopy(tagOrigin, resultOrigin);
    if ( vehSlotIdx == VEHICLE_RIDESLOT_DRIVER || vehSlotIdx == VEHICLE_RIDESLOT_PASSENGER )
    {
      resultOrigin[2] -= 35.0;
    }
  }
}

void SeatTransformForClientInfo(int localClientNum, clientInfo_t *ci, float *resultOrigin, float *resultAngles)
{
  assert(ci);
  assert(ci->attachedVehEntNum != ENTITYNUM_NONE);

  SeatTransformForSlot(localClientNum, ci->attachedVehEntNum, ci->attachedVehSeat, resultOrigin, resultAngles);
}


clientInfo_t* ClientInfoForEntity(int localClientNum, int entNum)
{
  centity_t *cent;

  cent = CG_GetEntity(localClientNum, entNum);
  if ( cent->nextState.eType != ET_PLAYER )
  {
    return 0;
  }
  assert(cent->nextState.clientNum < MAX_CLIENTS);
  return &cg.bgs.clientinfo[cent->nextState.clientNum];
}

void CG_VehSeatTransformForPlayer(int localClientNum, int entNum, float *resultOrigin, float *resultAngles)
{
  clientInfo_t *ci;

  ci = ClientInfoForEntity(localClientNum, entNum);
  /*
  centity_t *centPlayer = CG_GetEntity(localClientNum, entNum);

  assert(centPlayer->nextState.eType == ET_PLAYER);
  assert(centPlayer->nextState.eType < ET_EVENTS);
  */
  assert(ci->attachedVehEntNum != 0);

  SeatTransformForClientInfo(localClientNum, ci, resultOrigin, resultAngles);
}

void CG_VehSeatOriginForLocalClient(int localClientNum, float *result)
{
  clientInfo_t *ci;

  assert(result);
  ci = ClientInfoForLocalClient(localClientNum);
  SeatTransformForClientInfo(localClientNum, ci, result, 0);
}

void CalcViewValuesVehicleGunner(int localClientNum)
{
  cg_t *cgameGlob = CG_GetLocalClientGlobals(localClientNum);

  CG_VehGunnerPOV(localClientNum, cgameGlob->refdef.vieworg, cgameGlob->refdefViewAngles);
  AnglesToAxis(cgameGlob->refdefViewAngles, cgameGlob->refdef.viewaxis);
  CG_ApplyViewAnimation(localClientNum);
  CG_PerturbCamera(cgameGlob);
  CG_CalcFov(localClientNum);
}

void CalcViewValuesVehiclePassenger(int localClientNum)
{
  playerState_t *ps;

  cg_t *cgameGlob = CG_GetLocalClientGlobals(localClientNum);

  ps = &cgameGlob->predictedPlayerState;
  CG_VehSeatOriginForLocalClient(localClientNum, ps->origin);

  VectorCopy(ps->origin, cgameGlob->refdef.vieworg);
  VectorCopy(ps->viewangles, cgameGlob->refdefViewAngles);

  CG_OffsetFirstPersonView(cgameGlob);
  AnglesToAxis(cgameGlob->refdefViewAngles, cgameGlob->refdef.viewaxis);
  CG_ApplyViewAnimation(localClientNum);
  CG_PerturbCamera(cgameGlob);
  CG_CalcFov(localClientNum);
}

void CalcViewValuesVehicle(int localClientNum)
{
  int slot;

  assert(CG_VehLocalClientUsingVehicle( localClientNum ));

  slot = CG_VehLocalClientVehicleSlot(localClientNum);
  if(slot == VEHICLE_RIDESLOT_DRIVER)
  {
    CalcViewValuesVehicleDriver(localClientNum);
  }else if(slot == VEHICLE_RIDESLOT_PASSENGER){
    CalcViewValuesVehiclePassenger(localClientNum);
  }else{
    assert(slot == VEHICLE_RIDESLOT_GUNNER);
    CalcViewValuesVehicleGunner(localClientNum);
  }
}

void CG_CalcViewValues(int localClientNum)
{
  //float maxmin;
  //float *lastViewAngles;
  //float *predictedError;
  //float *refdefViewAngles;
  //float *viewangles;
  //float *vieworg;
  //float *origin;
  //vec3_t angles;
  int t;
  float f;
  //CameraMode camMode;
  cg_t *cgameGlob;
  float uiBlurRadius;
  playerState_t *ps;
  static bool initVeh;

  cgameGlob = CG_GetLocalClientGlobals(localClientNum);
  cgameGlob->refdef.zNear = 0.0;
  cgameGlob->refdef.time = cgameGlob->time;
  cgameGlob->refdef.localClientNum = localClientNum;
  /*
  cgameGlob->refdef.sunVisibility = 1.0;
  cgameGlob->refdef.noLodCullOut = 0;
  */
  uiBlurRadius = CL_GetMenuBlurRadius(localClientNum);
  cgameGlob->refdef.blurRadius = sqrt((float)(cgDC[localClientNum].blurRadiusOut * cgDC[localClientNum].blurRadiusOut) + (float)(uiBlurRadius * uiBlurRadius));
  /*
  camMode = CG_UpdateCameraMode(localClientNum);
  CG_CalcFov(localClientNum, -1.0);
  */
  CG_VisionSetApplyToRefdef(localClientNum);
  if ( cgameGlob->cubemapShot )
  {
    //CG_CalcFov(localClientNum, -1.0);
    CG_CalcCubemapViewValues(cgameGlob);
    return;
  }

  CG_CalcVrect(localClientNum);
  ps = &cgameGlob->predictedPlayerState;
  if ( cgameGlob->predictedPlayerState.pm_type == PM_INTERMISSION )
  {
    VectorCopy(ps->origin, cgameGlob->refdef.vieworg);
    VectorCopy(ps->viewangles, cgameGlob->refdefViewAngles);
    AnglesToAxis(cgameGlob->refdefViewAngles, cgameGlob->refdef.viewaxis);
    CG_CalcFov(localClientNum); //CoD4
    return;
  }

  if(CG_VehLocalClientUsingVehicle(localClientNum))
  {

    if(!initVeh)
    {
      initVeh = true;
      CG_VehUseInitialize();
    }
    CalcViewValuesVehicle(localClientNum);
    return;
  }

  if(initVeh)
  {
    initVeh = false;
    CG_VehUseDeinitialize();
  }

  cgameGlob->fBobCycle = BG_GetBobCycle(ps);
  cgameGlob->xyspeed = BG_GetSpeed(&cgameGlob->predictedPlayerState, cgameGlob->time);
  VectorCopy(ps->origin, cgameGlob->refdef.vieworg);
  if ( !cgameGlob->playerTeleported )
  {
    if(cgameGlob->nextSnap->ps.pm_type == PM_NORMAL || cgameGlob->nextSnap->ps.pm_type == PM_NOCLIP || cgameGlob->nextSnap->ps.pm_type == PM_UFO)
    {
//      if( cgameGlob->renderingThirdPerson == TP_OFF )
        CG_SmoothCameraZ(cgameGlob);
    }
  }
  VectorCopy(cgameGlob->refdef.vieworg, cgameGlob->lastVieworg);
  VectorCopy(ps->viewangles, cgameGlob->refdefViewAngles); //cod4
  if ( /*camMode != CAM_VEHICLE && camMode != CAM_VEHICLE_THIRDPERSON && */ cg_errorDecay->floatval > 0.0 )
  {
    t = cgameGlob->time - cgameGlob->predictedErrorTime;
    f = (cg_errorDecay->floatval - (float)t) / cg_errorDecay->floatval;
    if ( f <= 0.0 || f >= 1.0 )
    {
      cgameGlob->predictedErrorTime = 0;
    }
    else
    {
      VectorMA(cgameGlob->refdef.vieworg, f, cgameGlob->predictedError, cgameGlob->refdef.vieworg);
    }
  }
  CG_CalcTurretViewValues(localClientNum);
  if ( !cgameGlob->renderingThirdPerson ){
    CG_OffsetFirstPersonView(cgameGlob);
  }
  CG_ShakeCamera(localClientNum);
  AnglesToAxis(cgameGlob->refdefViewAngles, cgameGlob->refdef.viewaxis);
  CG_ApplyViewAnimation(localClientNum);
  if ( cgameGlob->renderingThirdPerson ){
    CG_OffsetThirdPersonView(cgameGlob);
  }
  CG_PerturbCamera(cgameGlob);
  CG_CalcFov(localClientNum);

/*  else if ( Demo_IsPlaying() && ps->pm_type == 4 && !Demo_IsMovieCamera() && ps->stats[4] < 2 )
  {
    VectorCopy(ps->origin, cgameGlob->refdef.vieworg);
    VectorCopy(ps->viewangles, cgameGlob->refdefViewAngles);
    AnglesToAxis(cgameGlob->refdefViewAngles, cgameGlob->refdef.viewaxis);
  }
  else if ( CG_KillCamEntityEnabled(localClientNum) )
  {
    CG_UpdateKillCamEntity(cgameGlob->killCamEntityType, localClientNum);
    CG_VisionSetApplyToRefdef(localClientNum, 0);
  }
  else
  {
    cgameGlob->fBobCycle = BG_GetBobCycle(ps);
    cgameGlob->xyspeed = BG_GetSpeed(&cgameGlob->predictedPlayerState, cgameGlob->time);
    VectorCopy(ps->origin, cgameGlob->refdef.vieworg);

    if ( !cgameGlob->playerTeleported
      && (!cgameGlob->nextSnap->ps.pm_type || cgameGlob->nextSnap->ps.pm_type == 2 || cgameGlob->nextSnap->ps.pm_type == 3)
      && cgameGlob->renderingThirdPerson == TP_OFF )
    {
      CG_SmoothCameraZ(cgameGlob);
    }
    VectorCopy(ps->viewangles, cgameGlob->refdefViewAngles);

    switch ( camMode )
    {
      case CAM_VEHICLE:
        CG_CalcVehicleViewValues(localClientNum);
        VectorCopy(ps->viewangles, cgameGlob->refdefViewAngles);
        break;
      case CAM_VEHICLE_THIRDPERSON:
        CG_Calc3rdPersonVehicleViewValues(localClientNum);
        break;
      case CAM_MISSILE:
        CG_CalcMissileViewValues(localClientNum);
        break;
      case CAM_EXTRACAM:
        CG_CalcExtraCamViewValues(localClientNum);
        break;
    }
    if ( camMode != CAM_VEHICLE && camMode != CAM_VEHICLE_THIRDPERSON && cg_errorDecay->current.value > 0.0 )
    {
      t = cgameGlob->time - cgameGlob->predictedErrorTime;
      f = (float)(cg_errorDecay->current.value - (float)t) / cg_errorDecay->current.value;
      if ( f <= 0.0 || f >= 1.0 )
      {
        cgameGlob->predictedErrorTime = 0;
      }
      else
      {
        VectorMA(cgameGlob->refdef.vieworg, f, cgameGlob->predictedError, cgameGlob->refdef.vieworg);
      }
    }
    switch ( camMode )
    {
      case CAM_TURRET:
        CG_CalcTurretViewValues(localClientNum);
        break;
      case CAM_VEHICLE:
      case CAM_VEHICLE_THIRDPERSON:
        CG_OffsetVehicleView(localClientNum, camMode);
        break;
      case CAM_VEHICLE_GUNNER:
        cgameGlob->refdef.noLodCullOut = 1;
        CG_OffsetVehicleGunner(localClientNum, cgameGlob);
        break;
      default:
        if ( camMode != CAM_EXTRACAM && camMode != CAM_MISSILE )
        {
          if ( camMode == CAM_RADIANT )
          {
            CG_RadiantCamCalcView(localClientNum);
          }
          else if ( cgameGlob->renderingThirdPerson != TP_FOR_MODEL )
          {
            CG_OffsetFirstPersonView(cgameGlob);
          }
        }
        break;
    }
    CG_ShakeCamera(localClientNum);
    CG_CalcFov(localClientNum, -1.0);
    CG_UpdateCameraTween(localClientNum);
    if ( camMode == CAM_MISSILE )
    {
      CG_CalcMissileAngleValues(localClientNum);
      VectorCopy(cgameGlob->refdefViewAngles, cgameGlob->cameraData.lastViewAngles);
    }
    AnglesToAxis(cgameGlob->refdefViewAngles, cgameGlob->refdef.viewaxis);
    CG_ApplyViewAnimation(localClientNum);
    if ( cgameGlob->renderingThirdPerson == TP_FOR_MODEL && CG_ShouldRenderThirdPerson(camMode) )
    {
      CG_OffsetThirdPersonView(localClientNum);
    }
    else if ( camMode == CAM_VEHICLE && (ps->otherFlags & 2) != 0 && cgameGlob->renderingThirdPerson == TP_FOR_MODEL )
    {
      CG_OffsetChaseCamView(localClientNum, camMode);
    }
    CG_PerturbCamera(cgameGlob);
    R_SetADSZScale(localClientNum, min(cg_adsZScaleMax->floatval - ps->fWeaponPosFrac * ps->fWeaponPosFrac, 1.0));
    if ( cg_thirdPerson->integer && cg_thirdPersonMode->integer == 2 )
    {
      AnglesToAxis(devSavedAngles, cgameGlob->refdef.viewaxis);
      VectorCopy(devSavedOrigin, cgameGlob->refdef.vieworg);

    }
    else
    {
      AxisToAngles(cgameGlob->refdef.viewaxis, angles);
      CG_DevSaveCamera(angles, cgameGlob->refdef.vieworg);
    }
    CG_ExtraCamDebug_SaveView(localClientNum);
  }
  */
}


#ifndef OFFICIAL

void CG_ClearAngles()
{
	VectorClear(cg.kickAVel);
	VectorClear(cg.kickAngles);
}



void CG_UpdateViewAngles()
{
  cl.cgameKickAngles[0] = cg.kickAngles[0] + cg.offsetAngles[0];
  cl.cgameKickAngles[1] = cg.offsetAngles[1] + cg.kickAngles[1];
  cl.cgameKickAngles[2] = cg.offsetAngles[2] + cg.kickAngles[2];
}

#endif
