#include "qcommon.h"
#include "ui_shared.h"
#include "client.h"


#define BYTE1(x)   BYTEn(x,  1)         // byte 1 (counting from 0)
#define BYTE2(x)   BYTEn(x,  2)
#define BYTE3(x)   BYTEn(x,  3)
#define BYTEn(x, n) (*((byte*)&(x)+n))

#define g_currentAsian (*(int*)0xCAE4C38)
#define loc_translate getcvaradr(0xCAE4C34)

int __cdecl SEH_GetCurrentLanguage()
{
  return loc_language->integer;
}



bool sub_539090(unsigned int a1, unsigned int a2)
{
  return a1 - 176 <= 0x18 && a2 - 161 <= 93;
}

bool sub_5390B0(unsigned int a1)
{
  unsigned int v1;

  v1 = a1 >> 8;
  return ((BYTE1(a1) >= 0xA1u && (byte)v1 <= 0xC6u) || (byte)(v1 + 55) <= 0x30u) && (((byte)a1 >= 0x40u && (byte)a1 <= 0x7Eu) || ((byte)a1 >= 0xA1u && (byte)a1 <= 0xFEu));
}


bool sub_5390E0(unsigned int a1, unsigned int a2)
{
  return ((a1 >= 0x81 && a1 <= 0x9F) || a1 - 224 <= 0xF) && ((a2 >= 0x40 && a2 <= 0x7E) || a2 - 128 <= 0x7C);
}

bool sub_539110(unsigned int a1)
{
  return (byte)(BYTE1(a1) + 127) <= 125u && (byte)a1 > 64u && (byte)a1 < 255u;
}



unsigned int SEH_DecodeLetter(byte firstChar, byte secondChar, int *usedCount)
{
  unsigned int v3; // edx@1
  int v4; // esi@1
  char v5; // al@3
  unsigned int result; // eax@5
  unsigned int v7; // ecx@6
  bool v8; // al@6

  v3 = firstChar;
  v4 = secondChar;
  if ( g_currentAsian )
  {
    switch ( loc_language->integer )
    {
      case 8:
        v5 = sub_539090(v3, secondChar);
        goto LABEL_4;
      case 9:
        v7 = secondChar + (v3 << 8);
        v8 = sub_5390B0(v7);
        goto LABEL_7;
      case 0xA:
        v5 = sub_5390E0(v3, secondChar);
LABEL_4:
        if ( !v5 )
          goto LABEL_11;
        *usedCount = 2;
        return v4 + (v3 << 8);
      case 0xB:
        v7 = secondChar + (v3 << 8);
        v8 = sub_539110(v7);
LABEL_7:
        if ( !v8 )
          goto LABEL_11;
        *usedCount = 2;
        result = v7;
        break;
      default:
        goto LABEL_11;
    }
  }
  else
  {
LABEL_11:
    *usedCount = 1;
    result = v3;
  }
  return result;
}



unsigned int __cdecl SEH_ReadCharFromString(const char **text)
{
  int usedCount;
  unsigned int letter;

  letter = SEH_DecodeLetter(*(unsigned char *)*text, *((unsigned char *)*text + 1), &usedCount);
  *text += usedCount;
  return letter;
}

int __cdecl SEH_PrintStrlen(const char *string)
{
  unsigned int c;
  int len;
  const char *p;

  if ( !string )
  {
    return 0;
  }
  len = 0;
  p = string;
  while ( *p )
  {
    c = SEH_ReadCharFromString(&p);
    if ( c == '^' && p && *p != '^' && *p >= '0' && *p <= '@' )
    {
      ++p;
    }
    else if ( c != '\n' && c != '\r' )
    {
      ++len;
    }
  }
  return len;
}



const char *SEH_StringEd_GetString(const char *findentry)
{
  if ( loc_translate && loc_translate->boolean && *findentry && findentry[1] )
  {
		return SE_GetString(findentry);
  }
  return findentry;
}