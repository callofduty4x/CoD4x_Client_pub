#ifndef __R_SHARED_H__
#define __R_SHARED_H__

#include "q_shared.h"
#include "xassets/material.h"
#include "xassets/gfximage.h"
#include "xassets/font.h"
#include "xassets/material.h"
#include "refdef.h"
#include <d3d9.h>
#include <stdbool.h>
//#include <d3dx9.h>

#define r_dx (*((DxGlobals_t*)(0xCC9A400)))
#define rg (*((struct r_globals_t*)(0xCC9D150)))
#define rgp (*((r_global_permanent_t*)(0xCC98280)))
#define _vidConfig (*((vidConfig_t*)(0x0CC9D0E0)))
#define scrPlaceView ((ScreenPlacement*)(0xE343D8))
#define scrPlaceFull (*(ScreenPlacement*)(0xE34420))
#define scrPlaceFullUnsafe (*(ScreenPlacement*)(0xE34468))
#define tess (*((materialCommands_t*)(0xD085EE0)))
#define gfxCmdBufState (*((struct GfxCmdBufState*)(0xD5404F0)))
#define pixelCostMode (*(int*)(0xD5EC684))
#define rdebugfilehandle (*(FILE**)(0xD5844BC))
#define MAX_WINDOWHANDLES 2
#define gfxCfg (*((struct GfxConfiguration*)(0xCC9D110)))
#define gfxCmdBufSourceState (*((struct GfxCmdBufSourceState*)(0xD53F5F0)))
#define r_glob (*((struct GfxGlobals*)(0xCC9D140)))



#define r_customMode getcvaradr(0xD569750)
#define r_aaSamples getcvaradr(0xD56968C)
#define r_loadForRenderer getcvaradr(0xD5696A4)
#define r_logFile getcvaradr(0xD5697F8)
#define r_aspectRatio getcvaradr(0xD569818)
#define r_wideScreen getcvaradr(0x1476F14)
#define r_vsync getcvaradr(0xD5695DC)

#pragma pack(push, 2)
struct GfxGlobals
{
  byte haveThreadOwnership;
  byte startedRenderThread;
  byte isRenderingRemoteUpdate;
  byte pad;
  volatile int screenUpdateNotify;
  volatile int remoteScreenUpdateNesting;
};
#pragma pack(pop)


struct GfxConfiguration
{
  unsigned int maxClientViews;
  unsigned int entCount;
  unsigned int entnumNone;
  unsigned int entnumOrdinaryEnd;
  int threadContextCount;
  int critSectCount;
  const char *codeFastFileName;
  const char *uiFastFileName;
  const char *commonFastFileName;
  const char *localizedCodeFastFileName;
  const char *localizedCommonFastFileName;
  const char *modFastFileName;
};


typedef IDirect3DBaseTexture9           D3DBaseTexture;
typedef IDirect3DTexture9               D3DTexture;
typedef IDirect3DVolumeTexture9         D3DVolumeTexture;
typedef IDirect3DCubeTexture9           D3DCubeTexture;
typedef IDirect3DSurface9				D3DSurface;

typedef struct
{
  D3DSurface *color;
  D3DSurface *depthStencil;
}GfxRenderTargetSurface_t;

typedef struct
{
  struct GfxImage *image;
  GfxRenderTargetSurface_t surface;
  unsigned int width;
  unsigned int height;
}GfxRenderTarget_t;

typedef struct
{
  unsigned int Flags;
  unsigned int PrimarySize;
  void *pPrimary;
  unsigned int SecondarySize;
  void *pSecondary;
  unsigned int SegmentCount;
}_D3DRING_BUFFER_PARAMETERS;

typedef struct
{
  HWND windowHandle;
  IDirect3DSwapChain9 *d3d9swpchain;
  int width;
  int height;
}rWindowProp_t;

typedef struct
{
  int numWindows;
  rWindowProp_t windows[MAX_WINDOWHANDLES];
}rWindowsState_t;

#define MAX_ADAPTERMODES 256

typedef struct DxGlobals_s
{
  struct HINSTANCE__ *hinst;
  IDirect3D9 *d3d9;
  IDirect3DDevice9 *device;
  unsigned int adapterIndex;
  byte adapterNativeIsValid;
  byte pad[3];
  int adapterNativeWidth;
  int adapterNativeHeight;
  int adapterFullscreenWidth;
  int adapterFullscreenHeight;
  D3DFORMAT depthStencilFormat;
  int numAdapterModes;
  D3DDISPLAYMODE adapterModes[MAX_ADAPTERMODES];
  const char* resolutionStrings[MAX_ADAPTERMODES];
  int numunk;
  const char* refreshRateStrings[MAX_ADAPTERMODES];
  int unk3;
  char modeStringList[2048];
  byte unkbig[3108];
  qboolean multigpu;
  D3DMULTISAMPLE_TYPE multiSampleType;
  unsigned int multiSampleQuality;
  D3DSurface *singleSampleDepthStencilSurface;
  D3DTexture *frontBufferTexture;

  byte inScene; //bool
  byte padb[3];
  int windowCount;
  rWindowsState_t rWindowsState;

  D3DTEXTUREFILTERTYPE linearNonMippedMinFilter;
  D3DTEXTUREFILTERTYPE linearNonMippedMagFilter;
  D3DTEXTUREFILTERTYPE linearMippedMinFilter;
  D3DTEXTUREFILTERTYPE linearMippedMagFilter;
  D3DTEXTUREFILTERTYPE anisotropicMinFilter;
  D3DTEXTUREFILTERTYPE anisotropicMagFilter;
  int linearMippedAnisotropy;
  int anisotropyFor2x;
  int anisotropyFor4x;
  int mipFilterMode;
  unsigned int mipBias;
  _D3DRING_BUFFER_PARAMETERS ringBufferParameters;
  unsigned int swapFence;
  volatile int showDirtyDiscError;
  int field_2CD4;
  int field_2CD8;
  int field_2CDC;
}DxGlobals_t;




typedef struct GfxWindowParms
{
  HWND hwnd;
  int hz;
  byte fullscreen;
  byte pad[3];
  int displayXPos;
  int displayYPos;
  int sceneWidth;
  int sceneHeight;
  int displayWidth;
  int displayHeight;
  int aaSamples;
  int monitorWidth;
  int monitorHeight;
  int monitorX;
  int monitorY;
}GfxWindowParms_t;

#pragma pack(push, 1)

typedef struct
{
  float field_0[16];
}matrix4x4_t;


#pragma pack(pop)


typedef struct {
	Material* sortedMaterials[2048];
	int needSortMaterials;
	int materialCount;

	struct GfxImage* whiteImage;
	struct GfxImage* blackImage;
	struct GfxImage* black3dImage;
	struct GfxImage* blackCubeImage;
	struct GfxImage* grayImage;
	struct GfxImage* identityNormalMapImage;
	struct GfxImage* specularityImage; //potentially unused
	struct GfxImage* outdoorImage; //potentially unused
	struct GfxImage* pixelCostColorCodeImage;

	struct GfxLightDef* dlightDef;

	Material* defaultMaterial; //b0
	Material* whiteMaterial;
	Material* additiveMaterial;
	Material* pointMaterial;
	Material* lineMaterial; //c0
	Material* lineMaterialNoDepth;
	Material* clearAlphaStencilMaterial;
	Material* shadowClearMaterial;
	Material* shadowCookieOverlayMaterial;//d0
	Material* shadowCookieBlurMaterial;
	Material* shadowCasterMaterial;
	Material* shadowOverlayMaterial;
	Material* depthPrepassMaterial;//e0
	Material* glareBlindMaterial;
	Material* pixelCostAddDepthAlwaysMaterial;
	Material* pixelCostAddDepthDisableMaterial;
	Material* pixelCostAddDepthEqualMaterial;//f0
	Material* pixelCostAddDepthLessMaterial;
	Material* pixelCostAddDepthWriteMaterial;
	Material* pixelCostAddNoDepthWriteMaterial;
	Material* pixelCostColorCodeMaterial;//00
	Material* stencilShadowMaterial;
	Material* stencilDisplayMaterial;
	Material* floatZDisplayMaterial;
	Material* colorChannelMixerMaterial;//10
	Material* frameColorDebugMaterial;
	Material* frameAlphaDebugMaterial;//18

	int usednullvar; //0x31c

	struct GfxWorld_s* world; //0x320

	Material* feedbackReplaceMaterial;//24
	Material* feedbackBlendMaterial;
	Material* feedbackFilmBlendMaterial;//2c
	Material* cinematicMaterial;
	Material* dofDownsampleMaterial;
	Material* dofNearCocMaterial;
	Material* smallBlurMaterial;
	Material* postFxDofMaterial;//40
	Material* postFxDofColorMaterial;
	Material* postFxColorMaterial;
	Material* postFxMaterial; //34C
	Material* symmetricFilterMaterial[8]; //350
	Material* shellShockBlurredMaterial;//370
	Material* shellShockFlashedMaterial;
	Material* glowConsistentSetupMaterial; //378
	Material* glowApplyBloomMaterial; //37C

	int savedScreenTimes[32];
}r_global_permanent_t;





typedef struct
{
  int data;
  int field_4;
  int field_8;
  int field_C;
  int field_10;
  int field_14;
  int field_18;
  int field_1C;
  int field_20;
  int field_24;
  int field_28;
  int field_2C;
  int field_30;
  int field_34;
  int field_38;
  int field_3C;
  int field_40;
  int field_44;
  int field_48;
  int field_4C;
  int field_50;
}r_debug_global_t;

typedef struct
{
  float scaleVirtualToReal[2];
  float scaleVirtualToFull[2];
  float scaleRealToVirtual[2];
  float virtualViewableMin[2];
  float virtualViewableMax[2];
  float realViewportSize[2];
  float realViewableMin[2];
  float realViewableMax[2];
  float subScreenLeft;
}ScreenPlacement;



/* 7636 */
typedef struct
{
  int x;
  int y;
  int width;
  int height;
}GfxViewport;

/* 6918 */
typedef union
{
  unsigned int packed;
  uint8_t array[4];
}GfxColor;

/* 8449 */
typedef struct
{
  float xyz[3];
  uint8_t color[4];
}GfxPointVertex;



typedef enum
{
  R_WARN_FRONTEND_ENT_LIMIT = 0x0,
  R_WARN_KNOWN_MODELS = 0x1,
  R_WARN_KNOWN_SPECIAL_MODELS = 0x2,
  R_WARN_MODEL_LIGHT_CACHE = 0x3,
  R_WARN_SCENE_ENTITIES = 0x4,
  R_WARN_TEMP_SKIN_BUF_SIZE = 0x5,
  R_WARN_MAX_SKINNED_CACHE_VERTICES = 0x6,
  R_WARN_MAX_SCENE_SURFS_SIZE = 0x7,
  R_WARN_PORTAL_PLANES = 0x8,
  R_WARN_MAX_CLOUDS = 0x9,
  R_WARN_MAX_DLIGHTS = 0xA,
  R_WARN_SMODEL_LIGHTING = 0xB,
  R_WARN_MAX_DRAWSURFS = 0xC,
  R_WARN_GFX_CODE_MESH_LIMIT = 0xD,
  R_WARN_GFX_MARK_MESH_LIMIT = 0xE,
  R_WARN_MAX_SCENE_DRAWSURFS = 0xF,
  R_WARN_MAX_FX_DRAWSURFS = 0x10,
  R_WARN_NONEMISSIVE_FX_MATERIAL = 0x11,
  R_WARN_NONLIGHTMAP_MARK_MATERIAL = 0x12,
  R_WARN_PRIM_DRAW_SURF_BUFFER_SIZE = 0x13,
  R_WARN_CMDBUF_OVERFLOW = 0x14,
  R_WARN_MISSING_DECL_NONDEBUG = 0x15,
  R_WARN_MAX_DYNENT_REFS = 0x16,
  R_WARN_MAX_SCENE_DOBJ_REFS = 0x17,
  R_WARN_MAX_SCENE_MODEL_REFS = 0x18,
  R_WARN_MAX_SCENE_BRUSH_REFS = 0x19,
  R_WARN_MAX_CODE_INDS = 0x1A,
  R_WARN_MAX_CODE_VERTS = 0x1B,
  R_WARN_MAX_CODE_ARGS = 0x1C,
  R_WARN_MAX_MESH_INDEX = 0x1D,
  R_WARN_MAX_VERT_LIMIT = 0x1E,
  R_WARN_DEBUG_ALLOC = 0x1F,
  R_WARN_SPOT_LIGHT_LIMIT = 0x20,
  R_WARN_FX_ELEM_LIMIT = 0x21,
  R_WARN_WORKER_CMD_SIZE = 0x22,
  R_WARN_UNKNOWN_STATICMODEL_SHADER = 0x23,
  R_WARN_UNKNOWN_XMODEL_SHADER = 0x24,
  R_WARN_DYNAMIC_INDEX_BUFFER_SIZE = 0x25,
  R_WARN_TOO_MANY_LIGHT_GRID_POINTS = 0x26,
  R_WARN_FOGABLE_MATERIAL_2D_GLYPH = 0x27,
  R_WARN_FOGABLE_MATERIAL_2D_TEXT = 0x28,
  R_WARN_COUNT = 0x29
}GfxWarningType;



/* 6920 */
typedef union
{
  unsigned int packed;
}PackedUnitVec;

/* 7667 */
typedef struct
{
  float xyzw[4];
  GfxColor color;
  float texCoord[2];
  PackedUnitVec normal;
}GfxVertex;


/* 874 */
typedef enum
{
  VERTDECL_GENERIC = 0x0,
  VERTDECL_PACKED = 0x1,
  VERTDECL_WORLD = 0x2,
  VERTDECL_WORLD_T1N0 = 0x3,
  VERTDECL_WORLD_T1N1 = 0x4,
  VERTDECL_WORLD_T2N0 = 0x5,
  VERTDECL_WORLD_T2N1 = 0x6,
  VERTDECL_WORLD_T2N2 = 0x7,
  VERTDECL_WORLD_T3N0 = 0x8,
  VERTDECL_WORLD_T3N1 = 0x9,
  VERTDECL_WORLD_T3N2 = 0xA,
  VERTDECL_WORLD_T4N0 = 0xB,
  VERTDECL_WORLD_T4N1 = 0xC,
  VERTDECL_WORLD_T4N2 = 0xD,
  VERTDECL_POS_TEX = 0xE,
  VERTDECL_COUNT = 0xF,
}MaterialVertexDeclType;


/* 8388 */
typedef struct __attribute__((aligned(16)))
{
  GfxVertex verts[5450];
  unsigned __int16 indices[1024*1024];
  MaterialVertexDeclType vertDeclType;
  unsigned int vertexSize;
  int indexCount;
  int vertexCount;
  int firstVertex;
  int lastVertex;
}materialCommands_t;


enum GfxRenderTargetId
{
  R_RENDERTARGET_SAVED_SCREEN = 0x0,
  R_RENDERTARGET_FRAME_BUFFER = 0x1,
  R_RENDERTARGET_SCENE = 0x2,
  R_RENDERTARGET_RESOLVED_POST_SUN = 0x3,
  R_RENDERTARGET_RESOLVED_SCENE = 0x4,
  R_RENDERTARGET_FLOAT_Z = 0x5,
  R_RENDERTARGET_DYNAMICSHADOWS = 0x6,
  R_RENDERTARGET_SHADOWCOOKIE = 0x7,
  R_RENDERTARGET_POST_EFFECT_0 = 0x8,
  R_RENDERTARGET_POST_EFFECT_1 = 0x9,
  R_RENDERTARGET_SHADOWMAP_SUN = 0xA,
  R_RENDERTARGET_SHADOWMAP_SPOT = 0xB,
  R_RENDERTARGET_HALF_RESAMPLED_SCENE = 0xC,
  R_RENDERTARGET_COUNT = 0xD,
  R_RENDERTARGET_NONE = 0xE,
};

enum GfxDepthRangeType
{
  GFX_DEPTH_RANGE_SCENE = 0x0,
  GFX_DEPTH_RANGE_VIEWMODEL = 0x2,
  GFX_DEPTH_RANGE_FULL = 0xFFFFFFFF,
};

struct GfxCmdBufPrimStateStream
{
  unsigned int stride;
  struct D3DVertexBuffer *vb;
  unsigned int offset;
};


struct GfxCmdBufPrimState
{
  struct D3DDevice *device;
  struct D3DIndexBuffer *indexBuffer;
  MaterialVertexDeclType vertDeclType;
  struct GfxCmdBufPrimStateStream streams[2];
  struct D3DVertexDeclaration *vertexDecl;
};


#pragma pack(push, 4)
struct GfxCmdBufState
{
  char refSamplerState[16];
  unsigned int samplerState[16];
  union GfxTexture *samplerTexture[16];
  struct GfxCmdBufPrimState prim;
  Material *material;
  MaterialTechniqueType techType;
  struct MaterialTechnique *technique;
  struct MaterialPass *pass;
  unsigned int passIndex;
  enum GfxDepthRangeType depthRangeType;
  float depthRangeNear;
  float depthRangeFar;
  uint64_t vertexShaderConstState[32];
  uint64_t pixelShaderConstState[256];
  byte alphaRef;
  char pad[3];
  unsigned int refStateBits[2];
  unsigned int activeStateBits[2];
  struct MaterialPixelShader *pixelShader;
  struct MaterialVertexShader *vertexShader;
  GfxViewport viewport;
  enum GfxRenderTargetId renderTargetId;
  Material *origMaterial;
  MaterialTechniqueType origTechType;
};
#pragma pack(pop)

struct GfxMatrix
{
  float m[4][4];
};


struct GfxCodeMatrices
{
  struct GfxMatrix matrix[32];
};

struct GfxViewParms
{
  struct GfxMatrix viewMatrix;
  struct GfxMatrix projectionMatrix;
  struct GfxMatrix viewProjectionMatrix;
  struct GfxMatrix inverseViewProjectionMatrix;
  float origin[4];
  float axis[3][3];
  float depthHackNearClip;
  int pad[2];
};

#pragma pack(push, 1)
struct r_globals_t
{
  struct GfxViewParms identityViewParms;
  char gap_140[8332];
  GfxColor color_axis;
  GfxColor color_allies;
  int myteam;
  int field_21D8;
  char field_21DC[104];
  float field_2244;
  float field_2248;
  float field_224C;
  float field_2250;
  float field_2254;
  float field_2258;
  float field_225C;
  float field_2260;
  char field_2264[156];
};
#pragma pack(pop)


struct __declspec(aligned(8)) GfxCmdBufInput
{
  float consts[58][4];
  struct GfxImage *codeImages[27];
  char codeImageSamplerStates[27];
  __declspec(aligned(2)) struct GfxBackEndData *data;
};

struct GfxPlacement
{
  vec4_t quat;
  vec3_t origin;
};

struct GfxScaledPlacement
{
  struct GfxPlacement base;
  float scale;
};

enum GfxViewMode
{
  VIEW_MODE_NONE = 0x0,
  VIEW_MODE_3D = 0x1,
  VIEW_MODE_2D = 0x2,
  VIEW_MODE_IDENTITY = 0x3,
};

struct GfxSceneDef
{
  int time;
  float floatTime;
  float viewOffset[3];
};

enum GfxViewportBehavior
{
  GFX_USE_VIEWPORT_FOR_VIEW = 0x0,
  GFX_USE_VIEWPORT_FULL = 0x1,
};

struct __declspec(aligned(16)) GfxCmdBufSourceState
{
  struct GfxCodeMatrices matrices;
  struct GfxCmdBufInput input;
  struct GfxViewParms viewParms;
  struct GfxMatrix shadowLookupMatrix;
  unsigned __int16 constVersions[90];
  unsigned __int16 matrixVersions[8];
  float eyeOffset[4];
  unsigned int shadowableLightForShadowLookupMatrix;
  struct GfxScaledPlacement *objectPlacement;
  struct GfxViewParms *viewParms3D;
  unsigned int depthHackFlags;
  struct GfxScaledPlacement skinnedPlacement;
  int cameraView;
  enum GfxViewMode viewMode;
  struct GfxSceneDef sceneDef;
  GfxViewport sceneViewport;
  float materialTime;
  enum GfxViewportBehavior viewportBehavior;
  int renderTargetWidth;
  int renderTargetHeight;
  byte viewportIsDirty;
  __declspec(aligned(4)) unsigned int shadowableLightIndex;
};


void sub_60D7D0();
void sub_5F3FD0();
unsigned int R_GetAdapterInfo();
void sub_5F39C0(int adapter);
void sub_5F4140(int adapter);
void DB_ShutdownXAssets();

//#define r_reflectionProbeGenerate getcvaradr(0xCC9F5F8)
void R_BspGenerateReflections();
Material_t* R_GetDefaultShader();
void R_SetupModeCvars(UINT adapterIndex);
void R_AddCmdDrawText(const char *text, int maxChars, Font_t *font, float x, float y, float xScale, float yScale, float rotation, const float *color, int style);
void R_AddCmdDrawStretchPic(float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float *color, Material *material);
void R_AddCmdDrawTextWithEffects(const char *text, int maxChars, Font_t *font, float x, float y, float xScale, float yScale, float rotation, const float *color, int style, const float *glowColor, Material *fxMaterial, Material *fxMaterialGlow, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration);
void R_AddCmdProjectionSet2D(void);
long double R_NormalizedTextScale(Font_t *font, float scale);
void ScrPlace_ApplyRect(const ScreenPlacement *scrPlace, float *x, float *y, float *w, float *h, int horzAlign, int vertAlign);
int R_TextWidth(const char* text, int, Font_t* font);
int R_TextHeight(Font_t* font);
void R_TTFTextHeightWidth(const char *text, int maxChars, Font_t *font, float scale, int* outwidth, int* outheight);
qboolean R_GfxWorldLoaded();
void REGPARM(1) R_RenderScene(refdef_t *refdef);
void REGPARM(1) RB_DrawStretchPicRotate(Material_t *material, float x, float y, float w, float h, float s0, float t0, float s1, float t1, float sinAngle, float cosAngle, unsigned int color);
void R_WarnOncePerFrame(GfxWarningType type, ...);
void R_SyncGpuPF();

void *R_AddCmdDrawTTFText(const char *text, int maxChars, Font_t *font, float x, float y, float xScale, float yScale, float rotation, const float *color, int style);
void __cdecl RB_BeginSurface(Material *material, MaterialTechniqueType techType);
void __cdecl RB_DrawStretchPicRotate_New(Material *material, float x, float y, float w, float width, float height, float s0, float t0, float s1, float t1, float sinAngle, float cosAngle, unsigned int color /*, GfxPrimStatsTarget statsTarget*/);
int __cdecl SetupFadeinFXVars(const char *text, int maxLength, int renderFlags, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, bool *resultDrawRandChar, int *resultRandSeed, bool *resultDecaying, int *resultdecayTimeElapsed);
int __cdecl SetupTypewriterFXVars(const char *text, int maxLength, int renderFlags, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, bool *resultDrawRandChar, int *resultRandSeed, int *resultMaxLength, bool *resultDecaying, int *resultdecayTimeElapsed);
int __cdecl SetupPopInFXVars(const char *text, int maxLength, int renderFlags, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, float *sizeIncrease, int *resultRandSeed, int *resultMaxLength, bool *resultDecaying, int *resultdecayTimeElapsed);
int __cdecl SetupRedactTextFXVars(const char *text, int maxLength, int renderFlags, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, bool *resultDrawRandChar, int *resultRandSeed, int *resultMaxLength, bool *resultDecaying, int *resultdecayTimeElapsed);
int __cdecl SetupCOD7DecodeFXVars(const char *text, int maxLength, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, int *randSeed, int *resultRandomCharsLength, int *resultMaxLength, bool *resultDecaying, int *resultDecayTimeElapsed);
int __cdecl SetupPulseFXVars(const char *text, int maxLength, int renderFlags, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, bool *resultDrawRandChar, int *resultRandSeed, int *resultMaxLength, bool *resultDecaying, int *resultdecayTimeElapsed);
unsigned int __cdecl RB_ProcessLetterColor(unsigned int color, unsigned int letter);
void __cdecl RB_LookupColor(char c, char *color);
void __cdecl RotateXY(float cosAngle, float sinAngle, float pivotX, float pivotY, float x, float y, float *outX, float *outY);
signed int __cdecl ModulateByteColors(char colorA, char colorB);
void __cdecl GetShiftColor(GfxColor targetColor, int fxBirthTime, int fxDecayStart, int fxDecayDuration, GfxColor startColor, GfxColor *shiftColor);
void __cdecl DrawTextFxExtraCharacter(Material *material, int charIndex, float x, float y, float w, float h, float sinAngle, float cosAngle, unsigned int color);
Glyph *__cdecl R_GetCharacterGlyph(struct Font_s *font, unsigned int letter);
Material *__cdecl Material_FromHandle(Material *handle);
void __cdecl GlowColor(GfxColor *result, GfxColor baseColor, GfxColor forcedGlowColor, int renderFlags);
void __cdecl RB_DrawCursor(Material *material, char cursor, float x, float y, float sinAngle, float cosAngle, struct Font_s *font, float xScale, float yScale, unsigned int color);
double __cdecl RB_DrawHudIcon(const char *text, float x, float y, float sinAngle, float cosAngle, Font_t *font, float xScale, float yScale, unsigned int color);
qboolean __cdecl IsValidMaterialHandle(Material *const handle);
void __cdecl DrawText2D(const char *text, float x, float y, float w, struct Font_s *font, float xScale, float yScale, float sinAngle, float cosAngle, GfxColor color, int maxLength, int renderFlags, int cursorPos, char cursorLetter, float padding, GfxColor glowForcedColor, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, int fxRedactDecayStartTime, int fxRedactDecayDuration, Material *fxMaterial, Material *fxMaterialGlow);
void __cdecl ScrPlace_SetupFloatViewport(ScreenPlacement *scrPlace, float viewportX, float viewportY, float viewportWidth, float viewportHeight);
void __cdecl ScrPlace_SetupViewport(ScreenPlacement *scrPlace, int viewportX, int viewportY, int viewportWidth, int viewportHeight);
void R_BeginRegistration( );
struct Font_s *__cdecl R_RegisterFont_FastFile(const char *fontName);
qboolean R_GetMonitorDimensions(int *width, int *heigth, int *x, int *y);

#define CONTXTCMD_TYPE_HUDICON_FLIP 2
#define TEXT_RENDERFLAG_FX_DECODE 0x40


#endif
