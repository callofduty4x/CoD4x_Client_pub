#include "font.h"
#include "../qcommon.h"
#include "../r_shared.h"
#include "../gfxshared.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "freetype/ftbitmap.h"
#include "freetype/ftbdf.h"
#include <stdbool.h>
#include "../xassets/material.h"
#include "../xzone.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


int R_TextHeight(Font_t* font)
{
	return font->pixelHeight;
}

long double R_NormalizedTextScale(Font_t *font, float scale)
{
  return (float)((float)(48.0 * scale) / (float)R_TextHeight(font));
}

Glyph *__cdecl R_GetCharacterGlyph(struct Font_s *font, unsigned int letter)
{
  Glyph *result;
  Glyph *glyph;
  int top;
  int bottom;
  int mid;

  if ( letter < ' ' || letter > 127 )
  {
    top = font->glyphCount - 1;
    bottom = 96;
    while ( bottom <= top )
    {
      mid = (bottom + top) / 2;
      if ( font->glyphs[mid].letter == letter )
      {
        return &font->glyphs[mid];
      }
      if ( font->glyphs[mid].letter >= letter )
      {
        top = mid - 1;
      }
      else
      {
        bottom = mid + 1;
      }
    }
    result = font->glyphs + 95;
  }
  else
  {
    glyph = &font->glyphs[letter - ' '];
    assert(glyph->letter == letter);
    result = glyph;
  }
  return result;
}



int R_TextWidth(const char *text, signed int maxChars, Font_t *font)
{
  int maxWidth;
  int lineWidth;
  unsigned int decodedLetter;
  signed int count;
  int usedCount;


  if ( maxChars <= 0 )
  {
    maxChars = 0x7FFFFFFF;
  }
  maxWidth = 0;
  lineWidth = 0;
  count = 0;
  while ( *text )
  {
    if ( count >= maxChars )
      break;
    decodedLetter = SEH_DecodeLetter(*text, text[1], &usedCount);
    text += usedCount;
    if ( decodedLetter == '\r' || decodedLetter == '\n' )
    {
      lineWidth = 0;
    }
    else if ( decodedLetter == '^' && *text != '^' && (*text >= '0' && *text <= '@'))
    {
      ++text;
    }
    else
    {
      lineWidth += R_GetCharacterGlyph(font, decodedLetter)->dx;
      if ( maxWidth < lineWidth )
        maxWidth = lineWidth;
      ++count;
    }
  }
  return maxWidth;
}

/*
#define FILE_HASH_SIZE 65536
#define MAX_FONTCACHE 4096
#define MAX_TEXTUREATLASCOUNT 128

typedef struct fontCache_s
{
	struct fontCache_s *hashNext;
	Font_t* font;
	GlyphInfo_t glyph;
	int ttl; // -1 for each frame
}fontCache_t;

struct TextureAtlas
{
  IDirect3DTexture9 *texture;
  Material texture2;
  int refcount;
};

static	fontCache_t*	hashTable[FILE_HASH_SIZE];
fontCache_t	sprite_indexes[MAX_FONTCACHE];
struct TextureAtlas textureAtlasRefs[MAX_TEXTUREATLASCOUNT];

const GlyphInfo_t *Glyph_Find( Font_t* font, wchar_t index )
{
	fontCache_t	*var;

	for (var=hashTable[index] ; var ; var=var->hashNext)
	{
		if (var->font == font)
		{
			return &var->glyph;
		}
	}
	return NULL; //New String, have to render the font new
}


const GlyphInfo_t *Glyph_Insert(Font_t* font, const GlyphInfo_t *glyph, wchar_t index)
{
	fontCache_t	*var;
	int i;

	for(i = 0; i < MAX_FONTCACHE; ++i)
	{
		var = &sprite_indexes[i];
		if(var->font == NULL)
		{
			var->hashNext = hashTable[index];
			hashTable[index] = var;
			var->glyph = *glyph;
			var->font = font;
			var->ttl = 1; //0 or 1?

			return &var->glyph;
		}
	}
  return NULL;
}

void Glyph_CollectGarbage() //Called after FrameEnd
{
	fontCache_t	*var, *newlist, *next;
	int index;

	for(index = 0; index < FILE_HASH_SIZE; ++index)
	{
	  newlist = NULL;
		for (var = hashTable[index] ; var ; var = next)
		{
		  next = var->hashNext;
			if(var->ttl == 0)
			{
				//Remvoe it
        if(var->glyph.glyph)
        {
          var->glyph.glyph->lpVtbl->Release(var->glyph.glyph);
        }
        memset(&var->glyph, 0, sizeof(D3DXFontInfo));

			}else{
				--var->ttl; //Short the live by one

			    //Copy to new list as we want to keep it
			    var->hashNext = newlist;
			    newlist = var;
			}
		}
		hashTable[index] = newlist;
	}
}

#define MAX_TTFFONTS 32
TTFFontStorage_t registeredTTFFonts[MAX_TTFFONTS];

*/

#define MAX_USERFONTS 3

struct FontStorage
{
  struct Font_s *systemfont; //protected
  struct Font_s *consolefont; //protected
  struct Font_s *userfonts[MAX_USERFONTS]; //3 more fonts
};

static struct FontStorage fontassets;

struct Font_s* Font_GetSystemFont()
{
  return fontassets.systemfont;
}

struct Font_s* Font_GetConsoleFont()
{
  return fontassets.consolefont;
}

void Font_ShutdownSingle(struct Font_s* f)
{
  ((IDirect3DTexture9*)f->material->textureTable->u.image->texture.basemap)->lpVtbl->Release((IDirect3DTexture9*)f->material->textureTable->u.image->texture.basemap);
  free(f->material->textureTable->u.image);
  free(f->material);
  Z_Free(f);
}

void Font_ShutdownUser() //unloading all user fonts and replacing with default variants
{
  for(int i = 0; i < MAX_USERFONTS; ++i){
    if(fontassets.userfonts[i] == NULL)
    {
      continue;
    }
    Font_ShutdownSingle(fontassets.userfonts[i]);
    fontassets.userfonts[i] = NULL;
  }
}

static void StoreChar(FT_Face face, int tex_width, int tex_height, int* pen_x, int* pen_y, int* maxheight, char* pixels, Glyph* glyph)
{
	FT_Bitmap* bmp = &face->glyph->bitmap;

  int bitsperpixel;
  int pixelsperbyte;

  switch(bmp->pixel_mode)
  {
    case FT_PIXEL_MODE_MONO:	
      bitsperpixel = 1;
      pixelsperbyte = 8;
      break;
    case FT_PIXEL_MODE_GRAY2:
      bitsperpixel = 2;
      pixelsperbyte = 4;
      break;
    case FT_PIXEL_MODE_GRAY4:
      bitsperpixel = 4;
      pixelsperbyte = 2;
      break;
    case FT_PIXEL_MODE_GRAY:
      bitsperpixel = 8;
      pixelsperbyte = 1;
      break;

    default:
    case FT_PIXEL_MODE_LCD:
    case FT_PIXEL_MODE_LCD_V:
    case FT_PIXEL_MODE_BGRA:
      bitsperpixel = 0;
      pixelsperbyte = 0;
  }

  if(bitsperpixel == 0)
  {
    return; //Unsupported pixel mode format
  }


	if(*pen_x + bmp->width > tex_width){
		*pen_x = 0;
		*pen_y += *maxheight +1;
    *maxheight = 0;
	}

  *maxheight = max(bmp->rows, *maxheight);

	for(int row = 0; row < bmp->rows; ++row){
		for(int col = 0; col < bmp->width; ++col){
			int x = *pen_x + col;
			int y = *pen_y + row;

      byte val = (bmp->buffer[(row * bmp->pitch + col / pixelsperbyte)]);

      int bshift = (((8 - bitsperpixel) - col) % pixelsperbyte) * bitsperpixel;

      byte pixel = val >> bshift;
      int fillbit = pixel & 1;
     
      pixel <<= (8 - bitsperpixel);
      if(fillbit)
      {
        pixel |= ((1 << (8 - bitsperpixel)) -1);
      }


      //8-Bit Alpha + 8-Bit intensity texture
			pixels[y * 2*tex_width + 2*x +0] = 0xff; //Turning Intensity to 100% 
      pixels[y * 2*tex_width + 2*x +1] = pixel;

		}
	}
  glyph->pixelWidth = bmp->width;
  glyph->pixelHeight = bmp->rows;  
  glyph->dx = (face->glyph->advance.x >> 6);
  glyph->y0 = -face->glyph->bitmap_top + (face->size->metrics.descender >> 6);
  //glyph->y0 = -face->glyph->bitmap_top - (char)(0.18f * (float)face->available_sizes->height);
  glyph->x0 = face->glyph->bitmap_left;
  glyph->s0 = ((float)*pen_x +0.005)/ (float)tex_width;
  glyph->s1 = ((float)(*pen_x + bmp->width) +0.005)/ (float)tex_width;
  glyph->t0 = ((float)*pen_y +0.005)/ (float)tex_height;
  glyph->t1 = ((float)(*pen_y + bmp->rows) +0.005)/ (float)tex_height;

	*pen_x += bmp->width +1;
}

Material* Material_Create2DWithTexture(const char* name, IDirect3DTexture9* texture, int height, int width, int depth);

struct Font_s* R_LoadNewFont(const char* fontfile, int size, int loadFlags, bool smoothing)
{
  const char* fontname;
  const char* reason;
  D3DLOCKED_RECT lockedRect;
  FT_Library ft;
	FT_Face    face = NULL;
  int pixelHeight = 0;
  IDirect3DTexture9* fonttexture;
  FT_Error errval;
  BDF_PropertyRec proprec;
  fontname = fontfile;

	if(FT_Init_FreeType(&ft) != 0)
	{
		Com_Error(ERR_FATAL, "Error init freetype!\n");
		return NULL;
	}

	if((errval = FT_New_Face(ft, fontfile, 0, &face)) != 0)
	{
    if(errval == FT_Err_Unknown_File_Format )
    {
		  Com_Error(ERR_FATAL, "Error loading font %s! Unknown file format.\n", fontfile);
    }else
    {
		  Com_Error(ERR_FATAL, "Error loading font %s!\n", fontfile);
    }
		return NULL;
	}
/*  if(!face->charmap || face->charmap->encoding != FT_ENCODING_UNICODE)
  {
		Com_Error(ERR_FATAL, "Error loading font %s! Character map is not unicode.\n", fontfile);
		return NULL;
  }
*/
  if(face->face_flags & FT_FACE_FLAG_SCALABLE)
  {
    if(FT_Set_Char_Size(face, 0, size << 6, 96, 96) != 0)
    {
      Com_Error(ERR_FATAL, "Error setting size of font!\n");
      return NULL;
    }
  }else{
    if(FT_Get_BDF_Property(face, "FONTBOUNDINGBOX", &proprec) == 0)
    {
      //face->size->metrics.descender = proprec.u.integer;
      face->size->metrics.descender = -1 << 6;
      //face->available_sizes->size
    }
    if(face->available_sizes){
      face->size->metrics.height = face->available_sizes->height << 6;
    }else{
      Com_Error(ERR_FATAL, "Font %s has no size metrics.", fontfile);
      return NULL;
    }


  }
  unsigned int findex, glyphcount;
  unsigned long character;
  FT_Bitmap* bmp;
  int x_resolution = 2048;
  int xcounter = 0;
  int ycounter = 0;
  int max_glyphheight = 0;
  char neededchar = ' '; //space is first needed one
  int loadcharacter; //this gets loaded, independed on next character

  glyphcount = 0;
  character = FT_Get_First_Char(face, &findex);

	if(findex > 0)
	{
		do
		{
      loadcharacter = character;
      if(neededchar <= 126)
      {
        if(character > neededchar)
        {
          Com_Error(ERR_FATAL, "Needed character for code %d but it is not contained within font %s\n", neededchar, fontfile);
        }
        if(character == neededchar)
        {
          neededchar++;
        }
      }
      if(character >= ' ' && character != 127)
      {
LOAD_GLYPH_127:
        FT_Load_Char(face, loadcharacter, loadFlags);
        bmp = &face->glyph->bitmap;
        if(xcounter + bmp->width > x_resolution)
        {
          xcounter = 0;
          ycounter += max_glyphheight +1;
          pixelHeight = max(pixelHeight, max_glyphheight);
          max_glyphheight = 0;
        }
        max_glyphheight = max(bmp->rows, max_glyphheight);
        xcounter += bmp->width +1;
        glyphcount++;
        if(loadcharacter == 126)
        {
          loadcharacter = 0;
          goto LOAD_GLYPH_127;
        }

      }
		  character = FT_Get_Next_Char(face, character, &findex);
		}while(findex > 0);
    if(max_glyphheight > 0)
    {
      pixelHeight = max(pixelHeight, max_glyphheight);
      ycounter += max_glyphheight;
    }
	}else{
    Com_Error(ERR_FATAL, "Empty font containing no characters. File: %s\n", fontfile);
    return NULL;
  }
  int height = ycounter;

  if(height < 1)
  {
    Com_Error(ERR_FATAL, "Empty font texture while loading font %s\n", fontfile);
    return NULL;
  }

  HRESULT hres = r_dx.device->lpVtbl->CreateTexture(r_dx.device, x_resolution, height, 1, 0, D3DFMT_A8L8, D3DPOOL_MANAGED, &fonttexture, NULL);

  if(hres != D3D_OK)
  {
    switch(hres)
    {
      case D3DERR_INVALIDCALL:
        reason = "Invalid call";
        break;
      case D3DERR_OUTOFVIDEOMEMORY:
        reason = "Out of video memory";
        break;
      case E_OUTOFMEMORY:
        reason = "Out of system memory";
        break;
    }
    Com_Error(ERR_FATAL, "Couldn't create font texture of size X=%d Y=%d: '%s'", x_resolution, height, reason);
  }

  if(fonttexture->lpVtbl->LockRect(fonttexture, 0, &lockedRect, NULL, 0) != D3D_OK)
  {
    Com_Error(ERR_FATAL, "Couldn't LockRect for font texture");
  }

  xcounter = 0;
  ycounter = 0;
  max_glyphheight = 0;

  Font_t* newfont = Z_Malloc(sizeof(Font_t) + sizeof(Glyph) * glyphcount + strlen(fontname) + 1);
  if(newfont == NULL)
  {
    Com_Error(ERR_FATAL, "Out of memory, can not load font");
    return NULL;
  }

  newfont->glyphCount = glyphcount;
  newfont->glyphs = (Glyph*)(newfont +1);

  Glyph* glyph = newfont->glyphs;

  character = FT_Get_First_Char(face, &findex);
	if(findex > 0)
	{
		do
		{
      if(character >= ' ' && character != 127)
      {
        FT_Load_Char(face, character, loadFlags);
        StoreChar(face, x_resolution, height, &xcounter, &ycounter, &max_glyphheight, (char*)lockedRect.pBits, glyph);
        glyph->letter = character;
        glyph++;
        if(character == 126)
        {

          FT_Load_Char(face, 0, loadFlags);
          Com_Printf(CON_CHANNEL_CLIENT, "Font: %s Pitch: %d Width: %d\n", fontfile, face->glyph->bitmap.pitch, face->glyph->bitmap.width);
          StoreChar(face, x_resolution, height, &xcounter, &ycounter, &max_glyphheight, (char*)lockedRect.pBits, glyph);
          glyph->letter = 127;
          glyph++;
        }
      }
			character = FT_Get_Next_Char(face, character, &findex);
		}while(findex > 0);
	}


  Com_Printf(CON_CHANNEL_CLIENT, "Font descend=%d\n", face->size->metrics.descender >> 6);

  newfont->pixelHeight = face->size->metrics.height >> 6;

  assert(newfont->pixelHeight > 0);

 	FT_Done_FreeType(ft);


  char* pixels = (char*)lockedRect.pBits;
	char* png_data = (char*)calloc(x_resolution * height * 4, 1);
	if(png_data != NULL)
	{
    for(int i = 0; i < (x_resolution * height); ++i){
      png_data[i * 4 + 0] = 255 - (byte)pixels[2*i +1];
      png_data[i * 4 + 1] = 255 - (byte)pixels[2*i +1];
      png_data[i * 4 + 2] = 255 - (byte)pixels[2*i +1];
      png_data[i * 4 + 3] = 0xff;
    }
    char imgfile[2048];
    sprintf(imgfile, "%s.png", fontname);
    stbi_write_png(imgfile, x_resolution, height, 4, png_data, x_resolution * 4);
    free(png_data);
  }
  

  fonttexture->lpVtbl->UnlockRect(fonttexture, 1);

  newfont->material = Material_Create2DWithTexture(fontname, fonttexture, x_resolution, height, 1);
  newfont->glowMaterial = NULL;

  char* storename = (char*)newfont + sizeof(Font_t) + sizeof(Glyph) * glyphcount;
  strcpy(storename, fontname);
  newfont->fontName = storename;
  if(smoothing)
  {
    newfont->material->textureTable->sampleState = -21;
  }
  return newfont;
}

void Font_InitSystemFonts()
{

/*
  if(fontassets.systemfont == NULL)
  {
    fontassets.systemfont = R_LoadNewFont("arial.ttf", 28, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT, true);
  }else{
    fontassets.systemfont->material->techniqueSet = DB_FindXAssetHeader(ASSET_TYPE_TECHNIQUE_SET, "2d").techniqueSet;
  }
*/
  if(fontassets.consolefont == NULL)
  {
    fontassets.consolefont = R_LoadNewFont("cns1-16.bdf", 0, FT_LOAD_RENDER, false);
    //fontassets.consolefont = R_LoadNewFont("tahoma.ttf", 11, FT_LOAD_RENDER | FT_RENDER_MODE_MONO, false);

    fontassets.systemfont = fontassets.consolefont;
  }else{
    fontassets.consolefont->material->techniqueSet = DB_FindXAssetHeader(ASSET_TYPE_TECHNIQUE_SET, "2d").techniqueSet;
  }
}

void Font_InitTTF()
{
  return;
  Font_InitSystemFonts();
  //regaining a handle over Techset 2d
  for(int i = 0; i < MAX_USERFONTS; ++i)
  {
    if(fontassets.userfonts[i] != NULL)
    {
      fontassets.userfonts[i]->material->techniqueSet = DB_FindXAssetHeader(ASSET_TYPE_TECHNIQUE_SET, "2d").techniqueSet;
    }    
  }


}





double __cdecl GetMonospaceWidth(struct Font_s *font, int renderFlags)
{
  if ( renderFlags & 1 )
  {
    return (double)(unsigned __int8)R_GetCharacterGlyph(font, 0x6Fu)->dx;
  }
  return 0.0;
}


void __cdecl GetDecayingStringAlphaInfo(int decayTimeElapsed, int fxDecayDuration, char alpha, char *resultAlpha)
{
  float fade;

  fade = 1.0;
  if ( (signed int)(float)(30.0 * (float)((float)fxDecayDuration / 1000.0)) > 0 )
  {
    if ( decayTimeElapsed <= fxDecayDuration )
    {
      fade = (float)((float)(unsigned __int8)alpha / 255.0) * (float)(1.0 - (float)((float)decayTimeElapsed / (float)fxDecayDuration));
    }
    *resultAlpha = 255 * fade;

    if(*resultAlpha > 255)
    {
      *resultAlpha = 255;
    }
    if(*resultAlpha < 0)
    {
      *resultAlpha = 0;
    }
  }
  else
  {
    *resultAlpha = 0;
  }
}


static unsigned char gJitterData[] =
{
    0,   0,   1,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0,   2,   0, 
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
    1,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
    0,   1,   0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   1,   0,   0,   0, 
    0,   1,   0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   1,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   1,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
    0,   1,   0,   0,   0,   0,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0,   0,   1, 
    0,   0,   0,   0,   0,   0,   1,   0,   0,   0, 
    0,   0,   0,   0,   0,   0
};
static const char MY_ALTCOLOR_TWO[4] = { 230u, 255u, 230u, 220u };
static const float MY_OFFSETS[4][2] =
{ { -1.0, -1.0 }, { -1.0,  1.0 }, {  1.0, -1.0 }, {  1.0,  1.0 } };

static char *MYRANDOMCHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
static char *MYRANDOMNUMCHARS = "023456789";

unsigned int __cdecl R_FontGetRandomLetter(struct Font_s *font, int seed)
{
  return MYRANDOMCHARS[RandWithSeed(&seed) % 0x3Eu];
}

unsigned int __cdecl R_FontGetRandomNumberCharacter(struct Font_s *font, int seed)
{
  return MYRANDOMNUMCHARS[RandWithSeed(&seed) % 9u];
}


//CoD4 version: void __cdecl DrawText2D(const char *text, float x, float y,                 Font_t *font, float xScale, float yScale, float sinAngle, float cosAngle, GfxColor color, int maxLength, int renderFlags, int cursorPos, char cursorLetter, float padding, GfxColor glowForcedColor, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration,                                                        Material *fxMaterial, Material *fxMaterialGlow);

void __cdecl DrawText2D(const char *text, float x, float y, float w, struct Font_s *font, float xScale, float yScale, float sinAngle, float cosAngle, GfxColor color, int maxLength, int renderFlags, int cursorPos, char cursorLetter, float padding, GfxColor glowForcedColor, int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration, int fxRedactDecayStartTime, int fxRedactDecayDuration, Material *fxMaterial, Material *fxMaterialGlow)
{
  unsigned __int8 modcolor;
  int randseed;
  signed int decaymil; 
  float resizeOffsY;
  int offIdx; 
  float resizeOffsX; 
  float ofs; 
  GfxColor shiftColor; 
  GfxColor targetColor;
  Glyph *glyphOriginal;
  int tempSeed; 
  float iconWidth;
  GfxColor lookupColor; 
  const char *altColorTwo;
  bool drawExtraFxChar;
  Glyph *glyph;
  float yAdj;
  float decayOffset;
  float xAdj;
  bool skipDrawing;
  unsigned int letter;
  int extraFxChar;
  float deltaX;
  unsigned int origLetter;
  float yRot;
  int passRandSeed;
  int maxLengthRemaining;
  float xRot;
  bool subtitleAllowGlow;
  GfxColor currentColor;
  const char *curText;
  int count;
  int xdelta;
  int ydelta;
  int jitterSeed;
  bool drawLargeCharAtEnd;
  GfxColor finalColor;
  int passIdx;
  bool bFlash;
  GfxColor dropShadowColor;
  Material *material;
  bool drawRandomCharAtEnd;
  int randSeed;
  bool shiftColorOn;
  float startX; 
  int randomCharsLength;
  bool drawUnderscoreCharAtEnd;
  int decayTimeElapsed;
  char shiftColorTarget;
  Material *glowMaterial;
  bool continueDrawing;
  float monospaceWidth;
  float total_rect[4];
  float sizeIncrease;
  bool decaying;
  float startY; 
  int passCount;
  char fadeAlpha;


  #if 0
  //This needs redact material from BlackOps to work, without it this can not be used.
  const int MAX_REDACT_RECT = 4; // [esp+2D0h] [ebp-4h]
  const int MAX_FLASH_TIME = 175;
  int redactAlpha = 255;
  bool redactLeftOn = 0;
  int redactLeftCount = 0;
  float redactLeftRect[4][4];
  float redactRect[4][4];
  bool redactOn = 0;
  int redactCount = 0;
  float maxY  = -3.4028235e38;
  float minY = 3.4028235e38;
  float out_rect; 
  float (*rect)[4]; 
  unsigned int fadeColor; 
  Material *mat; 
  float u; 
  char curChar; 
  int h; 
  int i; 
  #endif
  
  float xa; 
  float ya; 
  static int lastFlashTime;
  static bool bFlashToggle;

  sizeIncrease = 1.0;
  shiftColorOn = 0;
  shiftColorTarget = 0;
  bFlash = 0;

  assert(text);
  assert(font);

  dropShadowColor.packed = 0;
  dropShadowColor.array[3] = color.array[3];
  randSeed = 1;
  randomCharsLength = 0;
  drawRandomCharAtEnd = 0;
  drawUnderscoreCharAtEnd = 0;
  drawLargeCharAtEnd = 0;
  monospaceWidth = GetMonospaceWidth(font, renderFlags);
  glowMaterial = 0;
  material = Material_FromHandle(font->material);

  assert(material);

  assert(!(renderFlags & TEXT_RENDERFLAG_FX_DECODE) || (fxMaterial && fxMaterial->techniqueSet));
  assert(!(renderFlags & TEXT_RENDERFLAG_FX_DECODE) || (fxMaterialGlow && fxMaterialGlow->techniqueSet));

  if ( !lastFlashTime )
  {
    lastFlashTime = Sys_Milliseconds();
  }
  int flashTime = Sys_Milliseconds() - lastFlashTime;
  if ( flashTime > 175 )
  {
    lastFlashTime = Sys_Milliseconds();
    bFlashToggle = bFlashToggle == 0;
  }
  if ( renderFlags & 0x800 )
  {
    continueDrawing = SetupFadeinFXVars(text, maxLength, renderFlags, fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration, &drawRandomCharAtEnd, &randSeed, &decaying, &decayTimeElapsed);
  }
  else if ( renderFlags & 0x400 )
  {
    continueDrawing = SetupTypewriterFXVars(text, maxLength, renderFlags, fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration, &drawRandomCharAtEnd, &randSeed, &maxLength, &decaying, &decayTimeElapsed);
  }
  else if ( renderFlags & 0x1000 )
  {
    drawUnderscoreCharAtEnd = 1;
    continueDrawing = SetupTypewriterFXVars(text, maxLength, renderFlags, fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration, &drawRandomCharAtEnd, &randSeed, &maxLength, &decaying, &decayTimeElapsed);
  }
  else if ( renderFlags & 0x2000 )
  {
    drawLargeCharAtEnd = 1;
    continueDrawing = SetupPopInFXVars(text, maxLength, renderFlags, fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration, &sizeIncrease, &randSeed, &maxLength, &decaying, &decayTimeElapsed);
  }
#if 0
        //This needs redact material from BlackOps to work, without it this can not be used.
  else if ( renderFlags & 0x4000 )
  {
    continueDrawing = SetupRedactTextFXVars(text, maxLength, renderFlags, fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration, &drawRandomCharAtEnd, &randSeed, &maxLength, &decaying, &decayTimeElapsed);
  }
#endif
  else if ( renderFlags & 0x10000 )
  {
    continueDrawing = SetupCOD7DecodeFXVars(text, maxLength, fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration, &randSeed, &randomCharsLength, &maxLength, &decaying, &decayTimeElapsed);
  }
  else
  {
    continueDrawing = SetupPulseFXVars(text, maxLength, renderFlags, fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration, &drawRandomCharAtEnd, &randSeed, &maxLength, &decaying, &decayTimeElapsed);
  }
  if ( continueDrawing )
  {
    passCount = 1;
    if ( renderFlags & 0x10 )
    {
      glowMaterial = Material_FromHandle(font->glowMaterial);
      assert(glowMaterial);
      ++passCount;
    }
    if ( renderFlags & TEXT_RENDERFLAG_FX_DECODE )
    {
      if ( renderFlags & 0x10 )
      {
        assert(fxMaterialGlow);
      }
      assert(fxMaterial);
    }

    if ( renderFlags & 0x8000 )
    {
      if ( gJitterData[(gfxCmdBufSourceState.sceneDef.time >> 5) % 176u] )
      {
        jitterSeed = gfxCmdBufSourceState.sceneDef.time >> 5;
        if ( gJitterData[(gfxCmdBufSourceState.sceneDef.time >> 5) % 176u] == 1 )
        {
          x = (float)(RandWithSeed(&jitterSeed) % 7 - 3) + x;
          y = (float)(RandWithSeed(&jitterSeed) % 11 - 5) + y;
        }
        if ( gJitterData[(gfxCmdBufSourceState.sceneDef.time >> 5) % 176u] == 2 )
        {
          xdelta = RandWithSeed(&jitterSeed) % 10 + 45;
          ydelta = RandWithSeed(&jitterSeed) % 10 + 45;
          if ( RandWithSeed(&jitterSeed) % 2 )
          {
            ydelta = -ydelta;
          }
          x = (float)xdelta + x;
          y = (float)ydelta + y;
        }
      }
    }
    startX = x - (0.5 * xScale);
    startY = y - (0.5 * yScale);
    total_rect[0] = 3.4028235e38;
    total_rect[1] = 3.4028235e38;
    total_rect[2] = -3.4028235e38;
    total_rect[3] = -3.4028235e38;
    for ( passIdx = 0; passIdx < passCount; ++passIdx )
    {
      maxLengthRemaining = maxLength;
      passRandSeed = randSeed;
      currentColor.packed = color.packed;
      xa = startX;
      ya = startY;
      subtitleAllowGlow = 0;
      count = 0;
      curText = text;
      while ( *curText && maxLengthRemaining )
      {
        drawExtraFxChar = 0;
        letter = SEH_ReadCharFromString(&curText);
        skipDrawing = 0;
        fadeAlpha = 0;
        drawExtraFxChar = 0;
        extraFxChar = 0;
        if ( letter == '^' && curText && *curText != '^' && *curText >= '0' && *curText <= '@' )
        {
          subtitleAllowGlow = 0;
          if ( (unsigned char)ColorIndex(*curText) == (unsigned char)ColorIndex(55) )
          {
            currentColor.packed = color.packed;
          }
          else if ( renderFlags & 0x100 && ColorIndex(*curText) == 2 )
          {
            altColorTwo = MY_ALTCOLOR_TWO;
            modcolor = ModulateByteColors(MY_ALTCOLOR_TWO[3], color.array[3]);
            currentColor.packed = (modcolor << 24) | *((unsigned char *)altColorTwo + 2) | (*((unsigned char *)altColorTwo + 1) << 8) | (*(unsigned char *)altColorTwo << 16);
            subtitleAllowGlow = 1;
          }
          else
          {
            RB_LookupColor(*curText, (char*)lookupColor.array);
            currentColor.packed = ((unsigned char)color.array[3] << 24) | (unsigned char)lookupColor.array[2] | ((unsigned char)lookupColor.array[1] << 8) | ((unsigned char)lookupColor.array[0] << 16);
          }
          ++curText;
          count += 2;
        }
        else
        //if ( letter != '^' || !curText || *curText == '^' || *curText != 'F' || r_glob.isMultiplayer ) //always Multiplayer
        {
       
#if 0
        //This needs redact material from BlackOps to work, without it this can not be used.


          if ( renderFlags & 0x4000 && letter == '^' )
          {
            curChar = *curText;
            if ( *curText == 'C' )
            {
              if ( shiftColorOn )
              {
                shiftColorOn = 0;
                ++curText;
                ++count;
              }
              else
              {
                shiftColorOn = 1;
                shiftColorTarget = *++curText;
                ++curText;
                count += 3;
              }
            }
            else if ( curChar == 'L' )
            {
              if ( redactLeftOn )
              {
                redactLeftOn = 0;
                ++redactLeftCount;
                ++curText;
                count += 2;
              }
              else if ( redactLeftCount < 4 )
              {
                redactLeftRect[redactLeftCount][0] = 3.4028235e38;
                redactLeftRect[redactLeftCount][1] = 3.4028235e38;
                redactLeftRect[redactLeftCount][2] = -3.4028235e38;
                redactLeftRect[redactLeftCount][3] = -3.4028235e38;
                redactLeftOn = 1;
                ++curText;
                count += 2;
              }
            }
            else if ( curChar == 'U' )
            {
              if ( redactOn )
              {
                redactOn = 0;
                ++redactCount;
                ++curText;
                count += 2;
              }
              else if ( redactCount < 4 )
              {
                redactRect[redactCount][0] = 3.4028235e38;
                redactRect[redactCount][1] = 3.4028235e38;
                redactRect[redactCount][2] = -3.4028235e38;
                redactRect[redactCount][3] = -3.4028235e38;
                redactOn = 1;
                ++curText;
                count += 2;
              }
            }
          }
          else
#endif
          {
            if ( maxLengthRemaining > randomCharsLength || letter == ' ' )
            {
              if ( randomCharsLength <= 0 )
              {
                if ( drawRandomCharAtEnd && maxLengthRemaining == 1 )
                {
                  letter = R_FontGetRandomLetter(font, passRandSeed);
                  fadeAlpha = -64;
                  if ( RandWithSeed(&passRandSeed) % 2 )
                  {
                    drawExtraFxChar = 1;
                    letter = 'O';
                  }
                }
              }
              else
              {
                fadeAlpha = -1;
              }
            }
            else
            {
              randseed = RandWithSeed(&passRandSeed);
              letter = R_FontGetRandomNumberCharacter(font, randseed);
              fadeAlpha = -64;
            }
            if ( drawUnderscoreCharAtEnd && maxLengthRemaining == 1 )
            {
              letter = '_';
            }
            if ( letter != '^' || (*curText != 1 && *curText != 2) )
            {
              if ( letter == '\n' )
              {
                xa = startX;
                ya = ((float)font->pixelHeight * yScale) + ya;
              }
              else if ( letter == '\r' )
              {
                xa = startX;
              }
              else
              {
                origLetter = letter;
                if ( decaying )
                {
                  if ( renderFlags & 0x1000 )
                  {
                    skipDrawing = 0;
                    decaymil = (signed int)(float)(30.0 * ((float)fxDecayDuration / 1000.0));
                    if ( decayTimeElapsed >= (fxDecayDuration / decaymil) * (RandWithSeed(&passRandSeed) % decaymil) )
                    {
                      skipDrawing = 1;
                    }
                    fadeAlpha = 255;
                    drawExtraFxChar = 0;
                  }
                  else
                  {
                    GetDecayingStringAlphaInfo(decayTimeElapsed, fxDecayDuration, currentColor.array[3], &fadeAlpha);
                  }
                }
                if ( drawExtraFxChar )
                {
                  tempSeed = passRandSeed;
                  extraFxChar = RandWithSeed(&tempSeed);
                }
                glyph = R_GetCharacterGlyph(font, letter);
                if ( letter == origLetter )
                {
                  decayOffset = 0.0;
                  deltaX = (float)(unsigned __int8)glyph->dx;
                }
                else
                {
                  glyphOriginal = R_GetCharacterGlyph(font, origLetter);
                  decayOffset = ((float)(unsigned char)glyphOriginal->pixelWidth * 0.5) - ((float)(unsigned __int8)glyph->pixelWidth * 0.5);
                  deltaX = (float)(unsigned __int8)glyphOriginal->dx;
                }
                xAdj = ((float)glyph->x0 + decayOffset) * xScale;
                yAdj = (float)glyph->y0 * yScale;
                if ( drawLargeCharAtEnd && maxLengthRemaining == 1 )
                {
                  yScale = yScale * sizeIncrease;
                  xScale = xScale * sizeIncrease;
                  yAdj = (float)((float)(sizeIncrease + 1.0) * yAdj) / 2.0;
                }
                finalColor.packed = RB_ProcessLetterColor(currentColor.packed, letter);
                if ( decaying || (drawRandomCharAtEnd && maxLengthRemaining == 1) || randomCharsLength > 0 )
                {
                  finalColor.array[3] = ModulateByteColors(finalColor.array[3], fadeAlpha);
                  dropShadowColor.array[3] = fadeAlpha;
                }
                if ( shiftColorOn )
                {
                  RB_LookupColor(shiftColorTarget, (char *)&targetColor);
                  targetColor.packed = ((unsigned __int8)finalColor.array[3] << 24) | (unsigned __int8)targetColor.array[2] | ((unsigned __int8)targetColor.array[1] << 8) | ((unsigned __int8)targetColor.array[0] << 16);
                  GetShiftColor(targetColor, fxBirthTime, fxRedactDecayDuration + fxRedactDecayStartTime, fxRedactDecayDuration, finalColor, &shiftColor);
                  finalColor.packed = shiftColor.packed;
                }
                if ( !skipDrawing )
                {
                  if ( passIdx )
                  {
                    if ( passIdx == 1 && (!(renderFlags & 0x100) || subtitleAllowGlow) )
                    {
                      GlowColor(&finalColor, finalColor, glowForcedColor, renderFlags);
                      resizeOffsX = (float)((float)((float)(unsigned char)glyph->pixelWidth) * xScale) * (-0.75 * 0.5);
                      resizeOffsY = (float)((float)((float)(unsigned char)glyph->pixelHeight) * yScale) * (0.125 * 0.5);
                      for ( offIdx = 0; offIdx < 4; ++offIdx )
                      {
                        xRot = (float)((float)(xa + xAdj) + resizeOffsX) + (float)((float)((float)MY_OFFSETS[offIdx][0] * 2.0) * xScale);
                        yRot = (float)((float)(ya + yAdj) + resizeOffsY) + (float)((float)((float)MY_OFFSETS[offIdx][1] * 2.0) * yScale);
                        RotateXY(cosAngle, sinAngle, startX, startY, xRot, yRot, &xRot, &yRot);
                        assert(glowMaterial);

                        if ( !bFlash || bFlashToggle )
                        {
                          if ( drawExtraFxChar )
                          {
                            DrawTextFxExtraCharacter(fxMaterialGlow, extraFxChar, xRot, yRot, (float)(unsigned __int8)glyph->pixelWidth * xScale, (float)(unsigned __int8)glyph->pixelHeight * yScale, sinAngle, cosAngle, finalColor.packed);
                          }
                          else
                          {
                            if(glowMaterial)
                            RB_DrawStretchPicRotate_New(glowMaterial, xRot, yRot, w,
                              ((float)(unsigned __int8)glyph->pixelWidth * xScale) * (1.0 + 0.75),
                              ((float)(unsigned __int8)glyph->pixelHeight * yScale) * (1.0 + 0.125),
                              glyph->s0, glyph->t0, glyph->s1, glyph->t1, sinAngle, cosAngle, finalColor.packed/*, GFX_PRIM_STATS_HUD*/);
                          }
                        }
                      }
                    }
                  }
                  else
                  {
                    if ( renderFlags & 4 )
                    {
                      ofs = 1.0;
                      if ( renderFlags & 8 )
                      {
                        ofs = ofs + 1.0;
                      }
                      xRot = (xa + xAdj) + (ofs * xScale);
                      yRot = (ya + yAdj) + (ofs * yScale);
                      RotateXY(cosAngle, sinAngle, startX, startY, xRot, yRot, &xRot, &yRot);
                      if ( !bFlash || bFlashToggle )
                      {
                        if ( drawExtraFxChar )
                        {
                          DrawTextFxExtraCharacter(fxMaterial, extraFxChar, xRot, yRot, (float)(unsigned __int8)glyph->pixelWidth * xScale, (float)(unsigned __int8)glyph->pixelHeight * yScale, sinAngle, cosAngle, dropShadowColor.packed);
                        }
                        else
                        {
                          RB_DrawStretchPicRotate_New(material, xRot, yRot, w, (float)(unsigned __int8)glyph->pixelWidth * xScale, (float)(unsigned __int8)glyph->pixelHeight * yScale, glyph->s0, glyph->t0, glyph->s1, glyph->t1, sinAngle, cosAngle, dropShadowColor.packed /*, GFX_PRIM_STATS_HUD*/);
                        }
                      }
                    }
                    xRot = xa + xAdj;
                    yRot = ya + yAdj;
                    RotateXY(cosAngle, sinAngle, startX, startY, xa + xAdj, ya + yAdj, &xRot, &yRot);
                    if ( !bFlash || bFlashToggle )
                    {
                      if ( drawExtraFxChar )
                      {
                        DrawTextFxExtraCharacter(fxMaterial, extraFxChar, xRot, yRot, (float)(unsigned __int8)glyph->pixelWidth * xScale, (float)(unsigned __int8)glyph->pixelHeight * yScale, sinAngle, cosAngle, finalColor.packed);
                      }
                      else
                      {
                        RB_DrawStretchPicRotate_New(material, xRot, yRot, w, (float)(unsigned __int8)glyph->pixelWidth * xScale, (float)(unsigned __int8)glyph->pixelHeight * yScale, glyph->s0, glyph->t0, glyph->s1, glyph->t1, sinAngle, cosAngle, finalColor.packed/*, GFX_PRIM_STATS_HUD*/);
                      }
                    }
#if 0
    //This needs redact material from BlackOps to work, without it this can not be used.  
                    if ( redactLeftOn || redactOn )
                    {
                      redactAlpha = (unsigned __int8)finalColor.array[3];
                      if ( redactLeftOn )
                      {
                        rect = (float (*)[4])redactLeftRect[redactLeftCount];
                      }
                      else
                      {
                        rect = (float (*)[4])redactRect[redactCount];
                      }
                      if ( (*rect)[0] > xRot )
                      {
                        (*rect)[0] = xRot;
                      }
                      if ( (float)((float)((float)(unsigned __int8)glyph->dx * xScale) + xa) > (*rect)[2] )
                      {
                        (*rect)[2] = (float)((float)(unsigned __int8)glyph->dx * xScale) + xa;
                      }
                      if ( glyph->letter != ' ' )
                      {
                        if ( minY > (float)(ya - (float)(unsigned __int8)glyph->pixelHeight) )
                        {
                          minY = ya - (float)(unsigned __int8)glyph->pixelHeight;
                        }
                        if ( ya > maxY )
                        {
                          maxY = ya;
                        }
                      }
                    }
#endif
                    if ( total_rect[0] > xRot )
                    {
                      total_rect[0] = xRot;
                    }
                    if ( (float)((float)((float)(unsigned __int8)glyph->dx * xScale) + xa) > total_rect[2] )
                    {
                      total_rect[2] = (float)((float)(unsigned __int8)glyph->dx * xScale) + xa;
                    }
                    if ( renderFlags & 2 )
                    {
                      if ( count == cursorPos )
                      {
                        xRot = xa + xAdj;
                        RotateXY(cosAngle, sinAngle, startX, startY, xa + xAdj, ya, &xRot, &yRot);
                        RB_DrawCursor(material, cursorLetter, xRot, yRot, sinAngle, cosAngle, font, xScale, yScale, finalColor.packed);
                      }
                    }
                  }
                }
                if ( renderFlags & 1 )
                {
                  xa = (float)(monospaceWidth * xScale) + xa;
                }
                else
                {
                  xa = (float)(deltaX * xScale) + xa;
                }
                if ( renderFlags & 0x80 )
                {
                  xa = (float)(padding * xScale) + xa;
                }
                ++count;
                --maxLengthRemaining;
              }
            }
            else
            {
              RotateXY(cosAngle, sinAngle, startX, startY, xa, ya, &xRot, &yRot);
              if ( !bFlash || bFlashToggle )
              {
                iconWidth = RB_DrawHudIcon(curText, xRot, yRot, sinAngle, cosAngle, font, xScale, yScale, currentColor.packed);
              }
              else
              {
                iconWidth = (float)((font->pixelHeight * (curText[1] - 16) + 16) / 32) * xScale;
              }
              assert(iconWidth > 0);

              xa = xa + iconWidth;
              if ( renderFlags & 0x80 )
              {
                xa = (float)(padding * xScale) + xa;
              }
              curText += 7;
              ++count;
              --maxLengthRemaining;
            }
          }
        }
        /*
        always Multiplayer
        else
        {
          bFlash = bFlash == 0;
          ++curText;
          count += 2;
        }*/
      }
      if ( renderFlags & 2 )
      {
        if ( count == cursorPos )
        {
          xRot = xa;
          RotateXY(cosAngle, sinAngle, startX, startY, xa, ya, &xRot, &yRot);
          RB_DrawCursor(material, cursorLetter, xRot, yRot, sinAngle, cosAngle, font, xScale, yScale, color.packed);
        }
      }
    }
#if 0
    //This needs redact material from BlackOps to work, without it this can not be used.    
    if ( renderFlags & 0x4000 )
    {
      fadeColor = 0;
      if ( 16 * redactAlpha > 255 )
      {
        v33 = -1;
      }
      else
      {
        v33 = 16 * redactAlpha;
      }
      HIBYTE(fadeColor) = v33;
      for ( i = 0; i < redactLeftCount; ++i )
      {
        SetupRedactFXVars(&randSeed, fxBirthTime, maxLength, fxRedactDecayStartTime, fxRedactDecayDuration, 1, redactLeftRect[i], total_rect, &out_rect, &u, &mat);
        if ( u != 0.0 )
        {
          RB_DrawStretchPic(mat, out_rect, minY, v45 - out_rect, maxY - minY, 0.0, 0.0, u, 1.0, fadeColor/*, GFX_PRIM_STATS_HUD*/);
        }
      }
      for ( h = 0; h < redactCount; ++h )
      {
        RB_DrawStretchPic(rgp.redactMaterial1, redactRect[h][0], minY, redactRect[h][2] - redactRect[h][0], maxY - minY, 0.0, 0.0, 1.0, 1.0, fadeColor/*, GFX_PRIM_STATS_HUD*/);
      }
    }
#endif
  }
}


