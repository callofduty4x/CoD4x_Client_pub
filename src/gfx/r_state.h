#include "r_types_d3d.h"

extern float shadowmapClearColor[4];

static inline void R_SetCodeImageTexture(GfxCmdBufSourceState *source, unsigned int codeTexture, GfxImage *image)
{
    assert(source);
    assert(codeTexture < TEXTURE_SRC_CODE_COUNT);

    source->input.codeImages[codeTexture] = image;
    //source->input.codeImageRenderTargetControl[codeTexture].packed = 0;
}

static inline void R_DirtyCodeConstant(GfxCmdBufSourceState *source, unsigned int constant)
{
  assert( constant < ARRAY_COUNT( source->constVersions ));

  ++source->constVersions[constant];
}

static inline void R_SetCodeConstantFromVec4(GfxCmdBufSourceState *source, unsigned int constant, const float *value)
{
  assert(constant < CONST_SRC_CODE_COUNT_FLOAT4);

  Vector4Copy(value,source->input.consts[constant]);

  R_DirtyCodeConstant(source, constant);
}

void R_FinishGpuFence();
