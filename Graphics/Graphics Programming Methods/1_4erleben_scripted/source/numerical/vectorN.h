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
#ifndef __VECTOR_N_INCLUDED__
#define __VECTOR_N_INCLUDED__
#include <numerical/scalar.h>
#include<string>

/**
 * Forward Declaration.
 */
class MatrixMxN;

/**
 * A N-dimensional standard euclidean vector.
 *
 * Default dimension is 4.
 */
class VectorN
{

  friend class MatrixMxN;

public:
  
  VectorN();
  virtual ~VectorN();

public:

  VectorN(const int & n);
  VectorN(const VectorN & v);
  VectorN(scalar * coordinates,const int & n);
	
public:
    
	int getDimension(void)const;
  void resize(const int & n);
	void setZero();
	bool isZero();
  bool equals(const VectorN & v);
  bool nearlyEquals(const VectorN & v);
  void set(const VectorN & v);
  void set(scalar * coordiantes,const int & n);
  void add(const VectorN & v);
  void add(const VectorN & a,const VectorN & b);
  void sub(const VectorN & v);
  void sub(const VectorN & a,const VectorN & b);
  void negate(void);
  void negate(const VectorN & v);
  scalar dot(const VectorN & v);
  void displace(const VectorN & dir,const scalar & scale);
  void displace(const VectorN & p,const VectorN & dir,const scalar & scale);
  void mul(const scalar & s);
  void mul(const MatrixMxN & M,const VectorN & a);
  void mul(const VectorN & a,const MatrixMxN & M);
  void div(const scalar & s);
  scalar length(void);
  scalar sqrLength(void);
  scalar distance(const VectorN & p);
  void unit(void);
  void unit(const VectorN & v);
  void truncate(void);
  void nonNegative(void);

  void random(const scalar & lower,const scalar & upper);
  void random(void);
  std::string toString(void);

public:
  
  static bool safety;  ///< Safety Checking Flag.

protected:

	int n;      ///< The number of coordinates in the vector. 
              ///< The default value is 4.
	int maxN;   ///< The maximum number of entries this
              ///< vector can be resized to without
              ///< requireing new memory allocation.
public:

  scalar * entry;  ///< The coordinates of this vector.

};/* End class VectorN */
#endif //__VECTOR_N_INCLUDED__
