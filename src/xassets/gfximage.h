#ifndef __GFXIMAGE_H__
#define __GFXIMAGE_H__

#include <d3d9.h>

typedef struct IDirect3DBaseTexture9           D3DBaseTexture;
typedef struct IDirect3DTexture9               D3DTexture;
typedef struct IDirect3DVolumeTexture9         D3DVolumeTexture;
typedef struct IDirect3DCubeTexture9           D3DCubeTexture;
typedef struct IDirect3DSurface9			   D3DSurface;

typedef enum MapType_s
{
  MAPTYPE_NONE = 0x0,
  MAPTYPE_INVALID1 = 0x1,
  MAPTYPE_INVALID2 = 0x2,
  MAPTYPE_2D = 0x3,
  MAPTYPE_3D = 0x4,
  MAPTYPE_CUBE = 0x5,
  MAPTYPE_COUNT = 0x6,
}MapType_t;


struct GfxImageLoadDef
{
  char levelCount;
  char flags;
  int16_t dimensions[3];
  enum _D3DFORMAT format;
  int resourceSize;
  byte data[1];
};



union GfxTexture {
    struct IDirect3DBaseTexture9 * basemap;
    struct IDirect3DTexture9 * map;
    struct IDirect3DVolumeTexture9 * volmap;
    struct IDirect3DCubeTexture9 * cubemap;
    struct GfxImageLoadDef * loadDef;
};

struct Picmip
{
  char platform[2];
};

struct CardMemory
{
  int platform[2];
};


struct GfxImage
{
  MapType_t mapType;
  union GfxTexture texture;
  struct Picmip picmip;
  byte noPicmip;
  byte semantic;
  byte track;
  byte field_05;
  unsigned __int16 field_06;
  struct CardMemory cardMemory;
  unsigned __int16 width;
  unsigned __int16 height;
  unsigned __int16 depth;
  byte category;
  byte delayLoadPixels;
  const char *name;
};

enum $B2FD3713A7CE728D7D4B9B08B4C380D8
{
  IMG_CATEGORY_UNKNOWN = 0x0,
  IMG_CATEGORY_AUTO_GENERATED = 0x1,
  IMG_CATEGORY_LIGHTMAP = 0x2,
  IMG_CATEGORY_LOAD_FROM_FILE = 0x3,
  IMG_CATEGORY_RAW = 0x4,
  IMG_CATEGORY_FIRST_UNMANAGED = 0x5,
  IMG_CATEGORY_WATER = 0x5,
  IMG_CATEGORY_RENDERTARGET = 0x6,
  IMG_CATEGORY_TEMP = 0x7,
};



#endif