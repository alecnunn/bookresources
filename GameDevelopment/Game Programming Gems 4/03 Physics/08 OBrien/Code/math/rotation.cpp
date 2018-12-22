/*********************************************************************
 * rotation.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Classes involving 3D rotations.
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


#include "rotation.h"
#include <stdlib.h>
#include <stdio.h>

EulerAngleRotation::EulerAngleRotation()
{}

EulerAngleRotation::EulerAngleRotation(const EulerAngleRotation& r)
  :Vector3(r)
{}

EulerAngleRotation::EulerAngleRotation(const Vector3& v)
  :Vector3(v)
{}

EulerAngleRotation::EulerAngleRotation(Real x, Real y, Real z)
  :Vector3(x,y,z)
{}

void EulerAngleRotation::setMatrixXYZ(const Matrix3& m)
{
  printf("EulerAngleRotation XYZ stuff isn't done yet, aborting\n");
  abort();
}

void EulerAngleRotation::setMatrixZYX(const Matrix3& m)
{
  double beta, alpha_grad, gamma_grad;
  double Pi2 = Pi*Two;
  beta=-asin(m.data[0][2]);
  double cos_beta = cos(beta);
  //----------------------------------- beta > 0 ------------------------------
  if (fabs(cos_beta) > 1.0e-10) {
    double cos_alpha, cos_gamma;
    cos_alpha = m.data[2][2]/cos_beta;
    cos_alpha = Sign(cos_alpha)*Min(One,Abs(cos_alpha));
    if (Sign(m.data[1][2]) == Sign(cos_beta)) {
      if (cos_alpha >= 0.0)   
		alpha_grad = acos(cos_alpha);              // 1. Quadrant
      else
		alpha_grad = acos(cos_alpha);              // 2. Quadrant
    }
    else {
      if (cos_alpha <= 0.0)
		alpha_grad = Pi2 - acos(cos_alpha);         // 3. Quadrant
      else
		alpha_grad = Pi2 - acos(cos_alpha);         // 4. Quadrant
    }
    // jetzt dito mit gamma
    cos_gamma  = m.data[0][0] / cos_beta;
    cos_gamma = Sign(cos_gamma)*Min(One, Abs(cos_gamma));
    
    if (Sign(m.data[0][1]) == Sign(cos_beta)) {
      if (cos_gamma >= 0)   
		gamma_grad = acos(cos_gamma);
      else
		gamma_grad = acos(cos_gamma);
    }
    else {
      if (cos_gamma <= 0)   
		gamma_grad = Pi2 - acos(cos_gamma);   
      else
		gamma_grad = Pi2 - acos(cos_gamma);
    }
  }
  else {
	  printf("2\n");
    // ----- for beta close to 90 degrees  
    if  (m.data[0][2] < 0) {	//this is -sin(ry), which is -1 if ry = pi/2, and +1 if = -pi/2
      alpha_grad = asin(m.data[1][0]);	//either this or pi - this
      if(Sign(m.data[2][0]) != Sign(cos(alpha_grad)))
		alpha_grad = Pi-alpha_grad;
      gamma_grad = 0;
    }
    else {
      alpha_grad = -asin(m.data[1][0]);	//either this or pi - this
      if(Sign(m.data[2][0]) == Sign(cos(alpha_grad)))
		alpha_grad = Pi-alpha_grad;
      gamma_grad = 0;
    }
  }

  x=(Real)alpha_grad;
  y=(Real)beta;
  z=(Real)gamma_grad;
}

void EulerAngleRotation::getMatrixXYZ(Matrix3&) const
{
  printf("EulerAngleRotation XYZ stuff isn't done yet, aborting\n");
  abort();
}

void EulerAngleRotation::getMatrixZYX(Matrix3& m) const
{
  Real ca = Cos(x);
  Real cb = Cos(y);
  Real cc = Cos(z);
  Real sa = Sin(x);
  Real sb = Sin(y);
  Real sc = Sin(z);
  /*	m._11=ca*cb*cc- sa*sc; 
	m._12=sa*cb*cc+ca*sc; 
	m._13=-sb*cc; 
	m._21=-ca*cb*sc-sa*cc; 
	m._22=-sa*cb*sc+ca*cc; 
	m._23=sb*sc; 
	m._31=ca*sb; 
	m._32=sa*sb; 
	m._33=cb;*/

  m.data[0][0]=cb*cc;
  m.data[0][1]=cb*sc;
  m.data[0][2]=-sb; 
  m.data[1][0]=sa*sb*cc-ca*sc;
  m.data[1][1]=sa*sb*sc+ca*cc; 
  m.data[1][2]=sa*cb; 
  m.data[2][0]=ca*sb*cc+sa*sc; 
  m.data[2][1]=ca*sb*sc-sa*cc;
  m.data[2][2]=ca*cb;
}


AngleAxisRotation::AngleAxisRotation()
{}

AngleAxisRotation::AngleAxisRotation(const AngleAxisRotation& r)
  :angle(r.angle), axis(r.axis)
{}

AngleAxisRotation::AngleAxisRotation(Real _angle, const Vector3& _axis)
{
  set(_angle, _axis);
}

AngleAxisRotation::AngleAxisRotation(const MomentRotation& r) 
{
  setMoment(r);
}

void AngleAxisRotation::set(const AngleAxisRotation& r)
{
  angle = r.angle;
  axis = r.axis;
}

void AngleAxisRotation::set(Real _angle, const Vector3& _axis)
{
  angle = _angle;
  setAxis(_axis);
}

void AngleAxisRotation::setAxis(const Vector3& _axis)
{
  Real l2 = _axis.normSquared();
  if(FuzzyEquals(l2,One))
    axis = _axis;
  else
    axis.div(_axis, Inv(Sqrt(l2)));
}

void AngleAxisRotation::setIdentity()
{
  angle = Zero;
  axis.set(One,Zero,Zero);
}

void AngleAxisRotation::setMatrix(const Matrix3& r)
{
  MomentRotation m;
  m.setMatrix(r);
  setMoment(m);
}

void AngleAxisRotation::getMatrix(Matrix3& m) const
{
  if(angle == Zero) {
    m.setIdentity();
    return;
  }

  Real cm = Cos(angle);
  Real sm = Sin(angle);

  Matrix3 rmat;
  rmat.setCrossProduct(axis);

  m.mul(rmat, rmat);
  m.inplaceScale(cm);

  rmat.inplaceScale(sm);
  m.sub(rmat, m);

  for(int i=0; i<3; i++)
    for(int j=0; j<3; j++)
      m.data[i][j] += axis[i]*axis[j];
}

void AngleAxisRotation::setMoment(const MomentRotation& r)
{
  Real l = r.norm();
  angle = l;
  if(l == Zero)
    axis.setZero();
  else
    axis.div(r, l);
}

void AngleAxisRotation::getMoment(MomentRotation& r) const
{
  r.mul(axis, angle);
}

MomentRotation::MomentRotation()
{}

MomentRotation::MomentRotation(const MomentRotation& r)
  :Vector3(r)
{}

MomentRotation::MomentRotation(const Vector3& v)
  :Vector3(v)
{}

MomentRotation::MomentRotation(const AngleAxisRotation& a)
{
  setAngleAxis(a);
}

void MomentRotation::setMatrix(const Matrix3& r)
{
  Real theta = Acos((r.trace() - One)*Half);

  if(theta == Zero) {
    setIdentity();
    return;
  }

  Matrix3 r_cross;
  r_cross.setTranspose(r);
  r_cross.sub(r, r_cross);
  r_cross.inplaceScale(Half*theta/Sin(theta));

  x = r_cross.data[1][2];
  y = r_cross.data[2][0];
  z = r_cross.data[0][1];
}

void MomentRotation::getMatrix(Matrix3& m) const
{
  AngleAxisRotation a;
  a.setMoment(*this);
  a.getMatrix(m);
}

void MomentRotation::setAngleAxis(const AngleAxisRotation& a)
{
  mul(a.axis, a.angle);
}

void MomentRotation::getAngleAxis(AngleAxisRotation& a) const
{
  a.setMoment(*this);
}

QuaternionRotation::QuaternionRotation()
{}

QuaternionRotation::QuaternionRotation(Real w, Real x, Real y, Real z)
:Quaternion(w,x,y,z)
{}

QuaternionRotation::QuaternionRotation(const QuaternionRotation& q)
:Quaternion(q)
{}

QuaternionRotation::QuaternionRotation(const Quaternion& q)
:Quaternion(q)
{}

void QuaternionRotation::setEulerAnglesZYX(const EulerAngleRotation& r)
{
  Quaternion qx,qy,qz;

  qx.set(Cos(r.x*Half), Sin(r.x*Half), Zero, Zero);
  qy.set(Cos(r.y*Half), Zero, Sin(r.y*Half), Zero);
  qz.set(Cos(r.z*Half), Zero, Zero, Sin(r.z*Half));

  *this = qz*qy*qx;
}

void QuaternionRotation::setEulerAnglesXYZ(const EulerAngleRotation& r)
{
  Quaternion qx,qy,qz;

  qx.set(Cos(r.x*Half), Sin(r.x*Half), Zero, Zero);
  qy.set(Cos(r.y*Half), Zero, Sin(r.y*Half), Zero);
  qz.set(Cos(r.z*Half), Zero, Zero, Sin(r.z*Half));

  *this = qx*qy*qz;
}

void QuaternionRotation::setAngleAxis(const AngleAxisRotation& r)
{
  Real imscale = Sin(r.angle*Half);
  w = Cos(r.angle*Half);
  x = imscale*r.axis.x;
  y = imscale*r.axis.y;
  z = imscale*r.axis.z;
}

void QuaternionRotation::setMoment(const MomentRotation& m)
{
  setAngleAxis(AngleAxisRotation(m));
}


void QuaternionRotation::setMatrix(const Matrix3& m)
{
  Real tr, s, q[4];
  int    i, j, k;

  int nxt[3] = {1, 2, 0};

  tr = m.trace();

  // check the diagonal
  if (tr > Zero) {
    s = Sqrt (tr + One);
    w = s * Half;
    s = Half / s;
    x = (m.data[1][2] - m.data[2][1]) * s;
    y = (m.data[2][0] - m.data[0][2]) * s;
    z = (m.data[0][1] - m.data[1][0]) * s;
  }
  else {                
    // diagonal is negative
    i = 0;
    if (m.data[1][1] > m.data[0][0]) i = 1;
    if (m.data[2][2] > m.data[i][i]) i = 2;
    j = nxt[i];
    k = nxt[j];
    
    s = Sqrt ((m.data[i][i] - (m.data[j][j] + m.data[k][k])) + One);
    q[i] = s * Half;
    
    if (s != Zero) s = Half / s;
    
    q[3] = (m.data[j][j] - m.data[k][j]) * s;
    q[j] = (m.data[j][i] + m.data[i][j]) * s;
    q[k] = (m.data[k][i] + m.data[i][i]) * s;

    x = q[0];
    y = q[1];
    z = q[2];
    w = q[3];
  }
}


void QuaternionRotation::getAngleAxis(AngleAxisRotation& r) const
{
  r.angle = Two*Acos(w);
  Real imscale = One/imNorm();
  r.axis.set(x*imscale, y*imscale, z*imscale);
}

void QuaternionRotation::getMoment(MomentRotation& r) const
{
  Real angle = Two*Acos(w);
  Real imscale = angle/imNorm();
  r.set(x*imscale, y*imscale, z*imscale);
}

void QuaternionRotation::getMatrix(Matrix3& m) const {
  Real wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

  // calculate coefficients
  x2 = x + x; y2 = y + y; z2 = z + z;
  xx = x * x2;   xy = x * y2;   xz = x * z2;
  yy = y * y2;   yz = y * z2;   zz = z * z2;
  wx = w * x2;   wy = w * y2;   wz = w * z2;

  m.data[0][0] = One - (yy + zz);    m.data[1][0] = xy - wz; 			m.data[2][0] = xz + wy;
  m.data[0][1] = xy + wz;            m.data[1][1] = One - (xx + zz);	m.data[2][1] = yz - wx;
  m.data[0][2] = xz - wy;            m.data[1][2] = yz + wx;			m.data[2][2] = One - (xx + yy);
}


void QuaternionRotation::transform(const Vector3& v, Vector3& out) const
{
  Quaternion qinv, x(0,v), tmp;
  qinv.setConjugate(*this);
  tmp.mul(x,*this);
  x.mul(qinv,tmp);
  out.x=x.x;
  out.y=x.y;
  out.z=x.z;
}

void QuaternionRotation::slerp(const Quaternion& a, const Quaternion& b, Real t)
{
  //a + b unit quaternions?
  /* angle theta is defined as
     cos(theta)*|a||b| = <a,b>
  */
  Real d = dot(a,b);
  if(d == One) {	//axes are the same axis
    set(b);
    return;
  }
  else if(d == -One) {	//axes are opposing axis
    printf("QuaternionRotation.slerp(): Quaternions on opposing sides of unit sphere\n");
    return;
  }

  Real theta = Acos(d);
  Real sininv = Sin(theta);
  sininv = One/sininv;

  //this = (Sin((One-t)*theta)*sininv) * a +  (Sin(t*theta)*sininv) * b;
  Real a_coeff = Sin((One-t)*theta)*sininv;
  Real b_coeff = Sin(t*theta)*sininv;
  mul(a, a_coeff);
  madd(b, b_coeff);
}

void QuaternionRotation::mag(const Quaternion& a, Real t)
{
  //just like slerp(identity transform, a)

  if(a.w == One) {	//a is identity
    set(a);
    return;
  }
  else if(a.w == -One) { //a is a rotation of 180 degrees
    printf("QuaternionRotation.mag(): Quaternion is a rotation of 180 degrees\n");
    return;
  }


  Real theta = Acos(a.w);
  Real sininv = Sin(theta);
  sininv = One/sininv;

  Real a_coeff = Sin((One-t)*theta)*sininv;
  Real b_coeff = (Sin(t*theta)*sininv);
  mul(a, b_coeff);
  w += a_coeff;
}


void SLerp(const Quaternion& a,
	   const Quaternion& b,
	   Quaternion& out,
	   Real t)
{
  //a + b unit quaternions?
  /* angle theta is defined as
     cos(theta)*|a||b| = <a,b>
  */
  Real d = dot(a,b);
  if(d == One) {	//axes are the same axis
    out.set(b);
    return;
  }
  else if(d == -One) {	//axes are opposing axis
    printf("SLerp(): Quaternions on opposing sides of unit sphere\n");
    return;
  }

  Real theta = Acos(d);
  Real sininv = Sin(theta);
  sininv = One/sininv;

  //out = (Sin((One-t)*theta)*sininv) * a +  (Sin(t*theta)*sininv) * b;
  Real a_coeff = Sin((One-t)*theta)*sininv;
  Real b_coeff = Sin(t*theta)*sininv;
  out.mul(a, a_coeff);
  out.madd(b, b_coeff);
}

void SCerp(const Quaternion& q_1,
	   const Quaternion& q0,
	   const Quaternion& q1,
	   const Quaternion& q2,
	   Quaternion& out,
	   Real t)
{
  //calc control pts
  Quaternion c0,c1;
  Quaternion p,q;

  static const Real Third = 1.0/3.0;
  SLerp(q0, q_1, p, -1.0);
  SLerp(p, q1, q, 0.5);
  SLerp(q0, q, c0, Third);

  SLerp(q1, q2, p, -1.0);
  SLerp(p, q0, q, 0.5);
  SLerp(q1, q, c1, Third);

  SBezier(q0, c0, c1, q1, out, t);
}

void SBezier(const Quaternion& q0,
	     const Quaternion& c0,
	     const Quaternion& c1,
	     const Quaternion& q1,
	     Quaternion& out,
	     Real t)
{
  Quaternion a,b,c,d,e;
  SLerp(q0,c0, a,t);
  SLerp(c0,c1, b,t);
  SLerp(c1,q1, c,t);

  SLerp(a,b, d,t);
  SLerp(b,c, e,t);

  SLerp(d,e, out,t);
}



void SetMatrixRotationZYX(Matrix3& m, const Vector3& v)
{
	EulerAngleRotation e(v);
	e.getMatrixZYX(m);
}

void SetMatrixRotationZYX(Matrix4& m, const Vector3& v)
{
	Matrix3 m3;
	SetMatrixRotationZYX(m3,v);
	m.set(m3);
}

void SetMatrixRotationVector(Matrix3& m, const Vector3& v)
{
	MomentRotation r(v);
	r.getMatrix(m);
}

void SetMatrixRotationVector(Matrix4& m, const Vector3& v)
{
	Matrix3 m3;
	SetMatrixRotationVector(m3,v);
	m.set(m3);
}

void SetMatrixRotationVector(Matrix3& m, const Vector3& v, Real angle)
{
	AngleAxisRotation r(angle,v);
	r.getMatrix(m);
}

void SetMatrixRotationVector(Matrix4& m, const Vector3& v, Real angle)
{
	Matrix3 m3;
	SetMatrixRotationVector(m3,v,angle);
	m.set(m3);
}

void SetMatrixRotationQuaternion(Matrix3& m, const Quaternion& q)
{
	QuaternionRotation r(q);
	r.getMatrix(m);
}

void SetMatrixRotationQuaternion(Matrix4& m, const Quaternion& q)
{
	Matrix3 m3;
	SetMatrixRotationQuaternion(m3,q);
	m.set(m3);
}
