#ifndef __R_SHARED_H__
#define __R_SHARED_H__

#include "../q_shared.h"

struct Material;

/*
typedef struct
{
  unsigned int sceneWidth;
  unsigned int sceneHeight;
  unsigned int displayWidth;
  unsigned int displayHeight;
  int displayFrequency;
  int isFullscreen;
  float aspectRatioWindow;
  float aspectRatioScenePixel;
  float aspectRatioDisplayPixel;
  unsigned int maxTextureSize;
  unsigned int maxTextureMaps;
  byte deviceSupportsGamma;
  byte pad0[3];
}vidConfig_t;*/

#define _clip2domain(v, min, max) (v > max ? max : (v < min ? min : v))


void R_SyncRenderThread();
void R_WaitWorkerCmds();
extern cvar_t* r_reflectionProbeGenerate;
extern cvar_t* r_fullscreen;
extern cvar_t* vid_xpos;
extern cvar_t* vid_ypos;


void R_EndRemoteScreenUpdate(void (*pumpfunc)());
void R_BeginRemoteScreenUpdate();
bool R_CheckLostDevice();

/************************
Fonts, texts
*************************/
struct Font_s;

int     R_TextWidth(const char *text, int maxChars, Font_s *font);
void    R_AddCmdDrawText(const char *text, int maxChars, Font_s *font, float x, float y, float xScale, float yScale, float rotation, const float *color, int style);
void    R_AddCmdDrawStretchPic(float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float *color, Material *material);
void    R_AddCmdDrawConsoleText(const char *textPool, int poolSize, int firstChar, int charCount, Font_s *font, float x, float y, float xScale, float yScale, const float *color, int style);
int     R_TextHeight(Font_s *font);
#endif