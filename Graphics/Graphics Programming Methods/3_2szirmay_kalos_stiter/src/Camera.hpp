#pragma once

#include "World.hpp"

//! Implements a camera and stores its parameter 
/*! The camera is defined by its position, focus point, clipping plane 
and some other parameters.*/
class Camera {
public:
	//! virtual camera position in 3D space
	Vector	eyep;

	//! focus point of camera
	Vector	lookp;

	//! direction pointing upward
	Vector	updir;		

	//! distance from eyepoint to focus point
	float	viewdist;		

	//! half of the field of view, horizontal and vertical, in degrees.
	float	fov, hfov, vfov;

	//! near and far clipping plane distance
	float	nearClip, farClip;	

	//! horizontal and vertical resolution
	long	hres, vres;		
		
	//! eye coordinate system (right-hand-orientation): X=right
	Vector	X;

	//! eye coordinate system (right-hand-orientation): Y=down
	Vector	Y;

	//! eye coordinate system (right-hand-orientation): Z=viewing direction
	Vector	Z;

	//! Width and height of a pixel
	float	pixh, pixv;

	//! Precomputed values (tangens of hfov and vfov) 
	float	tanFovH, tanFovV;	

public:
	Camera ();
	void CompleteCamera ();

};