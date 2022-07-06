#include "cg_shared.h"

void SeatTransformForSlot(int localClientNum, int vehEntNum, int vehSlotIdx, float *resultOrigin, float *resultAngles);

void SeatTransformForClientInfo(int localClientNum, clientInfo_t *ci, float *resultOrigin, float *resultAngles)
{
	SeatTransformForSlot(localClientNum, ci->attachedVehEntNum, ci->attachedVehSlotIndex, resultOrigin, resultAngles);
}

void CG_VehSeatOriginForLocalClient(int localClientNum, float *result)
{
	SeatTransformForClientInfo(localClientNum, &cg.bgs.clientinfo[cg.predictedPlayerState.clientNum], result, 0);
}