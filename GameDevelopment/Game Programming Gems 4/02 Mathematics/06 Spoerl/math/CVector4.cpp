#include	"CVector4.h"
#include	"CMatrix4.h"

//*********************************************************************************************************************
//*********************************************************************************************************************
//	Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The value to set at each coordinate
//
//*****************************************************************************
CVector4::CVector4( float a_fX, float a_fY, float a_fZ, float a_fW )
{
	SetCoordinates( a_fX, a_fY, a_fZ, a_fW );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The value to set at each coordinate
//
//*****************************************************************************
CVector4::CVector4( float a_fCoordinates )
{
	SetVector( a_fCoordinates );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The coordinates to set
//
//*****************************************************************************
CVector4::CVector4( const	float	a_pfCoordinates[4] )
{
	SetCoordinates( a_pfCoordinates );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Copy constructor
//! \param	[in] The vector to set
//
//*****************************************************************************
CVector4::CVector4( const CVector4& a_vecSource )
{
	SetVector( a_vecSource );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Default constructor	
//
//*****************************************************************************
CVector4::CVector4()
{
	Clear();
}

//*****************************************************************************
//	Destructor
//*****************************************************************************
//
//! \brief	Destructor	
//
//*****************************************************************************
CVector4::~CVector4()
{
}

//*****************************************************************************
//	GetCoordinates
//*****************************************************************************
//
//! \brief		Retrieves the vector's coordinates
//! \param		[out] The vector's coordinates
//
//*****************************************************************************
void CVector4::GetCoordinates( float a_pfCoordinates[4] ) const
{
	a_pfCoordinates[ 0 ] = m_pfCoordinates[ 0 ];
	a_pfCoordinates[ 1 ] = m_pfCoordinates[ 1 ];
	a_pfCoordinates[ 2 ] = m_pfCoordinates[ 2 ];
	a_pfCoordinates[ 3 ] = m_pfCoordinates[ 3 ];
}

//*****************************************************************************
//	GetCoordinates
//*****************************************************************************
//
//! \brief		Retrieves the vector's coordinates
//! \param		[out] The vector's x coordinate
//! \param		[out] The vector's y coordinate
//! \param		[out] The vector's z coordinate
//! \param		[out] The vector's w coordinate
//
//*****************************************************************************
void CVector4::GetCoordinates( float& a_fX, float& a_fY, float& a_fZ, float& a_fW ) const
{
	a_fX = m_pfCoordinates[ 0 ];
	a_fY = m_pfCoordinates[ 1 ];
	a_fZ = m_pfCoordinates[ 2 ];
	a_fW = m_pfCoordinates[ 3 ];
}

//*****************************************************************************
//	SetCoordinates
//*****************************************************************************
//
//! \brief		Sets the vector's coordinates
//! \param		[in] The vector's coordinates to set
//
//*****************************************************************************
void CVector4::SetCoordinates( const float a_pfCoordinates[4] )
{
	m_pfCoordinates[ 0 ] = a_pfCoordinates[ 0 ];
	m_pfCoordinates[ 1 ] = a_pfCoordinates[ 1 ];
	m_pfCoordinates[ 2 ] = a_pfCoordinates[ 2 ];
	m_pfCoordinates[ 3 ] = a_pfCoordinates[ 3 ];
}

//*****************************************************************************
//	SetCoordinates
//*****************************************************************************
//
//! \brief		Sets the vector's coordinates
//! \param		[in] The x coordinate to set
//! \param		[in] The y coordinate to set
//! \param		[in] The z coordinate to set
//! \param		[in] The w coordinate to set
//
//*****************************************************************************
void CVector4::SetCoordinates( float a_fX, float a_fY, float a_fZ, float a_fW )
{
	m_pfCoordinates[ 0 ] = a_fX;
	m_pfCoordinates[ 1 ] = a_fY;
	m_pfCoordinates[ 2 ] = a_fZ;
	m_pfCoordinates[ 3 ] = a_fW;
}

//*****************************************************************************
//	SetVector
//*****************************************************************************
//
//! \brief		Sets the vector's coordinates to the specified vector's cooridantes
//! \param		[in] The vector to set
//
//*****************************************************************************
void CVector4::SetVector( const CVector4& a_vecSource )
{
	m_fX = a_vecSource.m_fX;
	m_fY = a_vecSource.m_fY;
	m_fZ = a_vecSource.m_fZ;
	m_fW = a_vecSource.m_fW;
}

//*****************************************************************************
//	SetTransformed
//*****************************************************************************
//
//! \brief		Makes this vector the product of the specified matrix multiplied by the specified vector
//! \param		[in] The vector to transform
//! \param		[in] The matrix to transform by
//
//*****************************************************************************
void CVector4::SetTransformed( const CVector4& a_vecBase, const CMatrix4& a_matTransform )
{
	if ( this == &a_vecBase )
	{
  		CVector4 vecTransformed;

		vecTransformed.SetTransformed( a_vecBase, a_matTransform );

		SetVector( a_vecBase );
	}
	else
	{
		m_fX	= a_vecBase.m_fX *	a_matTransform( 0, 0 ) 
				+ a_vecBase.m_fY *	a_matTransform( 1, 0 ) 
				+ a_vecBase.m_fZ *	a_matTransform( 2, 0 ) 
				+ a_vecBase.m_fW *	a_matTransform( 3, 0 );

		m_fY  = a_vecBase.m_fX *	a_matTransform( 0, 1 ) 
				+ a_vecBase.m_fY *	a_matTransform( 1, 1 ) 
				+ a_vecBase.m_fZ *	a_matTransform( 2, 1 ) 
				+ a_vecBase.m_fW *	a_matTransform( 3, 1 );

		m_fZ	= a_vecBase.m_fX *	a_matTransform( 0, 2 ) 
				+ a_vecBase.m_fY *	a_matTransform( 1, 2 ) 
				+ a_vecBase.m_fZ *	a_matTransform( 2, 2 ) 
				+ a_vecBase.m_fW *	a_matTransform( 3, 2 );

		m_fW	= a_vecBase.m_fX *	a_matTransform( 0, 3 ) 
				+ a_vecBase.m_fY *	a_matTransform( 1, 3 ) 
				+ a_vecBase.m_fZ *	a_matTransform( 2, 3 ) 
				+ a_vecBase.m_fW *	a_matTransform( 3, 3 );
	}
}

//*****************************************************************************
//	GetTransformed
//*****************************************************************************
//
//! \brief		Returns a transformed copy of the vector
//! \param		[in] The matrix to transform by
//! \return		The transformed copy of the vector
//
//*****************************************************************************
CVector4 CVector4::GetTransformed( const CMatrix4& a_matTransform ) const			
{
  	CVector4	vecTransformed;
				vecTransformed.SetTransformed( *this, a_matTransform );

	return vecTransformed;
}

//*****************************************************************************
//	GetInvTransformed
//*****************************************************************************
//
//! \brief		Returns a transformed copy of the vector
//! \param		[in] The matrix to transform by
//! \return		The transformed copy of the vector
//! \todo		Optimize
//
//*****************************************************************************
CVector4 CVector4::GetInvTransformed( const CMatrix4& a_matTransform ) const			
{
	CMatrix4	matInverted( a_matTransform );
				matInverted.Invert();

	return GetTransformed( matInverted );
}

//*****************************************************************************
//	Clear
//*****************************************************************************
//
//! \brief		Clears the vector (sets all coordinates to zero)
//
//*****************************************************************************
void CVector4::Clear()
{
	m_fX = 0.0f;
	m_fY = 0.0f;
	m_fZ = 0.0f;
	m_fW = 0.0f;
}

//*****************************************************************************
//	Invert
//*****************************************************************************
//
//! \brief		Reverses the vector's direction
//
//*****************************************************************************
void CVector4::Invert()
{
	m_fX = - m_fX;
	m_fY = - m_fY;
	m_fZ = - m_fZ;
	m_fW = - m_fW;
}

//*****************************************************************************
//	Normalize
//*****************************************************************************
//
//! \brief		Normalizes the vector (assures magnitude == 1.0f)
//
//*****************************************************************************
void CVector4::Normalize()
{
	float fMagnitude = GetMagnitude();

	Scale( 1.0f / fMagnitude);
}

//*****************************************************************************
//	Scale
//*****************************************************************************
//
//! \brief		Scales the vector by the specified factor (outer/scalar product)
//! \param		[in] The factor to scale by
//
//*****************************************************************************
void CVector4::Scale( float a_fScalar )
{
	m_fX *= a_fScalar;
	m_fY *= a_fScalar;
	m_fZ *= a_fScalar;
	m_fW *= a_fScalar;
}

//*****************************************************************************
//	Scale
//*****************************************************************************
//
//! \brief		Scales each of the vector's coordinates by the specified vector's corresponding coordinate
//! \param		[in] The factor to scale by
//
//*****************************************************************************
void CVector4::Scale( const CVector4& a_vecScalar )
{
	m_fX *= a_vecScalar.m_fX;
	m_fY *= a_vecScalar.m_fY;
	m_fZ *= a_vecScalar.m_fZ;
	m_fW *= a_vecScalar.m_fW;
}

//*****************************************************************************
//	Translate
//*****************************************************************************
//
//! \brief		Translates each of the vector's coordinate by the specified value
//! \param		[in] The value to translate by
//
//*****************************************************************************
void CVector4::Translate( float a_fTranslate )
{
	m_fX += a_fTranslate;
	m_fY += a_fTranslate;
	m_fZ += a_fTranslate;
	m_fW += a_fTranslate;
}

//*****************************************************************************
//	Translate
//*****************************************************************************
//
//! \brief		Translates the vector by the specified vector (vector addition)
//! \param		[in] The vector to translate by
//
//*****************************************************************************
void CVector4::Translate( const CVector4& a_vecTranslate )
{
	m_fX += a_vecTranslate.m_fX;
	m_fY += a_vecTranslate.m_fY;
	m_fZ += a_vecTranslate.m_fZ;
	m_fW += a_vecTranslate.m_fW;
}

//*****************************************************************************
//	InvScale
//*****************************************************************************
//
//! \brief		InvScales each of the vector's coordinates by the specified vector's corresponding coordinate
//! \param		[in] The factor to scale by
//
//*****************************************************************************
void CVector4::InvScale( const CVector4& a_vecScalar )
{
	m_fX /= a_vecScalar.m_fX;
	m_fY /= a_vecScalar.m_fY;
	m_fZ /= a_vecScalar.m_fZ;
	m_fW /= a_vecScalar.m_fW;
}

//*****************************************************************************
//	InvTranslate
//*****************************************************************************
//
//! \brief		Translates each of the vector's coordinate by the inverse of the specified factor
//! \param		[in] The value to translate by
//
//*****************************************************************************
void CVector4::InvTranslate( float a_fTranslate )
{
	m_fX -= a_fTranslate;
	m_fY -= a_fTranslate;
	m_fZ -= a_fTranslate;
	m_fW -= a_fTranslate;
}

//*****************************************************************************
//	InvTranslate
//*****************************************************************************
//
//! \brief		Translates the vector by the inverse of the specified vector (vector subtraction)
//! \param		[in] The vector to translate by
//
//*****************************************************************************
void CVector4::InvTranslate( const CVector4& a_vecTranslate )
{
	m_fX -= a_vecTranslate.m_fX;
	m_fY -= a_vecTranslate.m_fY;
	m_fZ -= a_vecTranslate.m_fZ;
	m_fW -= a_vecTranslate.m_fW;
}

//*****************************************************************************
//	IsEqual
//*****************************************************************************
//
//! \brief		Determines whether or not the specified vector is equal to this one
//! \param		[in] The vector to compare to
//! \return		The method returns either of the following values
//! \retval		true	| The vectors are identical
//! \retval		false	| The vectors differ
//
//*****************************************************************************
bool CVector4::IsEqual( const CVector4& a_vecOther ) const
{
	if ( m_fX != a_vecOther.m_fX ) return false;
	if ( m_fY != a_vecOther.m_fY ) return false;
	if ( m_fZ != a_vecOther.m_fZ ) return false;
	if ( m_fW != a_vecOther.m_fW ) return false;

	return true;
}

//*****************************************************************************
//	GetMagnitudeNormalize
//*****************************************************************************
//
//! \brief		Returns the vector's original magnitude (length) and normalizes the vector
//! \return		The vector's original magnitude
//
//*****************************************************************************
float CVector4::GetMagnitudeNormalize( void )
{
	float fMagnitude = GetMagnitude();

	Scale( 1.0f / fMagnitude );

	return fMagnitude;
}

//*****************************************************************************
//	GetInverted
//*****************************************************************************
//
//! \brief		Returns an inverted copy of the vector
//! \return		The inverted copy of the vector
//
//*****************************************************************************
CVector4 CVector4::GetInverted( void ) const			
{
	CVector4	vecInverted( *this );
				vecInverted.Invert();

	return vecInverted;
}

//*****************************************************************************
//	GetNormalized
//*****************************************************************************
//
//! \brief		Returns a normalized copy of the vector
//! \return		The normalized copy of the vector
//
//*****************************************************************************
CVector4 CVector4::GetNormalized( void ) const			
{
	CVector4	vecNormalized( *this );
				vecNormalized.Normalize();

	return vecNormalized;
}

//*****************************************************************************
//	GetScaled
//*****************************************************************************
//
//! \brief		Returns a scaled copy of the vector
//! \param		[in] The factor to scale by
//! \return		The scaled copy of the vector
//
//*****************************************************************************
CVector4 CVector4::GetScaled( float a_fScalar ) const			
{
	CVector4	vecScaled( *this );
				vecScaled.Scale( a_fScalar );

	return vecScaled;
}

//*****************************************************************************
//	GetScaled
//*****************************************************************************
//
//! \brief		Returns a scaled copy of the vector
//! \param		[in] The factor to scale by
//! \return		The scaled copy of the vector
//
//*****************************************************************************
CVector4 CVector4::GetScaled( const CVector4& a_vecScalar ) const			
{
	CVector4	vecScaled( *this );
				vecScaled.Scale( a_vecScalar );

	return vecScaled;
}

//*****************************************************************************
//	GetTranslated
//*****************************************************************************
//
//! \brief		Returns a translated copy of the vector
//! \param		[in] The factor to translate by
//! \return		The translated copy of the vector
//
//*****************************************************************************
CVector4 CVector4::GetTranslated( float a_fTranslate ) const			
{
	CVector4	vecTranslated( *this );
				vecTranslated.Translate( a_fTranslate );

	return vecTranslated;
}

//*****************************************************************************
//	GetTranslated
//*****************************************************************************
//
//! \brief		Returns a translated copy of the vector
//! \param		[in] The factor to translate by
//! \return		The translated copy of the vector
//
//*****************************************************************************
CVector4 CVector4::GetTranslated( const CVector4& a_vecTranslate ) const			
{
	CVector4	vecTranslated( *this );
				vecTranslated.Translate( a_vecTranslate );

	return vecTranslated;
}

//*****************************************************************************
//	GetInvScaled
//*****************************************************************************
//
//! \brief		Returns a scaled copy of the vector
//! \param		[in] The factor to scale by
//! \return		The scaled copy of the vector
//
//*****************************************************************************
CVector4 CVector4::GetInvScaled( float a_fScalar ) const			
{
	CVector4	vecScaled( *this );
				vecScaled.InvScale( a_fScalar );

	return vecScaled;
}

//*****************************************************************************
//	GetInvScaled
//*****************************************************************************
//
//! \brief		Returns a scaled copy of the vector
//! \param		[in] The factor to scale by
//! \return		The scaled copy of the vector
//
//*****************************************************************************
CVector4 CVector4::GetInvScaled( const CVector4& a_vecScalar ) const			
{
	CVector4	vecScaled( *this );
				vecScaled.InvScale( a_vecScalar );

	return vecScaled;
}

//*****************************************************************************
//	GetInvTranslated
//*****************************************************************************
//
//! \brief		Returns a translated copy of the vector
//! \param		[in] The factor to translate by
//! \return		The translated copy of the vector
//
//*****************************************************************************
CVector4 CVector4::GetInvTranslated( float a_fTranslate ) const			
{
	CVector4	vecTranslated( *this );
				vecTranslated.InvTranslate( a_fTranslate );

	return vecTranslated;
}

//*****************************************************************************
//	GetInvTranslated
//*****************************************************************************
//
//! \brief		Returns a translated copy of the vector
//! \param		[in] The factor to translate by
//! \return		The translated copy of the vector
//
//*****************************************************************************
CVector4 CVector4::GetInvTranslated( const CVector4& a_vecTranslate ) const			
{
	CVector4	vecTranslated( *this );
				vecTranslated.InvTranslate( a_vecTranslate );

	return vecTranslated;
}

//*****************************************************************************
//	operator *=
//*****************************************************************************
//
//! \brief		Inplace mul operator
//! \param		[in] The matrix to transform by
//! \return		A reference to self
//
//*****************************************************************************
CVector4& CVector4::operator *= ( const CMatrix4& a_matTransform ) 
{
	Transform( a_matTransform );

	return (*this);
}

//*****************************************************************************
//	operator /=
//*****************************************************************************
//
//! \brief		Incplace div operator
//! \param		[in] The matrix to transform by
//! \return		A reference to self
//
//*****************************************************************************
CVector4& CVector4::operator /= ( const CMatrix4& a_matTransform )
{
	InvTransform( a_matTransform );

	return (*this);
}

//*****************************************************************************
//	operator =
//*****************************************************************************
//
//! \brief		Copy operator
//! \param		[in] The vector to copy
//! \return		A reference to self
//
//*****************************************************************************
CVector4& CVector4::operator = ( const CVector4& a_vecSource ) 
{
	SetVector( a_vecSource );

	return (*this);
} 

//*****************************************************************************
//	operator +=
//*****************************************************************************
//
//! \brief		Inplace add operator
//! \param		[in] The vector to translate by
//! \return		A reference to self
//
//*****************************************************************************
CVector4& CVector4::operator += ( const CVector4& a_vecTranslate ) 
{
	Translate( a_vecTranslate );

	return (*this);
}

//*****************************************************************************
//	operator -=
//*****************************************************************************
//
//! \brief		Inplace sub operator
//! \param		[in] The vector to translate by
//! \return		A reference to self
//
//*****************************************************************************
CVector4& CVector4::operator -= ( const CVector4& a_vecTranslate ) 
{
	InvTranslate( a_vecTranslate );

	return (*this);
} 

//*****************************************************************************
//	operator *=
//*****************************************************************************
//
//! \brief		Inplace mul operator
//! \param		[in] The factor to scale by
//! \return		A reference to self
//
//*****************************************************************************
CVector4& CVector4::operator *= ( float a_fScale ) 
{
	Scale( a_fScale );

	return (*this);
} 

//*****************************************************************************
//	operator /=
//*****************************************************************************
//
//! \brief		Inplace div operator
//! \param		[in] The factor to scale by
//! \return		A reference to self
//
//*****************************************************************************
CVector4& CVector4::operator /= ( float a_fScale ) 
{
	InvScale( a_fScale );

	return (*this);
}
