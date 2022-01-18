#include "r_local.h"

void R_HW_SetPixelShaderConstant(D3DDevice *device, unsigned int dest, const float *data, unsigned int rowCount)
{
  D3DCALL(device->SetPixelShaderConstantF(dest, data, rowCount));
}

void R_HW_SetVertexShaderConstant(D3DDevice *device, unsigned int dest, const float *data, unsigned int rowCount)
{
  D3DCALL(device->SetVertexShaderConstantF(dest, data, rowCount));
}

