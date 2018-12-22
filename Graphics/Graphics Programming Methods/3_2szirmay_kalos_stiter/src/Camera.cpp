#include <stdio.h>

#include "Camera.hpp"

#define INITIAL_SCREEN_SIZE_X	600
#define INITIAL_SCREEN_SIZE_Y	400

#define	DEFAULT_CAMERA_NEARCLIP 0.01f
#define	DEFAULT_CAMERA_FARCLIP	1500.0f

Camera::Camera () 
{
	eyep.Set(0.0, 1.000, 5.0);
	lookp.Set(0.0, 0.0, 0.0);
	updir.Set(0., 1.0, 0.0);
	fov		= 22.5;

	nearClip	= DEFAULT_CAMERA_NEARCLIP; 
	farClip		= DEFAULT_CAMERA_FARCLIP;

	hres		= INITIAL_SCREEN_SIZE_X;
	vres		= INITIAL_SCREEN_SIZE_Y;

	CompleteCamera ();
}

void Camera::CompleteCamera ()
{
	// set up Z
	Z=lookp - eyep;

	// distance from virtual camera position to focus point
	viewdist = Z.Norm ();
	if (viewdist < EPSILON) {
		printf("Camera error: eyepoint and look-point coincide\n");
		return;
	}
	Z *= 1.0f / viewdist;
	
	// set up X   Camera->X is a direction pointing to the right in the window 
	X = Z && updir;		//crossproduct

	float lengthX = X.Norm ();
	if (lengthX < EPSILON) {
		printf("Camera error: up-direction and viewing direction coincide\n");
		return;
	}
	X *= 1.0f / lengthX;
	
	// set up Y
	Y = Z && X;		// CrossProduct 
	Y.Normalize ();
	
	// compute horizontal and vertical field of view angle from the specified one
	// if the vertical resolution is smaller, it is the specified fov = 45, the other is greater than 45
	if (hres < vres) {
		hfov = fov; 
		vfov = atanf(tanf(fov * M_PI/180.0f) * (float)vres/(float)hres) * 180.0f/M_PI;
	} else {
		vfov = fov; 
		hfov = atanf(tanf(fov * M_PI/180.0f) * (float)hres/(float)vres) * 180.0f/M_PI;
	}
	
	/* Compute some extra frequently used quantities */
	tanFovH = tanf(hfov * M_PI / 180.0f);
	tanFovV = tanf(vfov * M_PI / 180.0f);
	pixh = 2.0f * tanFovH / (float)(hres);
	pixv = 2.0f * tanFovV / (float)(vres);

}