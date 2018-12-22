
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Project:			IGNITION
//*
//********************************************************************************************************************************************************************
//********************************************************************************************************************************************************************
//*
//*				Subproject:			General shared
//*				File:				ign_vector_shared.h
//*				Description:		Shared file with vector declarations/definitions
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

#ifndef			IGN_VECTOR_SHARED_H_INCLUDED
#define			IGN_VECTOR_SHARED_H_INCLUDED

#if				_MSC_VER > 1000
#pragma once
#ifdef			IGN_SWITCH_SHOW_HEADER
#pragma message ("---> ign_vector_shared.h\n")
#endif
#endif 




//------------- DEPENDENCIES -----------------------------------------------------------------------------------------------------------------------------------------

#ifndef			IGN_TYPES_SHARED_H_INCLUDED
  #include		"../ign_types_shared.h"
#endif

#ifndef			IGN_MATRIX_SHARED_H_INCLUDED
  #include		"ign_matrix_shared.h"
#endif

#include		<math.h>




//------------- FORWARD DECLARATIONS ---------------------------------------------------------------------------------------------------------------------------------
//------------- DEFINITIONS ------------------------------------------------------------------------------------------------------------------------------------------
//------------- DECLARATIONS -----------------------------------------------------------------------------------------------------------------------------------------

struct	IGN_STRUCT_3DVector
{
    IGNFLOAT											m_fX;										// The x component
    IGNFLOAT											m_fY;										// The y component
    IGNFLOAT											m_fZ;										// The z component

	IGN_STRUCT_3DVector()
	{
	}

	IGN_STRUCT_3DVector( IGNFLOAT a_fX, IGNFLOAT a_fY, IGNFLOAT a_fZ )
	{
		m_fX = a_fX;
		m_fY = a_fY;
		m_fZ = a_fZ;
	}

	IGN_STRUCT_3DVector MultiplyMatrix( IGN_STRUCT_4x4Matrix& a_Matrix )
	{
		// Adds specified matrix to this one.

		IGN_STRUCT_3DVector origVector = (*this);
		m_fX =	origVector.m_fX*a_Matrix.m_Elements.m_f11 +
				origVector.m_fY*a_Matrix.m_Elements.m_f12 +
				origVector.m_fZ*a_Matrix.m_Elements.m_f13;
		m_fY =	origVector.m_fX*a_Matrix.m_Elements.m_f21 +
				origVector.m_fY*a_Matrix.m_Elements.m_f22 +
				origVector.m_fZ*a_Matrix.m_Elements.m_f23;
		m_fZ =	origVector.m_fX*a_Matrix.m_Elements.m_f31 +
				origVector.m_fY*a_Matrix.m_Elements.m_f32 +
				origVector.m_fZ*a_Matrix.m_Elements.m_f33;
		return (*this);
	}

	IGNFLOAT GetSquareMagnitude() const
	{
		return m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ;
	}

	IGNFLOAT GetMagnitude() const
	{
		return static_cast<IGNFLOAT>( sqrt( GetSquareMagnitude() ) );
	}

	IGN_STRUCT_3DVector Normalize()
	{
		// Adds specified matrix to this one.

		IGNFLOAT fLength;
		fLength = static_cast<IGNFLOAT>( sqrt( m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ ) );
		m_fX /= fLength;
		m_fY /= fLength;
		m_fZ /= fLength;
		return (*this);
	}
	
	IGN_STRUCT_3DVector operator+( const IGN_STRUCT_3DVector& a_vecTranslate ) const
	{
		IGN_STRUCT_3DVector vecDummy;

		vecDummy.m_fX = m_fX + a_vecTranslate.m_fX;
		vecDummy.m_fY = m_fY + a_vecTranslate.m_fY;
		vecDummy.m_fZ = m_fZ + a_vecTranslate.m_fZ;

		return vecDummy;
	}

	IGN_STRUCT_3DVector operator-( const IGN_STRUCT_3DVector& a_vecTranslate ) const
	{
		IGN_STRUCT_3DVector vecDummy;

		vecDummy.m_fX = m_fX - a_vecTranslate.m_fX;
		vecDummy.m_fY = m_fY - a_vecTranslate.m_fY;
		vecDummy.m_fZ = m_fZ - a_vecTranslate.m_fZ;

		return vecDummy;
	}

	IGN_STRUCT_3DVector operator*( IGNFLOAT a_fScale ) const
	{
		IGN_STRUCT_3DVector vecDummy;

		vecDummy.m_fX = m_fX * a_fScale;
		vecDummy.m_fY = m_fY * a_fScale;
		vecDummy.m_fZ = m_fZ * a_fScale;

		return vecDummy;
	}

	IGN_STRUCT_3DVector& operator+=( const IGN_STRUCT_3DVector& a_vecTranslate	)
	{
		m_fX += a_vecTranslate.m_fX;
		m_fY += a_vecTranslate.m_fY;
		m_fZ += a_vecTranslate.m_fZ;

		return *this;
	}

	IGN_STRUCT_3DVector& operator-=( const IGN_STRUCT_3DVector& a_vecTranslate	)
	{
		m_fX -= a_vecTranslate.m_fX;
		m_fY -= a_vecTranslate.m_fY;
		m_fZ -= a_vecTranslate.m_fZ;

		return *this;
	}

	static IGNFLOAT DotProduct( const IGN_STRUCT_3DVector& a_vecFactorA, const IGN_STRUCT_3DVector& a_vecFactorB )
	{
		return	( a_vecFactorA.m_fX * a_vecFactorB.m_fX 
				+ a_vecFactorA.m_fY * a_vecFactorB.m_fY 
				+ a_vecFactorA.m_fZ * a_vecFactorB.m_fZ );
	}

	static IGN_STRUCT_3DVector CrossProduct( const IGN_STRUCT_3DVector& a_vecA, const IGN_STRUCT_3DVector& a_vecB )
	{
		IGN_STRUCT_3DVector vecDummy;

		vecDummy.m_fX = a_vecA.m_fY * a_vecB.m_fZ - a_vecA.m_fZ * a_vecB.m_fY;
		vecDummy.m_fY = a_vecA.m_fZ * a_vecB.m_fX - a_vecA.m_fX * a_vecB.m_fZ;
		vecDummy.m_fZ = a_vecA.m_fX * a_vecB.m_fY - a_vecA.m_fY * a_vecB.m_fX;

		return vecDummy;
	}
};

struct	IGN_STRUCT_3DKeyVector
{
    IGNFLOAT											m_fTime;									// Keyframe time stamp
    IGNFLOAT											m_fX;										// The x component
    IGNFLOAT											m_fY;										// The y component
    IGNFLOAT											m_fZ;										// The z component
};

struct	IGN_STRUCT_4DVector
{
    IGNFLOAT											m_fX;										// The x component
    IGNFLOAT											m_fY;										// The y component
    IGNFLOAT											m_fZ;										// The z component
    IGNFLOAT											m_fW;										// The w component
};




//------------- PREPROCESSOR END -------------------------------------------------------------------------------------------------------------------------------------

#endif // IGN_VECTOR_SHARED_H_INCLUDED
