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
//! \param	[in] The axis and angle to set as rotation
//! \param	[in] The vector to set as translation
//
//*****************************************************************************
CMatrix4::CMatrix4( const CAxisAngle& a_Rotation, const CVector3& a_vecTranslation ) 
{
	SetMatrix( a_Rotation, a_vecTranslation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The euler angles to set as rotation
//! \param	[in] The vector to set as translation
//
//*****************************************************************************
CMatrix4::CMatrix4( const CEulerAngles& a_Rotation, const CVector3& a_vecTranslation ) 
{
	SetMatrix( a_Rotation, a_vecTranslation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The quaternion to set as rotation
//! \param	[in] The vector to set as translation
//
//*****************************************************************************
CMatrix4::CMatrix4( const CQuaternion& a_quatRotation, const CVector3& a_vecTranslation ) 
{
	SetMatrix( a_quatRotation, a_vecTranslation );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The array to copy
//
//*****************************************************************************
CMatrix4::CMatrix4( const float a_pfElements [ EElement_Count ] ) 
{
	SetMatrix( a_pfElements );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The arrays to copy
//
//*****************************************************************************
CMatrix4::CMatrix4( const float a_pfRowColumn [ ERow_Count ][ EColumn_Count ] ) 
{
	SetMatrix( a_pfRowColumn );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The vectors to set as row vectors
//
//*****************************************************************************
CMatrix4::CMatrix4( const CVector4 a_pvecRows [ ERow_Count ] ) 
{
	SetMatrix( a_pvecRows );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The vectors to set as row vectors
//
//*****************************************************************************
CMatrix4::CMatrix4( const CVector3 a_pvecRows [ ERow_Count ] ) 
{
	SetMatrix( a_pvecRows );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The value to set at each element's index
//
//*****************************************************************************
CMatrix4::CMatrix4( float a_fElements ) 
{
	SetMatrix( a_fElements );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Copy constructor
//! \param	[in] The matrix to copy
//
//*****************************************************************************
CMatrix4::CMatrix4( const CMatrix4& a_matSource ) 
{
	SetMatrix( a_matSource );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Default constructor
//
//*****************************************************************************
CMatrix4::CMatrix4() 
{
	SetIdentity();
}

//*****************************************************************************
//	Destructor
//*****************************************************************************
//
//! \brief	Destructor
//
//*****************************************************************************
CMatrix4::~CMatrix4() 
{
}

//*****************************************************************************
//	GetColumn
//*****************************************************************************
//
//! \brief	Returns a 3D vector representation of the specified column
//! \param	[in] The desired column's index
//! \return	The vector representation of the specified column
//
//*****************************************************************************
CVector3 CMatrix4::GetColumn( int a_iColumn ) const 
{
	CVector3 vecColumn;

	GetColumn( a_iColumn, vecColumn );

	return vecColumn;
}

//*****************************************************************************
//	GetColumn
//*****************************************************************************
//
//! \brief	Returns a 4D vector representation of the specified column
//! \param	[in] The desired column's index
//! \return	The vector representation of the specified column
//
//*****************************************************************************
CVector4 CMatrix4::GetColumn4( int a_iColumn ) const 
{
	CVector4 vecColumn;

	GetColumn( a_iColumn, vecColumn );

	return vecColumn;
}

//*****************************************************************************
//	GetRotation
//*****************************************************************************
//
//! \brief	Returns matrix' rotation
//! \return	The matrix' rotation
//
//*****************************************************************************
CMatrix4 CMatrix4::GetRotation( void ) const
{
	CMatrix4 matRotation;

	GetRotation( matRotation );

	return matRotation;
}

//*****************************************************************************
//	GetRotation3
//*****************************************************************************
//
//! \brief	Returns matrix' rotation
//! \return	The matrix' rotation
//
//*****************************************************************************
CMatrix3 CMatrix4::GetRotation3( void ) const
{
	CMatrix3 matRotation;

	GetRotation( matRotation );

	return matRotation;
}

//*****************************************************************************
//	GetColumn
//*****************************************************************************
//
//! \brief	Returns the specified column's vector representation
//! \param	[in] The column's index
//! \param	[out] The specified column's vector representation
//
//*****************************************************************************
void CMatrix4::GetColumn( int a_iColumn, CVector3& a_vecColumn ) const 
{
	a_vecColumn.SetX( GetElement( ERow_RotationAxisX, a_iColumn ) );
	a_vecColumn.SetY( GetElement( ERow_RotationAxisY, a_iColumn ) );
	a_vecColumn.SetZ( GetElement( ERow_RotationAxisZ, a_iColumn ) );
}

//*****************************************************************************
//	GetColumn
//*****************************************************************************
//
//! \brief	Returns the specified column's vector representation
//! \param	[in] The column's index
//! \param	[out] The specified column's vector representation
//
//*****************************************************************************
void CMatrix4::GetColumn( int a_iColumn, CVector4& a_vecColumn ) const 
{
	a_vecColumn.SetX( GetElement( ERow_RotationAxisX, a_iColumn ) );
	a_vecColumn.SetY( GetElement( ERow_RotationAxisY, a_iColumn ) );
	a_vecColumn.SetZ( GetElement( ERow_RotationAxisZ, a_iColumn ) );
	a_vecColumn.SetW( GetElement( ERow_Translation	, a_iColumn ) );
}

//*****************************************************************************
//	GetColumn
//*****************************************************************************
//
//! \brief	Returns the specified column's values
//! \param	[in] The column's index
//! \param	[out] The specified column's values
//
//*****************************************************************************
void CMatrix4::GetColumn( int a_iColumn, float a_pfColumn [ CMatrix4::ERow_Count ] ) const 
{
	a_pfColumn[ ERow_RotationAxisX ] = m_pfRowColumn[ ERow_RotationAxisX	][ a_iColumn ];
	a_pfColumn[ ERow_RotationAxisY ] = m_pfRowColumn[ ERow_RotationAxisY	][ a_iColumn ];
	a_pfColumn[ ERow_RotationAxisZ ] = m_pfRowColumn[ ERow_RotationAxisZ	][ a_iColumn ];
	a_pfColumn[ ERow_Translation	 ] = m_pfRowColumn[ ERow_Translation	][ a_iColumn ];
}

//*****************************************************************************
//	GetRotation
//*****************************************************************************
//
//! \brief	Returns the matrix' rotation
//! \param	[out] The matrix' rotation
//
//*****************************************************************************
void CMatrix4::GetRotation( CMatrix4& a_matRotation ) const 
{
	a_matRotation.SetIdentity();
	a_matRotation.AccessRow( ERow_RotationAxisX ) = GetRow( ERow_RotationAxisX );
	a_matRotation.AccessRow( ERow_RotationAxisY ) = GetRow( ERow_RotationAxisY );
	a_matRotation.AccessRow( ERow_RotationAxisZ ) = GetRow( ERow_RotationAxisZ );
}

//*****************************************************************************
//	GetRotation
//*****************************************************************************
//
//! \brief	Returns the matrix' rotation
//! \param	[out] The matrix' rotation
//
//*****************************************************************************
void CMatrix4::GetRotation( CMatrix3& a_matRotation ) const 
{
	a_matRotation.SetIdentity();
	a_matRotation.AccessRow( ERow_RotationAxisX ) = GetRow( ERow_RotationAxisX );
	a_matRotation.AccessRow( ERow_RotationAxisY ) = GetRow( ERow_RotationAxisY );
	a_matRotation.AccessRow( ERow_RotationAxisZ ) = GetRow( ERow_RotationAxisZ );
}

//*****************************************************************************
//	SetIdentity
//*****************************************************************************
//
//! \brief	Modifies the matrix to identity
//
//*****************************************************************************
void CMatrix4::SetIdentity( void ) 
{
	Clear();

	AccessElement( ERow_RotationAxisX, EColumn_X		  ) = 1.0f;
	AccessElement( ERow_RotationAxisY, EColumn_Y		  ) = 1.0f;
	AccessElement( ERow_RotationAxisZ, EColumn_Z		  ) = 1.0f;
	AccessElement( ERow_Translation  , EColumn_Reserved ) = 1.0f;
}

//*****************************************************************************
//	SetIdentityRotation
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation to identity
//
//*****************************************************************************
void CMatrix4::SetIdentityRotation( void ) 
{
	ClearRotation();

	AccessElement( ERow_RotationAxisX, EColumn_X	) = 1.0f;
	AccessElement( ERow_RotationAxisY, EColumn_Y	) = 1.0f;
	AccessElement( ERow_RotationAxisZ, EColumn_Z	) = 1.0f;
}

//*****************************************************************************
//	ModifyScaling
//*****************************************************************************
//
//! \brief	Modifies the scaling
//! \param	[in] The scaling to set
//
//*****************************************************************************
void CMatrix4::ModifyScaling( float a_fScaling )
{
	ClearRotation();

	AccessElement( ERow_RotationAxisX, EColumn_X ) = a_fScaling;
	AccessElement( ERow_RotationAxisY, EColumn_Y ) = a_fScaling;
	AccessElement( ERow_RotationAxisZ, EColumn_Z ) = a_fScaling;
}

//*****************************************************************************
//	ModifyScaling
//*****************************************************************************
//
//! \brief	Modifies the scaling offsets 
//! \param	[in] The scaling to set
//
//*****************************************************************************
void CMatrix4::ModifyScaling( const CVector3& a_vecScaling )
{
	ClearRotation();

	AccessElement( ERow_RotationAxisX, EColumn_X		  ) = a_vecScaling.GetX();
	AccessElement( ERow_RotationAxisY, EColumn_Y		  ) = a_vecScaling.GetY();
	AccessElement( ERow_RotationAxisZ, EColumn_Z		  ) = a_vecScaling.GetZ();
}


//*****************************************************************************
//	ModifyRotation
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation to the specified rotation matrix
//! \param	[in] The rotation matrix to set
//
//*****************************************************************************
void CMatrix4::ModifyRotation( const CMatrix3& a_matRotation ) 
{
	for (int iRow = 2; iRow >= 0 ; --iRow )
		for (int iColumn = 2; iColumn >= 0 ; --iColumn )
			AccessElement(iRow, iColumn) = a_matRotation.GetElement(iRow, iColumn);
}


//*****************************************************************************
//	ModifyRotation
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation to the specified axis and angle
//! \param	[in] The axis and angle to set
//
//*****************************************************************************
void CMatrix4::ModifyRotation( const CAxisAngle& a_Rotation ) 
{
	//
	//	CHECK FOR IDENTITY
	//
	if (	( 0.0f	 == a_Rotation.GetAngle() ) 
		 ||	( true == a_Rotation.GetAxis().HasZeroMagnitude() ) )
	{
		AccessElement( EElement_11 ) = 1.0f;
		AccessElement( EElement_12 ) = 0.0f;
		AccessElement( EElement_13 ) = 0.0f;
		AccessElement( EElement_21 ) = 0.0f;
		AccessElement( EElement_22 ) = 1.0f;
		AccessElement( EElement_23 ) = 0.0f;
		AccessElement( EElement_31 ) = 0.0f;
		AccessElement( EElement_32 ) = 0.0f;
		AccessElement( EElement_33 ) = 1.0f;

		return;
	} 

	float fCos = ::g_cosf( a_Rotation.GetAngle() );
	float fSin = ::g_sinf( a_Rotation.GetAngle() );

	CVector3	vecNormal( a_Rotation.GetAxis() );
				vecNormal.Normalize();

	AccessElement( 0, 0 ) = ( vecNormal.GetX() * vecNormal.GetX() ) * ( 1.0f - fCos ) + fCos;
	AccessElement( 0, 1 ) = ( vecNormal.GetX() * vecNormal.GetY() ) * ( 1.0f - fCos ) - ( vecNormal.GetZ() * fSin );
	AccessElement( 0, 2 ) = ( vecNormal.GetX() * vecNormal.GetZ() ) * ( 1.0f - fCos ) + ( vecNormal.GetY() * fSin );

	AccessElement( 1, 0 ) = ( vecNormal.GetY() * vecNormal.GetX() ) * ( 1.0f - fCos ) + ( vecNormal.GetZ() * fSin );
	AccessElement( 1, 1 ) = ( vecNormal.GetY() * vecNormal.GetY() ) * ( 1.0f - fCos ) + fCos ;
	AccessElement( 1, 2 ) = ( vecNormal.GetY() * vecNormal.GetZ() ) * ( 1.0f - fCos ) - ( vecNormal.GetX() * fSin );

	AccessElement( 2, 0 ) = ( vecNormal.GetZ() * vecNormal.GetX() ) * ( 1.0f - fCos ) - ( vecNormal.GetY() * fSin );
	AccessElement( 2, 1 ) = ( vecNormal.GetZ() * vecNormal.GetY() ) * ( 1.0f - fCos ) + ( vecNormal.GetX() * fSin );
	AccessElement( 2, 2 ) = ( vecNormal.GetZ() * vecNormal.GetZ() ) * ( 1.0f - fCos ) + fCos;
}

//*****************************************************************************
//	ModifyRotationAxisX
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation to a rotation along the x axis rotation by the specified angle 
//! \param	[in] The rotation's angle 
//
//*****************************************************************************
void CMatrix4::ModifyRotationAxisX( float a_fAngle ) 
{
	SetIdentityRotation();

	float fSin = ::g_sinf( a_fAngle );
	float fCos = ::g_cosf( a_fAngle );

    AccessElement( 1, 1 ) =  fCos;
    AccessElement( 1, 2 ) =  fSin;

    AccessElement( 2, 1 ) = -fSin;
    AccessElement( 2, 2 ) =  fCos;
}

//*****************************************************************************
//	ModifyRotationAxisY
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation to a rotation along the y axis rotation by the specified angle 
//! \param	[in] The rotation's angle 
//
//*****************************************************************************
void CMatrix4::ModifyRotationAxisY( float a_fAngle ) 
{
	SetIdentityRotation();

	float fSin = ::g_sinf( a_fAngle );
	float fCos = ::g_cosf( a_fAngle );

    AccessElement( 0, 0 ) =  fCos;
    AccessElement( 0, 2 ) = -fSin;

    AccessElement( 2, 0 ) =  fSin;
    AccessElement( 2, 2 ) =  fCos;
}

//*****************************************************************************
//	ModifyRotationAxisZ
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation to a rotation along the z axis rotation by the specified angle 
//! \param	[in] The rotation's angle 
//
//*****************************************************************************
void CMatrix4::ModifyRotationAxisZ( float a_fAngle ) 
{
	SetIdentityRotation();

	float fSin = ::g_sinf( a_fAngle );
	float fCos = ::g_cosf( a_fAngle );

    AccessElement( 0, 0 ) =  fCos;
    AccessElement( 0, 1 ) =  fSin;

    AccessElement( 1, 0 ) = -fSin;
    AccessElement( 1, 1 ) =  fCos;
}

//*****************************************************************************
//	SetToScaling
//*****************************************************************************
//
//! \brief	Modifies the matrix to the specified scaling
//! \param	[in] The scaling factor to set for all axes
//
//*****************************************************************************
void CMatrix4::SetToScaling( float a_fScaling ) 
{
	SetIdentity();
	ModifyScaling( a_fScaling );
}

//*****************************************************************************
//	SetToScaling
//*****************************************************************************
//
//! \brief	Modifies the matrix to the specified scaling
//! \param	[in] The vector containing the scaling factors to set
//
//*****************************************************************************
void CMatrix4::SetToScaling( const CVector3& a_vecScaling ) 
{
	SetIdentity();
	ModifyScaling( a_vecScaling );
}

//*****************************************************************************
//	SetToTranslation
//*****************************************************************************
//
//! \brief	Modifies the matrix to the specified translation
//! \param	[in] The translation to set
//
//*****************************************************************************
void CMatrix4::SetToTranslation( const CVector3& a_vecTranslation ) 
{
	SetIdentity();
	ModifyTranslation( a_vecTranslation );
}

//*****************************************************************************
//	SetToRotation
//*****************************************************************************
//
//! \brief	Modifies the matrix to the specified rotation
//! \param	[in] The rotation to set
//
//*****************************************************************************
void CMatrix4::SetToRotation( const CQuaternion& a_quatRotation ) 
{
	SetIdentity();
	ModifyRotation( a_quatRotation );
}

//*****************************************************************************
//	SetToRotation
//*****************************************************************************
//
//! \brief	Modifies the matrix to the specified rotation
//! \param	[in] The rotation to set
//
//*****************************************************************************
void CMatrix4::SetToRotation( const CAxisAngle& a_Rotation ) 
{
	SetIdentity();
	ModifyRotation( a_Rotation );
}

//*****************************************************************************
//	SetToRotation
//*****************************************************************************
//
//! \brief	Modifies the matrix to the specified rotation
//! \param	[in] The rotation to set
//
//*****************************************************************************
void CMatrix4::SetToRotation( const CEulerAngles& a_Rotation ) 
{
	SetIdentity();
	ModifyRotation( a_Rotation );
}

//*****************************************************************************
//	SetToRotationAxisX
//*****************************************************************************
//
//! \brief	Modifies the matrix to a rotation along the x axis rotation by the specified angle 
//! \param	[in] The rotation's angle 
//
//*****************************************************************************
void CMatrix4::SetToRotationAxisX( float a_fAngle ) 
{
	SetIdentity();
	ModifyRotationAxisX( a_fAngle );
}

//*****************************************************************************
//	SetToRotationAxisY
//*****************************************************************************
//
//! \brief	Modifies the matrix to a rotation along the y axis rotation by the specified angle 
//! \param	[in] The rotation's angle 
//
//*****************************************************************************
void CMatrix4::SetToRotationAxisY( float a_fAngle ) 
{
	SetIdentity();
	ModifyRotationAxisY( a_fAngle );
}

//*****************************************************************************
//	SetToRotationAxisZ
//*****************************************************************************
//
//! \brief	Modifies the matrix to a rotation along the z axis rotation by the specified angle 
//! \param	[in] The rotation's angle 
//
//*****************************************************************************
void CMatrix4::SetToRotationAxisZ( float a_fAngle ) 
{
	SetIdentity();
	ModifyRotationAxisZ( a_fAngle );
}

//*****************************************************************************
//	SetToCrossProduct
//*****************************************************************************
//
//! \brief	Modifies the matrix so that multiplication by a vector will product the specified vector's cross-product (sets all other members to identity)
//! \param	[in] The vector's cross product
//
//*****************************************************************************
void CMatrix4::SetToCrossProduct( const CVector3& a_vecFactor ) 
{
	Clear();

	AccessElement( ERow_RotationAxisX, EColumn_Y ) =  a_vecFactor.GetZ();
	AccessElement( ERow_RotationAxisX, EColumn_Z ) = -a_vecFactor.GetY();
	AccessElement( ERow_RotationAxisY, EColumn_X ) = -a_vecFactor.GetZ();
	AccessElement( ERow_RotationAxisY, EColumn_Z ) =  a_vecFactor.GetX();
	AccessElement( ERow_RotationAxisZ, EColumn_X ) =  a_vecFactor.GetY();
	AccessElement( ERow_RotationAxisZ, EColumn_Y ) = -a_vecFactor.GetX();
}

//*****************************************************************************
//	SetToProduct
//*****************************************************************************
//
//! \brief	Modifies the matrix to the product of the specified matrices. 
//	Neither of the specified matrices must be supplied with the matrix itself.
//! \param	[in] The basic matrix  
//! \param	[in] The matrix to multiply by
//
//*****************************************************************************
void CMatrix4::SetToProduct( const CMatrix4& a_matLeft, const CMatrix4& a_matRight ) 
{
	if (	( this == &a_matLeft  )
		||	( this == &a_matRight ) )
	{
		static CMatrix4 matProduct;

		matProduct.SetMatrix( 0.0f );

		FOR_EACH_ROW_COLUMN_CMatrix4( iRow, iColumn )
		FOR_EACH_ROW_CMatrix4		( iRowDst		)
		{
			matProduct.AccessElement( iRow, iColumn ) += a_matLeft .GetElement( iRowDst, iColumn )  
													   * a_matRight.GetElement( iRow, iRowDst	 );
		}

		SetMatrix( matProduct );
	}
	else
	{
		Clear();

		FOR_EACH_ROW_COLUMN_CMatrix4( iRow, iColumn )
		FOR_EACH_ROW_CMatrix4		( iRowDst		)
		{
			AccessElement( iRow, iColumn ) += a_matLeft .GetElement( iRowDst, iColumn )  
											* a_matRight.GetElement( iRow, iRowDst	);
		}
	}
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Modifys all rows to the specified vectors
//! \param	[in] The vector representations of the rows to set
//
//*****************************************************************************
void CMatrix4::SetMatrix ( const CVector3 a_pvecRows [ CMatrix4::ERow_Count ] ) 
{
	AccessRow( ERow_RotationAxisX ) = a_pvecRows[ ERow_RotationAxisX	];	
	AccessRow( ERow_RotationAxisY ) = a_pvecRows[ ERow_RotationAxisY	];	
	AccessRow( ERow_RotationAxisZ ) = a_pvecRows[ ERow_RotationAxisZ	];	
	AccessRow( ERow_Translation	  ) = a_pvecRows[ ERow_Translation	];	

	ValidateReserved();
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Modifys all rows to the specified vectors
//! \param	[in] The vector representations of the rows to set
//
//*****************************************************************************
void CMatrix4::SetMatrix( const CVector4 a_pvecRows [ CMatrix4::ERow_Count ] ) 
{
	AccessRow4( ERow_RotationAxisX	) = a_pvecRows[ ERow_RotationAxisX	];	
	AccessRow4( ERow_RotationAxisY	) = a_pvecRows[ ERow_RotationAxisY	];	
	AccessRow4( ERow_RotationAxisZ	) = a_pvecRows[ ERow_RotationAxisZ	];	
	AccessRow4( ERow_Translation	) = a_pvecRows[ ERow_Translation		];	
}


//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Modifys the matrix' rotation and translation
//! \param	[in] The rotation to set
//! \param	[in] The translation to set
//
//*****************************************************************************
void CMatrix4::SetMatrix( const CQuaternion& a_quatRotation, const CVector3& a_vecTranslation ) 
{
	ModifyRotation	( a_quatRotation	);
	ModifyTranslation( a_vecTranslation	);

	ValidateReserved();
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Modifys the matrix' rotation and translation
//! \param	[in] The rotation to set
//! \param	[in] The translation to set
//
//*****************************************************************************
void CMatrix4::SetMatrix( const CEulerAngles& a_Rotation, const CVector3& a_vecTranslation ) 
{
	ModifyRotation	( a_Rotation		);
	ModifyTranslation( a_vecTranslation	);

	ValidateReserved();
} 

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Modifys the matrix' rotation and translation
//! \param	[in] The rotation to set
//! \param	[in] The translation to set
//
//*****************************************************************************
void CMatrix4::SetMatrix( const CAxisAngle& a_Rotation, const CVector3& a_vecTranslation ) 
{
	ModifyRotation	( a_Rotation		);
	ModifyTranslation( a_vecTranslation	);

	ValidateReserved();
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Modifys all elements to the specified value
//! \param	[in] The element to fill the matrix with
//
//*****************************************************************************
void CMatrix4::SetMatrix ( float a_fElements ) 
{
	U32 u32Temp;

	memcpy( &u32Temp	,
			&a_fElements,
			sizeof(U32)	);

	memset(	m_pfElements					, 
			u32Temp							, 
			EElement_Count * sizeof(float)	);
}

//*****************************************************************************
//	GetInvertedExcentric
//*****************************************************************************
//
//! \brief	Returns a completely inverted copy of the matrix
//! \return	The completely inverted copy of the matrix
//
//*****************************************************************************
CMatrix4 CMatrix4::GetInvertedExcentric() const
{

#define m11 m_pfElements[ EElement_11 ]
#define m12 m_pfElements[ EElement_21 ]
#define m13 m_pfElements[ EElement_31 ]
#define m14 m_pfElements[ EElement_41 ]
#define m21 m_pfElements[ EElement_12 ]
#define m22 m_pfElements[ EElement_22 ]
#define m23 m_pfElements[ EElement_32 ]
#define m24 m_pfElements[ EElement_42 ]
#define m31 m_pfElements[ EElement_13 ]
#define m32 m_pfElements[ EElement_23 ]
#define m33 m_pfElements[ EElement_33 ]
#define m34 m_pfElements[ EElement_43 ]
#define m41 m_pfElements[ EElement_14 ]
#define m42 m_pfElements[ EElement_24 ]
#define m43 m_pfElements[ EElement_34 ]
#define m44 m_pfElements[ EElement_44 ]

	CMatrix4	matInverted;
	float		fDet;
	float		d12, d13, d23, d24, d34, d41;
	
	d12 = (m31*m42-m41*m32);
	d13 = (m31*m43-m41*m33);
	d23 = (m32*m43-m42*m33);
	d24 = (m32*m44-m42*m34);
	d34 = (m33*m44-m43*m34);
	d41 = (m34*m41-m44*m31);
	
	matInverted[0] =  (m22 * d34 - m23 * d24 + m24 * d23);
	matInverted[1] = -(m21 * d34 + m23 * d41 + m24 * d13);
	matInverted[2] =  (m21 * d24 + m22 * d41 + m24 * d12);
	matInverted[3] = -(m21 * d23 - m22 * d13 + m23 * d12);
	
	fDet = m11 * matInverted[0] + m12 * matInverted[1] + m13 * matInverted[2] + m14 * matInverted[3];
	
	if ( fDet == 0.0f ) 
	{
		matInverted.SetIdentity();
	
		return matInverted;
	}

	fDet = 1.0f / fDet;

	matInverted[0] *= fDet;
	matInverted[1] *= fDet;
	matInverted[2] *= fDet;
	matInverted[3] *= fDet;

	matInverted[4] = -(m12 * d34 - m13 * d24 + m14 * d23) * fDet;
	matInverted[5] =  (m11 * d34 + m13 * d41 + m14 * d13) * fDet;
	matInverted[6] = -(m11 * d24 + m12 * d41 + m14 * d12) * fDet;
	matInverted[7] =  (m11 * d23 - m12 * d13 + m13 * d12) * fDet;

	d12 = m11*m22-m21*m12;
	d13 = m11*m23-m21*m13;
	d23 = m12*m23-m22*m13;
	d24 = m12*m24-m22*m14;
	d34 = m13*m24-m23*m14;
	d41 = m14*m21-m24*m11;

	matInverted[8]  =  (m42 * d34 - m43 * d24 + m44 * d23) * fDet;
	matInverted[9]  = -(m41 * d34 + m43 * d41 + m44 * d13) * fDet;
	matInverted[10] =  (m41 * d24 + m42 * d41 + m44 * d12) * fDet;
	matInverted[11] = -(m41 * d23 - m42 * d13 + m43 * d12) * fDet;
	matInverted[12] = -(m32 * d34 - m33 * d24 + m34 * d23) * fDet;
	matInverted[13] =  (m31 * d34 + m33 * d41 + m34 * d13) * fDet;
	matInverted[14] = -(m31 * d24 + m32 * d41 + m34 * d12) * fDet;
	matInverted[15] =  (m31 * d23 - m32 * d13 + m33 * d12) * fDet;

#undef m11 
#undef m12 
#undef m13 
#undef m14 
#undef m21 
#undef m22 
#undef m23 
#undef m24 
#undef m31 
#undef m32 
#undef m33 
#undef m34 
#undef m41 
#undef m42 
#undef m43 
#undef m44 

	return matInverted;
}

//*****************************************************************************
//	ValidateReserved
//*****************************************************************************
//
//! \brief	Modifys the reserved elements to their respective default values
//
//*****************************************************************************
void CMatrix4::ValidateReserved( void )
{
	AccessElement( ERow_RotationAxisX, EColumn_Reserved ) = 0.0f;
	AccessElement( ERow_RotationAxisY, EColumn_Reserved ) = 0.0f;
	AccessElement( ERow_RotationAxisZ, EColumn_Reserved ) = 0.0f;
	AccessElement( ERow_Translation	 , EColumn_Reserved ) = 1.0f;
}

//*****************************************************************************
//	ClearRotation
//*****************************************************************************
//
//! \brief	Clears the matrix' rotation (sets all elements to 0.0f)
//
//*****************************************************************************
void CMatrix4::ClearRotation( void ) 
{
	AccessXAxis().SetVector( 0.0f );
	AccessYAxis().SetVector( 0.0f );
	AccessZAxis().SetVector( 0.0f );
}

//*****************************************************************************
//	TransposeRotation
//*****************************************************************************
//
//! \brief	Transposes the matrix' rotation
//
//*****************************************************************************
void CMatrix4::TransposeRotation( void ) 
{
	float fSwap;

	SWAP_CMatrix4( fSwap, ERow_RotationAxisX, EColumn_Y );
	SWAP_CMatrix4( fSwap, ERow_RotationAxisX, EColumn_Z );
	SWAP_CMatrix4( fSwap, ERow_RotationAxisY, EColumn_Z );
}

//*****************************************************************************
//	Transpose
//*****************************************************************************
//
//! \brief	Transposes the matrix completely. Does not make sense mathematically, but is needed to pass the matrix to some APIs (vertex shaders...)
//
//*****************************************************************************
void CMatrix4::Transpose( void ) 
{
	float fSwap;

	SWAP_CMatrix4( fSwap, ERow_RotationAxisX, EColumn_Y );

	SWAP_CMatrix4( fSwap, ERow_RotationAxisX, EColumn_Z );
	SWAP_CMatrix4( fSwap, ERow_RotationAxisY, EColumn_Z );

	SWAP_CMatrix4( fSwap, ERow_RotationAxisX, EColumn_Reserved );
	SWAP_CMatrix4( fSwap, ERow_RotationAxisY, EColumn_Reserved );
	SWAP_CMatrix4( fSwap, ERow_RotationAxisZ, EColumn_Reserved );
}

//*****************************************************************************
//	Negate
//*****************************************************************************
//
//! \brief	Negates the matrix
//
//*****************************************************************************
void CMatrix4::Negate( void ) 
{
	FOR_EACH_ELEMENT_CMatrix4( iElement )
	{
		m_pfElements[ iElement ] = -m_pfElements[ iElement ];
	}
}

//*****************************************************************************
//	Add
//*****************************************************************************
//
//! \brief	Adds the specified matrix to this one
//! \param	[in] The matrix to add
//
//*****************************************************************************
void CMatrix4::Add( const CMatrix4& a_mat ) 
{
	FOR_EACH_ELEMENT_CMatrix4( iElement )
	{
		m_pfElements[ iElement ] += a_mat.m_pfElements[ iElement ];
	}
}

//*****************************************************************************
//	Sub
//*****************************************************************************
//
//! \brief	Subtracts the specified matrix from this one
//! \param	[in] The matrix to subtract
//
//*****************************************************************************
void CMatrix4::Sub( const CMatrix4& a_mat ) 
{
	FOR_EACH_ELEMENT_CMatrix4( iElement )
	{
		m_pfElements[ iElement ] -= a_mat.m_pfElements[ iElement ];
	}
}

//*****************************************************************************
//	Multiply
//*****************************************************************************
//
//! \brief	Multiplies each element by the specified factor
//! \param	[in] The factor to multiply by
//
//*****************************************************************************
void CMatrix4::Multiply( float a_fFactor ) 
{
	FOR_EACH_ELEMENT_CMatrix4( iElement )
	{
		m_pfElements[ iElement ] *= a_fFactor;
	}
}

//*****************************************************************************
//	LookAt
//*****************************************************************************
//
//! \brief	Changes the rotation part of the matrix to a look-at-matrix along a vector, changing the up vector as little as possible
//! \param	[in] The new normalized LookAt-vector (will be found in the z-Axis of the matrix after the call)
//
//*****************************************************************************
void CMatrix4::LookAt( const CVector3& a_vecLookAt )
{
	//
	// set the z-Axis
	//
	ModifyZAxis( a_vecLookAt );

	//
	// make the up vector perpendicular to the at vector:
	//
	float fUpAtProduct = CVector3::DotProduct( a_vecLookAt, GetYAxis() );
	AccessYAxis() -= (fUpAtProduct/a_vecLookAt.GetSquareMagnitude()) * a_vecLookAt;

	//
	// get the up vector's length
	//
	float fUpSquareMagnitude = GetYAxis().GetSquareMagnitude();

	//
	// find a better up vector if the length is too small
	//
	if ( fUpSquareMagnitude < ::g_fEpsilon)
	{
		//
		// try the global z-axis
		//
		ModifyYAxis( CVector3( 0.0f, 0.0f, 1.0f ) );

		//
		// make the up vector perpendicular to the at vector:
		//
		float fUpAtProduct = CVector3::DotProduct( a_vecLookAt, GetYAxis() );
		AccessYAxis() -= fUpAtProduct * a_vecLookAt;

		//
		// get the up vector's length
		//
		fUpSquareMagnitude = GetYAxis().GetSquareMagnitude();

		//
		// if the length is still not better, try another one. It has to work (it can't be)
		//
		if (fUpSquareMagnitude < ::g_fEpsilon)
		{
			//
			// try the global y-axis
			//
			ModifyYAxis( CVector3( 0.0f, 1.0f, 0.0f ) );

			//
			// make the up vector perpendicular to the at vector:
			//
			float fUpAtProduct = CVector3::DotProduct( a_vecLookAt, GetYAxis() );
			AccessYAxis() -= fUpAtProduct * a_vecLookAt;

			//
			// get the up vector's length
			//
			fUpSquareMagnitude = GetYAxis().GetSquareMagnitude();
		}
	}

	//
	// normalize the up vector
	//
	AccessYAxis() *= 1.0f / ::g_sqrtf( fUpSquareMagnitude );

	//
	// the X-Axis simply is the cross product of Y and Z-Axis
	//
	AccessXAxis().SetCrossProduct( GetYAxis(), GetZAxis() );
}

//*****************************************************************************
//	LookAt
//*****************************************************************************
//
//! \brief	Changes the rotation part of the matrix to a lookat-matrix along a vector, taking the up vector as close to the passed up vector as possible
//! \param	[in] The new normalized LookAt-vector (will be found in the z-Axis of the matrix after the call)
//! \param	[in] The desired Up-Vector (we'll try to make the y-Axis as close to this vector as possible)
//
//*****************************************************************************
void CMatrix4::LookAt( const CVector3& a_vecLookAt, const CVector3& a_vecUp )
{
	if ( a_vecUp.GetSquareMagnitude() > ::g_fEpsilon )
	{
		ModifyYAxis( a_vecUp );
	}

	LookAt( a_vecLookAt	);
}

//*****************************************************************************
//	Orthonormalize
//*****************************************************************************
//
//! \brief	Orthonormalizes the matrix
//
//*****************************************************************************
void CMatrix4::Orthonormalize( void ) 
{
	//
	// make sure the Z-Axis is normalized
	//
	float fZAxisSquareMagnitude = GetZAxis().GetSquareMagnitude();

	if ( fZAxisSquareMagnitude < ::g_fEpsilon )
	{
		AccessZAxis()[ CVector3::ECoordinate_Z ] = 1.0f;
	}
	else
	{
		AccessZAxis() /= ::g_sqrtf( fZAxisSquareMagnitude );
	}

	LookAt( GetZAxis() );
}

//*****************************************************************************
//	Orthogonalize								 
//*****************************************************************************
//
//! \brief	Orthogonalizes the matrix
//
//*****************************************************************************
void CMatrix4::Orthogonalize( void ) 
{
	int intI, intJ;

	for ( intJ = 2; intJ > 0; --intJ )
	{
		const CVector3& vecAxisJ	= GetRow(intJ);
		float fAxisJMagSquared	= vecAxisJ.GetSquareMagnitude();
		
		if ( fAxisJMagSquared > 0 )
		{
			for ( intI = intJ-1; intI>=0; --intI )
			{
				// make the i'th axis vector perpendicular to the j'th
				CVector3& vecAxisI = AccessRow(intI);

				vecAxisI -= vecAxisJ * ( CVector3::DotProduct(vecAxisJ, vecAxisI) / fAxisJMagSquared );
			}
		}
	}
}

//*****************************************************************************
//	ModifyRotation
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation to the specified euler angles
//! \param	[in] The euler angles to set
//
//*****************************************************************************
void CMatrix4::ModifyRotation( const CEulerAngles& a_Rotation ) 
{
	float fSinX = ::g_sinf( a_Rotation.GetPitch	() );
	float fCosX = ::g_cosf( a_Rotation.GetPitch	() );
	float fSinY = ::g_sinf( a_Rotation.GetYaw		() );
	float fCosY = ::g_cosf( a_Rotation.GetYaw		() );	
	float fSinZ = ::g_sinf( a_Rotation.GetRoll	() );
	float fCosZ = ::g_cosf( a_Rotation.GetRoll	() );

	AccessElement( 0, 0 ) =   fCosY * fCosZ + fSinY * fSinX * fSinZ	;
	AccessElement( 0, 1 ) =							  fCosX * fSinZ	;
	AccessElement( 0, 2 ) = - fSinY * fCosZ + fCosY * fSinX * fSinZ	;

	AccessElement( 1, 0 ) = - fCosY * fSinZ + fSinY * fSinX * fCosZ	;
	AccessElement( 1, 1 ) =							  fCosX * fCosZ	;
	AccessElement( 1, 2 ) =   fSinY * fSinZ + fCosY * fSinX * fCosZ	;

	AccessElement( 2, 0 ) =	fSinY * fCosX;
	AccessElement( 2, 1 ) = -		fSinX;
	AccessElement( 2, 2 ) =	fCosY * fCosX;
}

//*****************************************************************************
//	ModifyRotation
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation to the specified quaternion
//! \param	[in] The quaternion to set
//
//*****************************************************************************
void CMatrix4::ModifyRotation( const CQuaternion& a_quatRotation ) 
{
	float wx,wy,wz,xx,yy,yz,xy,xz,zz,x2,y2,z2;

	x2 = a_quatRotation.GetX() + a_quatRotation.GetX();
	y2 = a_quatRotation.GetY() + a_quatRotation.GetY();
	z2 = a_quatRotation.GetZ() + a_quatRotation.GetZ();
	xx = a_quatRotation.GetX() * x2;
	xy = a_quatRotation.GetX() * y2;
	xz = a_quatRotation.GetX() * z2;
	yy = a_quatRotation.GetY() * y2;
	yz = a_quatRotation.GetY() * z2;
	zz = a_quatRotation.GetZ() * z2;
	wx = a_quatRotation.GetW() * x2;
	wy = a_quatRotation.GetW() * y2;
	wz = a_quatRotation.GetW() * z2;

	AccessElement( 0, 0 ) = 1.0f - ( yy + zz );
	AccessElement( 0, 1 ) = xy + wz;
	AccessElement( 0, 2 ) = xz - wy;

	AccessElement( 1, 0 ) = xy - wz;
	AccessElement( 1, 1 ) = 1.0f - ( xx + zz );
	AccessElement( 1, 2 ) = yz + wx;

	AccessElement( 2, 0 ) = xz + wy;
	AccessElement( 2, 1 ) = yz - wx;
	AccessElement( 2, 2 ) = 1.0f - ( xx + yy );
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Builds a complete PRS transformation matrix
//! \param	[in] The translation to set
//! \param	[in] The rotation to set
//! \param	[in] The scale to set
//
//*****************************************************************************
void CMatrix4::SetMatrix( const CVector3& a_vecTranslation, const CQuaternion& a_quatRotation, const CVector3& a_vecScale )
{
	SetToTranslation(a_vecTranslation);
	ModifyRotation(a_quatRotation);

	AccessElement(EElement_11) *= a_vecScale.GetX();
	AccessElement(EElement_12) *= a_vecScale.GetX();
	AccessElement(EElement_13) *= a_vecScale.GetX();

	AccessElement(EElement_21) *= a_vecScale.GetY();
	AccessElement(EElement_22) *= a_vecScale.GetY();
	AccessElement(EElement_23) *= a_vecScale.GetY();

	AccessElement(EElement_31) *= a_vecScale.GetZ();
	AccessElement(EElement_32) *= a_vecScale.GetZ();
	AccessElement(EElement_33) *= a_vecScale.GetZ();

	ValidateReserved();
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Builds a complete PRS transformation matrix
//! \param	[in] The translation to set
//! \param	[in] The rotation to set
//! \param	[in] The scale to set
//
//*****************************************************************************
void CMatrix4::SetMatrix( const CVector3& a_vecTranslation, const CEulerAngles& a_eulerRotation, const CVector3& a_vecScale )
{
	SetToTranslation(a_vecTranslation);
	ModifyRotation(a_eulerRotation);

	AccessElement(EElement_11) *= a_vecScale.GetX();
	AccessElement(EElement_12) *= a_vecScale.GetX();
	AccessElement(EElement_13) *= a_vecScale.GetX();

	AccessElement(EElement_21) *= a_vecScale.GetY();
	AccessElement(EElement_22) *= a_vecScale.GetY();
	AccessElement(EElement_23) *= a_vecScale.GetY();

	AccessElement(EElement_31) *= a_vecScale.GetZ();
	AccessElement(EElement_32) *= a_vecScale.GetZ();
	AccessElement(EElement_33) *= a_vecScale.GetZ();

	ValidateReserved();
}

//*****************************************************************************
//	Scale
//*****************************************************************************
//
//! \brief	Scales the matrix by the specified factor
//! \param	[in] The factor to scale by
//
//*****************************************************************************
void CMatrix4::Scale( float a_fScaling ) 
{
	CMatrix4	matScale;
				matScale.SetToScaling( a_fScaling );

	Multiply( matScale );
}

//*****************************************************************************
//	Scale
//*****************************************************************************
//
//! \brief	Scales the matrix by the specified factor
//! \param	[in] The vector to scale by
//
//*****************************************************************************
void CMatrix4::Scale( const CVector3& a_vecScaling ) 
{
	CMatrix4	matScale;
				matScale.SetToScaling( a_vecScaling );

	Multiply( matScale );
}

//*****************************************************************************
//	Rotate
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified quaternion
//! \param	[in] The quaternion to rotate by
//
//*****************************************************************************
void CMatrix4::Rotate( const CQuaternion& a_quatRotation ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotation( a_quatRotation );

	ReverseMultiply( matRotation );
}

//*****************************************************************************
//	Rotate
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified axis and angle
//! \param	[in] The axis and angle to rotate by
//
//*****************************************************************************
void CMatrix4::Rotate( const CAxisAngle& a_Rotation ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotation( a_Rotation );

	ReverseMultiply( matRotation );
}

//*****************************************************************************
//	Rotate
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified euler angles
//! \param	[in] The euler angles to rotate by
//
//*****************************************************************************
void CMatrix4::Rotate( const CEulerAngles& a_Rotation ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotation( a_Rotation );

	ReverseMultiply( matRotation );
}

//*****************************************************************************
//	RotateAxisX
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified angle along the x axis 
//! \param	[in] The angle to rotate along the x axis
//
//*****************************************************************************
void CMatrix4::RotateAxisX( float a_fAngle ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotationAxisX( a_fAngle );

	ReverseMultiply( matRotation );
}

//*****************************************************************************
//	RotateAxisY
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified angle along the y axis 
//! \param	[in] The angle to rotate along the y axis
//
//*****************************************************************************
void CMatrix4::RotateAxisY( float a_fAngle ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotationAxisY( a_fAngle );

	ReverseMultiply( matRotation );
}

//*****************************************************************************
//	RotateAxisZ
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified angle along the z axis 
//! \param	[in] The angle to rotate along the x axis
//
//*****************************************************************************
void CMatrix4::RotateAxisZ( float a_fAngle ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotationAxisZ( a_fAngle );

	ReverseMultiply( matRotation );
}

//*****************************************************************************
//	RotateLocal
//*****************************************************************************
//
//! \brief	Locally rotates the matrix by the specified quaternion
//! \param	[in] The quaternion to rotate by
//
//*****************************************************************************
void CMatrix4::RotateLocal( const CQuaternion& a_quatRotation ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotation( a_quatRotation );

	Multiply( matRotation );
}

//*****************************************************************************
//	RotateLocal
//*****************************************************************************
//
//! \brief	Locally rotates the matrix by the specified axis and angle
//! \param	[in] The axis and angle to rotate by
//
//*****************************************************************************
void CMatrix4::RotateLocal( const CAxisAngle& a_Rotation ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotation( a_Rotation );

	Multiply( matRotation );
}

//*****************************************************************************
//	RotateLocal
//*****************************************************************************
//
//! \brief	Locally rotates the matrix by the specified euler angles
//! \param	[in] The euler angles to rotate by
//
//*****************************************************************************
void CMatrix4::RotateLocal( const CEulerAngles& a_Rotation ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotation( a_Rotation );

	Multiply( matRotation );
}

//*****************************************************************************
//	RotateLocalAxisX
//*****************************************************************************
//
//! \brief	Locally rotates the matrix by the specified angle along the x axis 
//! \param	[in] The angle to rotate by along the x axis
//
//*****************************************************************************
void CMatrix4::RotateLocalAxisX( float a_fAngle ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotationAxisX( a_fAngle );

	Multiply( matRotation );
}

//*****************************************************************************
//	RotateLocalAxisY
//*****************************************************************************
//
//! \brief	Locally rotates the matrix by the specified angle along the y axis 
//! \param	[in] The angle to rotate by along the y axis
//
//*****************************************************************************
void CMatrix4::RotateLocalAxisY( float a_fAngle ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotationAxisY( a_fAngle );

	Multiply( matRotation );
}

//*****************************************************************************
//	RotateLocalAxisZ
//*****************************************************************************
//
//! \brief	Locally rotates the matrix by the specified angle along the z axis 
//! \param	[in] The angle to rotate by along the z axis
//
//*****************************************************************************
void CMatrix4::RotateLocalAxisZ( float a_fAngle ) 
{
	CMatrix4	matRotation;
				matRotation.SetToRotationAxisZ( a_fAngle );

	Multiply( matRotation );
}

//*****************************************************************************
//	IsExcentric
//*****************************************************************************
//
//! \brief	Determines whether or not the matrix is excentric
//! \return	The method returns either of the following values
//! \retval	true	| The matrix is excentric
//! \retval	false	| The matrix is not excentric
//
//*****************************************************************************
bool CMatrix4::IsExcentric() const
{
	if ( GetElement( ERow_RotationAxisX, EColumn_Reserved ) != 0.0f )	return true;
	if ( GetElement( ERow_RotationAxisY, EColumn_Reserved ) != 0.0f )	return true;
	if ( GetElement( ERow_RotationAxisZ, EColumn_Reserved ) != 0.0f )	return true;
	if ( GetElement( ERow_Translation  , EColumn_Reserved ) != 1.0f )	return true;

	return false;
}

//*****************************************************************************
//	IsIdentity
//*****************************************************************************
//
//! \brief	Determines whether or not the matrix represents an identity matrix
//! \return	The method returns either of the following values
//! \retval	true	| The matrix represents an identity matrix
//! \retval	false	| The matrix does not represent an identity matrix
//
//*****************************************************************************
bool CMatrix4::IsIdentity() const
{
	float fIdentity;

	FOR_EACH_ROW_COLUMN_CMatrix4( iRow, iColumn )
	{
		fIdentity = ( ( iRow == iColumn ) ? 1.0f : 0.0f );			// diagonal elements must be 1.0f, all other elements must be zero

		if ( m_pfRowColumn[ iRow ][ iColumn ] != fIdentity ) 
		{
			return false;
		}
	}

	return true;
}

//*****************************************************************************
//	IsEqual 
//*****************************************************************************
//
//! \brief	Determines whether or not the matrix equals the specified matrix
//! \param	[in] The matrix to compare to
//! \return	The method returns either of the following values
//! \retval	true	| The matrices are equal
//! \retval	false	| The matrices differ
//
//*****************************************************************************
bool CMatrix4::IsEqual( const CMatrix4& a_matOther ) const
{
	FOR_EACH_ELEMENT_CMatrix4( iElement )
	{
		if ( GetElement( iElement ) != a_matOther.GetElement( iElement ) )
		{
			return false;
		}
	}

	return true;
}

#pragma warning( push )
#pragma warning( disable : 4701 )	// local variable may be used without having been initialized

//*****************************************************************************
//	GetDeterminantAdjoint
//*****************************************************************************
//
//! \brief	Determines the determinant for the matrix adjoint to the specified element
//! \param	[in] The index of the row to omit
//! \param	[in] The index of the column to omit
//! \return	The matrix' determinant
//
//*****************************************************************************
 
float CMatrix4::GetDeterminantAdjoint( int a_iRow, int a_iColumn ) const 
{
	int iCol0, iCol1, iCol2;
	int iRow0, iRow1, iRow2;

	switch ( a_iColumn ) 
	{
		case 0:	iCol0 = 1;	iCol1 = 2;	iCol2 = 3;	break;
		case 1:	iCol0 = 0;	iCol1 = 2;	iCol2 = 3;	break;
		case 2:	iCol0 = 0;	iCol1 = 1;	iCol2 = 3;	break;
		case 3:	iCol0 = 0;	iCol1 = 1;	iCol2 = 2;	break;
	}

	switch ( a_iRow ) 
	{
		case 0:	iRow0 = 1;	iRow1 = 2;	iRow2 = 3;	break;
		case 1:	iRow0 = 0;	iRow1 = 2;	iRow2 = 3;	break;
		case 2:	iRow0 = 0;	iRow1 = 1;	iRow2 = 3;	break;
		case 3:	iRow0 = 0;	iRow1 = 1;	iRow2 = 2;	break;
	}

	return (	+ GetElement( iRow0, iCol0 ) * (  GetElement( iRow1, iCol1 ) * GetElement( iRow2, iCol2 ) 
												- GetElement( iRow1, iCol2 ) * GetElement( iRow2, iCol1 ) ) 
				- GetElement( iRow0, iCol1 ) * (  GetElement( iRow1, iCol0 ) * GetElement( iRow2, iCol2 ) 
												- GetElement( iRow1, iCol2 ) * GetElement( iRow2, iCol0 ) ) 
				+ GetElement( iRow0, iCol2 ) * (  GetElement( iRow1, iCol0 ) * GetElement( iRow2, iCol1 ) 
												- GetElement( iRow1, iCol1 ) * GetElement( iRow2, iCol0 ) ) );
}

#pragma warning( pop )

//*****************************************************************************
//	GetIdentity
//*****************************************************************************
//
//! \brief	Returns an identity matrix
//! \return	An identity matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetIdentity()
{
	CMatrix4	matIdentity;
				matIdentity.SetIdentity();

	return matIdentity;
}

//*****************************************************************************
//	GetNegated
//*****************************************************************************
//
//! \brief	Returns a negated copy of the matrix
//! \return	The negated copy of the matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetNegated() const
{
	CMatrix4	matNegated( *this );
				matNegated.Negate();

	return matNegated;
}

//*****************************************************************************
//	GetInverted
//*****************************************************************************
//
//! \brief	Returns an inverted copy of the matrix
//! \return	The inverted copy of the matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetInverted() const
{
	CMatrix4 matInverted;

	GetInverted( matInverted );

	return matInverted;
}

//*****************************************************************************
//	GetOrthogonalized
//*****************************************************************************
//
//! \brief	Returns an orthogonalized copy of the matrix
//! \return	The orthogonalized copy of the matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetOrthogonalized( void ) const 
{
	CMatrix4	matOrthogonalized( *this );
				matOrthogonalized.Orthogonalize();

	return matOrthogonalized;
}

//*****************************************************************************
//	GetOrthonormalized
//*****************************************************************************
//
//! \brief	Returns an orthonormalized copy of the matrix
//! \return	The orthonormalized copy of the matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetOrthonormalized( void ) const 
{
	CMatrix4	matOrthonormalized( *this );
				matOrthonormalized.Orthonormalize();

	return matOrthonormalized;
}

//*****************************************************************************
//	GetSum
//*****************************************************************************
//
//! \brief	Returns the sum of this matrix and the specified one
//! \param	[in] The matrix to add
//! \return	The sum
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetSum( const CMatrix4& a_matOffset ) const 
{
	CMatrix4	matSum( *this );
				matSum.Add( a_matOffset );

	return matSum;
}

//*****************************************************************************
//	GetDifference
//*****************************************************************************
//
//! \brief	Returns the difference of this matrix and the specified one
//! \param	[in] The matrix to subtract
//! \return	The difference
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetDifference( const CMatrix4& a_matOffset ) const 
{
	CMatrix4	matDifference( *this );
				matDifference.Sub( a_matOffset );

	return matDifference;
}

//*****************************************************************************
//	GetProductPrefix
//*****************************************************************************
//
//! \brief	Returns the product of the specified matrix and this one
//! \param	[in] The matrix to multiply
//! \return	The product
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetProductPrefix( const CMatrix4& a_matPrefix ) const 
{
	CMatrix4 matProduct;

	GetProductPrefix( matProduct, a_matPrefix );

	return matProduct;
}

//*****************************************************************************
//	GetProduct
//*****************************************************************************
//
//! \brief	Returns the product of this matrix and the specified one
//! \param	[in] The matrix to multiply
//! \return	The product
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetProduct( const CMatrix4& a_matFactor ) const 
{
	CMatrix4 matProduct;

	GetProduct( matProduct, a_matFactor );

	return matProduct;
}

//*****************************************************************************
//	GetProduct
//*****************************************************************************
//
//! \brief	Returns the scalar product of the matrix and the specified scalar factor
//! \param	[in] The factor to multiply by
//! \return	The product
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetProduct( float a_fFactor ) const 
{
	CMatrix4	matProduct( *this );
				matProduct.Multiply( a_fFactor );

	return matProduct;
}

//*****************************************************************************
//	GetQuotient
//*****************************************************************************
//
//! \brief	Determines the quotient of the matrix devided by the given factor
//! \param	[in] The factor to divide by
//! \return	The quotient
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetQuotient( float a_fDivisor ) const 
{
	CMatrix4	matQuotient( *this );
				matQuotient.Divide( a_fDivisor );

	return matQuotient;
}

//*****************************************************************************
//	GetRotated
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The quaternion to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetRotated( const CQuaternion& a_quatRotation ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.Rotate( a_quatRotation );

	return matRotated;
}

//*****************************************************************************
//	GetRotated
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The axis and angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetRotated( const CAxisAngle& a_Rotation ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.Rotate( a_Rotation );

	return matRotated;
}

//*****************************************************************************
//	GetRotated
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The euler angles to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetRotated( const CEulerAngles& a_Rotation ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.Rotate( a_Rotation );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedByAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetRotatedByAxisX( float a_fAngle ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.RotateAxisX( a_fAngle );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedByAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetRotatedByAxisY( float a_fAngle ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.RotateAxisY( a_fAngle );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedByAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetRotatedByAxisZ( float a_fAngle ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.RotateAxisZ( a_fAngle );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedLocal
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The quaternion to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetRotatedLocal( const CQuaternion& a_quatRotation ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.RotateLocal( a_quatRotation );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedLocal
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The axis and angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetRotatedLocal( const CAxisAngle& a_Rotation ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.RotateLocal( a_Rotation );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedLocal
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The euler angles to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetRotatedLocal( const CEulerAngles& a_Rotation ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.RotateLocal( a_Rotation );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedByLocalAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetRotatedByLocalAxisX( float a_fAngle ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.RotateLocalAxisX( a_fAngle );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedByLocalAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
 
CMatrix4 CMatrix4::GetRotatedByLocalAxisY( float a_fAngle ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.RotateLocalAxisY( a_fAngle );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedByLocalAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************

CMatrix4 CMatrix4::GetRotatedByLocalAxisZ( float a_fAngle ) const 
{
	CMatrix4	matRotated( *this );
				matRotated.RotateLocalAxisZ( a_fAngle );

	return matRotated;
}

//*****************************************************************************
//	GetNegated
//*****************************************************************************
//
//! \brief	Returns a negated copy of the matrix
//! \param	[out] The negated copy of the matrix
//
//*****************************************************************************
 
void CMatrix4::GetNegated( CMatrix4& a_matNegated ) const
{
	a_matNegated.SetMatrix( *this );
	a_matNegated.Negate();
}

//*****************************************************************************
//	GetInverted
//*****************************************************************************
//
//! \brief	Returns an inverted copy of the matrix
//! \param	[out] The inverted copy of the matrix
//
//*****************************************************************************
 
void CMatrix4::GetInverted( CMatrix4& a_matInverted ) const
{
	if ( IsExcentric() )
	{
		GetInvertedExcentric( a_matInverted );
	}
	else
	{
		a_matInverted.SetMatrix( *this );
		a_matInverted.Invert();
	}
}

//*****************************************************************************
//	GetOrthogonalized
//*****************************************************************************
//
//! \brief	Returns an orthogonalized copy of the matrix
//! \param	[out] The orthogonalized copy of the matrix
//
//*****************************************************************************
 
void CMatrix4::GetOrthogonalized( CMatrix4& a_matOrthogonalized ) const 
{
	a_matOrthogonalized.SetMatrix( *this );
	a_matOrthogonalized.Orthogonalize();
}

//*****************************************************************************
//	GetOrthonormalized
//*****************************************************************************
//
//! \brief	Returns an orthonormalized copy of the matrix
//! \param	[out] The orthonormalized copy of the matrix
//
//*****************************************************************************
 
void CMatrix4::GetOrthonormalized( CMatrix4& a_matOrthonormalized ) const 
{
	a_matOrthonormalized.SetMatrix( *this );
	a_matOrthonormalized.Orthonormalize();
}

//*****************************************************************************
//	GetSum
//*****************************************************************************
//
//! \brief	Returns the sum of this matrix and the specified one
//! \param	[out] The sum
//! \param	[in] The matrix to add
//
//*****************************************************************************
 
void CMatrix4::GetSum( CMatrix4& a_matSum, const CMatrix4& a_matOffset ) const 
{
	a_matSum.SetMatrix( *this );
	a_matSum.Add( a_matOffset );
}

//*****************************************************************************
//	GetDifference
//*****************************************************************************
//
//! \brief	Returns the difference of this matrix and the specified one
//! \param	[out] The difference
//! \param	[in] The matrix to subtract
//
//*****************************************************************************
 
void CMatrix4::GetDifference( CMatrix4& a_matDifference, const CMatrix4& a_matOffset ) const 
{
	a_matDifference.SetMatrix( *this );
	a_matDifference.Sub( a_matOffset );
}


//*****************************************************************************
//	GetProduct
//*****************************************************************************
//
//! \brief	Returns the scalar product of the matrix and the specified scalar factor
//! \param	[out] The product
//! \param	[in] The factor to multiply by
//
//*****************************************************************************
 
void CMatrix4::GetProduct( CMatrix4& a_matProduct, float a_fFactor ) const 
{
	a_matProduct.SetMatrix( *this );
	a_matProduct.Multiply( a_fFactor );
}

//*****************************************************************************
//	GetQuotient
//*****************************************************************************
//
//! \brief	Returns the scalar product of the matrix and the reciprocal of the specified scalar divisor
//! \param	[out] The quotient
//! \param	[in] The factor to divide by
//
//*****************************************************************************
 
void CMatrix4::GetQuotient( CMatrix4& a_matQuotient, float a_fDivisor ) const 
{
	a_matQuotient.SetMatrix( *this );
	a_matQuotient.Divide( a_fDivisor );
}

//*****************************************************************************
//	GetRotated
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The quaternion to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotated( CMatrix4& a_matRotated, const CQuaternion& a_quatRotation ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.Rotate( a_quatRotation );
}

//*****************************************************************************
//	GetRotated
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The axis and angle to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotated( CMatrix4& a_matRotated, const CAxisAngle& a_Rotation ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.Rotate( a_Rotation );
}

//*****************************************************************************
//	GetRotated
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The euler angles to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotated( CMatrix4& a_matRotated, const CEulerAngles& a_Rotation ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.Rotate( a_Rotation );
}

//*****************************************************************************
//	GetRotatedByAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotatedByAxisX( CMatrix4& a_matRotated, float a_fAngle ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateAxisX( a_fAngle );
}

//*****************************************************************************
//	GetRotatedByAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotatedByAxisY( CMatrix4& a_matRotated, float a_fAngle ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateAxisY( a_fAngle );
}

//*****************************************************************************
//	GetRotatedByAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotatedByAxisZ( CMatrix4& a_matRotated, float a_fAngle ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateAxisZ( a_fAngle );
}

//*****************************************************************************
//	GetRotatedLocal
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The quaternion to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotatedLocal( CMatrix4& a_matRotated, const CQuaternion& a_quatRotation ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateLocal( a_quatRotation );
}

//*****************************************************************************
//	GetRotatedLocal
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The axis and angle to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotatedLocal( CMatrix4& a_matRotated, const CAxisAngle& a_Rotation ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateLocal( a_Rotation );
}

//*****************************************************************************
//	GetRotatedLocal
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The euler angles to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotatedLocal( CMatrix4& a_matRotated, const CEulerAngles& a_Rotation ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateLocal( a_Rotation );
}

//*****************************************************************************
//	GetRotatedByLocalAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotatedByLocalAxisX( CMatrix4& a_matRotated, float a_fAngle ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateLocalAxisX( a_fAngle );
}

//*****************************************************************************
//	GetRotatedByLocalAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotatedByLocalAxisY( CMatrix4& a_matRotated, float a_fAngle ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateLocalAxisY( a_fAngle );
}

//*****************************************************************************
//	GetRotatedByLocalAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
 
void CMatrix4::GetRotatedByLocalAxisZ( CMatrix4& a_matRotated, float a_fAngle ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateLocalAxisZ( a_fAngle );
}

//*****************************************************************************
//	operator = 
//*****************************************************************************
//
//! \brief	Copies the specified matrix
//! \param	[in] The matrix to copy
//! \return	A reference to self
//
//*****************************************************************************
 
CMatrix4& CMatrix4::operator = ( const CMatrix4& a_matOther ) 
{
	SetMatrix( a_matOther );

	return (*this);
}

//*****************************************************************************
//	operator +=
//*****************************************************************************
//
//! \brief	Adds the specified matrix to this one
//! \param	[in] The matrix to add
//! \return	A reference to self
//
//*****************************************************************************
 
CMatrix4& CMatrix4::operator += ( const CMatrix4& a_matOffset ) 
{
	Add( a_matOffset );

	return (*this);
}

//*****************************************************************************
//	operator -=
//*****************************************************************************
//
//! \brief	Subtractss the specified matrix from this one
//! \param	[in] The matrix to subtract
//! \return	A reference to self
//
//*****************************************************************************
 
CMatrix4& CMatrix4::operator -= ( const CMatrix4& a_matOffset ) 
{
	Sub( a_matOffset );

	return (*this);
}

//*****************************************************************************
//	operator *=
//*****************************************************************************
//
//! \brief	Multiplies each element by the specified factor
//! \param	[in] The factor to multiply by
//! \return	A reference to self
//
//*****************************************************************************
 
CMatrix4& CMatrix4::operator *= ( float a_fFactor ) 
{
	Multiply( a_fFactor );

	return (*this);
}

//*****************************************************************************
//	operator /=
//*****************************************************************************
//
//! \brief	Divides each element by the specified factor
//! \param	[in] The factor to divide by
//! \return	A reference to self
//
//*****************************************************************************
 
CMatrix4& CMatrix4::operator /= ( float a_fFactor ) 
{
	Divide( a_fFactor );

	return (*this);
}

//*****************************************************************************
//	operator *=
//*****************************************************************************
//
//! \brief	Multiplies the matrix by the specified matrix
//! \param	[in] The matrix to multiply this one by
//! \return	A reference to self
//
//*****************************************************************************
 
CMatrix4& CMatrix4::operator *= ( const CMatrix4& a_matFactor ) 
{
	Multiply( a_matFactor );

	return (*this);
}

//*****************************************************************************
//	operator *=
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified axis and angle
//! \param	[in] The axis and angle to rotate by
//! \return	A reference to self
//
//*****************************************************************************
 
CMatrix4& CMatrix4::operator *= ( const CAxisAngle& a_Rotation ) 
{
	Rotate( a_Rotation );

	return (*this);
}

//*****************************************************************************
//	operator *=
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified euler angles
//! \param	[in] The euler angles to rotate by
//! \return	A reference to self
//
//*****************************************************************************
 
CMatrix4& CMatrix4::operator *= ( const CEulerAngles& a_Rotation ) 
{
	Rotate( a_Rotation );

	return (*this);
}

//*****************************************************************************
//	operator *=
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified quaternion
//! \param	[in] The quaternion to rotate by
//! \return	A reference to self
//
//*****************************************************************************
 
CMatrix4& CMatrix4::operator *= ( const CQuaternion& a_quatRotation ) 
{
	Rotate( a_quatRotation );

	return (*this);
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief	Transforms the specified vector by the matrix
//! \param	[in] The vector to transform
//! \return	The transformed vector
//
//*****************************************************************************
CVector3 CMatrix4::operator * ( const CVector3& a_vec ) const 
{
	CVector3	vecTranformed( a_vec );
				vecTranformed.Transform( *this );

	return vecTranformed;
}
