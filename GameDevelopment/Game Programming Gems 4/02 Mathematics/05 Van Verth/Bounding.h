//===============================================================================
// @ Bounding.h
// 
// Collection of routines for finding bounding volumes for set of points
// ------------------------------------------------------------------------------
// Copyright (C) 2003  James M. Van Verth
//
// Change history:
//
// 01-Aug-03	JMV	First version
//
//===============================================================================

#pragma once

#ifndef __Bounding__h__
#define __Bounding__h__

//-------------------------------------------------------------------------------
//-- Forward declarations -------------------------------------------------------
//-------------------------------------------------------------------------------

struct Vector3;

//-------------------------------------------------------------------------------
//-- Function declarations ------------------------------------------------------
//-------------------------------------------------------------------------------

void ComputeBoundingBox( const Vector3* points, int nPoints, 
		Vector3& centroid, Vector3 basis[3],
		Vector3& min, Vector3& max );

void ComputeBoundingCylinder( const Vector3* points, int nPoints, 
		Vector3& centroid, Vector3& axis,
		float& min, float& max, float& radius );


#endif // __Bounding__h__