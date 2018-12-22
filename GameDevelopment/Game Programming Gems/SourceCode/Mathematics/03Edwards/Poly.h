/* Copyright (C) Eddie Edwards, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Eddie Edwards, 2000"
 */
// Poly.h
//
// polynomial class

class Poly : public DifferentiableFunction
{
public:
	Poly(Number constant = 0);
	~Poly();

	void	SetDegree(int degree);		// handles memory-management issues
	void	SetCoeff(int coeff, Number value);

	int		GetDegree();
	Number	GetCoeff(int coeff);

	bool	IsZero();
	Number	Evaluate(Number x);
	void	Differentiate();

	void	ChangeUnits(Number old_units, Number new_units);
	void	ChangeOutputUnits(Number old_units, Number new_units);
	void	OffsetDomain(Number pt);
	void	OffsetRange(Number pt);

	void	CopyPoly(Poly* p);
	void	MakeTaylorSeries(DifferentiableFunction* fn, Number pt, int max_degree);
	void	MakeLagrangeSeries(int numpts, Number* x, Number* y);

	void	ForceOdd();
	void	ForceEven();

	void	MultiplyByFactor(Number a);
	void	Add(Poly* p, Number coeff = 1);

	void	Print();

private:
	int		Degree;
	Number*	Coeff;
	int		MaxDegree;
};
