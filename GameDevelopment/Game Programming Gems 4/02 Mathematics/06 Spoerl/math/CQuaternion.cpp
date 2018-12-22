#include	"CQuaternion.h"
#include	"CAxisAngle.h"
#include	"CEulerAngles.h"
#include	"CSphericalRotation.h"
#include	"CMatrix3.h"
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
//! \param	[in] The x coordinate to set
//! \param	[in] The y coordinate to set
//! \param	[in] The z coordinate to set
//! \param	[in] The w coordinate to set
//
//*****************************************************************************
CQuaternion::CQuaternion( float a_fX, float a_fY, float a_fZ, float a_fW )
{
	SetQuaternion( a_fX, a_fY, a_fZ, a_fW );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The rotation to set
//
//*****************************************************************************
CQuaternion::CQuaternion( const CMatrix4& a_Rotation )
{
	SetQuaternion( a_Rotation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The rotation to set
//
//*****************************************************************************
CQuaternion::CQuaternion( const CMatrix3& a_Rotation )
{
	SetQuaternion( a_Rotation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The rotation to set
//
//*****************************************************************************
CQuaternion::CQuaternion( const CAxisAngle& a_Rotation )
{
	SetQuaternion( a_Rotation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The rotation to set
//
//*****************************************************************************
CQuaternion::CQuaternion( const CEulerAngles& a_Rotation )
{
	SetQuaternion( a_Rotation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The rotation to set
//
//*****************************************************************************
CQuaternion::CQuaternion( const CSphericalRotation& a_Rotation )
{
	SetQuaternion( a_Rotation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Copy constructor
//! \param	[in] The quaternion to copy
//
//*****************************************************************************
CQuaternion::CQuaternion( const CQuaternion& a_quatRotation )
{
	SetQuaternion( a_quatRotation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Default constructor
//
//*****************************************************************************
CQuaternion::CQuaternion()
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
CQuaternion::~CQuaternion()
{
}

//*****************************************************************************
//	GetMatrix
//*****************************************************************************
//
//! \brief	Determines a matrix representation of the quaternion
//
//*****************************************************************************
void CQuaternion::GetMatrix( CMatrix4& a_matRotation ) const 
{
	a_matRotation.SetToRotation( *this );
}

//*****************************************************************************
//	GetMatrix3
//*****************************************************************************
//
//! \brief	Determines a matrix representation of the quaternion
//
//*****************************************************************************
void CQuaternion::GetMatrix3( CMatrix3& a_matRotation ) const 
{
	a_matRotation.SetToRotation( *this );
}

//*****************************************************************************
//	GetAxisAngle
//*****************************************************************************
//
//! \brief	Determines an axis and angle representation of the quaternion
//
//*****************************************************************************
void CQuaternion::GetAxisAngle( CAxisAngle& a_Rotation ) const 
{
	a_Rotation.SetAxisAngle( *this );
}

//*****************************************************************************
//	GetEulerAngles
//*****************************************************************************
//
//! \brief	Determines an euler angles representation of the quaternion
//
//*****************************************************************************
void CQuaternion::GetEulerAngles( CEulerAngles& a_Rotation ) const 
{
	a_Rotation.SetEulerAngles( *this );
}

//*****************************************************************************
//	GetMatrix
//*****************************************************************************
//
//! \brief	Determines a matrix representation of the quaternion
//
//*****************************************************************************
CMatrix4 CQuaternion::GetMatrix( void ) const 
{
	CMatrix4 matRotation;

	GetMatrix( matRotation );

	return matRotation;
}

//*****************************************************************************
//	GetMatrix3
//*****************************************************************************
//
//! \brief	Determines a matrix representation of the quaternion
//
//*****************************************************************************
CMatrix3 CQuaternion::GetMatrix3( void ) const 
{
	CMatrix3 matRotation;

	GetMatrix3( matRotation );

	return matRotation;
}

//*****************************************************************************
//	GetAxisAngle
//*****************************************************************************
//
//! \brief	Determines an axis and angle representation of the quaternion
//
//*****************************************************************************
CAxisAngle CQuaternion::GetAxisAngle( void ) const 
{
	CAxisAngle rotation;

	GetAxisAngle( rotation );

	return rotation;
}

//*****************************************************************************
//	GetEulerAngles
//*****************************************************************************
//
//! \brief	Determines an euler angles representation of the quaternion
//
//*****************************************************************************
CEulerAngles CQuaternion::GetEulerAngles( void ) const 
{
	CEulerAngles rotation;

	GetEulerAngles( rotation );

	return rotation;
}

//*****************************************************************************
//	GetSphericalRotation
//*****************************************************************************
//
//! \brief	Determines a spherical rotation representation of the quaternion
//
//*****************************************************************************
CSphericalRotation CQuaternion::GetSphericalRotation( void ) const 
{
	CSphericalRotation rotation;

	GetSphericalRotation( rotation );

	return rotation;
}

//*****************************************************************************
//	GetSphericalRotation
//*****************************************************************************
//
//! \brief	Determines a spherical rotation representation of the quaternion
//
//*****************************************************************************
void CQuaternion::GetSphericalRotation( CSphericalRotation& a_Rotation ) const 
{
	float cos_angle = m_fX;
	float sin_angle = ::g_sqrtf( 1.0f - cos_angle * cos_angle );

	a_Rotation.AccessAzimuth() = ::g_acosf( cos_angle ) * 2.0f * ::g_fRadToDeg;

	if( ::g_fabsf( sin_angle ) < ::g_fEpsilon )
	{
		sin_angle = 1.0f;
	}

	float tX = m_fX / sin_angle;
	float tY = m_fY / sin_angle;
	float tZ = m_fZ / sin_angle;

	a_Rotation.AccessLatitude() = - ::g_asinf( tY );

	if( tX * tX + tZ * tZ < ::g_fEpsilon )
	{
		a_Rotation.AccessLongitude() = 0.0f;
	}
	else
	{
		a_Rotation.AccessLongitude() = ::g_atan2f( tX, tZ ) * ::g_fRadToDeg;
	}

	if( a_Rotation.AccessLongitude() < 0.0f )
	{
		a_Rotation.AccessLongitude() += 360.0f;
	}
}

//*****************************************************************************
//	GetTransformedVector
//*****************************************************************************
//
//! \brief	Returns the transformed version of the vector
//! \param	[in] The vector to transform
//! \return	the transformed vector
//!	\todo	optimize; there exists a special formula.
//
//*****************************************************************************
CVector3 CQuaternion::GetTransformedVector( const CVector3& a_vecVector ) const
{
	CMatrix3	matTrans;
				matTrans.SetToRotation( *this );

	return matTrans * a_vecVector;
}

//*****************************************************************************
//	SetQuaternion
//*****************************************************************************
//
//! \brief	Sets the quaternion to represent the rotation by the specified spherical rotation
//! \param	[in] The rotation to set
//
//*****************************************************************************
void CQuaternion::SetQuaternion( const CMatrix4& a_matRotation )
{
	float tr,s;
	float q[4];
	int i,j,k;

	int nxt[3] = { 1, 2, 0 };

	tr = a_matRotation.GetElement(0,0) + a_matRotation.GetElement(1,1) + a_matRotation.GetElement(2,2);

	// check the diagonal
	if( tr > 0.0f ) 
	{
		s = ::g_sqrtf( tr + 1.0f );

		m_fW = s * 0.5f;

		s = 0.5f / s;

		m_fX = ( a_matRotation.GetElement(1,2) - a_matRotation.GetElement(2,1) ) * s;
		m_fY = ( a_matRotation.GetElement(2,0) - a_matRotation.GetElement(0,2) ) * s;
		m_fZ = ( a_matRotation.GetElement(0,1) - a_matRotation.GetElement(1,0) ) * s;
	} 
	else 
	{
		// diagonal is negative

		i = 0;

		if( a_matRotation.GetElement(1,1) > a_matRotation.GetElement(0,0) ) i = 1;
		if( a_matRotation.GetElement(2,2) > a_matRotation.GetElement(i,i) ) i = 2;

		j = nxt[i];
		k = nxt[j];

		s = ::g_sqrtf( ( a_matRotation.GetElement(i,i) - ( a_matRotation.GetElement(j,j) + a_matRotation.GetElement(k,k) ) ) + 1.0f );

		q[i] = s * 0.5f;

		if( s != 0.0f ) s = 0.5f / s;

		q[3] = ( a_matRotation.GetElement(j,k) - a_matRotation.GetElement(k,j) ) * s;
		q[j] = ( a_matRotation.GetElement(i,j) + a_matRotation.GetElement(j,i) ) * s;
		q[k] = ( a_matRotation.GetElement(i,k) + a_matRotation.GetElement(k,i) ) * s;

		m_fX = q[0];
		m_fY = q[1];
		m_fZ = q[2];
		m_fW = q[3];
	}
}

//*****************************************************************************
//	SetQuaternion
//*****************************************************************************
//
//! \brief	Sets the quaternion to represent the rotation by the specified spherical rotation
//! \param	[in] The rotation to set
//
//*****************************************************************************
void CQuaternion::SetQuaternion( const CMatrix3& a_matRotation )
{
	float tr,s;
	float q[4];
	int i,j,k;

	int nxt[3] = { 1, 2, 0 };

	tr = a_matRotation.GetElement(0,0) + a_matRotation.GetElement(1,1) + a_matRotation.GetElement(2,2);

	// check the diagonal
	if( tr > 0.0f ) 
	{
		s = ::g_sqrtf( tr + 1.0f );

		m_fW = s * 0.5f;

		s = 0.5f / s;

		m_fX = ( a_matRotation.GetElement(1,2) - a_matRotation.GetElement(2,1) ) * s;
		m_fY = ( a_matRotation.GetElement(2,0) - a_matRotation.GetElement(0,2) ) * s;
		m_fZ = ( a_matRotation.GetElement(0,1) - a_matRotation.GetElement(1,0) ) * s;
	} 
	else 
	{
		// diagonal is negative

		i = 0;

		if( a_matRotation.GetElement(1,1) > a_matRotation.GetElement(0,0) ) i = 1;
		if( a_matRotation.GetElement(2,2) > a_matRotation.GetElement(i,i) ) i = 2;

		j = nxt[i];
		k = nxt[j];

		s = ::g_sqrtf( ( a_matRotation.GetElement(i,i) - ( a_matRotation.GetElement(j,j) + a_matRotation.GetElement(k,k) ) ) + 1.0f );

		q[i] = s * 0.5f;

		if( s != 0.0f ) s = 0.5f / s;

		q[3] = ( a_matRotation.GetElement(j,k) - a_matRotation.GetElement(k,j) ) * s;
		q[j] = ( a_matRotation.GetElement(i,j) + a_matRotation.GetElement(j,i) ) * s;
		q[k] = ( a_matRotation.GetElement(i,k) + a_matRotation.GetElement(k,i) ) * s;

		m_fX = q[0];
		m_fY = q[1];
		m_fZ = q[2];
		m_fW = q[3];
	}
}

//*****************************************************************************
//	SetQuaternion
//*****************************************************************************
//
//! \brief	Sets the quaternion to represent the rotation by the specified spherical rotation
//! \param	[in] The rotation to set
//
//*****************************************************************************
void CQuaternion::SetQuaternion( const CAxisAngle& a_Rotation )
{
	float theta	 = a_Rotation.GetAngle() * 0.5f;
	float sinTheta = ::g_sinf( theta );
	
	m_fX = sinTheta * a_Rotation.GetAxis().GetX();
	m_fY = sinTheta * a_Rotation.GetAxis().GetY();
	m_fZ = sinTheta * a_Rotation.GetAxis().GetZ();
	m_fW = ::g_cosf( theta );
}

//*****************************************************************************
//	SetQuaternion
//*****************************************************************************
//
//! \brief	Sets the quaternion to represent the rotation by the specified spherical rotation
//! \param	[in] The rotation to set
//
//*****************************************************************************
void CQuaternion::SetQuaternion( const CEulerAngles& a_Rotation )
{
    float fSinYaw   = ::g_sinf( a_Rotation.GetYaw		() * 0.5f );
    float fSinPitch = ::g_sinf( a_Rotation.GetPitch	() * 0.5f );
    float fSinRoll  = ::g_sinf( a_Rotation.GetRoll	() * 0.5f );

    float fCosYaw   = ::g_cosf( a_Rotation.GetYaw		() * 0.5f );
    float fCosPitch = ::g_cosf( a_Rotation.GetPitch	() * 0.5f );
    float fCosRoll  = ::g_cosf( a_Rotation.GetRoll	() * 0.5f );

	// Do it the lazy way: concatenate the three rotations

	CQuaternion quatRotY(	0, fSinYaw, 0, fCosYaw );
	CQuaternion quatRotX( fSinPitch, 0, 0, fCosPitch );
	CQuaternion quatRotZ( 0,0, fSinRoll, fCosRoll );

	CQuaternion quatTemp;
				quatTemp.SetToProduct( quatRotX, quatRotZ );

	SetToProduct( quatRotY, quatTemp );
}

//*****************************************************************************
//	SetQuaternion
//*****************************************************************************
//
//! \brief	Sets the quaternion to represent the rotation by the specified spherical rotation
//! \param	[in] The rotation to set
//
//*****************************************************************************
void CQuaternion::SetQuaternion( const CSphericalRotation& a_Rotation )
{
	float sin_a		= ::g_sinf( a_Rotation.GetAzimuth() * 0.5f );
	float cos_a		= ::g_cosf( a_Rotation.GetAzimuth() * 0.5f );

	float sin_lat	= ::g_sinf( a_Rotation.GetLatitude()  );
	float cos_lat	= ::g_cosf( a_Rotation.GetLatitude()  );

	float sin_long	= ::g_sinf( a_Rotation.GetLongitude() );
	float cos_long	= ::g_cosf( a_Rotation.GetLongitude() );

	m_fX = sin_a * cos_lat * sin_long;
	m_fY = sin_a * sin_lat;
	m_fZ = sin_a * sin_lat * cos_long;
	m_fW = cos_a;
}

//*****************************************************************************
//	SetQuaternion
//*****************************************************************************
//
//! \brief	Sets the quaternion to represent the rotation from src axis to dst axis
//! \param	[in] The source axis
//! \param	[in] The target axis
//
//*****************************************************************************
void CQuaternion::SetQuaternion( const CVector3& a_vecAxisSrc, const CVector3& a_vecAxisDst )
{
	//
	//	get angle
	//
    float		fCosTheta = CVector3::DotProduct( a_vecAxisSrc, a_vecAxisDst );
	CVector3	vecTemp	;

	//
    // check if parallel
	//
    if( fCosTheta > 0.99999f )
	{
		SetQuaternion( 0.0f, 0.0f, 0.0f, 1.0f );

		return;
    }
    else if( fCosTheta < -0.99999f ) // check if opposite
	{		
		//
		// check if we can use cross product of source vector and [1, 0, 0]
		//
		vecTemp.SetVector( 0.0f, a_vecAxisSrc.GetX(), -a_vecAxisSrc.GetY() );

		if( vecTemp.GetMagnitude() < ::g_fEpsilon )
		{
			//
			// nope! we need cross product of source vector and [0, 1, 0]
			//
			vecTemp.SetVector( -a_vecAxisSrc.GetZ(), 0.0f, a_vecAxisSrc.GetX() ) ;
		}

		//
		// normalize
		//
		vecTemp.Normalize();

		//
		//	set
		//
		SetQuaternion( vecTemp.GetX(), vecTemp.GetY(), vecTemp.GetZ(), 0.0f );

		return;
    }

	//
	// ... else we can just cross multiply the axis vectors
	//

	vecTemp.SetCrossProduct( a_vecAxisSrc, a_vecAxisDst );
	vecTemp.Normalize();

	//
    // we have to use half-angle formulae (sin^2 t = ( 1 - cos (2t) ) /2)
	//
	
	vecTemp *= ::g_sqrtf( 0.5f * ( 1.0f - fCosTheta ) );

	//
    // scale the axis to get the normalized quaternion
    // w part is cosine of half the rotation angle
    // cos^2 t = ( 1 + cos (2t) ) / 2
	//

	SetQuaternion( vecTemp.GetX(), vecTemp.GetY(), vecTemp.GetZ(), ::g_sqrtf( 0.5f * ( 1.0f + fCosTheta) ) );
}

//*****************************************************************************
//	SetQuaternion
//*****************************************************************************
//
//! \brief	Sets the quaternion's coordinates
//! \param	[in] The x coordinate to set
//! \param	[in] The y coordinate to set
//! \param	[in] The z coordinate to set
//! \param	[in] The w coordinate to set
//
//*****************************************************************************
void CQuaternion::SetQuaternion( float a_fX, float a_fY, float a_fZ, float a_fW )
{
	m_fX = a_fX;
	m_fY = a_fY;
	m_fZ = a_fZ;
	m_fW = a_fW;
}

//*****************************************************************************
//	SetQuaternion
//*****************************************************************************
//
//! \brief	Copies the specified quaternion
//! \param	[in] The quaternion to copy
//
//*****************************************************************************
void CQuaternion::SetQuaternion( const CQuaternion& a_quatRotation )
{
	m_fX = a_quatRotation.m_fX;
	m_fY = a_quatRotation.m_fY;
	m_fZ = a_quatRotation.m_fZ;
	m_fW = a_quatRotation.m_fW;
}

//*****************************************************************************
//	Negate
//*****************************************************************************
//
//! \brief	Negates the quaternion
//
//*****************************************************************************
void CQuaternion::Negate()
{
	m_fX = - m_fX;
	m_fY = - m_fY;
	m_fZ = - m_fZ;
	m_fW = - m_fW;
}

//*****************************************************************************
//	Invert
//*****************************************************************************
//
//! \brief	Inverts the quaternion
//
//*****************************************************************************
void CQuaternion::Invert()
{
	float fNorm		= GetSquareMagnitude();
	float fInvNorm	= 1.0f / fNorm;

	m_fX *= - fInvNorm;
	m_fY *= - fInvNorm;
	m_fZ *= - fInvNorm;
	m_fW *=   fInvNorm;
}

//*****************************************************************************
//	Normalize
//*****************************************************************************
//
//! \brief	Normalizes the quaternion
//
//*****************************************************************************
void CQuaternion::Normalize()
{
	float fSquare = GetSquareMagnitude();

	if( fSquare == 0.0f )
	{
		return;
	}

	float fDistance = 1.0f / ::g_sqrtf( fSquare );

	m_fX *= fDistance;
	m_fY *= fDistance;
	m_fZ *= fDistance;
	m_fW *= fDistance;
}


//*****************************************************************************
//	Add
//*****************************************************************************
//
//! \brief	Adds another quaternion to this one
//! \param	[in] The quaternion to add
//
//*****************************************************************************
void CQuaternion::Add( const CQuaternion& a_quat ) 
{
	AccessX() += a_quat.GetX();
	AccessY() += a_quat.GetY();
	AccessZ() += a_quat.GetZ();
	AccessW() += a_quat.GetW();
}

//*****************************************************************************
//	Sub
//*****************************************************************************
//
//! \brief	Subtractss another quaternion from this one
//! \param	[in] The quaternion to subtract
//
//*****************************************************************************
void CQuaternion::Sub( const CQuaternion& a_quat ) 
{
	AccessX() -= a_quat.GetX();
	AccessY() -= a_quat.GetY();
	AccessZ() -= a_quat.GetZ();
	AccessW() -= a_quat.GetW();
}

//*****************************************************************************
//	Multiply
//*****************************************************************************
//
//! \brief	Multiplies the quaternion by a scalar factor
//! \param	[in] The factor to multiply by
//
//*****************************************************************************
void CQuaternion::Multiply( float a_fFactor ) 
{
	AccessX() *= a_fFactor;
	AccessY() *= a_fFactor;
	AccessZ() *= a_fFactor;
	AccessW() *= a_fFactor;
}

//*****************************************************************************
//	SetToProduct
//*****************************************************************************
//
//! \brief	Sets the quaternion to the product of the specified quaternions
//! \param	[in] The left quaternion
//! \param	[in] The right quaternion
//
//*****************************************************************************
void CQuaternion::SetToProduct( const	CQuaternion& a_quatLeft, const CQuaternion&	a_quatRight	)
{
	if (	( this == &a_quatLeft  )
		 ||	( this == &a_quatRight ) )
	{
		CQuaternion quatTarget;

		quatTarget.m_fX = a_quatLeft.m_fW * a_quatRight.m_fX + a_quatLeft.m_fX * a_quatRight.m_fW + a_quatLeft.m_fY * a_quatRight.m_fZ - a_quatLeft.m_fZ * a_quatRight.m_fY;
		quatTarget.m_fY = a_quatLeft.m_fW * a_quatRight.m_fY + a_quatLeft.m_fY * a_quatRight.m_fW + a_quatLeft.m_fZ * a_quatRight.m_fX - a_quatLeft.m_fX * a_quatRight.m_fZ;
		quatTarget.m_fZ = a_quatLeft.m_fW * a_quatRight.m_fZ + a_quatLeft.m_fZ * a_quatRight.m_fW + a_quatLeft.m_fX * a_quatRight.m_fY - a_quatLeft.m_fY * a_quatRight.m_fX;
		quatTarget.m_fW = a_quatLeft.m_fW * a_quatRight.m_fW - a_quatLeft.m_fX * a_quatRight.m_fX - a_quatLeft.m_fY * a_quatRight.m_fY - a_quatLeft.m_fZ * a_quatRight.m_fZ;

		SetQuaternion( quatTarget );
	}
	else
	{
		m_fX = a_quatLeft.m_fW * a_quatRight.m_fX + a_quatLeft.m_fX * a_quatRight.m_fW + a_quatLeft.m_fY * a_quatRight.m_fZ - a_quatLeft.m_fZ * a_quatRight.m_fY;
		m_fY = a_quatLeft.m_fW * a_quatRight.m_fY + a_quatLeft.m_fY * a_quatRight.m_fW + a_quatLeft.m_fZ * a_quatRight.m_fX - a_quatLeft.m_fX * a_quatRight.m_fZ;
		m_fZ = a_quatLeft.m_fW * a_quatRight.m_fZ + a_quatLeft.m_fZ * a_quatRight.m_fW + a_quatLeft.m_fX * a_quatRight.m_fY - a_quatLeft.m_fY * a_quatRight.m_fX;
		m_fW = a_quatLeft.m_fW * a_quatRight.m_fW - a_quatLeft.m_fX * a_quatRight.m_fX - a_quatLeft.m_fY * a_quatRight.m_fY - a_quatLeft.m_fZ * a_quatRight.m_fZ;
  }
}

//*****************************************************************************
//	SetLerp
//*****************************************************************************
//
//! \brief	Sets the quaternion to the linearily interpolated quaternion of the specified quaternions
//! \param	[in] source quaternion
//! \param	[in] target quaternion
//! \param	[in] the wight factor for blending
//
//*****************************************************************************
void CQuaternion::SetLerp( CQuaternion a_quatSource, const CQuaternion& a_quatTarget, float a_fAlpha )
{
	float fScale1;
	float fScale2;

	// Compute dot product, aka cos( theta ):
	float fCosTheta = a_quatSource.GetDotProduct( a_quatTarget );

	if( fCosTheta < 0.0f )
	{
		// Flip start quaternion
        a_quatSource.m_fX = -a_quatSource.m_fX;
        a_quatSource.m_fY = -a_quatSource.m_fY;
        a_quatSource.m_fZ = -a_quatSource.m_fZ;
        a_quatSource.m_fW = -a_quatSource.m_fW;
		fCosTheta = -fCosTheta;
	}

	fScale1 = 1.0f - a_fAlpha;
	fScale2 = a_fAlpha;

	m_fX = fScale1 * a_quatSource.m_fX + fScale2 * a_quatTarget.m_fX;
	m_fY = fScale1 * a_quatSource.m_fY + fScale2 * a_quatTarget.m_fY;
	m_fZ = fScale1 * a_quatSource.m_fZ + fScale2 * a_quatTarget.m_fZ;
	m_fW = fScale1 * a_quatSource.m_fW + fScale2 * a_quatTarget.m_fW;
}

//*****************************************************************************
//	SetSlerp
//*****************************************************************************
//
//! \brief	Sets the quaternion to the linearily interpolated quaternion of the specified quaternions
//! \param	[in] source quaternion
//! \param	[in] target quaternion
//! \param	[in] the wight factor for blending
//
//*****************************************************************************
void CQuaternion::SetSlerp( CQuaternion a_quatSource, CQuaternion a_quatTarget, float a_fAlpha )
{
	float fScale1;
	float fScale2;

	// Compute dot product, aka cos( theta ):
	float fCosTheta = a_quatSource.GetDotProduct( a_quatTarget );

	if( fCosTheta < 0.0f )
	{
		// Flip start quaternion
        a_quatSource.m_fX = -a_quatSource.m_fX;
        a_quatSource.m_fY = -a_quatSource.m_fY;
        a_quatSource.m_fZ = -a_quatSource.m_fZ;
        a_quatSource.m_fW = -a_quatSource.m_fW;
		fCosTheta		  = -fCosTheta;
	}

	if( fCosTheta + 1.0f > 0.05f )
	{
		// If the quaternions are close, use linear interploation
		if( 1.0f - fCosTheta < 0.05f ) 
		{
			fScale1 = 1.0f - a_fAlpha;
			fScale2 = a_fAlpha;
		} 
		else 
		{ // Otherwise, do spherical interpolation
			float fTheta = (float)acos( fCosTheta );
			float fSinTheta = ::g_sinf( fTheta );

			fScale1 = ::g_sinf( fTheta * ( 1.0f - a_fAlpha ) ) / fSinTheta;
			fScale2 = ::g_sinf( fTheta * a_fAlpha ) / fSinTheta;
		}
	} 
	else 
	{
		a_quatTarget.m_fX = -a_quatSource.m_fY;
		a_quatTarget.m_fY =  a_quatSource.m_fX;
		a_quatTarget.m_fZ = -a_quatSource.m_fW;
		a_quatTarget.m_fW =  a_quatSource.m_fZ;

		fScale1 = ::g_sinf( ::g_fPI * ( 0.5f - a_fAlpha ) );
		fScale2 = ::g_sinf( ::g_fPI * a_fAlpha );
	}

	m_fX = fScale1 * a_quatSource.m_fX + fScale2 * a_quatTarget.m_fX;
	m_fY = fScale1 * a_quatSource.m_fY + fScale2 * a_quatTarget.m_fY;
	m_fZ = fScale1 * a_quatSource.m_fZ + fScale2 * a_quatTarget.m_fZ;
	m_fW = fScale1 * a_quatSource.m_fW + fScale2 * a_quatTarget.m_fW;
}

//*****************************************************************************
//	IsEqual
//*****************************************************************************
//
//! \brief		Determines whether or not the quaternion equals the specified one
//! \return		The method returns either of the following values
//! \retval		true	|	The quaternions are equal
//! \retval		false	|	The quaternions differ
//
//*****************************************************************************
bool CQuaternion::IsEqual( const CQuaternion& a_quatOther ) const 
{
	if ( m_fX != a_quatOther.m_fX ) return false;
	if ( m_fY != a_quatOther.m_fY ) return false;
	if ( m_fZ != a_quatOther.m_fZ ) return false;
	if ( m_fW != a_quatOther.m_fW ) return false;

	return true;
}

//*****************************************************************************
//	GetNormalized
//*****************************************************************************
//
//! \brief	Returns a normalized copy of the quaternion
//! \return	The normalized quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetNormalized( void ) const 
{
	CQuaternion quatNormalized;

	GetNormalized( quatNormalized );

	return quatNormalized;
}

//*****************************************************************************
//	GetNegated
//*****************************************************************************
//
//! \brief	Returns an negated copy of the quaternion
//! \return	The negated quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetNegated( void ) const 
{
	CQuaternion quatNegated;

	GetNegated( quatNegated );

	return quatNegated;
}

//*****************************************************************************
//	GetInverted
//*****************************************************************************
//
//! \brief	Returns an inverted copy of the quaternion
//! \return	The inverted quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetInverted( void ) const 
{
	CQuaternion quatInverted;

	GetInverted( quatInverted );

	return quatInverted;
}

//*****************************************************************************
//	GetReverseProduct
//*****************************************************************************
//
//! \brief	Returns the product of the specified quaternion and this one
//! \param	[in] A reference to a quaternion to clac reverse product to
//! \return	The resverse product with the specified quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetReverseProduct( const CQuaternion& a_quatPrefix ) const 
{
	CQuaternion quatProduct;

	quatProduct.SetToProduct( a_quatPrefix, (*this) );

	return quatProduct;
}

//*****************************************************************************
//	GetSum
//*****************************************************************************
//
//! \brief	Returns the sum of this quaternion and the specified one
//! \param	[in] A quaternion
//! \return	The sum of this and the specified quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetSum( const CQuaternion& a_quat ) const 
{
	CQuaternion quatProduct( *this );

	quatProduct.Add( a_quat );

	return quatProduct;
}

//*****************************************************************************
//	GetDifference
//*****************************************************************************
//
//! \brief	Returns the difference of this quaternion and the specified one
//! \param	[in] A quaternion
//! \return	The difference of this and the specified quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetDifference( const CQuaternion& a_quat ) const 
{
	CQuaternion quatProduct( *this );

	quatProduct.Sub( a_quat );

	return quatProduct;
}

//*****************************************************************************
//	GetProduct
//*****************************************************************************
//
//! \brief	Returns the product of the quaternion and the specified scalar factor
//! \param	[in] A factor to multiply with
//! \return	The product of this quaternion and the specified factor
//
//*****************************************************************************
CQuaternion CQuaternion::GetProduct( float a_fFactor ) const 
{
	CQuaternion quatProduct( *this );

	quatProduct.Multiply( a_fFactor );

	return quatProduct;
}

//*****************************************************************************
//	GetProduct
//*****************************************************************************
//
//! \brief	Returns the product of this quaternion and the specified one
//! \param	[in] A quaternion to multiply with
//! \return	The product of this quaternion and the specified quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetProduct( const CQuaternion& a_quatFactor ) const 
{
	CQuaternion quatProduct;

	quatProduct.SetToProduct( (*this), a_quatFactor );

	return quatProduct;
}

//*****************************************************************************
//	GetRotatedAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[in] The angle
//! \return	A rotated quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetRotatedAxisX( float a_fAngle ) const 
{
	CQuaternion quatRotated;

	GetRotatedAxisX( quatRotated, a_fAngle );

	return quatRotated;
}

//*****************************************************************************
//	GetRotatedAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[in] The angle
//! \return	A rotated quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetRotatedAxisY( float a_fAngle ) const 
{
	CQuaternion quatRotated;

	GetRotatedAxisY( quatRotated, a_fAngle );

	return quatRotated;
}

//*****************************************************************************
//	GetRotatedAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[in] The angle
//! \return	A rotated quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetRotatedAxisZ( float a_fAngle ) const 
{
	CQuaternion quatRotated;

	GetRotatedAxisZ( quatRotated, a_fAngle );

	return quatRotated;
}

//*****************************************************************************
//	GetRotatedLocalAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[in] The angle
//! \return	A rotated quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetRotatedLocalAxisX( float a_fAngle ) const 
{
	CQuaternion quatRotated;

	GetRotatedLocalAxisX( quatRotated, a_fAngle );

	return quatRotated;
}

//*****************************************************************************
//	GetRotatedLocalAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[in] The angle
//! \return	A rotated quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetRotatedLocalAxisY( float a_fAngle ) const 
{
	CQuaternion quatRotated;

	GetRotatedLocalAxisY( quatRotated, a_fAngle );

	return quatRotated;
}

//*****************************************************************************
//	GetRotatedLocalAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[in] The angle
//! \return	A rotated quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetRotatedLocalAxisZ( float a_fAngle ) const 
{
	CQuaternion quatRotated;

	GetRotatedLocalAxisZ( quatRotated, a_fAngle );

	return quatRotated;
}

//*****************************************************************************
//	GetLerp
//*****************************************************************************
//
//! \brief	Returns a spherically interpolated quaternion
//! \param	[in] The target quaternion
//! \param	[in] The weight for blening
//! \return	The interpolated quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetLerp( const CQuaternion& a_quatTarget, float a_fAlpha ) const 
{
	CQuaternion quatInterpolated;

	quatInterpolated.SetLerp( (*this), a_quatTarget, a_fAlpha );

	return quatInterpolated;
}

//*****************************************************************************
//	GetSlerp
//*****************************************************************************
//
//! \brief	Returns a linearily interpolated quaternion
//! \param	[in] The target quaternion
//! \param	[in] The weight for blening
//! \return	The interpolated quaternion
//
//*****************************************************************************
CQuaternion CQuaternion::GetSlerp( const CQuaternion& a_quatTarget, float a_fAlpha ) const 
{
	CQuaternion quatInterpolated;

	quatInterpolated.SetSlerp( (*this), a_quatTarget, a_fAlpha );

	return quatInterpolated;
}

//*****************************************************************************
//	GetNormalized
//*****************************************************************************
//
//! \brief	Returns a normalized copy of the quaternion
//! \param	[out] The normalized quaternion
//
//*****************************************************************************
void CQuaternion::GetNormalized( CQuaternion& a_quatNormalized ) const 
{
	a_quatNormalized.SetQuaternion( *this );
	a_quatNormalized.Normalize();
}

//*****************************************************************************
//	GetNegated
//*****************************************************************************
//
//! \brief	Returns an negated copy of the quaternion
//! \param	[out] The negated quaternion
//
//*****************************************************************************
void CQuaternion::GetNegated( CQuaternion& a_quatNegated ) const 
{
	a_quatNegated.SetQuaternion( *this );
	a_quatNegated.Negate();
}

//*****************************************************************************
//	GetInverted
//*****************************************************************************
//
//! \brief	Returns an inverted copy of the quaternion
//! \param	[out] The inverted quaternion
//
//*****************************************************************************
void CQuaternion::GetInverted( CQuaternion& a_quatInverted ) const 
{
	a_quatInverted.SetQuaternion( *this );
	a_quatInverted.Invert();
}

//*****************************************************************************
//	GetSum
//*****************************************************************************
//
//! \brief	Returns the sum of this quaternion and the specified one 
//! \param	[out] The sum quaternion
//! \param	[in] The other quaternion for the addition
//
//*****************************************************************************
void CQuaternion::GetSum( CQuaternion& a_quatSum, const CQuaternion& a_quat ) const 
{
	a_quatSum.SetQuaternion( *this );
	a_quatSum.Add( a_quat );
}

//*****************************************************************************
//	GetDifference
//*****************************************************************************
//
//! \brief	Returns the difference of this quaternion and the specified one 
//! \param	[out] The difference quaternion
//! \param	[in] The quaternion to subtract from this
//
//*****************************************************************************
void CQuaternion::GetDifference( CQuaternion& a_quatDifference, const CQuaternion& a_quat ) const 
{
	a_quatDifference.SetQuaternion( *this );
	a_quatDifference.Sub( a_quat );
}

//*****************************************************************************
//	GetProduct
//*****************************************************************************
//
//! \brief	Returns the product of this quaternion and the specified scalar factor
//! \param	[out] The result quaternion
//! \param	[in] The factor to multiply this quaternion with
//
//*****************************************************************************
void CQuaternion::GetProduct( CQuaternion& a_quatProduct, float a_fFactor ) const 
{
	a_quatProduct.SetQuaternion( *this );
	a_quatProduct.Multiply( a_fFactor );
}

//*****************************************************************************
//	GetRotatedAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[out] The rotated quaternion
//! \param	[in] The angle
//
//*****************************************************************************
void CQuaternion::GetRotatedAxisX( CQuaternion& a_quatRotated, float a_fAngle ) const 
{
	a_quatRotated.SetQuaternion( *this );
	a_quatRotated.RotateAxisX( a_fAngle );
}

//*****************************************************************************
//	GetRotatedAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[out] The rotated quaternion
//! \param	[in] The angle
//
//*****************************************************************************
void CQuaternion::GetRotatedAxisY( CQuaternion& a_quatRotated, float a_fAngle ) const 
{
	a_quatRotated.SetQuaternion( *this );
	a_quatRotated.RotateAxisY( a_fAngle );
}

//*****************************************************************************
//	GetRotatedAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[out] The rotated quaternion
//! \param	[in] The angle
//
//*****************************************************************************
void CQuaternion::GetRotatedAxisZ( CQuaternion& a_quatRotated, float a_fAngle ) const 
{
	a_quatRotated.SetQuaternion( *this );
	a_quatRotated.RotateAxisZ( a_fAngle );
}

//*****************************************************************************
//	GetRotatedLocalAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[out] The rotated quaternion
//! \param	[in] The angle
//
//*****************************************************************************
void CQuaternion::GetRotatedLocalAxisX( CQuaternion& a_quatRotated, float a_fAngle ) const 
{
	a_quatRotated.SetQuaternion( *this );
	a_quatRotated.RotateLocalAxisX( a_fAngle );
}

//*****************************************************************************
//	GetRotatedLocalAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[out] The rotated quaternion
//! \param	[in] The angle
//
//*****************************************************************************
void CQuaternion::GetRotatedLocalAxisY( CQuaternion& a_quatRotated, float a_fAngle ) const 
{
	a_quatRotated.SetQuaternion( *this );
	a_quatRotated.RotateLocalAxisY( a_fAngle );
}

//*****************************************************************************
//	GetRotatedLocalAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the quaternion
//! \param	[out] The rotated quaternion
//! \param	[in] The angle
//
//*****************************************************************************
void CQuaternion::GetRotatedLocalAxisZ( CQuaternion& a_quatRotated, float a_fAngle ) const 
{
	a_quatRotated.SetQuaternion( *this );
	a_quatRotated.RotateLocalAxisZ( a_fAngle );
}

//*****************************************************************************
//	operator Assignment
//*****************************************************************************
//
//! \brief	Assignment operator
//! \param	[in] A rotation matrix to set this quaternion by
//! \return	A reference to self
//
//*****************************************************************************
CQuaternion& CQuaternion::operator = ( const CMatrix4& a_matRotation ) 
{
	SetQuaternion( a_matRotation );

	return (*this);
}

//*****************************************************************************
//	operator Assignment
//*****************************************************************************
//
//! \brief	Assignment operator
//! \param	[in] A rotation matrix to set this quaternion by
//! \return	A reference to self
//
//*****************************************************************************
CQuaternion& CQuaternion::operator = ( const CMatrix3& a_matRotation ) 
{
	SetQuaternion( a_matRotation );

	return (*this);
}

//*****************************************************************************
//	operator Assignment
//*****************************************************************************
//
//! \brief	Assignment operator
//! \param	[in] An  axisangle to set this quaternion by
//! \return	A reference to self
//
//*****************************************************************************
CQuaternion& CQuaternion::operator = ( const CAxisAngle& a_Rotation ) 
{
	SetQuaternion( a_Rotation );

	return (*this);
}

//*****************************************************************************
//	operator Assignment
//*****************************************************************************
//
//! \brief	Assignment operator
//! \param	[in] An eulerangle to set this quaternion by
//! \return	A reference to self
//
//*****************************************************************************
CQuaternion& CQuaternion::operator = ( const CEulerAngles& a_Rotation ) 
{
	SetQuaternion( a_Rotation );

	return (*this);
}

//*****************************************************************************
//	operator Assignment
//*****************************************************************************
//
//! \brief	Assignment operator
//! \param	[in] A spherical rotation to set this quaternion by
//! \return	A reference to self
//
//*****************************************************************************
CQuaternion& CQuaternion::operator = ( const CSphericalRotation& a_Rotation ) 
{
	SetQuaternion( a_Rotation );

	return (*this);
}

//*****************************************************************************
//	operator_Inplace_Mul
//*****************************************************************************
//
//! \brief	Inplace mul operator
//! \param	[in] The factor to multiply with
//! \return	A reference to self
//
//*****************************************************************************
CQuaternion& CQuaternion::operator *= ( float a_fFactor ) 
{
	Multiply( a_fFactor );

	return (*this);
}

//*****************************************************************************
//	operator_Inplace_Div
//*****************************************************************************
//
//! \brief	Inplace div operator
//! \param	[in] The factor to divide by
//! \return	A reference to self
//
//*****************************************************************************
CQuaternion& CQuaternion::operator /= ( float a_fFactor ) 
{
	Multiply( 1.0f / a_fFactor );

	return (*this);
}

//*****************************************************************************
//	operator_Inplace_Mul
//*****************************************************************************
//
//! \brief	Inplace mul operator
//! \param	[in] The quaternion to multiply with
//! \return	A reference to self
//
//*****************************************************************************
CQuaternion& CQuaternion::operator *= ( const CQuaternion& a_quatRotation ) 
{
	Multiply( a_quatRotation );

	return (*this);
}

//*****************************************************************************
//	operator_Inplace_Add
//*****************************************************************************
//
//! \brief	Inplace add operator
//! \param	[in] The quaternion to add to 
//! \return	A reference to self
//
//*****************************************************************************
CQuaternion& CQuaternion::operator += ( const CQuaternion& a_quatRotation ) 
{
	Add( a_quatRotation );

	return (*this);
}

//*****************************************************************************
//	operator_Inplace_Sub
//*****************************************************************************
//
//! \brief	Inplace sub operator
//! \param	[in] The quaternion to subtract from this
//! \return	A reference to self
//
//*****************************************************************************
CQuaternion& CQuaternion::operator -= ( const CQuaternion& a_quatRotation ) 
{
	Sub( a_quatRotation );

	return (*this);
}
