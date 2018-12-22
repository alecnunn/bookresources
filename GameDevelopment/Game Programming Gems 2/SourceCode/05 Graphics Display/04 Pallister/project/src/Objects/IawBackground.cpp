// IawBackground.cpp App Wizard Version 2.0 Beta 1
// ----------------------------------------------------------------------
// 
// Copyright © 2001 Intel Corporation
// All Rights Reserved
// 
// Permission is granted to use, copy, distribute and prepare derivative works of this 
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  This software is provided "AS IS." 
//
// Intel specifically disclaims all warranties, express or implied, and all liability,
// including consequential and other indirect damages, for the use of this software, 
// including liability for infringement of any proprietary rights, and including the 
// warranties of merchantability and fitness for a particular purpose.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

// ----------------------------------------------------------------------
//
// PURPOSE:
//    
// IawBackground.cpp: implementation of the CIawBackground class
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Construction
IawBackground::IawBackground(DWORD flags) : IawObject(flags)
{
  SetObjectType(IAW_OBJECT_TYPE_BACKGROUND);
}

// Destruction
IawBackground::~IawBackground()
{
  ;// empty
}


// Initialize
HRESULT IawBackground::Init(IawD3dWrapper* pWrapper)
{
  // Set member variable to wrapper
  SetWrapper(pWrapper);

  //The background is just a quad
  SetNumVerts(4);
  SetNumIndices(6);

  // Allocate enough memory for background plane vertices
  AllocVertMem(GetVertexSize() );
  AllocIndicesMem(6);

  if (SetupVertices(GetVerticesPtr()) != S_OK)
  {
    OutputDebugString("unable to set up verts");
    return -1;
  }

  if (SetupIndices() != S_OK)
  {
    OutputDebugString("unable to set up indices");
    return -1;
  }

  return S_OK;
}


// Set up background plane vertices, takiong into account window size
HRESULT IawBackground::SetupVertices(float* pVertices)
{
  float width     = float(GetWrapper()->GetWidth());
  float height    = float(GetWrapper()->GetHeight());
  float margin    = 0.01f;

  // local method SuperVertex array to set up background vertices
  IAW_SUPERVERTEX sVertex[4];

  sVertex[0].mX = 0.0f + margin;    sVertex[0].mY = 0.0f + margin;
  sVertex[1].mX = width - margin;   sVertex[1].mY = height - margin;
  sVertex[2].mX = 0.0f + margin;    sVertex[2].mY = height - margin;
  sVertex[3].mX = width + margin;   sVertex[3].mY = 0.0f + margin;

  sVertex[0].mU = 0.0f;   sVertex[0].mU = 0.0f;
  sVertex[1].mU = 1.0f;   sVertex[1].mU = 1.0f;
  sVertex[2].mU = 0.0f;   sVertex[2].mU = 1.0f;
  sVertex[3].mU = 1.0f;   sVertex[3].mU = 0.0f;

  // common elements
  for (int i=0;i<4;i++)
  {
    sVertex[i].mZ        = 0.999f;
    sVertex[i].mRhw      = 1.00000f;
    sVertex[i].mDiffuse  = D3DCOLOR_COLORVALUE(0.1f, 0.1f, 0.1f, 1.0f);
    sVertex[i].mSpecular = 0;
  }

  // color the top of the Background differently (R,G,B for X,Y,Z)
  FLOAT r, g, b;
  // Note that the retrieved color value is divided by MAX_COLOR_VALUE in
  // order to keep r, g, and b in the range [0.0, 1.0].
  r = GetRValue(0x00110000);
  g = GetGValue(0x00110000);
  b = GetBValue(0x00110000);
  r /= 255.0f;
  g /= 255.0f;
  b /= 255.0f;
  sVertex[0].mDiffuse = D3DCOLOR_COLORVALUE(r, g, b, 1.0f);
  sVertex[3].mDiffuse = D3DCOLOR_COLORVALUE(r, g, b, 1.0f);

  for(i=0; i<4; i++)
    SetVertex(pVertices, sVertex[i]);

  return S_OK;
}

HRESULT IawBackground::SetupIndices()
{
  WORD* indices_array = new WORD[GetNumIndices()];

  indices_array[0] = 0; indices_array[1] = 1; indices_array[2] = 2;
  indices_array[3] = 0; indices_array[4] = 3; indices_array[5] = 1;

  SetIndexArray(indices_array);

  delete [] indices_array;

  return S_OK;
}



