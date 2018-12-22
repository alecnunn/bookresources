/* Copyright (C) Eddie Edwards, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eddie Edwards, 2000"
 */
// TrigFunction.cpp
//
// see TrigFunction.h

#include <cmath>

#include "DifferentiableFunction.h"
#include "TrigFunction.h"

TrigFunction::TrigFunction()
{
	Fn = sin;
	Sign = 1;
}

bool TrigFunction::IsZero()
{
	return false;	// never identically zero
}

Number TrigFunction::Evaluate(Number x)
{
	return Sign * Number(Fn(double(x)));
}

void TrigFunction::Differentiate()
{
	if (Fn == sin)
	{
		// sin -> cos
		Fn = cos;
	}
	else
	{
		// cos -> -sin
		Fn = sin;
		Sign = -Sign;
	}
}
