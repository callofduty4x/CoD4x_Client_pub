
#include "q_shared.h"
#include "ui_shared.h"
#include "r_shared.h"



Font_t * UI_GetFontHandle(ScreenPlacement *place, int fontindex, float fontscale)
{
  float fontscalea;
  if ( fontindex == 2 )
	return uiMem.bigFont;
  if ( fontindex == 3 )
    return uiMem.extraSmallFont;
  if ( fontindex == 5 )
    return uiMem.font1;
  if ( fontindex == 6 )
    return uiMem.font2;
  
  fontscalea = place->scaleVirtualToReal[1] * fontscale;
  
  if ( fontindex == 4 )
  {
    if ( uiMem.ui_smallFont->floatval >= fontscalea )
      return uiMem.extraSmallFont;
    if ( uiMem.ui_bigFont->floatval <= fontscalea )
      return uiMem.font3;
    return uiMem.smallFont;
  }
  if ( uiMem.ui_smallFont->floatval >= fontscalea )
    return uiMem.extraSmallFont;
  if ( uiMem.ui_extraBigFont->floatval <= fontscalea )
    return uiMem.extraBigFont;
  if ( uiMem.ui_bigFont->floatval > fontscalea )
    return uiMem.smallFont;
  return uiMem.bigFont;
}

int UI_TextWidth(const char* text, int maxChars, Font_t *font, float scale)
{
  float normscale;

  normscale = R_NormalizedTextScale(font, scale);
  return (int)((float)R_TextWidth(text, maxChars, font) * normscale);
}


int UI_TextHeight(Font_t *font, float scale)
{
  float normscale;

  normscale = R_NormalizedTextScale(font, scale);
  return (int)((float)R_TextHeight(font) * normscale);
}