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
#if(_MSC_VER == 1200)
#pragma warning(disable:4244)   // No warnings on precision truncation
#pragma warning(disable:4305)   // No warnings on precision truncation
#pragma warning(disable:4786)   // stupid symbol size limitation
#endif
#include <numerical/interpolator/polynomial.h>
#include <utils/output.h>
#include<cmath>
using namespace std;

/**
 * Default Constructor.
 */
PolynomialInterpolator::PolynomialInterpolator(){
  error = 0.0f;
  n     = -1;
  xa    = 0;
  ya    = 0;
  C     = 0;
  D     = 0;
};

/**
 * Deconstructor.
 */
PolynomialInterpolator::~PolynomialInterpolator(){
  if(C)
    delete [] C;
  if(D)
    delete [] D;
};

/**
 * Set Tableau Method.
 *
 * @param x   The array of parameter values.
 * @param y   The array of corresponding function values. Must
 *            have same length as the x-array.
 */
void PolynomialInterpolator::setTableau(scalar * x,scalar * y,int cnt){

  //--- Assign local data members
  this->xa = x;
  this->ya = y;

  //--- (Lazy) Allocate internal storage needed during the computation
  if(C==0){
    C = new scalar[cnt];
    D = new scalar[cnt];
  }else if(cnt<n){
    delete [] C;
    C = new scalar[cnt];
    delete [] D;
    D = new scalar[cnt];
  }
  this->n = cnt;
};

/**
 * Get Interpolation Value.
 *
 * @param x    The point at which the interpolated or extrapolated
 *             value should be found at.
 *
 * @return     The interpolated (or extrapolated) value at
 *             the point x.
 */
scalar PolynomialInterpolator::getValue(const scalar & x){
  //--- First we find the the closest table
  //--- entry index to x and initialize tables
  //--- C and D at the same time.
  scalar diff = (scalar)fabs(x-xa[0]);
  int ns = 0;
  D[0] = C[0] = ya[0];
  for(int i=1;i<n;i++){
    scalar test = (scalar)fabs(x-xa[i]);
    if(test<diff){
      ns =  i;
      diff = test;
    }
    C[i] = D[i] = ya[i];
  }

  //--- This is the initial approximation to y=f(x).
  scalar y = ya[ns];     //---- This was original ya[ns--]

  for(int m=1;m<n;m++){
    for(int i=0;i<(n-m);i++){
      scalar ho = xa[i]-x;
      //--- You might wonder why the line below doesn't say
      //---   hp = xa[i+m+1]-x
      //--- This is because the addition with the constant 1
      //--- have been moved to the outer loop
      scalar hp = xa[i+m]-x;
      scalar w = C[i+1]-D[i];
      scalar denominator= ho-hp;
      if(denominator==0){
        //--- This error can occur only if two input x's are (to within roundoff) identical.
        Output::error(
                      "PolynomialInterpolator",
                      "getValue()",
                      "Unexpected internal error, unable to compute!"
                      );
        return 0.0f;
      }
      denominator = w / denominator;
      //---
      //---              ( x(i+m+1) - x )  ( C(m,i+1) - D(m,i) )
      //---  D(m+1,i) = -----------------------------------------
      //---                         x(i) - x(i+m+1)
      //---
      D[i] = hp*denominator;
      //---
      //---              ( x(i) - x )  ( C(m,i+1) - D(m,i) )
      //---  C(m+1,i) = -------------------------------------
      //---                       x(i) - x(i+m+1)
      //---
      C[i] = ho*denominator;
    }
    //---
    //--- The value of (n-m) is the current number of rows
    //--- in the tableaus of C and D
    //---
    if((2*ns)<(n-m)){
      error = C[ns];        //--- This was original C[ns+1]
    }else{
      ns = ns - 1;
      error = D[ns];       //--- This was original D[ns--]
    }
    y += error;
  }

  return y;
}

/**
 * Error Estimate Method.
 *
 * @return   The value of the error estimate.
 */
scalar PolynomialInterpolator::getErrorEstimate(){
  return error;
};
