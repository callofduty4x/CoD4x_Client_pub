#include "client.h"
#include "cg_shared.h"
#include "ui_shared.h"

/*
void CG_SetNextSnap_PlayerRenamed(clientInfo_t* ci, int* offsetcs)
{
  const char* msg;
  clientState_t* cs;

  cs = (clientState_t*)(offsetcs - 1); //compiler optimization issue

  if ( strcmp(ci->name, cs->name ))
  {
    if ( ci->name[0] )
    {
      msg = UI_SafeTranslateString("CGAME_PLAYERRENAMES");
      CG_GameMessage(va("%s^7 %s %s", ci->name, msg, cs->name));
    }
    Q_strncpyz(ci->name, cs->name, sizeof(ci->name));
  }
}
*/


void CL_NetAddDebugString( const char* formattedstr)
{
  signed int duration;
  vec3_t origin;
  float scale;
  vec4_t color;
  char text[1024];

//parsed string: "x y z;r g b a;s;d;string"

  int num = sscanf(formattedstr, "%f %f %f;%f %f %f %f;%f;%d;%s", &origin[0], &origin[1], &origin[2], 
          &color[0], &color[1], &color[2], &color[3], &scale, &duration, text);

  if(num != 10)
  {
    return;
  }
  CL_AddDebugString(origin, color, scale, text, qfalse, duration);
}

void CL_NetAddDebugStarWithText( const char* formattedstr)
{
  signed int duration;
  vec3_t point;
  vec4_t starColor, textColor;
  float fontsize;
  char textbuf[1024];
  const char* text;


  int num = sscanf(formattedstr, "%f %f %f;%f %f %f %f;%f %f %f %f;%f;%d;%s", &point[0], &point[1], &point[2], 
          &starColor[0], &starColor[1], &starColor[2], &starColor[3], 
          &textColor[0], &textColor[1], &textColor[2], &textColor[3], 
          &fontsize, &duration, textbuf);

  if(num != 14 && num != 13)
  {
    return;
  }
  if(num == 13)
  {
    text = NULL;
  }else{
    text = textbuf;
  }

  CL_AddDebugStarWithText(point, starColor, textColor, text, fontsize, duration, qfalse);
}

void CL_NetAddDebugLine( const char* formattedstr)
{
  signed int duration;
  vec3_t start, end;
  vec4_t color;
  int depthTest;

  int num = sscanf(formattedstr, "%f %f %f;%f %f %f;%f %f %f %f;%d;%d", 
          &start[0], &start[1], &start[2], &end[0], &end[1], &end[2], 
          &color[0], &color[1], &color[2], &color[3], 
          &depthTest, &duration);

  if(num != 12)
  {
    return;
  }
  CL_AddDebugLine(start, end, color, depthTest, duration, qfalse);

}

void CL_NetAddDebugData( )
{
  const char* typestr = Cmd_Argv(1);
  char type = *typestr;
  char formattedstr[1024];
	Cmd_Argsv( 2, formattedstr, sizeof(formattedstr) );
  
  if(strlen(formattedstr) > 1023)
  {
    return;
  }

  switch(type){
    case 't':
      CL_NetAddDebugString( formattedstr);
      break;
    case 'S':
      CL_NetAddDebugStarWithText( formattedstr);
      break;
    case 'l':
      CL_NetAddDebugLine(formattedstr);
    default:
      break;
  }

}
