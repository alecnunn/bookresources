#ifndef			CQUATERNION_H_INCLUDED
#define			CQUATERNION_H_INCLUDED

#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once
#endif 
#endif 

#include		"CVector3.h"

class			CAxisAngle			;
class			CEulerAngles		;
class			CSphericalRotation	;
class			CMatrix3			;
class			CMatrix4			;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CQuaternion
//*****************************************************************************
//
//!	Representation of a quaternion
//
//*****************************************************************************
class CQuaternion 
{

protected:

	// Attribs

	float m_fX;	//!< Quaternion's x coordinate
	float m_fY;	//!< Quaternion's y coordinate
	float m_fZ;	//!< Quaternion's z coordinate
	float m_fW;	//!< Quaternion's w coordinate
																																									
public:

	// Con-/Destruction

			 CQuaternion( float a_fX, float a_fY, float a_fZ, float a_fW );
	explicit CQuaternion( const CMatrix4&			a_matRotation	);																					
	explicit CQuaternion( const CMatrix3&			a_matRotation	);																					
	explicit CQuaternion( const CAxisAngle&			a_Rotation		);																					
	explicit CQuaternion( const CEulerAngles&		a_Rotation		);																					
	explicit CQuaternion( const CSphericalRotation&	a_Rotation		);																					
			 CQuaternion( const CQuaternion&		a_quatRotation	);																					
			 CQuaternion();																																
			~CQuaternion();																																

	// Get/Set

			float&				AccessX				( void )																							;
			float&				AccessY				( void )																							;
			float&				AccessZ				( void )																							;
			float&				AccessW				( void )																							;
																																				
			float				GetX				( void )																					const	;
			float				GetY				( void )																					const	;
			float				GetZ				( void )																					const	;
			float				GetW				( void )																					const	;
																																				
			void				GetX				(		float&				a_fX			)												const	;
			void				GetY				(		float&				a_fY			)												const	;
			void				GetZ				(		float&				a_fZ			)												const	;
			void				GetW				(		float&				a_fW			)												const	;
																																			
			void				SetX				(		float				a_fX			)														;
			void				SetY				(		float				a_fY			)														;
			void				SetZ				(		float				a_fZ			)														;
			void				SetW				(		float				a_fW			)														;

			void				SetQuaternion		(		float				a_fX																	
													,		float				a_fY																	
													,		float				a_fZ																	
													,		float				a_fW			)														;
			void				SetQuaternion		( const	CVector3&			a_vecAxisSrc
													, const	CVector3&			a_vecAxisDst	)														;													
			void				SetQuaternion		( const CMatrix4&			a_matRotation	)														;
			void				SetQuaternion		( const CMatrix3&			a_matRotation	)														;
			void				SetQuaternion		( const CAxisAngle&			a_Rotation		)														;
			void				SetQuaternion		( const CEulerAngles&		a_Rotation		)														;
			void				SetQuaternion		( const CSphericalRotation&	a_Rotation		)														;
			void				SetQuaternion		( const CQuaternion&		a_quatRotation	)														;

	// Modification

			void				Clear				( void );																							
			void				Invert				( void );																							
			void				Negate				( void );																							
			void				Normalize			( void );																							
																																						
			void				Add					( const CQuaternion&		a_quat			)														;
			void				Sub					( const CQuaternion&		a_quat			)														;
			void				Multiply			(		float				a_fFactor		)														;
			void				Multiply			( const CQuaternion&		a_quatFactor	)														;
			void				ReverseMultiply		( const CQuaternion&		a_quatPrefix	)														;
			void				RotateAxisX			(		float				a_fAngle		)														;
			void				RotateAxisY			(		float				a_fAngle		)														;
			void				RotateAxisZ			(		float				a_fAngle		)														;
			void				RotateLocalAxisX	(		float				a_fAngle		)														;
			void				RotateLocalAxisY	(		float				a_fAngle		)														;
			void				RotateLocalAxisZ	(		float				a_fAngle		)														;

			void				SetToProduct		( const	CQuaternion&		a_quatLeft		
													, const	CQuaternion&		a_quatRight	)															;
			void				SetLerp				(		CQuaternion			a_quatSource
													, const	CQuaternion&		a_quatTarget
													,		float				a_fAlpha		)														;
			void				SetSlerp			(		CQuaternion			a_quatSource
													,		CQuaternion			a_quatTarget
													,		float				a_fAlpha		)														;

	// Computations

			bool				IsEqual				( const CQuaternion&		a_quatFactor	)												const	;

			float				GetMagnitude		( void )																					const	;
			float				GetSquareMagnitude	( void )																					const	;
			float				GetDotProduct		( const CQuaternion&		a_quatFactor	)												const	;
																																				
			CMatrix4			GetMatrix			( void )																					const	;
			CMatrix3			GetMatrix3			( void )																					const	;
			CAxisAngle			GetAxisAngle		( void )																					const	;
			CEulerAngles		GetEulerAngles		( void )																					const	;
			CSphericalRotation	GetSphericalRotation( void )																					const	;
																																					
			CQuaternion			GetInverted			( void	)																					const	;
			CQuaternion			GetNegated			( void	)																					const	;
			CQuaternion			GetNormalized		( void	)																					const	;
			CQuaternion			GetSum				( const CQuaternion&		a_quat			)												const	;
			CQuaternion			GetDifference		( const CQuaternion&		a_quat			)												const	;
			CQuaternion			GetProduct			(		float				a_fFactor		)												const	;
			CQuaternion			GetProduct			( const CQuaternion&		a_quatFactor	)												const	;
			CQuaternion			GetReverseProduct	( const CQuaternion&		a_quatPrefix	)												const	;
			CQuaternion			GetRotatedAxisX		(		float				a_fAngle		)												const	;
			CQuaternion			GetRotatedAxisY		(		float				a_fAngle		)												const	;
			CQuaternion			GetRotatedAxisZ		(		float				a_fAngle		)												const	;
			CQuaternion			GetRotatedLocalAxisX(		float				a_fAngle		)												const	;
			CQuaternion			GetRotatedLocalAxisY(		float				a_fAngle		)												const	;
			CQuaternion			GetRotatedLocalAxisZ(		float				a_fAngle		)												const	;
			CQuaternion			GetLerp				( const CQuaternion&		a_quatTarget	
													,		float				a_fAlpha		)												const	;
			CQuaternion			GetSlerp			( const	CQuaternion&		a_quatTarget
													,		float				a_fAlpha		)												const	;
			CVector3			GetTransformedVector( const	CVector3&			a_vecVector		)												const	;
																																				
			void				GetMatrix			(		CMatrix4&			a_matRotation	)												const	;
			void				GetMatrix3			(		CMatrix3&			a_matRotation	)												const	;
			void				GetAxisAngle		(		CAxisAngle&			a_Rotation		)												const	;
			void				GetEulerAngles		(		CEulerAngles&		a_Rotation		)												const	;
			void				GetSphericalRotation(		CSphericalRotation&	a_Rotation		)												const	;
																																			
			void				GetInverted			(		CQuaternion&		a_quatInverted		)											const	;
			void				GetNegated			(		CQuaternion&		a_quatNegated		)											const	;
			void				GetNormalized		(		CQuaternion&		a_quatNormalized	)											const	;
			void				GetSum				(		CQuaternion&		a_quatProduct		, const	CQuaternion&	a_quatFactor	)	const	;
			void				GetDifference		(		CQuaternion&		a_quatProduct		, const	CQuaternion&	a_quatFactor	)	const	;
			void				GetProduct			(		CQuaternion&		a_quatProduct		,		float			a_fFactor		)	const	;
			void				GetProduct			(		CQuaternion&		a_quatProduct		, const	CQuaternion&	a_quatFactor	)	const	;
			void				GetReverseProduct	(		CQuaternion&		a_quatReverseProduct, const	CQuaternion&	a_quatPrefix	)	const	;
			void				GetRotatedAxisX		(		CQuaternion&		a_quatRotated		, 		float			a_fAngle		)	const	;
			void				GetRotatedAxisY		(		CQuaternion&		a_quatRotated		, 		float			a_fAngle		)	const	;
			void				GetRotatedAxisZ		(		CQuaternion&		a_quatRotated		,		float			a_fAngle		)	const	;
			void				GetRotatedLocalAxisX(		CQuaternion&		a_quatRotated		,		float			a_fAngle		)	const	;
			void				GetRotatedLocalAxisY(		CQuaternion&		a_quatRotated		,		float			a_fAngle		)	const	;
			void				GetRotatedLocalAxisZ(		CQuaternion&		a_quatRotated		,		float			a_fAngle		)	const	;

	// Operators

			bool				operator ==			( const CQuaternion&		a_quatOther		)												const	;
			bool				operator !=			( const CQuaternion&		a_quatOther		)												const	;

			CQuaternion&		operator =			( const CMatrix4&			a_matRotation	)														;
			CQuaternion&		operator =			( const CMatrix3&			a_matRotation	)														;
			CQuaternion&		operator =			( const CAxisAngle&			a_Rotation		)														;
			CQuaternion&		operator =			( const CEulerAngles&		a_Rotation		)														;
			CQuaternion&		operator =			( const CSphericalRotation&	a_Rotation		)														;

			CQuaternion&		operator *=			(		float				a_fFactor		)														;
			CQuaternion&		operator /=			(		float				a_fFactor		)														;
			CQuaternion&		operator *=			( const CQuaternion&		a_quatRotation	)														;
			CQuaternion&		operator +=			( const CQuaternion&		a_quatRotation	)														;
			CQuaternion&		operator -=			( const CQuaternion&		a_quatRotation	)														;

			CQuaternion			operator  ~			( void )																					const	;
			CQuaternion			operator  -			( void )																					const	;

			CQuaternion			operator  *			(		float				a_fFactor		)												const	;
			CQuaternion			operator  /			(		float				a_fFactor		)												const	;
			CQuaternion			operator  +			( const CQuaternion&		a_quatRotation	)												const	;
			CQuaternion			operator  -			( const CQuaternion&		a_quatRotation	)												const	;
			CQuaternion			operator  *			( const CQuaternion&		a_quatRotation	)												const	;
	friend	CQuaternion			operator  *			(		float				a_fFactor
													, const CQuaternion&		a_quatSource	)														;
																																																					
};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	AccessX
//*****************************************************************************
//
//! \brief		Returns the quaternion's x coordinate for modification purposes
//! \return		The quaternion's x coordinate
//
//*****************************************************************************
inline float& CQuaternion::AccessX( void )
{
	return m_fX;
}

//*****************************************************************************
//	AccessY
//*****************************************************************************
//
//! \brief		Returns the quaternion's y coordinate for modification purposes
//! \return		The quaternion's y coordinate
//
//*****************************************************************************
inline float& CQuaternion::AccessY( void )
{
	return m_fY;
}

//*****************************************************************************
//	AccessZ
//*****************************************************************************
//
//! \brief		Returns the quaternion's z coordinate for modification purposes
//! \return		The quaternion's z coordinate
//
//*****************************************************************************
inline float& CQuaternion::AccessZ( void )
{
	return m_fZ;
}

//*****************************************************************************
//	AccessW
//*****************************************************************************
//
//! \brief		Returns the quaternion's w coordinate for modification purposes
//! \return		The quaternion's w coordinate
//
//*****************************************************************************
inline float& CQuaternion::AccessW( void )
{
	return m_fW;
}

//*****************************************************************************
//	GetX
//*****************************************************************************
//
//! \brief		Returns the quaternion's x coordinate for look-up purposes
//! \return		The quaternion's x coordinate
//
//*****************************************************************************
inline float CQuaternion::GetX( void ) const			
{
	return m_fX;
}

//*****************************************************************************
//	GetY
//*****************************************************************************
//
//! \brief		Returns the quaternion's y coordinate for look-up purposes
//! \return		The quaternion's y coordinate
//
//*****************************************************************************
inline float CQuaternion::GetY( void ) const			
{
	return m_fY;
}

//*****************************************************************************
//	GetZ
//*****************************************************************************
//
//! \brief		Returns the quaternion's z coordinate for look-up purposes
//! \return		The quaternion's z coordinate
//
//*****************************************************************************
inline float CQuaternion::GetZ( void ) const			
{
	return m_fZ;
}

//*****************************************************************************
//	GetW
//*****************************************************************************
//
//! \brief		Returns the quaternion's w coordinate for look-up purposes
//! \return		The quaternion's w coordinate
//
//*****************************************************************************
inline float CQuaternion::GetW( void ) const			
{
	return m_fW;
}

//*****************************************************************************
//	GetX
//*****************************************************************************
//
//! \brief		Retrieves the quaternion's x coordinate
//! \param		[out] The quaternion's x coordinate
//
//*****************************************************************************
inline void CQuaternion::GetX( float& a_fX ) const
{
	a_fX = m_fX;
}

//*****************************************************************************
//	GetY
//*****************************************************************************
//
//! \brief		Retrieves the quaternion's y coordinate
//! \param		[out] The quaternion's y coordinate
//
//*****************************************************************************
inline void CQuaternion::GetY( float& a_fY ) const
{
	a_fY = m_fY;
}

//*****************************************************************************
//	GetZ
//*****************************************************************************
//
//! \brief		Retrieves the quaternion's z coordinate
//! \param		[out] The quaternion's z coordinate
//
//*****************************************************************************
inline void CQuaternion::GetZ( float& a_fZ ) const
{
	a_fZ = m_fZ;
}

//*****************************************************************************
//	GetW
//*****************************************************************************
//
//! \brief		Retrieves the quaternion's w coordinate
//! \param		[out] The quaternion's w coordinate
//
//*****************************************************************************
inline void CQuaternion::GetW( float& a_fW ) const
{
	a_fW = m_fW;
}

//*****************************************************************************
//	SetX
//*****************************************************************************
//
//! \brief		Sets the quaternion's x coordinate
//! \param		[in] The quaternion's x coordinate to set
//
//*****************************************************************************
inline void CQuaternion::SetX( float a_fX )
{
	m_fX = a_fX;
}

//*****************************************************************************
//	SetY
//*****************************************************************************
//
//! \brief		Sets the quaternion's y coordinate
//! \param		[in] The quaternion's y coordinate to set
//
//*****************************************************************************
inline void CQuaternion::SetY( float a_fY )
{
	m_fY = a_fY;
}

//*****************************************************************************
//	SetZ
//*****************************************************************************
//
//! \brief		Sets the quaternion's z coordinate
//! \param		[in] The quaternion's z coordinate to set
//
//*****************************************************************************
inline void CQuaternion::SetZ( float a_fZ )
{
	m_fZ = a_fZ;
}

//*****************************************************************************
//	SetW
//*****************************************************************************
//
//! \brief		Sets the quaternion's w coordinate
//! \param		[in] The quaternion's w coordinate to set
//
//*****************************************************************************
inline void CQuaternion::SetW( float a_fW )
{
	m_fW = a_fW;
}

//*****************************************************************************
//	Clear
//*****************************************************************************
//
//! \brief	Clears the quaternion
//
//*****************************************************************************
inline void CQuaternion::Clear()
{
	SetQuaternion( 0.0f, 0.0f, 0.0f, 1.0f );
}

//*****************************************************************************
//	ReverseMultiply
//*****************************************************************************
//
//! \brief	Multiplies a prefix by the quatertnion
//! \param	[in] The prefix to multiply by the quaternion
//
//*****************************************************************************
inline void CQuaternion::ReverseMultiply( const CQuaternion& a_quatPrefix ) 
{
	SetToProduct( a_quatPrefix, (*this) );
}

//*****************************************************************************
//	Multiply
//*****************************************************************************
//
//! \brief	Multiplies the quaternion by another one
//! \param	[in] The quaternion to multiply with
//
//*****************************************************************************
inline void CQuaternion::Multiply( const CQuaternion& a_quatFactor ) 
{
	SetToProduct( (*this), a_quatFactor );
}

//*****************************************************************************
//	RotateAxisX
//*****************************************************************************
//
//! \brief	Rotates the quaternion by the specified angle along the x axis
//! \param	[in] The angle to rotate by along the x axis
//
//*****************************************************************************
inline void CQuaternion::RotateAxisX( float a_fAngle ) 
{
	Multiply( CQuaternion( ::g_sinf( a_fAngle * 0.5f ), 0.0f, 0.0f, ::g_cosf( a_fAngle * 0.5f ) ) );
}

//*****************************************************************************
//	RotateAxisY
//*****************************************************************************
//
//! \brief	Rotates the quaternion by the specified angle along the y axis
//! \param	[in] The angle to rotate by along the y axis
//
//*****************************************************************************
inline void CQuaternion::RotateAxisY( float a_fAngle ) 
{
	Multiply( CQuaternion( 0.0f, ::g_sinf( a_fAngle * 0.5f ), 0.0f, ::g_cosf( a_fAngle * 0.5f ) ) );
}

//*****************************************************************************
//	RotateAxisZ
//*****************************************************************************
//
//! \brief	Rotates the quaternion by the specified angle along the z axis
//! \param	[in] The angle to rotate by along the z axis
//
//*****************************************************************************
inline void CQuaternion::RotateAxisZ( float a_fAngle ) 
{
	Multiply( CQuaternion( 0.0f, 0.0f, ::g_sinf( a_fAngle * 0.5f ), ::g_cosf( a_fAngle * 0.5f ) ) );
}

//*****************************************************************************
//	RotateLocalAxisX
//*****************************************************************************
//
//! \brief	Locally rotates the quaternion by the specified angle along the x axis
//! \param	[in] The angle to rotate by along the x axis
//
//*****************************************************************************
inline void CQuaternion::RotateLocalAxisX( float a_fAngle ) 
{
	ReverseMultiply( CQuaternion( ::g_sinf( a_fAngle * 0.5f ), 0.0f, 0.0f, ::g_cosf( a_fAngle * 0.5f ) ) );
}

//*****************************************************************************
//	RotateLocalAxisY
//*****************************************************************************
//
//! \brief	Locally rotates the quaternion by the specified angle along the y axis
//! \param	[in] The angle to rotate by along the y axis
//
//*****************************************************************************
inline void CQuaternion::RotateLocalAxisY( float a_fAngle ) 
{
	ReverseMultiply( CQuaternion( 0.0f, ::g_sinf( a_fAngle * 0.5f ), 0.0f, ::g_cosf( a_fAngle * 0.5f ) ) );
}

//*****************************************************************************
//	RotateLocalAxisZ
//*****************************************************************************
//
//! \brief	Locally rotates the quaternion by the specified angle along the z axis
//! \param	[in] The angle to rotate by along the z axis
//
//*****************************************************************************
inline void CQuaternion::RotateLocalAxisZ( float a_fAngle ) 
{
	ReverseMultiply( CQuaternion( 0.0f, 0.0f, ::g_sinf( a_fAngle * 0.5f ), ::g_cosf( a_fAngle * 0.5f ) ) );
}

//*****************************************************************************
//	GetMagnitude
//*****************************************************************************
//
//! \brief	Determines the quaternion's magnitude
//! \return	The magnitude
//
//*****************************************************************************
inline float CQuaternion::GetMagnitude( void ) const 
{
	return ::g_sqrtf( GetSquareMagnitude() );
}

//*****************************************************************************
//	GetSquareMagnitude
//*****************************************************************************
//
//! \brief	Determines the quaternion's square magnitude
//! \return	The square of the megnitude
//
//*****************************************************************************
inline float CQuaternion::GetSquareMagnitude( void ) const 
{
	return (  m_fX * m_fX
			+ m_fY * m_fY
			+ m_fZ * m_fZ
			+ m_fW * m_fW );
}

//*****************************************************************************
//	GetDotProduct
//*****************************************************************************
//
//! \brief	Determines the dot product of this quaternion an dthe specified one
//! \return	The result of the dotproduct
//
//*****************************************************************************
inline float CQuaternion::GetDotProduct( const CQuaternion& a_quatFactor ) const 
{
   return (	 m_fX * a_quatFactor.m_fX 
		   + m_fY * a_quatFactor.m_fY 
		   + m_fZ * a_quatFactor.m_fZ 
		   + m_fW * a_quatFactor.m_fW );
}

//*****************************************************************************
//	GetReverseProduct
//*****************************************************************************
//
//! \brief	Returns the product of the specified quaternion and this one
//! \param	[out] The calculated quaternion
//! \param	[in] The other quaternion needed for the calculation
//
//*****************************************************************************
inline void CQuaternion::GetReverseProduct( CQuaternion& a_quatReverseProduct, const CQuaternion& a_quatPrefix ) const 
{
	a_quatReverseProduct.SetToProduct( a_quatPrefix, (*this) );
}

//*****************************************************************************
//	GetProduct
//*****************************************************************************
//
//! \brief	Returns the product of this quaternion and the specified one 
//! \param	[out] The result quaternion
//! \param	[in] The quaternion to be multiplied to this one
//
//*****************************************************************************
inline void CQuaternion::GetProduct( CQuaternion& a_quatProduct, const CQuaternion& a_quatFactor ) const 
{
	a_quatProduct.SetToProduct( (*this), a_quatFactor );
}

//*****************************************************************************
//	operator Comparison
//*****************************************************************************
//
//! \brief	Comparison operator
//! \return	The method returns either of the following values
//! \retval	true	|	The quaternions are equal
//! \retval	false	|	The quaternions differ
//
//*****************************************************************************
inline bool CQuaternion::operator == ( const CQuaternion& a_quatOther ) const 
{
	return IsEqual( a_quatOther );
}

//*****************************************************************************
//	operator Comparison
//*****************************************************************************
//
//! \brief	Comparison operator
//! \return	The method returns either of the following values
//! \retval	true	|	The quaternions differ
//! \retval	false	|	The quaternions are equal
//
//*****************************************************************************
inline bool CQuaternion::operator != ( const CQuaternion& a_quatOther ) const 
{
	return ! ( IsEqual( a_quatOther ) );
}

//*****************************************************************************
//	operator Inv
//*****************************************************************************
//
//! \brief	inv operator
//! \return	An inverted copy of this quaternion
//
//*****************************************************************************
inline CQuaternion CQuaternion::operator ~ ( void ) const 
{
	return GetInverted();
}

//*****************************************************************************
//	operator Neg
//*****************************************************************************
//
//! \brief	Neg operator
//! \return	A negated copy of this quaternion
//
//*****************************************************************************
inline CQuaternion CQuaternion::operator - ( void ) const 
{
	return GetNegated();
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief	mul operator
//! \param	[in] The factor to multiply with
//! \return	The result of the multiplication
//
//*****************************************************************************
inline CQuaternion CQuaternion::operator * ( float a_fFactor ) const 
{
	return GetProduct( a_fFactor );
}

//*****************************************************************************
//	operator /
//*****************************************************************************
//
//! \brief	div operator
//! \param	[in] The factor to divide by
//! \return	The result of the division
//
//*****************************************************************************
inline CQuaternion CQuaternion::operator / ( float a_fFactor ) const 
{
	return GetProduct( 1.0f / a_fFactor );
}

//*****************************************************************************
//	operator +
//*****************************************************************************
//
//! \brief	add operator
//! \param	[in] The quaternion to add to this one
//! \return	The result of the addition
//
//*****************************************************************************
inline CQuaternion CQuaternion::operator + ( const CQuaternion& a_quatRotation ) const 
{
	return GetSum( a_quatRotation );
}

//*****************************************************************************
//	operator -
//*****************************************************************************
//
//! \brief	sub operator
//! \param	[in] The quaternion to subtract from this one
//! \return	The result of the subtraction
//
//*****************************************************************************
inline CQuaternion CQuaternion::operator - ( const CQuaternion& a_quatRotation ) const 
{
	return GetDifference( a_quatRotation );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief	mul operator
//! \param	[in] The quaternion to multiply by this one
//! \return	The result of the multiplikation
//
//*****************************************************************************
inline CQuaternion CQuaternion::operator * ( const CQuaternion& a_quatRotation ) const 
{
	return GetProduct( a_quatRotation );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief	Mul operator
//! \param	[in] The factor for the multiplication
//! \param	[in] The quaternion to be multiplied with the specified factor
//! \return	The result of multiplying the specified factor with the specified quaternion
//
//*****************************************************************************
inline CQuaternion operator * ( float a_fFactor, const CQuaternion& a_quatFactor )
{
	return a_quatFactor.GetProduct( a_fFactor );
}

#endif	// CQUATERNION_H_INCLUDED
