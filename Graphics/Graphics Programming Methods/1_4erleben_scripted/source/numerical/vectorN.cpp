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
#include<numerical/vectorN.h>
#include<numerical/matrixMxN.h>
#include<utils/random.h>
#include<utils/output.h>
#include<cmath>
#include<string>
using namespace std;

/**
 * Safety Checking.
 * Boolean flag that indicates whatever dimension safety control
 * (among other) is on or off.
 *
 * It is usefull to switch these safety checks on during debugging.
 */
bool VectorN::safety = true;

/**
 * Default constructor.
 */
VectorN::VectorN(){
  n = 4;
  maxN = n;
  entry = new scalar[n];
	setZero();
};

/**
 * Deconstructor.
 */
VectorN::~VectorN(){
  delete [] entry;
};

/**
 * Special Constructor.
 */
VectorN::VectorN(const int & n){
  this->maxN = this->n = n;
	entry = new scalar[n];
	setZero();
};

/**
 * Copy constructor.
 *
 * @param v    The vector which this vector should look like.
 */
VectorN::VectorN(const VectorN & v){
	this->maxN = this->n = v.n;
	entry = new scalar[n];
	set(v);
};

/**
 * A custom constructor.
 *
 * @param coordinates  The coordinates of the vector.
 */
VectorN::VectorN(scalar * coordinates,const int & n){
	this->maxN = this->n = n;
	entry = new scalar[n];
	set(coordinates,n);
};

/**
 * Retrieve the number of coordiantes in the vector.
 */
int VectorN::getDimension(void)const{
	return n;
};

/**
 * Resize Vector.
 *
 * @param n   The new dimension of the vector, must be
 *            greather than zero.
 */
void VectorN::resize(const int & n){
	if(safety){
		if(n<=0){
      Output::error("VectorN","resize","Improper dimension");
		}
	}
	if(n>maxN){
		scalar * newentry = new scalar[n];

    scalar * dst = newentry;
    scalar * src = entry;
    for(int i=0;i<this->n;++i){
      *dst++ = *src++;
    }

    delete [] entry;
		entry = newentry;
		this->maxN = this->n = n;
	}else{
		this->n = n;
	}
};

/**
 * Set all coordinates of the vector to zero.
 */
void VectorN::setZero(){
	for(int i=0;i<n;i++)
     entry[i]=0.0f;
};

/**
 * Test if vector is zero vector.
 */
bool VectorN::isZero(){
  for(int i=0;i<n;i++)
    if(entry[i]!=0.0f)
      return false;
    return true;
};

/**
 * Equality comparison test.
 *
 * @param v     The vector which this vector is compared with.
 *
 * @return      true if the vectors are "equal" otherwise zero.
 */
bool VectorN::equals(const VectorN & v){
	if(n!=v.n){
  	return false;
	}
	for(int i=0;i<n;i++){
		if(entry[i]!=v.entry[i])
		  return false;
	}
	return true;
};

/**
 * Equality comparison test within numerical precision.
 *
 * @param v        The vector which this vector is compared with.
 * 
 * @return         True if the vectors are equal within numerical
 *                 precision otherwise false.
 */
bool VectorN::nearlyEquals(const VectorN & v){
	if(n!=v.n){
		return false;
	}
	scalar eps = precision;
	for(int i=0;i<n;i++){
		if((fabs(entry[i]-v.entry[i])>=eps))
		  return false;
	}
  return true;
};

/**
 * Assignment.
 *
 * @param v    The vector that this vector should match.
 */
void VectorN::set(const VectorN & v){
	if(safety){
		if(v.n!=n){
      Output::error("VectorN","set(VectorN)","Invalid dimension");
		}
	}
	for(int i=0;i<n;i++)
		entry[i] = v.entry[i];
};

/**
 * Assignment.
 *
 * @param x    The new value of the x-coordinate.
 * @param y    The new value of the y-coordinate.
 * @param z    The new value of the z-coordinate.
 */
void VectorN::set(scalar * coordiantes,const int & n){
  //--- The length of coordinates must be equal to
	//--- the dimension of the vector.
	if(safety){
		if(this->n!=n){
      Output::error("VectorN","set(scalar[])","Invalid dimension");
		}
	}
	for(int i=0;i<n;i++)
		entry[i] = coordiantes[i];
};

/**
 * Addition.
 * Takes a vector as an argument and adds it to itself.
 *
 * @param v    The vector that should be added to this vector.
 */
void VectorN::add(const VectorN & v){
	if(safety){
		if(v.n!=n){
      Output::error("VectorN","add(VectorN)","Invalid dimension");
		}
	}
	for(int i=0;i<n;i++)
		entry[i] += v.entry[i];
};

/**
 * Addition.
 * Sets this vector equal to the sum of the two vector arguments.
 *
 * @param a    One of the vectors.
 * @param b    The other of the vectors.
 */
void VectorN::add(const VectorN & a,const VectorN & b){
	//--- Notice that the dimension of a and b must be
	//--- equal to the dimension of this vector.
	if(safety){
		if(a.n!=n || b.n!=n){
      Output::error("VectorN","add(VectorN,VectorN)","Invalid dimension");
		}
	}
	for(int i=0;i<n;i++)
		entry[i] = a.entry[i]+b.entry[i];
};

/**
 * Subtraction.
 * Takes a vector as an argument and subtracts it from itself.
 *
 * @param v    The vector that should be subtracted from this Vector3.
 */
void VectorN::sub(const VectorN & v){
	if(safety){
		if(v.n!=n){
			Output::error("VectorN","sub(VectorN)","Invalid dimension");
		}
	}
	for(int i=0;i<n;i++)
		entry[i] -= v.entry[i];
};

/**
 * Subtraction.
 * Sets this vector equal difference of the two vector arguments.
 *
 * @param a    One of the vectors.
 * @param b    The other of the vectors.
 */
void VectorN::sub(const VectorN & a,const VectorN & b){
	//--- Notice that the dimension of a and b must be
	//--- equal to the dimension of this vector.
	if(safety){
		if(a.n!=n || b.n!=n){
			Output::error("VectorN","sub(VectorN,VectorN)","Invalid dimension");
		}
	}
	for(int i=0;i<n;i++)
		entry[i] = a.entry[i]-b.entry[i];
};

/**
 * Negation.
 */
void VectorN::negate(){
	for(int i=0;i<n;i++)
		entry[i] = - entry[i];
};

/**
 * Negation.
 */
void VectorN::negate(const VectorN & v){
	set(v);
	negate();
};

/**
 * Inner product of this vector and the argument.
 *
 * @param v   The vector that should be used in the dot-product.
 *
 * @return    The dot-product.
 */
scalar VectorN::dot(const VectorN & v){
	if(safety){
		if(v.n!=n){
			Output::error("VectorN","dot(VectorN)","Invalid dimension");
		}
	}
	scalar value = 0;
	for(int i=0;i<n;i++)
	  value += entry[i] * v.entry[i];
	return value;
};

/**
 * Displacement.
 * Adds the specified scaled direction vector to this vector.
 */
void VectorN::displace(const VectorN & dir,const scalar & scale){
	if(safety){
		if(dir.n!=n){
			Output::error("VectorN","displace(VectorN,scalar)","Invalid dimension");
		}
	}
	for(int i=0;i<n;i++)
		entry[i] += dir.entry[i]*scale;
};
  
/**
 * Assignment and Displacement.
 * Adds the specified scaled direction vector to the
 * specified point and store the result in this vector.
 */
void VectorN::displace(const VectorN & p,const VectorN & dir,const scalar & scale){
	if(safety){
		if(dir.n!=n||p.n!=n){
			Output::error("VectorN","displace(VectorN,VectorN,scalar)","Invalid dimension");
		}
	}
	for(int i=0;i<n;i++)
		entry[i] = p.entry[i] + dir.entry[i]*scale;
};
  
/**
 * Scalar multiplication.
 * Multiplies this vector with a scalar.
 *
 * @param s    The scalar that is used in the multiplication.
 */
void VectorN::mul(const scalar & s){
	for(int i=0;i<n;i++)
		entry[i] *= s;
};
 
/**
 * Matrix Multiplication.
 * Post multiplies a vector with a matrix and assigns the result to this vector.
 */
void VectorN::mul(const MatrixMxN & M,const VectorN & a){
  //--- Notice that M.columns must be equal to a.n
	//--- and this.n must be equal to M.rows
	if(safety){
		if(M.columns!=a.n||M.rows!=this->n){
			Output::error("VectorN","mul(MatrixMxN,VectorN)","Invalid dimensions");
		}
	}
	for(int i=0; i<n; i++){
		entry[i] = 0.0f;
		for(int c=0; c< M.columns; c++){
			entry[i] += M.entry[i][c]*a.entry[c];
		}
	}
};

/**
 * Matrix Multiplication.
 * Pre multiplies a vector with a matrix and assigns the result to this vector.
 */
void VectorN::mul(const VectorN & a,const MatrixMxN & M){
	//--- Notice that M.rows must be equal to a.n
	//--- and this.n must be equal to M.columns
	if(safety){
		if(M.rows!=a.n||M.columns!=this->n){
			Output::error("VectorN","mul(VectorN,MatrixMxN)","Invalid dimensions");
		}
	}
	for(int i=0; i<n; i++){
		entry[i] = 0.0f;
		for(int r=0; r< M.rows; r++){
			entry[i] += a.entry[r]*M.entry[r][i];
		}
	}
};

/**
 * Scalar division
 * Divides this vector with a scalar.
 *
 * @param s    The scalar that is used in the division.
 */
void VectorN::div(const scalar & s){

  if(s!=0){
	  for(int i=0;i<n;i++)
		  entry[i] /= s;
  }else{
    Output::warning("VectorN","div","division by zero");
  }
};

/**
 * Calculates the length of this vector.
 *
 * @return   The length of this vector.
 */
scalar VectorN::length(){
  return (scalar)(sqrt( sqrLength() ));
};

/**
 * Calculates the square of the length of this vector.
 *
 * @return   The squared length of this vector.
 */
scalar VectorN::sqrLength(){
  return this->dot(*this);
};
  
/**
 * Calculates the distance from the point representated
 * by this vector to the specified point.
 */
scalar VectorN::distance(const VectorN & p){
  if(safety){
		if(p.n!=n){
			Output::error("VectorN","distance()","Invalid dimension");
		}
	}
	scalar value = 0;
	for(int i=0;i<n;i++){
	  scalar diff = entry[i] - p.entry[i];
		value += diff*diff;
	}
  return (scalar)sqrt(value);
};

/**
 * Makes a unit vector from this vector. This is just another name for normalize.
 */
void VectorN::unit(){
  div(length());
};

/**
 * Assigns this vector the the unit vector of the specified argument.
 *
 * @param v
 */
void VectorN::unit(const VectorN & v){
  set(v);
  unit();
};

/**
 * This method forces entries in this vector to zero.
 * that is all entries numerical less than the accuracy
 * is set to zero.
 */
void VectorN::truncate(void){
	for(int i=0;i<n;i++)
    ::truncate(entry[i]);
};


/**
 * Make Non Negative.
 * This method forces every entry in the vector to be
 * a non negative value, it does so by setting all negative
 * entries to zero.
 */
void VectorN::nonNegative(){
  for(int i=0;i<n;i++){
    if(entry[i]<0){
      entry[i] = 0.0f;
    }
  }
};

/**
 * Random Vector.
 * This method generates a vector of random values, such
 * that all values is between the upper and lower values
 * specified.
 *
 * @param  Lowest allowable random value.
 * @param  Highest allowable random value.
 */
void VectorN::random(const scalar & lower,const scalar & upper){
  RandomNumber R(lower,upper);
  for(int i=0;i<n;i++){
    entry[i] = R;
  }
};

/**
 * Random Vector.
 * This method generates a vector of random values.
 */
void VectorN::random(){
  RandomNumber R;
  for(int i=0;i<n;i++){
    entry[i] = R;
  }
};

/**
 * Converts this vector into a string.
 */
std::string VectorN::toString(){
  std::string vector;
/*    
  vector = "[" + entry[0];
	for(int i=1;i<n;i++){
		vector += "," + entry[i];
	}
	vector += "]";
*/
  return vector;
};
