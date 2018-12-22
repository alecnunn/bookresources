//===============================================================================
// @ Eigen.cpp
// 
// Collection of routines for solving eigenvectors of symmetric 3x3 matrices
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

#include "Eigen.h"
#include "Vector3.h"
#include <math.h>

//-------------------------------------------------------------------------------
//-- Prototypes -----------------------------------------------------------------
//-------------------------------------------------------------------------------

void CalcSymmetricEigenvector( float A[6], float l, Vector3& v1 );
void CalcSymmetricEigenPair( float A[6], float l, Vector3& v1, Vector3& v2 );

//-------------------------------------------------------------------------------
//-- Functions ------------------------------------------------------------------
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
// Given symmetric matrix in A, returns three eigenvectors in order of 
// decreasing eigenvalue
//-------------------------------------------------------------------------------
void 
GetRealSymmetricEigenvectors( float A[6], Vector3& v1, Vector3& v2, Vector3& v3 )
{
	// compute coefficients for cubic equation
    float c2 = A[0] + A[3] + A[5];
    float a12sq = A[1]*A[1];
    float a13sq = A[2]*A[2];
    float a23sq = A[4]*A[4];
    float a11a22 = A[0]*A[3];
    float c1 = a11a22 - a12sq + A[0]*A[5] - a13sq 
                + A[3]*A[5] - a23sq;
    float c0 = a11a22*A[5] + 2.0f*A[1]*A[2]*A[4] - A[0]*a23sq - A[3]*a13sq - A[5]*a12sq;

    // compute intermediate values for root solving
    float c2sq = c2*c2;
    float a = (3.0f*c1 - c2sq)/3.0f;
    float b = (9.0f*c1*c2 - 2.0f*c2sq*c2 - 27.f*c0)/27.0f; 
    float halfb = b*0.5f;
    float halfb2 = halfb*halfb;
    float Q = halfb2 + a*a*a/27.0f;

    // determine type of eigenspaces
    if ( Q > 1.0e-6f )
    {
        // one eigenvalue, use standard basis
        v1.Set(1.0f, 0.0f, 0.0f);
        v2.Set(0.0f, 1.0f, 0.0f);
        v3.Set(0.0f, 0.0f, 1.0f);

        return;
    }

    else if ( Q < -1.0e-6f )
    {
        // three distinct eigenvalues

        // intermediate terms
        float theta_3 = atan2f(sqrtf(-Q),-halfb)/3.0f;
        float rho = sqrtf(halfb2 - Q);
        float c2_3 = c2/3.0f;
        float rho_13 = powf(fabsf(rho),1.0f/3.0f);
        if (rho < 0.0f)
            rho_13 = -rho_13;
        float ct_3, st_3;
        const float sqrt3 = sqrtf(3.0f);
        ct_3 = cosf(theta_3);
        st_3 = sinf(theta_3);

        // compute each eigenvalue and eigenvector
        // sort from largest to smallest
        float lambda1 = c2_3 + 2.0f*rho_13*ct_3;
        CalcSymmetricEigenvector( A, lambda1, v1 );

        float lambda2 = c2_3 - rho_13*(ct_3 + sqrt3*st_3);
        if (lambda2 > lambda1)
        {
            v2 = v1;
            float temp = lambda2;
            lambda2 = lambda1;
            lambda1 = temp;
            CalcSymmetricEigenvector( A, lambda2, v1 );
        }
        else
        {
            CalcSymmetricEigenvector( A, lambda2, v2 );               
        }

        float lambda3 = c2_3 - rho_13*(ct_3 - sqrt3*st_3);
        if ( lambda3 > lambda1 )
        {
            v3 = v2;
            v2 = v1;
            CalcSymmetricEigenvector( A, lambda3, v1 );
        }
        else if (lambda3 > lambda2 )
        {
            v3 = v2;
            CalcSymmetricEigenvector( A, lambda3, v2 );
        }
        else
        {
            CalcSymmetricEigenvector( A, lambda3, v3 );
        }
    }
    else
    {
        // two distinct eigenvalues

        // intermediate terms
        float c2_3 = c2/3.0f;
        float halfb_13 = powf(fabsf(halfb),1.0f/3.0f);
        if (halfb < 0.0f)
            halfb_13 = -halfb_13;

        // compute each eigenvalue and eigenvector
        // sort from largest to smallest
        float lambda1 = c2_3 + halfb_13;					
        CalcSymmetricEigenPair(A, lambda1, v1, v2 ); 

        float lambda2 = c2_3 - 2.0f*halfb_13;
        if (lambda2 > lambda1)
        {
            v3 = v2;
            v2 = v1;
            CalcSymmetricEigenvector( A, lambda2, v1 );
        }
        else
        {
            CalcSymmetricEigenvector( A, lambda2, v3 );
        }
    }

    v1.Normalize();
    v2.Normalize();
    v3.Normalize();

    if ( (v1.Cross(v2)).Dot(v3) < 0.0f )
        v3 = -v3;
}


//-------------------------------------------------------------------------------
// Given symmetric matrix A and eigenvalue l, returns corresponding eigenvector
// Assumes that order of eigenvalue is 1
//-------------------------------------------------------------------------------
void 
CalcSymmetricEigenvector( float A[6], float l, Vector3& v1 )
{
    float m11 = A[0] - l;
    float m12 = A[1];
    float m13 = A[2];
    float m22 = A[3] - l;
    float m23 = A[4];
    float m33 = A[5] - l;

    // compute cross product matrix, and find column with maximal entry
    float u11 = m22*m33 - m23*m23;
    float max = fabsf(u11);
    int c = 1;
    float u12 = m13*m23 - m12*m33;
    if (fabsf(u12) > max)
    {
        max = fabsf(u12);
        c = 2;
    }
    float u13 = m12*m23 - m13*m22;
    if (fabsf(u13) > max)
    {
        max = fabsf(u13);
        c = 3;
    }
    float u22 = m11*m33 - m13*m13;
    if (fabsf(u22) > max)
    {
        max = fabsf(u22);
        c = 2;
    }
    float u23 = m12*m13 - m23*m11;
    if (fabsf(u23) > max)
    {
        max = fabsf(u23);
        c = 3;
    }
    float u33 = m11*m22 - m12*m12;
    if (fabsf(u33) > max)
    {
	    max = fabsf(u33);
        c = 3;
    }

    // return column with maximal entry
    if (c == 1)
    {
	    v1.Set(u11, u12, u13);
	}
    else if (c == 2)
    {
        v1.Set(u12,u22,u23);
    }
    else
    {
        v1.Set(u13,u23,u33);
    }
}


//-------------------------------------------------------------------------------
// Given symmetric matrix A and eigenvalue l, returns eigenvector pair
// Assumes that order of eigenvalue is 2
//-------------------------------------------------------------------------------
void 
CalcSymmetricEigenPair( float A[6], float l, Vector3& v1, Vector3& v2 )
{
    // find maximal entry in M
    float m11 = A[0] - l;
    float max = fabsf(m11);
    int r=1,c=1;
    if (fabsf(A[1]) > max)
    {
        max = fabsf(A[1]);
        r = 1; c = 2;
    }
    if (fabsf(A[2]) > max)
    {
        max = fabsf(A[2]);
        r = 1; c = 3;
    }
    float m22 = A[3]-l;
    if (fabsf(m22) > max)
    {
        max = fabsf(m22);
        r = 2; c = 2;
    }
    if (fabsf(A[4]) > max)
    {
        max = fabsf(A[4]);
        r = 2; c = 3;
    }
    float m33 = A[5]-l;
    if (fabsf(m33) > max)
    {
        r = 3; c = 3;
    }

    // compute eigenvectors for each case
    if ( r == 1 )
    {
        if ( c == 3 )
        {
            v1.Set(A[2],0.0f,-m11);
            v2.Set(-A[1]*m11, m11*m11 + A[2]*A[2],-A[1]*A[2]);
        }
        else
        {
            v1.Set(-A[1],m11,0.0f);
            v2.Set(-A[2]*m11,-A[2]*A[1],m11*m11 + A[1]*A[1]);
        }
    }
    else if ( r == 2 )
    {
        v1.Set(0.0f,-A[4],m22);
        v2.Set(m22*m22 + A[4]*A[4],-A[1]*m22,-A[1]*A[4]);		
    }
    else if ( r == 3 )
    {
        v1.Set(0.0f,-m33,A[4]);
        v2.Set(A[4]*A[4] + m33*m33,-A[2]*A[4],-A[2]*m33);		
    }
}

