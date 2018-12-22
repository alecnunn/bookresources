// IawCube.cpp App Wizard Version 2.0 Beta 1
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
// IawCube.cpp: definition of the Cube Object, derived from the IAWObject base
// class. 
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constructor
IawCube::IawCube(DWORD flags) : IawObject(flags)
{
  SetObjectType(IAW_OBJECT_TYPE_CUBE);
  SetFVF(flags);
}

// Destructor
IawCube::~IawCube()
{
  // void
}

// Initialize
HRESULT IawCube::Init(IawD3dWrapper* pWrapper)
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

  SetNumVerts(24);    // 6 faces * 4 vertices per face
  SetNumIndices(36);  // 6 faces * 2 triangles * 3 indices per triangle

  // Multiply the size of one vertex by 24 and allocate memory for cube
  AllocVertMem(GetVertexSize() * 6 /* * 4 */); //because getvertex is in floats, but alloc is in bytes, we div by four
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


// Generate vertices that describe cube geometry
HRESULT IawCube::SetupVertices(float* pVertices)
{
  if (NULL == pVertices)
    return E_FAIL;

  // RHW
  float rhw = 0.00001f;

  // All potential 5 blending weight values all default to equal b1
  float b1 = 0.0f;

  // Define the normals for the cube
  IawVector n0( 0.0f, 0.0f,-1.0f); // Front face
  IawVector n1( 0.0f, 0.0f, 1.0f); // Back face
  IawVector n2( 0.0f, 1.0f, 0.0f); // Top face
  IawVector n3( 0.0f,-1.0f, 0.0f); // Bottom face
  IawVector n4( 1.0f, 0.0f, 0.0f); // Right face
  IawVector n5(-1.0f, 0.0f, 0.0f); // Left face

  // Diffuse color
  D3DCOLOR diffuse = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);

  // Specular color
  D3DCOLOR specular = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);

  // All 8*4 possible texture coordinate sets are set to be equal to u, v, s,
  // and t, respectively.
  // Front face
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f, 1.0f,-1.0f), n0, 0.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector( 1.0f, 1.0f,-1.0f), n0, 1.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f,-1.0f,-1.0f), n0, 0.0f, 1.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector( 1.0f,-1.0f,-1.0f), n0, 1.0f, 1.0f));

  // Back face
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f, 1.0f, 1.0f), n1, 1.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f,-1.0f, 1.0f), n1, 1.0f, 1.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector( 1.0f, 1.0f, 1.0f), n1, 0.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector( 1.0f,-1.0f, 1.0f), n1, 0.0f, 1.0f));

  // Top face
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f, 1.0f, 1.0f), n2, 0.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector( 1.0f, 1.0f, 1.0f), n2, 1.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f, 1.0f,-1.0f), n2, 0.0f, 1.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector( 1.0f, 1.0f,-1.0f), n2, 1.0f, 1.0f));

  // Bottom face
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f,-1.0f, 1.0f), n3, 0.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f,-1.0f,-1.0f), n3, 0.0f, 1.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector( 1.0f,-1.0f, 1.0f), n3, 1.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector( 1.0f,-1.0f,-1.0f), n3, 1.0f, 1.0f));

  // Right face
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(1.0f, 1.0f,-1.0f), n4, 0.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(1.0f, 1.0f, 1.0f), n4, 1.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(1.0f,-1.0f,-1.0f), n4, 0.0f, 1.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(1.0f,-1.0f, 1.0f), n4, 1.0f, 1.0f));

  // Left face
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f, 1.0f,-1.0f), n5, 1.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f,-1.0f,-1.0f), n5, 1.0f, 1.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f, 1.0f, 1.0f), n5, 0.0f, 0.0f));
  SetVertex(pVertices, IAW_SUPERVERTEX(IawVector(-1.0f,-1.0f, 1.0f), n5, 0.0f, 1.0f));

  return S_OK;
}


// Set up indexed vertex indices for using DrawIndexedPrimitive(VB/Strided)
HRESULT IawCube::SetupIndices()
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

