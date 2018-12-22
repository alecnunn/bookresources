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
#ifndef __RANDOM_INCLUDED__
#define __RANDOM_INCLUDED__
#include<stdlib.h>
#include<time.h>
#include<numerical/scalar.h>

/**
 * This class generates random numbers in a specified range.
 *
 * Notice that an instance of this class can be used as an ordinary
 * scalar variable in any expression, and each time the epression is
 * evaluated, the instance has a new random value.
 *
 * Notice that the random values are scalaring point values and not
 * integer values
 *
 * The random values are in the interval [MIN..MAX], this interval can be set
 * when an instance is declared or afterwards.
 */
class RandomNumber
{
public:

	RandomNumber(){
		m_min  = 0;
		m_max  = RAND_MAX;
		m_diff = m_max - m_min;
		init();
	};

	RandomNumber(scalar _min,scalar _max){
		m_min  = _min;
		m_max  = _max;
		m_diff = m_max - m_min;
		init();
	};

	~RandomNumber(){};

	//Set minimum and maximum bounds for random values
	void setMin(scalar _min){	m_min=_min;	m_diff=m_max-m_min;	};
	void setMax(scalar _max){	m_max=_max;	m_diff=m_max-m_min;	};

	//Get minimum and maximum bounds for random values
	scalar getMin(void){return m_min;};
	scalar getMax(void){return m_max;};

	//Get a random number from the bounds given
	operator scalar(){return calc();};
	scalar value(){return calc();};

private:

	//Initialize the random number generator
	void init(){srand( (unsigned)time( NULL ) );};

	//Calculate the random number
	scalar calc(){
		return m_min + (rand() * m_diff)/RAND_MAX;
	};

private:

  //bounds of random numbers 0<=min<max<=RAND_MAX
	scalar m_min,m_max,m_diff;

};
#endif //__RANDOM_INCLUDED__
