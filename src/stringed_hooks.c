#include "qcommon.h"
#include "stringed_public.h"
#include "q_shared.h"

#include <ctype.h>
#include <string.h>


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

#define MAX_TOKENBUF_SZ 1024

int SEH_GetLocalizedTokenReference(char *token, const char *reference, const char *messageType, enum msgLocErrType_t errType)
{
    const char *translation;
    char tmpbuf[1024];

    translation = SEH_StringEd_GetString(reference);
    if ( !translation )
    {
        Com_sprintf(tmpbuf, sizeof(tmpbuf), "%s", reference);
        if ( errType == LOCMSG_NOERR )
        {
            return 0;
        }
        translation = tmpbuf;
    }
    Q_strncpyz(token, translation, MAX_TOKENBUF_SZ);
    return 1;
}


#define MAX_TEMP_STRINGS 10

const char* SEH_LocalizeTextMessage(const char *pszInputBuffer, const char *pszMessageType, enum msgLocErrType_t errType)
{
    char szInsertBuf[MAX_TOKENBUF_SZ];
    char szTokenBuf[MAX_TOKENBUF_SZ];
    int bLocOn;
    int iTokenLen;
    int iInsertLevel;
    int iLen;
    int bInsertEnabled;
    int insertIndex;
    const char *pszIn;
    int bLocSkipped;
    const char *pszTokenStart;
    int i;
    char *pszString;
    int digit;

    static int iCurrString;
    static char szStrings[MAX_TEMP_STRINGS][MAX_TOKENBUF_SZ];

    iCurrString = (iCurrString + 1) % MAX_TEMP_STRINGS;
    memset(szStrings[iCurrString], 0, sizeof(szStrings[0]));
    pszString = szStrings[iCurrString];
    iLen = 0;
    bLocOn = 1;
    bInsertEnabled = 1;
    iInsertLevel = 0;
    insertIndex = 1;
    bLocSkipped = 0;
    pszTokenStart = pszInputBuffer;
    pszIn = pszInputBuffer;
    while ( *pszTokenStart )
    {
        if ( *pszIn && *pszIn != 20 && *pszIn != 21 && *pszIn != 22 )
        {
            ++pszIn;
        }
        else
        {
            if ( pszIn > pszTokenStart )
            {
                iTokenLen = pszIn - pszTokenStart;
                Q_strncpyz(szTokenBuf, pszTokenStart, iTokenLen >= sizeof(szTokenBuf) ? sizeof(szTokenBuf) : iTokenLen + 1);
                if ( bLocOn )
                {
                    if ( !SEH_GetLocalizedTokenReference(szTokenBuf, szTokenBuf, pszMessageType, errType) )
                    {
                        return 0;
                    }
                    iTokenLen = strlen(szTokenBuf);
                }
                if ( iTokenLen + iLen >= (signed)sizeof(szTokenBuf) )
                {
                    Com_Printf(CON_CHANNEL_SYSTEM, "%s too long when translated: \"%s\"\n", pszMessageType, pszInputBuffer);
                    return 0;
                }
                for ( i = 0; i < iTokenLen - 2; ++i )
                {
                    if ( !strncmp(&szTokenBuf[i], "&&", 2u) && isdigit(szTokenBuf[i + 2]) )
                    {
                        if ( bInsertEnabled )
                        {
                            ++iInsertLevel;
                        }
                        else
                        {
                            szTokenBuf[i] = 22;
                            bLocSkipped = 1;
                        }
                    }
                }
                if ( iInsertLevel <= 0 || iLen <= 0 )
                {
                    Q_strncpyz(&pszString[iLen], szTokenBuf, sizeof(szTokenBuf) - iLen);
                }
                else
                {
                    for ( i = 0; i < iLen - 2; ++i )
                    {
                        if ( !strncmp(&pszString[i], "&&", 2u) && isdigit(pszString[i + 2]) )
                        {
                            digit = pszString[i + 2] - '0';
                            if ( !digit )
                            {
                                Com_Printf(CON_CHANNEL_SYSTEM, "%s cannot have &&0 as conversion format: \"%s\"\n", pszMessageType, pszInputBuffer);
                            }
                            if ( digit == insertIndex )
                            {
                                Q_strncpyz(szInsertBuf, &pszString[i + 3], sizeof(szInsertBuf));
                                pszString[i] = 0;
                                ++insertIndex;
                                break;
                            }
                        }
                    }
                    Q_strncpyz(&pszString[i], szTokenBuf, sizeof(szTokenBuf) - i);
                    Q_strncpyz(&pszString[iTokenLen + i], szInsertBuf, sizeof(szTokenBuf) - iTokenLen - i);
                    iLen -= 3;
                    --iInsertLevel;
                }
                iLen += iTokenLen;
            }
            bInsertEnabled = 1;
            if ( *pszIn == 20 )
            {
                bLocOn = 1;
                ++pszIn;
            }
            else if ( *pszIn == 21 )
            {
                bLocOn = 0;
                ++pszIn;
            }
            if ( *pszIn == 22 )
            {
                bInsertEnabled = 0;
                ++pszIn;
            }
            pszTokenStart = pszIn;
        }
    }
    if ( bLocSkipped )
    {
        for ( i = 0; i < iLen; ++i )
        {
            if ( pszString[i] == 22 )
            {
                pszString[i] = '%';
            }
        }
    }
    return pszString;
}
