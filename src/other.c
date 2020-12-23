#include "qcommon.h"
#include "ui_shared.h"
#include "client.h"

#include <stdbool.h>

void Scr_UpdateLoadScreen2( );

void Scr_UpdateLoadScreen()
{   
	if ( !com_useFastFiles->boolean )
      Scr_UpdateLoadScreen2();
}


signed int GetKeyBindingLocalizedString(int localClientNum, char *search, char *out, char a4)
{
  signed int bindings;
  char bindingString[256];
  const char* translated;
  const char* translated2;  
  
  bindings = CL_GetKeyBinding(localClientNum, search, bindingString);

  if ( !bindings )
  {
    Q_strncpyz(out, UI_SafeTranslateString("KEY_UNBOUND"), 256);
    return bindings;
  }
  
  if ( a4 && bindings > 1 )
  {
    bindings = 1;
  }
  if ( bindings == 1 )
  {
    translated = SEH_StringEd_GetString(bindingString);
    if ( !translated )
      translated = bindingString;
    Q_strncpyz(out, translated, 256);
    return bindings;
  }

  translated = SEH_StringEd_GetString(bindingString);
  translated2 = SEH_StringEd_GetString(bindingString +128);

  if ( !translated2 )
  {
    translated2 = &bindingString[128];
  }
  if ( !translated )
  {
    translated = bindingString;
  }
  Com_sprintf(out, 256, "%s %s %s", translated, UI_SafeTranslateString("KEY_OR"), translated2);
  return bindings;
}

int __stdcall MSS_FileSeekCallback(fileHandle_t f, int offset, unsigned int org)
{
  if ( org == 1 )
  {
    FS_Seek(f, offset, 0);
    return FS_FTell(f);
  }
  if ( org >= 1 )
  {
    if ( org == 2 )
    {
      FS_Seek(f, offset, 1);
      return FS_FTell(f);
    }
    return 0;
  }
  FS_Seek(f, offset, 2);
  return FS_FTell(f);

}

