#include "r_local.h"
#include "rb_backend.h"


void R_HW_DisableScissor(D3DDevice *device)
{
    D3DCALL(device->SetRenderState( D3DRS_SCISSORTESTENABLE, 0 ));
}


void R_HW_EnableScissor(D3DDevice *device, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
  RECT scissor;

  scissor.left = x;
  scissor.top = y;
  scissor.right = w + x;
  scissor.bottom = h + y;


  D3DCALL(device->SetRenderState(D3DRS_SCISSORTESTENABLE, 1));
  D3DCALL(device->SetScissorRect( &scissor ));

}

