#ifndef			CSPHERICALROTATION_H_INCLUDED
#define			CSPHERICALROTATION_H_INCLUDED

#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once
#endif 
#endif 

#include		"Basics.h"

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CSphericalRotation
//*****************************************************************************
//
//!	A rotation described by latutude, longitude and azimuth
//
//*****************************************************************************
class CSphericalRotation	
{

protected:

	// Attribs

	float	m_fLatitude	;	//!< Latitude
	float	m_fLongitude;	//!< Longitude
	float	m_fAzimuth	;	//!< Azimuth

public:

	// Con-/Destruction

			CSphericalRotation	( const CSphericalRotation& a_Other );
			CSphericalRotation	()									;
			~CSphericalRotation	()									;

	// Get/Set

			float&				AccessLatitude	( void )												;
			float&				AccessLongitude	( void )												;
			float&				AccessAzimuth	( void )												;
			float				GetLatitude		( void )										const	;
			float				GetLongitude	( void )										const	;
			float				GetAzimuth		( void )										const	;
			void				GetLatitude		(		float&				a_fLatitude		)	const	;
			void				GetLongitude	(		float&				a_fLongitude	)	const	;
			void				GetAzimuth		(		float&				a_fAzimuth		)	const	;
			void				SetLatitude		(		float				a_fLatitude		)			;
			void				SetLongitude	(		float				a_fLongitude	)			;
			void				SetAzimuth		(		float				a_fAzimuth		)			;

	// Operators

			CSphericalRotation&	operator	=	( const	CSphericalRotation&	a_Other			)			;

};																																							

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	AccessLatitude
//*****************************************************************************
//
//! \brief		Returns the latitude for modification purposes
//! \return		The latitude
//
//*****************************************************************************
inline float& CSphericalRotation::AccessLatitude( void )
{
	return this->m_fLatitude;
}

//*****************************************************************************
//	AccessLongitude
//*****************************************************************************
//
//! \brief		Returns the longitude for modification purposes
//! \return		The longitude
//
//*****************************************************************************
inline float& CSphericalRotation::AccessLongitude( void )
{
	return this->m_fLongitude;
}

//*****************************************************************************
//	AccessAzimuth
//*****************************************************************************
//
//! \brief		Returns the azimuth for modification purposes
//! \return		The azimuth
//
//*****************************************************************************
inline float& CSphericalRotation::AccessAzimuth( void )
{
	return this->m_fAzimuth;
}

//*****************************************************************************
//	GetLatitude
//*****************************************************************************
//
//! \brief		Returns the latitude for look-up purposes
//! \return		The latitude
//
//*****************************************************************************
inline float CSphericalRotation::GetLatitude( void ) const			
{
	return this->m_fLatitude;
}

//*****************************************************************************
//	GetLongitude
//*****************************************************************************
//
//! \brief		Returns the longitude for look-up purposes
//! \return		The longitude
//
//*****************************************************************************
inline float CSphericalRotation::GetLongitude( void ) const			
{
	return this->m_fLongitude;
}

//*****************************************************************************
//	GetAzimuth
//*****************************************************************************
//
//! \brief		Returns the azimuth for look-up purposes
//! \return		The azimuth
//
//*****************************************************************************
inline float CSphericalRotation::GetAzimuth( void ) const			
{
	return this->m_fAzimuth;
}

//*****************************************************************************
//	GetLatitude
//*****************************************************************************
//
//! \brief		Retrieves the latitude
//! \param		[out] The latitude
//
//*****************************************************************************
inline void CSphericalRotation::GetLatitude( float& a_fLatitude ) const
{
	a_fLatitude = this->m_fLatitude;
}

//*****************************************************************************
//	GetLongitude
//*****************************************************************************
//
//! \brief		Retrieves the longitude
//! \param		[out] The longitude
//
//*****************************************************************************
inline void CSphericalRotation::GetLongitude( float& a_fLongitude ) const
{
	a_fLongitude = this->m_fLongitude;
}

//*****************************************************************************
//	GetAzimuth
//*****************************************************************************
//
//! \brief		Retrieves the azimuth
//! \param		[out] The azimuth
//
//*****************************************************************************
inline void CSphericalRotation::GetAzimuth( float& a_fAzimuth ) const
{
	a_fAzimuth = this->m_fAzimuth;
}

//*****************************************************************************
//	SetLatitude
//*****************************************************************************
//
//! \brief		Sets the latitude
//! \param		[in] The latitude to set
//
//*****************************************************************************
inline void CSphericalRotation::SetLatitude( float a_fLatitude )
{
	this->m_fLatitude = a_fLatitude;
}

//*****************************************************************************
//	SetLongitude
//*****************************************************************************
//
//! \brief		Sets the longitude
//! \param		[in] The longitude to set
//
//*****************************************************************************
inline void CSphericalRotation::SetLongitude( float a_fLongitude )
{
	this->m_fLongitude = a_fLongitude;
}

//*****************************************************************************
//	SetAzimuth
//*****************************************************************************
//
//! \brief		Sets the azimuth
//! \param		[in] The azimuth to set
//
//*****************************************************************************
inline void CSphericalRotation::SetAzimuth( float a_fAzimuth )
{
	this->m_fAzimuth = a_fAzimuth;
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif	// CSPHERICALROTATION_H_INCLUDED
