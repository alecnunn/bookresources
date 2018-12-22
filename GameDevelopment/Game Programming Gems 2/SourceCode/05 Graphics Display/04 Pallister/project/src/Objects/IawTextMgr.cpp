// IawTextMgr.cpp App Wizard Version 2.0 Beta 1
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
// IawTextMgr.cpp: implementation of the CIawTextMgr class
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constructor
IawTextMgr::IawTextMgr(DWORD dwFVF) : IawObject(dwFVF)
{
  SetObjectType(IAW_OBJECT_TYPE_TEXTMGR);
  SetFVF(dwFVF);
  
  SetObjectName("Text Manager");
  
  SetVertexSize();  // Set m_dwVertexSize based on new FVF flags
  
  mLastScreenHRes = 0;
  mLastScreenVRes = 0;
  
}

// Destructor
IawTextMgr::~IawTextMgr()
{ 
 
}

HRESULT IawTextMgr::Init(IawD3dWrapper* pWrapper, int shaderId)
{
  
  IawObject::Init(pWrapper, (char *)NULL);//IawVector,(NULL); //setup the default matrices and the like
  
  SetWrapper(pWrapper);
  SetShaderID(shaderId, false);
  SetVertexSize();
  
  return S_OK;
}

// Add a string
HRESULT IawTextMgr::AddString(char *pString, int* pStringId, float x, float y, float hsize, float vsize)
{
  //this is for tlverts only!
  if (!(D3DFVF_XYZRHW & GetFVF()))
    return E_FAIL;
  
  IawString* new_string = new IawString(GetFVF());
  
  new_string->Init(GetWrapper(), pString, pStringId,  x, y, hsize, vsize);
  
  new_string->SetShaderID(GetShaderID()); //set it's shader to the managers shader
  
  // Find the object in the list immediately preceeding the last object starting with "this"
  IawObject* temp = GetFirst();
  if (temp)
  {
    while (temp->GetNext() != NULL) temp = temp->GetNext();
    new_string->SetID(((IawString*)temp)->GetID()+1);// = temp->mStringId + 1;
  }
  //else new_string's ID will be 0, set in the strings constructor;

  *pStringId = new_string->GetID();
  
  AddChild(new_string);
  
  return S_OK;
}

HRESULT IawTextMgr::AddString(char* pString,
                              int* pStringId,
                              IawVector pos,
                              IawVector scale,
                              IawVector rotationAxis,
                              float rotationAngle,
                              IawVector velocity,
                              IawVector scaleVelocity,
                              float rotationalVelocity)
{

  //this is not for TL verts
  if (D3DFVF_XYZRHW & GetFVF())
    return E_FAIL;

  IawString *new_string = new IawString(GetFVF());

  new_string->Init(GetWrapper(),
                   pString,
                   pStringId,
                   pos,
                   scale,
                   rotationAxis,
                   rotationAngle,
                   velocity,
                   scaleVelocity,
                   rotationalVelocity);

  new_string->SetShaderID(GetShaderID()); //set it's shader to the managers shader

  // Find the object in the list immediately preceeding the last object starting with "this"
  IawObject* temp = GetFirst();
  if (temp)
  {
    while (temp->GetNext() != NULL) temp = temp->GetNext();
    ;//new_string->mStringId++;// = temp->mStringId + 1;
  }
  //else new_string's ID will be 0, set in the strings constructor;

  AddChild(new_string);

  return S_OK;
}

HRESULT IawTextMgr::ChangeStringContent(int stringID, char *pString)
{
  IawString *temp = (IawString *)GetFirst();
  
  while ((temp != NULL) && (temp->GetID() != stringID))
    temp = (IawString *)temp->GetNext();

  if (NULL == temp)
	  return E_FAIL;
  
  temp->ChangeContent(pString);
  
  temp->UpdateVB();
  
  
  //temp->SetShaderID(GetShaderID()); //set it's shader to the managers shader
  
  return S_OK;
  
}


HRESULT IawTextMgr::Render(IawMatrix& rWorldMat, bool renderChildren)
{
  if (GetVisible())
  {
	IawObject* temp = GetFirst();
  
	while (temp)
	{
		temp->Render(rWorldMat, false);
		temp = temp->GetNext();
	}
  }
  
  return S_OK;
  
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//
//   Begin implementation of string class
//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
IawString::IawString(DWORD dwFVF) : IawObject(dwFVF)
{
  SetObjectType(IAW_OBJECT_TYPE_STRING);

  mpString = NULL;
  mStringId = 0;
  mStringLen = 0;

  SetObjectName("String");

  //these are used if it's in screenspace
  mXpos = 0;
  mYpos = 0;
  mHsize = 0;
  mVsize = 0;
}

IawString::~IawString()
{
  SAFE_DELETE(mpString);
}

HRESULT IawString::Init(IawD3dWrapper *pWrapper,
                        char* pString,
                        int* pStringId,
                        IawVector pos,
                        IawVector scale,
                        IawVector rotationaxis,
                        float rotationangle,
                        IawVector vel ,
                        IawVector scalevel,
                        float   rotationalvel)
{
  SetWrapper(pWrapper);
//  IawObject::Init(NULL,pos,scale,rotationaxis,rotationangle,vel,scalevel,rotationalvel,0);
  SetupStringContents(pString, pos.mX, pos.mY, pos.mZ, scale.mX, scale.mY);
  return S_OK;
}

HRESULT IawString::Init(IawD3dWrapper *pWrapper,
                        char *pString,
                        int* stringID,
                        float x,
                        float y,
                        float hsize,
                        float vsize)
{
  SetWrapper(pWrapper);

  IawObject::Init(pWrapper, (float*)NULL);

  SetupStringContents(pString, x, y, 0.05f, hsize, vsize);

  return S_OK;
}

HRESULT IawString::ChangeContent(char* pString)
{
  SetupStringContents(pString, mXpos, mYpos, 0.05f, mHsize, mVsize);
  return S_OK;
}

void IawString::SetupStringContents(char *pString, float x, float y, float z, float hsize, float vsize)
{
  mStringLen = lstrlen(pString);

  SAFE_DELETE(mpString);

  mpString = new char[mStringLen+1];
  strcpy(mpString, pString);

  mXpos   = x;
  mYpos   = y;
  mHsize  = hsize;
  mVsize  = vsize;


  SetNumVerts(mStringLen * 4);
  SetNumIndices(mStringLen * 6);

  AllocVertMem(GetNumVerts() * (GetVertexSize() / 4));
  AllocIndicesMem(GetNumIndices());

  SetupVertices(GetVerticesPtr());
  SetupIndices();

}

HRESULT IawString::SetupVertices(float *pVertices)
{
  float x_start;
  float y_start;
  float char_width;
  float char_height;

  if (D3DFVF_XYZRHW & GetFVF())
  {
    x_start   = mXpos * (float)GetWrapper()->GetWidth();
    y_start   = mYpos * (float)GetWrapper()->GetHeight();;
    char_width  = mHsize * (float)GetWrapper()->GetWidth();
    char_height = mVsize * (float)GetWrapper()->GetHeight();
  }
  else
  {

    x_start   = mXpos;
    y_start   = mYpos;
    char_width  = mHsize;
    char_height = mVsize;
  }

  float char_offset = 1.0f/IawTexture::FONT_TEXT_CHARS_PER_ROW; //the fraction between 0&1 per char

  float u_start   = 0.0f;
  float v_start   = 0.0f;


  IAW_SUPERVERTEX curr_char[4];   // Each character in string contains 4 vertices

  for (int i = 0; i < mStringLen; i++) 
  {
    // Set (x,y) coordinates
    //NEED TO CHECK SCREEN EXTENTS!!! Assumes D3D will clip!

    // Top left
    curr_char[0].mX = (float)x_start + (i * char_width);
    curr_char[0].mY = (float)y_start;

    // Top right
    curr_char[1].mX = (float)x_start + (i * char_width) + char_width;
    curr_char[1].mY = (float)y_start;

    // Bottom left
    curr_char[2].mX = (float)x_start + (i * char_width);
    curr_char[2].mY = (float)y_start + char_height;

    // Bottom right
    curr_char[3].mX = (float)x_start + (i * char_width) + char_width;
    curr_char[3].mY = (float)y_start + char_height;


    for (int j = 0; j < 4; j++)
    {
      //note, the Z value may need to change if you operate out of this range
      curr_char[j].mZ     = 0.01f;
      curr_char[j].mRhw   = 1.0;
      curr_char[j].mSpecular  = 0;
    }

    curr_char[0].mDiffuse = IawTexture::TOP_COLOR;
    curr_char[1].mDiffuse = IawTexture::TOP_COLOR;
    curr_char[2].mDiffuse = IawTexture::BOTTOM_COLOR;
    curr_char[3].mDiffuse = IawTexture::BOTTOM_COLOR;

    //set the UV's into the font texture.
    u_start = (float)((int)mpString[i] % IawTexture::FONT_TEXT_CHARS_PER_SIDE) * char_offset;
    v_start = (float)((int)mpString[i] / IawTexture::FONT_TEXT_CHARS_PER_SIDE) * char_offset;

    // Top left
    curr_char[0].mU = u_start;
    curr_char[0].mV = 1.0f - (v_start);

    // Top right
    curr_char[1].mU = u_start + char_offset;
    curr_char[1].mV = 1.0f - (v_start);

    // Bottom left
    curr_char[2].mU = u_start;
    curr_char[2].mV = 1.0f - (v_start + char_offset);

    // Bottom right
    curr_char[3].mU = u_start + char_offset;
    curr_char[3].mV = 1.0f - (v_start + char_offset);


    for (j = 0; j < 4; j++)
      SetVertex(pVertices, curr_char[j]);
  }

  return S_OK;
}

// This is needed for the buggy MSVC compiler.  With optimizations on, the compiler
// spits out some bogus code -- Intel compiler doesn't have this problem
HRESULT IawString::SetupIndices()
{
  WORD *indices_array = new WORD[GetNumIndices()];
  int j;

  for (int i=0;i<GetNumIndices()/6;i++)
  {
    j = i*6;

    indices_array[j+0] = i*4+0;
    indices_array[j+1] = i*4+1; 
    indices_array[j+2] = i*4+2;
    indices_array[j+3] = i*4+1;
    indices_array[j+4] = i*4+3;
    indices_array[j+5] = i*4+2;
  }

  SetIndexArray(indices_array);

  delete [] indices_array;

  return S_OK;
}

