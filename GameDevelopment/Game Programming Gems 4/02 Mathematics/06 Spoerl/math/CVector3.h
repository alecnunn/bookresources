#ifndef			CVECTOR3_H_INCLUDED
#define			CVECTOR3_H_INCLUDED

#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once
#endif 
#endif 

class			CMatrix4		;
class			CMatrix3		;

#include		"Basics.h"

#pragma	warning( push )
#pragma warning( disable : 4201 )	// nonstandard extension used : nameless struct/union

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CVector3
//*****************************************************************************
//
//!	Representation of a 3-dimensional vector
//
//*****************************************************************************
class CVector3
{

public:

	// Enums

	enum ECoordinate								//! Coordinate indices
	{
		ECoordinate_X							,	//!< The x	(width)		coordinate
		ECoordinate_Y							,	//!< The y	(height)	coordinate	
		ECoordinate_Z							,	//!< The z	(depth)		coordinate	
		ECoordinate_Count						,	//!< The total number of coordinates
		ECoordinate_Min			= ECoordinate_X	,	//!< The first coordinate
		ECoordinate_Max			= ECoordinate_Z	,	//!< The first coordinate
		ECoordinate_ForceDWORD	= FORCE_DWORD		//!< Forces all compilers to reserve 32 bits for the enumeration
	};																												
																												
protected:

	// Attribs

	union
	{
		float m_pfCoordinates[ ECoordinate_Count ];	//!< Array representation of vector's coordinates

		struct
		{
			float	m_fX;							//!< Vector's X coordinate
			float	m_fY;							//!< Vector's Y coordinate
			float	m_fZ;							//!< Vector's Z coordinate
		};
	};

public:

	// Con-/Destruction

			 CVector3	(		float		a_fX															
						,		float		a_fY															
						,		float		a_fZ				);											
	explicit CVector3	(		float		a_fCoordinates		);											
	explicit CVector3	( const	float		a_pfCoordinates[3]	);											
			 CVector3	( const	CVector3&	a_vecSource			);											
			 CVector3	();																					
			~CVector3	();																					

	// Get/Set

	static const CVector3&	GetUp					( void )												;
			float&			AccessX					( void )												;
			float&			AccessY					( void )												;
			float&			AccessZ					( void )												;
			float(&			AccessCoordinates		( void )										  ) [3]	;
			float&			AccessCoordinate		(		int			a_iIndex )							;
			float			GetX					( void )									const		;
			float			GetY					( void )									const		;
			float			GetZ					( void )									const		;
	const	float(&			GetCoordinates			( void )									const ) [3]	;
	const	float&			GetCoordinate			(		int			a_iIndex )				const		;
			void			GetX					(		float&		a_fX )					const		;
			void			GetY					(		float&		a_fY )					const		;
			void			GetZ					(		float&		a_fZ )					const		;
			void			GetCoordinates			(		float		a_pfCoordinates[3]	)	const		;
			void			GetCoordinates			(		float&		a_fX				
													,		float&		a_fY				
													,		float&		a_fZ				)	const		;
			void			GetCoordinate			(		int			a_iIndex 
													,		float&		a_fCoordinate		)	const		;
			void			SetX					( const float&		a_fX )								;
			void			SetY					( const float&		a_fY )								;
			void			SetZ					( const float&		a_fZ )								;
			void			SetCoordinates			( const	float		a_pfCoordinates[3]	)				;
			void			SetCoordinates			(		float		a_fX
													,		float		a_fY
													,		float		a_fZ				)				;
			void			SetCoordinate			(		int			a_iIndex 
													,		float		a_fCoordinate		)				;
			void			SetVector				(		float		a_fCoordinates		)				;
			void			SetVector				( const CVector3&	a_vecSource			)				;
			void			SetVector				(		float		a_fX
													,		float		a_fY
													,		float		a_fZ				)				;

	// Modification

			void			Clear					( void )												;
			void			Invert					( void )												;
			void			Normalize				( void )												;
			void			Scale					(		float		a_fScalar		)					;
			void			Scale					( const CVector3&	a_vecScalar		)					;
			void			Translate				(		float		a_fTranslate	)					;
			void			Translate				( const CVector3&	a_vecTranslate	)					;
			void			CrossProduct			( const CVector3&	a_vecFactor		)					;
			void			Transform				( const CMatrix3&	a_matTransform	)					;
			void			Transform				( const CMatrix4&	a_matTransform	)					;
			void			TransformDirection		( const CMatrix4&	a_matTransform	)					;
			void			TransposedTransform		( const CMatrix3&	a_matTransform	)					;
			void			InvScale				(		float		a_fScalar		)					;
			void			InvScale				( const CVector3&	a_vecScalar		)					;
			void			InvTranslate			(		float		a_fScalar		)					;
			void			InvTranslate			( const CVector3&	a_vecScalar		)					;
			void			InvTransform			( const CMatrix3&	a_matTransform	)					;
			void			InvTransform			( const CMatrix4&	a_matTransform	)					;
			void			InvTransformDirection	( const CMatrix4&	a_matTransform	)					;
			void			SetCrossProduct			( const CVector3&	a_vecA		
													, const CVector3&	a_vecB			)					;
			void			SetLerp					( const CVector3&	a_vecSource		
													, const CVector3&	a_vecTarget 
													,		float		a_fAlpha		)					;
			void			SetSlerp				(		CVector3	a_vecSource		
													,		CVector3	a_vecTarget 
													,		float		a_fAlpha		)					;
			void			SetTransformed			( const CVector3&	a_vecBase		,
													  const CMatrix4&	a_matTransform	)					;
			void			SetTransformed			( const CVector3&	a_vecBase		,
													  const CMatrix3&	a_matTransform	)					;
			void			SetTransformedDirection	( const CVector3&	a_vecBase		,
													  const CMatrix4&	a_matTransform	)					;
			void			SetTransposedTransformed( const CVector3&	a_vecBase		,
													  const CMatrix3&	a_matTransform	)					;

	// Computations

	static	float			DotProduct					( const CVector3&	a_vecFactorA, 
														  const CVector3&	a_vecFactorB	)				;
	static	CVector3		CrossProduct				( const CVector3&	a_vecFactorA, 
														  const CVector3&	a_vecFactorB	)				;
			bool			IsEqual						( const CVector3&	a_vecOther		)		const	;
			bool			HasZeroMagnitude			( void )									const	;
			float			GetMagnitude				( void )									const	;
			float			GetMagnitudeNormalize		( void )											;
			float			GetSquareMagnitude			( void )									const	;
			CVector3		GetInverted					( void )									const	;
			CVector3		GetNormalized				( void )									const	;
			CVector3		GetScaled					(		float		a_fScalar		)		const	;
			CVector3		GetScaled					( const CVector3&	a_vecScalar		)		const	;
			CVector3		GetTranslated				(		float		a_fTranslate	)		const	;
			CVector3		GetTranslated				( const CVector3&	a_vecTranslate	)		const	;
			float			GetDotProduct				( const CVector3&	a_vecScalar		)		const	;
			CVector3		GetCrossProduct				( const CVector3&	a_vecScalar		)		const	;
			CVector3		GetTransformed				( const CMatrix3&	a_matTransform	)		const	;
			CVector3		GetTransformed				( const CMatrix4&	a_matTransform	)		const	;
			CVector3		GetTransformedDirection		( const CMatrix4&	a_matTransform	)		const	;
			CVector3		GetInvScaled				( const float		a_fScalar		)		const	;
			CVector3		GetInvScaled				( const CVector3&	a_vecScalar		)		const	;
			CVector3		GetInvTranslated			( const float		a_fScalar		)		const	;
			CVector3		GetInvTranslated			( const CVector3&	a_vecScalar		)		const	;
			CVector3		GetInvTransformed			( const CMatrix3&	a_matTransform	)		const	;
			CVector3		GetInvTransformed			( const CMatrix4&	a_matTransform	)		const	;
			CVector3		GetInvTransformedDirection	( const CMatrix4&	a_matTransform	)		const	;
			CVector3		GetLerp						( const CVector3&	a_vecTarget		
														,		float		a_fAlpha		)		const	;
			CVector3		GetSlerp					( const CVector3&	a_vecTarget		
														,		float		a_fAlpha		)		const	;

	// Operators

			CVector3&		operator	=				( const CVector3&	a_vecSource		)				;
			CVector3&		operator	+=				( const CVector3&	a_vecTranslate	)				;
			CVector3&		operator	-=				( const CVector3&	a_vecTranslate	)				;
			CVector3&		operator	*=				(		float		a_fScale		)				;
			CVector3&		operator	/=				(		float		a_fScale		)				;
			CVector3&		operator	*=				( const	CMatrix3&	a_matTransform	)				;
			CVector3&		operator	/=				( const	CMatrix3&	a_matTransform	)				;
			CVector3&		operator	*=				( const	CMatrix4&	a_matTransform	)				;
			CVector3&		operator	/=				( const	CMatrix4&	a_matTransform	)				;
			CVector3		operator	-				( void )									const	;
			CVector3		operator	+				( const CVector3&	a_vecTranslate	)		const	;
			CVector3		operator	-				( const CVector3&	a_vecTranslate	)		const	;
			CVector3		operator	*				(		float		a_fScale		)		const	;
			CVector3		operator	/				(		float		a_fScale		)		const	;
			CVector3		operator	*				( const	CMatrix4&	a_matTransform	)		const	;
			CVector3		operator	/				( const	CMatrix4&	a_matTransform	)		const	;
	friend	CVector3		operator	*				(		float		a_fScale		
														, const CVector3&	a_vecSource		)				;
	friend	CVector3		operator	/				(		float		a_fScale		
														, const CVector3&	a_vecSource		)				;
			bool			operator	==				( const CVector3&	a_vecOther		)		const	;
			bool			operator	!=				( const CVector3&	a_vecOther		)		const	;
	const	float&			operator	[]				(		int			a_iIndex		)		const	;
			float&			operator	[]				(		int			a_iIndex		)				;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#pragma warning( pop )

//*****************************************************************************
//	AccessX
//*****************************************************************************
//
//! \brief		Returns the vector's x coordinate for modification purposes
//! \return		The vector's x coordinate
//
//*****************************************************************************
inline float& CVector3::AccessX( void )
{
	return m_fX;
}

//*****************************************************************************
//	AccessY
//*****************************************************************************
//
//! \brief		Returns the vector's y coordinate for modification purposes
//! \return		The vector's y coordinate
//
//*****************************************************************************
inline float& CVector3::AccessY( void )
{
	return m_fY;
}

//*****************************************************************************
//	AccessZ
//*****************************************************************************
//
//! \brief		Returns the vector's z coordinate for modification purposes
//! \return		The vector's z coordinate
//
//*****************************************************************************
inline float& CVector3::AccessZ( void )
{
	return m_fZ;
}

//*****************************************************************************
//	AccessCoordinates
//*****************************************************************************
//
//! \brief		Returns the vector's coordinates for modification purposes
//! \return		The vector's coordinates
//
//*****************************************************************************
inline float (&CVector3::AccessCoordinates( void )) [3]
{
	return m_pfCoordinates;
}

//*****************************************************************************
//	AccessCoordinate
//*****************************************************************************
//
//! \brief		Returns the specified coordinate for modification purposes
//! \param		[in] The desired coordinate's index
//! \return		The desired coordinate
//
//*****************************************************************************
inline float& CVector3::AccessCoordinate( int a_iCoordinate )
{
	return m_pfCoordinates[ a_iCoordinate ];
}

//*****************************************************************************
//	GetX
//*****************************************************************************
//
//! \brief		Returns the vector's x coordinate for look-up purposes
//! \return		The vector's x coordinate
//
//*****************************************************************************
inline float CVector3::GetX( void ) const			
{
	return m_fX;
}

//*****************************************************************************
//	GetY
//*****************************************************************************
//
//! \brief		Returns the vector's y coordinate for look-up purposes
//! \return		The vector's y coordinate
//
//*****************************************************************************
inline float CVector3::GetY( void ) const			
{
	return m_fY;
}

//*****************************************************************************
//	GetZ
//*****************************************************************************
//
//! \brief		Returns the vector's z coordinate for look-up purposes
//! \return		The vector's z coordinate
//
//*****************************************************************************
inline float CVector3::GetZ( void ) const			
{
	return m_fZ;
}

//*****************************************************************************
//	GetCoordinates
//*****************************************************************************
//
//! \brief		Returns the vector's coordinates for look-up purposes
//! \return		The vector's coordinates
//
//*****************************************************************************
inline const float (&CVector3::GetCoordinates( void ) const) [3]
{
	return m_pfCoordinates;
}

//*****************************************************************************
//	GetCoordinate
//*****************************************************************************
//
//! \brief		Returns the specified coordinate for look-up purposes
//! \param		[in] The desired coordinate's index
//! \return		The desired coordinate
//
//*****************************************************************************
inline const float& CVector3::GetCoordinate( int a_iCoordinate ) const
{
	return m_pfCoordinates[ a_iCoordinate ];
}

//*****************************************************************************
//	GetX
//*****************************************************************************
//
//! \brief		Retrieves the vector's x coordinate
//! \param		[out] The vector's x coordinate
//
//*****************************************************************************
inline void CVector3::GetX( float& a_fX ) const
{
	a_fX = m_fX;
}

//*****************************************************************************
//	GetY
//*****************************************************************************
//
//! \brief		Retrieves the vector's y coordinate
//! \param		[out] The vector's y coordinate
//
//*****************************************************************************
inline void CVector3::GetY( float& a_fY ) const
{
	a_fY = m_fY;
}

//*****************************************************************************
//	GetZ
//*****************************************************************************
//
//! \brief		Retrieves the vector's z coordinate
//! \param		[out] The vector's z coordinate
//
//*****************************************************************************
inline void CVector3::GetZ( float& a_fZ ) const
{
	a_fZ = m_fZ;
}

//*****************************************************************************
//	GetCoordinate
//*****************************************************************************
//
//! \brief		Returns the specified coordinate for modification purposes
//! \param		[in] The desired coordinate's index
//! \param		[out] The desired coordinate
//
//*****************************************************************************
inline void CVector3::GetCoordinate( int a_iCoordinate, float& a_fCoordinate ) const
{
	a_fCoordinate = m_pfCoordinates[ a_iCoordinate ];
}

//*****************************************************************************
//	SetX
//*****************************************************************************
//
//! \brief		Sets the vector's x coordinate
//! \param		[in] The vector's x coordinate to set
//
//*****************************************************************************
inline void CVector3::SetX( const float& a_fX )
{
	m_fX = a_fX;
}

//*****************************************************************************
//	SetY
//*****************************************************************************
//
//! \brief		Sets the vector's y coordinate
//! \param		[in] The vector's y coordinate to set
//
//*****************************************************************************
inline void CVector3::SetY( const float& a_fY )
{
	m_fY = a_fY;
}

//*****************************************************************************
//	SetZ
//*****************************************************************************
//
//! \brief		Sets the vector's z coordinate
//! \param		[in] The vector's z coordinate to set
//
//*****************************************************************************
inline void CVector3::SetZ( const float& a_fZ )
{
	m_fZ = a_fZ;
}

//*****************************************************************************
//	SetCoordinate
//*****************************************************************************
//
//! \brief		Sets the specified coordinate
//! \param		[in] The index of the coordinate's to set
//! \param		[in] The coordinate's value
//
//*****************************************************************************
inline void CVector3::SetCoordinate( int a_iCoordinate, float a_fCoordinate )
{
	m_pfCoordinates[ a_iCoordinate ] = a_fCoordinate;
}

//*****************************************************************************
//	SetVector
//*****************************************************************************
//
//! \brief		Sets the vector's coordinates
//! \param		[in] The value to set at all coordinates
//
//*****************************************************************************
inline void CVector3::SetVector( float a_fCoordinates )
{
	SetCoordinates( a_fCoordinates, a_fCoordinates, a_fCoordinates );
}

//*****************************************************************************
//	SetVector
//*****************************************************************************
//
//! \brief		Sets the vector's coordinates
//! \param		[in] The vector's coordinates to set
//
//*****************************************************************************
inline void CVector3::SetVector( float a_fX, float a_fY, float a_fZ )
{
	SetCoordinates( a_fX, a_fY, a_fZ );
}

//*****************************************************************************
//	CrossProduct
//*****************************************************************************
//
//! \brief		Cross-multiplies this vector with the specified vector
//! \param		[in] The vector to cross-multiply by
//
//*****************************************************************************
inline void CVector3::CrossProduct( const CVector3& a_vecFactor )
{
	SetVector( GetCrossProduct( a_vecFactor ) );
}

//*****************************************************************************
//	Transform
//*****************************************************************************
//
//! \brief		Transforms the vector by the specified matrix (matrix multiplication). 
//!				The vector is treated as a 4-dimensional vector whose last element is 
//!				considered to be 1.0. (location vector)
//! \param		[in] The matrix to transform by
//
//*****************************************************************************
inline void CVector3::Transform( const CMatrix4& a_matTransform )
{
	SetVector( GetTransformed( a_matTransform ) );
}

//*****************************************************************************
//	TransformDirection
//*****************************************************************************
//
//! \brief		Transforms the vector by the specified matrix (matrix multiplication). 
//!				The vector is treated as a 4-dimensional vector whose last element is 
//!				considered to be 0.0. (direction vector)
//! \param		[in] The matrix to transform by
//
//*****************************************************************************
inline void CVector3::TransformDirection( const CMatrix4& a_matTransform )
{
	SetVector( GetTransformedDirection( a_matTransform ) );
}

//*****************************************************************************
//	InvScale
//*****************************************************************************
//
//! \brief		Scales the vector by the inverse of the specified factor (outer/scalar product)
//! \param		[in] The factor to scale by
//
//*****************************************************************************
inline void CVector3::InvScale( float a_fScalar )
{
	Scale( 1.0f / a_fScalar );
}

//*****************************************************************************
//	InvTransform
//*****************************************************************************
//
//! \brief		Transforms the vector by the inverse of the specified matrix
//! \param		[in] The matrix to transform by
//
//*****************************************************************************
inline void CVector3::InvTransform( const CMatrix4& a_matTransform )
{
	SetVector( GetInvTransformed( a_matTransform ) );
}

//*****************************************************************************
//	InvTransformDirection
//*****************************************************************************
//
//! \brief		Transforms the vector by the inverse of the specified matrix; treat it as a direction vector.
//! \param		[in] The matrix to transform by
//
//*****************************************************************************
inline void CVector3::InvTransformDirection( const CMatrix4& a_matTransform )
{
	SetVector( GetInvTransformedDirection( a_matTransform ) );
}

//*****************************************************************************
//	DotProduct
//*****************************************************************************
//
//! \brief		Returns the outer/dot product of the specified vectors
//! \param		[in] A vector
//! \param		[in] A vector
//! \return		The dot product
//
//*****************************************************************************
inline float CVector3::DotProduct( const CVector3& a_vecFactorA, const CVector3& a_vecFactorB )
{
	return a_vecFactorA.GetDotProduct( a_vecFactorB );
}

//*****************************************************************************
//	HasZeroMagnitude
//*****************************************************************************
//
//! \brief		Determines whether or not the vector has no magnitude
//! \return		The method returns either of the following values
//! \retval		true	| The vector has no magnitude
//! \retval		false	| The vector has a magnitude
//
//*****************************************************************************
inline bool CVector3::HasZeroMagnitude( void ) const
{
	return ( 0.0f == m_fX )
		&& ( 0.0f == m_fY )
		&& ( 0.0f == m_fZ );
}

//*****************************************************************************
//	GetMagnitude
//*****************************************************************************
//
//! \brief		Returns the vector's magnitude (length)
//! \return		The vector's magnitude
//
//*****************************************************************************
inline float CVector3::GetMagnitude( void ) const			
{
	return ::g_sqrtf( GetSquareMagnitude() );
}

//*****************************************************************************
//	GetSquareMagnitude
//*****************************************************************************
//
//! \brief		Returns the squared vector's magnitude (length)
//! \return		The squared vector's magnitude
//
//*****************************************************************************
inline float CVector3::GetSquareMagnitude( void ) const			
{
	return ( m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ );
}

//*****************************************************************************
//	GetDotProduct
//*****************************************************************************
//
//! \brief		Returns the outer/dot product of this vector and the specified vector
//! \param		[in] The factor to multiply by
//! \return		The dot product
//
//*****************************************************************************
inline float CVector3::GetDotProduct( const CVector3& a_vecFactor ) const			
{
	return	( m_fX * a_vecFactor.m_fX 
			+ m_fY * a_vecFactor.m_fY 
			+ m_fZ * a_vecFactor.m_fZ );
}

//*****************************************************************************
//	operator -
//*****************************************************************************
//
//! \brief		Neg operator
//! \return		The inverted vector
//
//*****************************************************************************
inline CVector3 CVector3::operator - () const
{	
	return GetInverted();
}

//*****************************************************************************
//	operator +
//*****************************************************************************
//
//! \brief		Add operator
//! \param		[in] The vector to translate by
//! \return		The translated vector
//
//*****************************************************************************
inline CVector3 CVector3::operator + ( const CVector3& a_vecTranslate ) const
{
	return GetTranslated( a_vecTranslate );
}

//*****************************************************************************
//	operator -
//*****************************************************************************
//
//! \brief		Sub operator
//! \param		[in] The vector to translate by
//! \return		The translated vector
//
//*****************************************************************************
inline CVector3 CVector3::operator - ( const CVector3& a_vecTranslate ) const
{	
	return GetInvTranslated( a_vecTranslate );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief		Mul operator
//! \param		[in] The factor to scale by
//! \return		The scaled vector
//
//*****************************************************************************
inline CVector3 CVector3::operator * ( float a_fScale ) const
{
	return GetScaled( a_fScale );
}

//*****************************************************************************
//	operator /
//*****************************************************************************
//
//! \brief		Div operator
//! \param		[in] The factor to scale by 
//! \return		The scaled vector
//
//*****************************************************************************
inline CVector3 CVector3::operator / ( float a_fScale ) const
{
	return GetInvScaled( a_fScale );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief		Mul operator
//! \param		[in] The matrix to transform by
//! \return		The transformed vector
//
//*****************************************************************************
inline CVector3 CVector3::operator * ( const CMatrix4& a_matTransform ) const
{
	return GetTransformed( a_matTransform );
}

//*****************************************************************************
//	operator /
//*****************************************************************************
//
//! \brief		Div operator
//! \param		[in] The matrix to transform by 
//! \return		The transformed vector
//
//*****************************************************************************
inline CVector3 CVector3::operator / ( const CMatrix4& a_matTransform ) const
{
	return GetInvTransformed( a_matTransform );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief		CVector3 | operator * | Mul operator
//! \param		[in] The factor to scale by
//! \return		The scaled vector
//
//*****************************************************************************
inline CVector3 operator * ( float a_fScale, const CVector3& a_vecSource )
{
	return ( a_vecSource * a_fScale );
}

//*****************************************************************************
//	operator /
//*****************************************************************************
//
//! \brief		CVector3 | operator / | Div operator
//! \param		[in] The factor to scale by 
//! \return		The scaled vector
//
//*****************************************************************************
inline CVector3 operator / ( float a_fScale, const CVector3& a_vecSource )
{
	return ( a_vecSource / a_fScale );
}

//*****************************************************************************
//	operator ==
//*****************************************************************************
//
//! \brief		Comparison operator
//! \param		[in] The vector to compare to
//! \return		The method returns either of the following values
//! \retval		true	| The vectors are identical
//! \retval		false	| The vectors differ
//
//*****************************************************************************
inline bool CVector3::operator == ( const CVector3& a_vecOther ) const
{
	return IsEqual( a_vecOther );
}

//*****************************************************************************
//	operator !=
//*****************************************************************************
//
//! \brief		Comparison operator
//! \param		[in] The vector to compare to
//! \return		The method returns either of the following values
//! \retval		true	| The vectors differ
//! \retval		false	| The vectors are identical
//
//*****************************************************************************
inline bool CVector3::operator != ( const CVector3& a_vecOther ) const
{
	return ( ! IsEqual( a_vecOther ) );
}

//*****************************************************************************
//	operator []
//*****************************************************************************
//
//! \brief		Array operator
//! \param		[in] The desired coordinate's index 
//! \return		The desired coordinate
//
//*****************************************************************************
inline const float& CVector3::operator [] ( int a_iIndex ) const 
{
	return GetCoordinate( a_iIndex );
}

//*****************************************************************************
//	operator []
//*****************************************************************************
//
//! \brief		Array operator
//! \param		[in] The desired coordinate's index 
//! \return		The desired coordinate
//
//*****************************************************************************
inline float& CVector3::operator [] ( int a_iIndex ) 
{
	return AccessCoordinate( a_iIndex );
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif	// CVECTOR3_H_INCLUDED
