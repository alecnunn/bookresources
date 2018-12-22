/*********************************************************************
 * fastvector.h
 * Authored by Kris Hauser 2002-2003
 *
 * Declares vector_type (an efficient C-style vector)
 * and several inlined helper functions.
 *
 * Copyright (c) 2002 SquireSoft, used with permission.
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



#ifndef FAST_VECTOR_H
#define FAST_VECTOR_H

#include <stdlib.h>
#include <stdio.h>

#include "primitives.h"

typedef Real* vector_type;

inline vector_type vector_create(int n)
{
	if(n != 0)
		return (Real*)malloc(sizeof(Real)*n);
	return NULL;
}

inline void vector_delete(vector_type v)
{
	if(v)
		free(v);
}


inline void vector_equal(vector_type a, const vector_type b, int n)
{
	int i;
	for(i=0;i<n;i++)
		a[i] = b[i];
}

inline vector_type vector_copy(const vector_type a, int n)
{
  vector_type temp = vector_create(n);
  vector_equal(temp, a, n);
  return temp;
}

inline void vector_zero(vector_type a, int n)
{
	int i;
	for(i=0;i<n;i++)
		a[i] = Zero;
}

inline void vector_add(vector_type x, const vector_type a, const vector_type b, int n)
{
	int i;
	for(i=0;i<n;i++)
		x[i] = a[i]+b[i];
}

inline void vector_sub(vector_type x, const vector_type a, const vector_type b, int n)
{
	int i;
	for(i=0;i<n;i++)
		x[i] = a[i]-b[i];
}

inline void vector_multiply(vector_type x, const vector_type a, Real s, int n)
{
	int i;
	for(i=0;i<n;i++)
		x[i] = a[i]*s;
}

inline void vector_madd(vector_type x, const vector_type a, Real s, int n)
{
	int i;
	for(i=0;i<n;i++)
		x[i] += a[i]*s;
}

inline void vector_scale(vector_type a, float s, int n)
{
	int i;
	for(i=0;i<n;i++)
		a[i] *= s;
}

inline void vector_scale(vector_type a, const vector_type b, float s, int n)
{
	vector_multiply(a,b,s,n);
}

inline Real vector_dot(const vector_type a, const vector_type b, int n)
{
	int i;
	Real sum = Zero;
	for(i=0;i<n;i++)
		sum += a[i]*b[i];
	return sum;
}

inline void vector_print(vector_type v, int n)
{
	printf("(");
	for(int j=0; j<n; j++)
	{
		printf("%f\t", v[j]);
	}
	printf(")\n");
}

#endif

