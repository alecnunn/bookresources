// IawCone.cpp App Wizard Version 2.0 Beta 1
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
// IawCone.cpp: definition of  the Cone Object, derived from the IawObject base class.
// This class is used to contain a Cone object, it's vertices, matrices, etc.
// The class stores two copies of the vertices, an array of D3DVerts, and a Vertex
// buffer used for rendering. At the time this class was authored, it was unclear
// whether the array was needed in case of a lost vertex buffer (DX7 beta docs unclear)
// so there may be some memory savings to be made there.
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constructor
IawCone::IawCone(DWORD flags) : IawObject(flags)
{
  SetObjectType(IAW_OBJECT_TYPE_CONE);
}

// Destructor
IawCone::~IawCone()
{
  // empty
}

// Initialize
HRESULT IawCone::Init(IawD3dWrapper* pWrapper,
                      float height,
                      float radius,
                      int div)
{
  // vertex and index counters
  if (div < 3)
    div = 3;
  mNumDiv = div;
  mRadius = radius;
  mHeight = height;

  //ptr to the wrapper
  SetWrapper(pWrapper);

  SetNumVerts(mNumDiv * 3);
  SetNumIndices((3 * mNumDiv) + ((mNumDiv -2) * 3));

  AllocVertMem(GetNumVerts() * (GetVertexSize() / 4));
  AllocIndicesMem(GetNumIndices());

  //if we are being fed a bunch of verts, then we need to set them up
  if (SetupVertices(GetVerticesPtr()) != S_OK)
  {
    OutputDebugString("unable to set up verts");
    return E_FAIL;
  }

  if (SetupIndices() != S_OK)
  {
    OutputDebugString("unable to set up indices");
    return E_FAIL;
  }

  if (SetupIndexedVertexBuffer() != S_OK)
  {
    OutputDebugString("unable to set up VB");
    return E_FAIL;
  }

  return S_OK;
}

// Generate vertices that describe cone geometry
HRESULT IawCone::SetupVertices(float* pVertices)
{
  if (NULL == pVertices)
    return E_FAIL;

  // theta is the current latitudinal angle
  float theta = PI / 2.0f;

  // delta values for the angles depending
  // on the number of divisions
  float d_theta = 2.0f*PI / mNumDiv;

  IAW_SUPERVERTEX SV;

  int current_vertex = 0;
  float x, y, z, nx, ny, nz;
  float* p_temp = NULL;

  float angle = (float)atan(0.5f*mRadius / mHeight);

  // Initialize the vertices at the top of the Cone
  for (int i=0; i<mNumDiv; i++)
  {
    theta += d_theta;
    x = (float)cos(theta);
    y = (float)sin(theta);

    // We need multiple vertices at the top position in order
    // to correctly map a texture to the Cone
    SV.mX = 0;
    SV.mY = mHeight;
    SV.mZ = 0;

    SV.mNx = x*(float)cos(angle);
    SV.mNy = (float)sin(angle); //0.0f
    SV.mNz = y*(float)cos(angle); //This isn't quit correct. Should depends on radius (upward tilt). to be fixed later

    SV.mU = (float)i / mNumDiv;
    SV.mV = 0.0f;

    SetVertex(pVertices, SV);

    current_vertex += (GetVertexSize() / 4);
  }

  //reset this
  theta = PI / 2.0f;

  // Intialize the vertices that make up the cone base
  for (i = 0; i<mNumDiv; i++)
  {
    theta += d_theta;

    x = (float)cos(theta);// * (float)cos(phi);
    y = 0.0f - mHeight;
    z = (float)sin(theta);

    nx = x * (float)cos(angle);
    ny = (float)sin(angle); //0.0f
    nz = y * (float)cos(angle); //This isn't quit correct. Should depends on radius (upward tilt). to be fixed later

    IawVector temp = mRadius * IawVector(x,0,z);
    temp.mY = y;

    SV = IAW_SUPERVERTEX(temp, IawVector(nx, ny, nz), (float)i/mNumDiv, 1.0f);
    SetVertex(pVertices, SV);
    SV = IAW_SUPERVERTEX(temp, IawVector(0, -1, 0), (float)i/mNumDiv, 1.0f);
    p_temp = &GetVerticesPtr()[(mNumDiv*(GetVertexSize() / 4) + current_vertex)];
    SetVertex(p_temp, SV);

    current_vertex += (GetVertexSize() / 4);
  }

  return S_OK;
}


// Set up indexed vertex indices for using DrawIndexedPrimitive(VB/Strided)
HRESULT IawCone::SetupIndices()
{
  int i;
  int current_vertex = 0;
  WORD* indices_array = new WORD[GetNumIndices()];

  // Top section
  for (i=0; i<mNumDiv; i++)
  {
    indices_array[current_vertex++] = i;

    // Skip past the vertices at the top of the Cone (+mNumDiv)
    // and also skip to the next vertice (+1)
    indices_array[current_vertex++] = i+mNumDiv+1;

    indices_array[current_vertex++] = i+mNumDiv;
  }

  indices_array[current_vertex-2] = mNumDiv;

  for (i=0; i<(mNumDiv-2); i++)
  {
    indices_array[current_vertex++] = 2* mNumDiv;

    // Skip past the vertices at the top of the Cone (+mNumDiv)
    // and also skip to the next vertice (+1)
    indices_array[current_vertex++] = i+2*mNumDiv+1;

    indices_array[current_vertex++] = i+2*mNumDiv+2;
  }

  SetIndexArray(indices_array);

  delete [] indices_array;

  return S_OK;
}

