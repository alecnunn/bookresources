/*********************************************************************
 * rotation.h
 * Authored by Kris Hauser 2002-2003
 *
 * Headers for classes involving 3D rotations.
 *
 * Copyright (c) 2002,2003 SquireSoft, used with permission.
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



#ifndef MATH_ROTATION_H
#define MATH_ROTATION_H

#include "primitives.h"

class EulerAngleRotation;
class AngleAxisRotation;
class MomentRotation;
class QuaternionRotation;

class EulerAngleRotation : public Vector3
{
 public:
  EulerAngleRotation();
  EulerAngleRotation(const EulerAngleRotation&);
  EulerAngleRotation(const Vector3&);
  EulerAngleRotation(Real x, Real y, Real z);

  inline void set(const EulerAngleRotation& r) { Vector3::set(r.x,r.y,r.z); }
  inline void setIdentity() { setZero(); }

  void setMatrixXYZ(const Matrix3&);
  void setMatrixZYX(const Matrix3&);

  void getMatrixXYZ(Matrix3&) const;
  void getMatrixZYX(Matrix3&) const;
};

class AngleAxisRotation
{
 public:
  AngleAxisRotation();
  AngleAxisRotation(const AngleAxisRotation&);
  AngleAxisRotation(Real angle, const Vector3& axis);
  AngleAxisRotation(const MomentRotation&);

  void set(const AngleAxisRotation&);
  void set(Real angle, const Vector3& axis);
  void setAxis(const Vector3& axis);
  void setIdentity();

  void setMatrix(const Matrix3&);
  void getMatrix(Matrix3&) const;

  void setMoment(const MomentRotation&);
  void getMoment(MomentRotation&) const;

  Real angle;
  Vector3 axis;
};

class MomentRotation : public Vector3
{
 public:
  MomentRotation();
  MomentRotation(const MomentRotation&);
  MomentRotation(const Vector3& v);
  MomentRotation(const AngleAxisRotation&);

  inline void set(const MomentRotation& r) { Vector3::set(r); }
  inline void set(const Vector3& v)  { Vector3::set(v); }
  inline void set(Real x, Real y, Real z)  { Vector3::set(x,y,z); }
  inline void setIdentity() { Vector3::setZero(); }

  void setMatrix(const Matrix3&);
  void getMatrix(Matrix3&) const;

  void setAngleAxis(const AngleAxisRotation&);
  void getAngleAxis(AngleAxisRotation&) const;

  Real angle;
  Vector3 axis;
};


class QuaternionRotation : public Quaternion
{
 public:
  QuaternionRotation();
  QuaternionRotation(const QuaternionRotation&);
  QuaternionRotation(const Quaternion&);
  QuaternionRotation(Real w, Real x, Real y, Real z);

  void slerp(const Quaternion& a, const Quaternion& b, Real t);
  void mag(const Quaternion& a, Real t);
  void transform(const Vector3& a, Vector3& out) const;

  inline void set(const QuaternionRotation& q) { Quaternion::set(q); }
  inline void setIdentity() { Quaternion::set(One); }
  void setEulerAnglesZYX(const EulerAngleRotation&);
  void setEulerAnglesXYZ(const EulerAngleRotation&);
  void setAngleAxis(const AngleAxisRotation&);
  void setMoment(const MomentRotation&);
  void setMatrix(const Matrix3&);

  void getAngleAxis(AngleAxisRotation&) const;
  void getMoment(MomentRotation&) const;
  void getMatrix(Matrix3&) const;
};

void SetMatrixRotationZYX(Matrix3&, const Vector3&);		//euler
void SetMatrixRotationZYX(Matrix4&, const Vector3&);
void SetMatrixRotationVector(Matrix3&, const Vector3&);		//moment
void SetMatrixRotationVector(Matrix4&, const Vector3&);
void SetMatrixRotationVector(Matrix3&, const Vector3&, Real angle);  //angle axis
void SetMatrixRotationVector(Matrix4&, const Vector3&, Real angle);
void SetMatrixRotationQuaternion(Matrix3&, const Quaternion&);  //quaternion
void SetMatrixRotationQuaternion(Matrix4&, const Quaternion&);


void SLerp(const Quaternion& q0,
	   const Quaternion& q1,
	   Quaternion& out,
	   Real t);
void SCerp(const Quaternion& q_1,
	   const Quaternion& q0,
	   const Quaternion& q1,
	   const Quaternion& q2,
	   Quaternion& out,
	   Real t);
void SBezier(const Quaternion& q0,
	     const Quaternion& c0,
	     const Quaternion& c1,
	     const Quaternion& q1,
	     Quaternion& out,
	     Real t);


#endif
