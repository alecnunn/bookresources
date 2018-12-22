/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#include "nvinc.h"
#include "Tree.h"

#include <math.h>

using namespace nv_objects;

static const double PI = 3.141592653589;

void Tree::setBarkTexture(NVResourceID textureID) {
  bark_textureID = textureID;
  geometry_dirty = true;
}

void Tree::setLeafTexture(NVResourceID textureID) {
  leaf_textureID = textureID;
  geometry_dirty = true;
}

void Tree::setWire(bool w) {

  wireframe_flag = w;
}

void Tree::setLeafBackTexture(NVResourceID textureID) {
  leaf_back_textureID = textureID;
  geometry_dirty = true;
}


void Tree::newCharacter() {

  delete bn;
  bn = new BranchNoise();
  geometry_dirty = true;
}

Tree::Tree()
  : depth(31),
    bn(new BranchNoise()),
    leaf_threshhold(0.07f),
    branch_angle_bias(1.0f), 
    branch_twist(30.0f),
    branch_noise(0.0f),
    leaves(0),
    branches(0),
    leaf_size(1.0f),
	branch_segments(20.0f),
    branch_radius(0.12f),
    wireframe_flag(false),
    geometry_dirty(true) 
{
}


void Tree::build() {

  if (geometry_dirty) {

    geometry_dirty = false;

    delete leaves;
    leaves = new Leaves(leaf_textureID, leaf_back_textureID, 7, leaf_size);
    delete branches;
    branches = new Branches(bark_textureID, branch_segments);

    Branch branch(*this, *branches, *leaves, bn);
  }
}


Tree::~Tree() {

  delete leaves;
  delete branches;
  delete bn;
}



void Tree::Transfer(NVComplexMesh* pMesh) {

	build();
	pMesh->Release();
	pMesh->AddPass(0);

	NVComplexMeshVertexData* pPosition = pMesh->AddVertexData("position", NVCOMPLEXMESH_FLOAT3);
	NVComplexMeshVertexData* pNormal = pMesh->AddVertexData("normal", NVCOMPLEXMESH_FLOAT3);
	NVComplexMeshVertexData* pTexture = pMesh->AddVertexData("texture0", NVCOMPLEXMESH_FLOAT2);
	NVASSERT(pPosition && pNormal && pTexture, "No data in mesh");

	leaves->Transfer(pMesh);
	branches->Transfer(pMesh);

	
	pMesh->SetNumVertices(pPosition->GetVec3Array().size());
	pMesh->SetNumIndices(pMesh->GetIndexArray().size());
}


void Tree::setLeafSize(const float& r) {

  leaf_size = r;
  geometry_dirty = true;
}


void Tree::setBranchSize(const float& r) {

  branch_radius = r;
  geometry_dirty = true;
}


void Tree::setThreshhold(const float& a) {
  leaf_threshhold = a;
  geometry_dirty = true;
}

void Tree::setBranchNoise(const float& n) {
  branch_noise = n;
  geometry_dirty = true;
}


void Tree::setBranchBias(const float& a) {
  branch_angle_bias = a;
  geometry_dirty = true;
}


void Tree::setBranch(const float& a) {

  branch_angle = a;
  geometry_dirty = true;
}

void Tree::setBranchSegments(const int& a) {

  branch_segments = a;
  geometry_dirty = true;
}


void Tree::setTwist(const float& a) {
  branch_twist = a;
  geometry_dirty = true;
}
