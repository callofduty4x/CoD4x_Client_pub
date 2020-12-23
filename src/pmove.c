#include "q_shared.h"
#include "qcommon.h"


#define mantle_enable getcvaradr(0x730e58)
#define mantle_view_yawcap getcvaradr(0x730e68)


void Mantle_CapView(playerState_t *ps)
{
  double v2;
  double v3;

  if ( mantle_enable->boolean )
  {
    v2 = AngleDelta(ps->mantleState.yaw, ps->viewangles[1]);
    v3 = -mantle_view_yawcap->floatval;
    if ( v3 > (double)v2 || mantle_view_yawcap->floatval < v2 )
    {
      if(mantle_view_yawcap->floatval <= 0.0)
      {
        Cvar_SetFloat(mantle_view_yawcap, 0.5);
      }

      while( v3 > v2 )
      {
          v2 += mantle_view_yawcap->floatval;
      }
      while( mantle_view_yawcap->floatval < v2)
      {
          v2 -= mantle_view_yawcap->floatval;
      }

      if ( v2 <= 0.0 )
      {
        v3 = mantle_view_yawcap->floatval;
      }
      ps->delta_angles[1] = v2 + ps->delta_angles[1];
      ps->viewangles[1] = AngleNormalize360(ps->mantleState.yaw + v3);
    }
  }
}
