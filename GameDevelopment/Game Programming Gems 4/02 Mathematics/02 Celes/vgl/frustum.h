// frustum.h
// Tecgraf/PUC-Rio
// celes@tecgraf.puc-rio.br
// Jul 2003

/* This code is free software; you can redistribute it and/or modify it. 
** The software provided hereunder is on an "as is" basis, and 
** the author has no obligation to provide maintenance, support, updates,
** enhancements, or modifications. 
*/

#ifndef VGL_FRUSTUM_H
#define VGL_FRUSTUM_H

#include "alg/plane.h"
#include "alg/frustum.h"

//* Frustum plane ids
enum {
 VGL_PLANE_LEFT = 0,
 VGL_PLANE_RIGHT,
 VGL_PLANE_BOTTOM,
 VGL_PLANE_TOP,
 VGL_PLANE_NEAR,
 VGL_PLANE_FAR
};

class VglFrustum : public AlgFrustum
{
public:

  // Constructors
		// Based on modelview and projection matrices.
 VglFrustum (float* Mmv, float* Mp);
		// Based on modelview-projection matrices.
 VglFrustum (float* Mmvp);
		// Based on current modelview and projection matrices.
		// The matrices are obtained via glGet.
 VglFrustum ();

 // Update object matrices
 void Update (float* Mmv, float* Mp);
 void Update (float* Mmvp);
 void Update ();

 // Gets frustum plane in object space
 AlgPlane GetPlane (int id);

 // Gets the current eye position in object space
 AlgVector GetEyePos ();

 // Gets the current view direction
 AlgVector GetViewDir ();

 // Gets the current view up direction
 AlgVector GetViewUp ();

 // Gets the current near plane distance
 float GetNearDist ();

 // Gets the current far plane distance
 float GetFarDist ();

 // Gets the current angle of the vertical field of view
 float GetFovy ();

 // Gets the current angle of the horizontal field of view
 float GetFovx ();

private:
	void InitCanonicalPlanes ();
};

#endif
