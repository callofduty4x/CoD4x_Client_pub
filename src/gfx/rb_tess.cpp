#include "r_local.h"
#include "r_material.h"
#include "rb_backend.h"



void R_SetObjectIdentityPlacement(GfxCmdBufSourceState *source)
{
  if ( source->objectPlacement != &rg.remotePlacement )
  {
    source->objectPlacement = &rg.remotePlacement;
    R_ChangeObjectPlacementRemote(source, &rg.remotePlacement);
  }
}

void R_SetVertexDeclTypeNormal(GfxCmdBufState *state, MaterialVertexDeclType vertDeclType)
{
  state->prim.vertDeclType = vertDeclType;
  R_UpdateVertexDecl(state);
}


void R_SetVertexDeclTypeWorld(GfxCmdBufState *state)
{
  if ( state->technique->flags & 8 )
  {
    R_SetVertexDeclTypeNormal(state, (MaterialVertexDeclType)((uint8_t)state->material->techniqueSet->worldVertFormat + VERTDECL_WORLD));
  }
  else
  {
    R_SetVertexDeclTypeNormal(state, VERTDECL_WORLD);
  }
}

unsigned int R_TessTrianglesPreTessList(const GfxDrawSurfListArgs *listArgs, GfxCmdBufContext prepassContext)
{
  D3DIndexBuffer *ib;
  GfxDepthRangeType depthRangeType;
  GfxCmdBufContext context;
  GfxDrawSurfListInfo *info;
  GfxBackEndData *data;
  GfxCmdBufSourceState *commonSource;
  char baseTechType;
  const unsigned int *primDrawSurfPos;

  context = listArgs->context;
  commonSource = listArgs->context.source;


  //assert(prepassContext.state == NULL);

  info = listArgs->info;
//  R_SetTerrainScorchTextures(listArgs->context.source, listArgs->context.state->material);
  R_SetupPassCriticalPixelShaderArgs(context); 
  baseTechType = info->baseTechType;
  R_SetObjectIdentityPlacement(commonSource);
  R_ChangeDepthHackNearClip(commonSource, 0);
  R_SetVertexDeclTypeWorld(context.state);

  if(commonSource->cameraView != 0)
  {
    depthRangeType = GFX_DEPTH_RANGE_SCENE;
  }else{
    depthRangeType = GFX_DEPTH_RANGE_FULL;
  }

  if ( depthRangeType != context.state->depthRangeType )
  {
    R_ChangeDepthRange(context.state, depthRangeType);
  }
  data = commonSource->input.data;
  ib = data->preTessIb;
  if ( context.state->prim.indexBuffer != ib )
  {
    R_ChangeIndices(&context.state->prim, ib);
  }
  uint16_t objectId = info->drawSurfs[listArgs->firstDrawSurfIndex].fields.objectId;
  primDrawSurfPos = &data->primDrawSurfsBuf[objectId];

//  R_TrackPrims(&context.state->prim, 0);
  if ( baseTechType == TECHNIQUE_LIT )
  {
    R_DrawBspDrawSurfsLitPreTess(primDrawSurfPos, context);
  }
  else
  {
    R_DrawBspDrawSurfsPreTess(primDrawSurfPos, context);
  }
//  context.state->prim.primStats = 0;
  return 1;
}