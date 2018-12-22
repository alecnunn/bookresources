#include "JPIKSolver.h"

JPIKSolver::JPIKSolver()
{
	MaxSteps = 200;
	fnJacobian = NULL;
	fnValues = NULL;
	m_pClient = NULL;
	K1 = 0.2f;
	K2 = 0.0f;
	Tolerance = 0.05f;
}

JPIKSolver::~JPIKSolver()
{
}

matrix<float> JPIKSolver::Solve()
{
	int NumFunc = FinalVal.RowNo();
	int NumVar = InitVars.RowNo();

	matrix<float> Jacob;				// Jacobian matrix.
	matrix<float> PsI( NumFunc, NumVar );	// Pseudoinverse matrix.
	matrix<float> Vel( NumFunc, 1 );		// Column matrix for functions increment.
	matrix<float> Z( NumVar, 1 );			// Vector for secondary task.
	matrix<float> I; I.Unit(NumVar);		// Unit matrix.
	matrix<float> Sol;				// Solution vector.

	matrix<float> InitVal = fnValues( InitVars, m_pClient );

	bool ifc;
	int n, i;

	// If the difference between final values and those returned by function 'fnValues'
	// is lower than Tolerance, we must keep initial variables.

	Sol = InitVars;
	
	n = 0;
	while( true )
	{
		Vel = FinalVal - fnValues( Sol, m_pClient );
		for( i = 0; i < NumFunc; i++ )
		{
			if( fabs( Vel(i,0) ) > Tolerance )
			{
				ifc = true;
				break;
			}
			else ifc = false;
		}
		if( !ifc )
			break;

		Jacob = fnJacobian( Sol, m_pClient );

		// Jacobian pseudoinverse.
			
		PsI = (~Jacob) * Inv( Jacob * (~Jacob) );


		// New solution values.

		Sol = Sol + K1 * ( PsI * Vel )  + K2 * ( I - PsI * Jacob ) * Z;

		n++;
		if( n > MaxSteps )
			break;
	}

	return Sol;
}