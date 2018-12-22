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



#ifndef __I_CMATERIALPOINT_
#define __I_CMATERIALPOINT_


#include "CVector3.h"

/**
 * @short Class of Material Point object. A Soft Body will be built of it. 
 *
 * @detailed Contains informations about mass, position and velocity of material
 * point in three dimensional space. It will keep also information about normal
 * to point vector. Force accumulator has been included too. We keep here also
 * vectors of small changes in position (_dr) and velocity (_dv) which will be 
 * used by integration procedures.
 *
 * @see CVector3
 * 
 * @author Maciej Matyka
 * http://panoramix.ift.uni.wroc.pl/~maq
 *
 */


class CMaterialPoint 
{

public:
	
	CMaterialPoint()
	{}

	CMaterialPoint(float mass, CVector3 r, CVector3 v) 
		:	_mass(mass), _r(r), _v(v), _f(0)
	{

		// calculate 1/mass speed up computation. set it to 0 if you want to fix material point
		// in space (infinite mass)

		if(mass)
			_oneOverMass=1.0f/_mass;
		else
			_oneOverMass=0;				// point fixed in space (infinite mass)
	}



	/**
	 * Force Accumulation - simple adding proc.	
	 */

	void inline doAccumulateForce(CVector3 const &force)
	{
		_f = _f + force;
	}

	/**
	 * Clear forces 	
	 */

	void inline doClearForce(void)
	{
		_f = CVector3(0,0,0);
	}



public:

	float _mass;
	float _oneOverMass;
	
	int		_i;			// point index

	CVector3 _r;		// position
	CVector3 _v;		// velocity
	CVector3 _f;		// force

	CVector3 _n;		// normal to a point

	CVector3 _dr;		// almost infinite small change of position (for integration procedures)
	CVector3 _dv;		// almost infinite small change of velocity (for integration procedures)
};




#endif