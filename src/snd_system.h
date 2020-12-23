#ifndef __SND_SYSTEM_H__
#define __SND_SYSTEM_H__




void SND_LoadEntChannelFile();
void SND_SetEnvironmentEffects_f();
void SND_DeactivateEnvironmentEffects_f();
void SND_PlayLocal_f();
void SND_SetEq_f();
void SND_SetEqFreq_f();
void SND_SetEqGain_f();
void SND_SetEqQ_f();
void SND_SetEqType_f();
void SND_DeactivateEq_f();
void Voice_Init( );
qboolean Voice_IsClientTalking(int num);
void sub_57B350( );
void sub_57B5C0( const char* );
int sub_57B070( );
int __cdecl sub_57B1D0(uint16_t a1);

void sub_57B860( qboolean );
void sub_4ED0B0( HWND handle );
void sub_4ECA60( );
void sub_4EC880( );

qboolean __cdecl SND_InitDSCaptureDevice();
DWORD mixerGetRecordSource();
qboolean SND_IsInitialized();
void Voice_GetLocalVoiceData();
void sub_4ED380();
void sub_4ED610(int);
void SND_DisconnectListener();
qboolean IsSelectedVoiceChannelValid(unsigned int channel);
void Voice_IncomingVoiceData(int clnum, byte* voicedata, int size);
int SND_PlayLocalSoundAliasByName(int localClientNum, const char *alias);
void SND_StopBackground(int track, int fadetime);
void SND_StopAmbient(int fadetime);
void SND_UnpauseSounds();
void SND_PauseSounds();
void SND_InitDirectSoundHandle(HWND hWnd);

#endif