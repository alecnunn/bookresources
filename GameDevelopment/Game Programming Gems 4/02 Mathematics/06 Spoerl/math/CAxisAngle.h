#ifndef			CAXISANGLE_H_INCLUDED
#define			CAXISANGLE_H_INCLUDED

#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once
#endif 
#endif 

#include		"CVector3.h"

class			CMatrix3			;
class			CEulerAngles		;
class			CQuaternion			;
class			CSphericalRotation	;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CAxisAngle
//*****************************************************************************
//
//!	Representation of a rotation by an angle along an axis 
//
//*****************************************************************************
class CAxisAngle
{

protected:

	// Attribs

	CVector3	m_vecAxis	;	//!< Rotation's axis
	float		m_fAngle	;	//!< Rotation's angle

public:

	// Con-/Destruction

			 CAxisAngle	( const CVector3&		a_vecAxis
						,		float			a_fAngle		);									
	explicit CAxisAngle	( const CQuaternion&	a_quatRotation	);									
	explicit CAxisAngle	( const CEulerAngles&	a_Rotation		);									
			 CAxisAngle	( const CAxisAngle&		a_Source		);									
			 CAxisAngle	();																			
			~CAxisAngle	();																			

	// Get/Set

			CVector3&		AccessAxis		( void )											;	
			float&			AccessAngle		( void )											;	
																						
	const	CVector3&		GetAxis			( void )									const	;	
			float			GetAngle		( void )									const	;	
																						
			void			GetAxis			(		CVector3&		a_vecAxis		)	const	;	
			void			GetAngle		(		float&			a_fAngle		)	const	;	
																						
			void			SetAxis			( const CVector3&		a_vecAxis		)			;	
			void			SetAngle		(		float			a_fAngle		)			;	
																						
			void			SetAxisAngle	( const CVector3&		a_vecAxis
											,		float			a_fAngle		)			;	
			void			SetAxisAngle	( const CQuaternion&	a_quatRotation	)			;	
			void			SetAxisAngle	( const CEulerAngles&	a_Rotation		)			;	
			void			SetAxisAngle	( const CAxisAngle&		a_Source		)			;	

	// Modification

			void			Clear			( void )											;	

	// Computations

			bool			IsEqual			( const	CAxisAngle&		a_Rotation		)	const	;	
																						
			CQuaternion		GetQuaternion	( void )									const	;	
			CEulerAngles	GetEulerAngles	( void )									const	;	

	// Operators

			bool			operator ==		( const	CAxisAngle&		a_Rotation		)	const	;	
			bool			operator !=		( const CAxisAngle&		a_Rotation		)	const	;	
			CAxisAngle&		operator =		( const CAxisAngle&		a_Rotation		)			;	
			CAxisAngle&		operator =		( const CQuaternion&	a_quatRotation	)			;	
			CAxisAngle&		operator =		( const CEulerAngles&	a_Rotation		)			;	

};																															

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#include	"CQuaternion.h"

//*****************************************************************************
//	AccessAxis
//*****************************************************************************
//
//! \brief	Returns the rotation's axis for modification purposes
//! \return	The rotation's axis
//
//*****************************************************************************
inline CVector3& CAxisAngle::AccessAxis( void )
{
	return m_vecAxis;
}

//*****************************************************************************
//	AccessAngle
//*****************************************************************************
//
//! \brief	Returns the rotation's angle for modification purposes
//! \return	The rotation's angle
//
//*****************************************************************************
inline float& CAxisAngle::AccessAngle( void )
{
	return m_fAngle;
}

//*****************************************************************************
//	GetAxis
//*****************************************************************************
//
//! \brief	Returns the rotation's axis for look-up purposes
//! \return	The rotation's axis
//
//*****************************************************************************
inline const CVector3& CAxisAngle::GetAxis( void ) const			
{
	return m_vecAxis;
}

//*****************************************************************************
//	GetAngle
//*****************************************************************************
//
//! \brief	Returns the rotation's angle for look-up purposes
//! \return	The rotation's angle
//
//*****************************************************************************
inline float CAxisAngle::GetAngle( void ) const			
{
	return m_fAngle;
}

//*****************************************************************************
//	GetAxis
//*****************************************************************************
//
//! \brief	Retrieves the rotation's axis
//! \param	[out] The rotation's axis
//
//*****************************************************************************
inline void CAxisAngle::GetAxis( CVector3& a_vecAxis ) const
{
	a_vecAxis = m_vecAxis;
}

//*****************************************************************************
//	GetAngle
//*****************************************************************************
//
//! \brief	Retrieves the rotation's angle
//! \param	[out] The rotation's angle
//
//*****************************************************************************
inline void CAxisAngle::GetAngle( float& a_fAngle ) const
{
	a_fAngle = m_fAngle;
}

//*****************************************************************************
//	SetAxis
//*****************************************************************************
//
//! \brief	Sets the rotation's axis
//! \param	[in] The rotation's axis to set
//
//*****************************************************************************
inline void CAxisAngle::SetAxis( const CVector3& a_vecAxis )
{
	m_vecAxis = a_vecAxis;
}

//*****************************************************************************
//	SetAngle
//*****************************************************************************
//
//! \brief	Sets the rotation's angle
//! \param	[in] The rotation's angle to set
//
//*****************************************************************************
inline void CAxisAngle::SetAngle( float a_fAngle )
{
	m_fAngle = a_fAngle;
}

//*****************************************************************************
//	SetAxisAngle
//*****************************************************************************
//
//! \brief	Sets the rotation's axis and angle
//! \param	[in] The euler angles to set
//
//*****************************************************************************
inline void CAxisAngle::SetAxisAngle( const CEulerAngles& a_Rotation ) 
{
	SetAxisAngle( CQuaternion( a_Rotation ) );
}

//*****************************************************************************
//	SetAxisAngle
//*****************************************************************************
//
//! \brief	Sets the rotation's axis and angle
//! \param	[in] The axis and angle to copy
//
//*****************************************************************************
inline void CAxisAngle::SetAxisAngle( const CAxisAngle& a_Source ) 
{
	m_vecAxis = a_Source.GetAxis();
}

//*****************************************************************************
//	operator == 
//*****************************************************************************
//
//! \brief	Determines whether the axis and angle equals the specified axis and angle
//! \param	[in] A reference to an axisangle to compare to
//! \return	Returns true, if both axis and angles are equal, either false
//
//*****************************************************************************
inline bool CAxisAngle::operator == ( const CAxisAngle& a_Rotation ) const
{
	return IsEqual( a_Rotation );
}

//*****************************************************************************
//	operator != 
//*****************************************************************************
//
//! \brief	Determines whether the axis and angle differs from the specified axis and angle
//! \param	[in] A reference to an axisangle to compare to
//! \return	Returns false, if both axis and angles are equal, either true
//
//*****************************************************************************
inline bool CAxisAngle::operator != ( const CAxisAngle& a_Rotation ) const
{
	return ! (IsEqual( a_Rotation ) ); 
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif	// CAXISANGLE_H_INCLUDED
