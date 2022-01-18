#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "r_types_d3d.h"

enum MaterialTechniqueType:int
{
  TECHNIQUE_DEPTH_PREPASS,
  TECHNIQUE_BUILD_FLOAT_Z,
  TECHNIQUE_BUILD_SHADOWMAP_DEPTH,
  TECHNIQUE_BUILD_SHADOWMAP_COLOR,
  TECHNIQUE_UNLIT,
  TECHNIQUE_EMISSIVE,
  TECHNIQUE_EMISSIVE_SHADOW,
  TECHNIQUE_LIT,
  TECHNIQUE_LIT_SUN,
  TECHNIQUE_LIT_SUN_SHADOW,
  TECHNIQUE_LIT_SPOT,
  TECHNIQUE_LIT_SPOT_SHADOW,
  TECHNIQUE_LIT_OMNI,
  TECHNIQUE_LIT_OMNI_SHADOW,

  TECHNIQUE_LIT_INSTANCED,
  TECHNIQUE_LIT_INSTANCED_SUN,
  TECHNIQUE_LIT_INSTANCED_SUN_SHADOW,
  TECHNIQUE_LIT_INSTANCED_SPOT,
  TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW,
  TECHNIQUE_LIT_INSTANCED_OMNI,
  TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW,

  TECHNIQUE_LIGHT_SPOT,
  TECHNIQUE_LIGHT_OMNI,
  TECHNIQUE_LIGHT_SPOT_SHADOW,
  TECHNIQUE_FAKELIGHT_NORMAL,
  TECHNIQUE_FAKELIGHT_VIEW,
  TECHNIQUE_SUNLIGHT_PREVIEW,
  TECHNIQUE_CASE_TEXTURE,
  TECHNIQUE_WIREFRAME_SOLID,
  TECHNIQUE_WIREFRAME_SHADED,
  TECHNIQUE_SHADOWCOOKIE_CASTER,
  TECHNIQUE_SHADOWCOOKIE_RECEIVER,
  TECHNIQUE_DEBUG_BUMPMAP,
  TECHNIQUE_DEBUG_BUMPMAP_INSTANCED,
  TECHNIQUE_COUNT,
  TECHNIQUE_TOTAL_COUNT,
  TECHNIQUE_NONE
};





union MaterialArgumentDef
{
  const float *literalConst;
  MaterialArgumentCodeConst codeConst;
  unsigned int codeSampler;
  unsigned int nameHash;
};


struct MaterialShaderArgument
{
  uint16_t type;
  uint16_t dest;
  MaterialArgumentDef u;
};



#pragma pack(push, 1)
struct __align(2) MaterialPass
{
  struct MaterialVertexDeclaration *vertexDecl;
  struct MaterialVertexShader *vertexShader;
  struct MaterialPixelShader *pixelShader;
  char perPrimArgCount;
  char perObjArgCount;
  char stableArgCount;
  char customSamplerFlags;
  struct MaterialShaderArgument *args;
};
#pragma pack(pop)


struct MaterialTechnique
{
  const char *name;
  uint16_t flags;
  uint16_t passCount;
  MaterialPass passArray[1];
};


struct MaterialConstantDef
{
  unsigned int nameHash;
  char name[12];
  vec4_t literal;
};

union MaterialTextureDefInfo
{
  struct GfxImage *image;
  struct water_t *water;
};

struct MaterialTextureDef
{
  unsigned int nameHash;
  char nameStart;
  char nameEnd;
  char samplerState;
  char semantic;
  MaterialTextureDefInfo u;
};

struct __align(8) MaterialInfo
{
  const char *name;
  char gameFlags;
  char sortKey;
  char textureAtlasRowCount;
  char textureAtlasColumnCount;
  GfxDrawSurf drawSurf;
  int surfaceTypeBits;
};

struct Material
{
  MaterialInfo info;
  char stateBitsEntry[34];
  uint8_t textureCount;
  uint8_t constantCount;
  char stateBitsCount;
  char stateFlags;
  char cameraRegion;
  char unk;
  struct MaterialTechniqueSet *techniqueSet;
  MaterialTextureDef *textureTable;
  MaterialConstantDef *constantTable;
  struct GfxStateBits *stateBitsTable;
};


struct MaterialStreamRouting
{
  uint8_t source;
  uint8_t dest;
};

struct MaterialVertexStreamRouting
{
  MaterialStreamRouting data[16];
  D3DVertexDeclaration *decl[VERTDECL_COUNT];
};

struct MaterialVertexDeclaration
{
  uint8_t streamCount;
  bool8 hasOptionalSource;
  bool8 isLoaded;
  bool8 padding;
  MaterialVertexStreamRouting routing;
};

typedef struct MaterialTechniqueSet
{
  const char *name;
  char worldVertFormat;
  bool8 uploaded;
  char unused[2];
  struct MaterialTechniqueSet *remappedTechniqueSet;
  struct MaterialTechnique *techniques[TECHNIQUE_COUNT];
} MaterialTechniqueSet;

enum MtlTechSetNotFoundBehavior //Has to be defined
{
  MtlTECHNOTFOUND_NONE,
  MtlTECHNOTFOUND_ONE
};

bool Material_WouldTechniqueSetBeOverridden(MaterialTechniqueSet const*);
void Material_Shutdown();
void Material_ReleaseAll();
void Material_PreLoadAllShaderText();
Material* Material_Register(const char *name, int imageTrack);
MaterialTechniqueSet *Material_FindTechniqueSet(char const*, MtlTechSetNotFoundBehavior);
MaterialTechniqueSet *Material_RegisterTechniqueSet(const char *name);
void Material_ForEachTechniqueSet(void (*)(MaterialTechniqueSet*));
#endif



