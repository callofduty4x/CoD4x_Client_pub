#include "r_draw_cmdbuf.h"

unsigned int R_ReadPrimDrawSurfInt(GfxReadCmdBuf *cmdBuf)
{
    unsigned int value;

    value = *cmdBuf->primDrawSurfPos;
    ++cmdBuf->primDrawSurfPos;
    return value;
}

const unsigned int * R_ReadPrimDrawSurfData(GfxReadCmdBuf *cmdBuf, unsigned int count)
{
    const unsigned int *data;

    data = cmdBuf->primDrawSurfPos;
    cmdBuf->primDrawSurfPos += count;
    return data;
}