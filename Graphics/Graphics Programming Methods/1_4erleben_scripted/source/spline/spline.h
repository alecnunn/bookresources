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
#ifndef __SPLINE_INCLUDED__
#define __SPLINE_INCLUDED__
#include <numerical/scalar.h>
#include <numerical/vectorN.h>
#include <spline/curve.h>

/**
 * Forward Declaration
 */
class NUBSpline;

/**
 * Spline Class.
 * This class is a general purpose base class for splines.
 */
class Spline
{
public:

  Spline();
  virtual ~Spline();

public:

  void setDataPoints(int k,VectorN * dataPoints,const int & length,bool cyclic);
	void setDataPoints(int k,VectorN * dataPoints,const int & length,scalar * knotValues,bool cyclic);

  void getPointAtArcLength(const scalar & s,VectorN & p);
	scalar getTotalArcLength(void);
  bool isCyclic();	
  int getSegmentIndexAtArcLength(const scalar & s);
  int getSegmentIndexAtCoordinate(const int & j,const scalar & value);

public:

  /**
   * Retrieve the dimension of the spline.
   *
   * @return   The number of dimensions.
   */
  virtual int getDimension()=0;

  /**
   * Retrieve the order of the spline.
   *
   * @return   The order of the spline.
   */
  virtual int getOrder()=0;

  /**
	 * Get Segment Count.
	 *
	 * @return  The number of cubic curves making
	 *          up this spline. Each such cubic curve
	 *          is referred to as a segement of the spline.
	 */
  virtual int getSegmentCount()=0;
	
	/**
	 * Get Segment.
	 * This method retrievs a cubic curve segement
	 * of the spline,
	 *
	 * @param i    The index of the curve/segment, 0 <= i < C, where
	 *             C is the total segment count.
	 *
	 * @return     A reference to a cubic curve representing the
	 *             specified curve segement. If the segment does
	 *             not exist or an error has occured then the
	 *             return value is null.
	 */
  virtual CubicCurve * getSegment(int i)=0;

  /**
   * Get Point On Spline.
   *
   * This is infact the function C(u).
   *
   * @param u  Global spline parameter. This means
   *           that it runs from 0 to |X|.
   *
   * @param p  A reference to a vector, which upon return will
   *           contain the computed point on the spline.
   *
   * @return   A reference to a vector containing the coordinates
   *           of the point on the spline at the specified global
   *           normalized parameter value.
   */
  virtual void getPointOnSpline(const scalar & u,VectorN & p)=0;

  
  /**
   * Retrive ControlPoints.
   * This method should retrieve a reference to an array containing
   * all the control points of the spline.
   *
   * The received control points should be treated as read-only, that
   * is don't expect the curve segments and/or the data points will reflect any
   * changes you make to the received control points.
   *
   * For the exact consequences of modifing control points "outside" the
   * spline class you should refer to the specific details of the implemention
   * of the method in the spline class you use.
   *
   * @return    A reference to an array of Vectors each vector represents
   *            a corresponding control point of the spline.
   */
  virtual VectorN * getControlPoints()=0;

  /**
   * Retrive DataPoints.
   * This method should retrieve a reference to an array containing
   * all the data points of the spline.
   *
   * The received data points should be treated as read-only, that
   * is don't expect the curve segments and/or the control points will
   * reflect any changes you make to the received data points.
   *
   * For the exact consequences of modifing data points "outside" the
   * spline class you should refer to the specific details of the implemention
   * of the method in the spline class you use.
   *
   * @return    A reference to an array of Vectors each vector represents
   *            a corresponding data point of the spline.
   */
  virtual VectorN * getDataPoints()=0;
  
  /**
   * Retrive Knot Vector.
   * This method should retrieve a reference to an array containing
   * all the knots of the spline.
   *
   * Each knot corresponds to a parametric value of a data point. The
   * values in the knot vector should be an increasing function.
   *
   * The received knot values should be treated as read-only, that
   * is don't expect the curve segments, the data points and/or the
   * control points will reflect any changes you make to the knot
   * values.
   *
   * For the exact consequence of modifing the knotvalues "outside" the
   * spline class you should refer to the specific details of the implemention
   * of the method in the spline class you use.
   *
   * @return    A reference to an array of knotvalues of the spline.
   */
  virtual scalar * getKnotVector()=0;
  
protected:

  /**
	 * Specialized Assignment Routine.
	 * This method should be responsible for creating and initializing the
   * spline, but not necessarily the curve segments, these should
   * be constructed by invokation of the computeSegments method.
	 *
	 * Developers Note: Use the method isCyclic to test if the keypositions should be
	 * used to create a cyclic spline.
	 *
   * @param k            The order of the spline.
	 * @param dataPoints   An array of datapoints, which the
	 *                     spline must run through. In other
	 *                     terms the keypositions of the spline.
	 */
  virtual void doSetDataPoints(const int & k,VectorN * dataPoints,const int & count,bool cyclic)=0;
	
  /**
	 * Specialized Assignment Routine.
	 * This method should be responsible for creating and initializing the
   * spline, but not necessarily the curve segments, these should
   * be constructed by invokation of the computeSegments method.
	 *
	 * Developers Note: Use the method isCyclic to test if the keypositions should be
	 * used to create a cyclic spline.
	 *
   * @param k            The order of the spline.
	 * @param dataPoints   An array of datapoints, which the
	 *                     spline must run through. In other
	 *                     terms the keypositions of the spline.
   * @param knotValues   An array of knot values, which should be used
   *                     to create the spline. The array should contain:
   *                     dataPoints.length+1+k elements.
	 */
	virtual void doSetDataPoints(const int & k,VectorN * dataPoints,const int & count,scalar * knotValues,bool cyclic)=0;

  /**
   * Specialized Curve Segment Computation Routine.
   * This method should implement how the curve segments of
   * the specific spline class (you are implementing) should
   * be computed.
   */
  virtual void doComputeSegments(void)=0;

protected:

	scalar getNormalizedArcLengthAtSegment(const int & i);
  void initArcLengthTable();

public:

  void scriptedMotion(const scalar & t,NUBSpline & V,VectorN & p,VectorN & v,VectorN & a);

protected:

  void interpolateMotion(
                          const scalar & t,
                          scalar & tmin,scalar & tmax,
                          NUBSpline & V,
                          VectorN & p,
                          VectorN & v,
                          VectorN & a
                          );
	
protected:

	/**
	 * The total arc length of the spline.
	 */
	scalar totalArcLength;

	/**
	 * A table of accumulated normalized
	 * arclengths of the curve segments.
	 */
	scalar * arcLengths;

	/**
	 * Cyclic Indicator.
	 * This boolean flag tells whatever the spline is cyclic
	 * (periodic) or not.
	 */
	bool cyclic;


};/* End of class Spline */

#endif //__SPLINE_INCLUDED__

