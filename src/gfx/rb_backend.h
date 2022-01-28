#include "r_types_d3d.h"



struct GfxCmdBufContext;
struct GfxCmdBufState;
enum GfxRenderTargetId:int;
struct GfxCmdBufPrimState;
struct MaterialVertexDeclaration;
struct Material;


extern dvar_s* r_texFilterAnisoMax;
extern dvar_s* r_texFilterDisable;
extern dvar_s* r_texFilterAnisoMin;
extern dvar_s* r_texFilterMipMode;
extern dvar_s* r_texFilterMipBias;
extern dvar_s* r_showPixelCost;


struct PolySet
{
  float (*verts)[3];
  int vertCount;
  int vertLimit;
  struct GfxDebugPoly *polys;
  int polyCount;
  int polyLimit;
};


struct DebugGlobals
{
  PolySet polySet;
  struct trDebugString_t *strings;
  int stringCount;
  int stringLimit;
  struct trDebugString_t *externStrings;
  int externStringCount;
  int externMaxStringCount;
  struct trDebugLine_t *lines;
  int lineCount;
  int lineLimit;
  struct trDebugLine_t *externLines;
  int externLineCount;
  int externMaxLineCount;
  struct GfxDebugPlume *plumes;
  int plumeCount;
  int plumeLimit;
};



struct __align(32) GfxBackEndData
{
  char surfsBuffer[131072];
  FxCodeMeshData codeMeshes[2048];
  unsigned int primDrawSurfsBuf[65536];
  GfxViewParms viewParms[4];
  char primaryLightTechType[8][2][2][256];
  float codeMeshArgs[256][4];
  GfxParticleCloud clouds[256];
  GfxDrawSurf drawSurfs[32768];
  char unknowngap[2816];
  GfxMeshData codeMesh;
  __align(16) GfxSModelCachedVertex smcPatchVerts[8192];
  uint16_t smcPatchList[256];
  unsigned int smcPatchCount;
  unsigned int smcPatchVertsUsed;
  GfxModelLightingPatch modelLightingPatchList[4096];
  volatile int modelLightingPatchCount;
  GfxBackEndPrimitiveData prim;
  unsigned int shadowableLightHasShadowMap[8];
  unsigned int frameCount;
  int drawSurfCount;
  volatile int surfPos;
  volatile int gfxEntCount;
  GfxEntity gfxEnts[128];
  volatile int cloudCount;
  volatile int codeMeshCount;
  volatile int codeMeshArgsCount;
  volatile int markMeshCount;
  FxMarkMeshData markMeshes[1536];
  GfxMeshData markMesh;
  GfxVertexBufferState *skinnedCacheVb;
  D3DQuery *endFence;
  char *tempSkinBuf;
  volatile int tempSkinPos;
  D3DIndexBuffer *preTessIb;
  int viewParmCount;
  GfxFog fogSettings;
  GfxCmdArray *commands;
  unsigned int viewInfoIndex;
  unsigned int viewInfoCount;
  struct GfxViewInfo *viewInfo;
  const void *cmds;
  GfxLight sunLight;
  int hasApproxSunDirChanged;
  volatile int primDrawSurfPos;
  int unk1;
  DebugGlobals debugGlobals;
  unsigned int drawType;
};

struct r_backEndGlobals_t
{
  int glowCount;
  GfxImage *glowImage;
  struct Font_s *debugFont;
};

extern r_backEndGlobals_t backEnd;


struct __align(4) GfxPixelShaderLoadDef
{
  unsigned int *program;
  uint16_t programSize;
  uint16_t rendertype;
};

struct __align(4) GfxVertexShaderLoadDef
{
  unsigned int *program;
  uint16_t programSize;
  uint16_t rendertype;
};


struct MaterialPixelShaderProgram
{
  D3DPixelShader *ps;
  GfxPixelShaderLoadDef loadDef;
};

struct MaterialVertexShaderProgram
{
  D3DVertexShader *vs;
  GfxVertexShaderLoadDef loadDef;
};


struct MaterialPixelShader
{
  const char *name;
  MaterialPixelShaderProgram prog;
};

struct MaterialVertexShader
{
  const char *name;
  MaterialVertexShaderProgram prog;
};

enum GfxPixelCostMode
{
  GFX_PIXEL_COST_MODE_OFF,
  GFX_PIXEL_COST_MODE_MEASURE_COST,
  GFX_PIXEL_COST_MODE_MEASURE_MSEC,
  GFX_PIXEL_COST_MODE_ADD_COST_USE_DEPTH,
  GFX_PIXEL_COST_MODE_ADD_COST_IGNORE_DEPTH,
  GFX_PIXEL_COST_MODE_ADD_PASSES_USE_DEPTH,
  GFX_PIXEL_COST_MODE_ADD_PASSES_IGNORE_DEPTH
};

#define gfxRenderTargets (*(GfxRenderTarget**)(0xD573EB0))
#define pixelCostMode (*(GfxPixelCostMode*)(0xD5EC684))


struct GfxVertex
{
  float xyzw[4];
  GfxColor color;
  float texCoord[2];
  PackedUnitVec normal;
};


struct __align(16) materialCommands_t
{
  GfxVertex verts[5450];
  uint16_t indices[1048576];
  MaterialVertexDeclType vertDeclType;
  unsigned int vertexSize;
  int indexCount;
  int vertexCount;
  int firstVertex;
  int lastVertex;
  bool finishedFilling;
};

struct GfxGammaRamp
{
  uint16_t entries[256];
};


extern materialCommands_t tess;
extern GfxBackEndData *backEndData;
extern GfxBackEndData s_backEndData;


static inline void Byte4CopyRgbaToVertexColor(const unsigned char *rgbaFrom, unsigned char *nativeTo)
{
  *(uint32_t*)nativeTo = (rgbaFrom[3] << 24) | rgbaFrom[2] | (rgbaFrom[1] << 8) | (rgbaFrom[0] << 16);
}

static inline void Byte4UnpackVertexColor(const unsigned char *from, float *to)
{
  to[0] = (float)from[2] * 0.0039215689;
  to[1] = (float)from[1] * 0.0039215689;
  to[2] = (float)from[0] * 0.0039215689;
  to[3] = (float)from[3] * 0.0039215689;
}

static inline void Byte4PackPixelColor(const float *from, uint8_t *to)
{
  to[2] = (unsigned char)_clip2domain(255.0 * from[0], 0, 255);
  to[1] = (unsigned char)_clip2domain(255.0 * from[1], 0, 255);
  to[0] = (unsigned char)_clip2domain(255.0 * from[2], 0, 255);
  to[3] = (unsigned char)_clip2domain(255.0 * from[3], 0, 255);
}


void R_HW_EnableScissor(D3DDevice *device, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
void R_HW_DisableScissor(D3DDevice *device);
extern "C" void R_DrawSurfs(GfxCmdBufState *prepassState, const GfxDrawSurfListInfo *info, GfxCmdBufContext context);
GfxRenderTargetId RB_PixelCost_OverrideRenderTarget(GfxRenderTargetId targetId);
void R_HW_SetRenderTarget(GfxCmdBufState *state, unsigned int newTargetId);
void R_HW_SetVertexShader(D3DDevice *device, const MaterialVertexShader *mtlShader);
void R_SetVertexDecl(GfxCmdBufPrimState *primState, MaterialVertexDeclaration *vertexDecl);
void R_HW_SetPixelShader(D3DDevice *device, const MaterialPixelShader *mtlShader);
void RB_UnbindAllImages();
void RB_InitSceneViewport();
void RB_InitCodeImages();
void RB_InitBackendGlobalStructs();
void RB_InitSceneViewport();
void RB_InitCodeConstants();
D3DQuery *RB_HW_AllocOcclusionQuery();
void RB_AllocSunSpriteQueries();
void RB_RegisterBackendAssets();
void RB_ResetStatTracking(int viewIndex);
void RB_EndTessSurface();
void R_CheckVertexDataOverflow(int dataSize);
void RB_Resource_CreateVertexDeclaration(D3DVERTEXELEMENT9 *elements, D3DVertexDeclaration **declaration);
void RB_SetIdentity();
void RB_SetTessTechnique(const Material *material, MaterialTechniqueType techType);
void R_SetLightGridSampleDeltas(int rowStride, int sliceStride);
void R_ApplyLightGridColorsPatch(const GfxModelLightingPatch *patch, unsigned char *pixels);
void R_ShutdownDynamicMesh(GfxMeshData *mesh);
void R_SetCompleteState(IDirect3DDevice9 *device, const unsigned int *stateBits);
void R_HW_ForceSamplerState(IDirect3DDevice9 *device, unsigned int samplerIndex, unsigned int samplerState);
void R_Resolve(GfxCmdBufContext context, GfxImage *image);
void RB_UploadShaderStep();
void RB_PatchModelLighting(GfxModelLightingPatch *patchList, unsigned int patchCount);
void sub_615230();
void R_InsertGpuFence();
void R_AcquireGpuFenceLock();
void R_ReleaseGpuFenceLock();
void RB_Draw3DCommon();
void RB_ExecuteRenderCommandsLoop(const void *cmds);
unsigned int RB_HW_ReadOcclusionQuery(D3DQuery *query);
void RB_HW_BeginOcclusionQuery(D3DQuery *query, D3DDevice* device);
void RB_ShowCollision(GfxViewParms *viewParms);
void R_HW_InsertFence(D3DQuery **fence);
void R_InitLocalCmdBufState(GfxCmdBufState *state);
void RB_DrawDebug(const GfxViewParms *viewParms);
void RB_BeginSurface(const Material *material, MaterialTechniqueType techType);
void RB_CheckTessOverflow(int vertexCount, int indexCount);
void RB_TessOverflow();
int  RB_CompareTouchImages(const void *e0, const void *e1);
void R_PixelCost_BeginSurface(GfxCmdBufContext context);
void R_PixelCost_EndSurface(GfxCmdBufContext context);
void R_HW_SetVertexShaderConstant(D3DDevice *device, unsigned int dest, const float *data, unsigned int rowCount);
void R_HW_SetPixelShaderConstant(D3DDevice *device, unsigned int dest, const float *data, unsigned int rowCount);

