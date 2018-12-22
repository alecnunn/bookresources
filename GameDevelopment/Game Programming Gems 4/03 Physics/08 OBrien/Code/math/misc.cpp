/*********************************************************************
 * misc.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Miscellaneous math functions.
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


#include "misc.h"
#include <stdio.h>

//returns the # of real roots found (-1 if infinite)
int quadratic(Real a, Real b, Real c, Real& x1, Real& x2) {
  //printf("quadratic %f %f %f\n", a, b, c);
  static const Real Four = 4.0;

  if(a == Zero)
  {
	  if(b == Zero)
	  {
		if(c == Zero)
			return -1;
		return 0;
	  }
	  x1 = -c/b;
	  return 1;
  }

  Real det = b*b-Four*a*c;
  if(det < Zero)
    return 0;
  det = Sqrt(det);
  x1 = (-b + det)/a*Half;
  x2 = (-b - det)/a*Half;
  return 2;
}

Real pythag(Real a, Real b)		//reduce roundoff of large numbers
{
	Real absa,absb;
	absa=Abs(a);
	absb=Abs(b);

	if(absa > absb)
		return absa*Sqrt(One + Sqr(b/a));
	else
		return absb*Sqrt(One + Sqr(a/b));
}

