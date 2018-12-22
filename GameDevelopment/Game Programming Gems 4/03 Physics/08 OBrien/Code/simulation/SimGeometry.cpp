/*********************************************************************
 * SimGeometry.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * This file holds definitions for CGeometry
 * as well as CSimulation::___Penetration_Check()
 * methods
 *
 * Copyright 2003, Regents of the University of California 
 *
 *********************************************************************/

/**************************************************************************
 * Copyright (c) 2003, University of California at Berkeley
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the University of California nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/




#include "Simulation.h"
#include "tetCollision.h"
#include "math/geometry.h"

struct TetTetPack
{
  CBody* body;
  tet_ptr_element* tets;
  tet_index_element* tet_indices;
  mylist<CConstraint>* penetrations;
  int* n;
};

struct TetPlanePack {
  CBody* body;
  tet_ptr_element* tets;
  tet_index_element* tet_indices;
  mylist<CConstraint>* penetrations;
  int* n;
};

int tetTetOverlapCB(const BoundOverlapServer  &oe,
  const BoundTree::leafNode &indexOne, void *uprmOne,
  const BoundTree::leafNode &indexTwo, void *uprmTwo)
{
  TetTetPack *o1, *o2;
  int t1,t2;
  t1 = indexOne.mapIndex;
  t2 = indexTwo.mapIndex;
  o1 = (TetTetPack*)uprmOne;
  o2 = (TetTetPack*)uprmTwo;

  Vector3 a[4];
  Vector3 b[4];
  for(int i=0; i<4; i++) {
    a[i].set(o1->tets[t1][i]);
    b[i].set(o2->tets[t2][i]);
  }

  Real vol;
  Vector3 center, dir;
  computeTetTetOverlap(a[0],a[1],a[3],a[2],
		       b[0],b[1],b[3],b[2],
		       vol, center, dir);

  if(vol != 0.0) {
    //vol = fabs(vol);
    o1->penetrations->push_back();
    CConstraint* p = o1->penetrations->back();
    p->a = o1->body;
    p->b = o2->body;
    p->p = center;
    get_barycentric_coordinates(center, o1->tets[t1], p->p1.coords);
    get_barycentric_coordinates(center, o2->tets[t2], p->p2.coords);
    for(int i=0; i<4; i++) {
      p->p1.vert[i] = o1->tet_indices[t1][i];
      p->p2.vert[i] = o2->tet_indices[t2][i];
    }
    p->p1.n = p->p2.n = 4;
    p->constraint_type = CONSTRAINT_NORMAL | CONSTRAINT_PENETRATION;

    normalize(dir);
    p->fn.normal = dir;
    p->fn.offset = dot(dir,center);
    p->pen = vol;
    (*o1->n)++;
  }

  return 1;
}

int planeTetOverlapCB(const BoundOverlapServer  &        ,
			   const BoundTree::leafNode &index   ,
			   void                        *packVP  ,
			   const Plane              &plane) 
{
  TetPlanePack *pack = (TetPlanePack*)packVP ;

  int t = index.mapIndex;

  Vector3 a[4];
  for(int i=0; i<4; i++) {
    a[i].set(pack->tets[t][i]);

    if(isnan(a[i][0])) {
      printf("Tetrahedron %d\n", t);
      printf("ACK! bad tetrahedron\n");
      float_t* v = pack->body->geometry->object.GetPositionBuffer();
      for(int i=0; i<pack->body->deformState->NumVertices(); i++, v+=3) {
	printf("%f %f %f\n", v[0], v[1], v[2]);
	printf("%x %x %x\n", *(float*)(v), *(float*)(v+1), *(float*)(v+2));
	if(i%10 == 9)
	  getchar();
      }
      abort();
    }
  }

  //printf("coll %x\n", pack->body);
  //cout << a[0] << a[1] << a[2] << a[3]<<endl;

  Real vol;
  Vector3 center, dir;
  computeTetPlaneOverlap(a[0], a[1], a[3], a[2],
			 plane,
			 vol, center, dir);

  if(vol != 0.0) {
    vol = fabs(vol);

    pack->penetrations->push_back();
    CConstraint* p = pack->penetrations->back();

    p->a = pack->body;
    p->b = NULL;
    p->p = center;

    float_t r = get_barycentric_coordinates(center, pack->tets[t], p->p1.coords);
    if(isnan(r)) {
      printf("Tetrahedron %d\n", t);
      printf("ACK! bad tetrahedron\n");
      printf("%f %f %f %f\n", p->p1.coords[0], p->p1.coords[1], p->p1.coords[2], p->p1.coords[3]);
      printf("%d %d %d %d\n", pack->tet_indices[t][0], pack->tet_indices[t][1], pack->tet_indices[t][2], pack->tet_indices[t][3]);
      printf("%x %x %x %x\n", pack->tets[t][0], pack->tets[t][1], pack->tets[t][2], pack->tets[t][3]);
      cout << a[0] <<endl;
      cout << a[1] <<endl;
      cout << a[2] <<endl;
      cout << a[3] <<endl;
      cout << "point " << center << endl;
      abort();
    }
    else if(fabs(r-1.0) > 0.01) {
      printf("Error at tetrahedron %d, point is outside tet\n", t);
      Matrix4 mat, inv;
      mat.set(a[0],a[1],a[2],a[3]);
      mat(3,0) = mat(3,1) = mat(3,2) = mat(3,3) = One;
      inv.setInverse(mat);
      cout << "Bary to world: " << endl << mat << endl;
      cout << "World to bary: " << endl << inv << endl;
      cout << "point " << center << endl;
      Vector4 cent, tmp;
      cent.set(center[0], center[1], center[2], One);
      inv.mul(center, tmp);
      cout << "Bary coords of point: " << tmp << endl;

      tmp.get(p->p1.coords);
    }
    for(int i=0; i<4; i++) {
      p->p1.vert[i] = pack->tet_indices[t][i];
      assert(p->p1.vert[i] >= 0 && p->p1.vert[i] < p->a->deformState->NumVertices());
    }
    p->p1.n = 4;

    p->p2.coords[0] = center[0];
    p->p2.coords[1] = center[1];
    p->p2.coords[2] = center[2];
    p->p2.n = -1;
    p->constraint_type = CONSTRAINT_NORMAL | CONSTRAINT_PENETRATION;

    //normalize(dir);
    //p->fn.set(dir, center);
    p->fn = plane;
    p->pen = vol;
    (*pack->n)++;
  }
  return 1;
}




CGeometry::CGeometry() {
}

CGeometry::~CGeometry() {
}

void matrix_print(matrix4_t m)
{
  printf("%f %f %f %f\n", m[0][0], m[1][0], m[2][0], m[3][0]);
  printf("%f %f %f %f\n", m[0][1], m[1][1], m[2][1], m[3][1]);
  printf("%f %f %f %f\n", m[0][2], m[1][2], m[2][2], m[3][2]);
  printf("%f %f %f %f\n", m[0][3], m[1][3], m[2][3], m[3][3]);
}

void CGeometry::TransformWorldCoords(const RigidTransform& xform) {
  //get the world transformation matrix

  object.UpdatePositions();
 
  Vector3 temp;
  float_t* vbuf = object.GetPositionBuffer();
  for(int i=0; i<object.NumVertices(); i++, vbuf+=3) {
    xform.mulPoint(vbuf, temp);
    temp.get(vbuf);
  }
  
  object.UpdateMesh();
}

void CGeometry::InitTree() {
  Vector3* clist;
  int nc;

  //update using the tetrahedra
  vec3_t center;
  nc = object.object_data->num_elements;
  clist = new Vector3[nc];
  for(int i=0; i<nc; i++) {
    //set the center list to the centroid of the tet
    tet_centroid(object.tets[i], center);
    clist[i].set(center);
  }

  BuildTree(clist, nc, TetTree);
  delete [] clist;
  
  BoundTree::leafNode *leaves = TetTree.beginLeafUpdate();
  register unsigned int l,numL = TetTree.leafLevelSize();
  for(l=0;l<numL;l++) {
    leaves->mapIndex = l;
    leaves++;
  }
  TetTree.endLeafUpdate();
}


void CGeometry::UpdateTree()
{
  Vector3  vert;

  BoundTree::leafNode *leaves = TetTree.beginLeafUpdate();
  
  register unsigned int l,numL = TetTree.leafLevelSize();
 
  for(l=0;l<numL;l++) {
    
    register unsigned int mapIndex = leaves->mapIndex;
    
    leaves->bound.bmax.set(object.tets[l][0]);
    leaves->bound.bmin.set(object.tets[l][0]);
    
    leaves->bound.bmax.maxSet(object.tets[l][1]);
    leaves->bound.bmin.minSet(object.tets[l][1]);
    
    leaves->bound.bmax.maxSet(object.tets[l][2]);
    leaves->bound.bmin.minSet(object.tets[l][2]);
    
    leaves->bound.bmax.maxSet(object.tets[l][3]);
    leaves->bound.bmin.minSet(object.tets[l][3]);
    
    leaves++;
  }
  
  TetTree.endLeafUpdate();
}

void CGeometry::Draw()
{
  object.mesh_buffer.PreDraw();
  object.mesh_buffer.Draw();
}





/****************************************************
 * Penetration Checking goes here!!!
 ***************************************************/

int CSimulation::Penetration_Check(CBody* a, CBody* b, int& flag)
{
  TetTetPack env1, env2;
  int n=0;

  env1.n = &n;
  env1.penetrations = &constraints;
  env1.body = a;
  env1.tets = a->geometry->object.tets.elements;
  env1.tet_indices = a->geometry->object.object_data->elements;

  env2.n = &n;
  env2.penetrations = &constraints;
  env2.body = b;
  env2.tets = b->geometry->object.tets.elements;
  env2.tet_indices = b->geometry->object.object_data->elements;

  int num_pen = OS.doTraversal(tetTetOverlapCB, a->geometry->TetTree, &env1, b->geometry->TetTree, &env2);
  return n;
}

int CSimulation::Plane_Penetration_Check(CBody* a, const Plane& p, int& flag)
{
  TetPlanePack env;
  int n=0;

  env.n = &n;
  env.penetrations = &constraints;
  env.body = a;
  env.tets = a->geometry->object.tets.elements;
  env.tet_indices = a->geometry->object.object_data->elements;

  int num_pen = OS.doPlaneTraversal(planeTetOverlapCB, a->geometry->TetTree, &env, p);  
  return n;
}

