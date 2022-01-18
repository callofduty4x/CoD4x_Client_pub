#ifndef __R_TYPES_D3D_H__
#define __R_TYPES_D3D_H__

#include <d3d9.h>
#include "../q_shared.h"


#ifdef _MSC_VER
#define __align(X) __declspec(align(X))
#else
#define __align(X) __attribute__((aligned (X)))
#endif


struct complex_s
{
  float real;
  float imag;
};

typedef char bool8;

typedef IDirect3DBaseTexture9           D3DBaseTexture;
typedef IDirect3DTexture9               D3DTexture;
typedef IDirect3DVolumeTexture9         D3DVolumeTexture;
typedef IDirect3DCubeTexture9           D3DCubeTexture;
typedef IDirect3DSurface9				        D3DSurface;
typedef IDirect3DDevice9                D3DDevice;
typedef IDirect3DIndexBuffer9           D3DIndexBuffer;
typedef IDirect3DVertexBuffer9          D3DVertexBuffer;
typedef IDirect3DVertexDeclaration9     D3DVertexDeclaration;
typedef IDirect3DPixelShader9           D3DPixelShader;
typedef IDirect3DVertexShader9          D3DVertexShader;
typedef IDirect3DQuery9                 D3DQuery;

typedef uint16_t r_index_t;
typedef unsigned int raw_uint;
typedef unsigned char raw_byte;
typedef unsigned int raw_uint128;
typedef char raw_byte16;

struct GfxImage;

#define R_NORMAL_OVERRIDE_NONE 0

union GfxTexture
{
  D3DBaseTexture *basemap;
  D3DTexture *map;
  D3DVolumeTexture *volmap;
  D3DCubeTexture *cubemap;
  struct GfxImageLoadDef *loadDef;
};

union PackedTexCoords
{
  unsigned int packed;
  uint16_t w;
};

union PackedUnitVec
{
  unsigned int packed;
};

union PackedLightingCoords
{
  unsigned int packed;
  char array[4];
};


struct WaterWritable
{
  float floatTime;
};

struct water_t
{
  WaterWritable writable;
  complex_s *H0;
  float *wTerm;
  int M;
  int N;
  float Lx;
  float Lz;
  float gravity;
  float windvel;
  float winddir[2];
  float amplitude;
  float codeConstant[4];
  GfxImage *image;
};


union GfxColor
{
  unsigned int packed;
  unsigned char array[4];
};


enum R_COLOR_OVERRIDES
{
  R_COLOR_OVERRIDE_BLACK = 0x0,
  R_COLOR_OVERRIDE_NONE = 0x1,
  R_COLOR_OVERRIDE_WHITE = 0x2,
  R_COLOR_OVERRIDE_GRAY = 0x3,
  R_COLOR_OVERRIDE_COUNT = 0x4,
};

enum MapType_s
{
  MAPTYPE_NONE = 0x0,
  MAPTYPE_INVALID1 = 0x1,
  MAPTYPE_INVALID2 = 0x2,
  MAPTYPE_2D = 0x3,
  MAPTYPE_3D = 0x4,
  MAPTYPE_CUBE = 0x5,
  MAPTYPE_COUNT = 0x6,
};

#define MAX_GFXCMDINPUTCONST 58
#define CONST_SRC_FIRST_CODE_MATRIX MAX_GFXCMDINPUTCONST
#define CONST_SRC_CODE_COUNT_FLOAT4 CONST_SRC_FIRST_CODE_MATRIX
#define TEXTURE_SRC_CODE_COUNT 27

struct MaterialArgumentCodeConst
{
  uint16_t index;
  char firstRow;
  char rowCount;
};

struct GfxShaderConstantState_fields
{
  MaterialArgumentCodeConst codeConst;
  unsigned int version;
};

union GfxShaderConstantState
{
  GfxShaderConstantState_fields fields;
  uint64_t packed;
};




struct FxCodeMeshData
{
  unsigned int triCount;
  uint16_t *indices;
  uint16_t argOffset;
  uint16_t argCount;
  unsigned int lightHandle;
};

struct FxMarkMeshData
{
  unsigned int triCount;
  uint16_t *indices;
  uint16_t modelIndex;
  char modelTypeAndSurf;
  char pad0;
  unsigned int pad1;
};


struct GfxStateBits
{
  int loadBits[2];
};

struct GfxMatrix
{
  float m[4][4];
};


struct GfxViewParms
{
  GfxMatrix viewMatrix;
  GfxMatrix projectionMatrix;
  GfxMatrix viewProjectionMatrix;
  GfxMatrix inverseViewProjectionMatrix;
  float origin[4];
  float axis[3][3];
  float depthHackNearClip;
  int pad[2];
};


struct GfxPlacement
{
  vec4_t quat;
  vec3_t origin;
};


struct GfxScaledPlacement
{
  GfxPlacement base;
  float scale;
};


struct GfxParticleCloud
{
  GfxScaledPlacement placement;
  float endpos[3];
  GfxColor color;
  float radius[2];
  unsigned int pad[2];
};

struct GfxCodeMatrices
{
  GfxMatrix matrix[32];
};

/* Blackops bit fields
struct GfxDrawSurfFields
{
  uint64_t objectId : 16;           //0
  uint64_t fade : 4;                //16
  uint64_t customIndex : 5;         //20
  uint64_t reflectionProbeIndex : 3;//25
  uint64_t hdrBits : 1;             //28
  uint64_t glightRender : 1;        //29
  uint64_t dlightRender : 1;        //30
  uint64_t materialSortedIndex : 12;//31
  uint64_t primaryLightIndex : 8;   //43
  uint64_t surfType : 4;            //51
  uint64_t prepass : 2;             //55
  uint64_t noDynamicShadow : 1;     //57
  uint64_t primarySortKey : 6;      //58
};                                  //64
*/

//CoD4 bit fields, 12 bits unknown
struct GfxDrawSurfFields
{
  uint64_t objectId : 16;           //0
  uint64_t reflectionProbeIndex : 8;//16
  uint64_t customIndex : 5;         //24
  uint64_t materialSortedIndex : 11;//29
  uint64_t prepass : 2;             //40
  uint64_t tobedefined : 12;        //42
  uint64_t primarySortKey : 6;      //54
};                                  //60

union GfxDrawSurf
{
  GfxDrawSurfFields fields;
  uint64_t packed;
};

struct GfxIndexBufferState
{
  volatile int used;
  int total;
  D3DIndexBuffer *buffer;
  uint16_t *indices;
};


#pragma pack(push, 1)
struct __align(4) GfxVertexBufferState
{
  volatile int used;
  int total;
  D3DVertexBuffer *buffer;
  char *verts;
};
#pragma pack(pop)



struct GfxMeshData
{
  unsigned int indexCount;
  unsigned int totalIndexCount;
  uint16_t *indices;
  GfxVertexBufferState vb;
  unsigned int vertSize;
};



struct GfxSModelCachedVertex
{
  vec3_t xyz;
  GfxColor color;
  PackedTexCoords texCoord;
  PackedUnitVec normal;
  PackedUnitVec tangent;
  PackedLightingCoords baseLighting;
};


#pragma pack(push, 1)
struct GfxModelLightingPatch
{
  uint16_t modelLightingIndex;
  char primaryLightWeight;
  char colorsCount;
  char groundLighting[4];
  uint16_t colorsWeight[8];
  uint16_t colorsIndex[8];
};
#pragma pack(pop)


struct GfxBackEndPrimitiveData
{
  int hasSunDirChanged;
};


struct GfxEntity
{
  unsigned int renderFxFlags;
  float materialTime;
};


struct GfxFog
{
  int startTime;
  int finishTime;
  GfxColor color;
  float fogStart;
  float density;
};

struct GfxCmdHeader
{
  uint16_t id;
  uint16_t byteCount;
};

struct GfxCmdArray
{
  byte *cmds;
  int usedTotal;
  int usedCritical;
  GfxCmdHeader *lastCmd;
};

struct GfxLight
{
  char type;
  char canUseShadowMap;
  char unused[2];
  float color[3];
  float dir[3];
  vec3_t origin;
  float radius;
  float cosHalfFovOuter;
  float cosHalfFovInner;
  int exponent;
  unsigned int spotShadowIndex;
  struct GfxLightDef *def;
};

struct r_resource_action
{
  int action;
  void *resource;
  void *data;
  int p1;
  int p2;
  int p3;
};

struct GfxDynamicIndices
{
  volatile int used;
  int total;
  uint16_t *indices;
};

struct srfTriangles_t
{
  unsigned int vertexLayerData;
  unsigned int firstVertex;
  uint16_t vertexCount;
  uint16_t triCount;
  int baseIndex;
};

typedef D3DVertexBuffer *GfxVertexBuffer;

typedef struct 
{
  float origin[3];
  struct GfxImage *reflectionImage;
}GfxReflectionProbe;

enum ShadowType
{
  SHADOW_NONE = 0x0,
  SHADOW_COOKIE = 0x1,
  SHADOW_MAP = 0x2,
};

enum MaterialTechniqueType:int;

#pragma pack(push, 4)
struct GfxDrawSurfListInfo
{
  GfxDrawSurf *drawSurfs;
  unsigned int drawSurfCount;
  MaterialTechniqueType baseTechType;
  struct GfxViewInfo *viewInfo;
  float viewOrigin[4];
  struct GfxLight *light;
  int cameraView;
};
#pragma pack(pop)

struct __align(16) ShadowCookie
{
  GfxMatrix shadowLookupMatrix;
  float boxMin[3];
  float boxMax[3];
  GfxViewParms *shadowViewParms;
  float fade;
  unsigned int sceneEntIndex;
  GfxDrawSurfListInfo casterInfo;
  GfxDrawSurfListInfo receiverInfo;
};

#pragma pack(push, 16)
struct ShadowCookieList
{
  ShadowCookie cookies[24];
  unsigned int cookieCount;
};
#pragma pack(pop)

struct PointLightPartition
{
  GfxLight light;
  GfxDrawSurfListInfo info;
};


struct GfxDepthOfField
{
  float viewModelStart;
  float viewModelEnd;
  float nearStart;
  float nearEnd;
  float farStart;
  float farEnd;
  float nearBlur;
  float farBlur;
};


#pragma pack(push, 4)
struct GfxFilm
{
  bool8 enabled;
  float brightness;
  float contrast;
  float desaturation;
  bool8 invert;
  float tintDark[3];
  float tintLight[3];
};
#pragma pack(pop)

#pragma pack(push, 4)
struct GfxGlow
{
  bool8 enabled;
  float bloomCutoff;
  float bloomDesaturation;
  float bloomIntensity;
  float radius;
};
#pragma pack(pop)

struct __align(16) GfxSpotShadow
{
  GfxViewParms shadowViewParms;
  GfxMatrix lookupMatrix;
  char shadowableLightIndex;
  char pad[3];
  GfxLight *light;
  float fade;
  GfxDrawSurfListInfo info;
  char pad2[4];
  char unknown[30];
};

struct GfxQuadMeshData
{
  float x;
  float y;
  float width;
  float height;
  GfxMeshData meshData;
};

enum GfxDrawSceneMethod
{
  GFX_DRAW_SCENE_NONE = 0x0,
  GFX_DRAW_SCENE_FULLBRIGHT = 0x1,
  GFX_DRAW_SCENE_DEBUGSHADER = 0x2,
  GFX_DRAW_SCENE_STANDARD = 0x3
};

struct GfxDrawMethod
{
  GfxDrawSceneMethod drawScene;
  MaterialTechniqueType baseTechType;
  MaterialTechniqueType emissiveTechType;
  char litTechType[16][7];
};

struct GfxSunShadowBoundingPoly
{
  float snapDelta[2];
  int pointCount;
  float points[5][2];
};


struct GfxPackedVertex
{
  float xyz[3];
  float binormalSign;
  GfxColor color;
  PackedTexCoords texCoord;
  PackedUnitVec normal;
  PackedUnitVec tangent;
};


struct Picmip
{
  char platform[2];
};

struct CardMemory
{
  int platform[2];
};

enum FullscreenType
{
  FULLSCREEN_DISPLAY = 0x0,
  FULLSCREEN_MIXED = 0x1,
  FULLSCREEN_SCENE = 0x2
};

enum RenderTargetUsage
{
  RENDERTARGET_USAGE_DEPTH = 0x0,
  RENDERTARGET_USAGE_RENDER = 0x1,
  RENDERTARGET_USAGE_RENDER_SHARE_SCENE = 0x2,
  RENDERTARGET_USAGE_TEXTURE = 0x3
};

enum ShadowmapRTTiling
{
  SHADOWMAP_RT_TILE_VERTICAL = 0x0,
  SHADOWMAP_RT_TILE_GRID = 0x1
};



enum GfxRenderTargetId:int
{
  R_RENDERTARGET_SAVED_SCREEN,
  R_RENDERTARGET_FRAME_BUFFER,
  R_RENDERTARGET_SCENE,
  R_RENDERTARGET_RESOLVED_POST_SUN,
  R_RENDERTARGET_RESOLVED_SCENE,
  R_RENDERTARGET_FLOAT_Z,
  R_RENDERTARGET_DYNAMICSHADOWS,
  R_RENDERTARGET_PINGPONG_0,
  R_RENDERTARGET_PINGPONG_1,
  R_RENDERTARGET_SHADOWCOOKIE,
  R_RENDERTARGET_SHADOWCOOKIE_BLUR,
  R_RENDERTARGET_POST_EFFECT_0,
  R_RENDERTARGET_POST_EFFECT_1,
  R_RENDERTARGET_SHADOWMAP_SUN,
  R_RENDERTARGET_SHADOWMAP_SPOT,
  R_RENDERTARGET_COUNT
};




enum MaterialVertexDeclType:int
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
  VERTDECL_PARTICLECLOUD = 0xE,
  VERTDECL_STATICMODELCACHE = 0xF,
  VERTDECL_COUNT
};



enum GfxViewportBehavior
{
  GFX_USE_VIEWPORT_FOR_VIEW = 0x0,
  GFX_USE_VIEWPORT_FULL = 0x1
};


struct GfxViewport
{
  int x;
  int y;
  int width;
  int height;
};

struct GfxSceneDef
{
  int time;
  float floatTime;
  float viewOffset[3];
};


enum GfxViewMode
{
  VIEW_MODE_NONE = 0x0,
  VIEW_MODE_3D = 0x1,
  VIEW_MODE_2D = 0x2,
  VIEW_MODE_IDENTITY = 0x3,
};

struct __align(8) GfxCmdBufInput
{
  float consts[MAX_GFXCMDINPUTCONST][4];
  GfxImage *codeImages[TEXTURE_SRC_CODE_COUNT];
  char codeImageSamplerStates[TEXTURE_SRC_CODE_COUNT];
  __align(2) struct GfxBackEndData *data;
};

#pragma pack(push, 1)
struct __align(16) GfxCmdBufSourceState
{
  GfxCodeMatrices matrices;
//  GfxMatrix extramatrices[2];
  GfxCmdBufInput input;
  GfxViewParms viewParms;
  GfxMatrix shadowLookupMatrix;
  uint16_t constVersions[90];
  uint16_t matrixVersions[8];
  float eyeOffset[4];
  unsigned int shadowableLightForShadowLookupMatrix;
  GfxScaledPlacement *objectPlacement;
  GfxViewParms *viewParms3D;
  unsigned int depthHackFlags;
  GfxScaledPlacement skinnedPlacement;
  int cameraView;
  GfxViewMode viewMode;
  GfxSceneDef sceneDef;
  GfxViewport sceneViewport;
  float materialTime;
  GfxViewportBehavior viewportBehavior;
  int renderTargetWidth;
  int renderTargetHeight;
  bool8 viewportIsDirty;
  __align(4) unsigned int shadowableLightIndex;
};
#pragma pack(pop)

struct GfxCmdBuf
{
  D3DDevice *device;
};


enum GfxPrimStatsTarget
{
  GFX_PRIM_STATS_WORLD = 0x0,
  GFX_PRIM_STATS_SMODELCACHED = 0x1,
  GFX_PRIM_STATS_SMODELRIGID = 0x2,
  GFX_PRIM_STATS_XMODELRIGID = 0x3,
  GFX_PRIM_STATS_XMODELSKINNED = 0x4,
  GFX_PRIM_STATS_BMODEL = 0x5,
  GFX_PRIM_STATS_FX = 0x6,
  GFX_PRIM_STATS_HUD = 0x7,
  GFX_PRIM_STATS_DEBUG = 0x8,
  GFX_PRIM_STATS_CODE = 0x9,
  GFX_PRIM_STATS_COUNT = 0xA
};


struct GfxPackedPlacement
{
  float origin[3];
  float axis[3][3];
  float scale;
};


struct GfxStaticModelDrawInst
{
  float cullDist;
  GfxPackedPlacement placement;
  struct XModel *model;
  uint16_t smodelCacheIndex[4];
  char reflectionProbeIndex;
  char primaryLightIndex;
  uint16_t lightingHandle;
  char flags;
  char pad[3];
};

struct GfxCachedSModelSurf
{
  unsigned int baseVertIndex;
  uint16_t lodIndex;
  uint16_t smodelIndex;
};

struct static_model_node_list_t
{
  static_model_node_list_t *prev;
  static_model_node_list_t *next;
};

union static_model_leaf_t
{
  GfxCachedSModelSurf cachedSurf;
  static_model_node_list_t freenode;
};

struct DpvsPlane
{
  float coeffs[4];
  char side[3];
  char pad;
};

struct DpvsPlanes
{
  DpvsPlane *planes;
  int count;
};

struct GfxLightmapArray
{
  GfxImage *primary;
  GfxImage *secondary;
};


#define SAMPLER_FILTER_MASK 7 //unchecked for cod4
#define SAMPLER_MIPMAP_MASK 24
#endif