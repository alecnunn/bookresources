// IawTerrainMgr.cpp App Wizard Version 2.0 Beta 1
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
// IawTerrainMgr.cpp: implementation of the CIawTerrainMgr class
//
// ----------------------------------------------------------------------
// Authors: Kim Pallister,Dean Macri - Intel Technology Diffusion Team
// ----------------------------------------------------------------------

#include "..\IawFrameWork.h"

// Constants
const int IawTerrainMgr::TERRAIN_MAX_HEIGHT       = 1000;
const int IawTerrainMgr::TERRAIN_NUM_TESSELATIONS = 16;

const int IawTerrainMgr::EARTH_RADIUS = 6500;
const int IawTerrainMgr::CLOUD_HEIGHT = 2500;
const int IawTerrainMgr::HORIZON      = 5000;

// Constructor
IawTerrainMgr::IawTerrainMgr(DWORD flags)
{
  mFVF = flags;
  
  for (int x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
  {
    for (int z = 0; z<TERRAIN_NUM_DIVISIONS;z++)
    { 
      mpTerrainArray[z][x] = NULL;
      mpRootObjectArray[z][x] = NULL;
    }
  }
  
  
  mTerrainShaderId = 0;
  mWaterShaderId = 0;
  
  mUseCulling = true;
  
  
  
  mMaxHeight = TERRAIN_MAX_HEIGHT;
  
  mCurrentUpdateNumH = -1;
  mCurrentUpdateNumV = -1;
  mShiftX = 0.0f;
  mShiftZ = 0.0f;
  
  mpWrapper             = NULL;
  mpShaderMgr           = NULL;
  //  mShaderId             = 0;
}

IawTerrainMgr::~IawTerrainMgr()
{
  for (int x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
  {
    for (int z = 0; z<TERRAIN_NUM_DIVISIONS;z++)
    { 
      SAFE_DELETE(mpRootObjectArray[z][x]);
    }
  }
}



HRESULT IawTerrainMgr::Init(IawD3dWrapper* pWrapper,
                            IawVector pos,
                            IawVector scale,
                            float horizondistance)
{
  mPos = pos;
  mAbsPos = pos;
  mScale = scale;
  mHorizon = horizondistance;

  mpWrapper = pWrapper;

  InitTerrainArray();

  return S_OK;
}

HRESULT IawTerrainMgr::Update(float frameTime, IawVector viewPos, IawVector viewDir)
{
  mViewPosition = viewPos;
  mViewDirection = viewDir;
  
  //need to decide if it's time to trash a row or column and add it to the other side
  //
  
  /////////////////////////////
  // x direction first  - if the view position is to the positive direction by more than half a grid division
  if (mCurrentUpdateNumV == -1)
  {
    if (mPos.mX < (-mHorizon/(float)TERRAIN_NUM_DIVISIONS)) 
    {
      mCurrentUpdateNumH++;
      if (TERRAIN_NUM_DIVISIONS == mCurrentUpdateNumH)
      {
        // Reset the counter
        mCurrentUpdateNumH = -1;
        
        // Update the position
        mPos.mX = mPos.mX + 2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
        mAbsPos.mX -= 2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
        mShiftX = 0;
        
        // Move all the terrain objects
        for (int z=0; z<TERRAIN_NUM_DIVISIONS;z++)
        {
          for (int x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
          {
            mpRootObjectArray[z][x]->SetPos(mpRootObjectArray[z][x]->GetPos() +
              IawVector(-2.0f * mHorizon/(float)TERRAIN_NUM_DIVISIONS,0,0));
          }
        }
      }
      else
      {
        IawObject* temp_object = NULL;
        IawTerrain* temp_terrain_object = NULL;
        
        // Update one patch
        temp_terrain_object = mpTerrainArray[mCurrentUpdateNumH][0];
        temp_object = mpRootObjectArray[mCurrentUpdateNumH][0];
        
        for (int x = 0; x<TERRAIN_NUM_DIVISIONS-1;x++)
        {
          mpRootObjectArray[mCurrentUpdateNumH][x] = mpRootObjectArray[mCurrentUpdateNumH][x+1];
          mpTerrainArray[mCurrentUpdateNumH][x] = mpTerrainArray[mCurrentUpdateNumH][x+1];
        }
        
        mpTerrainArray[mCurrentUpdateNumH][TERRAIN_NUM_DIVISIONS-1] = temp_terrain_object;
        mpRootObjectArray[mCurrentUpdateNumH][TERRAIN_NUM_DIVISIONS-1] = temp_object;
        
        mpTerrainArray[mCurrentUpdateNumH][TERRAIN_NUM_DIVISIONS-1]->mObserverPos.mX += 2.0f * mHorizon;
        mpTerrainArray[mCurrentUpdateNumH][TERRAIN_NUM_DIVISIONS-1]->SetUpdate(true);
        mpRootObjectArray[mCurrentUpdateNumH][TERRAIN_NUM_DIVISIONS-1]->SetPos(
          mpRootObjectArray[mCurrentUpdateNumH][TERRAIN_NUM_DIVISIONS-1]->GetPos() +
          IawVector(2.0f * mHorizon,0,0));
        if (mCurrentUpdateNumH >= TERRAIN_NUM_DIVISIONS/2)
          mShiftX = 2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
      }
      
    }
    else if (mPos.mX >  (mHorizon/(float)TERRAIN_NUM_DIVISIONS)) 
    {
      mCurrentUpdateNumH++;
      if (TERRAIN_NUM_DIVISIONS == mCurrentUpdateNumH)
      {
        // Reset the counter
        mCurrentUpdateNumH = -1;
        
        // Update the position
        mPos.mX = mPos.mX - 2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
        mAbsPos.mX += 2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
        mShiftX = 0;
        
        // Move all the terrain objects
        for (int z=0; z<TERRAIN_NUM_DIVISIONS;z++)
        {
          for (int x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
          {
            mpRootObjectArray[z][x]->SetPos(mpRootObjectArray[z][x]->GetPos() +
              IawVector(2.0f * mHorizon/(float)TERRAIN_NUM_DIVISIONS,0,0));
          }
        }
      }
      else
      {
        IawObject* temp_object = NULL;
        IawTerrain* temp_terrain_object = NULL;
        
        // Update one patch
        temp_terrain_object = mpTerrainArray[mCurrentUpdateNumH][TERRAIN_NUM_DIVISIONS-1];
        temp_object = mpRootObjectArray[mCurrentUpdateNumH][TERRAIN_NUM_DIVISIONS-1];
        
        for (int x = TERRAIN_NUM_DIVISIONS-1; x>0;x--)
        {
          mpRootObjectArray[mCurrentUpdateNumH][x] = mpRootObjectArray[mCurrentUpdateNumH][x-1];
          mpTerrainArray[mCurrentUpdateNumH][x] = mpTerrainArray[mCurrentUpdateNumH][x-1];
        }
        
        mpTerrainArray[mCurrentUpdateNumH][0] = temp_terrain_object;
        mpRootObjectArray[mCurrentUpdateNumH][0] = temp_object;
        
        mpTerrainArray[mCurrentUpdateNumH][0]->mObserverPos.mX -= 2.0f * mHorizon;
        mpTerrainArray[mCurrentUpdateNumH][0]->SetUpdate(true);
        mpRootObjectArray[mCurrentUpdateNumH][0]->SetPos(
          mpRootObjectArray[mCurrentUpdateNumH][0]->GetPos() - 
          IawVector(2.0f * mHorizon,0,0));
        
        mpRootObjectArray[mCurrentUpdateNumH][0]->SetPos(
          mpRootObjectArray[mCurrentUpdateNumH][0]->GetPos() +
          IawVector (-2.0f * mHorizon));
        
        if (mCurrentUpdateNumH >= TERRAIN_NUM_DIVISIONS/2)
          mShiftX = -2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
      }
    }
  }
  /////////////////////
  // now for Z
  if (mCurrentUpdateNumH == -1)
  {
    if (mPos.mZ <  (-mHorizon/TERRAIN_NUM_DIVISIONS)) 
    {
      mCurrentUpdateNumV++;
      if (TERRAIN_NUM_DIVISIONS == mCurrentUpdateNumV)
      {
        // Reset the counter
        mCurrentUpdateNumV = -1;
        
        // Update the position
        mPos.mZ = mPos.mZ + 2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
        mAbsPos.mZ -= 2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
        mShiftZ = 0;
        
        // Move all the terrain objects
        for (int z=0; z<TERRAIN_NUM_DIVISIONS;z++)
        {
          for (int x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
          {
            mpRootObjectArray[z][x]->SetPos(
              mpRootObjectArray[z][x]->GetPos() -
              IawVector(0,0, 2.0f * mHorizon/(float)TERRAIN_NUM_DIVISIONS));
          }
        }
      }
      else
      {
        IawObject *temp_object = NULL;
        IawTerrain *temp_terrain_object = NULL;
        
        // Update one patch
        temp_terrain_object = mpTerrainArray[0][mCurrentUpdateNumV];
        temp_object = mpRootObjectArray[0][mCurrentUpdateNumV];
        
        for (int z = 0; z<TERRAIN_NUM_DIVISIONS-1; z++)
        {
          mpRootObjectArray[z][mCurrentUpdateNumV] = mpRootObjectArray[z+1][mCurrentUpdateNumV];
          mpTerrainArray[z][mCurrentUpdateNumV] = mpTerrainArray[z+1][mCurrentUpdateNumV];
        }
        
        mpTerrainArray[TERRAIN_NUM_DIVISIONS-1][mCurrentUpdateNumV] = temp_terrain_object;
        mpRootObjectArray[TERRAIN_NUM_DIVISIONS-1][mCurrentUpdateNumV] = temp_object;
        
        mpTerrainArray[TERRAIN_NUM_DIVISIONS-1][mCurrentUpdateNumV]->mObserverPos.mZ += 2.0f * mHorizon;
        mpTerrainArray[TERRAIN_NUM_DIVISIONS-1][mCurrentUpdateNumV]->SetUpdate(true);
        mpRootObjectArray[TERRAIN_NUM_DIVISIONS-1][mCurrentUpdateNumV]->SetPos(
          mpRootObjectArray[TERRAIN_NUM_DIVISIONS-1][mCurrentUpdateNumV]->GetPos().mZ +
          IawVector(0,0,2.0f * mHorizon));
        if (mCurrentUpdateNumV >= TERRAIN_NUM_DIVISIONS/2)
          mShiftZ = 2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
      }
    }
    //now again for Z, but in the other direction
    else if (mPos.mZ >  (mHorizon/(float)TERRAIN_NUM_DIVISIONS)) 
    {
      mCurrentUpdateNumV++;
      if (TERRAIN_NUM_DIVISIONS == mCurrentUpdateNumV)
      {
        // Reset the counter
        mCurrentUpdateNumV = -1;
        
        // Update the position
        mPos.mZ = mPos.mZ - 2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
        mAbsPos.mZ += 2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
        mShiftZ = 0;
        
        // Move all the terrain objects
        for (int z=0; z<TERRAIN_NUM_DIVISIONS;z++)
        {
          for (int x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
          {
            mpRootObjectArray[z][x]->SetPos(
              mpRootObjectArray[z][x]->GetPos().mZ +
              IawVector(0,0,2.0f * mHorizon/(float)TERRAIN_NUM_DIVISIONS));
          }
        }
      }
      else
      {
        IawObject* temp_object = NULL;
        IawTerrain* temp_terrain_object = NULL;
        
        // Update one patch
        temp_terrain_object = mpTerrainArray[TERRAIN_NUM_DIVISIONS-1][mCurrentUpdateNumV];
        temp_object = mpRootObjectArray[TERRAIN_NUM_DIVISIONS-1][mCurrentUpdateNumV];
        
        for (int z = TERRAIN_NUM_DIVISIONS-1; z>0; z--)
        {
          mpRootObjectArray[z][mCurrentUpdateNumV] = mpRootObjectArray[z-1][mCurrentUpdateNumV];
          mpTerrainArray[z][mCurrentUpdateNumV] = mpTerrainArray[z-1][mCurrentUpdateNumV];
        }
        
        mpTerrainArray[0][mCurrentUpdateNumV] = temp_terrain_object;
        mpRootObjectArray[0][mCurrentUpdateNumV] = temp_object;
        
        mpTerrainArray[0][mCurrentUpdateNumV]->mObserverPos.mZ -= 2.0f * mHorizon;
        mpTerrainArray[0][mCurrentUpdateNumV]->SetUpdate(true);
        mpRootObjectArray[0][mCurrentUpdateNumV]->SetPos(
          mpRootObjectArray[0][mCurrentUpdateNumV]->GetPos() -
          IawVector(0,0,2.0f * mHorizon));
        
        if (mCurrentUpdateNumV >= TERRAIN_NUM_DIVISIONS/2)
          mShiftZ = -2.0f * (mHorizon/(float)TERRAIN_NUM_DIVISIONS);
      }
    }
  }
  
  //don't even need to do this first loop if there'll be no animation.
  for (int x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
  {
    for (int z = 0; z<TERRAIN_NUM_DIVISIONS;z++)
    {
      mpRootObjectArray[z][x]->Update(frameTime,true);        
    }
  }

  return S_OK;
}

HRESULT IawTerrainMgr::Render(IawMatrix worldMat)
{
  //still need to
  // do trivial reject
  // select appropriate LOD
  
  IawMatrix temp = worldMat;
  temp.Translate(mPos.mX,mPos.mY,mPos.mZ);
  temp.Scale(mScale.mX, mScale.mY, mScale.mZ);
  
  //  mpWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
  
  
  bool render_children = false;
  
  if (mUseCulling)
  {
    
    IawMatrix mx_rot_left(true);
    mx_rot_left.RotateY(-1.57f/2.0f);
    
    IawMatrix mx_rot_right(true);
    mx_rot_right.RotateY(1.57f/2.0f);
    
    IawVector view_normal_left = CrossProduct(Normalize(mViewDirection), IawVector(0,1,0));
    
    IawVector view_normal_right = view_normal_left*IawVector(-1.0f);
    
    view_normal_left = view_normal_left * mx_rot_left; 
    view_normal_right = view_normal_right * mx_rot_right; 
    
    
    IawVector normal_view_dir = Normalize(mViewDirection);
    
    float patchwidth = (mHorizon/TERRAIN_NUM_DIVISIONS);
    
    for (int x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
    {
      for (int z = 0; z<TERRAIN_NUM_DIVISIONS;z++)
      {
        //easy out for the four corners, since they are beyond the fog plane
        if (((0 == x) || (TERRAIN_NUM_DIVISIONS-1 == x)) &&
          ((0 == z) || (TERRAIN_NUM_DIVISIONS-1 == z)))
        {
          ;//do nothing, it's a corner
        }
        else
        {
          //do dot product of (corner of the patch - viewpoint) for all
          // four corners. Do this with both normals. If all 4 are negative
          // in either case, then don't draw the patch
          
          bool draw = true;
          
          
          //test four points. If all are outside the view frustum, don't draw the object at all
          IawVector vec1 = (mpRootObjectArray[z][x]->GetPos() + mPos) + IawVector(1.0f * patchwidth, 0.0f, 1.0f * patchwidth); 
          vec1 = Normalize(vec1);
          
          IawVector vec2 = (mpRootObjectArray[z][x]->GetPos() + mPos) + IawVector(1.0f * patchwidth, 0.0f, -1.0f * patchwidth); 
          vec2 = Normalize(vec2);
          
          IawVector vec3 = (mpRootObjectArray[z][x]->GetPos() + mPos) + IawVector(-1.0f * patchwidth, 0.0f, 1.0f * patchwidth); 
          vec3 = Normalize(vec3);
          
          IawVector vec4 = (mpRootObjectArray[z][x]->GetPos() + mPos) + IawVector(-1.0f * patchwidth, 0.0f, -1.0f * patchwidth); 
          vec4 = Normalize(vec4);
          
          float angle1 = DotProduct(view_normal_left, vec1);
          float angle2 = DotProduct(view_normal_left, vec2);
          float angle3 = DotProduct(view_normal_left, vec3);
          float angle4 = DotProduct(view_normal_left, vec4);
          
          if ((angle1 > 0.0f) && (angle2 > 0.0f) && 
            (angle3 > 0.0f) && (angle4 > 0.0f))
            draw = false;
          else
          {
            angle1 = DotProduct(view_normal_right, vec1);
            angle2 = DotProduct(view_normal_right, vec2);
            angle3 = DotProduct(view_normal_right, vec3);
            angle4 = DotProduct(view_normal_right, vec4);
            
            if ((angle1 > 0.0f) && (angle2 > 0.0f) && 
              (angle3 > 0.0f) && (angle4 > 0.0f))
              draw = false;
            else
            {
              IawMatrix temp2 = temp * mpRootObjectArray[z][x]->GetLocalToParentMx();
              mpRootObjectArray[z][x]->GetFirst()->Render(temp2, render_children);                  
            }
          }   
        }//end easy out
      } //for z
    } //forx

    //Need to go through the same loop and render the water. 
    //These needed to be rendered last because of the transparency

    for (x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
    {
      for (int z = 0; z<TERRAIN_NUM_DIVISIONS;z++)
      {
        //do dot product of (corner of the patch - viewpoint) for all
        // four corners. Do this with both normals. If all 4 are negative
        // in either case, then don't draw the patch
        
        bool draw = true;
        
        //test four points. If all are outside the view frustum, don't draw the object at all
        IawVector vec1 = (mpRootObjectArray[z][x]->GetPos() + mPos) + IawVector(1.0f * patchwidth, 0.0f, 1.0f * patchwidth); 
        vec1 = Normalize(vec1);
        
        IawVector vec2 = (mpRootObjectArray[z][x]->GetPos() + mPos) + IawVector(1.0f * patchwidth, 0.0f, -1.0f * patchwidth); 
        vec2 = Normalize(vec2);
        
        IawVector vec3 = (mpRootObjectArray[z][x]->GetPos() + mPos) + IawVector(-1.0f * patchwidth, 0.0f, 1.0f * patchwidth); 
        vec3 = Normalize(vec3);
        
        IawVector vec4 = (mpRootObjectArray[z][x]->GetPos() + mPos) + IawVector(-1.0f * patchwidth, 0.0f, -1.0f * patchwidth); 
        vec4 = Normalize(vec4);
        
        float angle1 = DotProduct(view_normal_left, vec1);
        float angle2 = DotProduct(view_normal_left, vec2);
        float angle3 = DotProduct(view_normal_left, vec3);
        float angle4 = DotProduct(view_normal_left, vec4);
        
        if ((angle1 > 0.0f) && (angle2 > 0.0f) && 
          (angle3 > 0.0f) && (angle4 > 0.0f))
          draw = false;
        else
        {
          angle1 = DotProduct(view_normal_right, vec1);
          angle2 = DotProduct(view_normal_right, vec2);
          angle3 = DotProduct(view_normal_right, vec3);
          angle4 = DotProduct(view_normal_right, vec4);
          
          if ((angle1 > 0.0f) && (angle2 > 0.0f) && 
            (angle3 > 0.0f) && (angle4 > 0.0f))
            draw = false;
          else
          {
            if (mpTerrainArray[z][x]->GetWater())
            {
              IawMatrix temp2 = temp * mpRootObjectArray[z][x]->GetLocalToParentMx();
              mpRootObjectArray[z][x]->GetFirst()->GetNext()->Render(temp2, render_children);
            }
          }
        }
      }
    }
    //done rendering water, need to reset fog color
    //mpWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_FOGCOLOR, m_dwFogColor);
  }//end if use culling
  else
  {
    for (int x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
    {
      for (int z = 0; z<TERRAIN_NUM_DIVISIONS;z++)
      {
        IawMatrix temp2 = temp * mpRootObjectArray[z][x]->GetLocalToParentMx();
        mpRootObjectArray[z][x]->GetFirst()->Render(temp2, render_children);      
      }
    }
    
    //Need to go through the same loop and render the water. 
    //These needed to be rendered last because of the transparency
    for (x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
    {
      for (int z = 0; z<TERRAIN_NUM_DIVISIONS;z++)
      {
        if (mpTerrainArray[z][x]->GetWater())
        {
          IawMatrix temp2 = temp * mpRootObjectArray[z][x]->GetLocalToParentMx();   
          mpRootObjectArray[z][x]->GetFirst()->GetNext()->Render(temp2, false);     
        }
      }
    }
    
    //done rendering water, need to reset fog color
    //mpWrapper->m_pD3DDevice7->SetRenderState(D3DRENDERSTATE_FOGCOLOR, m_dwFogColor);
  }

  return S_OK;
}


HRESULT IawTerrainMgr::TargetChanged(IawD3dWrapper* pWrapper, IawShaderMgr* pShaderMgr)
{
  HRESULT hr;
  mpWrapper = pWrapper;
  mpShaderMgr = pShaderMgr;


  for (int x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
  {
    for (int z = 0; z<TERRAIN_NUM_DIVISIONS;z++)
    {       
      hr = mpRootObjectArray[z][x]->TargetChanged(mpWrapper, mpShaderMgr, true);  //note: make TRUE if we attach stuff        
    }
  }

  
  for (x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
  {
    for (int z = 0; z<TERRAIN_NUM_DIVISIONS;z++)
    {     
      mpTerrainArray[z][x]->SetShaderID(mTerrainShaderId);      
    }
  }
  

  for (x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
  {
    for (int z = 0; z<TERRAIN_NUM_DIVISIONS;z++)
    {
      if (mpRootObjectArray[z][x]->GetFirst()->GetNext())
        mpRootObjectArray[z][x]->GetFirst()->GetNext()->SetShaderID(mWaterShaderId);
    }
  }
  
  
  return S_OK;

}


//////////  
//private:
//////////

HRESULT IawTerrainMgr::InitTerrainArray() //this should get a random number at some time
{
  IawVector scale = IawVector(2.0f*mHorizon/((float)TERRAIN_NUM_DIVISIONS), 1, 2.0f*mHorizon/((float)TERRAIN_NUM_DIVISIONS)); 

  float x_pos, z_pos;

  char str[64];

  for (int x = 0; x<TERRAIN_NUM_DIVISIONS;x++)
  {     
    x_pos = 2.0f*(float)x*mHorizon/(float)TERRAIN_NUM_DIVISIONS - mHorizon;

    for (int z = 0; z<TERRAIN_NUM_DIVISIONS;z++)
    {
      z_pos = 2.0f*(float)z*mHorizon/(float)TERRAIN_NUM_DIVISIONS - mHorizon;

      mpRootObjectArray[z][x] = new IawObject();
      mpTerrainArray[z][x] = new IawTerrain(mFVF, 0);

      mpRootObjectArray[z][x]->Init(mpWrapper, NULL, NULL, 0, 0, 0);
      mpRootObjectArray[z][x]->SetPos(IawVector(x_pos, 0.0f, z_pos));
//      mpRootObjectArray[z][x]->SetScale(IawVector(1.0f));

      sprintf(str,"Terrain Grid Root Object %2d %2d",x,z);
      mpRootObjectArray[z][x]->SetObjectName(str);
      sprintf(str,"Terrain Grid Terrain Object %2d %2d",x,z);
      mpTerrainArray[z][x]->SetObjectName(str);

      mpTerrainArray[z][x]->mObserverPos = mpRootObjectArray[z][x]->GetPos();
      mpTerrainArray[z][x]->SetMaxHeight(mMaxHeight);

      mpTerrainArray[z][x]->Init(mpWrapper, TERRAIN_NUM_TESSELATIONS);

        
      mpRootObjectArray[z][x]->AddChild(mpTerrainArray[z][x]);


      //add water and stir
      IawPlane  *temp_plane;
      temp_plane = new IawPlane;
      temp_plane->Init(mpWrapper, 1);
      temp_plane->SetPos(IawVector(0.0f, -0.3f, 0.0f));
      temp_plane->SetScale(IawVector((float)HORIZON/(float)TERRAIN_NUM_DIVISIONS, 1.0f, (float)HORIZON/(float)TERRAIN_NUM_DIVISIONS));
      temp_plane->SetRotationAxis(IawVector(1.0f,0.0f,0.0f));
      sprintf(str,"Terrain Grid Water Plane %2d %2d",x,z);
      temp_plane->SetObjectName(str);
      mpRootObjectArray[z][x]->AddChild(temp_plane);
    }
  }
  
  return S_OK;
}

