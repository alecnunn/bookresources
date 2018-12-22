#include	"CVector3.h"
#include	"CMatrix4.h"
#include	"CMatrix3.h"

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
CVector3::CVector3( float a_fX, float a_fY, float a_fZ )
{
	SetCoordinates( a_fX, a_fY, a_fZ );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The value to set at each coordinate
//
//*****************************************************************************
CVector3::CVector3( float a_fCoordinates )
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
CVector3::CVector3( const	float	a_pfCoordinates[3] )
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
CVector3::CVector3( const CVector3& a_vecSource )
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
CVector3::CVector3()
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
CVector3::~CVector3()
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
void CVector3::GetCoordinates( float a_pfCoordinates[3] ) const
{
	a_pfCoordinates[ 0 ] = m_pfCoordinates[ 0 ];
	a_pfCoordinates[ 1 ] = m_pfCoordinates[ 1 ];
	a_pfCoordinates[ 2 ] = m_pfCoordinates[ 2 ];
}

//*****************************************************************************
//	GetCoordinates
//*****************************************************************************
//
//! \brief		Retrieves the vector's coordinates
//! \param		[out] The vector's x coordinate
//! \param		[out] The vector's y coordinate
//! \param		[out] The vector's z coordinate
//
//*****************************************************************************
void CVector3::GetCoordinates( float& a_fX, float& a_fY, float& a_fZ ) const
{
	a_fX = m_pfCoordinates[ 0 ];
	a_fY = m_pfCoordinates[ 1 ];
	a_fZ = m_pfCoordinates[ 2 ];
}

//*****************************************************************************
//	SetCoordinates
//*****************************************************************************
//
//! \brief		Sets the vector's coordinates
//! \param		[in] The vector's coordinates to set
//
//*****************************************************************************
void CVector3::SetCoordinates( const float a_pfCoordinates[3] )
{
	m_pfCoordinates[ 0 ] = a_pfCoordinates[ 0 ];
	m_pfCoordinates[ 1 ] = a_pfCoordinates[ 1 ];
	m_pfCoordinates[ 2 ] = a_pfCoordinates[ 2 ];
}

//*****************************************************************************
//	SetCoordinates
//*****************************************************************************
//
//! \brief		Sets the vector's coordinates
//! \param		[in] The x coordinate to set
//! \param		[in] The y coordinate to set
//! \param		[in] The z coordinate to set
//
//*****************************************************************************
void CVector3::SetCoordinates( float a_fX, float a_fY, float a_fZ )
{
	m_pfCoordinates[ 0 ] = a_fX;
	m_pfCoordinates[ 1 ] = a_fY;
	m_pfCoordinates[ 2 ] = a_fZ;
}

//*****************************************************************************
//	SetVector
//*****************************************************************************
//
//! \brief		Sets the vector's coordinates to the specified vector's coordinates
//! \param		[in] The vector to set
//
//*****************************************************************************
void CVector3::SetVector( const CVector3& a_vecSource )
{
	m_fX = a_vecSource.m_fX;
	m_fY = a_vecSource.m_fY;
	m_fZ = a_vecSource.m_fZ;
}

//*****************************************************************************
//	Clear
//*****************************************************************************
//
//! \brief		Clears the vector (sets all coordinates to zero)
//
//*****************************************************************************
void CVector3::Clear()
{
	m_fX = 0.0f;
	m_fY = 0.0f;
	m_fZ = 0.0f;
}

//*****************************************************************************
//	Invert
//*****************************************************************************
//
//! \brief		Reverses the vector's direction
//
//*****************************************************************************
void CVector3::Invert()
{
	m_fX = - m_fX;
	m_fY = - m_fY;
	m_fZ = - m_fZ;
}

//*****************************************************************************
//	Normalize
//*****************************************************************************
//
//! \brief		Normalizes the vector (assures magnitude == 1.0f)
//
//*****************************************************************************
void CVector3::Normalize()
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
void CVector3::Scale( float a_fScalar )
{
	m_fX *= a_fScalar;
	m_fY *= a_fScalar;
	m_fZ *= a_fScalar;
}

//*****************************************************************************
//	Scale
//*****************************************************************************
//
//! \brief		Scales each of the vector's coordinates by the specified vector's corresponding coordinate
//! \param		[in] The factor to scale by
//
//*****************************************************************************
void CVector3::Scale( const CVector3& a_vecScalar )
{
	m_fX *= a_vecScalar.m_fX;
	m_fY *= a_vecScalar.m_fY;
	m_fZ *= a_vecScalar.m_fZ;
}

//*****************************************************************************
//	Translate
//*****************************************************************************
//
//! \brief		Translates each of the vector's coordinate by the specified value
//! \param		[in] The value to translate by
//
//*****************************************************************************
void CVector3::Translate( float a_fTranslate )
{
	m_fX += a_fTranslate;
	m_fY += a_fTranslate;
	m_fZ += a_fTranslate;
}

//*****************************************************************************
//	Translate
//*****************************************************************************
//
//! \brief		Translates the vector by the specified vector (vector addition)
//! \param		[in] The vector to translate by
//
//*****************************************************************************
void CVector3::Translate( const CVector3& a_vecTranslate )
{
	m_fX += a_vecTranslate.m_fX;
	m_fY += a_vecTranslate.m_fY;
	m_fZ += a_vecTranslate.m_fZ;
}

//*****************************************************************************
//	InvScale
//*****************************************************************************
//
//! \brief		InvScales each of the vector's coordinates by the specified vector's corresponding coordinate
//! \param		[in] The factor to scale by
//
//*****************************************************************************
void CVector3::InvScale( const CVector3& a_vecScalar )
{
	m_fX /= a_vecScalar.m_fX;
	m_fY /= a_vecScalar.m_fY;
	m_fZ /= a_vecScalar.m_fZ;
}

//*****************************************************************************
//	InvTranslate
//*****************************************************************************
//
//! \brief		Translates each of the vector's coordinate by the inverse of the specified factor
//! \param		[in] The value to translate by
//
//*****************************************************************************
void CVector3::InvTranslate( float a_fTranslate )
{
	m_fX -= a_fTranslate;
	m_fY -= a_fTranslate;
	m_fZ -= a_fTranslate;
}

//*****************************************************************************
//	InvTranslate
//*****************************************************************************
//
//! \brief		Translates the vector by the inverse of the specified vector (vector subtraction)
//! \param		[in] The vector to translate by
//
//*****************************************************************************
void CVector3::InvTranslate( const CVector3& a_vecTranslate )
{
	m_fX -= a_vecTranslate.m_fX;
	m_fY -= a_vecTranslate.m_fY;
	m_fZ -= a_vecTranslate.m_fZ;
}

//*****************************************************************************
//	Transform
//*****************************************************************************
//
//! \brief		Transforms the vector by the specified matrix (matrix multiplication)
//! \param		[in] The matrix to transform by
//
//*****************************************************************************
void CVector3::Transform( const CMatrix3& a_matTransform )
{
	SetTransformed( (*this), a_matTransform );
}

//*****************************************************************************
//	TransposedTransform
//*****************************************************************************
//
//! \brief		Transforms the vector by the transposed form of the specified matrix
//! \param		[in] The matrix with whose transposed version to transform by
//
//*****************************************************************************
void CVector3::TransposedTransform( const CMatrix3& a_matTransform )
{
	SetTransposedTransformed( (*this), a_matTransform );
}

//*****************************************************************************
//	InvTransform
//*****************************************************************************
//
//! \brief		Transforms the vector by the inverse of the specified matrix
//! \param		[in] The matrix to transform by
//
//*****************************************************************************
void CVector3::InvTransform( const CMatrix3& a_matTransform )
{
	SetTransformed( (*this), a_matTransform.GetInverted() );
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
void CVector3::SetTransformed( const CVector3& a_vecBase, const CMatrix4& a_matTransform )
{
	if ( this == &a_vecBase )
	{
  		CVector3 vecTransformed;

		vecTransformed.SetTransformed( a_vecBase, a_matTransform );

		SetVector( a_vecBase );
	}
	else
	{
		//
		//	CHECK EXCENTRICITY
		//
		if ( a_matTransform.IsExcentric() )
		{
			float	fW	= a_vecBase.m_fX * a_matTransform( 0, 3 ) 
						+ a_vecBase.m_fY * a_matTransform( 1, 3 )
						+ a_vecBase.m_fZ * a_matTransform( 2, 3 )
						+				   a_matTransform( 3, 3 );


			//
			//	CHECK IDENTITY
			//
			if ( ::g_fabsf( fW ) < ::g_fEpsilon )
			{
				Clear();

				return;
			}

			m_fX	= a_vecBase.m_fX *	a_matTransform( 0, 0 ) 
					+ a_vecBase.m_fY *	a_matTransform( 1, 0 ) 
					+ a_vecBase.m_fZ *	a_matTransform( 2, 0 ) 
					+					a_matTransform( 3, 0 );

			m_fY  = a_vecBase.m_fX *	a_matTransform( 0, 1 ) 
					+ a_vecBase.m_fY *	a_matTransform( 1, 1 ) 
					+ a_vecBase.m_fZ *	a_matTransform( 2, 1 ) 
					+					a_matTransform( 3, 1 );

			m_fZ	= a_vecBase.m_fX *	a_matTransform( 0, 2 ) 
					+ a_vecBase.m_fY *	a_matTransform( 1, 2 ) 
					+ a_vecBase.m_fZ *	a_matTransform( 2, 2 ) 
					+					a_matTransform( 3, 2 );

			InvScale( fW );
		}
		else
		{
			m_fX	= a_vecBase.m_fX *	a_matTransform( 0, 0 ) 
					+ a_vecBase.m_fY *	a_matTransform( 1, 0 ) 
					+ a_vecBase.m_fZ *	a_matTransform( 2, 0 ) 
					+					a_matTransform( 3, 0 );

			m_fY  = a_vecBase.m_fX *	a_matTransform( 0, 1 ) 
					+ a_vecBase.m_fY *	a_matTransform( 1, 1 ) 
					+ a_vecBase.m_fZ *	a_matTransform( 2, 1 ) 
					+					a_matTransform( 3, 1 );

			m_fZ	= a_vecBase.m_fX *	a_matTransform( 0, 2 ) 
					+ a_vecBase.m_fY *	a_matTransform( 1, 2 ) 
					+ a_vecBase.m_fZ *	a_matTransform( 2, 2 ) 
					+					a_matTransform( 3, 2 );
		}
	}
}

//*****************************************************************************
//	SetTransformedDirection
//*****************************************************************************
//
//! \brief		Makes this vector the product of the specified matrix multiplied by the 
//!				specified vector. Treat the vector as a direction vector; ignore the matrix's translation part.
//! \param		[in] The vector to transform
//! \param		[in] The matrix to transform by
//
//
//*****************************************************************************
void CVector3::SetTransformedDirection( const CVector3& a_vecBase, const CMatrix4& a_matTransform )
{
	if ( this == &a_vecBase )
	{
  		CVector3 vecTransformed;

		vecTransformed.SetTransformed( a_vecBase, a_matTransform );

		SetVector( a_vecBase );
	}
	else
	{
		m_fX	= a_vecBase.m_fX *	a_matTransform( 0, 0 ) 
				+ a_vecBase.m_fY *	a_matTransform( 1, 0 ) 
				+ a_vecBase.m_fZ *	a_matTransform( 2, 0 ) ;

		m_fY  = a_vecBase.m_fX *	a_matTransform( 0, 1 ) 
				+ a_vecBase.m_fY *	a_matTransform( 1, 1 ) 
				+ a_vecBase.m_fZ *	a_matTransform( 2, 1 ) ; 

		m_fZ	= a_vecBase.m_fX *	a_matTransform( 0, 2 ) 
				+ a_vecBase.m_fY *	a_matTransform( 1, 2 ) 
				+ a_vecBase.m_fZ *	a_matTransform( 2, 2 ) ;
	}
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
void CVector3::SetTransformed( const CVector3& a_vecBase, const CMatrix3& a_matTransform )
{
	if ( this == &a_vecBase )
	{
  		CVector3 vecTransformed;

		vecTransformed.SetTransformed( a_vecBase, a_matTransform );

		SetVector( vecTransformed );
	}
	else
	{
		m_fX  = a_vecBase.m_fX * a_matTransform( 0, 0 ) 
				+ a_vecBase.m_fY * a_matTransform( 1, 0 ) 
				+ a_vecBase.m_fZ * a_matTransform( 2, 0 );

		m_fY  = a_vecBase.m_fX * a_matTransform( 0, 1 ) 
				+ a_vecBase.m_fY * a_matTransform( 1, 1 ) 
				+ a_vecBase.m_fZ * a_matTransform( 2, 1 );

		m_fZ  = a_vecBase.m_fX * a_matTransform( 0, 2 ) 
				+ a_vecBase.m_fY * a_matTransform( 1, 2 ) 
				+ a_vecBase.m_fZ * a_matTransform( 2, 2 );
	}
}

//*****************************************************************************
//	SetTransposedTransformed
//*****************************************************************************
//
//! \brief		Makes this vector the product of the specified matrix transposed and multiplied by the specified vector
//! \param		[in] The vector to transform
//! \param		[in] The matrix to transform by
//
//*****************************************************************************
void CVector3::SetTransposedTransformed( const CVector3& a_vecBase, const CMatrix3& a_matTransform )
{
	if ( this == &a_vecBase )
	{
  		CVector3 vecTransformed;

		vecTransformed.SetTransposedTransformed( a_vecBase, a_matTransform );

		SetVector( vecTransformed );
	}
	else
	{
		m_fX  = a_vecBase.m_fX * a_matTransform( 0,0 ) 
				+ a_vecBase.m_fY * a_matTransform( 0,1 ) 
				+ a_vecBase.m_fZ * a_matTransform( 0,2 );

		m_fY  = a_vecBase.m_fX * a_matTransform( 1,0 ) 
				+ a_vecBase.m_fY * a_matTransform( 1,1 ) 
				+ a_vecBase.m_fZ * a_matTransform( 1,2 );

		m_fZ  = a_vecBase.m_fX * a_matTransform( 2,0 ) 
				+ a_vecBase.m_fY * a_matTransform( 2,1 ) 
				+ a_vecBase.m_fZ * a_matTransform( 2,2 );
	}
}

//*****************************************************************************
//	GetUp
//*****************************************************************************
//
//! \brief		Returns the global up vector
//! \return		The global up vector
//
//*****************************************************************************
const CVector3& CVector3::GetUp( void )
{
	static CVector3 s_vecUp( 0.0f, 1.0f, 0.0f );

	return s_vecUp;
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
CVector3 CVector3::GetTransformed( const CMatrix3& a_matTransform ) const			
{
  	CVector3	vecTransformed;
				vecTransformed.SetTransformed( *this, a_matTransform );

	return vecTransformed;
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
CVector3 CVector3::GetTransformed( const CMatrix4& a_matTransform ) const			
{
  	CVector3	vecTransformed;
				vecTransformed.SetTransformed( *this, a_matTransform );

	return vecTransformed;
}

//*****************************************************************************
//	GetTransformedDirection
//*****************************************************************************
//
//! \brief		Returns a transformed copy of the vector; treat it as a direction vector.
//! \param		[in] The matrix to transform by
//! \return		The transformed copy of the vector
//
//*****************************************************************************
CVector3 CVector3::GetTransformedDirection( const CMatrix4& a_matTransform ) const			
{
  	CVector3	vecTransformed;
				vecTransformed.SetTransformedDirection( *this, a_matTransform );

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
CVector3 CVector3::GetInvTransformed( const CMatrix3& a_matTransform ) const			
{
	CMatrix3	matInverted( a_matTransform );
				matInverted.Invert();

	return GetTransformed( matInverted );
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
CVector3 CVector3::GetInvTransformed( const CMatrix4& a_matTransform ) const			
{
	CMatrix4	matInverted( a_matTransform );
				matInverted.Invert();

	return GetTransformed( matInverted );
}

//*****************************************************************************
//	GetInvTransformedDirection
//*****************************************************************************
//
//! \brief		Returns a transformed copy of the vector; treat it as a direction vector
//! \param		[in] The matrix to transform by
//! \return		The transformed copy of the vector
//! \todo		Optimize
//
//*****************************************************************************
CVector3 CVector3::GetInvTransformedDirection( const CMatrix4& a_matTransform ) const			
{
	CMatrix4	matInverted( a_matTransform );
				matInverted.Invert();

	return GetTransformedDirection( matInverted );
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
CVector3& CVector3::operator *= ( const CMatrix3& a_matTransform ) 
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
CVector3& CVector3::operator /= ( const CMatrix3& a_matTransform )
{
	InvTransform( a_matTransform );

	return (*this);
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
CVector3& CVector3::operator *= ( const CMatrix4& a_matTransform ) 
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
CVector3& CVector3::operator /= ( const CMatrix4& a_matTransform )
{
	InvTransform( a_matTransform );

	return (*this);
}

//*****************************************************************************
//	SetCrossProduct
//*****************************************************************************
//
//! \brief		Makes the vector the inner/cross product of the specified vectors
//! \param		[in] A factor
//! \param		[in] A factor
//
//*****************************************************************************
void CVector3::SetCrossProduct( const CVector3& a_vecA, const CVector3& a_vecB )
{
    m_fX = a_vecA.m_fY * a_vecB.m_fZ - a_vecA.m_fZ * a_vecB.m_fY;
    m_fY = a_vecA.m_fZ * a_vecB.m_fX - a_vecA.m_fX * a_vecB.m_fZ;
    m_fZ = a_vecA.m_fX * a_vecB.m_fY - a_vecA.m_fY * a_vecB.m_fX;
}

//*****************************************************************************
//	SetLerp
//*****************************************************************************
//
//! \brief		Makes the vector the linear interpolation of the specified vectors at the 
//	specified blend position
//! \param		[in] The starting vector
//! \param		[in] The end vector
//! \param		[in] The relative position between start and end vector [0..1]
//
//*****************************************************************************
void CVector3::SetLerp( const CVector3& a_vecSource, const CVector3& a_vecTarget, float a_fAlpha )
{
	//
	//	DO LINERAR INTERPOLATION
	//
	float fInvAlpha = 1.0f - a_fAlpha;

	SetVector( fInvAlpha * a_vecSource + a_fAlpha * a_vecTarget );
}

//*****************************************************************************
//	SetSlerp
//*****************************************************************************
//
//! \brief		Makes the vector the spherical interpolation of the specified vectors at the 
//	specified blend position
//! \param		[in] The starting vector
//! \param		[in] The end vector
//! \param		[in] The relative position between start and end vector [0..1]
//
//*****************************************************************************
void CVector3::SetSlerp( CVector3 a_vecSource, CVector3 a_vecTarget, float a_fAlpha )
{
	float fScale1		;
	float fScale2		;
	float fCosTheta	= a_vecSource.GetDotProduct( a_vecTarget );
	float fInvAlpha	= 1.0f - a_fAlpha;

	if( fCosTheta < 0.0f ) 
	{
		// Flip start quaternion
        a_vecSource.m_fX = -a_vecSource.m_fX;
        a_vecSource.m_fY = -a_vecSource.m_fY;
        a_vecSource.m_fZ = -a_vecSource.m_fZ;
		fCosTheta = -fCosTheta;
	}

	if( fCosTheta + 1.0f > 0.05f ) 
	{
		// If the quaternions are close, use linear interploation
		if( 1.0f - fCosTheta < 0.05f ) 
		{
			fScale1 = fInvAlpha;
			fScale2 = a_fAlpha;
		} 
		// Otherwise, do spherical interpolation
		else 
		{ 
			float fTheta		= ::g_acosf( fCosTheta );
			float fInvSinTheta	= 1.0f / ::g_sinf ( fTheta );

			fScale1 = ::g_sinf( fTheta * fInvAlpha );
			fScale2 = ::g_sinf( fTheta * a_fAlpha  );

			fScale1 *= fInvSinTheta;
			fScale2 *= fInvSinTheta;
		}
	} 
	else 
	{
		a_vecTarget.m_fX = -a_vecSource.m_fY;
		a_vecTarget.m_fY =  a_vecSource.m_fX;
		a_vecTarget.m_fZ = -a_vecSource.m_fZ;

		fScale1 = ::g_sinf( ::g_fPI * ( 0.5f - a_fAlpha ) );
		fScale2 = ::g_sinf( ::g_fPI * a_fAlpha );
	}

	m_fX = fScale1 * a_vecSource.m_fX + fScale2 * a_vecTarget.m_fX;
	m_fY = fScale1 * a_vecSource.m_fY + fScale2 * a_vecTarget.m_fY;
	m_fZ = fScale1 * a_vecSource.m_fZ + fScale2 * a_vecTarget.m_fZ;
}

//*****************************************************************************
//	CrossProduct
//*****************************************************************************
//
//! \brief		Returns the inner/cross product of the specified vectors
//! \param		[in] A vector
//! \param		[in] A vector
//! \return		The cross product
//
//*****************************************************************************
CVector3 CVector3::CrossProduct( const CVector3& a_vecFactorA, const CVector3& a_vecFactorB )
{
	CVector3 vecProduct;	// no initialization
			 vecProduct.SetCrossProduct( a_vecFactorA, a_vecFactorB );

	return vecProduct;
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
bool CVector3::IsEqual( const CVector3& a_vecOther ) const
{
	if ( m_fX != a_vecOther.m_fX ) return false;
	if ( m_fY != a_vecOther.m_fY ) return false;
	if ( m_fZ != a_vecOther.m_fZ ) return false;

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
float CVector3::GetMagnitudeNormalize( void )
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
CVector3 CVector3::GetInverted( void ) const			
{
	CVector3	vecInverted( *this );
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
CVector3 CVector3::GetNormalized( void ) const			
{
	CVector3	vecNormalized( *this );
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
CVector3 CVector3::GetScaled( float a_fScalar ) const			
{
	CVector3	vecScaled( *this );
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
CVector3 CVector3::GetScaled( const CVector3& a_vecScalar ) const			
{
	CVector3	vecScaled( *this );
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
CVector3 CVector3::GetTranslated( float a_fTranslate ) const			
{
	CVector3	vecTranslated( *this );
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
CVector3 CVector3::GetTranslated( const CVector3& a_vecTranslate ) const			
{
	CVector3	vecTranslated( *this );
				vecTranslated.Translate( a_vecTranslate );

	return vecTranslated;
}

//*****************************************************************************
//	GetCrossProduct
//*****************************************************************************
//
//! \brief		Returns the inner/cross product of this vector and the specified vector
//! \param		[in] The factor to multiply by
//! \return		The cross product
//
//*****************************************************************************
CVector3 CVector3::GetCrossProduct( const CVector3& a_vecFactor ) const			
{
	CVector3	vecProduct;	// no initialization
				vecProduct.SetCrossProduct( *this, a_vecFactor );

	return vecProduct;
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
CVector3 CVector3::GetInvScaled( float a_fScalar ) const			
{
	CVector3	vecScaled( *this );
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
CVector3 CVector3::GetInvScaled( const CVector3& a_vecScalar ) const			
{
	CVector3	vecScaled( *this );
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
CVector3 CVector3::GetInvTranslated( float a_fTranslate ) const			
{
	CVector3	vecTranslated( *this );
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
CVector3 CVector3::GetInvTranslated( const CVector3& a_vecTranslate ) const			
{
	CVector3	vecTranslated( *this );
				vecTranslated.InvTranslate( a_vecTranslate );

	return vecTranslated;
}

//*****************************************************************************
//	GetLerp
//*****************************************************************************
//
//! \brief		Returns a linearily interpolated vector
//! \param		[in] The end vector
//! \param		[in] The relative position between start and end vector [0..1]
//! \return		The linearily interpolated vector
//
//*****************************************************************************
CVector3 CVector3::GetLerp( const CVector3& a_vecTarget, float a_fAlpha ) const			
{
	CVector3	vecInterpolated;
				vecInterpolated.SetLerp( *this, a_vecTarget, a_fAlpha );

	return vecInterpolated;
}

//*****************************************************************************
//	GetSlerp
//*****************************************************************************
//
//! \brief		Returns a spherically interpolated vector
//! \param		[in] The end vector
//! \param		[in] The relative position between start and end vector [0..1]
//! \return		The spherically interpolated vector
//
//*****************************************************************************
CVector3 CVector3::GetSlerp( const CVector3& a_vecTarget, float a_fAlpha ) const			
{
	CVector3	vecInterpolated;
				vecInterpolated.SetLerp( *this, a_vecTarget, a_fAlpha );

	return vecInterpolated;
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
CVector3& CVector3::operator = ( const CVector3& a_vecSource ) 
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
CVector3& CVector3::operator += ( const CVector3& a_vecTranslate ) 
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
CVector3& CVector3::operator -= ( const CVector3& a_vecTranslate ) 
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
CVector3& CVector3::operator *= ( float a_fScale ) 
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
CVector3& CVector3::operator /= ( float a_fScale ) 
{
	InvScale( a_fScale );

	return (*this);
}
