/*********************************************************************
 * rigidbody.h
 *
 * Rigid body objects.
 * Copied almost verbatim from James O'Brien's SM rigid body code,
 * Adapted to use my math structures.
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



#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "math/primitives.h"
#include "math/rotation.h"

class RigidBody {
  // Abstract base class to describe the location
  // of a rigid body.  Basicly a rotation and
  // a translation.  The rotation aligns the
  // object in the world system. The subsequent
  // translation moves it into the world system.
  //
  // A velocity matrix is also stored.  This is the
  // pos matrix/dt.  
  //
  // ie:
  // Xworld = POS * Xlocal
  // Vworld = VEL * Xlocal + POS.rot * Vlocal

public:

  //----------------------------------------------------------
  // Get access to the rot matrix and trans vec.
  // Changing these values is dependant on the 
  // derived class.

  const RigidTransform &pos() const { return _pos; }
  const RigidTransform &vel() const { return _vel; }

  const Matrix3 &rotMat() const { return _pos.R; }
  const Vector3 &trnVec() const { return _pos.t; }
  
  const Matrix3 &rotMatDt() const { return _vel.R; }
  const Vector3 &trnVecDt() const { return _vel.t; }

  //----------------------------------------------------------
  // Build in transforms.

  Vector3 posToWorldPos(const Vector3 &v) const; // inline
  Vector3 posToWorldVel(const Vector3 &v) const; // inline
  // Transforms a point in body coordinates to 
  // world coordinates. (pos or vel)

  Vector3 velToWorldVel(const Vector3 &v) const; // inline
  // Rotates a velocity into workd cordinates.
  
  //----------------------------------------------------------
  // Update for time varing objects.
  
  virtual double update(             ) = 0;
  virtual double update(double deltaT) = 0;
  // return value is the deltaT actually taken.

  //----------------------------------------------------------
  //----------------------------------------------------------

protected:
  RigidTransform _pos;
  RigidTransform _vel;
};

class DynamicsRigidBody : public RigidBody {
  // This class is actually a simple
  // simulation engine for a single
  // unconstrained rigid body.  If
  // no forces act on the object, then
  // it will keep a constant angular
  // momentum.  Force couples may be
  // applied to the object to mdify its
  // behavior in a dynamicly consistent way.
  //
  // Note that three refrence frames are relavent
  // for this object: world,graphics/local, and sim.
  // The graphic/local frame is related to the 
  // world by the pos and vel transforms.  The
  // sim frame is equivalent to the local but with
  // the center of mass translated to the origin.

public:


  //----------------------------------------------------------
  // Constructors... 

  DynamicsRigidBody();


  //----------------------------------------------------------
  // Member access.

  inline void tVelSet(const Vector3 &tVel);  // In world frame
  inline void aVelSet(const Vector3 &aVel);  // In world frame, about COM

  inline const Vector3 &tVel() const { return _tVel; } // In world frame
  inline const Vector3 &aVel() const { return _aVel; } // In world frame

  inline void posSet   (const RigidTransform &pos); // In world frame
  inline void posTrnSet(const Vector3    &trn); // In world frame
  inline void posRotSet(const Matrix3  &rot); // In world frame

  
  //----------------------------------------------------------
  // Mass properties (In local frame, rot about COM)

  inline void massSet(double mass);
  inline const double &mass() const { return _mass; }

  inline void centerOfMassSet(const Vector3 &com);
  inline const Vector3 &centerOfMass() const { return  _com; }

  inline void inertiaMatSet(const Matrix3 &I);
  inline const Matrix3 &inertiaMat() const { return _IMat; }

  //----------------------------------------------------------
  // Force/Moment accumulators.. 

  inline void applyCoupleSet(const Vector3 &frc, 
			     const Vector3 &mom,
			     const Vector3 &loc); // World frame
  inline void applyCoupleAdd(const Vector3 &frc,
			     const Vector3 &mom,
			     const Vector3 &loc); // World frame

  inline void  applyForceAdd(const Vector3 &frc,const Vector3 &loc); // World frame
  inline void applyMomentAdd(const Vector3 &mom,const Vector3 &loc); // World frame
			     

  inline void applyCoupleSetCOM(const Vector3 &frc, const Vector3 &mom); // World frame @COM
  inline void applyCoupleAddCOM(const Vector3 &frc, const Vector3 &mom); // World frame @COM
  
  inline void  applyForceAddCOM(const Vector3 &frc); // World frame @COM
  inline void applyMomentAddCOM(const Vector3 &mom); // World frame @COM


  //----------------------------------------------------------
  // Following methods use parameters in SIM space, NOT in world!

  inline void setForce(const Vector3 &frc) { _frc  = frc; } // sim frame
  inline void addForce(const Vector3 &frc) { _frc += frc; } // sim frame
  
  inline void setMoment(const Vector3 &mom) { _mom  = mom; } // sim frame
  inline void addMoment(const Vector3 &mom) { _mom += mom; } // sim frame

  inline const Vector3 &netFrc() const { return _frc; } // sim frame
  inline const Vector3 &netMom() const { return _mom; } // sim frame
  // Does not include object inertial forces.
  
  //----------------------------------------------------------
  // Minor constraints.

  inline void posLockSet(int allowTrn);
  inline void rotLockSet(int allowRot);
  // Does not clear the tVel or the aVel but _tVel will not
  // be effected by "locked velocities".

  inline int isPosLockSet() const { return _posLock; }
  inline int isRotLockSet() const { return _rotLock; }


  //----------------------------------------------------------
  // virtual overides from parent.
  // Does NOT clear force or moment accumulators!

  virtual double update(             );
  virtual double update(double deltaT);
  // Uses a simple euler integrator
  // Also clears the frc and mom accumulators.

protected:

  void recomputeVelMat();


  // Varible which describe system state
  // (include) _pos and _vel from parent.
  Vector3   _tVel;  // World frame
  Vector3   _aVel;  // World frame

  double      _mass;  
  Vector3   _com ;     // local frame
  Matrix3 _IMat;     // local frame about COM
  Matrix3 _IMatInv;  // local frame about COM (inverse of IMat)

  // Force and moment about COM accumulators
  Vector3 _frc;     // Sim frame
  Vector3 _mom;     // Sim frame

  // Constrints on pos or rot...
  int _posLock; 
  int _rotLock; 
};


//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
// Inline functions....


// RigidBody

inline
Vector3 RigidBody::posToWorldPos(const Vector3 &v) const {
  return _pos * v;
}


inline
Vector3 RigidBody::posToWorldVel(const Vector3 &v) const {
  return _vel * v;
}


inline
Vector3 RigidBody::velToWorldVel(const Vector3 &v) const {
  return _pos.R * v;
}


//-------------------------------------------------------------------

// DynamicsRigidBody

inline void DynamicsRigidBody::tVelSet(const Vector3 &tVel) {
  _tVel = tVel;
  recomputeVelMat();
}

inline void DynamicsRigidBody::aVelSet(const Vector3 &aVel) {
  _aVel = aVel;
  recomputeVelMat();
}

inline void DynamicsRigidBody::posSet(const RigidTransform &pos) {
  _pos = pos;
  recomputeVelMat();
}

inline void DynamicsRigidBody::posTrnSet(const Vector3   &trn) {
  _pos.t = trn;
  //recomputeVelMat(); 
}

inline void DynamicsRigidBody::posRotSet(const Matrix3 &rot) {
  _pos.R = rot;
  recomputeVelMat();
}

inline void DynamicsRigidBody::massSet(double mass) {
  _mass = mass;
}

inline void DynamicsRigidBody::centerOfMassSet(const Vector3 &com) {
  _com = com;
  recomputeVelMat();
}
  

inline void DynamicsRigidBody::inertiaMatSet(const Matrix3 &I) {
  _IMat    =         I ;
  _IMatInv.setInverse(I);
}

inline void DynamicsRigidBody::applyCoupleSet(const Vector3 &frc, 
						const Vector3 &mom,
						const Vector3 &loc) {
  _frc = 0.0;
  _mom = 0.0;
  applyCoupleAdd(frc,mom,loc);
}

inline void DynamicsRigidBody::applyCoupleAdd(const Vector3 &frc,
						const Vector3 &mom,
						const Vector3 &loc) {
  // parameter are in World frame <O>
  const Matrix3 invR = transpose(_pos.R);
  const Vector3 dstW = loc-_pos*_com;
  const Vector3 frcL = invR*frc;
  _frc += frcL;
  _mom += invR*(mom + cross(dstW,frc));
}


inline void DynamicsRigidBody::applyForceAdd(const Vector3 &frc,const Vector3 &loc) {
  // parameter are in World frame
  const Matrix3 invR = transpose(_pos.R);
  const Vector3 locL = invR*(loc-_pos.t);
  const Vector3 frcL = invR*frc;
  
  _frc += frcL;
  _mom += cross(locL-_com,frcL);
}


inline void DynamicsRigidBody::applyMomentAdd(const Vector3 &mom,const Vector3 &loc) {
  // parameter are World frame
  const Matrix3 invR = transpose(_pos.R);
  const Vector3 momL = invR*mom;
  _mom += momL;
}
			     

inline void DynamicsRigidBody::applyCoupleSetCOM(const Vector3 &frc, const Vector3 &mom) {
  // parameters are World frame @COM
  _frc = 0.0;
  _mom = 0.0;
  applyCoupleAddCOM(frc,mom);
}

inline void DynamicsRigidBody::applyCoupleAddCOM(const Vector3 &frc, const Vector3 &mom) {
  // parameters are World frame @COM
  const Matrix3 invR = transpose(_pos.R);
  const Vector3 frcL = invR*frc;
  const Vector3 momL = invR*mom;
  _frc += frcL;
  _mom += momL;
}
  
inline void DynamicsRigidBody::applyForceAddCOM(const Vector3 &frc) {
  // parameters are World frame @COM
  const Matrix3 invR = transpose(_pos.R);
  const Vector3   frcL = invR*frc;
  _frc += frcL;
}
 
inline void DynamicsRigidBody::applyMomentAddCOM(const Vector3 &mom) {
  // parameters are World frame @COM
  const Matrix3 invR = transpose(_pos.R);
  const Vector3 momL = invR*mom;
  _mom += momL;
}


inline void DynamicsRigidBody::posLockSet(int allowTrn) {
  _posLock = allowTrn;
  recomputeVelMat();
}
  
inline void DynamicsRigidBody::rotLockSet(int allowRot) {
  _rotLock = allowRot;
  recomputeVelMat();
}


#endif
