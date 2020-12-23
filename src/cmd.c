#include "q_shared.h"
#include "qcommon.h"
#include "win_sys.h"

#include <stdlib.h>

typedef struct cmd_function_s
{
	struct cmd_function_s   *next;
	char                    *name;
	char					*autocomplete1;
	char					*autocomplete2;
//	int						minPower;
//	int						unknown;
	//completionFunc_t		complete;
	xcommand_t 				function;
} cmd_function_t;


#define cmd_insideCBufExecute *(byte*)(0x110cebd)
void Cbuf_Execute_stub(int a, int b);
void Cbuf_SV_Execute();

cmd_function_t** cmd_functions = ((cmd_function_t**)(cmd_functions_ADDR));
cmd_function_t** sv_cmd_functions = ((cmd_function_t**)(sv_cmd_functions_ADDR));

void Cmd_Shutdown()
{
    *cmd_functions = 0;
    *sv_cmd_functions = 0;
}

/*
============
Cmd_AddCommand
============
*/
void    Cmd_AddCommand( const char *cmd_name, xcommand_t function ) {
	cmd_function_t  *cmd;
	//*cmd_functions = ((cmd_function_t*)(cmd_functions_ADDR));

	// fail if the command already exists
	for ( cmd = *cmd_functions ; cmd ; cmd = cmd->next ) {
		if ( !strcmp( cmd_name, cmd->name )) {
			// allow completion-only commands to be silently doubled
			if ( function != NULL ) {
				Com_Printf(CON_CHANNEL_SYSTEM, "Cmd_AddCommand: %s already defined\n", cmd_name );
			}
			return;
		}
	}

	// use a small malloc to avoid zone fragmentation
	cmd = malloc( sizeof( cmd_function_t ) + strlen(cmd_name) + 1);
	if(cmd == NULL){
		return;
	}
	strcpy((char*)(cmd +1), cmd_name);
	cmd->name = (char*)(cmd +1);
	cmd->autocomplete1 = NULL;
	cmd->autocomplete2 = NULL;
	cmd->function = function;
	cmd->next = *cmd_functions;
	*cmd_functions = cmd;
}


/*
============
Cmd_AddServerCommand
============
*/
void    Cmd_AddServerCommand( const char *cmd_name, xcommand_t function ) {
	cmd_function_t  *cmd;
	//*cmd_functions = ((cmd_function_t*)(cmd_functions_ADDR));

	// fail if the command already exists
	for ( cmd = *sv_cmd_functions ; cmd ; cmd = cmd->next ) {
		if ( !strcmp( cmd_name, cmd->name )) {
			// allow completion-only commands to be silently doubled
			if ( function != NULL ) {
				Com_Printf(CON_CHANNEL_SYSTEM, "Cmd_AddCommand: %s already defined\n", cmd_name );
			}
			return;
		}
	}

	// use a small malloc to avoid zone fragmentation
	cmd = malloc( sizeof( cmd_function_t ) + strlen(cmd_name) + 1);
	strcpy((char*)(cmd +1), cmd_name);
	cmd->name = (char*)(cmd +1);
	cmd->autocomplete1 = NULL;
	cmd->autocomplete2 = NULL;
	cmd->function = function;
	cmd->next = *sv_cmd_functions;
	*sv_cmd_functions = cmd;
}

/*
============
Cmd_RemoveCommand
============
*/
void    Cmd_RemoveCommand( const char *cmd_name ) {
	cmd_function_t  *cmd, **back;

	back = cmd_functions;
	while ( 1 ) {
		cmd = *back;
		if ( !cmd ) {
			// command wasn't active
			return;
		}
		if ( !strcmp( cmd_name, cmd->name ) ) {
			*back = cmd->next;
			if ( cmd->name && cmd->name == (char*)(cmd +1))
			{
				free( cmd );
			}
			return;
		}
		back = &cmd->next;
	}
}

/*
============
Cmd_SetAutoComplete
============
*/
void Cmd_SetAutoComplete( const char *cmd_name, const char *arg_1, const char *arg_2){
	cmd_function_t  *cmd = *cmd_functions;

	while ( cmd ) {
		if ( !strcmp( cmd_name, cmd->name ) ) {
			cmd->autocomplete1 = (char*)arg_1;
			cmd->autocomplete2 = (char*)arg_2;
			return;
		}
		cmd = cmd->next;
	}
}


#define MAX_TOKENIZE_STRINGS 8
#define tokenStrings_ADDR 0x1410b40
#define tokenbuf_ADDR 0x1433490
#define tokenStrings (*(struct CmdArgs*)(tokenStrings_ADDR))
#define tokenbuf (*(struct CmdArgsPrivate*)(tokenbuf_ADDR))

#pragma pack(push, 1)


struct CmdArgsPrivate
{
  char textPool[8192];
  const char *argvPool[512];
  int usedTextPool[MAX_TOKENIZE_STRINGS];
  int totalUsedArgvPool;
  int totalUsedTextPool;
};


struct CmdArgs
{
  int nesting; //0x1410b40 Count of parsed strings
  int localClientNum[MAX_TOKENIZE_STRINGS];
  int argshift[MAX_TOKENIZE_STRINGS];
  int argc[MAX_TOKENIZE_STRINGS]; //0x1410b84 Number of parsed tokens in each string
  const char **argv[MAX_TOKENIZE_STRINGS];
};

#pragma pack(pop)

void Cmd_TokenizeStringKernel(const char *text_in, int max_tokens, struct CmdArgs *args, struct CmdArgsPrivate *argsPriv);

/*
============
Cmd_Argc	Returns count of commandline arguments
============
*/
int	Cmd_Argc( void )
{
	return tokenStrings.argc[tokenStrings.nesting];
}

/*
============
Cmd_Argv	Returns pointer to current argument string
============
*/
const char*	Cmd_Argv( int argv )
{
	if(argv >= Cmd_Argc())
		return "";
	else
		return tokenStrings.argv[tokenStrings.nesting][argv];
}

qboolean Cmd_IsWhiteSpaceChar(char c)
{
	if ( c == '\x14' || c == '\x15' || c == '\x16' || c == '\0' )
		return 0;
    
	return (uint8_t)c <= (uint8_t)' ';
}

void Cmd_TokWriteChar(struct CmdArgsPrivate *argsPriv, char c)
{
  int cn;

  argsPriv->textPool[argsPriv->totalUsedTextPool] = c;
  cn = argsPriv->totalUsedTextPool + 1;
  if ( cn >= sizeof(argsPriv->textPool) -2)
  {
    cn = sizeof(argsPriv->textPool) -2;
  }
  argsPriv->totalUsedTextPool = cn;
}

/*
============
Cmd_TokenizeString

Parses the given string into command line tokens.
The text is copied to a seperate buffer and 0 characters
are inserted in the apropriate place, The argv array
will point into this temporary buffer.
============
*/
int Cmd_TokenizeStringInternal(const char *text_in, int max_tokens, const char **argv, struct CmdArgsPrivate *argsPriv)
{
	const char  *text;

	// clear previous args
	int argc = 0;

	if ( !text_in ) {
		return 0;
	}

	text = text_in;

	while ( 1 ) {
		if ( argc >= max_tokens ) {
			return argc;
		}
		if(argc == (max_tokens -1) && max_tokens > 0)
		{
			argv[argc] = &argsPriv->textPool[argsPriv->totalUsedTextPool];
			argc++;
			while ( Cmd_IsWhiteSpaceChar(*text)) {
				text++;
			}
			while(*text)
			{
				Cmd_TokWriteChar(argsPriv, *text);
				++text;
			}
			Cmd_TokWriteChar(argsPriv, 0);
			return argc;
		}

		while ( 1 ) {
			// skip whitespace
			while ( Cmd_IsWhiteSpaceChar(*text)) {
				text++;
			}
			if ( !*text ) {
				return argc;         // all tokens parsed
			}

			// skip // comments
			if ( text[0] == '/' && text[1] == '/' ) {
				return argc;         // all tokens parsed
			}

			// skip /* */ comments
			if ( text[0] == '/' && text[1] == '*' ) {
				while ( *text && ( text[0] != '*' || text[1] != '/' ) ) {
					text++;
				}
				if ( !*text ) {
					return argc;     // all tokens parsed
				}
				text += 2;
			} else {
				break;          // we are ready to parse a token
			}
		}

		argv[argc] = &argsPriv->textPool[argsPriv->totalUsedTextPool];
		argc++;

		// handle quoted strings
		if ( *text == '"' ) {
			text++;
			while ( *text && *text != '"' ) {
				Cmd_TokWriteChar(argsPriv, *text);
				++text;
			}
			Cmd_TokWriteChar(argsPriv, 0);
			if ( !*text ) {
				return argc;     // all tokens parsed
			}
			text++;
			continue;
		}

		// regular token


		// skip until whitespace, quote, or command
		while ( *text && !Cmd_IsWhiteSpaceChar(*text) ) {
			if ( text[0] == '"' ) {
				break;
			}

			if ( text[0] == '/' && text[1] == '/' ) {
				break;
			}

			// skip /* */ comments
			if ( text[0] == '/' && text[1] == '*' ) {
				break;
			}
			Cmd_TokWriteChar(argsPriv, *text);
			++text;
		}
		Cmd_TokWriteChar(argsPriv, 0);

		if ( !*text ) {
			return argc;     // all tokens parsed
		}
	}
}



void Cmd_TokenizeStringKernel(const char *text_in, int max_tokens, struct CmdArgs *args, struct CmdArgsPrivate *argsPriv)
{
	++args->nesting;
	argsPriv->usedTextPool[args->nesting] = -argsPriv->totalUsedTextPool;
	args->localClientNum[args->nesting] = -1;
	args->argshift[args->nesting] = 0;
	args->argv[args->nesting] = &argsPriv->argvPool[argsPriv->totalUsedArgvPool];
	args->argc[args->nesting] = Cmd_TokenizeStringInternal(text_in, max_tokens, args->argv[args->nesting], argsPriv);
	argsPriv->totalUsedArgvPool += args->argc[args->nesting];
	argsPriv->usedTextPool[args->nesting] += argsPriv->totalUsedTextPool;
}


void Cmd_TokenizeString(const char *line)
{
    Cmd_TokenizeStringKernel( line, 512 - tokenbuf.totalUsedArgvPool, &tokenStrings, &tokenbuf);
}

void Cmd_TokenizeStringWithLimit(const char *line, unsigned int limit)
{
	Cmd_TokenizeStringKernel(line, limit, &tokenStrings, &tokenbuf);
}



void Cmd_EndTokenizedStringKernel(struct CmdArgsPrivate *argsPriv, struct CmdArgs *args)
{
  //AssertCmdArgsConsistency(args);
  if ( args->nesting != -1 )
  {
/*    if ( args->nesting >= 8u
      && !Assert_MyHandler(
            __FILE__, __LINE__, 0,
            "args->nesting doesn't index CMD_MAX_NESTING\n\t%i not in [0, %i)",
            args->nesting,
            8) )
    {
      __debugbreak();
    }*/
    argsPriv->totalUsedArgvPool -= args->argc[args->nesting];
    argsPriv->totalUsedArgvPool -= args->argshift[args->nesting];
    argsPriv->totalUsedTextPool -= argsPriv->usedTextPool[args->nesting];
	args->nesting--;
    //AssertCmdArgsConsistency(args);
  }
}

struct CmdArgs * Cmd_Args()
{
  struct CmdArgs *cmd_args; // [sp+0h] [bp-4h]@1

  cmd_args = &tokenStrings;
/*
  cmd_args = (CmdArgs *)Sys_GetValue(4);
  if ( !cmd_args
    && !(unsigned __int8)Assert_MyHandler(
                           "c:\\projects_pc\\cod\\codsrc\\src\\bgame\\../qcommon/cmd.h",
                           203,
                           0,
                           "%s",
                           "cmd_args != NULL") )
  {
    __debugbreak();
  }*/
  return cmd_args;
}


struct CmdArgsPrivate* Cmd_ArgsPrivate()
{
  struct CmdArgsPrivate *cmd_argspriv; // [sp+0h] [bp-4h]@1

  cmd_argspriv = &tokenbuf;
/*
  cmd_args = (CmdArgs *)Sys_GetValue(4);
  if ( !cmd_args
    && !(unsigned __int8)Assert_MyHandler(
                           "c:\\projects_pc\\cod\\codsrc\\src\\bgame\\../qcommon/cmd.h",
                           203,
                           0,
                           "%s",
                           "cmd_args != NULL") )
  {
    __debugbreak();
  }*/
  return cmd_argspriv;
}


void Cmd_EndTokenizedString()
{
  struct CmdArgs *args;
  struct CmdArgsPrivate *argsPriv;

  args = Cmd_Args();
  argsPriv = Cmd_ArgsPrivate();
  Cmd_EndTokenizedStringKernel(argsPriv, args);
}

char* Cmd_Argsv( int arg, char * buff, int bufsize )
{

	int i;
	int cmd_argc = Cmd_Argc();
	buff[0] = 0;
	for ( i = arg ; i < cmd_argc ; i++ ) {
		if( strchr( Cmd_Argv(i),  ' ') )
		{
			Q_strcat( buff, bufsize, "\"" );
			Q_strcat( buff, bufsize, Cmd_Argv(i) );
			Q_strcat( buff, bufsize, "\"" );
		}else{
			Q_strcat( buff, bufsize, Cmd_Argv(i) );
		}

		if ( i != cmd_argc -1 ) {
			Q_strcat( buff, bufsize, " " );
		}
	}

	return buff;
}


void Cbuf_Execute( )
{
	cmd_insideCBufExecute = 1;
	Cbuf_Execute_stub(0, 0);
	cmd_insideCBufExecute = 0;
	Cbuf_SV_Execute();
}



qboolean Com_IsConfigMp(const char* filename)
{
   if(Q_stricmp(Com_GetFilenameSubString(filename), "config_mp.cfg") == 0)
   {
		return qtrue;
   }
   return qfalse;
}

qboolean Cmd_ExecFile(char *qpath)
{
  char *buf;

  FS_ReadFile(qpath, (void **)&buf);
  if ( buf )
  {
    Com_Printf(CON_CHANNEL_SYSTEM, "execing %s from disk\n", qpath);
    Cbuf_ExecuteBuffer(0, 0, buf);
    FS_FreeFile(buf);
    return 1;
  }
  return 0;
}

qboolean Cmd_ExecUserdir(char *qpath)
{
  char *buf;
  char finalpath[MAX_QPATH];
  fileHandle_t fp;
  int len;

  if ( Com_IsConfigMp(qpath) && com_playerProfile->string[0])
  {
		Com_BuildPlayerProfilePath(finalpath, sizeof(finalpath), NULL, "config_mp.cfg");
  }else{
		Com_sprintf(finalpath, sizeof(finalpath), "players/%s", qpath);
  }

  len = FS_SV_FOpenFileRead( finalpath, &fp );

  if(len < 1 || len > 0x100000)
  {
	return 0;
  }

  buf = malloc(len +1);
  if(buf == NULL)
  {
		return 0;
  }
  if(FS_Read(buf, len, fp) != len)
  {
		free(buf);
		return 0;
  }
	buf[len] = 0;
  FS_FCloseFile(fp);

  Com_Printf(CON_CHANNEL_SYSTEM, "execing %s from player dir\n", finalpath);
  Cbuf_ExecuteBuffer(0, 0, buf);
  free(buf);
  return 1;

}

void Cmd_Exec_f()
{
  char arg[MAX_QPATH];

  if ( Cmd_Argc() != 2 )
  {
    Com_Printf(CON_CHANNEL_DONT_FILTER, "exec <filename> : execute a script file\n");
  }else{

    Q_strncpyz(arg, Cmd_Argv(1), sizeof(arg));
	COM_DefaultExtension( arg, sizeof( arg ), ".cfg" );
    if ( Com_IsConfigMp(arg) )
    {
	  Com_Printf(CON_CHANNEL_SYSTEM, "^5Loading config_mp.cfg...\n");
      if(Cmd_ExecUserdir(arg) || Cmd_ExecFile(arg))
	  {
		return;
	  }
  	  Com_PrintError(CON_CHANNEL_DONT_FILTER, "couldn't exec %s\n", arg);
	  return;
    }

	if (com_useFastFiles->boolean && Cmd_ExecFastfile(arg))
	{
		return;
	}

	if(Cmd_ExecUserdir(arg) || Cmd_ExecFile(arg) )
    {
		return;
	}

	Com_PrintError(CON_CHANNEL_DONT_FILTER, "couldn't exec %s\n", arg);
  }
}

