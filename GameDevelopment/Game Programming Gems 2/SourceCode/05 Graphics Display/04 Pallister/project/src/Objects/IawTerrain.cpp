// IawTerrain.cpp App Wizard Version 2.0 Beta 1
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
// IawTerrain.cpp: implementation of the CIawTerrain class
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constructor
IawTerrain::IawTerrain(DWORD flags, long seed) : IawObject(flags)
{
  SetObjectType(IAW_OBJECT_TYPE_TERRAIN);

  // added from terrainobject code
  mNoiseGenerator = new IawExponentialNoise(seed);
  mpHeightField = NULL;

  mMaxHeight = 100.0f; //this is just a default

  mObserverPos = IawVector(0, 0, 0);
  mTerrainPos = IawVector(0, 0, 0);
  // need to change this stuff
  mTerrainScale = 8.0f;

  mUpdate = true;

  mScale2 = IawVector(1.0);

#ifdef DRAWNORMALS
  mpNormalVb = NULL; 
  mpNormalVerts = NULL;
#endif
}

// Destructor
IawTerrain::~IawTerrain() 
{
#ifdef DRAWNORMALS
  SAFE_RELEASE(mpNormalVb);
  
#endif
}

// Initialize
HRESULT IawTerrain::Init(IawD3dWrapper* pWrapper,
                         int divisions)
{
  int divs = divisions;
  if (divisions < 1)
    divs = 1;

  mGridDim = divs+1;
  mNumDivisions = divs;

  SetWrapper(pWrapper);

  mScale2 = mMaxHeight;

  SetNumVerts((divs + 1) * (divs + 1));
  SetNumIndices(divs * divs * 2 * 3);

  AllocVertMem(GetNumVerts() * (GetVertexSize() / 4));
  AllocIndicesMem(GetNumIndices());

  int height_field_vertices = (mGridDim+2) * (mGridDim+2);
  mpHeightField = new float[height_field_vertices];
  memset(mpHeightField, 0, height_field_vertices * sizeof(float));

#ifdef DRAWNORMALS
  mpNormalVerts = new NormalVert[2*height_field_vertices];
  memset(mpNormalVerts, 0, height_field_vertices * sizeof(NormalVert) * 2);
#endif

  mTerrainPos = mObserverPos * (mTerrainScale / mScale2.mX);
  GenerateTerrain();

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

// Generate terrain
HRESULT IawTerrain::GenerateTerrain()
{
  float scale;

  scale = mTerrainScale + (float)2.0 * mTerrainScale / (float)(mGridDim-1);

  mNoiseGenerator->BuildMap(mTerrainPos.mX,
                            mTerrainPos.mY,
                            mTerrainPos.mZ,
                            scale,
                            1.0,
                            scale,
                            mGridDim + 2,
                            mGridDim + 2,
                            mpHeightField);

  return S_OK;
}



// Generate vertices that describe plane geometry
HRESULT IawTerrain::SetupVertices(float* pVertices)
{
  if (NULL == pVertices)
    return E_FAIL;

  int count     = 0;
  int num_divisions = (int)sqrt(GetNumIndices()/6);

  float delta = 2.0f/(float)num_divisions;
  float delta_x = 1.0f/(float)mNumDivisions;
  float delta_z = 1.0f/(float)mNumDivisions;

  //end stuff needed for drawing normals

  float y;

  IAW_SUPERVERTEX SV;

  // Some temp vars used to calculate normals at each vertex
  int idx, idx2;
  D3DVECTOR p1, p2, p3, p4, p5, p6, p7;
  D3DVECTOR n1, n2, n3, n4, n5, n6, n;

  // Because the spacing is uniform in X and Z, we can set up these components of
  // the temp vectors before we go into the loop
  p1.x = 0.0f;
  p1.z = -1.0f * delta_z * mScale2.mZ;

  p2.x = delta_x * mScale2.mX;
  p2.z = -1.0f * delta_z * mScale2.mZ;

  p3.x = -1.0f * delta_x * mScale2.mX;
  p3.z = 0.0f;

  p4.x = 0.0f;
  p4.z = 0.0f;

  p5.x = delta_x * mScale2.mX;
  p5.z = 0.0f;

  p6.x = -1.0f * delta_x * mScale2.mX;
  p6.z = delta_z * mScale2.mZ;

  p7.x = 0.0f;
  p7.z = delta_z * mScale2.mZ;

  for (int i=0;i<=num_divisions;i++)
  {
    for (int j=0;j<=num_divisions;j++)
    {
      y = mpHeightField[(i+1)*(mGridDim+2)+j+1];// * mMaxHeight;

      SV.mX = (-0.5f + ((float)j*delta_x)) * mScale2.mX; 
      SV.mY =  (float)y * mScale2.mY;
      SV.mZ = (-0.5f + ((float)i*delta_z)) * mScale2.mZ;

      //if (y</*m_WaterLevel*/WATERALTITUDE * MAX_HEIGHT)
      //    m_bHasWater = true;
      SV.mNx = 0.0f;
      SV.mNy = 1.0f;
      SV.mNz = 0.0f;

      SV.mV = 0.8f+ (float)y/(1.0f/*MAX_HEIGHT*/);

      if (SV.mV < (1.0f/(float)128))
        SV.mV = (1.0f/(float)128);

      if (SV.mV > 1.0f-(1.0f/(float)128))
        SV.mV = 1.0f-(1.0f/(float)128);

	  SV.mU = 0.2f;

      SV.mU1 = (float)j * delta_x * 10.0f/*NOISETEXTURETILEFACTOR*/;
      SV.mV1 = 1.0f - (float)i * delta_z * 10.0f/*NOISETEXTURETILEFACTOR*/; 


      // Update normals
      idx = (i+1)*(mGridDim+2)+j+1;
      idx2 = i * mGridDim + j;

      p1.y = (float)mpHeightField[idx-mGridDim-2] * mScale2.mY;
      p2.y = (float)mpHeightField[idx-mGridDim-1] * mScale2.mY;
      p3.y = (float)mpHeightField[idx-1] * mScale2.mY;
      p4.y = (float)mpHeightField[idx] * mScale2.mY;
      p5.y = (float)mpHeightField[idx+1] * mScale2.mY;
      p6.y = (float)mpHeightField[idx+mGridDim+1] * mScale2.mY;
      p7.y = (float)mpHeightField[idx+mGridDim+2] * mScale2.mY;

      n1 = Normalize(CrossProduct(p3 - p4, p1 - p4));
      n2 = Normalize(CrossProduct(p1 - p4, p2 - p4));
      n3 = Normalize(CrossProduct(p2 - p4, p5 - p4));
      n4 = Normalize(CrossProduct(p6 - p4, p3 - p4));
      n5 = Normalize(CrossProduct(p7 - p4, p6 - p4));
      n6 = Normalize(CrossProduct(p5 - p4, p7 - p4));

      n = Normalize((n1+n2+n3+n4+n5+n6));

      SV.mNx = -n.x;
      SV.mNy = -n.y;
      SV.mNz = -n.z;

	  //second tex coord based on slope
	  SV.mU = 1.0f-DotProduct(IawVector(0.0f,1.0f,0.0f),IawVector(-n.x, -n.y, -n.z));

	  if (SV.mU < (1.0f/(float)128))
        SV.mU = (1.0f/(float)128);

      if (SV.mU > 1.0f-(1.0f/(float)128))
        SV.mU = 1.0f-(1.0f/(float)128);

#ifdef DRAWNORMALS
      // Divide down the size of the normal to make it reasonable to view
      float div = 1.0f/(float)mGridDim;

      mpNormalVerts[(i*mGridDim+j)*2].x = SV.m_x;
      mpNormalVerts[(i*mGridDim+j)*2].y = SV.m_y;
      mpNormalVerts[(i*mGridDim+j)*2].z = SV.m_z;
      mpNormalVerts[(i*mGridDim+j)*2].color = 0xff00ff00;

      mpNormalVerts[(i*mGridDim+j)*2+1].x = SV.m_x + (SV.m_nx * div);
      mpNormalVerts[(i*mGridDim+j)*2+1].y = SV.m_y + (SV.m_ny * div);
      mpNormalVerts[(i*mGridDim+j)*2+1].z = SV.m_z + (SV.m_nz * div);
      mpNormalVerts[(i*mGridDim+j)*2+1].color = 0xffff00ff;
#endif

      SetVertex(pVertices, SV);
    }
  }

  return S_OK;
}

// Set up indexed vertex indices for using DrawIndexedPrimitive(VB/Strided)
HRESULT IawTerrain::SetupIndices()
{
  int num_divisions = (int)sqrt(GetNumIndices()/6);
  int index = 0;
  int start_vertex = 0;
  WORD* indices_array = new WORD[GetNumIndices()];

  for (int i=0;i<num_divisions;i++)
  {
    for (int j=0;j<num_divisions;j++)
    {
      start_vertex = (i*(num_divisions+1) + j);

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

  SetIndexArray(indices_array);

  delete [] indices_array;

  return S_OK;
}

#ifdef DRAWNORMALS

HRESULT IawTerrain::Render(IawMatrix& rWorldMat, bool renderChildren)
{
  HRESULT hr = S_OK;

  //Setting the material and matrix here is not the most efficient way to do it (redundant calls
  // while stepping down hierarchy) but gets around some issues with the .x file hierarchy 
  // we build right now. It's at least functional, and will be improved in a later version of 
  // the appwizard

  // Set the world matrix to the local matrix
  SetLocalToWorldMx(rWorldMat, false); //mLocalToWorldMx =  rWorldMat * mLocalToParentMx ;

  int render_passes = 0;

  /*this needs to be replaced with the new stuff*/
  IawShader* temp_shader = GetShader();


    if (GetNumVerts() && GetWrapper() && GetWrapper()->m_pDevice && GetVisible())
    {
      GetWrapper()->m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

      GetWrapper()->m_pDevice->SetTransform(D3DTS_WORLD, &GetLocalToWorldMx().m_Mx);

      // Draw the object
      // the first 0 is for where to start in the VB. For now, we are assuming one VB per object
      GetWrapper()->m_pDevice->SetStreamSource(0, mpNormalVb, sizeof(NormalVert));

      GetWrapper()->m_pDevice->SetVertexShader(D3DFVF_LVERTEX);

      //placeholder
      int num_prims = GetNumVerts();

      hr = GetWrapper()->m_pDevice->DrawPrimitive(D3DPT_LINELIST,
                                                  0,
                                                  num_prims);
      GetWrapper()->m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    }
    //restore world matrix
    GetWrapper()->m_pDevice->SetTransform(D3DTS_WORLD, &rWorldMat.m_Mx);

    return IawObject::Render(rWorldMat, renderChildren);
}


HRESULT IawTerrain::TargetChanged(IawD3dWrapper* pWrapper, bool updateChildren)
{
  HRESULT hr;

  SAFE_RELEASE(mpNormalVb);

  //DWORD temp_flags = 0 ;
  DWORD temp_flags = D3DUSAGE_WRITEONLY;

  hr = GetWrapper()->mpDevice->CreateVertexBuffer(sizeof(NormalVert)*GetNumVerts()*2,
                                                  temp_flags,
                                                  D3DFVF_LVERTEX,
                                                  D3DPOOL_DEFAULT,
                                                  &mpNormalVb);
  if (!FAILED(hr))
  {
    NormalVert* temp_normal_verts;

    if (!FAILED(hr = mpNormalVb->Lock(0,
                                      sizeof(NormalVert)*GetNumVerts()*2,
                                      (unsigned char**)&temp_normal_verts, 
                                      NULL)))
      memcpy(temp_normal_verts, mpNormalVerts, sizeof(NormalVert)*GetNumVerts()*2);
    
    hr = mpNormalVb->Unlock();
  }
  //  return hr;
  //call the base class's TargetChanged function
  return IawObject::TargetChanged(pWrapper, updateChildren);
}
#endif // DRAWNORMALS

