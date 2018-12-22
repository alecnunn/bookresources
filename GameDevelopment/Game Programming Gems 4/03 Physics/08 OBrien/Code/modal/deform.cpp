/*********************************************************************
 * deform.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Defines the DeformState and ConstraintState methods which encapsulates
 * the modal deformation.
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



#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <limits.h>
#include <math.h>
#include <stdincludes.h>

//----------------------------------------------


#include <assert.h>

#include "primitives.h"

#include "deform.h"

//#include "lu_decomp.h"
#include "math/svd.h"


#ifndef M_2PI
#define M_2PI 6.283185307179586476925286766559
#endif
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define EPSILON 1e-4


//some weird matrix math functions to speed up computation

//does a dot product with the sparse vector vals of length n 
Real vector_select_dot(const vector_type v, int* sel, const Real* vals, int n)
{
  	Real sum = 0.0;
	/*	for(int i=0; i<n; i++)
		sum += v[sel[i]] * vals[i];
	*/
	for(;n>0;n--, sel++, vals++)
	  sum += v[*sel] * *vals;
	return sum;
}

inline float_t matrix_row_select_dot(const matrix_type m, int i, int* sel, const Real* vals, int n)
{
	return vector_select_dot(m[i], sel, vals, n);
}

float_t matrix_col_select_dot(const matrix_type m, int i, int* sel, const float_t* vals, int n)
{
  float_t sum = 0.0;
  for(int j=0; j<n; j++) {
    sum += m[sel[j]][i]*vals[j];
  }
  return sum;
}

//x += a*c
void vector_multiply_add(vector_type x, const vector_type a, Real c, int n)
{
  /*	for(int i=0; i<n; i++)
		x[i] += a[i]*c;
  */
  const float_t* fa = a;
  for(;n>0;n--,x++,fa++)
    *x = *fa*c;
}


//X gets the m rows from A indexed by indices
void matrix_row_selection(matrix_type X, const matrix_type A, int* indices, int m_indices, int n);
//X gets the n cols from A indexed by indices
void matrix_col_selection(matrix_type X, const matrix_type A, int m, int* indices, int n_indices);

//X gets the m rows from A^t indexed by indices
void matrix_row_selection_transpose(matrix_type X, const matrix_type A, int* indices, int m_indices, int n);
//X gets the n cols from A^t indexed by indices
void matrix_col_selection_transpose(matrix_type X, const matrix_type A, int m, int* indices, int n_indices);


void matrix_row_selection(matrix_type X, const matrix_type A, int* indices, int m_indices, int n)
{
	int i,j;
	for(i=0; i<m_indices; i++)
		for(j=0; j<n; j++)
			X[i][j] = A[indices[i]][j];
}

void matrix_col_selection(matrix_type X, const matrix_type A, int m, int* indices, int n_indices)
{
	int i,j;
	for(i=0; i<m; i++)
		for(j=0; j<n_indices; j++)
			X[i][j] = A[i][indices[j]];
}

void matrix_row_selection_transpose(matrix_type X, const matrix_type A, int* indices, int m_indices, int n)
{
	int i,j;
	for(i=0; i<m_indices; i++)
		for(j=0; j<n; j++)
			X[i][j] = A[j][indices[i]];
}

void matrix_col_selection_transpose(matrix_type X, const matrix_type A, int m, int* indices, int n_indices)
{
	int i,j;
	for(i=0; i<m; i++)
		for(j=0; j<n_indices; j++)
			X[i][j] = A[indices[j]][i];
}






DeformData::DeformData()
  : vEigenValues(NULL),
  mEigenVectors(NULL),
  rest_positions(NULL)
{}

DeformData::~DeformData()
{
  vector_delete(vEigenValues);
  matrix_delete(mEigenVectors);
  vector_delete(rest_positions);

  rest_positions = NULL;
  vEigenValues = NULL;
  mEigenVectors = NULL;
}


bool DeformData::Load(ifstream& f)
{
  f >> num_Freq;
  f >> length;

  printf("num_Freq = %d, length = %d\n", num_Freq, length);

  vEigenValues = vector_create(num_Freq);
  mEigenVectors = matrix_create(length, num_Freq);
  rest_positions = vector_create(length);

  int i;
  for(i=0; i<num_Freq; i++) {
    f >> vEigenValues[i];
  }
  if(!f.good()) {
    printf("DeformData::Load()- Error reading eigenvalues\n");
    return false;
  }

  Real* eigenptr = mEigenVectors[0];
  int l = num_Freq*length;
  for(i=0; i<l; i++) {
    f >> eigenptr[i];
  }
  if(!f.good()) {
    printf("DeformData::Load()- Error reading eigenvectors\n");
    return false;
  }

  for(i=0; i<length; i++) {
    f >> rest_positions[i];
  }
  if(!f.good()) {
    printf("DeformData::Load()- Error reading rest positions\n");
    return false;
  }
  return true;
}

bool DeformData::Save(ofstream& f)
{
  f << num_Freq << " " << length << endl;

  int i;
  for(i=0; i<num_Freq; i++) {
    f << vEigenValues[i] << " ";
  }
  f << endl;

  for(i=0; i<length; i++) {
    for(int j=0; j<num_Freq; j++)
      f << mEigenVectors[i][j] << " ";
    f << endl;
  }
  f << endl;

  for(i=0; i<length; i++) {
    f << rest_positions[i] << " ";
  }
  f << endl;

  return true;
}


#if 0
bool DeformData::Load(myFile& f)
{
  /*  int infile;

  // load in the freqency file
  infile = sceOpen(filename, SCE_RDONLY);
  if ( infile < 0) {
    printf("couldnt find the file %s\n", filename);
    return false;
    }*/
  
  if(!ReadFile(f, num_Freq)) return false;
  if(!ReadFile(f, length)) return false;

  printf("num_Freq = %d, length = %d\n", num_Freq, length);

  vEigenValues = vector_create(num_Freq);
  mEigenVectors = matrix_create(length, num_Freq);
  rest_positions = vector_create(length);

  if(!ReadArrayFile(f, vEigenValues, num_Freq)) {
    printf("Error reading eigenvalues\n");
    return false;
  }

  if(!ReadArrayFile(f, matrix_block_ptr(mEigenVectors), num_Freq*length)) {
    printf("Error reading eigenvectors\n");
    return false;
  }

  if(!ReadArrayFile(f, rest_positions, length)) {
    printf("Error reading rest positions\n");
    return false;
  }

  return true;
}
#endif

DeformState::DeformState()
: DeformRate(60), Deform_Scale(1),
  Phi(1.0),
  Psi(1.0),
  Lame_Scale(1.0),
  deformation(NULL),
  oscillators(NULL),
  Limit_Deform(false),
  Max_Amplitude(2.0)
{}

DeformState::~DeformState()
{
	Cleanup();
}

void DeformState::Cleanup()
{
	SafeArrayDelete(oscillators);
	SafeDeleteProc(deformation, vector_delete);
}

void DeformState::Init(DeformData* dat) {
  data = dat;
  num_Freq = data->num_Freq;
  length = data->length;

  deformation = vector_create(num_Freq);
  
  oscillators = new Oscillator [num_Freq];

  RefreshMaterial();
  ResetDeform();
}

void DeformState::operator = (const DeformState& rhs)
{
  if(data != rhs.data) {
    Init(rhs.data);
  }
  for(int i=0; i<num_Freq; i++)
    oscillators[i] = rhs.oscillators[i];
  FillDeformation();

  DeformRate = rhs.DeformRate;
  Deform_Scale = rhs.Deform_Scale;
  
  Phi = rhs.Phi;
  Psi = rhs.Psi;
  Lame_Scale = rhs.Lame_Scale;

  Limit_Deform = rhs.Limit_Deform;
  Max_Amplitude = rhs.Max_Amplitude;
}

void DeformState::Reset()
{
  ResetDeform();
}

void DeformState::ResetDeform()
{
  int i;
  for ( i = 0; i < num_Freq; i++ ) {
    oscillators[i].Set_z_dz(0,0);
  }
  FillDeformation();
}

void DeformState::SetMaterialProperties(float_t lame_scale, float_t alpha1, float_t alpha2)
{
  Lame_Scale = lame_scale;
  Phi = alpha1;
  Psi = alpha2;
  RefreshMaterial();
  ResetDeform();
}

void DeformState::SetMaterialProperties2(float_t lame_scale, float_t alpha1, float_t alpha2)
{
  Lame_Scale = lame_scale;
  Phi = alpha1;
  Psi = alpha2;
  RefreshMaterial();
  for(int i=0; i<num_Freq; i++)
    oscillators[i].Set_z_dz(deformation[i], 0.0);
}

void DeformState::RefreshMaterial()
{
  float_t lambda, omega;
  int i;

  double DeformPeriod = 1.0 / DeformRate;

  for ( i = 0; i < num_Freq; i++ ) {
    omega = data->vEigenValues[i]; // in Hz
    lambda = omega*Lame_Scale;
    oscillators[i].Setup_KMa1a2(lambda, 1.0, Phi, Psi);
    oscillators[i].Setup_dt(DeformPeriod);
  }
}



/****************************************************
      Position / velocity output functions
****************************************************/


void DeformState::GetPositions(float_t* destcoords) const 
{
  int i;
  float_t sum;

  float_t* srccoords = data->rest_positions;

  for(i=0; i<length; i++) {
    sum = matrix_row_dot(data->mEigenVectors, i, deformation, num_Freq);
    *destcoords = *srccoords + sum*Deform_Scale;
    srccoords ++;
    destcoords ++;
  }
}

void DeformState::GetVelocities(float_t* destcoords) const 
{
  int i;
  float_t sum;

  vector_type dz = vector_create(num_Freq);
  for(i=0; i<num_Freq; i++)
    dz[i] = oscillators[i].Get_dz();

  for(i=0; i<length; i++) {
    sum = matrix_row_dot(data->mEigenVectors, i, dz, num_Freq);
    *destcoords = sum*Deform_Scale;
    destcoords ++;
  }

  vector_delete(dz);
}

void DeformState::GetPositionsPartial(float_t* pos, int* verts, int n) const 
{
  int i;
  int ind;
  vec3_t sum;

  float_t* rest;
  for(i=0; i<n; i++) {
      ind = verts[i]*3;

      rest = &data->rest_positions[ind];
      sum[0] = matrix_row_dot(data->mEigenVectors, ind, deformation, num_Freq);
      sum[1] = matrix_row_dot(data->mEigenVectors, ind+1, deformation, num_Freq);
      sum[2] = matrix_row_dot(data->mEigenVectors, ind+2, deformation, num_Freq);
      
      pos[ind] = (float_t)(sum[0]*Deform_Scale) + rest[0];
      pos[ind+1] = (float_t)(sum[1]*Deform_Scale) + rest[1];
      pos[ind+2] = (float_t)(sum[2]*Deform_Scale) + rest[2];
  }
}

void DeformState::GetPositionsSelected(float_t* pos, int* verts, int n) const 
{
  int i;
  int ind;
  vec3_t sum;

  float_t* rest;
  for(i=0; i<n; i++) {
      ind = verts[i]*3;
      rest = &data->rest_positions[ind];

      sum[0] = matrix_row_dot(data->mEigenVectors, ind, deformation, num_Freq);
      sum[1] = matrix_row_dot(data->mEigenVectors, ind+1, deformation, num_Freq);
      sum[2] = matrix_row_dot(data->mEigenVectors, ind+2, deformation, num_Freq);
      
      (*pos++) = (float_t)(sum[0]*Deform_Scale) + rest[0];
      (*pos++) = (float_t)(sum[1]*Deform_Scale) + rest[1];
      (*pos++) = (float_t)(sum[2]*Deform_Scale) + rest[2];
  }
}

void DeformState::GetVelocitiesSelected(float_t* vel, int* verts, int n) const 
{
  int i;
  int ind;
  vec3_t sum;

  vector_type dz = vector_create(num_Freq);
  for(i=0; i<num_Freq; i++)
    dz[i] = oscillators[i].Get_dz();

  for(i=0; i<n; i++) {
      ind = verts[i]*3;

      sum[0] = matrix_row_dot(data->mEigenVectors, ind, dz, num_Freq);
      sum[1] = matrix_row_dot(data->mEigenVectors, ind+1, dz, num_Freq);
      sum[2] = matrix_row_dot(data->mEigenVectors, ind+2, dz, num_Freq);

      (*vel++) = (float_t)(sum[0]*Deform_Scale);
      (*vel++) = (float_t)(sum[1]*Deform_Scale);
      (*vel++) = (float_t)(sum[2]*Deform_Scale);
  }
  vector_delete(dz);
}


void DeformState::Update()
{
  TimeStep();
  FillDeformation();
}

void DeformState::TimeStep()
{
  for(int i = 0; i < num_Freq; i++)
    oscillators[i].Inc_t();
}

void DeformState::TimeStep(float_t dt)
{
  for(int i = 0; i < num_Freq; i++)
    oscillators[i].Inc_t(dt);
}

void DeformState::FillDeformation()
{
  for(int i = 0; i < num_Freq; i++) {
      deformation[i] = oscillators[i].Get_z();

    /*    if(Limit_Deform) {
      if(deformation[i] > Max_Amplitude + 0.02) {
	printf("WARNING Deformation exceeds limit, %f\n", deformation[i]);
      }
      }*/
  }
}

void DeformState::applyImpulses(const float_t* forces, int* vertices, int n)
{
  int i,j,k;
  int v;

  float_t zero = 0.0f;

  for(i=0; i<num_Freq; i++) {
    float_t proj = 0.0;
    for(j=0, k=0; j<n; j++, k+=3) {
      v = vertices[j]*3;
      proj += data->mEigenVectors[v][i]*forces[k];
      proj += data->mEigenVectors[v+1][i]*forces[k+1];
      proj += data->mEigenVectors[v+2][i]*forces[k+2];
    }

    if(Limit_Deform)
      oscillators[i].Crop_Inc(zero, proj, Max_Amplitude);

    oscillators[i].Inc_z_dz(0.0f, proj);
  }
}

void DeformState::applyImpulses(const float_t* forces)
{
  int i;
  float_t zero;

  for ( i = 0; i < num_Freq; i++ ) {
    float_t proj = matrix_col_dot(data->mEigenVectors, i, (float_t*)forces, length);
    if(Limit_Deform)
      oscillators[i].Crop_Inc(zero, proj, Max_Amplitude);
    oscillators[i].Inc_z_dz(0.0f, proj);
  }
}

void DeformState::SetModeStates(float_t* amp, float_t* vel) {
  for(int i=0; i<num_Freq; i++) {
    if(Limit_Deform)
      oscillators[i].Crop_Set(amp[i], vel[i], Max_Amplitude);
    oscillators[i].Set_z_dz(amp[i], vel[i]);
  }
  FillDeformation();
}

void DeformState::IncModeStates(float_t* amp, float_t* vel) {
  for(int i=0; i<num_Freq; i++) {
    if(Limit_Deform)
      oscillators[i].Crop_Inc(amp[i], vel[i], Max_Amplitude);
    oscillators[i].Inc_z_dz(amp[i], vel[i]);
  }
  FillDeformation();
}







ConstraintState::ConstraintState()
  :DeformState(), do_constraints(true), use_brief_constraints(false),
   C(NULL), dC(NULL),
   Ex(NULL), Ev(NULL),
   z0(NULL), dz0(NULL),
   num_constraints(0)
{
}


ConstraintState::~ConstraintState()
{
  SafeDeleteProc(C, matrix_delete);
  SafeDeleteProc(dC, matrix_delete);
  SafeDeleteProc(Ex, vector_delete);
  SafeDeleteProc(Ev, vector_delete);
  SafeDeleteProc(z0, vector_delete);
  SafeDeleteProc(dz0, vector_delete);
}

void ConstraintState::Init(DeformData* dat) {
  DeformState::Init(dat);
  z0 = vector_create(num_Freq);
  dz0 = vector_create(num_Freq);
}

void ConstraintState::initializeW(const matrix_type W)
{
  //x = displacements
  //z' = dz/dt = oscillator's force effects
  //z = oscillator values

  //x = (W * z' * Wt) f
  //z = z' * Wt * f
  //A := (W * z' * Wt)
  //C := z' * Wt * A inverse
  //=> z = C*x

  int nc = num_constraints;
  int nf = num_Freq;

  int i;
  matrix_type Wt, A, Ainv;
  matrix_type dAinv;
  Wt = matrix_create(nf, nc);
  A = matrix_create(nc, nc);
  Ainv = matrix_create(nc, nc);
  dAinv = matrix_create(nc, nc);
  if(!use_brief_constraints) {
    C = matrix_create(nf, nc);
    dC = matrix_create(nf, nc);
  }

  matrix_transpose(Wt, W, nc, nf);

  matrix_type temp;
  temp = matrix_create(nf, nc);

  //solve for Ainv
  //temp gets z * Wt
  for(i=0; i<nf; i++)
    {
      float_t z = oscillators[i].Get_z_df();
      vector_scale(temp[i], Wt[i], z, nc);
    }

  matrix_multiply(A, W, temp, nc, nf, nc);
  svd_minv_solve(A, Ainv, nc, nc);

  //solve for dAinv
  //temp gets dz * Wt
  for(i=0; i<nf; i++)
    {
      float_t z = oscillators[i].Get_dz_df();
      vector_scale(temp[i], Wt[i], z, nc);
    }

  matrix_multiply(A, W, temp, nc, nf, nc);
  svd_minv_solve(A, dAinv, nc, nc);

  matrix_delete(temp);
  matrix_delete(A);

  if(!use_brief_constraints) {
    //solve for C and dC
    matrix_multiply(C, Wt, Ainv, nf, nc, nc);

    for(i=0; i<nf; i++) {
      float_t z = oscillators[i].Get_z_df();
      vector_scale(C[i], C[i], z, nc);
    }

    matrix_multiply(dC, Wt, dAinv, nf, nc, nc);

    for(i=0; i<nf; i++) {
      float_t z = oscillators[i].Get_dz_df();
      vector_scale(dC[i], dC[i], z, nc);
    }

    /*  temp = matrix_create(nc,nc);
	matrix_multiply(temp, W, C, nc, nf, nc);
	printf("Accuracy:\n");
	matrix_print(temp, nc, nc);
	matrix_delete(temp);
    */
    matrix_delete(Wt);
    matrix_delete(Ainv);
    matrix_delete(dAinv);
  }
  else {
    C = Ainv;
    dC = dAinv;
    matrix_delete(Wt);
  }
}




void ConstraintState::ResetConstraints()
{
  SafeDeleteProc(C, matrix_delete);
  SafeDeleteProc(dC, matrix_delete);
  SafeDeleteProc(Ex, vector_delete);
  SafeDeleteProc(Ev, vector_delete);
  num_constraints = 0;
  constraints.clear();
}

void ConstraintState::RefreshConstraints()
{
  if(constraints.size()*3 != num_constraints) {
    SafeDeleteProc(C, matrix_delete);
    SafeDeleteProc(dC, matrix_delete);
    SafeDeleteProc(Ex, vector_delete);
    SafeDeleteProc(Ev, vector_delete);
    num_constraints = constraints.size()*3;
  }
  if(num_constraints != 0) {
    printf("Refreshing constraints\n");
    matrix_type W = matrix_create(num_constraints, num_Freq);
    buildW(W);
    initializeW(W);
    matrix_delete(W);
    Ex = vector_create(num_constraints);
    Ev = vector_create(num_constraints);
  }
}

Constraint* ConstraintState::AddConstraint() {
  constraints.push_back();
  return constraints.back();
}



void ConstraintState::RemoveConstraint(Constraint* c)
{
  constraints.erase(c);
}


void ConstraintState::buildW(matrix_type W)
{
  matrix_zero(W, num_constraints, num_Freq);
  int u = 0;
  Constraint* c = NULL;
  while((c = constraints.next(c))) {
    if(c->type & CONSTRAINT_BARYCENTRIC) {
      for(int j=0; j<4; j++) {
	int v = c->indices[j]*3;
	float_t tmp = c->bary_coords[j];
	if(c->type & CONSTRAINT_X)
	  vector_madd(W[u], data->mEigenVectors[v], tmp, num_Freq);
	if(c->type & CONSTRAINT_Y)
	  vector_madd(W[u+1], data->mEigenVectors[v+1], tmp, num_Freq);
	if(c->type & CONSTRAINT_Z)
	  vector_madd(W[u+2], data->mEigenVectors[v+2], tmp, num_Freq);
      }
    }
    else {
      int v = c->indices[0]*3;
      if(c->type & CONSTRAINT_X)
	vector_equal(W[u], data->mEigenVectors[v], num_Freq);
      if(c->type & CONSTRAINT_Y)
	vector_equal(W[u+1], data->mEigenVectors[v+1], num_Freq);
      if(c->type & CONSTRAINT_Z)
	vector_equal(W[u+2], data->mEigenVectors[v+2], num_Freq);
    }
    u += 3;
  }
  assert(u == num_constraints);
}

void ConstraintState::getPositionErrors(float_t* Ex) {
  int i = 0;
  Constraint* c = NULL;
  while((c = constraints.next(c))) {
    vec3_t pos;
    if(c->type & CONSTRAINT_BARYCENTRIC) {
      vec3_t v[4];
      GetPositionsSelected(v[0], c->indices, 4);
      vec3_multiply(pos, v[0], c->bary_coords[0]);
      vec3_madd(pos, v[1], c->bary_coords[1]);
      vec3_madd(pos, v[2], c->bary_coords[2]);
      vec3_madd(pos, v[3], c->bary_coords[3]);
    }
    else {
      GetPositionsSelected(pos, c->indices, 1);
    }

    vec3_sub(&Ex[i], c->pos, pos);
    i+=3;
  }
}

void ConstraintState::getVelocityErrors(float_t* Ev) {
  int i = 0;
  Constraint* c = NULL;
  while((c = constraints.next(c))) {
    vec3_t pos;
    if(c->type & CONSTRAINT_BARYCENTRIC) {
      vec3_t v[4];
      GetVelocitiesSelected(v[0], c->indices, 4);
      vec3_multiply(pos, v[0], c->bary_coords[0]);
      vec3_madd(pos, v[1], c->bary_coords[1]);
      vec3_madd(pos, v[2], c->bary_coords[2]);
      vec3_madd(pos, v[3], c->bary_coords[3]);
    }
    else {
      GetVelocitiesSelected(pos, c->indices, 1);
    }
    //printf("velocity %d: %f %f %f\n", i, pos[0], pos[1], pos[2]);
    vec3_sub(&Ev[i], c->vel, pos);
    i+=3;
  }
}

void ConstraintState::getZ(float_t* Ex, float_t* Ev, int n, vector_type z, int nf)
{
	assert(n == num_constraints);
	assert(nf == num_Freq);
	assert(!use_brief_constraints);

	matrix_vector_multiply(z, C, Ex, nf, n);
}


void ConstraintState::getDz(float_t* Ex, float_t* Ev, int n, vector_type Dz, int nf)
{
	assert(n == num_constraints);
	assert(nf == num_Freq);
	assert(!use_brief_constraints);

	matrix_vector_multiply(Dz, dC, Ev, nf, n);
}


//x += A*b
inline void matrix_vector_madd(vector_type x, const matrix_type a, const vector_type b, int m, int n)
{
	int i;
	for(i=0; i<m; i++)
	{
		x[i] += vector_dot(a[i], b, n);
	}
}

void applyIntegratedImpulses(const float_t* shifts, int* vertices, int n, DeformState* state)
{
  int i,j,k;
  int v;

  //for(j=0, k=0; j<n; j++, k+=3)
  //printf("Force on %d: %f %f %f\n", vertices[j], shifts[k], forces[k+1], forces[k+2]);
  for(i=0; i<state->num_Freq; i++) {
    float_t proj = 0.0;
    for(j=0, k=0; j<n; j++, k+=3) {
      v = vertices[j]*3;
      proj += state->data->mEigenVectors[v][i]*shifts[k];
      proj += state->data->mEigenVectors[v+1][i]*shifts[k+1];
      proj += state->data->mEigenVectors[v+2][i]*shifts[k+2];
    }
    
    state->oscillators[i].Inc_z_dz(proj*state->oscillators[i].Get_z_df(), 0.0);
  }
}

void ConstraintState::Constrain()
{
  if(num_constraints == 0)
    return;
  getPositionErrors(Ex);
  getVelocityErrors(Ev);
  if(!use_brief_constraints) {
    getZ(Ex,Ev,num_constraints,z0,num_Freq);
    getDz(Ex,Ev,num_constraints,dz0,num_Freq);
    IncModeStates(z0,dz0);
  }
  else {
    assert(num_constraints < num_Freq);
    matrix_vector_multiply(z0, C, Ex, num_constraints, num_constraints);
    matrix_vector_multiply(dz0, dC, Ev, num_constraints, num_constraints);
    
    /*    printf("position errors:\n");
    vector_print(Ex, num_constraints);
    printf("mode amplitude changes:\n");
    vector_print(z0, num_constraints);*/
    //printf("velocity errors:\n");
    //vector_print(Ev, num_constraints);

    vec3_t f;
    int k=0;
    Constraint* c = NULL;
    while((c = constraints.next(c))) {
      if(c->type & CONSTRAINT_BARYCENTRIC) {
	for(int i=0; i<4; i++) {
	  vec3_multiply(f, &dz0[k], c->bary_coords[i]);
	  applyImpulses(f, &c->indices[i], 1);
	  vec3_multiply(f, &z0[k], c->bary_coords[i]);
	  applyIntegratedImpulses(f, &c->indices[i], 1, this);
	}
      }
      else {
	applyImpulses(&dz0[k], &c->indices[0], 1);
	applyIntegratedImpulses(&z0[k], &c->indices[0], 1, this);
      }
      k+=3;
    }
    //printf("done\n");
    FillDeformation();

    //getVelocityErrors(Ev);
    //printf("new velocity errors:\n");
    //vector_print(Ev, num_constraints);

    //getPositionErrors(Ex);
    //printf("new position errors:\n");
    //vector_print(Ex, num_constraints);
  }
}

void ConstraintState::GetConstraintVelocityForces(float_t* positions, float_t* forces)
{
  if(num_constraints == 0)
    return;
  assert(use_brief_constraints == true);
  getPositionErrors(Ex);
  getVelocityErrors(Ev);

  matrix_vector_multiply(forces, dC, Ev, num_constraints, num_constraints);

  int i=0;
  Constraint* c = NULL;
  while((c = constraints.next(c))) {
    vec3_sub(&positions[i*3], c->pos, &Ex[i*3]);
    i++;
  }
}

void ConstraintState::GetConstraintPositionForces(float_t* positions, float_t* forces)
{
  if(num_constraints == 0)
    return;
  assert(use_brief_constraints == true);
  getPositionErrors(Ex);
  matrix_vector_multiply(forces, C, Ex, num_constraints, num_constraints);

  int i=0;
  Constraint* c = NULL;
  while((c = constraints.next(c))) {
    vec3_sub(&positions[i*3], c->pos, &Ex[i*3]);
    i++;
  }
}


DeformData* LoadDeformData(const char* fn)
{
  DeformData* d = new DeformData;
  ifstream i(fn);
  if(i.fail())
    return NULL;
  if(!d->Load(i)) {
    delete d;
    return NULL;
  }
  return d;
}

myDeformState* CreateDeformState(DeformData* d)
{
  myDeformState* state = new myDeformState;
  state->Init(d);
  return state;
}
