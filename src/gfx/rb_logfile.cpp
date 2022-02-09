#include "r_local.h"
#include "r_debug.h"

struct rLogFile_t{
    FILE* fp;
};

#define r_logFileGlob (*(rLogFile_t*)(0xD5844BC))

bool RB_IsLogging()
{
  if ( r_logFile->integer )
  {
    return r_logFileGlob.fp != 0;
  }
  return false;
}



void RB_LogPrint(const char *text)
{
  assert(r_logFile->integer);
  if ( RB_IsLogging() )
  {
    fprintf(r_logFileGlob.fp, "%s", text);
    fflush(r_logFileGlob.fp);
  }
}

void RB_LogPrintf(const char *fmt, ...)
{
    char msgbuf[4096];

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);
    va_end(ap);

    RB_LogPrint(msgbuf);
}

void RB_CloseLogFile()
{
  if ( r_logFileGlob.fp )
  {
    fclose(r_logFileGlob.fp);
    r_logFileGlob.fp = 0;
  }
}

void RB_OpenLogFile()
{
  int64_t aclock;
  tm *newtime;

  if ( !r_logFileGlob.fp )
  {
    r_logFileGlob.fp = fopen("dx.log", "wt");
    if ( r_logFileGlob.fp )
    {
      _time64(&aclock);
      newtime = _localtime64(&aclock);
      fprintf(r_logFileGlob.fp, "%s\n", asctime(newtime));
      fflush(r_logFileGlob.fp);
    }
  }
}

void RB_LogPrintfDebug(const char *fmt, ...)
{
  if(!r_logFile || !r_logFile->integer)
  {
    return;
  }
  char msgbuf[4096];

  va_list ap;
  va_start(ap, fmt);
  vsnprintf(msgbuf, sizeof(msgbuf), fmt, ap);
  va_end(ap);

  RB_LogPrint(msgbuf);
}

void RB_LogBlendOp(const char *format, int blendOp)
{
  const char *blendOpNames[6];

  blendOpNames[0] = "Disabled";
  blendOpNames[1] = "Add";
  blendOpNames[2] = "Subtract";
  blendOpNames[3] = "RevSubtract";
  blendOpNames[4] = "Min";
  blendOpNames[5] = "Max";
  assert(r_logFile->integer);
  assert( (unsigned int)blendOp < ARRAY_COUNT( blendOpNames ));

  RB_LogPrintf(format, blendOpNames[blendOp]);
}

void RB_LogBlend(const char *format, int blend)
{
  const char *blendNames[14];

  blendNames[0] = "Disabled";
  blendNames[1] = "Zero";
  blendNames[2] = "One";
  blendNames[3] = "SrcColor";
  blendNames[4] = "InvSrcColor";
  blendNames[5] = "SrcAlpha";
  blendNames[6] = "InvSrcAlpha";
  blendNames[7] = "DestAlpha";
  blendNames[8] = "InvDestAlpha";
  blendNames[9] = "DestColor";
  blendNames[10] = "InvDestColor";
  blendNames[11] = "SrcAlphaSat";
  blendNames[12] = "BlendFactor";
  blendNames[13] = "InvBlendFactor";
  
  assert(r_logFile->integer);
  assert(blend < ARRAY_COUNT( blendNames ));

  RB_LogPrintf(format, blendNames[blend]);
}

struct StateBitsTable
{
  int stateBits;
  const char *name;
};


void RB_LogFromTable(const char *keyName, int stateBits, int changedBits, int bitMask, int bitShift, StateBitsTable *table, int tableCount)
{
  int tableIndex;

  stateBits = (bitMask & stateBits) >> bitShift;
  for ( tableIndex = 0; tableIndex < tableCount; ++tableIndex )
  {
    if ( table[tableIndex].stateBits == stateBits )
    {
      RB_LogPrintf("---------- (%c)%-14s: %s\n", (bitMask & changedBits) != 0 ? 42 : 32, keyName, table[tableIndex].name);
      return;
    }
  }
  RB_LogPrintf("---------- (%c)%-14s: unknown - %08x\n", (bitMask & changedBits) != 0 ? 42 : 32, keyName, stateBits);
}


void RB_LogPrintState_0(int stateBits0, int changedBits0)
{

  StateBitsTable alphaTestTable[4];
  StateBitsTable cullTable[3];

  alphaTestTable[0].stateBits = 2048;
  alphaTestTable[0].name = "Disabled";
  alphaTestTable[1].stateBits = 4096;
  alphaTestTable[1].name = "GT0";
  alphaTestTable[2].stateBits = 0x2000;
  alphaTestTable[2].name = "GE255";
  alphaTestTable[3].stateBits = 12288;
  alphaTestTable[3].name = "GE128";
  cullTable[0].stateBits = 0x4000;
  cullTable[0].name = "None";
  cullTable[1].stateBits = 0x8000;
  cullTable[1].name = "Back";
  cullTable[2].stateBits = 0xC000;
  cullTable[2].name = "Front";

  assert(r_logFile->integer);

  RB_LogPrintf("---------- (%c)Blend         : ", (changedBits0 & 0x7FF) != 0 ? 42 : 32);
  if ( stateBits0 & 0x700 )
  {
    RB_LogBlendOp("%s( ", (stateBits0 & 0x700) >> 8);
    RB_LogBlend("%s, ", stateBits0 & 0xF);
    RB_LogBlend("%s )\n", (stateBits0 & 0xF0) >> 4);
  }
  else
  {
    RB_LogPrintf("Disabled\n");
  }
  RB_LogPrintf("---------- (%c)SeparateAlpha : ", (changedBits0 & 0x7FF0000) != 0 ? 42 : 32);
  if ( stateBits0 & 0x7000000 )
  {
    RB_LogBlendOp("%s( ", (stateBits0 & 0x7000000) >> 24);
    RB_LogBlend("%s, ", (stateBits0 & 0xF0000) >> 16);
    RB_LogBlend("%s )\n", (0xF00000 & stateBits0) >> 20);
  }
  else
  {
    RB_LogPrintf("Disabled\n");
  }
  RB_LogFromTable("AlphaTest", stateBits0, changedBits0, 0x3800, 0, alphaTestTable, 4);
  RB_LogPrintf("---------- (%c)Color Write   : ", (changedBits0 & 0x18000000) != 0 ? 42 : 32);
  if ( stateBits0 & 0x8000000 )
  {
    RB_LogPrintf("%s, ", "true");
  }
  else
  {
    RB_LogPrintf("%s, ", "false");
  }
  if ( stateBits0 & 0x8000000 )
  {
    RB_LogPrintf("%s, ", "true");
  }
  else
  {
    RB_LogPrintf("%s, ", "false");
  }
  if ( stateBits0 & 0x8000000 )
  {
    RB_LogPrintf("%s, ", "true");
  }
  else
  {
    RB_LogPrintf("%s, ", "false");
  }
  if ( stateBits0 & 0x10000000 )
  {
    RB_LogPrintf("%s\n", "true");
  }
  else
  {
    RB_LogPrintf("%s\n", "false");
  }
  RB_LogFromTable("Cull Face", stateBits0, changedBits0, 49152, 0, cullTable, 3);
}


struct StencilLogBits
{
  const char *description;
  int enableMask;
  int passShift;
  int failShift;
  int zfailShift;
  int funcShift;
};

void RB_LogBool(const char *keyName, int stateBits, int changedBits, int bitMask, const char *trueName, const char *falseName)
{
  changedBits = bitMask & changedBits;
  if ( bitMask & stateBits )
  {
    RB_LogPrintf("---------- (%c)%-14s: %s\n", changedBits != 0 ? 42 : 32, keyName, trueName);
  }
  else
  {
    RB_LogPrintf("---------- (%c)%-14s: %s\n", changedBits != 0 ? 42 : 32, keyName, falseName);
  }
}


void RB_LogStencilState(int stateBits1, int changedBits1, StencilLogBits *desc)
{
  StateBitsTable stencilFuncNames[8];
  StateBitsTable stencilOpNames[8];

  stencilFuncNames[0].stateBits = 0;
  stencilFuncNames[0].name = "Never";
  stencilFuncNames[1].stateBits = 1;
  stencilFuncNames[1].name = "Less";
  stencilFuncNames[2].stateBits = 2;
  stencilFuncNames[2].name = "Equal";
  stencilFuncNames[3].stateBits = 3;
  stencilFuncNames[3].name = "LessEqual";
  stencilFuncNames[4].stateBits = 4;
  stencilFuncNames[4].name = "Greater";
  stencilFuncNames[5].stateBits = 5;
  stencilFuncNames[5].name = "NotEqual";
  stencilFuncNames[6].stateBits = 6;
  stencilFuncNames[6].name = "GreaterEqual";
  stencilFuncNames[7].stateBits = 7;
  stencilFuncNames[7].name = "Always";
  stencilOpNames[0].stateBits = 0;
  stencilOpNames[0].name = "Keep";
  stencilOpNames[1].stateBits = 1;
  stencilOpNames[1].name = "Zero";
  stencilOpNames[2].stateBits = 2;
  stencilOpNames[2].name = "Replace";
  stencilOpNames[3].stateBits = 3;
  stencilOpNames[3].name = "IncrSat";
  stencilOpNames[4].stateBits = 4;
  stencilOpNames[4].name = "DecrSat";
  stencilOpNames[5].stateBits = 5;
  stencilOpNames[5].name = "Invert";
  stencilOpNames[6].stateBits = 6;
  stencilOpNames[6].name = "Incr";
  stencilOpNames[7].stateBits = 7;
  stencilOpNames[7].name = "Decr";

  RB_LogBool(va("Stencil %s", desc->description), stateBits1, changedBits1, desc->enableMask, "Enabled", "Disabled");
  if ( desc->enableMask & stateBits1 )
  {
    RB_LogFromTable(va("%s Func", desc->description), stateBits1, changedBits1, 7 << desc->funcShift, desc->funcShift, stencilFuncNames, 8);
    RB_LogFromTable(va("%s Pass", desc->description), stateBits1, changedBits1, 7 << desc->passShift, desc->passShift, stencilOpNames, 8);
    RB_LogFromTable(va("%s Fail", desc->description), stateBits1, changedBits1, 7 << desc->failShift, desc->failShift, stencilOpNames, 8);
    RB_LogFromTable(va("%s ZFail", desc->description), stateBits1, changedBits1, 7 << desc->zfailShift, desc->zfailShift, stencilOpNames, 8);
  }
}


void RB_LogPrintState_1(int stateBits1, int changedBits1)
{
  StencilLogBits stencilLogFront;
  StencilLogBits stencilLogBack;
  StateBitsTable depthTestTable[5];
  StateBitsTable polygonOffsetTable[4];

  depthTestTable[0].stateBits = 2;
  depthTestTable[0].name = "Disabled";
  depthTestTable[1].stateBits = 4;
  depthTestTable[1].name = "Less";
  depthTestTable[2].stateBits = 12;
  depthTestTable[2].name = "LessEqual";
  depthTestTable[3].stateBits = 8;
  depthTestTable[3].name = "Equal";
  depthTestTable[4].stateBits = 0;
  depthTestTable[4].name = "Always";
  polygonOffsetTable[0].stateBits = 0;
  polygonOffsetTable[0].name = "0";
  polygonOffsetTable[1].stateBits = 16;
  polygonOffsetTable[1].name = "1";
  polygonOffsetTable[2].stateBits = 32;
  polygonOffsetTable[2].name = "2";
  polygonOffsetTable[3].stateBits = 48;
  polygonOffsetTable[3].name = "shadowmap";
  stencilLogFront.description = "Front";
  stencilLogFront.enableMask = 64;
  stencilLogFront.passShift = 8;
  stencilLogFront.failShift = 11;
  stencilLogFront.zfailShift = 14;
  stencilLogFront.funcShift = 17;
  stencilLogBack.description = "Back";
  stencilLogBack.enableMask = 128;
  stencilLogBack.passShift = 20;
  stencilLogBack.failShift = 23;
  stencilLogBack.zfailShift = 26;
  stencilLogBack.funcShift = 29;

  assert(r_logFile->integer);

  RB_LogBool("Depth Write", stateBits1, changedBits1, 1, "Enabled", "Disabled");
  RB_LogFromTable("Depth Test", stateBits1, changedBits1, 14, 0, depthTestTable, 5);
  RB_LogFromTable("Polygon Offset", stateBits1, changedBits1, 48, 0, polygonOffsetTable, 4);
  RB_LogStencilState(stateBits1, changedBits1, &stencilLogFront);
  RB_LogStencilState(stateBits1, changedBits1, &stencilLogBack);
}


void RB_UpdateLogging()
{
  if ( RB_IsLogging() )
  {
    Cvar_SetInt(r_logFile, r_logFile->integer - 1);
  }
  if ( r_logFile->integer )
  {
    RB_OpenLogFile();
  }
  else
  {
    RB_CloseLogFile();
  }
}