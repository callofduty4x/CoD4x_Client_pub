
#include "q_shared.h"
#include "qcommon.h"
#include <stdlib.h>
#include <time.h>

#ifdef DEBUG

#define IMAGE_START 0x400000
#define IMAGE_STOP 0xD935FFF

typedef struct
{
	int maxinstructionlength;
	unsigned int addrtofind;
}xrefparseparams_t;

void Com_LogDebugData(byte* data, int len);

void Dev_ReadXRefIDALine(const char* line, void* params)
{
	int i;
	unsigned int *addr;
	xrefparseparams_t* parm = (xrefparseparams_t*)params;
	char patch[1024];
	//1st field
	for(i = 0; i < 2; ++i)
	{
		while(*line != ' ' && *line != '\0')
		{
			++line;
		}
		
		while(*line == ' ')
		{
			++line;
		}
	}

	if(Q_stricmpn(line, "loc_", 4) == 0)
	{
		line += 4;
	}else if(Q_stricmpn(line, "sub_", 4) == 0){
		line += 4;
	}
	
	//3rd field
	addr = (unsigned int*)strtol(line, NULL, 16);
	
	if((unsigned int)addr < IMAGE_START || (unsigned int)addr > IMAGE_STOP){
		Com_Error(ERR_DROP, "Parsing file: Address 0x%x is out of range of image\n", (unsigned int)addr);
		return;
	}
	
	for(i = 0; i < parm->maxinstructionlength; ++i)
	{
		if(*addr == parm->addrtofind)
		{
			break;
		}
		addr = (unsigned int*)((byte*)addr +1);
	}
	
	if( i == parm->maxinstructionlength)
	{
		Com_Printf(CON_FIRST_DEBUG_CHANNEL, "Addr 0x%x not found near 0x%x\n",  parm->addrtofind, (unsigned int)addr);
		return;
	}else{
		Com_sprintf(patch, sizeof(patch), "\t*((gitem_s**)0x%x) = bg_itemlist;\n", (unsigned int)addr);
		Com_LogDebugData((byte*)patch, strlen(patch));
	}
	
	
}


void Dev_ReadXRefLine(const char* line, void* params)
{
	int i;
	unsigned int *addr;
	xrefparseparams_t* parm = (xrefparseparams_t*)params;
	char patch[1024];
	unsigned int adding;
	char* add;
	unsigned int subrotinehead;
	
	adding = 0;
	
	//1st field
	while(*line == ' ' && *line != '\0')
	{
		++line;
	}

	if(Q_stricmpn(line, "loc_", 4) == 0)
	{
		line += 4;
	}else if(Q_stricmpn(line, "sub_", 4) == 0){
		line += 4;
	}
	
	add = strchr(line, '+');
	if(add)
	{
		*add = '\0';
		++add;
		adding = strtol(add, NULL, 16);
	}
	subrotinehead = strtol(line, NULL, 16);

	if(parm->addrtofind == 0)
	{
		parm->addrtofind = subrotinehead + adding;
		Com_Printf(CON_FIRST_DEBUG_CHANNEL, "searching for 0x%x:\n", parm->addrtofind);
		return;
	}
	
	//3rd field
	addr = (unsigned int*) (subrotinehead + adding);
	if((unsigned int)addr < IMAGE_START || (unsigned int)addr > IMAGE_STOP){
		Com_Error(ERR_DROP, "Parsing file: Address 0x%x is out of range of image\n", (unsigned int)addr);
		return;
	}
	
	for(i = 0; i < parm->maxinstructionlength; ++i)
	{
		if(*addr == parm->addrtofind)
		{
			break;
		}
		addr = (unsigned int*)((byte*)addr +1);
	}
	
	if( i == parm->maxinstructionlength)
	{
		Com_Printf(CON_FIRST_DEBUG_CHANNEL, "Addr 0x%x not found near 0x%x\n",  parm->addrtofind, (unsigned int)addr);
		return;
	}else{
		Com_sprintf(patch, sizeof(patch), "\t*((WeaponDef***)0x%x) = bg_weapAmmoTypes;\n", (unsigned int)addr);
		Com_LogDebugData((byte*)patch, strlen(patch));
		Com_Printf(CON_FIRST_DEBUG_CHANNEL, "%s", patch);
	}
	
	
}




void Dev_ParseFileByLine(const char* filename, void* params, void (*func)(const char*, void*))
{
	char* buf, *lineend;
	char line[1024];
	int len = FS_ReadFile(filename, (void**)&buf);
	if(len < 1)
	{
		Com_PrintError(CON_FIRST_DEBUG_CHANNEL, "File to parse not found\n");
		return;
	}
	
	const char* linestart = buf;


	while(1)
	{
		lineend = strchr(linestart, '\n');
		
		if(lineend)
		{
			*lineend = '\0';
			Q_strncpyz(line, linestart, sizeof(line));
			
			func(line, params);
			
			linestart = lineend +1;

		}else{
			FS_FreeFile(buf);
			return;
		}
	}
	
}


void Dev_ParseXRefIDAFile_f()
{
	xrefparseparams_t p;
	
	if(Cmd_Argc() < 2)
	{
		Com_Printf(CON_CHANNEL_DONT_FILTER, "Usage: xrefparse <filename>\n")	;
		return;
	}
	
	const char* name = Cmd_Argv(1);
 
	p.addrtofind = 0x0;
	p.maxinstructionlength = 4;//atoi(Cmd_Argv(3));
//	Dev_ParseFileByLine(name, &p, Dev_ReadXRefIDALine);
	Dev_ParseFileByLine(name, &p, Dev_ReadXRefLine);
}


/*
=============
Com_Error_f

Just throw a fatal error to
test error shutdown procedures
=============
*/
static void Com_Error_f( void ) {
	if ( Cmd_Argc() > 1 ) {
		Com_Error( atoi(Cmd_Argv(1)), "Testing %d error", atoi(Cmd_Argv(1)) );
	} else {
		Com_Error( ERR_FATAL, "Testing fatal error" );
	}
}


/*
=============
Com_Freeze_f

Just freeze in place for a given number of seconds to test
error recovery
=============
*/
static void Com_Freeze_f( void ) {
	float s;
	int start, now;

	s = 5.0;
	/*
	if ( Cmd_Argc() != 2 ) {
		Com_Printf( "freeze <seconds>\n" );
		return;
	}
	s = atof( Cmd_Argv( 1 ) );*/

	start = Sys_Milliseconds();

	while ( 1 ) {
		now = Sys_Milliseconds();
		if ( ( now - start ) * 0.001 > s ) {
			break;
		}
	}
}

/*
=================
Com_Crash_f

A way to force a bus error for development reasons
=================
*/
static void Com_Crash_f( void ) {
	*( int * ) 0 = 0x12345678;
}



void Dev_UtilsInit()
{
	
	Cmd_AddCommand("xrefparse", Dev_ParseXRefIDAFile_f);
	Cmd_AddCommand ("error", Com_Error_f);
	Cmd_AddCommand ("crash", Com_Crash_f);	
	Cmd_AddCommand ("freeze", Com_Freeze_f);
	//	Cmd_AddCommand ("assert", Com_Assert_f);
	
}





void Com_LogDebugData(byte* data, int len)
{
		static fileHandle_t debuglogfile;
	


		Sys_EnterCriticalSection(0);
		// TTimo: only open the qconsole.log if the filesystem is in an initialized state
		//   also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)

		if(FS_Initialized())
		{
			if ( !debuglogfile ) {
				struct tm *newtime;
				char openstring[128];
				__time64_t aclock;

				_time64( &aclock );
				newtime = _localtime64( &aclock );
				
				Com_sprintf(openstring, sizeof(openstring), "\nDebugLogfile opened on %s\n", asctime( newtime ));
				
				debuglogfile = FS_FOpenFileWrite( "debug_data.log" );
				if ( debuglogfile )
				{
					FS_Write(openstring, strlen(openstring), debuglogfile);
				}
			}
			
			if ( debuglogfile ) {
				FS_Write(data, len, debuglogfile);
				FS_Flush(debuglogfile);
			}
		}

		Sys_LeaveCriticalSection(0);

	
	
	
}

#endif





