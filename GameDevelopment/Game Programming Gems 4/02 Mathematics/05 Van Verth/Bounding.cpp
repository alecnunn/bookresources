//===============================================================================
// @ Bounding.cpp
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

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include "Bounding.h"
#include "Vector3.h"
#include "Eigen.h"
#include <math.h>
#include <string.h>
#include <float.h>

//-------------------------------------------------------------------------------
//-- Prototypes -----------------------------------------------------------------
//-------------------------------------------------------------------------------

void CovarianceMatrix( const Vector3* points, int numPoints, Vector3& mean, float C[6] );


//-------------------------------------------------------------------------------
//-- Functions ------------------------------------------------------------------
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
// Compute bounding box for set of points
// Returns centroid, basis for bounding box, and min/max ranges 
//         (in terms of centroid/basis coordinate frame)
//-------------------------------------------------------------------------------
void ComputeBoundingBox( const Vector3* points, int nPoints, 
		Vector3& centroid, Vector3 basis[3],
		Vector3& min, Vector3& max )
{
    // compute covariance matrix
    float C[6];
    CovarianceMatrix( points, nPoints, centroid, C );

    // get principle axes
    GetRealSymmetricEigenvectors( C, basis[0], basis[1], basis[2] );

    min.Set(FLT_MAX, FLT_MAX, FLT_MAX);
    max.Set(FLT_MIN, FLT_MIN, FLT_MIN);

    // for each point do
    for ( int i = 0; i < nPoints; ++i )
    {
        Vector3 diff = points[i]-centroid;
        for (int j = 0; j < 3; ++j)
        {
            float length = diff.Dot(basis[j]);
            if (length > max[j])
            {
                max[j] = length;
            }
            else if (length < min[j])
            {
                min[j] = length;
            }
        }
    }
}


//-------------------------------------------------------------------------------
// Compute bounding cylinder for set of points
// Returns centroid, axis, min/max ranges (in terms of axis/centroid frame), 
//         and radius
//-------------------------------------------------------------------------------
void ComputeBoundingCylinder( const Vector3* points, int nPoints, 
		Vector3& centroid, Vector3& axis,
		float& min, float& max, float& radius )
{
    // compute covariance matrix
    float C[6];
    CovarianceMatrix( points, nPoints, centroid, C );

    // get main axis
    Vector3 v2, v3;
    GetRealSymmetricEigenvectors( C, axis, v2, v3 );

    min = FLT_MAX;
    max = FLT_MIN;
    float maxDistSq = 0.0f;

    // for each point do
    for ( int i = 0; i < nPoints; ++i )
    {
        // compute min, max along axis
        Vector3 diff = points[i]-centroid;
        float length = diff.Dot(axis);
        if (length > max)
        {
            max = length;
        }
        else if (length < min)
        {
            min = length;
        }

        // compute radius
        Vector3 proj = (diff.Dot(axis))*axis;
        Vector3 distv = diff - proj;

        float distSq = distv.Dot(distv);
        if (distSq > maxDistSq)
          maxDistSq = distSq;
    }

    radius = sqrtf(maxDistSq);
}


//-------------------------------------------------------------------------------
// Compute covariance matrix for set of points
// Returns centroid and unique values of matrix
// Does not normalize matrix by 1/(n-1) (not needed)
//-------------------------------------------------------------------------------
void 
CovarianceMatrix( const Vector3* points, int numPoints, Vector3& mean, float C[6] )
{
    int i;

    // compute mean
    mean = points[0];
    for (i = 1; i < numPoints; ++i)
    {
        mean += points[i];	
    }
    float recip = 1.0f/numPoints;
    mean *= recip;

    // compute each element of matrix
    memset( C, 0, sizeof(float)*6 );
    for (i = 0; i < numPoints; ++i)
    {
        Vector3 diff = points[i]-mean;
        C[0] += diff.x*diff.x;
        C[1] += diff.x*diff.y;
        C[2] += diff.x*diff.z;
        C[3] += diff.y*diff.y;
        C[4] += diff.y*diff.z;
        C[5] += diff.z*diff.z;
    }	
}
