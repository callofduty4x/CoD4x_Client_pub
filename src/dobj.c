#include "dobj.h"


#define clientObjMap ((uint16_t*)(0x14A9F30))
#define objBuf ((DObj_t*)(0x1477F30))

DObj_t *Com_GetClientDObj(int handle)
{
  uint16_t index;

  index = clientObjMap[handle];
  
  if ( index )
  {
    return &objBuf[index];
  }
  return NULL;
  
}

DObjAnimMat * DObjGetRotTransArray(DObj_t *obj)
{
  return obj->skel.mat;
}