#ifndef			CEULERANGLES_H_INCLUDED
#define			CEULERANGLES_H_INCLUDED

#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once
#endif 
#endif 

#include		"CVector3.h"

class			CMatrix3			;
class			CAxisAngle			;
class			CQuaternion			;
class			CSphericalRotation	;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CEulerAngles
//*****************************************************************************
//
//!	Representation of Euler angles
//
//! Definition: in GLOBAL coordinates, rotate by m_fRoll around
//! the z-axis, then by m_fPitch around the x-axis and finally by
//! m_fYaw around the y-axis (or the inverse order in local coordinates)
//
//*****************************************************************************
class CEulerAngles 
{

protected:

	// Attribs

	float	m_fYaw;		//!< Yaw angle	( heading; direction the local z-axis points to in the global z-x-plane)
	float	m_fPitch;	//!< Pitch angle ( elevation; determines the projection of the local z-Axis onto the global y-axis; positive values push the z-axis down. )
	float	m_fRoll;	//!< Roll angle  ( determines rotation around the local z-axis )

public:

	// Con-/Destruction

			 CEulerAngles	(		float			a_fYaw
							,		float			a_fPitch
							,		float			a_fRoll			);							
	explicit CEulerAngles	( const	CQuaternion&	a_quatRotation	);							
	explicit CEulerAngles	( const	CAxisAngle&		a_Rotation		);							
			 CEulerAngles	( const	CEulerAngles&	a_Source		);							
			 CEulerAngles	();																	
			~CEulerAngles	();																	

	// Get/Set

			float&			AccessYaw		( void )											;
			float&			AccessPitch		( void )											;
			float&			AccessRoll		( void )											;
																						
			float			GetYaw			( void )									const	;
			float			GetPitch		( void )									const	;
			float			GetRoll			( void )									const	;
																						
			void			GetYaw			(		float&			a_fYaw			)	const	;
			void			GetPitch		(		float&			a_fPitch		)	const	;
			void			GetRoll			(		float&			a_fRoll			)	const	;
																						
			void			SetYaw			(		float			a_fYaw			)			;
			void			SetPitch		(		float			a_fPitch		)			;
			void			SetRoll			(		float			a_fRoll			)			;
																						
			void			SetEulerAngles	(		float			a_fYaw				
											,		float			a_fPitch			
											,		float			a_fRoll			)			;
			void			SetEulerAngles	( const CQuaternion&	a_quatRotation	)			;
			void			SetEulerAngles	( const CAxisAngle&		a_Rotation		)			;
			void			SetEulerAngles	( const CMatrix4&		a_matRotation	)			;
			void			SetEulerAngles	( const CEulerAngles&	a_Source		)			;
																								
	// Modification

			void			Clear			( void )											;

	// Computations

			CQuaternion		GetQuaternion	( void )									const	;
			CAxisAngle		GetAxisAngle	( void )									const	;
																																
	// Operators

			bool			operator ==		( const CEulerAngles&	a_Other			)	const	;
			bool			operator !=		( const CEulerAngles&	a_Other			)	const	;

			CEulerAngles&	operator =		( const CEulerAngles&	a_Source		)			;
			CEulerAngles&	operator =		( const CQuaternion&	a_quatRotation	)			;
			CEulerAngles&	operator =		( const CAxisAngle&		a_Source		)			;
			CEulerAngles&	operator =		( const CMatrix4&		a_Source		)			;
																			
			CEulerAngles&	operator +=		( const CEulerAngles&	a_Rotation		)			;
			CEulerAngles&	operator -=		( const CEulerAngles&	a_Rotation		)			;
			CEulerAngles&	operator *=		(		float			a_fScale		)			;
																						
	const	CEulerAngles	operator -		( void )									const	;
	const	CEulerAngles	operator +		( const CEulerAngles&	a_Rotation		)	const	;
	const	CEulerAngles	operator -		( const CEulerAngles&	a_Rotation		)	const	;
	const	CEulerAngles	operator *		(		float			a_fScale		)	const	;
	friend const CEulerAngles	operator  *	(		float			a_fScale		,
											  const CEulerAngles&	a_quatSource	)			;
																								
			float&			operator []		(		int				a_iIndex		)			;
			float			operator []		(		int				a_iIndex		)	const	;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	AccessYaw
//*****************************************************************************
//
//! \brief	Returns the yaw angle for modification purposes
//! \return	The yaw angle
//
//*****************************************************************************
inline float& CEulerAngles::AccessYaw( void )
{
	return m_fYaw;
}

//*****************************************************************************
//	AccessPitch
//*****************************************************************************
//
//! \brief	Returns the pitch angle for modification purposes
//! \return	The pitch angle
//
//*****************************************************************************
inline float& CEulerAngles::AccessPitch( void )
{
	return m_fPitch;
}

//*****************************************************************************
//	AccessRoll
//*****************************************************************************
//
//! \brief	Returns the roll angle for modification purposes
//! \return	The roll angle
//
//*****************************************************************************
inline float& CEulerAngles::AccessRoll( void )
{
	return m_fRoll;
}

//*****************************************************************************
//	GetYaw
//*****************************************************************************
//
//! \brief	Returns the yaw angle for look-up purposes
//! \return	The yaw angle
//
//*****************************************************************************
inline float CEulerAngles::GetYaw( void ) const			
{
	return m_fYaw;
}

//*****************************************************************************
//	GetPitch
//*****************************************************************************
//
//! \brief	Returns the pitch angle for look-up purposes
//! \return	The pitch angle
//
//*****************************************************************************
inline float CEulerAngles::GetPitch( void ) const			
{
	return m_fPitch;
}

//*****************************************************************************
//	GetRoll
//*****************************************************************************
//
//! \brief	Returns the roll angle for look-up purposes
//! \return	The roll angle
//
//*****************************************************************************
inline float CEulerAngles::GetRoll( void ) const			
{
	return m_fRoll;
}

//*****************************************************************************
//	GetYaw
//*****************************************************************************
//
//! \brief	Retrieves the yaw angle
//! \param	[out] The yaw angle
//
//*****************************************************************************
inline void CEulerAngles::GetYaw( float& a_fYaw ) const
{
	a_fYaw = m_fYaw;
}

//*****************************************************************************
//	GetPitch
//*****************************************************************************
//
//! \brief	Retrieves the pitch angle
//! \param	[out] The pitch angle
//
//*****************************************************************************
inline void CEulerAngles::GetPitch( float& a_fPitch ) const
{
	a_fPitch = m_fPitch;
}

//*****************************************************************************
//	GetRoll
//*****************************************************************************
//
//! \brief	Retrieves the roll angle
//! \param	[out] The roll angle
//
//*****************************************************************************
inline void CEulerAngles::GetRoll( float& a_fRoll ) const
{
	a_fRoll = m_fRoll;
}

//*****************************************************************************
//	SetYaw
//*****************************************************************************
//
//! \brief	Sets the yaw angle
//! \param	[in] The yaw angle to set
//
//*****************************************************************************
inline void CEulerAngles::SetYaw( float a_fYaw )
{
	m_fYaw = a_fYaw;
}

//*****************************************************************************
//	SetPitch
//*****************************************************************************
//
//! \brief	Sets the pitch angle
//! \param	[in] The pitch angle to set
//
//*****************************************************************************
inline void CEulerAngles::SetPitch( float a_fPitch )
{
	m_fPitch = a_fPitch;
}

//*****************************************************************************
//	SetRoll
//*****************************************************************************
//
//! \brief	Sets the roll angle
//! \param	[in] The roll angle to set
//
//*****************************************************************************
inline void CEulerAngles::SetRoll( float a_fRoll )
{
	m_fRoll = a_fRoll;
}

//*****************************************************************************
//	operator -
//*****************************************************************************
//
//! \brief	Sub operator
//! \return	The negative Euler angle
//
//*****************************************************************************
inline const CEulerAngles CEulerAngles::operator - ( void ) const
{
	return CEulerAngles(	- m_fYaw   ,
							- m_fPitch ,
							- m_fRoll  );
}

//*****************************************************************************
//	operator +
//*****************************************************************************
//
//! \brief	Add operator
//! \return	The result of the addition
//
//*****************************************************************************
inline const CEulerAngles CEulerAngles::operator + ( const CEulerAngles& a_Rotation ) const
{
	return CEulerAngles(	m_fYaw   + a_Rotation.m_fYaw  ,
							m_fPitch + a_Rotation.m_fPitch,
							m_fRoll  + a_Rotation.m_fRoll );
}

//*****************************************************************************
//	operator -
//*****************************************************************************
//
//! \brief	Sub operator
//! \return	The result of the subtraction
//
//*****************************************************************************
inline const CEulerAngles CEulerAngles::operator - ( const CEulerAngles& a_Rotation ) const
{
	return CEulerAngles(	m_fYaw   - a_Rotation.m_fYaw  ,
							m_fPitch - a_Rotation.m_fPitch,
							m_fRoll  - a_Rotation.m_fRoll );
}

//*****************************************************************************
//	operator -
//*****************************************************************************
//
//! \brief	Sub operator
//! \return	The result of the multiplication
//
//*****************************************************************************
inline const CEulerAngles CEulerAngles::operator * ( 		float			a_fScale	 ) const
{
	return CEulerAngles(	m_fYaw   * a_fScale,
							m_fPitch * a_fScale,
							m_fRoll  * a_fScale );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief	Mul operator
//! \return	The result of the multiplication
//
//*****************************************************************************
inline const CEulerAngles operator * ( float a_fFactor, const CEulerAngles& a_Factor )
{
	return a_Factor * a_fFactor;
}

//*****************************************************************************
//	operator []
//*****************************************************************************
//
//! \brief	Subscription operator
//! \return	A reference to an angle by index
//
//*****************************************************************************
inline float& CEulerAngles::operator [] ( int a_iIndex )
{
	return ((CVector3&) (*this))[ a_iIndex ];
}

//*****************************************************************************
//	operator []
//*****************************************************************************
//
//! \brief	Subscription operator
//! \return	An angle by index
//
//*****************************************************************************
inline float CEulerAngles::operator [] ( int a_iIndex ) const
{
	return ((CVector3&) (*this))[ a_iIndex ];
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif	// CEULERANGLES_H_INCLUDED
