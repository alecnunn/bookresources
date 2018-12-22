/*********************************************************************
 * smesh.h
 * Authored by Kris Hauser 2002-2003
 *
 * A simple mesh structure.
 *
 * Copyright (c) 2003, Regents of the University of California
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


#ifndef SURFACE_MESH_H
#define SURFACE_MESH_H

#include <primitives.h>
#include <fstream.h>

struct surface_mesh
{
  surface_mesh();
  ~surface_mesh();
  bool load(istream&);
  bool save(ostream&);
  bool load(const char* fn);
  bool save(const char* fn);

  //creates this as an instance of the given mesh (shares the same topology
  //as the other surface mesh, but the coordinates are different)
  void instantiate(surface_mesh*);
  //calculates the vertex normals by averaging face normals
  void calculate_normals();
  //initializes the rest of the structure from just the vertices and faces
  void init_verts_faces();

  void draw();

  bool is_valid() const;
  void get_aabb(float_t bb [6]) const;
  int ray_cast(const vec3_t src, const vec3_t dir, vec3_t out) const;
  bool face_ray_intersect(const vec3_t src, const vec3_t dir, int f, vec3_t out, float_t& dist) const;
  bool face_ray_intersect_bf(const vec3_t src, const vec3_t dir, int f, vec3_t out, float_t& dist) const;

  //Data types.
  typedef float_t texcoord_t [2];
  typedef float_t rgba_t [4];
  struct vertex_ref_t {
    int vertex;
    int normal;
    int texcoords;
  };
  typedef vertex_ref_t face_t [3];
  struct material_t {
    rgba_t ambient, diffuse, specular, emissive;
    float_t specular_strength;
  };
  struct face_group_t {
    int face_start, face_end;
    int material;
  };

  //Mesh data.  These are the data that are saved and loaded.
  vec3_t* vertices;
  vec3_t* normals;
  texcoord_t* texcoords;
  face_t* faces;
  material_t* materials;
  face_group_t* face_groups;
  int num_vertices;
  int num_normals;
  int num_texcoords;
  int num_faces;
  int num_materials;
  int num_face_groups;

  //this is a flag set by instantiate().
  //linked meshes share faces, materials, and face groups
  //so don't delete them on destruction.
  bool linked;
};


#endif  //SURFACE_MESH_H
