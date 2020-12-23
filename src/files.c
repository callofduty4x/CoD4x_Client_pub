
#include "qcommon.h"
#include "unzip/unzip.h"
#include "client.h"

#include <string.h>
#include <stdlib.h>

#define g_disablePureCheck *(byte*)0xD5EC497
#define fs_gamedir (char*)0xCB19898
#define fs_checksumFeed *(int*)0xCB199A0
#define MAX_ZPATH	256

wchar_t fs_savepathDir[MAX_OSPATH];

typedef struct iwdPureReferences_s
{
	struct iwdPureReferences_s* next;
	int checksum;
	char baseName[MAX_OSPATH];
	char gameName[MAX_OSPATH];
}iwdPureReferences_t;

#define MAX_FILEHASH_SIZE	1024


#define fs_iwdPureChecks (*(iwdPureReferences_t**)0xD5EC4E4)
#define fs_numServerIwds *(int*)(0xD5EC4E8)
#define fs_numServerReferencedIwds *(int*)(0xCB1988C)
#define fs_numServerReferencedFFs *(int*)(0xCB1DCC4)
		
#define fs_serverIwdNames ((const char**)(0xCB1CCC0))
#define fs_serverReferencedIwdNames ((const char**)(0xCB199B8))
#define fs_serverReferencedFFNames ((const char**)(0xCB1BAC0))
#define fs_loadStack *(int*)(0xCB1AABC)
#define fs_fakeChkSum *(int*)(0xCB199B0)
#define fs_packFiles *(int*)(0xd5ec4e0)
#define fs_restrict getcvaradr(0xCB1999C)

int *fs_serverReferencedIwdSums = (int*)0xCB1BBC0;
int *fs_serverReferencedFFSums = (int*)0xCB1BB40;
int *fs_serverIwds = (int*)0xCB1AAC0;

#define fs_searchpaths *(searchpath_t**)(0xD5EC4DC)


typedef union qfile_gus {
	FILE*	o;
	unzFile	z;
} qfile_gut;

typedef struct qfile_us {
	qfile_gut	file;
	qboolean	iwdIsClone;
} qfile_ut;

typedef struct fileInPack_s {
	unsigned long	pos;	// file info position in zip
	char	*name;	// name of the file
	struct	fileInPack_s*	next;	// next file in the hash
} fileInPack_t;

typedef struct pack_t{	//Verified
	char	pakFilename[MAX_OSPATH];	// c:\quake3\baseq3\pak0.pk3
	char	pakBasename[MAX_OSPATH];	// pak0
	char	pakGamename[MAX_OSPATH];	// baseq3
	unzFile	handle;	// handle to zip file +0x300
	int	checksum;	// regular checksum
	int	pure_checksum;	// checksum for pure
	int	hasOpenFile;
	int	numFiles;	// number of files in pk3
	int	referenced;	// referenced file flags
	int	hashSize;	// hash table size (power of 2) +0x318
	fileInPack_t*	*hashTable;	// hash table +0x31c
	fileInPack_t*	buildBuffer;	// buffer with the filenames etc. +0x320
} pack_t;

typedef struct {
	qfile_ut	handleFiles;
	qboolean	handleSync;
	int			fileSize;
	int			zipFilePos;
	pack_t*		zipFile;
	qboolean	streamed;
	char		name[MAX_ZPATH];
} fileHandleData_t; //0x11C (284) Size

typedef struct {	//Verified
	char	path[MAX_OSPATH];	// c:\quake3
	char	gamedir[MAX_OSPATH];	// baseq3
} directory_t;

typedef struct searchpath_s {	//Verified
	struct searchpath_s *next;
	pack_t	*pack;	// only one of pack / dir will be non NULL
	directory_t	*dir;
	qboolean	bLocalized;
	int ignore;
	int ignorePureCheck;
	int language;
} searchpath_t;


fileHandleData_t *fsh = (fileHandleData_t*)(0xCB1DCC8);

qboolean FS_Started()
{
	return fs_loadStack != 0;
}

void __cdecl FS_CheckFileSystemStarted()
{
  if(!fs_searchpaths)
  {
 	Com_Error(ERR_FATAL, "FS_FOpenFileRead used before Filesystem has started");
  }

}

FILE *__cdecl FileWrapper_Open(const char *ospath, const char *mode)
{
  FILE *file;

  file = fopen(ospath, mode);

  return file;
}


int __cdecl FileWrapper_Seek(FILE *h, int offset, fsOrigin_t origin)
{

  switch ( origin )
  {
    case FS_SEEK_CUR:
      return fseek(h, offset, SEEK_CUR);
    case FS_SEEK_END:
      return fseek(h, offset, SEEK_END);
    case FS_SEEK_SET:
      return fseek(h, offset, SEEK_SET);
  }
  assertx(0, "Bad origin %i in FS_Seek", origin);
  return 0;
}

int __cdecl FileWrapper_GetFileSize(FILE *h)
{
  int startPos;
  int fileSize;

  startPos = ftell(h);
  fseek(h, 0, SEEK_END);
  fileSize = ftell(h);
  fseek(h, startPos, SEEK_SET);
  return fileSize;
}

FILE *__cdecl FS_FileOpenWriteBinary(const char *filename)
{
  FILE *file;

//  ProfLoad_BeginTrackedValue(0);
  file = FileWrapper_Open(filename, "wb");
//  ProfLoad_EndTrackedValue(0);
  return file;
}


FILE* __cdecl FS_FileOpenReadBinary(const char *filename)
{
  FILE *file;

//  ProfLoad_BeginTrackedValue(0);
  file = FileWrapper_Open(filename, "rb");
//  ProfLoad_EndTrackedValue(0);
  return file;
}

FILE* __cdecl FS_FileOpenWriteText(const char *filename)
{
  FILE *file;

//  ProfLoad_BeginTrackedValue(0);
  file = FileWrapper_Open(filename, "wt");
//  ProfLoad_EndTrackedValue(0);
  return file;
}

FILE *__cdecl FS_FileOpenReadText(const char *filename)
{
  FILE *file;

//  ProfLoad_BeginTrackedValue(0);
  file = FileWrapper_Open(filename, "rt");
//  ProfLoad_EndTrackedValue(0);
  return file;
}

int __cdecl FS_FileSeek(FILE *file, int offset, fsOrigin_t whence)
{
  int seek;

  //ProfLoad_BeginTrackedValue(MAP_PROFILE_FILE_SEEK);
  seek = FileWrapper_Seek(file, offset, whence);
  //ProfLoad_EndTrackedValue(MAP_PROFILE_FILE_SEEK);
  return seek;
}


void __cdecl FS_FileClose(FILE *stream)
{
  fclose(stream);
}

int __cdecl FS_FileGetFileSize(FILE *file)
{
  return FileWrapper_GetFileSize(file);
}

unsigned int __cdecl FS_FileRead(void *ptr, unsigned int len, FILE *stream)
{
  unsigned int read_size;

//  ProfLoad_BeginTrackedValue(MAP_PROFILE_FILE_READ);
  read_size = fread(ptr, 1u, len, stream);
//  ProfLoad_EndTrackedValue(MAP_PROFILE_FILE_READ);
  return read_size;
}

static fileHandle_t FS_HandleForFileForThread(int FsThread)
{
  signed int startIndex;
  signed int size;
  signed int i;

  if ( FsThread == 1 )
  {
    startIndex = 49;
    size = 13;
  }else if ( FsThread == 3 ){
    startIndex = 63;
    size = 1;
  }else if ( FsThread ){
    startIndex = 62;
    size = 1;
  }else{
    startIndex = 1;
	size = 48;
  }
  
  for (i = 0 ; size > i ; i++)
  {
    if ( fsh[i + startIndex].handleFiles.file.o == NULL )
    {
		return i + startIndex;
    }
  }
  
  if ( !FsThread )
  {
	for(i = 1; i < MAX_FILE_HANDLES; i++)
	{
		Com_Printf(CON_CHANNEL_FILES, "FILE %2i: '%s' 0x%x\n", i, fsh[i].name, (unsigned int)fsh[i].handleFiles.file.o);
	}
	Com_Error(2, "FS_HandleForFile: none free");
  }
  Com_PrintWarning(CON_CHANNEL_FILES, "FILE %2i: '%s' 0x%x\n", startIndex, fsh[startIndex].name, (unsigned int)fsh[startIndex].handleFiles.file.o);
  Com_PrintWarning(CON_CHANNEL_FILES, "FS_HandleForFile: none free (%d)\n", FsThread);
  return 0;
}


#define FS_HandleForFile() FS_HandleForFileForThread(0)


static FILE	*FS_FileForHandle( fileHandle_t f ) {

	if ( f < 0 || f > MAX_FILE_HANDLES ) {
		Com_Error( ERR_DROP, "FS_FileForHandle: out of range %i\n", f);
	}

	if ( !fsh[f].handleFiles.file.o ) {
		Com_Error( ERR_DROP, "FS_FileForHandle: NULL" );
	}

	return fsh[f].handleFiles.file.o;
}

/*
==============
FS_FCloseFile
If the FILE pointer is an open pak file, leave it open.
For some reason, other dll's can't just cal fclose()
on files returned by FS_FOpenFile...
==============
*/

qboolean REGPARM(1) FS_FCloseFile( fileHandle_t f ) {
	char fmsg[1024];
	
	if ( f < 0 || f > MAX_FILE_HANDLES ) {
		Com_Error( ERR_DROP, "FS_FCloseFile: out of range %i\n", f);
	}
	
	if ( fs_debug->integer > 1 )
	{
		Com_sprintf(fmsg, sizeof(fmsg), "^4Close filehandle: %d File: %s\n", f, fsh[f].name);
		Sys_Print(fmsg);
	}
	
	if(fsh[f].zipFile)
	{
		unzCloseCurrentFile(fsh[f].handleFiles.file.z);
		if(fsh[f].handleFiles.iwdIsClone)
		{
			unzClose((unz_s *)fsh[f].handleFiles.file.z);
		}else{
			assert(fsh[f].zipFile->hasOpenFile);
			fsh[f].zipFile->hasOpenFile = 0;
		}
		Com_Memset( &fsh[f], 0, sizeof( fsh[f] ) );
		return qtrue;
	}
	
	if (fsh[f].handleFiles.file.o) {
		// we didn't find it as a pak, so close it as a unique file
		fclose (fsh[f].handleFiles.file.o);
		Com_Memset( &fsh[f], 0, sizeof( fsh[f] ) );
		return qtrue;
	}
	Com_Memset( &fsh[f], 0, sizeof( fsh[f] ) );
	return qfalse;
}

/*
================
FS_filelength

If this is called on a non-unique FILE (from a pak file),
it will return the size of the pak file, not the expected
size of the file.
================
*/
int FS_filelength( fileHandle_t f ) {
	FILE*	h;
	
	FS_CheckFileSystemStarted();
	
	if ( fsh[f].zipFile )
	{
		return ((unz_s*)fsh[f].handleFiles.file.z)->cur_file_info.uncompressed_size;
	}

	h = FS_FileForHandle(f);
	return FS_FileGetFileSize(h);
}

unsigned int __cdecl FS_FTell(fileHandle_t f)
{
  FILE* h;

  if ( fsh[f].zipFile )
  {
    return unztell(fsh[f].handleFiles.file.z);
  }
  h = FS_FileForHandle(f);
  return ftell(h);
}

void FS_DisablePureCheck(int state)
{
  if (fs_gameDirVar->string[0])
    g_disablePureCheck = state;
}

char* FS_GetGameDir()
{
	return fs_gamedir;
}
/*
==========
FS_ShiftStr
perform simple string shifting to avoid scanning from the exe
==========
*/
void FS_ShiftStr( const char *string, int shift, char *buf )
{
	int i,l;

	l = strlen( string );
	for ( i = 0; i < l; i++ ) {
		buf[i] = string[i] + shift;
	}
	buf[i] = '\0';
}

/*
====================
FS_ReplaceSeparators

Fix things up differently for win/unix/mac
====================
*/
void FS_ReplaceSeparators( char *path ) {
	char	*s;

	for ( s = path ; *s ; s++ ) {
		if ( *s == '/' || *s == '\\' ) {
			*s = PATH_SEP;
		}
	}
}

/*
====================
FS_ReplaceSeparatorsUni

Fix things up differently for win/unix/mac
====================
*/
void FS_ReplaceSeparatorsUni( wchar_t *path ) {
	wchar_t	*s;

	for ( s = path ; *s ; s++ ) {
		if ( *s == L'/' || *s == L'\\' ) {
			*s = PATH_SEPUNI;
		}
	}
}

/*
====================
FS_StripTrailingSeperator

Fix things up differently for win/unix/mac
====================
*/
void FS_StripTrailingSeperator( char *path ) {

	int i = 1;
	int len = strlen(path);

	while(len > i && path[len -i] == PATH_SEP)
	{
		path[len -i] = '\0';
		++i;
	}

}

/*
====================
FS_StripTrailingSeperatorUni

Fix things up differently for win/unix/mac
====================
*/
static void FS_StripTrailingSeperatorUni( wchar_t *path ) {

	int i = 1;
	int len = wcslen(path);

	while(len > i && path[len -i] == PATH_SEPUNI)
	{
		path[len -i] = L'\0';
		++i;
	}
}


void FS_BuildOSPathForThread(const char *base, const char *game, const char *qpath, char *fs_path, int fs_thread)
{
  char basename[MAX_OSPATH];
  char gamename[MAX_OSPATH];

  int len;

  if ( !game || !*game )
    game = fs_gamedir;

  Q_strncpyz(basename, base, sizeof(basename));
  Q_strncpyz(gamename, game, sizeof(gamename));

  len = strlen(basename);
  if(len > 0 && (basename[len -1] == '/' || basename[len -1] == '\\'))
  {
        basename[len -1] = '\0';
  }

  len = strlen(gamename);
  if(len > 0 && (gamename[len -1] == '/' || gamename[len -1] == '\\'))
  {
        gamename[len -1] = '\0';
  }
  if ( Com_sprintf(fs_path, MAX_OSPATH, "%s/%s/%s", basename, gamename, qpath) >= MAX_OSPATH )
  {
    if ( fs_thread )
    {
        fs_path[0] = 0;
        return;
    }
    Com_Error(ERR_FATAL, "FS_BuildOSPath: os path length exceeded");
  }
  FS_ReplaceSeparators(fs_path);
  FS_StripTrailingSeperator(fs_path);

}

void FS_BuildOSPathForThreadUni(const wchar_t *base, const char *game, const char *qpath, wchar_t *fs_path, int fs_thread)
{
  wchar_t basename[MAX_OSPATH];
  wchar_t gamename[MAX_OSPATH];
  wchar_t qpathname[MAX_QPATH];
  int len;

  if ( !game || !*game )
    game = fs_gamedir;

  Q_strncpyzUni(basename, base, sizeof(basename));
  Q_StrToWStr(gamename, game, sizeof(gamename));
  Q_StrToWStr(qpathname, qpath, sizeof(qpathname));
  
  len = wcslen(basename);
  if(len > 0 && (basename[len -1] == L'/' || basename[len -1] == L'\\'))
  {
        basename[len -1] = L'\0';
  }

  len = wcslen(gamename);
  if(len > 0 && (gamename[len -1] == L'/' || gamename[len -1] == L'\\'))
  {
        gamename[len -1] = L'\0';
  }
  
  if( Com_sprintfUni(fs_path, sizeof(wchar_t) * MAX_OSPATH, L"%s/%s/%s", basename, gamename, qpathname) >= MAX_OSPATH )
  {
    if ( fs_thread )
    {
        fs_path[0] = 0;
        return;
    }
    Com_Error(ERR_FATAL, "FS_BuildOSPath: os path length exceeded");
  }
  FS_ReplaceSeparatorsUni(fs_path);
  FS_StripTrailingSeperatorUni(fs_path);
}



/*
===========
FS_WriteTestOSPath

===========
*/

qboolean FS_WriteTestOSPath( const char *osPath ) {
	
	FILE* fh = fopen( osPath, "wb" );
	char testbuf[] = "MZ_test";
	
	if(fh == NULL)
		return qfalse;
	
	fwrite(testbuf, 1, sizeof(testbuf), fh);
	fclose( fh );
	
	fh = fopen( osPath, "rb" );
	
	if(fh == NULL)
		return qfalse;
	fclose( fh );
	
	remove( osPath );
	
	return qtrue;
	
}



/*
===========
FS_RemoveOSPath

===========
*/
void FS_RemoveOSPath( const char *osPath ) {
	remove( osPath );
}

/*
===========
FS_RemoveOSPathUni

===========
*/
void FS_RemoveOSPathUni( const wchar_t *osPath ) {
	_wremove( osPath );
}

/*
===========
FS_SV_RemoveSavePath

===========
*/
void FS_SV_RemoveSavePath(char* path)
{
	wchar_t ospath[MAX_OSPATH];

	FS_BuildOSPathForThreadUni( FS_GetSavePath(), path, "", ospath, 0);
	
	FS_RemoveOSPathUni(ospath);
}

/*
===========
FS_SV_Remove

===========
*/

void FS_SV_Remove(char* qpath)
{
	char ospath[MAX_OSPATH];

	FS_BuildOSPathForThread( fs_homepath->string, qpath, "", ospath, 0);
	remove( ospath );
	FS_BuildOSPathForThread( fs_basepath->string, qpath, "", ospath, 0);
	remove( ospath );
}


/*
===========
FS_SV_RemoveDir

===========
*/
qboolean FS_SV_RemoveDir(char* qpath)
{
	wchar_t ospath[MAX_OSPATH];
	wchar_t basepath[MAX_OSPATH];
	qboolean del;
	
	del = qfalse;
	
	FS_BuildOSPathForThreadUni( FS_GetSavePath(), qpath, "", ospath, 0);
	if(Sys_RemoveDirTreeUni(ospath))
	{
		del = qtrue;
	}
	
	Q_StrToWStr(basepath, fs_homepath->string, sizeof(basepath));
	FS_BuildOSPathForThreadUni( basepath, qpath, "", ospath, 0);
	if(Sys_RemoveDirTreeUni(ospath))
	{
		del = qtrue;
	}
	
	Q_StrToWStr(basepath, fs_basepath->string, sizeof(basepath));
	FS_BuildOSPathForThreadUni( basepath, qpath, "", ospath, 0);
	if(Sys_RemoveDirTreeUni(ospath))
	{
		del = qtrue;
	}
	return del;
}



/*
===========
FS_Remove

===========
*/
/*
void FS_SV_Remove( const char *qPath ) {
	
	char	ospath1[MAX_OSPATH];
	char	ospath2[MAX_OSPATH];
	char	ospath3[MAX_OSPATH];
	char	ospath4[MAX_OSPATH];
	
	FS_BuildOSPathForThread( fs_homepath->string, "", qPath, ospath1, 0);
	FS_BuildOSPathForThread( fs_basepath->string, "", qPath, ospath2, 0);
	FS_BuildOSPathForThread( fs_homepath->string, qPath, "", ospath1, 0);
	FS_BuildOSPathForThread( fs_basepath->string, qPath, "", ospath2, 0);
	FS_StripTrailingSeperator( ospath1 );
	FS_StripTrailingSeperator( ospath2 );
	FS_StripTrailingSeperator( ospath3 );
	FS_StripTrailingSeperator( ospath4 );
	remove( ospath1 );
	remove( ospath2 );
	remove( ospath3 );
	remove( ospath4 );	
	
}
*/

/*
===========
FS_FileExistsOSPath

===========
*/
qboolean FS_FileExistsOSPath( const char *osPath ) {
	
	FILE* fh = fopen( osPath, "rb" );
	
	if(fh == NULL)
		return qfalse;
	
	fclose( fh );
	return qtrue;
	
}



qboolean FS_FileExists(const char *qpath)
{
	char ospath[MAX_OSPATH];

	FS_BuildOSPathForThread(fs_homepath->string, fs_gamedir, qpath, ospath, 0);
	return FS_FileExistsOSPath( ospath ); 
}
/*
===========
FS_Rename

===========
*/
void FS_Rename( const char *from, const char *to ) {
	char	from_ospath[MAX_OSPATH];
	char	to_ospath[MAX_OSPATH];

	FS_BuildOSPathForThread( fs_homepath->string, "", from, from_ospath, 0);
	FS_BuildOSPathForThread( fs_homepath->string, "", to, to_ospath, 0);
	FS_StripTrailingSeperator( to_ospath );
	FS_StripTrailingSeperator( from_ospath );

	if (rename( from_ospath, to_ospath )) {
		// Failed, try copying it and deleting the original
		FS_CopyFile ( from_ospath, to_ospath );
		FS_RemoveOSPath ( from_ospath );
	}
}

/*
===========
FS_SV_RenameSavePath

===========
*/
void FS_SV_RenameSavePath( const char *from, const char *to ) {
	wchar_t	from_ospath[MAX_OSPATH];
	wchar_t	to_ospath[MAX_OSPATH];

	FS_BuildOSPathForThreadUni( FS_GetSavePath(), from, "", from_ospath, 0);
	FS_BuildOSPathForThreadUni( FS_GetSavePath(), to, "", to_ospath, 0);

	if (_wrename( from_ospath, to_ospath )) {
		// Failed, try copying it and deleting the original
		FS_CopyFileUni( from_ospath, to_ospath );
		FS_RemoveOSPathUni ( from_ospath );
	}
}

/*
===========
FS_SV_Rename

===========
*/

/*
void FS_SV_Rename( const char *from, const char *to ) {
	char	from_ospath[MAX_OSPATH];
	char	to_ospath[MAX_OSPATH];

	FS_BuildOSPathForThread( fs_homepath->string, from, "", from_ospath, 0);
	FS_BuildOSPathForThread( fs_homepath->string, to, "", to_ospath, 0);
	FS_StripTrailingSeperator( to_ospath );
	FS_StripTrailingSeperator( from_ospath );

	Com_Printf("Rename from %s to %s\n", from_ospath, to_ospath);
//	Q_strcat(to_ospath, sizeof(to_ospath), "s");

//	if (rename( from_ospath, to_ospath )) {
		// Failed, try copying it and deleting the original
		FS_CopyFile ( from_ospath, to_ospath );
		//FS_RemoveOSPath ( from_ospath );
//	}
}
*/

/*
===========
FS_RenameOSPath

===========
*/
void FS_RenameOSPath( const char *from_ospath, const char *to_ospath ) {


	if (rename( from_ospath, to_ospath )) {
		// Failed, try copying it and deleting the original
		FS_CopyFile ( (char*)from_ospath, (char*)to_ospath );
		FS_RemoveOSPath ( from_ospath );
	}
}



/*
=================
FS_CopyFile

Copy a fully specified file from one place to another
=================
*/
void FS_CopyFile( char *fromOSPath, char *toOSPath ) {
	FILE    *f;
	int len;
	byte    *buf;

	Sys_EnterCriticalSection(CRIT_FILESYSTEM);

	f = fopen( fromOSPath, "rb" );
	if ( !f ) {
		Sys_LeaveCriticalSection(CRIT_FILESYSTEM);
		return;
	}
	fseek( f, 0, SEEK_END );
	len = ftell( f );
	fseek( f, 0, SEEK_SET );

	// we are using direct malloc instead of Z_Malloc here, so it
	// probably won't work on a mac... Its only for developers anyway...
	buf = malloc( len );
	if ( fread( buf, 1, len, f ) != len ) {
		Com_Error( ERR_FATAL, "Short read in FS_Copyfiles()\n" );
	}
	fclose( f );

	if ( FS_CreatePath( toOSPath ) ) {
		free( buf );
		Sys_LeaveCriticalSection(CRIT_FILESYSTEM);
		return;
	}

	f = fopen( toOSPath, "wb" );
	if ( !f ) {
		free( buf );    //DAJ free as well
		Sys_LeaveCriticalSection(CRIT_FILESYSTEM);
		return;
	}
	if ( fwrite( buf, 1, len, f ) != len ) {
		Sys_LeaveCriticalSection(CRIT_FILESYSTEM);
		Com_Error( ERR_FATAL, "Short write in FS_Copyfiles()\n" );
	}
	fclose( f );
	free( buf );
	Sys_LeaveCriticalSection(CRIT_FILESYSTEM);

}

/*
=================
FS_CopyFileUni

Copy a fully specified file from one place to another
=================
*/
qboolean FS_CopyFileUni( wchar_t *fromOSPath, wchar_t *toOSPath )
{
	qboolean suc;

	Sys_EnterCriticalSection(CRIT_FILESYSTEM);

	if ( FS_CreatePathUni( toOSPath ) ) {
		Sys_LeaveCriticalSection(CRIT_FILESYSTEM);
		return qfalse;
	}

	suc = Sys_CopyFileUni(fromOSPath, toOSPath);
	
	Sys_LeaveCriticalSection(CRIT_FILESYSTEM);
	return suc;
}

/*
=================
FS_SV_CopyFileFromOsPathToSavePath

Copy a fully specified file from one place to another
=================
*/
qboolean FS_SV_CopyFromOSPathToSavePath( const char *fromOSPath, const char* to )
{
	wchar_t toOSPathUni[MAX_OSPATH];
	wchar_t fromOSPathUni[MAX_OSPATH];
	
	Q_StrToWStr(fromOSPathUni, fromOSPath, sizeof(fromOSPathUni));
	
	FS_BuildOSPathForThreadUni( FS_GetSavePath(), to, "", toOSPathUni, 0);
	
	FS_ReplaceSeparatorsUni(fromOSPathUni);
	FS_StripTrailingSeperatorUni(fromOSPathUni);
	
	return FS_CopyFileUni( fromOSPathUni, toOSPathUni );

}

/*
=================
FS_SV_CopyFromBaseToSavePath

Copy a fully specified file from one place to another
=================
*/
qboolean FS_SV_CopyFromBaseToSavePath( const char* base, const char *from, const char* to )
{

	char fromOSPath[MAX_OSPATH];

	
	FS_BuildOSPathForThread( base, from, "", fromOSPath, 0);
	
	return FS_SV_CopyFromOSPathToSavePath(fromOSPath ,to);

}

/*
============
FS_CreatePath

Creates any directories needed to store the given filename
============
*/
qboolean FS_CreatePath (char *OSPath) {
	char	*ofs;

	// make absolutely sure that it can't back up the path
	// FIXME: is c: allowed???
	if ( strstr( OSPath, ".." ) || strstr( OSPath, "::" ) ) {
		Com_Printf(CON_CHANNEL_FILES, "WARNING: refusing to create relative path \"%s\"\n", OSPath );
		return qtrue;
	}

	for (ofs = OSPath+1 ; *ofs ; ofs++) {
		if (*ofs == PATH_SEP) {	
			// create the directory
			*ofs = 0;
			Sys_Mkdir (OSPath);
			*ofs = PATH_SEP;
		}
	}
	return qfalse;
}

/*
============
FS_CreatePathUni

Creates any directories needed to store the given filename
============
*/
qboolean FS_CreatePathUni (wchar_t *OSPath) {
	wchar_t	*ofs;

	// make absolutely sure that it can't back up the path
	// FIXME: is c: allowed???
	if ( wcsstr( OSPath, L".." ) || wcsstr( OSPath, L"::" ) ) {
		Com_Printf(CON_CHANNEL_FILES, "WARNING: refusing to create relative unicode path\n" );
		return qtrue;
	}

	if(OSPath[0] == L'\0')
	{
		return qtrue;
	}
	
	for (ofs = OSPath+1 ; *ofs ; ofs++) {
		if (*ofs == PATH_SEPUNI) {	
			// create the directory
			*ofs = 0;
			Sys_MkdirUni(OSPath);
			*ofs = PATH_SEPUNI;
		}
	}
	return qfalse;
}


#define FS_filelengthOSPath FS_FileGetFileSize

int FS_FileLengthOSPathByName( const char *osPath ) {
	
	FILE* fh = fopen( osPath, "rb" );
	
	if(fh == NULL)
		return -1;
	
	int len = FS_FileGetFileSize(fh);

	fclose( fh );
	return len;
	
}

/*
===========
FS_FOpenFileReadOSPath
search for a file somewhere below the home path, base path or cd path
we search in that order, matching FS_SV_FOpenFileRead order
===========
*/
int FS_FOpenFileReadOSPath( const char *filename, FILE **fp ) {
	char ospath[MAX_OSPATH];
	FILE* fh;
	
	Q_strncpyz( ospath, filename, sizeof( ospath ) );

	fh = fopen( ospath, "rb" );

	if ( !fh ){
		*fp = NULL;
		return -1;
	}

	*fp = fh;

	return FS_filelengthOSPath(fh);
}

/*
===========
FS_FOpenFileReadOSPathUni
search for a file somewhere below the home path, base path or cd path
we search in that order, matching FS_SV_FOpenFileRead order
===========
*/
int FS_FOpenFileReadOSPathUni( const wchar_t *filename, FILE **fp ) {
	wchar_t ospath[MAX_OSPATH];
	FILE* fh;
	
	Q_strncpyzUni( ospath, filename, sizeof( ospath ) );

	fh = _wfopen( ospath, L"rb" );

	if ( !fh ){
		*fp = NULL;
		return -1;
	}

	*fp = fh;

	return FS_filelengthOSPath(fh);
}

/*
==============
FS_FCloseFileOSPath

==============
*/
qboolean FS_FCloseFileOSPath( FILE* f ) {

	if (f) {
	    fclose (f);
	    return qtrue;
	}
	return qfalse;
}

/*
=================
FS_ReadOSPath

Properly handles partial reads
=================
*/
int FS_ReadOSPath( void *buffer, int len, FILE* f ) {
	int		block, remaining;
	int		read;
	byte	*buf;

	if ( !f ) {
		return 0;
	}

	buf = (byte *)buffer;

	remaining = len;
	while (remaining) {
		block = remaining;
		read = fread (buf, 1, block, f);
		if (read == 0)
		{
			return len-remaining;	//Com_Error (ERR_FATAL, "FS_Read: 0 bytes read");
		}

		if (read == -1) {
			Com_Error (ERR_FATAL, "FS_ReadOSPath: -1 bytes read");
		}

		remaining -= read;
		buf += read;
	}
	return len;

}

/*
============
FS_ReadFileOSPath

Filename are relative to the quake search path
a null buffer will just return the file length without loading
============
*/
int FS_ReadFileOSPath( const char *ospath, void **buffer ) {
	byte*	buf;
	int		len;
	FILE*   h;
	
	
	if ( !ospath || !ospath[0] ) {
		Com_Error( ERR_FATAL, "FS_ReadFile with empty name\n" );
	}

	buf = NULL;	// quiet compiler warning

	// look for it in the filesystem or pack files
	len = FS_FOpenFileReadOSPath( ospath, &h );
	if ( len == -1 ) {
		if ( buffer ) {
			*buffer = NULL;
		}
		return -1;
	}
	
	if ( !buffer ) {
		FS_FCloseFileOSPath( h );
		return len;
	}

	buf = Hunk_AllocateTempMemory(len+1);
	++fs_loadStack;
	*buffer = buf;

	FS_ReadOSPath (buf, len, h);

	// guarantee that it will have a trailing 0 for string operations
	buf[len] = 0;
	FS_FCloseFileOSPath( h );
	return len;
}

/*
============
FS_ReadFileOSPathUni

Filename are relative to the quake search path
a null buffer will just return the file length without loading
============
*/
int FS_ReadFileOSPathUni( const wchar_t *ospath, void **buffer ) {
	byte*	buf;
	int		len;
	FILE*   h;
	
	
	if ( !ospath || !ospath[0] ) {
		Com_Error( ERR_FATAL, "FS_ReadFileOSPathUni with empty name\n" );
	}

	buf = NULL;	// quiet compiler warning

	// look for it in the filesystem or pack files
	len = FS_FOpenFileReadOSPathUni( ospath, &h );
	if ( len == -1 ) {
		if ( buffer ) {
			*buffer = NULL;
		}
		return -1;
	}
	
	if ( !buffer ) {
		FS_FCloseFileOSPath( h );
		return len;
	}

	buf = Hunk_AllocateTempMemory(len+1);
	++fs_loadStack;
	*buffer = buf;

	FS_ReadOSPath (buf, len, h);

	// guarantee that it will have a trailing 0 for string operations
	buf[len] = 0;
	FS_FCloseFileOSPath( h );
	return len;
}

int	FS_FOpenFileRead( const char* qpath, fileHandle_t *fh)
{
	return FS_FOpenFileReadForThread( qpath, fh, 0);
}

/*
===========
FS_SV_FOpenFileReadOSPath
search for a file somewhere below the home path, base path or cd path
we search in that order, matching FS_SV_FOpenFileRead order
===========
*/
int FS_SV_FOpenFileReadOSPath( const char *filename, fileHandle_t *fp ) {
	char ospath[MAX_OSPATH];
	fileHandle_t	f = 0;

	f = FS_HandleForFile();
	if(f == 0){
		*fp = 0;
		return 0;
	}

	fsh[f].zipFile = qfalse;
	Q_strncpyz( fsh[f].name, filename, sizeof( fsh[f].name ) );
	Q_strncpyz( ospath, filename, sizeof(ospath) );
	
	FS_ReplaceSeparators(ospath);

	fsh[f].handleFiles.file.o = fopen( ospath, "rb" );
	fsh[f].handleSync = qfalse;

	if ( !fsh[f].handleFiles.file.o )
	{
		f = 0;
	}

	*fp = f;
	if (f) {
		return FS_filelength(f);
	}
	return 0;
}


/*
===========
FS_SV_FOpenFileRead
===========
*/
int FS_SV_FOpenFileRead( const char *filename, fileHandle_t *fp ) {
	char ospath[MAX_OSPATH];
	wchar_t ospathw[MAX_OSPATH];
	fileHandle_t f = 0;

	if ( !FS_Initialized() ) {
		Com_Error( ERR_FATAL, "Filesystem call made without initialization\n" );
	}

	f = FS_HandleForFile();

	if(f == 0){
		return 0;
	}

	fsh[f].zipFile = qfalse;
	fsh[f].handleSync = qfalse;
	Q_strncpyz( fsh[f].name, filename, sizeof( fsh[f].name ) );

	//Search in fs_savepath
	FS_BuildOSPathForThreadUni( FS_GetSavePath(), filename, "", ospathw, 0 );
	if ( fs_debug->integer ) {
		Com_DPrintf(CON_CHANNEL_FILES, "FS_SV_FOpenFileRead trying: fs_savepath:%s\n", filename );
	}
	fsh[f].handleFiles.file.o = _wfopen( ospathw, L"rb" );
	if ( fsh[f].handleFiles.file.o ) {
		*fp = f;
		return FS_filelength(f);
	}
	
	//Search in fs_homepath
	FS_BuildOSPathForThread( fs_homepath->string, filename, "", ospath, 0 );
	if ( fs_debug->integer ) {
		Com_DPrintf(CON_CHANNEL_FILES, "FS_SV_FOpenFileRead trying: %s\n", ospath );
	}
	fsh[f].handleFiles.file.o = fopen( ospath, "rb" );
	if ( fsh[f].handleFiles.file.o ) {
		*fp = f;
		return FS_filelength(f);
	}

	//Search in fs_basepath
	FS_BuildOSPathForThread( fs_basepath->string, filename, "", ospath, 0 );
	if ( fs_debug->integer ) {
		Com_DPrintf(CON_CHANNEL_FILES, "FS_SV_FOpenFileRead trying: %s\n", ospath );
	}
	fsh[f].handleFiles.file.o = fopen( ospath, "rb" );
	if ( fsh[f].handleFiles.file.o ) {
		*fp = f;
		return FS_filelength(f);
	}
	
	
	if ( fs_debug->integer ) {
		Com_DPrintf(CON_CHANNEL_FILES, "FS_SV_FOpenFileRead failed: %s\n", filename );
	}
	*fp = 0;
	return 0;
}

void __cdecl FS_ShutdownSearchPaths(struct searchpath_s *p)
{
  struct searchpath_s *next;
  
  while ( p )
  {
    next = p->next;
    if ( p->pack )
    {
      unzClose((unz_s *)p->pack->handle);
      Z_Free(p->pack->buildBuffer);
      Z_Free(p->pack);
    }
    if ( p->dir )
    {
      Z_Free(p->dir);
    }
    Z_Free(p);
    p = next;
  }
}

void FS_ShutdownSearchPathsCoD4( )
{
	FS_ShutdownSearchPaths(fs_searchpaths);
}


void __cdecl FS_RemoveCommands()
{
  Cmd_RemoveCommand("path");
  Cmd_RemoveCommand("fullpath");
  Cmd_RemoveCommand("dir");
  Cmd_RemoveCommand("fdir");
  Cmd_RemoveCommand("touchFile");
}




/*
============
FS_SV_ReadFile

Filename are relative to the quake search path
a null buffer will just return the file length without loading
============
*/
int FS_SV_ReadFile( const char *qpath, void **buffer ) {
	fileHandle_t	h;
	byte*			buf;
	int			len;

	if ( !qpath || !qpath[0] ) {
		Com_Error( ERR_FATAL, "FS_SV_ReadFile with empty name\n" );
	}

	buf = NULL;	// quiet compiler warning

	// look for it in the filesystem or pack files
	len = FS_SV_FOpenFileRead( qpath, &h );
	if ( h == 0 ) {
		if ( buffer ) {
			*buffer = NULL;
		}
		return -1;
	}
	
	if ( !buffer ) {
		FS_FCloseFile( h);
		return len;
	}

	buf = Hunk_AllocateTempMemory(len+1);
	*buffer = buf;
	++fs_loadStack;
	
	FS_Read (buf, len, h);

	// guarantee that it will have a trailing 0 for string operations
	buf[len] = 0;
	FS_FCloseFile( h );
	return len;
}





qboolean FS_NeedRestart(int requestChecksumFeed)
{
	if ( !com_sv_running->boolean && (fs_gameDirVar->modified || requestChecksumFeed != fs_checksumFeed) )
	{
		return qtrue;
	}	
	return qfalse;
}




int FS_ServerSetReferencedFiles(const char *sums, const char *names, int *sumsNummeric, const char **stringList)
{
	int numSumTokens;
	int i;
	int numNameTokens;
	int j;

	Cmd_TokenizeString( sums );
	numSumTokens = Cmd_Argc();

	if ( numSumTokens > 1024 )
	{
		numSumTokens = 1024;
	}

	for ( i = 0; i < numSumTokens; ++i )
	{
		sumsNummeric[i] = atol(Cmd_Argv(i));
	}

	Cmd_EndTokenizedString();
	
	if ( !names || !*names )
	{
	    if ( numSumTokens )
		{
			Com_Error(ERR_DROP, "file sum/name mismatch (Sum defined but not a name)\nSums: %s", sums);
		}
		return numSumTokens;
	}
	
	Cmd_TokenizeString( names );
    numNameTokens = Cmd_Argc();
	
    if ( numNameTokens > 1024 )
	{
		numNameTokens = 1024;
	}  
    
	if ( numSumTokens != numNameTokens )
    {
		Com_Error(ERR_DROP, "file sum/name mismatch (count of sums and names mismatch)\nNames: %s\nSums: %s", names, sums);
    }
	
    for ( j = 0; j < numNameTokens; ++j )
    {
		stringList[j] = CopyString(Cmd_Argv(j));
    }
	Cmd_EndTokenizedString();
    return numSumTokens;
}


void FS_ShutdownServerIwdNames()
{
	FS_ShutdownReferencedFiles(&fs_numServerIwds, fs_serverIwdNames);
}
void FS_ShutdownServerReferencedIwds()
{
	FS_ShutdownReferencedFiles(&fs_numServerReferencedIwds, fs_serverReferencedIwdNames);
}
void FS_ShutdownServerReferencedFFs()
{
	FS_ShutdownReferencedFiles(&fs_numServerReferencedFFs, fs_serverReferencedFFNames);
}

void FS_ServerSetReferencedIwds(const char* sums, const char* names)
{
    FS_ShutdownServerReferencedIwds();
    fs_numServerReferencedIwds = FS_ServerSetReferencedFiles(sums, names, fs_serverReferencedIwdSums, fs_serverReferencedIwdNames);
}

void FS_ServerSetReferencedFFs(const char* sums, const char* names)
{
    FS_ShutdownServerReferencedFFs();
    fs_numServerReferencedFFs = FS_ServerSetReferencedFiles(sums, names, fs_serverReferencedFFSums, fs_serverReferencedFFNames);

}

qboolean FS_ExistsInReferencedFFs(const char* name)
{
	int i;

	char normalname[1024];
	char normalcmp[1024];

	Q_strncpyz(normalname, name, sizeof(normalname));
	FS_ReplaceSeparators(normalname);

	for(i = 0; i < fs_numServerReferencedFFs; ++i)
	{
		Q_strncpyz(normalcmp, fs_serverReferencedFFNames[i], sizeof(normalcmp));
		FS_ReplaceSeparators(normalcmp);

		if(Q_stricmp(normalcmp, normalname) == 0)
		{
			return qtrue;
		}
	}
	return qfalse;
}

void FS_ShutdownIwdPureCheckReferences()
{

    iwdPureReferences_t *fipc, *freefipc;
	
	for(fipc = fs_iwdPureChecks; fipc; )
	{
		freefipc = fipc;
		fipc = fipc->next;
		Z_Free(freefipc);
	}
    fs_iwdPureChecks = 0;
}


signed int FS_CompareFiles(char *downloadlist, int len, qboolean dlstring)
{
  unsigned int paklen;
  const char *ffList;
  signed int ffcmp;
  int pakcmp;

  *downloadlist = 0;
  pakcmp = FS_ComparePaks(downloadlist, dlstring);
  if ( pakcmp == 2 )
  {
    return 2;
  }
  paklen = strlen(downloadlist);
  ffList = &downloadlist[paklen];
  ffcmp = FS_CompareFastFiles(&downloadlist[paklen], len - paklen, dlstring);
  if ( ffcmp == 2 )
  {
    if ( paklen > 0 )
    {
      Q_strncpyz(downloadlist, ffList, len);
    }
    return 2;
  }
  if(pakcmp == 1 || ffcmp == 1)
  {
	return 1;
  }
  return 0;
}


void FS_AddUserMapDirIWD(const char *file)
{
  searchpath_t *s;
  char ospath[MAX_OSPATH];

  for( s = fs_searchpaths ; s ; s = s->next)
  {
    if ( s->pack && !Q_stricmp(s->pack->pakGamename, file) )
	{
		return;
	}
  }
  
  FS_BuildOSPathForThread(fs_homepath->string, file, "", ospath, 0);
  if(FS_FileExistsOSPath( ospath ))
  {
	FS_AddIwdFilesForGameDirectory(fs_homepath->string, file);
	return;
  }
  FS_BuildOSPathForThread(fs_basepath->string, file, "", ospath, 0);
  if(FS_FileExistsOSPath( ospath ))
  {
	FS_AddIwdFilesForGameDirectory(fs_basepath->string, file);
	return;
  }
  FS_AddIwdFilesForGameDirectory(fs_homepath->string, file);
}


qboolean FS_Initialized()
{
	if(fs_searchpaths != NULL)
		return qtrue;
	return qfalse;
}

void FS_Flush( fileHandle_t f )
{
	fflush(fsh[f].handleFiles.file.o);
}

void FS_ForceFlush( fileHandle_t f ) {
	FILE *file;
	file = FS_FileForHandle(f);
	setvbuf( file, NULL, _IONBF, 0 );
}




/*
===========
FS_FOpenFileWrite
===========
*/
fileHandle_t FS_FOpenFileWrite( const char *filename ) {
	char ospath[MAX_OSPATH];
	fileHandle_t f;

	if ( !FS_Initialized() ) {
		Com_Error( ERR_FATAL, "Filesystem call made without initialization\n" );
	}

	f = FS_HandleForFile();

	if(f == 0){
		return 0;
	}

	fsh[f].zipFile = qfalse;
	FS_BuildOSPathForThread( fs_homepath->string, fs_gamedir, filename, ospath, 0 );
	if ( fs_debug->integer ) {
		Com_DPrintf(CON_CHANNEL_FILES, "FS_FOpenFileWrite: %s\n", ospath );
	}
	if ( FS_CreatePath( ospath ) ) {
		return 0;
	}
	// enabling the following line causes a recursive function call loop
	// when running with +set logfile 1 +set developer 1
	//Com_DPrintf( "writing to: %s\n", ospath );
	fsh[f].handleFiles.file.o = fopen( ospath, "wb" );
	Q_strncpyz( fsh[f].name, filename, sizeof( fsh[f].name ) );
	fsh[f].handleSync = qfalse;
	if ( !fsh[f].handleFiles.file.o ) {
		f = 0;
	}
	return f;
}

/*
============
FS_SV_WriteFileToSavePath
 
Filename are reletive to the quake search path
============
*/
int FS_SV_WriteFileToSavePath( const char *qpath, const void *buffer, int size ) {
  fileHandle_t f;
  int len;
  
  if ( !FS_Initialized() ) {
    Com_Error( ERR_FATAL, "Filesystem call made without initialization\n" );
  }

  if ( !qpath || !buffer ) {
    Com_Error( ERR_FATAL, "FS_WriteFile: NULL parameter" );
  }

  f = FS_SV_FOpenFileWriteSavePath( qpath );
  if ( !f ) {
    Com_Printf(CON_CHANNEL_FILES, "Failed to open %s\n", qpath );
    return 0;
  }

  len = FS_Write( buffer, size, f );

  FS_FCloseFile( f );
  
  return len;
}


/*
===========
FS_SV_FOpenFileWriteSavePath
===========
*/
fileHandle_t FS_SV_FOpenFileWriteSavePath( const char *filename ) {
	wchar_t ospath[MAX_OSPATH];
	fileHandle_t f;

	if ( !FS_Initialized() ) {
		Com_Error( ERR_FATAL, "Filesystem call made without initialization\n" );
	}

	f = FS_HandleForFile();

	if(f == 0){
		return 0;
	}

	fsh[f].zipFile = qfalse;
	FS_BuildOSPathForThreadUni( FS_GetSavePath(), filename, "", ospath, 0 );

	if ( fs_debug->integer ) {
		Com_DPrintf(CON_CHANNEL_FILES, "FS_SV_FOpenFileWriteSavePath fs_savepath:%s\n", filename );
	}
	if ( FS_CreatePathUni( ospath ) ) {
		return 0;
	}
	// enabling the following line causes a recursive function call loop
	// when running with +set logfile 1 +set developer 1
	//Com_DPrintf( "writing to: %s\n", ospath );
	fsh[f].handleFiles.file.o = _wfopen( ospath, L"wb" );
	Q_strncpyz( fsh[f].name, filename, sizeof( fsh[f].name ) );
	fsh[f].handleSync = qfalse;
	if ( !fsh[f].handleFiles.file.o ) {
		f = 0;
	}
	return f;
}

/*
=================
FS_Write
Properly handles partial writes
=================
*/
int FS_Write( const void *buffer, int len, fileHandle_t h ) {
	int	block, remaining;
	int	written;
	byte	*buf;
	int	tries;
	FILE	*f;
	
	if ( !FS_Initialized() ) {
		Com_Error( ERR_FATAL, "Filesystem call made without initialization" );
	}
	if ( !h ) {
		return 0;
	}
	f = FS_FileForHandle(h);
	buf = (byte *)buffer;
	remaining = len;
	tries = 0;
	while (remaining) {
		block = remaining;
		written = fwrite (buf, 1, block, f);
		if (written == 0) {
			if (!tries) {
				tries = 1;
			} else {
				Com_Printf(CON_CHANNEL_FILES, "FS_Write: 0 bytes written\n" );
				return 0;
			}
		}
		if (written == -1) {
			Com_Printf(CON_CHANNEL_FILES, "FS_Write: -1 bytes written\n" );
			return 0;
		}
		remaining -= written;
		buf += written;
	}
	if ( fsh[h].handleSync ) {
		fflush( f );
	}
	return len;
}

void FS_SetupSavePath()
{
	char multibyte[4*MAX_OSPATH];

	if(Sys_GetAppDataDir(fs_savepathDir, sizeof(fs_savepathDir)) == NULL)
	{
		Q_StrToWStr(fs_savepathDir, fs_homepath->string, sizeof(fs_savepathDir));
	}
	wcstombs(multibyte, fs_savepathDir, sizeof(multibyte));
	Cvar_RegisterString("fs_savepath", multibyte, CVAR_INIT, "The savepath for user specific settings like profiles and stats");
}

wchar_t* FS_GetSavePath()
{
	return fs_savepathDir;
}

qboolean FS_SV_DirExists(char* qpath)
{
	wchar_t unipath[MAX_OSPATH];
	wchar_t dir[MAX_OSPATH];
	qboolean exists;
	
	FS_BuildOSPathForThreadUni(FS_GetSavePath(), qpath, "", dir, 0);
	exists = Sys_DirectoryExistsUni( dir );
	if(exists)
	{
		return qtrue;
	}
	
	Q_StrToWStr(unipath, fs_homepath->string, sizeof(unipath));
	FS_BuildOSPathForThreadUni(unipath, qpath, "", dir, 0);
	exists = Sys_DirectoryExistsUni( dir );
	if(exists)
	{
		return qtrue;
	}
	
	Q_StrToWStr(unipath, fs_basepath->string, sizeof(unipath));
	FS_BuildOSPathForThreadUni(unipath, qpath, "", dir, 0);
	exists = Sys_DirectoryExistsUni( dir );
	if(exists)
	{
		return qtrue;
	}
	
	return qfalse;
}



void QDECL __attribute__ ((format (printf, 2, 3))) FS_Printf( fileHandle_t f, const char *fmt, ... )
{
	va_list		argptr;
	char		msg[4096];

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

	FS_Write(msg, strlen(msg), f);
}

void FS_FreeFile(void* buf)
{
	--fs_loadStack;
    Hunk_FreeTempMemory(buf);
}

char* findinlist(char** list, char* search)
{
	int i;

	for(i = 0; list[i]; ++i)
	{
		if(Q_stricmp(list[i], search) == 0)
		{
			return list[i];
		}
	}
	return NULL;
}


int FS_SV_ListDirectories(const char* dir, char** list, int limit)
{
	char* names[1024];
	int num, i, z;
	wchar_t basepath[MAX_OSPATH];
	wchar_t homepath[MAX_OSPATH];
	wchar_t savepath[MAX_OSPATH];
	wchar_t tmppath[MAX_OSPATH];
	
	wchar_t* basedir[] = { savepath, homepath, basepath, NULL};
	
	num = 0;
	list[num] = NULL;
	
	FS_BuildOSPathForThreadUni(FS_GetSavePath(), dir, "", savepath, 0);
	
	Q_StrToWStr(tmppath, fs_basepath->string, sizeof(tmppath));
	FS_BuildOSPathForThreadUni(tmppath, dir, "", basepath, 0);
	
	Q_StrToWStr(tmppath, fs_homepath->string, sizeof(tmppath));
	FS_BuildOSPathForThreadUni(tmppath, dir, "", homepath, 0);
	
	for(z = 0; basedir[z]; ++z)
	{

		if( Sys_ListDirectories(basedir[z], names, 1024) > 0)
		{
			for(i = 0; i < 1024 && num < (limit -1); ++i)
			{
				list[num] = NULL;
				
				if(names[i] == NULL)
				{
					break;
				}

				if(findinlist(list, names[i]))
				{
					free(names[i]);
					continue;
				}
				
				list[num] = names[i];
				num++;

			}
		}
	
	
	}
	
	return num;
}

void FS_ClearChecksumFeed()
{
	fs_checksumFeed = 0;
}

/*
============
FS_Path_f

============
*/
void FS_Path_f( void ) {
	searchpath_t	*s;
	int				i;

	Com_Printf (CON_CHANNEL_DONT_FILTER, "Current search path:\n");
	for (s = fs_searchpaths; s; s = s->next) {
		if (s->pack) {
			Com_Printf (CON_CHANNEL_DONT_FILTER, "%s (%i files)\n", s->pack->pakFilename, s->pack->numFiles);
		} else {
			Com_Printf (CON_CHANNEL_DONT_FILTER, "%s%c%s\n", s->dir->path, PATH_SEP, s->dir->gamedir );
		}
	}

	Com_Printf(CON_CHANNEL_DONT_FILTER, "\n" );
	for ( i = 1 ; i < MAX_FILE_HANDLES ; i++ ) {
		if ( fsh[i].handleFiles.file.o ) {
			Com_Printf(CON_CHANNEL_DONT_FILTER, "handle %i: %s\n", i, fsh[i].name );
		}
	}
}

/*
============
FS_Path_f

============
*/
void FS_PathToBuffer( char* buf, int maxlen )
{
	searchpath_t	*s;
	int				i;
	char line[1024];

	Q_strcat(buf, maxlen, "Current search path:\n");
	for (s = fs_searchpaths; s; s = s->next) {
		if (s->pack) {
			Com_sprintf (line, sizeof(line), "%s (%i files)\n", s->pack->pakFilename, s->pack->numFiles);
			Q_strcat(buf, maxlen, line);
		} else {
			Com_sprintf (line, sizeof(line), "%s%c%s\n", s->dir->path, PATH_SEP, s->dir->gamedir );
			Q_strcat(buf, maxlen, line);
		}
	}

	Q_strcat(buf, maxlen, "\n" );
	for ( i = 1 ; i < MAX_FILE_HANDLES ; i++ ) {
		if ( fsh[i].handleFiles.file.o ) {
			Com_sprintf (line, sizeof(line),  "handle %i: %s\n", i, fsh[i].name );
			Q_strcat(buf, maxlen, line);
		}
	}
}



int FS_SkipDataInCurrentZipFile(unzFile file, unsigned int len)
{
	char dummybuffer[0x10000];
	int efflen;
	int readlen = 0;
	int r;


	while(len > 0)
	{
		if(len > sizeof(dummybuffer))
		{
			efflen = sizeof(dummybuffer);
		}else{
			efflen = len;
		}

		r = unzReadCurrentFile(file, dummybuffer, efflen);
		if(r > 0)
		{
			readlen += r;
		}
		len -= efflen;
	}
	return readlen;
}


int __cdecl FS_Seek(fileHandle_t f, long offset, fsOrigin_t origin)
{
  int iZipPos; 
  unsigned int iZipOffset;

  FS_CheckFileSystemStarted();
  assert(!fsh[f].streamed);

  if ( !fsh[f].zipFile )
  {
    return FS_FileSeek(FS_FileForHandle(f), offset, origin);
  }
  if ( !offset && origin == FS_SEEK_SET)
  {
    unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
    return unzOpenCurrentFile(fsh[f].handleFiles.file.z);
  }
  if ( !offset && origin == FS_SEEK_CUR )
  {
    return 0;
  }
  iZipPos = unztell(fsh[f].handleFiles.file.z);
  switch ( origin )
  {
    case FS_SEEK_CUR:
	  assert(offset != 0);
      if ( offset >= 0 )
      {
        iZipOffset = offset;
      }
      else
      {
        unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
        unzOpenCurrentFile(fsh[f].handleFiles.file.z);
        iZipOffset = offset + iZipPos;
      }
	  break;
    case FS_SEEK_END:
      if ( offset + FS_filelength(f) >= iZipPos )
      {
        iZipOffset = offset + FS_filelength(f) - iZipPos;
      }
      else
      {
        unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
        unzOpenCurrentFile(fsh[f].handleFiles.file.z);
        iZipOffset = offset + FS_filelength(f);
      }
	  break;
    case FS_SEEK_SET:
      if ( offset >= iZipPos )
      {
        iZipOffset = offset - iZipPos;
      }
      else
      {
        unzSetCurrentFileInfoPosition(fsh[f].handleFiles.file.z, fsh[f].zipFilePos);
        unzOpenCurrentFile(fsh[f].handleFiles.file.z);
        iZipOffset = offset;
      }
	  break;
	default:
	  Com_Error(ERR_FATAL, "Bad origin %i in FS_Seek", origin);
      return -1;
  }
  if ( FS_SkipDataInCurrentZipFile(fsh[f].handleFiles.file.z, iZipOffset) )
  {
    return 0;
  }
  return -1;
}




/*
============
FS_WriteFile

Filename are reletive to the quake search path
============
*/
int FS_WriteFile( const char *qpath, const void *buffer, int size ) {
	fileHandle_t f;
	int len;

	if ( !FS_Initialized() ) {
		Com_Error( ERR_FATAL, "Filesystem call made without initialization" );
	}

	if ( !qpath || !buffer ) {
		Com_Error( ERR_FATAL, "FS_WriteFile: NULL parameter" );
		return -1;
	}

	f = FS_FOpenFileWrite( qpath );
	if ( !f ) {
		Com_Printf(CON_CHANNEL_FILES, "Failed to open %s\n", qpath );
		return -1;
	}

	len = FS_Write( buffer, size, f );

	FS_FCloseFile( f );

	return len;

}


void DB_BuildOSPath(const char *filename, int ff_dir, int pathlen, char *path)
{
  char *find;
  char mapname[64];
  
  switch(ff_dir)
  {
	  case 1:
	  		Com_sprintf(path, pathlen, "%s/%s/%s.ff", Sys_DefaultInstallPath(), fs_gameDirVar->string, filename);
			break;
	  case 2:
	        
			Q_strncpyz(mapname, filename, sizeof(mapname));
            find = strstr(mapname, "_load");
            if ( find )
            {
                find[0] = '\0';
            }
   			Com_sprintf(path, pathlen, "%s/%s/%s/%s.ff", Sys_DefaultInstallPath(), "usermaps", mapname, filename);
			break;
	  default:
			Com_sprintf(path, pathlen, "%s/zone/%s/%s.ff", Sys_DefaultInstallPath(), Win_GetLanguage(), filename);
			break;
  }
  FS_ReplaceSeparators(path);
}

qboolean DB_FileExists(const char *fileName, int ff_dir)
{
  char path[MAX_OSPATH];

  DB_BuildOSPath(fileName, ff_dir, sizeof(path), path);
  return FS_FileExistsOSPath(path);
}

qboolean DB_FileExistsLoadscreen(const char *fileName)
{
  char path[MAX_OSPATH];
  char mapname[64];
  char *find;

	Q_strncpyz(mapname, fileName, sizeof(mapname));
	
	find = strstr(mapname, "_load");
  if ( find )
  {
    find[0] = '\0';
  }

	Com_sprintf(path, sizeof(path), "%s/zone/%s/%s_load.ff", Sys_DefaultInstallPath(), Win_GetLanguage(), mapname);
  FS_ReplaceSeparators(path);
	if(FS_FileLengthOSPathByName(path) > 50)
	{
		return qtrue;
	}

	Com_sprintf(path, sizeof(path), "%s/usermaps/%s/%s_load.ff", Sys_DefaultInstallPath(), mapname, mapname);
  FS_ReplaceSeparators(path);
	if(FS_FileLengthOSPathByName(path) > 50)
	{
		return qtrue;
	}
	return qfalse;
}


void DB_AddUserMapDir(const char *usermapDir)
{
	if ( fs_gameDirVar->string[0] == '\0')
	{
		return;
	}
  
    if ( DB_FileExists(usermapDir, 2) )
    {
		FS_AddUserMapDirIWD(va("%s/%s", "usermaps", usermapDir));
    }
}

qboolean DB_ModFileExists()
{
  if ( fs_gameDirVar->string[0] && DB_FileExists("mod", 1))
  {
    return qtrue;
  }
  return qfalse;
}


/*
=====================
FS_PureServerSetLoadedPaks

If the string is empty, all data sources will be allowed.
If not empty, only pk3 files that match one of the space
separated checksums will be checked for files, with the
exception of .cfg and .dat files.
=====================
*/

qboolean FS_PureServerSetLoadedIwds(const char *paksums, const char *paknames)
{
  int i, k, l, rt;
  int numPakSums;
  iwdPureReferences_t *pureSums;
  int numPakNames;
  const char *lpakNames[1024];
  int lpakSums[1024];

  rt = 0;
  
  Cmd_TokenizeString(paksums);
  
  numPakSums = Cmd_Argc();
  
  if ( numPakSums > sizeof(lpakSums)/sizeof(lpakSums[0]))
  {
    numPakSums = sizeof(lpakSums)/sizeof(lpakSums[0]);
  }
  
  for ( i = 0 ; i < numPakSums ; i++ ) {
	lpakSums[i] = atoi( Cmd_Argv( i ) );
  }
  Cmd_EndTokenizedString();

  Cmd_TokenizeString(paknames);
  numPakNames = Cmd_Argc();
  
  if ( numPakNames > sizeof(lpakNames)/sizeof(lpakNames[0]) )
  {
    numPakNames = sizeof(lpakNames)/sizeof(lpakNames[0]);
  }
  
  for ( i = 0 ; i < numPakNames ; i++ ) {
	lpakNames[i] = CopyString( Cmd_Argv( i ) );
  }
  
  Cmd_EndTokenizedString();

  if ( numPakSums != numPakNames )
  {
    Com_Error(ERR_FATAL, "iwd sum/name mismatch");
	return rt;
  }
  
	if ( numPakSums )
	{
  
		for(pureSums = fs_iwdPureChecks; pureSums; pureSums = pureSums->next)
		{

			for ( i = 0; i < numPakSums; i++)
			{
				if(lpakSums[i] == pureSums->checksum && !Q_stricmp(lpakNames[i], pureSums->baseName))
				{
					break;
				}
			}
			if ( i == numPakSums )
			{
				rt = 1;
				break;
			}
		}
	}

	if ( numPakSums == fs_numServerIwds && rt == 0)
	{
		for ( i = 0, k = 0; i < fs_numServerIwds; )
		{
		  if ( lpakSums[k] == fs_serverIwds[i] && !Q_stricmp(lpakNames[k], fs_serverIwdNames[i]) )
		  {
			++k;
			if ( k < numPakSums )
			{
			  i = 0;
			  continue;
			}
			
			for ( l = 0; l < numPakNames; ++l )
			{
				SL_RemoveString(lpakNames[l]);
				lpakNames[l] = NULL;
			}
			return 0;
		  
		  }
		  ++i;
		}
		if ( numPakSums == 0 )
		{
			return rt;
		}
	}

    SND_StopSounds(8);
    FS_ShutdownServerIwdNames( );
    fs_numServerIwds = numPakSums;
    if ( numPakSums )
    {
      Com_DPrintf(CON_CHANNEL_FILES, "Connected to a pure server.\n");
      Com_Memcpy(fs_serverIwds, lpakSums, sizeof(int) * fs_numServerIwds);
      Com_Memcpy(fs_serverIwdNames, lpakNames, sizeof(char*) * fs_numServerIwds);
      fs_fakeChkSum = 0;
    }
    return rt;
}

qboolean FS_ResetPureServerIwds()
{
	return FS_PureServerSetLoadedIwds("", "");
}

const char* FS_GetNameFromHandle(fileHandle_t f)
{
	if ( f < 0 || f > MAX_FILE_HANDLES ) {
		Com_Error( ERR_DROP, "FS_GetNameFromHandle: out of range %i\n", f);
	}
	if(fsh[f].handleFiles.file.o == NULL)
	{
		return "";
	}
	return fsh[f].name;
}

wchar_t* FS_DirNameUni(wchar_t* s)
{
	wchar_t* f = wcsrchr(s, PATH_SEPUNI);
	if(f)
	{
		*f = 0;
	}
	return s;
}

struct z_stream_s_internal
{
  char *next_in;
  unsigned int avail_in;
  unsigned int total_in;
  char *next_out;
  unsigned int avail_out;
  unsigned int total_out;
  char *msg;
  struct internal_state *state;
  char *(__cdecl *zalloc)(char *, unsigned int, unsigned int);
  void (__cdecl *zfree)(char *, char *);
  char *opaque;
  int data_type;
  unsigned int adler;
};


struct file_in_zip_read_info_s
{
  char *read_buffer;
  struct z_stream_s_internal stream;
  unsigned int pos_in_zipfile;
  unsigned int stream_initialised;
  unsigned int offset_local_extrafield;
  unsigned int size_local_extrafield;
  unsigned int pos_local_extrafield;
  unsigned int rest_read_compressed;
  unsigned int rest_read_uncompressed;
  struct _iobuf *file;
  unsigned int compression_method;
  unsigned int byte_before_the_zipfile;
};


void FS_AddIwdFilesForGameDirectory_SafePath(const char *dir);

FILE* _fopen_savepathhelper(const char* filepath, const char* method)
{
	wchar_t _method[16];
	wchar_t _tfilepath[1024];
	wchar_t _filepath[1024];

	if(strncmp(filepath, "\\fs_savepath", 12) == 0)
	{
		Q_StrToWStr(_method, method, sizeof(_method));
		Q_StrToWStr(_tfilepath, filepath +12, sizeof(_tfilepath));
		Com_sprintfUni(_filepath, sizeof(_filepath), L"%s%s", fs_savepathDir, _tfilepath);
		return _wfopen(_filepath, _method);
	}

	return fopen(filepath, method);
}


void FS_AddGameDirectory(const char* path, const char *dir) //FS_AddLocalizedGameDirectory
{
	int i;
	for(i = 14; i >= 0; --i)
	{
		FS_AddGameDirectory_real(path, dir, 1, i);
	}
	FS_AddGameDirectory_real(path, dir, 0, 0);
}


void FS_Startup(const char *gameName)
{
    Com_Printf(CON_CHANNEL_FILES,"----- FS_Startup -----\n");
	FS_InitCvars();
	
	FS_SetupSavePath();

    if (fs_basepath->string[0])
    {
        if (fs_usedevdir->boolean)
        {
            FS_AddGameDirectory(fs_basepath->string, "devraw_shared");
            FS_AddGameDirectory(fs_basepath->string, "devraw");
            FS_AddGameDirectory(fs_basepath->string, "raw_shared");
            FS_AddGameDirectory(fs_basepath->string, "raw");
        }
        FS_AddGameDirectory(fs_basepath->string, "players");
    }

    if (fs_homepath->string[0] && Q_stricmp(fs_basepath->string, fs_homepath->string) && fs_usedevdir->boolean)
    {
        FS_AddGameDirectory(fs_homepath->string, "devraw_shared");
        FS_AddGameDirectory(fs_homepath->string, "devraw");
        FS_AddGameDirectory(fs_homepath->string, "raw_shared");
        FS_AddGameDirectory(fs_homepath->string, "raw");
    }

    /* CDPath set. */
    if (fs_cdpath->string[0] && Q_stricmp(fs_basepath->string, fs_cdpath->string))
    {
        if (fs_usedevdir->boolean)
        {
            FS_AddGameDirectory(fs_cdpath->string, "devraw_shared");
            FS_AddGameDirectory(fs_cdpath->string, "devraw");
            FS_AddGameDirectory(fs_cdpath->string, "raw_shared");
            FS_AddGameDirectory(fs_cdpath->string, "raw");
        }
        FS_AddGameDirectory(fs_cdpath->string, gameName);
    }
    /* BaseGame set. */
    if (fs_basepath->string[0])
    {
        FS_AddGameDirectory(fs_basepath->string, va("%s_shared", gameName));
        FS_AddGameDirectory(fs_basepath->string, gameName);
    }
    /* BaseGame set and (HomePath not equal to BaseGame). (aka multiple servers on one game). */
    if (fs_basepath->string[0] && Q_stricmp(fs_homepath->string, fs_basepath->string))
    {
        FS_AddGameDirectory(fs_basepath->string, va("%s_shared", gameName));
        FS_AddGameDirectory(fs_homepath->string, gameName);
    }
	FS_AddIwdFilesForGameDirectory_SafePath(gameName);
    
	/* BaseGame set, fs_game is "main" and BaseGame not equal to "main". WUT? */
    if (fs_basegame->string[0] && !Q_stricmp(gameName, BASEGAME) && Q_stricmp(fs_basegame->string, gameName))
    {
        if (fs_cdpath->string[0])
            FS_AddGameDirectory(fs_cdpath->string, fs_basegame->string);
        if (fs_basepath->string[0])
            FS_AddGameDirectory(fs_basepath->string, fs_basegame->string);
        if (fs_homepath->string[0] && Q_stricmp(fs_homepath->string, fs_basepath->string))
            FS_AddGameDirectory(fs_homepath->string, fs_basegame->string);

		FS_AddIwdFilesForGameDirectory_SafePath(fs_basegame->string);
    }

    if (fs_gameDirVar->string[0] && !Q_stricmp(gameName, BASEGAME) && Q_stricmp(fs_gameDirVar->string, gameName))
    {
        if (fs_cdpath->string[0])
            FS_AddGameDirectory(fs_cdpath->string, fs_gameDirVar->string);
        if (fs_basepath->string[0])
            FS_AddGameDirectory(fs_basepath->string, fs_gameDirVar->string);
        if (fs_homepath->string[0] && Q_stricmp(fs_homepath->string, fs_basepath->string))
            FS_AddGameDirectory(fs_homepath->string, fs_gameDirVar->string);
    }

    Com_ReadCDKey();
	FS_AddCommands();
    FS_DisplayPath(1);

    Cvar_ClearModified(fs_gameDirVar);

    Com_Printf(CON_CHANNEL_FILES,"----------------------\n");
    Com_Printf(CON_CHANNEL_FILES,"%d files in iwd files\n", fs_packFiles);
}


/*
===========
FS_PathCmp
Ignore case and seprator char distinctions
===========
*/
int FS_PathCmp( const char *s1, const char *s2 ) {
	int		c1, c2;

	do {
		c1 = *s1++;
		c2 = *s2++;

		if (c1 >= 'a' && c1 <= 'z') {
			c1 -= ('a' - 'A');
		}
		if (c2 >= 'a' && c2 <= 'z') {
			c2 -= ('a' - 'A');
		}

		if ( c1 == '\\' || c1 == ':' ) {
			c1 = '/';
		}
		if ( c2 == '\\' || c2 == ':' ) {
			c2 = '/';
		}

		if (c1 < c2) {
			return -1;		// strings not equal
		}
		if (c1 > c2) {
			return 1;
		}
	} while (c1);

	return 0;		// strings are equal
}

static signed int iwdsort(const void *cmp1_arg, const void *cmp2_arg)
{
  const char* cmp1;
  const char* cmp2;

  cmp1 = *(const char**)cmp1_arg;
  cmp2 = *(const char**)cmp2_arg;

  return FS_PathCmp(cmp1, cmp2);
}

#define MAX_PAKFILES 1024
void FS_AddIwdFilesForGameDirectory_SafePath(const char *dir)
{

  searchpath_t *search, *prev, *sp;
  int numfiles;
  int i;
  pack_t* pak;
  wchar_t** pakfiles;
  wchar_t pakdir[MAX_OSPATH];
  char pakfile[MAX_OSPATH];
  char* sorted[MAX_PAKFILES];

  FS_BuildOSPathForThreadUni( FS_GetSavePath(), dir, "", pakdir, 0);

  pakfiles = Sys_ListFilesW(pakdir, L"iwd", 0, &numfiles, 0);

  if(!pakfiles)
    return;

  if ( numfiles > MAX_PAKFILES )
  {
    Com_PrintWarning(CON_CHANNEL_FILES,"WARNING: Exceeded max number of iwd files in savepath (%d/%d)\n", numfiles, MAX_PAKFILES);
    numfiles = MAX_PAKFILES;
  }

  for(i = 0; i < numfiles; i++)
  {
	char printable[1024];
	wcstombs(printable, pakfiles[i], sizeof(printable));
	sorted[i] = strdup(printable);
  }
  Sys_FreeFileListW(pakfiles);



  qsort(sorted, numfiles, 4, iwdsort);

  for(i = 0; i < numfiles; i++)
  {
    if ( !Q_stricmp(dir, BASEGAME) && Q_stricmpn(sorted[i], "jcod4x_", 7))
	{
		Com_PrintWarning(CON_CHANNEL_FILES,"Invalid IWD %s in \\main.\n", sorted[i]);
		continue;
	}

	FS_BuildOSPathForThread("\\fs_savepath", dir, sorted[i], pakfile, 0);
	pak = FS_LoadZipFile( pakfile, sorted[i]);
	if(pak == NULL)
	{
		continue;
	}

	Q_strncpyz(pak->pakGamename, dir, sizeof(pak->pakGamename));

	search = (searchpath_t *)Z_Malloc(sizeof(searchpath_t));
	search->pack = pak;
	search->bLocalized = 0;
	search->language = 0;

	prev = (searchpath_t*)&fs_searchpaths;
	sp = fs_searchpaths;

	search->next = sp;
	prev->next = search;
  }
}

/*
================
return a hash value for the filename
================
*/
long FS_HashFileName( const char *fname, int hashSize ) {
	int i;
	long hash;
	char letter;

	hash = 0;
	i = 0;
	while ( fname[i] != '\0' ) {
		letter = tolower( fname[i] );
		if ( letter == '.' ) {
			break;                          // don't include extension
		}
		if ( letter == '\\' ) {
			letter = '/';                   // damn path names
		}
		if ( letter == PATH_SEP ) {
			letter = '/';                           // damn path names
		}
		hash += (long)( letter ) * ( i + 119 );
		i++;
	}
	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	hash &= ( hashSize - 1 );
	return hash;
}




/*
=================
FS_LoadZipFile

Creates a new pak_t in the search chain for the contents
of a zip file.
=================
*/


struct pack_t *FS_LoadZipFile( const char *zipfile, const char *basename ) {
	fileInPack_t    *buildBuffer;
	pack_t          *pack;
	unzFile uf;
	int err;
	unz_global_info gi;
	char filename_inzip[MAX_ZPATH];
	unz_file_info file_info;
	int i, len;
	long hash;
	int fs_numHeaderLongs;
	int             *fs_headerLongs;
	char            *namePtr;

	fs_numHeaderLongs = 0;

	uf = unzOpen( zipfile );
	err = unzGetGlobalInfo( uf,&gi );

	if ( err != UNZ_OK ) {
		return NULL;
	}

	fs_packFiles += gi.number_entry;

	len = 0;
	unzGoToFirstFile( uf );
	for ( i = 0; i < gi.number_entry; i++ )
	{
		err = unzGetCurrentFileInfo( uf, &file_info, filename_inzip, sizeof( filename_inzip ), NULL, 0, NULL, 0 );
		if ( err != UNZ_OK ) {
			break;
		}
		len += strlen( filename_inzip ) + 1;
		unzGoToNextFile( uf );
	}


	buildBuffer = Z_Malloc( ( gi.number_entry * sizeof( fileInPack_t ) ) + len );
	namePtr = ( (char *) buildBuffer ) + gi.number_entry * sizeof( fileInPack_t );
	fs_headerLongs = Z_Malloc( gi.number_entry * sizeof( int ) );

	// get the hash table size from the number of files in the zip
	// because lots of custom pk3 files have less than 32 or 64 files
	for ( i = 1; i <= MAX_FILEHASH_SIZE; i <<= 1 ) {
		if ( i > gi.number_entry ) {
			break;
		}
	}

	pack = Z_Malloc( sizeof( pack_t ) + i * sizeof( fileInPack_t * ) );
	pack->hashSize = i;
	pack->hashTable = ( fileInPack_t ** )( ( (char *) pack ) + sizeof( pack_t ) );
	for ( i = 0; i < pack->hashSize; i++ ) {
		pack->hashTable[i] = NULL;
	}

	Q_strncpyz( pack->pakFilename, zipfile, sizeof( pack->pakFilename ) );
	Q_strncpyz( pack->pakBasename, basename, sizeof( pack->pakBasename ) );

	// strip .pk3 if needed
	if ( strlen( pack->pakBasename ) > 4 && !Q_stricmp( pack->pakBasename + strlen( pack->pakBasename ) - 4, ".iwd" ) ) {
		pack->pakBasename[strlen( pack->pakBasename ) - 4] = 0;
	}

	pack->handle = uf;
	pack->numFiles = gi.number_entry;
	pack->hasOpenFile = 0;
	unzGoToFirstFile( uf );

	for ( i = 0; i < gi.number_entry; i++ )
	{
		err = unzGetCurrentFileInfo( uf, &file_info, filename_inzip, sizeof( filename_inzip ), NULL, 0, NULL, 0 );
		if ( err != UNZ_OK ) {
			break;
		}
		if ( file_info.uncompressed_size > 0 ) {
			fs_headerLongs[fs_numHeaderLongs++] = LittleLong( file_info.crc );
		}
		Q_strlwr( filename_inzip );
		hash = FS_HashFileName( filename_inzip, pack->hashSize );
		buildBuffer[i].name = namePtr;
		strcpy( buildBuffer[i].name, filename_inzip );
		namePtr += strlen( filename_inzip ) + 1;
		// store the file position in the zip
		buildBuffer[i].pos = unzGetOffset( uf );
		buildBuffer[i].next = pack->hashTable[hash];
		pack->hashTable[hash] = &buildBuffer[i];
		unzGoToNextFile( uf );
	}

	pack->checksum = Com_BlockChecksumKey32( fs_headerLongs, 4 * fs_numHeaderLongs, LittleLong( 0 ) );
	if(fs_checksumFeed)
		pack->pure_checksum = Com_BlockChecksumKey32( fs_headerLongs, 4 * fs_numHeaderLongs, LittleLong( fs_checksumFeed ) );
	else
		pack->pure_checksum = pack->checksum;
	// TTimo: DO_LIGHT_DEDICATED
	// curious about the size of those
	//Com_DPrintf("Com_BlockChecksumKey: %s %u\n", pack->pakBasename, 4 * fs_numHeaderLongs);
	// cumulated for light dedicated: 21558 bytes
	pack->checksum = LittleLong( pack->checksum );
	pack->pure_checksum = LittleLong( pack->pure_checksum );

	Z_Free( fs_headerLongs );

	pack->buildBuffer = buildBuffer;
	return pack;
}


bool __cdecl FS_IsBackupSubStr(const char *filenameSubStr)
{
  if ( filenameSubStr[0] != '.' || filenameSubStr[1] != '.' )
  {
    if(filenameSubStr[0] == ':' && filenameSubStr[1] == ':')
    {
        return true;
    }
    else
    {
        return false;
    }
  }
  return true;
}

char __cdecl FS_SanitizeFilename(const char *filename, char *sanitizedName, int sanitizedNameSize)
{
  int srcIndex;
  int dstIndex;

  assert(filename);
  assert(sanitizedName);
  assert(sanitizedNameSize > 0);

  for ( srcIndex = 0; ; ++srcIndex )
  {
    if ( !(filename[srcIndex] == '/' || filename[srcIndex] == '\\') )
    {
      break;
    }
  }

  dstIndex = 0;
  while ( filename[srcIndex] )
  {
    if ( FS_IsBackupSubStr(&filename[srcIndex]) )
    {
      return 0;
    }
    if ( filename[srcIndex] != '.' || (filename[srcIndex + 1] != 0 && 
	filename[srcIndex + 1] != '/' && filename[srcIndex + 1] != '\\' ))
    {
      if ( dstIndex + 1 >= sanitizedNameSize )
      {
        assert(dstIndex + 1 < sanitizedNameSize);
        return 0;
      }
      if ( filename[srcIndex] == '/' || filename[srcIndex] == '\\' )
      {
        sanitizedName[dstIndex] = '/';
        while ( 1 )
        {
          if ( !(filename[srcIndex +1] == '/' || filename[srcIndex +1] == '\\') )
          {
            break;
          }
          ++srcIndex;
        }
      }
      else
      {
        sanitizedName[dstIndex] = filename[srcIndex];
      }
      ++dstIndex;
    }
    ++srcIndex;
  }
  assert ( dstIndex <= srcIndex);
  sanitizedName[dstIndex] = 0;
  return 1;
}

qboolean __cdecl FS_UseSearchPath(struct searchpath_s *pSearch)
{

  if ( pSearch->bLocalized && fs_ignoreLocalized->boolean )
  {
    return 0;
  }
  return !pSearch->bLocalized || pSearch->language == SEH_GetCurrentLanguage();
}

qboolean __cdecl FS_FilesAreLoadedGlobally(const char *filename)
{
  const char *extensions[9];
  int filenameLen;
  int extensionNum;

  extensions[0] = ".hlsl";
  extensions[1] = ".txt";
  extensions[2] = ".cfg";
  extensions[3] = ".levelshots";
  extensions[4] = ".menu";
  extensions[5] = ".arena";
  extensions[6] = ".str";
  extensions[7] = "";


  filenameLen = strlen(filename);
  for ( extensionNum = 0; *extensions[extensionNum]; ++extensionNum )
  {
    if ( !Q_stricmp(&filename[filenameLen - strlen(extensions[extensionNum])], extensions[extensionNum]) )
    {
      return qtrue;
    }
  }
  return qfalse;
}


bool __cdecl FS_PureIgnoreFiles(const char *filename)
{
  const char *extension;

  if ( !Q_stricmp(filename, "ban.txt") )
  {
    return true;
  }
  extension = Com_GetExtensionSubString(filename);
  if ( *extension == '.' )
  {
    ++extension;
  }
  if ( Q_stricmp(extension, "cfg") )
  {
    return Q_stricmp(extension, ".dm_1") == 0;
  }
  return true;
}

qboolean FS_IwdIsPure(pack_t* iwd)
{
  int i;

  if ( !fs_numServerIwds )
  {
    return 1;
  }

  //Com_Printf(CON_CHANNEL_FILES, "IWD check: %s\n", iwd->pakBasename);
  if(Com_IsLegacyServer())
  {
	  if(Q_stricmpn(iwd->pakBasename, "jcod4x_", 7) == 0)
	  {
		  return 1;
	  }
  }

  for ( i = 0; i < fs_numServerIwds; ++i )
  {
    if ( iwd->checksum == fs_serverIwds[i] )
    {
      return 1;
    }
  }
  return 0;
}


qboolean REGPARM(2) FS_IwdIsPureStub(int dummy, pack_t* iwd)
{
	return FS_IwdIsPure(iwd);
}

/*
===========
FS_FilenameCompare

Ignore case and seprator char distinctions
===========
*/
qboolean FS_FilenameCompare( const char *s1, const char *s2 ) {
	int		c1, c2;

	do {
		c1 = *s1++;
		c2 = *s2++;

		if (c1 >= 'a' && c1 <= 'z') {
			c1 -= ('a' - 'A');
		}
		if (c2 >= 'a' && c2 <= 'z') {
			c2 -= ('a' - 'A');
		}

		if ( c1 == '\\' || c1 == ':' ) {
			c1 = '/';
		}
		if ( c2 == '\\' || c2 == ':' ) {
			c2 = '/';
		}

		if (c1 != c2) {
			return -1;		// strings not equal
		}
	} while (c1);

	return 0;		// strings are equal
}


void __cdecl FS_AddIwdPureCheckReference(struct searchpath_s *search)
{
  struct iwdPureReferences_s *pureCheckInfo;
  struct iwdPureReferences_s *p;

  assert(search->pack);

  if ( !search->bLocalized )
  {
	if(Q_stricmpn(search->pack->pakBasename, "jcod4x_", 7) == 0)
	{
		return;
	}
    for ( p = fs_iwdPureChecks; p; p = p->next )
    {
      if ( p->checksum == search->pack->checksum && !Q_stricmp(p->baseName, search->pack->pakBasename) )
      {
        return;
      }
    }
    pureCheckInfo = (struct iwdPureReferences_s *)Z_Malloc(sizeof(struct iwdPureReferences_s));
    pureCheckInfo->next = 0;
    pureCheckInfo->checksum = search->pack->checksum;
    Q_strncpyz(pureCheckInfo->baseName, search->pack->pakBasename, sizeof(pureCheckInfo->baseName));
    Q_strncpyz(pureCheckInfo->gameName, search->pack->pakGamename, sizeof(pureCheckInfo->gameName));
    if ( fs_iwdPureChecks )
    {
      for ( p = fs_iwdPureChecks; p; p = p->next )
      {
        if ( !p->next )
        {
          p->next = pureCheckInfo;
          return;
        }
      }
    }
    else
    {
      fs_iwdPureChecks = pureCheckInfo;
    }
  }
}



unsigned int __cdecl FS_FOpenFileReadForThread(const char *filename, fileHandle_t *file, enum FsThread thread)
{
  unsigned int result; // eax
  char copypath[256]; // [esp+0h] [ebp-338h]
  struct fileInPack_s *i; // [esp+100h] [ebp-238h]
  pack_t *iwd; // [esp+108h] [ebp-230h]
  struct fileInPack_s *iwdFile; // [esp+10Ch] [ebp-22Ch]
  int hash; // [esp+110h] [ebp-228h]
  char sanitizedName[256]; // [esp+118h] [ebp-220h]
  directory_t *dir; // [esp+218h] [ebp-120h]
  unz_s *zfi; // [esp+21Ch] [ebp-11Ch]
  const char *impureIwd; // [esp+220h] [ebp-118h]
  struct file_in_zip_read_info *ziptemp; // [esp+224h] [ebp-114h]
  char netpath[256]; // [esp+228h] [ebp-110h]
  bool wasSkipped; // [esp+32Fh] [ebp-9h]
  struct searchpath_s *search; // [esp+330h] [ebp-8h]
  void *filetemp; // [esp+334h] [ebp-4h]

  impureIwd = 0;
  wasSkipped = 0;
  if(filename == NULL)
  {
	Com_Error(ERR_FATAL, "FS_FOpenFileRead: NULL 'filename' parameter passed");
  }
  FS_CheckFileSystemStarted();
  if ( FS_SanitizeFilename(filename, sanitizedName, sizeof(sanitizedName)) )
  {
    if ( file )
    {
      *file = FS_HandleForFileForThread(thread);
      if ( *file )
      {
        for ( search = fs_searchpaths; search; search = search->next )
        {
          if ( FS_UseSearchPath(search) )
          {
            iwd = search->pack;
            if ( iwd && iwd->numFiles )
            {
			  assert(iwd->hashTable && iwd->hashSize);
              hash = FS_HashFileName(sanitizedName, iwd->hashSize);
              for ( i = iwd->hashTable[hash]; i; i = i->next )
              {
                if ( !FS_FilenameCompare(i->name, sanitizedName) )
                {
                  if ( !g_disablePureCheck && !search->bLocalized && !search->ignorePureCheck && !FS_IwdIsPure(iwd) )
                  {
                    impureIwd = (const char *)iwd;
                    break;
                  }
                  if ( !iwd->referenced && !FS_FilesAreLoadedGlobally(sanitizedName) )
                  {
                    iwd->referenced = 1;
                    FS_AddIwdPureCheckReference(search);
                  }
                  if ( _InterlockedCompareExchange((volatile long int*)&iwd->hasOpenFile, 1, 0) == 1 )
                  {
                    fsh[*file].handleFiles.iwdIsClone = 1;
                    fsh[*file].handleFiles.file.z = unzReOpen(iwd->pakFilename, iwd->handle);
                    if ( !fsh[*file].handleFiles.file.o )
                    {
                      if ( thread )
                      {
                        FS_FCloseFile(*file);
                        *file = 0;
                        return -1;
                      }
                      Com_Error(ERR_FATAL, "Couldn't reopen %s", iwd->pakFilename);
                    }
                  }
                  else
                  {
                    fsh[*file].handleFiles.iwdIsClone = 0;
                    fsh[*file].handleFiles.file.z = iwd->handle;
                  }
                  Q_strncpyz(fsh[*file].name, sanitizedName, 256);
                  fsh[*file].zipFile = iwd;
                  zfi = (unz_s *)fsh[*file].handleFiles.file.z;
                  filetemp = zfi->filestream;
                  ziptemp = zfi->pfile_in_zip_read;
                  unzSetCurrentFileInfoPosition(iwd->handle, i->pos);
                  Com_Memcpy(zfi, iwd->handle, sizeof(unz_s));
                  zfi->filestream = filetemp;
                  zfi->pfile_in_zip_read = ziptemp;
                  unzOpenCurrentFile(fsh[*file].handleFiles.file.z);
                  fsh[*file].zipFilePos = i->pos;
                  if ( fs_debug->boolean )
                  {
                    Com_Printf(CON_CHANNEL_FILES, "FS_FOpenFileReadfrom thread '%s', handle '%d', %s (found in '%s')\n", Sys_GetCurrentThreadName(), *file, sanitizedName, iwd->pakFilename);
                  }
                  return zfi->cur_file_info.uncompressed_size;
                }
              }
            }
            else if ( search->dir )
            {
              if ( (!search->ignore && !fs_restrict->boolean && !fs_numServerIwds) || search->bLocalized || search->ignorePureCheck || FS_PureIgnoreFiles(sanitizedName) )
              {
                dir = search->dir;
                FS_BuildOSPathForThread(dir->path, dir->gamedir, sanitizedName, netpath, thread);
                fsh[*file].handleFiles.file.o = FS_FileOpenReadBinary(netpath);
                if ( fsh[*file].handleFiles.file.o )
                {
                  if ( !search->bLocalized && !search->ignorePureCheck && !FS_PureIgnoreFiles(sanitizedName) )
                  {
                    fs_fakeChkSum = rand() + 1;
                  }
                  Q_strncpyz(fsh[*file].name, sanitizedName, 256);
                  fsh[*file].zipFile = 0;
                  if ( fs_debug->boolean )
                  {
                    Com_Printf(CON_CHANNEL_FILES, "FS_FOpenFileRead from thread '%s', handle '%d', %s (found in '%s/%s')\n", Sys_GetCurrentThreadName(), *file, sanitizedName, dir->path, dir->gamedir);
                  }
                  if ( fs_copyfiles->boolean )
                  {
                    if ( !Q_stricmp(dir->path, fs_cdpath->string) )
                    {
                      FS_BuildOSPathForThread(fs_basepath->string, dir->gamedir, sanitizedName, copypath, thread);
                      FS_CopyFile(netpath, copypath);
                    }
                  }
                  return FS_filelength(*file);
                }
              }
              else if ( !wasSkipped )
              {
                dir = search->dir;
                FS_BuildOSPathForThread(dir->path, dir->gamedir, sanitizedName, netpath, thread);
                filetemp = FS_FileOpenReadBinary(netpath);
                if ( filetemp )
                {
                  wasSkipped = 1;
                  FS_FileClose(filetemp);
                }
              }
            }
          }
        }
        if ( fs_debug->boolean )
        {
          if ( thread == FS_THREAD_MAIN )
          {
            Com_Printf(CON_CHANNEL_FILES, "Can't find %s\n", filename);
          }
        }
        FS_FCloseFile(*file);
        *file = 0;
        if ( impureIwd )
        {
          Com_Error(ERR_DROP, "Impure client detected. Invalid .IWD files referenced!\n%s", impureIwd);
        }
        if ( wasSkipped )
        {
          if ( fs_numServerIwds || fs_restrict->boolean )
          {
            Com_Printf(CON_CHANNEL_FILES, "Error: %s must be in an IWD\n", filename);
          }
          else
          {
            Com_Printf(CON_CHANNEL_FILES, "Error: %s must be in an IWD or not in the main directory\n", filename);
          }
          result = -2;
        }
        else
        {
          result = -1;
        }
      }
      else
      {
        result = -1;
      }
    }
    else
    {
      for ( search = fs_searchpaths; search; search = search->next )
      {
        if ( FS_UseSearchPath(search) )
        {
          iwd = search->pack;
          if ( iwd && iwd->numFiles )
          {
			assert(iwd->hashTable && iwd->hashSize);
            hash = FS_HashFileName(sanitizedName, iwd->hashSize);
            for ( iwdFile = iwd->hashTable[hash]; iwdFile; iwdFile = iwdFile->next )
            {
              if ( !FS_FilenameCompare(iwdFile->name, sanitizedName) )
              {
                return 1;
              }
            }
          }
          else if ( search->dir )
          {
            dir = search->dir;
            FS_BuildOSPathForThread(dir->path, dir->gamedir, sanitizedName, netpath, thread);
            filetemp = FS_FileOpenReadBinary(netpath);
            if ( filetemp )
            {
              FS_FileClose(filetemp);
              return 1;
            }
          }
        }
      }
      result = -1;
    }
  }
  else
  {
    if ( file )
    {
      *file = 0;
    }
    if ( fs_debug->boolean )
    {
      Com_Printf(CON_CHANNEL_FILES, "fs_debug: %s is invalid or contains and invalid substring\n", filename);
    }
    result = -1;
  }
  return result;
}


int __cdecl FS_Read(void *buffer, int len, int h)
{
  int tries;
  unsigned int remaining; 
  char *buf; 
  FILE *f; 
  int read; 

  FS_CheckFileSystemStarted();
  if ( !h )
  {
    return 0;
  }
  if ( fsh[h].zipFile )
  {
    return unzReadCurrentFile(fsh[h].handleFiles.file.z, (char *)buffer, len);
  }
  f = FS_FileForHandle(h);
  buf = (char *)buffer;
  remaining = len;
  tries = 0;
  while ( remaining )
  {
    read = FS_FileRead(buf, remaining, f);
    if ( !read )
    {
      if ( tries )
      {
        return len - remaining;
      }
      tries = 1;
    }
    if ( read == -1 )
    {
      if ( h >= 50 && h < 61 )
      {
        return -1;
      }
      Com_Error(ERR_FATAL, "FS_Read: -1 bytes read");
    }
    remaining -= read;
    buf += read;
  }
  return len;
}



//FS_ReferencedIwdPureChecksums()
void __cdecl FS_DebugPakChecksums_f()
{
#if 0
  struct searchpath_s *search;

  Com_Printf(CON_CHANNEL_FILES, "--------- FS_DebugPakChecksums ---------\n");

  for ( search = fs_searchpaths; search; search = search->next )
  {
    if ( !search->pack)
    {
		continue;
	}
	Com_Printf(CON_CHANNEL_FILES, "  %s: %x\n", search->pack->pakBasename, search->pack->pure_checksum);
  }
  Com_Printf(CON_CHANNEL_FILES, "----------------------------------------\n");
#endif
}


//FS_ReferencedIwdPureChecksums()
const char *__cdecl FS_ReferencedPakChecksums(char *info6, int maxsize)
{
  int checksum; 
  struct searchpath_s *search;
  int numIwds;

  FS_DebugPakChecksums_f();  

  info6[0] = 0;
  checksum = fs_checksumFeed;
  numIwds = 0;
  info6[strlen(info6) + 1] = 0;
  info6[strlen(info6) + 2] = 0;
#ifdef BLACKOPS
  info6[strlen(info6)] = '#';
#else
  info6[strlen(info6)] = '@';
#endif
  info6[strlen(info6)] = ' ';
  if(!Com_IsLegacyServer())
  {
    Q_strcat(info6, maxsize, va("L%i ", SEH_GetCurrentLanguage()));
  }

  for ( search = fs_searchpaths; search; search = search->next )
  {
    if ( !search->pack || (search->bLocalized && Com_IsLegacyServer()) || !search->pack->referenced)
    {
		continue;
	}

    if ( Q_stricmpn(search->pack->pakBasename, "jcod4x_", 7) == 0 && Com_IsLegacyServer())
    {
		continue;
	}

    Q_strcat(info6, maxsize, va("%i ", search->pack->pure_checksum));
    checksum ^= search->pack->pure_checksum;
    ++numIwds;
  }
  if ( fs_fakeChkSum )
  {
    Q_strcat(info6, maxsize, va("%i ", fs_fakeChkSum));
  }
  Q_strcat(info6, maxsize, va("%i ", numIwds ^ checksum));

  DB_WritePureInfoString(info6, maxsize, fs_checksumFeed);
  
  return info6;
}


void FS_ValidateIwdFiles()
{
#if 0
  struct searchpath_s *search;
  int i;

  int validchecksums[] = {
	1489187914, -707015979, 2115322797, 1043426064, -292863836, 173489706, -1846635013, -1079795827, -1041447890, -715502781,
  	489843315, 499941093, 309900900, -165391090, 648005745, -1190209610, 1594892969, 827202539, 64315714, -1677284403, -1533126169,
	1711546343, -1116733145, 492300637, -447146714, 1718774667, -865200138, -1248474720, -174036185, -359731006, 979319220, -1133804886,
	388659855, -770299200, -707299953, -1713101681, -1856209985, -2117571754, -1780628728, -349627133, -1027452566, -1162455287,
	1697430914, -1667871300, -341477932, 1642889745, -127539841, 1812272767, 1523571568, 1063914055, 1470482569, -1496898392, -1495991264,
	1266105597, 1765273397, 1582787038, -2029990542, -36132527, 1726798886, -447146714, 1855127956, -174036185, -359731006, 492300637,
	-865200138, -1248474720
  };

  char badfiles[4096];
  char errormsg[4096];
  badfiles[0] = 0;

  if (fs_gameDirVar->string[0])
  {
	  return; //no checks for mods on startup
  }

  for ( search = fs_searchpaths; search; search = search->next )
  {
    if ( !search->pack)
    {
		continue;
	}
	for(i = 0; i < sizeof(validchecksums)/sizeof(int); ++i)
	{
		if(search->pack->checksum == validchecksums[i])
		{
			break;
		}
	}
	if(i == sizeof(validchecksums)/sizeof(int))
	{
		Q_strcat(badfiles, sizeof(badfiles), search->pack->pakFilename);
		Q_strcat(badfiles, sizeof(badfiles), "\n");
	}
  }
  badfiles[sizeof(badfiles) -1] = 0;
  if(badfiles[0])
  {
	Com_sprintf(errormsg, sizeof(errormsg), "Invalid IWD files detected!\nPlease restore the original versions of IWD files from disc or delete them,\ndepending on what is applicable. Bad files:\n%s", badfiles);
	Com_Printf(CON_CHANNEL_FILES, "%s", errormsg);
	errormsg[1023] = 0;
	Com_SetErrorMessage(errormsg);
  }
#endif
}
