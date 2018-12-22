/*********************************************************************
 * deform.h
 * Authored by Kris Hauser 2002-2003
 *
 * Declares the DeformState class which encapsulates
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




#ifndef ELASTIC_DEFORM_CORE_H
#define ELASTIC_DEFORM_CORE_H

#include "math/fastvector.h"
#include "math/fastmatrix.h"
#include "oscillator.h"
#include "primitives.h"
#include <mylist.h>
#include <fstream.h>

typedef unsigned char BYTE;


struct DeformData
{
  DeformData();
  virtual ~DeformData();
  bool Load(ifstream&);
  bool Save(ofstream&);

  //num_Freq = # of modes, length = # of vertices * 3
  int num_Freq, length;

  vector_type vEigenValues;
  matrix_type mEigenVectors;
  
  vector_type rest_positions;
};

DeformData* LoadDeformData(const char* fn);

struct DeformState
{
  DeformState();
  virtual ~DeformState();
  virtual void Init(DeformData*);
  virtual void Cleanup();

  virtual void operator = (const DeformState&);

  inline int NumVertices() const { return length/3; }
  inline int NumModes() const { return num_Freq; }

  //deformation properties
  void SetDeformRate(float_t hertz) { DeformRate = hertz; }
  void SetMaterialProperties(float_t lame_scale, float_t alpha1, float_t alpha2);
  void SetMaterialProperties2(float_t lame_scale, float_t alpha1, float_t alpha2);
  
  //general control
  virtual void Update();
  virtual void Reset();

  //usually don't have to call these manually
  void TimeStep();
  void TimeStep(float_t dt);
  void FillDeformation();
  void ResetDeform();
  void RefreshMaterial();  //this is called when the material changes

  void applyImpulses(const float_t* imps, int* vertices, int n);
  void applyImpulses(const float_t* imps); //impulses on all vertices
  void applyForces(const float_t* forces, int* vertices, int n);
  void applyForces(const float_t* forces); //forces on all vertices

  void GetPositions(float_t* pos) const;
  void GetPositionsSelected(float_t* pos, int* verts, int n) const;
  void GetPositionsPartial(float_t* pos, int* verts, int n) const;

  void GetVelocities(float_t* vel) const;
  void GetVelocitiesSelected(float_t* vel, int* verts, int n) const;
  void GetVelocitiesPartial(float_t* vel, int* verts, int n) const;

  void SetModeStates(float_t* amp, float_t* vel);
  void IncModeStates(float_t* amp, float_t* vel);

  DeformData* data;
  
  //global properties
  float_t DeformRate;     //# of frames / sec
  float_t Deform_Scale;
  
  float_t Phi, Psi, Lame_Scale;

  //num_Freq = # of modes, length = # of vertices * 3
  int num_Freq, length;
  vector_type deformation;

  Oscillator *oscillators;
  vector_type forces;		//Mar 2003, added in to allow true simulated forces at each timestep
  
  bool Limit_Deform;
  float_t Max_Amplitude;
};







#define CONSTRAINT_X        0x1
#define CONSTRAINT_Y        0x2
#define CONSTRAINT_Z        0x4
#define CONSTRAINT_POSITION 0x7
#define CONSTRAINT_VX       0x10
#define CONSTRAINT_VY       0x20
#define CONSTRAINT_VZ       0x40
#define CONSTRAINT_VELOCITY 0x70
#define CONSTRAINT_BARYCENTRIC 0x80000000

struct Constraint
{
  int type;
  vec3_t pos;
  vec3_t vel;
  int indices [4];
  vec4_t bary_coords;
};

//allows constraints to be placed on vertices' positions and velocities
struct ConstraintState : public DeformState
{
  public:
  ConstraintState();
  ~ConstraintState();

  virtual void Init(DeformData*);

  virtual void Update() {
    DeformState::Update();
    if(do_constraints)
      Constrain();
  }
  virtual void Reset() {
    ResetConstraints();
    DeformState::Reset();
  }

  void ResetConstraints();
  void RefreshConstraints();
  void Constrain();
  void ConstrainPositions();
  void ConstrainVelocities();

  int NumConstraintPositions() { return constraints.size(); }

  Constraint* AddConstraint();
  void RemoveConstraint(Constraint* c);

  void GetConstraintPositionForces(float_t* positions, float_t* forces);
  void GetConstraintVelocityForces(float_t* positions, float_t* forces);

  bool do_constraints;
  bool use_brief_constraints;

  protected:
  void buildW(matrix_type W);
  void initializeW(const matrix_type W);

  void getPositionErrors(float_t* Ex);
  void getVelocityErrors(float_t* Ev);

  void getZ(float_t* Ex, float_t* Ev, int n, vector_type z, int nf);
  void getDz(float_t* Ex, float_t* Ev, int n, vector_type Dz, int nf);

  matrix_type C;
  matrix_type dC;
  vector_type Ex, Ev;
  vector_type z0, dz0;

  mylist<Constraint> constraints;
  int num_constraints;
};

typedef ConstraintState myDeformState;
myDeformState* CreateDeformState(DeformData*);

#endif
