#include "r_local.h"
#include "r_draw_cmdbuf.h"
#include "gfx_world.h"

int g_layerDataStride[] = { 0, 0, 0, 8, 12, 16, 20, 24, 24, 28, 32, 32, 36, 40, 0, 0 };


bool R_ReadBspPreTessDrawSurfs(GfxReadCmdBuf *cmdBuf, GfxBspPreTessDrawSurf **list, unsigned int *count, unsigned int *baseIndex)
{
    *count = R_ReadPrimDrawSurfInt(cmdBuf);
    if ( !*count )
    {
        return false;
    }
    *baseIndex = R_ReadPrimDrawSurfInt(cmdBuf);
    *list = (GfxBspPreTessDrawSurf *)R_ReadPrimDrawSurfData(cmdBuf, *count);
    return true;
}


void R_SetStreamsForBspSurface(GfxCmdBufPrimState *state, const srfTriangles_t *tris)
{
  int layerDataStride;
  GfxWorld *world = rgp.world;


  layerDataStride = g_layerDataStride[state->vertDeclType];
  if ( layerDataStride )
  {
    R_SetDoubleStreamSource(state, world->vd.worldVb, 44 * tris->firstVertex, 44, world->vld.layerVb, tris->vertexLayerData, layerDataStride);
    return;
  }
  R_SetStreamSource(state, world->vd.worldVb, 44 * tris->firstVertex, 44);
}

void R_DrawPreTessTris(GfxCmdBufPrimState *state, srfTriangles_t *tris, unsigned int baseIndex, unsigned int triCount)
{
    GfxDrawPrimArgs args;

    R_SetStreamsForBspSurface(state, tris);
    args.vertexCount = tris->vertexCount;
    args.triCount = triCount;
    args.baseIndex = baseIndex;
    R_DrawIndexedPrimitive(state, &args);
    
    /*
    state->frameStats.geoIndexCount += 3 * triCount;
    assert(state->primStats);
    state->primStats->dynamicIndexCount += 3 * triCount;
    */
}


void R_DrawBspDrawSurfsLitPreTess(const unsigned int *primDrawSurfPos, GfxCmdBufContext context)
{
    unsigned int baseIndex;
    unsigned int surfIndex;
    GfxReadCmdBuf cmdBuf;
    srfTriangles_t *tris;
    GfxBspPreTessDrawSurf *list;
    unsigned int reflectionProbeIndex;
    GfxSurface *bspSurf;
    unsigned int index;
    unsigned int lightmapIndex;
    unsigned int count;

    //if ( sc_enable->boolean )
    {
      //context.source->input.codeImages[16] = gfxRenderTargets[R_RENDERTARGET_DYNAMICSHADOWS].image;
    }
    //else
    {
      context.source->input.codeImages[16] = rgp.whiteImage;
    }

    cmdBuf.primDrawSurfPos = primDrawSurfPos;
    while ( R_ReadBspPreTessDrawSurfs(&cmdBuf, &list, &count, &baseIndex) )
    {
        reflectionProbeIndex = 255;
        lightmapIndex = 31;
        for ( index = 0; index < count; ++index )
        {
            surfIndex = list[index].baseSurfIndex;

            bspSurf = &rgp.world->dpvs.surfaces[surfIndex];
            tris = &bspSurf->tris;
            
            if ( reflectionProbeIndex != bspSurf->reflectionProbeIndex || lightmapIndex != bspSurf->lightmapIndex )
            {
                reflectionProbeIndex = bspSurf->reflectionProbeIndex;
                lightmapIndex = bspSurf->lightmapIndex;
                R_SetReflectionProbe(context, reflectionProbeIndex);
                R_SetLightmap(context, lightmapIndex);
                R_SetupPassPerObjectArgs(context);
                R_SetupPassPerPrimArgs(context);
            }
            
            R_DrawPreTessTris(&context.state->prim, tris, baseIndex, list[index].totalTriCount);
            baseIndex += 3 * list[index].totalTriCount;
        }
    }
}


void R_DrawBspDrawSurfsPreTess(const unsigned int *primDrawSurfPos, GfxCmdBufContext context)
{
    unsigned int baseIndex;
    unsigned int surfIndex;
    GfxReadCmdBuf cmdBuf;
    srfTriangles_t *tris;
    srfTriangles_t *prevTris;
    GfxBspPreTessDrawSurf *list;
    unsigned int triCount;
    GfxSurface *bspSurf;
    unsigned int index;
    unsigned int count;
    int baseVertex;

    R_SetupPassPerObjectArgs(context);
    R_SetupPassPerPrimArgs(context);
    cmdBuf.primDrawSurfPos = primDrawSurfPos;
    while ( R_ReadBspPreTessDrawSurfs(&cmdBuf, &list, &count, &baseIndex) )
    {
        prevTris = 0;
        triCount = 0;
        baseVertex = -1;
        for ( index = 0; index < count; ++index )
        {
            surfIndex = list[index].baseSurfIndex;
            assert(surfIndex < rgp.world->surfaceCount);

            bspSurf = &rgp.world->dpvs.surfaces[surfIndex];
            tris = &bspSurf->tris;
            if ( baseVertex != bspSurf->tris.firstVertex )
            {
                if ( triCount )
                {
                    R_DrawPreTessTris(&context.state->prim, prevTris, baseIndex, triCount);
                    baseIndex += 3 * triCount;
                    triCount = 0;
                }
                prevTris = tris;
                baseVertex = tris->firstVertex;
            }
            triCount += list[index].totalTriCount;
        }
        R_DrawPreTessTris(&context.state->prim, prevTris, baseIndex, triCount);
    }
}

