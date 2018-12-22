/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#ifndef Tree_INCLUDED
#define Tree_INCLUDED

#include "Branch.h"
#include "Leaves.h"
#include "Branches.h"

struct Tree {

  Tree();
  ~Tree();

  void Transfer(nv_objects::NVComplexMesh* pMesh);
  void build();

  void newCharacter();

  void setWire(bool);
  void setBarkTexture(nv_objects::NVResourceID TextureID);
  void setLeafTexture(nv_objects::NVResourceID TextureID);
  void setLeafBackTexture(nv_objects::NVResourceID TextureID);

  void setLeafSize(const float&);
  void setBranchSize(const float&);
  void setBranchNoise(const float&);
  void setThreshhold(const float&);
  void setBranchBias(const float&);
  void setBranch(const float&); 
  void setTwist(const float&);
  void setBranchSegments(const int& a);

  int depth;
  int branch_facets;
  float branch_radius;

  nv_objects::NVResourceID leaf_textureID;
  nv_objects::NVResourceID leaf_back_textureID;
  nv_objects::NVResourceID bark_textureID;

  float leaf_size;
  float branch_size;
  float branch_noise;      // between 0 and 1
  float branch_angle_bias; // between 0 and 1
  float branch_angle; // between 0 and 1
  float branch_twist; // between 0 and 180
  float leaf_threshhold;
  DWORD branch_segments; // Number of segments around the branch

  bool wireframe_flag;

  bool geometry_dirty;

  Leaves* leaves;
  Branches* branches;
  BranchNoise* bn; // per branch data structure
};


#endif

