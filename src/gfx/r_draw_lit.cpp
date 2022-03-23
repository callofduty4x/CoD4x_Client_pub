#include "r_local.h"
#include "rb_backend.h"
#include "r_debug.h"
#include "r_material.h"


template<> struct fmt::formatter<GfxDrawSurfListInfo>
{
  template<typename ParseContext> constexpr auto parse(ParseContext& ctx);

  template<typename FormatContext> auto format(GfxDrawSurfListInfo const& type, FormatContext& ctx);
};

template<typename ParseContext> constexpr auto fmt::formatter<GfxDrawSurfListInfo>::parse(ParseContext& ctx)
{
  return ctx.begin();
}

template<typename FormatContext> auto fmt::formatter<GfxDrawSurfListInfo>::format(GfxDrawSurfListInfo const& type, FormatContext& ctx)
{
  return fmt::format_to(ctx.out(), "drawSurfCount: {0} BaseTechType: {1} CameraView: {2} ViewOrigin: {3} {4} {5} {6}", type.drawSurfCount, type.baseTechType, type.cameraView, type.viewOrigin[0], type.viewOrigin[1], type.viewOrigin[2], type.viewOrigin[3]);
}


extern "C" void R_DrawDecalCallback(const void *userData, GfxCmdBufContext context, GfxCmdBufContext prepassContext)
{
  GfxViewInfo *data = (GfxViewInfo*)userData;

  bool portalmat = false;

  for(int i = 0; i < data->decalInfo.drawSurfCount; ++i)
  {
    int materialidx = data->decalInfo.drawSurfs[i].fields.materialSortedIndex;
    if(strcmp(rgp.sortedMaterials[materialidx]->info.name, "wc/hdrportal_lighten") == 0)
    {
      portalmat = true;
    }
  }
  if(data->decalInfo.drawSurfCount == 1 && portalmat)
  {

    _RB_LogPrintf("\n-------- R_DrawDecalCallback ---------\n");



    dbgCon->Print("\n-------- R_DrawDecalCallback ---------\n");
    dbgCon->Print("{}\n", data->decalInfo);

    for(int i = 0; i < data->decalInfo.drawSurfCount; ++i)
    {
      int materialidx = data->decalInfo.drawSurfs[i].fields.materialSortedIndex;
      dbgCon->Print("Surf material {} {}\n", materialidx, rgp.sortedMaterials[materialidx]->info.name);

    }
  }

  R_SetRenderTarget(context, R_RENDERTARGET_SCENE);
  if ( prepassContext.state )
  {
    R_SetRenderTarget(prepassContext, R_RENDERTARGET_SCENE);
  }

  R_HW_EnableScissor(context.state->prim.device, data->scissorViewport.x, data->scissorViewport.y, data->scissorViewport.width, data->scissorViewport.height);
  R_DrawSurfs(prepassContext.state, &data->decalInfo, context);
  R_HW_DisableScissor(context.state->prim.device);


  if(data->decalInfo.drawSurfCount == 1 && portalmat)
  {
    _RB_LogPrintf("\n-------- Finished R_DrawDecalCallback ---------\n");
  }

}
