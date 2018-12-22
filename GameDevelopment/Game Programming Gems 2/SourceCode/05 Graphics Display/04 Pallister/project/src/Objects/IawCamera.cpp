// IawCamera.cpp App Wizard Version 2.0 Beta 1
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
// IawCamera.cpp.cpp: definition of the Camera Object, derived from the IAWObject base
// class. THIS CLASS ISN'T 100% FUNCTIONAL AS OF BETA 1.
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constructor
IawCamera::IawCamera(DWORD flags) : IawObject(flags)
{
  SetObjectType(IAW_OBJECT_TYPE_CAMERA);

  mViewMat.SetIdentity();
  mProjectionMat.SetIdentity();

  mViewMat.SetView(0, 2, IawVector(0, 1, 0));
  //set the projection matrix to 1.57 field of view (radians), 1.33 aspect ratio 
  // (640x480), //the near clip plane to 1 and the far to clip plane 100
  mProjectionMat.SetProjection(1.57f, 1.333f, 1.0f, 50.0f);

  mViewDirection = IawVector(0.0f, 0.0f, 1.0f);
}

// Destructor
IawCamera::~IawCamera()
{
  // empty
}


// Initialize
HRESULT IawCamera::Init(IawD3dWrapper* pWrapper)
{

  //ptr to the wrapper
  SetWrapper(pWrapper);

  SetNumVerts(24);    // 6 faces * 4 vertices per face
  SetNumIndices(36);  // 6 faces * 2 triangles * 3 indices per triangle

  // Multiply the size of one vertex by 24 and allocate memory for Camera
  AllocVertMem(GetVertexSize() * 6 ); //because getvertex() is in floats, but alloc is in bytes, we div 24 by 4 to get 6
  AllocIndicesMem(36);

  // Set up vertices, indices, indexed vertex buffer
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

// Generate vertices that describe Camera geometry. Note that the geometry
//  is only used if we are not viewing from the camera, but rather viewing 
//  it from outside. It would be possible to add geometry that is visible 
//  from within the camera, such as for a HUD, crosshair, etc.
HRESULT IawCamera::SetupVertices(float* pVertices)
{
  if(NULL == pVertices)
    return E_FAIL;

  float Q = mProjectionMat.mMx._33;
  float QZn = -mProjectionMat.mMx._43;
  float w = mProjectionMat.mMx._11;
  float h = mProjectionMat.mMx._22;
  float nearClip = QZn / Q;
  float farClip = -QZn / (1-Q);
  float fFOVw, fFOVh;
  float frontw, fronth, backw, backh;
  IawVector corner[8];
  IawVector view, up, right, pos;
  IawMatrix mx_view = mViewMat;

  mViewMat.Invert();
  //	mx_view.PostMultiply(*mViewMat);
  right.mX = mViewMat.mMx._11;
  right.mY = mViewMat.mMx._21;
  right.mZ = mViewMat.mMx._31;
  right = Normalize(right);
  up.mX = mViewMat.mMx._12;
  up.mY = mViewMat.mMx._22;
  up.mZ = mViewMat.mMx._32;
  up = Normalize(up);
  view.mX = mViewMat.mMx._13;
  view.mY = mViewMat.mMx._23;
  view.mZ = mViewMat.mMx._33;
  view = Normalize(view);
  pos.mX = mx_view.mMx._41;
  pos.mY = mx_view.mMx._42;
  pos.mZ = mx_view.mMx._43;

  fFOVw = (float)atan(1) / w;
  fFOVh = (float)atan(1) / h;

  farClip *= 0.04f;

  frontw = (float)tan(fFOVw) * nearClip;
  fronth = (float)tan(fFOVh) * nearClip;
  backw = (float)tan(fFOVw) * farClip;
  backh = (float)tan(fFOVh) * farClip;

  //
  // Compute the 8 corners
  //
  corner[0] = pos + view * nearClip - right * frontw + up * fronth;
  corner[1] = pos + view * nearClip + right * frontw + up * fronth;
  corner[2] = pos + view * nearClip - right * frontw - up * fronth;
  corner[3] = pos + view * nearClip + right * frontw - up * fronth;

  corner[4] = pos + view * farClip - right * backw + up * backh;
  corner[5] = pos + view * farClip + right * backw + up * backh;
  corner[6] = pos + view * farClip - right * backw - up * backh;
  corner[7] = pos + view * farClip + right * backw - up * backh;



  IAW_SUPERVERTEX	SV;

  SV.mX = corner[0].mX; SV.mY = corner[0].mY; SV.mZ = corner[0].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = -1.0f;
  SV.mU = 0.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[1].mX; SV.mY = corner[1].mY; SV.mZ = corner[1].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = -1.0f;
  SV.mU = 1.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[2].mX; SV.mY = corner[2].mY; SV.mZ = corner[2].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = -1.0f;
  SV.mU = 0.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[3].mX; SV.mY = corner[3].mY; SV.mZ = corner[3].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = -1.0f;
  SV.mU = 1.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);


  ////////////
  // left plane (x = -1)

  SV.mX = corner[4].mX; SV.mY = corner[4].mY; SV.mZ = corner[4].mZ;
  SV.mNx = -1.0f; SV.mNy = 0.0f; SV.mNz = 0.0f;
  SV.mU = 0.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[0].mX; SV.mY = corner[0].mY; SV.mZ = corner[0].mZ;
  SV.mNx = -1.0f; SV.mNy = 0.0f; SV.mNz = 0.0f;
  SV.mU = 1.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[6].mX; SV.mY = corner[6].mY; SV.mZ = corner[6].mZ;
  SV.mNx = -1.0f; SV.mNy = 0.0f; SV.mNz = 0.0f;
  SV.mU = 0.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[2].mX; SV.mY = corner[2].mY; SV.mZ = corner[2].mZ;
  SV.mNx = -1.0f; SV.mNy = 0.0f; SV.mNz = 0.0f;
  SV.mU = 1.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);

  ////////////
  // front plane

  SV.mX = corner[5].mX; SV.mY = corner[5].mY; SV.mZ = corner[5].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = 1.0f;
  SV.mU = 0.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[4].mX; SV.mY = corner[4].mY; SV.mZ = corner[4].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = 1.0f;
  SV.mU = 1.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[7].mX; SV.mY = corner[7].mY; SV.mZ = corner[7].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = 1.0f;
  SV.mU = 0.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[6].mX; SV.mY = corner[6].mY; SV.mZ = corner[6].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = 1.0f;
  SV.mU = 1.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);


  ////////////
  // right plane

  SV.mX = corner[1].mX; SV.mY = corner[1].mY; SV.mZ = corner[1].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = 1.0f;
  SV.mU = 0.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[5].mX; SV.mY = corner[5].mY; SV.mZ = corner[5].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = 1.0f;
  SV.mU = 1.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[3].mX; SV.mY = corner[3].mY; SV.mZ = corner[3].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = 1.0f;
  SV.mU = 0.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[7].mX; SV.mY = corner[7].mY; SV.mZ = corner[7].mZ;
  SV.mNx = 0.0f; SV.mNy = 0.0f; SV.mNz = 1.0f;
  SV.mU = 1.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);

  ////////////
  // top plane

  SV.mX = corner[4].mX; SV.mY = corner[4].mY; SV.mZ = corner[4].mZ;
  SV.mNx = 0.0f; SV.mNy = 1.0f; SV.mNz = 0.0f;
  SV.mU = 0.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[5].mX; SV.mY = corner[5].mY; SV.mZ = corner[5].mZ;
  SV.mNx = 0.0f; SV.mNy = 1.0f; SV.mNz = 0.0f;
  SV.mU = 1.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[0].mX; SV.mY = corner[0].mY; SV.mZ = corner[0].mZ;
  SV.mNx = 0.0f; SV.mNy = 1.0f; SV.mNz = 0.0f;
  SV.mU = 0.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[1].mX; SV.mY = corner[1].mY; SV.mZ = corner[1].mZ;
  SV.mNx = 0.0f; SV.mNy = 1.0f; SV.mNz = 0.0f;
  SV.mU = 1.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);

  ////////////
  // bottom plane

  SV.mX = corner[2].mX; SV.mY = corner[2].mY; SV.mZ = corner[2].mZ;
  SV.mNx = 0.0f; SV.mNy = -1.0f; SV.mNz = 0.0f;
  SV.mU = 0.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[3].mX; SV.mY = corner[3].mY; SV.mZ = corner[3].mZ;
  SV.mNx = 0.0f; SV.mNy = -1.0f; SV.mNz = 0.0f;
  SV.mU = 1.0f; SV.mV = 0.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[6].mX; SV.mY = corner[6].mY; SV.mZ = corner[6].mZ;
  SV.mNx = 0.0f; SV.mNy = -1.0f; SV.mNz = 0.0f;
  SV.mU = 0.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);

  SV.mX = corner[7].mX; SV.mY = corner[7].mY; SV.mZ = corner[7].mZ;
  SV.mNx = 0.0f; SV.mNy = -1.0f; SV.mNz = 0.0f;
  SV.mU = 1.0f; SV.mV = 1.0f; 
  SetVertex(pVertices, SV);

  return S_OK;
}


// Set up indexed vertex indices for using DrawIndexedPrimitive(VB/Strided)
HRESULT IawCamera::SetupIndices()
{
  WORD* indices_array = new WORD[GetNumIndices()];

  for (int i=0;i<6;i++)
  {
    int j = i*6;
  
    indices_array[j+0] = i*4+0; indices_array[j+1] = i*4+1; indices_array[j+2] = i*4+2;
    indices_array[j+3] = i*4+2; indices_array[j+4] = i*4+1; indices_array[j+5] = i*4+3;
  }

  SetIndexArray(indices_array);

  delete [] indices_array;

  return S_OK;
}

