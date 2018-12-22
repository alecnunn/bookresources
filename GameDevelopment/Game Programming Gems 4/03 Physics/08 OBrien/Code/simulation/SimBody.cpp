/*********************************************************************
 * SimBody.cpp
 * Authored by Kris Hauser/Chen Shen 2002-2003
 *
 * Definitions for the ImpulseRigidBody, CDeformState, and CBody class
 * methods.
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
#include "SimAnim.h"
#include "math/bary.h"
#include "stdincludes.h"
/*
#include "SimLog.h"
extern bool sim_logging;
extern SimLog sim_log;
*/

void ImpulseRigidBody::applyImpulseAdd(const Vector3& frc, const Vector3& loc) {
  const Matrix3 invR = transpose(_pos.R);
  const Vector3 locL = invR*(loc-_pos.t);
  const Vector3 frcL = invR*frc;

  _tVel += frc/_mass;
  _aVel += _pos.R*_IMatInv*cross(locL-_com,frcL);

}

void ImpulseRigidBody::applyShiftAdd(const Vector3& frc, const Vector3& loc) {
  const Matrix3 invR = transpose(_pos.R);
  const Vector3 locL = invR*(loc-_pos.t);
  const Vector3 frcL = invR*frc;
  
  Vector3 dRotV = _pos.R*_IMatInv*cross(locL-_com,frcL);

  Matrix3 dRotM;
  SetMatrixRotationVector(dRotM,dRotV);
  Matrix3 newRot;
  newRot.mul(dRotM,_pos.R);
  
  _pos.t += frc/_mass;
  _pos.R = newRot;

  recomputeVelMat();
}


void ImpulseRigidBody::operator = (const CRigidState& state)
{
  posTrnSet(state.trnVec);
  posRotSet(state.rotMat);
  tVelSet(state.tVel);
  aVelSet(state.aVel);
  setForce(state.netFrc);
  setMoment(state.netMom);
}

void CRigidState::operator = (const ImpulseRigidBody& body)
{
  trnVec = body.trnVec();
  rotMat = body.rotMat();
  tVel   = body.tVel();
  aVel   = body.aVel();
  netFrc = body.netFrc();
  netMom = body.netMom();
}


void CDeformState::Init(DeformData* dat)
{
  DeformState::Init(dat);


  // Changed due to bug (?) in some versions of gcc.
  //#ifdef UPDATE_VERTEX_STATE
  //  position = new Vector3 [NumVertices()];
  //  velocity = new Vector3 [NumVertices()];
  //  accel = new Vector3 [NumVertices()];
  //#endif

#ifdef UPDATE_VERTEX_STATE
  int _nv = NumVertices();
  position = new Vector3 [_nv];
  velocity = new Vector3 [_nv];
  accel = new Vector3 [_nv];
#endif

  mode_velocity = vector_create(num_Freq);
  mode_accel = vector_create(num_Freq);
  ext_mode_forces = vector_create(num_Freq);
  vector_zero(ext_mode_forces, num_Freq);

  Object_Scale = One;

  vertex_state_updated = false;
}

void CDeformState::Cleanup()
{
  DeformState::Cleanup();
  SafeArrayDelete(position);
  SafeArrayDelete(velocity);
  SafeArrayDelete(accel);

  SafeArrayDelete(mode_accel);
  SafeArrayDelete(mode_velocity);
  SafeArrayDelete(ext_mode_forces);
}

void CDeformState::operator = (const CDeformState& rhs)
{
  DeformState::operator = (rhs);

  int        nump = NumVertices();

  long       size = sizeof(Vector3)*nump;
#ifdef UPDATE_VERTEX_POSITION
  memcpy(position,    rhs.position,    size);
  memcpy(velocity,    rhs.velocity,    size);
  memcpy(accel, rhs.accel, size);
#endif

  forces = rhs.forces;

  vector_equal(mode_velocity, rhs.mode_velocity, num_Freq);
  vector_equal(mode_accel, rhs.mode_accel, num_Freq);
  vector_equal(ext_mode_forces, rhs.ext_mode_forces, num_Freq);
}

void CDeformState::AddShift(const Vector3& force, const CBodyPoint& p)
{
  float_t *evec1, *evec2, *evec3;
  int i,j;
  float_t fa, fb, fc;
  Real InvScale = Inv(Object_Scale);

  Vector3 vf;
  for(i=0; i<p.n; i++) {
    vf = force*p.coords[i]*InvScale;
    fa = vf[0];
    fb = vf[1];
    fc = vf[2];

    evec1 = data->mEigenVectors[p.vert[i]*3];
    evec2 = data->mEigenVectors[p.vert[i]*3+1];
    evec3 = data->mEigenVectors[p.vert[i]*3+2];
    for(j=0; j<num_Freq; j++) {
      oscillators[j].Inc_z_dz(*evec1*fa + *evec2*fb + *evec3*fc, 0);
      evec1++;
      evec2++;
      evec3++;
    }
  }
}

void CDeformState::AddImpulse(const Vector3& force, const CBodyPoint& p)
{
  float_t f [12];
  Real InvScale = Inv(Object_Scale);
  for(int i=0; i<p.n; i++) {
    f[i*3] = force[0]*p.coords[i]*InvScale;
    f[i*3+1] = force[1]*p.coords[i]*InvScale;
    f[i*3+2] = force[2]*p.coords[i]*InvScale;
  }
  applyImpulses(f, (int*)p.vert, p.n);
}

void CDeformState::AddForce(const Vector3& force, const CBodyPoint& p)
{
  float_t *evec1, *evec2, *evec3;
  int i,j;
  float_t fa, fb, fc;
  Real InvScale = Inv(Object_Scale);

  Vector3 vf;
  for(i=0; i<p.n; i++) {
    vf = force*p.coords[i]*InvScale;
    fa = vf[0];
    fb = vf[1];
    fc = vf[2];

    evec1 = data->mEigenVectors[p.vert[i]*3];
    evec2 = data->mEigenVectors[p.vert[i]*3+1];
    evec3 = data->mEigenVectors[p.vert[i]*3+2];
    for(j=0; j<num_Freq; j++) {
      ext_mode_forces[j] += *evec1*fa + *evec2*fb + *evec3*fc;
      evec1++;
      evec2++;
      evec3++;
    }
  }
}

void CDeformState::RefreshModes()
{
  unsigned int i;

  for(i=0; i<num_Freq; i++) {
    deformation[i] = oscillators[i].Get_z();
    mode_velocity[i] = oscillators[i].Get_dz();
    mode_accel[i] = -(data->vEigenValues[i]*deformation[i] + (Phi*data->vEigenValues[i]+Psi)*mode_velocity[i]) + ext_mode_forces[i]; 
  }
}

void CDeformState::RefreshSelected(int* verts, int n)
{
  unsigned int i, j, k;
  int nump = NumVertices();

  vec3_t sumd, sumv, suma;
  int vert;
  for ( i = 0; i < n; i++ ) {
    vert = verts[i];
    float_t *col1 = data->mEigenVectors[vert*3];
    float_t *col2 = data->mEigenVectors[vert*3+1];
    float_t *col3 = data->mEigenVectors[vert*3+2];
    float_t *pdeformation = deformation;
    float_t *pvelocity = mode_velocity;
    float_t *paccel = mode_accel;
    register float_t pt,vt,at;
    register float_t e1,e2,e3;
    
    vec3_zero(sumd);
    vec3_zero(sumv);
    vec3_zero(suma);

    for ( j = 0; j < num_Freq; j++) {
      e1 = *col1;
      e2 = *col2;
      e3 = *col3;
      pt = *pdeformation;
      vt = *pvelocity;
      at = *paccel;
      sumd[0] += e1*pt; sumd[1] += e2*pt; sumd[2] += e3*pt; 
      sumv[0] += e1*vt; sumv[1] += e2*vt; sumv[2] += e3*vt; 
      suma[0] += e1*at; suma[1] += e2*at; suma[2] += e3*at; 
      col1++;
      col2++;
      col3++;
      pdeformation++;
      pvelocity++;
      paccel++;
    }

    //vec3_multiply(sumd, sumd, Object_Scale);
    vec3_multiply(sumv, sumv, Object_Scale);
    vec3_multiply(suma, suma, Object_Scale);

    for(j=0; j<3; j++) {
      position[vert][j] = data->rest_positions[i*3+j] + sumd[j];
    }
    position[vert] *= Object_Scale;
    velocity[vert].set(sumv);
    accel[vert].set(suma);
  }
}

void CDeformState::RefreshSelectedPositions(int* verts, int n)
{
  unsigned int i, j, k;
  int nump = NumVertices();

  vec3_t sumd;
  int vert;
  for ( i = 0; i < n; i++ ) {
    vert = verts[i];
    GetPosition(sumd, vert);

    for(j=0; j<3; j++) {
      position[vert][j] = data->rest_positions[i*3+j] + sumd[j];
    }
  }
}

void CDeformState::RefreshSelectedVelocities(int* verts, int n)
{
  unsigned int i, j, k;
  int nump = NumVertices();

  vec3_t sumv;
  int vert;
  for ( i = 0; i < n; i++ ) {
    vert = verts[i];
    GetVelocity(sumv, vert);

    velocity[vert].set(sumv);
  }
}

void CDeformState::RefreshSelectedAccels(int* verts, int n)
{
  unsigned int i, j, k;
  int nump = NumVertices();

  vec3_t suma;
  int vert;
  for ( i = 0; i < n; i++ ) {
    vert = verts[i];
    GetAccel(suma, vert);

    accel[vert].set(suma);
  }
}

void CDeformState::Refresh()
{
  unsigned int i, j, k;
  int nump = NumVertices();

  RefreshModes();

#ifdef UPDATE_VERTEX_STATE
  Vector3 *p = position;
  Vector3 *v = velocity;
  Vector3 *a = accel;
  vec3_t sumd, sumv, suma;

  for ( i = 0; i < nump; i++ ) {
    float_t *col1 = data->mEigenVectors[i*3];
    float_t *col2 = data->mEigenVectors[i*3+1];
    float_t *col3 = data->mEigenVectors[i*3+2];
    float_t *pdeformation = deformation;
    float_t *pvelocity = mode_velocity;
    float_t *paccel = mode_accel;
    register float_t pt,vt,at;
    register float_t e1,e2,e3;
    
    vec3_zero(sumd);
    vec3_zero(sumv);
    vec3_zero(suma);

    for ( j = 0; j < num_Freq; j++) {
      e1 = *col1;
      e2 = *col2;
      e3 = *col3;
      pt = *pdeformation;
      vt = *pvelocity;
      at = *paccel;
      sumd[0] += e1*pt; sumd[1] += e2*pt; sumd[2] += e3*pt; 
      sumv[0] += e1*vt; sumv[1] += e2*vt; sumv[2] += e3*vt; 
      suma[0] += e1*at; suma[1] += e2*at; suma[2] += e3*at; 
      col1++;
      col2++;
      col3++;
      pdeformation++;
      pvelocity++;
      paccel++;
    }

    //vec3_multiply(sumd, sumd, Object_Scale);
    vec3_multiply(sumv, sumv, Object_Scale);
    vec3_multiply(suma, suma, Object_Scale);

    for(j=0; j<3; j++) {
      (*p)[j] = data->rest_positions[i*3+j] + sumd[j];
      (*v)[j] = sumv[j];
      (*a)[j] = suma[j];
    }

    (*p) *= Object_Scale;

    p++;
    v++;
    a++;
  }

  vertex_state_updated = true;
#else
  vertex_state_updated = false;
#endif
}

void CDeformState::Update(float_t dt)
{
  unsigned int i, j, k;
  int nump = NumVertices();

  //apply external forces
  for(i=0; i<num_Freq; i++) {
    oscillators[i].Inc_z_dz(0, ext_mode_forces[i]*dt);
  }

  TimeStep(dt);

  RefreshModes();

  vector_zero(ext_mode_forces, num_Freq);

#ifdef UPDATE_VERTEX_STATE
  Vector3 *p = position;
  Vector3 *v = velocity;
  Vector3 *a = accel;
  vec3_t sumd, sumv, suma;

  for ( i = 0; i < nump; i++ ) {
    float_t *col1 = data->mEigenVectors[i*3];
    float_t *col2 = data->mEigenVectors[i*3+1];
    float_t *col3 = data->mEigenVectors[i*3+2];
    float_t *pdeformation = deformation;
    float_t *pvelocity = mode_velocity;
    float_t *paccel = mode_accel;
    register float_t pt,vt,at;
    register float_t e1,e2,e3;
    
    vec3_zero(sumd);
    vec3_zero(sumv);
    vec3_zero(suma);

    for ( j = 0; j < num_Freq; j++) {
      e1 = *col1;
      e2 = *col2;
      e3 = *col3;
      pt = *pdeformation;
      vt = *pvelocity;
      at = *paccel;
      sumd[0] += e1*pt; sumd[1] += e2*pt; sumd[2] += e3*pt; 
      sumv[0] += e1*vt; sumv[1] += e2*vt; sumv[2] += e3*vt; 
      suma[0] += e1*at; suma[1] += e2*at; suma[2] += e3*at; 
      col1++;
      col2++;
      col3++;
      pdeformation++;
      pvelocity++;
      paccel++;
    }

    //vec3_multiply(sumd, sumd, Object_Scale);
    vec3_multiply(sumv, sumv, Object_Scale);
    vec3_multiply(suma, suma, Object_Scale);

    for(j=0; j<3; j++) {
      (*p)[j] = data->rest_positions[i*3+j] + sumd[j];
      (*v)[j] = sumv[j];
      (*a)[j] = suma[j];
    }

    (*p) *= Object_Scale;

    p++;
    v++;
    a++;
  }

  vertex_state_updated = true;
#else
  vertex_state_updated = false;
#endif
}

void CDeformState::GetPosition(vec3_t x, int i) const
{
  int j;
  register float_t *col1 = data->mEigenVectors[i*3];
  register float_t *col2 = data->mEigenVectors[i*3+1];
  register float_t *col3 = data->mEigenVectors[i*3+2];
  register float_t *ptr = deformation;
  register float_t pt;
  
  vec3_t sum;
  vec3_zero(sum);
  
  for ( j = 0; j < num_Freq; j++) {
    pt = *ptr;
    sum[0] += *col1*pt;
    sum[1] += *col2*pt;
    sum[2] += *col3*pt;
    col1++;
    col2++;
    col3++;
    ptr++;
  }
  
  for(j=0; j<3; j++) {
    x[j] = Object_Scale*(data->rest_positions[i*3+j] + sum[j]);
  }
}

void CDeformState::GetVelocity(vec3_t x, int i) const
{
  int j;
  register float_t *col1 = data->mEigenVectors[i*3];
  register float_t *col2 = data->mEigenVectors[i*3+1];
  register float_t *col3 = data->mEigenVectors[i*3+2];
  register float_t *ptr = mode_velocity;
  register float_t pt;
  
  vec3_t sum;
  vec3_zero(sum);
  
  for ( j = 0; j < num_Freq; j++) {
    pt = *ptr;
    sum[0] += *col1*pt;
    sum[1] += *col2*pt;
    sum[2] += *col3*pt;
    col1++;
    col2++;
    col3++;
    ptr++;
  }
  
  vec3_multiply(x, sum, Object_Scale);
}

void CDeformState::GetAccel(vec3_t x, int i) const
{
  int j;
  register float_t *col1 = data->mEigenVectors[i*3];
  register float_t *col2 = data->mEigenVectors[i*3+1];
  register float_t *col3 = data->mEigenVectors[i*3+2];
  register float_t *ptr = mode_accel;
  register float_t pt;
  
  vec3_t sum;
  vec3_zero(sum);
  
  for ( j = 0; j < num_Freq; j++) {
    pt = *ptr;
    sum[0] += *col1*pt;
    sum[1] += *col2*pt;
    sum[2] += *col3*pt;
    col1++;
    col2++;
    col3++;
    ptr++;
  }
  
  vec3_multiply(x, sum, Object_Scale);
}













CBody::CBody(const CBodyParameters& p) 
{
  const char* bodyname = p.name.c_str();
  char            statefilename[256], datafilename[256], temp[256];
  ifstream        fprop;
  int k;

  name = p.name;

  birthTime = -1;
  deathTime = -1;

  sprintf(statefilename, "models/%s.prop", bodyname);
  fprop.open(statefilename);
  if(!fprop) {
    cout << "Can't open property file " << statefilename << endl;
    exit(1);
  }


  fprop >> temp;
  sprintf(datafilename, "models/%s", temp);

  fprop >> Rigid_Body;
  fprop >> Deformable;
  Animated = 0;

  fprop >> Mass;
  for (k = 0; k < 3; k++ )
    fprop >> Com.data[k];

  for (k = 0; k < 3; k++ )
    for ( int l = 0; l < 3; l++ )
      fprop >> Inertia.data[k][l];

  // Prepare rigid state
  rigidBody            =      new ImpulseRigidBody();
  backupRigidState       =      new CRigidState;

  geometry        =      new CGeometry;

  //if ( Deformable ) {
    // Prepare Eigen stuff
    DeformObjectData* data = LoadDeformObjectData(datafilename);
    if(!data) {
      cout << "Can't open deformable object file " << datafilename << endl;
      exit(1);
    }

    // Prepare deform state
    deformState = new CDeformState;
    backupDeformState = new CDeformState;
    deformState->Init(data);
    *backupDeformState      =      *deformState;

    //data->PrecalculateResponse();
    geometry->object.Init(data, deformState);
    /*}
  else {
    //create a mesh buffer
    geometry->object = NULL;
    geometry->mesh_buffer = new surface_mesh;
    geometry->mesh_buffer->instantiate(&geometry->mesh.smesh);
    }*/


  refresh_vertices.init(geometry->object.NumVertices());

  double Force_Scalar, Lame_Scale, Phi, Psi;
  if ( Deformable ) {
    fprop >> Force_Scalar;
    fprop >> Lame_Scale;
    fprop >> Phi;
    fprop >> Psi;
  }
  else {
    double temp;
    fprop >> temp;
    fprop >> temp;
    fprop >> temp;
    fprop >> temp;
  }

  //KRIS: removed TimeStepThreshold, Use_Penalty

  fprop >> deformState->Object_Scale;
  fprop >> Penetration_Threshold;

  fprop >> Penalty_kp;
  fprop >> Penalty_kv;
  fprop >> Penalty_kb;

  fprop.close();

  //some of these things need to be scaled by the object scale, such as inertia
  Inertia.inplaceScale(deformState->Object_Scale*deformState->Object_Scale);
  invInertia.setInverse(Inertia);

  rigidBody->massSet(Mass);
  rigidBody->inertiaMatSet(Inertia);
  rigidBody->centerOfMassSet(Com*deformState->Object_Scale);

  if ( Deformable ) {
    //this doesnt really matter, just so long as it's there
    deformState->SetDeformRate(1.0/300.0);
    deformState->SetMaterialProperties(Lame_Scale, Phi, Psi);
  }

  //numLevels     =    (int)(log(timestep/MIN_TIME_STEP)/log(2));
  //Force_Last_N  =    1;
  //Phasor_Limit2 =    DBL_MAX;  


  //done with all the body inputting, set up the state

  setState(p.initialState);

  //get the world transformation matrix
  RigidTransform m = rigidBody->pos();
  m.R.inplaceScale(deformState->Object_Scale);
  geometry->TransformWorldCoords(m);
  geometry->InitTree();

  animationPath = NULL;
  if(!p.animationPath.empty()) {
    animationPath = new CRigidBodyAnimation;
    if(!animationPath->Load(p.animationPath.c_str())) {
      printf("Error loading animation %s\n", p.animationPath.c_str());
      exit(-1);
    }
    Animated = 1;
  }

  birthTime = p.birthTime;
  deathTime = p.deathTime;
}


CBody::~CBody() {
  if (geometry != NULL)
    delete geometry;
  if (rigidBody != NULL)
    delete rigidBody;
  if (backupRigidState != NULL)
    delete backupRigidState;
  if (deformState != NULL)
    delete deformState;
  if (backupDeformState != NULL)
    delete backupDeformState;
  if(animationPath != NULL)
    delete animationPath;
}

void CBody::setState(const CRigidState& state)
{
  rigidBody->posTrnSet(state.trnVec);
  rigidBody->posRotSet(state.rotMat);
  rigidBody->tVelSet(state.tVel);
  rigidBody->aVelSet(state.aVel);

  *backupRigidState = state;
}

void CBody::UpdateGeometry()
{
  RigidTransform m = rigidBody->pos();
  m.R.inplaceScale(deformState->Object_Scale);
  geometry->TransformWorldCoords(m);
  geometry->UpdateTree();
}

void CBody::Deform(double dt)
{
#ifndef NO_RB_DEFORM
  matrix4_t m;
  matrix4_identity(m);
  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++) {
      m[i][j] = rigidBody->rotMat()(i,j);
    }
  }
  Vector3 dv, dav, av;
  dv = rigidBody->tVel() - backupRigidState->tVel;
  dav = (rigidBody->aVel()*rigidBody->rotMat()) - (backupRigidState->aVel*backupRigidState->rotMat);
  av = rigidBody->aVel() * rigidBody->rotMat();

  geometry->object->AddRigidBodyForces(m, dv, av, dav);
#endif

  deformState->Update(dt);
}

void CBody::applyGravity()
{
  if(Rigid_Body) {
    if(Animated)
      return;

    rigidBody->applyForceAddCOM(Mass*gravity);
    /*
    if(sim_logging) {
      CBodyPoint pt;
      pt.n = 0;
      sim_log.LogForce(bodyIndex, pt, Mass*gravity, FTForce | FTCOM);
    }
    */
  }
  else if(Deformable) {
    CBodyPoint p;
    p.coords[0] = 1.0f;
    p.n = 1;
    for(int i=0; i<deformState->NumVertices(); i++) {
      p.vert[0] = i;
      ApplyForce_World_Deform(p, gravity);
      //sim_log.LogForce(bodyIndex, p, gravity, FTForce | FTDeform | FTWorld);
    }
  }
}


//HACK
extern CSimulation dyn_sim;

void	CBody::advance(double dTime) 
{
  if(Animated) {
    CRigidState state;
    animationPath->GetState(dyn_sim.sim_time + dTime, state);

    //the force and moment are just accelerations, we need them to be moments
    state.netFrc *= Mass;
    state.netMom = Inertia * state.netMom;

    *rigidBody = state;
  }
  if(Rigid_Body) {
    rigidBody->update(dTime);
  }
  if(Deformable) {
    Deform(dTime);
  }
  UpdateGeometry();
}

void	CBody::rollback() 
{
  // copyback rigid state
  *rigidBody = *backupRigidState;

  // copyback deformation state
  if ( Deformable )
    *deformState = *backupDeformState;

  UpdateGeometry();
}

void	CBody::commit() 
{
  // Copy rigid state
  *backupRigidState = *rigidBody;

  // commit deformation state
  if ( Deformable )
    *backupDeformState = *deformState;
}

void CBody::GetBodyPointPosition_Deform(const CBodyPoint& p, Vector3& out) const {
  if(deformState->vertex_state_updated) {
    out.set(0,0,0);
    for(int i=0; i<p.n; i++) {
      out += deformState->position[p.vert[i]]*p.coords[i];
    }
  }
  else {
    vec3_t o;
    vec3_t pos;
    vec3_zero(o);
    for(int i=0; i<p.n; i++) {
      deformState->GetPosition(pos, p.vert[i]);
      vec3_madd(o, pos, p.coords[i]);
    }
    out.set(o);
  }
}

void CBody::GetBodyPointVelocity_Deform(const CBodyPoint& p, Vector3& out) const {
  if(deformState->vertex_state_updated) {
    out.set(0,0,0);
    for(int i=0; i<p.n; i++) {
      out += deformState->velocity[p.vert[i]]*p.coords[i];
    }
  }
  else {
    vec3_t o;
    vec3_t pos;
    vec3_zero(o);
    for(int i=0; i<p.n; i++) {
      deformState->GetVelocity(pos, p.vert[i]);
      vec3_madd(o, pos, p.coords[i]);
    }
    out.set(o);
  }
}

void CBody::GetBodyPointAccel_Deform(const CBodyPoint& p, Vector3& out) const {
  if(deformState->vertex_state_updated) {
    out.set(0,0,0);
    for(int i=0; i<p.n; i++) {
      out += deformState->accel[p.vert[i]]*p.coords[i];
    }
  }
  else {
    vec3_t o;
    vec3_t pos;
    vec3_zero(o);
    for(int i=0; i<p.n; i++) {
      deformState->GetAccel(pos, p.vert[i]);
      vec3_madd(o, pos, p.coords[i]);
    }
    out.set(o);
  }
}

void CBody::GetBodyPointPosition_Local(const CBodyPoint& p, Vector3& out) const
{
  if(p.n == -1) { //it's a vector in world coords
    Vector3 pt (p.coords[0], p.coords[1], p.coords[2]);
    out = posToLocalPos(pt);
  }
  else { //its a linear combination of vertices
    GetBodyPointPosition_Deform(p, out);
  }
}

void CBody::GetBodyPointPosition_World(const CBodyPoint& p, Vector3& out) const
{
  if(p.n == -1) {
    out.set(p.coords[0], p.coords[1], p.coords[2]);
  }
  else {
    GetBodyPointPosition_Local(p, out);
    out = posToWorldPos(out);
  }
}

void CBody::GetBodyPointVelocity_Local(const CBodyPoint& p, Vector3& out) const
{
  if(Deformable) {
    GetBodyPointVelocity_Deform(p, out);
  }
  else {
    out = 0.0;
  }
}

void CBody::GetBodyPointVelocity_World(const CBodyPoint& p, Vector3& out) const
{
  //point state (accel, velocity, position)
  Vector3 d_v, d_p;
  if(Deformable) {
    GetBodyPointVelocity_Deform(p, d_v);
    GetBodyPointPosition_Deform(p, d_p);
  }
  else if (Rigid_Body) {
    GetBodyPointPosition_Local(p, d_p);
  }
  else {
    out = 0.0;
    return;
  }

  Vector3       v, omegal, com;
  Matrix3       R;

  R         =     rigidBody->rotMat();
  v         =     rigidBody->tVel();
  com       =     rigidBody->centerOfMass();

  R.mulTranspose(rigidBody->aVel(), omegal);
  //Rt        =     transpose(R);
  //omegaw    =     rigidBody->aVel();
  //omegal    =     Rt * omegaw;
  d_p -= com;

  if(Rigid_Body) {
    out = v - R*cross(d_p, omegal);
  }
  else {
    out = 0.0;
  }

#ifndef IGNORE_DEFORM
  if(Deformable) {
    out += R*d_v;
  }
#endif
}

void CBody::GetBodyPointAccel_Local(const CBodyPoint& p, Vector3& out) const
{
  //point state (accel, velocity, position)
  Vector3 d_a, d_v, d_p;
  if(Deformable) {
    GetBodyPointAccel_Deform(p, d_a);
    GetBodyPointVelocity_Deform(p, d_v);
    GetBodyPointPosition_Deform(p, d_p);
  }
  else if(Rigid_Body) {
    GetBodyPointPosition_Local(p, d_p);
  }
  else {
    out = 0.0;
    return;
  }

  Vector3       v, omegal, com;
  Matrix3       R;

  R         =     rigidBody->rotMat();
  v         =     rigidBody->tVel();
  com       =     rigidBody->centerOfMass();

  R.mulTranspose(rigidBody->aVel(), omegal);
  //Rt        =     transpose(R);
  //omegaw    =     rigidBody->aVel();
  //omegal    =     Rt * omegaw;
  d_p -= com;

  if(Rigid_Body) {
    //gravity + other forces
    out = rigidBody->netFrc()/rigidBody->mass();

    //moment around d_p
    out -= cross(d_p, invInertia * rigidBody->netMom());

    //torsion
    out += cross(d_p, (invInertia * cross(omegal, (Inertia * omegal))));
  }
  else {
    out = 0.0;
  }

#ifndef IGNORE_DEFORM
  if(Deformable) {
    //accel
    out += d_a;  
    
    //centrifugal
    //out += cross(omegal, cross(omegal,d_p));
    //coriolis
    //out += 2*cross(omegal, d_v);
    /*
    //centrifugal
    if(geometry->object->use_centrifugal_effects)
      out += cross(omegal, cross(omegal,d_p));
    //coriolis
    if(geometry->object->use_coriolis_effects)
      out += 2*cross(omegal, d_v);
    */
  }
#endif
}


void CBody::GetBodyPointAccel_World(const CBodyPoint& p, Vector3& out) const {
  GetBodyPointAccel_Local(p, out);
  out = vecToWorldVec(out);
}

void CBody::GetEigenvectorMatrixVelocity(const CBodyPoint& a, const CBodyPoint& b, Matrix3& out) const {
  float_t sum;
  int i,j,k;
  int x,y;
  matrix_type e = deformState->data->mEigenVectors;
  int num_Freq = deformState->num_Freq;
  for (i=0; i<3; i++) {
    for(j=0; j<3; j++) {

      sum = 0.0;
      for(k=0; k<num_Freq; k++) {
	float_t va = 0, vb = 0;
	for(int p=0; p<a.n; p++) {
	  x = a.vert[p]*3+i;
	  va += a.coords[p]*e[x][k];
	}
	for(int q=0; q<b.n; q++) {
	  y = b.vert[q]*3+j;
	  vb += b.coords[q]*e[y][k];
	}
	sum += va*vb;
      }
      out(i,j) = sum;
    }
  }
} 

void CBody::GetEigenvectorMatrixAccel(const CBodyPoint& a, const CBodyPoint& b, Matrix3& out) const {
  float_t sum;
  int i,j,k;
  int x,y;
  matrix_type e = deformState->data->mEigenVectors;
  vector_type v = deformState->data->vEigenValues;
  double Phi = deformState->Phi, Psi = deformState->Psi;
  int num_Freq = deformState->num_Freq;

  for (i=0; i<3; i++) {
    for(j=0; j<3; j++) {

      sum = 0.0;
      for(k=0; k<num_Freq; k++) {
	float_t va = 0, vb = 0;
	for(int p=0; p<a.n; p++) {
	  x = a.vert[p]*3+i;
	  va += a.coords[p]*e[x][k];
	}
	for(int q=0; q<b.n; q++) {
	  y = b.vert[q]*3+j;
	  vb += b.coords[q]*e[y][k];
	}
	sum += va*vb;
      }
      out(i,j) = sum;
    }
  }
} 


void CBody::GetForceResponse(const CBodyPoint& at, const CBodyPoint& force, Matrix3& out) const
{
  //point state (accel, velocity, position)
  Vector3 d_p;
  if(Deformable) {
    GetBodyPointPosition_Deform(at, d_p);
  }
  else if(Rigid_Body) {
    GetBodyPointPosition_Local(at, d_p);
  }
  else {
    out.setZero();
  }

  Vector3       omegal, com;
  Matrix3     M, R, Rt;
  Matrix3     d_x, omegalx;

  M.setZero();
  M(0,0) = M(1,1) = M(2,2) = Inv(rigidBody->mass());
  R         =     rigidBody->rotMat();
  com       =     rigidBody->centerOfMass();
  Rt.setTranspose(R);

  Rt.mul(rigidBody->aVel(), omegal);
  //omegaw    =     rigidBody->aVel();
  //omegal    =     Rt * omegaw;

  d_p -= com;

  omegalx.setCrossProduct(omegal);
  d_x.setCrossProduct(d_p);

  if(Rigid_Body && !Animated) {
    Vector3 dlj;
    Matrix3 dljx;

    GetBodyPointPosition_Local(force, dlj);
    dlj -= com;
    dljx.setCrossProduct(dlj);

    out = M - R * d_x * invInertia * dljx * Rt;
  }
  else {
    out.setZero();
  }

#ifndef IGNORE_DEFORM
  if(Deformable) {
    Matrix3 WiWj;
    GetEigenvectorMatrixAccel(at,force,WiWj);

    //divide and multiply on either side by Object_Scale
    out += R * WiWj * Rt;
  }
#endif
}


float_t CBody::GetNormalForceResponse(const CBodyPoint& at, const CBodyPoint& force, const Vector3& n) const
{
  float_t out = 0.0;

  //point state (accel, velocity, position)
  Vector3 d_p;
  if(Deformable) {
    GetBodyPointPosition_Deform(at, d_p);
  }
  else {
    GetBodyPointPosition_Local(at, d_p);
  }

  Vector3       omegal, com;
  Matrix3     R, Rt;

  R         =     rigidBody->rotMat();
  com       =     rigidBody->centerOfMass();
  Rt.setTranspose(R);

  Rt.mul(rigidBody->aVel(), omegal);
  //omegaw    =     rigidBody->aVel();
  //omegal    =     Rt * omegaw;

  d_p -= com;

  if(Rigid_Body) {
    Vector3 dlj;
    GetBodyPointPosition_Local(force, dlj);
    dlj -= com;

    //simplification of nt*A*n
    out = Inv(rigidBody->mass()) - dot(n, R * cross(d_p, invInertia * cross(dlj, Rt * n)));
  }

#ifndef IGNORE_DEFORM
  if(Deformable && !Animated) {
    Matrix3 WiWj;
    GetEigenvectorMatrixAccel(at,force,WiWj);

    out += dot(n, R * (WiWj * (Rt * n)));
    if(verbose >= 3)
      printf("nonrigid part: %f\n", out);
  }
#endif

#ifdef NORMAL_RESPONSE_CHECK
  Matrix3 mat;
  GetForceResponse(at,force,mat);
  if(fabs(out - dot(n,mat*n)) > 0.001) {
    printf("normal error %f vs:\n", out);
    cout<<mat;
    abort();
  }
#endif
  return out;
}

void CBody::GetImpulseResponse(const CBodyPoint& at, const CBodyPoint& force, Matrix3& out) const
{
  //point state (accel, velocity, position)
  Vector3 d_p;
  if(Deformable) {
    GetBodyPointPosition_Deform(at, d_p);
  }
  else if(Rigid_Body) {
    GetBodyPointPosition_Local(at, d_p);
  }
  else {
    out.setZero();
  }

  Vector3       omegal, com;
  Matrix3     M, R, Rt;
  Matrix3     d_x, omegalx;

  M.setZero();
  M(0,0) = M(1,1) = M(2,2) = Inv(rigidBody->mass());
  R         =     rigidBody->rotMat();
  com       =     rigidBody->centerOfMass();
  Rt.setTranspose(R);

  Rt.mul(rigidBody->aVel(), omegal);
  //omegaw    =     rigidBody->aVel();
  //omegal    =     Rt * omegaw;

  d_p -= com;

  omegalx.setCrossProduct(omegal);
  d_x.setCrossProduct(d_p);

  if(Rigid_Body && !Animated) {
    Vector3 dlj;
    Matrix3 dljx;

    GetBodyPointPosition_Local(force, dlj);
    dlj -= com;
    dljx.setCrossProduct(dlj);

    out = M - R * d_x * invInertia * dljx * Rt;
  }
  else {
    out.setZero();
  }

#ifndef IGNORE_DEFORM
  if(Deformable) {
    Matrix3 WiWj;
    GetEigenvectorMatrixVelocity(at,force,WiWj);

    //divide and multiply on either side by Object_Scale
    out += R * WiWj * Rt;
  }
#endif
}

float_t CBody::GetNormalImpulseResponse(const CBodyPoint& at, const CBodyPoint& force, const Vector3& n) const
{
  float_t out = 0.0;

  //point state (accel, velocity, position)
  Vector3 d_p;
  if(Deformable) {
    GetBodyPointPosition_Deform(at, d_p);
  }
  else {
    GetBodyPointPosition_Local(at, d_p);
  }

  Vector3       t, v, omegaw, omegal, com;
  Matrix3     R, Rt;

  t         =     rigidBody->trnVec();
  R         =     rigidBody->rotMat();
  v         =     rigidBody->tVel();
  omegaw    =     rigidBody->aVel();
  com       =     rigidBody->centerOfMass();
  Rt        =     transpose(R);

  omegal    =     Rt * omegaw;

  d_p -= com;

  if(Rigid_Body && !Animated) {
    Vector3 dlj;
    GetBodyPointPosition_Local(force, dlj);
    dlj -= com;

    //simplification of nt*A*n
    out = Inv(rigidBody->mass()) - dot(n, R * cross(d_p, invInertia * cross(dlj, Rt * n)));
  }

#ifndef IGNORE_DEFORM
  if(Deformable) {
    Matrix3 WiWj;
    GetEigenvectorMatrixVelocity(at,force,WiWj);

    out += dot(n, R * (WiWj * (Rt * n)));
  }
#endif

#ifdef NORMAL_RESPONSE_CHECK
  Matrix3 mat;
  GetForceResponse(at,force,mat);
  if(fabs(out - dot(n,mat*n)) > 0.001) {
    printf("normal error %f vs:\n", out);
    cout<<mat;
    abort();
  }
#endif
  return out;
}


void CBody::ApplyShift_World_RB(const CBodyPoint& p, const Vector3& force)
{
  Vector3 pos;
  GetBodyPointPosition_World(p, pos);
  rigidBody->applyShiftAdd(force, pos);
}

void CBody::ApplyShift_World_Deform(const CBodyPoint& p, const Vector3& force)
{
  Vector3 fl = force*rigidBody->rotMat();
  deformState->AddShift(fl, p);
}

void CBody::ApplyShift_World(const CBodyPoint& p, const Vector3& force)
{
  if(Rigid_Body && !Animated)
    ApplyShift_World_RB(p, force);
#ifndef IGNORE_DEFORM
  if(Deformable)
    ApplyShift_World_Deform(p, force);
#endif
}

void CBody::ApplyImpulse_World_RB(const CBodyPoint& p, const Vector3& force)
{
  Vector3 pos;
  GetBodyPointPosition_World(p, pos);
  rigidBody->applyImpulseAdd(force, pos);
}

void CBody::ApplyImpulse_World_Deform(const CBodyPoint& p, const Vector3& force)
{
  Vector3 fl = force*rigidBody->rotMat();
  deformState->AddImpulse(fl, p);
}

void CBody::ApplyImpulse_World(const CBodyPoint& p, const Vector3& force) {
  if(Rigid_Body && !Animated)
    ApplyImpulse_World_RB(p, force);
#ifndef IGNORE_DEFORM
  if(Deformable)
    ApplyImpulse_World_Deform(p, force);
#endif
} 

void CBody::ApplyForce_World_RB(const CBodyPoint& p, const Vector3& force) {
  Vector3 pos;
  GetBodyPointPosition_World(p, pos); 
  rigidBody->applyForceAdd(force, pos);
}

void CBody::ApplyForce_World_Deform(const CBodyPoint& p, const Vector3& force) {
  Vector3 fl = force*rigidBody->rotMat();
  deformState->AddForce(fl, p);
}

void CBody::ApplyForce_World(const CBodyPoint& p, const Vector3& force) {
  if(Rigid_Body && !Animated)
    ApplyForce_World_RB(p, force);
#ifndef IGNORE_DEFORM
  if(Deformable)
    ApplyForce_World_Deform(p, force);
#endif
} 


bool CBody::IsAlive(double t) const
{
  if(birthTime > t)
    return false;
  if(deathTime != -1.0 && t > deathTime)
    return false;
  return true;
}



void CBody::GetClosestBodypoint(const Vector3& pt, CBodyPoint& p) const
{
  float_t min_dist = FLT_MAX, temp_dist;
  vec4_t min_bary, temp_bary;
  int min_tet = -1;

  tet_ptr_element* t = geometry->object.tets.elements;
  int nt = geometry->object.tets.num_elements;
  for(int i=0; i<nt; i++, t++) {
    temp_dist = get_barycentric_coordinates(pt, *t, temp_bary);
    if(temp_dist < min_dist) {
      min_dist = temp_dist;
      vec4_equal(min_bary, temp_bary);
      min_tet = i;
    }
  }

  assert(min_tet != -1);


  for(int i=0; i<4; i++) {
    p.vert[i] = geometry->object.object_data->elements[min_tet][i];
  }
  p.n = 4;

  if(min_dist <= 1.001f) {  //it's inside the tet
    vec4_equal(p.coords, min_bary);
  }
  else {  //it's outside, so find a closest tet
    Vector3 a,b,c,d,  x;
    for(int i=0; i<3; i++) {
      a[i] = geometry->object.tets.elements[min_tet][0][i];
      b[i] = geometry->object.tets.elements[min_tet][1][i];
      c[i] = geometry->object.tets.elements[min_tet][2][i];
      d[i] = geometry->object.tets.elements[min_tet][3][i];
    }
    get_tetrahedron_closest_point(a,b,c,d,  pt,x);
    cout << "constraint point " << pt << endl;
    cout << "surface point " << x << endl;
    temp_dist = get_barycentric_coordinates(x, geometry->object.tets.elements[min_tet], p.coords);
    //cout << "Bary coords " << p.coords[0] << " " << p.coords[1] << " " << p.coords[2] << " " << p.coords[3] << endl;
    assert(FuzzyEquals(temp_dist, One));
  }
}
