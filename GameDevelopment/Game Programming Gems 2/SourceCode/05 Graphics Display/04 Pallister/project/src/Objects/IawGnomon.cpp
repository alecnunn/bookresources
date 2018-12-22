// IawGnomon.cpp App Wizard Version 2.0 Beta 1
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
// Declaration of the CIawGnomon class.
// This class is used to contain an XYZ axis, stored and drawn as a series of line
// segments.
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constructor
IawGnomon::IawGnomon(DWORD flags) : IawObject(flags)
{
  SetObjectType(IAW_OBJECT_TYPE_GNOMON);

  SetNumVerts(22);
  SetNumIndices(22);

  SetPrimType(D3DPT_LINELIST);
}

// Destructor
IawGnomon::~IawGnomon() { }

// Initialize
HRESULT IawGnomon::Init(IawD3dWrapper* pWrapper)
{
  SetWrapper(pWrapper);

  AllocVertMem(22 * (GetVertexSize() / 4));
  AllocIndicesMem(22);

  // If we are being fed a bunch of verts, then we need to set them up
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

// Generate vertices that describe XYZ axis
HRESULT IawGnomon::SetupVertices(float* pVertices)
{
  if (NULL == pVertices)
    return E_FAIL;

  IAW_SUPERVERTEX SV[22];

  SV[0].mX = 0.0f; SV[0].mY = 0.0f; SV[0].mZ = 0.0f; //line along x axis
  SV[0].mDiffuse = D3DCOLOR_COLORVALUE(1.0f, 0.0f, 0.0f ,1.0f);
  SV[0].mSpecular= D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f ,1.0f);
  SV[1].mX = 1.0f; SV[1].mY = 0.0f; SV[1].mZ = 0.0f;
  SV[1].mDiffuse = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f ,1.0f);
  SV[1].mSpecular= D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f ,1.0f);

  SV[2].mX = 0.0f; SV[2].mY = 0.0f; SV[2].mZ = 0.0f; //line along y axis
  SV[2].mDiffuse = D3DCOLOR_COLORVALUE(0.0f, 1.0f, 0.0f ,1.0f);
  SV[2].mSpecular= D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f ,1.0f);
  SV[3].mX = 0.0f; SV[3].mY = 1.0f; SV[3].mZ = 0.0f;
  SV[3].mDiffuse = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f ,1.0f);
  SV[3].mSpecular= D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f ,1.0f);

  SV[4].mX = 0.0f; SV[4].mY = 0.0f; SV[4].mZ = 0.0f; //line along z axis
  SV[4].mDiffuse = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 1.0f ,1.0f);
  SV[4].mSpecular= D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f ,1.0f);
  SV[5].mX = 0.0f; SV[5].mY = 0.0f; SV[5].mZ = 1.0f;
  SV[5].mDiffuse = D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f ,1.0f);
  SV[5].mSpecular= D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f ,1.0f);

  //letter X
  SV[6].mX = 1.1f; SV[6].mY = 0.0f; SV[6].mZ = 0.0f;
  SV[7].mX = 1.2f; SV[7].mY = 0.2f; SV[7].mZ = 0.0f;
  SV[8].mX = 1.2f; SV[8].mY = 0.0f; SV[8].mZ = 0.0f;
  SV[9].mX = 1.1f; SV[9].mY = 0.2f; SV[9].mZ = 0.0f;

  //letter Y
  SV[10].mX = 0.0f;  SV[10].mY = 1.1f;  SV[10].mZ = 0.0f;
  SV[11].mX = 0.0f;  SV[11].mY = 1.2f;  SV[11].mZ = 0.0f;
  SV[12].mX = 0.0f;  SV[12].mY = 1.2f;  SV[12].mZ = 0.0f;
  SV[13].mX = 0.1f;  SV[13].mY = 1.3f;  SV[13].mZ = 0.0f;
  SV[14].mX = 0.0f;  SV[14].mY = 1.2f;  SV[14].mZ = 0.0f;
  SV[15].mX = -0.1f; SV[15].mY = 1.3f;  SV[15].mZ = 0.0f;

  //letter Z
  SV[16].mX = 0.1f;  SV[16].mY = 0.2f;  SV[16].mZ = 1.1f;
  SV[17].mX = 0.2f;  SV[17].mY = 0.2f;  SV[17].mZ = 1.1f;
  SV[18].mX = 0.2f;  SV[18].mY = 0.2f;  SV[18].mZ = 1.1f;
  SV[19].mX = 0.1f;  SV[19].mY = 0.0f;  SV[19].mZ = 1.1f;
  SV[20].mX = 0.1f;  SV[20].mY = 0.0f;  SV[20].mZ = 1.1f;
  SV[21].mX = 0.2f;  SV[21].mY = 0.0f;  SV[21].mZ = 1.1f;

  // color elements for letters
  for (int i = 6; i < 22; i++)
  {
    SV[i].mDiffuse = 0xffffffff; // White
    SV[i].mSpecular= 0;
  }

  // texture coordinates
  for (i = 0; i < 11; i++)
  {
    SV[ i ].mU  = 0.0f;
    SV[ i ].mV  = 0.0f;
    SV[i+1].mU  = 1.0f;
    SV[i+1].mV  = 1.0f;
  }

  for (i=0; i<22; i++)
    SetVertex(pVertices, SV[i]);

  return S_OK;
}

// Set up indexed vertex indices for using DrawIndexedPrimitive(VB/Strided)
HRESULT IawGnomon::SetupIndices()
{
  WORD indices_array[22] = { 0 };
  for (int i=0; i<22; i++)
  {
    indices_array[i] = i;
  }
  SetIndexArray(indices_array);

  return S_OK;
}



//  This draws the object to the screen using DrawIndexedPrimitiveVB.
HRESULT IawGnomon::Render(IawMatrix& rWorldMat, bool renderChildren)
{
  HRESULT hr;

  // Set the world matrix to the local matrix
  SetWorldMxToLocalMx(rWorldMat);

  //if the object has children, and we want to render them, now is the time
  if (renderChildren)
  {
    IawObject* pTempObject = GetFirst();
    while (pTempObject != NULL)
    {
      hr |= pTempObject->Render(GetLocalToWorldMx(), true);
      pTempObject = pTempObject->GetNext();
    }
  }

  if (GetWrapper() && GetWrapper()->mpDevice  && GetVisible())
  {
    GetWrapper()->mpDevice->SetTransform(D3DTS_WORLD, GetLocalToWorldD3DMx());
    GetWrapper()->mpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    GetWrapper()->mpDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    GetWrapper()->mpDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    // Draw the object

    GetWrapper()->mpDevice->SetStreamSource(0, GetVBptr(), GetVertexSize());

    GetWrapper()->mpDevice->SetVertexShader(GetFVF());
    GetWrapper()->mpDevice->SetIndices(GetIBptr(), 0L);

    //placeholder
    int numprims = GetNumIndices();
    switch (GetPrimType())
    {
    case D3DPT_TRIANGLELIST:
      numprims/=3;
      break;
    case D3DPT_LINELIST:
      numprims/=2;
      break;
    }

    hr = GetWrapper()->mpDevice->DrawIndexedPrimitive(GetPrimType(),
                                                      0,
                                                      GetNumVerts(),//m_iNumIndices,
                                                      0,
                                                      numprims);

    GetWrapper()->mpDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
  }

  GetWrapper()->mpDevice->SetTransform(D3DTS_WORLD, &rWorldMat.mMx);

  return S_OK;
}


