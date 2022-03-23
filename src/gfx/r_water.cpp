#include "r_local.h"

void R_UploadWaterTexture(water_t *water, float floatTime)
{
    assert(water);

    if ( water->writable.floatTime != floatTime )
    {
        water->writable.floatTime = floatTime;
        /*if ( r_drawWater->current.enabled )
        {
            R_UploadWaterTextureInternal(&water);
        }*/
    }
}

