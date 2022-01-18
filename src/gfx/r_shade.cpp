#include "r_local.h"
#include "r_material.h"
#include "rb_backend.h"

extern "C" void Com_Error(int code, const char* fmt, ...);

void R_TextureFromCodeError(GfxCmdBufState *state, unsigned int codeTexture)
{ 
  //Com_Error(ERR_FATAL, "Tried to use '%s' when it isn't valid\n", rg.codeImageNames[codeTexture]);

  if ( rg.codeImageNames[codeTexture] )
  {
    Com_Error(0, "Code texture %u '%s' isn't valid. Material='%s', tech='%s', techType=%d\n", codeTexture, rg.codeImageNames[codeTexture], state->material->info.name, state->technique->name, (uint8_t)state->techType);
  }
  else
  {
    Com_Error(0, "Code texture %u '%s' isn't valid. Material='%s', tech='%s', techType=%d\n", codeTexture, "noname", state->material->info.name, state->technique->name, (uint8_t)state->techType);
  }
}

const float *R_GetCodeConstant(GfxCmdBufContext *context, unsigned int constant)
{
//  assert(context.local.state);
//  assert(context.local.source);
  assert( constant < MAX_GFXCMDINPUTCONST);

  return context->source->input.consts[constant];
}

bool R_IsShaderConstantUpToDate(GfxCmdBufSourceState *source, uint64_t *constant, MaterialShaderArgument *routingData)
{
  GfxShaderConstantState newState;
  assert(source);

  newState.fields.codeConst = routingData->u.codeConst;
  newState.fields.version = source->constVersions[routingData->u.codeConst.index];
  if ( *constant == newState.packed )
  {
    return true;
  }
  *constant = newState.packed;
  assert(routingData->u.codeConst.rowCount == 1);
  return false;
}

bool R_IsShaderMatrixUpToDate(GfxCmdBufSourceState *source, uint64_t *constant, MaterialShaderArgument *routingData)
{
  GfxShaderConstantState newState;
  unsigned int rowCount;

  newState.fields.codeConst = routingData->u.codeConst;
  newState.fields.version = source->matrixVersions[(routingData->u.codeConst.index - MAX_GFXCMDINPUTCONST) / 4];
  if ( *constant == newState.packed )
  {
    return true;
  }

  *constant = newState.packed;
  rowCount = (uint8_t)routingData->u.codeConst.rowCount;
  assert(rowCount);
  --rowCount;
  while ( rowCount )
  {
    ++constant;
    *constant = -1;
    --rowCount;
  }
  return false;
}

bool R_IsVertexShaderConstantUpToDate(GfxCmdBufContext context, MaterialShaderArgument *routingData)
{
  uint16_t dest = routingData->dest;
  assert(dest < ARRAY_COUNT( context.state->vertexShaderConstState ));

  if ( (signed int)routingData->u.codeConst.index < MAX_GFXCMDINPUTCONST )
  {
    return R_IsShaderConstantUpToDate(context.source, &context.state->vertexShaderConstState[dest], routingData);
  }
  return R_IsShaderMatrixUpToDate(context.source, &context.state->vertexShaderConstState[dest], routingData);

}

void R_SetVertexShaderConstantFromCode_Old(GfxCmdBufContext context, MaterialShaderArgument *routingData)
{
  unsigned int rowCount;
  unsigned int dest;
  D3DDevice *device;
  GfxCmdBufContext source;
  const float *data;

  if ( R_IsVertexShaderConstantUpToDate(context, routingData) != 0 )
  {
    return;
  }
  source = context;
  if ( (signed int)routingData->u.codeConst.index < MAX_GFXCMDINPUTCONST )
  {
    data = R_GetCodeConstant(&source, routingData->u.codeConst.index);
  }
  else
  {
    data = R_GetCodeMatrix(source.source, routingData->u.codeConst.index, (uint8_t)routingData->u.codeConst.firstRow);
  }
  rowCount = (uint8_t)routingData->u.codeConst.rowCount;
  dest = routingData->dest;
  device = context.state->prim.device;

  R_HW_SetVertexShaderConstant(device, dest, data, rowCount);

}


void R_SetPassShaderObjectArguments(GfxCmdBufContext *context, unsigned int argCount, MaterialShaderArgument *arg)
{
  const GfxImage *image;
  char samplerState;

  while ( arg->type == 3 )
  {
/*
    if ( Use_R_SetVertexShaderConstantFromCode_New )
    {
      R_SetVertexShaderConstantFromCode_New(context, arg);
    }
    else
*/
    {
      R_SetVertexShaderConstantFromCode_Old(*context, arg);
    }
    ++arg;
    if ( !--argCount )
    {
      return;
    }
  }
  while ( arg->type == 4 )
  {
/*
    if ( context->source->input.codeImageRenderTargetControl[arg->u.codeSampler].fields.renderTargetId )
    {
      R_SetTextureSamplerCodeImageRenderTarget(*context, arg->dest, context->source->input.codeImageRenderTargetControl[arg->u.codeSampler]);
      ++arg;
      if ( !--argCount )
      {
        return;
      }
    }
    else
*/
    {
      image = R_GetTextureFromCode(context->source, arg->u.codeSampler, &samplerState);
      if ( !image )
      {
        R_TextureFromCodeError(context->state, arg->u.codeSampler);
      }
      R_SetSampler(*context, arg->dest, samplerState, image);
      ++arg;
      if ( !--argCount )
      {
        return;
      }
    }
  }
  //assertx(0, "unreachable");
}

void R_SetPassShaderPrimArguments(GfxCmdBufContext *context, unsigned int argCount, MaterialShaderArgument *arg)
{
  while ( arg->type == 3 )
  {
/*
    if ( Use_R_SetVertexShaderConstantFromCode_New )
    {
      R_SetVertexShaderConstantFromCode_New(context, arg);
    }
    else
*/
    {
      R_SetVertexShaderConstantFromCode_Old(*context, arg);
    }
    ++arg;
    if ( !--argCount )
    {
      return;
    }
  }
  //assertx(0, "unreachable");
}


void R_SetupPassPerObjectArgs(GfxCmdBufContext context)
{
  MaterialPass *pass;

  pass = context.state->pass;
  if ( pass->perObjArgCount )
  {
    R_SetPassShaderObjectArguments(&context, (uint8_t)pass->perObjArgCount, &pass->args[(uint8_t)pass->perPrimArgCount]);
  }
}


void R_SetupPassPerPrimArgs(GfxCmdBufContext context)
{
  MaterialPass *pass;

  pass = context.state->pass;
  if ( pass->perPrimArgCount )
  {
    R_SetPassShaderPrimArguments(&context, (uint8_t)pass->perPrimArgCount, pass->args);
  }
}





void R_SetPixelShaderConstantFromLiteral(GfxCmdBufState *state, unsigned int dest, const float *literal)
{
  assert ( dest < ARRAY_COUNT( state->pixelShaderConstState ));

  state->pixelShaderConstState[dest] = -1;
  R_HW_SetPixelShaderConstant(state->prim.device, dest, literal, 1u);
}

void R_SetPixelShaderConstantFromCode(GfxCmdBufContext *context, MaterialShaderArgument *routingData)
{
  const float *data;

  assertx(context->source->constVersions[routingData->u.codeConst.index], "constant: %d, material: %s, technique: %s", routingData->u.codeConst.index, context->state->material->info.name, context->state->technique->name);
  assert(routingData->u.codeConst.index < CONST_SRC_FIRST_CODE_MATRIX);

  data = R_GetCodeConstant(context, routingData->u.codeConst.index);
  R_HW_SetPixelShaderConstant(context->state->prim.device, routingData->dest, data, (uint8_t)routingData->u.codeConst.rowCount);
}

bool R_IsPixelShaderConstantUpToDate(GfxCmdBufContext *context, MaterialShaderArgument *routingData)
{
  GfxShaderConstantState newState;

  assert(routingData->dest < ARRAY_COUNT( context->state->pixelShaderConstState ));
  assert(routingData->u.codeConst.rowCount == 1);
  assert(context->source);

  newState.fields.codeConst = routingData->u.codeConst;
  newState.fields.version = context->source->constVersions[routingData->u.codeConst.index];
  
  assert(newState.fields.version);
  
  if ( context->state->pixelShaderConstState[routingData->dest] == newState.packed )
  {
    return true;
  }
  context->state->pixelShaderConstState[routingData->dest] = newState.packed;
  return false;
}

void R_SetPassPixelShaderStableArguments(GfxCmdBufContext *context, unsigned int argCount, MaterialShaderArgument *arg)
{
  Material *material;
  MaterialConstantDef *constDef;

  material = context->state->material;
  while ( (signed int)arg->type < 5 )
  {
    ++arg;
    if ( !--argCount )
    {
      return;
    }
  }
  while ( arg->type == 5 )
  {
    if ( !R_IsPixelShaderConstantUpToDate(context, arg) )
    {
      R_SetPixelShaderConstantFromCode(context, arg);
    }
    ++arg;
    if ( !--argCount )
    {
      return;
    }
  }
  constDef = material->constantTable;
  while ( arg->type == 6 )
  {
    while ( constDef->nameHash != arg->u.nameHash )
    {
      ++constDef;
      assertx(constDef != &material->constantTable[material->constantCount], "material '%s' is missing a required named constant", material->info.name);
    }
    R_SetPixelShaderConstantFromLiteral(context->state, arg->dest, constDef->literal);
    ++arg;
    if ( !--argCount )
    {
      return;
    }
  }
  while ( arg->type == 7 )
  {
    R_SetPixelShaderConstantFromLiteral(context->state, arg->dest, arg->u.literalConst);
    ++arg;
    if ( !--argCount )
    {
      return;
    }
  }
  assertx(0, "unreachable");

}


void R_SetupPassCriticalPixelShaderArgs(GfxCmdBufContext context)
{
  MaterialPass *pass;

  pass = context.state->pass;
  if ( pass->stableArgCount )
  {
    R_SetPassPixelShaderStableArguments(&context, (uint8_t)pass->stableArgCount, &pass->args[(uint8_t)pass->perPrimArgCount + (uint8_t)pass->perObjArgCount]);
  }
}


void R_SetVertexShader(GfxCmdBufState *state, const MaterialVertexShader *vertexShader)
{
  if ( state->vertexShader != vertexShader )
  {
    assert(vertexShader);
    assertx(vertexShader->prog.vs, "(vertexShader->name) = %s", vertexShader->name);

    R_HW_SetVertexShader(state->prim.device, vertexShader);
    state->vertexShader = vertexShader;
  }
}

void R_UpdateVertexDecl(GfxCmdBufState *state)
{
  MaterialPass *pass;
  MaterialVertexShader *vertexShader;

  pass = state->pass;
  assert(pass->vertexDecl);

  vertexShader = pass->vertexShader;
  assert(vertexShader);

  R_SetVertexDecl(&state->prim, pass->vertexDecl);

  assert(pass->pixelShader);
  assert(pass->vertexDecl->routing.decl);

  if ( !pass->vertexDecl->routing.decl[state->prim.vertDeclType] )
  {
    Com_Error(ERR_FATAL, "Vertex type %i doesn't have the information used by shader %s in material %s\n", state->prim.vertDeclType, pass->vertexShader->name, state->material->info.name);
  }
  R_SetVertexShader(state, vertexShader);
}


void R_SetVertexDecl(GfxCmdBufPrimState *primState, MaterialVertexDeclaration *vertexDecl)
{
  D3DVertexDeclaration *decl;
  D3DDevice *device;

  if ( vertexDecl )
  {
    decl = vertexDecl->routing.decl[primState->vertDeclType];
  }
  else
  {
    decl = NULL;
  }
  if ( primState->vertexDecl != decl )
  {
    device = primState->device;
    assert(device);
    if ( decl )
    {
      D3DCALL(device->SetVertexDeclaration( decl ));
    }
    primState->vertexDecl = decl;
  }
}
void R_HW_SetVertexShader(D3DDevice *device, const MaterialVertexShader *mtlShader)
{
  assert(device);
  
  D3DCALL(device->SetVertexShader( mtlShader ? mtlShader->prog.vs : NULL ));
}