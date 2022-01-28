#include "r_local.h"
#include "rb_backend.h"
#include "r_debug.h"

//void R_InitDebugSystems();


extern "C" void R_DrawDecalCallback(const void *userData, GfxCmdBufContext context, GfxCmdBufContext prepassContext)
{
  GfxViewInfo *data = (GfxViewInfo*)userData;

  R_SetRenderTarget(context, R_RENDERTARGET_SCENE);
  if ( prepassContext.state )
  {
    R_SetRenderTarget(prepassContext, R_RENDERTARGET_SCENE);
  }

  R_HW_EnableScissor(context.state->prim.device, data->scissorViewport.x, data->scissorViewport.y, data->scissorViewport.width, data->scissorViewport.height);
  R_DrawSurfs(prepassContext.state, &data->decalInfo, context);
  R_HW_DisableScissor(context.state->prim.device);

/*
  static bool created;
  if(created == false)
  {
    R_InitDebugSystems();
    created = true;
  }
*/

  dbgCon->Print("\n-------- R_DrawDecalCallback ---------\n");

  //for(int i = 0; i < data->decalInfo.drawSurfCount; ++i)
  {
    //dbgCon->Print("Surf material {} \n", (int)(data->decalInfo.drawSurfs[i].fields.materialSortedIndex));
  }

}
