#include "r_types_d3d.h"
#include "r_state.h"
#include "r_local.h"
#include "rb_backend.h"
#include "r_material.h"
#include "r_debug.h"

const Material* R_PixelCost_GetAccumulationMaterial(const Material *material)
{
  int index;

  if ( pixelCostMode == GFX_PIXEL_COST_MODE_ADD_COST_IGNORE_DEPTH )
  {
    return rgp.pixelCostAddDepthDisableMaterial;
  }
  if ( pixelCostMode == GFX_PIXEL_COST_MODE_ADD_PASSES_IGNORE_DEPTH )
  {
    return rgp.pixelCostAddDepthDisableMaterial;
  }
  if ( material->techniqueSet->techniques[TECHNIQUE_UNLIT] )
  {
    index = (uint8_t)material->stateBitsEntry[4];
  }
  else
  {
    index = 0;
  }
  switch ( material->stateBitsTable[index].loadBits[1] & 0xF )
  {
      case 0u:
      case 1u:
          return rgp.pixelCostAddDepthAlwaysMaterial;

      case 2u:
      case 3u:
          return rgp.pixelCostAddDepthDisableMaterial;

      case 4u:
      case 5u:
          return rgp.pixelCostAddDepthLessMaterial;

      case 6u:
      case 7u:
          return rgp.pixelCostAddDepthDisableMaterial;

      case 8u:
      case 9u:
          return rgp.pixelCostAddDepthEqualMaterial;

      case 10u:
      case 11u:
          return rgp.pixelCostAddDepthDisableMaterial;

      case 12u:
          return rgp.pixelCostAddNoDepthWriteMaterial;

      case 13u:
          return rgp.pixelCostAddDepthWriteMaterial;

      case 14u:
      case 15u:
          return rgp.pixelCostAddDepthDisableMaterial;

      default:
          assertx(0, "unhandled case %i", material->stateBitsTable[index].loadBits[1] & 0xF);
  }
  return rgp.pixelCostAddNoDepthWriteMaterial;
}

void R_CalcGameTimeVec(float gameTime, float *out)
{
    float cosOfFracPartOfGameTime;
    float sinOfFracPartOfGameTime;
    float fracPartOfGameTime;

    fracPartOfGameTime = gameTime - floor(gameTime);

    sincosf(6.2831855 * fracPartOfGameTime, &sinOfFracPartOfGameTime, &cosOfFracPartOfGameTime);

    out[0] = sinOfFracPartOfGameTime;
    out[1] = cosOfFracPartOfGameTime;
    out[2] = fracPartOfGameTime;
    out[3] = gameTime;
}

void R_SetGameTime(GfxCmdBufSourceState *source, float gameTime)
{
    float gameTimeVec[4];

    R_CalcGameTimeVec(gameTime, gameTimeVec);
    R_SetCodeConstantFromVec4(source, CONST_SRC_CODE_GAMETIME, gameTimeVec);
}


int R_UpdateMaterialTime(GfxCmdBufSourceState *source, float materialTime)
{
    /*
    source->destructibleBurnAmount = burn;
    source->destructibleFadeAmount = fade;
    source->wetness = wetness;
    */
    if ( materialTime == source->materialTime )
    {
        return 0;
    }
    source->materialTime = materialTime;
    R_SetGameTime(source, source->sceneDef.floatTime - materialTime);
    return 1;
}


MaterialTechniqueSet *Material_GetTechniqueSet(Material *material)
{
    assert(material);
    assertx(material && material->techniqueSet, "material '%s' missing techset. If you are building fastfile, check Launcher for error messages.", material->info.name);
    return material ? material->techniqueSet : 0;
}



MaterialTechnique *Material_GetTechnique(Material *material, byte techType)
{
    MaterialTechniqueSet *techSet = Material_GetTechniqueSet(material);
    assertx(techSet, "material '%s' missing techset. %d tech %d", material ? material->info.name : "<undefined>", material ? material->techniqueSet != 0 : -1, techType);
    return techSet ? techSet->techniques[techType] : 0;
}



const char *RB_LogTechniqueType(int id)
{
  const char *techstrings[35];

  techstrings[0] = "TECHNIQUE_DEPTH_PREPASS";
  techstrings[1] = "TECHNIQUE_BUILD_FLOAT_Z";
  techstrings[2] = "TECHNIQUE_BUILD_SHADOWMAP_DEPTH";
  techstrings[3] = "TECHNIQUE_BUILD_SHADOWMAP_COLOR";
  techstrings[4] = "TECHNIQUE_UNLIT";
  techstrings[5] = "TECHNIQUE_EMISSIVE";
  techstrings[6] = "TECHNIQUE_EMISSIVE_SHADOW";
  techstrings[7] = "TECHNIQUE_LIT";
  techstrings[8] = "TECHNIQUE_LIT_SUN";
  techstrings[9] = "TECHNIQUE_LIT_SUN_SHADOW";
  techstrings[10] = "TECHNIQUE_LIT_SPOT";
  techstrings[11] = "TECHNIQUE_LIT_SPOT_SHADOW";
  techstrings[12] = "TECHNIQUE_LIT_OMNI";
  techstrings[13] = "TECHNIQUE_LIT_OMNI_SHADOW";
  techstrings[14] = "TECHNIQUE_LIT_INSTANCED";
  techstrings[15] = "TECHNIQUE_LIT_INSTANCED_SUN";
  techstrings[16] = "TECHNIQUE_LIT_INSTANCED_SUN_SHADOW";
  techstrings[17] = "TECHNIQUE_LIT_INSTANCED_SPOT";
  techstrings[18] = "TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW";
  techstrings[19] = "TECHNIQUE_LIT_INSTANCED_OMNI";
  techstrings[20] = "TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW";
  techstrings[21] = "TECHNIQUE_LIGHT_SPOT";
  techstrings[22] = "TECHNIQUE_LIGHT_OMNI";
  techstrings[23] = "TECHNIQUE_LIGHT_SPOT_SHADOW";
  techstrings[24] = "TECHNIQUE_FAKELIGHT_NORMAL";
  techstrings[25] = "TECHNIQUE_FAKELIGHT_VIEW";
  techstrings[26] = "TECHNIQUE_SUNLIGHT_PREVIEW";
  techstrings[27] = "TECHNIQUE_CASE_TEXTURE";
  techstrings[28] = "TECHNIQUE_WIREFRAME_SOLID";
  techstrings[29] = "TECHNIQUE_WIREFRAME_SHADED";
  techstrings[30] = "TECHNIQUE_SHADOWCOOKIE_CASTER";
  techstrings[31] = "TECHNIQUE_SHADOWCOOKIE_RECEIVER";
  techstrings[32] = "TECHNIQUE_DEBUG_BUMPMAP";
  techstrings[33] = "TECHNIQUE_DEBUG_BUMPMAP_INSTANCED";
  techstrings[34] = "TECHNIQUE_COUNT";
  return techstrings[id];
}


bool R_SetMaterial(GfxCmdBufContext *context, Material *material, MaterialTechniqueType techType)
{
  MaterialTechnique *technique;

  technique = material->techniqueSet->techniques[techType];
  context->state->material = material;
  context->state->technique = technique;
  if ( !technique )
  {
    return false;
  }
  if ( technique->flags & 1 )
  {
    if ( !rg.distortion )
    {
      return false;
    }
  }
  if ( (techType == TECHNIQUE_EMISSIVE || techType == TECHNIQUE_UNLIT) && technique->flags & 0x10 && !context->source->constVersions[4] )
  {
    return false;
  }
  if ( r_logFile->integer )
  {
    _RB_LogPrintf("R_SetMaterial( %s, %s, %s )\n", material->info.name, technique->name, RB_LogTechniqueType(techType));
  }
  context->state->techType = techType;
  return 1;


}


bool R_SetPrepassMaterial(GfxCmdBufContext context, GfxDrawSurf drawSurf, MaterialTechniqueType techType)
{
    MaterialTechnique *technique;
    Material *material;
    unsigned int prepass;
    
    prepass = drawSurf.fields.prepass;
    if ( prepass == 2 )
    {
        return true;
    }
    material = rgp.sortedMaterials[drawSurf.fields.materialSortedIndex];
    if ( !prepass )
    {
        if ( material->stateFlags & 1 )
        {
            material = rgp.depthPrepassMaterial;
        }
    }
    context.state->material = material;
    technique = Material_GetTechnique(material, techType);
    context.state->technique = technique;
    if ( !technique )
    {
        return false;
    }
/*
    if ( technique->flags & 2 )
    {
        if ( !(unsigned __int8)Assert_MyHandler("C:\\projects_pc\\cod\\codsrc\\src\\gfx_d3d\\r_draw_material.cpp", 214, 0, "%s", "!(technique->flags & MTL_TECHFLAG_NEEDS_RESOLVED_SCENE)") )
        {
            __debugbreak();
        }
    }
    if ( technique->flags & 1 )
    {
        if ( !(unsigned __int8)Assert_MyHandler("C:\\projects_pc\\cod\\codsrc\\src\\gfx_d3d\\r_draw_material.cpp", 215, 0, "%s", "!(technique->flags & MTL_TECHFLAG_NEEDS_RESOLVED_POST_SUN)") )
        {
            __debugbreak();
        }
    }
    if ( !material && !(unsigned __int8)Assert_MyHandler("C:\\projects_pc\\cod\\codsrc\\src\\gfx_d3d\\r_draw_material.cpp", 217, 0, "%s", "material") )
    {
        __debugbreak();
    }
    RB_ApplyShaderConstantSet(context.source, &drawSurf);
*/
    context.state->techType = techType;
    return true;
}


bool R_SetTechnique(GfxCmdBufContext context, GfxCmdBufContext *prepassContext, const GfxDrawSurfListInfo *info, GfxDrawSurf drawSurf)
{
  MaterialTechniqueType baseTechType;
  int surfType;
  int primaryLightIndex;
  MaterialTechniqueType techType;

  baseTechType = info->baseTechType;
  surfType = drawSurf.fields.surfType;
  Material* material = rgp.sortedMaterials[drawSurf.fields.materialSortedIndex];

  if(baseTechType == TECHNIQUE_DEPTH_PREPASS)
  {
    if ( !R_SetPrepassMaterial(context, drawSurf, baseTechType) )
    {
      return false;
    }
  }else if(baseTechType == TECHNIQUE_LIT){
    primaryLightIndex = drawSurf.fields.primaryLightIndex;
    if ( !R_SetMaterial(&context, material, (MaterialTechniqueType)context.source->input.data->primaryLightTechType[surfType][primaryLightIndex]) )
    {
      return false;
    }
    R_SetShadowableLight(context.source, primaryLightIndex, info->viewInfo);
  }else{
      if ( baseTechType != TECHNIQUE_DEBUG_BUMPMAP || (surfType != SF_XMODEL_SKINNED && surfType != SF_BEGIN_XMODEL) )
      {
        techType = info->baseTechType;
      }
      else
      {
        techType = TECHNIQUE_DEBUG_BUMPMAP_INSTANCED;
      }
      if ( !R_SetMaterial(&context, material, techType))
      {
        return false;
      }

  }
  if ( prepassContext->state && !R_SetPrepassMaterial(*prepassContext, drawSurf, TECHNIQUE_DEPTH_PREPASS) )
  {
    prepassContext->state = 0;
  }

  context.state->origMaterial = context.state->material;
  context.state->origTechType = context.state->techType;
  if ( (signed int)pixelCostMode > 2 )
  {
      const Material* pixelcostmat = R_PixelCost_GetAccumulationMaterial(context.state->material);
      material = rgp.sortedMaterials[pixelcostmat->info.drawSurf.fields.materialSortedIndex];
      R_SetMaterial(&context, material, TECHNIQUE_UNLIT);
  }

  return true;
}