#include <stdint.h>
struct GfxReadCmdBuf;

struct GfxBspPreTessDrawSurf
{
  uint16_t baseSurfIndex;
  uint16_t totalTriCount;
};


struct GfxReadCmdBuf
{
  const unsigned int *primDrawSurfPos;
};


unsigned int R_ReadPrimDrawSurfInt(GfxReadCmdBuf *cmdBuf);
const unsigned int * R_ReadPrimDrawSurfData(GfxReadCmdBuf *cmdBuf, unsigned int count);