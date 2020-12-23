#include "q_shared.h"
#include "qcommon.h"
#include "win_sys.h"
#include "fsdword.h"

#include <windows.h>


MilesSoundSystem_t mss;

void Scr_InitVariables()
{

__asm__ __volatile__ (
"	push   %edi\n"
"	mov    $0x1,%edi\n"
"	mov    $0x8001,%eax\n"
"	mov    $0x519180, %edx\n"
"	call   *%edx\n"
"	mov    $0x18000,%eax\n"
"	mov    $0x8002,%edi\n"
"	mov    $0x519180, %edx\n"
"	call   *%edx\n"
"	pop    %edi\n");
}



void __cdecl DObj_Init(){

__asm__ __volatile__ (
"	sub    %esp, 0x38\n"
"	xor    %eax,%eax\n"
"	push   $0x11\n"
"	push   %eax\n"
"	lea    0x18(%esp),%ecx\n"
"	push   %ecx\n"
"	mov    %eax,0x1c(%esp)\n"
"	mov    %eax,0x20(%esp)\n"
"	mov    %eax,0x24(%esp)\n"
"	mov    %eax,0x28(%esp)\n"
"	mov    %eax,0x2c(%esp)\n"
"	mov    $0x518290, %eax\n"
"	call   *%eax\n"
"	mov    %eax,0xd5ec518\n"
"	add    %esp, 0x44\n");
}


typedef struct
{
	int field_00;
	int field_04;
	int field_08;
	int field_0c;
	void** values;
}sysThreadData_t;


const void* Sys_GetValue(int key)
{
		const void* valptr;
		sysThreadData_t *sy;
		sy = *(sysThreadData_t**)__readfsdword(44);
		
		valptr = sy->values[key];

		return valptr;
}

/* This function pointer is undocumented and just valid for windows 2000.
    Therefore I guess.  */
typedef VOID (WINAPI *PPS_POST_PROCESS_INIT_ROUTINE)(VOID);

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
}UNICODE_STRING;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;


typedef struct _PEB_LDR_DATA {
  BYTE       Reserved1[8];
  PVOID      Reserved2[1];
  LIST_ENTRY InLoadOrderModuleList;  
  LIST_ENTRY InMemoryOrderModuleList;
  LIST_ENTRY InInitOrderModuleList;  
}PEB_LDR_DATA, *PPEB_LDR_DATA;


typedef struct _PEB {
  BYTE                          Reserved1[2];
  BYTE                          BeingDebugged;
  BYTE                          Reserved2[1];
  PVOID                         Reserved3[2];
  PPEB_LDR_DATA                 Ldr;
  PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
  BYTE                          Reserved4[104];
  PVOID                         Reserved5[52];
  PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
  BYTE                          Reserved6[128];
  PVOID                         Reserved7[1];
  ULONG                         SessionId;
} PEB, *PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    PVOID Reserved3;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    PVOID Reserved5[3];
    union {
        ULONG CheckSum;
        PVOID Reserved6;
    };
    ULONG TimeDateStamp;
}LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;
/*
void peb_test()
{
	PEB* peb;
	PEB_LDR_DATA* pebdata;
	LDR_DATA_TABLE_ENTRY* imgdataentry, *headentry;
	
	peb = (PEB*)__readfsdword(48);
	
	pebdata = peb->Ldr;
	headentry = (LDR_DATA_TABLE_ENTRY*)&pebdata->InLoadOrderModuleList.Flink;
	imgdataentry = (LDR_DATA_TABLE_ENTRY*)pebdata->InLoadOrderModuleList.Flink;
	
	while(imgdataentry != headentry)
	{
		MessageBoxW(NULL, imgdataentry->BaseDllName.Buffer, L"Imagepath", MB_OK);
		imgdataentry = (LDR_DATA_TABLE_ENTRY*)imgdataentry->InLoadOrderLinks.Flink;
	}
	
}

*/