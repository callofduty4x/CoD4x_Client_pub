#ifndef __R_LOCAL_H__
#define __R_LOCAL_H__

#include "../q_shared.h"
#include <d3d9.h>
#include "r_types_d3d.h"
#include "r_shared.h"

typedef cvar_t dvar_s;

/*
  Error codes:
    0x88760869 = D3DERR_DEVICENOTRESET
    0x88760868 = D3DERR_DEVICELOST
*/
#define r_lightMap getcvaradr(0xD5696D0)


#define MAX_WINDOWHANDLES 1
#define MAX_ADAPTERMODES 256



struct GfxGlobals
{
  bool8 haveThreadOwnership;
  bool8 startedRenderThread;
  bool8 isRenderingRemoteUpdate;
  bool8 pad;
  volatile int screenUpdateNotify;
  volatile int remoteScreenUpdateNesting;
  int backEndFrameCount;
};


extern GfxGlobals r_glob;

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


extern GfxConfiguration gfxCfg;

struct GfxRenderTargetSurface
{
  D3DSurface *color;
  D3DSurface *depthStencil;
};

struct GfxRenderTarget
{
  struct GfxImage *image;
  GfxRenderTargetSurface surface;
  unsigned int width;
  unsigned int height;
};

typedef struct
{
  unsigned int Flags;
  unsigned int PrimarySize;
  void *pPrimary;
  unsigned int SecondarySize;
  void *pSecondary;
  unsigned int SegmentCount;
}_D3DRING_BUFFER_PARAMETERS;

struct GfxWindowTarget
{
  HWND hwnd;
  IDirect3DSwapChain9 *swapChain;
  int width;
  int height;
};


typedef struct DxGlobals_s
{
  struct HINSTANCE__ *hinst;
  IDirect3D9 *d3d9;
  IDirect3DDevice9 *device;
  unsigned int adapterIndex;
  bool8 adapterNativeIsValid;
  byte pad[3];
  int adapterNativeWidth;
  int adapterNativeHeight;
  int adapterFullscreenWidth;
  int adapterFullscreenHeight;
  D3DFORMAT depthStencilFormat;
  int displayModeCount;
  D3DDISPLAYMODE displayModes[MAX_ADAPTERMODES];
  const char* resolutionStrings[MAX_ADAPTERMODES];
  int numunk;
  const char* refreshRateStrings[MAX_ADAPTERMODES];
  int unk3;
  char modeStringList[2048];
  byte unkbig[3072];
  D3DQuery *fencePool[8];
  unsigned int nextFence;
  int gpuSync;
  D3DMULTISAMPLE_TYPE multiSampleType;
  unsigned int multiSampleQuality;
  unsigned int sunSpriteSamples;

  IDirect3DSurface9 *singleSampleDepthStencilSurface;

  bool8 deviceLost;
  bool8 inScene;
  byte padb[2];
  int targetWindowIndex;
  int windowCount;
  GfxWindowTarget windows[MAX_WINDOWHANDLES];

  IDirect3DQuery9 *flushGpuFence;
  uint64_t gpuSyncDelay;
  uint64_t gpuSyncStart;
  uint64_t gpuSyncEnd;
  bool8 flushGpuFenceIssued;
  byte pad2[3];
  _D3DTEXTUREFILTERTYPE linearNonMippedMinFilter;
  _D3DTEXTUREFILTERTYPE linearNonMippedMagFilter;
  _D3DTEXTUREFILTERTYPE linearMippedMinFilter;
  _D3DTEXTUREFILTERTYPE linearMippedMagFilter;
  _D3DTEXTUREFILTERTYPE anisotropicMinFilter;
  _D3DTEXTUREFILTERTYPE anisotropicMagFilter;
  int linearMippedAnisotropy;
  int anisotropyFor2x;
  int anisotropyFor4x;
  int mipFilterMode;
  unsigned int mipBias;
  D3DQuery* swapFence;
  volatile int showDirtyDiscError;
  int field_2CDC;
}DxGlobals_t;


extern DxGlobals_t r_dx;






enum CubemapShot
{
  CUBEMAPSHOT_NONE = 0x0,
  CUBEMAPSHOT_RIGHT = 0x1,
  CUBEMAPSHOT_LEFT = 0x2,
  CUBEMAPSHOT_BACK = 0x3,
  CUBEMAPSHOT_FRONT = 0x4,
  CUBEMAPSHOT_UP = 0x5,
  CUBEMAPSHOT_DOWN = 0x6,
  CUBEMAPSHOT_COUNT = 0x7,
};

enum GfxScreenshotType
{
  R_SCREENSHOT_JPG = 0x0,
  R_SCREENSHOT_TGA = 0x1,
};

enum GfxDepthRangeType
{
  GFX_DEPTH_RANGE_SCENE = 0x0,
  GFX_DEPTH_RANGE_VIEWMODEL = 0x2,
  GFX_DEPTH_RANGE_FULL = 0xFFFFFFFF,
};


struct GfxWindowParms
{
  HWND hwnd;
  int hz;
  byte fullscreen;
  byte pad[3];
  int x;
  int y;
  int sceneWidth;
  int sceneHeight;
  int displayWidth;
  int displayHeight;
  int aaSamples;
  int monitorWidth;
  int monitorHeight;
  int monitorX;
  int monitorY;
};


struct GfxDrawPrimArgs
{
  int vertexCount;
  int triCount;
  int baseIndex;
};

struct GfxCmdBufPrimStateStream
{
  unsigned int stride;
  D3DVertexBuffer *vb;
  unsigned int offset;
};


struct GfxCmdBufPrimState
{
  D3DDevice *device;
  D3DIndexBuffer *indexBuffer;
  MaterialVertexDeclType vertDeclType;
  GfxCmdBufPrimStateStream streams[2];
  D3DVertexDeclaration *vertexDecl;
};


#pragma pack(push, 4)
struct GfxCmdBufState
{
  unsigned char refSamplerState[16];
  unsigned int samplerState[16];
  const union GfxTexture *samplerTexture[16];
  GfxCmdBufPrimState prim;
  struct Material *material;
  MaterialTechniqueType techType;
  struct MaterialTechnique *technique;
  struct MaterialPass *pass;
  unsigned int passIndex;
  GfxDepthRangeType depthRangeType;
  float depthRangeNear;
  float depthRangeFar;
  uint64_t vertexShaderConstState[32];
  uint64_t pixelShaderConstState[256];
  byte alphaRef;
  char pad[3];
  unsigned int refStateBits[2];
  unsigned int activeStateBits[2];
  const struct MaterialPixelShader *pixelShader;
  const struct MaterialVertexShader *vertexShader;
  GfxViewport viewport;
  GfxRenderTargetId renderTargetId;
  struct Material *processMaterial;
  MaterialTechniqueType processTechType;
};
#pragma pack(pop)


#pragma pack(push, 4)
struct GfxCmdBufContext
{
  struct GfxCmdBufSourceState *source;
  GfxCmdBufState *state;
};
#pragma pack(pop)




struct GfxLodRamp
{
  float scale;
  float bias;
};

struct GfxLodParms
{
  float origin[3];
  GfxLodRamp ramp[2];
};



extern GfxCmdBufSourceState gfxCmdBufSourceState;

#pragma pack(push, 4)
struct GfxDrawSurfListArgs
{
  GfxCmdBufContext context;
  unsigned int firstDrawSurfIndex;
  GfxDrawSurfListInfo *info;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct r_globals_t
{
  GfxViewParms identityViewParms;
  bool8 registered;
  byte pad[3];
  float viewOrg[3];
  float viewDir[3];
  int frontEndFrameCount;
  char gap_140[8300];
  GfxColor color_axis;
  GfxColor color_allies;
  int myteam;
  int field_21D8;
  char field_21DC[28];
  GfxLodParms lodParms;
  bool8 hasAnyImageOverrides;
  char field_21[47];
  GfxScaledPlacement remotePlacement;
  char field_2264[12];
  const char *codeImageNames[TEXTURE_SRC_CODE_COUNT];
  unsigned int viewInfoCount;
  int sunShadowFull;
  float sunShadowmapScale;
  float sunShadowmapScaleNum;
  unsigned int sunShadowSize;
  float sunShadowPartitionRatio;
  int drawSunShadow;
  bool isNotRenderingFullScreen;
  int skinnedCacheReachedThreshold;
  int unknown;
};
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

#define rgp (*((r_global_permanent_t*)(0xCC98280)))
#define rg (*((struct r_globals_t*)(0xCC9D150)))




struct __attribute__((aligned(4))) GfxBuffers
{
  GfxDynamicIndices smodelCache;
  D3DVertexBuffer *smodelCacheVb;
  GfxIndexBufferState preTessIndexBufferPool[2];
  GfxIndexBufferState *preTessIndexBuffer;
  int preTessBufferFrame;
  GfxIndexBufferState dynamicIndexBufferPool[1];
  GfxIndexBufferState *dynamicIndexBuffer;
  GfxVertexBufferState skinnedCacheVbPool[1];
  char *skinnedCacheLockAddr;
  GfxVertexBufferState dynamicVertexBufferPool[1];
  GfxVertexBufferState *dynamicVertexBuffer;
  D3DVertexBuffer *particleCloudVertexBuffer;
  D3DIndexBuffer *particleCloudIndexBuffer;
  int dynamicBufferFrame;
  char _gfxBufExtentionUnknown[2359338];
};

extern GfxBuffers gfxBuf;
extern vidConfig_t vidConfig;


struct GfxViewInfo
{
  GfxViewParms viewParms;
  GfxSceneDef sceneDef;
  GfxViewport sceneViewport;
  GfxViewport displayViewport;
  GfxViewport scissorViewport;
  ShadowType dynamicShadowType;
  byte padding[8];
  ShadowCookieList shadowCookieList;
  int localClientNum;
  int isRenderingFullScreen;
  bool8 needsFloatZ;
  char pad[3];
  GfxLight shadowableLights[255];
  unsigned int shadowableLightCount;
  PointLightPartition pointLightPartitions[4];
  GfxMeshData pointLightMeshData[4];
  int pointLightCount;
  unsigned int emissiveSpotLightIndex;
  GfxLight emissiveSpotLight;
  int emissiveSpotDrawSurfCount;
  GfxDrawSurf *emissiveSpotDrawSurfs;
  unsigned int emissiveSpotLightCount;
  float blurRadius;
  float frustumPlanes[4][4];
  GfxDepthOfField dof;
  GfxFilm film;
  GfxGlow glow;
  const void *cmds;
  byte padding2[4];
  byte sunShadow[1184];
  unsigned int spotShadowCount;
  byte padding3[12];
  GfxSpotShadow spotShadows[4];
  char unknown[64];
  GfxQuadMeshData *fullSceneViewMesh;
  GfxDrawSurfListInfo litInfo;
  GfxDrawSurfListInfo decalInfo;
  GfxDrawSurfListInfo emissiveInfo;
  byte padding4[4];
  GfxCmdBufInput input;
};

#pragma pack(push, 8)
struct GfxSunShadowPartition
{
  GfxViewParms shadowViewParms;
  int partitionIndex;
  GfxViewport viewport;
  GfxDrawSurfListInfo info;
  GfxSunShadowBoundingPoly boundingPoly;
  char unknown[80];
};
#pragma pack(pop)



struct __align(4) GfxMetrics
{
  uint16_t cubemapShotRes;
  uint16_t cubemapShotPixelBorder;
  uint16_t feedbackWidth;
  uint16_t feedbackHeight;
  bool8 hasAnisotropicMinFilter;
  bool8 hasAnisotropicMagFilter;
  int16_t pad0;
  int maxAnisotropy;
  int maxClipPlanes;
  bool8 hasHardwareShadowmap;
  bool8 pad1[3];
  D3DFORMAT shadowmapFormatPrimary;
  D3DFORMAT shadowmapFormatSecondary;
  MaterialTechniqueType shadowmapBuildTechType;
  byte shadowmapSamplerState;
  bool8 slopeScaleDepthBias;
  bool8 canMipCubemaps;
  bool8 hasTransparencyMsaa;
};

struct GfxMeshGlobals
{
  GfxQuadMeshData fullSceneViewMesh[4];
  GfxMeshData spotShadowClearMeshData[4];
  GfxMeshData sunShadowClearMeshData[2];
};


extern GfxDrawMethod gfxDrawMethod;
extern GfxCmdBufContext gfxCmdBufContext;
extern GfxCmdBufState gfxCmdBufState;
extern GfxMetrics gfxMetrics;
extern GfxCmdBufInput gfxCmdBufInput;
extern GfxViewInfo g_viewInfo[4];
extern GfxMeshGlobals gfxMeshGlob;

struct DebugGlobals;


//void __noreturn R_FatalInitError(const char *msgfmt, ...);
void R_SetPixelShader(GfxCmdBufState*, MaterialPixelShader const*);
void R_AssertDXDeviceOwnership();
int R_AcquireDXDeviceOwnership(void (__cdecl *pumpfunc)());
int R_ReleaseDXDeviceOwnership();
void RB_LogPrint(const char *text);
void RB_LogPrintf(const char *fmt, ...);
void RB_LogPrintState_0(int stateBits0, int changedBits0);
void RB_LogPrintState_1(int stateBits1, int changedBits1);
void RB_LogPrintfDebug(const char *fmt, ...);
void RB_UpdateLogging();

const char * R_ErrorDescription(HRESULT hr);
void R_UploadWaterTexture(water_t *water, float floatTime);
GfxImage *R_OverrideGrayscaleImage(dvar_s *dvar);
void R_SetSampler(GfxCmdBufContext context, unsigned int samplerIndex, unsigned char samplerState, const GfxImage *image);
float * R_GetCodeMatrix(GfxCmdBufSourceState *source, unsigned int sourceIndex, unsigned int firstRow);
void R_SetupPassCriticalPixelShaderArgs(GfxCmdBufContext context);
void R_SetGameTime(GfxCmdBufSourceState *source, float gameTime);
void R_SetupPassVertexShaderArgs(GfxCmdBufContext context);
void R_ChangeDepthHackNearClip(GfxCmdBufSourceState *source, unsigned int depthHackFlags);
void R_UpdateVertexDecl(GfxCmdBufState *state);
void R_ChangeDepthRange(GfxCmdBufState *state, GfxDepthRangeType depthRangeType);
void R_SetupPassPerObjectArgs(GfxCmdBufContext context);
void R_DrawStaticModelSurfLit(const unsigned int *primDrawSurfPos, GfxCmdBufContext context, GfxCmdBufContext prepassContext, const GfxDrawSurfListInfo *info);
void R_DrawStaticModelSurf(const unsigned int *primDrawSurfPos, GfxCmdBufContext context, const GfxDrawSurfListInfo *info);
//void __noreturn R_FatalLockError(HRESULT hr);
int R_GetOptimizeVBTransfersValue();
void R_InitTempSkinBuf();
void R_ShutdownTempSkinBuf();
void R_SetupPassPerPrimArgs(GfxCmdBufContext context);
void R_SetupPass(GfxCmdBufContext context, unsigned int passIndex);
void R_DrawIndexedPrimitive(GfxCmdBufPrimState *state, const GfxDrawPrimArgs *args);
void R_ChangeIndices(GfxCmdBufPrimState *state, D3DIndexBuffer *ib);
void R_GetViewport(GfxCmdBufSourceState *source, GfxViewport *outViewport);
void R_SetViewport(GfxCmdBufState *state, const GfxViewport *viewport);
void R_UpdateViewport(GfxCmdBufSourceState *source, GfxViewport *viewport);


void RB_Resource_Lock();
void RB_Resource_Unlock();
void RB_Resource_Flush();
void RB_Resource_Release(IUnknown *resource);
r_resource_action * RB_Resource_AllocEntry();


void R_ChangeStreamSource(GfxCmdBufPrimState *state, int streamIndex, D3DVertexBuffer *vb, unsigned int vertexOffset, unsigned int vertexStride);
void R_SetDoubleStreamSource(GfxCmdBufPrimState *primState, D3DVertexBuffer *vb0, unsigned int vertexOffset0, unsigned int vertexStride0, D3DVertexBuffer *vb1, unsigned int vertexOffset1, unsigned int vertexStride1);
void R_SetStreamSource(GfxCmdBufPrimState *primState, D3DVertexBuffer *vb, unsigned int vertexOffset, unsigned int vertexStride);
void R_SetRenderTarget(GfxCmdBufContext context, GfxRenderTargetId newTargetId);
const char * R_RenderTargetName(GfxRenderTargetId renderTargetId);
void R_InitRenderTargets();
void R_ShutdownRenderTargets();
void R_InitRenderBuffers();
void R_ShutdownRenderBuffers();
void R_InitModelLightingImage();
void R_ShutdownModelLightingImage();
void R_InitStaticModelCache();
void R_ShutdownStaticModelCache();
void R_FlushStaticModelCache();
void R_CreateDynamicBuffers();
void R_DestroyDynamicBuffers();
void R_CreateParticleCloudBuffer();
void R_DestroyParticleCloudBuffer();
void R_Cinematic_Shutdown();
void R_Cinematic_Init();
void R_ShutdownModelLightingGlobals();
void R_FreeGlobalVariable(void *var);
void R_ShutdownDebugEntry(DebugGlobals *debugGlobalsEntry);
void R_ShutdownSceneBuffers();
void R_ShutdownDebug();
void R_SaveLightVisHistory();
void R_ShutdownLightDefs();
void R_ShutdownWorld();
void R_ShutdownLoadWater();
void R_ShutdownFonts();
void R_ResetModelLighting();
void R_UnregisterCmds();
void R_ShutdownRenderCommands();
void R_ShutdownDynamicIndices(GfxDynamicIndices*);

void R_InitCmdBufSourceState(GfxCmdBufSourceState *source, const GfxCmdBufInput *input, int cameraView);
void R_InitCmdBufState(GfxCmdBufState *state);
void R_SetRenderTargetSize(GfxCmdBufSourceState *source, GfxRenderTargetId newTargetId);
void R_SetViewportStruct(GfxCmdBufSourceState *source, const GfxViewport *viewport);
void R_DrawCall(void (*callback)(const void *, GfxCmdBufContext, GfxCmdBufContext), const void *userData, GfxCmdBufSourceState *source, const GfxViewInfo *viewInfo, const GfxDrawSurfListInfo *info, const GfxViewParms *viewParms, GfxCmdBuf *cmdBufEA, GfxCmdBuf *prepassCmdBufEA);
void RB_SetInitialState();
void R_InitScene();
void R_ClearAllStreamSources(GfxCmdBufPrimState *state);
void R_SetVertexShader(GfxCmdBufState *state, const MaterialVertexShader *vertexShader);
void R_ClearScreen(IDirect3DDevice9 *device, unsigned char whichToClear, const float *color, float depth, unsigned char stencil, const GfxViewport *viewport);
qboolean R_Is1900Card();
int R_HashAssetName(const char *name);
void R_ClearScreenInternal(D3DDevice*, unsigned char, const float *, float, unsigned char, GfxViewport const*);
void R_Set2D(GfxCmdBufSourceState *source);
void R_Set3D(GfxCmdBufSourceState *source);

void R_InitDrawMethod();
void R_InitSystems();
void R_RegisterDvars();
void R_RegisterCmds();
void R_InitWater();
void R_CheckDxCaps(const D3DCAPS9 *caps);
D3DFORMAT R_GetDepthStencilFormat(D3DFORMAT renderTargetFormat);
GfxImage *Image_AllocProg(int imageProgType, unsigned char category, unsigned char semantic);
unsigned int RB_CalcSunSpriteSamples();
void RB_FreeSunSpriteQueries();
void RB_PixelCost_Init();
void RB_DrawStretchPic(const Material *material, float x, float y, float w, float h, float s0, float t0, float s1, float t1, unsigned long color, GfxPrimStatsTarget statsTarget);
void * R_LockIndexBuffer(D3DIndexBuffer *handle, int offset, int bytes, int lockFlags);
void R_UnlockIndexBuffer(D3DIndexBuffer *handle);
void * R_LockVertexBuffer(D3DVertexBuffer *handle, int offset, int bytes, int lockFlags);
void R_UnlockVertexBuffer(D3DVertexBuffer *handle);
int R_SetIndexData(GfxCmdBufPrimState *state, const uint16_t *indices, int triCount);
int R_SetVertexData(GfxCmdBufState *state, const void *data, int vertexCount, int stride);

int R_PopRemoteScreenUpdate();
void R_PushRemoteScreenUpdate(int remoteScreenUpdateNesting);
void R_Cinematic_BeginLostDevice();
void R_ReleaseWorld();
void R_ReloadWorld();
void R_ReloadLostImages();
void R_Cinematic_EndLostDevice();
void R_AbortRenderCommands();
void Material_ReloadAll();
void R_SetColorMappings();
bool R_CheckDvarModified(dvar_s const*);
void R_SetTexFilter();
void R_PixelCost_PrintColorCodeKey();
void R_SetAlphaAntiAliasingState(D3DDevice *device, int stateBits0);
static_model_leaf_t * R_GetCachedSModelSurf(unsigned int cacheIndex);
signed int XModelGetStaticModelCacheVertCount(XModel *model, unsigned int lod);
void R_Cinematic_UpdateFrame();
void R_Cinematic_UpdateFrame(bool);
void R_DrawQuadMesh(GfxCmdBufContext context, const Material *material, GfxMeshData *quadMesh);
void R_BeginView(GfxCmdBufSourceState *source, const GfxSceneDef *sceneDef, const GfxViewParms *viewParms);
void R_FrustumClipPlanes(const GfxMatrix *viewProjMtx, const float (*sidePlanes)[4], int sidePlaneCount, DpvsPlane *frustumPlanes);
void R_ConvertColorToBytes(const float *colorFloat, unsigned char *colorBytes);
void R_DrawBspDrawSurfsLitPreTess(const unsigned int *primDrawSurfPos, GfxCmdBufContext context);
void R_DrawBspDrawSurfsPreTess(const unsigned int *primDrawSurfPos, GfxCmdBufContext context);
void R_ChangeObjectPlacement(GfxCmdBufSourceState *source, const GfxScaledPlacement *placement);
void R_ChangeObjectPlacementRemote(GfxCmdBufSourceState *source, GfxScaledPlacement *remotePlacement);
unsigned int R_DecodeSamplerState(char samplerState);
void R_UploadWaterTextureInternal(void *waterdef);
void R_SetReflectionProbe(GfxCmdBufContext context, unsigned int reflectionProbeIndex);
void R_SetLightmap(GfxCmdBufContext context, unsigned int lmapIndex);
GfxImage * R_GetTextureFromCode(GfxCmdBufSourceState *source, unsigned int codeTexture, char *samplerState);
GfxCmdBufSourceState * R_GetActiveWorldMatrix(GfxCmdBufSourceState *source);

template <class T> static void R_ReleaseAndSetNULL(T *var, const char *fn, const char *filename, int line)
{
  unsigned int useCount;
  assert(var);

  useCount = var->Release( );
  if ( useCount )
  {
    assertx(0, "%s (%i) %s->Release() failed: %i leak(s)!", filename, line, fn, useCount);
  }
}

struct GfxAssets
{
  D3DQuery *pixelCountQuery;
};


extern int g_disableRendering;
extern GfxScaledPlacement s_manualObjectPlacement;
extern int alwaysfails;
extern bool g_allocateMinimalResources;
extern GfxAssets gfxAssets;

template <class T> void R_ReleaseAndSetNULL(T *var, const char *fn, const char *filename, int line);

#define _RB_LogPrintf RB_LogPrintfDebug

#define D3DSEMSTART \
        int semaphore;\
        semaphore = R_AcquireDXDeviceOwnership(0);

#define D3DSEMEND \
        if ( semaphore ){\
          R_ReleaseDXDeviceOwnership();\
        }

#define D3DSEMGUARDEDCALL(EXP)\
do{\
      int semaphore;\
      semaphore = R_AcquireDXDeviceOwnership(0);\
      EXP;\
      if ( semaphore ){\
        R_ReleaseDXDeviceOwnership();\
      }\
}while(0)\




#define FloatAsInt(x) (*(int*)x)

#define D3DCALL(EXP) EXP



void R_OpenGL_SetBufferHint(unsigned int hint);
void R_OpenGL_ClearBufferHints();

#ifdef assert
#undef assert
#undef assertx
#endif
#define assert(x)
#define assertx(x, ...)

#endif

