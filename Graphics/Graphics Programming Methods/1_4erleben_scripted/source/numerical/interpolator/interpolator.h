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
#ifndef __INTERPOLATOR_INCLUDED__
#define __INTERPOLATOR_INCLUDED__
#include <numerical/scalar.h>

/**
 * Interpolator Base Class.
 * All interpolation and extrapolation algorithms must
 * be derived from this base class.
 *
 * This inteface defines the basic methods which all
 * (1-dimensional) interpolation and extrapolation algorithms
 *  must support.
 */
class Interpolator
{
public:
  /**
   * Set Tableau Method.
   * This method should be used to initialize the tableau
   * that should be used to compute the interpolated (or
   * extrapolated) value.
   *
   * We have
   *
   * y0 = f(x0),y1 = f(x1),...yn = f(xn),
   *
   * @param x   The array of parameter values.
   * @param y   The array of corresponding function values. Must
   *            have same length as the x-array.
   * @param cnt The number of value pairs.
   */
  virtual void setTableau(scalar  * x,scalar * y,int cnt)=0;

  /**
   * Get Interpolation Value.
   * This method should compute the interpolated or extrapolated
   * value at the specified point.
   *
   * @param x    The point at which the interpolated or extrapolated
   *             value should be found at.
   *
   * @return     The interpolated (or extrapolated) value at
   *             the point x.
   */
  virtual scalar getValue(const scalar & x)=0;

  /**
   * Error Estimate Method.
   * This method should return the error estimate of the
   * last interpolated value computation, which have
   * taken place.
   *
   * @return   The value of the error estimate.
   */
  virtual scalar getErrorEstimate(void) = 0;

};/* End of class Interpolator */

#endif //__INTERPOLATOR_INCLUDED__

