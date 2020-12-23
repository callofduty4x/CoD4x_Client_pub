#ifndef __FONT_H__
#define __FONT_H__
#include "material.h"
#include <d3d9.h>

/* 7043 */
#pragma pack(push, 2)
typedef struct
{
  uint16_t letter;
  char x0;
  char y0;
  char dx;
  char pixelWidth;
  char pixelHeight;
  char pad;
  float s0;
  float t0;
  float s1;
  float t1;
}Glyph;
#pragma pack(pop)

/* 7044 */
typedef struct Font_s
{
  const char *fontName;
  int pixelHeight;
  int glyphCount;
  Material *material;
  Material *glowMaterial;
  Glyph *glyphs;
}Font_t;

/*
typedef struct
{
  Font_t f;
  D3DXFontInfo dxf;
}TTFFontStorage_t;
typedef struct
{
  IDirect3DTexture9* glyph;
  float size;
  RECT rect;
  POINT pt;
  float glyphWidth;
  float glyphHeight;
}GlyphInfo_t;

//Basic settings for ttf fonts
typedef struct
{
  struct ID3DXFont* h;
  UINT Weight;
  UINT MipLevels;
  BOOL Italic;
  DWORD CharSet;
  DWORD OutputPrecision;
  DWORD PitchAndFamily;
  char fontName[32];
  WORD QuestionGlyphIndex; //if the character is not available we use ?
}D3DXFontInfo;
*/

void Font_InitTTF();
struct Font_s* Font_GetSystemFont();
struct Font_s* Font_GetConsoleFont();
void Font_InitSystemFonts();

#endif
