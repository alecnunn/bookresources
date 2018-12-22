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



#ifndef __CHOOKE_SPRING_
#define __CHOOKE_SPRING_


#include "CMaterialPoint.h"


/**
 * @short Class of linear Hooke Spring.
 *
 * @detailed It points that two material points interact between. Keeps information about
 * rest distance between points (where no force exists). ks and kd constants are typical spring
 * force elasticity and damping factors.
 *
 *
 * @see CVector3
 * @see CMaterialPoint
 *
 * @author Maciej Matyka
 * http://panoramix.ift.uni.wroc.pl/~maq
 *
 */


class CHookeSpring
{

public:

	CHookeSpring(CMaterialPoint *p1, CMaterialPoint *p2, float ks, float kd, float restLength, float possibleChange) 
		: _p1(p1), _p2(p2), _ks(ks), _kd(kd), _restLength(restLength), _possibleChange(possibleChange) 
	  {

		// user did not define rest length
		// so set actual length as rest length

		if(!_restLength)
			_restLength = (_p1->_r - _p2->_r).getLength();


		// calculate maximum change value for inverse dynamics constraints

		_maximumDistance = float( ( _possibleChange * _restLength ) / 100.0f );


	  }

	  
	  CHookeSpring()
	  {}


	  /**
	   * Calculate spring force acting on a first point (actually we will use it for both points, where
	   * Fs1 = -Fs2, there Fs1 will be returned.
	   */

	  void doCalculateSpringForce(void);

	  /**
	   * Apply Inverse Dynamics Constraints to a spring                                                                     
	   */

	  void doApplyInverseDynamicsConstraint(void);

	  /**
	   * == operator needed for find STL algorithm                                                                     
	   */

	  bool operator==(CHookeSpring const &s2)
	  {
			if(_p1->_r == s2._p1->_r)
			if(_p2->_r == s2._p2->_r)
				return true;

			if(_p1->_r == s2._p2->_r)
			if(_p2->_r == s2._p1->_r)
				return true;

			return false;
	  }

public:

	CMaterialPoint *_p1;					// start point of a spring
	CMaterialPoint *_p2;					// end point of a spring
	
	float			_ks;					// spring constant
	float			_kd;					// damping factor
	float			_restLength;			// length where Fs = 0 (Fs - Hooke's Force)

	float			_possibleChange;		// percentage value of possible spring length change
											// used for inverse dynamics constraints calculation
	float			_maximumDistance;		// maximum distance between points (Inverse Dynamics Constraint)
};


#endif