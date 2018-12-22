/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */



enum Legal
{
  ILLEGAL_ALWAYS,
  ILLEGAL_IN_STAGE0,
  LEGAL
}; 

typedef struct CapDef
{
  char *  menu;           // name of cap
  DWORD   dwFlag;         // bit flag for cal
  int op;
  char * name;
  Legal legal;
} CapDef;



extern CapDef SrcAlphaBlendModes[];
extern CapDef DestAlphaBlendModes[];
extern CapDef TextureBlendModes[];
extern CapDef TextureOps[] ;
extern CapDef AlphaOps[] ;
extern CapDef  MINTextureFilterModes[] ;
extern CapDef  MAGTextureFilterModes[] ;
extern CapDef  MIPTextureFilterModes[] ;
extern CapDef CompareModes[] ;

extern CapDef FillModes[] ;
extern CapDef ShadeModes[] ;
extern CapDef AntiAliasModes[] ;
extern CapDef ZBufferModes[] ; 
extern CapDef TextureAddressModes[] ;

extern CapDef VertexFogModes[];
extern CapDef TableFogModes[];

extern CapDef  ArgumentOps[];
extern CapDef  Complement[];
extern CapDef  AlphaReplicate[];
extern CapDef CullModes[];
extern CapDef StencilModes[];
extern CapDef LightTypes[];

