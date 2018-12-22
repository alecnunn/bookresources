/*********************************************************************
 * deformdata.h
 * Authored by Kris Hauser 2002-2003
 *
 * Headers for the deformable object data.
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




#ifndef DEFORM_DATA_H
#define DEFORM_DATA_H

#include "deform.h"
#include "smesh.h"
#include "math/bary.h"
#include "math/geometry.h"

//#define NO_CALCULATE_NORMALS


struct RayCastCollision
{
  vec3_t pos;
  int poly [3];
  vec3_t bary;
};

struct DeformMeshData : public surface_mesh
{
  bool Load(ifstream& f) { return surface_mesh::load(f); }
  bool Save(ofstream& f) { return surface_mesh::save(f); }
  int RayCast(const Ray& r, vec3_t pos);
  int RayCast(const Ray& r, RayCastCollision& rc);
  float_t* VertexPointer() { return (float_t*)vertices; }
  void PreDraw() {
#ifndef NO_CALCULATE_NORMALS
    calculate_normals();
#endif
  }
  void Draw() { draw(); }
  int NumVertices() const { return num_vertices; }
  int NumPolygons() const { return num_faces; }
  void GetPolygon(int i, int verts[3]) const;
  void GetVertex(int i, vec3_t v) const;
  void GetBarycentricCoords(const vec3_t point, const int face[3], vec3_t out);
};

typedef int tet_index_element [4];

struct barycentric_index
{
  int tet;
  bary_t coords;
};

/*
 * The DeformTetData structure holds the tetrahedral topology of the object.
 */
struct DeformTetData
{
  DeformTetData();
  ~DeformTetData();
  void Init(int size);
  bool Load(ifstream& f);
  bool Save(ofstream& f);
  tet_index_element& operator [] (int i) { return elements[i]; }
  const tet_index_element& operator [] (int i) const { return elements[i]; }

  tet_index_element* elements;
  int num_elements;
};

/*
 * The DeformTetPointers structure holds the tetrahedral topology of the object,
 * but using pointers to the actual vertex data.
 */
struct DeformTetPointers
{
  DeformTetPointers();
  ~DeformTetPointers();
  void Init(const DeformTetData&, float_t* verts, int nv);
  inline void Deform(const barycentric_index& bary, vec3_t out) const
  {
	  Deform(bary.tet, bary.coords, out);
  }
  inline void Deform(int tet, const bary_t b, vec3_t out) const
  {
	  const tetrahedron_ptr& e = elements[tet];
	  vec3_multiply(out, e[0], b[0]);
	  vec3_madd(out, e[1], b[1]);
	  vec3_madd(out, e[2], b[2]);
	  vec3_madd(out, e[3], b[3]);
  }
  tetrahedron_ptr& operator [] (int i) { return elements[i]; }
  const tetrahedron_ptr& operator [] (int i) const { return elements[i]; }

  tetrahedron_ptr* elements;
  int num_elements;
};

/*
 * The DeformTetEmbedding structure holds the barycentric coordinates of mesh vertices
 * relative to some tetrahedral data.
 */
struct DeformTetEmbedding
{
  DeformTetEmbedding();
  ~DeformTetEmbedding();
  bool Load(ifstream& f);
  bool Save(ofstream& f);
  void Init(int);

  //Embed() initializes this structure by calculating the barycentric coords of the the given mesh vertices,
  //relative to the tetrahedral topology and vertices
  int Embed(const float_t* meshvertices, int nmv, const DeformTetData&, const float_t* tetvertices);
  int Embed(const float_t* meshvertices, int nmv, const DeformTetPointers&);

  //Apply() outputs the positions of the mesh vertices via the barycentric FFD
  void Apply(float_t* meshvertices, int nmv, const DeformTetData&, const float_t* tetvertices) const;
  void Apply(float_t* meshvertices, int nmv, const DeformTetPointers&) const;

  barycentric_index* bary;
  int num_bary;
};

/*
 * The DeformObjectData structure holds the eigenvalue data as well as the
 * object's base shape, i.e. its vertex information and the topological 
 * information of the faces and tetrahedra.
 * Inherits from DeformMeshData and DeformTetData
 */
struct DeformObjectData : public DeformData, public DeformMeshData, public DeformTetData
{
  DeformObjectData();
  ~DeformObjectData();
  bool Load(ifstream& f);
  bool Save(ofstream& f);

  bool carton_embedded;
  DeformTetEmbedding* embedding;

/*
  void PrecalculateResponse();
  bool approximate_response;
  vector_type accel_response[3];
  vector_type angvel_response[3];
  vector_type angaccel_response[3];
*/
};



DeformObjectData* LoadDeformObjectData(const char* fn);

#endif
