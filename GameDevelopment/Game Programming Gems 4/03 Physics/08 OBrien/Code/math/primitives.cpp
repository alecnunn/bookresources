/*********************************************************************
 * primitives.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Mostly-inlined math primitives.
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



#include "primitives.h"
#include <stdlib.h>


template <class type>
void swap(type& a, type& b)
{
	type temp = a;
	a = b;
	b = temp;
}

Vector3::Vector3()
{
  setZero();
}

Vector3::Vector3(const Vector3& v)
:x(v.x), y(v.y), z(v.z)
{}

Vector3::Vector3(Real _x)
:x(_x), y(_x), z(_x)
{}

Vector3::Vector3(Real _x, Real _y, Real _z)
:x(_x), y(_y), z(_z)
{}

Vector3::Vector3(const Real data[3])
:x(data[0]), y(data[1]), z(data[2])
{}




Vector4::Vector4()
{
  setZero();
}

Vector4::Vector4(const Vector4& v)
:x(v.x), y(v.y), z(v.z), w(v.w)
{}

Vector4::Vector4(Real _x)
:x(_x), y(_x), z(_x), w(_x)
{}

Vector4::Vector4(Real _x, Real _y, Real _z, Real _w)
:x(_x), y(_y), z(_z), w(_w)
{}

Vector4::Vector4(const Real data[4])
:x(data[0]), y(data[1]), z(data[2]), w(data[3])
{}

Vector4::Vector4(const Vector3& v)
:x(v.x), y(v.y), z(v.z), w(One)
{}





Matrix3::Matrix3()
{}

Matrix3::Matrix3(const Matrix3& m)
{
	set(m);
}

Matrix3::Matrix3(Real x)
{
	set(x);
}

Matrix3::Matrix3(const Real m[3][3])
{
	set(m);
}

Matrix3::Matrix3(const Real m[9])
{
	set(m);
}

Matrix3::Matrix3(const Vector3& xb, const Vector3& yb, const Vector3& zb)
{
	set(xb,yb,zb);
}


bool Matrix3::operator == (const Matrix3& a) const
{
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
			if(data[i][j] != a.data[i][j])
				return false;
	}
	return true;
}

bool Matrix3::operator != (const Matrix3& a) const
{
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
			if(data[i][j] == a.data[i][j])
				return false;
	}
	return true;
}

void Matrix3::mul(const Matrix3& a, const Matrix3& b)
{
	int i,j,k;
	Real sum;
	Real dat[3][3];
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			sum = Zero;
			for(k=0; k<3; k++)
				sum += b.data[i][k]*a.data[k][j];	//column major
			dat[i][j] = sum;
		}
	}
	set(dat);
}

#define SWAP(a,b) { tmp = (a) ; (a) = (b) ; (b) = tmp; }

static void rowswap(Matrix3 &m, int i, int j) {
  register Real tmp;
  SWAP(m(i,0),m(j,0));
  SWAP(m(i,1),m(j,1));
  SWAP(m(i,2),m(j,2));
}

bool Matrix3::setInverse(const Matrix3& a)
{
  Matrix3 _a(a);
  Matrix3& _b = *this;
  _b.setIdentity();

  int i,j,i1;
  for(j=0;j<3;j++) {
    i1 = j;
    for(i=j+1;i<3;i++)
      if (Abs(_a(i,j)) > Abs(_a(i1,j)))
	i1 = i;
    rowswap(_a,i1,j);
    rowswap(_b,i1,j);
    if (FuzzyEquals(a(j,j),Zero)){
      cerr << "Inverse of singular matrix\n" << flush;
      abort();
    }
    register double div = 1.0/_a(j,j);
    _b(j,0) *= div;
    _b(j,1) *= div;
    _b(j,2) *= div;
    _a(j,0) *= div;
    _a(j,1) *= div;
    _a(j,2) *= div;
    for(i=0;i<3;i++) {
      if (i != j) {
	register double tmp = _a(i,j);
	_b(i,0) -= tmp*_b(j,0);
	_b(i,1) -= tmp*_b(j,1);
	_b(i,2) -= tmp*_b(j,2);
	_a(i,0) -= tmp*_a(j,0);
	_a(i,1) -= tmp*_a(j,1);
	_a(i,2) -= tmp*_a(j,2);
      }
    }
  }

   /*

  Real det = a.determinant();
  if(det == Zero)
    return false;
  Real detinv = One/det;
  
  int i,j;
  for(i=0; i<3; i++)  {
    for(j=0; j<3; j++) {
      //the data matrix is transposed, so be careful
      data[i][j] = a.cofactor(i,j) * detinv;
    }
    }*/

#ifdef DEBUG
  Matrix3 tmp;
  tmp.mul(*this, a);
  for(i=0; i<3; i++) {
    for(j=0; j<3; j++) {
      if(!FuzzyEquals(Delta(i,j), tmp(i,j), 0.001)) {
	cout << "Matrix3 inverse is not correct!" << endl;
	cout << "a matrix:" << endl;
	cout << a << endl;
	Real det = a.determinant();
	cout << "determinant: " << det << endl;
	cout << "inverse:" << endl;
	cout << *this << endl;
	cout << "a * inv :" << endl;
	cout << tmp << endl;
	abort();
      }
    }
  }
#endif

   return true;


	return true;

}

Real Matrix3::determinant() const
{
	Real a = data[0][0] * (data[1][1] * data[2][2] - data[1][2] * data[2][1]);
	Real b = data[0][1] * (data[1][2] * data[2][0] - data[1][0] * data[2][2]);
	Real c = data[0][2] * (data[1][0] * data[2][1] - data[1][1] * data[2][0]);
	return a - b + c;
}

Real Matrix3::cofactor(int i, int j) const
{
	static int rotation [] = { 0, 1, 2, 0, 1, 2};
	int i1,i2,j1,j2;
	j1 = rotation[(i+1)];
	j2 = rotation[(i+2)];
	i1 = rotation[(j+1)];
	i2 = rotation[(j+2)];
	return data[i1][j1]*data[i2][j2] - data[i1][j2]*data[i2][j1];
}


Matrix4::Matrix4()
{}

Matrix4::Matrix4(const Matrix4& m)
{
	set(m);
}

Matrix4::Matrix4(Real x)
{
	set(x);
}

Matrix4::Matrix4(const Real m[4][4])
{
	set(m);
}

Matrix4::Matrix4(const Real m[16])
{
	set(m);
}

Matrix4::Matrix4(const Vector3& xb, const Vector3& yb, const Vector3& zb, const Vector3& trans)
{
	set(xb,yb,zb,trans);
}

Matrix4::Matrix4(const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w)
{
	set(x,y,z,w);
}

Matrix4::Matrix4(const Matrix3& m)
{
	set(m);
}

Matrix4::Matrix4(const Matrix3& m, const Vector3& trans)
{
	set(m,trans);
}

Matrix4::Matrix4(const Vector3& trans)
{
	setTranslate(trans);
}

Matrix4::operator Matrix3() const
{
	Matrix3 tmp;
	get(tmp);
	return tmp;
}

bool Matrix4::operator == (const Matrix4& a) const
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
			if(data[i][j] != a.data[i][j])
				return false;
	}
	return true;
}

bool Matrix4::operator != (const Matrix4& a) const
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
			if(data[i][j] == a.data[i][j])
				return false;
	}
	return true;
}

void Matrix4::mul(const Matrix4& a, const Matrix4& b)
{
	int i,j,k;
	Real sum;
	Real dat[4][4];
	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			sum = Zero;
			for(k=0; k<4; k++)
				sum += b.data[i][k]*a.data[k][j];	//column major
			dat[i][j] = sum;
		}
	}
	set(dat);
}



bool Matrix4::setInverse(const Matrix4& a)
{
  int i, j, k, l, ll;
  int icol=0, irow=0;
  int indxc[4], indxr[4], ipiv[4];
  Real big, dum, pivinv, temp;

  set(a);

   // Gauss-jordan elimination with full pivoting.
   
   // from numerical recipies in C second edition, pg 39
 
  for(j=0;j<4;j++) ipiv[j] = 0;
  for(i=0;i<4;i++) {
    big=0.0;
    for (j=0;j<4;j++) {
      if(ipiv[j] != 1) {
	for (k=0;k<4;k++) {
	  if(ipiv[k] == 0) {
	    if(fabs(data[j][k]) >= big) {
	      big = Abs(data[j][k]);
               irow=j;
               icol=k;
             }
           } else if (ipiv[k] > 1) {
	     printf("Error in gauss-jordan inversion\n");
	     return false;
	   }
         } 
       }
    }
     ++(ipiv[icol]);
     if (irow != icol) {
       for (l=0;l<4;l++) swap(data[irow][l],data[icol][l]);
     }
     indxr[i]=irow;
     indxc[i]=icol;
     if(data[icol][icol] == Zero) {
       printf("Error in gauss-jordan inversion\n");
       return false;
     }
     pivinv = One / data[icol][icol];
     data[icol][icol]=One;
     for (l=0;l<4;l++) data[icol][l] *= pivinv;
     for (ll=0;ll<4;ll++) {
       if (ll != icol) {
         dum=data[ll][icol];
         data[ll][icol]=Zero;
         for (l=0;l<4;l++) data[ll][l] -= data[icol][l]*dum;
       }
     }
   }
   for (l=3;l>=0;l--) {
     if (indxr[l] != indxc[l]) {
       for (k=0;k<4;k++) {
         swap(data[k][indxr[l]],data[k][indxc[l]]);
       }
     }
   }

/*
	Real det = m.determinant();
	if(det == Zero)
		return false;
	Real detinv = One/det;

	int i,j;
	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			data[i][j] = m.cofactor(j,i) * detinv;
		}
	}
	*/

#ifdef DEBUG
  Matrix4 tmp;
  tmp.mul(*this, a);
  for(i=0; i<4; i++) {
    for(j=0; j<4; j++) {
      if(!FuzzyEquals(Delta(i,j), tmp(i,j), 0.001)) {
	cout << "Matrix3 inverse is not correct!" << endl;
	cout << "a matrix:" << endl;
	cout << a << endl;
	Real det = a.determinant();
	cout << "determinant: " << det << endl;
	cout << "inverse:" << endl;
	cout << *this << endl;
	cout << "a * inv :" << endl;
	cout << tmp << endl;
	abort();
      }
    }
  }
#endif

   return true;
}


Real Matrix4::determinant() const
{
	Real a = data[0][0] * cofactor(0,0);
	Real b = data[0][1] * cofactor(0,1);
	Real c = data[0][2] * cofactor(0,2);
	Real d = data[0][3] * cofactor(0,3);
	return a - b + c - d;
}

Real Matrix4::cofactor(int i, int j) const
{
	static int rotation [] = { 0, 1, 2, 3, 0, 1, 2};
	int i1,i2,i3,j1,j2,j3;
	i1 = rotation[(i+1)];
	i2 = rotation[(i+2)];
	i3 = rotation[(i+3)];
	j1 = rotation[(j+1)];
	j2 = rotation[(j+2)];
	j3 = rotation[(j+3)];
	Real a = data[i1][j1] * (data[i2][j2] * data[i3][j3] - data[i2][j3] * data[i3][j2]);
	Real b = data[i1][j2] * (data[i2][j3] * data[i3][j1] - data[i2][j1] * data[i3][j3]);
	Real c = data[i1][j3] * (data[i2][j1] * data[i3][j2] - data[i2][j2] * data[i3][j1]);
	return a - b + c;
}




RigidTransform::RigidTransform()
{}

RigidTransform::RigidTransform(const RigidTransform& x)
:R(x.R), t(x.t)
{}

RigidTransform::RigidTransform(const Matrix3& _R, const Vector3& _t)
:R(_R), t(_t)
{}

RigidTransform::RigidTransform(const Vector3& _t)
:t(_t)
{
	R.setIdentity();
}

RigidTransform::RigidTransform(const Matrix3& _R)
:R(_R)
{
	t.setZero();
}

RigidTransform::RigidTransform(const Matrix4& m)
{
	m.get(R,t);
}

bool RigidTransform::isValid() const
{
	return FuzzyEquals(Abs(R.determinant()), One);
}


Complex::Complex()
{}

Complex::Complex(const Complex& z)
:x(z.x), y(z.y)
{}

Complex::Complex(Real _x)
:x(_x), y(Zero)
{}

Complex::Complex(Real _x, Real _y)
:x(_x), y(_y)
{}



void Complex::setExp (const Complex& z)
{
	Real mag = Exp(z.x);

	Real ctheta = Cos(z.y);
	Real stheta = Sin(z.y);

	x = mag * ctheta;
	y = mag * stheta;
}

bool Complex::setLog (const Complex& z, int n)
{
	Real mag = z.norm();
	if(mag == 0.0)
		return false;
	x = Log(mag);
	y = z.arg() + Two*Pi*n;
	return true;
}

void Complex::setPow (const Complex& z, Real n)
{
	Real mag = z.norm();
	Real theta = z.arg();

	Real cntheta = Cos(n*theta);
	Real sntheta = Sin(n*theta);
	Real powm = Pow(mag, n);

	x = powm * cntheta;
	y = powm * sntheta;
}

Quaternion::Quaternion()
{}

Quaternion::Quaternion(const Quaternion& q)
:w(q.w), x(q.x), y(q.y), z(q.z)
{}

Quaternion::Quaternion(Real W)
:w(W), x(Zero), y(Zero), z(Zero) 
{}

Quaternion::Quaternion(Real W, const Vector3& V)
:w(W), x(V.x), y(V.y), z(V.z)
{}

Quaternion::Quaternion(Real W, Real X, Real Y, Real Z)
:w(W), x(X), y(Y), z(Z)
{}


void Quaternion::mul (const Quaternion& a, const Quaternion& b)
{
	Real A, B, C, D, E, F, G, H;

	A = (a.w + a.x)* (b.w + b.x);
	B = (a.z - a.y)* (b.y - b.z);
	C = (a.x - a.w)* (b.y + b.z);
	D = (a.y + a.z)* (b.x - b.w);
	E = (a.x + a.z)* (b.x + b.y);
	F = (a.x - a.z)* (b.x - b.y);
	G = (a.w + a.y)* (b.w - b.z);
	H = (a.w - a.y)* (b.w + b.z);

	w= B + (-E - F + G + H)*Half, 
	x= A - (E + F + G + H)*Half;
	y=-C + (E - F + G - H)*Half;
	z=-D + (E - F - G + H)*Half;
}

void Quaternion::div (const Quaternion& a, const Quaternion& b)
{
	Quaternion binv;
	binv.setInverse(b);
	mul(a,binv);
}

void Quaternion::madd (const Quaternion& a, const Quaternion& b)
{
	Real A, B, C, D, E, F, G, H;

	A = (a.w + a.x)* (b.w + b.x);
	B = (a.z - a.y)* (b.y - b.z);
	C = (a.x - a.w)* (b.y + b.z);
	D = (a.y + a.z)* (b.x - b.w);
	E = (a.x + a.z)* (b.x + b.y);
	F = (a.x - a.z)* (b.x - b.y);
	G = (a.w + a.y)* (b.w - b.z);
	H = (a.w - a.y)* (b.w + b.z);

	w+= B + (-E - F + G + H)*Half, 
	x+= A - (E + F + G + H)*Half;
	y+=-C + (E - F + G - H)*Half;
	z+=-D + (E - F - G + H)*Half;
}

void Quaternion::setExp(const Quaternion& q)
{
	Real scale = Exp(q.w);
	Real immag = q.imNorm();
	Real immaginv;
	if(immag == Zero) immaginv = Zero;  //it's a real number, zero out the imaginary part
	else immaginv = One / immag;
	Real sm = Sin(immag);
	Real cm = Cos(immag);

	w = scale * cm;
	x = scale * sm * immaginv * q.x;
	y = scale * sm * immaginv * q.y;
	z = scale * sm * immaginv * q.z;
}

bool Quaternion::setLog(const Quaternion& q, int n)
{
	Real mag = q.norm();
	Real immag = q.imNorm();
	if(immag == Zero)
	{
		//there are infinitely (uncountably) many such logarithms, just choose the real case
		if(mag == Zero)
			return false;
		w = Log(mag);
		x = Zero;
		y = Zero;
		z = Zero;
	}
	else
	{
		Real immaginv = One / immag;
		Real arg = Atan2(immag, q.w) + Two*Pi*n;

		w = Log(mag);
		x = immaginv * arg * q.x;
		y = immaginv * arg * q.y;
		z = immaginv * arg * q.z;
	}
	return true;
}

void Quaternion::setPow(const Quaternion& q, Real n)
{
	Real mag = q.norm();
	Real immag = q.imNorm();
	Real immaginv;
	if(immag == Zero) immaginv = Zero;  //it's a real number, zero out the imaginary part
	else immaginv = One / immag;
	Real theta = Atan2(immag, q.w);

	Real cntheta = Cos(n*theta);
	Real sntheta = Sin(n*theta);
	Real powm = Pow(mag, n);

	w = powm * cntheta;
	x = powm * sntheta * immaginv * q.x;
	y = powm * sntheta * immaginv * q.y;
	z = powm * sntheta * immaginv * q.z;
}



myostream& operator << (myostream& out, const Vector3& v)
{
	out << v.x << " " << v.y << " " << v.z;
	return out;
}

myistream& operator >> (myistream& in, Vector3& v)
{
	in >> v.x >> v.y >> v.z;
	return in;
}

myostream& operator << (myostream& out, const Vector4& v)
{
	out << v.x << " " << v.y << " " << v.z << " " << v.w;
	return out;
}

myistream& operator >> (myistream& in, Vector4& v)
{
	in >> v.x >> v.y >> v.z >> v.w;
	return in;
}


myostream& operator << (myostream& out, const Matrix3& m)
{
	out << m(0,0) << " " << m(0,1) << " " << m(0,2) << std::endl;
	out << m(1,0) << " " << m(1,1) << " " << m(1,2) << std::endl;
	out << m(2,0) << " " << m(2,1) << " " << m(2,2);
	return out;
}

myistream& operator >> (myistream& in, Matrix3& m)
{
	in >> m(0,0) >> m(0,1) >> m(0,2);
	in >> m(1,0) >> m(1,1) >> m(1,2);
	in >> m(2,0) >> m(2,1) >> m(2,2);
	return in;
}

myostream& operator << (myostream& out, const Matrix4& m)
{
	out << m(0,0) << " " << m(0,1) << " " << m(0,2) << " " << m(0,3) << std::endl;
	out << m(1,0) << " " << m(1,1) << " " << m(1,2) << " " << m(1,3) << std::endl;
	out << m(2,0) << " " << m(2,1) << " " << m(2,2) << " " << m(2,3) << std::endl;
	out << m(3,0) << " " << m(3,1) << " " << m(3,2) << " " << m(3,3);
	return out;
}

myistream& operator >> (myistream& in, Matrix4& m)
{
	in >> m(0,0) >> m(0,1) >> m(0,2) >> m(0,3);
	in >> m(1,0) >> m(1,1) >> m(1,2) >> m(1,3);
	in >> m(2,0) >> m(2,1) >> m(2,2) >> m(2,3);
	in >> m(3,0) >> m(3,1) >> m(3,2) >> m(3,3);
	return in;
}

myostream& operator << (myostream& out, const RigidTransform& r)
{
  out << r.R << endl;
  out << r.t;
  return out;
}

myistream& operator >> (myistream& in, RigidTransform& r)
{
  in >> r.R;
  in >> r.t;
  return in;
}

myostream& operator << (myostream& out, const Complex&);
myistream& operator >> (myistream& in, Complex&);

myostream& operator << (myostream& out, const Quaternion& q)
{
	out << q.w << " " << q.x << " " << q.y << " " << q.z;
	return out;
}

myistream& operator >> (myistream& in, Quaternion& q)
{
	in >> q.w >> q.x >> q.y >> q.z;
	return in;
}

