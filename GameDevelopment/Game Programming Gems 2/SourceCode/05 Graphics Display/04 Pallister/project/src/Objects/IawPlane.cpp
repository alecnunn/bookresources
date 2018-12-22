// IawPlane.cpp App Wizard Version 2.0 Beta 1
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
// IawPlane.cpp: definition of the Plane Object, derived from the IAWObject
// base class. Creates a grid of polys, tesselated to an arbitrary level
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constructor
IawPlane::IawPlane(DWORD flags) : IawObject(flags)
{
  SetObjectType(IAW_OBJECT_TYPE_PLANE);

  mUScale = 1.0f;
  mVScale = 1.0f;

  mUOffset = 0.0f;
  mVOffset = 0.0f;	 
}

// Destructor
IawPlane::~IawPlane()
{
  // void
}

// Initialize
HRESULT IawPlane::Init(IawD3dWrapper* pWrapper,
                       int divisions)
{
  int divs = divisions;
  if (divisions < 1)
    divs = 1;
  SetWrapper(pWrapper);

  SetNumVerts((divs + 1) * (divs + 1));
  SetNumIndices(divs * divs * 2 * 3);

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

  return S_OK;
}

void IawPlane::SetUVScale(float us, float vs)
{
	mUScale = us;
	mVScale = vs;	
}

void IawPlane::SetUVOffset(float uo, float vo)
{
	mUOffset = uo;
	mVOffset = vo;	
}


// Generate vertices that describe plane geometry
HRESULT IawPlane::SetupVertices(float* pVertices)
{
  if (NULL == pVertices)
    return E_FAIL;

  int count     = 0;
  int num_divisions = (int)sqrt(GetNumIndices()/6);

  

    float delta = 2.0f/(float)num_divisions;
  
  IAW_SUPERVERTEX SV;

  for (int i=0;i<=num_divisions;i++)
  {
    for (int j=0;j<=num_divisions;j++)
    {
	  if (GetFVF() & D3DFVF_XYZRHW) //if this is a TL vert
	  {
        SV.mX = (0.5f*((float)j*delta)) * GetScale().mX;
		SV.mY = (0.5f*((float)i*delta)) * GetScale().mZ;
		SV.mZ = 0.5f;//(-1.0f + ((float)i*delta)) * GetScale().mZ;
		SV.mRhw = 0.5f;
	  }
	  else
	  {
	    SV.mX = -1.0f + ((float)j*delta);
		SV.mY =  0.0f;
		SV.mZ = -1.0f + ((float)i*delta);
	  }

      SV.mNx = 0.0f;
      SV.mNy = 1.0f;
      SV.mNz = 0.0f;

      SV.mU = ((float)j * delta*0.5f) * mUScale;
      SV.mV = (1.0f - (float)i * delta*0.5f) * mVScale;

	  SV.mU += mUOffset;
      SV.mV += mVOffset;

	  //for this demo only
	  SV.mDiffuse = GetDiffuse();

      SetVertex(pVertices, SV);
	   
    }
  }
  OutputDebugString("\n");

  return S_OK;
}

// Set up indexed vertex indices for using DrawIndexedPrimitive(VB/Strided)
HRESULT IawPlane::SetupIndices()
{
  int num_divisions = (int)sqrt(GetNumIndices()/6);
  int index     = 0;
  int start_vertex   = 0;
  WORD* indices_array  = new WORD[GetNumIndices()];

  for (int i=0;i<num_divisions;i++)
  {
    for (int j=0;j<num_divisions;j++)
    {
      start_vertex = (i*(num_divisions+1) + j);
	  if (GetFVF() & D3DFVF_XYZRHW) //if this is a TL vert
	  {
	  indices_array[index++] = start_vertex; 
	    indices_array[index++] = start_vertex+1; 
      indices_array[index++] = start_vertex+num_divisions+1; 

      // tri 2
      indices_array[index++] = start_vertex+1;
	  indices_array[index++] = start_vertex+num_divisions+2;
      indices_array[index++] = start_vertex+num_divisions+1;
      }
	  else
	  {

      // tri 1
      indices_array[index++] = start_vertex; 
      indices_array[index++] = start_vertex+num_divisions+1; 
      indices_array[index++] = start_vertex+1; 

      // tri 2
      indices_array[index++] = start_vertex+1;
      indices_array[index++] = start_vertex+num_divisions+1;
      indices_array[index++] = start_vertex+num_divisions+2;
	  }
    }
  }

  SetIndexArray(indices_array);

  delete [] indices_array;

  return S_OK;
}

