/* Copyright (C) Eddie Edwards, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eddie Edwards, 2000"
 */
// TrigFunction.h
//
// a DifferentiableFunction representing one of the
// functions +/- sin and +/- cos

class TrigFunction : public DifferentiableFunction
{
public:
	TrigFunction();

	bool	IsZero();
	Number	Evaluate(Number x);
	void	Differentiate();

private:
	double	(*Fn)(double);		// function pointer - sin or cos
	Number	Sign;
};
