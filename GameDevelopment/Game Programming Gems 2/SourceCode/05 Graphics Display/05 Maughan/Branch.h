/* Copyright (C) Chris Maughan, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Chris Maughan, 2001"
 */
#ifndef Branch_INCLUDED
#define Branch_INCLUDED

#include "BranchNoise.h"

struct Tree;
struct Branches;
struct Leaves;

struct Branch {

  Branch(const Tree&, Branches&, Leaves&, BranchNoise*);
  Branch(const Tree&,
	 Branches&,
	 Leaves&,
	 BranchNoise*,
	 const D3DXMATRIX&,
	 float texcoord_y,
	 float twist,
	 float decay,
	 int level);
  ~Branch();

  void build(const Tree&,
	     Branches&,
	     Leaves&,
	     BranchNoise*,
	     const D3DXMATRIX&,
	     float texcoord_y,
	     float twist,
	     float decay,
	     int level);

  int* lower;
  int* upper;

  Branch* left;
  Branch* right;

};

#endif
