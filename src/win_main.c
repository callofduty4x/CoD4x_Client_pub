#include "q_shared.h"
#include "qcommon.h"
#include "win_sys.h"
#include "client.h"
#include "crc.h"
#include "crashrpt/TinyCrashRpt.h"

#include <stdlib.h>
#include <Mmsystem.h>
#include <windows.h>
#include <WinBase.h>
#include <direct.h>
#include <Shellapi.h>
#include <shlobj.h>
#include <psapi.h>
#include <stdbool.h>

#define splashScreen_HWND *((HWND*)(splashScreen_HWND_ADDR))
#define sys_cmdline ((char*)(sys_cmdline_ADDR))
#define sys_timeBase *((int*)(sys_timeBase_ADDR))
#define sys_timeInitialized *((qboolean*)(sys_timeInitialized_ADDR))
#define sys_processSemaphoreFile (char*)(0xCC15FE4)
#define localization (*((localization_t*)(0xCC147D4)))

void Sys_SetMiniDumpStarted();

DWORD *threadHandles = (DWORD *)0x14E89A8; //array of 12

const char *__cdecl Sys_GetCurrentThreadName()
{
	return "Unknown"; //Help
  //return s_threadNames[Sys_GetThreadContext()];
}

void Sys_EventLoop(){
	MSG msg;

	// pump the message loop
	while ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) {
		if ( !GetMessage( &msg, NULL, 0, 0 ) ) {
			Com_Quit_f();
		}
		// save the msg time, because wndprocs don't have access to the timestamp
		g_wv.sysMsgTime = msg.time;

		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

}

void Sys_GetLastErrorAsString(char* errbuf, int len)
{
	int lastError = GetLastError();
	if(lastError != 0)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPSTR)errbuf, len -1, NULL);
	}else{
		Q_strncpyz(errbuf, "Unknown Error", len);
	}
}

#define MAX_XCRITICAL_SECTIONS 3

LPCRITICAL_SECTION sys_criticalSection = (LPCRITICAL_SECTION)(sys_criticalSections_ADDR);
CRITICAL_SECTION sys_xCriticalSection[MAX_XCRITICAL_SECTIONS];
void Sys_InitializeCriticalSections()
{
	int i;
	for(i = 0; i < 20; i++)
	{
		InitializeCriticalSection(&sys_criticalSection[i]);
	}
	for(i = 0; i < MAX_XCRITICAL_SECTIONS; i++)
	{
		InitializeCriticalSection(&sys_xCriticalSection[i]);
	}
}

void Sys_EnterCriticalSection(int section)
{
	if(section < 20)
		EnterCriticalSection(&sys_criticalSection[section]);
	else
		EnterCriticalSection(&sys_xCriticalSection[section -20]);
}

void Sys_LeaveCriticalSection(int section)
{
	if(section < 20)
		LeaveCriticalSection(&sys_criticalSection[section]);
	else
		LeaveCriticalSection(&sys_xCriticalSection[section -20]);
}


static qboolean mainthread_lock;
static HANDLE mainThreadSuspended;
static HMODULE dllHandle;

/* Called once on initialization */
qboolean Sys_InitializeGlobalCriticalSection()
{
	mainThreadSuspended = CreateEvent(NULL,FALSE,FALSE,"MainThreadSuspendingOnBeginFrame");
	if(mainThreadSuspended)
	{
		return qtrue;
	}
	return qfalse;
}

/* Called by mainthread just before the usual frame begins */
void Sys_MainThreadCheckForGlobalCriticalSection()
{

	if(!mainthread_lock)
	{
		return;
	}
	SetEvent(mainThreadSuspended);
	WaitForSingleObject(mainThreadSuspended, INFINITE);

}

/* Called by every other thread which wants to do thread unsafe operations the mainthread can usually do only */
/* Never call it twice without prior leaving */
void Sys_EnterGlobalCriticalSection()
{

	Sys_EnterCriticalSection(CRIT_WAITFORMAINTHREADSUSPENDED);
	if(mainthread_lock)
	{
		Com_Error(ERR_FATAL, "Sys_EnterCriticalSectionGlobal called twice without leaving");
	}
	mainthread_lock = qtrue;
	WaitForSingleObject(mainThreadSuspended, INFINITE);
}

void Sys_LeaveGlobalCriticalSection()
{
	mainthread_lock = qfalse;
	SetEvent(mainThreadSuspended);
	Sys_LeaveCriticalSection(CRIT_WAITFORMAINTHREADSUSPENDED);

}


qboolean Sys_CreateNewThread(void* (*ThreadMain)(void*), threadid_t *tid, void* arg)
{
	char errMessageBuf[512];
	DWORD lastError;
	HANDLE thid = CreateThread(	NULL, // LPSECURITY_ATTRIBUTES lpsa,
								0, // DWORD cbStack,
								(LPTHREAD_START_ROUTINE)ThreadMain, // LPTHREAD_START_ROUTINE lpStartAddr,
								arg, // LPVOID lpvThreadParm,
								0, // DWORD fdwCreate,
								tid );
	if(thid == NULL)
	{
		lastError = GetLastError();
		if(lastError != 0)
		{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPSTR)errMessageBuf, sizeof(errMessageBuf) -1, NULL);
			Com_PrintError(CON_CHANNEL_SYSTEM, "Failed to start thread with error: %s\n", errMessageBuf);
		}else{
			Com_PrintError(CON_CHANNEL_SYSTEM, "Failed to start thread!\n");
		}
		return qfalse;
	}
	CloseHandle(thid);
	return qtrue;
}



void preInitError(const char* error){
	MessageBoxA( 0, error, "Call of Duty 4 - Modern Warfare - Fatal Error", 0x10 );
	exit(1);
}

byte Sys_IsMainThread(){

	if(threadHandles[0] == GetCurrentThreadId()){
		return qtrue;
	}
	return qfalse;
}

typedef int (__stdcall *MSGBOXAAPI)(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

int MessageBoxTimeoutA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds)
{
    static MSGBOXAAPI MsgBoxTOA = NULL;

    if (!MsgBoxTOA)
    {
        HMODULE hUser32 = GetModuleHandleA("user32.dll");
        if (hUser32)
        {
            MsgBoxTOA = (MSGBOXAAPI)GetProcAddress(hUser32, "MessageBoxTimeoutA");
            //fall through to 'if (MsgBoxTOA)...'
        }
        else
        {
            //stuff happened, add code to handle it here
            return MessageBoxA(hWnd, lpText, lpCaption, uType);
        }
    }

    if (MsgBoxTOA)
    {
        return MsgBoxTOA(hWnd, lpText, lpCaption,
              uType, wLanguageId, dwMilliseconds);
    }

    return 0;
}


/*
================
Sys_Milliseconds
================
*/

int Sys_Milliseconds(void){
	int sys_curtime;

	if( !sys_timeInitialized){
		sys_timeBase = timeGetTime();
		sys_timeInitialized = qtrue;
	}
	sys_curtime = timeGetTime() - sys_timeBase;
	return sys_curtime;
}


/*
================
Sys_RandomBytes
================
*/
qboolean Sys_RandomBytes( byte *string, int len )
{
	HCRYPTPROV  prov;

	if( !CryptAcquireContext( &prov, NULL, NULL,
		PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )  {

		return qfalse;
	}

	if( !CryptGenRandom( prov, len, (BYTE *)string ) )  {
		CryptReleaseContext( prov, 0 );
		return qfalse;
	}
	CryptReleaseContext( prov, 0 );
	return qtrue;
}

void Sys_CreateMainWindowClass(){
/*
		WNDCLASS wc;

		memset( &wc, 0, sizeof( wc ) );

		wc.style         = 0;
		wc.lpfnWndProc   = MainWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = g_wv.hInstance;
		wc.hIcon         = LoadIconA( g_wv.hInstance, MAKEINTRESOURCE( IDI_ICON1 ) );
		wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
		//wc.hbrBackground = (void *)COLOR_GRAYTEXT;
		wc.lpszMenuName  = 0;
		wc.lpszClassName = WINDOW_CLASS_NAME;

		if ( !RegisterClass( &wc ) )
			Com_Error( ERR_FATAL, "EXE_ERR_COULDNT_REGISTER_WINDOW" );
	*/


	WNDCLASSEXA wndClass;

	memset(&wndClass, 0, sizeof( wndClass ));
	wndClass.cbSize = sizeof( wndClass );
	wndClass.lpfnWndProc = (WNDPROC)MainWndProc;
	wndClass.hInstance = g_wv.hInstance;
	wndClass.hIcon = LoadIconA(g_wv.hInstance, (LPCSTR)1);
	wndClass.hCursor = LoadCursorA(0, (LPCSTR)0x7F00);
	wndClass.hbrBackground = CreateSolidBrush(0);
	wndClass.lpszClassName = "CoD4";
	if ( !RegisterClassExA(&wndClass) )
	{
		Com_Error(0, "EXE_ERR_COULDNT_REGISTER_WINDOW");
	}
}

char* mss32importnames[] = {
	"AIL_debug_printf",
	"AIL_sprintf",
	"DLSClose",
	"DLSCompactMemory",
	"DLSGetInfo",
	"DLSLoadFile",
	"DLSLoadMemFile",
	"DLSMSSOpen",
	"DLSSetAttribute",
	"DLSUnloadAll",
	"DLSUnloadFile",
	"RIB_alloc_provider_handle",
	"RIB_enumerate_interface",
	"RIB_error",
	"RIB_find_file_provider",
	"RIB_free_provider_handle",
	"RIB_free_provider_library",
	"RIB_load_provider_library",
	"RIB_register_interface",
	"RIB_request_interface",
	"RIB_request_interface_entry",
	"RIB_type_string",
	"RIB_unregister_interface",
	"_AIL_3D_distance_factor@4",
	"_AIL_3D_doppler_factor@4",
	"_AIL_3D_rolloff_factor@4",
	"_AIL_DLS_close@8",
	"_AIL_DLS_compact@4",
	"_AIL_DLS_get_info@12",
	"_AIL_DLS_load_file@12",
	"_AIL_DLS_load_memory@12",
	"_AIL_DLS_open@28",
	"_AIL_DLS_sample_handle@4",
	"_AIL_DLS_unload@8",
	"_AIL_HWND@0",
	"_AIL_MIDI_handle_reacquire@4",
	"_AIL_MIDI_handle_release@4",
	"_AIL_MIDI_to_XMI@20",
	"_AIL_MMX_available@0",
	"_AIL_WAV_file_write@20",
	"_AIL_WAV_info@8",
	"_AIL_XMIDI_master_volume@4",
	"_AIL_active_sample_count@4",
	"_AIL_active_sequence_count@4",
	"_AIL_allocate_sample_handle@4",
	"_AIL_allocate_sequence_handle@4",
	"_AIL_auto_service_stream@8",
	"_AIL_background@0",
	"_AIL_background_CPU_percent@0",
	"_AIL_branch_index@8",
	"_AIL_calculate_3D_channel_levels@56",
	"_AIL_channel_notes@8",
	"_AIL_close_XMIDI_driver@4",
	"_AIL_close_digital_driver@4",
	"_AIL_close_filter@4",
	"_AIL_close_input@4",
	"_AIL_close_stream@4",
	"_AIL_compress_ADPCM@12",
	"_AIL_compress_ASI@20",
	"_AIL_compress_DLS@20",
	"_AIL_controller_value@12",
	"_AIL_create_wave_synthesizer@16",
	"_AIL_decompress_ADPCM@12",
	"_AIL_decompress_ASI@24",
	"_AIL_delay@4",
	"_AIL_destroy_wave_synthesizer@4",
	"_AIL_digital_CPU_percent@4",
	"_AIL_digital_configuration@16",
	"_AIL_digital_driver_processor@8",
	"_AIL_digital_handle_reacquire@4",
	"_AIL_digital_handle_release@4",
	"_AIL_digital_latency@4",
	"_AIL_digital_master_reverb@16",
	"_AIL_digital_master_reverb_levels@12",
	"_AIL_digital_master_volume_level@4",
	"_AIL_digital_output_filter@4",
	"_AIL_end_sample@4",
	"_AIL_end_sequence@4",
	"_AIL_enumerate_MP3_frames@4",
	"_AIL_enumerate_filter_properties@12",
	"_AIL_enumerate_filter_sample_properties@12",
	"_AIL_enumerate_filters@12",
	"_AIL_enumerate_output_filter_driver_properties@12",
	"_AIL_enumerate_output_filter_sample_properties@12",
	"_AIL_enumerate_sample_stage_properties@16",
	"_AIL_extract_DLS@28",
	"_AIL_file_error@0",
	"_AIL_file_read@8",
	"_AIL_file_size@4",
	"_AIL_file_type@8",
	"_AIL_file_type_named@12",
	"_AIL_file_write@12",
	"_AIL_filter_DLS_with_XMI@24",
	"_AIL_filter_property@20",
	"_AIL_find_DLS@24",
	"_AIL_find_filter@8",
	"_AIL_ftoa@4",
	"_AIL_get_DirectSound_info@12",
	"_AIL_get_input_info@4",
	"_AIL_get_preference@4",
	"_AIL_get_timer_highest_delay@0",
	"_AIL_init_sample@12",
	"_AIL_init_sequence@12",
	"_AIL_inspect_MP3@12",
	"_AIL_last_error@0",
	"_AIL_list_DLS@20",
	"_AIL_list_MIDI@20",
	"_AIL_listener_3D_orientation@28",
	"_AIL_listener_3D_position@16",
	"_AIL_listener_3D_velocity@16",
	"_AIL_listener_relative_receiver_array@8",
	"_AIL_load_sample_attributes@8",
	"_AIL_load_sample_buffer@16",
	"_AIL_lock@0",
	"_AIL_lock_channel@4",
	"_AIL_lock_mutex@0",
	"_AIL_map_sequence_channel@12",
	"_AIL_mem_alloc_lock@4",
	"_AIL_mem_free_lock@4",
	"_AIL_mem_use_free@4",
	"_AIL_mem_use_malloc@4",
	"_AIL_merge_DLS_with_XMI@16",
	"_AIL_midiOutClose@4",
	"_AIL_midiOutOpen@12",
	"_AIL_minimum_sample_buffer_size@12",
	"_AIL_ms_count@0",
	"_AIL_open_XMIDI_driver@4",
	"_AIL_open_digital_driver@16",
	"_AIL_open_filter@8",
	"_AIL_open_input@4",
	"_AIL_open_stream@12",
	"_AIL_output_filter_driver_property@20",
	"_AIL_pause_stream@8",
	"_AIL_platform_property@20",
	"_AIL_primary_digital_driver@4",
	"_AIL_process_digital_audio@24",
	"_AIL_quick_copy@4",
	"_AIL_quick_halt@4",
	"_AIL_quick_handles@12",
	"_AIL_quick_load@4",
	"_AIL_quick_load_and_play@12",
	"_AIL_quick_load_mem@8",
	"_AIL_quick_load_named_mem@12",
	"_AIL_quick_ms_length@4",
	"_AIL_quick_ms_position@4",
	"_AIL_quick_play@8",
	"_AIL_quick_set_low_pass_cut_off@8",
	"_AIL_quick_set_ms_position@8",
	"_AIL_quick_set_reverb_levels@12",
	"_AIL_quick_set_speed@8",
	"_AIL_quick_set_volume@12",
	"_AIL_quick_shutdown@0",
	"_AIL_quick_startup@20",
	"_AIL_quick_status@4",
	"_AIL_quick_type@4",
	"_AIL_quick_unload@4",
	"_AIL_redbook_close@4",
	"_AIL_redbook_eject@4",
	"_AIL_redbook_id@4",
	"_AIL_redbook_open@4",
	"_AIL_redbook_open_drive@4",
	"_AIL_redbook_pause@4",
	"_AIL_redbook_play@12",
	"_AIL_redbook_position@4",
	"_AIL_redbook_resume@4",
	"_AIL_redbook_retract@4",
	"_AIL_redbook_set_volume_level@8",
	"_AIL_redbook_status@4",
	"_AIL_redbook_stop@4",
	"_AIL_redbook_track@4",
	"_AIL_redbook_track_info@16",
	"_AIL_redbook_tracks@4",
	"_AIL_redbook_volume_level@4",
	"_AIL_register_EOB_callback@8",
	"_AIL_register_EOS_callback@8",
	"_AIL_register_ICA_array@8",
	"_AIL_register_SOB_callback@8",
	"_AIL_register_beat_callback@8",
	"_AIL_register_event_callback@8",
	"_AIL_register_falloff_function_callback@8",
	"_AIL_register_prefix_callback@8",
	"_AIL_register_sequence_callback@8",
	"_AIL_register_stream_callback@8",
	"_AIL_register_timbre_callback@8",
	"_AIL_register_timer@4",
	"_AIL_register_trace_callback@8",
	"_AIL_register_trigger_callback@8",
	"_AIL_release_all_timers@0",
	"_AIL_release_channel@8",
	"_AIL_release_sample_handle@4",
	"_AIL_release_sequence_handle@4",
	"_AIL_release_timer_handle@4",
	"_AIL_request_EOB_ASI_reset@12",
	"_AIL_resume_sample@4",
	"_AIL_resume_sequence@4",
	"_AIL_room_type@4",
	"_AIL_sample_3D_cone@16",
	"_AIL_sample_3D_distances@16",
	"_AIL_sample_3D_orientation@28",
	"_AIL_sample_3D_position@16",
	"_AIL_sample_3D_velocity@16",
	"_AIL_sample_51_volume_levels@28",
	"_AIL_sample_51_volume_pan@24",
	"_AIL_sample_buffer_info@20",
	"_AIL_sample_buffer_ready@4",
	"_AIL_sample_channel_levels@8",
	"_AIL_sample_exclusion@4",
	"_AIL_sample_granularity@4",
	"_AIL_sample_loop_block@12",
	"_AIL_sample_loop_count@4",
	"_AIL_sample_low_pass_cut_off@4",
	"_AIL_sample_ms_position@12",
	"_AIL_sample_obstruction@4",
	"_AIL_sample_occlusion@4",
	"_AIL_sample_playback_rate@4",
	"_AIL_sample_position@4",
	"_AIL_sample_processor@8",
	"_AIL_sample_reverb_levels@12",
	"_AIL_sample_stage_property@24",
	"_AIL_sample_status@4",
	"_AIL_sample_user_data@8",
	"_AIL_sample_volume_levels@12",
	"_AIL_sample_volume_pan@12",
	"_AIL_save_sample_attributes@8",
	"_AIL_send_channel_voice_message@20",
	"_AIL_send_sysex_message@8",
	"_AIL_sequence_loop_count@4",
	"_AIL_sequence_ms_position@12",
	"_AIL_sequence_position@12",
	"_AIL_sequence_status@4",
	"_AIL_sequence_tempo@4",
	"_AIL_sequence_user_data@8",
	"_AIL_sequence_volume@4",
	"_AIL_serve@0",
	"_AIL_service_stream@8",
	"_AIL_set_3D_distance_factor@8",
	"_AIL_set_3D_doppler_factor@8",
	"_AIL_set_3D_rolloff_factor@8",
	"_AIL_set_DirectSound_HWND@8",
	"_AIL_set_XMIDI_master_volume@8",
	"_AIL_set_digital_driver_processor@12",
	"_AIL_set_digital_master_reverb@16",
	"_AIL_set_digital_master_reverb_levels@12",
	"_AIL_set_digital_master_volume_level@8",
	"_AIL_set_error@4",
	"_AIL_set_file_async_callbacks@20",
	"_AIL_set_file_callbacks@16",
	"_AIL_set_input_state@8",
	"_AIL_set_listener_3D_orientation@28",
	"_AIL_set_listener_3D_position@16",
	"_AIL_set_listener_3D_velocity@20",
	"_AIL_set_listener_3D_velocity_vector@16",
	"_AIL_set_listener_relative_receiver_array@12",
	"_AIL_set_named_sample_file@20",
	"_AIL_set_preference@8",
	"_AIL_set_redist_directory@4",
	"_AIL_set_room_type@8",
	"_AIL_set_sample_3D_cone@16",
	"_AIL_set_sample_3D_distances@16",
	"_AIL_set_sample_3D_orientation@28",
	"_AIL_set_sample_3D_position@16",
	"_AIL_set_sample_3D_velocity@20",
	"_AIL_set_sample_3D_velocity_vector@16",
	"_AIL_set_sample_51_volume_levels@28",
	"_AIL_set_sample_51_volume_pan@24",
	"_AIL_set_sample_address@12",
	"_AIL_set_sample_adpcm_block_size@8",
	"_AIL_set_sample_channel_levels@12",
	"_AIL_set_sample_exclusion@8",
	"_AIL_set_sample_file@12",
	"_AIL_set_sample_info@8",
	"_AIL_set_sample_loop_block@12",
	"_AIL_set_sample_loop_count@8",
	"_AIL_set_sample_low_pass_cut_off@8",
	"_AIL_set_sample_ms_position@8",
	"_AIL_set_sample_obstruction@8",
	"_AIL_set_sample_occlusion@8",
	"_AIL_set_sample_playback_rate@8",
	"_AIL_set_sample_position@8",
	"_AIL_set_sample_processor@12",
	"_AIL_set_sample_reverb_levels@12",
	"_AIL_set_sample_user_data@12",
	"_AIL_set_sample_volume_levels@12",
	"_AIL_set_sample_volume_pan@12",
	"_AIL_set_sequence_loop_count@8",
	"_AIL_set_sequence_ms_position@8",
	"_AIL_set_sequence_tempo@12",
	"_AIL_set_sequence_user_data@12",
	"_AIL_set_sequence_volume@12",
	"_AIL_set_speaker_configuration@16",
	"_AIL_set_speaker_reverb_levels@16",
	"_AIL_set_stream_loop_block@12",
	"_AIL_set_stream_loop_count@8",
	"_AIL_set_stream_ms_position@8",
	"_AIL_set_stream_position@8",
	"_AIL_set_stream_user_data@12",
	"_AIL_set_timer_divisor@8",
	"_AIL_set_timer_frequency@8",
	"_AIL_set_timer_period@8",
	"_AIL_set_timer_user@8",
	"_AIL_shutdown@0",
	"_AIL_size_processed_digital_audio@16",
	"_AIL_speaker_configuration@20",
	"_AIL_speaker_reverb_levels@16",
	"_AIL_start_all_timers@0",
	"_AIL_start_sample@4",
	"_AIL_start_sequence@4",
	"_AIL_start_stream@4",
	"_AIL_start_timer@4",
	"_AIL_startup@0",
	"_AIL_stop_all_timers@0",
	"_AIL_stop_sample@4",
	"_AIL_stop_sequence@4",
	"_AIL_stop_timer@4",
	"_AIL_stream_info@20",
	"_AIL_stream_loop_count@4",
	"_AIL_stream_ms_position@12",
	"_AIL_stream_position@4",
	"_AIL_stream_sample_handle@4",
	"_AIL_stream_status@4",
	"_AIL_stream_user_data@8",
	"_AIL_true_sequence_channel@8",
	"_AIL_unlock@0",
	"_AIL_unlock_mutex@0",
	"_AIL_update_listener_3D_position@8",
	"_AIL_update_sample_3D_position@8",
	"_AIL_us_count@0",
	"_DLSMSSGetCPU@4",
	"_MIX_RIB_MAIN@8",
	"_MSSDisableThreadLibraryCalls@4",
	"_RIB_enumerate_providers@12",
	"_RIB_find_file_dec_provider@20",
	"_RIB_find_files_provider@20",
	"_RIB_find_provider@12",
	"_RIB_load_application_providers@4",
	"_RIB_load_static_provider_library@8",
	"_RIB_provider_system_data@8",
	"_RIB_provider_user_data@8",
	"_RIB_set_provider_system_data@12",
	"_RIB_set_provider_user_data@12"
};

void* mss32importprocs[AIL_TOP_COUNT];
static qboolean sys_tempinstall;

void Sys_LoadModules(HINSTANCE hInstance){

	HINSTANCE base;
	FARPROC proc;
	int i, copylen;
	char moduledir[1024];
	char mss32path[1024];
	char miles32path[1024];
	static qboolean loaded = qfalse;
	char* find;

	if(loaded == qtrue)
		return;

	copylen = GetModuleFileNameA(hInstance, moduledir, sizeof(moduledir));

	if (copylen < 1 || strrchr(moduledir, '\\') == NULL)
	{
		Q_strncpyz(miles32path, "miles32.dll", sizeof(miles32path));
		Q_strncpyz(mss32path, "mss32.dll", sizeof(mss32path));
	} else {
		find = strrchr(moduledir, '\\');
		*find = '\0';
		Com_sprintf(miles32path, sizeof(miles32path), "%s\\miles32.dll", moduledir);
		Com_sprintf(mss32path, sizeof(mss32path), "%s\\mss32.dll", moduledir);
	}


	if (sys_tempinstall)
		base = LoadLibraryA(mss32path);
	else
		base = LoadLibraryA(miles32path);

	if(!base){
		preInitError("Error loading module mss32.dll\n");
	}
	proc = GetProcAddress(base, "_AIL_set_DirectSound_HWND@8");

	if(!proc)
		preInitError("No entry point for procedure _AIL_set_DirectSound_HWND\n");
	else{
		mss.AIL_set_DirectSound_HWND_int = (void*)proc;
	}

	for(i = 0; i < AIL_TOP_COUNT; i++)
	{
		proc = GetProcAddress(base, mss32importnames[i]);
		if(!proc)
			preInitError(va("No entry point for procedure: %s\n", mss32importnames[i]));
		else
			mss32importprocs[i] = proc;
	}
}


//================================================================

static char exeFilename[ MAX_STRING_CHARS ] = { 0 };
static wchar_t dllFilename[ MAX_STRING_CHARS ] = { 0 };

/*
=================
Sys_SetExeFile
=================
*/
void Sys_SetExeFile(const char *filepath)
{
	Q_strncpyz(exeFilename, filepath, sizeof(exeFilename));
}

/*
=================
Sys_SetDllFile
=================
*/
void Sys_SetDllFile(const wchar_t *filepath)
{
	Q_strncpyzUni(dllFilename, filepath, sizeof(dllFilename));
}

/*
=================
Sys_ExeFile
=================
*/
const char* Sys_ExeFile( void )
{
	return exeFilename;
}

/*
=================
Sys_DllFile
=================
*/
const wchar_t* Sys_DllFile( void )
{
	return dllFilename;
}


/*
=================
Sys_Print
=================
*/
void Sys_Print( const char *msg )
{
	if(Sys_IsMainThread())
		Conbuf_AppendText( msg );

}



/*
=================
Sys_In_Restart_f

Restart the input subsystem
=================
*/
void Sys_In_Restart_f( void ) {
	IN_Shutdown();
	IN_Init();
}


/*
=================
Sys_Net_Restart_f

Restart the network subsystem
=================
*/
void Sys_Net_Restart_f( void ) {
	NET_Restart_f();
}

/*
================
Sys_GetCurrentUser
================
*/
const char *Sys_GetCurrentUser( void )
{

	static char s_userName[1024];
	unsigned long size = sizeof( s_userName );

	if( !GetUserName( s_userName, &size ) || !s_userName[0] )
	{
		Q_strncpyz( s_userName, "Unknown Player", sizeof(s_userName) );
	}

	return s_userName;
}

int Sys_TimeGetTime()
{
	return timeGetTime();
}

/*
================
Sys_Init

Called after the common systems (cvars, files, etc)
are initialized
================
*/

void Sys_Init( void ) {
	OSVERSIONINFOA osversion;
	// make sure the timer is high precision, otherwise
	// NT gets 18ms resolution
	timeBeginPeriod( 1 );

	Cmd_AddCommand( "in_restart", Sys_In_Restart_f );
	osversion.dwOSVersionInfoSize = sizeof( osversion );

	if ( !GetVersionEx( &osversion ) ) {
		Sys_Error( "Couldn't get OS info" );
	}

	if ( osversion.dwMajorVersion < 4 ) {
		Sys_Error( va("%s requires Windows version 4 or greater", LONG_PRODUCT_NAME));
	}
	if ( osversion.dwPlatformId == VER_PLATFORM_WIN32s ) {
		Sys_Error( va("%s doesn't run on Win32s", LONG_PRODUCT_NAME));
	}
	//
	// figure out our CPU
	//
	Com_Printf(CON_CHANNEL_SYSTEM, "CPU is: %s\n", Cvar_GetVariantString("sys_cpuName"));
	Com_Printf(CON_CHANNEL_SYSTEM, "Measured CPU speed is %s GHz\n", Cvar_GetVariantString("sys_cpuGHz"));
	Com_Printf(CON_CHANNEL_SYSTEM, "Total CPU performance is estimated as %s\n", Cvar_GetVariantString("sys_configureGHz"));
	Com_Printf(CON_CHANNEL_SYSTEM, "System memory is %s MB (capped at 1 GB)\n", Cvar_GetVariantString("sys_sysMB"));
	Com_Printf(CON_CHANNEL_SYSTEM, "Video card is \"%s\"\n", Cvar_GetVariantString("sys_gpu"));
	Com_Printf(CON_CHANNEL_SYSTEM, "Streaming SIMD Extensions (SSE) support: %s\n", Cvar_GetVariantString("sys_SSE"));

	Cvar_RegisterString( "username", Sys_GetCurrentUser(), CVAR_ROM, "The current username" );
}

char sys_cwd[1024];

void Sys_Cwd(char* path, int len)
{
	Q_strncpyz(path, sys_cwd, len);
}

void Sys_SetupCwd(  )
{
	//char path[1024];
	//char filepath[1024];
	char* cut;

	/* DLL planting vulnerable...
	_getcwd(path, sizeof(path));
	// Test for file: path\main\iw_00.iwd
	FS_BuildOSPathForThread(path, "main", "iw_00.iwd", filepath, 0);
	if(FS_FileExistsOSPath( filepath ))
	{
		Q_strncpyz(sys_cwd, path, sizeof(sys_cwd));
		return;
	}
	*/

	Q_strncpyz(sys_cwd, Sys_ExeFile(), sizeof(sys_cwd));
	cut = strrchr(sys_cwd, PATH_SEP);
	if(cut != NULL)
	{
		*cut = '\0';
		SetCurrentDirectory(sys_cwd);
	}
}

typedef struct
{
	const char* language;
	const char* translations;
}localization_t;


void Sys_InitLocalization()
{
  /* Function does depend on Sys_SetupCwd() */
  char* buf, *split;
  static char language[32];
  char path[1024];
  char cwd[1024];

  localization.language = NULL;
  localization.translations = NULL;

  Sys_Cwd(cwd, sizeof(cwd));

  Com_sprintf(path, sizeof(path), "%s%c%s", cwd, PATH_SEP, "localization.txt");

  if(FS_ReadFileOSPath( path, (void**)&buf ) < 1)
  {
	localization.language = NULL;
	localization.translations = NULL;
	return;
  }
  Q_strncpyz(language, buf, sizeof(language));
  FS_FreeFile(buf);
  localization.language = language;

  split = strchr(language, '\n');
  if(split != NULL)
  {
	*split = '\0';
	localization.translations = split +1;
  }

  split = strchr(language, '\r');
  if(split != NULL)
  {
	*split = '\0';
  }
}

/* Needs 256 wchar_t characters */
const wchar_t* Sys_DllPath(wchar_t* path)
{
	char protocolstring[128];
	Com_sprintf(protocolstring, sizeof(protocolstring), "cod4x_%03u", PROTOCOL_VERSION);
	FS_BuildOSPathForThreadUni(FS_GetSavePath(), "bin", protocolstring, path, 0);
	return path;
}



char UPDATE_VERSION[64];
char sys_restartCmdLine[4096];
qboolean AutoupdateRequiresElevatedPermissions;
void Sys_RestartProcessOnExit( );
qboolean Sys_DetectNvidiaD3d9Wrap();

void Sys_MitigateNvidiaD3d9Wrap();
void CleanKernel32Funcs();

void Sys_SetRestartParams(const char* params)
{
	Q_strncpyz(sys_restartCmdLine, params, sizeof(sys_restartCmdLine));
}
//=======================================================================

void Sys_CleanUpNvd3d9Wrap()
{
	static qboolean nvd3d9wrapcleanedup = 0;

	if(nvd3d9wrapcleanedup == qfalse && Sys_DetectNvidiaD3d9Wrap())
	{
		//MessageBoxA(NULL, "NVD3D9Wrap detected", "NVD3D9Wrap detected", MB_OK);
		Sys_MitigateNvidiaD3d9Wrap();
		nvd3d9wrapcleanedup = qtrue;
	}
}


void Sys_FixUpThirdPartyModules()
{
	//Sys_CleanUpNvd3d9Wrap();
}

void Sys_SetupCrashReporter()
{
	CR_INSTALL_INFOA info;
	wchar_t crashrptdllpath[1024];
	wchar_t dllpath[1024];
	wchar_t displayMessageBuf[4096];
	wchar_t errorMessageBuf[4096];
	int WINAPI (*crInstallAImp)(PCR_INSTALL_INFOA pInfo);
	int WINAPI (*crGetLastErrorMsgAImp)(LPSTR pszBuffer, UINT uBuffSize);

	Sys_DllPath(dllpath);
	Com_sprintfUni(crashrptdllpath, sizeof(crashrptdllpath), L"%s\\crashrpt1403.dll", dllpath);
	HMODULE hCrashRpt = LoadLibraryExW(crashrptdllpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if(hCrashRpt)
	{
		crInstallAImp = GetProcAddress(hCrashRpt, "crInstallA");
		crGetLastErrorMsgAImp = GetProcAddress(hCrashRpt, "crGetLastErrorMsgA");
	}else{
		crInstallAImp = NULL;
		crGetLastErrorMsgAImp = NULL;
	}
	Com_Memset(&info, 0, sizeof(info));
	info.cb = sizeof(info);
	info.pszAppName = "Call of Duty 4 X - Modern Warfare (not affiliated with Activision)";
	info.pszAppVersion = UPDATE_VERSION_NUM;
	info.pszUrl = "https://cod4x.me/crashrpt/crashrpt.php";
	info.uPriorities[CR_HTTP] = 1;
	info.dwFlags = CR_INST_SEH_EXCEPTION_HANDLER | CR_INST_TERMINATE_HANDLER |
					CR_INST_UNEXPECTED_HANDLER | CR_INST_INVALID_PARAMETER_HANDLER |
					CR_INST_SIGABRT_HANDLER | CR_INST_SIGFPE_HANDLER |
					CR_INST_SIGILL_HANDLER | CR_INST_SIGSEGV_HANDLER |
					CR_INST_ALLOW_ATTACH_MORE_FILES | CR_INST_AUTO_THREAD_HANDLERS;

	info.pszRestartCmdLine = "";

	if(crInstallAImp == NULL || crInstallAImp(&info) != 0)
	{

		if(crGetLastErrorMsgAImp)
		{
			char szErrorMsg[512] = "";
			crGetLastErrorMsgAImp(szErrorMsg, 512);
			MessageBoxA(NULL, szErrorMsg, "Error loading crash reporter API", MB_OK);
		}else{
			FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				0, GetLastError(), 0x400, errorMessageBuf, sizeof(errorMessageBuf) /2, 0);

			Com_sprintfUni(displayMessageBuf, sizeof(displayMessageBuf), L"%s\nModule %s", errorMessageBuf, crashrptdllpath);

			MessageBoxW(NULL, displayMessageBuf, L"Error loading crash reporter API", MB_OK);
		}
	}else{
		Sys_SetMiniDumpStarted();
	}

}

qboolean Sys_AlreadyRunning(qboolean destroy)
{
	char varname[1024];
	static HANDLE hMutexOneInstance;
	qboolean alreadyRunning;

	uint32_t crc = crc32_16bytes(Sys_ExeFile(), strlen(Sys_ExeFile()), 0);

	Com_sprintf(varname, sizeof(varname), "Global\\%X-6-BZ34-1337-7331-IW3", crc);

	if(destroy)
	{
		if(hMutexOneInstance == NULL)
		{
			return 1;
		}
		hMutexOneInstance = NULL;
		CloseHandle(hMutexOneInstance);
		return 1;
	}

	hMutexOneInstance = CreateMutexA( NULL, TRUE, varname);
	alreadyRunning = (GetLastError() == ERROR_ALREADY_EXISTS);

	if (hMutexOneInstance != NULL)
    {
        ReleaseMutex(hMutexOneInstance);
    }
	return alreadyRunning;
}

__stdcall void ExcHndlInit();

__declspec( dllexport ) int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){

	char cwd[256];
	char lpFilename[MAX_STRING_CHARS];
	int copylen;
	int choice;
	char cmdline[4096];
	char tchr;
	qboolean pbrequested;

#ifndef NDEBUG
	MessageBoxA(NULL, "Attach debugger now", "Waiting for debugger", MB_OK);
#endif



	copylen = GetModuleFileNameA(hInstance, lpFilename, sizeof(lpFilename));
	if(copylen >= (sizeof(lpFilename) -1))
	{
		Sys_SetExeFile( "" );
	}else{
		Sys_SetExeFile( lpFilename );
	}

	Sys_SetupCwd( );
	SetDllDirectoryA("");

	Sys_InitializeCriticalSections();
	if(!Sys_InitializeGlobalCriticalSection())
	{
		preInitError("Couldn't create GlobalCriticalSection object");
		return -1;
	}

	Patch_MainModule(Patch_Other);

	Sys_InitMainThread();

	Sys_FixUpThirdPartyModules();

	Sys_InitLocalization();

	Sys_LoadModules(hInstance);

	if(sys_tempinstall)
	{
		tchr = 't';
	}else{
		tchr = '\0';
	}
	Com_sprintf(UPDATE_VERSION, sizeof(UPDATE_VERSION), "%s%c", UPDATE_VERSION_NUM, tchr);

  //MessageBoxA(NULL, "Wait for debugger", "Wait for debugger", MB_OK);

	if(!strstr(lpCmdLine, "allowdupe") && !strstr(lpCmdLine, "dedicated") && !strstr(lpCmdLine, "installupdatefiles") && !strstr(lpCmdLine, "ui_playerProfileAlreadyChosen"))
	{
		//GetApplicationLockFileName();

		while(Sys_AlreadyRunning(qfalse))
		{
			choice = MessageBoxTimeoutA(NULL, "An copy of CoD4 is already running", "CoD4 is already running",
										MB_ABORTRETRYIGNORE | MB_DEFBUTTON1 | MB_SETFOREGROUND | MB_ICONERROR,
										0, 60000);
			if(choice == IDRETRY)
			{
				Sys_AlreadyRunning(qtrue);
				continue;
			}
			if(choice == IDIGNORE)
			{
				break;
			}
			Sys_AlreadyRunning(qtrue);
			return -1;
		}

		if(strstr(lpCmdLine, "+save")){
			if(!Sys_CheckCrashOrRerun()){
				*((DWORD*)(0xcc147d4)) = 0;
				*((DWORD*)(0xcc147d8)) = 0;
				Sys_AlreadyRunning(qtrue);
				return(0);
			}
		}
	}


	Com_InitParse();
	Cvar_Init();

	*(double*)(0xCC11EF8) = InitTiming() * 1000.0;

	Sys_GetHardwareInfo();

	g_wv.hInstance = hInstance;

	//Clean the commandline from the executable path so no weird error show up
	const char* c = lpCmdLine;
	cmdline[0] = '\0';
	while(*c == ' ' && *c != '\0')
	{
		++c;
	}
	if(*c == '\"')
	{
		++c;
		while(*c != '\"' && *c != '\0')
		{
			++c;
		}
		if(*c != '\0')
		{
			++c;
			if(*c == ' ')
			{
				++c;
			}
		}
		Q_strncpyz( cmdline, c, sizeof(cmdline));
	}else{
		Q_strncpyz( cmdline, lpCmdLine, sizeof(cmdline));
	}

	if(strstr(lpCmdLine, "+nosplash") == NULL)
	{
		Sys_CreateSplashWindow();

		if(splashScreen_HWND){
			ShowWindow(splashScreen_HWND ,SW_SHOW );
			UpdateWindow(splashScreen_HWND);
		}
	}


	Sys_CreateMainWindowClass();




	if(strstr(lpCmdLine, "+sysconsole") != NULL)
	{
		Sys_CreateConsole( qtrue );
	}else{
		Sys_CreateConsole( qfalse );
	}

	SetErrorMode(1);

	Sys_Milliseconds();

	Com_Init( cmdline );
	Sys_FixUpThirdPartyModules();
	if(!com_dedicated || !com_dedicated->integer)
		Cbuf_AddText("readStats\n");


	Sys_Cwd( cwd, sizeof(cwd));

	Com_Printf(CON_CHANNEL_SYSTEM, "Working directory: %s\n", cwd);

	if((!com_dedicated || !com_dedicated->integer))
	{
		pbrequested = Cvar_GetBool("cl_punkbuster");
		PBCL_GlobalConstructor();
		if(!PbClientInitialize(g_wv.hInstance)){
			if(pbrequested)
			{
				Com_SetErrorMessage("MPUI_NOPUNKBUSTER");
			}
		}
	}

	pbrequested = Cvar_GetBool("sv_punkbuster");
	PBSV_GlobalConstructor();
	if(!PbServerInitialize()){
		if(pbrequested)
		{
			Com_PrintError(CON_CHANNEL_SYSTEM, "Unable to initialize PunkBuster. PunkBuster is disabled\n");
			Com_SetErrorMessage("MPUI_NOPUNKBUSTER");
		}
	}

	SetFocus(g_wv.hWnd);

	if(com_dedicated && com_dedicated->integer)
		Patch_MainModule(Patch_Dedicated);

	CleanKernel32Funcs();

	FS_ValidateIwdFiles();

	while(qtrue)
	{
		if((com_dedicated && com_dedicated->integer) || g_wv.isMinimized){
			Sleep(5);
		}
		Sys_MainThreadCheckForGlobalCriticalSection();
		Com_Frame();
		PbServerProcessEvents(0);

		if(!com_dedicated || !com_dedicated->integer)
		{
			PbClientProcessEvents();



		}
	}
	return 0;
}

void __iw3mp_security_init_cookie();
int __iw3mp_tmainCRTStartup();

/* We have a too small stacksize on mainthread.
This trick will replace the mainthread with a new mainthread which has an large enough stack */

int entrypoint()
{
	static qboolean mainThreadCreated;

	if(!mainThreadCreated)
	{
		mainThreadCreated = qtrue;

		if (CreateThread(NULL, //Choose default security
							1024*1024*12, //stack size
							(LPTHREAD_START_ROUTINE)&entrypoint, //Routine to execute
							NULL, //Thread parameter
							0, //Immediately run the thread
							NULL //Thread Id
		) == NULL)
		{
			return -1;
		}
		return 0;
	}

	HINSTANCE hInstance = (HINSTANCE)0x400000;
	HINSTANCE hPrevInstance = (HINSTANCE)0x0;
	char lpCmdLine[1024];

	Q_strncpyz(lpCmdLine, GetCommandLineA(), sizeof(lpCmdLine));

	int r = WinMain(hInstance, hPrevInstance, lpCmdLine, 0);
	exit(r);
}



void Sys_RunUpdater( );
void Sys_MakeCoDXTempInstall();
qboolean Patch_isiw3mp();
qboolean Patch_isiw3sp();
void Sys_RestartAndPatch();
void Sys_RunInstallerOnDemand();


BOOL WINAPI __declspec(dllexport) DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID pvReserved)
{
	wchar_t lpFilename[MAX_STRING_CHARS];
	int copylen;

	if(dwReason == DLL_PROCESS_ATTACH)
	{
		dllHandle = hInstance;
	}
	if(dwReason == DLL_PROCESS_ATTACH && *(int*)0x690429 != 0 /* Will bbe 1 due to writeprocessmemory() on temp install */)
	{
		copylen = GetModuleFileNameW(hInstance, lpFilename, sizeof(lpFilename) /2);
		if(copylen >= (sizeof(lpFilename) -1))
		{
			Sys_SetDllFile( L"" );
		}else{
			Sys_SetDllFile( lpFilename );
		}

		if(Com_HunkInitialized() == qfalse)
		{
				/* Startup from temp install */
				sys_tempinstall = qtrue;
				Patch_MainModule(Patch_WinMainEntryPoint);
		}

	}
	return TRUE;
}

HMODULE Sys_DllHandle()
{
	return dllHandle;
}

void __cdecl Win_ShutdownLocalization()
{
  *(int*)0xCC147D4 = 0;
  *(int*)0xCC147D8 = 0;
}




void Sys_Quit()
{

    Sys_EnterCriticalSection( 2 );
    timeEndPeriod(1u);
    IN_Shutdown(); //sub_575E90();
    Key_Shutdown(); //sub_4685D0();
	Sys_NormalExit();
	Win_ShutdownLocalization( );
	/* Function does not release all elements properly in some cases */
    //Dvar_Shutdown(); //sub_56B7D0();
    Cmd_Shutdown();
	//Con_Shutdown();
	//Com_ShutdownEvents();
	Sys_DestroyConsole();
    sub_477210();
	Sys_AlreadyRunning(qtrue);
    Sys_RestartProcessOnExit( );
	Sys_RunInstallerOnDemand();
	ExitProcess(0);
  //doexit(0, 0, 0);

}

void Sys_NormalExit()
{
    DeleteFileA(sys_processSemaphoreFile);
}

void Sys_DestroySplashWindow()
{
    if ( splashScreen_HWND )
    {
        ShowWindow(splashScreen_HWND, 0);
        DestroyWindow(splashScreen_HWND);
        splashScreen_HWND = 0;
    }
}

void Sys_HideSplashWindow()
{
	if ( splashScreen_HWND )
	{
		ShowWindow(splashScreen_HWND, 0);
	}
}



void Sys_RestartProcessOnExit()
{
	void* HWND = NULL;
	char displayMessageBuf[1024];
	const char *exefile;
	const char* method;
    SHELLEXECUTEINFO sei;
	int pid;

	if ( sys_restartCmdLine[0] == '\0' )
	{
		/* No restart (normal exit) */
		return;
	}


	exefile = Sys_ExeFile();
	if(strlen(exefile) < 9)
	{
		/* Can not restart (normal exit) */
		return;
	}


	if(AutoupdateRequiresElevatedPermissions)
	{
		method = "runas";

		MessageBoxA(HWND, "Note: Installation of this update for Call of Duty 4 will require extended permissions" , "Call of Duty 4 - Autoupdate", MB_OK | MB_ICONEXCLAMATION);

	}else{
		method = "open";

	}
/*
	if ( !ShellExecuteA(NULL, method, exefile, sys_restartCmdLine, NULL, SW_MAXIMIZE | SW_RESTORE) )
	{
		FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						0, GetLastError(), 0x400, displayMessageBuf, sizeof(displayMessageBuf), 0);

		MessageBoxA(HWND, va("ShellExec of commandline: %s %s has failed.\nError: %s\n" , exefile, sys_restartCmdLine, displayMessageBuf), "Call of Duty - Error", MB_OK | MB_ICONERROR);
	}
*/


	// Launch itself as administrator.
	memset(&sei, 0, sizeof(sei));
	sei.cbSize = sizeof(sei);
    sei.lpVerb = method;
    sei.lpFile = exefile;
	sei.lpParameters = sys_restartCmdLine;
    sei.hwnd = HWND;
    sei.nShow = SW_NORMAL;
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;

	if (!ShellExecuteExA(&sei))
	{
		FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						0, GetLastError(), 0x400, displayMessageBuf, sizeof(displayMessageBuf), 0);

		MessageBoxA(HWND, va("ShellExec of commandline: %s %s has failed.\nError: %s\n" , exefile, sys_restartCmdLine, displayMessageBuf), "Call of Duty - Error", MB_OK | MB_ICONERROR);
		return;
	}
	if(sei.hProcess == NULL)
	{
		return;
	}
	pid = GetProcessId(sei.hProcess);
    AllowSetForegroundWindow(pid);
}

void Sys_CreateErrorBox(const char* text)
{
  MessageBoxA(GetActiveWindow(), text, "Error", 0x10u);
}

void __cdecl Sys_Error(const char *fmt, ...)
{
	struct tagMSG Msg; // [sp+Ch] [bp-101Ch]@3
	char		errormsg[0x1000];
	va_list		argptr;

    Sys_EnterCriticalSection( 2 );

	Com_SetErrorEntered();

	Sys_SuspendOtherThreads();

	R_DestroyGameWindows();

	va_start (argptr,fmt);
	Q_vsnprintf (errormsg, sizeof(errormsg), fmt, argptr);
	va_end (argptr);

	sub_55A170();

	if ( com_dedicated->integer && Sys_IsMainThread() )
	{
		Sys_ShowConsole(1, qfalse);
		Conbuf_AppendText("\n\n");
		Conbuf_AppendText(errormsg);
		Conbuf_AppendText("\n");
		Sys_CreateErrorBox(errormsg);
		while ( GetMessageA(&Msg, 0, 0, 0) )
		{
		  TranslateMessage(&Msg);
		  DispatchMessageA(&Msg);
		}
	}else{
		Sys_CreateErrorBox(errormsg);
	}
	Sys_AlreadyRunning(qtrue);
	ExitProcess(0);

//	doexit( 0, 0, 0 );
}

void Sys_Mkdir(const char* dir)
{
	_mkdir(dir);
}

void Sys_MkdirUni(const wchar_t* dir)
{
	_wmkdir(dir);
}

qboolean Sys_CopyFileUni(wchar_t* fromOSPath, wchar_t* toOSPath)
{

	if(CopyFileW( fromOSPath, toOSPath, FALSE) == 0)
	{
		return qfalse;
	}
	return qtrue;
}

#ifndef KF_FLAG_CREATE
	#define KF_FLAG_CREATE 0x00008000
#endif

const GUID FOLDERID_LocalAppData = {0xF1B32785, 0x6FBA, 0x4FCF, {0x9D, 0x55, 0x7B, 0x8E, 0x7F, 0x15, 0x70, 0x91}};

HMODULE LoadSystemLibraryA(char* libraryname)
{
	char system32dir[1024];
	char modulepath[1024];
	int len;

	len = GetSystemDirectoryA(system32dir, sizeof(system32dir));

	if (len < 1 || len >= sizeof(system32dir))
	{
		return LoadLibraryA(libraryname);
	}

	Com_sprintf(modulepath, sizeof(modulepath), "%s\\%s", system32dir, libraryname);
	return LoadLibraryA(libraryname);

}


wchar_t* Sys_GetAppDataDir(wchar_t *basepath, int size)
{



	HMODULE hModuleShl = LoadSystemLibraryA("shell32.dll");
	HMODULE hModuleOle = LoadSystemLibraryA("ole32.dll");
	HRESULT (WINAPI *farproc)(const GUID *rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);
	HRESULT (__cdecl *farprocxp)(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);
	void (__cdecl *CoTaskMemFree_t)( void* mem );
	wchar_t xpPath[MAX_OSPATH];
	LPWSTR wszPath = NULL;


	/* Get the AppData folder */
	basepath[0] = L'\0';

	if(hModuleShl)
	{
		farproc = (void*)GetProcAddress(hModuleShl, "SHGetKnownFolderPath");
		farprocxp = (void*)GetProcAddress(hModuleShl, "SHGetFolderPathW");
		if(farproc)
		{

			HRESULT hr = farproc ( &FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &wszPath );
			if ( SUCCEEDED(hr) )
			{
				Q_strncpyzUni(basepath, wszPath, size);
				Q_strcatUni(basepath, size, L"\\CallofDuty4MW");
			}
			if(hModuleOle)
			{
				CoTaskMemFree_t = (void*)GetProcAddress(hModuleShl, "CoTaskMemFree");
				if(CoTaskMemFree_t)
				{
					CoTaskMemFree_t( wszPath );
				}
			}


		}else if(farprocxp){

			HRESULT hr = farprocxp(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, (LPTSTR)xpPath);
			if ( SUCCEEDED(hr) )
			{
				Q_strncpyzUni(basepath, xpPath, size);
				Q_strcatUni(basepath, size, L"\\CallofDuty4MW");
			}

		}else{
			FreeLibrary(hModuleShl);
			return NULL;
		}
		FreeLibrary(hModuleShl);

	}else{
		return NULL;
	}

	if(hModuleOle)
	{
		FreeLibrary(hModuleOle);
	}

	return basepath;
}


qboolean Sys_TestPermissions()
{
	char testpath[MAX_STRING_CHARS];
	const char* from;
	unsigned short rannum;

	from = Sys_ExeFile( );

	if(strlen(from) < 9)
	{
		Com_Error(ERR_DROP, "Autoupdate failed because of invalid HMODULE path. Manual installation is required\n");
		return qtrue;
	}
	Com_RandomBytes((byte*)&rannum, sizeof(rannum));
	Com_sprintf( testpath, sizeof(testpath), "%s.%u_test.exe", from, rannum);

	/* Disable folder virtualization */

    HANDLE Token;
	DWORD disable;

    if (OpenProcessToken(GetCurrentProcess(), MAXIMUM_ALLOWED, &Token)){
        SetTokenInformation(Token, (TOKEN_INFORMATION_CLASS)24, &disable, sizeof(disable));
		CloseHandle(Token);
    }

	if(FS_WriteTestOSPath( testpath ) == qfalse)
	{
		return qtrue;
	}
	return qfalse;

}

/* This function determins if we need elevated permissions, and sets the restart commandline arguments */
void Sys_SetupUpdater( const char* updatefiles )
{
	char excmdline[4*MAX_STRING_CHARS];

/*
	//Postponed - only when mss32.dll needs to be updated, otherwise nope
	if(Sys_TestPermissions())
	{
		AutoupdateRequiresElevatedPermissions = qtrue;
	}
*/

	Com_sprintf(excmdline, sizeof(excmdline), "+nosplash +set installupdatefiles \"%s\"", updatefiles);
//	MessageBoxA(NULL, excmdline, "Call of Duty Update", MB_OK | MB_ICONERROR);
	Sys_SetRestartParams(excmdline);
}

/* This function determins if we need elevated permissions, and sets the restart commandline arguments */


void Sys_RegisterCoD4Protocol( )
{
	int status, len;
	HKEY hKey;
	char openpath[MAX_STRING_CHARS];
	char iconpath[MAX_STRING_CHARS];
	const char *exepath = Sys_ExeFile();
	const char *handler = "URL: CoD4 Connect Handler";
	len = strlen(exepath);

	if( len < 5)
	{
		return;
	}


	status = RegCreateKeyA(HKEY_CURRENT_USER, "Software\\Classes\\cod4", &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with protocol cod4");
		return;
	}
    RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)handler, strlen(handler) +1);
	RegSetValueExA(hKey, "URL Protocol", 0, 1u, NULL, 0);
    RegCloseKey(hKey);

	Com_sprintf(iconpath, sizeof(iconpath), "%s,0", exepath);

	status = RegCreateKeyA(HKEY_CURRENT_USER, "Software\\Classes\\cod4\\DefaultIcon", &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with protocol cod4");
		return;
	}
    RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)iconpath, strlen(iconpath) +1);
    RegCloseKey(hKey);


	status = RegCreateKeyA(HKEY_CURRENT_USER, "Software\\Classes\\cod4\\shell\\open\\command", &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with protocol cod4");
		return;
	}

	Com_sprintf(openpath, sizeof(openpath), "\"%s\" +openurl \"%%1\"", exepath);

	RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)openpath, strlen(openpath));
    RegCloseKey(hKey);
}


void Sys_RegisterDemoExt( )
{
	int status, len, size;
	HKEY hKey;
	HKEY hKey2;
	char openpath[MAX_STRING_CHARS];
	char iconpath[MAX_STRING_CHARS];
	const char *exepath = Sys_ExeFile();
	len = strlen(exepath);
	char value[MAX_STRING_CHARS];
	char typepath[MAX_STRING_CHARS];
	char commandpath[MAX_STRING_CHARS];
	char* productIndentifier = "CoD4X.CallofDuty4X.1";
	char buf[] = { "eOIc\\_" };
    int i;

    for(i = 0; i < 6; ++i)
    {
        buf[i] ^= 44;
    }

	if( len < 5)
	{
		return;
	}

	status = RegCreateKeyA(HKEY_CURRENT_USER, "Software\\Classes\\.dm_1", &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with extension .dm_1");
		return;
	}

	size = sizeof(value);

	status = RegQueryValueExA( hKey, "", NULL, NULL, (BYTE*)value, (DWORD*)&size);

	if(status == ERROR_SUCCESS && strstr(value, buf))
	{
		RegDeleteValue(hKey, NULL);
		if(RegOpenKeyA(HKEY_CURRENT_USER, "Software\\Classes", &hKey2) == ERROR_SUCCESS)
		{
			RegDeleteKeyA(hKey2, va("%s\\shell\\open\\command", value));
			RegDeleteKeyA(hKey2, va("%s\\shell\\open", value));
			RegDeleteKeyA(hKey2, va("%s\\shell", value));
			RegDeleteKeyA(hKey2, value);
			RegCloseKey(hKey2);
		}

	}


	status = RegQueryValueExA( hKey, "", NULL, NULL, (BYTE*)value, (DWORD*)&size);

	if(status != ERROR_SUCCESS || value[0] == '\0')
	{
		size = sizeof(value);
		RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)productIndentifier, strlen(productIndentifier) +1);
		if(RegQueryValueExA( hKey, "", NULL, NULL, (BYTE*)value, (DWORD*)&size) != ERROR_SUCCESS || value[0] == '\0' )
		{
			Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with extension .dm_1");
			RegCloseKey(hKey);
			return;
		}
	}
    RegCloseKey(hKey);

	Com_sprintf(typepath, sizeof(typepath), "Software\\Classes\\%s", value);


	if(RegOpenKeyA(HKEY_CURRENT_USER, typepath, &hKey) == ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		if(Q_stricmpn(value, productIndentifier, strlen(productIndentifier)))
		{
		/* Already registered with some else application. Will stop*/
			return;
		}
	}

	Com_sprintf(iconpath, sizeof(iconpath), "%s,0", exepath);

	status = RegCreateKeyA(HKEY_CURRENT_USER, typepath, &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with extension .dm_1");
		return;
	}
    RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)LONG_PRODUCT_NAME, strlen(LONG_PRODUCT_NAME) +1);
    RegSetValueExA(hKey, "CurVer", 0, 1u, (const BYTE*)value, strlen(value) +1);
	RegSetValueExA(hKey, "DefaultIcon", 0, 1u, (const BYTE*)iconpath, strlen(iconpath) +1);
    RegCloseKey(hKey);


	Com_sprintf(commandpath, sizeof(commandpath), "%s\\shell\\open\\command", typepath);

	status = RegCreateKeyA(HKEY_CURRENT_USER, commandpath, &hKey);
    if(status)
	{
		Com_PrintError(CON_CHANNEL_SYSTEM, "Couldn't update the registry with extension .dm_1");
		return;
	}
	Com_sprintf(openpath, sizeof(openpath), "\"%s\" +demo \"%%1\" fullpath", exepath);

	size = sizeof(value);

	if(RegQueryValueExA( hKey, "", NULL, NULL, (BYTE*)value, (DWORD*)&size) == ERROR_SUCCESS && !Q_stricmpn(value, openpath, strlen(openpath)) )
	{
		RegCloseKey(hKey);
		return;
	}
	RegSetValueExA(hKey, NULL, 0, 1u, (const BYTE*)openpath, strlen(openpath));
	RegCloseKey(hKey);
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	Com_Printf(CON_CHANNEL_SYSTEM, "Updated demo-file association record\n");
}


qboolean Sys_DirectoryExistsUni(wchar_t* path)
{
  DWORD dwAttrib = GetFileAttributesW(path);

  if(dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
  {
	return qfalse;
  }
  return qtrue;
}



int Sys_RemoveDirTreeUni(const wchar_t *dir)
{
  HANDLE dhandle;
  qboolean hassep;
  struct _WIN32_FIND_DATAW FindFileData;
  wchar_t FileName[256];
  wchar_t lastchar;

  lastchar = dir[wcslen(dir) - 1];
  if ( lastchar == L'\\' || lastchar == L'/' )
  {
    hassep = 1;
    Com_sprintfUni(FileName, sizeof(FileName), L"%s*", dir);
  }
  else
  {
    hassep = 0;
	Com_sprintfUni(FileName, sizeof(FileName), L"%s\\*", dir);
  }
  dhandle = FindFirstFileW(FileName, &FindFileData);
  if ( dhandle == (HANDLE)-1 )
  {
    return _wrmdir(dir) != -1;
  }

  do
  {

    if ( FindFileData.cFileName[0] != L'.' || (FindFileData.cFileName[1] && (FindFileData.cFileName[1] != L'.' || FindFileData.cFileName[2])) )
    {
	    if(hassep)
		{
		  Com_sprintfUni(FileName, sizeof(FileName), L"%s%s", dir, FindFileData.cFileName);
        }else{
		  Com_sprintfUni(FileName, sizeof(FileName), L"%s\\%s", dir, FindFileData.cFileName);
		}

        if ( FindFileData.dwFileAttributes & 0x10 ? Sys_RemoveDirTreeUni(FileName) == 0 : _wremove(FileName) == -1 )
		{
			FindClose(dhandle);
			return 0;
		}
    }
  }
  while ( FindNextFileW(dhandle, &FindFileData) != 0 );
  FindClose(dhandle);
  return _wrmdir(dir) != -1;
}


int Sys_ListDirectories(const wchar_t* dir, char** list, int limit)
{
  int count, i;
  HANDLE dhandle;
  struct _WIN32_FIND_DATAW FindFileData;
  char *ansifilename;
  wchar_t searchdir[MAX_OSPATH];
  char errorString[1024];

  list[0] = NULL;

  FS_BuildOSPathForThreadUni( dir, "*", "", searchdir, 0 );

  dhandle = FindFirstFileW(searchdir, &FindFileData);
  if ( dhandle == (HANDLE)-1 )
  {
	Sys_GetLastErrorAsString(errorString, sizeof(errorString));
	Com_DPrintf(CON_CHANNEL_SYSTEM,"Sys_ListDirectories: %s\n", errorString);
    return 0;
  }
  count = 0;

  do
  {

    if ( !FindFileData.cFileName[0])
    {
		continue;
	}
	if(FindFileData.cFileName[0] == L'.' && (FindFileData.cFileName[1] == L'.' || !FindFileData.cFileName[1]))
	{
		continue;
	}

	/* is directory ? */
    if ( FindFileData.dwFileAttributes & 0x10)
	{

		if(Q_WIsAnsiString(FindFileData.cFileName) == qfalse)
		{

			//We won't support non ANSI chars
			continue;
		}

		list[count] = malloc(wcslen(FindFileData.cFileName) +1);

		if(list[count] == NULL)
		{
			break;
		}

		ansifilename = list[count];

		/* String copy as ANSI string */
		for(i = 0; FindFileData.cFileName[i]; ++i)
		{
			ansifilename[i] = FindFileData.cFileName[i];
		}
		ansifilename[i] = '\0';
		count++;
	}

  }
  while ( FindNextFileW(dhandle, &FindFileData) != 0 && count < limit -1);

  FindClose(dhandle);

  list[count] = NULL;

  return count;
}

qboolean Sys_IsTempInstall()
{
	return sys_tempinstall;
}

qboolean PatchEntrypoint(HANDLE hProcess, const char* dllfile);

static qboolean run_installer = qfalse;

void Sys_InstallCoD4X_f()
{
	if(!sys_tempinstall)
	{
		return;
	}
	run_installer = qtrue;
	Cbuf_AddText("quit\n");
}

wchar_t* Sys_GetBinDir1k(wchar_t* bindir)
{
    HMODULE hMods[1024];
	DWORD cbNeeded;
    wchar_t szModName[1024];
	int i;

	HANDLE hProcess = GetCurrentProcess( );

	if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
        {
			szModName[0] = 0;
            // Get the full path to the module's file.
            if ( GetModuleFileNameW( hMods[i], szModName, sizeof(szModName) / sizeof(szModName[0])))
            {
                // Print the module name and handle value.
				wchar_t* f = wcsstr(szModName, L"CallofDuty4MW\\bin\\cod4x_");
				if(f)
				{
					FS_DirNameUni(szModName);
					Q_strncpyzUni(bindir, szModName, 1024);
					return bindir;
				}
            }
        }
    }
	return NULL;

}


void Sys_RunInstallerOnDemand()
{
	const char* from;
	wchar_t exefile[1024];
	wchar_t params[MAX_STRING_CHARS];
	wchar_t* shellcmd;
	char bindir[MAX_STRING_CHARS];
	wchar_t bindiruni[MAX_STRING_CHARS];
	char dir[MAX_STRING_CHARS];
	wchar_t displayMessageBuf[MAX_STRING_CHARS];
	wchar_t errormsgbuf[2*MAX_STRING_CHARS];
	char* cut;
	/* For now included. Maybe it will update later too */
	if(!sys_tempinstall || !run_installer)
	{
		return;
	}

	from = Sys_ExeFile();

	if(strlen(from) < 9)
	{
		Com_Error(ERR_DROP, "Installer failed because of invalid HMODULE path. Manual installation is required\n");
		return;
	}

	if(Sys_TestPermissions())
	{
		shellcmd = L"runas";
		MessageBoxA(NULL, "Note: Installation of the Call of Duty 4 X update will require extended permissions" , "Call of Duty 4 X - Preinstaller", MB_OK | MB_ICONEXCLAMATION);
	}else{
		shellcmd = L"open";
	}

	FS_ShiftStr( AUTOUPDATE_DIR, AUTOUPDATE_DIR_SHIFT, dir );
	FS_BuildOSPathForThreadUni( FS_GetSavePath(), dir, "cod4xupd.exe", exefile, 0);


	Q_strncpyz(bindir, from, sizeof(bindir));
	cut = strrchr(bindir, PATH_SEP);
	if(cut != NULL)
	{
		*cut = '\0';
	}else{
		Com_Error(ERR_DROP, "Installer failed because of invalid HMODULE path. Manual installation is required\n");
		return;
	}

	Q_StrToWStr(bindiruni, bindir, sizeof(bindiruni));

	Com_sprintfUni(params, sizeof(params), L"+set installdir \"%s\"", bindiruni);


	if ( !ShellExecuteW(NULL, shellcmd, exefile, params, NULL, SW_RESTORE) )
	{
		FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						0, GetLastError(), 0x400, displayMessageBuf, sizeof(displayMessageBuf)/2, 0);
		Com_sprintfUni(errormsgbuf, sizeof(errormsgbuf), L"ShellExec of commandline: %s %s has failed.\nError: %s\n" , exefile, params, displayMessageBuf);
		MessageBoxW(NULL, errormsgbuf, L"Call of Duty X Preinstaller - Error", MB_OK | MB_ICONERROR);
	}
}

/*
void* Steam_FindOverlayHandler(HMODULE hModule){

	int i;
	byte *dataptr, *textptr;
	DWORD dwSectionSize;
	DWORD dwSectionBase;
	IMAGE_SECTION_HEADER sectionHeader;

	const char* searchstr = "hookDirect3DCreate9 called\n";

	if (! hModule)
		return NULL;

	// get dos header
	IMAGE_DOS_HEADER dosHeader = *(IMAGE_DOS_HEADER*)(hModule);

	// get nt header
	IMAGE_NT_HEADERS ntHeader = *(IMAGE_NT_HEADERS*)((DWORD)hModule + dosHeader.e_lfanew);

	// iterate through section headers and search for ".text" section
	int nSections = ntHeader.FileHeader.NumberOfSections;

	dwSectionSize = 0;

	for ( i = 0; i < nSections; i++)
	{
		sectionHeader = *(IMAGE_SECTION_HEADER*) ((DWORD)hModule + dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS) + i*sizeof(IMAGE_SECTION_HEADER));

		if (strcmp((char*)sectionHeader.Name,".rdata") == 0)
		{
			// the values you need
			dwSectionBase = (DWORD)hModule + sectionHeader.VirtualAddress;
			dwSectionSize = sectionHeader.Misc.VirtualSize;
			break;
		}
	}

	if(!dwSectionSize)
	{
		return NULL;
	}

	for(i = 0, dataptr = (byte*)dwSectionBase; i < dwSectionSize; ++i, ++dataptr)
	{
		if(dataptr[0] == searchstr[0] && strcmp((char*)dataptr, searchstr) == 0)
		{
			break;
		}
	}
	if(i >= dwSectionSize)
	{
		return NULL;
	}

	dwSectionSize = 0;

	for ( i = 0; i < nSections; i++)
	{
		sectionHeader = *(IMAGE_SECTION_HEADER*) ((DWORD)hModule + dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS) + i*sizeof(IMAGE_SECTION_HEADER));

		if (strcmp((char*)sectionHeader.Name,".text") == 0)
		{
			// the values you need
			dwSectionBase = (DWORD)hModule + sectionHeader.VirtualAddress;
			dwSectionSize = sectionHeader.Misc.VirtualSize;
			break;
		}
	}

	if(!dwSectionSize){
		return NULL;
	}

	for(i = 0, textptr = (byte*)dwSectionBase; i < dwSectionSize; ++i, ++textptr)
	{
		//Push offset = 0x68
		if(*textptr == 0x68)
		{
			++i;
			++textptr;

			if(*(DWORD*)textptr == (DWORD)dataptr)
			{
				break;
			}
		}
	}

	if(i >= dwSectionSize)
	{
		return NULL;
	}

	//Find begin of function - step backwards
	for(i = 0; i < 15; ++i, --textptr)
	{
		if(textptr[0] == 0x55 && textptr[1] == 0x8b && textptr[2] == 0xec)
		{
			break;
		}
	}

	if(i >= 15)
	{
		//Function start not found
		return NULL;
	}

	return textptr;
}


void Sys_SteamSetOverlayhandler()
{
	void (*farproc)( );
	HMODULE hModule = GetModuleHandleA("GameOverlayRenderer");
	if(hModule == NULL)
	{
		return;
	}

	farproc = Steam_FindOverlayHandler(hModule);

	if(farproc == NULL){
		return;
	}

	R_SetDirect3DCreate9Func(farproc);

}
*/

qboolean Sys_DetectNvidiaD3d9Wrap()
{
	// Get a list of all the modules in this process.
/*
	HMODULE hMods[1024];
	DWORD cbNeeded;
	int i;
	char* sep;
    char szModName[MAX_OSPATH];

	HANDLE hProcess = GetCurrentProcess( );

    if( !EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
		return qfalse;
	}

    for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
    {
        // Get the full path to the module's file.
        if ( GetModuleFileNameA( hMods[i], szModName, sizeof(szModName)))
        {
            // Print the module name and handle value.
			sep = strrchr(szModName, '\\');
			if(sep)
			{
				*sep = '\0';
				sep++;
			}else{
				sep = szModName;
			}

			if(sep)
			{

				if(!Q_stricmpn(sep, "nvd3d9wrap", 10))
				{
					return qtrue;
				}

			}
        }
    }
*/
	if(GetModuleHandleA("nvd3d9wrap"))
	{
		return qtrue;
	}

	return qfalse;
}

qboolean IsJumpInstruction(void* adr)
{
	byte* inst = adr;
	if(inst[0] == 0xe9)
	{
		return qtrue;
	}
	return qfalse;
}


qboolean IsHotpatchEntrypoint(void* adr)
{
	byte* inst = adr;

	if(inst[2] != 0x55){
		return qfalse;
	}
	if(inst[3] != 0x8b || inst[4] != 0xec)
	{
		return qfalse;
	}
	return qtrue;

}

qboolean Sys_RemoveHookFromProc(void* topatchproc, byte* replacebytes, int replacebyteslen)
{
	DWORD oldprotect, dummyoldp;

	/* Detect if a hook was really installed */
	if(IsJumpInstruction(topatchproc) == qfalse)
	{
		//No hook found
		return qtrue;
	}

	if(VirtualProtect(topatchproc, replacebyteslen, PAGE_EXECUTE_READWRITE, &oldprotect) == 0)
	{

		return qfalse;
	}
	memcpy(topatchproc, replacebytes, replacebyteslen);

	VirtualProtect(topatchproc, replacebyteslen, oldprotect, &dummyoldp);

	return qtrue;

}


qboolean Sys_RemoveHooksFromModule(const char* libraryname, const char* cleanprocname, const char **tocleanprocs, int numprocs)
{
	void* topatchproc;
	void* cleancmpproc;
	int i;

	HMODULE hModule = GetModuleHandleA(libraryname);
	byte buffer[5];

	if(hModule == NULL)
	{
		return qfalse;
	}

	cleancmpproc = GetProcAddress(hModule, cleanprocname);
	if(cleancmpproc == NULL)
	{
		return qfalse;
	}

	/* Dirty Hook is detected! Removing it... */
	if(IsHotpatchEntrypoint(cleancmpproc) == qfalse)
	{
		return qfalse;
	}

	memcpy(buffer, cleancmpproc, sizeof(buffer));

	for(i = 0; i < numprocs; ++i)
	{
		/* Get the proc addresses of "Direct3DCreate9" for fixing and "DebugSetLevel" to get bytes to copy from */
		topatchproc = GetProcAddress(hModule, tocleanprocs[i]);
		if(topatchproc == NULL){
			continue;
		}

		if(Sys_RemoveHookFromProc(topatchproc, buffer, sizeof(buffer)) == qfalse)
		{
			return qfalse;
		}
	}
	return qtrue;
}

void CleanKernel32Funcs()
{
	const char *kernelbaseprocs[] = {"GetModuleHandleW", "GetModuleHandleA", "GetModuleHandleExW", "GetModuleHandleExA"};

	Sys_RemoveHooksFromModule("KernelBase.dll", "Sleep", kernelbaseprocs, 4);
	Sys_RemoveHooksFromModule("kernel32.dll", "Sleep", kernelbaseprocs, 4);
}



void Sys_MitigateNvidiaD3d9Wrap()
{
	const char *kernelbaseprocs[] = {"LoadLibraryExW", "FreeLibrary", "GetModuleHandleW",
									"GetModuleHandleExW", "GetModuleFilenameExW", "EnumProcessModulesEx", "GetMappedFileNameW",
									"GetModuleInformation", "RegQueryValueExW", "RegDeleteValueW", "RegSetValueExA", "RegSetValueExW"};

	const char *d3d9procs[] = {"Direct3DCreate9", "Direct3DCreate9Ex"};

	if(Sys_RemoveHooksFromModule("KernelBase.dll", "Sleep", kernelbaseprocs, 12) == qfalse)
	{
	//MessageBoxA(NULL, "Failed to disable nvd3d9wrap from KernelBase.dll. The game does maybe crash when Steam is running and nvd3d9wrap.dll is installed.", "Failed to mitigate nvd3d9wrap", MB_OK | MB_ICONWARNING);
		return;
	}

	if(Sys_RemoveHooksFromModule("d3d9.dll", "DebugSetLevel", d3d9procs, 2) == qfalse)
	{
	//MessageBoxA(NULL, "Failed to disable nvd3d9wrap from d3d9.dll. The game does maybe crash when Steam is running and nvd3d9wrap.dll is installed.", "Failed to mitigate nvd3d9wrap", MB_OK | MB_ICONWARNING);
		return;
	}

	//Sys_SteamSetOverlayhandler();

}


void Sys_LockCriticalSection_v1(critSectionLock_t *section)
{
  while ( 1 )
  {
    if ( section->operand1 )
	{
        Sleep(0);
		continue;
	}

    if ( InterlockedIncrement(&section->operand2) == 1 && !section->operand1 )
	{
      break;
	}

	InterlockedDecrement(&section->operand2);
    Sleep(0);
  }
}

void Sys_UnlockCriticalSection_v1(critSectionLock_t *section)
{
	InterlockedDecrement(&section->operand2);
}

const char* Sys_DefaultInstallPath()
{
  HMODULE hModule;
  DWORD len;
  static char exePath[256];

  if ( !exePath[0] )
  {
    hModule = GetModuleHandleA(0);
    if(hModule == NULL)
	{
		Com_Error(ERR_FATAL, "Sys_DefaultInstallPath: Got no HMODULE");
		return exePath;
	}
	len = GetModuleFileNameA(hModule, exePath, 0x100u);
    if ( len == 256 )
    {
      len = 255;
    }
    else if ( len == 0 )
    {
		Com_Error(ERR_FATAL, "Sys_DefaultInstallPath: Got no ModuleFileName");
		return exePath;
    }

    do
    {
      if ( exePath[len] == '\\' )
        break;
      if ( exePath[len] == '/' )
        break;
      if ( exePath[len] == ':' )
        break;

      --len;
    }
    while ( len );
    exePath[len] = 0;

  }
  return exePath;
}


qboolean Sys_IsClientActive()
{
	return g_wv.activeApp;
}


/*
==============================================================

DIRECTORY SCANNING

==============================================================
*/

#define MAX_FOUND_FILES 0x1000

/*
==============
Sys_ListFilteredFiles
==============
*/
void Sys_ListFilteredFiles( const char *basedir, char *subdirs, char *filter, char **list, int *numfiles )
{
	char	search[MAX_OSPATH], newsubdirs[MAX_OSPATH];
	char	filename[MAX_OSPATH];
	intptr_t	findhandle;
	struct _finddata64i32_t findinfo;

	if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
		return;
	}

	if (strlen(subdirs)) {
		Com_sprintf( search, sizeof(search), "%s\\%s\\*", basedir, subdirs );
	}
	else {
		Com_sprintf( search, sizeof(search), "%s\\*", basedir );
	}

	findhandle = _findfirst64i32 (search, &findinfo);
	if (findhandle == -1) {
		return;
	}

	do {
		if (findinfo.attrib & _A_SUBDIR) {

			if (Q_stricmp(findinfo.name, ".") && Q_stricmp(findinfo.name, "..")) {

				if (strlen(subdirs)) {
					Com_sprintf( newsubdirs, sizeof(newsubdirs), "%s\\%s", subdirs, findinfo.name);
				}
				else {
					Com_sprintf( newsubdirs, sizeof(newsubdirs), "%s", findinfo.name);
				}
				Sys_ListFilteredFiles( basedir, newsubdirs, filter, list, numfiles );
			}
		}
		if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
			break;
		}
		Com_sprintf( filename, sizeof(filename), "%s\\%s", subdirs, findinfo.name );
		if (!Com_FilterPath( filter, filename, qfalse ))
			continue;
		list[ *numfiles ] = (char*)CopyString( filename );
		(*numfiles)++;
	} while ( _findnext64i32 (findhandle, &findinfo) != -1 );

	_findclose (findhandle);
}

void Sys_ListFilteredFilesW( const wchar_t *basedir, wchar_t *subdirs, wchar_t *filter, wchar_t **list, int *numfiles )
{
	wchar_t	search[MAX_OSPATH], newsubdirs[MAX_OSPATH];
	wchar_t	filename[MAX_OSPATH];
	intptr_t	findhandle;
	struct _wfinddata64i32_t findinfo;

	if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
		return;
	}

	if (subdirs[0]) {
		Com_sprintfUni( search, sizeof(search), L"%s\\%s\\*", basedir, subdirs );
	}
	else {
		Com_sprintfUni( search, sizeof(search), L"%s\\*", basedir );
	}

	findhandle = _wfindfirst64i32 (search, &findinfo);
	if (findhandle == -1) {
		return;
	}

	do {
		if (findinfo.attrib & _A_SUBDIR) {

			if (wcscmp(findinfo.name, L".") && wcscmp(findinfo.name, L"..")) {

				if (wcslen(subdirs)) {
					Com_sprintfUni( newsubdirs, sizeof(newsubdirs), L"%s\\%s", subdirs, findinfo.name);
				}
				else {
					Com_sprintfUni( newsubdirs, sizeof(newsubdirs), L"%s", findinfo.name);
				}
				Sys_ListFilteredFilesW( basedir, newsubdirs, filter, list, numfiles );
			}
		}
		if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
			break;
		}
		Com_sprintfUni( filename, sizeof(filename), L"%s\\%s", subdirs, findinfo.name );
		if (!Com_FilterPathW( filter, filename, qfalse ))
			continue;
		list[ *numfiles ] = _wcsdup( filename );
		(*numfiles)++;
	} while ( _wfindnext64i32 (findhandle, &findinfo) != -1 );

	_findclose (findhandle);
}



/*
==============
strgtr
==============
*/

static qboolean strgtr(const char *s0, const char *s1)
{
	int l0, l1, i;

	l0 = strlen(s0);
	l1 = strlen(s1);

	if (l1<l0) {
		l0 = l1;
	}

	for(i=0;i<l0;i++) {
		if (s1[i] > s0[i]) {
			return qtrue;
		}
		if (s1[i] < s0[i]) {
			return qfalse;
		}
	}
	return qfalse;
}


static qboolean strgtrw(const wchar_t *s0, const wchar_t *s1)
{
	int l0, l1, i;

	l0 = wcslen(s0);
	l1 = wcslen(s1);

	if (l1<l0) {
		l0 = l1;
	}

	for(i=0;i<l0;i++) {
		if (s1[i] > s0[i]) {
			return qtrue;
		}
		if (s1[i] < s0[i]) {
			return qfalse;
		}
	}
	return qfalse;
}

/*
==============
Sys_ListFiles
==============
*/
char **Sys_ListFiles( const char *directory, const char *extension, char *filter, int *numfiles, qboolean wantsubs )
{
	char	search[MAX_OSPATH];
	int	nfiles;
	char	**listCopy;
	char	*list[MAX_FOUND_FILES];
	struct _finddata64i32_t findinfo;
	intptr_t	findhandle;
	int	flag;
	int	i;

	if (filter) {

		nfiles = 0;
		Sys_ListFilteredFiles( directory, "", filter, list, &nfiles );

		list[ nfiles ] = 0;
		*numfiles = nfiles;

		if (!nfiles)
		return NULL;

		listCopy = Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
		for ( i = 0 ; i < nfiles ; i++ ) {
			listCopy[i] = list[i];
		}
		listCopy[i] = NULL;

		return listCopy;
	}

	if ( !extension) {
		extension = "";
	}

	// passing a slash as extension will find directories
	if ( extension[0] == '/' && extension[1] == 0 ) {
		extension = "";
		flag = 0;
	} else {
		flag = _A_SUBDIR;
	}

	Com_sprintf( search, sizeof(search), "%s\\*%s", directory, extension );

	// search
	nfiles = 0;

	findhandle = _findfirst64i32 (search, &findinfo);
	if (findhandle == -1) {
		*numfiles = 0;
		return NULL;
	}

	do {
		if ( (!wantsubs && flag ^ ( findinfo.attrib & _A_SUBDIR )) || (wantsubs && findinfo.attrib & _A_SUBDIR) ) {
			if ( nfiles == MAX_FOUND_FILES - 1 ) {
				break;
			}
			list[ nfiles ] = (char*)CopyString( findinfo.name );
			nfiles++;
		}
	} while ( _findnext64i32 (findhandle, &findinfo) != -1 );

	list[ nfiles ] = 0;

	_findclose (findhandle);

	// return a copy of the list
	*numfiles = nfiles;

	if ( !nfiles ) {
		return NULL;
	}

	listCopy = Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
	for ( i = 0 ; i < nfiles ; i++ ) {
		listCopy[i] = list[i];
	}
	listCopy[i] = NULL;

	do {
		flag = 0;
		for(i=1; i<nfiles; i++) {
			if (strgtr(listCopy[i-1], listCopy[i])) {
				char *temp = listCopy[i];
				listCopy[i] = listCopy[i-1];
				listCopy[i-1] = temp;
				flag = 1;
			}
		}
	} while(flag);

	return listCopy;
}

/*
==============
Sys_ListFiles
==============
*/
wchar_t **Sys_ListFilesW( const wchar_t *directory, const wchar_t *extension, wchar_t *filter, int *numfiles, qboolean wantsubs )
{
	wchar_t	search[MAX_OSPATH];
	int	nfiles;
	wchar_t	**listCopy;
	wchar_t	*list[MAX_FOUND_FILES];
	struct _wfinddata64i32_t findinfo;
	intptr_t	findhandle;
	int	flag;
	int	i;

	if (filter) {

		nfiles = 0;
		Sys_ListFilteredFilesW( directory, L"", filter, list, &nfiles );

		list[ nfiles ] = 0;
		*numfiles = nfiles;

		if (!nfiles)
		return NULL;

		listCopy = Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
		for ( i = 0 ; i < nfiles ; i++ ) {
			listCopy[i] = list[i];
		}
		listCopy[i] = NULL;

		return listCopy;
	}

	if ( !extension) {
		extension = L"";
	}

	// passing a slash as extension will find directories
	if ( extension[0] == '/' && extension[1] == 0 ) {
		extension = L"";
		flag = 0;
	} else {
		flag = _A_SUBDIR;
	}

	Com_sprintfUni( search, sizeof(search), L"%s\\*%s", directory, extension );

	// search
	nfiles = 0;

	findhandle = _wfindfirst64i32 (search, &findinfo);
	if (findhandle == -1) {
		*numfiles = 0;
		return NULL;
	}

	do {
		if ( (!wantsubs && flag ^ ( findinfo.attrib & _A_SUBDIR )) || (wantsubs && findinfo.attrib & _A_SUBDIR) ) {
			if ( nfiles == MAX_FOUND_FILES - 1 ) {
				break;
			}
			list[ nfiles ] = _wcsdup( findinfo.name );
			nfiles++;
		}
	} while ( _wfindnext64i32 (findhandle, &findinfo) != -1 );

	list[ nfiles ] = 0;

	_findclose (findhandle);

	// return a copy of the list
	*numfiles = nfiles;

	if ( !nfiles ) {
		return NULL;
	}

	listCopy = Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
	for ( i = 0 ; i < nfiles ; i++ ) {
		listCopy[i] = list[i];
	}
	listCopy[i] = NULL;

	do {
		flag = 0;
		for(i=1; i<nfiles; i++) {
			if (strgtrw(listCopy[i-1], listCopy[i])) {
				wchar_t *temp = listCopy[i];
				listCopy[i] = listCopy[i-1];
				listCopy[i-1] = temp;
				flag = 1;
			}
		}
	} while(flag);

	return listCopy;
}

/*
==============
Sys_FreeFileList
==============
*/
void Sys_FreeFileList( char **list )
{
	int i;

	if ( !list ) {
		return;
	}

	for ( i = 0 ; list[i] ; i++ ) {
		FreeString( list[i] );
	}

	Z_Free( list );
}

void Sys_FreeFileListW( wchar_t **list )
{
	int i;

	if ( !list ) {
		return;
	}

	for ( i = 0 ; list[i] ; i++ ) {
		free( list[i] );
	}

	Z_Free( list );
}



qboolean REGPARM(1) Sys_DirectoryHasContent(const char *dir)
{
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;

    char searchpath[MAX_OSPATH];

	if(strlen(dir) > MAX_OSPATH - 6 || dir[0] == '\0')
		return qfalse;

    Q_strncpyz(searchpath, dir, sizeof(searchpath));
	if( searchpath[strlen(searchpath) -1] ==  '\\' )
	{
		searchpath[strlen(searchpath) -1] = '\0';
	}
	Q_strcat(searchpath, sizeof(searchpath), "\\*");

    if((hFind = FindFirstFile(searchpath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        return qfalse;
    }

    do
    {
        if(stricmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0)
        {
			FindClose(hFind);
			return qtrue;
        }
    }
    while(FindNextFile(hFind, &fdFile));

    FindClose(hFind);

    return qfalse;
}


//Returns number of successful parsed certs
int Sys_ReadCertificate(void* cacert, int (*store_callback)(void* ca_ctx, const unsigned char* pemderbuf, int lenofpemder))
{
//From: http://stackoverflow.com/questions/9507184/can-openssl-on-windows-use-the-system-certificate-store
    HCERTSTORE hStore;
    PCCERT_CONTEXT pContext = NULL;

    hStore = CertOpenSystemStoreA(0, "ROOT");

    if (!hStore)
	{
        return 0;
	}
	int i = 0;
	while ((pContext = CertEnumCertificatesInStore(hStore, pContext)))
	{
		if(pContext->dwCertEncodingType != X509_ASN_ENCODING)
		{
			continue;
		}
		if(store_callback(cacert, pContext->pbCertEncoded, pContext->cbCertEncoded) >= 0)
		{
			++i;
		}
	}
	CertFreeCertificateContext(pContext);
	CertCloseStore(hStore, 0);
	return i;
}

void CopyMessageToClipboard(const char* message)
{
  HWND dw;
  char *mem;
  HGLOBAL memoryHandle;

  dw = GetDesktopWindow();
  if ( OpenClipboard(dw) )
  {
    EmptyClipboard();
    memoryHandle = GlobalAlloc(2u, strlen(message) + 1);
    if ( memoryHandle )
    {
      mem = (char *)GlobalLock(memoryHandle);
      if ( mem )
      {
		strcpy(mem, message);
        GlobalUnlock(memoryHandle);
        SetClipboardData(1u, memoryHandle);
      }
    }
    CloseClipboard();
  }
}

BOOL WINAPI HideWindowCallback(HWND hwnd, LPARAM lParam)
{
  int styleEx;
  char caption[1024];
  int style;
/*
  static HWND g_hwndGame[4];
  static int g_hiddenCount;
*/
  if ( !GetWindowTextA(hwnd, caption, 1024) || !strcmp(caption, Com_GetBuildDisplayName()) )
  {
    style = GetWindowLongA(hwnd, -16);
    styleEx = GetWindowLongA(hwnd, -20);
    if ( style & 0x10000000 )
    {
/*
      g_hwndGame[g_hiddenCount] = hwnd;
	  g_hiddenCount++;
	  if(g_hiddenCount >= 4)
	  {
		  g_hiddenCount = 0;
	  }
*/
      SetWindowLongA(hwnd, -16, style & 0xEFFFFFFF);
      SetWindowLongA(hwnd, -20, styleEx & 0xFFFFFFF7);
    }
    return 1;
  }
  return 0;
}

void FixWindowsDesktop()
{
  DWORD ti;
  HDC hdc;
  unsigned __int16 ramp[3][256];
  unsigned __int16 i;
  HWND hwndDesktop;

  ChangeDisplaySettingsA(0, 0);
  ti = GetCurrentThreadId();
  EnumThreadWindows(ti, HideWindowCallback, 0);
  hwndDesktop = GetDesktopWindow();
  hdc = GetDC(hwndDesktop);
  for ( i = 0; (signed int)i < 256; ++i )
  {
    ramp[0][i] = 257 * i;
    ramp[1][i] = 257 * i;
    ramp[2][i] = 257 * i;
  }
  SetDeviceGammaRamp(hdc, ramp);
  ReleaseDC(hwndDesktop, hdc);
}

#if 0
HINSTANCE __cdecl GetModuleBase(const char *name)
{
  char moduleName[260];
  HINSTANCE moduleHandle;
  int nameLength;
  int nameIndex;

  nameLength = strlen(name);
  for ( nameIndex = nameLength - 1; nameIndex >= 0 && name[nameIndex] != '.' && name[nameIndex] != '/' && name[nameIndex] != '\\'; --nameIndex );
  if ( nameIndex >= 0 && name[nameIndex] == '.' )
  {
    nameLength = nameIndex;
  }

  memcpy(moduleName, name, nameLength);
  strcat(moduleName, ".exe");
  moduleHandle = GetModuleHandleA(moduleName);
  if ( moduleHandle )
  {
    return moduleHandle;
  }
  memcpy(moduleName, (char *)name, nameLength);
  strcat(moduleName, ".dll");
  return GetModuleHandleA(moduleName);
}

void __cdecl ParseMapFile(FILE *fp, unsigned int baseAddress, const wchar_t *mapName)
{
  int v3; // eax@22
  const char *v4; // eax@28
  char *v5; // eax@43
  const char *v6; // eax@45
  const char *v7; // eax@59
  char *v8; // eax@74
  const char *v9; // eax@76
  char *v10; // eax@85
  char *v11; // eax@87
  const char *v12; // eax@106
  char *pszNameStop; // [sp+14h] [bp-878h]@87
  char *pszNameStart; // [sp+18h] [bp-874h]@85
  unsigned int loadAddress; // [sp+20h] [bp-86Ch]@3
  const char *filenameSubStr; // [sp+24h] [bp-868h]@28
  int j; // [sp+28h] [bp-864h]@14
  unsigned int address; // [sp+2Ch] [bp-860h]@8
  const char *filename; // [sp+30h] [bp-85Ch]@45
  AddressInfo_s *addressInfo; // [sp+34h] [bp-858h]@17
  unsigned int relAddress; // [sp+38h] [bp-854h]@31
  unsigned int lineOffset[4]; // [sp+3Ch] [bp-850h]@89
  char filenameBuffer[1024]; // [sp+4Ch] [bp-840h]@8
  unsigned int offset; // [sp+44Ch] [bp-440h]@8
  unsigned int baseEndAddress; // [sp+450h] [bp-43Ch]@5
  unsigned int group; // [sp+454h] [bp-438h]@8
  const char *funcName; // [sp+458h] [bp-434h]@40
  unsigned int lineGroup[4]; // [sp+45Ch] [bp-430h]@92
  int i; // [sp+46Ch] [bp-420h]@95
  unsigned int lineNumber[4]; // [sp+470h] [bp-41Ch]@92
  char *atChar; // [sp+480h] [bp-40Ch]@43
  char function[1024]; // [sp+484h] [bp-408h]@8
  int readCount; // [sp+888h] [bp-4h]@92

  lineBufferEndPos = 0;
  lineBufferStartPos = 0;
  while ( ReadLine(fp) )
  {
    if ( sscanf(lineBuffer, " Preferred load address is %x\r\n", &loadAddress) == 1 )
    {
      if ( SkipLines(2, fp, 1) )
      {
        baseEndAddress = 0;
        while ( ReadLine(fp) )
        {
          if ( !lineBuffer[0] )
          {
            for ( j = 0; j < g_assertAddressCount; ++j )
            {
              addressInfo = &g_assertAddress[j];
              if ( g_assertAddress[j].address >= baseAddress && addressInfo->address < baseEndAddress )
                I_strncpyz(addressInfo->moduleName, mapName, 64);
            }
            while ( ReadLine(fp) )
            {
              strstr(lineBuffer, "Publics by Value");
              if ( v3 )
              {
                if ( SkipLines(1, fp, 1) )
                {
                  while ( ReadLine(fp) )
                  {
                    if ( !lineBuffer[0] )
                    {
                      if ( SkipLines(2, fp, 1) && ReadLine(fp) )
                      {
                        if ( strcmp(lineBuffer, " Static symbols\r") )
                          goto LABEL_82;
                        if ( SkipLines(1, fp, 1) )
                        {
                          while ( ReadLine(fp) && lineBuffer[0] )
                          {
                            if ( sscanf(lineBuffer, "%x:%x %s %x", &group, &offset, function, &address) != 4 )
                            {
                              ParseError("Unknown line format in the static symbols section");
                              return;
                            }
                            strrchr(lineBuffer, 32);
                            filenameSubStr = v7;
                            if ( !v7 || sscanf(filenameSubStr + 1, "%s", filenameBuffer) != 1 )
                            {
                              ParseError("Couldn't parse file name in the static symbols section");
                              return;
                            }
                            relAddress = address;
                            for ( j = 0; j < g_assertAddressCount; ++j )
                            {
                              addressInfo = &g_assertAddress[j];
                              if ( g_assertAddress[j].address >= baseAddress
                                && addressInfo->address < baseEndAddress
                                && relAddress <= addressInfo->address
                                && (!addressInfo->bestFunction[0] || addressInfo->bestFunctionAddress < relAddress) )
                              {
                                addressInfo->bestFunctionAddress = relAddress;
                                funcName = function;
                                if ( function[0] == 95 || *funcName == 63 )
                                  ++funcName;
                                I_strncpyz(addressInfo->bestFunction, funcName, 64);
                                strchr(addressInfo->bestFunction, 64);
                                atChar = v8;
                                if ( v8 )
                                  *atChar = 0;
                                strrchr(filenameBuffer, 92);
                                filename = v9;
                                if ( v9 )
                                  ++filename;
                                else
                                  filename = filenameBuffer;
                                I_strncpyz(addressInfo->bestFunctionFilename, filename, 64);
                              }
                            }
                          }
LABEL_82:
                          while ( ReadLine(fp) )
                          {
                            if ( strncmp(lineBuffer, "Line numbers for ", 0x11u) )
                            {
                              ParseError("Expected line number section");
                              return;
                            }
                            strchr(lineBuffer, 40);
                            pszNameStart = v10;
                            if ( !v10 )
                            {
                              ParseError("Couldn't find '(' for the name of the source file in line number section");
                              return;
                            }
                            strchr(v10, 41);
                            pszNameStop = v11;
                            if ( !v11 )
                            {
                              ParseError("Couldn't find ')' for the name of the source file in line number section");
                              return;
                            }
                            strncpy(filenameBuffer, pszNameStart + 1, v11 - pszNameStart - 1);
                            *((_BYTE *)&lineOffset[3] + pszNameStop - pszNameStart + 3) = 0;
                            filenameSubStr = filenameBuffer;
                            if ( !SkipLines(1, fp, 1) )
                              return;
                            while ( ReadLine(fp) && lineBuffer[0] )
                            {
                              readCount = sscanf(
                                            lineBuffer,
                                            "%i %x:%x %i %x:%x %i %x:%x %i %x:%x\r\n",
                                            lineNumber,
                                            lineGroup,
                                            lineOffset,
                                            &lineNumber[1],
                                            &lineGroup[1],
                                            &lineOffset[1],
                                            &lineNumber[2],
                                            &lineGroup[2],
                                            &lineOffset[2],
                                            &lineNumber[3],
                                            &lineGroup[3],
                                            &lineOffset[3]);
                              if ( readCount % 3 || readCount / 3 <= 0 )
                              {
                                ParseError("unknown line format in the line number section");
                                return;
                              }
                              for ( i = 0; 3 * i < readCount; ++i )
                              {
                                relAddress = lineOffset[i] + baseAddress + 4096;
                                for ( j = 0; j < g_assertAddressCount; ++j )
                                {
                                  addressInfo = &g_assertAddress[j];
                                  if ( g_assertAddress[j].address >= baseAddress
                                    && addressInfo->address < baseEndAddress
                                    && relAddress <= addressInfo->address
                                    && (!addressInfo->bestLineFilename[0] || addressInfo->bestLineAddress < relAddress) )
                                  {
                                    addressInfo->bestLineAddress = relAddress;
                                    addressInfo->bestLineNumber = lineNumber[i];
                                    strrchr((char *)filenameSubStr, 92);
                                    filename = v12;
                                    if ( v12 )
                                      ++filename;
                                    else
                                      filename = filenameSubStr;
                                    I_strncpyz(addressInfo->bestLineFilename, filename, 64);
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                      return;
                    }
                    if ( sscanf(lineBuffer, "%x:%x %s %x", &group, &offset, function, &address) != 4 )
                    {
                      ParseError("Unknown line format in the public symbols section");
                      return;
                    }
                    strrchr(lineBuffer, 32);
                    filenameSubStr = v4;
                    if ( !v4 || sscanf(filenameSubStr + 1, "%s", filenameBuffer) != 1 )
                    {
                      ParseError("Couldn't parse file name in the public symbols section");
                      return;
                    }
                    relAddress = address;
                    for ( j = 0; j < g_assertAddressCount; ++j )
                    {
                      addressInfo = &g_assertAddress[j];
                      if ( g_assertAddress[j].address >= baseAddress
                        && addressInfo->address < baseEndAddress
                        && relAddress <= addressInfo->address
                        && (!addressInfo->bestFunction[0] || addressInfo->bestFunctionAddress < relAddress) )
                      {
                        addressInfo->bestFunctionAddress = relAddress;
                        funcName = function;
                        if ( function[0] == 95 || *funcName == 63 )
                          ++funcName;
                        I_strncpyz(addressInfo->bestFunction, funcName, 64);
                        strchr(addressInfo->bestFunction, 64);
                        atChar = v5;
                        if ( v5 )
                          *atChar = 0;
                        strrchr(filenameBuffer, 92);
                        filename = v6;
                        if ( v6 )
                          ++filename;
                        else
                          filename = filenameBuffer;
                        I_strncpyz(addressInfo->bestFunctionFilename, filename, 64);
                      }
                    }
                  }
                }
                return;
              }
            }
            return;
          }
          if ( sscanf(lineBuffer, "%x:%x %xH %s %s", &group, &offset, &address, function, filenameBuffer) != 5 )
          {
            ParseError("Unknown line format in the segments section");
            return;
          }
          if ( group == 1 && baseEndAddress < address + offset + baseAddress + 4096 )
            baseEndAddress = address + offset + baseAddress + 4096;
        }
      }
      return;
    }
  }
}

void __cdecl LoadMapFiles( )
{
  WIN32_FIND_DATAW FindFileData; // [sp+20h] [bp-1158h]@4
  DWORD baseAddress; // [sp+964h] [bp-814h]@5
  FILE *fp; // [sp+968h] [bp-810h]@6
  HANDLE hFindFile; // [sp+96Ch] [bp-80Ch]@4
  char string; // [sp+970h] [bp-808h]@2

  wchar_t mappath[1024];
  wchar_t filepath[2048];

  if(Sys_GetBinDir1k(mappath) == NULL)
  {
	  return;
  }
  if(wcslen(mappath) > 1000)
  {
	return;
  }
  wcscat(mappath, "\\*.map");

  hFindFile = FindFirstFileW(mappath, &FindFileData);
  if ( hFindFile != (HANDLE)-1 )
  {
    do
    {
	  if(wcslen(FindFileData.cFileName) < 5)
	  {
		continue;
	  }
	  if(!wcsstr(FindFileData.cFileName, ".exe") && !wcsstr(FindFileData.cFileName, ".dll"))
	  {
		  continue;
	  }
	  Com_sprintfUni(filepath, sizeof(filepath), L"%s%c%s", mappath, L'\\', FindFileData.cFileName);
	  baseAddress = GetModuleHandleW(filepath);
      if ( baseAddress )
      {
		int l = wcslen(filepath);
		filepath[l - 4] = 0;
        wcscat(filepath, L".map");
        fp = _wfopen(filepath, L"rb");
        if ( fp )
        {
          ParseMapFile(fp, baseAddress, filepath);
          fclose(fp);
        }
      }
    }
    while ( FindNextFileA(hFindFile, &FindFileData) );
    FindClose(hFindFile);
  }
}

#endif

/* 684 */
typedef enum
{
  FIRST_TIME = 0x0,
  RECURSIVE = 0x1,
}AssertOccurance;

static void (__cdecl *AssertCallback)(const char *) = NULL;
static char assertMessage[4096];
static int lastAssertType;
bool g_miniDumperStarted;

void Sys_SetMiniDumpStarted()
{
  g_miniDumperStarted = true;
}

bool Sys_IsMiniDumpStarted()
{
  return g_miniDumperStarted;
}

void AssertNotify(int type, AssertOccurance occurance)
{
	HWND h;
	const char *msg;

	if ( AssertCallback )
    	AssertCallback(assertMessage);

	if ( type )
	{
      if ( type == 1 )
        msg = "SANITY CHECK FAILURE... (this text is on the clipboard)";
      else
        msg = "INTERNAL ERROR";
    }
    else
    {
      msg = "ASSERTION FAILURE... (this text is on the clipboard)";
    }
    ShowCursor(1);
    h = GetActiveWindow();
    if ( MessageBoxA(h, assertMessage, msg, 0x12011u) == 1 && occurance != 1 )
    {
      if ( Sys_IsMiniDumpStarted() && !IsDebuggerPresent() )
	  {
        RaiseException(1u, 0, 0, 0);
	  }
	  exit(-1);
    }

}


void __cdecl Assert_BuildAssertMessage(const char *expr, const char *filename, int sourceline, int type, int skipLevels, char *message)
{
  char line[8192];
  char g_module[260];
  char bstring[256];

  if ( !filename )
  {
    filename = "<unknown>";
  }
  if ( !expr )
  {
    expr = "<unknown>";
  }
  if ( !GetModuleFileNameA(0, g_module, sizeof(g_module)) )
  {
    strcpy(g_module, "<unknown application>");
  }
  Com_BuildVersionString(bstring, sizeof(bstring));
  message[0] = 0;

  Com_sprintf(line, sizeof(line), "Build: %s\n", bstring, sizeof(line));
  strcat(message, line);
  Com_sprintf(line, sizeof(line), "Expression:\n    %s\n\nModule:    %s\nFile:    %s\nLine:    %d\n\n", expr, g_module, filename, sourceline);
  strcat(message, line);
  //Assert_DoStackTrace(message + strlen(message), skipLevels + 1, type, 0);
}


qboolean Assert_MyHandler(const char *filename, int line, int type, const char *fmt, ...)
{
  char shouldBreak;
  va_list va;
  static int isHandlingAssert;
  char message[1024];

  va_start(va, fmt);
  Sys_EnterCriticalSection(CRITSECT_ASSERT);
  _vsnprintf(message, 0x400u, fmt, va);
  message[1023] = 0;
  MessageBoxA(0, message, "Assertion failed", 0);
  if ( isHandlingAssert )
  {
    Com_Printf(CON_CHANNEL_SYSTEM, "ASSERTBEGIN - ( Recursive assert )---------------------------------------------\n");
    Com_Printf(CON_CHANNEL_SYSTEM, "Assert Expression:\n    %s\nFile:    %s\nLine:    %d\n\n", message, filename, line);
    Com_Printf(CON_CHANNEL_SYSTEM, "ASSERTEND - ( Recursive assert ) ----------------------------------------------\n\n");

	/*CopyMessageToClipboard();
    AssertNotify(lastAssertType, RECURSIVE);
    Assert_BuildAssertMessage(message, filename, line, type, 1, assertMessage);
    if ( isHandlingAssert == 1 )
    {
      isHandlingAssert = 2;
      Com_Printf(CON_CHANNEL_SYSTEM, "ASSERTBEGIN - ( Recursive assert )---------------------------------------------\n");
      Com_Printf(CON_CHANNEL_SYSTEM, assertMessage);
      Com_Printf(CON_CHANNEL_SYSTEM, "ASSERTEND - ( Recursive assert ) ----------------------------------------------\n\n");
    }*/
	Sleep(5);
    exit(-1);
  }
  lastAssertType = type;
  isHandlingAssert = 1;
  FixWindowsDesktop();
  Assert_BuildAssertMessage(message, filename, line, type, 1, assertMessage);
  Com_Printf(CON_CHANNEL_SYSTEM, "ASSERTBEGIN -------------------------------------------------------------------\n");
  Com_Printf(CON_CHANNEL_SYSTEM, "%s", assertMessage);
  Com_Printf(CON_CHANNEL_SYSTEM, "ASSERTEND ---------------------------------------------------------------------\n");
  /*
  if ( QuitOnError() )
  {
    ExitProcess(0xFFFFFFFF);
  }
  */
  CopyMessageToClipboard(assertMessage);
  shouldBreak = 1;
  AssertNotify(type, 0);
  isHandlingAssert = 0;
  Sys_LeaveCriticalSection(CRITSECT_ASSERT);
  return shouldBreak == 0;
}


void Sys_SetEvent(HANDLE hEvent)
{
	SetEvent(hEvent);
}



#if 0

typedef struct tagLC_ID {
    unsigned short wLanguage;
    unsigned short wCountry;
    unsigned short wCodePage;
}LC_ID,*LPLC_ID;

struct __lc_time_data {
        char *wday_abbr[7];
        char *wday[7];
        char *month_abbr[12];
        char *month[12];
        char *ampm[2];
        char *ww_sdatefmt;
        char *ww_ldatefmt;
        char *ww_timefmt;
        LCID ww_lcid;
        int  ww_caltype;
        int  refcount;

};

typedef struct threadlocaleinfostruct {
    int refcount;
    unsigned int lc_codepage;
    unsigned int lc_collate_cp;
    unsigned long lc_handle[6];
    LC_ID lc_id[6];
    struct {
      char *locale;
      wchar_t *wlocale;
      int *refcount;
      int *wrefcount;
    } lc_category[6];
    int lc_clike;
    int mb_cur_max;
    int *lconv_intl_refcount;
    int *lconv_num_refcount;
    int *lconv_mon_refcount;
    struct lconv *lconv;
    int *ctype1_refcount;
    unsigned short *ctype1;
    const unsigned short *pctype;
    const unsigned char *pclmap;
    const unsigned char *pcumap;
    struct __lc_time_data *lc_time_curr;
} threadlocinfo;


/***
* __addlocaleref(pthreadlocinfo ptloci)
*
* Purpose:
*       Increment the refrence count for each element in the localeinfo struct.
*
*******************************************************************************/
void __cdecl __addlocaleref( pthreadlocinfo ptloci)
{
    int category;
    InterlockedIncrement(&(ptloci->refcount));
    if ( ptloci->lconv_intl_refcount != NULL )
        InterlockedIncrement(ptloci->lconv_intl_refcount);

    if ( ptloci->lconv_mon_refcount != NULL )
        InterlockedIncrement(ptloci->lconv_mon_refcount);

    if ( ptloci->lconv_num_refcount != NULL )
        InterlockedIncrement(ptloci->lconv_num_refcount);

    if ( ptloci->ctype1_refcount != NULL )
        InterlockedIncrement(ptloci->ctype1_refcount);

    for (category = LC_MIN; category <= LC_MAX; ++category) {
        if (ptloci->lc_category[category].locale != __clocalestr &&
            ptloci->lc_category[category].refcount != NULL)
            InterlockedIncrement(ptloci->lc_category[category].refcount);
        if (ptloci->lc_category[category].wlocale != NULL &&
            ptloci->lc_category[category].wrefcount != NULL)
            InterlockedIncrement(ptloci->lc_category[category].wrefcount);
    }
    InterlockedIncrement(&(ptloci->lc_time_curr->refcount));
}

#define PFLS_CALLBACK_FUNCTION void (*)();

signed int _mtinit()
{
  struct HINSTANCE__ *v1; // eax@1
  struct HINSTANCE__ *v2; // edi@1
  FARPROC v4; // eax@3
  DWORD v5; // eax@8
  int v6; // eax@11
  int v7; // ST08_4@11
  PVOID v8; // eax@12
  PVOID v9; // esi@12
  PVOID v10; // ST0C_4@13
  int v11; // ST08_4@13
  int v12; // eax@13
  int v13; // ST04_4@13
  DWORD v14; // eax@14

  hModule = GetModuleHandleA("KERNEL32.DLL");
  if ( !hModule )
  {
    exit();
  }

  DWORD WINAPI (*FlsAlloc)(PFLS_CALLBACK_FUNCTION lpCallback);
  PVOID WINAPI (*FlsGetValue)(DWORD dwFlsIndex);
  BOOL WINAPI (*FlsSetValue)(DWORD dwFlsIndex, PVOID lpFlsData);
  BOOL WINAPI (*FlsFree)(DWORD dwFlsIndex);

  FlsAlloc = GetProcAddress(hModule, "FlsAlloc");
  FlsGetValue = GetProcAddress(hModule, "FlsGetValue");
  FlsSetValue = GetProcAddress(hModule, "FlsSetValue");
  FlsFree = GetProcAddress(hModule, "FlsFree");

  if ( !FlsAlloc || !FlsGetValue || !FlsSetValue || !FlsFree )
  {
    FlsGetValue = TlsGetValue;
    FlsAlloc = TlsAlloc;
    FlsSetValue = TlsSetValue;
    FlsFree = TlsFree;
  }


  v5 = TlsAlloc();
  dwTlsIndex = v5;
  if ( v5 != -1 && TlsSetValue(v5, FlsGetValue) )
  {
    _init_pointers();
    if ( _mtinitlocks() )
    {

      dword_71B4F0 = ((int (__thiscall *)(int, int (__stdcall *)(void *)))FlsAlloc)(v7, _freefls);
      if ( dword_71B4F0 != -1 )
      {
        v8 = _calloc_crt(1u, 532);
        v9 = v8;
        if ( v8 )
        {
          v10 = v8;
          v11 = dword_71B4F0;
          if ( FlsSetValue(v13, v11, v10) )
          {
            _initptd((int *)v9, 0);
            v14 = GetCurrentThreadId();
            *((_DWORD *)v9 + 1) = -1;
            *(_DWORD *)v9 = v14;
            return 1;
          }
        }
      }
    }
    _mtterm();
  }


  return 0;
}
#endif
