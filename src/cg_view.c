#include "cg_shared.h"

#define vehDriverViewHeightMax getcvaradr(0x0CB0CC84)
#define vehDriverViewFocusRange getcvaradr(0x008C9F80)
#define vehDriverViewDist getcvaradr(0x008C9F88)

void CG_UpdateThirdPerson()
{
    clientInfo_t *clInfo;
    int killCamEntity;

    cg.renderingThirdPerson = cg_thirdPerson->boolean || cg.nextSnap->ps.pm_type >= 7;
    clInfo = &cg.bgs.clientinfo[cg.predictedPlayerState.clientNum];

    if(clInfo->attachedVehEntNum != ENTITYNUM_NONE && clInfo->attachedVehSlotIndex == 0/*VEH_RIDESLOT_DRIVER*/)
    {
        cg.renderingThirdPerson = 1;
    }
    
    killCamEntity = cg.predictedPlayerState.killCamEntity;
    if(cg.inKillCam && killCamEntity != ENTITYNUM_NONE && cgEntities[killCamEntity].nextValid)
    {
        cg.renderingThirdPerson = 1;
    }
}

void CG_CalcFov()
{
    CG_UpdateFov(CG_GetViewFov());
}

void CalcViewValuesVehicleDriver()
{
    float viewHeightMax;
    vec3_t origin;
    vec3_t fwd;
    vec3_t pos;
    float v1;
    float v2;

    viewHeightMax = vehDriverViewHeightMax->value;
    VectorCopy(cg.predictedPlayerState.origin, origin);
    origin[2] += 55.0;

    v1 = (cg.predictedPlayerState.viewangles[0] * 0.0027);
    v2 = floorf(v1 + 0.5);

    origin[2] = (((viewHeightMax - fabsf((v1 - v2) * 360.0)) / viewHeightMax) * vehDriverViewFocusRange->value) + origin[2];

    // CG_VehSphereCoordsToPos(vehDriverViewDist->value, cg.predictedPlayerState.viewangles[1], cg.predictedPlayerState.viewangles[0], pos);
    AngleVectors(cg.predictedPlayerState.viewangles, fwd, NULL, NULL);
    VectorScale(fwd, vehDriverViewDist->value, fwd);
    cg.refdef.vieworg[0] = -fwd[0] + origin[0];
    cg.refdef.vieworg[1] = -fwd[1] + origin[1];
    cg.refdef.vieworg[2] = -fwd[2] + origin[2];

    ThirdPersonViewTrace(&cg, origin, cg.refdef.vieworg, cg.refdef.vieworg);

    pos[0] = origin[0] - cg.refdef.vieworg[0];
    pos[1] = origin[1] - cg.refdef.vieworg[1];
    pos[2] = origin[2] - cg.refdef.vieworg[2];

    VectorNormalize(pos);

    vectoangles(pos, cg.refdefViewAngles);
    AnglesToAxis(cg.refdefViewAngles, cg.refdef.viewaxis);

    CG_CalcFov();
}