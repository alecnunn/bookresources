/*********************************************************************
 * Simulation.cpp
 * Authored by Kris Hauser/Chen Shen 2002-2003
 *
 * Method definitions for the CSimulation class.
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
#include <signal.h>

#define SMALL 0.01
#define EPSILON 1e-4
#define MIN_TIME_STEP DBL_MIN


double kFriction = 1.0;

#include "env.h"
#include "SimHelpers.h"
#include "SimAnim.h"
#include "math/svd.h"
#include "math/bary.h"
#include "myarray.h"

const char* constraint_force_name(int type)
{
  switch(type) {
  case CONSTRAINT_POSITION:
    return "shift";
  case CONSTRAINT_VELOCITY:
    return "impulse";
  case CONSTRAINT_ACCEL:
    return "force";
  }
}


/********************COLLISION RESPONSE*********************/
/***********************CONSTRAINTS*************************/


/***************************
CSimulation::ApplyConstraintForce()
Given the solved response x[0...#dofs], applies the appropriate force
vector to apply.
This includes friction forces too.
***************************/
void CSimulation::ApplyConstraintForce(const CConstraint* p, float_t* x) {
  ApplyConstraintForce(p, ::ConstraintForce(p,x,FrictionScaleConstraint(p, CONSTRAINT_ACCEL)));
}

/***************************
CSimulation::ApplyConstraintForce()
Applies a force vector at a constraint point.
***************************/
void CSimulation::ApplyConstraintForce(const CConstraint* p, const Vector3& force)
{
  p->a->ApplyForce_World(p->p1, force);
  if(p->b) {
    p->b->ApplyForce_World(p->p2, -force);
  }
}

void CSimulation::ApplyConstraintImpulse(const CConstraint* p, float_t* x) {
  ApplyConstraintImpulse(p, ::ConstraintImpulse(p,x,FrictionScaleConstraint(p, CONSTRAINT_VELOCITY)));
}
 
void CSimulation:: ApplyConstraintImpulse(const CConstraint* p, const Vector3& force)
{    
  p->a->ApplyImpulse_World(p->p1, force);
  if(p->b) {
    p->b->ApplyImpulse_World(p->p2, -force);
  }
}




/***************************
CSimulation::Solve_Constraint_System()
Given the full nxn equation Ax=b, this solves a constraint system, where
penetration constraints must be > 0.  The constraint type parameter
specifies accel or velocity constraints.
A2 is a temporary matrix because the SVD erases the original matrix.
V is a temporary matrix to store the V matrix in the SVD
w is a temporary vector to store the singular values
***************************/
bool CSimulation::Solve_Constraint_System(matrix_type A, vector_type b, vector_type x, int n, int constraint_type,
	matrix_type A2, matrix_type V, vector_type w)
{
  if(n == 0)
    return true;

  int flag = 0;
  do {
    if(verbose >= 1)
      printf("Solving constraints for %d dofs\n", n);
    if(verbose >= 2) {
      printf("Response matrix\n");
      matrix_print(A,n,n);
      printf("Errors\n");
      vector_print(b,n);
    }
    matrix_equal(A2,A,n,n);
    svdecomp(A,n,n,w,V);
    svdbksub(A,w,V,n,n,  x,b);
    //svd_ls_solve(A,n,n,x,b);
    constraint_loops++;

    flag = 1;

    mylist <int> drops;
    int num_drops = 0;

    //check for negative penetration forces
    int i=0;
    CConstraint* p = NULL;
    while((p = constraints.next(p))) {
      if(!p->IsType(constraint_type))
	continue;
      if(p->IsPenetration()) {

	if(verbose >= 2)
	  printf("%s %d: %f\n",
		 constraint_force_name(constraint_type),
		 i, x[i]);

	if(x[i] < 0.0) { //negative impulse, drop it out
	  if(verbose >= 2)
	    printf("negative %s %d %f, dropping\n",
		   constraint_force_name(constraint_type),
		   i, x[i]);
	  p->constraint_type &= ~constraint_type;
	  flag = 0;
	  num_drops++;
	  drops.push_back(i);
	}
      }
      i+=p->NumDofs();
    }

    if(!flag) {
      if(n == num_drops) {
	printf("All collisions dropped in %s solve!!!\n", constraint_force_name(constraint_type));
	//if(verbose >= 1) {
	  printf("last A matrix:\n");
	  matrix_print(A2, n, n);
	  printf("last b vector:\n");
	  vector_print(b, n);
	  //}
	return false;
      }

      int* droplist = list_to_array(drops);
      drop_vector_entries(b,n,droplist,num_drops);
      drop_matrix_entries(A,A2,n,droplist,num_drops);
      delete [] droplist;

      n = n-num_drops;
    }
  } while(!flag);

  if(verbose >= 1)
    printf("%d constraints solved\n", n);
  return true;
}

void CSimulation::Constraint_Solve(double dtime)
{
  if(verbose >= 1)
    printf("solving constraints for %f seconds\n", dtime);

  if(collision_cluster_factor > 0.0)
    cleanCollisions();

  int ndofs = 0;
  CConstraint* c = NULL;
  while((c = constraints.next(c)))
    ndofs += c->NumDofs();

  matrix_type A = matrix_create(ndofs, ndofs);
  matrix_type A2 = matrix_create(ndofs, ndofs);
  matrix_type V = matrix_create(ndofs, ndofs);
  vector_type w = vector_create(ndofs);
  vector_type b = vector_create(ndofs);
  vector_type x = vector_create(ndofs);

  Vector3 force;
  int i;
  CConstraint* p;

#ifndef IGNORE_DEFORM
#ifndef NO_QUICK_UPDATE

  printf("Updating positions\n");

  //update positions, velocities to the current state
  p = NULL;
  while((p = constraints.next(p))) {
    for(i=0; i<p->p1.n; i++)
      p->a->refresh_vertices.insert(p->p1.vert[i]);
    if(p->b) {
      for(i=0; i<p->p2.n; i++)
	p->b->refresh_vertices.insert(p->p2.vert[i]);
    }
  }
  for(i=0; i<numBodies; i++) {
    if(bodies[i]->Deformable && bodies[i]->IsAlive()) {
      int nverts = bodies[i]->refresh_vertices.size();
      int* verts = bodies[i]->sorter.output();
      bodies[i]->deformState->RefreshModes();
      bodies[i]->deformState->RefreshSelectedPositions(verts, nverts);
      bodies[i]->deformState->RefreshSelectedVelocities(verts, nverts);
      bodies[i]->deformState->vertex_state_updated = true;
	  bodies[i]->refresh_vertices.clear();
    }
  }
  printf("done Updating positions\n");
#endif //NO_QUICK_UPDATE
#endif //IGNORE_DEFORM

#ifndef NO_POSITION_CONSTRAINTS
  /***************POSITION CONSTRAINTS******************/
  //no need to classify constraints, all of them are penetrating

  ndofs = Setup_PConstraints(A,b);
  if(!Solve_Constraint_System(A,b,x,ndofs,CONSTRAINT_POSITION,
			      A2,V,w)) {
#ifndef ALLOW_INFEASIBLE_CONSTRAINTS
    abort();
#endif //ALLOW_INFEASIBLE_CONSTRAINTS
  }
  else {
    if(verbose >= 1)
      printf("position constraints solved\n");

    //Constraints solved, now apply the impulses
    i=0;
    p = NULL;
    while((p = constraints.next(p))) {
      if(!p->IsType(CONSTRAINT_POSITION))
	continue;
      
      force = ConstraintShift(p, &x[i]);

      cout << "Position shift: " << force << endl;
      
      p->a->ApplyShift_World(p->p1, force);
      if(p->b) {
	p->b->ApplyShift_World(p->p2, -force);
      }
      
      i+=p->NumDofs();
    }
  }

#ifndef IGNORE_DEFORM
  //We need to update so we can get the proper positions
#ifndef NO_QUICK_UPDATE
  //update positions to the current state
  p = NULL;
  while((p = constraints.next(p))) {
    for(i=0; i<p->p1.n; i++)
      p->a->sorter.insert(p->p1.vert[i]);
    if(p->b) {
      for(i=0; i<p->p2.n; i++)
	p->b->sorter.insert(p->p2.vert[i]);
    }
  }
  for(i=0; i<numBodies; i++) {
    if(bodies[i]->Deformable && bodies[i]->IsAlive(sim_time)) {
      int nverts = bodies[i]->sorter.size();
      int* verts = new int[nverts];
      bodies[i]->sorter.output(verts);
      bodies[i]->sorter.clear();
      bodies[i]->deformState->RefreshModes();
      bodies[i]->deformState->RefreshSelectedPositions(verts, nverts);
      bodies[i]->deformState->vertex_state_updated = true;
      delete [] verts;
    }
  }
#else
  for(i=0; i<numBodies; i++) {
    if(bodies[i]->Deformable && bodies[i]->IsAlive(sim_time)) {
      bodies[i]->deformState->Refresh();
    }
  }
#endif //NO_QUICK_UPDATE
#endif //IGNORE_DEFORM
#endif // NO_POSITION_CONSTRAINTS
 
  /***************VELOCITY CONSTRAINTS******************/
  //classify constraints that we want to keep
  p = NULL;
  while((p = constraints.next(p))) {
    if(!p->IsPenetration())
      continue;
    
    float_t e = PenetrationVError(p);
    if(e > EPSILON) {        //V positive (i.e. constraint negative)
      if(verbose >= 2)
	printf("negative velocity constraint\n");
    }
    else if (e > -EPSILON) {  //near zero
      if(verbose >= 2)
	printf("static constraint\n");
      //Q: should we do this?
      //p->constraint_type |= CONSTRAINT_VELOCITY;
    }
    else {
      p->constraint_type |= CONSTRAINT_VELOCITY;
    }
  }
  
  ndofs = Setup_VConstraints(A,b);
  if(!Solve_Constraint_System(A,b,x,ndofs,CONSTRAINT_VELOCITY,
			      A2,V,w)) {
#ifndef ALLOW_INFEASIBLE_CONSTRAINTS
    abort();
#endif //ALLOW_INFEASIBLE_CONSTRAINTS
  }
  else {
    if(verbose >= 1)
      printf("velocity constraints solved\n");

    //Constraints solved, now apply the impulses
    i=0;
    p = NULL;
    while((p = constraints.next(p))) {
      if(!p->IsType(CONSTRAINT_VELOCITY))
	continue;
      
      ApplyConstraintImpulse(p, &x[i]);
      
      i+=p->NumDofs();
    }
  }

#ifndef NO_ACCEL_CONSTRAINTS

#ifndef IGNORE_DEFORM
  //We need to update so we can get the proper accels and such
#ifndef NO_QUICK_UPDATE
  //update velocities, accels to the current state
  p = NULL;
  while((p = constraints.next(p))) {
    for(i=0; i<p->p1.n; i++)
      p->a->sorter.insert(p->p1.vert[i]);
    if(p->b) {
      for(i=0; i<p->p2.n; i++)
	p->b->sorter.insert(p->p2.vert[i]);
    }
  }
  for(i=0; i<numBodies; i++) {
    if(bodies[i]->Deformable && bodies[i]->IsAlive(sim_time)) {
      int nverts = bodies[i]->sorter.size();
      int* verts = new int[nverts];
      bodies[i]->sorter.dumpsort(verts);
      bodies[i]->deformState->RefreshModes();
      bodies[i]->deformState->RefreshSelectedVelocities(verts, nverts);
      bodies[i]->deformState->RefreshSelectedAccels(verts, nverts);
      bodies[i]->deformState->vertex_state_updated = true;
      delete [] verts;
    }
  }
#else
  for(i=0; i<numBodies; i++) {
    if(bodies[i]->Deformable && bodies[i]->IsAlive(sim_time)) {
      bodies[i]->deformState->Refresh();
    }
  }
#endif //NO_QUICK_UPDATE
#endif //IGNORE_DEFORM

  /***************ACCLERATION CONSTRAINTS******************/

  //classify constraints that we want to keep
  p = NULL;
  while((p = constraints.next(p))) {
    if(!p->IsPenetration())
      continue;
    
    float_t e = PenetrationVError(p);
    if(e > EPSILON) {        //V positive (i.e. constraint negative)
      if(verbose >= 2)
	printf("good, the velocity is negative\n");
    }
    else if (e > -EPSILON) {  //near zero
      if(verbose >= 2)
	printf("static constraint\n");
      e = PenetrationAError(p);
      if(e >= -0.0) {
	if(verbose >= 2)
	  printf("negative accel constraint\n");
      }
      else {
	p->constraint_type |= CONSTRAINT_ACCEL;
      }
    }
    else {  //V negative, penetrating
      if(verbose >= 1) {
	printf("BAD!!! we didn't solve velocity correctly! %f\n", e);

	if(p->IsType(CONSTRAINT_VELOCITY)) {
	  printf("We were trying to solve for it, too!\n");
	}
      }

      //another accel constraint?
      e = PenetrationAError(p);
      if(e < 0.0) {
	p->constraint_type |= CONSTRAINT_ACCEL;
      }
    }
  }

  ndofs = Setup_AConstraints(A,b);
  if(!Solve_Constraint_System(A,b,x,ndofs,CONSTRAINT_ACCEL,
			      A2,V,w)) {
#ifndef ALLOW_INFEASIBLE_CONSTRAINTS
    abort();
#endif //ALLOW_INFEASIBLE_CONSTRAINTS
  }
  else {
    if(verbose >= 1)
      printf("accel constraints solved\n");

    //Constraints solved, now apply the forces
    i=0;
    p = NULL;
    while((p = constraints.next(p))) {
      if(!p->IsType(CONSTRAINT_ACCEL))
	continue;
      
      ApplyConstraintForce(p, &x[i]);
      
      i+=p->NumDofs();
    }
  }
  
#endif // NO_ACCEL_CONSTRAINTS

#ifdef RESPONSE_CHECK
  printf("\n");
  printf("*********RESPONSE CHECK*********\n");
  //should we revive all penetrations?
  /*
  p = NULL;
  while((p = constraints.next(p))) {
    if(p->IsPenetration()) {
      p->constraint_type |= CONSTRAINT_VELOCITY | CONSTRAINT_ACCEL;
    }
  }
  printf("Response check:\n");
  */
  for(int j=0; j<numBodies; j++) {
    if(bodies[j]->Deformable) {
      bodies[j]->deformState->Refresh();
    }
  }
  
#ifndef NO_POSITION_CONSTRAINTS
  ndofs = Setup_PConstraints(A,b);
  if(ndofs != 0) {
    cout << "new P errors: ";
    vector_print(b, ndofs);
  }
#endif // NO_POSITION_CONSTRAINTS
  ndofs = Setup_VConstraints(A,b);
  if(ndofs != 0) {
    cout << "new V errors: ";
    vector_print(b, ndofs);
  }
#ifndef NO_ACCEL_CONSTRAINTS
  ndofs = Setup_AConstraints(A,b);
  if(ndofs != 0) {
    cout << "new A errors: ";
    vector_print(b, ndofs);
  }
#endif // NO_ACCEL_CONSTRAINTS
  printf("*******END RESPONSE CHECK********\n");
  printf("\n");
#endif // RESPONSE_CHECK

  matrix_delete(A);
  matrix_delete(A2);
  matrix_delete(V);
  vector_delete(w);
  vector_delete(b);
  vector_delete(x);
}











int CSimulation::Setup_PConstraints(matrix_type A, vector_type b)
{
  Matrix3 m;
  int i=0;
  CConstraint* p = NULL;
  while((p = constraints.next(p))) {
    if(p->IsType(CONSTRAINT_POSITION)) {
      //FILL IN A
      //get the force reponse of p.a w.r.t. other constraints 
      int j=0;
      CConstraint* q = NULL;
      while((q = constraints.next(q))) {
	if(q->IsType(CONSTRAINT_POSITION)) {
	  SetPConstraints(A,i,j,p,q);
	  j+=q->NumDofs();
	}
      }

      SetPConstraints(b,i,p);

      i+=p->NumDofs();
    }
  }
  return i;
}

int CSimulation::Setup_VConstraints(matrix_type A, vector_type b)
{
  Matrix3 m;
  int i=0;
  CConstraint* p = NULL;
  while((p = constraints.next(p))) {
    if(p->IsType(CONSTRAINT_VELOCITY)) {
      //FILL IN A
      //get the force reponse of p.a w.r.t. other constraints 
      int j=0;
      CConstraint* q = NULL;
      while((q = constraints.next(q))) {
	if(q->IsType(CONSTRAINT_VELOCITY)) {
	  SetVConstraints(A,i,j,p,q);
	  j+=q->NumDofs();
	}
      }

      SetVConstraints(b,i,p);

      i+=p->NumDofs();
    }
  }
  return i;
}

int CSimulation::Setup_AConstraints(matrix_type A, vector_type b)
{
  Matrix3 m;
  int i=0;
  CConstraint* p = NULL;
  while((p = constraints.next(p))) {
    if(p->IsType(CONSTRAINT_ACCEL)) {
      //FILL IN A
      //get the force reponse of p.a w.r.t. other constraints 
      int j=0;
      CConstraint* q = NULL;
      while((q = constraints.next(q))) {
	if(q->IsType(CONSTRAINT_ACCEL)) {
	  SetAConstraints(A,i,j,p,q);
	  j+=q->NumDofs();
	}
      }

      SetAConstraints(b,i,p);

      i+=p->NumDofs();
    }
  }
  return i;
}




/********************COLLISION RESPONSE*********************/
/*************************PENALTY***************************/

void CSimulation::Penalty_Solve(double dtime)
{
  Vector3 v, vt;
  Vector3 f, ft;
  float_t vn, damping;
  float_t fn;
  vn = damping = fn = 0;

  CConstraint* p = NULL;
  while((p = constraints.next(p))) {
    if(p->IsPenetration()) {
      Vector3 n;
      n = p->fn.normal;

      //set v to relative velocity
      p->a->GetBodyPointVelocity_World(p->p1, v);
      if(p->b) {
	p->b->GetBodyPointVelocity_World(p->p2, vt);
	v = v-vt;
      }
    
      vn = dot(v,n);
      vt = v-vn*n;

      if(vn < 0) //going in
	damping = vn*p->a->Penalty_kv;
      else {
	damping = vn*p->a->Penalty_kb;
      }
    
      //printf("damping: %f\n", damping);
    
      //normal force
      fn = (p->pen *  p->a->Penalty_kp - damping);
      f = n*fn;
    
#ifndef NO_FRICTION
      ft = FrictionForce(p, vt, fn, FrictionScale(p));
      f += ft;
#endif
    
      ApplyConstraintForce(p, f);
    }
    else if (p->IsNormal()) {
      printf("ERROR: cannot use penalty forces with normal constraints\n");
    }
    else {
      Vector3 n;
      p->a->GetBodyPointPosition_World(p->p1, v);
      n = v;
      if(p->b) {
	p->b->GetBodyPointPosition_World(p->p2, v);
	n -= v;
      }
      n -= p->p;

      //set v to relative velocity
      p->a->GetBodyPointVelocity_World(p->p1, v);
      if(p->b) {
	p->b->GetBodyPointVelocity_World(p->p2, vt);
	v = v-vt;
      }

      damping = p->a->Penalty_kv*10;
    
      //printf("damping: %f\n", damping);
    
      //normal force (has length of n built in)
      fn = (p->a->Penalty_kp)*100;
      f = -n*fn - damping*v;

      ApplyConstraintForce(p, f);
    }
  }
}



//hack hack hack to make constraints stay put (instead of drifting)
void CSimulation::SpringForces()
{
  Vector3 v, vt;
  Vector3 f;
  float_t fn;
  fn = 0;

  CConstraint* p = NULL;
  while((p = constraints.next(p))) {
    if(p->IsPenetration()) {
#ifndef NO_SPRING_PENETRATIONS
      Vector3 n;
      n = p->fn.normal;
    
      //normal force
      fn = (p->pen *  p->a->Penalty_kp);
      f = n*fn;

      if(verbose >= 2)
	cout << "adding spring force " << f << endl;
      
      ApplyConstraintForce(p, f);

#endif // NO_SPRING_PENETRATIONS
    }
    else if (p->IsNormal()) {
      printf("ERROR: cannot use spring forces with normal constraints\n");
    }
    else {
      Vector3 n;
      p->a->GetBodyPointPosition_World(p->p1, v);
      n = v;
      if(p->b) {
	p->b->GetBodyPointPosition_World(p->p2, v);
	n -= v;
      }
      n -= p->p;

      //normal force (has length of n built in)
      fn = (p->a->Penalty_kp)*10;
      f = -n*fn;

      if(verbose >= 1)
	cout << "adding spring force " << f << endl;

      ApplyConstraintForce(p, f);
    }
  }
}




/*****************OTHER CSIMULATION METHODS****************/


CSimulation::CSimulation() 
{
  numBodies	=	0;
  maxBodies	=	100;

  typedef CBody *pCBody ;
  bodies	=	new pCBody[maxBodies];

  gravity.set(0,0,-9.8);

  sim_steps = 0;
  constraint_steps = 0;
  rollback_steps = 0;
  constraint_loops = 0;
  sim_time = 0.0;

  penetration_limit = 0.001;
  collision_cluster_factor = 0.2;
  collision_cluster_radius = 5.0;

  numPlanes = 0;
  planes = NULL;

  //drop_test();
}

CSimulation::~CSimulation() 
{
  if(bodies) {
    for(int i=0; i<numBodies; i++)
      delete bodies[i];
    delete [] bodies;
  }

  if(planes)
    delete [] planes;
}



void    CSimulation::readSetupFile(ifstream& fsetup)
{
    fsetup >> Use_Penalty;

    fsetup >> gravity;

    fsetup >> kFriction;

    fsetup >> penetration_limit;
    fsetup >> collision_cluster_factor;
    fsetup >> collision_cluster_radius;

    fsetup >> numBodies;
    if(bodies) {
      if (numBodies < maxBodies) {
	for(int i=0; i<numBodies; i++)
	  delete bodies[i];
	delete [] bodies;
	bodies = new CBody*[numBodies];
	maxBodies = numBodies;
      }
    }
    else {
      bodies = new CBody*[numBodies];
      maxBodies = numBodies;
    }
    for(int i=0; i<numBodies; i++) {
      CBodyParameters params;
      fsetup >> params;
      bodies[i] = new CBody(params);
      bodies[i]->gravity = gravity;
      bodies[i]->bodyIndex = i;
    }

    Vector3 dir, pt;
    fsetup >> numPlanes;
    planes = new Plane [numPlanes];
    for(int i=0; i<numPlanes; i++) {
      fsetup >> dir >> pt;
      planes[i].normal = dir;
      planes[i].offset = -dot(dir, pt);
    }

    int numConstraints;
    fsetup >> numConstraints;
    if(fsetup) {
      int bodya, bodyb;
      for(int i=0; i<numConstraints; i++) {
	fsetup >> bodya >> bodyb;
	fsetup >> pt;
	if(bodyb == -1)
	  AddJointConstraint(bodies[bodya], NULL, pt);
	else
	  AddJointConstraint(bodies[bodya], bodies[bodyb], pt);
      }
    }


#ifdef CONSTRAINT_TEST
    for(int i=0; i<numBodies; i++)
      TestConstraints(bodies[i]);
  exit(0);
#endif

}


void	CSimulation::addObject(CBody *object) 
{
  if (numBodies == maxBodies) {
    CBody	**newBodies;
    int		i;
    
    maxBodies	+=	100;
    typedef CBody *pCBody ;
    newBodies	 =	new pCBody[maxBodies];

    for (i=0;i<numBodies;i++)
      newBodies[i] = bodies[i];

    delete [] bodies;
    bodies = newBodies;
  }

  bodies[numBodies]	=     object;
  object->bodyIndex     =     numBodies;
  object->gravity       =     gravity;
  numBodies++;
}

#define MAX_ITERS 20

/*****************************
 * Simulation loop
 * while dt is not reached
 *   add gravity
 *   advance simulation (perhaps rolling back)
 *   solve collisions
 ****************************/
void	CSimulation::simulate(double dtime) 
{
  double total_time = dtime;

#ifdef ROLLBACK_CHECK
  int flag = 0;
  detectCollisions(flag);
  if(flag) {
    printf("Previous time step's collisions were not solved, aborting\n");
    abort();
  }
#endif

  while(total_time > 0.0) {
    dtime = total_time;
    int iters = 1;
    applyGravity();

#ifdef NO_ROLLBACK
    advance(dtime);
#else
    while(!advance(dtime)) {
      dtime *= 0.5;
      iters ++;
      if(iters > MAX_ITERS){
	printf("No convergence!\n");
	abort();
      }
    }
#endif //NO_ROLLBACK
    total_time -= dtime;
  }
}


void	CSimulation::addForces(CBody *body) 
{
  printf("This has been moved to CSimulation.advance()");
  abort();
}

void    CSimulation::applyGravity()
{
  int i;
  for(i=0; i<numBodies; i++)
    if(bodies[i]->IsAlive(sim_time))
       bodies[i]->applyGravity();
}

int	CSimulation::advance(double dtime) 
{
  int		i;
  int           release         =       1;

  for ( i = 0; i < numBodies; i++ ) { 
    if(bodies[i]->IsAlive(sim_time)) {
      // Integrate the simulation
      bodies[i]->advance(dtime);
    }
  }


  //see if it was a suitable advance
  int num;
  int flag = 0;
  num = detectCollisions(flag);

  if(num != 0) {
    printf("%d collisions detected\n", num);
  }

  //if(constraint_steps > 0)
  //abort();


#ifndef NO_ROLLBACK
  if(flag) {
    if(verbose >= 1)
      printf("penetration too large, rolling back...\n");
    for ( i = 0; i < numBodies; i++ )
      if(bodies[i]->IsAlive(sim_time))
	bodies[i]->rollback();
    rollback_steps++;
    return 0;
  }
#endif

  if(verbose >= 1)
    printf("Advanced by %f seconds...\n", dtime);

  sim_time += dtime;

  if ( Use_Penalty ) {
    Penalty_Solve(dtime);
  }
  else { // use constriants
    if ( !constraints.empty() )  {
      Constraint_Solve(dtime);
#ifdef SPRING_CONSTRAINTS
      SpringForces();
#endif
      constraint_steps ++;
    }
  }

  
#ifndef NO_ROLLBACK
  for ( i = 0; i < numBodies; i++ )
    if(bodies[i]->IsAlive(sim_time))
      bodies[i]->commit();
#endif

  sim_steps ++;
  return 1;
}

void CSimulation::clearCollisions() {
  if(verbose >= 3)
    printf("Clearing collisions\n");
  CConstraint* c = NULL;
  while((c = constraints.next(c))) {
    if(c->IsPenetration()) {
      c = constraints.erase(c);
    }
  }
}

int CSimulation::detectCollisions(int &flag)
{
  clearCollisions();
  if(verbose >= 3)
    printf("Detecting collisions\n");
  int i,j;
  int num = 0;
  //check vs other bodies
  for(i=0; i<numBodies; i++) {
    if(bodies[i]->IsAlive(sim_time)) {
      for(j=0; j<i; j++) {
	if(bodies[j]->IsAlive(sim_time))
	  num += Penetration_Check(bodies[i], bodies[j], flag);
      }
    }
  }
  //check vs planes
  for(i=0; i<numBodies; i++) {
    if(bodies[i]->IsAlive(sim_time)) {
      for(j=0; j<numPlanes; j++) {
	num += Plane_Penetration_Check(bodies[i], planes[j], flag);
      }
    }
  }

  //check to see if we need a rollback
  double penetration = 0.0;
  CConstraint* p = NULL;
  while((p = constraints.next(p))) {
    if(!p->IsPenetration())
      continue;
    
    penetration += p->pen;
    //if(p->pen > p->a->Penetration_Threshold)
    //flag = 1;
  }
  if(penetration - last_penetration > penetration_limit) {
    flag = 1;
  }
  else {
    last_penetration = penetration;
  }

  return num;
}


#include "kmeans.h"

void addconstraint(CConstraint& x, const CConstraint* c)
{
  x.p += c->p;
  x.fn.normal += c->fn.normal;
  x.pen += c->pen;
}

void divconstraint(CConstraint& x, double n)
{
  double a = 1.0/n;
  x.p *= a;
  normalize(x.fn.normal);
  //x.fn.normal *= a;
  //don't divide penetration
}

template <class type>
inline type bary_sum(const type bary[4]) {
  return bary[0] + bary[1] + bary[2] + bary[3];
}

template <class type>
inline void bary_equal(type x[4],  const type a[4]) {
  x[0] = a[0];
  x[1] = a[1];
  x[2] = a[2];
  x[3] = a[3];
}



float_t barycentric_coords(CBody* b, const int tet[4], const Vector3& p, float_t bary[4])
{
  tet_ptr_element element;
  float_t* buf = b->geometry->object.GetPositionBuffer();
  element[0] = &buf[tet[0]*3];
  element[1] = &buf[tet[1]*3];
  element[2] = &buf[tet[2]*3];
  element[3] = &buf[tet[3]*3];
  return get_barycentric_coordinates(p, element, bary);
}

//find the appropriate barycentric points of the new clusters
void assign_cluster_bodypoints(Vector3* clusters, CConstraint* newc,
			       CConstraint** oldc, int* membership, int n)
{
  int i;
  float_t coords[4];
  float_t dist, olddist;
  CConstraint* c;
  CConstraint* d;

  for(i=0; i<n; i++) {
    int clust = membership[i];
    if(clust != -1) {
      c = &newc[clust];  //c is new constraint
      d = oldc[i];       //d is old constraint

      //set the body to which the new constraint belongs
      //(Assume all constraints in a cluster belong to the same pair of objs)
      if(!c->a)
	c->a = d->a;
      else
	assert(c->a == d->a);

      //find the closest tet to which this vertex lies
      if(c->p1.n == 4) {
	olddist = bary_sum(c->p1.coords);
      }
      else {
	olddist = FLT_MAX;
      }

      //if the center already lies within a tet, let it be
      if(olddist > 1.0 + EPSILON) {
	//check if the center lies within this tet
	dist = barycentric_coords(d->a, d->p1.vert, clusters[clust], coords);
	if(dist < olddist) {
	  bary_equal(c->p1.vert, d->p1.vert);
	  bary_equal(c->p1.coords, coords);
	  c->p1.n = 4;
	}
      }

      //same for b
      if(d->b) {
	if(!c->b)
	  c->b = d->b;
	else
	  assert(c->b == d->b);

	if(c->p2.n == 4) {
	  olddist = bary_sum(c->p2.coords);
	}
	else {
	  olddist = FLT_MAX;
	}

	//if the center already lies within a tet, let it be
	if(olddist > 1.0 + EPSILON) {
	  //check if the center lies within this tet
	  dist = barycentric_coords(d->b, d->p2.vert, clusters[clust], coords);
	  if(dist < olddist) {
	    bary_equal(c->p2.vert, d->p2.vert);
	    bary_equal(c->p2.coords, coords);
	    c->p2.n = 4;
	  }
	}
      }
    }
  }
}


void CSimulation::cleanCollisions()
{
  CConstraint *c;

  int i=0, n=0;
  c = NULL;
  while((c = constraints.next(c))) {
    if(c->IsPenetration()) {
      n++;
    }
  }

  if(n == 0)
    return;

  int num_clusters = (int)(collision_cluster_factor * n);

  static myarray<Vector3> pts;
  static myarray<CConstraint*> ptdata;
  static myarray<Vector3> clusters;
  static myarray<int> membership;
  static myarray<int> nclustered;
  static myarray<CConstraint> newConstraints;

  pts.set_grow_size(50);
  ptdata.set_grow_size(50);
  clusters.set_grow_size(10);
  membership.set_grow_size(50);
  nclustered.set_grow_size(10);
  newConstraints.set_grow_size(10);

  pts.resize(n);
  ptdata.resize(n);
  clusters.resize(num_clusters);
  membership.resize(n);
  nclustered.resize(num_clusters);
  newConstraints.resize(num_clusters);

  i=0;
  c = NULL;
  while((c = constraints.next(c))) {
    if(c->IsPenetration()) {
      pts[i] = c->p;
      ptdata[i] = c;
      i++;
    }
  }

  if(verbose >= 0)
    printf("Cleaning collisions\n");

  mykmeans(pts, n, num_clusters, collision_cluster_radius, 3,
	 clusters, nclustered, membership);

  for(i=0; i<num_clusters; i++) {
    newConstraints[i].a = newConstraints[i].b = NULL;
    newConstraints[i].p1.n = newConstraints[i].p2.n = 0;
    newConstraints[i].constraint_type = CONSTRAINT_NORMAL | CONSTRAINT_PENETRATION;
    newConstraints[i].pen = 0.0;
    newConstraints[i].p.setZero();
    newConstraints[i].v.setZero();
    newConstraints[i].fn.normal.setZero();
    newConstraints[i].fn.offset = 0.0;
  }

  //add/average the new clusters
  int unclustered = 0;
  for(i=0; i<n; i++) {
    int c = membership[i];
    if(c != -1) {
      addconstraint(newConstraints[c], ptdata[i]);
    }
    else {
      unclustered ++;
    }
  }
  if(verbose >= 1)
    printf("%d unclustered penetrations\n", unclustered);
  for(i=0; i<num_clusters; i++) {
    if(nclustered[i] > 1)
      divconstraint(newConstraints[i], nclustered[i]);
    newConstraints[i].p = clusters[i];
  }

  assign_cluster_bodypoints(clusters, newConstraints, ptdata, membership, n);

  for(i=0; i<n; i++) {
    int c = membership[i];
    if(c != -1) {
      constraints.erase(ptdata[i]);
    }
  }

  for(i=0; i<num_clusters; i++) {
    if(nclustered[i] != 0) {
      //printf("new constraint bary %f %f %f %f\n", newConstraints[i].p1.coords[0], newConstraints[i].p1.coords[1] , newConstraints[i].p1.coords[2], newConstraints[i].p1.coords[3]);
      constraints.push_back(newConstraints[i]);
    }
  }

  if(verbose >= 0)
    printf("Reduced %d to %d collisions\n", n, constraints.size());
}




float_t CSimulation::FrictionScale(const CConstraint* c) const
{
  int num=0;
  CConstraint* p = NULL;
  while((p = constraints.next(p))) {
    if(c->b) {
      if((c->a == p->a && c->b == c->b) ||(c->a == p->b && c->b == c->a))
	num++;
    }
    else if(c->a == p->a && c->b == c->b)
      num++;
  }
  return 1.0/num;
}

float_t CSimulation::FrictionScaleConstraint(const CConstraint* c, int type) const
{
  int num=0;
  CConstraint* p = NULL;
  while((p = constraints.next(p))) {
    if(p->IsType(type)) {
      if(c->b) {
	if((c->a == p->a && c->b == c->b) ||(c->a == p->b && c->b == c->a))
	  num++;
      }
      else if(c->a == p->a && c->b == c->b)
	num++;
    }
  }
  return 1.0/num;
}















void CSimulation::AddJointConstraint(CBody* a, CBody* b, const Vector3& pt)
{
  constraints.push_back();
  CConstraint* c = constraints.back();
  c->constraint_type = CONSTRAINT_VELOCITY | CONSTRAINT_ACCEL | CONSTRAINT_POSITION;
  c->a = a;
  c->b = b;
  a->GetClosestBodypoint(pt,c->p1);
  if(b) {
    b->GetClosestBodypoint(pt,c->p2);

    Vector3 pa, pb;
    a->GetBodyPointPosition_World(c->p1, pa);
    b->GetBodyPointPosition_World(c->p2, pb);
    c->p = pa - pb;
  }
  else {
    Vector3 pa;
    a->GetBodyPointPosition_World(c->p1, pa);
    c->p = pa;
  }
}



ostream& operator << (ostream& out, const CRigidState& s)
{
  QuaternionRotation q;

  int k;
  for (k = 0; k < 3; k++ ) out << s.trnVec.data[k] << " ";
  out << endl;

  q.setMatrix(s.rotMat);
  for (k = 0; k < 4; k++ ) out << q.data[k] << " ";
  out << endl;

  for (k = 0; k < 3; k++ ) out << s.tVel.data[k] << " ";
  out << endl;

  for (k = 0; k < 3; k++ ) out << s.aVel.data[k] << " ";
  out << endl;

  return out;
}

istream& operator >> (istream& in, CRigidState& s)
{
    Vector3     P, V, AV;
    Matrix3   R;
    QuaternionRotation Q;
    
    int k;
    for (k = 0; k < 3; k++ ) in >> P.data[k];
    for (k = 0; k < 4; k++ ) in >> Q.data[k];
    for (k = 0; k < 3; k++ ) in >> V.data[k];
    for (k = 0; k < 3; k++ ) in >> AV.data[k];
    Q.getMatrix(R);

    s.trnVec = P;
    s.rotMat = R;
    s.tVel = V;
    s.aVel = AV;
    return in;
}

ostream& operator << (ostream& out, const CBodyPoint& p)
{
  out << p.n << endl;
  for(int i=0; i<p.n; i++) {
    out << p.vert[i] << " " << p.coords[i] << endl;
  }
  return out;
}

istream& operator >> (istream& in, CBodyPoint& p)
{
  in >> p.n;
  for(int i=0; i<p.n; i++) {
    in >> p.vert[i] >> p.coords[i];
  }
  return in;
}


ostream& operator << (ostream& out, const CBodyParameters& p)
{
  out << p.name;
  if(!p.animationPath.empty()) {
    out << " a";
  }
  out << endl;
  out << p.initialState; //endl
  out << p.birthTime << " " << p.deathTime << endl;
  if(!p.animationPath.empty()) {
    out << p.animationPath << endl;
  }
  return out;
}

istream& operator >> (istream& in, CBodyParameters& p)
{
  char str [256];
  str[0] = 0;
  while(str[0] == 0) {
    if(!in.getline(str, 256)) {
      printf("Error getting line for body's parameters\n");
      return in;
    }
  }
  p.name = str;

  int n = p.name.length();
  bool nostate = false, nodeath = false, anim = false;
  int nameend = -1;
  for(int i=0; i<n; i++) {
    if(isspace(p.name[i])) {
      nameend = i;
      break;
    }
  }
  if(nameend != -1) {
    printf("Found option at %d\n", nameend);
    for(int i=nameend+1; i<n; i++) {
      switch(p.name[i]) {
      case 'a':
	anim = true;
	break;
      default:
	cout<<"Error reading options: " << p.name.substr(i,n-i).c_str() << endl;
	in.set(ios::badbit);
	return in;
	break;
      }
    }

    p.name.resize(nameend);
  }

  if(nostate) {
    p.initialState.rotMat.setIdentity();
  }
  else {
    in >> p.initialState;
  }


  if(!nodeath) {
    in >> p.birthTime;
    in >> p.deathTime;
  }
  else {
    p.birthTime = p.deathTime = -1;
  }

  if(anim) {
    in >> p.animationPath;
  }
  
  return in;
}
