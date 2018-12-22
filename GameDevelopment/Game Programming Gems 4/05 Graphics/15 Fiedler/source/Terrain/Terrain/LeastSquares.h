// Least Squares fitting

#pragma once

#include "Mathematics/LinearAlgebra.h"

using namespace Mathematics;


/// Least Squares Sums class.
/// stores sums required for least squares plane calculations.

class LeastSquaresSums
{
public:

    float x,y,z;
    float xx,xy,xz;
    float yy,yz;
    float samples;

    void clear()
    {
        x = 0;
        y = 0;
        z = 0;
        xx = 0;
        xy = 0;
        xz = 0;
        yy = 0;
        yz = 0;
        samples = 0;
    }

    LeastSquaresSums& operator +=(LeastSquaresSums &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        xx += other.xx;
        xy += other.xy;
        xz += other.xz;
        yy += other.yy;
        yz += other.yz;
        samples += other.samples;
        return *this;
    }
};


/// calculate least squares sums.
/// simple and easy to read version, but slow because of extra multiplies per sample looking up heightfield sample inside vectorAtIndex etc

inline void calculateLeastSquaresSums_slow(Heightfield &heightfield, int x1, int y1, int x2, int y2, float rx, float ry, LeastSquaresSums &sums)
{
    sums.clear();

    for (int y=y1; y<=y2; y++)
    {
        for (int x=x1; x<=x2; x++)
        {
            Vector point = heightfield.vectorAtIndex(x,y);
            point.z = heightfield.heightAtIndex(x,y);

            sums.x += point.x;
            sums.y += point.y;
            sums.z += point.z;
            sums.xx += point.x * point.x;
            sums.xy += point.x * point.y;
            sums.xz += point.x * point.z;
            sums.yy += point.y * point.y;
            sums.yz += point.y * point.z;
            sums.samples ++;
        }                     
    }
}

/// calculate least squares sums.
/// faster version by reducing the per sample calculations

inline void calculateLeastSquaresSums(Heightfield &heightfield, int x1, int y1, int x2, int y2, float rx, float ry, LeastSquaresSums &sums)
{
    sums.clear();

    const int n = (x2-x1+1);
    const int n_squared = n*n;

    sums.samples = (float) n_squared;

    Vector point;
    point.y = ry;

    const float delta = heightfield.scale();
    const int step = heightfield.width() - n;

    int offset = heightfield.offsetAtIndex(x1,y1);

    for (int y=y1; y<=y2; y++)
    {
        point.x = rx;

        for (int x=x1; x<=x2; x++)
        {
            point.z = heightfield.heightAtOffset(offset);

            sums.x += point.x;
            sums.y += point.y;
            sums.z += point.z;
            sums.xx += point.x * point.x;
            sums.xy += point.x * point.y;
            sums.xz += point.x * point.z;
            sums.yy += point.y * point.y;
            sums.yz += point.y * point.z;

            point.x += delta;

            offset ++;
        }     

        point.y += delta;

        offset += step;
    }
}

/// calculate least squares sums.
/// optimized for special 1x1 case at leaf nodes.

inline void calculateLeastSquaresSums_1x1(Heightfield &heightfield, int ix, int iy, float x, float y, LeastSquaresSums &sums)
{
    // 0 1
    // 2 3

    const int width = heightfield.width();
    const int offset = heightfield.offsetAtIndex(ix,iy);

    const float h0 = heightfield.heightAtOffset(offset);
    const float h1 = heightfield.heightAtOffset(offset+1);
    const float h2 = heightfield.heightAtOffset(offset+width);
    const float h3 = heightfield.heightAtOffset(offset+width+1);

    // optimized special case - see LeastSquaresSums.nb for the derivation!

    const float x2 = x+x;
    const float x4 = x2+x2;
    const float y2 = y+y;
    const float y4 = y2+y2;

    sums.x = 2 + x4;
    sums.y = 2 + y4;
    sums.z = h0 + h1 + h2 + h3;
    sums.xx = 2 + x4*(1+x);
    sums.xy = (1 + x2)*(1 + y2);
    sums.xz = h1 + h3 + (h0 + h1 + h2 + h3)*x;
    sums.yy = 2 + y4*(1+y);
    sums.yz = h2 + h3 + (h0 + h1 + h2 + h3)*y;
    sums.samples = 4.0f;
}


/// Calculates the plane that best approximates the heightfield data set z=f(x,y) in the rectangular region (x1,y1) (x2,y2) inclusive.
/// Note that the output normal vector is not unit length, and has an implied z component of 1.

inline void leastSquaresPlane(const LeastSquaresSums &sums, float &normal_x, float &normal_y, float &center_x, float &center_y, float &center_z)
{
    // optimized solve for symmetric matrix from mathematica (see LeastSquaresPlane.nb)

    const float denominator = -2*sums.x*sums.xy*sums.y + sums.xx*sums.y*sums.y + sums.x*sums.x*sums.yy + sums.samples*(sums.xy*sums.xy - sums.xx*sums.yy);

    const float inverse_denominator = 1.0f / denominator;

    const float a = sums.xz*(-sums.y*sums.y + sums.samples*sums.yy) - sums.samples*sums.xy*sums.yz + sums.x*sums.y*sums.yz + (sums.xy*sums.y - sums.x*sums.yy)*sums.z;
    const float b = -sums.x*sums.x*sums.yz + sums.samples*(-sums.xy*sums.xz + sums.xx*sums.yz) - sums.xx*sums.y*sums.z + sums.x*(sums.xz*sums.y + sums.xy*sums.z);

    normal_x = a * inverse_denominator;
    normal_y = b * inverse_denominator;

    // note: normal_z = 1

    const float inverse_samples = 1.0f / sums.samples;

    center_x = sums.x * inverse_samples;
    center_y = sums.y * inverse_samples;
    center_z = sums.z * inverse_samples;
}

/// Calculate least squares plane.
/// Easy to understand but not optimized!

inline void leastSquaresPlane_slow(const LeastSquaresSums &sums, float &normal_x, float &normal_y, float &center_x, float &center_y, float &center_z)
{
    // solve Ax=b for 3x3 matrix

    float A[3][3] =
    {
        sums.xx, sums.xy, sums.x,
        sums.xy, sums.yy, sums.y,
        sums.x,  sums.y,  sums.samples
    };

    float b[3] =
    {
        sums.xz,
        sums.yz,
        sums.z
    };

    float x[3];

	LinearAlgebra::solveLinearSystem(A,b,x);

    normal_x = -x[0];
    normal_y = -x[1];

    // note: normal_z = 1

    const float inverse_samples = 1.0f / sums.samples;

    center_x = sums.x * inverse_samples;
    center_y = sums.y * inverse_samples;
    center_z = sums.z * inverse_samples;
}
