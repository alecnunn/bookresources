#ifndef __JPIKSOLVER_H__
#define __JPIKSOLVER_H__

#include "matrix.h"

using namespace math;

typedef matrix<float> (*JACOBIAN)( matrix<float> variables, void *pClient );
typedef matrix<float> (*VALUES)( matrix<float> variables, void *pClient );

class JPIKSolver
{
	int MaxSteps;			
	float Tolerance;		
	matrix<float> InitVars;	//  Initial variables values for iteration.
	matrix<float> FinalVal;	//  Final values functions must return.
	JACOBIAN fnJacobian;	//  Jacobian calculus method. It is specified by client class.
	VALUES fnValues;		//  Functions values calculus method. It's specified by client class.
	void *m_pClient;		//  Client class using the solver. Prior methods must be implemented.

	float K1, K2;

public:
	JPIKSolver();
	~JPIKSolver();

	inline void SetMaxSteps( int pm )
	{
		MaxSteps = pm;
	}

	inline void SetTolerance( float t )
	{
		Tolerance = t;
	}

	inline bool SetFinalVal( matrix<float> f )
	{
		if( f.ColNo() != 1 ) return false;
		FinalVal = f;
		return true;
	}

	inline bool SetInitVars( matrix<float> Ini )
	{
		if( Ini.ColNo() != 1 ) return false;
		InitVars = Ini;	
		return true;
	}

	inline void SetJacobian( JACOBIAN J )
	{
		fnJacobian = J;
	}

	inline void SetValues( VALUES V )
	{
		fnValues = V;
	}

	inline void SetK( float k1, float k2 )
	{
		K1 = k1;
		K2 = k2;
	}

	inline void SetClient( void *c )
	{
		m_pClient = c;
	}

	matrix<float> Solve();
};


#endif //__JPIKSOLVER_H__