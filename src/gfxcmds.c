
#include "gfxshared.h"
#include "xassets/font.h"
#include "r_shared.h"
#include "qcommon.h"
#include "sys_patch.h"
#include "xassets/material.h"
#include "xzone.h"
#include <stdbool.h>
#include <D3dx9core.h>
#define s_cmdList (*((GfxCmdArray**)(0xCC9F4A4)))
#define s_renderCmdBufferSize (*(int*)(0xCC9F49C))



#define R_ADDCMD(m_cmd, m_cmdid) \
	if ( s_cmdList->usedCritical - s_cmdList->usedTotal + s_renderCmdBufferSize - 0x2000 < sizeof(*m_cmd) ){	s_cmdList->lastCmd = 0; return;  } \
	m_cmd = (void*) &s_cmdList->cmds[s_cmdList->usedTotal]; s_cmdList->lastCmd = (void*) m_cmd; s_cmdList->usedTotal += sizeof(*m_cmd);	\
	m_cmd->header.id = m_cmdid; m_cmd->header.byteCount = sizeof(*m_cmd)

#define R_ADDDYNLENCMD(m_cmd, m_cmdid, m_cmddynlen) \
	if ( s_cmdList->usedCritical - s_cmdList->usedTotal + s_renderCmdBufferSize - 0x2000 < sizeof(*m_cmd) + m_cmddynlen){	s_cmdList->lastCmd = 0; return NULL;  } \
	m_cmd = (void*) &s_cmdList->cmds[s_cmdList->usedTotal]; s_cmdList->lastCmd = (void*) m_cmd; s_cmdList->usedTotal += (sizeof(*m_cmd) + m_cmddynlen);	\
	m_cmd->header.id = m_cmdid; m_cmd->header.byteCount = sizeof(*m_cmd) + m_cmddynlen

void RB_EndTessSurface();


/* 7655 */
typedef struct
{
  uint16_t id;
  uint16_t byteCount;
}GfxCmdHeader;

/* 7656 */
typedef struct
{
  uint8_t *cmds;
  int usedTotal;
  int usedCritical;
  GfxCmdHeader *lastCmd;
}GfxCmdArray;


uint8_t ByteFromFloatColor(float from)
{
	int intcolor = from * 0xff;
	if(intcolor < 0)
	{
		intcolor = 0;
	}else if(intcolor > 0xff)
	{
		intcolor = 0xff;
	}
	return intcolor;
}


void Byte4PackVertexColor(const float *from, uint8_t *to)
{
	to[2] = ByteFromFloatColor(from[0]);
	to[1] = ByteFromFloatColor(from[1]);
	to[0] = ByteFromFloatColor(from[2]);
	to[3] = ByteFromFloatColor(from[3]);
}

void R_ConvertColorToBytes(const float *colorFloat, GfxColor *color)
{
	if(colorFloat == NULL)
	{
		color->packed = 0xffffffff;
		return;
	}
	Byte4PackVertexColor(colorFloat, color->array);
}



static MaterialTechniqueSet *Material_GetTechniqueSet(Material *material)
{
    assert(material);
    assertx(material && material->techniqueSet, "material '%s' missing techset. If you are building fastfile, check Launcher for error messages.", material->info.name);
    return material ? material->techniqueSet : 0;
}



static MaterialTechnique *Material_GetTechnique(Material *material, byte techType)
{
    MaterialTechniqueSet *techSet = Material_GetTechniqueSet(material);
    assertx(techSet, "material '%s' missing techset. %d tech %d", material ? material->info.name : "<undefined>", material ? material->techniqueSet != 0 : -1, techType);
    return techSet ? techSet->techniques[techType] : 0;
}


static bool Material_HasAnyFogableTechnique(Material *material)
{
    return Material_GetTechnique(material, 7) || Material_GetTechnique(material, 5);
}



/**************************************************************************
GfxCmdDrawText2D
***************************************************************************/
/* 8448 */
typedef struct //Dynamic sized structure !!
{
  GfxCmdHeader header;
  float x;
  float y;
//  float w;
  float rotation;
  Font_t *font;
  float xScale;
  float yScale;
  GfxColor color;
  int maxChars;
  int renderFlags;
  int cursorPos;
  char cursorLetter;
  uint8_t pad[3];
  GfxColor glowForceColor;
  int fxBirthTime;
  int fxLetterTime;
  int fxDecayStartTime;
  int fxDecayDuration;
//  int fxRedactDecayStartTime;
//  int fxRedactDecayDuration;
  Material *fxMaterial;
  Material *fxMaterialGlow;
  float padding;
  char text[4]; //Dynamic sized field
}GfxCmdDrawText2D;



qboolean SetDrawText2DGlowParms(GfxCmdDrawText2D *cmd, const float *color, const float *glowColor)
{
  vec4_t c;

  if ( glowColor && 0.0 != glowColor[3] )
  {
    cmd->renderFlags |= 0x30u;
    c[0] = glowColor[0] * 0.1000000014901161;
    c[1] = glowColor[1] * 0.1000000014901161;
    c[2] = glowColor[2] * 0.1000000014901161;
    c[3] = glowColor[3] * color[3];
    R_ConvertColorToBytes(c, &cmd->glowForceColor);
    return qtrue;
  }
  return qfalse;
}






GfxCmdDrawText2D* R_AddCmdDrawTextWithCursor(const char *text, int maxChars, Font_t *font, float x, float y, float xScale, float yScale, float rotation, const float *color, int style, signed int cursorPos, char cursor)
{
  unsigned int textlen;
  unsigned int extralen;
  GfxCmdDrawText2D *cmd;


  if ( !*text && cursorPos < 0 )
  {
    return NULL;
  }
  textlen = strlen(text);
  extralen = (textlen +1) & 0xFFFFFFFC;

  R_ADDDYNLENCMD(cmd, 13, extralen);

  cmd->x = x;
  cmd->y = y;
#if 0
  cmd->w = 1.0; //old cod4 does not have this
  cmd->fxRedactDecayStartTime = 0;
  cmd->fxRedactDecayDuration = 0;
#endif
  cmd->rotation = rotation;
  cmd->font = font;
  cmd->xScale = xScale;
  cmd->yScale = yScale;
  R_ConvertColorToBytes(color, &cmd->color);
  cmd->maxChars = maxChars;
  cmd->renderFlags = 0;
  switch ( style )
  {
    case 3:
      cmd->renderFlags = 4;
      break;
    case 6:
      cmd->renderFlags = 12;
      break;
    case 128:
      cmd->renderFlags = 1;
      break;
  }
  if ( cursorPos >= 0 )
  {
    cmd->renderFlags |= 2u;
    cmd->cursorPos = cursorPos;
    cmd->cursorLetter = cursor;
  }
  memcpy(cmd->text, text, textlen);
  cmd->text[textlen] = 0;
  return cmd;
}

void R_AddCmdDrawText(const char *text, int maxChars, Font_t *font, float x, float y, float xScale, float yScale, float rotation, const float *color, int style)
{
 	R_AddCmdDrawTextWithCursor(text, maxChars, font, x, y, xScale, yScale, rotation, color, style, -1, 0);
}



void R_AddCmdDrawTextWithEffects(const char *text, int maxChars, Font_t *font, float x, float y, float xScale, float yScale, float rotation, const float *color, int style, const float *glowColor, Material *fxMaterial, Material *fxMaterialGlow, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration)
{
  Material *mat;
  GfxCmdDrawText2D *cmd;


  if ( fxMaterial )
  {
    mat = fxMaterial;
  }else{
    mat = rgp.defaultMaterial;
  }

  if ( Material_HasAnyFogableTechnique(mat) )
  {
    if ( !Material_IsDefault(mat) )
    {
      R_WarnOncePerFrame(R_WARN_FOGABLE_MATERIAL_2D_GLYPH, mat->info.name);
      return;
    }
    if ( !fxMaterialGlow )
	{
		mat = rgp.defaultMaterial;
	}else{
		mat = fxMaterialGlow;
	}
  }
  else if ( !fxMaterialGlow )
  {
    mat = rgp.defaultMaterial;
  }else{
	mat = fxMaterialGlow;
  }

  if ( Material_HasAnyFogableTechnique(mat) && !Material_IsDefault(mat) )
  {
    R_WarnOncePerFrame(R_WARN_FOGABLE_MATERIAL_2D_GLYPH, mat->info.name);
    return;
  }
  cmd = R_AddCmdDrawTextWithCursor(text, maxChars, (Font_t *)font, x, y, xScale, yScale, rotation, color, style, -1, 0);

  if ( !cmd )
  {
	  return;
  }

  SetDrawText2DGlowParms(cmd, color, glowColor);

  if ( fxMaterial && fxMaterialGlow )
  {
      if ( fxBirthTime )
      {
        cmd->renderFlags |= 0xC0u;
        cmd->fxMaterial = fxMaterial;
        cmd->fxMaterialGlow = fxMaterialGlow;
        cmd->fxBirthTime = fxBirthTime;
        cmd->fxLetterTime = fxLetterTime;
        cmd->fxDecayStartTime = fxDecayStartTime;
        cmd->fxDecayDuration = fxDecayDuration;
        cmd->padding = 0.0;
      }
  }

}






/**************************************************************************
GfxCmdProjectionSet
***************************************************************************/
typedef enum
{
  GFX_PROJECTION_2D = 0x0,
  GFX_PROJECTION_3D = 0x1,
}GfxProjectionTypes;


/* 8466 */
typedef struct
{
  GfxCmdHeader header;
  GfxProjectionTypes projection;
}GfxCmdProjectionSet;

void R_AddCmdProjectionSet2D(void)
{
	GfxCmdProjectionSet *cmd;

	R_ADDCMD(cmd, 20);

	cmd->projection = 0;
}

/**************************************************************************
GfxCmdSaveScreen
***************************************************************************/

/* 8450 */
typedef struct
{
  GfxCmdHeader header;
  int screenTimerId;
}GfxCmdSaveScreen;


/**************************************************************************
GfxCmdSaveScreenSection
***************************************************************************/

typedef struct
{
  GfxCmdHeader header;
  float s0;
  float t0;
  float ds;
  float dt;
  int screenTimerId;
}GfxCmdSaveScreenSection;

/**************************************************************************
GfxCmdStretchPicRotateXY
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  Material *material;
  float x;
  float y;
  float w;
  float h;
  float s0;
  float t0;
  float s1;
  float t1;
  GfxColor color;
  float rotation;
}GfxCmdStretchPicRotateXY;

/**************************************************************************
GfxCmdSetViewport
***************************************************************************/

typedef struct
{
  GfxCmdHeader header;
  GfxViewport viewport;
}GfxCmdSetViewport;

/**************************************************************************
GfxCmdDrawText3D
***************************************************************************/

typedef struct
{
  GfxCmdHeader header;
  float org[3];
  Font_t *font;
  float xPixelStep[3];
  float yPixelStep[3];
  GfxColor color;
  char text[4];
}GfxCmdDrawText3D;

/**************************************************************************
GfxCmdStretchRaw
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  int x;
  int y;
  int w;
  int h;
  int cols;
  int rows;
  const char *data;
  int client;
  int dirty;
}GfxCmdStretchRaw;

/**************************************************************************
GfxCmdDrawQuadPic
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  Material *material;
  float verts[4][2];
  GfxColor color;
}GfxCmdDrawQuadPic;

/**************************************************************************
GfxCmdClearScreen
***************************************************************************/

typedef struct
{
  GfxCmdHeader header;
  char whichToClear;
  char stencil;
  uint16_t pad;
  float depth;
  float color[4];
}GfxCmdClearScreen;

void R_AddCmdClearScreen(const float* color, float depth, uint8_t stencil)
{
  GfxCmdClearScreen *cmd;

  R_ADDCMD(cmd, 4);

  cmd->whichToClear = 1;
  cmd->depth = depth;
  cmd->stencil = stencil;
  cmd->color[0] = color[0];
  cmd->color[1] = color[1];
  cmd->color[2] = color[2];
  cmd->color[3] = color[3];
}


/**************************************************************************
GfxCmdDrawPoints
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  int16_t pointCount;
  char size;
  char dimensions;
  GfxPointVertex verts[1];
}GfxCmdDrawPoints;

/**************************************************************************
GfxCmdSetMaterialColor
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  float color[4];
}GfxCmdSetMaterialColor;


/**************************************************************************
GfxCmdDrawTriangles
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  Material *material;
  MaterialTechniqueType techType;
  int16_t indexCount;
  int16_t vertexCount;
}GfxCmdDrawTriangles;

/**************************************************************************
GfxCmdDrawLines
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  int16_t lineCount;
  char width;
  char dimensions;
  GfxPointVertex verts[2];
}GfxCmdDrawLines;

/**************************************************************************
GfxCmdStretchPic
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  Material *material;
  float x;
  float y;
  float w;
  float h;
  float s0;
  float t0;
  float s1;
  float t1;
  GfxColor color;
}GfxCmdStretchPic;

void R_AddCmdDrawStretchPic(
    float x, float y, float w, float h,
    float s0, float t0, float s1, float t1,
    const float *color, Material *material
)
{
    if (!material)
        material = rgp.defaultMaterial;

    if (Material_HasAnyFogableTechnique(material) && !Material_IsDefault(material))
    {
        Com_PrintWarning(CON_CHANNEL_GFX, "R_AddCmdDrawStretchPic: NOT DRAWING WITH MATERIAL \"%s\", because it has a fogable technique.\n", Material_GetName(material));
        material = rgp.defaultMaterial;
    }
    else if (material->stateFlags & 0x10)
    {
        Com_PrintWarning(CON_CHANNEL_GFX, "R_AddCmdDrawStretchPic: NOT DRAWING WITH MATERIAL \"%s\", because it uses the depth buffer. Set materialType to 2d.\n", Material_GetName(material));
        material = rgp.defaultMaterial;
    }

    GfxCmdStretchPic *cmd;
    R_ADDCMD(cmd, 6);

    cmd->material = material;
    cmd->x = x;
    cmd->y = y;
    cmd->w = w;
    cmd->h = h;
    cmd->s0 = s0;
    cmd->t0 = t0;
    cmd->s1 = s1;
    cmd->t1 = t1;
    R_ConvertColorToBytes(color, &cmd->color);
}

/**************************************************************************
GfxCmdStretchPicRotateST
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  Material *material;
  float x;
  float y;
  float w;
  float h;
  float centerS;
  float centerT;
  float radiusST;
  float scaleFinalS;
  float scaleFinalT;
  GfxColor color;
  float rotation;
}GfxCmdStretchPicRotateST;

/**************************************************************************
GfxCmdBlendSavedScreenFlashed
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  float intensityWhiteout;
  float intensityScreengrab;
  float s0;
  float t0;
  float ds;
  float dt;
}GfxCmdBlendSavedScreenFlashed;

/**************************************************************************
GfxCmdBlendSavedScreenBlurred
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  int fadeMsec;
  float s0;
  float t0;
  float ds;
  float dt;
  int screenTimerId;
}GfxCmdBlendSavedScreenBlurred;

/**************************************************************************
GfxCmdDrawFullScreenColoredQuad
***************************************************************************/
typedef struct
{
  GfxCmdHeader header;
  Material *material;
  float s0;
  float t0;
  float s1;
  float t1;
  GfxColor color;
}GfxCmdDrawFullScreenColoredQuad;


/*
typedef struct //Dynamic sized structure !!
{
  GfxCmdHeader header;
  float x;
  float y;
  float rotation;
  Font_t *font;
  float xScale;
  float yScale;
  GfxColor color;
  int maxChars;
  int renderFlags;
  int cursorPos;
  char cursorLetter;
  uint8_t pad[3];
  GfxColor glowForceColor;
  int fxBirthTime;
  int fxLetterTime;
  int fxDecayStartTime;
  int fxDecayDuration;
  Material *fxMaterial;
  Material *fxMaterialGlow;
  float padding;
  char text[4]; //Dynamic sized field
}GfxCmdDrawTTFText2D;




void* R_AddCmdDrawTTFText(const char *text, int maxChars, Font_t *font, float x, float y, float xScale, float yScale, float rotation, const float *color, int style)
{
	GfxCmdDrawTTFText2D* cmd;
	unsigned int textlen;
  unsigned int extralen;

  if ( !*text )
  {
    return NULL;
  }
  textlen = strlen(text);
	R_LoadGlyphs(font, text, textlen); //Cache all glyphs unless they are already

  extralen = (textlen +1) & 0xFFFFFFFC;
	R_ADDDYNLENCMD(cmd, 21, extralen);

	cmd->x = x;
  cmd->y = y;
  cmd->rotation = rotation;
  cmd->font = font;
  cmd->xScale = xScale;
  cmd->yScale = yScale;
  R_ConvertColorToBytes(color, &cmd->color);
  cmd->maxChars = maxChars;
  cmd->renderFlags = 0;
  cmd->cursorPos = 0;
  cmd->cursorLetter = 0;

	memcpy(cmd->text, text, textlen);
  cmd->text[textlen] = 0;

	return NULL;

}*/


void Material_Set2DWithTexture(Material* material, const char* name, IDirect3DTexture9* texture, int height, int width, int depth);
Material* Material_Create2DWithTexture(const char* name, IDirect3DTexture9* texture, int height, int width, int depth);
void __cdecl RB_DrawStretchPic(Material *material, float x, float y, float w, float h, float s0, float t0, float s1, float t1, unsigned int color/*, GfxPrimStatsTarget statsTarget*/);


#if 0
void RB_DrawTTFTextCmd(GfxCmdHeader **pheader)
{
	GfxCmdDrawTTFText2D *cmd = (GfxCmdDrawTTFText2D*)*pheader;
	static Material* drawmat;

	if ( tess.indexCount )
	{
		RB_EndTessSurface();
	}
	Font_t* font = cmd->font;
	if(font == NULL || font->glyphCount != -1337)
	{
		font = Font_GetDefaultFont();
		if(font == NULL)
		{
			byte** rawstream = (byte**)pheader;
			*rawstream += cmd->header.byteCount;
			return;
		}
	}


	RECT rct; //Font
	POINT pt;
	int i;

	D3DCOLOR fontColor = D3DCOLOR_RGBA(cmd->color.array[2], cmd->color.array[1], cmd->color.array[0], cmd->color.array[3]);

	int textlen = strlen(cmd->text);

	float next = 0;

	for(i = 0; i < textlen; ++i)
	{
		if(cmd->text[i] == ' '){
			next += 8; //Whitespace? Shift the draw x-baseline
			continue;
		}

		const GlyphInfo_t* glyph = Glyph_Find( font, cmd->text[i] );

		if(glyph == NULL)
		{
			continue;
		}

		rct = glyph->rect;
		pt = glyph->pt;

//		D3DXSaveTextureToFileA("glyph.dds", D3DXIFF_DDS, (IDirect3DBaseTexture9*)texture, 0);


		const float charWidth = rct.right - rct.left;
		const float charHeight = rct.bottom - rct.top;

/*		char buf[256];
		Com_sprintf(buf, sizeof(buf), "Top: %d Bottom: %d Height: %d PointX: %d PointY: %d Scale: %g Char: %c\r\n", rct.top, rct.bottom, desc.Height, pt.x, pt.y, cmd->yScale, c);
		FILE* fh = fopen("debug.txt", "ab");
		if(fh)
		{
			fwrite(buf, strlen(buf), 1, fh);
		}
		fclose(fh);
*/

		const float startX = cmd->x + next + pt.x * cmd->xScale;
		next += ((charWidth + pt.x) * cmd->xScale);
		const float startY = cmd->y + (pt.y - font->pixelHeight) * cmd->yScale;

		float u = (rct.left + 0.5f) / glyph->glyphWidth;
		float v = (rct.top + 0.5f) / glyph->glyphHeight;

		float u2 = u + (charWidth / glyph->glyphWidth);
		float v2 = v + (charHeight / glyph->glyphHeight);

		struct CUSTOMVERTEX
		{
		    float x, y, z;
				float rhw;
				DWORD color;
				float tu, tv;
		};
		// TRIANGLEFAN coords:
		//    v1-----v2  clockwise winding order
		//    |    / |
		//    |  /   |
		//    v0-----v3
		//
		const struct CUSTOMVERTEX char_quad[4] =
		{
				// Top left vertex
		    {
		        startX, startY, 0.0f, 1.0f,
						fontColor,
		        u, v
		    },
				// Bottom left vertex
		    {
		        startX, startY + charHeight * cmd->yScale, 0.0f, 1.0f,
						fontColor,
						u, v2
		    },
				// Bottom right vertex
		    {
		        startX + charWidth * cmd->xScale, startY + charHeight * cmd->yScale, 0.0f, 1.0f,
						fontColor,
						u2, v2
		    },
		    // Top right vertex
		    {
		        startX + charWidth * cmd->xScale, startY, 0.0f, 1.0f,
						fontColor,
						u2, v
		    },
		};
#if 0
    if(drawmat == NULL)
    {
      drawmat = Material_Create2DWithTexture("testmat", (IDirect3DTexture9*)glyph->glyph, charHeight, charWidth, 2);
    }else{
      Material_Set2DWithTexture(drawmat, "testmat", glyph->glyph, charHeight, charWidth, 2);
    }
    RB_DrawStretchPic(drawmat, startX, startY, charWidth, charHeight, 0, 1, 1, 0, cmd->color.packed/*, GfxPrimStatsTarget statsTarget*/);
#endif



  	if ( tess.indexCount )
    {
      RB_EndTessSurface();
    }





    if(r_dx.device->lpVtbl->SetTexture(r_dx.device, 0, (IDirect3DBaseTexture9*)glyph->glyph) != D3D_OK)
		{
			MessageBoxA(NULL, "Error SetTexture", "Error SetTexture", MB_OK);
			byte** rawstream = (byte**)pheader;
			*rawstream += cmd->header.byteCount;
			return;
		}

		if(r_dx.device->lpVtbl->DrawPrimitiveUP(r_dx.device, D3DPT_TRIANGLEFAN, 2, char_quad, sizeof(struct CUSTOMVERTEX)) != D3D_OK)
		{
			break;
		}

	}


	byte** rawstream = (byte**)pheader;
	*rawstream += cmd->header.byteCount;

}
#endif

#if 0
void __cdecl RB_DrawText2DCmd(GfxCmdHeader **pheader)
{
  float p;
  float cosAngle;
  float sinAngle;

	GfxCmdDrawText2D *cmd = (GfxCmdDrawText2D*)*pheader;

  p = cmd->rotation * 0.017453292;
  cosAngle = cos(p);
  sinAngle = sin(p);

//  PIXBeginNamedEvent(-1, cmd->text);

  DrawText2D(
    cmd->text,
    cmd->x,
    cmd->y,
    cmd->w,
    cmd->font,
    cmd->xScale,
    cmd->yScale,
    sinAngle,
    cosAngle,
    cmd->color,
    cmd->maxChars,
    cmd->renderFlags,
    cmd->cursorPos,
    cmd->cursorLetter,
    cmd->padding,
    cmd->glowForceColor,
    cmd->fxBirthTime,
    cmd->fxLetterTime,
    cmd->fxDecayStartTime,
    cmd->fxDecayDuration,
    cmd->fxRedactDecayStartTime,
    cmd->fxRedactDecayDuration,
    cmd->fxMaterial,
    cmd->fxMaterialGlow);

  /*
  if ( GetCurrentThreadId() == g_DXDeviceThread.owner && 0 == g_DXDeviceThread.aquired )
  {
    D3DPERF_EndEvent();
  }
  */
	byte** rawstream = (byte**)pheader;
	*rawstream += cmd->header.byteCount;
}
#endif


void __cdecl RB_DrawText2DCmd(GfxCmdHeader **pheader)
{
  float p;
  float cosAngle;
  float sinAngle;

	GfxCmdDrawText2D *cmd = (GfxCmdDrawText2D*)*pheader;

  p = cmd->rotation * 0.017453292;
  cosAngle = cos(p);
  sinAngle = sin(p);

//  PIXBeginNamedEvent(-1, cmd->text);

  DrawText2D(
    cmd->text,
    cmd->x,
    cmd->y,
    1.0,
    cmd->font,
    cmd->xScale,
    cmd->yScale,
    sinAngle,
    cosAngle,
    cmd->color,
    cmd->maxChars,
    cmd->renderFlags,
    cmd->cursorPos,
    cmd->cursorLetter,
    cmd->padding,
    cmd->glowForceColor,
    cmd->fxBirthTime,
    cmd->fxLetterTime,
    cmd->fxDecayStartTime,
    cmd->fxDecayDuration,
    0,
    0,
    cmd->fxMaterial,
    cmd->fxMaterialGlow);

  /*
  if ( GetCurrentThreadId() == g_DXDeviceThread.owner && 0 == g_DXDeviceThread.aquired )
  {
    D3DPERF_EndEvent();
  }
  */
	byte** rawstream = (byte**)pheader;
	*rawstream += cmd->header.byteCount;
}







void __cdecl RB_TessOverflow()
{
//  GfxPrimStats *primStats;
//  primStats = gfxCmdBufState.prim.primStats;
  RB_EndTessSurface();
  RB_BeginSurface(gfxCmdBufState.origMaterial, gfxCmdBufState.origTechType);
//  gfxCmdBufState.prim.primStats = primStats;
}



void __cdecl RB_CheckTessOverflow(int vertexCount, int indexCount)
{
  assert(vertexCount <= 5450);
  assert(indexCount <= ((2 * 1024 * 1024) / 2));

  if ( vertexCount + tess.vertexCount > 5450 || indexCount + tess.indexCount > 0x100000 )
  {
    RB_TessOverflow();
  }
}

void __cdecl RB_SetTessTechnique(Material *material, char techType)
{

  assert(material);

  if ( gfxCmdBufState.origMaterial != material || gfxCmdBufState.origTechType != techType )
  {
    if ( tess.indexCount )
    {
      RB_EndTessSurface();
    }
    RB_BeginSurface(material, techType);
  }
}

void __cdecl R_SetVertex2d(GfxVertex *vert, float x, float y, float s, float t, unsigned int color)
{
  vert->xyzw[0] = x;
  vert->xyzw[1] = y;
  vert->xyzw[2] = 0.0;
  vert->xyzw[3] = 1.0;
  vert->normal.packed = 0x3FFE7F7F;
  vert->color.packed = color;
  vert->texCoord[0] = s;
  vert->texCoord[1] = t;
}

void __cdecl R_SetVertex4d(GfxVertex *vert, float x, float y, float z, float w, float s, float t, unsigned int color)
{
  vert->xyzw[0] = x;
  vert->xyzw[1] = y;
  vert->xyzw[2] = z;
  vert->xyzw[3] = w;
  vert->normal.packed = 0x3FFE7F7F;
  vert->color.packed = color;
  vert->texCoord[0] = s;
  vert->texCoord[1] = t;
}

void __cdecl RB_DrawStretchPic(Material *material, float x, float y, float w, float h, float s0, float t0, float s1, float t1, unsigned int color/*, GfxPrimStatsTarget statsTarget*/)
{
  unsigned __int16 vertCount;

  assert(gfxCmdBufSourceState.viewMode == VIEW_MODE_2D);

  RB_SetTessTechnique(material, 4);
  //R_TrackPrims(&state, statsTarget);
  RB_CheckTessOverflow(4, 6);
  vertCount = tess.vertexCount;
  tess.indices[tess.indexCount] = vertCount + 3;
  tess.indices[tess.indexCount + 1] = vertCount;
  tess.indices[tess.indexCount + 2] = vertCount + 2;
  tess.indices[tess.indexCount + 3] = vertCount + 2;
  tess.indices[tess.indexCount + 4] = vertCount;
  tess.indices[tess.indexCount + 5] = vertCount + 1;
  R_SetVertex2d(&tess.verts[tess.vertexCount], x, y, s0, t0, color);
  R_SetVertex2d(&tess.verts[tess.vertexCount + 1], x + w, y, s1, t0, color);
  R_SetVertex2d(&tess.verts[tess.vertexCount + 2], x + w, y + h, s1, t1, color);
  R_SetVertex2d(&tess.verts[tess.vertexCount + 3], x, y + h, s0, t1, color);
  tess.vertexCount += 4;
  tess.indexCount += 6;
}


void __cdecl RB_DrawStretchPicRotate_New(Material *material, float x, float y, float w, float width, float height, float s0, float t0, float s1, float t1, float sinAngle, float cosAngle, unsigned int color /*, GfxPrimStatsTarget statsTarget*/)
{
  unsigned __int16 vertCount;
  int indexCount;
  float stepY;

  assert(gfxCmdBufSourceState.viewMode == VIEW_MODE_2D);

  RB_SetTessTechnique(material, 4);
  //R_TrackPrims(&gfxCmdBufState.prim, statsTarget);
  RB_CheckTessOverflow(4, 6);
  vertCount = tess.vertexCount;
  indexCount = tess.indexCount;
  tess.vertexCount += 4;
  tess.indexCount += 6;
  tess.indices[indexCount] = vertCount + 3;
  tess.indices[indexCount + 1] = vertCount;
  tess.indices[indexCount + 2] = vertCount + 2;
  tess.indices[indexCount + 3] = vertCount + 2;
  tess.indices[indexCount + 4] = vertCount;
  tess.indices[indexCount + 5] = vertCount + 1;
  stepY = -height * sinAngle;
  R_SetVertex4d(&tess.verts[vertCount], x, y, 0.0, w, s0, t0, color);
  R_SetVertex4d(&tess.verts[vertCount + 1], x + (width * cosAngle), y + (width * sinAngle), 0.0, w, s1, t0, color);
  R_SetVertex4d(&tess.verts[vertCount + 2], (float)(x + (float)(width * cosAngle)) + stepY, (float)(y + (float)(width * sinAngle)) + (float)(height * cosAngle), 0.0, w, s1, t1, color);
  R_SetVertex4d(&tess.verts[vertCount + 3], x + stepY, y + (float)(height * cosAngle), 0.0, w, s0, t1, color);
}


void __cdecl RB_StretchPicCmd(GfxCmdHeader **pheader)
{
  GfxCmdStretchPic *icmd;

  icmd = (GfxCmdStretchPic*)*pheader;
  RB_DrawStretchPic(icmd->material, icmd->x, icmd->y, icmd->w, icmd->h, icmd->s0, icmd->t0, icmd->s1, icmd->t1, icmd->color.packed);

  byte** rawstream = (byte**)pheader;
	*rawstream += icmd->header.byteCount;
}

  void RBCmd_6136E0(GfxCmdHeader **);
  void RBCmd_612420(GfxCmdHeader **);
  void RBCmd_612480(GfxCmdHeader **);
  void RBCmd_612340(GfxCmdHeader **);
  void RBCmd_613730(GfxCmdHeader **);
  void RBCmd_611650(GfxCmdHeader **);
  void RBCmd_6116B0(GfxCmdHeader **);
  void RBCmd_6119C0(GfxCmdHeader **);
  void RBCmd_612020(GfxCmdHeader **);
  void RBCmd_611CC0(GfxCmdHeader **);
  void RBCmd_611EC0(GfxCmdHeader **);
  void RBCmd_614FB0(GfxCmdHeader **);
  void RBCmd_615080(GfxCmdHeader **);
  void RBCmd_6124C0(GfxCmdHeader **);
  void RBCmd_612610(GfxCmdHeader **);
  void RBCmd_612CF0(GfxCmdHeader **);
  void RBCmd_6134B0(GfxCmdHeader **);
  void RBCmd_613680(GfxCmdHeader **);
  void RBCmd_6150C0(GfxCmdHeader **);



void (__cdecl *RB_RenderCommandTable[])(GfxCmdHeader **) =
{
  NULL,
  &RBCmd_6136E0,
  &RBCmd_612420,
  &RBCmd_612480,
  &RBCmd_612340,
  &RBCmd_613730,
  &RB_StretchPicCmd,
  &RBCmd_611650,
  &RBCmd_6116B0,
  &RBCmd_6119C0,
  &RBCmd_612020,
  &RBCmd_611CC0,
  &RBCmd_611EC0,
  &RB_DrawText2DCmd,
  &RBCmd_615080,
  &RBCmd_6124C0,
  &RBCmd_612610,
  &RBCmd_612CF0,
  &RBCmd_6134B0,
  &RBCmd_613680,
  &RBCmd_6150C0
};


void REGPARM(1) RB_ExecuteRendererCommandsLoop(GfxCmdHeader *rendererCmdQueue)
{
  GfxCmdHeader *i;

  for ( i = rendererCmdQueue; i->id; rendererCmdQueue = i )
  {
    RB_RenderCommandTable[rendererCmdQueue->id](&i);
  }
  if ( tess.indexCount )
  {
    RB_EndTessSurface();
  }
}


void Gfx_PatchRB_ExecuteRendererCommandsLoop()
{
	Com_Memset((byte*)0x649DE0, NOP, 62);

	byte patch[] =
	{
		MOV_EAX_EBP,
		CALL_ZERO
	};
	memcpy((byte*)0x649DE0, patch, sizeof(patch));
	SetCall(0x649DE0 + 2, RB_ExecuteRendererCommandsLoop);
	SetCall(0x64B646, RB_ExecuteRendererCommandsLoop);
	SetCall(0x6156EC, RB_ExecuteRendererCommandsLoop);

}

int __cdecl R_HashString(const char *string)
{
  const char *pos;
  int hash;

  hash = 0;
  for ( pos = string; *pos; ++pos )
  {
    hash = (char)(*pos | 0x20) ^ 33 * hash;
  }
  return hash;
}

Material* Material_Create2DWithGfxImage(struct GfxImage* image)
{
	Material* material = (Material*)malloc(sizeof(Material) + sizeof(MaterialTextureDef) + sizeof(GfxStateBits));
	memset(material, 0, sizeof(Material));

	material->textureTable = (MaterialTextureDef*)(material +1);
	memset(material->textureTable, 0, sizeof(MaterialTextureDef));

	material->info.name = image->name;
	//material->flags = 0x2F00;
	material->info.gameFlags = 0;//0x22;
	material->info.textureAtlasRowCount = 1;
	material->info.textureAtlasColumnCount = 1;
	memset(material->stateBitsEntry, 0xFF, sizeof(material->stateBitsEntry));
  material->stateBitsEntry[4] = 0;

	material->textureCount = 1;
	material->stateBitsCount = 1;
	material->stateFlags = 3;
	material->cameraRegion = 4;
	material->techniqueSet = DB_FindXAssetHeader(ASSET_TYPE_TECHNIQUE_SET, "2d").techniqueSet;

	material->textureTable->nameHash = R_HashString("colorMap");
	material->textureTable->nameStart = 'c';
	material->textureTable->nameEnd = 'p';
	material->textureTable->semantic = 0;//0xE2; // causes some nice filtering
	material->textureTable->u.image = image;
  material->stateBitTable = (GfxStateBits*)((char*)material + sizeof(Material) + sizeof(MaterialTextureDef));
	material->stateBitTable->loadBits[0] = 0x19289165;
  material->stateBitTable->loadBits[1] = 0xe00e0002;

	return material;
}

/*
Material* Material_Create(const char* name, const char* imageFile)
{
	return Material_CreateWithImage(name, Image_Load(imageFile));
}

*/

void Material_Set2DWithTexture(Material* material, const char* name, IDirect3DTexture9* texture, int height, int width, int depth)
{
  struct GfxImage* image;
  image = material->textureTable->u.image;
  image->name = name;
  image->noPicmip = true;
  image->picmip.platform[0] = 0;
  image->picmip.platform[1] = 0;
  image->track = 0;
  image->semantic = 0;
  image->category = IMG_CATEGORY_AUTO_GENERATED;
  image->mapType = 3;
  image->field_05 = 0;
  image->field_06 = 0;
  image->height = height;
  image->width = width;
  image->depth = depth;
  image->texture.map = texture;
/*
  MaterialTextureDef* texbackup = material->textureTable;
  struct MaterialTechniqueSet* techbackup = material->techniqueSet;
	memset(material, 0, sizeof(Material));
	memset(material->textureTable, 0, sizeof(MaterialTextureDef));

  material->textureTable = texbackup;
  material->techniqueSet = techbackup;
  material->textureTable->u.image = image;
*/
	//Material_InitializeStateMap();

	material->info.name = image->name;
	//material->flags = 0x2F00;
	material->info.gameFlags = 0x22;
	material->info.textureAtlasRowCount = 1;
	material->info.textureAtlasColumnCount = 1;
//	memset(material->stateBitsEntry, 0xFF, sizeof(material->stateBitsEntry));
  material->stateBitsEntry[4] = 0;

	material->textureCount = 1;
	material->stateBitsCount = 1;
	material->stateFlags = 3;
	material->cameraRegion = 4;
//	material->techniqueSet = DB_FindXAssetHeader(ASSET_TYPE_TECHNIQUE_SET, "2d").techniqueSet;

	material->textureTable->nameHash = R_HashString("colorMap");
	material->textureTable->nameStart = 'c';
	material->textureTable->nameEnd = 'p';
	material->textureTable->semantic = 0xE2; // causes some nice filtering
	material->textureTable->u.image = image;

	material->stateBitTable->loadBits[0] = 0x19289165;
  material->stateBitTable->loadBits[1] = 0xe00e0002;


}



Material* Material_Create2DWithTexture(const char* name, IDirect3DTexture9* texture, int height, int width, int depth)
{
  struct GfxImage* image;
  image = malloc(sizeof(struct GfxImage));
  image->name = name;
  image->noPicmip = true;
  image->picmip.platform[0] = 0;
  image->picmip.platform[1] = 0;
  image->track = 0;
  image->semantic = 0;
  image->category = IMG_CATEGORY_AUTO_GENERATED;
  image->mapType = 3;
  image->field_05 = 0;
  image->field_06 = 0;
  image->height = height;
  image->width = width;
  image->depth = depth;
  image->texture.map = texture;

  return Material_Create2DWithGfxImage(image);


}
