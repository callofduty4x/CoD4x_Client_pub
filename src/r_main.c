#include "q_shared.h"
#include "qcommon.h"
#include "r_shared.h"
#include "xassets/gfxworld.h"
#include "xzone.h"

#include <Shellapi.h>


/* 1370 */
#pragma pack(push, 1)

typedef struct
{
  int origin[3];
  int reflectionImage;
  int field_10;
  int field_14;
  int field_18;
  int field_1C;
  int field_20;
  int field_24;
  int field_28;
  int field_2C;
  int field_30;
  int field_34;
  int field_38;
  int field_3C;
  int field_40;
  int field_44;
  int field_48;
  int field_4C;
  int field_50;
  int field_54;
  int field_58;
  int field_5C;
  int field_60;
  int field_64;
  int field_68;
  int field_6C;
  int field_70;
  int field_74;
  int field_78;
  int field_7C;
  int field_80;
  int field_84;
  int field_88;
  int field_8C;
  int field_90;
  int field_94;
  int field_98;
  int field_9C;
  int field_A0;
  int field_A4;
  int field_A8;
  int field_AC;
  int field_B0;
  int field_B4;
  int field_B8;
  int field_BC;
  int field_C0;
  int field_C4;
  int field_C8;
  int field_CC;
  int field_D0;
  int field_D4;
  int field_D8;
  int field_DC;
  int field_E0;
  int field_E4;
  int field_E8;
}GfxReflectionProbe_t;

typedef struct
{
  char field_0[8240];
  Material_t* field_2030;
  char field_2034[108];
  GfxReflectionProbe_t *reflectionProbes;
  char field_20A4[219];
  char field_217F;
}rgp_t;

#pragma pack(pop)

     
//#define r_displayRefresh getcvaradr(0xD56980C)
//#define r_mode getcvaradr(0xD569610)

cvar_t* r_displayRefresh;
cvar_t* r_mode;

void DB_ClearXAssetMem();
void DB_InitXAssetMem();


typedef struct
{
	DWORD monitorNumber;
	HMONITOR monitorHandle;
}monitorEnumParam_t;

BOOL CALLBACK R_MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, monitorEnumParam_t *monitorParam)
{
  BOOL result;

  if ( monitorParam->monitorNumber == 1 )
  {
    monitorParam->monitorHandle = hMonitor;
    result = 0;
  }
  else
  {
    result = 1;
  }
  monitorParam->monitorNumber--;
  return result;
}



HMONITOR R_GetActiveMonitor()
{
  qboolean fullscreen;
  HMONITOR hMonitor;
  POINT point;
  monitorEnumParam_t monitorParam;
  fullscreen = Cvar_GetBool("r_fullscreen");

  if (fullscreen)
  {
    monitorParam.monitorNumber = Cvar_GetInt("r_monitor");
    monitorParam.monitorHandle = 0;
	EnumDisplayMonitors(0, 0, (MONITORENUMPROC)R_MonitorEnumProc, (LPARAM)&monitorParam);
	hMonitor = monitorParam.monitorHandle;
  }

  if (!fullscreen  || hMonitor == 0)
  {
    point.x = Cvar_GetInt("vid_xpos");
    point.y = Cvar_GetInt("vid_ypos");
    hMonitor = MonitorFromPoint(point, 1u);
  }
  return hMonitor;
}



UINT __cdecl R_GetAdapterInfo()
{
  HMONITOR hMonitor;
  UINT adapterMon;
  UINT adapterCount;
  UINT i;

  hMonitor = R_GetActiveMonitor();
  adapterMon = 0;
  adapterCount = r_dx.d3d9->lpVtbl->GetAdapterCount(r_dx.d3d9);
  for(i = 0; i < adapterCount; ++i)
  {
	if ( hMonitor && r_dx.d3d9->lpVtbl->GetAdapterMonitor(r_dx.d3d9, i) == hMonitor )
	{
        adapterMon = i;
	}
  }
  return adapterMon;
}



void R_DestroyGameWindows()
{
  int i;

  for ( i = 0; i < r_dx.rWindowsState.numWindows; i++ )
  {
    if ( IsWindow(r_dx.rWindowsState.windows[i].windowHandle) )
	{
      DestroyWindow(r_dx.rWindowsState.windows[i].windowHandle);
    }
	r_dx.rWindowsState.windows[i].windowHandle = 0;
  }
  r_dx.rWindowsState.numWindows = 0;
}

void __cdecl R_ShutdownD3D()
{

  if ( com_useFastFiles->boolean )
  {
    DB_ShutdownXAssets();
  }
  DB_ClearXAssetMem();
  sub_60D7D0();
  sub_5F3FD0();

  R_DestroyGameWindows();

  if ( r_dx.device )
  {
	r_dx.device->lpVtbl->Release(r_dx.device);
    r_dx.device = 0;
  }
  if ( r_dx.d3d9 )
  {
    r_dx.d3d9->lpVtbl->Release(r_dx.d3d9);
    r_dx.d3d9 = 0;
  }
}



qboolean R_GetMonitorDimensions(int *width, int *heigth, int *x, int *y)
{
  struct HMONITOR__ *hMonitor;
  struct tagMONITORINFO mi;

  *x = 0;
  *y = 0;

  hMonitor = r_dx.d3d9->lpVtbl->GetAdapterMonitor(r_dx.d3d9, r_dx.adapterIndex);
  mi.cbSize = 40;
  if ( GetMonitorInfoA(hMonitor, &mi) )
  {
	*x = mi.rcMonitor.left;
	*y = mi.rcMonitor.top;
    *width = mi.rcMonitor.right - mi.rcMonitor.left;
    *heigth = mi.rcMonitor.bottom - mi.rcMonitor.top;
    return qtrue;
  }
  *width = GetSystemMetrics(0);
  *heigth = GetSystemMetrics(1);
  if ( *width > 0 && *heigth > 0 )
  {
    return qtrue;
  }
  return qfalse;
}


qboolean R_ParseCustomMode(GfxWindowParms_t *wndParms, qboolean gotDim)
{
	int numParams;


	numParams = sscanf(r_customMode->string, "%ix%i", &wndParms->displayWidth, &wndParms->displayHeight);

	if(numParams == 2 && (!gotDim || (wndParms->displayWidth <= wndParms->monitorWidth && wndParms->displayHeight <= wndParms->monitorHeight)) )
	{
		return qtrue;
	}
	return qfalse;
}

int sub_5F4A10(int width, int heigth, int hz)
{
  int i;
  int numModes;
  int index;
  unsigned int flag;

	i = 0;
	numModes = r_dx.numAdapterModes - 1;

    while ( numModes >= i )
    {
      index = (numModes + i) / 2;
      flag = r_dx.adapterModes[index].Width - width;
      if ( r_dx.adapterModes[index].Width == width )
      {
        flag = r_dx.adapterModes[index].Height - heigth;
        if ( r_dx.adapterModes[index].Height == heigth )
        {
          flag = r_dx.adapterModes[index].RefreshRate - hz;
          if ( r_dx.adapterModes[index].RefreshRate == hz )
		  {
            return hz;
		  }
        }
      }
      if ( (flag & 0x80000000u) == 0 )
        numModes = index - 1;
      else
        i = index + 1;
    }

    if ( r_dx.adapterModes[numModes].Width != width || r_dx.adapterModes[numModes].Height != heigth )
	{
		return r_dx.adapterModes[i].RefreshRate;
    }
	return r_dx.adapterModes[numModes].RefreshRate;
}



void R_SetWndParms(GfxWindowParms_t *wndParms)
{
  const char *mode;
  const char *refreshRate;
  int hz;
  qboolean gotDim;

  wndParms->fullscreen = Cvar_GetInt("r_fullscreen");

  gotDim = R_GetMonitorDimensions(&wndParms->monitorWidth, &wndParms->monitorHeight, &wndParms->monitorX, &wndParms->monitorY);

  if ( wndParms->fullscreen || !R_ParseCustomMode(wndParms, gotDim) )
  {
	mode = Cvar_ToString(r_mode);
    sscanf(mode, "%ix%i", &wndParms->displayWidth, &wndParms->displayHeight);
  }
  wndParms->sceneWidth = wndParms->displayWidth;
  wndParms->sceneHeight = wndParms->displayHeight;

  if ( wndParms->fullscreen == 0 )
  {
    wndParms->hz = 60;
  }
  else
  {
	refreshRate = Cvar_ToString(r_displayRefresh);
    sscanf(refreshRate, "%i Hz", &hz);
    wndParms->hz = sub_5F4A10(wndParms->displayWidth, wndParms->displayHeight, hz);
  }
  wndParms->hwnd = 0;
  wndParms->displayXPos = Cvar_GetInt("vid_xpos");
  wndParms->displayYPos = Cvar_GetInt("vid_ypos");
  wndParms->aaSamples = Cvar_GetInt("r_aaSamples");
}


qboolean R_CreateWindow(struct GfxWindowParms *windowInfo)
{
  int wsExStyle;
  int wsStyle;
  HWND hwnd;
  struct tagRECT Rect;

  if ( windowInfo->fullscreen )
  {
    Com_Printf(CON_CHANNEL_GFX, "Attempting %i x %i fullscreen with 32 bpp at %i hz\n", windowInfo->displayWidth, windowInfo->displayHeight, windowInfo->hz);
    wsExStyle = WS_EX_TOPMOST;
    wsStyle = WS_POPUP;


  }else{
	  if(windowInfo->displayWidth == windowInfo->monitorWidth && windowInfo->displayHeight == windowInfo->monitorHeight)
	  {
		wsExStyle = 0;
		wsStyle = WS_POPUP;

		windowInfo->displayXPos = windowInfo->monitorX;
		windowInfo->displayYPos = windowInfo->monitorY;
		Com_Printf(CON_CHANNEL_GFX, "Attempting %i x %i window without border at (%i, %i)\n", windowInfo->displayWidth, windowInfo->displayHeight, windowInfo->displayXPos, windowInfo->displayYPos);

	  }
	  else
	  {
		Com_Printf(CON_CHANNEL_GFX, "Attempting %i x %i window at (%i, %i)\n", windowInfo->displayWidth, windowInfo->displayHeight, windowInfo->displayXPos, windowInfo->displayYPos);
		wsExStyle = 0;
		wsStyle = WS_GROUP | WS_CAPTION | WS_SYSMENU;

	  }
  }
  Rect.left = 0;
  Rect.right = windowInfo->displayWidth;
  Rect.top = 0;
  Rect.bottom = windowInfo->displayHeight;
  AdjustWindowRectEx(&Rect, wsStyle, 0, wsExStyle);

  hwnd = CreateWindowExA(wsExStyle, WINDOW_CLASS_NAME, Com_GetBuildDisplayName(), wsStyle, windowInfo->displayXPos, windowInfo->displayYPos,
						Rect.right - Rect.left, Rect.bottom - Rect.top, 0, 0, g_wv.hInstance, 0);

  windowInfo->hwnd = hwnd;
  if ( hwnd )
  {
    Com_Printf(CON_CHANNEL_GFX, "Game window successfully created.\n");
    return 1;
  }
  Com_Printf(CON_CHANNEL_GFX, "Couldn't create a window.\n");
  return 0;
}


IDirect3D9* (WINAPI *Direct3DCreate9_ptr)(unsigned int sdkver);

void R_SetDirect3DCreate9Func(void* farproc)
{
	Direct3DCreate9_ptr = farproc;
}

void R_PreCreateWindow()
{
  if(!r_dx.d3d9)
  {
	Com_Printf(CON_CHANNEL_GFX, "Getting Direct3D 9 interface...\n");

	//Sys_CleanUpNvd3d9Wrap();

	if(Direct3DCreate9_ptr == NULL)
    {
      Direct3DCreate9_ptr = Direct3DCreate9;
    }
	r_dx.d3d9 = Direct3DCreate9_ptr(0x20u);
  }


  if ( r_dx.d3d9 )
  {
    r_dx.adapterIndex = R_GetAdapterInfo();
    sub_5F39C0(r_dx.adapterIndex);
	//sub_5F4140(r_dx.adapterIndex);
	R_SetupModeCvars(r_dx.adapterIndex);


  }
  else
  {
    Com_Printf(CON_CHANNEL_GFX, "Direct3D 9 failed to initialize\n");

  }
}



qboolean R_ReduceWindowSettings()
{
  cvar_t *var;
  int value;

  var = r_aaSamples;
  value = r_aaSamples->integer;
  if ( r_aaSamples->integer > 1 ||
		((var = r_displayRefresh, value = r_displayRefresh->integer, value > 0) && _vidConfig.displayFrequency > 60) ||
		((var = r_mode, value = r_mode->integer, value > 0) && (_vidConfig.displayWidth > 640 || _vidConfig.displayHeight > 480)) )
  {
    Cvar_SetInt(var, value - 1);
    return 1;
  }
  return 0;

}


char __cdecl R_CreateGameWindow(struct GfxWindowParms *wndParms)
{
  if ( !R_CreateWindow(wndParms) )
  {
    return 0;
  }

  if(wndParms->fullscreen)
  {
    Sys_DestroyConsole(); //no more vid_restart bugs. Shitty fix but shall work
  }

  if ( !R_InitHardware(wndParms) )
  {
    if(wndParms->hwnd)
    {
      DestroyWindow(wndParms->hwnd);
      wndParms->hwnd = NULL;
    }
    return 0;
  }
  r_dx.windowCount = 0;
  ShowWindow(wndParms->hwnd, SW_SHOW);
  Sys_DestroySplashWindow();
  return 1;
}

void R_InitGraphicsApi()
{

	Com_DPrintf(CON_CHANNEL_GFX, "R_InitGraphicsApi():\n");
	struct GfxWindowParms wind;

  assert((r_dx.device != NULL) == (r_dx.d3d9 != NULL));

	if(!com_dedicated || !com_dedicated->integer )
	{
		Cvar_SetBool(r_loadForRenderer, qtrue);
	}

	if ( r_dx.device )
	{
		//Com_Printf(CON_CHANNEL_GFX, "Direct3D reported %d megabytes of available texture memory.", r_dx.device->lpVtbl->GetAvailableTextureMem(r_dx.device));
		R_InitSystems();
		return;
	}

	R_PreCreateWindow();

	//Com_Printf(CON_CHANNEL_GFX, "Direct3D reported %d megabytes of available texture memory.", r_dx.device->lpVtbl->GetAvailableTextureMem(r_dx.device));

	//DB_InitXAssetMem();
	while ( 1 )
	{
    R_SetWndParms(&wind);

    if ( R_CreateGameWindow(&wind) )
    {
      break;
    }

	  if ( !R_ReduceWindowSettings() )
	  {
        R_FatalError("Couldn't initialize renderer");
	  }
  }

}

void R_GenerateReflections(const char *reflection, GfxReflectionProbe *, int count);

void R_BspGenerateReflections()
{
	R_GenerateReflections(rgp.world->name, rgp.world->reflectionProbes + 1, rgp.world->reflectionProbeCount - 1);
}

qboolean R_GfxWorldLoaded()
{
	if(rgp.world == NULL)
			return qfalse;
	return qtrue;
}
Material* R_GetDefaultShader()
{
	return rgp.defaultMaterial;
}

void R_DirectXError(char *errorstring)
{
  char errormsg[4096];
  errormsg[0] = '\0';
  R_DestroyGameWindows();
  Q_strcat(errormsg, sizeof(errormsg), "********** DirectX returned an unrecoverable error code during initialization  **********\n");
  Q_strcat(errormsg, sizeof(errormsg), "********** Initialization also happens while playing if DirectX loses a device **********\n");
  Q_strcat(errormsg, sizeof(errormsg), "********** Consult the readme for how to continue from this problem            **********\n");
  Q_strcat(errormsg, sizeof(errormsg), "\n");
  Q_strcat(errormsg, sizeof(errormsg), errorstring);
  Q_strcat(errormsg, sizeof(errormsg), "\n");
  Sys_EnterCriticalSection(11);
  MessageBoxA(GetActiveWindow(), errormsg, sub_576280("WIN_DIRECTX_INIT_TITLE"), 0x10u);
  ShellExecuteA(0, "open", "Docs\\TechHelp\\Tech Help\\Information\\DirectX.htm", 0, 0, 3);
  exit(-1);
}


int __cdecl R_SortAdapterModes(const void* cmp1, const void* cmp2)
{
  const D3DDISPLAYMODE *d3dmode1 = cmp1;
  const D3DDISPLAYMODE *d3dmode2 = cmp2;

  if ( d3dmode1->Width == d3dmode2->Width )
  {
	if ( d3dmode1->Height == d3dmode2->Height )
	{
      return d3dmode1->RefreshRate - d3dmode2->RefreshRate;
	}
    return d3dmode1->Height - d3dmode2->Height;
  }
  return d3dmode1->Width - d3dmode2->Width;
}

int __cdecl R_SortAdapterRefreshRates(const void* cmp1, const void* cmp2)
{
	const int* r1 = cmp1;
	const int* r2 = cmp2;
	return *r1 - *r2;
}

typedef struct{
	int width;
	int height;
}resolutionPair_t;



void R_SetupModeCvars(UINT adapterIndex)
{
	int numRefreshRates;
	int i, k;
	int modeCnt;
	char* stringBuf;
	resolutionPair_t sortedResolutions[256];
	int sortedRefreshRates[256];
	int refreshRateDefault;

	modeCnt = r_dx.d3d9->lpVtbl->GetAdapterModeCount(r_dx.d3d9, adapterIndex, D3DFMT_X8R8G8B8);

	for(i = 0, r_dx.numAdapterModes = 0; i < modeCnt && r_dx.numAdapterModes < MAX_ADAPTERMODES; ++i)
	{
      if ( r_dx.d3d9->lpVtbl->EnumAdapterModes(r_dx.d3d9, adapterIndex, D3DFMT_X8R8G8B8,
		i, &r_dx.adapterModes[r_dx.numAdapterModes]) >= 0 )
      {
        if ( !r_dx.adapterModes[r_dx.numAdapterModes].RefreshRate )
        {
          r_dx.adapterModes[r_dx.numAdapterModes].RefreshRate = 60;
        }
        ++r_dx.numAdapterModes;
      }
	}
	qsort(r_dx.adapterModes, r_dx.numAdapterModes, 0x10u, R_SortAdapterModes);

    modeCnt = 0;
	numRefreshRates = 0;
	for(i = 0; i < r_dx.numAdapterModes; ++i)
	{
		if(r_dx.adapterModes[i].Width < 640 || r_dx.adapterModes[i].Height < 480){
			continue;
		}


		if ( (modeCnt <= 0 || sortedResolutions[modeCnt].width != r_dx.adapterModes[i].Width || sortedResolutions[modeCnt].height != r_dx.adapterModes[i].Height)  )
		{
			sortedResolutions[modeCnt +1].width = r_dx.adapterModes[i].Width;
			sortedResolutions[modeCnt +1].height = r_dx.adapterModes[i].Height;
			++modeCnt;
		}

		for(k = 0; k < numRefreshRates && sortedRefreshRates[k +1] != r_dx.adapterModes[i].RefreshRate; ++k );

		if(k < numRefreshRates)
		{
			continue;
		}

		sortedRefreshRates[numRefreshRates +1] = r_dx.adapterModes[i].RefreshRate;
		++numRefreshRates;
	}

	if ( modeCnt == 0 )
	{
		R_DirectXError(va("No valid resolutions of %i x %i or above found", 640, 480));
	}

	stringBuf = r_dx.modeStringList;
	for(i = 0; i < modeCnt; ++i)
	{
		r_dx.resolutionStrings[i] = stringBuf;
		stringBuf += sprintf(stringBuf, "%ix%i", sortedResolutions[i + 1].width, sortedResolutions[i + 1].height) + 1;
	}
	r_dx.resolutionStrings[i] = NULL;

	r_mode = Cvar_RegisterEnum("r_mode", r_dx.resolutionStrings, 0, 0x21, "Direct X resolution mode");

	qsort(&sortedRefreshRates[1], numRefreshRates, sizeof(sortedRefreshRates[1]), R_SortAdapterRefreshRates);

    refreshRateDefault = 0;

	for(i = 0; i < numRefreshRates ; ++i)
	{
		r_dx.refreshRateStrings[i] = stringBuf;
		stringBuf += sprintf(stringBuf, "%i Hz", sortedRefreshRates[i + 1]) + 1;

		//Set highest refresh rate but max 60Hz as default
		if ( sortedRefreshRates[i + 1] <= 60 )
		{
			refreshRateDefault = i;
		}

	}

	r_dx.refreshRateStrings[i] = NULL;

	r_displayRefresh = Cvar_RegisterEnum("r_displayRefresh", r_dx.refreshRateStrings, refreshRateDefault, 0x221, "Refresh rate");

}




void ScrPlace_ApplyRect(const ScreenPlacement *scrPlace, float *x, float *y, float *w, float *h, int horzAlign, int vertAlign)
{
  float v7;
  float v8;
  float v9;
  float v10;

  switch ( horzAlign )
  {
    case 7:
      v7 = *x * scrPlace->scaleVirtualToReal[0];
      v8 = (float)(scrPlace->realViewableMin[0] + scrPlace->realViewableMax[0]) * 0.5;
      *x = v7 + v8;
      *w = *w * scrPlace->scaleVirtualToReal[0];
      break;
    case 5:
      break;
    default:
      *x = (float)(*x * scrPlace->scaleVirtualToReal[0]) + scrPlace->subScreenLeft;
      *w = *w * scrPlace->scaleVirtualToReal[0];
      break;
    case 6:
      *x = *x * scrPlace->scaleRealToVirtual[0];
      *w = *w * scrPlace->scaleRealToVirtual[0];
      break;
    case 4:
      *x = *x * scrPlace->scaleVirtualToFull[0];
      *w = *w * scrPlace->scaleVirtualToFull[0];
      break;
    case 3:
      *x = (float)(*x * scrPlace->scaleVirtualToReal[0]) + scrPlace->realViewableMax[0];
      *w = *w * scrPlace->scaleVirtualToReal[0];
      break;
    case 2:
      v7 = *x * scrPlace->scaleVirtualToReal[0];
      v8 = 0.5 * scrPlace->realViewportSize[0];
      *x = v7 + v8;
      *w = *w * scrPlace->scaleVirtualToReal[0];
      break;
    case 1:
      *x = (float)(*x * scrPlace->scaleVirtualToReal[0]) + scrPlace->realViewableMin[0];
      *w = *w * scrPlace->scaleVirtualToReal[0];
      break;
  }

  switch ( vertAlign )
  {
    case 7:
      v9 = *y * scrPlace->scaleVirtualToReal[1];
      v10 = (float)(scrPlace->realViewableMin[1] + scrPlace->realViewableMax[1]) * 0.5;
      *y = v9 + v10;
      *h = *h * scrPlace->scaleVirtualToReal[1];
    case 5:
      return;
    default:
      *y = *y * scrPlace->scaleVirtualToReal[1];
      *h = *h * scrPlace->scaleVirtualToReal[1];
      break;
    case 1:
      *y = (float)(*y * scrPlace->scaleVirtualToReal[1]) + scrPlace->realViewableMin[1];
      *h = *h * scrPlace->scaleVirtualToReal[1];
      break;
    case 6:
      *y = *y * scrPlace->scaleRealToVirtual[1];
      *h = *h * scrPlace->scaleRealToVirtual[1];
      break;
    case 4:
      *y = *y * scrPlace->scaleVirtualToFull[1];
      *h = *h * scrPlace->scaleVirtualToFull[1];
      break;
    case 3:
      *y = (float)(*y * scrPlace->scaleVirtualToReal[1]) + scrPlace->realViewableMax[1];
      *h = *h * scrPlace->scaleVirtualToReal[1];
      break;
    case 2:
      v9 = *y * scrPlace->scaleVirtualToReal[1];
      v10 = 0.5 * scrPlace->realViewportSize[1];
      *y = v9 + v10;
      *h = *h * scrPlace->scaleVirtualToReal[1];
      break;
  }
}


Material* R_PixelCost_GetAccumulationMaterial(Material *material)
{
  Material *m;
  int bits; 

  if ( pixelCostMode == 4 || pixelCostMode == 6 )
  {
    m = rgp.pixelCostAddDepthDisableMaterial;
    return m;
  }
  else
  {
    if ( material->techniqueSet->techniques[4] )
      bits = material->stateBitsEntry[4];
    else
      bits = 0;
    switch ( material->stateBitTable[bits].loadBits[1] & 0xF )
    {
      case 2:
      case 3:
      case 6:
      case 7:
      case 0xA:
      case 0xB:
      case 0xE:
      case 0xF:
        m = rgp.pixelCostAddDepthDisableMaterial;
        break;
      case 0:
      case 1:
        m = rgp.pixelCostAddDepthAlwaysMaterial;
        break;
      case 4:
      case 5:
        m = rgp.pixelCostAddDepthLessMaterial;
        break;
      case 8:
      case 9:
        m = rgp.pixelCostAddDepthEqualMaterial;
        break;
      case 0xD:
        m = rgp.pixelCostAddDepthWriteMaterial;
        break;
      default:
        m = rgp.pixelCostAddNoDepthWriteMaterial;
        break;
    }
  }
  return m;
}

const char* R_GetTechsetName(int id)
{
  const char *techstrings[35];

  techstrings[0] = "TECHNIQUE_DEPTH_PREPASS";
  techstrings[1] = "TECHNIQUE_BUILD_FLOAT_Z";
  techstrings[2] = "TECHNIQUE_BUILD_SHADOWMAP_DEPTH";
  techstrings[3] = "TECHNIQUE_BUILD_SHADOWMAP_COLOR";
  techstrings[4] = "TECHNIQUE_UNLIT";
  techstrings[5] = "TECHNIQUE_EMISSIVE";
  techstrings[6] = "TECHNIQUE_EMISSIVE_SHADOW";
  techstrings[7] = "TECHNIQUE_LIT";
  techstrings[8] = "TECHNIQUE_LIT_SUN";
  techstrings[9] = "TECHNIQUE_LIT_SUN_SHADOW";
  techstrings[10] = "TECHNIQUE_LIT_SPOT";
  techstrings[11] = "TECHNIQUE_LIT_SPOT_SHADOW";
  techstrings[12] = "TECHNIQUE_LIT_OMNI";
  techstrings[13] = "TECHNIQUE_LIT_OMNI_SHADOW";
  techstrings[14] = "TECHNIQUE_LIT_INSTANCED";
  techstrings[15] = "TECHNIQUE_LIT_INSTANCED_SUN";
  techstrings[16] = "TECHNIQUE_LIT_INSTANCED_SUN_SHADOW";
  techstrings[17] = "TECHNIQUE_LIT_INSTANCED_SPOT";
  techstrings[18] = "TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW";
  techstrings[19] = "TECHNIQUE_LIT_INSTANCED_OMNI";
  techstrings[20] = "TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW";
  techstrings[21] = "TECHNIQUE_LIGHT_SPOT";
  techstrings[22] = "TECHNIQUE_LIGHT_OMNI";
  techstrings[23] = "TECHNIQUE_LIGHT_SPOT_SHADOW";
  techstrings[24] = "TECHNIQUE_FAKELIGHT_NORMAL";
  techstrings[25] = "TECHNIQUE_FAKELIGHT_VIEW";
  techstrings[26] = "TECHNIQUE_SUNLIGHT_PREVIEW";
  techstrings[27] = "TECHNIQUE_CASE_TEXTURE";
  techstrings[28] = "TECHNIQUE_WIREFRAME_SOLID";
  techstrings[29] = "TECHNIQUE_WIREFRAME_SHADED";
  techstrings[30] = "TECHNIQUE_SHADOWCOOKIE_CASTER";
  techstrings[31] = "TECHNIQUE_SHADOWCOOKIE_RECEIVER";
  techstrings[32] = "TECHNIQUE_DEBUG_BUMPMAP";
  techstrings[33] = "TECHNIQUE_DEBUG_BUMPMAP_INSTANCED";
  techstrings[34] = "TECHNIQUE_COUNT";
  return techstrings[id];
}


//void __usercall RB_BeginSurface(Material_s *material@<esi>, MaterialTechniqueType techType@<edi>)
void __cdecl RB_BeginSurface(Material *material, MaterialTechniqueType techType)
{
  if ( r_logFile->boolean && rdebugfilehandle)
  {
    fprintf(rdebugfilehandle, "---------- RB_BeginSurface( %s, %s )\n", material->info.name, R_GetTechsetName(techType));
    fflush(rdebugfilehandle);
  }
  tess.firstVertex = 0;
  tess.lastVertex = 0;
  gfxCmdBufState.material = material;
  gfxCmdBufState.techType = techType;
  gfxCmdBufState.prim.vertDeclType = 0;
  gfxCmdBufState.origMaterial = material;
  gfxCmdBufState.origTechType = techType;
  
  if ( pixelCostMode > 2 )
  {
    material = R_PixelCost_GetAccumulationMaterial(material);
    techType = TECHNIQUE_UNLIT;
    gfxCmdBufState.material = material;
    gfxCmdBufState.techType = TECHNIQUE_UNLIT;
  }
  gfxCmdBufState.technique = material->techniqueSet->remappedTechniqueSet->techniques[techType];
}

/*
#define MAX_MATERIALS 4096

Material *sortedMaterials[MAX_MATERIALS];
Material* materialHashTable[MAX_MATERIALS];


void R_PatchSortedMaterialEntry(unsigned int addr)
{
  *(Material***)addr = sortedMaterials;
}

void R_PatchMaterialHashTableEntry(unsigned int addr)
{
  *(Material***)addr = materialHashTable;
}


void R_PatchSortedMaterials()
{
  R_PatchSortedMaterialEntry(0x5f27c6);
  R_PatchMaterialHashTableEntry(0x5f27de);
  *(uint32_t*)0x5f27d6 = MAX_MATERIALS -1;
  *(uint32_t*)0x5f2794 = MAX_MATERIALS -1;
  *(uint32_t*)0x5f97eb = MAX_MATERIALS -1;
  R_PatchSortedMaterialEntry(0x5f97f2);
  *(uint32_t*)0x6029d3 = MAX_MATERIALS;
  R_PatchSortedMaterialEntry(0x6029d8);
  R_PatchSortedMaterialEntry(0x6029f2);
  *(uint32_t*)0x62171d = MAX_MATERIALS;
  R_PatchSortedMaterialEntry(0x621722);
  R_PatchSortedMaterialEntry(0x62173C);
  *(uint32_t*)0x648e0f = MAX_MATERIALS -1;
  R_PatchSortedMaterialEntry(0x648e1b);
  *(uint32_t*)0x648e6d = MAX_MATERIALS -1;
  R_PatchSortedMaterialEntry(0x648e74);
  *(uint32_t*)0x649005 = MAX_MATERIALS -1;
  R_PatchSortedMaterialEntry(0x64900f);
    
  R_PatchMaterialHashTableEntry(0x5f282f);
  R_PatchMaterialHashTableEntry(0x5f2987);
  R_PatchMaterialHashTableEntry(0x5f2997);
  R_PatchMaterialHashTableEntry(0x5f29e3);
  *(uint32_t*)0x5f29cf = MAX_MATERIALS -1;
  R_PatchMaterialHashTableEntry(0x5f2a2a);
  R_PatchMaterialHashTableEntry(0x5f2b12);
  *(uint32_t*)0x5f2b26 = 4 * MAX_MATERIALS;
  *(uint32_t*)0x5f2ba7 = 4 * MAX_MATERIALS;
  R_PatchMaterialHashTableEntry(0x5f2bae);

  R_PatchMaterialHashTableEntry(0x5f2d32);
  *(uint32_t*)0x5f21fc = MAX_MATERIALS -1;

  *(DWORD*)0x5F4E1B += 0x2000; //memset just rgp +0x2000 so debugger does not care about memset
  *(DWORD*)0x5F4E15 -= 0x2000; //Memset smaller size
  // --> I think memset() has to be hooked to reset our new array as well
}

*/

Material *__cdecl Material_FromHandle(Material *handle)
{
  assert(handle);
  assert(handle->info.name);
  assert(handle->info.name[0]);
  return handle;
}

qboolean __cdecl IsValidMaterialHandle(Material *const handle)
{
  if((int)handle < (int)0xFA4C98 || (int)handle >= (int)0xFCCC98 )
  {
    return qfalse;
  }
  int matoffset = (int)handle - 0xFA4C98;
  if(matoffset % sizeof(Material) != 0)
  {
    return qfalse;
  }
  return handle && handle->info.name && *handle->info.name;
}


void REGPARM(1) R_StoreWindowSettings(GfxWindowParms_t *wndParms)
{
  double rationselector; 
  int monitorHeight;
  int monitorWidth;


  assert(r_aspectRatio);

  _vidConfig.sceneWidth = wndParms->sceneWidth;
  _vidConfig.sceneHeight = wndParms->sceneHeight;
  _vidConfig.displayWidth = wndParms->displayWidth;
  _vidConfig.displayHeight = wndParms->displayHeight;
  _vidConfig.displayFrequency = wndParms->hz;
  _vidConfig.isFullscreen = wndParms->fullscreen;
  switch ( r_aspectRatio->integer )
  {
    case 0:
      if ( _vidConfig.isFullscreen && r_dx.adapterNativeIsValid )
      {
        monitorWidth = r_dx.adapterNativeWidth;
        monitorHeight = r_dx.adapterNativeHeight;
      }
      else
      {
        monitorWidth = _vidConfig.displayWidth;
        monitorHeight = _vidConfig.displayHeight;
      }
      rationselector = ((float)monitorHeight * 16.0) / (float)monitorWidth;
      if ( (signed int)rationselector == 10 )
      {
        _vidConfig.aspectRatioWindow = 1.6;
      }
      else if ( (signed int)rationselector >= 10 )
      {
        _vidConfig.aspectRatioWindow = 1.3333334;
      }
      else if((signed int)rationselector < 7)
      {
        _vidConfig.aspectRatioWindow = 2.3333333;
      }
      else
      {
        _vidConfig.aspectRatioWindow = 1.7777778;
      }
      break;
    default: //error case
    case 1:
      _vidConfig.aspectRatioWindow = 1.3333334;
      break;
    case 2:
      _vidConfig.aspectRatioWindow = 1.6;
      break;
    case 3:
      _vidConfig.aspectRatioWindow = 1.7777778;
      break;
    case 4:
      _vidConfig.aspectRatioWindow = 2.3333333;
      break;
  }
  assert ( r_wideScreen );

  if ( _vidConfig.aspectRatioWindow > 1.3333334 )
  {
    Cvar_SetBool(r_wideScreen, 1);
  }
  else
  {
    Cvar_SetBool(r_wideScreen, 0);
  }
  _vidConfig.aspectRatioScenePixel = (double)_vidConfig.sceneHeight * _vidConfig.aspectRatioWindow / (double)_vidConfig.sceneWidth;
  if ( _vidConfig.isFullscreen )
  {
    _vidConfig.aspectRatioDisplayPixel = ((float)r_dx.adapterFullscreenHeight * (float)_vidConfig.aspectRatioWindow) / (float)r_dx.adapterFullscreenWidth;
    if(_vidConfig.aspectRatioWindow > 2.0 && _vidConfig.aspectRatioDisplayPixel > 1.0)
    {
      _vidConfig.aspectRatioDisplayPixel = 1.0;
    }
  }
  else
  {
    _vidConfig.aspectRatioDisplayPixel = 1.0;
  }
  /*
  vidConfig.isToolMode = 0;
  v3 = r_reflectionProbeGenerate && r_reflectionProbeGenerate->current.enabled;
  vidConfig.isToolMode = v3;
  */
}


void __cdecl ScrPlace_SetupViewport(ScreenPlacement *scrPlace, int viewportX, int viewportY, int viewportWidth, int viewportHeight)
{
  ScrPlace_SetupFloatViewport(scrPlace, (float)viewportX, (float)viewportY, (float)viewportWidth, (float)viewportHeight);
}

struct Font_s *__cdecl R_RegisterFont_FastFile(const char *fontName)
{
  return DB_FindXAssetHeader(ASSET_TYPE_FONT, fontName).font;
}