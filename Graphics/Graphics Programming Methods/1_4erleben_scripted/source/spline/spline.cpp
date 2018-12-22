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
#include <spline/spline.h>
#include <spline/nubs.h>
#include <numerical/interpolator/polynomial.h>
#include <utils/output.h>
#include<cmath>
using namespace std;

/**
 * Default Constructor.
 */
Spline::Spline(){
	totalArcLength = 0.0;
	arcLengths = 0;
	cyclic = false;
}

/**
 * Deconstructor.
 */
Spline::~Spline(){
}


/**
 * Assign Data Points.
 *
 * @param k            The order of the spline.
 * @param dataPoints   An array of datapoints, which the
 *                     spline must run through. In other
 *                     terms the keypositions of the spline.
 * @param cyclic       A boolean flag telling whatever a cyclic spline
 *                     should be constucted from the key positions.
 */
void Spline::setDataPoints(int k,VectorN * dataPoints,const int & length,bool cyclic){
  doSetDataPoints(k,dataPoints,length,cyclic);
  doComputeSegments();
	initArcLengthTable();
};

/**
 * Assign Data Points.
 *
 * @param k            The order of the spline.
 * @param dataPoints   An array of datapoints, which the
 *                     spline must run through. In other
 *                     terms the keypositions of the spline.
 * @param knotValues   An array of knot values, which should be used
 *                     to create the spline. The array should contain:
 *                     dataPoints.length+1+k elements.
 * @param cyclic       A boolean flag telling whatever a cyclic spline
 *                     should be constucted from the key positions.
 */
void Spline::setDataPoints(int k,VectorN * dataPoints,const int & length,scalar * knotValues,bool cyclic){
  doSetDataPoints(k,dataPoints,length,knotValues,cyclic);
  doComputeSegments();
	initArcLengthTable();
};

/**
 * Get Point on Spline.
 * There is two phases to this method. First it uses
 * an accumulated chordlength strategy to locate a
 * curve segment of the spline. Afterwards it turns the
 * reparametrization problem into a root-problem.
 *
 * @param s   The arc length along the spline, which
 *            indicates the point we are trying to
 *            compute. This is a normalized arc length,
 *            meaning its value is in the interval [0..1] (if
 *            the spline is non-cyclic, if its cylic the
 *            period is 1).
 * @param p   Upon return this parameter will hold the
 *            values of the computed point.
 *
 * @return    A reference to a vector holding the
 *            value of the computed point.
 */
void Spline::getPointAtArcLength(const scalar & s,VectorN & p){
  int i = getSegmentIndexAtArcLength(s);
  scalar safter = getNormalizedArcLengthAtSegment(i);
	//--- Now i point to the segement on the
	//--- spline with the property
	//---
	//--- arcLength[i-1] < s < archLength[i+1]
	//---
	//--- In other words the point we seek must lie
	//--- on the i'th segement of the spline.
	CubicCurve * segment = getSegment(i);
  scalar slocal = getTotalArcLength()*(s-safter);
  scalar ulocal = segment->getParameterValueAtArcLength(slocal);

	segment->getPointAtParameterValue(ulocal,p);
};

/**
 * Get the Total Arc Length.
 *
 * Note the method initArcLengthTable must have been invoked
 * prior to the invokation of this method.
 *
 * @return     The total arc Length of entire spline.
 */
scalar Spline::getTotalArcLength(){
	return totalArcLength;
};

/**
 * Get Normalized Arclength.
 * This method retrieves the value of the arc
 * length of the spline at (begining of) the
 * specified curve segement.
 *
 * Note the method initArcLengthTable must have been invoked
 * prior to the invokation of this method.
 *
 * @param i   The index of the curve segment.
 *
 * @return    The normalized arclength at the
 *            specified curve segement.
 */
scalar Spline::getNormalizedArcLengthAtSegment(const int & i){
	return arcLengths[i];
};

/**
 * Set Arclength.
 * This specialized method computes the total arclength of
 * the spline and at the same time it initializes an
 * accumulated table of normalized curve segment arclengths.
 *
 * It is these values that is returned by the
 * getNormalizedArcLengthAtSegment method.
 */
void Spline::initArcLengthTable(){
	//--- Get the number of curve segements, if we have
	//--- n control points this must be n - 3.
	int count = getSegmentCount();
	//--- Allocate space for the accumulated arc
	//--- length table.
	arcLengths = new scalar[count+1];

	totalArcLength = 0.0f;
  arcLengths[0] = 0.0f;
	for(int i=0;i<count;i++){
		CubicCurve * segment = getSegment(i);
		totalArcLength += segment->getTotalArcLength();
    arcLengths[i+1] = totalArcLength;
	}

	//--- Normalize arc lengths, this means when lookup
	//--- is done in this table it appears as though the
	//--- arc length runs from 0 to 1.
	for(int j=0;j<=count;j++){
		arcLengths[j] /= totalArcLength;
	}
};

/**
 * Test Cyclic.
 * This method should be used to test if the spline is cyclic.
 *
 * @return        if the spline is cyclic then the return
 *                value is true otherwise it is false.
 */
bool Spline::isCyclic(){
	return cyclic;
};

/**
 * Get Cubic Curve Segment Index.
 * This method retrieves the index of the cubic
 * curve segment of the spline at the arc length s.
 *
 * @param s     The value of the arc length, which indicates
 *              the segment we are looking for.
 *
 * @return      The index value of the cubic curve segment.
 */
int Spline::getSegmentIndexAtArcLength(const scalar & s){
  scalar stmp = s;
  
  //--- Handle arclength in case of a cyclic spline.
  if(stmp<0.0f){
		if(!cyclic){
      Output::error("Spline","getSegmentIndexAtArcLength","Invalid normalized arclength, non cyclic spline requires values in [0..1]");
		}else{
			do{
				stmp = stmp + 1.0f;
			}while(stmp<0.0f);
		}
	}

	if(stmp>1.0f){
		if(!cyclic){
      Output::error("Spline","getSegmentIndexAtArcLength","Invalid normalized arclength, non cyclic spline requires values in [0..1]");
		}else{
			do{
				stmp = stmp - 1.0f;
			}while(stmp>1.0f);
		}
	}
	//--- First we try to find the correct
	//--- curve segement on the spline.
	int i=0;
	do{
		i++;
	}while(getNormalizedArcLengthAtSegment(i)<stmp);
	i--;
  return i;
};

/**
 * Get Cubic Curve Segement Index.
 * This method retrives the index of the cubic curve segment
 * of the spline at the the point where the j'th coordinate
 * assumes the specified value.
 *
 * The method implicitly assumes that the j'th parameter is
 * a monotom function.
 *
 * @param     j   The index value of the j'th coordinate.
 * @param value   The value of the j'th parameter at which
 *                we are looking for the corresponding
 *                segment.
 *
 * @return        The index of the segement where the j'th coordinate
 *                of the spline assumes the specified value.
 */
int Spline::getSegmentIndexAtCoordinate(const int & j,const scalar & value){
  VectorN C(this->getDimension());
  //--- We could have used an accumulation table for this, just
  //--- like we do with the arc length table. However we always
  //--- know that coordinates boundaries are tested where the
  //--- local parameter value of the segements are zero, so
  //--- computing the coordinates really does not require any
  //--- computation!!! The coordinates are simply read directly
  //--- from the coordinate matrix of the cubic curve.
  scalar min=0.0f,max=0.0f;
  for(int i=0;i<getSegmentCount();i++){
    CubicCurve * curve = getSegment(i);
    min = curve->d.entry[j];
    max = curve->a.entry[j] + curve->b.entry[j] + curve->c.entry[j] + curve->d.entry[j];
    if(min<=value && value<max)
      return i;
  }
  if(value==max){
    return getSegmentCount()-1;
  }
  return -1;
};

/**
 * The Linear Scripted Motion Function.
 * This method reparameterizes the (3 dimensional) space
 * spline (this spline) into the following spline.
 *
 *   C(t) ~ C(U(S(t)))
 *
 * Afterwards it computes the values C(t), C'(t) and C''(t).
 *
 * @param t   The value of the point in time where the
 *            scirpted motion should be computed.
 * @param V   The (2-dimensional) velocity spline, dictating
 *            the tavelling distance as a function of time, that
 *            is V(v) -> (t(v),s(t)).
 * @param p   Upon return this reference holds the point on the
 *            spline at the specified time, that is C(t).
 * @param v   Upon return this reference holds the velocity at
 *            the speicfied point in time, that is C'(t).
 * @param a   Upon return this reference holds the acceleration at
 *            the speicfied point in time, that is C''(t).
 */
void Spline::scriptedMotion(const scalar & t,NUBSpline & V,VectorN & p,VectorN & v,VectorN & a){
  //Output::message("Spline","scriptedMotion(...)","Invoked");

  //--- First some safety checks to verify the integrity of the
  //--- specified parameters...
  if(this->getOrder()!=4){
    Output::error(
                  "Spline",
                  "scriptedMotion(...)",
                  "Space spline was not a cubic spline"
                  );
    return;
  }

  if(V.getOrder()!=4){
    Output::error(
                  "Spline",
                  "scriptedMotion(...)",
                  "Velocity spline was not a cubic spline"
                  );
    return;
  }

  if(this->getDimension()!=3){
    Output::error(
                  "Spline",
                  "scriptedMotion(...)",
                  "Space spline must be in 3 dimensional space"
                  );
    return;
  }

  if(V.getDimension()!=2){
    Output::error(
                  "Spline",
                  "scriptedMotion(...)",
                  "Velocity spline must be in 2 dimensional space"
                  );
    return;
  }

  if(t<0){
    Output::error(
                  "Spline",
                  "scriptedMotion(...)",
                  "Time cannot be negative!"
                  );
    return;
  }

  CubicCurve * firstVsegment = V.getSegment(0);
  scalar tmin = firstVsegment->getCoordinateAtParameterValue(0,0.0f);
  if(t<tmin){
    Output::error(
                  "Spline",
                  "scriptedMotion(...)",
                  "Time exeeded lower bound on velocity spline!"
                  );
    return;
  }

  CubicCurve * lastVsegment = V.getSegment(V.getSegmentCount()-1);
  scalar tmax = lastVsegment->getCoordinateAtParameterValue(0,1.0f);
  if(t>tmax){
    Output::error(
                  "Spline",
                  "scriptedMotion(...)",
                  "Time exeeded upper bound on velocity spline!"
                  );
    return;
  }

  //--- Now we are ready to begin, first we take care
  //--- of the time reparameterization.

  int j = V.getSegmentIndexAtCoordinate(0,t);

  scalar Vscale = V.getKnotVector()[j+4]-V.getKnotVector()[j+3];

  CubicCurve * Vsegment = V.getSegment(j);
  
  //--- This is the value of v(t)
  scalar vt = Vsegment->getParameterValueAtCoordinate(0,t);
  VectorN Vvt(V.getDimension());
  VectorN dVvt(V.getDimension());
  VectorN ddVvt(V.getDimension());
  
  //--- Oh now Im getting derivatives with respect to local parameter value
  Vsegment->getDerivativesAtParameterValue(vt,Vvt,dVvt,ddVvt);
  
  //--- Convert derivatives so they match the global parameter
  //---
  //--- let u = f(U)
  //---
  //--- We have
  //---
  //---   C(U) = B(u) = B(f(U))
  //---
  //--- And we want to know
  //---
  //---  d C(U)      d B(f(U))
  //--- --------  = -----------
  //---   dU           dU
  //---
  //--- And
  //---
  //---
  //---  d2 C(U)      d2 B(f(U))
  //--- --------  = -----------
  //---   dU2           dU2
  //---
  //--- We know
  //---
  //--- f'(U) = 1/T  and f''(U) = 0
  //---
  //---                  3         2
  //---  B(f(U)) = A f(U)  + B f(U)  + C f(U) + D
  //---
  //---                     2
  //---  B'(f(U)) = 3 A f(U) f'(U)  +2 B f(U) f'(U)  + C f'(U) =  B'(u) / T
  //---
  //---                           2         2                2
  //---  B''(f(U)) = 6 A f(U)f''(U) + 3Af(U) f''(U) + 2Bf'(U)  + 2 B f(U)f''(U)  + C f''(U) = B'(u)/(T*T)
  //---
  dVvt.div(Vscale);
  ddVvt.div(Vscale*Vscale);
  scalar s = Vvt.entry[1];//--- This is the value of "normalized" s(t)
  if(s>1){
    Output::error(
                  "Spline",
                  "scriptedMotion(...)",
                  "Arclength function must be normalized!"
                  );
    return;
  }
  scalar dtdvt = dVvt.entry[0];
  scalar d2tdvt2 = ddVvt.entry[0];
  //--- Scale so we get derivatives of the unnormalized arclength
  scalar dsdvt = dVvt.entry[1]*getTotalArcLength();
  scalar d2sdvt2 = ddVvt.entry[1]*getTotalArcLength();

  //--- And now we can proceed to handle the arc length
  //--- reparameterization.

  int i = getSegmentIndexAtArcLength(s);
  scalar Sscale = this->getKnotVector()[i+4]-this->getKnotVector()[i+3];
  scalar sbefore = getNormalizedArcLengthAtSegment(i);
  //--- Now i point to the segement on the
	//--- spline with the property
	//---
	//--- arcLength[i-1] < s < archLength[i+1]
	//---
	//--- In other words the point we seek must lie
	//--- on the i'th segement of the spline.
	CubicCurve * segment = getSegment(i);
  scalar slocal = getTotalArcLength()*(s-sbefore);
  scalar ulocal = segment->getParameterValueAtArcLength(slocal);
  VectorN dCdu(this->getDimension());
  VectorN d2Cdu2(this->getDimension());
  //--- Oh now Im getting derivatives with respect to local parameter value
  segment->getDerivativesAtParameterValue(ulocal,p,dCdu,d2Cdu2);
  //--- Convert derivatives so they match the global parameter
  dCdu.div(Sscale);
  d2Cdu2.div(Sscale*Sscale);

  //--- Higher Order Derivatives:
  //---
  //--- a = d2Cdu2 * (duds)^2 * (dsdt)^2 + dCdu * d2uds2 * (dsdt)^2 + dCdu * duds * d2sdt2
  //---
  //--- Where
  //---
  //---   duds   =  1 / sqrt(dCdu * dCdu)
  //---   d2uds2 = - (dCdu * d2Cdu2)/(dCdu * dCdu)^2
  //---   dsdt   =  dsdv/dtdv
  //---   d2sdt2 =  d2sdv2/dtdv^2 - dsdv*d2tdv2/dtdv^3

  scalar dot = dCdu.dot(dCdu);
  if(dot==0){
    Output::message(
                    "Spline",
                    "scriptedMotion(...)",
                    "dCdu was zero"
                    );
    interpolateMotion(t,tmin,tmax,V,p,v,a);
    return;
  }
  scalar duds = 1.0f / (scalar)sqrt(dot);
  scalar d2uds2 = - dCdu.dot(d2Cdu2)/(dot*dot);

  if(dtdvt==0){
    Output::warning(
                    "Spline",
                    "scriptedMotion(...)",
                    "dtdv was zero"
                    );
    interpolateMotion(t,tmin,tmax,V,p,v,a);
    return;
  }
  scalar dsdt = dsdvt / dtdvt;
  scalar d2sdt2 = (d2sdvt2/(dtdvt*dtdvt))-((dsdvt*d2tdvt2)/(dtdvt*dtdvt*dtdvt));

  a.set(d2Cdu2);
  a.mul(duds*duds*dsdt*dsdt);

  v.set(dCdu);
  v.mul(d2uds2*dsdt*dsdt);
  a.add(v);

  v.set(dCdu);
  v.mul(duds*d2sdt2);
  a.add(v);

  //--- And final the velocity
  //---
  //---   v = dCdu * duds * dsdt
  v.set(dCdu);
  v.mul(duds*dsdt);
};

/**
 * Interpolate Scripted Motion.
 * Auxiliary method used by the method scriptedMotion. This
 * method is invoked whenever the scripted motion becomes
 * ill-conditioned.
 *
 * These bad cases only occur at a single point in time, so
 * the "value" is interpolated from nearby points in time.
 *
 * @param t      The point in time, where computations failed.
 * @param tmin   The minimum point in time of the motion.
 * @param tmax   The maximum point in time of the motion.
 * @param V      The velocity spline, used with the (this)
 *               space spline.
 * @param p      Upon return this vector should hold the value
 *               of the interpolated position.
 * @param v      Upon return this vector should hold the value
 *               of the interpolated velocity.
 * @param a      Upon return this vector should hold the value
 *               of the interpolated acceleration.
 *
 */
void Spline::interpolateMotion(const scalar & t,scalar & tmin,scalar & tmax,NUBSpline & V,VectorN & p,VectorN & v,VectorN & a){
  //Output::message(
  //                "Spline",
  //                "interpolateMotion",
  //                "Trying to resolve bad conditions by interpolation"
  //                );

  int steps = 5;
  scalar stepsize = 0.05f;//--- Corresponds approximately to 20 fps!!!
  if(stepsize>(tmax-tmin)){
    stepsize = (tmax-tmin)/(steps*10);
  }
  scalar t0 = t - (scalar)floor(steps/2.0f)*stepsize;
  if(t==tmin){
    t0 = tmin + stepsize;
  }
  if(t==tmax){
    t0 = tmax - steps*stepsize;
  }
  scalar * ta = new scalar[steps];
  scalar * vx = new scalar[steps];
  scalar * vy = new scalar[steps];
  scalar * vz = new scalar[steps];
  scalar * ax = new scalar[steps];
  scalar * ay = new scalar[steps];
  scalar * az = new scalar[steps];
  VectorN tmp(3);
  scalar tn = t0;
  for(int n=0;n<steps;n++){
    ta[n] = tn;
    tn += stepsize;
    scriptedMotion(ta[n],V,tmp,v,a);
    vx[n] = v.entry[0]; vy[n] = v.entry[1]; vz[n] = v.entry[2];
    ax[n] = a.entry[0]; ay[n] = a.entry[1]; az[n] = a.entry[2];
  }
  PolynomialInterpolator interpolator;
  interpolator.setTableau(ta,vx,steps);
  v.entry[0] = interpolator.getValue(t);
  interpolator.setTableau(ta,vy,steps);
  v.entry[1] = interpolator.getValue(t);
  interpolator.setTableau(ta,vz,steps);
  v.entry[2] = interpolator.getValue(t);

  interpolator.setTableau(ta,ax,steps);
  a.entry[0] = interpolator.getValue(t);
  interpolator.setTableau(ta,ay,steps);
  a.entry[1] = interpolator.getValue(t);
  interpolator.setTableau(ta,az,steps);
  a.entry[2] = interpolator.getValue(t);

  delete [] ta;
  delete [] vx;
  delete [] vy;
  delete [] vz;
  delete [] ax;
  delete [] ay;
  delete [] az;

  //Output::message("Spline","interpolatedMotion(...)","interpolated vel = "+v+" length="+v.length());
  //Output::message("Spline","interpolatedMotion(...)","interpolated acc = "+a+" length="+a.length());
};

