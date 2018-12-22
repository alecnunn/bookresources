/*********************************************************************
 * Simulation.h
 * Authored by Kris Hauser/Chen Shen 2002-2003
 *
 * Headers for the objects in the Sim___.C files, including the main
 * simulation core, and simulated bodies.
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




#ifndef SIMULATION_H
#define SIMULATION_H

#include <fstream.h>
#include <strstream.h>
#include <values.h>

//-------------------------------------------------------------------
#include "modal/deformobj.h"
#include "math/primitives.h"
#include "math/rotation.h"
#include "rigidbody.h"
#include "bounds.h"
#include "nodup.h"
#include <string>

//-------------------------------------------------------------------

class CRigidBodyAnimation;
struct CRigidState;


/*
 * Behavior Flags
 *
 * IGNORE DEFORM does everything with RB
 * RESPONSE CHECK checks to see if the constraint response is correct
 * NORMAL_RESPONSE_CHECK checks to see if the normal constraint response
 *    equations correspond to the free constraint equations
 * UPDATE_VERTEX_STATE refreshes the position,velocity,accel of all nodes
 *    every time step rather than per-request
 * ALLOW_INFEASIBLE_CONSTRAINTS doesn't crash if all collisions are dropped
 * NO_ROLLBACK restricts rollback from happening
 * ROLLBACK_CHECK checks for non-interpenetration at the beginning of
 *    every frame
 * NO_POSITION_CONSTRAINTS (leave on) turns off position constraints.
 *    Leave this on because they're bad!
 * NO_ACCEL_CONSTRAINTS turns off acceleration constraints
 * NO_FRICTION turns off friction
 * NO_RB_DEFORM (leave on) this turns off the mode rb forcing
 * NO_QUICK_UPDATE (leave on) this is a possibly quicker way to find
 *    which nodes need to be updated before constraint solving (buggy now)
 * SPRING_CONSTRAINTS combines penalty methods with constraint methods.
 * NO_SPRING_PENETRATIONS doesn't put penalty forces on penetrations if
 *    SPRING_CONSTRAINTS is on.
 * CONSTRAINT_TEST tests constraints, then quits
 */

//#define IGNORE_DEFORM
//#define RESPONSE_CHECK
//#define NORMAL_RESPONSE_CHECK
#define UPDATE_VERTEX_STATE
//#define ALLOW_INFEASIBLE_CONSTRAINTS
//#define NO_ROLLBACK
//#define ROLLBACK_CHECK
#define NO_POSITION_CONSTRAINTS
//#define NO_ACCEL_CONSTRAINTS
//#define NO_FRICTION
#define NO_RB_DEFORM
#define NO_QUICK_UPDATE
#define SPRING_CONSTRAINTS
//#define NO_SPRING_PENETRATIONS
//#define CONSTRAINT_TEST

extern int verbose;
extern double kFriction;





struct ImpulseRigidBody : public DynamicsRigidBody
{
  void operator = (const CRigidState& state);
  void applyImpulseAdd(const Vector3& frc, const Vector3& loc);
  void applyShiftAdd(const Vector3& frc, const Vector3& loc);
};

///////////////////////////////////////////////////////////////////////
// Class			:	CRigidState
// Description			:	This class holds all the rigid-body state variables
// Comments			:
// Date last edited		:	7/01/2002
class	CRigidState {
public:
  void operator = (const ImpulseRigidBody& body);

  Vector3                     trnVec;
  Matrix3                     rotMat;
  Vector3                     tVel;
  Vector3                     aVel;
  Vector3                     netFrc;
  Vector3                     netMom;
};



///////////////////////////////////////////////////////////////////////
// Class			:	CGeometry
// Description			:	This class defines a geometry that will
//					be used during the collisition detection
// Comments			:	
// Date last edited		:	7/01/2002
class	CGeometry {
public:
                                CGeometry();
  virtual                       ~CGeometry();

  void TransformWorldCoords(const RigidTransform& local_to_world);

  void Draw();
  void InitTree();
  void UpdateTree();

  DeformObject object;
  
  BoundTree                   TetTree;
};


class CBodyPoint
{
public:
  //some point on this object whose position is a linear combination of
  //vertices, e.g. barycentric coords
  int vert [4];
  float_t coords [4];
  int n;                  //# of vertices/coords used
};

  
///////////////////////////////////////////////////////////////////////
// Class			:	CDeformState
// Description		:	This class subclasses the deformation state and stores some extra data
// Comments			:
// Date last edited		:	9/03/2002
///////////////////////////////////////////////////////////////////////

struct CDeformState : public myDeformState
{
  void Init(DeformData*);
  void Cleanup();
  void Refresh();
  void RefreshModes();
  void RefreshSelected(int* v, int n);
  void RefreshSelectedPositions(int* v, int n);
  void RefreshSelectedVelocities(int* v, int n);
  void RefreshSelectedAccels(int* v, int n);
  void Update(float_t dt);
  void operator = (const CDeformState&);

  void GetPosition(vec3_t x, int i) const;
  void GetVelocity(vec3_t x, int i) const;
  void GetAccel(vec3_t x, int i) const;

  void AddForce(const Vector3& force, const CBodyPoint& p);
  void AddImpulse(const Vector3& force, const CBodyPoint& p);
  void AddShift(const Vector3& force, const CBodyPoint& p);

  vector_type mode_velocity;
  vector_type mode_accel;
  vector_type ext_mode_forces;
  
  Vector3 *position;           // deformed position
  Vector3 *velocity;           // velocities
  Vector3 *accel;              // accels = Lambda*Z + Q*Zd

  struct Force
  {
    CBodyPoint p;
    Vector3 f;
  };
  mylist<Force>forces;

  double                        Object_Scale;
  bool vertex_state_updated;
};


///////////////////////////////////////////////////////////////////////
// Class			:	CBodyParameters
// Description			:	The initial parameters of a body
// Comments			:
// Date last edited		:	7/01/2002
struct CBodyParameters
{
  string                                name;
  CRigidState                           initialState;
  double                                birthTime, deathTime;
  string                                animationPath;
};

///////////////////////////////////////////////////////////////////////
// Class			:	CBody
// Description			:	The class for object in the scene
// Comments			:
// Date last edited		:	7/01/2002
class	CBody {
public:

  CBody(const CBodyParameters&);
  ~CBody();

  void                                  setState(const CRigidState&);
  void		                        advance(double dTime);
  void                                  applyGravity();
  void                                  BackupDeformState();
  void		                        rollback();
  void		                        commit();

  void                                  Deform(double dt);
  void                                  UpdateGeometry();

  //helper functions
  inline Vector3 posToWorldPos(const Vector3& p) const {
    return rigidBody->posToWorldPos(p);
  }
  inline Vector3 posToLocalPos(const Vector3& p) const {
    return (p-rigidBody->trnVec())*rigidBody->rotMat();
  }
  inline Vector3 vecToWorldVec(const Vector3& p) const {
    return rigidBody->rotMat()*p;
  }
  inline Vector3 vecToLocalVec(const Vector3& p) const {
    return p*rigidBody->rotMat();
  }

  //deform coords: no rigid body stuff
  void GetBodyPointPosition_Deform(const CBodyPoint&, Vector3& out) const;
  void GetBodyPointVelocity_Deform(const CBodyPoint&, Vector3& out) const;
  void GetBodyPointAccel_Deform(const CBodyPoint&, Vector3& out) const;
  //local coords: deform + RB
  void GetBodyPointPosition_Local(const CBodyPoint&, Vector3& out) const;
  void GetBodyPointVelocity_Local(const CBodyPoint&, Vector3& out) const;
  void GetBodyPointAccel_Local(const CBodyPoint&, Vector3& out) const;
  //world: local -> world
  void GetBodyPointPosition_World(const CBodyPoint&, Vector3& out) const;
  void GetBodyPointVelocity_World(const CBodyPoint&, Vector3& out) const;
  void GetBodyPointAccel_World(const CBodyPoint&, Vector3& out) const;
  
  void GetEigenvectorMatrixVelocity(const CBodyPoint& a, const CBodyPoint& b, Matrix3& out) const;
  void GetEigenvectorMatrixAccel(const CBodyPoint& a, const CBodyPoint& b, Matrix3& out) const;
  void GetForceResponse(const CBodyPoint& at, const CBodyPoint& force, Matrix3& mat) const;
  float_t GetNormalForceResponse(const CBodyPoint& at, const CBodyPoint& force, const Vector3& normal) const;
  void GetImpulseResponse(const CBodyPoint& at, const CBodyPoint& force, Matrix3& mat) const;
  float_t GetNormalImpulseResponse(const CBodyPoint& at, const CBodyPoint& force, const Vector3& normal) const;

  void ApplyForce_World(const CBodyPoint&, const Vector3& force);
  void ApplyForce_World_RB(const CBodyPoint&, const Vector3& force);
  void ApplyForce_World_Deform(const CBodyPoint&, const Vector3& force);

  void ApplyImpulse_World(const CBodyPoint&, const Vector3& force);
  void ApplyImpulse_World_RB(const CBodyPoint&, const Vector3& force);
  void ApplyImpulse_World_Deform(const CBodyPoint&, const Vector3& force);

  void ApplyShift_World(const CBodyPoint&, const Vector3& force);
  void ApplyShift_World_RB(const CBodyPoint&, const Vector3& force);
  void ApplyShift_World_Deform(const CBodyPoint&, const Vector3& force);

  bool IsAlive(double t) const;
  void GetClosestBodypoint(const Vector3&, CBodyPoint&) const;

  string                                name;

  int			                bodyIndex;

  CGeometry                             *geometry;
  
  Vector3                             gravity;              // gravity
  
  double                                Mass;
  Vector3                             Com;
  Matrix3                             Inertia;
  Matrix3                             invInertia;

  ImpulseRigidBody                      *rigidBody;
  CRigidState		                *backupRigidState;
  CDeformState		                *deformState;
  CDeformState		                *backupDeformState;
  CRigidBodyAnimation                   *animationPath;

  //int                                   numLevels; // subdivid the time step for rolling back
  //int                                   Force_Last_N;
  //double                                Phasor_Limit2;

  double                                Penetration_Threshold;
  //double                                TimeStep_Threshold;

  double                                birthTime, deathTime;

  double                                Penalty_kp;
  double                                Penalty_kv;
  double                                Penalty_kb;

  int                                   Rigid_Body;
  int                                   Deformable;
  int                                   Animated;

  duplicate_remover                     refresh_vertices;
};


#define CONSTRAINT_POSITION 0x1
#define CONSTRAINT_VELOCITY 0x2
#define CONSTRAINT_ACCEL 0x4

#define CONSTRAINT_NORMAL 0x10

#define CONSTRAINT_PENETRATION 0x8000

// Class			:	CConstraint
// Description			:	This class is for a world-space constraint
// Comments			:
// Date last edited		:	9/03/2002
class CConstraint
{
public:
  CBody *a, *b;        //constrained bodies
  Vector3 p;         //point in world space
  Vector3 v;         //constraint velocity
  CBodyPoint p1, p2;   //point on the body

  int constraint_type;
  Plane fn;          //face normal for NORMAL constraints 
  double pen;          //penetration amount, for PENETRATION constraints

  inline int IsType(int type) const { return constraint_type & type; }
  inline int IsNormal() const { return constraint_type & CONSTRAINT_NORMAL; }
  inline int IsPenetration() const { return constraint_type & CONSTRAINT_PENETRATION; }
  inline int NumDofs() const { return (IsNormal() ? 1 : 3); }
};




///////////////////////////////////////////////////////////////////////
// Class			:	CSimulation
// Description			:	Obvious
// Comments			:
// Date last edited		:	7/01/2002
class	CSimulation {
public:
                                CSimulation();
  virtual                       ~CSimulation();

  void                          readSetupFile(ifstream&);

  void	                        addObject(CBody *);
  void	                        addForces(CBody *);	// Called by the simulator to accumulate forces on the object
  void		                simulate(double);

  void Constraint_Solve(double dt);
  void Penalty_Solve(double dt);

  int		                numBodies,maxBodies;		// The bodies array
  CBody                  	**bodies;
  
  Vector3                       gravity;

  int Use_Penalty;
  mylist<CConstraint> constraints;

  Plane* planes;
  int numPlanes;

  BoundOverlapServer          OS;

  //statistics
  double sim_time;
  int sim_steps;
  int constraint_steps;
  int rollback_steps;
  int constraint_loops;

  double penetration_limit;
  double last_penetration;
  double collision_cluster_factor;  //factor to reduce # of collisions by
  double collision_cluster_radius;  //radius of a cluster 
  
private:
  //helpers
  int advance(double);
  void applyGravity();
  void clearCollisions();
  int detectCollisions(int &flag);
  void cleanCollisions();
  int Penetration_Check(CBody* a, CBody* b, int& flag);
  int Plane_Penetration_Check(CBody* a, const Plane& p, int& flag);

  //constraint methods
  bool Solve_Constraint_System(matrix_type A, vector_type b, vector_type x, int n, int constraint_type,
			       matrix_type A2, matrix_type V, vector_type w);
  int Setup_PConstraints(matrix_type A, vector_type b);
  int Setup_VConstraints(matrix_type A, vector_type b);
  int Setup_AConstraints(matrix_type A, vector_type b);

  //these two versions apply the force calculated from the vector
  //that the constraint solver solves
  void ApplyConstraintForce(const CConstraint* p, float_t* x);
  void ApplyConstraintImpulse(const CConstraint* p, float_t* x);
  //these two apply the forces directly
  void ApplyConstraintForce(const CConstraint* p, const Vector3& x);
  void ApplyConstraintImpulse(const CConstraint* p, const Vector3& x);

  float_t FrictionScale(const CConstraint* c) const;
  float_t FrictionScaleConstraint(const CConstraint* c, int type) const;

  void AddJointConstraint(CBody* a, CBody* b, const Vector3& pt);

  void SpringForces();
};


ostream& operator << (ostream& out, const CRigidState& s);
istream& operator >> (istream& in, CRigidState& s);

ostream& operator << (ostream& out, const CBodyPoint& p);
istream& operator >> (istream& in, CBodyPoint& p);

ostream& operator << (ostream& out, const CBodyParameters& p);
istream& operator >> (istream& in, CBodyParameters& p);

#endif














