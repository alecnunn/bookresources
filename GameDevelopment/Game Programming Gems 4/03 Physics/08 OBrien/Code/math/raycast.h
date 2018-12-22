/*********************************************************************
 * raycast.h
 * Authored by Kris Hauser 2002-2003
 *
 * Borrowed heavily from code written by James O'Brien.
 * Structure to store the results of a ray-casting operation.
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



#include "geometry.h"

class RayCast
{
public:
  inline RayCast();
  inline RayCast(const Ray &ray , Real tMin = 0.0, Real tMax = Inf, long data = 0);
  inline RayCast(const RayCast &that);
  
  inline RayCast &operator=(const RayCast &that); 

  inline Vector3 minPos() const { return ray.source + tMin*ray.direction; }
  inline Vector3 maxPos() const { return ray.source + tMax*ray.direction; }

	Ray ray;
	Real tMin,tMax;
	long data;
};

inline RayCast::RayCast() :
  tMin(Zero),
  tMax(Inf),
  data(0) {
}
  

inline RayCast::RayCast(const Ray &_ray , Real _tMin, Real _tMax, long _data) :
  ray ( _ray),
  tMin(_tMin),
  tMax(_tMax),
  data(_data) {
}

inline RayCast::RayCast(const RayCast &that) :
  ray (that.ray),
  tMin(that.tMin),
  tMax(that.tMax),
  data(that.data) {
}
  
inline RayCast &RayCast::operator=(const RayCast &that) {
  ray  = that.ray ;
  tMin = that.tMin;
  tMax = that.tMax;
  data = that.data;
  return (*this);
}  

