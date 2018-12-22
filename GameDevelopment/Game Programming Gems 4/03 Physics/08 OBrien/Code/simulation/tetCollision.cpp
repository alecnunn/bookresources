/*********************************************************************
 * tetCollision.cpp
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



extern "C" {
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
}

//to make James' code portable
#define mag(x) ((x).norm())
#define sqrMag(x) ((x).normSquared())

#include "tetCollision.h"

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------

// Support for tet clip

class TetPolygon {
public:

  inline TetPolygon();
  inline TetPolygon(const TetPolygon &that);
  
  inline       Vector3 &operator[](unsigned int i)      ;
  inline const Vector3 &operator[](unsigned int i) const;

  inline unsigned int getnum() const;
  inline void setnum(unsigned int num);

  inline void addPoint(const Vector3 &v);


  static const unsigned int MaxVerts = 7;


protected:

  unsigned int numV;
  Vector3    vers[MaxVerts];

};

inline TetPolygon::TetPolygon() 
  : numV(0) {
}

inline TetPolygon::TetPolygon(const TetPolygon &that) 
  : numV(that.numV) {
  unsigned int i;
  for(i=0;i<numV;i++) {
    vers[i] = that[i];
  }
}
  
inline Vector3 &TetPolygon::operator[](unsigned int i) {
#ifdef DEBUG
  if (i>=numV) abort();
#endif
  return vers[i];
}

inline const Vector3 &TetPolygon::operator[](unsigned int i) const {
#ifdef DEBUG
  if (i>=numV) abort();
#endif
  return vers[i];
}
 
inline unsigned int TetPolygon::getnum() const {
  return numV;
}
  
inline void TetPolygon::setnum(unsigned int num) {
  if (num>MaxVerts) {
    cerr << "TetPolygon::setnum: Fatal error -- overflow .\n" << flush;
    throw TetPolygon(*this);
  }
  numV = num;
}

inline void TetPolygon::addPoint(const Vector3 &v) {
  setnum(numV+1);
  vers[numV-1] = v;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

static inline void clipPolygon(TetPolygon &f, const Vector3 norm, const Real offs) {

  const double eps = 1e-8;

  TetPolygon  tmp(f);
  double      dst[TetPolygon::MaxVerts]; 
  
  unsigned int i;

  f.setnum(0);

  for(i=0;i<tmp.getnum();i++) {
    dst[i] = dot(norm,tmp[i]) - offs;
  }

  for(i=0;i<tmp.getnum();i++) {
    unsigned int j = i+1; if (j == tmp.getnum()) j = 0;
    if ((dst[i]>eps) && (dst[j]<-eps)) {
      double u = dst[i]/(dst[i]-dst[j]);
      f.addPoint((1.0 - u) * tmp[i] + u * tmp[j]);
    }
    if (dst[i]<=eps) {
      f.addPoint(tmp[i]);
    }
    if ((dst[i]<-eps) && (dst[j]>eps)) {
      double u = dst[i]/(dst[i]-dst[j]);
      f.addPoint((1.0 - u) * tmp[i] + u * tmp[j]);
    }
  }
  if (f.getnum() < 3) f.setnum(0);
}
  
//---------------------------------------------------------------------
//---------------------------------------------------------------------

static inline void clipPolygonSphere(TetPolygon &f, const Vector3 center, const Real radius) {

  const double eps = 1e-8;

  TetPolygon  tmp(f);
  double      dst[TetPolygon::MaxVerts]; 
  
  unsigned int i;

  f.setnum(0);

  for(i=0;i<tmp.getnum();i++) {
    dst[i] = mag(tmp[i]-center) - radius;
  }

  for(i=0;i<tmp.getnum();i++) {
    unsigned int j = i+1; if (j == tmp.getnum()) j = 0;
    if ((dst[i]>eps) && (dst[j]<-eps)) {
      double u = dst[i]/(dst[i]-dst[j]);
      f.addPoint((1.0 - u) * tmp[i] + u * tmp[j]);
    }
    if (dst[i]<=eps) {
      f.addPoint(tmp[i]);
    }
    if ((dst[i]<-eps) && (dst[j]>eps)) {
      double u = dst[i]/(dst[i]-dst[j]);
      f.addPoint((1.0 - u) * tmp[i] + u * tmp[j]);
    }
  }
  if (f.getnum() < 3) f.setnum(0);
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------

unsigned int tc = 0;
unsigned int nc = 0;
unsigned int vc = 0;
unsigned int ct = 0;

void computeTetTetOverlap(const Vector3 &v10, 
				 const Vector3 &v11, 
				 const Vector3 &v12, 
				 const Vector3 &v13,
				 
				 const Vector3 &v20, 
				 const Vector3 &v21, 
				 const Vector3 &v22, 
				 const Vector3 &v23, Real &vol, Vector3 &cnt, Vector3 &dir) {

  vol = 0.0;
  cnt = 0.0;
  dir = 0.0;

  double     offs;
  Vector3  norm;

  static TetPolygon f[8];     

  f[0].setnum(3); f[0][0] = v11; f[0][1] = v12; f[0][2] = v13;
  f[1].setnum(3); f[1][0] = v10; f[1][1] = v13; f[1][2] = v12;
  f[2].setnum(3); f[2][0] = v10; f[2][1] = v11; f[2][2] = v13;
  f[3].setnum(3); f[3][0] = v10; f[3][1] = v12; f[3][2] = v11;
  
  norm = cross(v22-v21,v23-v21); offs = dot(norm,v21); ct++;
  clipPolygon(f[0],norm,offs);
  clipPolygon(f[1],norm,offs);
  clipPolygon(f[2],norm,offs);
  clipPolygon(f[3],norm,offs);
  
  if ( (f[0].getnum() == 0) &&
       (f[1].getnum() == 0) &&
       (f[2].getnum() == 0) &&
       (f[3].getnum() == 0) ) {
    // Could be a total interior collision, (#2 inside of #1) but this
    // would yield a 0 force direction, so ignore it...
    return;
  }

  norm = cross(v23-v20,v22-v20); offs = dot(norm,v20); ct++;
  clipPolygon(f[0],norm,offs);
  clipPolygon(f[1],norm,offs);
  clipPolygon(f[2],norm,offs);
  clipPolygon(f[3],norm,offs);
  
  if ( (f[0].getnum() == 0) &&
       (f[1].getnum() == 0) &&
       (f[2].getnum() == 0) &&
       (f[3].getnum() == 0) ) {
    // Could be a total interior collision, (#2 inside of #1) but this
    // would yield a 0 force direction, so ignore it...
    return;
  }

  norm = cross(v21-v20,v23-v20); offs = dot(norm,v20); ct++;
  clipPolygon(f[0],norm,offs);
  clipPolygon(f[1],norm,offs);
  clipPolygon(f[2],norm,offs);
  clipPolygon(f[3],norm,offs);
  
  if ( (f[0].getnum() == 0) &&
       (f[1].getnum() == 0) &&
       (f[2].getnum() == 0) &&
       (f[3].getnum() == 0) ) {
    // Could be a total interior collision, (#2 inside of #1) but this
    // would yield a 0 force direction, so ignore it...
    return;
  }

  norm = cross(v22-v20,v21-v20); offs = dot(norm,v20); ct++;
  clipPolygon(f[0],norm,offs);
  clipPolygon(f[1],norm,offs);
  clipPolygon(f[2],norm,offs);
  clipPolygon(f[3],norm,offs);
  
  if ( (f[0].getnum() == 0) &&
       (f[1].getnum() == 0) &&
       (f[2].getnum() == 0) &&
       (f[3].getnum() == 0) ) {
    // Could be a total interior collision, (#2 inside of #1) but this
    // would yield a 0 force direction, so ignore it...
    return;
  }

  // ---------------------------------------------
  
  f[4].setnum(3); f[4][0] = v21; f[4][1] = v22; f[4][2] = v23;
  f[5].setnum(3); f[5][0] = v20; f[5][1] = v23; f[5][2] = v22;
  f[6].setnum(3); f[6][0] = v20; f[6][1] = v21; f[6][2] = v23;
  f[7].setnum(3); f[7][0] = v20; f[7][1] = v22; f[7][2] = v21;
  
  norm = cross(v12-v11,v13-v11); offs = dot(norm,v11); ct++;
  clipPolygon(f[4],norm,offs);
  clipPolygon(f[5],norm,offs);
  clipPolygon(f[6],norm,offs);
  clipPolygon(f[7],norm,offs);
  
  if ( (f[4].getnum() == 0) &&
       (f[5].getnum() == 0) &&
       (f[6].getnum() == 0) &&
       (f[7].getnum() == 0) ) {
    // Could be a total interior collision, (#1 inside of #2) but this
    // would yield a 0 force direction, so ignore it...
    return;
  }

  norm = cross(v13-v10,v12-v10); offs = dot(norm,v10); ct++;
  clipPolygon(f[4],norm,offs);
  clipPolygon(f[5],norm,offs);
  clipPolygon(f[6],norm,offs);
  clipPolygon(f[7],norm,offs);

  if ( (f[4].getnum() == 0) &&
       (f[5].getnum() == 0) &&
       (f[6].getnum() == 0) &&
       (f[7].getnum() == 0) ) {
    // Could be a total interior collision, (#1 inside of #2) but this
    // would yield a 0 force direction, so ignore it...
    return;
  }

  norm = cross(v11-v10,v13-v10); offs = dot(norm,v10); ct++;
  clipPolygon(f[4],norm,offs);
  clipPolygon(f[5],norm,offs);
  clipPolygon(f[6],norm,offs);
  clipPolygon(f[7],norm,offs);
    
  if ( (f[4].getnum() == 0) &&
       (f[5].getnum() == 0) &&
       (f[6].getnum() == 0) &&
       (f[7].getnum() == 0) ) {
    // Could be a total interior collision, (#1 inside of #2) but this
    // would yield a 0 force direction, so ignore it...
    return;
  }

  norm = cross(v12-v10,v11-v10); offs = dot(norm,v10); ct++;
  clipPolygon(f[4],norm,offs);
  clipPolygon(f[5],norm,offs);
  clipPolygon(f[6],norm,offs);
  clipPolygon(f[7],norm,offs);
  
  if ( (f[4].getnum() == 0) &&
       (f[5].getnum() == 0) &&
       (f[6].getnum() == 0) &&
       (f[7].getnum() == 0) ) {
    // Could be a total interior collision, (#1 inside of #2) but this
    // would yield a 0 force direction, so ignore it...
    return;
  }

  // ---------------------------------------------

  unsigned int i,v;
  unsigned int fnd = 0;
  Vector3    prj;

  for( i=0; i<8; i++ ) {
    if ( f[i].getnum() != 0 ) {
      if ( !fnd ) {
	fnd = 1;
	prj = f[i][0];
      }
      Vector3  fst = f[i][0];
      for ( v=2; v<f[i].getnum(); v++ ) {
	Vector3 subara = cross(f[i][v]-fst,f[i][v-1]-fst); // Note reverse cross prod...
	double    subvol = dot(prj-fst,subara);
	cnt += (fst + f[i][v] + f[i][v-1] + prj) * subvol;
	vol += subvol;
	
	// Assume first 4 faces are from Tet A
	if (i<4) {
	  dir += subara;
	}
      }
    }
  }
  if (vol != 0.0) cnt /= (vol * 4.0);
  vol /= 6;
}


//---------------------------------------------------------------------
//---------------------------------------------------------------------

void computeTetPlaneOverlap(const Vector3 &v0, 
				   const Vector3 &v1, 
				   const Vector3 &v2, 
				   const Vector3 &v3,
				   const Plane  &plane, Real &vol, Vector3 &cnt, Vector3 &dir) {

  vol = 0.0;
  cnt = 0.0;
  dir = 0.0;
  
  TetPolygon f[4];

  f[0].setnum(3); f[0][0] = v1; f[0][1] = v2; f[0][2] = v3;
  f[1].setnum(3); f[1][0] = v0; f[1][1] = v3; f[1][2] = v2;
  f[2].setnum(3); f[2][0] = v0; f[2][1] = v1; f[2][2] = v3;
  f[3].setnum(3); f[3][0] = v0; f[3][1] = v2; f[3][2] = v1;

  clipPolygon(f[0],plane.normal,-plane.offset);
  clipPolygon(f[1],plane.normal,-plane.offset);
  clipPolygon(f[2],plane.normal,-plane.offset);
  clipPolygon(f[3],plane.normal,-plane.offset);


  // Clipped tet is possible open, missing face that would come from
  // the plane.  Pick prj point so that it lays in the clip plane
  // cancles the open face.

  Vector3  prj;
  if (f[0].getnum() != 0) {
    prj = f[0][0];
  }else if (f[1].getnum() != 0) {
    prj = f[1][0];
  }else if (f[2].getnum() != 0) {
    prj = f[2][0];
  }else if (f[3].getnum() != 0) {
    prj = f[3][0];
  }else{
    return;
  }

  prj.madd(plane.normal, -(dot(plane.normal,prj)+plane.offset));
  
  unsigned int i,v;
  
  for( i=0; i<4; i++ ) {
    if ( f[i].getnum() != 0 ) {
      Vector3  fst = f[i][0];
      for ( v=2; v<f[i].getnum(); v++ ) {
	Vector3 subara = cross(f[i][v]-fst,f[i][v-1]-fst);  // Note reverse cross prod...
	double    subvol = dot(prj-fst,subara);
	cnt += (fst + f[i][v] + f[i][v-1] + prj) * subvol;
	vol += subvol;
	dir += subara;
      }
    }
  }
  if (vol != 0.0) cnt /= (vol * 4.0);
  vol /= 6.0;
}


//---------------------------------------------------------------------
//---------------------------------------------------------------------

void computeTetSphereOverlap(const Vector3 &v0, 
				    const Vector3 &v1, 
				    const Vector3 &v2, 
				    const Vector3 &v3,
				    const Vector3 &center, const Real radius, 
				    Real &vol, Vector3 &cnt, Vector3 &dir) {

  vol = 0.0;
  cnt = 0.0;
  dir = 0.0;
  
  TetPolygon f[4];

  f[0].setnum(3); f[0][0] = v1; f[0][1] = v2; f[0][2] = v3;
  f[1].setnum(3); f[1][0] = v0; f[1][1] = v3; f[1][2] = v2;
  f[2].setnum(3); f[2][0] = v0; f[2][1] = v1; f[2][2] = v3;
  f[3].setnum(3); f[3][0] = v0; f[3][1] = v2; f[3][2] = v1;
  
  clipPolygonSphere(f[0],center,radius);
  clipPolygonSphere(f[1],center,radius);
  clipPolygonSphere(f[2],center,radius);
  clipPolygonSphere(f[3],center,radius);


  // Clipped tet is possible open, missing face that would come from
  // the plane.  Pick prj point so that it lays in the surface of
  // sphere cancles the open face is the tet is small wrt the sphere.

  Vector3  prj ;
  if (f[0].getnum() != 0) {
    prj = f[0][0];
  }else if (f[1].getnum() != 0) {
    prj = f[1][0];
  }else if (f[2].getnum() != 0) {
    prj = f[2][0];
  }else if (f[3].getnum() != 0) {
    prj = f[3][0];
  }else{
    return;
  }

  unsigned int i,j,v;

  for(i=0;i<4;i++) {
    for(j=0;j<f[i].getnum();j++) {
      if ((sqrMag(f[i][j]-center)-(radius*radius))<1e-8) {
	prj = f[i][j];
	i=4;
	j=f[i].getnum();
      }
    }
  }
  
  for( i=0; i<4; i++ ) {
    if ( f[i].getnum() != 0 ) {
      Vector3  fst = f[i][0];
      for ( v=2; v<f[i].getnum(); v++ ) {
	Vector3 subara = cross(f[i][v]-fst,f[i][v-1]-fst);  // Note reverse cross prod...
	double    subvol = dot(prj-fst,subara);
	cnt += (fst + f[i][v] + f[i][v-1] + prj) * subvol;
	vol += subvol;
	dir += subara;
      }
    }
  }
  if (vol != 0.0) cnt /= (vol * 4.0);
  vol /= 6.0;
}
