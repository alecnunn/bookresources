// IawCylinder.cpp App Wizard Version 2.0 Beta 1
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
// IawCylinder.cpp: implementation of the CIawCylinder class
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constructor
IawCylinder::IawCylinder(DWORD flags) : IawObject(flags)
{
  SetObjectType(IAW_OBJECT_TYPE_CYLINDER);
}

// Destructor
IawCylinder::~IawCylinder()
{
  // void
}

// Initialize
HRESULT IawCylinder::Init(IawD3dWrapper* pWrapper,
                          float baseRadius,
                          float height,
                          int heightDiv,
                          int radiusDiv,
                          float verticalAngle,
                          float horizontalAngle)
/*
                          IawVector    pos,
                          IawVector    scale,
                          IawVector    rotationAxis,
                          float        rotationAngle,
                          IawVector    velocity,
                          IawVector    scaleVelocity,
                          float        rotationalVel,
                          DWORD        flags)
*/
{
  mHeightDiv = heightDiv;
  mRadiusDiv = radiusDiv;
  mHeight    = height;
  mRadius    = baseRadius;
  mXyAngle   = verticalAngle;
  mXzAngle   = horizontalAngle;
/*
  SetRotationAxis(rotationAxis);
  SetRotationAngle(rotationAngle);
  SetScale(scale);
  SetPos(pos);
  SetRotationalVelocity(rotationalVel);
  SetScalingVelocity(scaleVelocity);
  SetVelocity(velocity);
*/
  SetWrapper(pWrapper);

  SetNumVerts(((heightDiv + 1) * radiusDiv) + (radiusDiv * 2));
  SetNumIndices((6 * heightDiv * radiusDiv) + ((radiusDiv-2) * 6));

  // Multiply the size of one vertex by 24 and allocate memory for cube
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

// Generate vertices that describe cylinder geometry
HRESULT IawCylinder::SetupVertices(float* pVertices)
{
  if (NULL == pVertices)
    return E_FAIL;

  IAW_SUPERVERTEX SV;

  // theta is the current latitudinal angle
  float theta  = 0.0f;
  float d_theta = 2.0f * PI / mRadiusDiv;
  float dh     = mHeight / mHeightDiv;

  float temp_x, temp_y, temp_z;

  float center = 0.0f;
  float diameter = 2 * mRadius;

  int count = 0;

  // Iterates mHeightDiv+1 times because we need an extra vertex at each
  // latitude to map a texture completely around the surface for a particular
  // value of i
  for (int i = 0; i<mHeightDiv+1; i++)
  {
    theta  = 0.0f;

    for (int j=0; j<mRadiusDiv; j++)
    {
      temp_x = (float)sin(mXyAngle) * center + (float)cos(mXyAngle) * ((float)cos(theta) * diameter);
      temp_y = (float)cos(mXyAngle) * center - (float)sin(mXyAngle) * ((float)cos(theta) * diameter);
      temp_z = (float)sin(theta) * diameter;

      SV.mX = (float)cos(mXzAngle) * temp_x - (float)sin(mXzAngle) * temp_z;
      SV.mZ = (float)sin(mXzAngle) * temp_x + (float)cos(mXzAngle) * temp_z;
      SV.mY =  temp_y;

      SV.mNx = SV.mX;
      SV.mNy = 0.0f;
      SV.mNz = (float)sin(theta) * diameter;

      SV.mU = (float)j/mHeightDiv;
      SV.mV = 1.0f - (float)i/mRadiusDiv;

      SetVertex(pVertices, SV);

      theta += d_theta;

      count += (GetVertexSize() / 4);
    }

    center = center + dh;
  }

  theta = 0.0f;

  // Initialize the top of the cylinder
  for (i=0; i<mRadiusDiv; i++)
  {
    theta += d_theta;

    temp_x = (float)sin(mXyAngle) * center + (float)cos(mXyAngle) * ((float)cos(theta) * diameter);
    temp_z = (float)sin(theta) * diameter;

    SV.mX = (float)cos(mXzAngle) * temp_x - (float)sin(mXzAngle) * temp_z;
    SV.mY = temp_y;
    SV.mZ = (float)sin(mXzAngle) * temp_x + (float)cos(mXzAngle) * temp_z;

    SV.mNx = 0.0f;
    SV.mNy = 1.0f;
    SV.mNz = 0.0f;

    SV.mU = (float)i / mRadiusDiv;
    SV.mV = 0.0f;

    SetVertex(pVertices, SV);

    count += (GetVertexSize() / 4);
  }

  theta = 0.0f;

  // Initialize the base of the cylinder
  for (i=0; i<mRadiusDiv; i++)
  {
    theta += d_theta;

    temp_x = (float)cos(mXyAngle) * ((float)cos(theta) * diameter);
    temp_z = (float)sin(theta) * diameter;

    SV.mX = (float)cos(mXzAngle) * temp_x - (float)sin(mXzAngle) * temp_z;
    SV.mY = 0;
    SV.mZ = (float)sin(mXzAngle) * temp_x + (float)cos(mXzAngle) * temp_z;

    SV.mNx = 0.0f;
    SV.mNy = -1.0f;
    SV.mNz = 0.0f;

    SV.mU = (float)i / mRadiusDiv;
    SV.mV = 0.0f;

    SetVertex(pVertices, SV);

    count += (GetVertexSize() / 4);
  }

  return S_OK;
}

// Set up indexed vertex indices for using DrawIndexedPrimitive(VB/Strided)
HRESULT IawCylinder::SetupIndices()
{
  int i, j;
  int current_vertex = 0;
  WORD* indices_array = new WORD[GetNumIndices()];

  for (i=0;i<mHeightDiv;i++)
  {
    for (j=0;j<mRadiusDiv;j++)
    {
      indices_array[current_vertex++] =  i * mRadiusDiv + j;
      indices_array[current_vertex++] = (i+1) * mRadiusDiv + j;
      indices_array[current_vertex++] =  i * mRadiusDiv + (j+1) % mRadiusDiv;

      indices_array[current_vertex++] =  i * mRadiusDiv + (j+1) % mRadiusDiv;
      indices_array[current_vertex++] = (i+1) * mRadiusDiv + j;
      indices_array[current_vertex++] = (i+1) * mRadiusDiv + (j+1) % mRadiusDiv;
    }
  }

  int top_index = ((mHeightDiv + 1) * mRadiusDiv);

  // Top of cylinder
  for (i=0; i < (mRadiusDiv - 2); i++)
  {
    indices_array[current_vertex++] = top_index;
    indices_array[current_vertex++] = i + top_index + 2;
    indices_array[current_vertex++] = i + top_index + 1;
  }

  int baseIndex = top_index + mRadiusDiv;

  // Base of cylinder
  for (i=0; i < (mRadiusDiv - 2); i++)
  {
    indices_array[current_vertex++] = baseIndex;
    indices_array[current_vertex++] = i + baseIndex + 1;
    indices_array[current_vertex++] = i + baseIndex + 2;	
  }

  SetIndexArray(indices_array);

  delete [] indices_array;

  return S_OK;
}

