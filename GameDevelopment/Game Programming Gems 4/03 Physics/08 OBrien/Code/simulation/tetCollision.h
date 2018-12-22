/*********************************************************************
 * tetCollision.h
 *
 * These functions determine the overlap volume, center, and direction
 * of a pair of possibly colliding objects.  Adapted from code written
 * by James O'Brien.
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



#include "math/primitives.h"
#include "math/geometry.h"

 void computeTetTetOverlap(const Vector3 &v10, 
			   const Vector3 &v11, 
			   const Vector3 &v12, 
			   const Vector3 &v13,
				 
			   const Vector3 &v20, 
			   const Vector3 &v21, 
			   const Vector3 &v22, 
			   const Vector3 &v23, Real &vol, Vector3 &cnt, Vector3 &dir);

void computeTetPlaneOverlap(const Vector3 &v0, 
			    const Vector3 &v1, 
			    const Vector3 &v2, 
			    const Vector3 &v3,
			    const Plane &plane, Real &vol, Vector3 &cnt, Vector3 &dir);

void computeTetSphereOverlap(const Vector3 &v0, 
			     const Vector3 &v1, 
			     const Vector3 &v2, 
			     const Vector3 &v3,
			     const Vector3 &center, const Real radius, 
			     Real &vol, Vector3 &cnt, Vector3 &dir);
