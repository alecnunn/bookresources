#include	"CEulerAngles.h"
#include	"CQuaternion.h"
#include	"CAxisAngle.h"
#include	"CMatrix4.h"
#include	"CSphericalRotation.h"
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
//! \brief		Assignment constructor
//! \param		[in] yaw angle
//! \param		[in] pitch angle
//! \param		[in] roll angle
//
//*****************************************************************************
CEulerAngles::CEulerAngles( float a_fYaw, float a_fPitch, float a_fRoll )
{
	SetEulerAngles( a_fYaw, a_fPitch, a_fRoll );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief		Assignment constructor
//! \param		[in] A reference to a quaternion to be set to
//
//*****************************************************************************
CEulerAngles::CEulerAngles( const CQuaternion& a_quatRotation )
{
	SetEulerAngles( a_quatRotation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief		Assignment constructor
//! \param		[in] A reference to an axisangle to be set to
//
//*****************************************************************************
CEulerAngles::CEulerAngles( const CAxisAngle& a_Rotation )
{
	SetEulerAngles( a_Rotation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief		Copy constructor
//! \param		[in] A reference to an eulerangle to be set to
//
//*****************************************************************************
CEulerAngles::CEulerAngles( const CEulerAngles& a_Source )
{
	SetEulerAngles( a_Source );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief		Default constructor
//
//*****************************************************************************
CEulerAngles::CEulerAngles()
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
CEulerAngles::~CEulerAngles()
{
}

//*****************************************************************************
//	SetEulerAngles
//*****************************************************************************
//
//! \brief	Sets all angles
//! \param	[in] A reference to a quaternion
//
//*****************************************************************************
void CEulerAngles::SetEulerAngles( const CQuaternion& a_quatRotation )
{
	CMatrix4	matRotation;
				matRotation.SetToRotation( a_quatRotation );

	SetEulerAngles( matRotation );
}

//*****************************************************************************
//	SetEulerAngles
//*****************************************************************************
//
//! \brief	Sets all angles
//! \param	[in] A reference to an axisangle
//
//*****************************************************************************
void CEulerAngles::SetEulerAngles( const CAxisAngle& a_Rotation )
{
	CMatrix4	matRotation;
				matRotation.SetToRotation( a_Rotation );

	SetEulerAngles( matRotation );
}

//*****************************************************************************
//	SetEulerAngles
//*****************************************************************************
//
//! \brief	Sets all angles
//!
//! Variable names are equivalent to those in CMatrix4::ModifyRotation( const CEulerAngles& a_Rotation ) 
//! fSinY,... fCosZ are scaled by fCosY
//! \param	[in] A reference to a matrix
//
//*****************************************************************************
void CEulerAngles::SetEulerAngles( const CMatrix4& a_matRotation )
{
	float fSinX = - a_matRotation.GetElement(2,1);

    m_fPitch  =  ::g_asinf( fSinX );

	float	fSinY, fSinZ, fCosY, fCosZ;

    if ( 1 - fabs( fSinX ) > ::g_fEpsilon )			// Gimbal lock?
	{
		  fCosY		=	a_matRotation(2,2);
		  fSinY		=	a_matRotation(2,0);
		  m_fYaw	=  ::g_atan2f( fSinY, fCosY );
		  
		  fCosZ		=	a_matRotation(1,1);
		  fSinZ		=	a_matRotation(0,1);
		  m_fRoll	=  ::g_atan2f( fSinZ, fCosZ );
	}
    else											// Gimbal lock has occurred
    {
		  m_fYaw	= 0;							// Yaw is undefined; just fix it

		  fCosZ		=	 a_matRotation(0,0);						
		  fSinZ		=	 a_matRotation(0,2) * fSinX;
		  m_fRoll	= ::g_atan2f( fSinZ, fCosZ );
    }
}

//*****************************************************************************
//	SetEulerAngles
//*****************************************************************************
//
//! \brief	Sets all angles
//! \param	[in] yaw angle
//! \param	[in] pitch angle
//! \param	[in] roll angle
//
//*****************************************************************************
void CEulerAngles::SetEulerAngles( float a_fYaw, float a_fPitch, float a_fRoll ) 
{
	m_fYaw		= a_fYaw	;
	m_fPitch	= a_fPitch	;
	m_fRoll		= a_fRoll	;
}

//*****************************************************************************
//	SetEulerAngles
//*****************************************************************************
//
//! \brief	Copies the specified euler angles
//! \param	[in] A reference to an eulerangle to be set to
//
//*****************************************************************************
void CEulerAngles::SetEulerAngles( const CEulerAngles& a_Source )
{
	m_fYaw		= a_Source.m_fYaw   ;
	m_fPitch	= a_Source.m_fPitch ; 
	m_fRoll		= a_Source.m_fRoll  ; 
}

//*****************************************************************************
//	GetQuaternion
//*****************************************************************************
//
//! \brief	Determines the rotation's quaternion representation
//! \return	A quaternion representing the current rotation of this eulerangles
//
//*****************************************************************************
CQuaternion CEulerAngles::GetQuaternion( void ) const
{
	CQuaternion	quatRotation;
				quatRotation.SetQuaternion( *this ); 

	return quatRotation;
}

//*****************************************************************************
//	GetAxisAngle
//*****************************************************************************
//
//! \brief	Determines the rotation's axis and angle representation
//! \return	An axisangle representing the current rotation of this eulerangles
//
//*****************************************************************************
CAxisAngle CEulerAngles::GetAxisAngle( void ) const
{
	CAxisAngle	quatRotation;
				quatRotation.SetAxisAngle( *this ); 

	return quatRotation;
}

//*****************************************************************************
//	Clear
//*****************************************************************************
//
//! \brief	Clears all angles
//
//*****************************************************************************
void CEulerAngles::Clear( void )
{
	m_fYaw   = 0.0f;
	m_fPitch = 0.0f; 
	m_fRoll  = 0.0f; 
}

//*****************************************************************************
//	operator ==
//*****************************************************************************
//
//! \brief	Comparison operator
//! \param	[in] A reference to an object to compare to
//! \return	true, if the referenced object is equal to this object, either false
//
//*****************************************************************************
bool CEulerAngles::operator == ( const CEulerAngles& a_Other ) const
{
	if ( m_fYaw		!= a_Other.m_fYaw	) return false;
	if ( m_fPitch	!= a_Other.m_fPitch ) return false;
	if ( m_fRoll	!= a_Other.m_fRoll	) return false;

	return true;
}

//*****************************************************************************
//	operator !=
//*****************************************************************************
//
//! \brief	Comparison operator
//! \param	[in] A reference to an object to compare to
//! \return	true, if the referenced object is not equal to this object, either false
//
//*****************************************************************************
bool CEulerAngles::operator != ( const CEulerAngles& a_Other ) const
{
	if ( m_fYaw		== a_Other.m_fYaw	) return false;
	if ( m_fPitch	== a_Other.m_fPitch	) return false;
	if ( m_fRoll	== a_Other.m_fRoll	) return false;

	return true;
}

//*****************************************************************************
//	operator =
//*****************************************************************************
//
//! \brief	Assignment operator
//! \return	The new eulerangle
//
//*****************************************************************************
CEulerAngles& CEulerAngles::operator = ( const CEulerAngles& a_Source ) 
{
	SetEulerAngles( a_Source );

	return *this;
}

//*****************************************************************************
//	operator =
//*****************************************************************************
//
//! \brief	Assignment operator
//
//*****************************************************************************
CEulerAngles& CEulerAngles::operator = ( const CQuaternion& a_quatRotation )
{
	SetEulerAngles( a_quatRotation );

	return *this;
}

//*****************************************************************************
//	operator =
//*****************************************************************************
//
//! \brief	Assignment operator
//
//*****************************************************************************
CEulerAngles& CEulerAngles::operator = ( const CAxisAngle& a_Rotation )
{
	SetEulerAngles( a_Rotation );

	return *this;
}

//*****************************************************************************
//	operator =
//*****************************************************************************
//
//! \brief	Assignment operator
//
//*****************************************************************************
CEulerAngles& CEulerAngles::operator = ( const CMatrix4& a_matRotation )
{
	SetEulerAngles( a_matRotation );

	return *this;
}

//*****************************************************************************
//	operator +=
//*****************************************************************************
//
//! \brief	Inplace add operator
//! \return	The result of the addition
//
//*****************************************************************************
CEulerAngles& CEulerAngles::operator += ( const CEulerAngles& a_Rotation )
{
	m_fYaw   += a_Rotation.m_fYaw  ;
	m_fPitch += a_Rotation.m_fPitch;
	m_fRoll  += a_Rotation.m_fRoll ;

	return *this;
}

//*****************************************************************************
//	operator -=
//*****************************************************************************
//
//! \brief	Inplace sub operator
//! \return	The result of the subtraction
//
//*****************************************************************************
CEulerAngles& CEulerAngles::operator -= ( const CEulerAngles& a_Rotation )
{
	m_fYaw   -= a_Rotation.m_fYaw  ;
	m_fPitch -= a_Rotation.m_fPitch;
	m_fRoll  -= a_Rotation.m_fRoll ;

	return *this;
}

//*****************************************************************************
//	operator *=
//*****************************************************************************
//
//! \brief	Inplace mul operator
//! \return	The result of the multiplication
//
//*****************************************************************************
CEulerAngles& CEulerAngles::operator *= ( float a_fScale )
{
	m_fYaw   *= a_fScale;
	m_fPitch *= a_fScale;
	m_fRoll  *= a_fScale;

	return *this;
}
