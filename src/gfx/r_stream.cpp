#include "r_local.h" 

void R_SetStreamSource(GfxCmdBufPrimState *primState, D3DVertexBuffer *vb, unsigned int vertexOffset, unsigned int vertexStride)
{

  if ( primState->streams[0].vb != vb || primState->streams[0].offset != vertexOffset || primState->streams[0].stride != vertexStride )
  {
    R_ChangeStreamSource(primState, 0, vb, vertexOffset, vertexStride);
  }

  if ( primState->streams[1].vb || primState->streams[1].offset || primState->streams[1].stride )
  {
    R_ChangeStreamSource(primState, 1, 0, 0, 0);
  }

#ifdef BLACKOPS

  if ( primState->streams[2].vb || primState->streams[2].offset || primState->streams[2].stride )
  {
    R_ChangeStreamSource(primState, 2, 0, 0, 0);
  }
#endif
}

void R_SetDoubleStreamSource(GfxCmdBufPrimState *primState, D3DVertexBuffer *vb0, unsigned int vertexOffset0, unsigned int vertexStride0, D3DVertexBuffer *vb1, unsigned int vertexOffset1, unsigned int vertexStride1)
{
  if ( primState->streams[0].vb != vb0 || primState->streams[0].offset != vertexOffset0 || primState->streams[0].stride != vertexStride0 )
  {
    R_ChangeStreamSource(primState, 0, vb0, vertexOffset0, vertexStride0);
  }
  if ( primState->streams[1].vb != vb1 || primState->streams[1].offset != vertexOffset1 || primState->streams[1].stride != vertexStride1 )
  {
    R_ChangeStreamSource(primState, 1, vb1, vertexOffset1, vertexStride1);
  }
#ifdef BLACKOPS
  if ( primState->streams[2].vb || primState->streams[2].offset || primState->streams[2].stride )
  {
    R_ChangeStreamSource(primState, 2, 0, 0, 0);
  }
#endif
}