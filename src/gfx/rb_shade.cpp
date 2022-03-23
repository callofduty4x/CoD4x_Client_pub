#include "r_local.h"
#include "rb_backend.h"
#include "r_material.h"


void R_HW_SetPixelShader(D3DDevice *device, const MaterialPixelShader *mtlShader)
{
  assert(device);

  D3DCALL(device->SetPixelShader( mtlShader ? mtlShader->prog.ps : NULL ));

}

