/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "q_shared.h"
#include "qcommon.h"
#include <stdlib.h>
#include <float.h>
#include <ctype.h>

// nothing outside the Cvar_*() functions should modify these fields!

#define cvar_cheats getcvaradr(0xCBA7404)
#define cvarCriticalSectionObject (*((critSectionLock_t*)(0xCBA73FC))) 
#define numCvars *((unsigned int*)(0xCBA73F8))
#define cvar_hashTable ((cvar_t**)(0xCBAB408))
#define cvarList ((cvar_t**)(0xCBA7408))
#define cvar_storage ((cvar_t*)(0xCBAB808))
#define areCvarsSorted *((qboolean*)(0xD5EC4F8))

bool cvar_latchedSet;




int Cvar_ValuesEqual(char cvarType, CvarValue_t cmp1, CvarValue_t cmp2)
{
 
  switch ( cvarType )
  {
    case CVAR_BOOL:
      return cmp1.boolean == cmp2.boolean;
	case CVAR_COLOR:
    case CVAR_INT:
    case CVAR_ENUM:
      return cmp1.integer == cmp2.integer;
    case CVAR_FLOAT:
      if ( cmp2.floatval != cmp1.floatval )
        return 0;
      return 1;
    case CVAR_VEC2:
      return cmp2.vec2[0] == cmp1.vec2[0] && cmp2.vec2[1] == cmp1.vec2[1];
    case CVAR_VEC3:
      return cmp2.vec3[0] == cmp1.vec3[0] && cmp2.vec3[1] == cmp1.vec3[1] && cmp2.vec3[2] == cmp1.vec3[2];
    case CVAR_VEC4:
      return cmp2.vec4[0] == cmp1.vec4[0] && cmp2.vec4[1] == cmp1.vec4[1] && cmp2.vec4[2] == cmp1.vec4[2] && cmp2.vec4[3] == cmp1.vec4[3];
    case CVAR_STRING:
      return strcmp(cmp1.string, cmp2.string) == 0;
    default:
	  break;
  }
  return 0;
}



qboolean Cvar_HasLatchedValue(cvar_t *cvar)
{
	CvarValue_t curVal;
	CvarValue_t latchedVal;
	
	memcpy(curVal.vec4, cvar->vec4, sizeof(curVal.vec4));
	memcpy(latchedVal.vec4, cvar->latchedVec4, sizeof(latchedVal.vec4));
	
	return Cvar_ValuesEqual(cvar->type, curVal, latchedVal) == 0;

}



void Cvar_VectorDomainToString(char *outstring, int dim, CvarLimits_t limits)
{
  if ( limits.fmin == -3.402823466385289e38 )
  {
    if ( limits.fmax == 3.402823466385289e38 )
	{
      Com_sprintf(outstring, 0x400u, "Domain is any %iD vector", dim);
	  return;
	}
    Com_sprintf(outstring, 0x400u, "Domain is any %iD vector with components %g or smaller", dim, limits.fmax);
	return;
  }
  
  if ( limits.fmax == 3.402823466385289e38 )
  {
    Com_sprintf(outstring, 0x400u, "Domain is any %iD vector with components %g or bigger", dim, limits.fmin);
	return;
  }
  Com_sprintf(outstring, 0x400u, "Domain is any %iD vector with components from %g to %g", dim, limits.fmin, limits.fmax);
  
}

char * Cvar_DomainToString(char cvarType, char *outString, CvarLimits_t limits)
{
	
  int i, len;

  switch ( cvarType )
  {
    case CVAR_BOOL:
      Com_sprintf(outString, 0x400u, "Domain is 0 or 1");
      return outString;

    case CVAR_INT:
      if ( limits.imin == 0x80000000 )
      {

		if ( limits.imax == 0x7FFFFFFF )
        {
          Com_sprintf(outString, 0x400u, "Domain is any integer");
          return outString;
        }
        Com_sprintf(outString, 0x400u, "Domain is any integer %i or smaller", limits.imax);
        return outString;

      }
      
	  if ( limits.imax == 0x7FFFFFFF )
      {
        Com_sprintf(outString, 0x400u, "Domain is any integer %i or bigger", limits.imin);
        return outString;
      }
      Com_sprintf(outString, 0x400u, "Domain is any integer from %i to %i", limits.imin, limits.imax);
      return outString;
      
    case CVAR_FLOAT:
      if ( limits.fmin == -3.402823466385289e38 )
      {
        if ( limits.fmax == 3.402823466385289e38 )
        {
          Com_sprintf(outString, 0x400u, "Domain is any number");
          return outString;
        }
        Com_sprintf(outString, 0x400u, "Domain is any number %g or smaller", limits.fmax);
        return outString;
      }
      
	  if ( limits.fmax == 3.402823466385289e38 )
      {
        Com_sprintf(outString, 0x400u, "Domain is any number %g or bigger", limits.fmin);
        return outString;
      }
      Com_sprintf(outString, 0x400u, "Domain is any number from %g to %g", limits.fmin, limits.fmax);
      return outString;

    case CVAR_VEC2:
      Cvar_VectorDomainToString(outString, 2, limits);
      return outString;
	  
    case CVAR_VEC3:
      Cvar_VectorDomainToString(outString, 3, limits);
      return outString;
	  
    case CVAR_VEC4:
      Cvar_VectorDomainToString(outString, 4, limits);
      return outString;
	  
    case CVAR_STRING:
      Com_sprintf(outString, 0x400u, "Domain is any text");
      return outString;
	  
    case CVAR_ENUM:
      len = Com_sprintf(outString, 0x400u, "Domain is one of the following:");

      for(i = 0; i < limits.enumCount && len > 0; ++i )
	  {
		len += Com_sprintf(outString + len, 0x400u - len, "\n  %2i: %s", i, limits.enumStrings[i]); 
	  }
      return outString;
    
	case CVAR_COLOR:
      Com_sprintf(outString, 0x400u, "Domain is any 4-component color, in RGBA format");
	  return outString;

    default:
      outString[0] = 0;
	  return outString;
  }
  return outString;
}


void Cvar_SetCommand(const char *var_name, const char *var_value);
//cvar_t* Cvar_Registernew_stub(const char* name, unsigned char type, unsigned short flags, const char* description ,CvarValue_t value, CvarLimits_t limits);
//#define Cvar_Registernew(name, type ,flags, value, limits, desc) Cvar_Registernew_stub(name, type ,flags, desc, value, limits)
int Cvar_SetVariant( cvar_t *var, CvarValue_t value ,int cvarsetsource );
void Cvar_Reregister(cvar_t *var, const char *cvarname, int type, unsigned short flags, CvarValue_t value, CvarLimits_t limits, const char *description);
static cvar_t *Cvar_Register(const char* var_name, cvarType_t type, unsigned short flags, CvarValue_t value, CvarLimits_t limits, const char *description);


cvar_t* Cvar_RegisterString(const char* name, const char* string, unsigned short flags, const char* description){

	cvar_t* cvar;
	CvarLimits_t limits;
	CvarValue_t value;
	
	limits.imin = 0;
	limits.imax = 0;	
	
	value.string = string;
	
	cvar = Cvar_Register(name, CVAR_STRING, flags, value, limits, description);
	return cvar;
}

cvar_t* Cvar_RegisterBool(const char* name, qboolean boolean, unsigned short flags, const char* description){

	cvar_t* cvar;
	CvarLimits_t limits;
	CvarValue_t value;
	
	limits.imin = 0;
	limits.imax = 0;	
	
	value.boolean = boolean;
	
	cvar = Cvar_Register(name, CVAR_BOOL, flags, value, limits, description);
	return cvar;
}

cvar_t* Cvar_RegisterInt(const char* name, int integer, int min, int max, unsigned short flags, const char* description){

	cvar_t* cvar;
	CvarLimits_t limits;
	CvarValue_t value;
	
	limits.imin = min;
	limits.imax = max;	
	
	value.integer = integer;
	
	cvar = Cvar_Register(name, CVAR_INT, flags, value, limits, description);
	return cvar;
}


cvar_t* Cvar_RegisterFloat(const char* name, float val, float min, float max, unsigned short flags, const char* description){

	cvar_t* cvar;
	CvarLimits_t limits;
	CvarValue_t value;
	
	limits.fmin = min;
	limits.fmax = max;	
	
	value.floatval = val;
	
	cvar = Cvar_Register(name, CVAR_FLOAT, flags, value, limits, description);

	return cvar;

}

cvar_t* Cvar_RegisterVec2(const char* name, float x, float y, float min, float max, unsigned short flags, const char* description){

	cvar_t* cvar;
	CvarLimits_t limits;
	CvarValue_t value;
	
	limits.fmin = min;
	limits.fmax = max;
	
	value.vec2[0] = x;
	value.vec2[1] = y;

	cvar = Cvar_Register(name, CVAR_VEC2, flags, value, limits, description);

	return cvar;

}

cvar_t* Cvar_RegisterVec3(const char* name, float x, float y, float z, float min, float max, unsigned short flags, const char* description){

	cvar_t* cvar;
	CvarLimits_t limits;
	CvarValue_t value;
	
	limits.fmin = min;
	limits.fmax = max;
	
	value.vec3[0] = x;
	value.vec3[1] = y;
	value.vec3[2] = z;

	cvar = Cvar_Register(name, CVAR_VEC3, flags, value, limits, description);

	return cvar;

}

cvar_t* Cvar_RegisterVec4(const char* name, float x, float y, float z, float imag, float min, float max, unsigned short flags, const char* description){

	cvar_t* cvar;
	CvarLimits_t limits;
	CvarValue_t value;
	
	limits.fmin = min;
	limits.fmax = max;
	
	value.vec4[0] = x;
	value.vec4[1] = y;
	value.vec4[2] = z;
	value.vec4[3] = imag;

	cvar = Cvar_Register(name, CVAR_VEC4, flags, value, limits, description);

	return cvar;

}

cvar_t* Cvar_RegisterColor(const char* name, float r, float g, float b, float alpha, unsigned short flags, const char* description){

	cvar_t* cvar;
	CvarLimits_t limits;
	CvarValue_t value;
	
	limits.fmin = 0.0;
	limits.fmax = 0.0;
	
	value.color.red = (byte)(0xff * r);
	value.color.green = (byte)(0xff * g);
	value.color.blue = (byte)(0xff * b);
	value.color.alpha = (byte)(0xff * alpha);

	cvar = Cvar_Register(name, CVAR_COLOR, flags, value, limits, description);

	return cvar;

}


cvar_t* Cvar_RegisterEnum(const char* name, const char** strings, int integer, unsigned short flags, const char* description){

	cvar_t* cvar;
	CvarLimits_t domain;
	CvarValue_t value;
	int enumCount;
	
	for(enumCount = 0; strings[enumCount] != NULL; ++enumCount);

	domain.enumCount = enumCount;
	domain.enumStrings = strings;

	if(integer < 0 || integer >= enumCount)
	{
		integer = 0;
	}
	value.integer = integer;
	
	cvar = Cvar_Register(name, CVAR_ENUM, flags, value, domain, description);

	return cvar;

}





/*
============
Cvar_SetFloat
============
*/
void Cvar_SetFloat( cvar_t* cvar, float value) {
	CvarValue_t cval;
	cval.floatval = value;
	Cvar_SetVariant( cvar, cval , 0);
}

/*
============
Cvar_SetInt
============
*/
void Cvar_SetInt( cvar_t* cvar, int value){
	CvarValue_t cval;
	cval.integer = value;
	Cvar_SetVariant( cvar, cval , 0);
}


/*
============
Cvar_SetEnum
============
*/
void Cvar_SetEnum( cvar_t* cvar, int value){
	CvarValue_t cval;
	cval.integer = value;
	Cvar_SetVariant( cvar, cval , 0);
}


/*
============
Cvar_SetBool
============
*/
void Cvar_SetBool( cvar_t* cvar, qboolean value){
	CvarValue_t cval;
	cval.boolean = value;
	Cvar_SetVariant( cvar, cval , 0);
}

/*
============
Cvar_SetString
============
*/
void Cvar_SetString( cvar_t* cvar, const char* value){
	CvarValue_t cval;
	cval.string = value;
	Cvar_SetVariant( cvar, cval , 0);
}

/*
============
Cvar_SetVec2
============
*/
void Cvar_SetVec2( cvar_t* cvar, float x, float y){
	CvarValue_t cval;
	cval.vec2[0] = x;
	cval.vec2[1] = y;
	Cvar_SetVariant( cvar, cval , 0);
}

/*
============
Cvar_SetVec3
============
*/
void Cvar_SetVec3( cvar_t* cvar, float x, float y, float z){
	CvarValue_t cval;
	cval.vec3[0] = x;
	cval.vec3[1] = y;
	cval.vec3[2] = z;
	Cvar_SetVariant( cvar, cval , 0);
}

/*
============
Cvar_SetVec4
============
*/
void Cvar_SetVec4( cvar_t* cvar, float x, float y, float z, float imag){
	CvarValue_t cval;
	cval.vec4[0] = x;
	cval.vec4[1] = y;
	cval.vec4[2] = z;
	cval.vec4[3] = imag;
	Cvar_SetVariant( cvar, cval , 0);
}

/*
============
Cvar_SetColor
============
*/
void Cvar_SetColor( cvar_t* cvar, float r, float g, float b, float alpha){
	CvarValue_t cval;

	cval.color.red = (byte)((float)0xff * r);
	cval.color.green = (byte)((float)0xff * g);
	cval.color.blue = (byte)((float)0xff * b);
	cval.color.alpha = (byte)((float)0xff * alpha);

	Cvar_SetVariant( cvar, cval , 0);
}
/*
============
Cvar_Set
============
*/
cvar_t* Cvar_Set(const char *varname, const char *valueString)
{
	return Cvar_SetFromStringByNameFromSource(varname, valueString, 0);
}

void Cvar_ClearModified(cvar_t* cvar){
	cvar->modified = 0;
}

const char *Cvar_ToString(cvar_t *var)
{
  switch ( var->type )
  {
    case CVAR_BOOL:
      if ( var->boolean )
        return "1";
	  return "0";
    case CVAR_INT:
      return va("%i", var->integer);
    case CVAR_FLOAT:
      return va("%g", var->floatval);
    case CVAR_VEC2:
      return va("%g %g", var->vec2[0], var->vec2[1]);
    case CVAR_VEC3:
      return va("%g %g %g", var->vec3[0], var->vec3[1], var->vec3[2]);
    case CVAR_VEC4:
      return va("%g %g %g %g", var->vec4[0], var->vec4[1], var->vec4[2], var->vec4[3]);
    case CVAR_COLOR:
      return va("%g %g %g %g", (double)var->color.red * 0.003921568859368563, (double)var->color.green * 0.003921568859368563,
                 (double)var->color.blue * 0.003921568859368563, (double)var->color.alpha * 0.003921568859368563);
    case CVAR_ENUM:
      if ( var->enumCount <= 0 || var->enumCount <= var->integer || var->integer < 0)
        return "";
	  return var->enumStrings[var->integer];
    case CVAR_STRING:
      return va("%s", var->string);
  }
  return "";
}



const char *Cvar_ValueToString(cvar_t* icvar, CvarValue_t val)
{
  cvar_t var;
  memcpy(&var, icvar, sizeof(var));
  memcpy(var.vec4, &val, sizeof(var.vec4));
  return Cvar_ToString(&var);
}

qboolean Dvar_ValueEqualStringToString(const char *varname, const char* ctx, const char * str1, const char *str2)
{
  cvar_t *var;
  const char *varstr;

  var = Cvar_FindMalleable(varname);
  if ( !var )
  {
    Com_Printf(CON_CHANNEL_SYSTEM, "%s: cannot find dvar %s\n", ctx, varname);
	return 0;
  }
  varstr = Cvar_ToString( var );
  return (Q_stricmp(str1, varstr) == 0) == (Q_stricmp(ctx, str2) == 0);
}


void Cvar_SetStringByName(const char *var_name, const char *valuestr)
{
  cvar_t *var;

  var = Cvar_FindMalleable(var_name);
  if ( var )
    Cvar_SetString(var, valuestr);
  else
    Cvar_RegisterString(var_name, valuestr, 0x4000u, "External Dvar");
}

void Cvar_SetBoolByName(const char *var_name, qboolean val)
{
  cvar_t *var;

  var = Cvar_FindMalleable(var_name);
  if ( var )
    Cvar_SetBool(var, val);
  else
    Cvar_RegisterBool(var_name, val, 0x4000u, "External Dvar");
}

void Cvar_SetIntByName(const char *var_name, int val)
{
  cvar_t *var;

  var = Cvar_FindMalleable(var_name);
  if ( var )
    Cvar_SetInt(var, val);
  else
    Cvar_RegisterInt(var_name, val, 0x80000000, 0x7fffffff, 0x4000u, "External Dvar");
}

void Cvar_SetFloatByName(const char *var_name, float val)
{
  cvar_t *var;

  var = Cvar_FindMalleable(var_name);
  if ( var )
    Cvar_SetFloat(var, val);
  else
    Cvar_RegisterFloat(var_name, val, FLT_MAX, -FLT_MAX, 0x4000u, "External Dvar");
}

const char* Cvar_GetVariantString(const char* var_name)
{
	cvar_t* var;
	var = Cvar_FindMalleable(var_name);
	if(var == NULL)
	{
		return "";
	}
	return Cvar_ToString(var);
}

int Cvar_GetInt(const char* var_name)
{
	cvar_t* var;
	var = Cvar_FindMalleable(var_name);
	if(var == NULL)
	{
		return 0;
	}
	
	if(var->type == CVAR_INT || var->type == CVAR_ENUM)
	{
		return var->integer;
	}
	if(var->type == CVAR_BOOL)
	{
		if(var->boolean)
		{
			return 1;
		}
		return 0;
	}
	if(var->type == CVAR_STRING)
	{
		return atoi(var->string);
	}
	return 0;
}

qboolean Cvar_GetBool(const char* var_name)
{
	cvar_t* var;
	var = Cvar_FindMalleable(var_name);
	if(var == NULL)
	{
		return qfalse;
	}
	if(var->type == CVAR_BOOL)
	{
		if(var->boolean)
		{
			return qtrue;
		}
		return qfalse;
	}
	if(var->type == CVAR_STRING)
	{
		return atoi(var->string) != 0;
	}
	return qfalse;
}

void Cvar_Reset(cvar_t* cvar)
{
	CvarValue_t cval;
	cval.vec4[0] = cvar->resetVec4[0];
	cval.vec4[1] = cvar->resetVec4[1];
	cval.vec4[2] = cvar->resetVec4[2];
	cval.vec4[3] = cvar->resetVec4[3];
	Cvar_SetVariant( cvar, cval , 0);
}

void Cvar_ResetByName(const char* var_name)
{
	cvar_t* cvar = Cvar_FindMalleable(var_name);
	if(cvar == NULL)
	{
		return;
	}
	Cvar_Reset(cvar);
}

/*
============
Cvar_VariableString
============
*/
const char *Cvar_VariableString( const char *var_name ) {
	cvar_t *var;
	
	var = Cvar_FindMalleable( var_name );
	if (var == NULL)
		return "";

	return Cvar_ToString(var);
}

/*
============
Dvar_MakeExplicitType_StringReset
This is a crash fixup insertion into Dvar_MakeExplicitType() - New: Check if type = CVAR_STRING
============
*/
/*
void Dvar_MakeExplicitType_StringReset(cvar_t* var)
{
  int index;

  
  
  
  if ( var->type != CVAR_STRING )
  {
    vstring = (char *)var->string;
    if ( vstring )
    {
      if ( vstring != var->latchedString && vstring != var->resetString )
      {
        index = SL_FindString(vstring, strlen(vstring) + 1]);
        SL_RemoveRefToString(index);
		var->string = NULL;
      }
    }
  }

  var->string = NULL;
  
 // if(var->type != CVAR_STRING)
  {
	//return;
  }

  char* latchedString = var->latchedString;
  if ( latchedString && latchedString != var->resetString )
  {
    index = SL_FindString(latchedString, strlen(latchedString) +1);
    SL_RemoveRefToString(index);
  }
  
  var->latchedString = NULL;
 
  char* resetString = var->resetString;
  if ( resetString && resetString != var->string )
  {
    index = SL_FindString(resetString, strlen(resetString) + 1);
    SL_RemoveRefToString(index);
  }
  
  var->resetString = NULL;
}
*/
void Cvar_ReinterpretCvar(cvar_t *var, const char *cvarname, byte type, unsigned short flags, CvarValue_t value, CvarLimits_t domain);
void Cvar_MakeExplicitType(cvar_t *var, byte type,  unsigned short flags, CvarValue_t value, CvarLimits_t domain);
void Cvar_SetLatchedValue(cvar_t *var, CvarValue_t value);
void Cvar_PerformUnregistration(cvar_t *var);

void Cvar_ReinterpretCvar(cvar_t *var, const char *cvarname, byte type, unsigned short flags, CvarValue_t value, CvarLimits_t domain)
{

  if ( var->flags & CVAR_USER_CREATED && !(flags & CVAR_USER_CREATED) )
  {
    Cvar_PerformUnregistration(var);
	SL_RemoveString(var->name);
    var->flags &= 0xBFFFu;
    var->name = (char*)cvarname;
	
	
	if(var->type != CVAR_STRING)
	{
		Com_Error(ERR_FATAL, "Cvar_ReinterpretCvar(): Cvar is not from string type but is supposed to be");
	}
	
    Cvar_MakeExplicitType(var, type, flags, value, domain);
  }
}


void Cvar_ApplyNewEnumDomain(cvar_t *var, CvarLimits_t domain, int newdefault)
{
	
	var->enumStrings = domain.enumStrings;
	var->enumCount = domain.enumCount;
	
	if(var->integer >= var->enumCount)
	{
		var->integer = newdefault;
	}
	
	if(var->integer >= var->enumCount)
	{
		var->integer = var->enumCount -1;
	}
	
	if(var->integer < 0)
	{
		var->integer = 0;
	}
	
	var->resetInteger = newdefault;
	
	if(var->resetInteger >= var->enumCount)
	{
		var->resetInteger = var->enumCount -1;
	}
	
	if(var->resetInteger < 0)
	{
		var->resetInteger = 0;
	}
	
	if(var->latchedInteger >= var->enumCount || var->latchedInteger < 0)
	{
		var->latchedInteger = var->integer;
	}

}


void Cvar_Reregister(cvar_t *var, const char *cvarname, int type, unsigned short flags, CvarValue_t value, CvarLimits_t domain, const char *description)
{
  CvarValue_t reset;
  CvarValue_t latched;
  
  if(var->type == CVAR_ENUM && type == CVAR_ENUM)
  {
	Cvar_ApplyNewEnumDomain(var, domain, value.integer);
  }
  
  if ( (flags ^ var->flags) & CVAR_USER_CREATED )
  {
    Cvar_ReinterpretCvar(var, cvarname, type, flags, value, domain);
  }
  if ( var->flags & CVAR_USER_CREATED && var->type != type )
  {
    Cvar_MakeExplicitType(var, type, flags, value, domain);
  }
  var->flags |= flags;

  if ( description )
    var->description = (char *)description;
  if ( var->flags & CVAR_CHEAT && cvar_cheats && !cvar_cheats->boolean )
  {
	memcpy(&reset, var->resetVec4, sizeof(reset));
    Cvar_SetVariant(var, reset, 0);
    Cvar_SetLatchedValue(var, reset);
  }
  if ( var->flags & CVAR_LATCH )
  {
	memcpy(&latched, var->latchedVec4, sizeof(latched));
    Cvar_SetVariant(var, latched, 0);
  }
  
}

void Cvar_AssignResetString(cvar_t *var, const char **outstr, const char *string)
{
  const char *str;
  
  str = var->string;

  if ( (!str || (string != str && strcmp(string, var->string))) && ((str = var->latchedString) == 0
  || (string != str && strcmp(string, var->latchedString))))
  {
    *outstr = CopyString(string);
  }
  else
  {
    *outstr = str;
  }
}

void Cvar_PerformUnregistration(cvar_t *var)
{
  CvarValue_t latchedVal;
  CvarValue_t resetVal;
 
  const char *valueasstring;


  if ( !(var->flags & CVAR_USER_CREATED) )
  {
    var->flags = var->flags | CVAR_USER_CREATED;
    var->name = (char*)CopyString(var->name);
  }
  if ( var->type != CVAR_STRING )
  {
	memcpy(&latchedVal, var->latchedVec4, sizeof(latchedVal));
	valueasstring = Cvar_ValueToString(var, latchedVal);
    
    var->string = (char*)CopyString(valueasstring);

/*
	This can cause crash at any time as a cvar which is not a string type cvar can never have valid reset/latched strings
	
	if ( var->latchedString && var->latchedString != var->string && var->latchedString != var->resetString )
    {
	  SL_RemoveString( var->latchedString );
    }
*/
    var->latchedString = var->string;
/*
    if ( var->resetString && var->resetString != var->string )
    {
	  SL_RemoveString( var->resetString );
    }
*/
    var->resetString = NULL;
	
	memcpy(&resetVal, var->resetVec4, sizeof(resetVal));

    valueasstring = Cvar_ValueToString(var, resetVal);
    Cvar_AssignResetString(var, &var->resetString, valueasstring);
    var->type = CVAR_STRING;

  }
}



typedef struct
{
	int numStrings;
	const char **stringlist;
}CvarEnumStrings_t;


int Cvar_StringToEnum(CvarEnumStrings_t *a1, const char *str1)
{
  int itemCount;
  const char **string;
  int result;
  int v9;
  const char **v10;
  int i;
  
  for(itemCount = 0, string = a1->stringlist; a1->numStrings > itemCount; ++itemCount, ++string)
  {

    if ( !Q_stricmp(str1, *string) )
	{
        return itemCount;
	}
  }


  result = 0;
  i = 0;
  if ( str1[i] )
  {
    while ( str1[i] >= '0' && str1[i] <= '9' )
    {
      result = str1[i] + 10 * result - '0';
      ++i;
      if ( !str1[i] )
      {
        if ( result < 0 || result >= a1->numStrings)
		{
          goto LABEL_14;
		}
		return result;
      }
    }
	return -1337;
  }


    if ( result < a1->numStrings )
      return result;
LABEL_14:
    v9 = strlen(str1);
    itemCount = 0;
    if ( a1->numStrings > 0 )
    {
      v10 = a1->stringlist;
      while ( Q_stricmpn(str1, *v10, v9) )
      {
        ++itemCount;
        ++v10;
        if ( itemCount >= a1->numStrings )
          return -1337;
      }
      return itemCount;
    }

  return -1337;
}

qboolean Cvar_Command()
{
  cvar_t *cvar;
  CvarValue_t v3;
  CvarLimits_t domain;
  char domainString[1024];
  char cmdargs[8192];
  
	cvar = Cvar_FindMalleable(Cmd_Argv(0));
  
	if ( cvar == NULL)
	{
		return qfalse;
	}
    
	if ( Cmd_Argc() == 1 )
  {
      memcpy(&v3, cvar->vec4, sizeof(v3));
	  Com_Printf(CON_CHANNEL_DONT_FILTER, "\"%s\" is: \"%s^7\" ", cvar->name, Cvar_ValueToString(cvar, v3));
	  
	  memcpy(&v3, cvar->resetVec4, sizeof(v3));
      Com_Printf(CON_CHANNEL_DONT_FILTER, "default: \"%s^7\"\n", Cvar_ValueToString(cvar, v3));
      
	  if ( Cvar_HasLatchedValue(cvar) )
    {
			memcpy(&v3, cvar->latchedVec4, sizeof(v3));
			Com_Printf(CON_CHANNEL_DONT_FILTER, "latched: \"%s\"\n", Cvar_ValueToString(cvar, v3));
    }
	  
	  domain.imin = cvar->imin;
	  domain.imax = cvar->imax;
	  Cvar_DomainToString(cvar->type, domainString, domain);
    Com_Printf(CON_CHANNEL_DONT_FILTER, "  %s\n", domainString);
  }
  else
  {
		Cmd_Argsv( 1, cmdargs, sizeof(cmdargs) );
		Cvar_SetCommand(Cmd_Argv(0), cmdargs);
  }
	return qtrue;
}

#define FILE_HASH_SIZE 256

/*
================
return a hash value for the filename
================
*/
static long generateHashValue( const char *fname ) {
	int		i;
	long	hash;
	char	letter;

	hash = 0;
	i = 0;
	while (fname[i] != '\0') {
		letter = tolower(fname[i]);
		hash+=(long)(letter)*(i+119);
		i++;
	}
	hash &= (FILE_HASH_SIZE-1);
	return hash;
}


static cvar_t* Cvar_RegisterNew(const char *cvar_name, byte cvarType, int varFlags, CvarValue_t value, CvarLimits_t domain, const char *description)
{
  cvar_t *cvar;
  unsigned int hIndex;

  Sys_LockCriticalSection_v1(&cvarCriticalSectionObject);
  if ( numCvars >= 4096 )
  {
	Sys_UnlockCriticalSection_v1(&cvarCriticalSectionObject);
    Com_Error(0, "Can't create dvar '%s': %i dvars already exist", cvar_name, 4096);
  }

  cvar = &cvar_storage[numCvars];
  cvarList[numCvars] = cvar;
  numCvars = numCvars +1;
  areCvarsSorted = 0;
  cvar->type = cvarType;

  if ( varFlags & 0x4000 )
  {
    cvar->name = (char*)CopyString(cvar_name);
  }
  else
  {
    cvar->name = (char*)cvar_name;
  }

  switch ( cvar->type )
  {
    case CVAR_STRING:
      cvar->string = (char*)CopyString(value.string);
      cvar->latchedString = cvar->string;
      cvar->resetString = cvar->string;
      break;
	
    case CVAR_VEC2:
    case CVAR_VEC3:
    case CVAR_VEC4:
	  memcpy(cvar->vec4, value.vec4, sizeof(cvar->vec4));
	  memcpy(cvar->resetVec4, value.vec4, sizeof(cvar->resetVec4));
	  memcpy(cvar->latchedVec4, value.vec4, sizeof(cvar->latchedVec4));
      break;
    
	default:
	  cvar->integer = value.integer;
	  cvar->resetInteger = value.integer;
	  cvar->latchedInteger = value.integer;
      break;
  }
  cvar->imin = domain.imin;
  cvar->imax = domain.imax;
  cvar->description = (char*)description;
  cvar->modified = 0;
  cvar->domainFunc = NULL;
  cvar->flags = varFlags;
  hIndex = generateHashValue(cvar_name);
  cvar->hashNext = cvar_hashTable[hIndex];
  cvar_hashTable[hIndex] = cvar;
  
  Sys_UnlockCriticalSection_v1(&cvarCriticalSectionObject);
  return cvar;
}

static cvar_t *Cvar_Register(const char* var_name, cvarType_t type, unsigned short flags, CvarValue_t value, CvarLimits_t domain, const char *description)
{
	cvar_t* cvar;

	cvar = Cvar_FindMalleable(var_name);
	if(cvar){
		Cvar_Reregister( cvar, var_name, type, flags, value, domain, description);
	
	}else{
		cvar = Cvar_RegisterNew(var_name, type, flags, value, domain, description);
	}
	return cvar;
}


void Cvar_StringToColor(ucolor_t *outcolor, const char *colorstring)
{
  vec4_t color;
  byte intensity;
  int i;

  color[0] = 0.0;
  color[1] = 0.0;
  color[2] = 0.0;
  color[3] = 0.0;
  sscanf(colorstring, "%g %g %g %g", &color[0], &color[1], &color[2], &color[3]);

  
  for(i = 0; i < 4; ++i)
  {
	
	if(color[i] > 1.0)
	{
		color[i] = 1.0;
	}
	if(color[i] < 0.0)
	{
		color[i] = 0.0;
	}	
	
	intensity = (byte)((float)0xff * color[i]);
	
	switch(i)
	{
		case 0:
			outcolor->red = intensity;
			break;		
		
		case 1:
			outcolor->green = intensity;
			break;
		
		case 2:
			outcolor->blue = intensity;
			break;
		
		case 3:
			outcolor->alpha = intensity;
			break;
	}
  }
  
}



void Cvar_GetUnpackedColor(cvar_t *cvar, float *expandedColor)
{
  ucolor_t color;

  if ( cvar->type == 8 )
  {
    color = cvar->color;
  }
  else
  {
    Cvar_StringToColor(&color, cvar->string);
  }
  expandedColor[0] = (float)color.red * 0.0039215689;
  expandedColor[1] = (float)color.green * 0.0039215689;
  expandedColor[2] = (float)color.blue * 0.0039215689;
  expandedColor[3] = (float)color.alpha * 0.0039215689;
}


void Cvar_GetUnpackedColorByName(const char* name, float *expandedColor)
{
  cvar_t *var;
  
  var = Cvar_FindMalleable(name);
  if ( var )
  {
    Cvar_GetUnpackedColor(var, expandedColor);
  }
  else
  {
    expandedColor[0] = 1.0;
    expandedColor[1] = 1.0;
    expandedColor[2] = 1.0;
    expandedColor[3] = 1.0;
  }
}

void Cvar_AddFlags(cvar_t* var, unsigned short flags)
{
	var->flags |= flags;
}

void Cvar_ClearFlags(cvar_t* var, int flags)
{
	var->flags &= ~flags;
}

int Cvar_GetFlags (cvar_t *var)
{
	return var->flags;
}

typedef CvarValue_t DvarValue;
typedef cvar_t cvar_s;
typedef CvarLimits_t DvarLimits;

bool Dvar_VectorInDomain(const float *vector, int components, float min, float max)
{
  int channel;

  for ( channel = 0; channel < components; ++channel )
  {
    if ( min > vector[channel] )
    {
      return false;
    }
    if ( vector[channel] > max )
    {
      return false;
    }
  }
  return true;
}

bool Dvar_ValueInDomain(cvarType_t type, DvarValue value, DvarLimits domain)
{

  switch ( type )
  {
    case CVAR_BOOL:
	  if(value.boolean == true || value.boolean == false)
	  {
		  return true;
	  }
      return false;
      break;
    case CVAR_FLOAT:
      if ( domain.fmin <= value.floatval )
      {
        return value.floatval <= domain.fmax;
      }
	  return false;
    case CVAR_VEC2:
      return Dvar_VectorInDomain(value.vec2, 2, domain.fmin, domain.fmax);
    case CVAR_VEC3:
      return Dvar_VectorInDomain(value.vec3, 3, domain.fmin, domain.fmax);
    case CVAR_VEC4:
      return Dvar_VectorInDomain(value.vec4, 4, domain.fmin, domain.fmax);
    case CVAR_INT:
	  assert(domain.imin <= domain.imax);
      if ( value.integer >= domain.imin )
      {
        return value.integer <= domain.imax;
      }
	  return false;
      break;
    case CVAR_ENUM:
      if((value.integer >= 0 && value.integer < domain.enumCount) || value.integer == 0)
	  {
		  return true;
	  }
	  return false;
    case CVAR_STRING:
    case CVAR_COLOR:
      return true;
    default:
	  break;
  }
  return false;
}

void Dvar_PrintDomain(cvarType_t type, DvarLimits domain)
{
	char domainString[1024];
	Com_Printf(CON_CHANNEL_SYSTEM, "  %s\n", Cvar_DomainToString(type, domainString, domain));
}

enum DvarSetSource
{
	DVAR_SOURCE_INTERNAL,
	DVAR_SOURCE_EXTERNAL,
	DVAR_SOURCE_SCRIPT,
	DVAR_SOURCE_DEVGUI
};

bool Dvar_CanChangeValue(cvar_s *dvar, DvarValue value, enum DvarSetSource source)
{
	char *reason;

	assert(dvar);

	if ( !dvar )
	{
		return false;
	}

	DvarValue currentValue;

	memcpy(currentValue.vec4, dvar->vec4, sizeof(currentValue.vec4));

	if ( Cvar_ValuesEqual(dvar->type, value, currentValue) )
	{
		return true;
	}
	reason = 0;
	if ( dvar->flags & CVAR_ROM )
	{
		reason = va("%s is read only.\n", dvar->name);
	}
	else if ( dvar->flags & CVAR_INIT )
	{
		reason = va("%s is write protected.\n", dvar->name);
	}
	else if ( dvar->flags & CVAR_CHEAT )
	{
		if ( !cvar_cheats->boolean )
		{
			if ( source == DVAR_SOURCE_EXTERNAL || source == DVAR_SOURCE_SCRIPT )
			{
				reason = va("%s is cheat protected.\n", dvar->name);
			}
		}
	}
	if ( !reason )
	{
		return true;
	}
	Com_Printf(CON_CHANNEL_ERROR, reason);
	return false;
}


  
bool Dvar_ShouldFreeCurrentString(cvar_t *var)
{
  return var->string && var->string != var->latchedString && var->string != var->resetString;
}

bool Dvar_ShouldFreeLatchedString(cvar_t *var)
{
  return var->latchedString && var->latchedString != var->string && var->latchedString != var->resetString;
}

void Dvar_WeakCopyString(const char *string, DvarValue *value)
{
	assert(string);
  value->string = string;
}

void Dvar_CopyString(const char *string, DvarValue *value)
{
	assert(string);
  	value->string = CopyString(string);
}

void Dvar_AssignCurrentStringValue(cvar_s *dvar, DvarValue *dest, const char *string)
{
	assert(string);

	if ( dvar->latchedString && (string == dvar->latchedString || !strcmp(string, dvar->latchedString)) )
	{
		Dvar_WeakCopyString(dvar->latchedString, dest);
	}
	else if ( dvar->resetString && (string == dvar->resetString || !strcmp(string, dvar->resetString)) )
	{
		Dvar_WeakCopyString(dvar->resetString, dest);
	}
	else
	{
		Dvar_CopyString(string, dest);
	}
}

void Dvar_AssignLatchedStringValue(cvar_s *dvar, DvarValue *dest, const char *string)
{
	assert(string);

	if ( dvar->string && (string == dvar->string || !strcmp(string, dvar->string)) )
	{
		Dvar_WeakCopyString(dvar->string, dest);
	}
	else if ( dvar->resetString && (string == dvar->resetString || !strcmp(string, dvar->resetString)) )
	{
		Dvar_WeakCopyString(dvar->resetString, dest);
	}
	else
	{
		Dvar_CopyString(string, dest);
	}
}

void Dvar_FreeString(const char **dvarString)
{
 	FreeString(*dvarString);
	*dvarString = NULL;
}


void Dvar_UpdateValue(cvar_t *var, CvarValue_t value)
{
	const char* oldString;
	
	if( var->type == CVAR_STRING)
	{
		assert(value.string != var->string || value.string == var->latchedString || value.string == var->resetString);
		
		bool shouldFreeString = Dvar_ShouldFreeCurrentString(var);
		if ( shouldFreeString )
		{
			oldString = var->string;
		}
		
		DvarValue currentString;

		Dvar_AssignCurrentStringValue(var, &currentString, value.string);
		var->string = currentString.string;
		
		if ( Dvar_ShouldFreeLatchedString(var) )
		{
			Dvar_FreeString(&var->latchedString);
		}
		var->latchedString = NULL;

		DvarValue newLatch;

		Dvar_WeakCopyString(var->string, &newLatch);
		var->latchedString = newLatch.string;

		if ( shouldFreeString )
		{
			Dvar_FreeString(&oldString);
		}

	}else{
		memcpy(var->vec4, value.vec4, sizeof(var->vec4));
		memcpy(var->latchedVec4, value.vec4, sizeof(var->latchedVec4));
	}

}


void Dvar_SetLatchedValue(cvar_s *dvar, DvarValue value)
{
  DvarValue latchedString;
  DvarValue oldString;
  bool shouldFree;


	if( dvar->type == CVAR_STRING)
	{
      if ( dvar->latchedString != value.string )
      {
        shouldFree = Dvar_ShouldFreeLatchedString(dvar);
        if ( shouldFree )
        {
          oldString.string = dvar->latchedString;
        }
        Dvar_AssignLatchedStringValue(dvar, &latchedString, value.string);
        dvar->latchedString = latchedString.string;
        if ( shouldFree )
        {
          Dvar_FreeString(&oldString.string);
        }
      }
	}else{
		memcpy(dvar->latchedVec4, value.vec4, sizeof(dvar->latchedVec4));
	}
}


void REGPARM(1) Dvar_SetVariant(cvar_s *dvar, DvarValue value, enum DvarSetSource source)
{
	DvarLimits domain;
	char string[1024];
	DvarValue curVal;

	assert(dvar);
	assert(dvar->name);

	if(!dvar || !dvar->name || !dvar->name[0]){
		return;
	}

	if ( Com_LogFileOpen() )
	{
		DvarValue CurrentValue;
		DvarValue NewValue;

		memcpy(CurrentValue.vec4, dvar->vec4, sizeof(CurrentValue.vec4));
		memcpy(NewValue.vec4, value.vec4, sizeof(NewValue.vec4));

		if ( !Cvar_ValuesEqual(dvar->type, CurrentValue, NewValue) )
		{
			Com_sprintf(string, 1024, "      dvar set %s %s\n", dvar->name, Cvar_ValueToString(dvar, NewValue));
			Com_PrintMessage(CON_CHANNEL_LOGFILEONLY, string, 0);
		}
	}

	domain.imin = dvar->imin;
	domain.enumStrings = dvar->enumStrings;
	
	if ( !Dvar_ValueInDomain(dvar->type, value, domain) )
	{
		Com_Printf(CON_CHANNEL_ERROR, "'%s' is not a valid value for dvar '%s'\n", Cvar_ValueToString(dvar, value), dvar->name);
		Dvar_PrintDomain(dvar->type, domain);
		if ( dvar->type == CVAR_ENUM )
		{
			DvarValue resetVal;
			memcpy(resetVal.vec4, dvar->resetVec4, sizeof(resetVal.vec4));
			Dvar_SetVariant(dvar, resetVal, source);
		}
		return;
	}


	if ( source == DVAR_SOURCE_EXTERNAL || source == DVAR_SOURCE_SCRIPT )
	{
		if ( !Dvar_CanChangeValue(dvar, value, source) )
		{
			return;
		}
		if ( dvar->flags & CVAR_LATCH )
		{
			Dvar_SetLatchedValue(dvar, value);

			if ( Cvar_HasLatchedValue(dvar))
			{
			  Com_Printf(CON_CHANNEL_SYSTEM, "%s will be changed upon restarting.\n", dvar->name);
			  cvar_latchedSet = true; 
			}
			return;
		}
	}

	memcpy(curVal.vec4, dvar->vec4, sizeof(curVal.vec4));
    if ( Cvar_ValuesEqual(dvar->type, curVal, value) )
    {
		Dvar_SetLatchedValue(dvar, curVal);
		return;
    }
	
	cvar_modifiedFlags |= dvar->flags;
    
	Dvar_UpdateValue(dvar, value);

	dvar->modified = 1;

}


cvar_t* Cvar_FindVar(const char *dvarName)
{
//  assert( dvarName );

  if ( dvarName && *dvarName )
  {
    return Cvar_FindMalleable(dvarName);
  }
  return NULL;
}

cvar_t* REGPARM(1) Cvar_SetFromStringByNameExternal(const char* varname, const char* valueString)
{
	return Cvar_SetFromStringByNameFromSource(varname, valueString, DVAR_SOURCE_EXTERNAL);
}
