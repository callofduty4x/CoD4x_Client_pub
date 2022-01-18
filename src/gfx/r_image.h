#include "r_types_d3d.h"

#define MAX_IMAGES 2048
#define MAX_IMAGES_MASK (MAX_IMAGES -1)

enum $E039F973340A0E27C29B04AB0197BED7
{
  PICMIP_PLATFORM_USED,
  PICMIP_PLATFORM_MINSPEC,
  PICMIP_PLATFORM_COUNT
};

struct ImageList
{
  unsigned int count;
  GfxImage *image[2048];
};


struct ImageGlobals_s
{
  struct GfxImage *imageHashTable[MAX_IMAGES];
  int picmip;
  int picmipBump;
  int picmipSpec;
  CardMemory totalMemory;
};

struct GfxImage
{
  MapType_s mapType;
  union GfxTexture texture;
  struct Picmip picmip;
  byte noPicmip;
  byte semantic;
  byte track;
  byte field_05;
  uint16_t field_06;
  struct CardMemory cardMemory;
  uint16_t width;
  uint16_t height;
  uint16_t depth;
  byte category;
  byte delayLoadPixels;
  const char *name;
};

struct GfxImageLoadDef
{
  uint8_t levelCount;
  uint8_t flags;
  int16_t width;
  int16_t height;
  int16_t depth;
  D3DFORMAT format;
  GfxTexture texture;
  unsigned char data[1];
};

struct __align(2) GfxImageFileHeader
{
  char tag[3];
  char version;
  char format;
  char flags;
  int16_t dimensions[3];
  int fileSizeForPicmip[4];
};

extern ImageGlobals_s imageGlobals;

#define IMG_CATEGORY_UNKNOWN 0
#define IMG_CATEGORY_FIRST_UNMANAGED 5
#define IMG_CATEGORY_NON_PROG 6


#define TS_2D 0

void Image_BuildWaterMap(GfxImage *image);
bool Image_LoadFromFile(GfxImage* image);
D3DCUBEMAP_FACES Image_CubemapFace(unsigned int faceIndex);
void Image_UploadData(const GfxImage *image, D3DFORMAT format, D3DCUBEMAP_FACES face, unsigned int mipLevel, const unsigned char *src);
unsigned int Image_CountMipmaps(unsigned int imageFlags, unsigned int width, unsigned int height, unsigned int depth);
unsigned int Image_GetCardMemoryAmountForMipLevel(D3DFORMAT format, unsigned int mipWidth, unsigned int mipHeight, unsigned int mipDepth);
void Image_PicmipForSemantic(uint8_t semantic, Picmip *picmip);
void Image_GetPicmip(const GfxImage *image, Picmip *picmip);
void Image_SetupRenderTarget(GfxImage *image, uint16_t width, uint16_t height, D3DFORMAT imageFormat, const char *rendertargetname, unsigned int allocFlags);
D3DSurface *Image_GetSurface(GfxImage *image);
void Image_TrackFullscreenTexture(GfxImage *image, int fullscreenWidth, int fullscreenHeight, int picmip, D3DFORMAT format);
void Image_TrackTexture(GfxImage *image, int imageFlags, D3DFORMAT format, int width, int height, int depth);
void Image_Release(GfxImage *image);
void Image_SetupAndLoad(GfxImage *image, int width, int height, int depth, int imageFlags, D3DFORMAT imageFormat, const char* debugname);
void R_ShutdownImages();
void R_ReleaseLostImages();
void R_GetImageList(ImageList *imageList);

void REGPARM(3) Image_LoadDxtc(GfxImage *image, const GfxImageFileHeader *fileHeader, const unsigned char *data, D3DFORMAT format, int bytesPerBlock);
void REGPARM(3) Image_LoadBitmap(GfxImage *image, const GfxImageFileHeader *fileHeader, unsigned char *data, D3DFORMAT format, int bytesPerPixel);
void REGPARM(3) Image_LoadWavelet(GfxImage *image, const GfxImageFileHeader *fileHeader, const unsigned char *data, D3DFORMAT format, int bytesPerPixel);

void Image_LoadFromData(GfxImage *image, GfxImageFileHeader *fileHeader, unsigned char *srcData, unsigned int allocFlags);
void Image_Create2DTexture_PC(GfxImage *image, uint16_t width, uint16_t height, int mipmapCount, int imageFlags, D3DFORMAT imageFormat);
void Image_Create3DTexture_PC(GfxImage *image, uint16_t width, uint16_t height, uint16_t depth, int mipmapCount, int imageFlags, D3DFORMAT imageFormat);
void Image_CreateCubeTexture_PC(GfxImage *image, uint16_t edgeLen, int mipmapCount, D3DFORMAT imageFormat);
