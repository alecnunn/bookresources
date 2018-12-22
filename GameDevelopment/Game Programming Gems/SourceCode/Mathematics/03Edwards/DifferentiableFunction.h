/* Copyright (C) Eddie Edwards, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eddie Edwards, 2000"
 */
// DifferentiableFunction.h
//
// abstract DifferentiableFunction class
//
// a DifferentiableFunction is a function object
// which can be evaluated at any point, compared
// with the zero function, and differentiated

// Number
//
// this typedef defines the number field over which we
// compute - this is primarily an easy way to switch from
// float to double, but you could replace this with, say,
// Complex, or the integers modulo a prime number

typedef float	Number;

class DifferentiableFunction
{
public:
	virtual bool	IsZero() = 0;			// function is identically zero?
	virtual Number	Evaluate(Number x) = 0;	// evaluate function at x
	virtual void	Differentiate() = 0;
};
