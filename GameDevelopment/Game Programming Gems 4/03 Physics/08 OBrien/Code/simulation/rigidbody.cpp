/*********************************************************************
 * rigidbody.cpp
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



#include "rigidbody.h"
#include <stdlib.h>

//-------------------------------------------------------------------
//-------------------------------------------------------------------

// DynamicsRigidBody


DynamicsRigidBody::DynamicsRigidBody() {
  _pos.setIdentity();
  _IMat   .setIdentity();
  _IMatInv.setIdentity();
  _mass = 1.0;
  _com  = 0.0;
  _tVel = 0.0;
  _aVel = 0.0;
  _frc  = 0.0;
  _mom  = 0.0;
  _posLock = 0;
  _rotLock = 0;
  recomputeVelMat(); 
}

  

double DynamicsRigidBody::update() {
  recomputeVelMat();
  return 0.0;
}



double DynamicsRigidBody::update(double deltaT) {

  if (!_posLock) {
    Vector3 accW =  _pos.R * ( _frc  / _mass );
    Vector3 velW = _tVel + accW * deltaT;

    _pos.t += ( _tVel + velW ) * (0.5 * deltaT);
    _tVel = velW;

  }
  
  if (!_rotLock) {
    Matrix3 pInv;
    pInv.setTranspose(_pos.R);

    Vector3 w  = (pInv*_aVel);   // rot vel in local frame
    Vector3 H  = (_IMat*w   );   // rot momentum in local frame
    Vector3 dH = (_mom-cross(w,H));  

    Vector3 accW = (_pos.R*_IMatInv*dH);
    Vector3 velW = ( _aVel + accW * deltaT );

    MomentRotation dRotV = ((_aVel + velW )*( 0.5 * deltaT ));
    _aVel = velW;

    Matrix3 dRotM;
    dRotV.getMatrix(dRotM);
    Matrix3 newRot;
    newRot.mul(dRotM,_pos.R);

    _pos.t += _pos.R*_com - newRot*_com;
    _pos.R = newRot;

  }

  recomputeVelMat();

  _frc = 0.0;
  _mom = 0.0;

  return deltaT;
}

void DynamicsRigidBody::recomputeVelMat() {

  if (_posLock) {
    _vel.t = 0.0;
  }else{
    _vel.t = _tVel;
  }

  if (_rotLock) {
    _vel.R = 0.0;
  }else{
    _vel.R = _pos.R;
    Matrix3 localRot;
    Vector3 localVel;
    _pos.R.mulTranspose(_aVel, localVel);
    localRot.setCrossProduct(localVel);
    _vel.R *= localRot;

    _vel.t -=  _vel.R*_com;
  }
}
