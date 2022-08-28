#ifndef __CG_LOCAL_MP_H__
#define __CG_LOCAL_MP_H__

#include "cg_shared.h"

static inline centity_t * CG_GetEntity(int localClientNum, int entityIndex)
{
  assert ( (unsigned int)localClientNum < MAX_LOCAL_CLIENTS );
  assert ( (unsigned int)entityIndex < MAX_GENTITIES );

  //return &cg_entitiesArray[localClientNum][entityIndex];
  return &cgEntities[entityIndex];
}

#endif
