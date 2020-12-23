#ifndef __DOBJ_H__
#define __DOBJ_H__

#include "cg_shared.h"

signed int DObjGetBoneIndex(DObj_t *obj, unsigned int constBonename, char *index) ;
  
int CG_DObjGetLocalBoneMatrix(cpose_t *pose, DObj_t *obj, char index);
DObj_t *Com_GetClientDObj(int handle);
DObjAnimMat * DObjGetRotTransArray(DObj_t *obj);
#endif