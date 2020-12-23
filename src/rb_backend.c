#include "xassets/font.h"
#include "qcommon.h"
#include "r_shared.h"

#include <D3dx9core.h>
#include <stdbool.h>


int __cdecl SetupFadeinFXVars(const char *text, int maxLength, int renderFlags, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, bool *resultDrawRandChar, int *resultRandSeed, bool *resultDecaying, int *resultdecayTimeElapsed)
{
  int timeElapsed;
  int decayTimeElapsed;
  char decaying;

  decaying = 0;
  timeElapsed = gfxCmdBufSourceState.sceneDef.time - fxBirthTime;
  assert(timeElapsed >= 0);

  if ( timeElapsed <= fxDecayDuration + fxDecayStartTime )
  {
    decayTimeElapsed = 0;
    if ( timeElapsed <= fxBirthTime )
    {
      decaying = 1;
      decayTimeElapsed = fxDecayDuration - timeElapsed;
    }
    else if ( timeElapsed > fxDecayStartTime )
    {
      decaying = 1;
      decayTimeElapsed = timeElapsed - fxDecayStartTime;
    }
    *resultDrawRandChar = 0;
    *resultRandSeed = 0;
    *resultDecaying = decaying;
    *resultdecayTimeElapsed = decayTimeElapsed;
    return 1;
  }
  *resultDrawRandChar = 0;
  *resultRandSeed = 1;
  *resultDecaying = 0;
  *resultdecayTimeElapsed = 0;
  return 0;

}

int __cdecl SetupTypewriterFXVars(const char *text, int maxLength, int renderFlags, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, bool *resultDrawRandChar, int *resultRandSeed, int *resultMaxLength, bool *resultDecaying, int *resultdecayTimeElapsed)
{
  int timeElapsed; 
  int strLength; 
  int decayTimeElapsed;
  char decaying;
  decaying = 0;
  timeElapsed = gfxCmdBufSourceState.sceneDef.time - fxBirthTime;
  assert(timeElapsed >= 0);

  strLength = SEH_PrintStrlen(text);
  if ( strLength > maxLength )
  {
    strLength = maxLength;
  }
  if ( timeElapsed <= fxDecayDuration + fxDecayStartTime )
  {
    decayTimeElapsed = 0;
    if ( timeElapsed <= fxLetterTime * strLength )
    {
      assert(fxLetterTime);
      maxLength = timeElapsed / fxLetterTime;
    }
    else if ( timeElapsed > fxDecayStartTime )
    {
      decaying = 1;
      decayTimeElapsed = timeElapsed - fxDecayStartTime;
    }
    *resultDrawRandChar = 0;
    *resultRandSeed = 0;
    *resultMaxLength = maxLength;
    *resultDecaying = decaying;
    *resultdecayTimeElapsed = decayTimeElapsed;
    return 1;
  }
  *resultDrawRandChar = 0;
  *resultRandSeed = 1;
  *resultMaxLength = maxLength;
  *resultDecaying = 0;
  *resultdecayTimeElapsed = 0;
  return 0;
}


int __cdecl SetupPopInFXVars(const char *text, int maxLength, int renderFlags, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, float *sizeIncrease, int *resultRandSeed, int *resultMaxLength, bool *resultDecaying, int *resultdecayTimeElapsed)
{

  int timeElapsed;
  int strLength;
  int decayTimeElapsed; 
  char decaying;

  decaying = 0;
  timeElapsed = gfxCmdBufSourceState.sceneDef.time - fxBirthTime;
  assert(timeElapsed >= 0);
  strLength = SEH_PrintStrlen(text);
  if ( strLength > maxLength )
  {
    strLength = maxLength;
  }
  if ( timeElapsed <= fxDecayDuration + fxDecayStartTime )
  {
    decayTimeElapsed = 0;
    if ( timeElapsed <= fxLetterTime * strLength )
    {
      assert(fxLetterTime);
      maxLength = timeElapsed / fxLetterTime;
    }
    else if ( timeElapsed > fxDecayStartTime )
    {
      decaying = 1;
      decayTimeElapsed = timeElapsed - fxDecayStartTime;
    }
    *sizeIncrease = 2.0;
    *resultRandSeed = 0;
    *resultMaxLength = maxLength;
    *resultDecaying = decaying;
    *resultdecayTimeElapsed = decayTimeElapsed;
    return 1;
  }
  *resultRandSeed = 1;
  *resultMaxLength = maxLength;
  *resultDecaying = 0;
  *resultdecayTimeElapsed = 0;
  return 0;

}

int __cdecl SetupRedactTextFXVars(const char *text, int maxLength, int renderFlags, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, bool *resultDrawRandChar, int *resultRandSeed, int *resultMaxLength, bool *resultDecaying, int *resultdecayTimeElapsed)
{
  int timeElapsed;
  int decayTimeElapsed;

  decayTimeElapsed = 0;
  *resultDrawRandChar = 0;
  *resultRandSeed = 1;
  *resultMaxLength = maxLength;
  *resultDecaying = 0;
  *resultdecayTimeElapsed = 0;
  timeElapsed = gfxCmdBufSourceState.sceneDef.time - fxBirthTime;
  
  assert(timeElapsed >= 0);

  if ( timeElapsed > fxDecayDuration + fxDecayStartTime )
  {
    return 0;
  }
  if ( timeElapsed > fxDecayStartTime )
  {
    decayTimeElapsed = timeElapsed - fxDecayStartTime;
    *resultDecaying = 1;
  }
  *resultdecayTimeElapsed = decayTimeElapsed;
  return 1;
}

int __cdecl SetupCOD7DecodeFXVars(const char *text, int maxLength, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, int *randSeed, int *resultRandomCharsLength, int *resultMaxLength, bool *resultDecaying, int *resultDecayTimeElapsed)
{
  int timeElapsed;
  int strLength;
  strLength = SEH_PrintStrlen(text);
  if ( strLength > maxLength )
  {
    strLength = maxLength;
  }
  timeElapsed = gfxCmdBufSourceState.sceneDef.time - fxBirthTime;
  if ( gfxCmdBufSourceState.sceneDef.time - fxBirthTime <= fxDecayDuration + fxDecayStartTime )
  {
    if ( timeElapsed <= fxLetterTime * strLength || timeElapsed <= fxDecayStartTime )
    {
      *resultDecaying = 0;
      *resultDecayTimeElapsed = 0;
      *resultMaxLength = strLength;
      *resultRandomCharsLength = strLength - timeElapsed / fxLetterTime;
      *randSeed = gfxCmdBufSourceState.sceneDef.time / 50;
      RandWithSeed(randSeed);
      RandWithSeed(randSeed);
      return 1;
    }
    *resultDecaying = 1;
    *resultDecayTimeElapsed = timeElapsed - fxDecayStartTime;
    *resultMaxLength = maxLength;
    *resultRandomCharsLength = 0;
    return 1;

  }
  *resultDecaying = 1;
  *resultDecayTimeElapsed = 0;
  *resultMaxLength = maxLength;
  *resultRandomCharsLength = 0;
  return 0;
}

int __cdecl SetupPulseFXVars(const char *text, int maxLength, int renderFlags, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, bool *resultDrawRandChar, int *resultRandSeed, int *resultMaxLength, bool *resultDecaying, int *resultdecayTimeElapsed)
{
  int timeRemainder;
  int timeElapsed;
  int randSeed;
  int strLength;
  bool drawRandCharAtEnd;
  int decayTimeElapsed;
  bool decaying;
  int maxLengtha;

  if ( renderFlags & TEXT_RENDERFLAG_FX_DECODE )
  {
    drawRandCharAtEnd = 0;
    randSeed = 1;
    decaying = 0;
    decayTimeElapsed = 0;
    timeElapsed = gfxCmdBufSourceState.sceneDef.time - fxBirthTime;

    assert(timeElapsed >= 0);

    strLength = SEH_PrintStrlen(text);
    if ( strLength > maxLength )
    {
      strLength = maxLength;
    }
    if ( timeElapsed <= fxDecayDuration + fxDecayStartTime )
    {
      if ( timeElapsed < fxLetterTime * strLength )
      {
        assert(fxLetterTime);
        maxLengtha = timeElapsed / fxLetterTime;
        drawRandCharAtEnd = 1;
        timeRemainder = timeElapsed - fxLetterTime * (timeElapsed / fxLetterTime);
        if ( fxLetterTime / 4 )
        {
          timeRemainder /= fxLetterTime / 4;
        }
        randSeed = maxLengtha + timeRemainder + strLength + fxBirthTime;
        RandWithSeed(&randSeed);
        RandWithSeed(&randSeed);
        maxLength = maxLengtha + 1;
      }
      else if ( timeElapsed > fxDecayStartTime )
      {
        decaying = 1;
        randSeed = strLength + fxBirthTime;
        RandWithSeed(&randSeed);
        RandWithSeed(&randSeed);
        decayTimeElapsed = timeElapsed - fxDecayStartTime;
      }
      *resultDrawRandChar = drawRandCharAtEnd;
      *resultRandSeed = randSeed;
      *resultMaxLength = maxLength;
      *resultDecaying = decaying;
      *resultdecayTimeElapsed = decayTimeElapsed;
      return 1;
    }
    *resultDrawRandChar = 0;
    *resultRandSeed = 1;
    *resultMaxLength = maxLength;
    *resultDecaying = 0;
    *resultdecayTimeElapsed = 0;
    return 0;

  }
  *resultDrawRandChar = 0;
  *resultRandSeed = 1;
  *resultMaxLength = maxLength;
  *resultDecaying = 0;
  *resultdecayTimeElapsed = 0;
  return 1;

}


static const int color_table[17] =
{
  4278190080,
  4281545727,
  4278255360,
  4286644223,
  4294901760,
  4294967040,
  4294925567,
  4294967295,
  4294967295,
  4294967295,
  4278190296,
  2013313084,
  4280128759,
  4291131648,
  4291346093,
  4287973248,
  4278213029
};

char __cdecl ColorIndex(char c)
{
  if ( (signed int)(unsigned __int8)(c - '0') >= 17 )
  {
    c = 7;
  }
  else
  {
    c = c - '0';
  }
  return c;
}

void __cdecl RB_LookupColor(char c, char *color)
{
  int ci;

  if ( c == '8' )
  {
    *(uint32_t *)color = rg.color_allies.packed;
    if ( rg.myteam != 2 )
    {
        *(uint32_t *)color = rg.color_axis.packed;
    }
  }
  else if ( c == '9' )
  {
    *(uint32_t *)color = rg.color_axis.packed;
    if ( rg.myteam != 2 )
    {
      *(uint32_t *)color = rg.color_allies.packed;
    }
  }
  else
  {
    ci = ColorIndex(c);
    if ( ci >= 17u )
    {
      *(uint32_t *)color = -1;
    }
    else
    {
      *(uint32_t *)color = color_table[ci];
    }
  }
}


unsigned int __cdecl RB_ProcessLetterColor(unsigned int color, unsigned int letter)
{
  GfxColor argcolor;
  GfxColor newColor;

  if ( !ValidGamePadButtonIcon(letter) )
  {
    return color;
  }
  argcolor.packed = color;
  newColor.packed = -1;
  newColor.array[3] = argcolor.array[1];
  return newColor.packed;
}

void __cdecl RotateXY(float cosAngle, float sinAngle, float pivotX, float pivotY, float x, float y, float *outX, float *outY)
{
  *outX = (((x - pivotX) * cosAngle) + pivotX) - ((y - pivotY) * sinAngle);
  *outY = (((y - pivotY) * cosAngle) + pivotY) + ((x - pivotX) * sinAngle);
}


signed int __cdecl ModulateByteColors(char colorA, char colorB)
{
  return (signed int)((((float)(unsigned char)colorA / 255.0) * ((float)(unsigned char)colorB / 255.0)) * 255.0);
}

void __cdecl GetShiftColor(GfxColor targetColor, int fxBirthTime, int fxDecayStart, int fxDecayDuration, GfxColor startColor, GfxColor *shiftColor)
{
  float frac;
  int timeElapsed;

  timeElapsed = gfxCmdBufSourceState.sceneDef.time - fxBirthTime;

  assert(timeElapsed >= 0);

  if ( timeElapsed <= fxDecayDuration + fxDecayStart )
  {
    if ( timeElapsed <= fxDecayStart )
    {
      shiftColor->packed = startColor.packed;
    }
    else
    {
      frac = (float)(timeElapsed - fxDecayStart) / (float)fxDecayDuration;
      shiftColor->array[2] = (signed int)((float)startColor.array[2] + ((float)(targetColor.array[2] - startColor.array[2]) * frac));
      shiftColor->array[1] = (signed int)((float)startColor.array[1] + ((float)(targetColor.array[1] - startColor.array[1]) * frac));
      shiftColor->array[0] = (signed int)((float)startColor.array[0] + ((float)(targetColor.array[0] - startColor.array[0]) * frac));
      shiftColor->array[3] = startColor.array[3];
    }
  }
  else
  {
    shiftColor->packed = targetColor.packed;
  }
}

void __cdecl GlowColor(GfxColor *result, GfxColor baseColor, GfxColor forcedGlowColor, int renderFlags)
{
  if ( renderFlags & 0x20 )
  {
    result->array[2] = forcedGlowColor.array[2];
    result->array[1] = forcedGlowColor.array[1];
    result->array[0] = forcedGlowColor.array[0];
  }
  else
  {
    result->array[2] = (signed int)(float)((float)(unsigned __int8)baseColor.array[2] * 0.059999999);
    result->array[1] = (signed int)(float)((float)(unsigned __int8)baseColor.array[1] * 0.059999999);
    result->array[0] = (signed int)(float)((float)(unsigned __int8)baseColor.array[0] * 0.059999999);
  }
}

void __cdecl DrawTextFxExtraCharacter(Material *material, int charIndex, float x, float y, float w, float h, float sinAngle, float cosAngle, unsigned int color)
{
  RB_DrawStretchPicRotate_New(material, x, y, 1.0, w, h, (float)(charIndex % 16) * 0.0625, 0.0, ((float)(charIndex % 16) * 0.0625) + 0.0625, 1.0, sinAngle, cosAngle, color/*, GFX_PRIM_STATS_HUD*/);
}

void __cdecl RB_DrawCursor(Material *material, char cursor, float x, float y, float sinAngle, float cosAngle, struct Font_s *font, float xScale, float yScale, unsigned int color)
{
  Glyph *cursorGlyph;
  unsigned int newColor;

  assert(font);

  if ( !(CL_ScaledMilliseconds() / 256 & 1) )
  {
    cursorGlyph = R_GetCharacterGlyph(font, (unsigned __int8)cursor);
    newColor = RB_ProcessLetterColor(color, (unsigned __int8)cursor);
    RB_DrawStretchPicRotate_New(material, x, ((float)cursorGlyph->y0 * yScale) + y, 1.0,
      (float)(unsigned __int8)cursorGlyph->pixelWidth * xScale, (float)(unsigned __int8)cursorGlyph->pixelHeight * yScale,
      cursorGlyph->s0, cursorGlyph->t0, cursorGlyph->s1, cursorGlyph->t1, sinAngle, cosAngle, newColor /*, GFX_PRIM_STATS_HUD*/);
  }
}


double __cdecl RB_DrawHudIcon(const char *text, float x, float y, float sinAngle, float cosAngle, Font_t *font, float xScale, float yScale, unsigned int color)
{
  Material *material;
  float s1;
  float s0;
  float h;
  float w;
  assert(text);

  if ( text[0] == 1 )
  {
    s0 = 0.0;
    s1 = 1.0;
  }
  else
  {
    assert(text[0] == CONTXTCMD_TYPE_HUDICON_FLIP);
    s0 = 1.0;
    s1 = 0.0;
  }
  w = (float)((font->pixelHeight * (text[1] - 16) + 16) / 32) * xScale;
  h = (float)((font->pixelHeight * (text[2] - 16) + 16) / 32) * yScale;
  y = y - (((float)font->pixelHeight * yScale) + h) * 0.5;
  
  assert(w > 0);
  assert( h > 0 );

  material = Material_FromHandle(*(Material **)(text + 3));

  if ( !IsValidMaterialHandle( material ))
  {
    return 0.0;
  }

  RB_DrawStretchPicRotate(material, x, y, w, h, s0, 0.0, s1, 1.0, sinAngle, cosAngle, color);
  return w;
}

