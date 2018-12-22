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
#include <spline/curve.h>
#include <utils/output.h>
#include<cmath>
using namespace std;

/**
 * Default Constructor.
 */
CubicCurve::CubicCurve():M(4,4){
	totalArcLength = 0.0;
};

/**
 * Deconstructor.
 */
CubicCurve::~CubicCurve(){
};

/**
 * Set Geometry.
 * This method should be used to define the geometry of the
 * curve. That is setting the control points of the curve. It
 * is these points that control the shape of the curve. This
 * is why it is called the "geometry".
 *
 * Notice that since its a cubic curve we always need at least
 * four control points in order to uniquely define the curve.
 *
 * It is assumed that the base matrix have been set prior (at
 * some time in the past) to the invokation of this method.
 *
 * @param g0    A reference to the first control point.
 * @param g1    A reference to the second control point.
 * @param g2    A reference to the third control point.
 * @param g3    A reference to the fourth control point.
 */
void CubicCurve::setGeometry(const VectorN & g0,const VectorN & g1,const VectorN & g2,const VectorN & g3){
	int n = g0.getDimension();

  //--- Store geometry vectors internally, so we can use them later on.
	this->g0.resize(n);this->g0.set(g0);
	this->g1.resize(n);this->g1.set(g1);
	this->g2.resize(n);this->g2.set(g2);
	this->g3.resize(n);this->g3.set(g3);

	//--- Allocate room for coefficient and working storage etc.
	a.resize(n);
	b.resize(n);
	c.resize(n);
	d.resize(n);
	tmp.resize(n);

	//--- Now compute coefficient matrix.
	a.set(g0); a.mul(M.entry[0][0]);
	tmp.set(g1); tmp.mul(M.entry[0][1]);
	a.add(tmp);
	tmp.set(g2); tmp.mul(M.entry[0][2]);
	a.add(tmp);
	tmp.set(g3); tmp.mul(M.entry[0][3]);
	a.add(tmp);

	b.set(g0); b.mul(M.entry[1][0]);
	tmp.set(g1); tmp.mul(M.entry[1][1]);
	b.add(tmp);
	tmp.set(g2); tmp.mul(M.entry[1][2]);
	b.add(tmp);
	tmp.set(g3); tmp.mul(M.entry[1][3]);
	b.add(tmp);

	c.set(g0); c.mul(M.entry[2][0]);
	tmp.set(g1); tmp.mul(M.entry[2][1]);
	c.add(tmp);
	tmp.set(g2); tmp.mul(M.entry[2][2]);
	c.add(tmp);
	tmp.set(g3); tmp.mul(M.entry[2][3]);
	c.add(tmp);

	d.set(g0); d.mul(M.entry[3][0]);
	tmp.set(g1); tmp.mul(M.entry[3][1]);
	d.add(tmp);
	tmp.set(g2); tmp.mul(M.entry[3][2]);
	d.add(tmp);
	tmp.set(g3); tmp.mul(M.entry[3][3]);
	d.add(tmp);
	//--- Finally compute total arc length of the curve.
	totalArcLength = getArcLength(0,1);
};

/**
 * Set Geometry Vector.
 * It is assumed that the base matrix have been set prior (at
 * some time in the past) to the invokation of this method.
 *
 * @param i   The index of the geometry vector that
 *            should be set. Either 0,1,2 or 3.
 * @param g   A reference to a vector keeping the new
 *            values of the geometry vector that should
 *            be set.
 */
void CubicCurve::setGeometry(const int & i,const VectorN & g){
	switch(i){
		case 0:
			tmp.set(g0); tmp.mul(M.entry[0][0]);
			a.sub(tmp);
			tmp.set(g0); tmp.mul(M.entry[1][0]);
			b.sub(tmp);
			tmp.set(g0); tmp.mul(M.entry[2][0]);
			c.sub(tmp);
			tmp.set(g0); tmp.mul(M.entry[3][0]);
			d.sub(tmp);
			g0.set(g);
			tmp.set(g0); tmp.mul(M.entry[0][0]);
			a.add(tmp);
			tmp.set(g0); tmp.mul(M.entry[1][0]);
			b.add(tmp);
			tmp.set(g0); tmp.mul(M.entry[2][0]);
			c.add(tmp);
			tmp.set(g0); tmp.mul(M.entry[3][0]);
			d.add(tmp);
			break;
		case 1:
			tmp.set(g1); tmp.mul(M.entry[0][1]);
			a.sub(tmp);
			tmp.set(g1); tmp.mul(M.entry[1][1]);
			b.sub(tmp);
			tmp.set(g1); tmp.mul(M.entry[2][1]);
			c.sub(tmp);
			tmp.set(g1); tmp.mul(M.entry[3][1]);
			d.sub(tmp);
			g1.set(g);
			tmp.set(g1); tmp.mul(M.entry[0][1]);
			a.add(tmp);
			tmp.set(g1); tmp.mul(M.entry[1][1]);
			b.add(tmp);
			tmp.set(g1); tmp.mul(M.entry[2][1]);
			c.add(tmp);
			tmp.set(g1); tmp.mul(M.entry[3][1]);
			d.add(tmp);
			break;
		case 2:
			tmp.set(g2); tmp.mul(M.entry[0][2]);
			a.sub(tmp);
			tmp.set(g2); tmp.mul(M.entry[1][2]);
			b.sub(tmp);
			tmp.set(g2); tmp.mul(M.entry[2][2]);
			c.sub(tmp);
			tmp.set(g2); tmp.mul(M.entry[3][2]);
			d.sub(tmp);
			g2.set(g);
			tmp.set(g2); tmp.mul(M.entry[0][2]);
			a.add(tmp);
			tmp.set(g2); tmp.mul(M.entry[1][2]);
			b.add(tmp);
			tmp.set(g2); tmp.mul(M.entry[2][2]);
			c.add(tmp);
			tmp.set(g2); tmp.mul(M.entry[3][2]);
			d.add(tmp);
			break;
		case 3:
			tmp.set(g3); tmp.mul(M.entry[0][3]);
			a.sub(tmp);
			tmp.set(g3); tmp.mul(M.entry[1][3]);
			b.sub(tmp);
			tmp.set(g3); tmp.mul(M.entry[2][3]);
			c.sub(tmp);
			tmp.set(g3); tmp.mul(M.entry[3][3]);
			d.sub(tmp);
			g3.set(g);
			tmp.set(g3); tmp.mul(M.entry[0][3]);
			a.add(tmp);
			tmp.set(g3); tmp.mul(M.entry[1][3]);
			b.add(tmp);
			tmp.set(g3); tmp.mul(M.entry[2][3]);
			c.add(tmp);
			tmp.set(g3); tmp.mul(M.entry[3][3]);
			d.add(tmp);
			break;
		default:
      Output::error("CubicCurve","SetGeometry(int,VectorN)","Invalid geometry vector index?");
			break;
	}/* End switch(i) */

	//--- Finally update the total arc length of the curve.
	totalArcLength = getArcLength(0,1);
};

/**
 * Get Geometry
 * This method was implemented in this way, so the geometry matrix
 * of G cannot be changed elsewhere without invokig the method
 * setGeometry. This way we are always sure the the coefficient
 * matrix are updated proberly.
 *
 * @param i   The index of the geometry vector that should
 *            be retireved.
 * @param g   Upon return this vector will hold the values of
 *            the entries of the specified geometry vector. That
 *            is its a copy of the specified geoemtry vector.
 */
void CubicCurve::getGeometry(const int & i,VectorN & g){
	switch(i){
		case 0:
			g.set(g0);
			break;
		case 1:
			g.set(g1);
			break;
		case 2:
			g.set(g2);
			break;
		case 3:
			g.set(g3);
			break;
		default:
      Output::error("CubicCurve","getGeometry(int,VectorN)","Invalid geometry vector index?");
			break;
	}/* End switch(i) */
};

/**
 * Get Coefficient Column.
 *
 * Recall that we have defined
 *
 *   Q(u) = U M G = U C
 *
 * Where U is the row-power vector.
 *
 * @param i   The index of the coefficient rwo to
 *            be retrieved, any value from 0 to 3.
 *
 * @param c   Upon return this vector will hold the values of
 *            the entries of the specified coefficient row.
 */
void CubicCurve::getCoefficientRow(const int & i,VectorN & C){
	switch(i){
		case 0:
			C.set(a);
			break;
		case 1:
			C.set(b);
			break;
		case 2:
			C.set(c);
			break;
		case 3:
			C.set(d);
			break;
		default:
      Output::error("CubicCurve","getCoefficientRow(int,VectorN)","Invalid coefficient row index?");
			break;
	}/* End switch(i) */
};

/**
 * Set Base Matrix.
 * Note the base should be set prior to setting the geometry. If
 * it is done the other way around then it would appear as though
 * the base matrix has not been set.
 *
 * @param m    A reference to a 4x4 basis matrix.
 */
void CubicCurve::setBase(const MatrixMxN & base){
  if(base.getColumnDimension()!=4 || base.getRowDimension()!=4){
    Output::error("CubicCurve","setBase","Invalid dimension of base matrix, must be 4x4");
	}
	M.set(base);
};

/**
 * Get Base Matrix.
 *
 * Note if the base have been altered outside you must invoke
 * one of the set geometry methods in order to see the changes
 * on the curve.
 *
 * @param base     A reference to a matrix, which upon return
 *                 constains the values of the base matrix.
 */
void CubicCurve::getBase(MatrixMxN & base){
	base.set(M);
};

/**
 * Get Curve Point.
 * This method computes the curve point at the specified
 * parameter value. It is implicitly assumed that u is in
 * the interval 0 to 1.
 *
 * Note this method works for invalid values of u, that
 * is when 0 < u  or u > 1.
 *
 * @param u   The curve parameter value.
 * @param p   Upon return this vector holds the value of
 *            the computed curve point.
 */
void CubicCurve::getPointAtParameterValue(const scalar & u,VectorN & p){
  //--- We apply Horner's rule for factoring polynomials
	p.set(a); p.mul(u);	p.add(b);
	p.mul(u); p.add(c);
	p.mul(u); p.add(d);
};

/**
 * Compute Higher Order Derivatives At Parameter Value.
 *
 * This is infact the functions Q(u), Q'(u) and Q''(u).
 *
 * @param   u   The parameter value.
 * @param   p   Upon return this reference contains the
 *              point on the curve at the specified parameter
 *              value.
 * @param  dp   Upon return this reference contains the
 *              first order derivative on the curve at the
 *              specified parameter value.
 * @param ddp   Upon return this reference contains the
 *              second order derivative on the curve at the
 *              specified parameter value.
 */
void CubicCurve::getDerivativesAtParameterValue(const scalar & u,VectorN & p,VectorN & dp,VectorN & ddp){
  //--- Safety check!
  if(u<0||u>1){
    Output::warning("CubicCurve","getDerivativesAtParameterValue(...)","Parameter value u was outside bounds");
  }

  //--- We apply Horner's rule for factoring polynomials
  p.set(a); p.mul(u);
  p.add(b);	p.mul(u);
  p.add(c);	p.mul(u);
  p.add(d);

  //--- Urgh! Damn this could proberly be implemented in
  //--- a more efficient way???
  tmp.set(b);
  tmp.mul(2);

  dp.set(a); dp.mul(3.0f*u);
  dp.add(tmp); dp.mul(u);
  dp.add(c);

  ddp.set(a); ddp.mul(6.0f*u);
  ddp.add(tmp);
};

/**
 * Get Total Arc Length.
 * Note this method corresponds to getArcLength(0,1)
 *
 * @return   The arc length of the entire curve.
 */
scalar CubicCurve::getTotalArcLength(){
	return totalArcLength;
};

/**
 * Get arc length of curve.
 * This method computes the arc length of a subpart of the
 * curve. The subpart is specified by to paramter values
 * umin and umax. Where
 *
 *  0 <= ustart < uend <= 1
 *
 * The method does not test if this conditions holds, but
 * it implicitly assumes that it does.
 *
 * @param ustart The minimum parameter value, this designates
 *               the starting point on the curve from where the
 *               arc length is measured.
 * @param uend   The maximum parameter value, this designates
 *               the ending point on the curve from where the
 *               arc length is measured.
 *
 * @return       The arc length of the part of the curve that
 *               corresponds to the internal umin to umax.
 */
scalar CubicCurve::getArcLength(const scalar & ustart,const scalar & uend){
  //--- We are going to compute the arc length by using
	//--- the extended Simpson's rule.
	//---
	//--- The iteration count, must be an even value.
  int n = 128;

  //--- The stepsize
	scalar h = (uend-ustart)/n;
	
  //--- Initialization of sum and "running" parametervalue
	scalar sum = 0;
	scalar u = ustart+h;

	for(int i=2;i<=n;i++){
		if((i&1)==1){	//--- i was uneven
      sum += 4.0*getArcIntegrand(u);
    }else{			  //--- i was even
			sum += 2.0*getArcIntegrand(u);
		}
		u += h;
	}
	return (h*(getArcIntegrand(ustart)+sum+getArcIntegrand(uend)))/3.0;
};

/**
 * Get Arc Integrand.
 * It can easily be derived that the arc length of a cubic
 * curve have the following form
 *
 *   S(u) = int_u0^u1 (Au^4 + Bu^3 +Cu^4 +Du + E)^(½)du
 *
 * Where (in 3-diemensional space)
 *
 *   A = 9 (ax ax + ay ay + az az)
 *   B = 12(ax bx + ay by + az bz)
 *   C = 6 (ax cx + ay cy + az cz) + 4(bx bx + by by + bz bz)
 *   D = 4 (bx cx + by cy + bz cz)
 *   E =   (cx cx + cy cy + cz cz)
 *
 * This function will not integrate. This method is
 * intended to be used in a numerical integration
 * of the function. This method will evaluate the
 * integrand at the specified parameter value.
 * That is it computes
 *
 *   s(u) = (Au^4 + Bu^3 +Cu^4 +Du + E)^(½)
 *
 * For some specified value of u.
 *
 * The total computational cost of this method
 * is (in 3 dimensional space):
 *
 *   22 multiplications, 16 addistions and 1 square root.
 *
 * Deriving the integrand function in the n-dimensional
 * case is done as follows:
 *
 *   S(u) = int_u0^u1 s(u) du
 *
 * Where
 *                 n-1
 *   s(u) =sqrt(   sum ( (d/du( Q(u)[i] ))^2   ) )
 *                 i=0
 *
 * Let us try to resolve this rather complex looking function from
 * the inside out.
 *
 *   d/du( Q(u)) =  [3u^2, 2u, 1, 0] C
 *
 * Looking at the i'th coordiante we get
 *
 *   d/du( Q(u))[i] =  3 C0i u^2  + 2 C1i u  +  C2i
 *
 * Squaring the above equation gives us the following equation:
 *
 *   (d/du( Q(u)[i] ))^2 = 9 C0i^2 u^4 + 12 C0i C1i u^3 + (4C1i^2 + 3 C0i C2i)u^2 + 2 C1i C2i u + C2i^2
 *
 * Writting this in matrix from gives us:
 *
 *                                              |      9 C0i^2       |
 *                                              |     12 C0i C1i     |
 *   (d/du( Q(u)[i] ))^2 = [u^4  u^3  u^2  u  1]|(4C1i^2 + 3 C0i C2i)|
 *                                              |    2 C1i C2i       |
 *                                              |      C2i^2         |
 *
 * Now let us take the summation into account
 *
 *                                                            |      9 C0i^2       |
 * n-1                                                   n-1  |     12 C0i C1i     |
 * sum ( (d/du( Q(u)[i] ))^2   ) = [u^4  u^3  u^2  u  1] sum (|(4C1i^2 + 3 C0i C2i)|)
 * i=0                                                   i=0  |    2 C1i C2i       |
 *                                                            |      C2i^2         |
 *
 * Let us now introduce the following terms
 *
 *          n-1
 *   A = 9  sum (C0i^2)
 *          i=0
 *
 *          n-1
 *   B = 12 sum (C0i C1i)
 *          i=0
 *
 *          n-1             n-1
 *   C = 4  sum (C1i^2) + 3 sum (C0i+C2i)
 *          i=0             i=0
 *
 *          n-1
 *   D = 2  sum (C1i C2i)
 *          i=0
 *
 *          n-1
 *   E =    sum (C2i^2)
 *          i=0
 *
 * With these terms we can easily see that we now can
 * write S(u) in the following form:
 *
 *   s(u) = sqrt( A*u^4 + B*u^3 + C*u^2 + D*u + E )
 *
 * By spplying Horner's rule for factoring polynomials we
 * can rewrite the equation into a more computational
 * friendly form:
 *
 *   s(u) = sqrt(   (((A*u + B)*u + C)*u + D)*u + E   )
 *
 * The computational cost is (disregarding loop variable
 * and test): 6n+6 multiplications, 6(n-1)+1 additions,
 * 1 square root. Besides that theres is an extra "penalty"
 * in the looping, the penalty is n-2 additions and n-1
 * boolean tests. However looking at the big picture the
 * method runs in O(n) time.
 *
 * @param u    The parameter value where we want to
 *             evaluate the arc integrand.
 * @return     The value of the arc integrand at the
 *             specified parameter value.
 */
scalar CubicCurve::getArcIntegrand(const scalar & u){
	int n = a.getDimension();
	scalar A  = a.entry[0] * a.entry[0];
	scalar B  = a.entry[0] * b.entry[0];
	scalar C1 = b.entry[0] * b.entry[0];
	scalar C2 = a.entry[0] * c.entry[0];
	scalar D  = b.entry[0] * c.entry[0];
	scalar E  = c.entry[0] * c.entry[0];
	for(int i=1;i<n;i++){
		A  += a.entry[i] * a.entry[i];
		B  += a.entry[i] * b.entry[i];
		C1 += b.entry[i] * b.entry[i];
		C2 += a.entry[i] * c.entry[i];
		D  += b.entry[i] * c.entry[i];
		E  += c.entry[i] * c.entry[i];
	}
	A *= 9.0f;
	B *= 12.0f;
	scalar C = 4.0f * C1 + 3.0f * C2;
	D *= 2.0f;
	//--- We apply Horner's rule for factoring polynomials
	scalar f2 = (((((A*u + B)*u + C)*u + D)*u) + E);
	return (scalar)sqrt(f2);
};

/**
 * Get Coordinate at Coordinate.
 * This method is highly special, it is meant to be used
 * when one defines velocity curves. A velocity curve
 * is a two dimensional curve (or spline) defined as
 * follows:
 *
 * V(u) -> (t,s)
 *
 * The first coordinate specifies an unique point in time,
 * and the second coordinate the "position" at that instant
 * of time.
 *
 * In an animation one usually knows a t-value and want to get
 * the corresponding s-value. It is exactly for this purpose
 * this method have been implemented.
 *
 * The problem is that given a t-value, we do not know the
 * corresponding u-value, and we can therfor not find the
 * s-value we want.
 *
 * The method solves the problem by turning it into a root-finding
 * problem, just as we did with the getPointAtArcLength method in
 * the Spline class.
 *
 * Not all curves are valid velocity curves, only does curves where
 * the t-value is a strictly monotonically increasing function of
 * the u-parameter.
 *
 * If the curve represents a velocity curve as described above then
 * this method would typically be invoked as follows:
 *
 * s =  getCoordinateAtCoordinate(1,0,t);
 *
 * @param i      The index of the coordinate we are searching for.
 * @param j      The index of the coordinate at whos value we are
 *               searching at.
 * @param value  The value of the j'th coordinate where we are
 *               searching for the i'th coordinate value.
 * @return       The value of the i'th coordinate at the
 *               corresponding value of the j'th coordinate.
 */
scalar CubicCurve::getCoordinateAtCoordinate(const int & i,const int & j,const scalar & value){
  scalar u = getParameterValueAtCoordinate(j,value);
  return getCoordinateAtParameterValue(i,u);
};

/**
 * Get Parameter Value At the j'th Coordinate.
 *
 * This is the function U(Q_j).
 *
 * @param j        The index of the coordinate, whos value we are
 *                 seeking a parameter value for.
 * @param value    The value of the j'th coordinate.
 *
 * @return         The parameter value u at the point of the curve,
 *                 where Q_j(u) equals the specified value.
 */
scalar CubicCurve::getParameterValueAtCoordinate(const int & j,const scalar & value){
  //--- Just to make sure we are cable of hitting the end point...
  scalar minValue = d.entry[j];
  if(value==minValue)
    return 0;
  scalar maxValue =  a.entry[j]+b.entry[j]+c.entry[j]+d.entry[j];
  if(value==maxValue)
    return 1;
  //--- Now we take care of out of bounds value...
  if(value<minValue){
    Output::warning("CubicCurve","getParameterValueAtCoordinate(...","Value was to small?");
    return 0;
  }
  if(value>maxValue){
    Output::warning("CubicCurve","getParameterValueAtCoordinate()","Value was to big?");
    return 1;
  }
  //--- Now we do a root search
  scalar umin = 0,umax = 1,umid;
	scalar t = value;
	scalar tmid = 10e30f;
	scalar tol = precision;
  int iteration = 0;
	//--- we are using a simple bisection method to search for
	//--- a t-value, which is close to the specified value.
	do{
		umid = (umin+umax)/2.0f;
		tmid = ((((a.entry[j]*umid)+b.entry[j])*umid+c.entry[j])*umid)+d.entry[j];
		if(t < tmid+tol){
			umax = umid;
		}else{
			umin = umid;
		}

    iteration++;

    if(iteration>=500){
      Output::warning(
              "CubicCurve",
              "getParameterValueAtCoordinate(...)",
              "To many iterations, no chance of improvement, breaking loop"
      );
      break;
    }

  }while((t>(tmid+tol))||(t<(tmid-tol)));
	//--- Haivng found the specified t-value, we have also
	//--- found the corresponding parameter value, u.
	return umid;
};

/**
 * Get i'th Coordinate at Parameter Value.
 *
 * This is the function Q_i(u).
 *
 * @param i      The index of the coordinate whos value we are seeking.
 * @param u      The parameter value (we assume that 0<= u <= 1) at
 *               which we want to know the i'th coordinate value.
 *
 * @return       The i'th coordinate value at the parameter
 *               value u, that is B_i(u).
 */
scalar CubicCurve::getCoordinateAtParameterValue(const int & i,const scalar & u){
	return ((((a.entry[i]*u)+b.entry[i])*u+c.entry[i])*u)+d.entry[i];
};

/**
 * Get Parameter Value At Arc Length.
 * This method determines the parameter value off the curve
 * segment at the corresponding arc length value.
 *
 * This is in fact the function U(s).
 *
 * @param s      The arc length value.
 * @return       The parameter u-value.
 */
scalar CubicCurve::getParameterValueAtArcLength(const scalar & s){
  //--- First we take care of the endpoints...
  if(s==0)
    return 0;
  if(s==getTotalArcLength())
    return 1;
  //--- Now we take care of out of bounds arc lengths...
  if(s<0){
    Output::warning("CubicCurve","getParameterValueAtArcLength()","Arclength value was to small?");
    return 0;
  }
  if(s>getTotalArcLength()){
    Output::warning("CubicCurve","getParameterValueAtArcLength()","Arclength value was to big?");
    return 1;
  }
  //--- The segment is parametrized in u
  //--- (going from 0 to 1) and not the
	//--- arc length of the spline (or
	//--- the segement itself).
	//---
	//--- We therefor need to reparametrize the
	//--- curve segment. However since the curve
	//--- segment function isn't invertible we have
	//--- to do it numerically.
	//---
	//--- We turn the problem into a root finding
	//--- seeking problem.
  scalar stmp = s;
	scalar umin = 0,umax = 1,umid;
	scalar smid = 10e30f;
	scalar tol = precision;
  int iteration = 0;

	do{
		umid = (umin+umax)/2.0f;
    if(umid==umin || umid==umax){
      //--- The getArcLength method numerical integrates
      //--- the curve segment in order to find the arc
      //--- length. However this will undoubtly introduce
      //--- numerical inaccuracies.
      //---
      //--- This test is a small real-world-hack to avoid
      //--- that things turn ugly when we are close to the
      //--- "boundaries" of the segment.
      //---
      return umid;
    }

    smid = this->getArcLength(umin,umid);
		if(stmp < smid+tol){
			umax = umid;
		}else{
			umin = umid;
			stmp -= smid;
		}


    iteration++;
    if(iteration>=500){
      Output::warning(
              "CubicCurve",
              "getParameterValueAtArcLength(...)",
              "To many iterations, no chance of improvement, breaking loop"
      );
      break;
    }

	}while((stmp>(smid+tol))||(stmp<(smid-tol)));

  //--- Now we have found an u value, which
	//--- produces an arclength, which is within
	//--- numerical tolerance of the arc length
	//--- we originally specified.
  return umid;
};

