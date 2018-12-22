/*********************************************************************
 * deformobj.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Definitions for the deformable object runtime classes.
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


#include "deformobj.h"

#ifdef DRAW_TETRAHEDRA
#include <GL/gl.h>
#endif //DRAW_TETRAHEDRA

#include <string.h>
#include <float.h>
#include "raytriangle.h"

#define mStartTimer(x)
#define mStopTimer(x)





#define EPSILON 1e-5

bool within_tolerance(float_t a, float_t b)
{
	return fabs(a-b) < EPSILON;
}


DeformObject::DeformObject()
  :state(NULL), object_data(NULL), position_buffer(NULL)
{}

DeformObject::~DeformObject()
{
  if(buffered && position_buffer)
    vector_delete(position_buffer);
}

void DeformObject::Init(DeformObjectData* dat, ConstraintState* s, float_t* pos_buf)
{
  state = s;
  assert(s->length == dat->length);
  {
    buffered = true;
    position_buffer = vector_create(s->length);
  }

  object_data = dat;
  tets.Init(*dat, position_buffer, s->length/3);
  mesh_buffer.instantiate(dat);
}

#ifdef DRAW_TETRAHEDRA

void draw_triangle(float* a, float* b, float* c)
{
  vec3_t p,q,n;
  vec3_sub(p, b, a);
  vec3_sub(q, c, a);
  vec3_cross(n, p, q);
  vec3_normalize(n);
  glNormal3fv(n);
  glVertex3fv(a);
  glNormal3fv(n);
  glVertex3fv(b);
  glNormal3fv(n);
  glVertex3fv(c);
}

void DrawTetrahedron(tet_ptr_element e)
{
    draw_triangle(e[0], e[1], e[2]);
    draw_triangle(e[1], e[3], e[2]);
    draw_triangle(e[3], e[1], e[0]);
    draw_triangle(e[2], e[3], e[0]);
}
#endif

void DeformObject::DrawTetrahedra()
{
#ifdef DRAW_TETRAHEDRA
  if(object_data->num_elements == 0)
    return;
  //glDisable(GL_LIGHTING)
  glBegin(GL_TRIANGLES);
  for(int i=0; i<object_data->num_elements; i++) {
    DrawTetrahedron(elements[i]);
  }
  glEnd();
  //glEnable(GL_LIGHTING);
#endif
}

Constraint* DeformObject::AddVertexConstraint(int vertex) {
  Constraint* c = state->AddConstraint();
  c->type = CONSTRAINT_POSITION | CONSTRAINT_VELOCITY;
  c->indices[0] = vertex;
  vec3_zero(c->pos);
  vec3_zero(c->vel);
  return c;
}

Constraint* DeformObject::AddBarycentricConstraint(const vec4_t bary, const int ind [4]) {
  Constraint* c = state->AddConstraint();
  c->type = CONSTRAINT_POSITION | CONSTRAINT_VELOCITY | CONSTRAINT_BARYCENTRIC;
  c->indices[0] = ind[0];
  c->indices[1] = ind[1];
  c->indices[2] = ind[2];
  c->indices[3] = ind[3];
  vec4_equal(c->bary_coords, bary);
  vec3_zero(c->pos);
  vec3_zero(c->vel);
  return c;
}


Constraint* DeformObject::AddInternalConstraint(const vec3_t point) {
  vec4_t bary;
  int ind[4];
  if(GetBarycentricCoords(point, bary, ind) != -1)
    return AddBarycentricConstraint(bary, ind);
  return NULL;
}


int DeformObject::GetBarycentricCoords(const vec3_t point, vec4_t bary, int ind[4]) const {
  float_t temp;
  //float fmax = 1000000;
  //vec4_t barytemp;
  //int imax = -1;
  for(int i=0; i<tets.num_elements; i++) {
    temp = get_barycentric_coordinates(point, tets.elements[i], bary);
    if(temp < 1.0001) {
      if(temp < 0.9500)
	printf("Weird! point has barycentric coords smaller than usual!\n");
      ind[0] = tets.elements[i][0]-position_buffer;
      ind[1] = tets.elements[i][1]-position_buffer;
      ind[2] = tets.elements[i][2]-position_buffer;
      ind[3] = tets.elements[i][3]-position_buffer;
      ind[0] /= 3;
      ind[1] /= 3;
      ind[2] /= 3;
      ind[3] /= 3;
      return i;
    }
    /*    else if(temp < fmax) {
      vec4_equal(bary, barytemp);
      fmax = temp;
      imax = i;
      }*/
  }
  /*  if(fmax < 1.1) {
    ind[0] = tets.elements[imax][0]-position_buffer;
    ind[1] = tets.elements[imax][1]-position_buffer;
    ind[2] = tets.elements[imax][2]-position_buffer;
    ind[3] = tets.elements[imax][3]-position_buffer;
    return imax;
    }
  printf("Point doesn't have barycentric coords, closest is %f on element %d\n", fmax, imax);*/
  return -1;
}
#define DEFORM_LIMIT 2.0

/*
void DeformObject::SetRigidBodyState(const RigidBodyState& rb) {
  float_t DeformPeriod = 1.0 / state->DeformRate;
  vec3_t dVel, dAngVel;
  vec3_multiply(dVel, rb.vAccel, DeformPeriod);
  vec3_multiply(dAngVel, rb.vAngAccel, DeformPeriod);
  AddRigidBodyForces(rb.mRotation, dVel, rb.vAngVel, dAngVel);
}


void DeformObject::AddRigidBodyForces(const matrix4_t mRotation, const vec3_t dVel, const vec3_t angVel, const vec3_t dAngVel) {
  int i,j;

  if(object_data->approximate_response) {
    float_t zero = 0.0;
    float_t sum;
    vec3_t accel_loc;
    //printf("approx response\n");
    matrix4_vec3_multiply_transpose(accel_loc, mRotation, dVel);

    int num_Freq = NumModes();
    for(i=0; i<num_Freq; i++) {
      sum = 0.0;
      for(j=0; j<3; j++)
	sum += accel_loc[j] * object_data->accel_response[j][i];
      for(j=0; j<3; j++)
	sum += angVel[j] * object_data->angvel_response[j][i];
      for(j=0; j<3; j++)
	sum += dAngVel[j] * object_data->angaccel_response[j][i];

      if(state->Limit_Deform)
	state->oscillators[i].Crop_Inc(zero, sum, state->Max_Amplitude);
      state->oscillators[i].Inc_z_dz(0, sum);
    }
  }
  else {
    int length = state->length;
    vector_type vaccel = vector_create(length);
    vec3_t temp;

    //add accel (in local coords)
    //this is a linear function of the local accel, but none of the others are
    vec3_t accel_loc;
    matrix4_vec3_multiply_transpose(accel_loc, mRotation, dVel);
    for(i=0; i<length; i+=3) {
      vec3_negate(&vaccel[i], accel_loc);
    }
    
    //add ang accel
    for(i=0; i<length; i+=3) {
      vec3_cross(temp, dAngVel, &position_buffer[i]);
      vec3_sub(&vaccel[i], &vaccel[i], temp);
    }
    
    //add centrifugal force
    if(use_centrifugal_effects) {
      for(i=0; i<length; i+=3) {
	vec3_t vtemp;
	vec3_cross(vtemp, angVel, &position_buffer[i]);
	vec3_cross(temp, angVel, vtemp);
	
	vec3_sub(&vaccel[i], &vaccel[i], temp);
      }
    }
    
    //add coriolis force
    if(use_coriolis_effects) {
      printf("Can't compute coriolis effects.\n");
      printf("They require finding the vertex velocities\n");
      abort();

      for(i=0; i<length; i+=3) {
        vec3_cross(temp, angvel, velocities);
        vec3_multiply(temp, temp, 2.0);
      
        vec3_sub(&vaccel[i], &vaccel[i], temp);
      }
    }

    //now apply the force to the oscillators
    state->applyImpulses(vaccel);

    vector_delete(vaccel);
  }
}
*/


void DeformObject::Draw()
{
#ifdef DRAW_TETRAHEDRA
  DrawTetrahedra();
#endif
  mesh_buffer.Draw();
}

void DeformObject::Update()
{
  UpdatePositions();
  UpdateMesh();
  mesh_buffer.PreDraw();
}

void DeformObject::UpdateMesh()
{
  if(IsCarton()) {
    mStartTimer("Mesh update");
    object_data->embedding->Apply(mesh_buffer.VertexPointer(), NumMeshVertices(), tets);
    mStopTimer("Mesh update");
  }
  else {
    if(buffered) {
      float_t* verts = GetPositionBuffer();
      memcpy(mesh_buffer.VertexPointer(), GetPositionBuffer(), sizeof(float_t)*3*NumMeshVertices());
    }
  }
}

float_t* DeformObject::CartonCoords(int v) const
{
  return object_data->embedding->bary[v].coords;
}

int DeformObject::CartonTet(int v) const
{
  return object_data->embedding->bary[v].tet;
}

int* DeformObject::CartonTetIndices(int v) const
{
  return object_data->elements[CartonTet(v)];
}

void DeformObject::ApplyMeshVertexImpulse(int v, const vec3_t f)
{
  if(IsCarton()) {
    ApplyBarycentricImpulse(CartonCoords(v), CartonTetIndices(v), f);
  }
  else {
    ApplyVertexImpulse(v, f);
  }
}

void DeformObject::ApplyMeshBarycentricImpulse(const vec3_t bary, int ind[3], const vec3_t f)
{
  if(IsCarton()) {
    vec3_t pos;
    vec3_t temp;
    mesh_buffer.GetVertex(ind[0], temp);
    vec3_multiply(pos, temp, bary[0]);
    mesh_buffer.GetVertex(ind[1], temp);
    vec3_madd(pos, temp, bary[1]);
    mesh_buffer.GetVertex(ind[2], temp);
    vec3_madd(pos, temp, bary[2]);

    DeformObject::ApplyInternalImpulse(pos, f);
  }
  else {
    vec3_t forces [3];
    vec3_multiply(forces[0], f, bary[0]);
    vec3_multiply(forces[1], f, bary[1]);
    vec3_multiply(forces[2], f, bary[2]);
    DeformObject::ApplyVertexImpulses(ind, forces, 3);
  }
}

void bary3_to_bary4(const vec3_t b3, const int i3[3], vec4_t b4, int i4[4])
{
    b4[0] = b3[0];
    b4[1] = b3[1];
    b4[2] = b3[2];
    b4[3] = 0.0;

    i4[0] = i3[0];
    i4[1] = i3[1];
    i4[2] = i3[2];
    i4[3] = 0;
}

Constraint* DeformObject::AddMeshVertexConstraint(int v)
{
  if(IsCarton()) {
    return DeformObject::AddBarycentricConstraint(CartonCoords(v), CartonTetIndices(v));
  }
  else
    return DeformObject::AddVertexConstraint(v);
}

Constraint* DeformObject::AddMeshBarycentricConstraint(const vec3_t bary, int ind[3])
{
  if(IsCarton()) {
    vec3_t pos;
    vec3_t temp;
    mesh_buffer.GetVertex(ind[0], temp);
    vec3_multiply(pos, temp, bary[0]);
    mesh_buffer.GetVertex(ind[1], temp);
    vec3_madd(pos, temp, bary[1]);
    mesh_buffer.GetVertex(ind[2], temp);
    vec3_madd(pos, temp, bary[2]);

    return DeformObject::AddInternalConstraint(pos);
  }
  else {
    vec4_t b;
    int i[4];
    bary3_to_bary4(bary, ind, b, i);
    return DeformObject::AddBarycentricConstraint(b, i);
  }
}

Constraint* DeformObject::ConstrainMeshVertex(int v, vec3_t pos)
{
  Constraint* c = AddMeshVertexConstraint(v);
  SetConstraintPosition(c, pos);
  return c;
}

Constraint* DeformObject::ConstrainMeshBarycentric(const vec3_t bary, int ind[3], vec3_t pos)
{
  Constraint* c = AddMeshBarycentricConstraint(bary, ind);
  SetConstraintPosition(c, pos);
  return c;
}


void DeformObject::RayForce(const Ray& r, float_t force)
{
  vec3_t f;
  RayCastCollision rc;
  if(mesh_buffer.RayCast(r, rc) != -1) {
    vec3_multiply(f, r.direction, force / vec3_length(r.direction));

    ApplyMeshBarycentricImpulse(rc.bary, rc.poly, f);
  }
}

Constraint* DeformObject::RayVertexConstrain(const Ray& r)
{
  RayCastCollision rc;
  if(mesh_buffer.RayCast(r, rc) == -1)
    return NULL;
  if(rc.bary[0] > rc.bary[1]) {
    if(rc.bary[2] > rc.bary[0])
      return ConstrainMeshVertex(rc.poly[2], &position_buffer[rc.poly[2]*3]);
    else
      return ConstrainMeshVertex(rc.poly[0], &position_buffer[rc.poly[0]*3]);
  }
  else {
    if(rc.bary[2] > rc.bary[1])
      return ConstrainMeshVertex(rc.poly[2], &position_buffer[rc.poly[2]*3]);
    else 
      return ConstrainMeshVertex(rc.poly[1], &position_buffer[rc.poly[1]*3]);
  }
}

Constraint* DeformObject::RayConstrain(const Ray& r)
{
  RayCastCollision rc;
  if(mesh_buffer.RayCast(r, rc) == -1)
    return NULL;
  return ConstrainMeshBarycentric(rc.bary, rc.poly, rc.pos);
}

















