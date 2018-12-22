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
#ifndef __CUBIC_CURVE_INCLUDED__
#define __CUBIC_CURVE_INCLUDED__
#include <numerical/scalar.h>
#include <numerical/vectorN.h>
#include <numerical/matrixMxN.h>

/**
 * A Cubic Curve.
 * This class encapsylates the basic behavior which all
 * kinds of cubic curves (bezier, hermite, b-spline curve
 * and many more) must share.
 *
 * In our opion there are only four differences between
 * cubic curves, these are:
 *
 *   # How does the base matrix look like (note
 *     it is always 4x4)?
 *   # What is the geometrical interpretation of
 *     the control points?
 *   # Is the curve affine invariant?
 *   # Does the control points have the convex
 *     hull property?
 *
 * Note that the base matrix together with the power vector
 * defines the blending functions of the curve.
 *
 * Now let us briefly take a look at some of the terms
 * and definitions we are going to use. A cubic curve has
 * the following form:
 *
 *   Q(u) = a u^3 + b u^2 + c u + a
 *
 * Looking at it in matrix form we see
 *
 *   Q(u) = U M G
 *   Q(u) = [u^3, u^2, u + 1] M [g0, g1, g2, g3]^T
 *
 * From this we observe that
 *
 *   a = m00 g0 + m01 g1 + m02 g2 + m03 g3
 *   b = m10 g0 + m11 g1 + m12 g2 + m13 g3
 *   c = m20 g0 + m21 g1 + m22 g2 + m23 g3
 *   d = m30 g0 + m31 g1 + m32 g2 + m33 g3
 *
 * These row vectors make up the coefficient
 * matrix C, that is
 *
 *   Q(u) = U C
 *
 * Now G is known as the geometry matrix (or vector as it is
 * sometimes called), U is named the power vector and M the
 * basis matrix. Finally C is called the coefficient matrix.
 *
 * The shape of a cubic curve is altered by changing the
 * geometry matrix. Usually the cubic curve is only defined
 * over the interval u>=0 and u<=1.
 */
class CubicCurve
{

  friend class Spline;

public:

  CubicCurve();
  virtual ~CubicCurve();

public:

  void setGeometry(
                   const VectorN & g0,
                   const VectorN & g1,
                   const VectorN & g2,
                   const VectorN & g3
                   );
	void setGeometry(const int & i,const VectorN & g);
	void getGeometry(const int & i,VectorN & g);
	void getCoefficientRow(const int & i,VectorN & C);
	void setBase(const MatrixMxN & base);
  void getBase(MatrixMxN & base);
  void getPointAtParameterValue(const scalar & u,VectorN & p);
  void getDerivativesAtParameterValue(const scalar & u,VectorN & p,VectorN & dp,VectorN & ddp);
	scalar getTotalArcLength(void);
	scalar getArcLength(const scalar & ustart,const scalar & uend);
	scalar getCoordinateAtCoordinate(const int & i,const int & j,const scalar & value);
  scalar getParameterValueAtCoordinate(const int & j,const scalar & value);
  scalar getCoordinateAtParameterValue(const int & i,const scalar & u);
  scalar getParameterValueAtArcLength(const scalar & s);

protected:
  
  scalar getArcIntegrand(const scalar & u);

protected:

	/**
	 * Arc Length.
	 * The total arc length of the curve.
	 * This is computed when the geometry vector is set.
	 */
	scalar totalArcLength;

	/**
	 * The Basis Matrix.
	 * This is a 4x4 matrix, it acutally defines
	 * the coefficients of the blending functions.
	 */
	MatrixMxN M;

	/**
	 * Coefficient Matrix.
	 * The coefficient matrix as four row vectors.
	 *
	 *     | a |   | m00 m01 m02 m03 | | g0 |    a = m00*g0 + m01*g1 + m02*g2 + m03*g3
	 *     | b |   | m10 m11 m12 m13 | | g1 |    b = m10*g0 + m11*g1 + m12*g2 + m13*g3
	 * C = | c | = | m20 m21 m22 m23 | | g2 | => c = m20*g0 + m21*g1 + m22*g2 + m23*g3
	 *     | d |   | m30 m31 m32 m33 | | g3 |    d = m30*g0 + m31*g1 + m32*g2 + m33*g3
	 *
	 */
	VectorN a;
	VectorN b;
	VectorN c;
	VectorN d;

	/**
	 * The Geometry Matrix.
	 * The geometry matrix as four row vectors (also
	 * called geometry vectors or control points).
	 */
	VectorN g0;
	VectorN g1;
	VectorN g2;
	VectorN g3;

	/**
	 * Working storage.
	 * This vector is used for intermediate computations.
	 */
	VectorN tmp;

};/* End of class CubicCurve */

#endif //__CUBIC_CURVE_INCLUDED__
