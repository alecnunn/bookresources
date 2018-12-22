// Linear Algebra routines

#pragma once

#include "Float.h"
#include "Vector.h"


namespace Mathematics
{
    /// Linear algebra routines.

    namespace LinearAlgebra
    {
        /// Solves a simple 3x3 linear system.
        /// Note that this method assumes the linear system is consistent!

        static inline void solveLinearSystem(const float A[3][3], const float b[3], float x[3])
        {
            float inverse[3][3];
            inverse[0][0] = A[1][1]*A[2][2]-A[1][2]*A[2][1];
            inverse[0][1] = A[0][2]*A[2][1]-A[0][1]*A[2][2];
            inverse[0][2] = A[0][1]*A[1][2]-A[0][2]*A[1][1];
            inverse[1][0] = A[1][2]*A[2][0]-A[1][0]*A[2][2];
            inverse[1][1] = A[0][0]*A[2][2]-A[0][2]*A[2][0];
            inverse[1][2] = A[0][2]*A[1][0]-A[0][0]*A[1][2];
            inverse[2][0] = A[1][0]*A[2][1]-A[1][1]*A[2][0];
            inverse[2][1] = A[0][1]*A[2][0]-A[0][0]*A[2][1];
            inverse[2][2] = A[0][0]*A[1][1]-A[0][1]*A[1][0];
            
            const float determinant = A[0][0]*inverse[0][0] + A[0][1]*inverse[1][0] + A[0][2]*inverse[2][0];
            
            const float inverse_determinant = 1.0f / determinant;

            for (int row=0; row<3; row++)
            {
                for (int col=0; col<3; col++)
                    inverse[row][col] *= inverse_determinant;
            }

            x[0] = inverse[0][0]*b[0]+inverse[0][1]*b[1]+inverse[0][2]*b[2];
            x[1] = inverse[1][0]*b[0]+inverse[1][1]*b[1]+inverse[1][2]*b[2];
            x[2] = inverse[2][0]*b[0]+inverse[2][1]*b[1]+inverse[2][2]*b[2];
        }
    }
}
