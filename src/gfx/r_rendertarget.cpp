#include "r_local.h"
#include "rb_backend.h"


const char *s_renderTargetNames[] =
{
  "R_RENDERTARGET_SAVED_SCREEN",
  "R_RENDERTARGET_FRAME_BUFFER",
  "R_RENDERTARGET_SCENE",
  "R_RENDERTARGET_RESOLVED_POST_SUN",
  "R_RENDERTARGET_RESOLVED_SCENE",
  "R_RENDERTARGET_FLOAT_Z",
  "R_RENDERTARGET_DYNAMICSHADOWS",
  "R_RENDERTARGET_PINGPONG_0",
  "R_RENDERTARGET_PINGPONG_1",
  "R_RENDERTARGET_SHADOWCOOKIE",
  "R_RENDERTARGET_SHADOWCOOKIE_BLUR",
  "R_RENDERTARGET_POST_EFFECT_0",
  "R_RENDERTARGET_POST_EFFECT_1",
  "R_RENDERTARGET_SHADOWMAP_SUN",
  "R_RENDERTARGET_SHADOWMAP_SPOT"
};

void R_HW_SetRenderTarget(GfxCmdBufState *state, unsigned int newTargetId)
{
  IDirect3DDevice9 *device;

  //R_PixStartNamedRenderTarget(newTargetId);

  device = state->prim.device;
  assert(device);

  if ( gfxRenderTargets[(uint8_t)state->renderTargetId].surface.color != gfxRenderTargets[(uint8_t)newTargetId].surface.color )
  {
    D3DCALL(device->SetRenderTarget( 0, gfxRenderTargets[newTargetId].surface.color ));

    state->viewport.x = 0;
    state->viewport.y = 0;
    state->viewport.width = gfxRenderTargets[(uint8_t)newTargetId].width;
    state->viewport.height = gfxRenderTargets[(uint8_t)newTargetId].height;
    state->depthRangeType = GFX_DEPTH_RANGE_FULL;
    state->depthRangeNear = 0.0;
    state->depthRangeFar = 1.0;
  }
  if ( gfxRenderTargets[(uint8_t)state->renderTargetId].surface.depthStencil != gfxRenderTargets[(uint8_t)newTargetId].surface.depthStencil )
  {
      D3DCALL(device->SetDepthStencilSurface( gfxRenderTargets[newTargetId].surface.depthStencil ));
  }
}


const char * R_RenderTargetName(GfxRenderTargetId renderTargetId)
{
  return s_renderTargetNames[(uint8_t)renderTargetId];
}

void R_SetRenderTarget(GfxCmdBufContext context, GfxRenderTargetId newTargetId)
{
  if ( pixelCostMode > GFX_PIXEL_COST_MODE_MEASURE_MSEC )
  {
    newTargetId = RB_PixelCost_OverrideRenderTarget(newTargetId);
  }
  if ( newTargetId != context.state->renderTargetId )
  {
    //_RB_LogPrintf("\n========== R_SetRenderTarget( %s ) ==========\n\n", R_RenderTargetName(newTargetId));
#ifdef BLACKOPS    
    R_UpdateStatsTarget(context, (uint8_t)newTargetId);
#endif
    if ( gfxRenderTargets[(uint8_t)newTargetId].image )
    {
      R_UnbindImage(context.state, gfxRenderTargets[(unsigned __int8)newTargetId].image);
    }

    assert(context.source->renderTargetHeight == (int)gfxRenderTargets[newTargetId].height);
    assert((context.source->renderTargetWidth > 0));
    assert(context.source->renderTargetHeight > 0);

    R_HW_SetRenderTarget(context.state, newTargetId);

    context.state->renderTargetId = newTargetId;
    context.source->viewMode = VIEW_MODE_NONE;
    context.source->viewportIsDirty = true;
  }
}