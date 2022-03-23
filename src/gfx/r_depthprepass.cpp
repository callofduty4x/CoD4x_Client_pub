#include "r_local.h"
#include "rb_backend.h"
#include "r_material.h"
#include "r_state.h"
#include "r_debug.h"


/*
unsigned int R_RenderDrawSurfListMaterial_debug(GfxDrawSurfListArgs *listArgs, GfxCmdBufContext prepassContext)
{
    unsigned int firstDrawSurfIndex;
    unsigned int passIndex;
    GfxCmdBufContext passPrepassContext;

    GfxDrawSurf drawSurf;
    unsigned int subListCount;
    uint16_t passCount;
    GfxDrawSurf *drawSurfList;
    bool isPixelCostEnabled;
    unsigned int drawSurfCount;

    firstDrawSurfIndex = listArgs->firstDrawSurfIndex;
    drawSurfCount = listArgs->info->drawSurfCount - listArgs->firstDrawSurfIndex;
    drawSurfList = &listArgs->info->drawSurfs[firstDrawSurfIndex];
    assert(*((unsigned int*)drawSurfList) != 0xb0b0b0b0);

    drawSurf = *drawSurfList;


    if ( !R_SetTechnique(listArgs->context, &prepassContext, listArgs->info, drawSurf) )
    {
      return R_SkipDrawSurfListMaterial(drawSurfList, drawSurfCount);
    }

    isPixelCostEnabled = pixelCostMode != 0;
    if ( isPixelCostEnabled )
    {
        R_PixelCost_BeginSurface(listArgs->context);
    }
    assert(!prepassContext.state || (prepassContext.state->technique->passCount == 1));

    passPrepassContext.source = prepassContext.source;
    subListCount = 0;
    passCount = listArgs->context.state->technique->passCount;
    for ( passIndex = 0; passIndex < passCount; ++passIndex )
    {
        R_UpdateMaterialTime(listArgs->context.source, 0.0);
        R_SetupPass(listArgs->context, passIndex);
        if ( passIndex || !prepassContext.state )
        {
            passPrepassContext.state = NULL;
        }
        else
        {
            R_SetupPass(prepassContext, 0);
            passPrepassContext.state = prepassContext.state;
        }
        assert(drawSurf.fields.surfType < ARRAY_COUNT( rb_tessTable ));
        subListCount = rb_tessTable[drawSurf.fields.surfType](listArgs, passPrepassContext);
    }
    if ( isPixelCostEnabled )
    {
        R_PixelCost_EndSurface(listArgs->context);
    }

    return subListCount;
}

*/


extern "C" unsigned int R_RenderDrawSurfListMaterial(GfxDrawSurfListArgs *listArgs, GfxCmdBufContext prepassContext);

void R_DrawSurfs_debug(GfxCmdBufContext context, GfxCmdBufState *prepassState, GfxDrawSurfListInfo *info)
{
    GfxViewport viewport;
    GfxCmdBufContext prepassContext;
    GfxDrawSurfListArgs listArgs;
    unsigned int drawMatCount;
    unsigned int processedDrawSurfCount;
    unsigned int drawSurfCount;

    assert(context.source->cameraView == info->cameraView);

    context.state->origMaterial = 0;
    //R_SetDrawSurfsShadowableLight(context.source, info);
    if(context.source->viewMode != VIEW_MODE_3D)
    {
        return;
    }
    //R_Set3D(context.source);
    prepassContext.source = prepassState != 0 ? context.source : 0;
    prepassContext.state = prepassState;
    if ( context.source->viewportIsDirty )
    {
        dbgCon->Print("Viewport behaviour: {}\n", context.source->viewportBehavior);

        R_GetViewport(context.source, &viewport);

        dbgCon->Print("h: {}, w: {} x: {} y: {}\n", viewport.height, viewport.width, viewport.x, viewport.y);


        R_SetViewport(context.state, &viewport);
        if ( prepassState )
        {
            R_SetViewport(prepassContext.state, &viewport);
        }
        R_UpdateViewport(context.source, &viewport);
    }
    assert(r_dx.d3d9 && r_dx.device);

    if ( context.state->prim.device != r_dx.device )
    {
//        PIXBeginNamedEvent(-1, "draw surf");
    }
    /*
    R_BeginPixMaterials(context.state);
    if ( prepassContext.state )
    {
        R_BeginPixMaterials(prepassContext.state);
    }*/

    drawSurfCount = info->drawSurfCount;
 
    listArgs.context = context;
    listArgs.firstDrawSurfIndex = 0;
    listArgs.info = info;
    drawMatCount = 0;

    while ( listArgs.firstDrawSurfIndex != drawSurfCount )
    {
        processedDrawSurfCount = R_RenderDrawSurfListMaterial(&listArgs, prepassContext);
        listArgs.firstDrawSurfIndex += processedDrawSurfCount;
        ++drawMatCount;
    }


    /*
    context.state->prim.viewStats->drawSurfCount += drawSurfCount;
    context.state->prim.viewStats->drawMatCount += drawMatCount;
    R_EndPixMaterials(context.state);
    if ( prepassContext.state )
    {
        R_EndPixMaterials(prepassContext.state);
    }
    if ( context.state->prim.device != r_dx.device && GetCurrentThreadId() == g_DXDeviceThread.owner && 0 == g_DXDeviceThread.aquired )
    {
        D3DPERF_EndEvent();
    }
    */
//    R_TessEnd(context, prepassContext);
    context.state->origMaterial = 0;
}



extern "C" void R_DepthPrepassCallback(const void *userData, GfxCmdBufContext context, GfxCmdBufContext prepassContext)
{
    GfxDrawSurfListInfo info;
    MaterialTechniqueType baseTechType;
    GfxViewInfo *viewInfo;
/*
    v3 = 3;
    for ( i = info.group; --v3 >= 0; ++i )
    {
        i->QueuedBatchCount = 0;
        i->ExecutingBatchCount = 0;
    }
*/
    viewInfo = (GfxViewInfo *)userData;

//    if ( !(userData->sceneComposition.renderingMode & 7) )
    {
//        R_HW_EnableScissor(context.state->prim.device, viewInfo->scissorViewport.x, viewInfo->scissorViewport.y, viewInfo->scissorViewport.width, viewInfo->scissorViewport.height);
    }

    if ( viewInfo->needsFloatZ )
    {
//        R_SetRenderTarget(context, R_RENDERTARGET_FLOAT_Z);
        
        baseTechType = TECHNIQUE_BUILD_FLOAT_Z;
        //R_DrawQuadMesh(context, rgp.shadowClearMaterial, &viewInfo->fullSceneViewMesh->meshData);

//        R_SetCodeConstantFromVec4(context.source, CONST_SRC_CODE_DEPTH_FROM_CLIP, (const float[]){0.0, 0.0, 0.0, 1.0});
    }
    else
    {
        
//        R_SetRenderTarget(context, R_RENDERTARGET_DYNAMICSHADOWS);
        baseTechType = TECHNIQUE_DEPTH_PREPASS;
    }

//    R_SetWindShaderConstants(context.source);
    
    memcpy(&info, &viewInfo->litInfo, sizeof(info));
    info.baseTechType = baseTechType;

//    PIXBeginNamedEvent(-1, "zprepass litInfo");

    R_DrawSurfs_debug(context, 0, &info);

/*
    if ( GetCurrentThreadId() == g_DXDeviceThread.owner && 0 == g_DXDeviceThread.aquired )
    {
        D3DPERF_EndEvent();
    }
*/
/*
    memcpy(&info, &viewInfo->decalInfo, sizeof(info));
    info.baseTechType = baseTechType;

    PIXBeginNamedEvent(-1, "zprepass decalInfo");

    R_DrawSurfs(context, 0, &info);
*/
/*
    if ( GetCurrentThreadId() == g_DXDeviceThread.owner && 0 == g_DXDeviceThread.aquired )
    {
        D3DPERF_EndEvent();
    }

    if ( !(viewInfo->sceneComposition.renderingMode & 7) )
*/
    {
//        R_HW_DisableScissor(context.state->prim.device);
    }
}