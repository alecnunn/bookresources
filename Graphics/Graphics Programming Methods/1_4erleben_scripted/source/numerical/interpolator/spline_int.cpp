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
#include<numerical/interpolator/spline.h>
#include<utils/output.h>

/**
 * Default Constructor.
 */
SplineInterpolator::SplineInterpolator(){
  y2  = 0;
  u   = 0;
  N   = n = 0;
  xa  = 0;
  y2a = 0;
  ya  = 0;
};

/**
 * Deconstructor.
 */
SplineInterpolator::~SplineInterpolator(){
  if(y2)
    delete [] y2;
  if(u)
    delete [] u;
};

/**
 * Set Tableau Method.
 *
 * @param x   The array of parameter values.
 * @param y   The array of corresponding function values. Must
 *            have same length as the x-array.
 */
void SplineInterpolator::setTableau(scalar * x,scalar * y,int cnt){
  setTableau(x,y,cnt,infinity,infinity);
};

/**
 *
 */
void SplineInterpolator::setTableau(scalar * x,scalar * y,int cnt,scalar yDerivStart,scalar yDerivEnd){


  //--- Allocate temporary working storage etc.
  if(cnt>N){
    N = cnt;
    if(y2)
      delete [] y2;
    y2 = new scalar[N];
    if(u)
      delete [] u;
    u = new scalar[N];
  }
  this ->n = cnt;
  //--- Setup references used by method getValue
  xa  = x;
  ya  = y;
  y2a = y2;


  scalar p,qn,sig,un;

  if(yDerivStart >= infinity)
    y2[0]=u[0]=0.0f;
  else{
    y2[0] = -0.5f;
    u[0]=(3.0f/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yDerivStart);
  }
  for(int i=1;i<=n-2;i++) {
    sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
    p=sig*y2[i-1]+2.0f;
    y2[i]=(sig-1.0f)/p;
    u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
    u[i]=(6.0f*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
  }
  if(yDerivEnd >= infinity)
    qn=un=0.0f;
  else{
    qn=0.5f;
    un=(3.0f/(x[n-1]-x[n-2]))*(yDerivEnd-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
  }
  y2[n-1]=(un-qn*u[n-2])/(qn*y2[n-2]+1.0f);

  for(int k=n-2;k>=0;k--)
    y2[k]=y2[k]*y2[k+1]+u[k];
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
scalar SplineInterpolator::getValue(const scalar & x){
  int klo,khi,k;
  scalar h,b,a;

  klo=0;
  khi=n-1;

  while (khi-klo > 1) {
    k=(khi+klo) >> 1;
    if (xa[k] > x) khi=k;
    else klo=k;
  }
  h=xa[khi]-xa[klo];
  if(h == 0.0){
    Output::error(
                  "SplineInterpolator",
                  "getValue(...)",
                  "Bad x-array input in method setTableau"
                  );
  }
  a=(xa[khi]-x)/h;
  b=(x-xa[klo])/h;

  scalar y = a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0f;

  return y;
};

/**
 * Error Estimate Method.
 *
 * @return   The value of the error estimate.
 */
scalar SplineInterpolator::getErrorEstimate(){
  Output::warning(
                  "SplineInterpolator",
                  "getErrorEstimate()",
                  "Sorry, not avaible!"
                  );
    return 0;
};

