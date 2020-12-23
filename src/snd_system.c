
#include "q_shared.h"
#include "qcommon.h"
#include "snd_system.h"
#include "xassets/sounds.h"

#define DIRECTSOUND_VERSION 0x0800

#include <string.h>
#include <windows.h>
#include <mmreg.h>
#include <mmsystem.h>

//#include "dsound.h"
#include <dsound.h>

#pragma pack(push, 1)

#define SND_BASE_ADDR 0x0CC8E250
#define SNDVOICECHANNELSSEL_BASE_ADDR 0x0CC1B4D0
#define sndsystem (*((sndSystem_t*)(SND_BASE_ADDR)))
#define voiceChannelsSelection ((voiceInit_t**)(SNDVOICECHANNELSSEL_BASE_ADDR))
#define captureDeviceInit *(byte*)0xD5EC42E
#define captureDeviceInitB *(byte*)0xD5EC42F
#define g_voice_initialized *(qboolean*)0xD5EC510

#define s_clientTalkTime ((int*)0xCC1B5D0)

typedef struct
{
  char field_0;
  char field_1;
  char field_2;
  char field_3;
  int field_4;
  int field_8;
  int field_C;
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
  __int16 field_44;
  char field_46;
  char field_47;
  int field_48;
  int field_4C;
}sndChannel_t;

typedef struct
{
  int field_0;
  int field_4;
  int field_8;
  int field_C;
  int field_10;
  int field_14;
  int field_18;
  int field_1C;
  int field_20;
}sndAxis_t;

typedef struct
{
  int field_0;
  int field_4;
  int field_8;
  sndAxis_t field_C;
  int field_30;
  int field_34;
}sndObj_t;


typedef struct
{
  int field_0;
  float field_4;
  float field_8;
  float field_C;
  float field_10;
  float field_14;
  float field_18;
}sndEnvironment_t;

typedef struct
{
  vec3_t vect[64];
  int field_300;
  char field_304[2300];
  int field_C00;
}sndChannelsAttr_t;


typedef struct
{
  byte field_0;
  byte pad76[3];
  int field_4;
  int field_8;
  int field_C;
  int field_10;
  int field_14;
  int field_18;
  int field_1C;
  char field_20[16384];
  int field_4020;
  float field_4024;
  float field_4028;
  float field_402C;
  float field_4030;
  sndChannelsAttr_t channelVects;
  int field_4C38;
  int field_4C3C;
  int field_4C40;
  sndChannelsAttr_t *channelVectPtr;
  int field_4C48;
  char field_4C4C[36];
  int field_4C70;
  float field_4C74;
  sndEnvironment_t sndEnv;
  char field_4C94;
  char field_4C95;
  char field_4C96;
  char field_4C97;
  int field_4C98;
  char field_4C9C[24];
  int field_4CB4;
  char field_4CB8[28];
  int field_4CD4;
  sndEnvironment_t *sndEnvPtr;
  byte field_4CDC[64];
  byte field_4D1C[64];
  sndObj_t listenerSndObj;
  sndObj_t defaultSndObj;
  int initTime1;
  int initTime2;
  sndObj_t *curSndObj;
  int field_4DD8;
  int field_4DDC;
  int field_4DE0;
  int field_4DE4;
  int field_4DE8;
  sndChannel_t sndChannels[64];
  int entChannelCount;
  int field_61F0;
  int field_61F4;
  int field_61F8;
  int field_61FC;
  int field_6200;
}sndSystem_t;

typedef struct
{
  LPDIRECTSOUNDCAPTUREBUFFER pDSCBuffer;
  LPDIRECTSOUNDBUFFER dsbuff;
  int field_8;
  int field_C;
  int field_10;
  int field_14;
  int field_18;
  int field_1C;
  int field_20;
  int field_24;
  int field_28;
  int sampleRate;
  int field_30;
  int field_34;
  int nChannels;
  int field_3C;
  int field_40;
  int field_44;
}voiceInit_t;

#pragma pack(pop)

HRESULT DSoundBufferCreate(DWORD sampleRate, DWORD nChannels, LPDIRECTSOUNDBUFFER *ppDSBuffer);
void REGPARM(1) sub_4ECF20(voiceInit_t *a1);



cvar_t* snd_errorOnMissing;
cvar_t* snd_volume;
cvar_t* snd_slaveFadeTime;
cvar_t* snd_enable2D;
cvar_t* snd_enable3D;
cvar_t* snd_enableStream;
cvar_t* snd_enableReverb;
cvar_t* snd_enableEq;
cvar_t* snd_draw3D;
cvar_t* snd_levelFadeTime;
cvar_t* snd_cinematicVolumeScale;
cvar_t* snd_touchStreamFilesOnLoad;
cvar_t* winvoice_mic_mute;
cvar_t* winvoice_mic_reclevel;
cvar_t* winvoice_save_voice;
cvar_t* winvoice_mic_scaler;

void SND_CopyCvars()
{
	*(cvar_t**)0x0CC8E248 = snd_errorOnMissing;
	*(cvar_t**)0x0CC8E244 = snd_volume;
	*(cvar_t**)0x0CC965D0 = snd_slaveFadeTime;
	*(cvar_t**)0x0CC8E238 = snd_enable2D;
	*(cvar_t**)0x0CC8E230 = snd_enable3D;
	*(cvar_t**)0x0CC965D4 = snd_enableStream;
	*(cvar_t**)0x0CC8E24C = snd_enableReverb;
	*(cvar_t**)0x0CC8E234 = snd_enableEq;
	*(cvar_t**)0x0CC8E240 = snd_draw3D;
	*(cvar_t**)0x0CC965D8 = snd_levelFadeTime;
	*(cvar_t**)0x0CC8E22C = snd_cinematicVolumeScale;
	*(cvar_t**)0x0CC8E23C = snd_touchStreamFilesOnLoad;
	*(cvar_t**)0x0CC1B4C4 = winvoice_mic_mute;
	*(cvar_t**)0x0CC1B3BC = winvoice_mic_reclevel;
	*(cvar_t**)0x0CC1B6E8 = winvoice_save_voice;
	*(cvar_t**)0x0CC1B4C8 = winvoice_mic_scaler;
}

static int lastVoiceChannel;



void SND_StopAmbient_f()
{
	SND_StopAmbient(0);
}

void SND_PauseSounds_f()
{
	SND_PauseSounds( );
}

void SND_UnpauseSounds_f()
{
	SND_UnpauseSounds( );
}

void SND_Init()
{
	int i;

	static const char *snd_draw3DNames[] = { "Off", "Targets", "Names", "Verbose", NULL };

  Com_Printf(CON_CHANNEL_SOUND, "\n------- sound system initialization -------\n");

  snd_errorOnMissing = Cvar_RegisterBool("snd_errorOnMissing", 0, 1u, "Cause a Com_Error if a sound file is missing.");
  snd_volume = Cvar_RegisterFloat("snd_volume", 0.80000001, 0.0, 1.0, 1u, "Game sound master volume");
  snd_slaveFadeTime = Cvar_RegisterInt( "snd_slaveFadeTime", 500, 0, 5000, 0x81u, "The amount of time in milliseconds for a 'slave' sound\nto fade its volumes when a master sound starts or stops");
  snd_enable2D = Cvar_RegisterBool("snd_enable2D", qtrue, 0x80u, "Enable 2D sounds");
  snd_enable3D = Cvar_RegisterBool("snd_enable3D", qtrue, 0x80u, "Enable 3D sounds");
  snd_enableStream = Cvar_RegisterBool("snd_enableStream", qtrue, 0x80u, "Enable streamed sounds");
  snd_enableReverb = Cvar_RegisterBool("snd_enableReverb", qtrue, 0x80u, "Enable sound reverberation");
  snd_enableEq = Cvar_RegisterBool("snd_enableEq", qtrue, 1u, "Enable equalization filter");
  snd_draw3D = Cvar_RegisterEnum( "snd_draw3D", snd_draw3DNames, 0, 0x80u, "Draw the position and info of world sounds");
  snd_levelFadeTime = Cvar_RegisterInt( "snd_levelFadeTime", 250, 0, 5000, 0x81u, "The amout of time in milliseconds for all audio to fade in at the start of a level");
  snd_cinematicVolumeScale = Cvar_RegisterFloat( "snd_cinematicVolumeScale", 0.85000002, 0.0, 1.0, 1u, "Scales the volume of Bink videos.");
  snd_touchStreamFilesOnLoad = Cvar_RegisterBool( "snd_touchStreamFilesOnLoad", 0, 1u, "Check whether stream sound files exist while loading");


  SND_CopyCvars();

  lastVoiceChannel = 0;

  sndsystem.sndEnv.field_4 = 1.0;
  sndsystem.sndEnv.field_8 = 1.0;
  sndsystem.sndEnvPtr = &sndsystem.sndEnv;
  sndsystem.sndEnv.field_C = 0.0;
  sndsystem.sndEnv.field_0 = 0;
  sndsystem.sndEnv.field_10 = 0.0;
  sndsystem.field_4C94 = 1;
  sndsystem.sndEnv.field_14 = 0.0;
  sndsystem.curSndObj = &sndsystem.defaultSndObj;
  sndsystem.sndEnv.field_18 = 0.0;
  SND_LoadEntChannelFile();
  sndsystem.field_4028 = 1.0;
  sndsystem.field_402C = 1.0;
  sndsystem.field_4 = 1;
  sndsystem.channelVectPtr = &sndsystem.channelVects;
  sndsystem.field_4030 = 0.0;

	for(i = 0; i < sndsystem.entChannelCount; i++)
	{
		sndsystem.channelVectPtr->vect[i][0] = 1.0;
		sndsystem.channelVectPtr->vect[i][1] = 1.0;
		sndsystem.channelVectPtr->vect[i][2] = 0.0;
	}

  sndsystem.channelVectPtr->field_300 = 1;
  sndsystem.field_4C74 = 0.0;
  sndsystem.initTime1 = sndsystem.initTime2 = Sys_Milliseconds();
  sndsystem.field_4024 = snd_volume->value * 0.75;
	for(i = 0; i < sndsystem.entChannelCount; i++)
	{
      sndsystem.field_4CDC[i] = sndsystem.sndChannels[i].field_46;
	}

    Com_Memcpy(sndsystem.field_4D1C, sndsystem.field_4CDC, sizeof(sndsystem.field_4D1C));
	*(int*)0xCC96348 = 0;
    Cmd_AddCommand("snd_setEnvironmentEffects", SND_SetEnvironmentEffects_f);
    Cmd_AddCommand("snd_deactivateEnvironmentEffects", SND_DeactivateEnvironmentEffects_f);
    Cmd_AddCommand("snd_playLocal", SND_PlayLocal_f);
    Cmd_AddCommand("snd_setEq", SND_SetEq_f );
    Cmd_AddCommand("snd_setEqFreq", SND_SetEqFreq_f);
    Cmd_AddCommand("snd_setEqGain", SND_SetEqGain_f);
    Cmd_AddCommand("snd_setEqQ", SND_SetEqQ_f);
    Cmd_AddCommand("snd_setEqType", SND_SetEqType_f);
	Cmd_AddCommand("snd_deactivateEq", SND_DeactivateEq_f);



	  Cmd_AddCommand("snd_stopambient", SND_StopAmbient_f);
	  Cmd_AddCommand("snd_pause", SND_PauseSounds_f);
	  Cmd_AddCommand("snd_unpause", SND_UnpauseSounds_f);


	Voice_Init();
	Com_Printf(CON_CHANNEL_SOUND, "------- sound system successfully initialized -------\n");
}


#define MAX_CAPTURECHANNELS 65
voiceInit_t* __cdecl Voice_CreateChannel()
{
	static voiceInit_t voiceChannels[MAX_CAPTURECHANNELS];
	voiceInit_t *chan;

	if ( captureDeviceInit == qfalse )
	{
		return NULL;
	}

	if(lastVoiceChannel == MAX_CAPTURECHANNELS)
	{
		Com_PrintError(CON_CHANNEL_SOUND, "Voice_CreateChannel: Exceeded MAX_CAPTURECHANNELS\n");
		return NULL;
	}
    chan = &voiceChannels[lastVoiceChannel];
    ++lastVoiceChannel;
    memset(chan, 0, sizeof(voiceInit_t));
    chan->nChannels = 1;
    chan->sampleRate = 8192;
    chan->field_30 = 255;
    chan->field_34 = 128;
    chan->field_1C = -1;
    chan->field_44 = 2; //Lobyte?
    return chan;

}


int __cdecl sub_57B1D0(uint16_t a1)
{
  DWORD v1; // esi@5
  DWORD v2; // edi@5
  HMIXEROBJ hmxobj; // [sp+10h] [bp-178h]@3
  int v5; // [sp+14h] [bp-174h]@12
  struct tMIXERCONTROLDETAILS pmxcd; // [sp+18h] [bp-170h]@12
  struct tagMIXERLINECONTROLSA pmxlc; // [sp+30h] [bp-158h]@11
  struct tagMIXERLINEA pmxl; // [sp+48h] [bp-140h]@4
  MIXERCONTROLA v9; // [sp+F0h] [bp-98h]@11

  if ( waveInGetNumDevs() < 1 || mixerGetNumDevs() < 1 || mixerOpen((LPHMIXER)&hmxobj, 0, 0, 0, 0))
  {
    return 0;
  }
  pmxl.cbStruct = 168;
  pmxl.dwComponentType = 7;
  if ( mixerGetLineInfoA(hmxobj, &pmxl, 3u) )
  {
    return 0;
  }
  v1 = 0;
  v2 = pmxl.cConnections;
  if ( !pmxl.cConnections )
  {
      mixerClose((HMIXER)hmxobj);
      return 0;
  }
  while ( 1 )
  {
    pmxl.dwSource = v1;
    if ( !mixerGetLineInfoA(hmxobj, &pmxl, 1u) )
    {
      if ( strstr(pmxl.szName, "Mic") )
      {
        break;
      }
    }
    if ( ++v1 >= v2 )
    {
      mixerClose((HMIXER)hmxobj);
      return 0;
    }
  }
  pmxlc.dwLineID = pmxl.dwLineID;
  pmxlc.cbStruct = 24;
  pmxlc.dwControlID = 0x50030001;
  pmxlc.cControls = 1;
  pmxlc.cbmxctrl = 148;
  pmxlc.pamxctrl = &v9;
  if ( mixerGetLineControlsA(hmxobj, &pmxlc, 2u) )
  {
    mixerClose((HMIXER)hmxobj);
    return 0;
  }
  pmxcd.cMultipleItems = 0;
  pmxcd.paDetails = &v5;
  pmxcd.cbStruct = 24;
  pmxcd.cChannels = 1;
  pmxcd.cbDetails = 4;
  pmxcd.dwControlID = v9.dwControlID;
  if ( !mixerGetControlDetailsA(hmxobj, &pmxcd, 0) )
  {
    v5 = a1;
    mixerSetControlDetails(hmxobj, &pmxcd, 0);
  }
  mixerClose((HMIXER)hmxobj);
  return 0;
}


void __cdecl sub_57AE10()
{
  int i;

  if ( SND_IsInitialized())
  {
    if ( *(int*)0xCC1B4C0 != (uint16_t)(signed int)winvoice_mic_reclevel->floatval )
    {
      *(int*)0xCC1B4C0 = (unsigned __int16)winvoice_mic_reclevel->floatval;
      sub_57B1D0(*(int*)0xCC1B4C0);
    }
    for(i = 0; i < 64; ++i)
    {
      if(voiceChannelsSelection[i])
      {
        sub_4ECF20(voiceChannelsSelection[i]);
      }
    }
  }
}



void Voice_Init()
{

  int i;

  HWND desktopwin; // esi@1
  voiceInit_t *vo; // esi@4
  LPDIRECTSOUNDBUFFER *lplpdsoundbuff;


  winvoice_mic_mute = Cvar_RegisterBool("winvoice_mic_mute", qfalse, CVAR_ARCHIVE, "Mute the microphone");
  winvoice_mic_reclevel = Cvar_RegisterFloat("winvoice_mic_reclevel", 65535.0, 0.0, 65535.0, CVAR_ARCHIVE, "Microphone recording level");
  winvoice_save_voice = Cvar_RegisterBool("winvoice_save_voice", 0, CVAR_ARCHIVE, "Write voice data to a file");
  winvoice_mic_scaler = Cvar_RegisterFloat("winvoice_mic_scaler", 1.0, 0.25, 2.0, CVAR_ARCHIVE, "Microphone scaler value");

  SND_CopyCvars();

  sub_57B350();
  sub_57B5C0("Mic");
  *(int*)0xCC1B3B8 = mixerGetRecordSource();
  *(int*)0xCC1B4C0 = (unsigned __int16)winvoice_mic_reclevel->floatval;
  sub_57B1D0( winvoice_mic_reclevel->floatval );
  sub_57B860( winvoice_mic_mute->boolean );
  *(int*)0xD5EC43C = 0;
  desktopwin = GetDesktopWindow();
  g_voice_initialized = SND_InitDSCaptureDevice();
  sub_4ED0B0(desktopwin);
  sub_4ECA60();
  sub_4EC880();
  Com_Memset(s_clientTalkTime, 0, 256);
  for(i = 0; i < 64; i++)
  {
    if ( !*(int*)0xD5EC42D )
    {
        voiceChannelsSelection[i] = NULL;
      continue;
    }

    vo = Voice_CreateChannel();

    if(vo == NULL)
    {
        voiceChannelsSelection[i] = NULL;
      continue;
    }

    lplpdsoundbuff = &vo->dsbuff;
    vo->field_8 = 51200;
    if ( DSoundBufferCreate(vo->sampleRate, vo->nChannels, &vo->dsbuff) < 0 )
    {
      Com_Printf(CON_CHANNEL_SOUND, "Error: Failed to create DirectSound play buffer\n");
      *lplpdsoundbuff = NULL;
      vo = NULL;
    }
    voiceChannelsSelection[i] = vo;
  }
}

qboolean IsSelectedVoiceChannelValid(unsigned int channel)
{
	if(channel >= 64)
	{
		return qfalse;
	}
	if(voiceChannelsSelection[channel] == NULL)
	{
		return qfalse;
	}
	return qtrue;

}

HRESULT DSoundBufferCreate(DWORD sampleRate, DWORD nChannels, LPDIRECTSOUNDBUFFER *ppDSBuffer)
{

  int nBlockAlign;
  HRESULT hRes;
  WAVEFORMATEX fxFormat;
  DSBUFFERDESC pcDSBufferDesc;

  hRes = -1;

  fxFormat.cbSize = 0;
  nBlockAlign = 16 * nChannels / 8;
  fxFormat.nBlockAlign = nBlockAlign;
  fxFormat.nChannels = nChannels;
  fxFormat.nAvgBytesPerSec = sampleRate * (unsigned __int16)nBlockAlign;
  pcDSBufferDesc.lpwfxFormat = &fxFormat;
  pcDSBufferDesc.dwReserved = 0;
  Com_Memset(&pcDSBufferDesc.guid3DAlgorithm, 0, sizeof(pcDSBufferDesc.guid3DAlgorithm));
  fxFormat.wFormatTag = 1;
  fxFormat.wBitsPerSample = 16;
  fxFormat.nSamplesPerSec = sampleRate;
  pcDSBufferDesc.dwSize = 36;
  pcDSBufferDesc.dwFlags = 33000;
  pcDSBufferDesc.dwBufferBytes = 51200;

  LPDIRECTSOUND lpds = *(LPDIRECTSOUND*)0x0D5EC44C;

  if(lpds)
  {
	hRes = IDirectSound8_CreateSoundBuffer(lpds, &pcDSBufferDesc, ppDSBuffer, 0);
  }

  if ( hRes < 0 )
  {
    Com_Printf(CON_CHANNEL_SOUND, "Failed to create sound buffer!\n");
  }
  return hRes;
}

qboolean __cdecl SND_InitDSCaptureDevice()
{
  HRESULT hres;

  *(int*)0x1408550 = 0x4ED320; /* Function pointer copy */
  captureDeviceInit = qfalse;
  hres = DirectSoundCaptureCreate(0, (LPDIRECTSOUNDCAPTURE*)0xD5EC458, 0);
  if ( hres >= 0 )
  {
    captureDeviceInit = qtrue;
    return qtrue;
  }
  Com_PrintError(CON_CHANNEL_SOUND, "Error initializing direct sound instance!\n");
  return qfalse;
}

qboolean __stdcall Sub_57B5C0_Patch(HMIXEROBJ hmxobj, struct tagMIXERLINEA* pmxl, DWORD flag)
{
  pmxl->cbStruct = 168;
  pmxl->dwComponentType = 7;
  if(mixerGetLineInfoA(hmxobj, pmxl, flag) != MMSYSERR_NOERROR)
  {
    Com_PrintError(CON_CHANNEL_SOUND, "Sub_57B5C0: Failed to get line info!\n");
    mixerClose((HMIXER)hmxobj);
    return qfalse;
  }
  return qtrue;
}


DWORD mixerGetRecordSource()
{
  DWORD i;
  DWORD numConnections;
  HMIXEROBJ hmxobj;
  DWORD paDetails;
  struct tagMIXERLINECONTROLSA pmxlc;
  struct tMIXERCONTROLDETAILS pmxcd;
  struct tagMIXERLINEA pmxl;
  MIXERCONTROLA mixerControlObj;

  if ( waveInGetNumDevs() < 1 || mixerGetNumDevs() < 1 || mixerOpen((LPHMIXER)&hmxobj, 0, 0, 0, 0) )
  {
    return -1;
  }
  pmxl.cConnections = 0;
  pmxl.cbStruct = 168;
  pmxl.dwComponentType = 7;

  if(mixerGetLineInfoA(hmxobj, &pmxl, 3u) != MMSYSERR_NOERROR)
  {
    Com_PrintError(CON_CHANNEL_SOUND, "mixerGetRecordSource: Failed to get line info(0)!\n");
    mixerClose((HMIXER)hmxobj);
    return -1;
  }

  numConnections = pmxl.cConnections;

  if(numConnections >= 128)
  {
	Com_PrintError(CON_CHANNEL_SOUND, "mixerGetRecordSource exceeded 128 devices!\n");
	numConnections = 0;
  }

  for( i = 0; i < numConnections; i++ )
  {
    pmxl.dwSource = i;
    if(mixerGetLineInfoA(hmxobj, &pmxl, 1u) != MMSYSERR_NOERROR)
    {
      Com_PrintError(CON_CHANNEL_SOUND, "mixerGetRecordSource: Failed to get line info(1)!\n");
      mixerClose((HMIXER)hmxobj);
      return -1;
    }

    if ( strstr(pmxl.szName, "Mic") )
  	{
  		break;
  	}
  }
  if(i == numConnections)
  {
		Com_PrintWarning(CON_CHANNEL_SOUND, "mixerGetRecordSource: No mic found\n");
  		mixerClose((HMIXER)hmxobj);
		return -1;
  }
  pmxlc.dwLineID = pmxl.dwLineID;
  pmxlc.cbStruct = 24;
  pmxlc.dwControlID = 0x50030001u;
  pmxlc.cControls = 1;
  pmxlc.cbmxctrl = 148;
  pmxlc.pamxctrl = &mixerControlObj;
  if ( mixerGetLineControlsA(hmxobj, &pmxlc, 2u) )
  {
    mixerClose((HMIXER)hmxobj);
    return -1;
  }
  pmxcd.hwndOwner = 0;
  pmxcd.paDetails = &paDetails;
  pmxcd.cbStruct = 24;
  pmxcd.cChannels = 1;
  pmxcd.cbDetails = 4;
  pmxcd.dwControlID = mixerControlObj.dwControlID;
  mixerGetControlDetailsA(hmxobj, &pmxcd, 0);
  mixerClose((HMIXER)hmxobj);
  return paDetails;
}

qboolean SND_IsInitialized()
{
	return g_voice_initialized;
}


void Voice_GetLocalVoiceData()
{
	static qboolean failed = qfalse;

  if ( !*(byte*)0xD5EC4F9 && !failed)
  {
    *(voiceInit_t **)0xD5EC50C = Voice_CreateChannel();
    if(*(voiceInit_t **)0xD5EC50C == NULL)
	{
		failed = qtrue;
		return;
	}
	sub_4ED610(*(int*)0xD5EC50C);
    (*(int*)0xD8B301C)++;
    *(byte*)0xD5EC4F9 = 1;
  }
  if ( winvoice_mic_scaler->value != *(float*)0x71FB08 )
  {
    *(float*)0x71FB08 = winvoice_mic_scaler->value;
    if ( *(float*)0x71FB08 >= 0.5 )
    {
      if ( *(float*)0x71FB08 > 1.5 )
        *(float*)0x71FB08 = 1.5;
    }
    else
    {
      *(float*)0x71FB08 = 0.5;
    }
  }
  if ( captureDeviceInit && captureDeviceInitB && *(int*)0xD5EC45C )
  {
    sub_4ED380();
  }
}

void SND_DisconnectListener()
{
    memset(&sndsystem.listenerSndObj, 0, sizeof(sndsystem.listenerSndObj));
}



void SND_StopAmbient(int fadetime)
{
	int i;

	if ( sndsystem.field_0 )
	{
		for(i = 0; i < 4; ++i)
		{
			SND_StopBackground(i+1, fadetime);
		}
	}
}

void SND_InitDirectSoundHandle(HWND hWnd)
{
    if ( sndsystem.field_0 )
	{
		mss.AIL_set_DirectSound_HWND_int(*(int*)0xcc120e8, hWnd);
	}
}

qboolean Voice_IsClientTalking(int num)
{
  return Sys_Milliseconds() - s_clientTalkTime[num] <= 299;
}
