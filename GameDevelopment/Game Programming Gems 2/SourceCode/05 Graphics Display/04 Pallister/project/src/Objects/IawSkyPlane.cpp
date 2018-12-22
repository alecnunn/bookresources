// IawSkyPlane.cpp App Wizard Version 2.0 Beta 1
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
// IawSkyPlane.cpp.cpp: definition of the Plane Object, derived from the IAWObject base class.
// This class is used to contain a Plane object, whose verts are 'pulled down over
// a sphere' to match the curvature. Use this for sky dome's, but easier to texture
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constructor
IawSkyPlane::IawSkyPlane(DWORD flags) : IawObject(flags)
{
  SetObjectType(IAW_OBJECT_TYPE_SKYPLANE);

  mPlanetRadius = 0.0f;
  mAtmosphereRadius = 0.0f;
  mHTileFactor = 1.0f;
  mVTileFactor = 1.0f;
}

// Destructor
IawSkyPlane::~IawSkyPlane()
{
  // void
}

// Initialize
HRESULT IawSkyPlane::Init(IawD3dWrapper* pWrapper,
                          int divisions,
                          float planetRadius,
                          float atmosphereRadius,
                          float hTilefactor,
                          float vTilefactor)
{
  int divs = divisions;
  if (divisions < 1) 
    divs = 1;

  if (divisions > 256) 
    divs = 256; //64k verts is the max size for a VB 


  mPlanetRadius = planetRadius;
  mAtmosphereRadius = atmosphereRadius;
  mHTileFactor = hTilefactor;
  mVTileFactor = vTilefactor;

  SetWrapper(pWrapper);

  SetNumVerts((divs + 1) * (divs + 1));   // 1 division would give 4 verts
  SetNumIndices(divs * divs * 2 * 3);   // 1 division would give 6 indices for 2 tris

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

// Generate vertices that describe a sky plane
HRESULT IawSkyPlane::SetupVertices(float *pVertices)
{
  if(pVertices == NULL) return E_FAIL;

  //since we don't store the number of divisions in the plane, we can just get it back
  int num_divisions = (int)sqrt(GetNumIndices()/6);

  float plane_size = 2.0f * (float)sqrt((mAtmosphereRadius*mAtmosphereRadius) - 
                     (mPlanetRadius*mPlanetRadius));

  float delta = plane_size/(float)num_divisions;
  float tex_delta = 2.0f/(float)num_divisions;
  float x_dist   = 0.0f;
  float z_dist   = 0.0f;
  float x_height = 0.0f;
  float z_height = 0.0f;
  float height = 0.0f;

  int count = 0;

  IAW_SUPERVERTEX SV;

  for (int i=0;i<=num_divisions;i++)
  {
    for (int j=0;j<=num_divisions;j++)
    {
      x_dist = (-0.5f * plane_size) + ((float)j*delta);
      z_dist = (-0.5f * plane_size) + ((float)i*delta);

      x_height = (x_dist*x_dist) / mAtmosphereRadius;
      z_height = (z_dist*z_dist) / mAtmosphereRadius;
      height = x_height + z_height;

      SV.mX = x_dist;
      SV.mY = 0.0f - height;
      SV.mZ = z_dist;

      SV.mNx = 0.0f;
      SV.mNy = -1.0f; //assume pointing downward
      SV.mNz = 0.0f;

      SV.mU = mHTileFactor*((float)j * tex_delta*0.5f);
      SV.mV = mVTileFactor*(1.0f - (float)i * tex_delta*0.5f);

      SetVertex(pVertices, SV);
    }
  }

  return S_OK;
}

// Set up indexed vertex indices for using DrawIndexedPrimitive(VB/Strided)
HRESULT IawSkyPlane::SetupIndices()
{
  int num_divisions = (int)sqrt(GetNumIndices()/6);
  int index = 0;
  WORD* indices_array = new WORD[GetNumIndices()];

  for (int i=0;i<num_divisions;i++)
  {
    for (int j=0;j<num_divisions;j++)
    {
      int startvert = (i*(num_divisions+1) + j);

      // tri 1
      indices_array[index++] = startvert;
      indices_array[index++] = startvert+1;
      indices_array[index++] = startvert+num_divisions+1;

      // tri 2
      indices_array[index++] = startvert+1;
      indices_array[index++] = startvert+num_divisions+2;
      indices_array[index++] = startvert+num_divisions+1;
    }
  }

  SetIndexArray(indices_array);

  delete [] indices_array;

  return S_OK;
}

