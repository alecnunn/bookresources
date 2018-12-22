/*********************************************************************
 * interp.h
 * Authored by Kris Hauser 2002-2003
 *
 * Several templated interpolation methods.
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


#ifndef INTERP_H
#define INTERP_H

/*
 * seek_segment()
 * given n sample locations xj, finds a range [xj,xj+1]
 * in which the given u value lies.  returns -1 if it's
 * before x0, n-1 if it's after xn-1.
 * Assumes xj < xj+1 for all j
 */
template <class real>
int seek_segment(const real* x, int n, real u)
{
  int j;
  //find which segment (xj, xj+1) to use
  for(j=0;j<n;j++)
    if(x[j] > u)
      break;
  j = j-1;
  return j;
}

/*
 * lin_interp()
 * given y0, y1
 * interpolates the function piecewise linearly at a point u
 * where t = (u-x0)/(x1 - x0)
 * to produce the answer z.
 */
template <class real, class val>
void lin_interp(const val& y0, const val& y1, real t, val& z)
{
	z = y0*(1.0-t);
	z += y1*t;
}


/*
 * lin_interp_piecewise()
 * given n values of a function yj = f(xj), f:R->val,
 * interpolates the function piecewise linearly at a point u
 * to produce the answer z.
 * Assumes xj < xj+1 for all j.
 */
template <class real, class val>
void lin_interp_piecewise(const real* x, const val* y, int n, real u, val& z)
{
	int j = seek_segment(x,n,u);

	if(j < 0)
		z = y[0];
	else if(j >= n-1)
		z = y[n-1];
	else
		lin_interp(y[j], y[j+1], (u-x[j])/(x[j+1]-x[j]), z);
}

/*
 * cardinal_coeffs()
 * returns the cardinal spline coefficients C[0:3] at this t value
 * with s = tension.
 */
template <class real>
void cardinal_coeffs(real t, real s, real C[])
{
	real t2 = t*t, t3 = t2*t;

	C[0]=s*(-t3 + 2.f*t2 - t);
	C[1]=(2.f-s)*t3 + (s-3.f)*t2 + 1.0f;
	C[2]=(s-2.f)*t3 + (3.f-2.f*s)*t2 + s*t;
	C[3]=s*(t3 - t2);
}

/*
 * cubic_coeffs()
 * returns the cubic spline coefficients C[0:3] at this t value.
 */
template <class real>
inline void cubic_coeffs(real t, real C[])
{
	cardinal_coeffs(t,(real)0.5,C);
}

/*
 * cubic_interp()
 * given y-1...y2
 * interpolates the function piecewise cubically at a point u
 * where t = (u-x0)/(x1 - x0)
 * to produce the answer z.
 */
template <class real, class val>
void cubic_interp(const val& y_1, const val& y0, const val& y1, const val& y2, real t, val& z)
{
	real C[4];
	cubic_coeffs(t, C);
	z = y_1*C[0];
	z += y0*C[1];
	z += y1*C[2];
	z += y2*C[3];
}

/*
 * cubic_interp_piecewise()
 * given n values of a function yj = f(xj), f:R->val,
 * interpolates the function piecewise cubically at a point u
 * to produce the answer z.
 * Assumes xj < xj+1 for all j.
 */
template <class real, class val>
void cubic_interp_piecewise(const real* x, const val* y, int n, real u, val& z)
{
	int j = seek_segment(x,n,u);

	if(j < 0)
		z = y[0];
	else if(j >= n-1)
		z = y[n-1];
	else {
		int j_1 = j-1;
		if(j_1 < 0)
			j_1 = 0;
		int j2 = j+2;
		if(j2 > n-1)
			j2 = n-1;

		cubic_interp(y[j_1], y[j], y[j+1], y[j2], (u-x[j])/(x[j+1]-x[j]), z);
	}
}


/*
 * interp_piecewise()
 * given n values of a function yj = f(xj), f:R->val,
 * and an interpolation function S(y, n, i, t, z) on the segment i
 * interpolates the function piecewise at a point u
 * to produce the answer z.
 * Assumes xj < xj+1 for all j.
 * Note: S(const real* y, int n, int i, real t, val& z)
 *   all vars are the same as the arguments, except for i and t.
 *   i: the i'th segment to interpolate (between xi and xi+1)
 *   t: the normalized u parameter along the segment [xi, xi+1]
 */
template <class real, class val>
void interp_piecewise(const real* x, const val* y, int n, real u,
		      void (*S) (const val* y, int n, int i, real t, val& z),
		      val& z)
{
	int j = seek_segment(x,n,u);

	if(j < 0)
		z = y[0];
	else if(j >= n-1)
		z = y[n-1];
	else {
	  S(y, n, j, (u-x[j])/(x[j+1]-x[j]), z);
	}
}

#endif
