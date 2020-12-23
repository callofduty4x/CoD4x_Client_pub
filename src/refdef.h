#ifndef __REFDEF_H__
#define __REFDEF_H__

#include "xassets/gfximage.h"

/* 7008 */
#pragma pack(push, 4)
typedef struct 
{
  struct GfxImage *image;
  char samplerState;
  byte pad[3]; 
}GfxLightImage;
#pragma pack(pop)

/* 7009 */
typedef struct 
{
  const char *name;
  GfxLightImage attenuation;
  int lmapLookupStart;
}GfxLightDef;

/* 7010 */
typedef struct 
{
  char type;
  char canUseShadowMap;
  char unused[2];
  float color[3];
  float dir[3];
  float origin[3];
  float radius;
  float cosHalfFovOuter;
  float cosHalfFovInner;
  int exponent;
  unsigned int spotShadowIndex;
  GfxLightDef *def;
}GfxLight;


#pragma pack(push, 4)
typedef struct 
{
  byte enabled;
  byte pad[3];
  float bloomCutoff;
  float bloomDesaturation;
  float bloomIntensity;
  float radius;
}GfxGlow;
#pragma pack(pop)

/* 7106 */
#pragma pack(push, 4)
typedef struct 
{
  byte enabled;
  byte pad[3];
  float brightness;
  float contrast;
  float desaturation;
  byte invert;
  byte pad2[3]; 
  float tintDark[3];
  float tintLight[3];
}GfxFilm;
#pragma pack(pop)

typedef struct 
{
  float viewModelStart;
  float viewModelEnd;
  float nearStart;
  float nearEnd;
  float farStart;
  float farEnd;
  float nearBlur;
  float farBlur;
}GfxDepthOfField;

/* 7108 */
typedef struct refdef_s
{
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;
  float tanHalfFovX;
  float tanHalfFovY;
  float vieworg[3];
  float viewaxis[3][3];
  float viewOffset[3];
  int time;
  float zNear;
  float blurRadius;
  GfxDepthOfField dof;
  GfxFilm film;
  GfxGlow glow;
  GfxLight primaryLights[255];
  int field_4080;
  int field_4084;
  int field_4088;
  int field_408c;
  int field_4090;
  int localClientNum;
}refdef_t;

#endif