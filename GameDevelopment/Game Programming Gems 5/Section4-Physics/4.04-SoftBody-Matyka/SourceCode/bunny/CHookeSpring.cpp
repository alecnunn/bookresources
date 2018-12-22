/************************************************************************/
/*                                                                      */
/*                       Soft Body 3.0                                  */
/*                 by Maciej Matyka, 21.IX.2004                         */
/*                                                                      */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/*                                                                      */
/*  Made for purposes of Grame Programming Gems 5 article.              */
/*                                                                      */
/*  note: this code has been attached to article  only as               */
/*  an example of described method. No any guarrantee of anything is    */
/*  is given. Use at your own risk.                                     */
/*                                                                      */
/*  if you want to use any part of that code in your project,           */
/*  email Author at:                                                    */
/*                                                                      */
/*  maq@panoramix.ift.uni.wroc.pl                                       */
/*  http://panoramix.ift.uni.wroc.pl/~maq                               */
/************************************************************************/





#include "CHookeSpring.h"
#include "CVector3.h"


void CHookeSpring::doCalculateSpringForce(void)
{

	
	// let us calculate linear spring force
	// according to Hooke's elasticity theory


	CVector3	r12;			// actual distance vector
	float		r12d;			// actual distance vector length
	CVector3	v12;			// velocity vector difference		
	float		f;				// force value we are calculating
	CVector3	F;				// final force vector
	

	// calculate distance of spring start and end points

	r12		=	(_p1->_r - _p2->_r);			// distance vector
	r12d	=	r12.getLength();				// distance vector length


	// if r12d is not 0 (actually shouldn't happen never, but...)
	if(r12d != 0)
	{	

		// calculate velocity vector difference for damping term
		v12 = (_p1->_v - _p2->_v);	  
		
		// now we are able to calculate force value
		f = (r12d -  _restLength) * _ks + (v12 * r12) * _kd / r12d;

		// and finally vector force
		F = (r12 * (1.0f/r12d)) * f;

	}

	/*
	 *	Accumulate force
	 */

	_p1->doAccumulateForce(-1.0 * F);
	_p2->doAccumulateForce(1.0 * F);

}





void CHookeSpring::doApplyInverseDynamicsConstraint(void)
{
	CVector3	r12;			// actual distance vector
	float		r12d;			// actual distance vector length
	float		diff;			// distance vector - rest length
	
	// calculate distance of spring start and end points

	r12		=	(_p1->_r - _p2->_r);			// distance vector
	r12d	=	r12.getLength();				// distance vector length
	diff	=	fabs(r12d - _restLength);		// distance vector - rest length
	
	// if distance between points in larger than maximum distance then
	// apply constraint

	if(diff > _maximumDistance )
	{
		// use diff for calculation of distance minus _maximumDistance

		diff = diff - _maximumDistance;

		// norm r12 vector

		r12.doNormalize();

		// calculate vector which will be added and substituted from p1 and p2

		r12 = r12 * (diff / 2.0f);

		// finally apply constraints

		if(_p1->_mass)
		_p1->_r = _p1->_r - r12;
		if(_p2->_mass)
		_p2->_r = _p2->_r + r12;
	}
	
}
