
#include "q_shared.h"
#include "qcommon.h"
#include "client.h"

#define con (*((struct Console*)(0x08DC8C0)))


void Con_CheckResize();
void Con_DrawConsoleInternal();
void SCR_DrawSmallStringExt(signed int x, signed int y, const char* string, const float* color);

const char *builtinChannels[34] =
{
  "dontfilter",
  "error",
  "gamenotify",
  "boldgame",
  "subtitle",
  "obituary",
  "logfile_only",
  "console_only",
  "gfx",
  "sound",
  "files",
  "devgui",
  "profile",
  "ui",
  "client",
  "server",
  "system",
  "playerweap",
  "ai",
  "anim",
  "physics",
  "fx",
  "leaderboards",
  "live",
  "parserscript",
  "script",
  "mpspawnsystem",
  "coopinfo",
  "serverdemo",
  "ddl",
  "network",
  "scheduler",
  "task",
  "spu"
};

struct MessageLine
{
  int messageIndex;
  int textBufPos;
  int textBufSize;
  int typingStartTime;
  int lastTypingSoundTime;
  int flags;
};


struct Message
{
  int startTime;
  int endTime;
};


struct MessageWindow
{
  struct MessageLine *lines;
  struct Message *messages;
  char *circularTextBuffer;
  int textBufSize;
  int lineCount;
  int padding;
  int scrollTime;
  int fadeIn;
  int fadeOut;
  int textBufPos;
  int firstLineIndex;
  int activeLineCount;
  int messageIndex;
};

struct ConDrawInputGlob
{
  char autoCompleteChoice[64];
  int matchIndex;
  int matchCount;
  const char *inputText;
  int inputTextLen;
  byte hasExactMatch;
  byte mayAutoComplete;
  byte pad[2];
  float x;
  float y;
  float leftX;
  float fontHeight;
};


struct MessageBuffer
{
  char gamemsgText[4][2048];
  struct MessageWindow gamemsgWindows[4];
  struct MessageLine gamemsgLines[4][12];
  struct Message gamemsgMessages[4][12];
  char miniconText[4096];
  struct MessageWindow miniconWindow;
  struct MessageLine miniconLines[100];
  struct Message miniconMessages[100];
  char errorText[1024];
  struct MessageWindow errorWindow;
  struct MessageLine errorLines[5];
  struct Message errorMessages[5];
};

struct Console
{
  qboolean initialized;
  struct MessageWindow consoleWindow;
  struct MessageLine consoleLines[1024];
  struct Message consoleMessages[1024];
  char consoleText[32768];
  char textTempLine[512];
  unsigned int lineOffset;
  int displayLineOffset;
  int prevChannel;
  byte outputVisible;
  byte pad[3];
  int fontHeight;
  int visibleLineCount;
  int visiblePixelWidth;
  float screenMin[2];
  float screenMax[2];
  struct MessageBuffer messageBuffer[1];
  vec4_t color;
};



void Con_DrawConsole()
{
  if(cls.consoleFont == NULL)
	{
		return;
	}
	Con_CheckResize();
    
	if ( clientUIActives.keyCatchers & 1 )
	{
      Con_DrawConsoleInternal();
	}
}


#define con_versionColor colorYellow


void __cdecl SCR_DrawSmallStringExt(signed int x, signed int y, const char *string, const float *setColor)
{
  float th;

  th = (float)R_TextHeight(cls.consoleFont);
  R_AddCmdDrawText(string, 0x7FFFFFFF, cls.consoleFont, (float)x, (float)y + th, 1.0, 1.0, 0.0, setColor, 0);
}

void Con_DrawBuildString(float x, float y, float y2)
{
  float ydraw;
  char versionstr[256];
  
  Com_BuildVersionString(versionstr, sizeof(versionstr));

  ydraw = y2 - 16.0 + y;
  SCR_DrawSmallStringExt((signed int)x, (signed int)ydraw, versionstr, con_versionColor);
}

void Con_ClearNotify(int localClientNum)
{
  int i;

  i = 0;
  for(i = 0; i < 4; ++i)
  {
    con.messageBuffer[localClientNum].gamemsgWindows[i].textBufPos = 0;
    con.messageBuffer[localClientNum].gamemsgWindows[i].messageIndex = 0;
    con.messageBuffer[localClientNum].gamemsgWindows[i].firstLineIndex = 0;
    con.messageBuffer[localClientNum].gamemsgWindows[i].activeLineCount = 0;
  }
}


const char* Con_LinePrefix()
{
  return ">";
}





void ReplaceConsoleInputArgument(int replaceCount, const char *replacement)
{
  signed int cmdLineLen;
//  assert(replacement);

  if ( *replacement )
  {
    for ( cmdLineLen = strlen(g_consoleField.buffer); cmdLineLen && isspace(g_consoleField.buffer[cmdLineLen -1]); --cmdLineLen );
//    assert(replaceCount < cmdLineLen);
    Q_strncpyz(&g_consoleField.buffer[cmdLineLen - replaceCount], replacement, 256 - (cmdLineLen - replaceCount));
  }
}

void Con_AutoCompleteFromList(const char **strings, unsigned int stringCount, const char *prefix, char *completed, unsigned int sizeofCompleted)
{
  int prefixlen;
  const char *string;
  unsigned int charIndex;
  unsigned int stringIndex;

  prefixlen = strlen(prefix);
  *completed = 0;
  for ( stringIndex = 0; stringIndex < stringCount; ++stringIndex )
  {
    string = strings[stringIndex];
    if ( !Q_stricmpn(prefix, string, prefixlen) )
    {
      if ( *completed )
      {
        for ( charIndex = prefixlen; string[charIndex] == completed[charIndex] && completed[charIndex]; ++charIndex );
        completed[charIndex] = 0;
      }
      else
      {
        Q_strncpyz(completed, string, sizeofCompleted);
      }
    }
  }
}

const char* Con_TokenizeInput()
{
  const char *cmd;

  Cmd_TokenizeString(g_consoleField.buffer);
  cmd = Cmd_Argv(0);
  if ( *cmd == '\\' || *cmd == '/' )
  {
    ++cmd;
  }
  while ( isspace(*cmd) )
  {
    ++cmd;
  }
  return cmd;
}

void CompleteDvarArgument()
{
  const char *dvarName;
  cvar_t *dvar;
  const char *dvarValuePrefix;
  char dvarValue[256];

  dvarName = Con_TokenizeInput();
  dvar = Cvar_FindVar(dvarName);
  if(!dvar)
  {
    Cmd_EndTokenizedString();
    return;
  }

  if ( dvar->type == CVAR_ENUM )
  {
    dvarValuePrefix = Cmd_Argv(1);
    if ( *dvarValuePrefix )
    {
      Con_AutoCompleteFromList(dvar->enumStrings, dvar->enumCount, dvarValuePrefix, dvarValue, 0x100u);
      ReplaceConsoleInputArgument(strlen(dvarValuePrefix), dvarValue);
    }
    Cmd_EndTokenizedString();
  }
  else
  {
    Cmd_EndTokenizedString();
  }
}

void Con_InitClientAssets()
{
  Con_CheckResize();
}