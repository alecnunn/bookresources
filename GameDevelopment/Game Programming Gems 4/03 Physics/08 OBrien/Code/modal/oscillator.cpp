/*********************************************************************
 * oscillator.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Method definitions for the Oscillator class, an efficient 
 * encapsulator of the solution to a second-order
 * differential equation.
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



#include "oscillator.h"
#include <math.h>
#include <stdio.h>

#define EPSILON 2e-5

//if not using doubles, this avoids having to convert from double
//to float every function call
const static float_t ONE = 1.0;
const static float_t TWO = 2.0;
const static float_t FOUR = 4.0;
const static float_t HALF = 0.5;

#ifdef VECTOR_DOUBLE
#define SQRT(x) sqrt(x)
#else
#define SQRT(x) sqrtf(x)
#endif



Oscillator::Oscillator()
:p1(0), q1(0), dp1(0), dq1(0),
p2(0), q2(0), dp2(0), dq2(0),
d1(0), f1(0), f2(0), wd(0)
{
}

void Oscillator::Setup_KMC(float_t K, float_t M, float_t C)
{
	float_t wd_2 = C*C - FOUR*K*M;
	if(fabs(wd_2) < EPSILON)
	  wd_2 = EPSILON;
	if(wd_2 < 0)	//underdamped
	{
		wd = SQRT(-wd_2);
		d1 = d2 = -C*HALF;
		f1 = wd*HALF;
		f2 = wd*HALF;

		//printf("underdamped:\n");
		//printf("d = %f\n", d1);
		//printf("f = %f\n", f1);
	}
	else			//overdamped
	{
		wd = -SQRT(wd_2);
		d1 = (-C + wd)*HALF;
		d2 = (-C - wd)*HALF;
		f1 = f2 = 0;

		//printf("overdamped:\n");
		//printf("d1 = %f\n", d1);
		//printf("d2 = %f\n", d2);
		//printf("wd = %f\n", wd);
	}
}

void Oscillator::Setup_DampingRatioNaturalFrequency(float_t R, float_t wn)
{
	float_t wd_2 = R - ONE;
	if(wd_2 < 0)	// underdamped
	{
		wd = SQRT(-wd_2)*wn;
		d1 = d2 = -R*wn;
		f1 = wd*HALF;
		f2 = wd*HALF;
	}
	else			// overdamped 
	{
		wd = -SQRT(wd_2)*wn;
		d1 = (-R*wn + wd);
		d2 = (-R*wn - wd);
		f1 = f2 = 0;
	}
}

void Oscillator::Setup_Omega1Omega2(float_t w1_real, float_t w2_real, float_t w1_imag, float_t w2_imag)
{
	d1 = w1_real;
	d2 = w2_real;
	f1 = w1_imag;
	f2 = w2_imag;

	if(f1 != f2) {
	  printf("don't yet support arbitrary oscillators with f1 != f2\n");
	}

	//solve for wd = sqrt(c^2-4km)
	//given w1 = -c+sqrt(c^2-4km) / 2 and w2 = -c-sqrt(c^2-4km) / 2
	//if w1_imag, w2_imag == 0, then wd = w1_real - w2_real
	//otherwise wd = w1_imag - w2_imag
	//float_t c = -w1_real - w2_real;
	if(w1_imag == 0.0 && w2_imag == 0.0)		//overdamped
	{
		wd = -fabs(w1_real - w2_real);
	}
	else		//underdamped
	{
		wd = fabs(w1_imag - w2_imag);
		wd = w1_imag * TWO;
	}
}

void Oscillator::Setup_KMa1a2(float_t K, float_t M, float_t alpha1, float_t alpha2)
{
	Setup_KMC(K,M,K*alpha1 + M*alpha2);
}

//evaluates the oscillator values at the given dt
void Oscillator::get_deltas(float_t dt, float_t& s, float_t& t, float_t& u, float_t& v) const
{
	float_t d;
	d = exp(-fabs(d1*dt));
	s = d*cos(f1*dt);
	t = d*sin(f1*dt);

	d = exp(-fabs(d2*dt));
	u = d*cos(f2*dt);
	v = d*sin(f2*dt);
}

void Oscillator::Setup_dt(float_t dt)
{
	get_deltas(dt, dp1, dq1, dp2, dq2);
}

void Oscillator::Inc_t()
{
	float_t oldreal = p1;
	float_t oldimag = q1;
	p1 = oldreal*dp1 - oldimag*dq1;
	q1 = oldreal*dq1 + oldimag*dp1;

	oldreal = p2;
	oldimag = q2;
	p2 = oldreal*dp2 - oldimag*dq2;
	q2 = oldreal*dq2 + oldimag*dp2;
}

void Oscillator::Inc_t(float_t dt)
{
	float_t s,t,u,v;
	get_deltas(dt, s,t,u,v);
	update_state(s,t,u,v);
}

float_t Oscillator::Get_z(float_t t_eval) const
{
	float_t s,t,u,v;
	get_deltas(t_eval, s,t,u,v);

	return p1*s - q1*t + p2*u - q2*v;
}

float_t Oscillator::Get_dz() const
{
	//if overdamped
	//dp1 = d1 * p1
	//dp2 = d2 * p2

	//if underdamped
	//dp = d*p - q
	//dq = d*q + p

  	if(UnderDamped())
	  return TWO*(d1*p1 - f1*q1);
	else
	  return d1*p1 + d2*p2;
}

float_t Oscillator::Get_ddz() const
{
  return (d1*d1-f1*f1)*p1 + d1*f1*q1 +
    (d2*d2-f2*f2)*p2 + d2*f2*q2;
}

void Oscillator::set_ab(float_t a, float_t b)
{
	if(UnderDamped())	//a is imaginary
	{
		p1 = p2 = b;
		q1 = q2 = -a;
	}
	else				//a and b are real, z = (a+b)p1(t) + (b-a)p2(t)
	{
		p1 = a+b;
		p2 = b-a;
		q1 = q2 = 0;
	}
}

void Oscillator::Set_z_dz (float_t z, float_t dz)
{
	//a = dz/wd + c*z/(2*wd)
	//b = z/2

	float_t c = -(d1+d2);
	set_ab((dz + HALF*c*z)/wd, z*HALF);
}

void Oscillator::Inc_z_dz (float_t z, float_t dz)
{
	//just set it to current z + the increment
	float_t old_z, old_dz;
	old_z = Get_z();
	old_dz = Get_dz();
	Set_z_dz(z+old_z, dz+old_dz);
}
#define M_2PI 6.283185307179586476925286766559f
//#define M_PI 3.1415926535897932384626433832795

#define FREQ_TO_HZ 0.159154943f

float_t Oscillator::GetDecay1() const
{
	return d1;
}

float_t Oscillator::GetDecay2() const
{
	return d2;
}

float_t Oscillator::GetFreq() const
{
	return wd*HALF*FREQ_TO_HZ;
}




float_t Oscillator::Get_z_df() const
{
	//change in z as a function of f
	//z = p1*dp1 - q1*dq1 + p1*dp2 - q2*dq2
	//if(UnderDamped())
	//	p1 = p2 = b
	//	q1(g) = q2(g) = -a
	//else
	//	p1(g) = a(g)+b
	//	p2(g) = b-a(g)
	//	q1 = q2 = 0
	//where
	//a(g) = (dz+g + 0.5*c*z)/wd   => a'(g) = 1/wd
	//b = z*0.5   => b'(g) = 0

	// => if underdamped, 
	//dz/dg = a'(g)*dq1 + a'(g)*dq2 = (dq1+dq2)/wd
	// => if overdamped
	//dz/dg = a'(g)*dp1 - a'(g)*dp2 = (dp1-dp2)/wd

  float val = (UnderDamped() ? (dq1+dq2)/wd : (dp1-dp2)/wd);
  if(val < 2e-4)
    val = 0;
  return val;
	//return 1.0/wd;
}

float_t Oscillator::Get_dz_df() const
{
	//change in dz as a function of f
	//if(UnderDamped())
	//	dz = 2*(d1*p1 - f1*q1)
	//else
	//	dz = d1*p1 + d2*p2
	//p1, q1, p2 are defined above
	// => if underdamped
        //ddz/dg = 2*( -f1*-a'(g)) = 2*f1/wd
	// => if overdamped
	//ddz/dg = d1*a'(g) - d2*a'(g) = (d1-d2)/wd

  //this turns out to be always 1.0
	if(UnderDamped())
	{
		return TWO*f1/wd;
	}
	else
	{
		return (d1-d2)/wd;
	}
}

//#include "complex.h"

void Oscillator::Crop_Set(float_t& z, float_t& dz, float_t limit) const
{
  /*  //step 1: find the t of maximum z
  complex_t a1,a2,a3;
  complex_t c1,c2;
  float_t a,b;
  float_t tmax = 0.0;
  float_t zmax;

  //get c1,c2
  a = (dz - HALF*(d1+d2)*z)/wd;
  b = z*HALF;
  if(UnderDamped()) {
    Complex::make(c1, b,-a);
    Complex::make(c2, b,-a);
  }
  else {
    Complex::make(c1, b+a,0);
    Complex::make(c2, b-a,0);
  }

  //log(-w1c1/w2c2) = (w1-w2)t
  Complex::make(a2, d1, f1);  //a2 = (d1,f1) = w1
  Complex::make(a3, d2, f2);  //a3 = (d2,f2) = w2 

  Complex::mult(a1, a2, c1);   //a1 = w1*c1
  Complex::mult(a2, a3, c2);   //a2 = w2*c2
  Complex::neg(a1, a1);
  Complex::div(a3, a1, a2);    //a3 = -w1*c1/w2*c2
  if(Complex::log(a1, a3)) {   //a1 = log(-w1*c1/w2*c2)
    Complex::make(a3,-(d1-d2),(f1-f2));  //a3 = w1-w2;
    if(Complex::div(a2, a1, a3)) {  //a2 = log(-w1*c1/w2*c2) / (w1-w2)
      if(fabs(Complex::Im(a2)) < EPSILON)   //t is real
	tmax = Complex::Re(a2);
    }
  }

  if(tmax == 0.0) {
    zmax = z;
  }
  else {
    //evaluate at tmax
    get_deltas(tmax, Complex::Re(a1),Complex::Im(a1),Complex::Re(a2),Complex::Im(a2));
    Complex::mult(a3, a1, c1);
    Complex::mult(a1, a2, c2);
    zmax = Complex::Re(a1)+Complex::Re(a3);
  }

  printf("max value of oscillator: %f at %f\n", zmax, tmax);

  if(fabs(zmax) > limit) {
    float_t alpha = limit / fabs(zmax);
    z *= alpha;
    dz *= alpha;
    }*/

  if(UnderDamped()) {
    //The frequencies are equal, and the pattern is a spiral towards 0
    //in complex space.  Therefore, z (complex) is the max already

    //there's a scale in the imaginary component first
    float_t zi = -(dz - HALF*(d1+d2)*z)/wd*TWO;
    float_t len2 = z*z + zi*zi;
    if(len2 > limit*limit) {
      float_t alpha = limit/SQRT(len2);
      z *= alpha;
      dz *= alpha;

      //zi = -(dz - HALF*(d1+d2)*z)/wd*TWO;
      //printf("Dest: %f, Limit: %f\n", SQRT(z*z+zi*zi), limit);
    }
  }
  else {
    //step 1: find the t of maximum z
    float_t a1,a2,a3;
    float_t c1,c2;
    float_t a,b;
    float_t tmax = 0.0;
    float_t zmax;

    //get c1,c2
    a = (dz - HALF*(d1+d2)*z)/wd;
    b = z*HALF;

    //Overdamped() = true
    c1 = b+a;
    c2 = b-a;


    //log(-w1c1/w2c2) = (w1-w2)t
    //a2 = d1;  //a2 = (d1,f1) = w1
    //a3 = d2;  //a3 = (d2,f2) = w2 

    a1 = -d1*c1;   //a1 = w1*c1
    a2 = d2*c2;   //a2 = w2*c2
    //a1 = -a1;
    a3 = a1/a2;    //a3 = -w1*c1/w2*c2
    if(a3>EPSILON) {
      a1 = log(a3);   //a1 = log(-w1*c1/w2*c2)
      a3 = -(d1-d2);
      if(fabs(a3) > EPSILON) {
	a2 = a1/a3;  //a2 = log(-w1*c1/w2*c2) / (w1-w2)
	tmax = a2;
      }
    }

    if(tmax == 0.0) {
      zmax = z;
    }
    else {
      //evaluate at tmax
      get_deltas(tmax, a1,a3,a2,a3);
      a3 = a1 * c1;
      a1 = a2 * c2;
      zmax = a1+a3;
    }

    if(fabs(zmax) < z)
      zmax = z;

    //printf("max value of oscillator: %f at %f\n", zmax, tmax);
    
    if(fabs(zmax) > limit) {
      float_t alpha = limit / fabs(zmax);
      z *= alpha;
      dz *= alpha;
    }
  }
}

#include "math/misc.h"

float_t solve_limit_scale(float_t z[2], float_t y[2], float_t limit)
{
  //solve for a scale to be applied to the increment
  float_t alpha1, alpha2;
  float_t a, b, c;

  //coefficients of the quadratic equation to get z+alpha*inc on the circle
  a = y[0]*y[0] + y[1]*y[1];
  b = z[0]*y[0] + z[1]*y[1];
  c = z[0]*z[0] + z[1]*z[1] - limit*limit;
  if(c > 0.0) {
    printf("already out of the limit, %f!\n", SQRT(c + limit*limit));
    return 0.0;
  }
  if(quadratic(a,b,c,alpha1,alpha2)) {
    if(alpha1 >= 0.0f && alpha1 <= 1.0f) {
      return alpha1;
    }
    else if(alpha2 >= 0.0f && alpha2 <= 1.0f) {
      return alpha2;
    }
    else {
      printf("quadratic gives bad values %f and %f\n", alpha1, alpha2);
      return 0.0;
    }
  }
  else {
    printf("couldn't solve quadratic\n"); 
    return 0.0;
  }
}

void Oscillator::Crop_Inc(float_t& iz, float_t& idz, float_t limit) const
{
  float_t z = Get_z() + iz;
  float_t dz = Get_dz() + idz;
  Crop_Set(z,dz,limit);
  iz = z - Get_z();
  idz = dz - Get_dz();
  return;

  if(UnderDamped()) {

    float_t z = Get_z()+iz;
    float_t dz = q1+q2-(idz -(d1+d2)*iz*HALF)/wd*TWO;

    float_t len2 = z*z + dz*dz;
    if(len2 > limit*limit) {
      float_t cur [2] = {p1+p2,q1+q2};
      float_t inc [2] = {iz,-(idz -(d1+d2)*iz*HALF)/wd*TWO};
      float_t alpha = solve_limit_scale(cur,inc, limit);

      if(alpha != 0.0) {
	cur[0] += inc[0]*alpha;
	cur[1] += inc[1]*alpha;
	//printf("Dest: %f, limit %f\n", SQRT(cur[0]*cur[0] + cur[1]*cur[1]), limit);
      }
 
      //printf("scale: %f\n", alpha);
      iz *= alpha;
      idz *= alpha;
    }
  }
  else {
    printf("don't yet have overdamped formula\n");
  } 
}

