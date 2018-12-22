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
#include <spline/nubs.h>
#include <utils/output.h>
#include <algorithm>
using namespace std;

#ifndef min
#define min(a,b)  ((a<b) ? a : b)
#endif
/**
 * Default Constructor.
 */
NUBSpline::NUBSpline(){
	count = 0;
	segments = 0;
	P = 0;
	X = 0;
	T = 0;
  K = 4;
};

/**
 * Deconstructor.
 */
NUBSpline::~NUBSpline(){
  if(segments)
    delete [] segments;
  if(P)
    delete [] P;
  if(X)
    delete [] X;
  if(T)
    delete [] T;
};


/**
 * Specialized Assignment Routine.
 * This method is capable of finding a spline of the specified
 * order, which runs through the specified set of data points.
 *
 * The method workd in exactly the same way as doSetDataPoints(k,dataPoint,knotValues)
 * the difference lies in last argument, this method computes the knot-values itself,
 * the other method uses the knot values an user supplies it with.
 *
 * Developers Note: The method isCyclic should be used to test
 * if a cyclic (also called periodic) spline should be created.
 *
 * @param dataPoints   An array of datapoints, which the
 *                     spline must run through. In other
 *                     terms the keypositions of the spline.
 * @param count        The number of data points in the specified array.
 * @param k            The order of the spline.
 */
void NUBSpline::doSetDataPoints(const int & k,VectorN * dataPoints,const int & count,bool cyclic){
  int i;
  this->n = count + 1;
  scalar * knotValues = new scalar[n+1+k];
	//--- Copmute knotvector (using the chord length parametrization heuristic).
	//---
	//--- If we had an uniform B-spline then we would know about the
	//--- global parameter U that
	//---
	//--- U in [0..n-2]
	//---
	//--- We want the non-uniform knot vector to take on values
	//--- in the same domain of values.
  for(i=0;i<k;i++){
    knotValues[i]=0.0f;
  }

	VectorN diff(dataPoints[0].getDimension());
  
  scalar accumulated = 0;
	for(i=0;i<=n-3;i++){
		diff.sub(dataPoints[i+1],dataPoints[i]);
    scalar length = diff.length();
    if(length!=0){
      accumulated += diff.length();
    }else{
      accumulated += 0.5f;
    }
	  knotValues[i+4] = accumulated;
	}
  for(i=n+2;i<n+k+1;i++){
    knotValues[i]=accumulated;
  }

	scalar maxU = (n-2.0f);
  scalar scale =  maxU/accumulated;
	for(i=0;i<=n+k;i++){
		knotValues[i] *= scale;
	}

	//--- Compute control points from the data
  //--- points and the knot vector.
  doSetDataPoints(k,dataPoints,count,knotValues,cyclic);

  delete [] knotValues;
};

/**
 * Specialized Assignment Routine.
 * This method assigns values to this spline, such that it will run
 * through the specified set of data points. See the method
 * doSetDataPoints(k,dataPoints) for further details.
 *
 * @param k            The order of the spline.
 * @param dataPoints   An array of data points (also called break points),
 *                     which the spline must run through. In other
 *                     terms the keypositions of the spline.
 * @param count        The number of data points in the specified array.
 * @param knotValues   A knot vector to be used together with the data
 *                     points.
 */
void NUBSpline::doSetDataPoints(const int & k,VectorN * dataPoints,const int & count,scalar * knotValues,bool cyclic){
  int i;

  //--- Test if spline should be cyclic.
	if(cyclic){
		Output::warning(
                    "NubSpline",
                    "doSetDataPoints(...)",
                    "Cyclic nonuniform B-splines are not currently implemented, sorry!"
                    );
		return;
	}
  if(k!=4){
    Output::warning(
                    "NubSpline",
                    "doSetDataPoints(...)",
                    "Only 4'th order splines are supported, sorry!"
                    );
		return;
  }

	//--- First we are going to allocate space for
	//--- the internal data structures. From theory
	//--- we already know:
	//---
	//---   |P| = n + 1         i in (0,1,2,3,...,n-1,n)
	//---   |X| = n - 1         i in (0,1,2,3,...,n-2)
	//---   |T| = |P| + k
	//---   |T| = n + 1 + k     i in (0,1,2,3,...,n+k-1,n+k)
	//---
	this->cyclic = cyclic;
	this->n = count + 1;
	this->K = k;
	this->dim = dataPoints[0].getDimension();

  //--- Allocate Control Points
  if(this->P)
    delete [] this->P;
	this->P = new VectorN[n+1];	
  for(i=0;i<=n;i++){
		P[i].resize(dim);
	}

  //--- Allocate and Initialize Data Points
  if(this->X)
    delete [] this->X;
  this->X = new VectorN[n-1];
  VectorN ** Ptmp = new VectorN*[n-1];
	for(i=0;i<=n-2;i++){
		X[i].resize(dim);
    X[i].set(dataPoints[i]);
    Ptmp[i] = &P[i+1];
	}

  //--- Allocate and Initialize Knot Vector
  if(this->T)
    delete [] this->T;
	this->T = new scalar[n+1+k];
  for(i=0;i<n+1+k;++i)
    T[i] = knotValues[i];

	//--- Compute control points from the data points
  //--- and the knot vector. We have:
	//---
	//---  M P = X
  //---
	//--- Where M is a tridiagonal matrix, X is the
  //--- column of all data points and P is a column
  //--- of all control points in the range 1,...,n-1.
  //---
  //--- It is implicitly assumed that we have a fourth
  //--- order spline (i.e. k=4).
  //---
	scalar * a = new scalar[n-1];
	scalar * b = new scalar[n-1];
	scalar * c = new scalar[n-1];

	a[0] = 0.0f; b[0] = 1.0f; c[0] = 0.0f;
	a[n-2] = 0.0f; b[n-2] = 1.0f; c[n-2] = 0.0f;

	for(i=1;i<n-2;i++){

		scalar t1 = T[i+1];
		scalar t2 = T[i+2];
		scalar t3 = T[i+3];
		scalar t4 = T[i+4];
		scalar t5 = T[i+5];

		a[i] = (t4-t3)*(t4-t3)/(t4-t1);
		b[i] = (t3-t1)*(t4-t3)/(t4-t1) + (t5-t3)*(t3-t2)/(t5-t2);
		c[i] = (t3-t2)*(t3-t2)/(t5-t2);
		a[i] /= (t4-t2);
		b[i] /= (t4-t2);
		c[i] /= (t4-t2);
	}

  triDiagSolve(a,b,c,Ptmp,X,n-1);

	P[0].set(*Ptmp[0]);
	P[n].set(*Ptmp[n-2]);

  delete [] a;
  delete [] b;
  delete [] c;
  delete [] Ptmp;
};

/**
 * Specialized Curve Segment Computation Routine.
 * This method should implement how the curve segments of
 * the specific spline class (you are implementing) should
 * be computed.
 */
void NUBSpline::doComputeSegments(void){
  int i;

  if(K!=4){
    Output::warning(
                    "NubSpline",
                    "doComputeSegments()",
                    "Only 4'th order splines are supported, sorry!"
                    );
  	return;
  }

	//--- Insert knots, so all knots gets multiplicity k
  NUBSpline * expanded = this;
  for(i=0;i<=(n+K);i++){
    int m = 0;
    scalar tcur = T[i];
    
    while((i+m)<=(n+K) && T[i+m]==tcur){
      m++;
    }
    i=i+m-1;
    
    for(int j=0;j<(K-m);j++){
      
      NUBSpline * tmp = expanded; 
      expanded = expanded->insertKnot(T[i]);
      if(tmp!=this)
        delete tmp;      
    }
  }


	//--- Create the corresponding curve segments (which by now
	//--- are all bezier curves).
  MatrixMxN bezier(4,4);

  bezier.entry[0][0] =-1.0f; bezier.entry[0][1] = 3.0f; bezier.entry[0][2] =-3.0f; bezier.entry[0][3] = 1.0f;
  bezier.entry[1][0] = 3.0f; bezier.entry[1][1] =-6.0f; bezier.entry[1][2] = 3.0f; bezier.entry[1][3] = 0.0f;
  bezier.entry[2][0] =-3.0f; bezier.entry[2][1] = 3.0f; bezier.entry[2][2] = 0.0f; bezier.entry[2][3] = 0.0f;
  bezier.entry[3][0] = 1.0f; bezier.entry[3][1] = 0.0f; bezier.entry[3][2] = 0.0f; bezier.entry[3][3] = 0.0f;

  this->count = n-K+2;
	segments = new CubicCurve[count];

  for(i=0;i<count;i++){
		segments[i].setBase(bezier);
    int j = i*4;
    VectorN * g0 = &expanded->P[j];
    VectorN * g1 = &expanded->P[j+1];
    VectorN * g2 = &expanded->P[j+2];
    VectorN * g3 = &expanded->P[j+3];
    segments[i].setGeometry(*g0,*g1,*g2,*g3);
	}

  if(expanded!=this)
    delete expanded;
};

/**
 * This method solves a tridiagonal linear equation set in m-dimensions.
 *
 *   M X = B
 *
 * where M is a tridiagonal matrix defined as follows:
 *
 * | b[0] c[1]                     |
 * | a[1] b[1] c[2]                |
 * |      a[2]  ..                 |
 * |                 b[n-2] c[n-2] |
 * |                 a[n-2] b[n-1] |
 *
 */
void NUBSpline::triDiagSolve(scalar * a,scalar * b,scalar * c,VectorN ** X,VectorN * B,int n){

  int i,j;
  VectorN g(n);
  MatrixMxN d(n,dim);

  //--- Forward elimination sweep
  g.entry[0] = c[0]/b[0];
  for(j=0;j<dim;j++){
    d.entry[0][j] = B[0].entry[j]/b[0];
  }
  for(i=1;i<n;i++){
    scalar tmp = b[i] - a[i]*g.entry[i-1];
    g.entry[i] = c[i]/tmp;
    for(j=0;j<dim;j++){
      d.entry[i][j] = (B[i].entry[j] - a[i]*d.entry[i-1][j]) /tmp;
    }
  }
  //--- Backward substitution sweep
  for(j=0;j<dim;j++){
    X[n-1]->entry[j] = d.entry[n-1][j];
  }
  for(i=n-2;i>=0;i--){
    for(j=0;j<dim;j++){
      X[i]->entry[j] = d.entry[i][j] - g.entry[i] * X[i+1]->entry[j];
    }
  }

};

/**
 * Insert Knot.
 * This implementation is based on the algorithm in "Fundamentals of
 * Computer Aided Geometric Design", by J. Horschek and D. Lasser,
 * pp. 190-192.
 *
 * Note that the algorithm in the book does not treat the cases
 * where
 *
 *    r < K or   r > n-1
 *
 * For an open spline this will cause interpolation of non-existent
 * de Boor points and therefor cause array out bound exceptions!!! In
 * short we must require the multiplicity at the endpoints to be K!!!
 *
 * @param t    The new knot-value that should be inserted.
 *
 * @return     A reference to a new spline, identical to this
 *             spline except that it also contains the inserted
 *             knot value.
 */
NUBSpline * NUBSpline::insertKnot(const scalar & t){
  int i;

  //--- Allocate memory for the new spline with
  //--- the ``extra'' knot value
  NUBSpline * spline = new NUBSpline();

  spline->K   = K;
  spline->n   = n + 1;
  spline->dim = dim;

  spline->T = new scalar[n+K+2];
  spline->P = new VectorN[n+2];
  spline->X = new VectorN[n];

  for(i=0;i<(n+2);++i){// i < |P'|
    spline->P[i].resize(dim);
  }
  for(i=0;i<n;++i){// i < |X'|
    spline->X[i].resize(dim);
  }

  //--- Determine the interval of knot values, where
  //--- the new knot value belongs to.
  int r = findKnotSpan(t,n,K,T);

  //--- Construct the new knot vector of the new spline
  //--- The i'th index is the indexing of the new spline,
  //--- not the old one.
  for(i=0;i<(n+K+2);++i){// i < |T'|
    if(i<=r)
      spline->T[i] = T[i];
    else if(i>(r+1))
      spline->T[i] = T[i-1];
    else
      spline->T[r+1] = t;
  }

  //--- Compute the new Control Points
  VectorN tmpA(dim);
  VectorN tmpB(dim);

  for(i=0;i<(n+2);++i){// i < |P'|
  
    spline->P[i].setZero();
    
    if(i<=(r-K+1)){
      spline->P[i].set(P[i]);
    }else if(i>r){
      spline->P[i].set(P[i-1]);
    }else{
      //---               t - t_i
      //---  a_i = --------------------
      //---          t_{i+k-1} - t_i
      scalar ai = (t - T[i])/(T[i+K-1]-T[i]);

      tmpA.set(P[i-1]);
      tmpA.mul(1-ai);
      tmpB.set(P[i]);
      tmpB.mul(ai);
      spline->P[i].add(tmpA,tmpB);
    }
  }

  //--- Compute the new data points.
  for(i=0;i<n;++i){// i < |X'|
    if(i<(r-K+2))
      spline->X[i].set(X[i]);
    else if (i>(r-K+2))
      spline->X[i].set(X[i-1]);
    else
      spline->getPointOnSpline(spline->T[r+1],spline->X[r-K+2]);
  }

  return spline;
};

/**
 * The i'th Basis Blending Function.
 * This method implements the de Cox's recursive definition of a
 * B-spline basis function.
 *
 * @param i  The index of the basis function.
 * @param k  The order of the spline (and not the degree, which are k-1).
 * @param u  The parameter value at which the i'th basis function should
 *           be evaluated at.
 *
 * @return   The value of the i'th k'th order basis function at the
 *           parameter value u.
 */
scalar NUBSpline::N(const int & i,const int & k,const scalar & u){
  if(k==1){

    //--- Oh dear!!!, N(i,k,T[n+1]) must be 1.
    //--- Otherwise the unity property will break?
    //--- and Q(T[n+1]) will always be zero, regardles
    //--- of control points and knot vector values.
    if(u==T[i+1] && T[i+1]==T[n+K])
      return 1;

    if((T[i] <= u) && (u <T[i+1])){
      return 1;
    }else{
      return 0;
    }

  }else{

    scalar a = T[i+k-1]-T[i];
    if(a!=0){
      scalar tmp=N(i,k-1,u);
      if(tmp!=0){
        a = ((u-T[i])*tmp)/a;
      }else{
        a=0;
      }
    }

    scalar b = T[i+k]-T[i+1];
    if(b!=0){
      scalar tmp=N(i+1,k-1,u);
      if(tmp!=0){
        b = ((T[i+k]-u)*tmp)/b;
      }else{
        b=0;
      }
    }

    return a + b;
  }
};

/**
 * Get Segment Count.
 *
 * @return  The number of cubic curves making
 *          up this spline. Each such cubic curve
 *          is referred to as a segement of the spline.
 */
int NUBSpline::getSegmentCount(){
  return count;
};

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
CubicCurve * NUBSpline::getSegment(int i){
	return &segments[i];
};

/**
 * Retrieve the dimension of the spline.
 *
 * @return   The number of dimensions.
 */
int NUBSpline::getDimension(){
  return P[0].getDimension();
};

/**
 * Get Point On Spline.
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
void NUBSpline::getPointOnSpline(const scalar & u,VectorN & p){
  //--- This implementation favors readability and understandability
  //--- The method is far from being optimal implemented.
  //---
  //--- An inverse triangular scheme would be the most efficient
  //--- way to compute this (see the nurbs handbook pp.68-70).

  int j= findKnotSpan(u,n,K,T);
  VectorN pi(dim);
  p.setZero();
  for(int i=(j-K+1);i<=j;i++){
    scalar nn = N(i,K,u);
    if(nn!=0){
      pi.set(P[i]);
      pi.mul(nn);
      p.add(pi);
    }
  }
};

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
VectorN * NUBSpline::getControlPoints(){
  return P;
};

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
VectorN * NUBSpline::getDataPoints(){
  return X;
};

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
scalar * NUBSpline::getKnotVector(){
  return T;
};

/**
 * Get Spline Order.
 *
 * @return   The order of the spline.
 */
int NUBSpline::getOrder(void){
  return K;
};

/**
 * String Conversion Method.
 * This method converts the spline into a textual
 * string representation.
 *
 * @return    A reference to a string representing
 *            information about the spline.
 */
std::string NUBSpline::toString(){
  std::string spline;

/*
  spline = "";
  spline += "P[0]="+P[0];
  for(int i=1;i<P.length;i++){
    spline += "\nP["+i+"]="+P[i];
  }

  spline += "\nX[0]="+X[0];
  for(int i=1;i<X.length;i++){
    spline += "\nX["+i+"]="+X[i];
  }

  spline += "\nT[0]="+T[0];
  for(int i=1;i<T.length;i++){
    spline += "\nT["+i+"]="+T[i];
  }

  scalar t = T[T.length-1];
*/

  return spline;
};

/**
 * Find Knot Span
 * This method performs a binary search on the specified knot
 * vector in order to determine and index value such that the
 * following condition holds
 *
 *   T[j] <= u < T[j+1] (*)
 *
 * The knot vector is defined to be
 *
 *   T = [ t0, t1, ..., t_{k-1}, tk, t_{k+1}, ..., t_{n-1}, tn, t_{n+1}, ..., t_{n+k} ]
 *
 * Where
 *
 *  t0 = t1 = ... = tk-1  and tn+1 = tn+2 = ... = tn+k
 *
 * @param u  The parameter value.
 * @param n  This number indicates how many entries there are in the knot
 *           vector (actually it defines the amount of a whole bunch of
 *           things).
 * @param k  The order of the spline (and knot the degree, which are k-1).
 * @param T  A reference to the knot vector. It will have n+k+1 knot values.
 * @return   The index value of the lowest knot value in the
 *           knot span, which overlaps with t. In other words
 *           the value of j in the equation (*).
 */
int NUBSpline::findKnotSpan(const scalar & u,const int & n,const int & k,scalar * T){
  //---  Linear search Version
//  int upper = n+k;
//  for(int i=0;i<(n+k);i++){
//    if(T[i]<=u && u<T[i+1])
//      return i;
//  }
//  if(T[n+k]==u)
//    return n+k;

  //--- Binary Search Version
  //int high = n+1;   //--- In case end knot has multiplicity k
  int m=0;
  scalar latch = T[n+k];
  while(T[n+k-m]==latch)
    m++;
  int high = n+k-m+1;

  if(u==T[high])//--- Special Case
    return n;

  //int low = k-1;  //--- In case start knot has multiplicity k
  m=0;
  latch = T[0];
  while(T[m]==latch)
    m++;
  int low = m-1;

  //--- Note that a right shift operation on an
  //--- integer corresponds to division by 2.
  int mid = (low+high)>>1;
  //--- Perform a binary search, looking for
  //--- the wanted index value j.
  while(u<T[mid] || u>=T[mid+1]){
    if(u<T[mid]){
      high = mid;
    }else{
      low = mid;
    }
    mid = (low+high)>>1;
  }
  return mid;
};

/**
 * Fast Get Point On Spline.
 * The effect of this method is exactly the same as the getPointOnSpline()
 * method. The only real difference lies in their implementation.
 *
 * Disregarding the dynamic memory allocation in this method it is
 * infact the optimal implementation. For details look at pp.68-70 in
 * "The Nurbs Book" by L. Piegl and W. Tiller.
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
void NUBSpline::getFastPointOnSpline(const scalar & u,VectorN & p){
  int i = findKnotSpan(u,n,K,T);

  VectorN left(K+1);
  VectorN right(K+1);
  VectorN M(K);

  left.entry[0]  = u - T[i+1];
  right.entry[0] = T[i] - u;

  //--- The 1'th order of the basis functions
  left.entry[1]  = u - T[i];
  right.entry[1] = T[i+1] - u;
  M.entry[0]     = 1.0f;

  for(int k=2;k<=K;k++){

      left.entry[k]  = u - T[i+1-k];
      right.entry[k] = T[i+k] - u;

      scalar saved = 0.0f;

      for(int r=0;r<=(k-2);r++){

        scalar tmp = 0;

        scalar nominator = (right.entry[r+1] + left.entry[k-r-1]);

        if(nominator!=0){
          tmp = M.entry[r]/nominator;
          M.entry[r] = saved + right.entry[r+1]*tmp;
          saved = left.entry[k-r-1]*tmp;
        }else{
          M.entry[r] = saved;
          saved = 0;
        }

      }

      M.entry[k-1] = saved;
  }

  //--- Now M contains the values N(i-k+1,k),..,N(i,k) in
  //--- that order.
  VectorN pj(dim);
  p.setZero();
  for(int r=0; r < K ; r++){
    int j = (i-K+1+r);
    pj.set(P[j]);
    pj.mul(M.entry[r]);
    p.add(pj);
  }

};

/**
 * Computation of Derivatives.
 * This method efficiently computes all derivatives from 0 upto J'th order
 * of the spline at the parameter value u. Note that the 0'th order derivaive
 * is simply the point on the spline at the parameter value u.
 *
 * The method can for instance be used to retrieve velocity and acceleration
 * information about an object moving along the spline.
 *
 * @param u      The parameter value at which the derivatives should be computed.
 * @param J      The highest order derivative, which should be computed, should be
 *               less than the order of the spline since all other higher order
 *               derivatives is trivially known to be zero.
 * @param dQ     Upon return this array will contain the computed derivatives. The
 *               array should have J+1 entries. dQ[j] will then contain the j'th
 *               derivative of Q(u).
 *
 * @return       A reference to an array, which will contain the computed derivatives.
 *               The array would have J+1 entries. dQ[j] will contain the j'th derivative
 *               of Q(u).
 */
void NUBSpline::getDerivatives(const scalar & u,int J,VectorN * dQ){
  int r,j;
  int i = findKnotSpan(u,n,K,T);

  //--- Safety control, all higher order derivatives with J>=K is
  //--- trivially known to be zero.
  J = min(J,K-1);

  //--- Allocation of some working storage. This could proberly be
  //--- optimized with lazy allocation in an enviroment of repreated
  //--- queries.
  VectorN left(K+1);
  VectorN right(K+1);
  MatrixMxN M(K,K);
  
  //--- Storage for the derivatives of the basis functions.
  //--- Entry dN[j][r] is the j'th derivative of
  //--- the basis function N(i-k+1+r,k)
  MatrixMxN dN(J+1,K);

  //--- Initialization of left and right arrays
  left.entry[0]  = u - T[i+1];
  right.entry[0] = T[i] - u;

  //--- Computation of the 1'th order of the basis functions
  left.entry[1]  = u - T[i];
  right.entry[1] = T[i+1] - u;
  M.entry[0][0]  = 1.0f;

  //--- Computation of higher order basis functions and their
  //--- corresponding "right side" knot differences.
  for(int k=2;k<=K;k++){
    left.entry[k]  = u - T[i+1-k];
    right.entry[k] = T[i+k] - u;

    scalar saved = 0.0f;

    for(r=0;r<=k-2;r++){
      //--- Store knot difference in lower triangle of M
      M.entry[k-1][r] = (right.entry[r+1]+left.entry[k-r-1]);
      scalar tmp = M.entry[r][k-2]/M.entry[k-1][r];

      //--- Save N(i-k+1+r,k) in upper triangle of M
      M.entry[r][k-1] = saved + right.entry[r+1]*tmp;
      saved = left.entry[k-r-1]*tmp;
    }
    //--- Save N(i,k) in the diagonal of M
    M.entry[k-1][k-1] = saved;
  }

  //--- Trivially we already know the 0'th derivatives
  //--- of the basis functions they are simply the basis
  //--- functions themself.
  for(r=0;r<K;r++){
    dN.entry[0][r] = M.entry[r][K-1];
  }

  //--- Now we must compute the remaining higher order derivatives.
  //--- We will do this by using eq 5, we will need some working
  //--- storage to hold the values of the a-coefficient.
  MatrixMxN a(2,K);

  //--- Out loop is over function indices
  for(r=0;r<=K-1;r++){

    int s1 = 0;
    int s2 = 1;
    a.entry[0][0] = 1.0f; //--- This is easily seen from (eq 5.A)

    //--- Inner loop is over the derivatives
    for(j=1;j<=J;j++){

      scalar djN = 0.0f;

      //--- The case of a(j,0)
      if(r >= j){
        if(M.entry[K-j][r-j]!=0)
          a.entry[s2][0] = a.entry[s1][0] / M.entry[K-j][r-j];
        else
          a.entry[s2][0] = 0;

        djN +=  a.entry[s2][0] * M.entry[r-j][K-j-1];
      }

      //--- The case of a(j,1) to a(j,j-1)
      int pMin,pMax;

      if((r + 1) >= j){//--- determine lowest p for which we have a non-zero basis function
        pMin = 1;
      }else{
        pMin = j-r;
      }

      if(r <= (K -j)){//--- determine highest p for which we have a non-zero basis function
        pMax = j - 1;
      }else{
        pMax = K - 1 -r;
      }

      for(int p=pMin;p<=pMax;p++){
        if(M.entry[K-j][r-j+p]!=0)
          a.entry[s2][p] = (a.entry[s1][p]-a.entry[s1][p-1]) / M.entry[K-j][r-j+p];
        else
          a.entry[s2][p] = 0.0f;

        djN += a.entry[s2][p]*M.entry[r-j+p][K-j-1];
      }

      //--- The case of a(j,j)
      if(r <= (K-1-j)){
        if(M.entry[K-j][r]!=0)
          a.entry[s2][j] = - a.entry[s1][j-1] / M.entry[K-j][r];
        else
          a.entry[s2][j] = 0.0f;

        djN +=  a.entry[s2][j] * M.entry[r][K-j-1];
      }

      dN.entry[j][r] = djN;

      //--- We only need the currently computed row of a-coefficients
      //--- in the next iteration. Swapping indices allows us to use
      //--- row s2 for computing the a-coefficients in the next iteration
      //--- and row s1 will then hold the old values in the next iteration.
      int tmp = s1;
      s1 = s2;
      s2 = tmp;
    }
  }

  //--- From (eq 5) we see that all that remains is to
  //--- premultiply by the factor (k-1)!/(k-j-1)!
  scalar factor = K-1;
  for(j=1;j<=J;j++){
    for(r=0;r<=(K-1);r++){
      dN.entry[j][r] = factor * dN.entry[j][r];
    }
    factor = factor * (K-1-j);
  }

  //--- Now let us compute the derivatives
  VectorN tmp(dim);

  for(j=0;j<=J;j++){
    dQ[j].setZero();
    for(int r=0;r<=K-1;r++){
      tmp.set(P[i-K+1+r]);
      tmp.mul(dN.entry[j][r]);
      dQ[j].add(tmp);
    }
  }

};
