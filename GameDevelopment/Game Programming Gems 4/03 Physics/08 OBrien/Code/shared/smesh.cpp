/*********************************************************************
 * smesh.cpp
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



#include "smesh.h"
#include <stdincludes.h>
//#include <myfile.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include "aabb.h"

#include <GL/gl.h>

#define RGBACONV(x) { tmp[0] = x[0]; tmp[1] = x[1]; tmp[2] = x[2]; tmp[3] = x[3]; }

void set_material(const surface_mesh::material_t& mat)
{
  if(mat.diffuse[3] != 1.0) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
  }
#ifdef MATH_DOUBLE
  float tmp[4];
  RGBACONV(mat.ambient);
  glMaterialfv(GL_FRONT, GL_AMBIENT, tmp);
  RGBACONV(mat.diffuse);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, tmp);
  RGBACONV(mat.specular);
  glMaterialfv(GL_FRONT, GL_SPECULAR, tmp);
  RGBACONV(mat.emissive);
  glMaterialfv(GL_FRONT, GL_EMISSION, tmp);
  glMaterialf(GL_FRONT, GL_SHININESS, mat.specular_strength);
#else
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat.specular);
  glMaterialfv(GL_FRONT, GL_EMISSION, mat.emissive);
  glMaterialf(GL_FRONT, GL_SHININESS, mat.specular_strength);
#endif
  //if(mat.diffuse[3] != 1.0)
  //glDisable(GL_BLEND);
}

void unset_material(const surface_mesh::material_t& mat)
{
  if(mat.diffuse[3] != 1.0) {
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
  }
}



surface_mesh::surface_mesh()
  :vertices(NULL), normals(NULL), texcoords(NULL),
   faces(NULL), materials(NULL), face_groups(NULL),
   num_vertices(0), num_normals(0), num_texcoords(0),
   num_faces(0), num_materials(0), num_face_groups(0),
   linked(false)
{
}

surface_mesh::~surface_mesh()
{
  SafeArrayDelete(vertices);
  SafeArrayDelete(normals);
  SafeArrayDelete(texcoords);
  if(!linked) {
    SafeArrayDelete(faces);
    SafeArrayDelete(materials);
    SafeArrayDelete(face_groups);
  }
}

istream& load_vec3(istream& in, vec3_t v)
{
  in >> v[0] >> v[1] >> v[2];
  return in;
}

istream& operator >> (istream& in, surface_mesh::texcoord_t t)
{
  in >> t[0] >> t[1];
  return in;
}

istream& load_rgba(istream& in, surface_mesh::rgba_t c)
{
  in >> c[0] >> c[1] >> c[2] >> c[3];
  return in;
}

istream& operator >> (istream& in, surface_mesh::vertex_ref_t& v)
{
  in >> v.vertex >> v.normal >> v.texcoords;
  return in;
}

istream& load_face (istream& in, surface_mesh::face_t f)
{
  in >> f[0] >> f[1] >> f[2];
  return in;
}

istream& operator >> (istream& in, surface_mesh::material_t& m)
{
  load_rgba(in, m.ambient);
  load_rgba(in, m.diffuse);
  load_rgba(in, m.specular);
  load_rgba(in, m.emissive);
  in >> m.specular_strength;
  return in;
}

istream& operator >> (istream& in, surface_mesh::face_group_t& f) {
  in >> f.face_start >> f.face_end >> f.material;
  return in;
};

bool surface_mesh::load(istream& f) {
  f >> num_vertices >> num_normals >> num_texcoords;
  f >> num_faces >> num_materials >> num_face_groups;

  if(!f.good()) return false;

  vertices = new vec3_t[num_vertices];
  normals = new vec3_t[num_normals];
  if(num_texcoords!=0)
    texcoords = new texcoord_t[num_texcoords];
  faces = new face_t [num_faces];
  materials = new material_t [num_materials];
  face_groups = new face_group_t [num_face_groups];

  int i;
  for(i=0; i<num_vertices; i++)
    load_vec3(f, vertices[i]);
  for(i=0; i<num_normals; i++)
    load_vec3(f, normals[i]);
  for(i=0; i<num_texcoords; i++)
    f >> texcoords[i];
  for(i=0; i<num_faces; i++)
    load_face(f, faces[i]);
  for(i=0; i<num_materials; i++)
    f >> materials[i];
  for(i=0; i<num_face_groups; i++)
    f >> face_groups[i];

  return f.good();
}


ostream& save_vec3(ostream& out, const vec3_t v)
{
  out << v[0] << " "
	  << v[1] << " "
	  << v[2];
  return out;
}

ostream& operator << (ostream& out, const surface_mesh::texcoord_t t)
{
  out << t[0] << " " 
	  << t[1];
  return out;
}

ostream& save_rgba(ostream& out, const surface_mesh::rgba_t c)
{
  out << c[0] << " "
	  << c[1] << " "
	  << c[2] << " "
	  << c[3];
  return out;
}

ostream& operator << (ostream& out, const surface_mesh::vertex_ref_t& v)
{
  out << v.vertex << " " << v.normal << " " << v.texcoords;
  return out;
}

ostream& save_face (ostream& out, const surface_mesh::face_t f)
{
  out << f[0] << " " << f[1] << " " << f[2];
  return out;
}

ostream& operator << (ostream& out, const surface_mesh::material_t& m)
{
  save_rgba(out, m.ambient); out << endl;
  save_rgba(out, m.diffuse); out << endl;
  save_rgba(out, m.specular); out << endl;
  save_rgba(out, m.emissive); out << endl;
  out << m.specular_strength;
  return out;
}

ostream& operator << (ostream& out, const surface_mesh::face_group_t& f) {
  out << f.face_start << " " << f.face_end << "\t" << f.material;
  return out;
};

bool surface_mesh::save(ostream& f) {
  f << num_vertices << " " << num_normals << " " << num_texcoords << " "
	<< num_faces << " " << num_materials << " " << num_face_groups << endl;

  if(!f.good()) return false;

  f << endl;

  int i;
  for(i=0; i<num_vertices; i++) {
    save_vec3(f, vertices[i]); f << endl;
  }
  f << endl;

  for(i=0; i<num_normals; i++) {
    save_vec3(f, normals[i]); f << endl;
  }
  f << endl;

  for(i=0; i<num_texcoords; i++)
    f << texcoords[i] << endl;
  f << endl;

  for(i=0; i<num_faces; i++) {
    save_face(f, faces[i]); f << endl;
  }
  f << endl;

  for(i=0; i<num_materials; i++)
    f << materials[i] << endl;
  f << endl;

  for(i=0; i<num_face_groups; i++)
    f << face_groups[i] << endl;
  f << endl;

  return f.good();
}

/*
bool surface_mesh::load(const char* fn) {
  myFile f;
  if(!f.Open(fn, FILEREAD)) {
    return false;
  }

  if(!ReadFile(f, num_vertices)) return false;
  vertices = new vec3_t[num_vertices];
  if(!ReadFile(f, num_normals)) return false;
  normals = new vec3_t[num_normals];
  if(!ReadFile(f, num_texcoords)) return false;
  if(num_texcoords!=0)
    texcoords = new texcoord_t[num_texcoords];
  if(!ReadFile(f, num_faces)) return false;
  faces = new face_t [num_faces];
  if(!ReadFile(f, num_materials)) return false;
  materials = new material_t [num_materials];
  if(!ReadFile(f, num_face_groups)) return false;
  face_groups = new face_group_t [num_face_groups];

  if(!ReadArrayFile(f, vertices, num_vertices)) return false;
  if(!ReadArrayFile(f, normals, num_normals)) return false;
  if(num_texcoords != 0)
    if(!ReadArrayFile(f, texcoords, num_texcoords)) return false;
  if(!ReadArrayFile(f, faces, num_faces)) return false;
  if(!ReadArrayFile(f, materials, num_materials)) return false;
  if(!ReadArrayFile(f, face_groups, num_face_groups)) return false;

  return true;
}

bool surface_mesh::save(const char* fn) {
  myFile f;
  if(!f.Open(fn, FILEWRITE))
    return false;

  if(!WriteFile(f, num_vertices)) return false;
  if(!WriteFile(f, num_normals)) return false;
  if(!WriteFile(f, num_texcoords)) return false;
  if(!WriteFile(f, num_faces)) return false;
  if(!WriteFile(f, num_materials)) return false;
  if(!WriteFile(f, num_face_groups)) return false;

  if(!WriteArrayFile(f, vertices, num_vertices)) return false;
  if(!WriteArrayFile(f, normals, num_normals)) return false;
  if(num_texcoords != 0)
    if(!WriteArrayFile(f, texcoords, num_texcoords)) return false;
  if(!WriteArrayFile(f, faces, num_faces)) return false;
  if(!WriteArrayFile(f, materials, num_materials)) return false;
  if(!WriteArrayFile(f, face_groups, num_face_groups)) return false;
  return true;
}
*/

bool surface_mesh::is_valid() const {
  bool res = true;
  for(int i=0; i<num_faces; i++) {
    for(int j=0; j<3; j++) {
      if(faces[i][j].vertex < 0 || faces[i][j].vertex >= num_vertices) {
	res = false;
	printf("face %d, vertex %d has a vertex index (%d) out of range\n", i, j, faces[i][j].vertex);
      }
      if(faces[i][j].normal < 0 || faces[i][j].normal >= num_normals) {
	res = false;
	printf("face %d, vertex %d has a normal index (%d) out of range\n", i, j, faces[i][j].normal);
      }
      if(faces[i][j].texcoords < -1 || faces[i][j].texcoords >= num_texcoords) {
	res = false;
	printf("face %d, vertex %d has a texcoord index (%d) out of range\n", i, j, faces[i][j].normal);
      }
    }
  }
  for(int i=0; i<num_face_groups; i++) {
    if(face_groups[i].face_start < 0 || face_groups[i].face_end >= num_faces
       || face_groups[i].face_end < face_groups[i].face_start) {
      res = false;
      printf("face group %d has a range (%d, %d) out of range\n", i, face_groups[i].face_end, face_groups[i].face_end);
    }
    if(face_groups[i].material < 0 || face_groups[i].material >= num_materials) {
      res = false;
      printf("face group %d has a material (%d) out of range\n", i, face_groups[i].material);
    }
  }
  return res;
}

void surface_mesh::get_aabb(float_t bb [6]) const {
  aabb_init(bb, (float_t)FLT_MAX, (float_t)-FLT_MAX);
  for(int i=0; i<num_vertices; i++) {
    aabb_expand_point(bb, vertices[i]);
  }
}

void triangle_normal(vec3_t n, const vec3_t a, const vec3_t b, const vec3_t c)
{
  vec3_t p,q;
  vec3_sub(p, b, a);
  vec3_sub(q, c, a);
  vec3_cross(n, p, q);
}

void surface_mesh::calculate_normals()
{
  for(int i=0; i<num_normals; i++)
    vec3_zero(normals[i]);
  face_t* f = faces;
  int a,b,c;
  vec3_t v;
  for(int i=0; i<num_faces; i++, f++) {
    a = (*f)[0].vertex;
    b = (*f)[1].vertex;
    c = (*f)[2].vertex;

    triangle_normal(v, vertices[a], vertices[b], vertices[c]);

    a = (*f)[0].normal;
    b = (*f)[1].normal;
    c = (*f)[2].normal;

    vec3_add(normals[a], normals[a], v);
    vec3_add(normals[b], normals[b], v);
    vec3_add(normals[c], normals[c], v);
  }

  for(int i=0; i<num_normals; i++)
    vec3_normalize(normals[i]);
}


void surface_mesh::init_verts_faces()
{
  for(int i=0; i<num_faces; i++) {
    for(int j=0; j<3; j++) {
      faces[i][j].normal = faces[i][j].vertex;
      faces[i][j].texcoords = -1;
    }
  }

  num_materials = 1;
  materials = new material_t [1];
  rgba_make(materials[0].diffuse, 0.7, 0.7, 0.7, 1.0);
  rgba_make(materials[0].ambient, 0.7, 0.7, 0.7, 1.0);
  rgba_zero(materials[0].specular);
  rgba_zero(materials[0].emissive);
  materials[0].specular_strength = 0.0;
  num_face_groups = 1;
  face_groups = new face_group_t [1];
  face_groups[0].face_start = 0;
  face_groups[0].face_end = num_faces-1;
  face_groups[0].material = 0;

  num_normals = num_vertices;
  normals = new vec3_t [num_normals];

  calculate_normals();
}

#ifdef MATH_DOUBLE
#define GLNORMAL glNormal3dv
#define GLTEXCOORD glTexCoord2dv
#define GLVERTEX glVertex3dv
#else
#define GLNORMAL glNormal3fv
#define GLTEXCOORD glTexCoord2fv
#define GLVERTEX glVertex3fv
#endif

void surface_mesh::draw() 
{
  face_group_t* fg = face_groups;
  vertex_ref_t* v;
  for(int i=0; i<num_face_groups; i++, fg++) {
    set_material(materials[fg->material]);
    glBegin(GL_TRIANGLES);
    face_t* f = &faces[fg->face_start];
    for(int j=fg->face_start; j<=fg->face_end; j++, f++) {
      v = *f;
      for(int k=0; k<3; k++, v++) {
	GLNORMAL(normals[v->normal]);
	if(v->texcoords != -1)
	  GLTEXCOORD(texcoords[v->texcoords]);
	GLVERTEX(vertices[v->vertex]);
	//printf("%f %f %f\n", vertices[v->vertex][0], vertices[v->vertex][1], vertices[v->vertex][2]);
      }
    }
    glEnd();
  }
}

void surface_mesh::instantiate(surface_mesh* mesh)
{
  if(vertices)
    abort();
  linked = true;
  vertices = new vec3_t [mesh->num_vertices];
  normals = new vec3_t [mesh->num_normals];
  if(mesh->num_texcoords != 0)
    texcoords = new texcoord_t [mesh->num_texcoords];

  faces = mesh->faces;
  materials = mesh->materials;
  face_groups = mesh->face_groups;

  num_vertices = mesh->num_vertices;
  num_normals = mesh->num_normals;
  num_texcoords = mesh->num_texcoords;
  num_faces = mesh->num_faces;
  num_materials = mesh->num_materials;
  num_face_groups = mesh->num_face_groups;


}

#include "raytriangle.h"

bool surface_mesh::face_ray_intersect(const vec3_t src, const vec3_t dir, int f, vec3_t out, float_t& dist) const
{
	float_t *a, *b, *c;
	a = vertices[faces[f][0].vertex];
	b = vertices[faces[f][1].vertex];
	c = vertices[faces[f][2].vertex];

	float_t t,u,v;
	if(ray_triangle_intersect_cull(src,dir,a,b,c,&t,&u,&v)) {
	  if(t < dist) {
	    dist = t;
	    intersect_to_intersection(t,u,v, a,b,c, out);
	    return true;
	  }
	}
	return false;
}

bool surface_mesh::face_ray_intersect_bf(const vec3_t src, const vec3_t dir, int f, vec3_t out, float_t& dist) const
{
	float_t *a, *b, *c;
	a = vertices[faces[f][0].vertex];
	b = vertices[faces[f][1].vertex];
	c = vertices[faces[f][2].vertex];

	float_t t,u,v;
	if(ray_triangle_intersect(src,dir,a,b,c,&t,&u,&v)) {
	  printf("triangle intersects, dist %f\n", t);
	  if(t < dist) {
	    dist = t;
	    intersect_to_intersection(t,u,v, a,b,c, out);
	    return true;
	  }
	}
	return false;
}

int surface_mesh::ray_cast(const vec3_t src, const vec3_t dir, vec3_t out) const {
  vec3_t temp;
  int res = -1;
  float_t dist = FLT_MAX;
  for(int i=0; i<num_faces; i++) {
    if(face_ray_intersect(src, dir, i, temp, dist)) {
      vec3_equal(out, temp);
      res = i;
    }
  }
  return res;
}



