// Quadtree Node class

#pragma once

#include "LeastSquares.h"
#include "Mathematics/Quad.h"
#include "Mathematics/Slab.h"
#include "Mathematics/Float.h"
#include "Mathematics/Vector.h"


/// Quadtree Node class.
/// efficiently packed into 16 bytes!

class QuadtreeNode
{
public:

    /// Initialize node bounding heightfield data over the rectangular area (x1,y1) (x2,y2).
    /// note: You must calculate the least squares sums yourself and pass them in!

    void initialize(const LeastSquaresSums &sums, Heightfield &heightfield, int x1, int y1, int x2, int y2, Vector &center)
    {
        // calculate least squares plane

        leastSquaresPlane(sums, normalX, normalY, center.x, center.y, center.z);

        d = center.x*normalX + center.y*normalY + center.z;

        // calculate error

        float lowest = FLT_MAX;
        float highest = -FLT_MAX;

        for (int y=y1; y<=y2; y++)
        {
            for (int x=x1; x<=x2; x++)
            {
                Vector point = heightfield.vectorAtIndex(x,y);
                point.z = heightfield.heightAtIndex(x,y);

                const float e = (point.x*normalX + point.y*normalY + point.z);

                if (e<lowest) lowest = e;
                if (e>highest) highest = e;
            }                     
		}

        error = Float::maximum(highest-d, -(lowest-d));
	}

    /// initialize least squares plane normal, center and d from sums without calculating error.
    /// error calculation must be done manually using pushOut and calculateError functions!
    /// note: used in bottom up tree build

    void initialize(const LeastSquaresSums &sums, Vector &center)
    {
        // calculate least squares plane

        leastSquaresPlane(sums, normalX, normalY, center.x, center.y, center.z);

        d = center.x*normalX + center.y*normalY + center.z;

        // note: you must now call pushOut, then call calculateError passing in the minimum and maximum d's from pushOut
	}

    /// Push minimum and maximum plane d's out to bound this planes of this node
    /// Derived from the "quad" method, look there to understand the mathematics.

    void pushOut(float x1, float y1, float x2, float y2, float planeNormalX, float planeNormalY, float &planeMaximumD, float &planeMinimumD) const
    {
        // fast push out

        const float x1_nx = x1 * normalX;
        const float y1_ny = y1 * normalY;
        const float x2_nx = x2 * normalX;
        const float y2_ny = y2 * normalY;

        const float x1_pnx = x1 * planeNormalX;
        const float y1_pny = y1 * planeNormalY;
        const float x2_pnx = x2 * planeNormalX;
        const float y2_pny = y2 * planeNormalY;

        // 0 1
        // 3 2
                
        const float center_d0 = x1_pnx + y1_pny - x1_nx - y1_ny + d;
        const float center_d1 = x2_pnx + y1_pny - x2_nx - y1_ny + d;
        const float center_d2 = x2_pnx + y2_pny - x2_nx - y2_ny + d;
        const float center_d3 = x1_pnx + y2_pny - x1_nx - y2_ny + d;

        const float maximum_d0 = center_d0 + error;
        const float maximum_d1 = center_d1 + error;
        const float maximum_d2 = center_d2 + error;
        const float maximum_d3 = center_d3 + error;

        if (maximum_d0>planeMaximumD) planeMaximumD = maximum_d0;
        if (maximum_d1>planeMaximumD) planeMaximumD = maximum_d1;
        if (maximum_d2>planeMaximumD) planeMaximumD = maximum_d2;
        if (maximum_d3>planeMaximumD) planeMaximumD = maximum_d3;

        const float minimum_d0 = center_d0 - error;
        const float minimum_d1 = center_d1 - error;
        const float minimum_d2 = center_d2 - error;
        const float minimum_d3 = center_d3 - error;

        if (minimum_d0<planeMinimumD) planeMinimumD = minimum_d0;
        if (minimum_d1<planeMinimumD) planeMinimumD = minimum_d1;
        if (minimum_d2<planeMinimumD) planeMinimumD = minimum_d2;
        if (minimum_d3<planeMinimumD) planeMinimumD = minimum_d3;
    }                           

    /// Calculate error given minimum and maximum plane d of sample points

    void calculateError(float maximumD, float minimumD)
    {
        const float maximumE = maximumD - d;
        const float minimumE = d - minimumD;

        assert(maximumE>=0);
        assert(minimumE>=0);

        error = Float::maximum(maximumE, minimumE);
    }

    /// node planes

    enum PlaneType
    {
        CENTER,             ///< the center plane determined by least squares
        MINIMUM,            ///< the minimum plane below all node samples
        MAXIMUM             ///< the maximum plane above all node samples
    };

    /// Build quad from node

    Quad quad(float x1, float y1, float x2, float y2, PlaneType plane = CENTER) const
    {
        // calculate quad plane d

        float quad_d = d;
        if (plane==MINIMUM) quad_d -= error;
        else if (plane==MAXIMUM) quad_d += error;

        // 0 1
        // 3 2

        Vector vertex[4];

        const float x1_nx = x1 * normalX;
        const float y1_ny = y1 * normalY;
        const float x2_nx = x2 * normalX;
        const float y2_ny = y2 * normalY;

        vertex[0].x = x1;
        vertex[0].y = y1;
        vertex[0].z = quad_d - x1_nx - y1_ny;

        vertex[1].x = x2;
        vertex[1].y = y1;
        vertex[1].z = quad_d - x2_nx - y1_ny;

        vertex[2].x = x2;
        vertex[2].y = y2;
        vertex[2].z = quad_d - x2_nx - y2_ny;

        vertex[3].x = x1;
        vertex[3].y = y2;
        vertex[3].z = quad_d - x1_nx - y2_ny;

        const Vector normal(normalX, normalY, 1);

        return Quad(vertex[0], vertex[1], vertex[2], vertex[3], normal);
    }

    /// Build slab from node

    Slab slab(float x1, float y1, float x2, float y2) const
    {
        const float maximum = d + error;
        const float minimum = d - error;

		return Slab( Vector(normalX, normalY, 1), minimum, maximum, x1, y1, x2, y2);
    }

    /// Get x component of normal.
	/// z component of normal is always 1.

	float getNormalX() const
	{                 
		return normalX;
	}

	/// Get y component of normal.
	/// z component of normal is always 1.

	float getNormalY() const
	{
		return normalY;
	}

    /// Get the node center plane d.

    float getCenterD() const
    {
        return d;
    }

    /// Get the node error.
    /// Node error corresponds to the maximum perpendicular displacement of any sample point
    /// from the least squares plane relative to the NON-UNIT normal because we are cheap (!) 

    float getError() const
    {
        return error;
    }

private:

    float normalX;          ///< plane normal x
    float normalY;          ///< plane normal y (normal z=1)
    float error;            ///< minimum and maximum plane error along normal +/- about plane d
    float d;                ///< plane d relative to non-unit length normal
};
