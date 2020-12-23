#ifndef __MATERIAL_ASSET_H__
#define __MATERIAL_ASSET_H__

#include "../q_shared.h"

/* 709 */
typedef enum
{
  TECHNIQUE_DEPTH_PREPASS = 0x0,
  TECHNIQUE_BUILD_FLOAT_Z = 0x1,
  TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
  TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x3,
  TECHNIQUE_UNLIT = 0x4,
  TECHNIQUE_EMISSIVE = 0x5,
  TECHNIQUE_EMISSIVE_SHADOW = 0x6,
  TECHNIQUE_LIT_BEGIN = 0x7,
  TECHNIQUE_LIT = 0x7,
  TECHNIQUE_LIT_SUN = 0x8,
  TECHNIQUE_LIT_SUN_SHADOW = 0x9,
  TECHNIQUE_LIT_SPOT = 0xA,
  TECHNIQUE_LIT_SPOT_SHADOW = 0xB,
  TECHNIQUE_LIT_OMNI = 0xC,
  TECHNIQUE_LIT_OMNI_SHADOW = 0xD,
  TECHNIQUE_LIT_INSTANCED = 0xE,
  TECHNIQUE_LIT_INSTANCED_SUN = 0xF,
  TECHNIQUE_LIT_INSTANCED_SUN_SHADOW = 0x10,
  TECHNIQUE_LIT_INSTANCED_SPOT = 0x11,
  TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW = 0x12,
  TECHNIQUE_LIT_INSTANCED_OMNI = 0x13,
  TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW = 0x14,
  TECHNIQUE_LIGHT_SPOT = 0x15,
  TECHNIQUE_LIGHT_OMNI = 0x16,
  TECHNIQUE_LIGHT_SPOT_SHADOW = 0x17,
  TECHNIQUE_FAKELIGHT_NORMAL = 0x18,
  TECHNIQUE_FAKELIGHT_VIEW = 0x19,
  TECHNIQUE_SUNLIGHT_PREVIEW = 0x1A,
  TECHNIQUE_CASE_TEXTURE = 0x1B,
  TECHNIQUE_WIREFRAME_SOLID = 0x1C,
  TECHNIQUE_WIREFRAME_SHADED = 0x1D,
  TECHNIQUE_SHADOWCOOKIE_CASTER = 0x1E,
  TECHNIQUE_SHADOWCOOKIE_RECEIVER = 0x1F,
  TECHNIQUE_DEBUG_BUMPMAP = 0x20,
  TECHNIQUE_DEBUG_BUMPMAP_INSTANCED = 0x21,
  TECHNIQUE_COUNT = 0x22,
  TECHNIQUE_TOTAL_COUNT = 0x23,
  TECHNIQUE_NONE = 0x24,
}MaterialTechniqueType;


struct GfxDrawSurfFields //Copied it from https://github.com/ZoneTool/zonetool/blob/develop/src/IW3/Structs.hpp //Valid? I don't know!
{
	uint64_t objectId : 16;
	uint64_t reflectionProbeIndex : 8;
	uint64_t customIndex : 5;
	uint64_t materialSortedIndex : 11;
	uint64_t prepass : 2;
	uint64_t primaryLightIndex : 8;
	uint64_t surfType : 4;
	uint64_t primarySortKey : 6;
	uint64_t unused : 4;
};

union GfxDrawSurf
{
	struct GfxDrawSurfFields fields;
	uint64_t packed;
};



#if 0

/* Be carefull. I can not remember to really have verified this */
#include "gfximage.h"

#define PC 1
#define COD4 1
typedef struct MaterialConstantDef_s
{
	int nameHash;
	char name[12];
	vec4_t literal;
}MaterialConstantDef_t;

typedef struct GfxStateBits_s
{
#ifdef XBOX
	int loadBits[2];
#elif defined PS3
	int (*loadBits)[2];
 #ifdef MW2 || MW3
	int unknown;
 #endif
#elif defined PC && COD4
	int loadBits[2];
#endif

}GfxStateBits_t;

typedef struct WaterWritable_s
{
	float floatTime;
}WaterWritable_t;

typedef struct water_s
{
	WaterWritable_t writable;
	float *H0X;		// Count = M * N
	float *H0Y;		// Count = M * N
	float *wTerm;		// Count = M * N
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
}water_t;

typedef union
{
	unsigned long long unused,
			primarySortKey,
			surfType,
			primaryLightIndex,
			prepass,
			materialSortedIndex,
			customIndex,
			reflectionProbeIndex,
			objectId;
}GFxDrawSurfFields;



#endif

	/* MaterialTextureDef->semantic */
#define TS_2D		0x0
#define TS_FUNCTION	0x1
#define TS_COLOR_MAP	0x2
#define TS_UNUSED_1	0x3
#define TS_UNUSED_2	0x4
#define TS_NORMAL_MAP	0x5
#define TS_UNUSED_3	0x6
#define TS_UNUSED_4	0x7
#define TS_SPECULAR_MAP	0x8
#define TS_UNUSED_5	0x9
#define TS_UNUSED_6	0xA
#define TS_WATER_MAP	0xB

typedef union
{
	struct GfxImage *image;	// MaterialTextureDef->semantic != TS_WATER_MAP
	struct water_t *water;		// MaterialTextureDef->semantic == TS_WATER_MAP
}MaterialTextureDefInfo;

typedef struct MaterialTextureDef_s
{
	unsigned int nameHash;
	char nameStart;
	char nameEnd;
	char sampleState;
	char semantic;
#ifdef WAW
	int unknown;
#endif
	MaterialTextureDefInfo u;
}MaterialTextureDef;


/* 6901 */
#pragma pack(push, 4)
struct MaterialPass
{
  struct MaterialVertexDeclaration *vertexDecl;
  struct MaterialVertexShader *vertexShader;
  struct MaterialPixelShader *pixelShader;
  char perPrimArgCount;
  char perObjArgCount;
  char stableArgCount;
  char customSamplerFlagsOrprecompiledIndex;
  struct MaterialShaderArgument *args;
};
#pragma pack(pop)

/* 6902 */
typedef struct MaterialTechnique
{
  const char *name;
  uint16_t flags;
  uint16_t  passCount;
  struct MaterialPass passArray[1];
} MaterialTechnique;


	/* 6888 */
#pragma pack(push, 2)
typedef struct MaterialTechniqueSet
{
  const char *name;
  char worldVertFormat;
  char unused[3];
  struct MaterialTechniqueSet *remappedTechniqueSet;
  struct MaterialTechnique *techniques[TECHNIQUE_COUNT];
} MaterialTechniqueSet;
#pragma pack(pop)



typedef struct
{
  uint64_t _bf0;
}GfxDrawSurfFields;


typedef union
{
  GfxDrawSurfFields fields;
  uint64_t packed;
}GfxDrawSurf;


typedef struct
{
	int loadBits[2];
}GfxStateBits;


typedef struct __attribute__((aligned (8)))
{
	const char *name;
	char gameFlags;
	char sortKey;
	char textureAtlasRowCount;
	char textureAtlasColumnCount;
	GfxDrawSurf drawSurf;
	int surfaceTypeBits;
}MaterialInfo;

typedef struct __attribute__((aligned (8))) Material_s
{
	MaterialInfo info;
	char stateBitsEntry[TECHNIQUE_COUNT];	// see MaterialTechniqueType
	char textureCount;
	char constantCount;
	char stateBitsCount;
	char stateFlags;
	char cameraRegion;
	char unk;
	struct MaterialTechniqueSet *techniqueSet;
	MaterialTextureDef *textureTable;
	struct MaterialConstantDef *constantTable;
	GfxStateBits *stateBitTable;


#ifdef MW2
	const char **unknownXStringArray;
#else
	#ifdef MW3
	const char **unknownXStringArray;
	#endif
#endif


}Material;


typedef Material Material_t;
Material* Material_RegisterHandle(const char* name, int track);
qboolean Material_IsDefault(Material *);
const char *__cdecl Material_GetName(Material *a1);

#endif
