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





#include "CSoftObject.h"
#include <GL/glut.h> 


void CSoftObject::glVisualize(void) const
{
	int i;

	/**
	 * Draw my Box                                                                     
	 */

	const float xmin = -4.5;
	const float xmax = 4.5;
	const float ymin = -4.5;
	const float ymax = 4.5;
	const float zmin = -4.5;
	const float zmax = 4.5;

	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
//		glColor3f(0.44,0.53,0.62);
//
//    	glVertex3f(xmin,ymax,zmin);
//		glVertex3f(xmax,ymax,zmin);
//		glVertex3f(xmax,ymax,zmax);
//		glVertex3f(xmin,ymax,zmax);
//
//		glColor3f(0.14,0.23,0.48);
//
//		glVertex3f(xmin,ymin,zmin);
//		glVertex3f(xmax,ymin,zmin);
//		glVertex3f(xmax,ymin,zmax);
//		glVertex3f(xmin,ymin,zmax);
//
//		glColor3f(0.44,0.43,0.62);
//
//		glVertex3f(xmin,ymin,zmin);
//		glVertex3f(xmin,ymax,zmin);
//		glVertex3f(xmin,ymax,zmax);
//		glVertex3f(xmin,ymin,zmax);
//
//		glColor3f(0.44,0.43,0.52);
//
//		glVertex3f(xmax,ymin,zmin);
//		glVertex3f(xmax,ymax,zmin);
//		glVertex3f(xmax,ymax,zmax);
//		glVertex3f(xmax,ymin,zmax);


	glEnd();

	
	/**
	 *	Draw Faces
	 */

	glEnable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);
		
		glColor3f(0,0,1);

		for(i=0; i<_vFaces.size() ; i++)
		{
			glNormal3f(_vFaces[i]._n.x,_vFaces[i]._n.y,_vFaces[i]._n.z);
//			glNormal3f(_vFaces[i]._p1->_n.x,_vFaces[i]._p1->_n.y,_vFaces[i]._p1->_n.z);
			glVertex3f(_vFaces[i]._p1->_r.x,_vFaces[i]._p1->_r.y,_vFaces[i]._p1->_r.z);

//			glNormal3f(_vFaces[i]._p2->_n.x,_vFaces[i]._p2->_n.y,_vFaces[i]._p2->_n.z);
			glVertex3f(_vFaces[i]._p2->_r.x,_vFaces[i]._p2->_r.y,_vFaces[i]._p2->_r.z);

//			glNormal3f(_vFaces[i]._p3->_n.x,_vFaces[i]._p3->_n.y,_vFaces[i]._p3->_n.z);
			glVertex3f(_vFaces[i]._p3->_r.x,_vFaces[i]._p3->_r.y,_vFaces[i]._p3->_r.z);
		}

	glEnd();


	/**
	 *	Draw Springs
	 */

	glDisable(GL_LIGHTING);
	glColor3f(1,1,1);
	glBegin(GL_LINES);
		
		/*
		 *	Only last spring
		 */
	
	    for(i=_vSprings.size()-1; i<_vSprings.size() ; i++)
		if(_vSprings[i]._ks==20000)							// mouse spring?
		{
			glVertex3f(_vSprings[i]._p1->_r.x,_vSprings[i]._p1->_r.y,_vSprings[i]._p1->_r.z);
			glVertex3f(_vSprings[i]._p2->_r.x,_vSprings[i]._p2->_r.y,_vSprings[i]._p2->_r.z);
		}

	glEnd();

	glEnable(GL_LIGHTING);

	
	/**
	 *	Draw Points
	 */

//	glColor3f(1,0,0);
//	glBegin(GL_POINTS);
//		
//	    for(i=0; i<_vPoints.size() ; i++)
//		{
//			glVertex3f(_vPoints[i]._r.x,_vPoints[i]._r.y,_vPoints[i]._r.z);
//		}
//
//	glEnd();


}



void CSoftObject::doIntegration(int method, float dt)
{

	int i;
	const float semifriction=0.6;
	

	switch(method)
	{
		case 0:			// Newton


			
				/*
				 * loop over all points and iuntegrate them
				 */

				for(i = 0; i < _vPoints.size(); i++)
				{
					_vPoints[i]._dv = _vPoints[i]._f * _vPoints[i]._oneOverMass * dt;
					_vPoints[i]._v = _vPoints[i]._v + _vPoints[i]._dv;
					_vPoints[i]._dr = _vPoints[i]._v * dt;
					_vPoints[i]._r = _vPoints[i]._r + _vPoints[i]._dr;
				}
				
				break;


		case 1:			// Heun predictor -> corrector

				/*
				 *	Copy Points->PointsCopy
				 */

				_vPointsCopy = _vPoints;

				/*
				 *	Make a Newton integration
				 */
				
				doIntegration(0, dt);

				
				/*
				 *	Calculate forces
				 */

				doCalculateForces();

				/*
				 *	Correction step, first copy back positions and velocities
				 * (forces are as calculated by prediction step)
				 */

				for(i = 0; i < _vPoints.size(); i++)
				{
					_vPoints[i]._v = _vPointsCopy[i]._v;
					_vPoints[i]._r = _vPointsCopy[i]._r;
				}


				/*
				 *	Then do correction step by integration with central average (Heun)
				 */


	
				for(i = 0; i < _vPoints.size(); i++)
				{
					_vPoints[i]._v = _vPoints[i]._v + 
									(dt/2.0f) * (_vPoints[i]._f + _vPointsCopy[i]._f) * _vPoints[i]._oneOverMass;

					_vPoints[i]._r = _vPoints[i]._r + _vPoints[i]._v * dt;

					/*
					 *	Simple (and bad - don't do that like that!) collision detection with the ground
					 */


					if(_vPoints[i]._r.y < - 4.5)
					{
						_vPoints[i]._r.y = -4.5;
						_vPoints[i]._v = _vPoints[i]._v * (-semifriction);
					}

					if(_vPoints[i]._r.y >   4.5)
					{
						_vPoints[i]._r.y = 4.5;
						_vPoints[i]._v = _vPoints[i]._v * (-semifriction);
					}

					if(_vPoints[i]._r.x < - 4.5)
					{
						_vPoints[i]._r.x = -4.5;
						_vPoints[i]._v.x = _vPoints[i]._v.x * (-semifriction);
					}

					if(_vPoints[i]._r.x >  4.5)
					{
						_vPoints[i]._r.x = 4.5;
						_vPoints[i]._v.x = _vPoints[i]._v.x * (-semifriction);
					}

					if(_vPoints[i]._r.z < - 4.5)
					{
						_vPoints[i]._r.z = -4.5;
						_vPoints[i]._v.z = _vPoints[i]._v.z * (-semifriction);
					}

					if(_vPoints[i]._r.z >  4.5)
					{
						_vPoints[i]._r.z = 4.5;
						_vPoints[i]._v.z = _vPoints[i]._v.z * (-semifriction);
					}

				}


			break;


		default:
			break;
	};



//	for(i = 0; i < _vSprings.size(); i++)
//		_vSprings[i].doApplyInverseDynamicsConstraint();

}



void CSoftObject::doCalculateVolume(void)
{

	/*
	 *	Clear Actual Volume
	 */

	_volume = 0;

	/*
	 *	Loop over surface and do a sum (Gauss theory) with assumption
	 * of constants vector field F, where dF/dx=1,dF/dy=0 and dF/dz=0, it gives...
	 * volume.
	 */

	int i;

	for(i=0 ; i< _vFaces.size() ; i++)
	{
		_volume +=	(	_vFaces[i]._p1->_r.x +
						_vFaces[i]._p2->_r.x +
						_vFaces[i]._p3->_r.x) * 0.333 * _vFaces[i]._n.x * _vFaces[i]._Field;
	}

	// calculate one over volume too

	_oneovervolume = 1.0f / _volume;
}



void CSoftObject::doCalculateForces()
{
	int i;


	/*
	 *	Clear all forces
	 */

	for(i = 0; i < _vPoints.size(); i++)
		_vPoints[i].doClearForce();


	/*
	 *	First - we accumulate gravity force for all points of the body
	 *  F = m * g
	 */

	for(i = 0; i < _vPoints.size(); i++)
	{
		_vPoints[i].doAccumulateForce(_vPoints[i]._mass * _gravity);
	}

	/*
	 *	Calculate and accumulate spring forces
	 */

	for(i = 0; i < _vSprings.size(); i++)
			_vSprings[i].doCalculateSpringForce();


	/*
	 *	Calculate normal vectors and fields of all faces triangles
	 */

	for(i = 0; i < _vFaces.size(); i++)
		_vFaces[i].doCalculateNormalAndField();

	/*
	 *	Calculate body volume
	 */

	doCalculateVolume();
	
	/*
	 *	Calculate Pressure Acting on faces of the body
	 */

	for(i = 0; i < _vFaces.size(); i++)
				_vFaces[i]._Force  = _vFaces[i]._n * _vFaces[i]._Field * _pressure * _oneovervolume;

	/*
	 *	Now distribute faces forces from faces to points
	 */

	CVector3 Force13;

	for(i = 0; i < _vFaces.size(); i++)
	{
		Force13 = _vFaces[i]._Force * 0.3333;

		_vFaces[i]._p1->doAccumulateForce(Force13);
		_vFaces[i]._p2->doAccumulateForce(Force13);
		_vFaces[i]._p3->doAccumulateForce(Force13);
	}

	/*
	 *	It's finished :)
	 */

}


void CSoftObject::doReadAsc(CVector3 translate, float rescale, float mass, float ks, float kd, char *ascobject)
{

	/*
	 *	Parse points and springs from a given file
	 */

		CFileParser objectParser(ascobject,F_3DSASC);
		
		objectParser.parsePoints();
		objectParser.parseFaces();


	/*
	 *	Copy parsed points into soft body structures
	 */

		int i;

		float x,y,z;	// point position

		for(i=0; i < objectParser.iNump; i++)
		{
			x=objectParser.fPoints[i*3+0];
			y=objectParser.fPoints[i*3+1];
			z=objectParser.fPoints[i*3+2];

			// put the point into my table

			_vPoints.push_back( CMaterialPoint(mass,CVector3(x,y,z)*rescale + translate, CVector3(0,0,0)) );
		}

	/*
	 *	Assign indexes to the points
	 */

		for(i=0; i < objectParser.iNump; i++)
		{
			_vPoints[i]._i = i;			// index of the point

		}



	/*
	 *	Copy parsed faces into soft body structures
	 */

		CTriangleFace tempFace;

		for(i=0; i < objectParser.iNumf; i++)
		{
			// get face from parser

			tempFace = CTriangleFace(	&_vPoints[objectParser.fFaces[i*3+0]],
										&_vPoints[objectParser.fFaces[i*3+1]],
										&_vPoints[objectParser.fFaces[i*3+2]]
									);
			
			// put the face into my table

			_vFaces.push_back( tempFace );
		}

	/*
	 *	Create spring using faces readed from the file object
	 */


		CMaterialPoint *p1,*p2,*p3;
		CHookeSpring tempSpring1;
		CHookeSpring tempSpring2;
		CHookeSpring tempSpring3;

		for(i=0;i< iNumf();i++)
		{
			p1 = _vFaces[i]._p1;				
			p2 = _vFaces[i]._p2;
			p3 = _vFaces[i]._p3;

			// create temporary springs

			tempSpring1 = CHookeSpring(p1,p2,ks,kd,0,100);
			tempSpring2 = CHookeSpring(p2,p3,ks,kd,0,100);
			tempSpring3 = CHookeSpring(p3,p1,ks,kd,0,100);

			// check if this spring1 exist (one spring per two faces in .asc objects!)
			// if does not exist - put it into vFaces vector

			if(find(_vSprings.begin(),_vSprings.end(),tempSpring1) == _vSprings.end())
				_vSprings.push_back(tempSpring1);		

			// check if this spring2 exist (one spring per two faces in .asc objects!)
			// if does not exist - put it into vFaces vector
			
			if(find(_vSprings.begin(),_vSprings.end(),tempSpring2) == _vSprings.end())
				_vSprings.push_back(tempSpring2);		

			// check if this spring3 exist (one spring per two faces in .asc objects!)
			// if does not exist - put it into vFaces vector
			
			if(find(_vSprings.begin(),_vSprings.end(),tempSpring3) == _vSprings.end())
				_vSprings.push_back(tempSpring3);		
		}


}





