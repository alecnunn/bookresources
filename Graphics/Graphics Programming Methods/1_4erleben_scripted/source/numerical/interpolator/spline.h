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
#ifndef __SPLINE_INTERPOLATOR_INCLUDED__
#define __SPLINE_INTERPOLATOR_INCLUDED__
#include<numerical/interpolator/interpolator.h>
#include<numerical/scalar.h>

/**
 * Spline Interpolation.
 * This is an implementation of the algorithm, which is
 * described in Numerical Recipes in C, pp. 113-116.
 *
 */
class SplineInterpolator : public Interpolator
{
public:

  SplineInterpolator();
  virtual ~SplineInterpolator();

public:

  void setTableau(scalar  * x,scalar * y,int cnt);
  void setTableau(scalar * x,scalar * y,int cnt,scalar yDerivStart,scalar yDerivEnd);
  scalar getValue(const scalar & x);
  scalar getErrorEstimate(void);

private:

  scalar * y2;    ///< Temporary Working Storage. Used by the method setTableau() for holding the derivatives of the spline.
  scalar * u;     ///< Temporary Working Storage.Used by the method setTableau(). Used for intermediate calculations.
  int n;          ///< The length of the tableau's
  int N;          ///< The maximum avaible length of the tableau's
  scalar * xa;    ///< Local Reference, used by method getValue().
  scalar * y2a;   ///< Local Reference, used by method getValue().
  scalar * ya;    ///< Local Reference, used by method getValue().

};/* End of class SplineInterpolator */

#endif //__SPLINE_INTERPOLATOR_INCLUDED__
