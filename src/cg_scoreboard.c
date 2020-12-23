#include "cg_scoreboard.h"

#include "ui_shared.h"
#include "client.h"
#include "cg_shared.h"

cvar_t* cg_ScoresPing_MaxBars;
cvar_t* cg_ScoresPing_Interval;
cvar_t* cg_ScoresPing_HighColor;
cvar_t* cg_ScoresPing_MedColor;
cvar_t* cg_ScoresPing_LowColor;
cvar_t* cg_ScoresPing_BgColor;
cvar_t* cg_scoreboardScrollStep;
cvar_t* cg_scoreboardBannerHeight;
cvar_t* cg_scoreboardItemHeight;
cvar_t* cg_scoreboardPingWidth;
cvar_t* cg_scoreboardPingHeight;
cvar_t* cg_scoreboardWidth;
cvar_t* cg_scoreboardHeight;
cvar_t* cg_scoreboardMyColor;
cvar_t* cg_scoreboardTextOffset;
cvar_t* cg_scoreboardFont;
cvar_t* cg_scoreboardHeaderFontScale;
cvar_t* cg_scoreboardPingText;
cvar_t* cg_scoreboardPingGraph;
cvar_t* cg_scoreboardRankFontScale;
cvar_t* cg_scoreboardTagColor;


void __cdecl CG_RegisterScoreboardCvars()
{
  cg_ScoresPing_MaxBars = Cvar_RegisterInt("cg_ScoresPing_MaxBars", 4, 1, 10, 1u, "Number of bars to show in ping graph");
  cg_ScoresPing_Interval = Cvar_RegisterInt("cg_ScoresPing_Interval", 100, 1, 500, 1u, "Number of milliseconds each bar represents");
  cg_ScoresPing_HighColor = Cvar_RegisterColor("cg_ScoresPing_HighColor", 0.80000001, 0.0, 0.0, 1.0, 1u, "Color for high ping");
  cg_ScoresPing_MedColor = Cvar_RegisterColor("cg_ScoresPing_MedColor", 0.80000001, 0.80000001, 0.0, 1.0, 1u, "Color for medium ping");
  cg_ScoresPing_LowColor = Cvar_RegisterColor("cg_ScoresPing_LowColor", 0.0, 0.75, 0.0, 1.0, 1u, "Color for low ping");
  cg_ScoresPing_BgColor = Cvar_RegisterColor("cg_ScoresPing_BgColor", 0.25, 0.25, 0.25, 0.5, 1u, "Background color of ping");
  cg_scoreboardScrollStep = Cvar_RegisterInt("cg_scoreboardScrollStep", 3, 1, 8, 0, "Scroll step amount for the scoreboard");
  cg_scoreboardBannerHeight = Cvar_RegisterInt("cg_scoreboardBannerHeight", 35, 1, 100, 0, "Banner height of the scoreboard");
  cg_scoreboardItemHeight = Cvar_RegisterInt("cg_scoreboardItemHeight", 18, 1, 1000, 0, "Item height of each item");
  cg_scoreboardPingWidth = Cvar_RegisterFloat("cg_scoreboardPingWidth", 0.035999998, 0.0, 1.0, 0, "Width of the ping graph as a % of the scoreboard");
  cg_scoreboardPingHeight = Cvar_RegisterFloat("cg_scoreboardPingHeight", 0.69999999, 0.0, 1.0, 0, "Height of the ping graph as a % of the scoreboard row height");
  cg_scoreboardWidth = Cvar_RegisterFloat("cg_scoreboardWidth", 500.0, 0.0, 3.4028235e38, 0, "Width of the scoreboard");
  cg_scoreboardHeight = Cvar_RegisterFloat("cg_scoreboardHeight", 435.0, 0.0, 3.4028235e38, 0, "Height of the scoreboard");
  cg_scoreboardMyColor = Cvar_RegisterColor("cg_scoreboardMyColor", 1.0, 0.80000001, 0.40000001, 1.0, 0, "The local player's font color when shown in scoreboard");
  cg_scoreboardTagColor = Cvar_RegisterColor("cg_scoreboardTagColor", 1.0, 0.8, 0.6, 1.0, 0, "The clan tag color when shown in scoreboard");
  cg_scoreboardTextOffset = Cvar_RegisterFloat("cg_scoreboardTextOffset", 0.5, 0.0, 3.4028235e38, 0, "Scoreboard text offset");
  cg_scoreboardFont = Cvar_RegisterInt("cg_scoreboardFont", 0, 0, 6, 0, "Scoreboard font enum ( see menudefinition.h )");
  cg_scoreboardHeaderFontScale = Cvar_RegisterFloat("cg_scoreboardHeaderFontScale", 0.34999999, 0.0, 3.4028235e38, 0, "Scoreboard header font scale");
  cg_scoreboardPingText = Cvar_RegisterBool("cg_scoreboardPingText", qtrue, 0, "Whether to show numeric ping value");
  cg_scoreboardPingGraph = Cvar_RegisterBool("cg_scoreboardPingGraph", 0, 0, "Whether to show graphical ping");
  cg_scoreboardRankFontScale = Cvar_RegisterFloat("cg_scoreboardRankFontScale", 0.25, 0.0, 3.4028235e38, 0, "Scale of rank font");
}



float CG_GetScoreboardHStart(int localClientNum)
{
  float screenwidth;
  float start;


  screenwidth = scrPlaceView[localClientNum].virtualViewableMax[0] - scrPlaceView[localClientNum].virtualViewableMin[0];
  float scoreboardW = cg_scoreboardWidth ? cg_scoreboardWidth->floatval : 0.0;
  start = (screenwidth - scoreboardW) * 0.5;
  if ( 0.0 - start < 0.0 )
  {
    return start;
  }
  return 0.0;
}


float CG_GetScoreboardHeight()
{
  float v0;
  float scoreboardH = cg_scoreboardHeight ? cg_scoreboardHeight->floatval : 0.0;
  v0 = (480.0 - scoreboardH) * 0.5;
  if ( 0.0 - v0 < 0.0 )
  {
    return v0;
  }
  return 0.0;

}



static void CG_DrawScoreboardServerNameAddress( float alpha )
{

  int addrwidth, hostnamewidth;
  const char *serverAddrText;
  Font_t *font;
  float y;
  vec4_t color;
  float xname;
  float xaddr;
  float fontscale;
  float ymap;
  char levelname[64];
  int len;

  color[0] = 1.0;
  color[1] = 1.0;
  color[2] = 1.0;
  color[3] = alpha;

  serverAddrText = CL_GetServerIPAddress();
  if ( !Q_stricmp(serverAddrText, "loopback") )
  {
    serverAddrText = "Listen Server";//UI_SafeTranslateString("CGAME_LISTENSERVER");
  }

  float scoreboardW = cg_scoreboardWidth ? cg_scoreboardWidth->floatval : 0.0;
  int scoreboardFont = cg_scoreboardFont ? cg_scoreboardFont->integer : 0;
  for(fontscale = 0.35; fontscale > 0.075; fontscale -= 0.001)
  {
    font = UI_GetFontHandle(&scrPlaceView[0], scoreboardFont, fontscale);
    hostnamewidth = UI_TextWidth(cgs.szHostName, 0, font, fontscale);
    addrwidth = UI_TextWidth(serverAddrText, 0, font, fontscale);

    if ( scoreboardW - 6.0 - 8.0 >= (float)(hostnamewidth + addrwidth +4))
	{
      break;
	}
  }
  float scoreboardH = cg_scoreboardHeight ? cg_scoreboardHeight->floatval : 0.0;
  y = CG_GetScoreboardHeight() + scoreboardH - 5.0 - (float)(14 - UI_TextHeight(font, fontscale)) * 0.5;
  xname = CG_GetScoreboardHStart(0) + 3.0 + 2.0 + 4.0;
  UI_DrawText(&scrPlaceView[0], cgs.szHostName, 0x7FFFFFFF, font, xname, y, 1, 0, fontscale, color, 3);

  Q_strncpyz(levelname, cgs.mapname +8, sizeof(levelname));
  len = strlen(levelname);
  if(len > 8)
  {
	levelname[len -7] = '\0';
  }
  ymap = y + UI_TextHeight(font, fontscale);
  UI_DrawText(&scrPlaceView[0], levelname, 0x7FFFFFFF, font, xname, ymap, 1, 0, fontscale, color, 3);

  xaddr = CG_GetScoreboardHStart(0) + scoreboardW - 3.0 - 2.0 - 4.0;
  xaddr = xaddr - (float)(UI_TextWidth(serverAddrText, 0, font, fontscale) + 4);
  UI_DrawText(&scrPlaceView[0], serverAddrText, 0x7FFFFFFF, font, xaddr, y, 1, 0, fontscale, color, 3);

}



signed int CG_CheckDrawScoreboardLine(int *drawLine, float y, float lineHeight)
{

  if ( cg.scoresBottom )
  {
    return 0;
  }
  if ( *drawLine < cg.scoresOffBottom )
  {
    ++*drawLine;
    return 0;
  }
  float scoreboardH = cg_scoreboardHeight ? cg_scoreboardHeight->floatval : 0.0;
  if ( CG_GetScoreboardHeight() + scoreboardH - 3.0 - 2.0 - 14.0 - 1.0 < y + lineHeight )
  {
    cg.scoresBottom = 1;
    return 0;
  }
  ++*drawLine;
  return 1;
}



typedef enum
{
  LCT_NAME = 0x0,
  LCT_CLAN = 0x1,
  LCT_SCORE = 0x2,
  LCT_DEATHS = 0x3,
  LCT_PING = 0x4,
  LCT_STATUS_ICON = 0x5,
  LCT_TALKING_ICON = 0x6,
  LCT_KILLS = 0x7,
  LCT_RANK_ICON = 0x8,
  LCT_ASSISTS = 0x9,
  LCT_NUM = 0xA,
}listColumnTypes_t;

typedef struct
{
  listColumnTypes_t type;
  float fWidth;
  const char *pszName;
  int iAlignment;
}listColumnInfo_t;



listColumnInfo_t columnInfoWithPing[] =
{
  { LCT_RANK_ICON,  0.05, "", 0 },
  { LCT_STATUS_ICON,  0.05, "", 2 },
  { LCT_NAME, 0.35, "", 0 },
  { LCT_TALKING_ICON,  0.05, "", 0 },
  { LCT_SCORE,  0.1, "CGAME_SB_SCORE", 2 },
  { LCT_KILLS,  0.1, "CGAME_SB_KILLS", 2 },
  { LCT_ASSISTS,  0.1, "CGAME_SB_ASSISTS", 2 },
  { LCT_DEATHS,  0.1, "CGAME_SB_DEATHS", 2 },
  { LCT_PING,  0.1, "CGAME_SB_PING", 2 }
};

listColumnInfo_t columnInfo[] =
{
  { LCT_RANK_ICON,  0.07, "", 0 },
  { LCT_STATUS_ICON,  0.05, "", 2 },
  { LCT_NAME,  0.43, "", 0 },
  { LCT_TALKING_ICON,  0.05, "", 0 },
  { LCT_SCORE,  0.1, "CGAME_SB_SCORE", 2 },
  { LCT_KILLS,  0.1, "CGAME_SB_KILLS", 2 },
  { LCT_ASSISTS,  0.1, "CGAME_SB_ASSISTS", 2 },
  { LCT_DEATHS,  0.1, "CGAME_SB_DEATHS", 2 }
};


void CG_DrawListString(int localClientNum, const char *string, float x, float y, float width, int alignment, Font_t *font, float scale, int style, const float *color)
{
  float tw;
  float v15;
  signed int height;

  if ( string )
  {
  	if(width < 5.0)
  	{
  		width = 5.0;
  	}

    while ( (float)UI_TextWidth(string, 0x7FFFFFFF, font, scale) > width )
    {
      scale = scale - 0.025;
    }

    if ( scale < 0.2 )
  	{
        style = 0;
  	}

    tw = (float)UI_TextWidth(string, 0x7FFFFFFF, font, scale);


	  if ( alignment == 1 )
    {
      v15 = (float)(width - tw) * 0.5;
    }
    else if ( alignment == 2 )
    {
      v15 = (float)(width - tw) - 4.0;
    }
    else
    {
      v15 = 0.0;
    }

    height = UI_TextHeight(font, scale);
    int scoreboardItemHeight = cg_scoreboardItemHeight ? cg_scoreboardItemHeight->integer : 0;
    float scoreboardTextOfs = cg_scoreboardTextOffset ? cg_scoreboardTextOffset->floatval : 0.0;
	UI_DrawText(&scrPlaceView[localClientNum], string, 0x7FFFFFFF, font, v15 + x, (float)(scoreboardTextOfs * (float)((float)height + (float)scoreboardItemHeight)) + y, 1, 0, scale, color, style);
  }
}

float sub_448570(int alignment, float width, float height)
{
  if ( alignment != 1)
  {
    if ( alignment == 2 )
    {
      return width - height;
    }
    return 0.0;
  }
  return (width - height) * 0.5;

}


void CG_DrawClientPing(int localClientNum, int ping, float x, float y, float maxWidth, float maxHeight)
{
  Material *white;
  float w;
  int interval;
  int i;
  int v19;
  float h;
  signed int maxbars;
  float v33, v23;
  float cs;
  float sx, sy;
  vec4_t color, color1, color2;

  white = Material_RegisterHandle("white", 0);

  Cvar_GetUnpackedColorByName("cg_ScoresPing_BgColor", color);

  v33 = x + 8.0;
  w = maxWidth + 2.0;
  UI_DrawHandlePic(&scrPlaceView[localClientNum], v33 - 1.0, y, w, maxHeight, 1, 0, color, white);

  maxbars = cg_ScoresPing_MaxBars ? cg_ScoresPing_MaxBars->integer : 0;
  interval = cg_ScoresPing_Interval ? cg_ScoresPing_Interval->integer : 1;


  if ( maxbars - ping / interval < 1 )
  {
    v19 = 1;
  }
  else
  {
    v19 = maxbars - ping / interval;
  }

  if ( v19 >= maxbars / 2 )
  {
	Cvar_GetUnpackedColorByName("cg_ScoresPing_MedColor", color1);
	Cvar_GetUnpackedColorByName("cg_ScoresPing_LowColor", color2);
	v23 = v19 - maxbars / 2;
  }
  else
  {
	Cvar_GetUnpackedColorByName("cg_ScoresPing_HighColor", color1);
    Cvar_GetUnpackedColorByName("cg_ScoresPing_MedColor", color2);
	v23 = v19;
  }

  cs = v23 / (float)(maxbars / 2);

  color[0] = (color2[0] - color1[0]) * cs + color1[0];
  color[1] = (color2[1] - color1[1]) * cs + color1[1];
  color[2] = (color2[2] - color1[2]) * cs + color1[2];
  color[3] = (color2[3] - color1[3]) * cs + color1[3];

  sx = v33;

  w = maxWidth / (float)maxbars - 1.0;

  if ( 1.0 > w )
    w = 1.0;

  for (i = 0; i < v19; ++i)
  {
      float pingH = cg_scoreboardPingHeight ? cg_scoreboardPingHeight->floatval : 0.0;
      h = pingH * maxHeight * (float)(i + 1) / (float)maxbars;
      sy = y + maxHeight - h;
      UI_DrawHandlePic(&scrPlaceView[localClientNum], sx, sy, w, h, 1, 0, color, white);
      sx += w + 1.0;
  }
}



float CG_DrawClientScore(int localClientNum, float *argcolor, float y, score_t *score, float listWidth)
{
  clientInfo_t *clientInfo;
  listColumnInfo_t *scoreitemList;
  int scoreitemCount;
  int i;
  float dx, dy, sx, sy;
  vec4_t color, expandedColor, tagcolor;
  Material *whitemat, *voiceimg;
  float hstart, width;
  Font_t* font;
  const char* name, *clantag;
  float scale, clantagwidth;
  dy = y;

  clientInfo = &cg.bgs.clientinfo[score->client];

  if ( !cg.bgs.clientinfo[score->client].infoValid )
  {
    return y;
  }



  hstart = CG_GetScoreboardHStart(localClientNum);
  whitemat = Material_RegisterHandle("white", 0);

  color[0] = argcolor[0];
  color[1] = argcolor[1];
  color[2] = argcolor[2];
  color[3] = argcolor[3] * 0.5;


  dx = hstart + 3.0 + 2.0 + 4.0;

  int itemHeight = cg_scoreboardItemHeight ? cg_scoreboardItemHeight->integer : 0;
  UI_DrawHandlePic(&scrPlaceView[0], dx, y, listWidth, itemHeight, 1, 0, color, whitemat);

  int scoreboardFont = cg_scoreboardFont ? cg_scoreboardFont->integer : 0;
  font = UI_GetFontHandle(&scrPlaceView[0], scoreboardFont, 0.35);
  //font = Font_GetSystemFont();

  if ( score->client == cg.clientNum )
  {
    Cvar_GetUnpackedColor(cg_scoreboardMyColor, expandedColor);
  }
  else
  {
    expandedColor[0] = 1.0;
    expandedColor[1] = 1.0;
    expandedColor[2] = 1.0;
  }
  expandedColor[3] = argcolor[3];

  Cvar_GetUnpackedColor(cg_scoreboardTagColor, tagcolor);
  tagcolor[3] = argcolor[3];

  byte sbPingText = cg_scoreboardPingText ? cg_scoreboardPingText->boolean : 0;
  if(Com_IsLegacyServer())
  {
	name = clientInfo->name;
	clantag = "";
	if ( sbPingText )
	{
		scoreitemList = columnInfoWithPing;
		scoreitemCount = 9;
	}
	else
	{
		scoreitemList = columnInfo;
		scoreitemCount = 8;
	}
  }else{
	name = CG_GetUsernameX(score->client);
	clantag = CG_GetClantag(score->client);
	if ( sbPingText )
	{
		scoreitemList = columnInfoWithPing;
		scoreitemCount = 9;
	}
	else
	{
		scoreitemList = columnInfo;
		scoreitemCount = 8;
	}
  }

  float sbRankFontScale = cg_scoreboardRankFontScale ? cg_scoreboardRankFontScale->floatval : 0.0;
  for(i = 0, width = scoreitemList->fWidth * listWidth; i < scoreitemCount; ++i, scoreitemList++ ,dx += width, width = scoreitemList->fWidth * listWidth)
  {
		switch(scoreitemList->type)
		{
			case LCT_NAME:
				scale = 0.35;
				if(clantag[0])
				{
					while ( (float)UI_TextWidth(name, 0x7FFFFFFF, font, scale) + (float)UI_TextWidth(clantag, 0x7FFFFFFF, font, scale) + 6.5 > width )
					{
					  scale = scale - 0.025;
					}
					clantagwidth = UI_TextWidth(clantag, 0x7FFFFFFF, font, scale);
					if(2 * clantagwidth > width)
					{
						clantagwidth = width / 2;
					}
					CG_DrawListString(localClientNum, clantag, dx, dy, clantagwidth, scoreitemList->iAlignment, font, scale, 3, tagcolor);
					CG_DrawListString(localClientNum, name, dx + clantagwidth + 6.5, dy, width - clantagwidth - 6.5, scoreitemList->iAlignment, font, scale, 3, expandedColor);
				}else{
					CG_DrawListString(localClientNum, name, dx, dy, width, scoreitemList->iAlignment, font, scale, 3, expandedColor);
				}
				break;
			case LCT_CLAN:
				//Will be handled in LTC_NAME - looks stupid otherwise
				break;
			case LCT_SCORE:
				if ( score->team != 3 )
				{
					CG_DrawListString(localClientNum, va("%i", score->score), dx, dy, width, scoreitemList->iAlignment, font, 0.35, 3, expandedColor);
				}
				break;
			case LCT_DEATHS:
				if ( score->team != 3 )
				{
					CG_DrawListString(localClientNum, va("%i", score->deaths), dx, dy, width, scoreitemList->iAlignment, font, 0.35, 3, expandedColor);
				}
				break;
			case LCT_PING:
				if ( score->team != 3 )
				{
					CG_DrawListString(localClientNum, va("%i", score->ping), dx, dy, width, scoreitemList->iAlignment, font, 0.35, 3, expandedColor);
				}
				break;
			case LCT_STATUS_ICON:
				if(score->hStatusIcon == NULL)
				{
					break;
				}
				color[0] = 1.0;
				color[1] = 1.0;
				color[2] = 1.0;
				color[3] = argcolor[3];
				UI_DrawHandlePic(&scrPlaceView[0], sub_448570(scoreitemList->iAlignment, width, itemHeight) + dx, dy, itemHeight, itemHeight, 1, 0, color, score->hStatusIcon);
				break;
			case LCT_TALKING_ICON:
			    if ( CL_IsPlayerMuted(score->client) )
				{
					voiceimg = Material_RegisterHandle("voice_off", 0);

				}else if(CL_IsPlayerTalking(score->client)){
					voiceimg = Material_RegisterHandle("voice_on", 0);

				}else{
					break;
				}

				if ( voiceimg == NULL)
				{
					break;
				}
				color[0] = 1.0;
				color[1] = 1.0;
				color[2] = 1.0;
				color[3] = argcolor[3];
				UI_DrawHandlePic(&scrPlaceView[0], dx, dy, itemHeight, itemHeight, 1, 0, color, voiceimg);
				break;

			case LCT_KILLS:
				if ( score->team != 3 )
				{
					CG_DrawListString(localClientNum, va("%i", score->kills), dx, dy, width, scoreitemList->iAlignment, font, 0.35, 3, expandedColor);
				}
				break;
			case LCT_RANK_ICON:
				if(score->hRankIcon == NULL)
				{
					break;
				}
				color[0] = 1.0;
				color[1] = 1.0;
				color[2] = 1.0;
				color[3] = argcolor[3];

				sx = sub_448570(scoreitemList->iAlignment, width, itemHeight) + dx;
				sy = UI_TextHeight(font, sbRankFontScale) * 0.25 + dy;

				UI_DrawHandlePic(&scrPlaceView[0], sx, dy, itemHeight, itemHeight, 1, 0, color, score->hRankIcon);

				CG_DrawListString(localClientNum, CL_GetRankData(score->rank), itemHeight + sx, sy, width, scoreitemList->iAlignment, font, sbRankFontScale, 3, colorWhite);
				break;

			case LCT_ASSISTS:
				if ( score->team != 3 )
				{
					CG_DrawListString(localClientNum, va("%i", score->assists), dx, dy, width, scoreitemList->iAlignment, font, 0.35, 3, expandedColor);
				}
				break;
			case LCT_NUM:
				break;
		}

  }

  if ( cg_scoreboardPingGraph && cg_scoreboardPingGraph->boolean )
  {
    float pingW = cg_scoreboardPingWidth ? cg_scoreboardPingWidth->floatval : 0.0;
    CG_DrawClientPing(localClientNum, score->ping, dx, dy, pingW * listWidth, itemHeight);
  }
  return (float)itemHeight + dy;

}

static vec_t *CG_FadeColor(int timeNow, int startMsec, int totalMsec, int fadeMsec)
{
    int elapsed;
    static vec4_t color;

    elapsed = timeNow - startMsec;

    if (startMsec && elapsed < totalMsec)
    {
        if (fadeMsec > 0 && fadeMsec > totalMsec - elapsed)
            color[3] = (float)(totalMsec - elapsed) / (float)fadeMsec;
        else
            color[3] = 1.0;

        color[2] = 1.0;
        color[1] = 1.0;
        color[0] = 1.0;
        return color;
    }
    return NULL;
}

static void sub_449900()
{
  float cbheight;
  signed int v2;
  int i, v4;
  score_t *score;

  cbheight = CG_GetScoreboardHeight();
  int itemHeight = cg_scoreboardItemHeight ? cg_scoreboardItemHeight->integer : 0;
  float scoreboardH = cg_scoreboardHeight ? cg_scoreboardHeight->floatval : 0.0;
  v2 = (signed int)((scoreboardH + cbheight - 19.0 - (cbheight + 30.0) - 1.0 - (float)(cg_scoreboardBannerHeight ? cg_scoreboardBannerHeight->integer : 0) - (float)(itemHeight - 4)) / ((float)(itemHeight + 4.0)));

  v4 = 1;
  if ( cg.bgs.clientinfo[cg.clientNum].team != 3 )
  {
	for(i = 0, score = cg.scores; i < cg.numScores; ++i, ++score)
	{
	    if ( score->team == cg.bgs.clientinfo[cg.clientNum].team )
        {
          if ( score->client == cg.clientNum )
		  {
            break;
		  }
          ++v4;
        }
	}
    if ( v4 <= v2 )
    {
      cg.scoresOffBottom = 1;
      return;
    }
    v4 = v4 - v2 / 2 + 1;
  }
  cg.scoresOffBottom = v4;
}


float CG_DrawScoreboard_ListColumnHeaders(int localClientNum, float *color, float y, float h, float listWidth)
{
  Font_t *font;
  float dx, hstart;
  listColumnInfo_t *scoreitemList;
  int scoreitemCount;
  int i;
  float width, v24;
  float textwidth;
  const char* translated;

  int scoreboardFont = cg_scoreboardFont ? cg_scoreboardFont->integer : 0;
  float scoreboardFontScale = cg_scoreboardHeaderFontScale ? cg_scoreboardHeaderFontScale->floatval : 0.0;
  font = UI_GetFontHandle(&scrPlaceView[0], scoreboardFont, scoreboardFontScale * 0.85);

  hstart = CG_GetScoreboardHStart(localClientNum);

  dx = hstart + 3.0 + 2.0 + 4.0;

  if ( cg_scoreboardPingText && cg_scoreboardPingText->boolean )
  {
    scoreitemList = columnInfoWithPing;
    scoreitemCount = 9;
  }
  else
  {
    scoreitemList = columnInfo;
    scoreitemCount = 8;
  }
  for(i = 0, width = scoreitemList->fWidth * listWidth; i < scoreitemCount; ++i, scoreitemList++, dx += width, width = scoreitemList->fWidth * listWidth)
  {
    if ( scoreitemList->pszName[0] == '\0')
    {
		continue;
	}
	translated = UI_SafeTranslateString( scoreitemList->pszName);
	textwidth = UI_TextWidth(translated, 0, font, scoreboardFontScale * 0.85);
    v24 = (listWidth * scoreitemList->fWidth - textwidth) * 0.5;
    UI_DrawText(&scrPlaceView[0], translated, 0x7FFFFFFF, font, v24 + dx, y + h, 1, 0, 0.85 * scoreboardFontScale, color, 3);
  }
  return y + h + 4.0;
}



static float CG_DrawScoreboard_ListBanner(int localClientNum, float *color, float y, float w, team_t team)
{
    int scoreboardFont = cg_scoreboardFont ? cg_scoreboardFont->integer : 0;
    Font_t* font = UI_GetFontHandle(&scrPlaceView[localClientNum], scoreboardFont, 0.35);

    char translated[256] = {'\0'};
    const char *iconDvar = "";
    const char *teamstring = "";
    switch (team)
    {
    case TEAM_FREE:
        iconDvar = "g_TeamIcon_Free";
        break;

    case TEAM_RED:
        iconDvar = "g_TeamIcon_Axis";
        teamstring = Cvar_GetString("g_TeamName_Axis");
        break;

    case TEAM_BLUE:
        iconDvar = "g_TeamIcon_Allies";
        teamstring = Cvar_GetString("g_TeamName_Allies");
        break;

    case TEAM_SPECTATOR:
        iconDvar = "g_TeamIcon_Spectator";
        teamstring = "CGAME_SPECTATORS";
        break;

    case TEAM_NUM_TEAMS:
        return w + y + 4.0;
    }

    const char* iconString = Cvar_GetString(iconDvar);
    if (TEAM_RED <= team && team < TEAM_NUM_TEAMS)
        Q_strncpyz(translated, SEH_LocalizeTextMessage((char *)teamstring, "scoreboard team name", 0), sizeof(translated));

    float x = CG_GetScoreboardHStart(localClientNum) + 3.0 + 2.0 + 4.0;
    Material* icon = Material_RegisterHandle(iconString, 0);
    if (!Material_IsDefault(icon))
    {
        UI_DrawHandlePic(&scrPlaceView[localClientNum], x, y, w, w, 1, 0, color, icon);
        x = w + 8.0 + x;
    }

    UI_DrawText(&scrPlaceView[localClientNum], translated, 0x7FFFFFFF, font, x, w + y, 1, 0, 0.35, color, 3);
    int tw = UI_TextWidth(translated, 0x7FFFFFFF, font, 0.35);
    UI_DrawText(&scrPlaceView[localClientNum], va("( %i )", cg.teamPlayers[team]), 0x7FFFFFFF, font, x + tw + 8, w + y, 1, 0, 0.35, color, 3);
    return w + y + 4.0;
}



void CG_TeamColor(team_t team, const char *prefix, float *color)
{
  const char *v3;
  char scoreColorString[64];

  switch ( team )
  {
    case 3:
      v3 = "%s_Spectator";
      break;
    case 0:
      v3 = "%s_Free";
      break;
    case 1:
      v3 = "%s_Axis";
      break;
    case 2:
      v3 = "%s_Allies";
      break;
    default:
      return;
  }
  Com_sprintf(scoreColorString, sizeof(scoreColorString), v3, prefix);
  Cvar_GetUnpackedColorByName(scoreColorString, color);

}



static float CG_DrawTeamOfClientScore(int localClientNum, float *argcolor, float y, team_t team, float listWidth, int *drawLine)
{
    int bannerHeight = cg_scoreboardBannerHeight ? cg_scoreboardBannerHeight->integer : 0;
    if(CG_CheckDrawScoreboardLine(drawLine, y, bannerHeight))
    {
      y = CG_DrawScoreboard_ListBanner(localClientNum, argcolor, y, bannerHeight, team);
    }
    vec4_t color;
    CG_TeamColor(team, "g_ScoresColor", color);
    color[3] = argcolor ? argcolor[3] : 0.0;

    int itemHeight = cg_scoreboardItemHeight ? cg_scoreboardItemHeight->integer : 0;
    for (int i = 0; i < cg.numScores; ++i)
    {
        score_t* score = &cg.scores[i];
        if (!score)
            continue;

        if (!cg.bgs.clientinfo[score->client].infoValid || score->team != team)
            continue;

        if (CG_CheckDrawScoreboardLine(drawLine, y, itemHeight))
        {
          y = CG_DrawClientScore(localClientNum, color, y, score, listWidth) + 4.0;
        }
    }
    return y;
}



void CG_DrawScrollbar(float *argcolor, int localClientNum, float top)
{
	int v4;
	float v9;
	float v10;
	float v11;
	float y;
	float v29;
	float v31;
	float v32;
	float v33;

	Material *white;
	Material *uparrow;
	Material *upkey;
	Material *downarrow;
	Material *downkey;
	Material *black;

	vec4_t color;

	if ( cg.scoresOffBottom <= 1 && !cg.scoresBottom)
	{
		return;
	}

    v4 = cg.numScores;
    if ( cg.teamPlayers[0] )
      ++v4;

    if ( cg.teamPlayers[1] )
      ++v4;

	if ( cg.teamPlayers[2] )
      ++v4;

	if ( cg.teamPlayers[3] )
      ++v4;

    color[0] = argcolor[0];
    color[1] = argcolor[1];
    color[2] = argcolor[2];
	color[3] = argcolor[3] * 0.5;

    black = Material_RegisterHandle("black", 0);

    float pingW = cg_scoreboardPingWidth ? cg_scoreboardPingWidth->floatval : 0.0;
    float scoreboardW = cg_scoreboardWidth ? cg_scoreboardWidth->floatval : 0.0;
    v9 = (scoreboardW - 6.0 - 4.0 - 8.0) * (pingW + 1.0) + CG_GetScoreboardHStart(localClientNum) + 3.0 + 2.0 + 4.0 + 8.0 + 4.0;
    float scoreboardH = cg_scoreboardHeight ? cg_scoreboardHeight->floatval : 0.0;
    v10 = CG_GetScoreboardHeight() + scoreboardH - 3.0 - 2.0 - 14.0 - 1.0 - top - 1.0;
    UI_DrawHandlePic(&scrPlaceView[localClientNum], v9, top, 8.0, v10, 1, 0, color, black);
    v32 = top + 1.0;
    v31 = v10 - 2.0;
    if ( v4 )
    {
      v11 = (double)v4;
      v32 = (double)(cg.scoresOffBottom - 1) / v11 * v31 + v32;
      v31 = v31 * ((double)(cg.bannerLines[0] - cg.scoresOffBottom + 1) / v11);
    }

    white = Material_RegisterHandle("white", 0);

    color[3] = argcolor[3] * 0.25;
    UI_DrawHandlePic(&scrPlaceView[localClientNum], v9 + 1.0, v32, 6.0, v31, 1, 0, color, white);
    color[3] = argcolor[3];
    if ( cg.scoresOffBottom > 1 )
    {
      uparrow = Material_RegisterHandle("hudscoreboardscroll_uparrow", 0);
      v29 = (scoreboardW - 6.0 - 4.0 - 8.0) * (pingW + 1.0) + CG_GetScoreboardHStart(localClientNum) + 3.0 + 2.0 + 4.0 + 8.0 + 8.0 + 8.0 + 2.0 + 0.0;
      UI_DrawHandlePic(&scrPlaceView[localClientNum], v29, top, 16.0, 16.0, 1, 0, color, uparrow);

      upkey = Material_RegisterHandle("hudscoreboardscroll_upkey", 0);
      y = top + 18.0;

      UI_DrawHandlePic(&scrPlaceView[localClientNum], v29 - 0.0, y, 16.0, 16.0, 1, 0, color, upkey);
    }
    if ( cg.scoresBottom )
    {
      downarrow = Material_RegisterHandle("hudscoreboardscroll_downarrow", 0);

      v29 = (scoreboardW - 6.0 - 4.0 - 8.0) * (pingW + 1.0) + CG_GetScoreboardHStart(localClientNum) + 3.0 + 2.0 + 4.0 + 8.0 + 8.0 + 8.0 + 2.0 + 0.0;

      v33 = CG_GetScoreboardHeight() + scoreboardH - 3.0 - 2.0 - 14.0 - 1.0 - 1.0 - 16.0;

      UI_DrawHandlePic(&scrPlaceView[localClientNum], v29, v33, 16.0, 16.0, 1, 0, color, downarrow);

      downkey = Material_RegisterHandle("hudscoreboardscroll_downkey", 0);

      UI_DrawHandlePic(&scrPlaceView[localClientNum], v29 - 0.0, v33 - 18.0, 16.0, 16.0, 1, 0, color, downkey);

	}


}

static void CG_DrawScoreboard_ScoresList(float alpha)
{
    if (!cg.numScores)
        return;

    vec4_t color;
    color[0] = 1.0;
    color[1] = 1.0;
    color[2] = 1.0;
    color[3] = alpha;

    cg.scoresBottom = 0;

    float scoreboardW = cg_scoreboardWidth ? cg_scoreboardWidth->floatval : 0.0;
    float listWidth = scoreboardW - 6.0 - 4.0 - 8.0;
    float y = CG_GetScoreboardHeight() + 3.0 + 2.0 + 24.0 + 1.0 + (cg_scoreboardItemHeight ? cg_scoreboardItemHeight->integer : 0) + 4.0 + 15.0;
    float sy = y;
    int bannerHeight = cg_scoreboardBannerHeight ? cg_scoreboardBannerHeight->integer : 0;
    if (cg.scoresOffBottom <= 1)
        CG_DrawScoreboard_ListColumnHeaders(0, color, y, bannerHeight, listWidth);
    else
        y = CG_DrawScoreboard_ListColumnHeaders(0, color, y, bannerHeight, listWidth);

    int drawLine = 1;
    if (cg.teamPlayers[1] || cg.teamPlayers[2])
    {
        int myteam = cg.bgs.clientinfo[cg.clientNum].team;

        if (myteam != TEAM_RED && myteam != TEAM_BLUE)
            myteam = TEAM_BLUE;

        y = CG_DrawTeamOfClientScore(0, color, y, myteam, listWidth, &drawLine);
        y += 4.0;
        y = CG_DrawTeamOfClientScore(0, color, y, (2 - (myteam != 1)), listWidth, &drawLine) + 4.0;
    }

    if (cg.teamPlayers[0])
        y = CG_DrawTeamOfClientScore(0, color, y, 0, listWidth, &drawLine) + 4.0;

    if (cg.teamPlayers[3])
        CG_DrawTeamOfClientScore(0, color, y, TEAM_SPECTATOR, listWidth, &drawLine);

    cg.bannerLines[0] = drawLine - 1;

    CG_DrawScrollbar(color, 0, bannerHeight + sy);
}

int REGPARM(1) CG_DrawScoreboard(int a1)
{
    float alpha;
    vec_t *fadecolor;

    if (cg_paused && cg_paused->boolean)
        return 0;

    if (cg.scoreFadeTime)
        alpha = 1.0;
    else
    {
        fadecolor = CG_FadeColor(cg.time, cg.scoresTop, 100, 100);
        if (!fadecolor)
            return 0;

        alpha = fadecolor[3];
    }

    if (cg.scoresRequestTime + 2000 < cg.time)
    {
        cg.scoresRequestTime = cg.time;
        CL_AddReliableCommand("score");
    }

    if (cg.scoresOffBottom <= 0)
        sub_449900();

    CG_DrawScoreboardServerNameAddress(alpha);
    CG_DrawScoreboard_ScoresList(alpha);
    return 1;
}



void sub_449AE0(cg_t *this)
{
  int v1;

  if ( this && this->scoresBottom )
  {
    v1 = cg.numScores;
    if ( cg.teamPlayers[0] )
      v1 = cg.numScores + 1;
    if ( cg.teamPlayers[1] )
      ++v1;
    if ( cg.teamPlayers[2] )
      ++v1;
    if ( cg.teamPlayers[3] )
      ++v1;

    this->scoresOffBottom += cg_scoreboardScrollStep ? cg_scoreboardScrollStep->integer : 0;
    if ( this->scoresOffBottom > v1 )
      this->scoresOffBottom = v1;
  }
}

int REGPARM(1) Scoreboard_HandleInput(int a1)
{
    switch (a1)
    {
    case 164:
    case 184:
    case 206:
        if (cg.scoresOffBottom > 1)
        {
            cg.scoresOffBottom -= cg_scoreboardScrollStep ? cg_scoreboardScrollStep->integer : 1;
            if (cg.scoresOffBottom < 2)
                cg.scoresOffBottom = 1;
        }
        return 1;

    case 163:
    case 190:
    case 205:
        sub_449AE0(&cg);
        return 1;

    default:
        break;
    }
    return 0;
}
/*
void UpdateScores()
{
	cg.scoresTop = cg.time;
}


void __cdecl CG_ScoresDown_f()
{
  Com_Printf("+scores\n");
  UpdateScores();
  if (UI_GetActiveMenu() != 10 )
  {
    UI_SetActiveMenuByName("scores");
  }
}

void CG_ScoresUp_f()
{
  if ( cg.scoreFadeTime )
  {
    cg.scoreFadeTime = 0;
    cg.scoresOffBottom = -1;
    cg.scoresTop = 0;
  }
  if ( UI_GetActiveMenu() == 10 )
  {
    Key_RemoveCatcher(-17);
    UI_CloseAllMenusInternal(0);
  }

}
*/
