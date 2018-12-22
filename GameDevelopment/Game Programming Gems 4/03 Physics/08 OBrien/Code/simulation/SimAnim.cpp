/*********************************************************************
 * SimAnim.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * This is just like a CFrameAnimation but it puts the output of the
 * path into a CRigidState structure, using numerical differentiation
 * to calculate approximate velocities and accelerations. 
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



#include "SimAnim.h"

void QuatToRotation(const Quaternion& q, Vector3& v)
{
  Real angle = Two*Acos(q.w);
  Real imscale = angle/q.imNorm();
  v.set(q.x*imscale, q.y*imscale, q.z*imscale);
}

void DQ_CenteredDifferences(const Quaternion& qminush,
		       const Quaternion& qplush,
		       double h,
		       Vector3& dq)
{
  //assuming unit quaternions
  Quaternion qminush_conj;
  qminush_conj.setConjugate(qminush);
  QuatToRotation(qplush * qminush_conj, dq);
  dq *= 1.0/(2.0*h);
}

void D2Q_CenteredDifferences(const Quaternion& qminush,
			const Quaternion& q,
			const Quaternion& qplush,
			double h,
			Vector3& ddq)
{
  //assuming unit quaternions
  Quaternion q_conj;
  q_conj.setConjugate(q);
  QuatToRotation(qplush * q_conj * q_conj * qminush, ddq);
  ddq *= 1.0/(h*h);
}


void CRigidBodyAnimation::GetState(double t, CRigidState& state) const
{
  AnimationPosition pos;
  AnimationRotation rot;
  Interp(t, pos, rot);

  state.trnVec = pos;
  rot.getMatrix(state.rotMat);

  //finite differencing for vel, accel
  double h = 0.01;
  AnimationPosition pos1, pos2;
  AnimationRotation rot1, rot2;
  Interp(t-h, pos1, rot1);
  Interp(t+h, pos2, rot2);

  state.tVel = (pos2-pos1)/(h*2.0);
  DQ_CenteredDifferences(rot1, rot2, h, state.aVel);
  //leave it in world coords

  state.netFrc = (pos2 - 2*pos + pos1)/(h*h);
  D2Q_CenteredDifferences(rot1, rot, rot2, h, state.netMom);

  //cout << "avel " << state.aVel<< endl;
  //cout << "mom " << state.netMom<< endl;

  //both still in world coords, change em to sim coords
  state.netFrc = state.netFrc * state.rotMat;
  state.netMom = state.netMom * state.rotMat;
}


