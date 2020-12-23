#if 0
//Does not work with multi monitor setup. I don't wanna bother to fix it right now!

#include "q_shared.h"
#include "qcommon.h"
#include "r_shared.h"
#include "xassets/gfxworld.h"
#include "xzone.h"

const char *__cdecl R_ErrorDescription(HRESULT hr);

signed int __cdecl R_GetDeviceType()
{
  D3DADAPTER_IDENTIFIER9 id; 
  unsigned int Adapter;

  r_dx.adapterIndex = 0;

  for ( Adapter = 0; ; ++Adapter )
  {
    if ( Adapter >= r_dx.d3d9->lpVtbl->GetAdapterCount(r_dx.d3d9) )
    {
      break;
    }
    if ( r_dx.d3d9->lpVtbl->GetAdapterIdentifier(r_dx.d3d9, Adapter, 0, &id) >= 0 && strstr(id.Description, "PerfHUD") )
    {
      r_dx.adapterIndex = Adapter;
      return 2;
    }
  }
  return 1;
}

HRESULT __cdecl R_CreateDeviceInternal(HWND hwnd, unsigned int behavior, D3DPRESENT_PARAMETERS *d3dpp)
{
  D3DDEVTYPE devtype;
  int x, y;
  HRESULT getModeSuccessCode;
  D3DDISPLAYMODE getModeResult;
  HRESULT hr;
  int attempt, attemptrecover;
#ifdef BLACKOPS
  NvAPI_Status status;
#endif
  Com_Printf(CON_CHANNEL_GFX, "Creating Direct3D device...\n");
  attempt = attemptrecover = 0;
  do
  {
    r_dx.adapterNativeIsValid = R_GetMonitorDimensions(&r_dx.adapterNativeWidth, &r_dx.adapterNativeHeight, &x, &y);
    d3dpp->hDeviceWindow = hwnd;
    devtype = R_GetDeviceType();
    hr = r_dx.d3d9->lpVtbl->CreateDevice(r_dx.d3d9, r_dx.adapterIndex, devtype, hwnd, behavior, d3dpp, &r_dx.device);
    if ( hr >= 0 )
    {
#ifdef BLACKOPS
      r_dx.nvStereoActivated = 0;
      r_dx.nvStereoHandle = 0;
      status = NvAPI_Stereo_CreateHandleFromIUnknown(r_dx.device, &r_dx.nvStereoHandle);
      if ( status == NVAPI_OK )
      {
        NvAPI_Stereo_IsActivated(r_dx.nvStereoHandle, &res);
        r_dx.nvStereoActivated = res != 0;
      }
#endif
      getModeSuccessCode = r_dx.d3d9->lpVtbl->GetAdapterDisplayMode(r_dx.d3d9, r_dx.adapterIndex, &getModeResult);
      if ( getModeSuccessCode < 0 )
      {
        r_dx.adapterFullscreenWidth = d3dpp->BackBufferWidth;
        r_dx.adapterFullscreenHeight = d3dpp->BackBufferHeight;
      }
      else
      {
        r_dx.adapterFullscreenWidth = getModeResult.Width;
        r_dx.adapterFullscreenHeight = getModeResult.Height;
      }
      return hr;
    }
    Sleep(0x64u);
    if ( hr != 0x88760868 )
    {
      ++attempt;
    }
    ++attemptrecover;
  }
  while ( attempt < 20 && attemptrecover < 80);
  if ( !r_dx.adapterIndex )
  {
    return hr;
  }
  r_dx.adapterIndex = 0;
  return R_CreateDeviceInternal(hwnd, behavior, d3dpp);
}


void __cdecl R_SetupAntiAliasing(struct GfxWindowParms *wndParms)
{
  D3DMULTISAMPLE_TYPE multiSampleCount;
  DWORD qualityLevels;

  assert(wndParms);
  assert(wndParms->aaSamples >= 1 && wndParms->aaSamples <= 16);

  if ( r_reflectionProbeGenerate->boolean )
  {
    multiSampleCount = 1;
  }
  else
  {
    multiSampleCount = wndParms->aaSamples;
  }
  while ( (signed int)multiSampleCount > 1 )
  {
    r_dx.multiSampleType = multiSampleCount;
    if(r_dx.d3d9->lpVtbl->CheckDeviceMultiSampleType(r_dx.d3d9, 0, D3DDEVTYPE_HAL, 21, wndParms->fullscreen == 0, multiSampleCount, &qualityLevels) >= 0 )
    {
      Com_Printf(CON_CHANNEL_GFX, "Using %ix anti-aliasing\n", multiSampleCount);
      r_dx.multiSampleQuality = 0;
      return;
    }
    --multiSampleCount;
  }
  r_dx.multiSampleType = 0;
  r_dx.multiSampleQuality = 0;
}

void __cdecl R_SetD3DPresentParameters(D3DPRESENT_PARAMETERS *d3dpp, struct GfxWindowParms *wndParms)
{
  assert(d3dpp);
  assert(wndParms);
  R_SetupAntiAliasing(wndParms);
  memset((char *)d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
  d3dpp->BackBufferHeight = wndParms->displayHeight;
  d3dpp->BackBufferWidth = wndParms->displayWidth;
  d3dpp->BackBufferFormat = 21;
  d3dpp->BackBufferCount = 1;
  d3dpp->MultiSampleType = r_dx.multiSampleType;
  d3dpp->MultiSampleQuality = r_dx.multiSampleQuality;
  d3dpp->SwapEffect = 1;
  d3dpp->EnableAutoDepthStencil = 0;
  d3dpp->AutoDepthStencilFormat = r_dx.depthStencilFormat;
  d3dpp->PresentationInterval = r_vsync->boolean != 0 ? 1 : 0x80000000;
  
  assert(wndParms->hwnd);

  d3dpp->hDeviceWindow = wndParms->hwnd;
  d3dpp->Flags = 0;
  if ( wndParms->fullscreen )
  {
    d3dpp->Windowed = 0;
    d3dpp->FullScreen_RefreshRateInHz = wndParms->hz;
  }
  else
  {
    d3dpp->Windowed = 1;
    d3dpp->FullScreen_RefreshRateInHz = 0;
  }
}


char __cdecl R_IsDepthStencilFormatOk(D3DFORMAT renderTargetFormat, D3DFORMAT depthStencilFormat)
{

  if ( r_dx.d3d9->lpVtbl->CheckDeviceFormat(r_dx.d3d9, r_dx.adapterIndex, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 2, D3DRTYPE_SURFACE, depthStencilFormat) < 0 )
  {
    return 0;
  }
  if ( r_dx.d3d9->lpVtbl->CheckDepthStencilMatch(r_dx.d3d9, r_dx.adapterIndex, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, renderTargetFormat, depthStencilFormat) >= 0 )
  {
    return 1;
  }
  return 0;
}

D3DFORMAT __cdecl R_GetDepthStencilFormat(D3DFORMAT renderTargetFormat)
{
  if ( R_IsDepthStencilFormatOk(renderTargetFormat, D3DFMT_D24FS8) )
  {
    return 83;
  }
  return 75;

}


qboolean __cdecl REGPARM(1) R_CreateDevice(struct GfxWindowParms *wndParms)
{
  D3DPRESENT_PARAMETERS d3dpp;
  HWND hwnd;
  HRESULT hr;
  unsigned int behavior; 

  hwnd = 0;
  assert(wndParms);
  //assert(r_dx.windowCount == 0); //assert(r_dx.windows[0].hwnd)
  assert(wndParms->hwnd);

  hwnd = wndParms->hwnd;
  assert(r_dx.device == NULL);
  r_dx.depthStencilFormat = R_GetDepthStencilFormat(D3DFMT_A8R8G8B8);
  R_SetD3DPresentParameters(&d3dpp, wndParms);
#ifdef BLACKOPS  
  behavior = 64;
  if ( r_multithreaded_device->boolean )
  {
    behavior |= 4u;
  }
#else
    behavior = 70;
#endif

  hr = R_CreateDeviceInternal(hwnd, behavior, &d3dpp);

#ifndef BLACKOPS    
  r_glob.haveThreadOwnership = 1;
#endif

  if ( hr >= 0 )
  {
    assert(r_dx.device);
    r_dx.inScene = 0;
    return 1;
  }
  Com_Printf(CON_CHANNEL_GFX, "Couldn't create a Direct3D device: %s\n", R_ErrorDescription(hr));
  return 0;
}

#endif