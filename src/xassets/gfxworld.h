#ifndef __GFXWORLD_H__
#define __GFXWORLD_H__

//#define gfxWorld (*((GfxWorld*)(0xD0701E0)))

typedef uint16_t r_index_t;
typedef unsigned int raw_uint;
typedef unsigned char raw_byte;
typedef unsigned int raw_uint128;
typedef char raw_byte16;

typedef struct
{
	IDirect3DVertexBuffer9 *d3dVb;
}GfxVertexBuffer;

/* 7022 */
typedef struct 
{
  float mins[3];
  float maxs[3];
}GfxBrushModelWritable;

/* 7023 */
typedef struct 
{
  GfxBrushModelWritable writable;
  float bounds[2][3];
  unsigned int surfaceCount;
  unsigned int startSurfIndex;
}GfxBrushModel;

struct GfxWorldVertex
{
  vec3_t xyz;
  float binormalSign;
  GfxColor color;
  vec2_t texCoord;
  vec2_t lmapCoord;
  PackedUnitVec normal;
  PackedUnitVec tangent;
};

/* 7005 */
typedef struct 
{
  struct GfxWorldVertex *vertices;
  GfxVertexBuffer worldVb;
}GfxWorldVertexData;

/* 7006 */
typedef struct 
{
  byte *data;
  GfxVertexBuffer layerVb;
}GfxWorldVertexLayerData;

/* 7007 */
#pragma pack(push, 4)
typedef struct 
{
  char name[64];
  float ambientScale;
  float ambientColor[3];
  float diffuseFraction;
  float sunLight;
  float sunColor[3];
  float diffuseColor[3];
  byte diffuseColorHasBeenSet;
  byte pad[3];
  float angles[3];
}SunLightParseParams;
#pragma pack(pop)


/* 7011 */
typedef struct 
{
  float origin[3];
  struct GfxImage *reflectionImage;
}GfxReflectionProbe;

/* 7012 */
typedef struct 
{
  int cellCount;
  struct cplane_s *planes; //1AC
  uint16_t *nodes;
  raw_uint *dpvsPlanesCells; //256 * cellCount
}GfxWorldDpvsPlanes;

/* 7021 */
#pragma pack(push, 4)
typedef struct 
{
  byte hasLightRegions;
  byte pad[3];
  unsigned int sunPrimaryLightIndex;
  uint16_t mins[3];
  uint16_t maxs[3];
  unsigned int rowAxis;
  unsigned int colAxis;
  uint16_t *rowDataStart;
  unsigned int rawRowDataSize;
  byte *rawRowData;
  unsigned int entryCount;
  struct GfxLightGridEntry *entries;
  unsigned int colorCount;
  struct GfxLightGridColors *colors;
}GfxLightGrid;
#pragma pack(pop)

/* 7025 */
#pragma pack(push, 4)
typedef struct 
{
  byte hasValidData;
  byte pad[3];
  Material *spriteMaterial;
  Material *flareMaterial;
  float spriteSize;
  float flareMinSize;
  float flareMinDot;
  float flareMaxSize;
  float flareMaxDot;
  float flareMaxAlpha;
  int flareFadeInTime;
  int flareFadeOutTime;
  float blindMinDot;
  float blindMaxDot;
  float blindMaxDarken;
  int blindFadeInTime;
  int blindFadeOutTime;
  float glareMinDot;
  float glareMaxDot;
  float glareMaxLighten;
  int glareFadeInTime;
  int glareFadeOutTime;
  float sunFxPosition[3];
}sunflare_t;
#pragma pack(pop)


/* 7040 */
typedef struct 
{
  unsigned int smodelCount;
  unsigned int staticSurfaceCount;
  
  /*------ Is maybe incorrect --------*/
  unsigned int litSurfsBegin;
  unsigned int litSurfsEnd;
  unsigned int decalSurfsBegin;
  unsigned int decalSurfsEnd;
  unsigned int emissiveSurfsBegin;
  unsigned int emissiveSurfsEnd;
  /*---------------------------------------*/
  
  unsigned int unk;
  unsigned int lodDataCount;
  unsigned int surfaceCastsSunShadowCount;
  raw_byte *smodelVisData[3];
  raw_byte *surfaceVisData[3];
  raw_uint128 *lodData;
  uint16_t *sortedSurfIndex;
  struct GfxStaticModelInst *smodelInsts;
  struct GfxSurface *surfaces;
  struct GfxCullGroup *cullGroups;
  struct GfxStaticModelDrawInst *smodelDrawInsts;
  union GfxDrawSurf *surfaceMaterials;
  raw_uint128 *surfaceCastsSunShadow;
  volatile int usageCount;
}GfxWorldDpvsStatic;

/* 7041 */
typedef struct 
{
  unsigned int dynEntClientWordCount[2];
  unsigned int dynEntClientCount[2];
  raw_uint *dynEntCellBits[2];
  raw_byte16 *dynEntVisData[2][3];
}GfxWorldDpvsDynamic;

/* 7024 */
typedef struct 
{
  Material *material;
  int memory;
}MaterialMemory;


/* 7042 */
#pragma pack(push, 4)
typedef struct GfxWorld_s
{
  const char *name;
  const char *baseName;
  int planeCount; //8
  int nodeCount;
  int indexCount;
  r_index_t *indices;
  //D3DIndexBuffer indexBuffer;
  int surfaceCount;
  int unk;
  //GfxWorldStreamInfo streamInfo;
  int skySurfCount;
  int *skyStartSurfs;
  struct GfxImage *skyImage;
  byte skySamplerState;
  byte pad[3];
  unsigned int vertexCount;
  GfxWorldVertexData vd;
  unsigned int vertexLayerDataSize;
  GfxWorldVertexLayerData vld;
  SunLightParseParams sunParse;
  struct GfxLight *sunLight;
  float sunColorFromBsp[3];
  unsigned int sunPrimaryLightIndex;
  unsigned int primaryLightCount;
  int cullGroupCount;
  unsigned int reflectionProbeCount;
  GfxReflectionProbe *reflectionProbes;
  union GfxTexture *reflectionProbeTextures;
  GfxWorldDpvsPlanes dpvsPlanes; //1A8
  int cellBitsCount;
  struct GfxCell *cells;
  int lightmapCount;
  struct GfxLightmapArray *lightmaps;
  GfxLightGrid lightGrid;
  union GfxTexture *lightmapPrimaryTextures;
  union GfxTexture *lightmapSecondaryTextures;
  int modelCount;
  struct GfxBrushModel *models;
  vec3_t mins;
  vec3_t maxs;
  unsigned int checksum;
  int materialMemoryCount;
  struct MaterialMemory *materialMemory;
  sunflare_t sun;
  float outdoorLookupMatrix[4][4]; //1DC
  struct GfxImage *outdoorImage;
  raw_uint *cellCasterBits; //Or sceneEntCellBits possible
  struct GfxSceneDynModel *sceneDynModel;
  struct GfxSceneDynBrush *sceneDynBrush;
  raw_uint *primaryLightEntityShadowVis;
  raw_uint *primaryLightDynEntShadowVis[2];
  raw_byte *primaryLightForModelDynEnt;
  struct GfxShadowGeometry *shadowGeom;
  struct GfxLightRegion *lightRegion;
  GfxWorldDpvsStatic dpvs;
  GfxWorldDpvsDynamic dpvsDyn;
}GfxWorld;
#pragma pack(pop)

#endif