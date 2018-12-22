/* Copyright (C) Eddie Edwards, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eddie Edwards, 2000"
 */
// Poly.cpp
//
// see Poly.h

#include <cmath>
#include <cstdio>

#include "DifferentiableFunction.h"
#include "Poly.h"

// Poly
//
// constructor / destructor

Poly::Poly(Number constant)
{
	Degree = 0;
	Coeff = new Number[9];
	MaxDegree = 8;
	Coeff[0] = constant;
}

Poly::~Poly()
{
	delete[] Coeff;
}

// SetDegree
//
// set the degree of the polynomial, and handle
// memory management

void Poly::SetDegree(int degree)
{
	if (degree > MaxDegree)
	{
		// reallocate
		Number*	coeff = new Number[degree + 1];
		int ii;

		for (ii = 0; ii <= Degree; ii++)
		{
			coeff[ii] = Coeff[ii];
		}
		for (; ii <= degree; ii++)
		{
			coeff[ii] = 0;
		}
		delete[] Coeff;
		Coeff = coeff;
		MaxDegree = degree;
	}
	else if (degree > Degree)
	{
                int ii;
		// else just zero the new coeffs
		for (ii = Degree + 1; ii <= degree; ii++)
		{
			Coeff[ii] = 0;
		}
	}

	Degree = degree;

	return;
}

// SetCoeff
//
// set a coefficient

void Poly::SetCoeff(int coeff, Number value)
{
	if (coeff > Degree)
	{
		SetDegree(coeff);
	}

	Coeff[coeff] = value;
}

// GetDegree
//
// get degree

int Poly::GetDegree()
{
	return Degree;
}

// GetCoeff
//
// get a coefficient

Number Poly::GetCoeff(int coeff)
{
	if ((coeff >= 0) && (coeff <= Degree))
	{
		return Coeff[coeff];
	}

	return 0;
}

// Evaluate
//
// evaluate the polynomial

Number Poly::Evaluate(Number x)
{
	Number	powx = 1;
	Number	sum = 0;

	for (int ii = 0; ii <= Degree; ii++)
	{
		sum += powx * Coeff[ii];
		powx *= x;
	}

	return sum;
}

// ChangeUnits
//
// change the polynomial's input units

void Poly::ChangeUnits(Number old_units, Number new_units)
{
	Number	mult = old_units / new_units;
	Number	powmult = 1;

	for (int ii = 0; ii <= Degree; ii++)
	{
		Coeff[ii] *= powmult;
		powmult *= mult;
	}
}

// ChangeOutputUnits
//
// changes the polynomial's output units

void Poly::ChangeOutputUnits(Number old_units, Number new_units)
{
	Number	mult = new_units / old_units;

	for (int ii = 0; ii <= Degree; ii++)
	{
		Coeff[ii] *= mult;
	}
}

// Differentiate
//
// differentiate the polynomial

void Poly::Differentiate()
{
	for (int ii = 1; ii <= Degree; ii++)
	{
		Coeff[ii-1] = Coeff[ii] * Number(ii);
	}
	Coeff[Degree] = 0;
	if (Degree > 0)
	{
		SetDegree(Degree - 1);
	}
}

// IsZero
//
// is the polynomial zero?

bool Poly::IsZero()
{
	return ((Degree == 0) && (Coeff[0] == 0));
}

// CopyPoly
//
// copy the supplied polynomial
// (note: corrupts the old copy)

void Poly::CopyPoly(Poly* p)
{
	Number	nfact = 1;

	SetDegree(0);
	Coeff[0] = p->Evaluate(0);
	p->Differentiate();

	while (!p->IsZero())
	{
		SetDegree(Degree + 1);
		nfact *= Number(Degree);
		Coeff[Degree] = p->Evaluate(0) / nfact;
		p->Differentiate();
	}
}

// MakeTaylorSeries
//
// make the Taylor Series for the differentiable function supplied
// (note: corrupts the old function object)

void Poly::MakeTaylorSeries(DifferentiableFunction* fn, Number pt, int max_degree)
{
	Number	nfact = 1;

	SetDegree(0);
	Coeff[0] = fn->Evaluate(pt);
	fn->Differentiate();

	while (!fn->IsZero() && (Degree < max_degree))
	{
		SetDegree(Degree + 1);
		nfact *= Number(Degree);
		Coeff[Degree] = fn->Evaluate(pt) / nfact;
		fn->Differentiate();
	}

	OffsetDomain(pt);
}

// MakeLagrangeSeries
//
// generate the unique polynomial which passes through all
// the supplied points

void Poly::MakeLagrangeSeries(int numpts, Number* x, Number* y)
{
	// clear this polynomial
	SetDegree(0);
	Coeff[0] = 0;

	// do each sub-polynomial
	for (int jj = 0; jj < numpts; jj++)
	{
		// start with the poly 1.0
		Poly	temp(1);

		// multiply by (x - xn) when n != jj
		for (int ii = 0; ii < numpts; ii++)
		{
			if (ii != jj)
			{
				temp.MultiplyByFactor(-x[ii]);
			}
		}

		// renormalize so temp @ x[jj] = y[jj]
		temp.ChangeOutputUnits(temp.Evaluate(x[jj]), y[jj]);

		// add in to the sum
		Add(&temp);
	}

	// all done
	return;
}

// OffsetDomain
//
// shift the input domain

void Poly::OffsetDomain(Number pt)
{
	Poly	pow(1);
	Poly	result;
	int ii;

	for (ii = 0; ii < Degree; ii++)
	{
		result.Add(&pow, Coeff[ii]);
		pow.MultiplyByFactor(-pt);
	}

	SetDegree(result.Degree);
	for (ii = 0; ii < Degree; ii++)
	{
		Coeff[ii] = result.Coeff[ii];
	}

	return;
}

// OffsetRange
//
// shift the output range

void Poly::OffsetRange(Number pt)
{
	Coeff[0] += pt;
}

// ForceOdd
//
// force the polynomial to be odd

void Poly::ForceOdd()
{
	for (int ii = 0; ii <= Degree; ii += 2)
	{
		Coeff[ii] = 0;
	}
}

// ForceEven
//
// force the polynomial to be even

void Poly::ForceEven()
{
	for (int ii = 1; ii <= Degree; ii += 2)
	{
		Coeff[ii] = 0;
	}
}

// MultiplyByFactor
//
// multiply the polynomial by (x + a)

void Poly::MultiplyByFactor(Number a)
{
	SetDegree(Degree + 1);

	for (int ii = Degree; ii > 0; ii--)
	{
		Coeff[ii] = Coeff[ii-1] + Coeff[ii] * a;
	}
	Coeff[0] = Coeff[0] * a;
}

// Add
//
// add another polynomial

void Poly::Add(Poly* p, Number coeff)
{
	if (p->Degree > Degree)
	{
		SetDegree(p->Degree);
	}

	for (int ii = 0; ii <= Degree; ii++)
	{
		Coeff[ii] += coeff * p->Coeff[ii];
	}
}

// Print
//
// print a polynomial out

void Poly::Print()
{
	if (sizeof(Number) == sizeof(float))
	{
		union
		{
			float			f;
			unsigned long	l;
		} number;

		printf("Degree: %d\n", Degree);

		for (int ii = 0; ii <= Degree; ii++)
		{
			number.f = Coeff[ii];

			printf("c[%d] = %f [0x%8.8X]\n", ii, number.f, number.l);
		}
	}
	else if (sizeof(Number) == sizeof(double))
	{
		union
		{
			double			d;
			unsigned long	l[2];
		} number;

		printf("Degree: %d\n", Degree);

		for (int ii = 0; ii <= Degree; ii++)
		{
			number.d = Coeff[ii];

			printf("c[%d] = %lf [0x%8.8X%8.8X]\n", ii, number.d, number.l[1], number.l[0]);	// assumes little-endian machine
		}
	}
}
