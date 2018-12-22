/*
Sample 2D

  Sample using 5 segments, 50 length units per segment.

  node 0 position: n0 = (0,0 (fixed)
  nodo 1 position: n1 = (L*cos(a0), L*sin(a0)
  nodo 2 position: n2 = (L*cos(a0) + L*cos(a0 + a1), L*sin(a0) + L*sin(a0+a1)
  ...
  nodo 5 position: n5 = (L*(cos(a0)+cos(a0+a1)+...+cos(a0+a1+a2+a3+a4)),
						L*(sin(a0)+sin(a0+a1)+...+sin(a0+a1+a2+a3+a4)))

  To define node 5 position we need its position derivatives to create Jacobian matrix.

  Derivatives: dn5/da0 = (-L*(sin(a0)+sin(a0+a1)+...+sin(a0+a1+a2+a3+a4)),
						L*(cos(a0)+cos(a0+a1)+...+cos(a0+a1+a2+a3+a4)))

			...
			dn5/da3 = (-L*(sin(a0+a1+a2+a3)+sin(a0+a1+a2+a3+a4)),
						L*(cos(a0+a1+a2+a3)+cos(a0+a1+a2+a3+a4)))

			dn5/da4 = (-L*(sin(a0+a1+a2+a3+a4)), L*(cos(a0+a1+a2+a3+a4)))
  
*/

#include "JPIKSolver.h"
#include <conio.h>

class Sim2D
{
public:
	// Computes node 5 position
	static matrix<float> Values( matrix<float> Vbles, void *pClient );

	// Computes node 5 position Jacobian
	static matrix<float> Jacobian( matrix<float> Vbles, void *pClient );

	// Initializes and calls the solver
	void Solve( float x, float y, float ini0, float ini1, float ini2, float ini3, float ini4 );
};



void main( void )
{
	Sim2D s;

	// Initial angles: 0.2, 0.4, 0.6, 0.8, and 1 radians
	// Desired position for node 5: (21,2)
	s.Solve( 21, 2, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f );
	getch();
}


void Sim2D::Solve( float x, float y, float ini0, float ini1, float ini2, float ini3, float ini4 )
{
	JPIKSolver solver;

	matrix<float> Ini(5,1);
	matrix<float> Fin(2,1);

	// Inital angles values
	Ini(0,0) = ini0;
	Ini(1,0) = ini1;
	Ini(2,0) = ini2;
	Ini(3,0) = ini3;
	Ini(4,0) = ini4;

	// Node 5 desired position
	Fin(0,0) = x;
	Fin(1,0) = y;

	// Initializing the solver
	solver.SetClient( this );
	solver.SetInitVars( Ini );
	solver.SetFinalVal( Fin );
	solver.SetValues( Values );
	solver.SetJacobian( Jacobian );
	solver.SetK( 0.1f, 0.0f ); // K2 not used in this implementation
	solver.SetMaxSteps( 200 );
	solver.SetTolerance( 0.001f );
	solver.Solve();
}

matrix<float> Sim2D::Values( matrix<float> Vbles, void *pClient )
{
	matrix<float> A;
	float b0, b1, b2, b3, b4;
	b0 = Vbles(0,0) + Vbles(1,0) + Vbles(2,0) + Vbles(3,0) + Vbles(4,0);
	b1 = Vbles(0,0) + Vbles(1,0) + Vbles(2,0) + Vbles(3,0);
	b2 = Vbles(0,0) + Vbles(1,0) + Vbles(2,0);
	b3 = Vbles(0,0) + Vbles(1,0);
	b4 = Vbles(0,0);

	A.SetSize(2,1);

	A(0,0) = 50 * (cos(b0) + cos(b1) + cos(b2) + cos(b3) + cos(b4));
	A(1,0) = 50 * (sin(b0) + sin(b1) + sin(b2) + sin(b3) + sin(b4));

	return A;
}

matrix<float> Sim2D::Jacobian( matrix<float> Vbles, void *pClient )
{
	matrix<float> J;
	float b0, b1, b2, b3, b4;
	b0 = Vbles(0,0) + Vbles(1,0) + Vbles(2,0) + Vbles(3,0) + Vbles(4,0);
	b1 = Vbles(0,0) + Vbles(1,0) + Vbles(2,0) + Vbles(3,0);
	b2 = Vbles(0,0) + Vbles(1,0) + Vbles(2,0);
	b3 = Vbles(0,0) + Vbles(1,0);
	b4 = Vbles(0,0);

	J.SetSize(2,5);

	J(0,0) = -50 * ( sin(b0) + sin(b1) + sin(b2) + sin(b3) + sin(b4) );
	J(1,0) = 50 * (cos(b0) + cos(b1) + cos(b2) + cos(b3) + cos(b4));

	J(0,1) = -50 * (sin(b0) + sin(b1) + sin(b2) + sin(b3));
	J(1,1) = 50 * (cos(b0) + cos(b1) + cos(b2) + cos(b3));

	J(0,2) = -50 * (sin(b0) + sin(b1) + sin(b2));
	J(1,2) = 50 * (cos(b0) + cos(b1) + cos(b2));

	J(0,3) = -50 * (sin(b0) + sin(b1));
	J(1,3) = 50 * (cos(b0) + cos(b1));

	J(0,4) = -50 * sin(b0);
	J(1,4) = 50 * cos(b0);

	return J;
}
