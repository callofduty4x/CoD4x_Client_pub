#include "q_shared.h"
#include "qcommon.h"
#include "cg_shared.h"

double BG_GetBobCycle(playerState_t *ps)
{
  return (double)(uint8_t)ps->bobCycle / 255.0 * 3.1415927 + (double)(uint8_t)ps->bobCycle / 255.0 * 3.1415927 + 6.2831855;
}


uint16_t BG_VehiclesGetSlotTagName(int seatIndex)
{
  if ( seatIndex == VEHICLE_RIDESLOT_DRIVER)
    return scr_const.tag_driver;

  if ( seatIndex == VEHICLE_RIDESLOT_PASSENGER )
    return scr_const.tag_passenger;
  
  assert(seatIndex == VEHICLE_RIDESLOT_GUNNER);

  return scr_const.tag_gunner;
}