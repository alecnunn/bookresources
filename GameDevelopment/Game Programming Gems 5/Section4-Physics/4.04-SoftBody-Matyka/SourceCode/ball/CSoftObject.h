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


#ifndef __I___CSOFTOBJECT_H_
#define __I___CSOFTOBJECT_H_


#include <vector>
#include <algorithm>
using namespace std;

#include "CMaterialPoint.h"
#include "CHookeSpring.h"
#include "CFileParser.h"
#include "CTriangleFace.h"
#include "iFileTypes.h"

#include "Drawable.h"


/**
 * @short Class of Soft Body Object.
 *
 * @detailed It's public class of Drawable property. The most important class in whole
 * program for soft bodies simulation. It keeps all needed informations and procedures
 * to load, keep and share objects points/faces/springs. 
 * 
 * @author Maciej Matyka
 * http://panoramix.ift.uni.wroc.pl/~maq
 *
 */



class CSoftObject : public Drawable
{

public:

	CSoftObject(CVector3 translate, float rescale, float mass, float ks, float kd, char *ascobject, int id, float Pressure)
		: _pressure(Pressure), _id(id), _gravity(CVector3(0,0,0))
	{

		/*
		 *	Read object from given file
		 */

		doReadAsc(translate,rescale,mass,ks,kd,ascobject);		

	}


	~CSoftObject()
	{}

	/**
	 * Get Pressure                                                                     
	 */

		inline float getPressure()
		{
			return(_pressure);
		}

	/**
	 * Set Pressure                                                                     
	 */

		inline void setPressure(float pressure)
		{
			_pressure = pressure;
		}

	/**
	 * Set Gravity                                                                     
	 */

		inline void setGravity(CVector3 Gravity)
		{
			_gravity = Gravity;
		}

	/**
	 * Read points, faces and create springs from .asc 3d studio ascii file                                                                     
	 */
	
		void doReadAsc(CVector3 translate, float rescale, float mass, float ks, float kd, char *ascobject);


	/**
	 * Return Points Number
	 */

		inline int iNump(void) const
		{ return _vPoints.size();}

	/**
	 * Return Faces Number
	 */

		inline int iNumf(void) const
		{ return _vFaces.size();}

	/**
	 * Return Springs Number
	 */

		inline int iNums(void) const
		{ return _vSprings.size();}

	/**
	 * Object GL Visualization                                                                     
	 */

		void glVisualize(void) const;
	

	/**
	 * Calculate Volume of the body                                                                     
	 */

		void doCalculateVolume(void);

	/**
	 * Calculate Forces for Soft Body object                                                                   
	 */

		void doCalculateForces();

	/**
	 * Integrate equations of motion                                                                     
	 */

		void doIntegration(int method, float dt);


public:

	int _id;

	CVector3 _gravity;						// object know about external gravity!

	float _volume;						// volume of the ball
	float _oneovervolume;				// small speeding up
	float _pressure;					// pressure of the ball

	vector<CMaterialPoint>	_vPointsCopy;	// points copy will be kept here (i.e. Heun integrator)
	vector<CMaterialPoint>	_vPoints;		// points are keeped here
	vector<CHookeSpring>	_vSprings;		// springs are keeped here
	vector<CTriangleFace>	_vFaces;		// object triangle faces 


};

#endif





















