#include "r_local.h"
#include "r_image.h"
#include "r_material.h"
#include "rb_backend.h"
#include "gfx_world.h"


#define GFXS0_ATEST_DISABLE 0x800
#define GFXS0_CULL_MASK 0xC000
#define GFXS0_CULL_SHIFT 14
#define GFXS0_CULL_NONE 0x4000
#define GFXS0_CULL_FRONT 0xC000
#define GFXS0_CULL_BACK 0x8000
#define GFXS0_POLYMODE_LINE 0x80000000
#define GFXS0_BLENDOP_RGB_SHIFT 8
#define GFXS_BLENDOP_MASK 0x7
#define GFXS_BLENDOP_DISABLED 0x7
#define GFXS0_BLENDOP_ALPHA_MASK 0x7000000
#define GFXS0_SRCBLEND_RGB_SHIFT 0
#define GFXS_BLEND_MASK 0xF
#define GFXS0_DSTBLEND_RGB_SHIFT 4
#define GFXS0_BLENDOP_ALPHA_SHIFT 24
#define GFXS0_SRCBLEND_ALPHA_SHIFT 16
#define GFXS0_DSTBLEND_ALPHA_SHIFT 20
#define GFXS1_DEPTHWRITE 1
#define GFXS1_DEPTHTEST_DISABLE 2
#define GFXS1_DEPTHTEST_SHIFT 2
#define GFXS1_DEPTHTEST_MASK 0xC
#define GFXS1_POLYGON_OFFSET_SHADOWMAP 48
#define GFXS0_ATEST_MASK 0x3000
#define GFXS0_ATEST_GE_128 0x3000
#define GFXS1_STENCIL_BACK_ENABLE 0x40
#define GFXS1_STENCIL_FRONT_ENABLE 0x80

#define FloatAsInt(x) (*(int*)x)


float shadowmapClearColor[4] = {  1.0,  1.0,  1.0,  1.0 };
const unsigned int s_cullTable[] = { 0u, 1u, 3u, 2u };
const unsigned int s_blendTable[] = { 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u };
const unsigned int s_blendOpTable[] = { 0u, 1u, 2u, 3u, 4u, 5u };
const unsigned int s_depthTestTable[] = { 8u, 2u, 3u, 4u };
const unsigned int s_stencilOpTable[] = { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u };
const unsigned int s_stencilFuncTable[] = { 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u };

#define sm_polygonOffsetScale getcvaradr(0xD5695F0)
#define sm_polygonOffsetBias getcvaradr(0xD569850)
#define r_polygonOffsetScale getcvaradr(0xD569600)
#define r_polygonOffsetBias getcvaradr(0xD569628)

#define gfxMetrics (*((struct GfxMetrics*)(0x0CC9F450)))


void R_HW_SetColorMask(D3DDevice *device, unsigned int stateBits0)
{
  int mask = 0;
  if(stateBits0 & 0x8000000)
  {
    mask |= 7;
  }
  if(stateBits0 & 0x10000000)
  {
    mask |= 8;
  }

  D3DCALL(device->SetRenderState( D3DRS_COLORWRITEENABLE, mask ));

}

void R_HW_SetAlphaTestEnable(D3DDevice *device, unsigned int stateBits0)
{
  D3DCALL(device->SetRenderState( D3DRS_ALPHATESTENABLE, (stateBits0 & GFXS0_ATEST_DISABLE) ? 0 : 1 ));
}

void R_HW_SetCullFace(D3DDevice *device, unsigned int stateBits0)
{
  assert((stateBits0 & GFXS0_CULL_MASK) == GFXS0_CULL_NONE || (stateBits0 & GFXS0_CULL_MASK) == GFXS0_CULL_FRONT || (stateBits0 & GFXS0_CULL_MASK) == GFXS0_CULL_BACK);
  D3DCALL(device->SetRenderState( D3DRS_CULLMODE, s_cullTable[(stateBits0 & GFXS0_CULL_MASK) >> GFXS0_CULL_SHIFT] ));

}


void R_SetAlphaTestFunction(GfxCmdBufState *state, unsigned int stateBits0)
{
  uint8_t ref;
  D3DDevice *device;
  unsigned int function;

  if ( (stateBits0 & GFXS0_ATEST_MASK) == 0x1000 )
  {
    function = 5;
    ref = 0;
  }
  else if ( (stateBits0 & GFXS0_ATEST_MASK) == 0x2000 )
  {
#ifdef BLACKOPS    
    function = 7;
    ref = 255;
#else
    function = 2;
    ref = 128;
#endif
  }
  else
  {
    assert((stateBits0 & GFXS0_ATEST_MASK) == GFXS0_ATEST_GE_128);
    function = 7;
    ref = 128;
  }
  device = state->prim.device;

  assert(device);

  D3DCALL(device->SetRenderState( D3DRS_ALPHAFUNC, function ));

  if ( state->alphaRef != ref )
  {
    D3DCALL(device->SetRenderState( D3DRS_ALPHAREF, ref ));
    state->alphaRef = ref;
  }
}

void R_HW_SetPolygonMode(D3DDevice *device, unsigned int stateBits0)
{
  D3DCALL(device->SetRenderState( D3DRS_FILLMODE, (stateBits0 & GFXS0_POLYMODE_LINE) ? D3DFILL_WIREFRAME : D3DFILL_SOLID ));
}


void R_HW_SetBlend(D3DDevice *device, bool blendWasEnabled, unsigned int changedBits, unsigned int stateBits0)
{

  if ( !blendWasEnabled )
  {
    D3DCALL(device->SetRenderState( D3DRS_ALPHABLENDENABLE, 1 ));
  }
  if ( (changedBits >> GFXS0_BLENDOP_RGB_SHIFT) & GFXS_BLENDOP_MASK )
  {
    D3DCALL(device->SetRenderState( D3DRS_BLENDOP, s_blendOpTable[(stateBits0 >> GFXS0_BLENDOP_RGB_SHIFT) & GFXS_BLENDOP_MASK] ));
  }
  if ( (changedBits >> GFXS0_SRCBLEND_RGB_SHIFT) & GFXS_BLEND_MASK )
  {
    D3DCALL(device->SetRenderState( D3DRS_SRCBLEND, s_blendTable[(stateBits0 >> GFXS0_SRCBLEND_RGB_SHIFT) & GFXS_BLEND_MASK] ));
  }
  if ( (changedBits >> GFXS0_DSTBLEND_RGB_SHIFT) & GFXS_BLEND_MASK )
  {
    D3DCALL(device->SetRenderState( D3DRS_DESTBLEND, s_blendTable[(stateBits0 >> GFXS0_DSTBLEND_RGB_SHIFT) & GFXS_BLEND_MASK] ));
  }
  if ( (changedBits >> GFXS0_BLENDOP_ALPHA_SHIFT) & GFXS_BLENDOP_MASK )
  {
    D3DCALL(device->SetRenderState( D3DRS_BLENDOPALPHA, s_blendOpTable[(stateBits0 >> GFXS0_BLENDOP_ALPHA_SHIFT) & GFXS_BLENDOP_MASK] ));
  }
  if ( (changedBits >> GFXS0_SRCBLEND_ALPHA_SHIFT) & GFXS_BLEND_MASK )
  {
    D3DCALL(device->SetRenderState( D3DRS_SRCBLENDALPHA, s_blendTable[(stateBits0 >> GFXS0_SRCBLEND_ALPHA_SHIFT) & GFXS_BLEND_MASK] ));
  }

  if ( (changedBits >> GFXS0_DSTBLEND_ALPHA_SHIFT) & GFXS_BLEND_MASK )
  {
    D3DCALL(device->SetRenderState( D3DRS_DESTBLENDALPHA, s_blendTable[(stateBits0 >> GFXS0_DSTBLEND_ALPHA_SHIFT) & GFXS_BLEND_MASK] ));
  }
}

void R_HW_DisableBlend(D3DDevice *device)
{
  D3DCALL(device->SetRenderState( D3DRS_ALPHABLENDENABLE, 0 ));
}


void R_HW_SetDepthWriteEnable(D3DDevice *device, unsigned int stateBits1)
{
  D3DCALL(device->SetRenderState(D3DRS_ZWRITEENABLE, (stateBits1 & GFXS1_DEPTHWRITE) ? 1 : 0));
}

void R_HW_SetDepthTestEnable(D3DDevice *device, unsigned int stateBits1)
{
  D3DCALL(device->SetRenderState(D3DRS_ZENABLE, (stateBits1 & GFXS1_DEPTHTEST_DISABLE) ? D3DZB_FALSE : D3DZB_TRUE));
}

void R_HW_SetDepthTestFunction(D3DDevice *device, unsigned int stateBits1)
{
  D3DCALL(device->SetRenderState( D3DRS_ZFUNC, s_depthTestTable[(stateBits1 & GFXS1_DEPTHTEST_MASK) >> GFXS1_DEPTHTEST_SHIFT] ));
}

void R_HW_SetPolygonOffset(D3DDevice *device, float scale, float bias)
{
  if ( gfxMetrics.slopeScaleDepthBias )
  {
    D3DCALL(device->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, FloatAsInt( &scale ) ));
  }
  else
  {
    bias = bias * 2.0;
  }
  D3DCALL(device->SetRenderState( D3DRS_DEPTHBIAS, FloatAsInt( &bias ) ));
}

void R_HW_EnableStencil(D3DDevice *device)
{
  D3DCALL(device->SetRenderState( D3DRS_STENCILENABLE, 1 ));
}

void R_HW_DisableStencil(D3DDevice *device)
{
  D3DCALL(device->SetRenderState( D3DRS_STENCILENABLE, 0 ));
}
/*
void R_SetAlphaAntiAliasingState(IDirect3DDevice9 *device, int stateBits0)
{
  DWORD aaAlphaFormat;

  if ( (stateBits0 >> GFXS0_SRCBLEND_ALPHA_SHIFT) & GFXS_BLEND_MASK )
  {
    aaAlphaFormat = 0;
  }
  else if ( r_aaAlpha->current.integer == 2 )
  {
    aaAlphaFormat = (D3DFORMAT)MAKEFOURCC('S', 'S', 'A', 'A');
  }
  else
  {
    aaAlphaFormat = (D3DFORMAT)MAKEFOURCC('A', 'T', 'O', 'C');
  }
  D3DCALL(device->SetRenderState( D3DRS_ADAPTIVETESS_Y, aaAlphaFormat ));
}
*/
void R_HW_SetFrontStencilFunc(D3DDevice *device, unsigned int stencilFunc)
{
  D3DCALL(device->SetRenderState( D3DRS_STENCILFUNC, s_stencilFuncTable[stencilFunc] ));
}

void R_HW_SetBackStencilFunc(D3DDevice *device, unsigned int stencilFunc)
{
  D3DCALL(device->SetRenderState( D3DRS_CCW_STENCILFUNC, s_stencilFuncTable[stencilFunc] ));
}

void R_ForceSetPolygonOffset(D3DDevice *device, unsigned int stateBits1)
{
  unsigned int offset;
  float bias;
  float scale;

  offset = stateBits1 & 0x30;
  if ( offset == GFXS1_POLYGON_OFFSET_SHADOWMAP )
  {
    bias = sm_polygonOffsetBias->value * 0.000015258789;
    scale = sm_polygonOffsetScale->value;
  }
  else
  {
    assert(offset != GFXS1_POLYGON_OFFSET_SHADOWMAP);

    offset >>= 4;
    bias = (double)offset * r_polygonOffsetBias->value * 0.000015258789;
    scale = (double)offset * r_polygonOffsetScale->value;
  }
  R_HW_SetPolygonOffset(device, scale, bias);
}

void R_HW_SetFrontStencilOp(D3DDevice *device, unsigned int stencilOpPass, unsigned int stencilOpFail, unsigned int stencilOpZFail)
{
  D3DCALL(device->SetRenderState( D3DRS_STENCILPASS, s_stencilOpTable[stencilOpPass] ));
  D3DCALL(device->SetRenderState( D3DRS_STENCILFAIL, s_stencilOpTable[stencilOpFail] ));
  D3DCALL(device->SetRenderState( D3DRS_STENCILZFAIL, s_stencilOpTable[stencilOpZFail] ));
}

void R_HW_SetBackStencilOp(D3DDevice *device, unsigned int stencilOpPass, unsigned int stencilOpFail, unsigned int stencilOpZFail)
{
  D3DCALL(device->SetRenderState( D3DRS_CCW_STENCILPASS, s_stencilOpTable[stencilOpPass] ));
  D3DCALL(device->SetRenderState( D3DRS_CCW_STENCILFAIL, s_stencilOpTable[stencilOpFail] ));
  D3DCALL(device->SetRenderState( D3DRS_CCW_STENCILZFAIL, s_stencilOpTable[stencilOpZFail] ));
}


extern "C" __attribute__((regparm(1))) void R_ChangeState_0(GfxCmdBufState *state, unsigned int stateBits0)
{
  bool blendOpRgbWasEnabled;
  unsigned int changedBits;
  D3DDevice *device;

  changedBits = state->activeStateBits[0] ^ stateBits0;
  if ( changedBits || (state->refStateBits[0] ^ stateBits0) & 0x7000700 )
  {
//    assert(r_dx.d3d9 && r_dx.device);
    
    device = state->prim.device;

    assert(device);

    if ( changedBits & 0x18000000 )
    {
      R_HW_SetColorMask(device, stateBits0);
    }
/*    
    if ( !(state->stateOverride & 0x40000000) )
    {
*/
      if ( changedBits & GFXS0_ATEST_DISABLE )
      {
        R_HW_SetAlphaTestEnable(state->prim.device, stateBits0);
      }
      if ( stateBits0 & GFXS0_ATEST_DISABLE )
      {
        assert((stateBits0 & GFXS0_ATEST_MASK) == 0);

        stateBits0 |= state->activeStateBits[0] & GFXS0_ATEST_MASK;
        changedBits &= ~GFXS0_ATEST_MASK;

        assert((stateBits0 ^ state->activeStateBits[0]) == (unsigned)changedBits);
      }
      if ( changedBits & GFXS0_ATEST_MASK )
      {
        R_SetAlphaTestFunction(state, stateBits0);
      }
/*
    }
*/
    if ( changedBits & GFXS0_CULL_MASK )
    {
      R_HW_SetCullFace(device, stateBits0);
    }
    if ( changedBits & GFXS0_POLYMODE_LINE )
    {
      R_HW_SetPolygonMode(device, stateBits0);
    }


    blendOpRgbWasEnabled = ((state->refStateBits[0] >> GFXS0_BLENDOP_RGB_SHIFT) & GFXS_BLENDOP_MASK) != 0;
    if ( (stateBits0 >> GFXS0_BLENDOP_RGB_SHIFT) & GFXS_BLENDOP_MASK )                                                                                                      //CHANGE!
    {
      if ( !((stateBits0 >> GFXS0_BLENDOP_ALPHA_SHIFT) & GFXS_BLENDOP_MASK) )
      {

        stateBits0 = (stateBits0 & ~(0x7FF << GFXS0_SRCBLEND_ALPHA_SHIFT)) | ((stateBits0 & 0x7FF) << GFXS0_SRCBLEND_ALPHA_SHIFT);
        changedBits = (changedBits & ~(0x7FF << GFXS0_SRCBLEND_ALPHA_SHIFT)) | ((state->activeStateBits[0] ^ stateBits0) & (0x7FF << GFXS0_SRCBLEND_ALPHA_SHIFT));

        assert((stateBits0 ^ state->activeStateBits[0]) == changedBits);

      }
      R_HW_SetBlend(device, blendOpRgbWasEnabled, changedBits, stateBits0);
    }
    else
    {
      assert((stateBits0 & GFXS0_BLENDOP_ALPHA_MASK) == (GFXS_BLENDOP_DISABLED << GFXS0_BLENDOP_ALPHA_SHIFT));

      stateBits0 = (stateBits0 & ~0x7FF07FFu) | (state->activeStateBits[0] & 0x7FF07FF);
      
      changedBits &= ~0x7FF07FFu;

      assert((stateBits0 ^ state->activeStateBits[0]) == changedBits);

      if ( blendOpRgbWasEnabled )
      {
        R_HW_DisableBlend(device);
      }
    }
/*
    if ( gfxMetrics.hasTransparencyMsaa )
    {
      if ( r_aaAlpha->current.integer )
      {
        if ( changedBits & (GFXS_BLEND_MASK << GFXS0_BLENDOP_RGB_SHIFT))
        {
          R_SetAlphaAntiAliasingState(device, stateBits0);
        }
      }
    }
*/    
    state->activeStateBits[0] = stateBits0;
  }
}



extern "C" void R_ChangeState_1(GfxCmdBufState *state, unsigned int stateBits1)
{
  unsigned int changedBits;
  D3DDevice *device;

  changedBits = state->activeStateBits[1] ^ stateBits1;
  if ( changedBits )
  {
    assert((!(stateBits1 & GFXS1_STENCIL_BACK_ENABLE)) | (stateBits1 & GFXS1_STENCIL_FRONT_ENABLE));


    assert(r_dx.d3d9 && r_dx.device);

    device = state->prim.device;

    assert(device);
/*
    if ( state->stateOverride >= 0 )
    {
*/
      if ( changedBits & GFXS1_DEPTHWRITE )
      {
        R_HW_SetDepthWriteEnable(device, stateBits1);
      }
      if ( changedBits & GFXS1_DEPTHTEST_DISABLE )
      {
        R_HW_SetDepthTestEnable(device, stateBits1);
      }
      if ( stateBits1 & GFXS1_DEPTHTEST_DISABLE )
      {
        assert((stateBits1 & GFXS1_DEPTHTEST_MASK) == 0);

        stateBits1 |= state->activeStateBits[1] & GFXS1_DEPTHTEST_MASK;
        changedBits &= ~GFXS1_DEPTHTEST_MASK;

        assert((stateBits1 ^ state->activeStateBits[1]) == changedBits);
      }
      if ( changedBits & GFXS1_DEPTHTEST_MASK )
      {
        R_HW_SetDepthTestFunction(device, stateBits1);
      }
/*
    }
    
*/  
    if ( changedBits & GFXS1_POLYGON_OFFSET_SHADOWMAP )
    {
      R_ForceSetPolygonOffset(device, stateBits1);
    }
    if ( stateBits1 & GFXS1_STENCIL_FRONT_ENABLE )
    {
      if ( changedBits & GFXS1_STENCIL_FRONT_ENABLE )
      {
        R_HW_EnableStencil(device);
      }
    }
    else
    {
      if ( changedBits & GFXS1_STENCIL_FRONT_ENABLE )
      {
        R_HW_DisableStencil(device);
      }
      stateBits1 = (stateBits1 & 0x7F) | (state->activeStateBits[1] & 0xFFFFFF80);
      changedBits &= 0x7Fu;
    }
    if ( !(stateBits1 & GFXS1_STENCIL_BACK_ENABLE) )
    {
      stateBits1 = (stateBits1 & 0xFFFFF) | ((stateBits1 & 0xFFF00) << 12);
      changedBits = state->activeStateBits[1] ^ stateBits1;
    }
    if ( changedBits & 0x1FF00 )
    {
      R_HW_SetFrontStencilOp(device, (stateBits1 >> GFXS0_BLENDOP_RGB_SHIFT) & GFXS_BLENDOP_MASK, (stateBits1 >> 11) & GFXS_BLENDOP_MASK, (stateBits1 >> GFXS0_CULL_SHIFT) & GFXS_BLENDOP_MASK);
    }
    if ( (0x3800000 | 0x700000 | 0x1C000000) & changedBits )
    {
      R_HW_SetBackStencilOp(device, (stateBits1 >> GFXS0_DSTBLEND_ALPHA_SHIFT) & GFXS_BLENDOP_MASK, (stateBits1 >> 23) & GFXS_BLENDOP_MASK, (stateBits1 >> 26) & GFXS_BLENDOP_MASK);
    }
    if ( changedBits & 0xE0000 )
    {
      R_HW_SetFrontStencilFunc(device, (stateBits1 >> 17) & GFXS_BLENDOP_MASK);
    }
    if ( changedBits & 0xE0000000 )
    {
      R_HW_SetBackStencilFunc(device, stateBits1 >> 29);
    }
    state->activeStateBits[1] = stateBits1;
  }
}


void R_ChangeStreamSource(GfxCmdBufPrimState *state, int streamIndex, D3DVertexBuffer *vb, unsigned int vertexOffset, unsigned int vertexStride)
{
  D3DDevice *device;

  device = state->device;
  assert(device);
  assert(state->streams[streamIndex].vb != vb || state->streams[streamIndex].offset != vertexOffset || state->streams[streamIndex].stride != vertexStride);

  state->streams[streamIndex].vb = vb;
  state->streams[streamIndex].offset = vertexOffset;
  state->streams[streamIndex].stride = vertexStride;
  
  device->SetStreamSource(streamIndex, vb, vertexOffset, vertexStride);
}


void R_DrawIndexedPrimitive(GfxCmdBufPrimState *state, const GfxDrawPrimArgs *args)
{
  signed int triCount = args->triCount;
  D3DDevice *device = state->device;
/*
  triCount = args->triCount;
  if ( triCount >= r_drawPrimFloor->current.integer && (!r_drawPrimCap->current.integer || triCount <= r_drawPrimCap->current.integer) )
  {
    if ( r_skipDrawTris->current.enabled )
    {
      triCount = 1;
    }
    RB_TrackDrawPrimCall(state, triCount);
*/
    device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, args->vertexCount, args->baseIndex, triCount);
//}

}


unsigned int R_HW_SetSamplerState(D3DDevice *device, unsigned int samplerIndex, unsigned int samplerState, unsigned int oldSamplerState)
{
    unsigned int finalSamplerState;
    unsigned int diffSamplerState;

    finalSamplerState = samplerState;
    diffSamplerState = oldSamplerState ^ samplerState;

    assert(diffSamplerState);

    if ( diffSamplerState & 0xF00 )
    {
      unsigned int minFilter = (samplerState & 0xF00) >> 8;
      device->SetSamplerState( samplerIndex, D3DSAMP_MINFILTER, minFilter );
    }
    if ( diffSamplerState & 0xF000 )
    {
      unsigned int magFilter = (samplerState & 0xF000) >> 12;
      device->SetSamplerState( samplerIndex, D3DSAMP_MAGFILTER, magFilter );
    }
    if ( diffSamplerState & 0xFF )
    {
      unsigned int anisotropy = samplerState & 0xFF;
      if ( anisotropy <= 1 )
      {
        finalSamplerState = (oldSamplerState & 0xFF) | (samplerState & 0xFFFFFF00);
      }
      else
      {
        device->SetSamplerState( samplerIndex, D3DSAMP_MAXANISOTROPY, anisotropy );
      }
    }
    if ( diffSamplerState & 0xF0000 )
    {
      unsigned int mipFilter = (samplerState & 0xF0000) >> 16;
      device->SetSamplerState( samplerIndex, D3DSAMP_MIPFILTER, mipFilter );
    }
    if ( diffSamplerState & 0x3F00000 )
    {
        if ( diffSamplerState & 0x300000 )
        {
          unsigned int address = (samplerState & 0x300000) >> 20;
          device->SetSamplerState( samplerIndex, D3DSAMP_ADDRESSU, address );
        }

        if ( diffSamplerState & 0xC00000 )
        {
          unsigned int address = (samplerState & 0xC00000) >> 22;
          device->SetSamplerState( samplerIndex, D3DSAMP_ADDRESSV, address );
        }
        if ( diffSamplerState & 0x3000000 )
        {
          unsigned int address = (samplerState & 0x3000000) >> 24;
          device->SetSamplerState( samplerIndex, D3DSAMP_ADDRESSW, address );
        }
    }
    return finalSamplerState;
}



void R_HW_SetSamplerTexture(D3DDevice *device, unsigned int samplerIndex, const GfxTexture *texture)
{
  device->SetTexture( samplerIndex, texture->basemap );
}


void R_SetSampler(GfxCmdBufContext context, unsigned int samplerIndex, unsigned char samplerState, const GfxImage *image)
{
    unsigned int decodedSamplerState;

    if ( context.state->samplerTexture[samplerIndex] != &image->texture )
    {
        context.state->samplerTexture[samplerIndex] = &image->texture;
        R_HW_SetSamplerTexture(context.state->prim.device, samplerIndex, &image->texture);
    }

    if ( context.state->refSamplerState[samplerIndex] != samplerState )
    {
        context.state->refSamplerState[samplerIndex] = samplerState;
        decodedSamplerState = R_DecodeSamplerState(samplerState);
        if ( context.state->samplerState[samplerIndex] != decodedSamplerState )
        {
            context.state->samplerState[samplerIndex] = R_HW_SetSamplerState(context.state->prim.device, samplerIndex, decodedSamplerState, context.state->samplerState[samplerIndex]);
        }
    }
}



void R_SetLightmap(GfxCmdBufContext context, unsigned int lmapIndex)
{
    MaterialPass *pass;
    GfxImage *overrideImage;
    assert(rgp.world);

    pass = context.state->pass;
    if ( lmapIndex == 31 )
    {
      return;
    }
    else
    {
        if ( r_lightMap->integer == 1 )
        {
            if ( pass->customSamplerFlags & 2 )
            {
                if ( context.source->input.data->prim.hasSunDirChanged )
                {
                    R_SetSampler(context, 2u, 98, rgp.whiteImage);
                }
                else
                {
                    R_SetSampler(context, 2u, 98, rgp.world->lightmaps[lmapIndex].primary);
                }
            }
            if ( pass->customSamplerFlags & 4 )
            {
                R_SetSampler(context, 3u, 98, rgp.world->lightmaps[lmapIndex].secondary);
            }
            /*
            if ( pass->customSamplerFlags & 8 )
            {
                R_SetSampler(context, 14u, 98, rgp.world->lightmaps[lmapIndex].secondaryB);
            }
            */
        }
        else
        {
            if(r_lightMap->integer == 2)
            {
              overrideImage = rgp.whiteImage;
            }else if(r_lightMap->integer == 0){
              overrideImage = rgp.blackImage;
            }else{
              overrideImage = rgp.grayImage;
            }
            if ( pass->customSamplerFlags & 2 )
            {
                R_SetSampler(context, 2u, 98, overrideImage);
            }
            if ( pass->customSamplerFlags & 4 )
            {
                R_SetSampler(context, 3u, 98, overrideImage);
            }
            /*
            if ( pass->customSamplerFlags & 8 )
            {
                R_SetSampler(context, 4u, 98, rgp.g16r16Image);
            }
            */
        }
    }
}


#define REFLECTION_PROBE_INVALID 255

void R_SetReflectionProbe(GfxCmdBufContext context, unsigned int reflectionProbeIndex)
{
  if ( context.state->pass->customSamplerFlags & 1 )
  {
    R_SetSampler(context, 1u, 114, rgp.world->reflectionProbes[reflectionProbeIndex].reflectionImage);
  }
}



GfxImage * R_GetTextureFromCode(GfxCmdBufSourceState *source, unsigned int codeTexture, char *samplerState)
{
    assert(codeTexture < TEXTURE_SRC_CODE_COUNT);
    assert(source);

    *samplerState = source->input.codeImageSamplerStates[codeTexture];

    assert(*samplerState & SAMPLER_FILTER_MASK);

    return source->input.codeImages[codeTexture];
}


void ikMatrixSet44(float (*out)[4], const float *origin, const float (*axis)[3], float scale)
{
    (*out)[0] = (*axis)[0] * scale;
    (*out)[1] = (float)(*axis)[1] * scale;
    (*out)[2] = (float)(*axis)[2] * scale;
    (*out)[3] = 0.0;
    (*out)[4] = (float)(*axis)[3] * scale;
    (*out)[5] = (float)(*axis)[4] * scale;
    (*out)[6] = (float)(*axis)[5] * scale;
    (*out)[7] = 0.0;
    (*out)[8] = (float)(*axis)[6] * scale;
    (*out)[9] = (float)(*axis)[7] * scale;
    (*out)[10] = (float)(*axis)[8] * scale;
    (*out)[11] = 0.0;
    (*out)[12] = *origin;
    (*out)[13] = origin[1];
    (*out)[14] = origin[2];
    (*out)[15] = 1.0;
}

void zUnitQuatToAxis(const float *quat, float (*axis)[3])
{
  float v3; // ST18_4@1
  float v4; // ST10_4@1
  float v5; // ST14_4@1
  float v6; // ST00_4@1
  float v8; // ST08_4@1
  float v9; // ST1C_4@1
  float v10; // ST00_4@1
  float v12; // ST0C_4@1
  float v13; // ST04_4@1

  vec3_t quat2;
  
  VectorAdd(quat, quat ,quat2);

  v3 = quat[0] * quat2[0];
  v4 = quat[1] * quat2[0];
  v5 = quat[2] * quat2[0];
  v6 = quat[3] * quat2[0];
  v8 = quat[1] * quat2[1];
  v9 = quat[2] * quat2[1];
  v10 = quat[3] * quat2[1];
  v12 = quat[3] * quat2[2];
  v13 = quat[2] * quat2[2];
  
  (*axis)[0] = 1.0 - (v13 + v8);
  (*axis)[1] = v12 + v4;
  (*axis)[2] = v5 - v10;
  (*axis)[3] = v4 - v12;
  (*axis)[4] = 1.0 - (v13 + v3);
  (*axis)[5] = v9 + v6;
  (*axis)[6] = v5 + v10;
  (*axis)[7] = v9 - v6;
  (*axis)[8] = 1.0 - (v3 + v8);
}


void R_ChangeObjectPlacement_Core(GfxCmdBufSourceState *source, GfxScaledPlacement *placement)
{
    double v2; // st7
    char *v3; // eax
    double v4; // st7
    char *v5; // eax
    double v6; // st7
    char *v7; // eax
    GfxCmdBufSourceState *matrix; // ST44_4
    float origin[3]; // [esp+44h] [ebp-30h]
    vec3_t axis[3]; // [esp+50h] [ebp-24h]

    assert(placement);

    zUnitQuatToAxis(placement->base.quat, axis);
    assert(Vec3IsNormalized( axis[0] ));
    assert(Vec3IsNormalized( axis[1] ));
    assert(Vec3IsNormalized( axis[2] ));

    matrix = R_GetActiveWorldMatrix(source);
    origin[0] = placement->base.origin[0] - source->eyeOffset[0];
    origin[1] = placement->base.origin[1] - source->eyeOffset[1];
    origin[2] = placement->base.origin[2] - source->eyeOffset[2];
    ikMatrixSet44((float (*)[4])matrix, origin, axis, placement->scale);
}

void R_ChangeObjectPlacement(GfxCmdBufSourceState *source, GfxScaledPlacement *placement)
{
    R_ChangeObjectPlacement_Core(source, placement);
}

void R_ChangeObjectPlacementRemote(GfxCmdBufSourceState *source, GfxScaledPlacement *remotePlacement)
{
  R_ChangeObjectPlacement(source, remotePlacement);
}

void R_DepthHackNearClipChanged(GfxCmdBufSourceState *source)
{
  ++source->matrixVersions[2];
  ++source->matrixVersions[4];
  ++source->matrixVersions[5];
  source->input.consts[54][3] = -source->input.consts[54][3];
  ++source->constVersions[54];
}


void R_ChangeDepthHackNearClip(GfxCmdBufSourceState *source, unsigned int depthHackFlags)
{
  assert(source);
  
  if ( depthHackFlags != source->depthHackFlags )
  {
    R_DepthHackNearClipChanged(source);
    source->depthHackFlags = depthHackFlags;
  }
}

void R_HW_SetViewport(D3DDevice *device, const GfxViewport *viewport, float nearValue, float farValue)
{
  D3DVIEWPORT9 d3dViewport;

  d3dViewport.X = viewport->x;
  d3dViewport.Y = viewport->y;
  d3dViewport.Width = viewport->width;
  d3dViewport.Height = viewport->height;
  d3dViewport.MinZ = nearValue;
  d3dViewport.MaxZ = farValue;

  assert(d3dViewport.MinZ < d3dViewport.MaxZ);

  D3DCALL(device->SetViewport( &d3dViewport ));
}


void R_ChangeDepthRange(GfxCmdBufState *state, GfxDepthRangeType depthRangeType)
{
  D3DDevice *device;

  assert(state->depthRangeType != depthRangeType);

  state->depthRangeType = depthRangeType;
  if ( depthRangeType )
  {
    state->depthRangeNear = 0.0;
    state->depthRangeFar = 0.015625;
  }
  else
  {
    state->depthRangeNear = 0.015625;
    state->depthRangeFar = 1.0;
  }

  device = state->prim.device;
  assert(device);

  R_HW_SetViewport(device, &state->viewport, state->depthRangeNear, state->depthRangeFar);
}


void R_ChangeIndices(GfxCmdBufPrimState *state, D3DIndexBuffer *ib)
{
  D3DDevice *device;

  assert(ib != state->indexBuffer);

  state->indexBuffer = ib;
  device = state->device;
  assert(device);

  D3DCALL(device->SetIndices( ib ));

}