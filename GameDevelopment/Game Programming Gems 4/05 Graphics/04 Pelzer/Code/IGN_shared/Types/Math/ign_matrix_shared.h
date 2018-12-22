
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Subproject:			General shared
//*				File:				ign_matrix_shared.h
//*				Description:		Shared file with matrix declarations/definitions
//*
//********************************************************************************************************************************************************************
//*
//*				Author:				Kurt Pelzer
//*									Copyright (c) 2002 Kurt Pelzer
//*									All rights reserved.
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Status:				In progress
//*
//********************************************************************************************************************************************************************
//*
//*				History:
//*
//*				01/07/03| 0.0.1		## Initial release															(kp) ##
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************




//------------- PREPROCESSOR -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_MATRIX_SHARED_H_INCLUDED
#define			IGN_MATRIX_SHARED_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ign_matrix_shared.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_TYPES_SHARED_H_INCLUDED
  #include		"../ign_types_shared.h"
#endif
#include		"math.h"




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------------------------------------------------------
//------------- DEFINITIONS ------------------------------------------------------------------------------------------------------------------------------------------
//------------- DECLARATIONS -----------------------------------------------------------------------------------------------------------------------------------------

struct	IGN_STRUCT_4x4Matrix
{
	union 
	{
        struct 
		{
            IGNFLOAT									m_f11, m_f12, m_f13, m_f14;					// First row
            IGNFLOAT									m_f21, m_f22, m_f23, m_f24;					// Second row
            IGNFLOAT									m_f31, m_f32, m_f33, m_f34;					// Third row
            IGNFLOAT									m_f41, m_f42, m_f43, m_f44;					// Fourth row

        }												m_Elements;
        IGNFLOAT										m_f[4][4];									// An array of floats that represent a 4x4 matrix
    };

	IGN_STRUCT_4x4Matrix Clear( void )
	{
		// Sets matrix to NULL.

		m_Elements.m_f11 = m_Elements.m_f12 = m_Elements.m_f13 = m_Elements.m_f14 =
		m_Elements.m_f21 = m_Elements.m_f22 = m_Elements.m_f23 = m_Elements.m_f24 =
		m_Elements.m_f31 = m_Elements.m_f32 = m_Elements.m_f33 = m_Elements.m_f34 =
		m_Elements.m_f41 = m_Elements.m_f42 = m_Elements.m_f43 = m_Elements.m_f44 = 0.0f;
		return (*this);
	}

	IGN_STRUCT_4x4Matrix SetIdentity( void )
	{
		// Sets matrix to identity.

		m_Elements.m_f11 = m_Elements.m_f22 = m_Elements.m_f33 = m_Elements.m_f44 = 1.0f;
		m_Elements.m_f12 = m_Elements.m_f13 = m_Elements.m_f14 =
		m_Elements.m_f21 = m_Elements.m_f23 = m_Elements.m_f24 =
		m_Elements.m_f31 = m_Elements.m_f32 = m_Elements.m_f34 =
		m_Elements.m_f41 = m_Elements.m_f42 = m_Elements.m_f43 = 0.0f;
		return (*this);
	}

	IGN_STRUCT_4x4Matrix Transpose( void )
	{
		// Transposes the matrix.

		IGN_STRUCT_4x4Matrix origMatrix = (*this);
		m_Elements.m_f12 = origMatrix.m_Elements.m_f21;
		m_Elements.m_f13 = origMatrix.m_Elements.m_f31;
		m_Elements.m_f14 = origMatrix.m_Elements.m_f41;
		m_Elements.m_f21 = origMatrix.m_Elements.m_f12;
		m_Elements.m_f23 = origMatrix.m_Elements.m_f32;
		m_Elements.m_f24 = origMatrix.m_Elements.m_f42;
		m_Elements.m_f31 = origMatrix.m_Elements.m_f13;
		m_Elements.m_f32 = origMatrix.m_Elements.m_f23;
		m_Elements.m_f34 = origMatrix.m_Elements.m_f43;
		m_Elements.m_f41 = origMatrix.m_Elements.m_f14;
		m_Elements.m_f42 = origMatrix.m_Elements.m_f24;
		m_Elements.m_f43 = origMatrix.m_Elements.m_f34;
		return (*this);
	}

	IGN_STRUCT_4x4Matrix Invert( void )
	{
		// Inverts the matrix.

		IGN_STRUCT_4x4Matrix origMatrix = (*this);
		IGNFLOAT fDet;
		IGNFLOAT d12, d13, d23, d24, d34, d41;
		d12 =	origMatrix.m_Elements.m_f31*origMatrix.m_Elements.m_f42 -
				origMatrix.m_Elements.m_f41*origMatrix.m_Elements.m_f32;
		d13 =	origMatrix.m_Elements.m_f31*origMatrix.m_Elements.m_f43 -
				origMatrix.m_Elements.m_f41*origMatrix.m_Elements.m_f33;
		d23 =	origMatrix.m_Elements.m_f32*origMatrix.m_Elements.m_f43 -
				origMatrix.m_Elements.m_f42*origMatrix.m_Elements.m_f33;
		d24 =	origMatrix.m_Elements.m_f32*origMatrix.m_Elements.m_f44 -
				origMatrix.m_Elements.m_f42*origMatrix.m_Elements.m_f34;
		d34 =	origMatrix.m_Elements.m_f33*origMatrix.m_Elements.m_f44 -
				origMatrix.m_Elements.m_f43*origMatrix.m_Elements.m_f34;
		d41 =	origMatrix.m_Elements.m_f34*origMatrix.m_Elements.m_f41 -
				origMatrix.m_Elements.m_f44*origMatrix.m_Elements.m_f31;
		m_Elements.m_f11 =   origMatrix.m_Elements.m_f22*d34 - 
							 origMatrix.m_Elements.m_f23*d24 +
							 origMatrix.m_Elements.m_f24*d23;
		m_Elements.m_f12 = -(origMatrix.m_Elements.m_f21*d34 +
							 origMatrix.m_Elements.m_f23*d41 +
							 origMatrix.m_Elements.m_f24*d13);
		m_Elements.m_f13 =   origMatrix.m_Elements.m_f21*d24 +
							 origMatrix.m_Elements.m_f22*d41 +
							 origMatrix.m_Elements.m_f24*d12;
		m_Elements.m_f14 = -(origMatrix.m_Elements.m_f21*d23 -
							 origMatrix.m_Elements.m_f22*d13 +
							 origMatrix.m_Elements.m_f23*d12);
		fDet =	origMatrix.m_Elements.m_f11*m_Elements.m_f11 + 
				origMatrix.m_Elements.m_f12*m_Elements.m_f12 +
				origMatrix.m_Elements.m_f13*m_Elements.m_f13 +
				origMatrix.m_Elements.m_f14*m_Elements.m_f14;
		if( fDet==0.0f ) 
		{
			SetIdentity();
			return (*this);
		}
		fDet = 1.0f/fDet;
		m_Elements.m_f11 *= fDet;
		m_Elements.m_f12 *= fDet;
		m_Elements.m_f13 *= fDet;
		m_Elements.m_f14 *= fDet;
		m_Elements.m_f21 = -(origMatrix.m_Elements.m_f12*d34 -
							 origMatrix.m_Elements.m_f13*d24 +
							 origMatrix.m_Elements.m_f14*d23) * fDet;
		m_Elements.m_f22 =  (origMatrix.m_Elements.m_f11*d34 +
							 origMatrix.m_Elements.m_f13*d41 +
							 origMatrix.m_Elements.m_f14*d13) * fDet;
		m_Elements.m_f23 = -(origMatrix.m_Elements.m_f11*d24 +
							 origMatrix.m_Elements.m_f12*d41 +
							 origMatrix.m_Elements.m_f14*d12) * fDet;
		m_Elements.m_f24 =  (origMatrix.m_Elements.m_f11*d23 -
							 origMatrix.m_Elements.m_f12*d13 +
							 origMatrix.m_Elements.m_f13*d12) * fDet;
		d12 =	origMatrix.m_Elements.m_f11*origMatrix.m_Elements.m_f22 - 
				origMatrix.m_Elements.m_f21*origMatrix.m_Elements.m_f12;
		d13 =	origMatrix.m_Elements.m_f11*origMatrix.m_Elements.m_f23 - 
				origMatrix.m_Elements.m_f21*origMatrix.m_Elements.m_f13;
		d23 =	origMatrix.m_Elements.m_f12*origMatrix.m_Elements.m_f23 - 
				origMatrix.m_Elements.m_f22*origMatrix.m_Elements.m_f13;
		d24 =	origMatrix.m_Elements.m_f12*origMatrix.m_Elements.m_f24 - 
				origMatrix.m_Elements.m_f22*origMatrix.m_Elements.m_f14;
		d34 =	origMatrix.m_Elements.m_f13*origMatrix.m_Elements.m_f24 - 
				origMatrix.m_Elements.m_f23*origMatrix.m_Elements.m_f14;
		d41 =	origMatrix.m_Elements.m_f14*origMatrix.m_Elements.m_f21 - 
				origMatrix.m_Elements.m_f24*origMatrix.m_Elements.m_f11;
		m_Elements.m_f31 =  (origMatrix.m_Elements.m_f42*d34 -
							 origMatrix.m_Elements.m_f43*d24 +
							 origMatrix.m_Elements.m_f44*d23) * fDet;
		m_Elements.m_f32 = -(origMatrix.m_Elements.m_f41*d34 +
							 origMatrix.m_Elements.m_f43*d41 +
							 origMatrix.m_Elements.m_f44*d13) * fDet;
		m_Elements.m_f33 =  (origMatrix.m_Elements.m_f41*d24 +
							 origMatrix.m_Elements.m_f42*d41 +
							 origMatrix.m_Elements.m_f44*d12) * fDet;
		m_Elements.m_f34 = -(origMatrix.m_Elements.m_f41*d23 -
							 origMatrix.m_Elements.m_f42*d13 +
							 origMatrix.m_Elements.m_f43*d12) * fDet;
		m_Elements.m_f41 = -(origMatrix.m_Elements.m_f32*d34 -
							 origMatrix.m_Elements.m_f33*d24 +
							 origMatrix.m_Elements.m_f34*d23) * fDet;
		m_Elements.m_f42 =  (origMatrix.m_Elements.m_f31*d34 +
							 origMatrix.m_Elements.m_f33*d41 +
							 origMatrix.m_Elements.m_f34*d13) * fDet;
		m_Elements.m_f43 = -(origMatrix.m_Elements.m_f31*d24 +
							 origMatrix.m_Elements.m_f32*d41 +
							 origMatrix.m_Elements.m_f34*d12) * fDet;
		m_Elements.m_f44 =  (origMatrix.m_Elements.m_f31*d23 -
							 origMatrix.m_Elements.m_f32*d13 +
							 origMatrix.m_Elements.m_f33*d12) * fDet;
		return (*this);
	}

	IGN_STRUCT_4x4Matrix AddMatrix( IGN_STRUCT_4x4Matrix& a_Matrix )
	{
		// Adds specified matrix to this one.

		m_Elements.m_f11 += a_Matrix.m_Elements.m_f11;
		m_Elements.m_f12 += a_Matrix.m_Elements.m_f12;
		m_Elements.m_f13 += a_Matrix.m_Elements.m_f13;
		m_Elements.m_f14 += a_Matrix.m_Elements.m_f14;
		m_Elements.m_f21 += a_Matrix.m_Elements.m_f21;
		m_Elements.m_f22 += a_Matrix.m_Elements.m_f22;
		m_Elements.m_f23 += a_Matrix.m_Elements.m_f23;
		m_Elements.m_f24 += a_Matrix.m_Elements.m_f24;
		m_Elements.m_f31 += a_Matrix.m_Elements.m_f31;
		m_Elements.m_f32 += a_Matrix.m_Elements.m_f32;
		m_Elements.m_f33 += a_Matrix.m_Elements.m_f33;
		m_Elements.m_f34 += a_Matrix.m_Elements.m_f34;
		m_Elements.m_f41 += a_Matrix.m_Elements.m_f41;
		m_Elements.m_f42 += a_Matrix.m_Elements.m_f42;
		m_Elements.m_f43 += a_Matrix.m_Elements.m_f43;
		m_Elements.m_f44 += a_Matrix.m_Elements.m_f44;
		return (*this);
	}

	IGN_STRUCT_4x4Matrix MultiplyMatrixFromRightSide( IGN_STRUCT_4x4Matrix& a_Matrix )
	{
		// Multiplies specified matrix (a_Matrix) to this one from the right side.
		//@TODO: Implement faster version ...

		IGN_STRUCT_4x4Matrix origMatrix = (*this);
		m_Elements.m_f11 =	origMatrix.m_Elements.m_f11*a_Matrix.m_Elements.m_f11 +
							origMatrix.m_Elements.m_f12*a_Matrix.m_Elements.m_f21 +
							origMatrix.m_Elements.m_f13*a_Matrix.m_Elements.m_f31 +
							origMatrix.m_Elements.m_f14*a_Matrix.m_Elements.m_f41;
		m_Elements.m_f21 =	origMatrix.m_Elements.m_f21*a_Matrix.m_Elements.m_f11 +
							origMatrix.m_Elements.m_f22*a_Matrix.m_Elements.m_f21 +
							origMatrix.m_Elements.m_f23*a_Matrix.m_Elements.m_f31 +
							origMatrix.m_Elements.m_f24*a_Matrix.m_Elements.m_f41;
		m_Elements.m_f31 =	origMatrix.m_Elements.m_f31*a_Matrix.m_Elements.m_f11 +
							origMatrix.m_Elements.m_f32*a_Matrix.m_Elements.m_f21 +
							origMatrix.m_Elements.m_f33*a_Matrix.m_Elements.m_f31 +
							origMatrix.m_Elements.m_f34*a_Matrix.m_Elements.m_f41;
		m_Elements.m_f41 =	origMatrix.m_Elements.m_f41*a_Matrix.m_Elements.m_f11 +
							origMatrix.m_Elements.m_f42*a_Matrix.m_Elements.m_f21 +
							origMatrix.m_Elements.m_f43*a_Matrix.m_Elements.m_f31 +
							origMatrix.m_Elements.m_f44*a_Matrix.m_Elements.m_f41;
		m_Elements.m_f12 =	origMatrix.m_Elements.m_f11*a_Matrix.m_Elements.m_f12 +
							origMatrix.m_Elements.m_f12*a_Matrix.m_Elements.m_f22 +
							origMatrix.m_Elements.m_f13*a_Matrix.m_Elements.m_f32 +
							origMatrix.m_Elements.m_f14*a_Matrix.m_Elements.m_f42;
		m_Elements.m_f22 =	origMatrix.m_Elements.m_f21*a_Matrix.m_Elements.m_f12 +
							origMatrix.m_Elements.m_f22*a_Matrix.m_Elements.m_f22 +
							origMatrix.m_Elements.m_f23*a_Matrix.m_Elements.m_f32 +
							origMatrix.m_Elements.m_f24*a_Matrix.m_Elements.m_f42;
		m_Elements.m_f32 =	origMatrix.m_Elements.m_f31*a_Matrix.m_Elements.m_f12 +
							origMatrix.m_Elements.m_f32*a_Matrix.m_Elements.m_f22 +
							origMatrix.m_Elements.m_f33*a_Matrix.m_Elements.m_f32 +
							origMatrix.m_Elements.m_f34*a_Matrix.m_Elements.m_f42;
		m_Elements.m_f42 =	origMatrix.m_Elements.m_f41*a_Matrix.m_Elements.m_f12 +
							origMatrix.m_Elements.m_f42*a_Matrix.m_Elements.m_f22 +
							origMatrix.m_Elements.m_f43*a_Matrix.m_Elements.m_f32 +
							origMatrix.m_Elements.m_f44*a_Matrix.m_Elements.m_f42;
		m_Elements.m_f13 =	origMatrix.m_Elements.m_f11*a_Matrix.m_Elements.m_f13 +
							origMatrix.m_Elements.m_f12*a_Matrix.m_Elements.m_f23 +
							origMatrix.m_Elements.m_f13*a_Matrix.m_Elements.m_f33 +
							origMatrix.m_Elements.m_f14*a_Matrix.m_Elements.m_f43;
		m_Elements.m_f23 =	origMatrix.m_Elements.m_f21*a_Matrix.m_Elements.m_f13 +
							origMatrix.m_Elements.m_f22*a_Matrix.m_Elements.m_f23 +
							origMatrix.m_Elements.m_f23*a_Matrix.m_Elements.m_f33 +
							origMatrix.m_Elements.m_f24*a_Matrix.m_Elements.m_f43;
		m_Elements.m_f33 =	origMatrix.m_Elements.m_f31*a_Matrix.m_Elements.m_f13 +
							origMatrix.m_Elements.m_f32*a_Matrix.m_Elements.m_f23 +
							origMatrix.m_Elements.m_f33*a_Matrix.m_Elements.m_f33 +
							origMatrix.m_Elements.m_f34*a_Matrix.m_Elements.m_f43;
		m_Elements.m_f43 =	origMatrix.m_Elements.m_f41*a_Matrix.m_Elements.m_f13 +
							origMatrix.m_Elements.m_f42*a_Matrix.m_Elements.m_f23 +
							origMatrix.m_Elements.m_f43*a_Matrix.m_Elements.m_f33 +
							origMatrix.m_Elements.m_f44*a_Matrix.m_Elements.m_f43;
		m_Elements.m_f14 =	origMatrix.m_Elements.m_f11*a_Matrix.m_Elements.m_f14 +
							origMatrix.m_Elements.m_f12*a_Matrix.m_Elements.m_f24 +
							origMatrix.m_Elements.m_f13*a_Matrix.m_Elements.m_f34 +
							origMatrix.m_Elements.m_f14*a_Matrix.m_Elements.m_f44;
		m_Elements.m_f24 =	origMatrix.m_Elements.m_f21*a_Matrix.m_Elements.m_f14 +
							origMatrix.m_Elements.m_f22*a_Matrix.m_Elements.m_f24 +
							origMatrix.m_Elements.m_f23*a_Matrix.m_Elements.m_f34 +
							origMatrix.m_Elements.m_f24*a_Matrix.m_Elements.m_f44;
		m_Elements.m_f34 =	origMatrix.m_Elements.m_f31*a_Matrix.m_Elements.m_f14 +
							origMatrix.m_Elements.m_f32*a_Matrix.m_Elements.m_f24 +
							origMatrix.m_Elements.m_f33*a_Matrix.m_Elements.m_f34 +
							origMatrix.m_Elements.m_f34*a_Matrix.m_Elements.m_f44;
		m_Elements.m_f44 =	origMatrix.m_Elements.m_f41*a_Matrix.m_Elements.m_f14 +
							origMatrix.m_Elements.m_f42*a_Matrix.m_Elements.m_f24 +
							origMatrix.m_Elements.m_f43*a_Matrix.m_Elements.m_f34 +
							origMatrix.m_Elements.m_f44*a_Matrix.m_Elements.m_f44;
		return (*this);
	}

	IGN_STRUCT_4x4Matrix MultiplyMatrixFromLeftSide( IGN_STRUCT_4x4Matrix& a_Matrix )
	{
		// Multiplies specified matrix (a_Matrix) to this one from the left side.
		//@TODO: Implement faster version ...

		IGN_STRUCT_4x4Matrix origMatrix = (*this);
		m_Elements.m_f11 =	a_Matrix.m_Elements.m_f11*origMatrix.m_Elements.m_f11 +
							a_Matrix.m_Elements.m_f12*origMatrix.m_Elements.m_f21 +
							a_Matrix.m_Elements.m_f13*origMatrix.m_Elements.m_f31 +
							a_Matrix.m_Elements.m_f14*origMatrix.m_Elements.m_f41;
		m_Elements.m_f21 =	a_Matrix.m_Elements.m_f21*origMatrix.m_Elements.m_f11 +
							a_Matrix.m_Elements.m_f22*origMatrix.m_Elements.m_f21 +
							a_Matrix.m_Elements.m_f23*origMatrix.m_Elements.m_f31 +
							a_Matrix.m_Elements.m_f24*origMatrix.m_Elements.m_f41;
		m_Elements.m_f31 =	a_Matrix.m_Elements.m_f31*origMatrix.m_Elements.m_f11 +
							a_Matrix.m_Elements.m_f32*origMatrix.m_Elements.m_f21 +
							a_Matrix.m_Elements.m_f33*origMatrix.m_Elements.m_f31 +
							a_Matrix.m_Elements.m_f34*origMatrix.m_Elements.m_f41;
		m_Elements.m_f41 =	a_Matrix.m_Elements.m_f41*origMatrix.m_Elements.m_f11 +
							a_Matrix.m_Elements.m_f42*origMatrix.m_Elements.m_f21 +
							a_Matrix.m_Elements.m_f43*origMatrix.m_Elements.m_f31 +
							a_Matrix.m_Elements.m_f44*origMatrix.m_Elements.m_f41;
		m_Elements.m_f12 =	a_Matrix.m_Elements.m_f11*origMatrix.m_Elements.m_f12 +
							a_Matrix.m_Elements.m_f12*origMatrix.m_Elements.m_f22 +
							a_Matrix.m_Elements.m_f13*origMatrix.m_Elements.m_f32 +
							a_Matrix.m_Elements.m_f14*origMatrix.m_Elements.m_f42;
		m_Elements.m_f22 =	a_Matrix.m_Elements.m_f21*origMatrix.m_Elements.m_f12 +
							a_Matrix.m_Elements.m_f22*origMatrix.m_Elements.m_f22 +
							a_Matrix.m_Elements.m_f23*origMatrix.m_Elements.m_f32 +
							a_Matrix.m_Elements.m_f24*origMatrix.m_Elements.m_f42;
		m_Elements.m_f32 =	a_Matrix.m_Elements.m_f31*origMatrix.m_Elements.m_f12 +
							a_Matrix.m_Elements.m_f32*origMatrix.m_Elements.m_f22 +
							a_Matrix.m_Elements.m_f33*origMatrix.m_Elements.m_f32 +
							a_Matrix.m_Elements.m_f34*origMatrix.m_Elements.m_f42;
		m_Elements.m_f42 =	a_Matrix.m_Elements.m_f41*origMatrix.m_Elements.m_f12 +
							a_Matrix.m_Elements.m_f42*origMatrix.m_Elements.m_f22 +
							a_Matrix.m_Elements.m_f43*origMatrix.m_Elements.m_f32 +
							a_Matrix.m_Elements.m_f44*origMatrix.m_Elements.m_f42;
		m_Elements.m_f13 =	a_Matrix.m_Elements.m_f11*origMatrix.m_Elements.m_f13 +
							a_Matrix.m_Elements.m_f12*origMatrix.m_Elements.m_f23 +
							a_Matrix.m_Elements.m_f13*origMatrix.m_Elements.m_f33 +
							a_Matrix.m_Elements.m_f14*origMatrix.m_Elements.m_f43;
		m_Elements.m_f23 =	a_Matrix.m_Elements.m_f21*origMatrix.m_Elements.m_f13 +
							a_Matrix.m_Elements.m_f22*origMatrix.m_Elements.m_f23 +
							a_Matrix.m_Elements.m_f23*origMatrix.m_Elements.m_f33 +
							a_Matrix.m_Elements.m_f24*origMatrix.m_Elements.m_f43;
		m_Elements.m_f33 =	a_Matrix.m_Elements.m_f31*origMatrix.m_Elements.m_f13 +
							a_Matrix.m_Elements.m_f32*origMatrix.m_Elements.m_f23 +
							a_Matrix.m_Elements.m_f33*origMatrix.m_Elements.m_f33 +
							a_Matrix.m_Elements.m_f34*origMatrix.m_Elements.m_f43;
		m_Elements.m_f43 =	a_Matrix.m_Elements.m_f41*origMatrix.m_Elements.m_f13 +
							a_Matrix.m_Elements.m_f42*origMatrix.m_Elements.m_f23 +
							a_Matrix.m_Elements.m_f43*origMatrix.m_Elements.m_f33 +
							a_Matrix.m_Elements.m_f44*origMatrix.m_Elements.m_f43;
		m_Elements.m_f14 =	a_Matrix.m_Elements.m_f11*origMatrix.m_Elements.m_f14 +
							a_Matrix.m_Elements.m_f12*origMatrix.m_Elements.m_f24 +
							a_Matrix.m_Elements.m_f13*origMatrix.m_Elements.m_f34 +
							a_Matrix.m_Elements.m_f14*origMatrix.m_Elements.m_f44;
		m_Elements.m_f24 =	a_Matrix.m_Elements.m_f21*origMatrix.m_Elements.m_f14 +
							a_Matrix.m_Elements.m_f22*origMatrix.m_Elements.m_f24 +
							a_Matrix.m_Elements.m_f23*origMatrix.m_Elements.m_f34 +
							a_Matrix.m_Elements.m_f24*origMatrix.m_Elements.m_f44;
		m_Elements.m_f34 =	a_Matrix.m_Elements.m_f31*origMatrix.m_Elements.m_f14 +
							a_Matrix.m_Elements.m_f32*origMatrix.m_Elements.m_f24 +
							a_Matrix.m_Elements.m_f33*origMatrix.m_Elements.m_f34 +
							a_Matrix.m_Elements.m_f34*origMatrix.m_Elements.m_f44;
		m_Elements.m_f44 =	a_Matrix.m_Elements.m_f41*origMatrix.m_Elements.m_f14 +
							a_Matrix.m_Elements.m_f42*origMatrix.m_Elements.m_f24 +
							a_Matrix.m_Elements.m_f43*origMatrix.m_Elements.m_f34 +
							a_Matrix.m_Elements.m_f44*origMatrix.m_Elements.m_f44;
		return (*this);
	}

	IGN_STRUCT_4x4Matrix GeneratePitchRotate( IGNFLOAT fPitch )
	{
		// Generates a pitch rotation matrix.

		this->SetIdentity();
		m_Elements.m_f22 = static_cast<IGNFLOAT>( cos(fPitch) );
		m_Elements.m_f23 = static_cast<IGNFLOAT>( sin(fPitch) );
		m_Elements.m_f32 = static_cast<IGNFLOAT>( -sin(fPitch) );
		m_Elements.m_f33 = static_cast<IGNFLOAT>( cos(fPitch) );
		return (*this);
	}

	IGN_STRUCT_4x4Matrix GenerateYawRotate( IGNFLOAT fYaw )
	{
		// Generates a yaw rotation matrix.

		this->SetIdentity();
		m_Elements.m_f11 = static_cast<IGNFLOAT>( cos(fYaw) );
		m_Elements.m_f13 = static_cast<IGNFLOAT>( -sin(fYaw) );
		m_Elements.m_f31 = static_cast<IGNFLOAT>( sin(fYaw) );
		m_Elements.m_f33 = static_cast<IGNFLOAT>( cos(fYaw) );
		return (*this);
	}

	IGN_STRUCT_4x4Matrix GenerateRollRotate( IGNFLOAT fRoll )
	{
		// Generates a roll rotation matrix.

		this->SetIdentity();
		m_Elements.m_f11 = static_cast<IGNFLOAT>( cos(fRoll) );
		m_Elements.m_f12 = static_cast<IGNFLOAT>( sin(fRoll) );
		m_Elements.m_f21 = static_cast<IGNFLOAT>( -sin(fRoll) );
		m_Elements.m_f22 = static_cast<IGNFLOAT>( cos(fRoll) );
		return (*this);
	}
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------------------------------------------------------

#endif // IGN_MATRIX_SHARED_H_INCLUDED
