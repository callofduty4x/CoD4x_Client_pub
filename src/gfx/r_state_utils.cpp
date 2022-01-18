#include "r_local.h"
#include "rb_backend.h"

#define s_decodeSamplerFilterState ((int*)(0xD584010))

void R_SetDefaultAlphaTestFunction(GfxCmdBufState *state)
{

  IDirect3DDevice9 *device;

  device = state->prim.device;
  assert(device);

  D3DCALL(device->SetRenderState( D3DRS_ALPHAREF, 0 ));

  state->alphaRef = 0;
}


void R_SetDefaultStateBits(unsigned int *stateBits)
{
  stateBits[0] = 0;
  stateBits[1] = 0;
  stateBits[0] |= 0x800u;
  stateBits[0] |= 0x18000000u;
  stateBits[0] |= 0x4000u;
  stateBits[1] |= 2u;
}


unsigned int R_DecodeSamplerState(char samplerState)
{
  unsigned int tableIndex;

  tableIndex = samplerState & 0x1F;

  assert(tableIndex < 24);

  return s_decodeSamplerFilterState[tableIndex] | 0x1500000 | ((samplerState & 0x20) << 16) | ((samplerState & 0x40) << 17) | ((uint8_t)(samplerState & 0x80) << 18);
}

/*
void R_HW_ForceSamplerState(D3DDevice *device, unsigned int samplerIndex, unsigned int samplerState)
{
    unsigned int minFilter = (samplerState & 0xF00) >> 8;
    unsigned int magFilter = (samplerState & 0xF000) >> 12;
    signed int anisotropy = (unsigned)(samplerState & 0xFF);
    unsigned int mipFilter = (samplerState & 0xF0000) >> 16;

    D3DCALL(device->SetSamplerState( samplerIndex, D3DSAMP_MINFILTER, minFilter ));
    D3DCALL(device->SetSamplerState( samplerIndex, D3DSAMP_MAGFILTER, magFilter ));

    if ( anisotropy > 1 )
    {
        D3DCALL(device->SetSamplerState( samplerIndex, D3DSAMP_MAXANISOTROPY, anisotropy ));
    }

    D3DCALL(device->SetSamplerState( samplerIndex, D3DSAMP_MIPFILTER, mipFilter ));

    D3DCALL(device->SetSamplerState( samplerIndex, D3DSAMP_MIPMAPLODBIAS, r_dx.mipBias));


    unsigned int address = (samplerState & 0x300000) >> 20;

    D3DCALL(device->SetSamplerState( samplerIndex, D3DSAMP_ADDRESSU, address ));
    
    
    address = (samplerState & 0xC00000) >> 22;
    
    D3DCALL(device->SetSamplerState( samplerIndex, D3DSAMP_ADDRESSV, address ));

    address = (samplerState & 0x3000000) >> 24;

    D3DCALL(device->SetSamplerState( samplerIndex, D3DSAMP_ADDRESSW, address ));

}
*/

void R_WorldMatrixChanged(GfxCmdBufSourceState *source)
{
    ++source->matrixVersions[0];
    ++source->matrixVersions[3];
    ++source->matrixVersions[5];
    ++source->matrixVersions[7];
    source->constVersions[CONST_SRC_FIRST_CODE_MATRIX] = source->matrixVersions[0];
}

GfxCmdBufSourceState * R_GetActiveWorldMatrix(GfxCmdBufSourceState *source)
{
    R_WorldMatrixChanged(source);
    return source;
}