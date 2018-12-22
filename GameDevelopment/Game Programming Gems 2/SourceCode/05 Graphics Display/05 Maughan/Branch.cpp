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
#include "Branch.h"

#include "Tree.h"
#include "Branches.h"
#include "Leaves.h"

static float clamp(float fValue, const float fMin, const float fMax)
{
	if (fValue < fMin)
		fValue = fMin;
	if (fValue > fMax)
		fValue = fMax;
	return fValue;
}

Branch::Branch(const Tree& tree, 
	       Branches& branches,
	       Leaves& leaves,
	       BranchNoise* bn) : lower(0), upper(0), left(0), right(0) {

	D3DXMATRIX Identity;
	D3DXMatrixIdentity(&Identity);
  build(tree, branches, leaves, bn, Identity, 0.0f, 0.0f, 1.0f, 0);

  // build stump

  int facets = branches.facets;
  float radius = tree.branch_radius;

  for (int i=0; i<(facets+1); ++i) {

    float t = 1.0/facets*i;
    D3DXVECTOR2 g(branches.geom[i]);

    D3DXVECTOR3 n(g[0], 0.5f, g[1]);
    D3DXVECTOR3 v(g[0]*radius*1.5, -0.5f, g[1]*radius*1.5);
    D3DXVECTOR2 tc(t, -radius-0.5);

	branches.addIndex(branches.add(n,tc,v));
    branches.addIndex(lower[i]);
    
  }

}



Branch::Branch(const Tree& tree,
	       Branches& branches,
	       Leaves& leaves,
	       BranchNoise* bn,
	       const D3DXMATRIX& mat,
	       float texcoord_y,
	       float twist,
	       float decay,
	       int level) 
  : lower(0), upper(0), left(0), right(0) {

  build(tree, branches, leaves, bn, mat, texcoord_y, twist, decay, level);
}


void Branch::build(const Tree& tree,
		   Branches& branches,
		   Leaves& leaves,
		   BranchNoise* bn,
		   const D3DXMATRIX& mat,
		   float texcoord_y,
		   float twist,
		   float decay,
		   int level) {

  int facets = branches.facets;
  float radius = tree.branch_radius;

  lower = new int[facets+1];
  upper = new int[facets+1];


  float branch_angle = tree.branch_angle;
  float branch_angle_bias = tree.branch_angle_bias;
  float branch_twist = twist + tree.branch_twist;

  BranchNoise* left_bn = bn->getLeft();
  BranchNoise* right_bn = bn->getRight();

  float left_branch_noise = ((left_bn->branch_noise * 0.3) - 0.1)*tree.branch_noise;
  float right_branch_noise = ((right_bn->branch_noise * 0.3) - 0.1)*tree.branch_noise;

  float left_radius = sqrtf(1.0 - branch_angle)+left_branch_noise;
  left_radius = clamp(left_radius, 0.0f, 1.0f);
  float left_angle = branch_angle * 90.0 * branch_angle_bias;

  float right_radius = sqrtf(branch_angle)+right_branch_noise;
  right_radius = clamp(right_radius, 0.0f, 1.0f);
  float right_angle = (1.0 - branch_angle) * -90.0 * branch_angle_bias;

  float taper = max(left_radius, right_radius);

  /* build cylinder lower */

  for (int i=0; i<(facets+1); ++i) {

    float t = 1.0/facets*i;
    D3DXVECTOR2 g(branches.geom[i]);

	D3DXVECTOR3 n = D3DXVECTOR3(g[0], 0.0f, g[1]);
	D3DXVECTOR3 v = D3DXVECTOR3(g[0]*radius, radius, g[1]*radius);
    D3DXVECTOR2 tc(t, texcoord_y);

	D3DXVec3TransformNormal(&n, &n, &mat);
	D3DXVec3TransformCoord(&v, &v, &mat);

    lower[i] = branches.add(n,tc,v);
  }

  texcoord_y += 1.0-2*radius;

  /* build cylinder upper */

  for (i=0; i<(facets+1); ++i) {

    float t = 1.0/facets*i;
    D3DXVECTOR2 g(branches.geom[i]);

	D3DXVECTOR3 n = D3DXVECTOR3(g[0], 0.0f, g[1]);
	D3DXVECTOR3 v = D3DXVECTOR3(g[0]*radius*taper, 1.0f - radius, g[1]*radius*taper);
    D3DXVECTOR2 tc(t, texcoord_y);

	D3DXVec3TransformNormal(&n, &n, &mat);
	D3DXVec3TransformCoord(&v, &v, &mat);

    upper[i] = branches.add(n,tc,v);
  }

  texcoord_y += 2*radius;

  /* build matricies */

  D3DXMATRIX translate;
  D3DXMatrixTranslation(&translate, 0.0f, 1.0f, 0.0f);

  // CM - CHECKME
  //D3DXMATRIX translate_mat = mat * translate;
  D3DXMATRIX translate_mat = translate * mat;

  float s = sin(branch_twist);
  float c = cos(branch_twist);

  D3DXMATRIX scale, rotate;
  D3DXMatrixScaling(&scale, left_radius, left_radius, left_radius);

  D3DXVECTOR3 axis(s, 0.0f, c);
  D3DXMatrixRotationAxis(&rotate, &axis, D3DXToRadian(left_angle)) ;

  D3DXMATRIX left_mat = scale * rotate * translate_mat;
  //D3DXMATRIX left_mat = translate_mat * rotate * scale;
  

  float left_decay = left_radius*decay;

  if (((level+1)>=tree.depth) || (left_decay<tree.leaf_threshhold)) {

    leaves.addLeaf(left_mat);

  } else {

    left = new Branch(tree, branches, 
		      leaves, 
		      left_bn, 
		      left_mat, 
		      texcoord_y,
		      branch_twist,
		      left_decay,
		      level+1);
  }

  D3DXMatrixScaling(&scale, right_radius, right_radius, right_radius);

  D3DXMatrixRotationAxis(&rotate, &axis, D3DXToRadian(right_angle)) ;

  //D3DXMATRIX right_mat = translate_mat * rotate * scale;
  D3DXMATRIX right_mat = scale * rotate * translate_mat;
      
  float right_decay = right_radius*decay;

  if (((level+1)>=tree.depth) || (right_decay<tree.leaf_threshhold)) {

    leaves.addLeaf(right_mat);

  } else {
    
    right = new Branch(tree,
		       branches,
		       leaves,
		       right_bn,
		       right_mat,
		       texcoord_y,
		       branch_twist,
		       right_decay,
		       level+1);
  }

  for (i=0; i<facets+1; ++i) {

    branches.addIndex(lower[i]);
	branches.addIndex(upper[i]);
  }

  if (right) {

    for (i=0; i<facets+1; ++i) {

      branches.addIndex(upper[i]);
	  branches.addIndex(right->lower[i]);
    }
  }

  if (left) {

    for (i=0; i<facets+1; ++i) {

      branches.addIndex(upper[i]);
	  branches.addIndex(left->lower[i]);
    }
  }

}  

Branch::~Branch() {

  delete left;
  delete right;

  delete [] lower;
  delete [] upper;
}
