#include	"CAxisAngle.h"
#include	"CEulerAngles.h"
#include	"CSphericalRotation.h"
#include	"CQuaternion.h"
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
//! \brief		Assignment constructor
//! \param		[in] A reference to an axis vector
//! \param		[in] angle
//
//*****************************************************************************
CAxisAngle::CAxisAngle( const CVector3& a_vecAxis, float a_fAngle )
{
	SetAxisAngle( a_vecAxis, a_fAngle );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief		Assignment constructor
//! \param		[in] A reference to a quaternion
//
//*****************************************************************************
CAxisAngle::CAxisAngle( const CQuaternion& a_quatRotation )
{
	SetAxisAngle( a_quatRotation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief		Assignment constructor
//! \param		[in] A reference to an eulerangle
//
//*****************************************************************************
CAxisAngle::CAxisAngle( const CEulerAngles& a_Rotation )
{
	SetAxisAngle( a_Rotation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief		Copy constructor
//! \param		[in] A reference to an axisangle to be set to
//
//*****************************************************************************
CAxisAngle::CAxisAngle( const CAxisAngle& a_Source )
{
	SetAxisAngle( a_Source );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief		Default constructor
//
//*****************************************************************************
CAxisAngle::CAxisAngle()
{
	Clear();
}

//*****************************************************************************
//	Destructor
//*****************************************************************************
//
//! \brief		Destructor
//
//*****************************************************************************
CAxisAngle::~CAxisAngle()
{
}

//*****************************************************************************
//	SetAxisAngle
//*****************************************************************************
//
//! \brief	Determines a quaternion representation of the underlying rotation
//! \return	The requestet quaternion
//
//*****************************************************************************
CQuaternion CAxisAngle::GetQuaternion() const
{
	CQuaternion	quatRotation;
				quatRotation.SetQuaternion( *this );

	return quatRotation;
}

//*****************************************************************************
//	GetEulerAngles
//*****************************************************************************
//
//! \brief	Determines an euler angles representation of the underlying rotation
//! \return	The requestet eulerangles
//
//*****************************************************************************
CEulerAngles CAxisAngle::GetEulerAngles() const
{
	CEulerAngles	rotation;
					rotation.SetEulerAngles( *this );

	return rotation;
}

//*****************************************************************************
//	SetAxisAngle
//*****************************************************************************
//
//! \brief	Sets the rotation's axis and angle
//! \param	[in] The axis to set
//! \param	[in] The angle to set
//
//*****************************************************************************
void CAxisAngle::SetAxisAngle( const CVector3& a_vecAxis, float a_fAngle ) 
{
	SetAxis ( a_vecAxis );
	SetAngle( a_fAngle  );
}

//*****************************************************************************
//	SetAxisAngle
//*****************************************************************************
//
//! \brief	Sets the rotation's axis and angle
//! \param	[in] The quaternion to set
//
//*****************************************************************************
void CAxisAngle::SetAxisAngle( const CQuaternion& a_quatRotation ) 
{
    m_fAngle = 2.0f * ::g_acosf( a_quatRotation.GetW() );

	float fScale = ::g_sinf( 0.5f * GetAngle() );

	if ( fScale != 0.0f )
	{
		fScale = 1.0f / fScale;

	    m_vecAxis.AccessX() = fScale * a_quatRotation.GetX();
	    m_vecAxis.AccessY() = fScale * a_quatRotation.GetY();
	    m_vecAxis.AccessZ() = fScale * a_quatRotation.GetZ();
	}
	else
	{	
		m_vecAxis.AccessX() = 1.0f;
		m_vecAxis.AccessY() = 0.0f;
		m_vecAxis.AccessZ() = 0.0f;	
	}
}

//*****************************************************************************
//	Clear
//*****************************************************************************
//
//! \brief	Clears the rotation's axis and angle
//
//*****************************************************************************
void CAxisAngle::Clear() 
{
	m_vecAxis.SetVector( 0.0f );
	m_fAngle = 0.0f;
}

//*****************************************************************************
//	IsEqual
//*****************************************************************************
//
//! \brief	Determines whether the axis and angle equals the specified axis and angle
//! \return	Returns true, if both axis and angles are equal, either false
//
//*****************************************************************************
bool CAxisAngle::IsEqual( const CAxisAngle& a_Rotation ) const
{
	if ( m_vecAxis != a_Rotation.m_vecAxis ) return false;
	if ( m_fAngle  != a_Rotation.m_fAngle  ) return false;

	return true;
}

//*****************************************************************************
//	operator =
//*****************************************************************************
//
//! \brief	Copy operator
//! \param	[in] A reference to an axisangle to copy from
//! \return	A reference to this object
//
//*****************************************************************************
CAxisAngle& CAxisAngle::operator = ( const CAxisAngle& a_Rotation ) 
{
	SetAxisAngle( a_Rotation );

	return (*this);
}

//*****************************************************************************
//	operator =
//*****************************************************************************
//
//! \brief	Assignment operator
//! \param	[in] A reference to a quaternion to copy from
//! \return	A reference to this object
//
//*****************************************************************************
CAxisAngle& CAxisAngle::operator = ( const CQuaternion& a_quatRotation ) 
{
	SetAxisAngle( a_quatRotation );

	return (*this);
}

//*****************************************************************************
//	operator =
//*****************************************************************************
//
//! \brief	Copy operator
//! \param	[in] A reference to an eulerangle to copy from
//! \return	A reference to this object
//
//*****************************************************************************
CAxisAngle& CAxisAngle::operator = ( const CEulerAngles& a_Rotation ) 
{
	SetAxisAngle( a_Rotation );

	return (*this);
}
