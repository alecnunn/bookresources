////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002
//  Department of Computer Science Copenhagen University (DIKU).
//
//  All Rights Reserved.
//
//  This software is provided as is WITHOUT ANY WARRANTY; Use it on
//  your own risk.
//
//  Please send remarks, questions and bug reports to kenny@diku.dk
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __NUB_SPLINE_INCLUDED__
#define __NUB_SPLINE_INCLUDED__
#include <numerical/scalar.h>
#include <numerical/vectorN.h>
#include <spline/spline.h>
#include <string>

/**
 * Non-Uniform B-spline.
 * This class is an implementation of nonunform B-splines.
 */
class NUBSpline : public Spline
{
public:

  NUBSpline();
  virtual ~NUBSpline();

public:
  
  NUBSpline * insertKnot(const scalar & t);
  scalar N(const int & i,const int & k,const scalar & u);
	int getSegmentCount(void);
	CubicCurve * getSegment(int i);
  int getDimension(void);
  void getPointOnSpline(const scalar & u,VectorN & p);
  VectorN * getControlPoints(void);
  VectorN * getDataPoints(void);
  scalar * getKnotVector(void);
  void getFastPointOnSpline(const scalar & u,VectorN & p);
  void getDerivatives(const scalar & u,int J,VectorN * dQ);
  int getOrder(void);
  std::string toString(void);

  int getn(void){return n;}

protected:

  void doSetDataPoints(const int & k,VectorN * dataPoints,const int & count,bool cyclic);
	void doSetDataPoints(const int & k,VectorN * dataPoints,const int & count,scalar * knotValues,bool cyclic);
  void doComputeSegments(void);
  void triDiagSolve(scalar * a,scalar * b,scalar * c,VectorN ** X,VectorN * B,int n);
  int findKnotSpan(const scalar & u,const int & n,const int & k,scalar* T);

protected:

	int count;             ///< Curve Segment Count.
	CubicCurve * segments; ///< The Cubic Curve Segments.
	VectorN * P;           ///< The Control Points.
	VectorN * X;           ///< The Data Points.
	scalar  * T;           ///< The Knot Vector.
  int K;                 ///< The order of the spline.
  int n;                 ///< |P| = n+1; |X| = n-1; |T| = n + k;
  int dim;               ///< The dimension of the spline.

};/* End of class NUBSpline */
#endif //__NUB_SPLINE_INCLUDED__
