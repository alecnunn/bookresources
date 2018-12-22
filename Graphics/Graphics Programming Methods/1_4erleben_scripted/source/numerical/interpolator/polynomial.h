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
#ifndef __POLYNOMIAL_INTERPOLATOR_INCLUDED__
#define __POLYNOMIAL_INTERPOLATOR_INCLUDED__
#include <numerical/interpolator/interpolator.h>
#include <numerical/scalar.h>

/**
 * Polynomial Interpolator.
 * This is an implementation of the algorithm, which is
 * described in Numerical Recipes in C, pp. 108-110.
 *
 * The original algorithm as it stands in the book have a small
 * error regarding how it tracks its way through the tableau (try
 * to watch what happens if you extrapolate on the right side, that
 * is only want to pick D's). We have corrected this error in our
 * implementation.
 */
class PolynomialInterpolator : public Interpolator
{
public:

  PolynomialInterpolator();
  virtual ~PolynomialInterpolator();

public:

  void setTableau(scalar  * x,scalar * y,int cnt);
  scalar getValue(const scalar & x);
  scalar getErrorEstimate(void);

private:

  scalar error; ///< Placeholder for the current error esitimate.
  int n;        ///< The current order of the interpolation.
  scalar * xa;  ///< A reference to the table of x-values
  scalar * ya;  ///< A reference to the table of y-values, such that yi = f[xi]
  scalar * C;   ///< Internal working storage, needed by the method getValue()
  scalar * D;   ///< Internal working storage, needed by the method getValue()

};/* End of class PolynomialInterpolator */

#endif //__POLYNOMIAL_INTERPOLATOR_INCLUDED__
