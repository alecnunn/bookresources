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
//! \param	[in] The array to copy
//
//*****************************************************************************
CMatrix3::CMatrix3( const float a_pfElements [ EElement_Count ] ) 
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
CMatrix3::CMatrix3( const float a_pfRowColumn [ ERow_Count ][ EColumn_Count ] ) 
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
CMatrix3::CMatrix3( const CVector3 a_pvecRows [ ERow_Count ] ) 
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
CMatrix3::CMatrix3( float a_fElements ) 
{
	SetMatrix( a_fElements );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//! \param	[in] The matrix whose rotation to copy
//
//*****************************************************************************
CMatrix3::CMatrix3( const CMatrix4& a_matSource ) 
{
	SetMatrix( a_matSource );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Copy constructor
//! \param	[in] The matrix to copy
//
//*****************************************************************************
CMatrix3::CMatrix3( const CMatrix3& a_matSource ) 
{
	SetMatrix( a_matSource );
}

//*****************************************************************************
//	Constructor
//*****************************************************************************
//
//! \brief	Assignment constructor
//
//*****************************************************************************
CMatrix3::CMatrix3() 
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
CMatrix3::~CMatrix3() 
{
}

//*****************************************************************************
//	GetColumn
//*****************************************************************************
//
//! \brief	Returns a vector representation of the specified column
//! \param	[in] The desired column's index
//! \return	The vector representation of the specified column
//
//*****************************************************************************
CVector3 CMatrix3::GetColumn( int a_iColumn ) const 
{
	CVector3 vecColumn;

	GetColumn( a_iColumn, vecColumn );

	return vecColumn;
}

//*****************************************************************************
//	GetColumn
//*****************************************************************************
//
//! \brief	Returns the specified column's vector representation
//! \param	[in] The desired column's index
//! \param	[out] The specified column's vector representation
//
//*****************************************************************************
void CMatrix3::GetColumn( int a_iColumn, CVector3& a_vecColumn ) const 
{
	a_vecColumn.SetX( GetElement( ERow_RotationAxisX, a_iColumn ) );
	a_vecColumn.SetY( GetElement( ERow_RotationAxisY, a_iColumn ) );
	a_vecColumn.SetZ( GetElement( ERow_RotationAxisZ, a_iColumn ) );
}

//*****************************************************************************
//	GetColumn
//*****************************************************************************
//
//! \brief	Returns the specified column's values
//! \param	[in] The desired column's index
//! \param	[out] The specified column's values
//
//*****************************************************************************
void CMatrix3::GetColumn( int a_iColumn, float a_pfColumn [ CMatrix3::ERow_Count ] ) const 
{
	a_pfColumn[ ERow_RotationAxisX ] = m_pfRowColumn[ ERow_RotationAxisX ][ a_iColumn ];
	a_pfColumn[ ERow_RotationAxisY ] = m_pfRowColumn[ ERow_RotationAxisY ][ a_iColumn ];
	a_pfColumn[ ERow_RotationAxisZ ] = m_pfRowColumn[ ERow_RotationAxisZ ][ a_iColumn ];
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Copies the specified matrix' rotation
//! \param	[in] The matrix whose rotation to copy
//
//*****************************************************************************
void CMatrix3::SetMatrix( const CMatrix4& a_matSource ) 
{
	FOR_EACH_ROW_COLUMN_CMatrix3( iRow, iColumn )
	{
		AccessElement( iRow, iColumn ) = a_matSource.GetElement( iRow, iColumn );
	}
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Sets all elements to the specified value
//! \param	[in] The element to fill the matrix with
//
//*****************************************************************************
void CMatrix3::SetMatrix( float a_fElements ) 
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
//	SetIdentity
//*****************************************************************************
//
//! \brief	Sets the matrix to identity
//
//*****************************************************************************
void CMatrix3::SetIdentity( void ) 
{
	Clear();

	AccessElement( ERow_RotationAxisX, EColumn_X ) = 1.0f;
	AccessElement( ERow_RotationAxisY, EColumn_Y ) = 1.0f;
	AccessElement( ERow_RotationAxisZ, EColumn_Z ) = 1.0f;
}


//*****************************************************************************
//	SetToScaling
//*****************************************************************************
//
//! \brief	Sets the matrix to the specified scaling
//! \param	[in] The scaling factor to set for all axes
//
//*****************************************************************************
void CMatrix3::SetToScaling( float a_fScaling ) 
{
	Clear();

	AccessElement( ERow_RotationAxisX, EColumn_X ) = a_fScaling;
	AccessElement( ERow_RotationAxisY, EColumn_Y ) = a_fScaling;
	AccessElement( ERow_RotationAxisZ, EColumn_Z ) = a_fScaling;
}

//*****************************************************************************
//	SetToScaling
//*****************************************************************************
//
//! \brief	Sets the matrix to the specified scaling
//! \param	[in] The vector containing the scaling factors to set
//
//*****************************************************************************
void CMatrix3::SetToScaling( const CVector3& a_vecScaling ) 
{
	Clear();

	AccessElement( ERow_RotationAxisX, EColumn_X ) = a_vecScaling.GetX();
	AccessElement( ERow_RotationAxisY, EColumn_Y ) = a_vecScaling.GetY();
	AccessElement( ERow_RotationAxisZ, EColumn_Z ) = a_vecScaling.GetZ();
}


//*****************************************************************************
//	SetToRotation
//*****************************************************************************
//
//! \brief	Sets the matrix to the specified axis and angle
//! \param	[in] The axis and angle to set
//
//*****************************************************************************
void CMatrix3::SetToRotation( const CAxisAngle& a_Rotation ) 
{
	//
	//	CHECK FOR IDENTITY
	//
	if( 0.0f == a_Rotation.GetAngle() ) 
	{
		SetIdentity();

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
//	SetToRotationAxisX
//*****************************************************************************
//
//! \brief	Sets the matrix to a rotation along the x axis rotation by the specified angle 
//! \param	[in] The rotation's angle 
//
//*****************************************************************************
void CMatrix3::SetToRotationAxisX( float a_fAngle ) 
{
	SetIdentity();

	float fSin = ::g_sinf( a_fAngle );
	float fCos = ::g_cosf( a_fAngle );

    AccessElement( 1, 1 ) =  fCos;
    AccessElement( 1, 2 ) =  fSin;

    AccessElement( 2, 1 ) = -fSin;
    AccessElement( 2, 2 ) =  fCos;
}

//*****************************************************************************
//	SetToRotationAxisY
//*****************************************************************************
//
//! \brief	Sets the matrix to a rotation along the y axis rotation by the specified angle 
//! \param	[in] The rotation's angle 
//
//*****************************************************************************
void CMatrix3::SetToRotationAxisY( float a_fAngle ) 
{
	SetIdentity();

	float fSin = ::g_sinf( a_fAngle );
	float fCos = ::g_cosf( a_fAngle );

    AccessElement( 0, 0 ) =  fCos;
    AccessElement( 0, 2 ) = -fSin;

    AccessElement( 2, 0 ) =  fSin;
    AccessElement( 2, 2 ) =  fCos;
}

//*****************************************************************************
//	SetToRotationAxisZ
//*****************************************************************************
//
//! \brief	Sets the matrix to a rotation along the z axis rotation by the specified angle 
//! \param	[in] The rotation's angle 
//
//*****************************************************************************
void CMatrix3::SetToRotationAxisZ( float a_fAngle ) 
{
	SetIdentity();

	float fSin = ::g_sinf( a_fAngle );
	float fCos = ::g_cosf( a_fAngle );

    AccessElement( 0, 0 ) =  fCos;
    AccessElement( 0, 1 ) =  fSin;

    AccessElement( 1, 0 ) = -fSin;
    AccessElement( 1, 1 ) =  fCos;
}

//*****************************************************************************
//	SetToCrossProduct
//*****************************************************************************
//
//! \brief	Sets the matrix so that multiplication by a vector will product the specified vector's cross-product (sets all other members to identity).
//! \param	[in] The vector's cross product 
//
//*****************************************************************************
void CMatrix3::SetToCrossProduct( const CVector3& a_vecFactor ) 
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
//! \brief	Sets the matrix to the product of the specified matrices. 
//! 		Though it will work, neither of the specified matrices should be supplied with the matrix itself.
//! \param	[in] The basic matrix  
//! \param	[in] The matrix to multiply by
//
//*****************************************************************************
void CMatrix3::SetToProduct( const CMatrix3& a_matLeft, const CMatrix3& a_matRight ) 
{
	if (	( this == &a_matLeft  )
		||	( this == &a_matRight ) )
	{
		static CMatrix3 matProduct;
		matProduct.SetMatrix( 0.0f );

		FOR_EACH_ROW_COLUMN_CMatrix3( iRow, iColumn )
		FOR_EACH_ROW_CMatrix3		 ( iRowDst		 )
		{
			matProduct.AccessElement( iRow, iColumn ) += a_matLeft .GetElement( iRowDst, iColumn )  
													   * a_matRight.GetElement( iRow, iRowDst	 );
		}

		SetMatrix( matProduct );
	}
	else
	{
		Clear();

		FOR_EACH_ROW_COLUMN_CMatrix3( iRow, iColumn )
		FOR_EACH_ROW_CMatrix3		 ( iRowDst		 )
		{
			AccessElement( iRow, iColumn ) += a_matLeft .GetElement( iRowDst, iColumn )  
											* a_matRight.GetElement( iRow, iRowDst	);
		}
	}
}

//*****************************************************************************
//	SetToInverse
//*****************************************************************************
//
//! \brief	Sets the matrix to the inverse of the specified matrix. 
//! \param	[in] The matrix  whose inverse to set
//
//*****************************************************************************
void CMatrix3::SetToInverse( const CMatrix3& a_matSource ) 
{
	if ( this == &a_matSource )
	{
		static CMatrix3 matInverted;
		
		matInverted.SetToInverse( *this );

		SetMatrix( matInverted );
	}
	else
	{
		float fDet    = a_matSource.GetDeterminant();
		float fDetInv = 1.0f / fDet;

		AccessElement(0,0) =  fDetInv * ( a_matSource(1,1) * a_matSource(2,2) - a_matSource(1,2) * a_matSource(2,1) );
		AccessElement(0,1) = -fDetInv * ( a_matSource(0,1) * a_matSource(2,2) - a_matSource(0,2) * a_matSource(2,1) );
		AccessElement(0,2) =  fDetInv * ( a_matSource(0,1) * a_matSource(1,2) - a_matSource(0,2) * a_matSource(1,1) );

		AccessElement(1,0) = -fDetInv * ( a_matSource(1,0) * a_matSource(2,2) - a_matSource(1,2) * a_matSource(2,0) );
		AccessElement(1,1) =  fDetInv * ( a_matSource(0,0) * a_matSource(2,2) - a_matSource(0,2) * a_matSource(2,0) );
		AccessElement(1,2) = -fDetInv * ( a_matSource(0,0) * a_matSource(1,2) - a_matSource(0,2) * a_matSource(1,0) );

		AccessElement(2,0) =  fDetInv * ( a_matSource(1,0) * a_matSource(2,1) - a_matSource(1,1) * a_matSource(2,0) );
		AccessElement(2,1) = -fDetInv * ( a_matSource(0,0) * a_matSource(2,1) - a_matSource(0,1) * a_matSource(2,0) );
		AccessElement(2,2) =  fDetInv * ( a_matSource(0,0) * a_matSource(1,1) - a_matSource(0,1) * a_matSource(1,0) );
	}
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Sets all rows to the specified vectors
//! \param	[in] The vector representations of the rows to set
//
//*****************************************************************************
void CMatrix3::SetMatrix( const CVector3 a_pvecRows [ CMatrix3::ERow_Count ] ) 
{
	AccessRow( ERow_RotationAxisX ) = a_pvecRows[ ERow_RotationAxisX ];	
	AccessRow( ERow_RotationAxisY ) = a_pvecRows[ ERow_RotationAxisY ];	
	AccessRow( ERow_RotationAxisZ ) = a_pvecRows[ ERow_RotationAxisZ ];	
}

//*****************************************************************************
//	Negate
//*****************************************************************************
//
//! \brief	Negates the matrix
//
//*****************************************************************************
void CMatrix3::Negate( void ) 
{
	FOR_EACH_ELEMENT_CMatrix3( iElement )
	{
		m_pfElements[ iElement ] = -m_pfElements[ iElement ];
	}
}

//*****************************************************************************
//	Transpose
//*****************************************************************************
//
//! \brief	Transposes the matrix
//
//*****************************************************************************
void CMatrix3::Transpose( void ) 
{
	float fSwap;

	SWAP_CMatrix3( fSwap, ERow_RotationAxisX, EColumn_Y );
	SWAP_CMatrix3( fSwap, ERow_RotationAxisX, EColumn_Z );
	SWAP_CMatrix3( fSwap, ERow_RotationAxisY, EColumn_Z );
}

//*****************************************************************************
//	Add
//*****************************************************************************
//
//! \brief	Adds the specified matrix to this one
//! \param	[in] The matrix to add
//
//*****************************************************************************
void CMatrix3::Add( const CMatrix3& a_mat ) 
{
	FOR_EACH_ELEMENT_CMatrix3( iElement )
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
void CMatrix3::Sub( const CMatrix3& a_mat ) 
{
	FOR_EACH_ELEMENT_CMatrix3( iElement )
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
void CMatrix3::Multiply( float a_fFactor ) 
{
	FOR_EACH_ELEMENT_CMatrix3( iElement )
	{
		m_pfElements[ iElement ] *= a_fFactor;
	}
}

//*****************************************************************************
//	Orthogonalize								 
//*****************************************************************************
//
//! \brief	Orthogonalizes the matrix
//
//*****************************************************************************
void CMatrix3::Orthogonalize( void ) 
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
//	Orthonormalize
//*****************************************************************************
//
//! \brief	Orthonormalizes the matrix
//
//*****************************************************************************
#pragma warning ( push )
#pragma warning ( disable : 4189 )	// unreferenced local variable

void CMatrix3::Orthonormalize( void ) 
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

	//
	// make the up vector perpendicular to the at vector:
	//
	float fUpAtProduct = CVector3::DotProduct( GetZAxis(), GetYAxis() );
	AccessYAxis() -= fUpAtProduct * GetZAxis();

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
		float fUpAtProduct = CVector3::DotProduct( GetZAxis(), GetYAxis() );
		AccessYAxis() -= fUpAtProduct * GetZAxis();

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
			float fUpAtProduct = CVector3::DotProduct( GetZAxis(), GetYAxis() );
			AccessYAxis() -= fUpAtProduct * GetZAxis();

			//
			// get the up vector's length
			//
			float fUpSquareMagnitude = GetYAxis().GetSquareMagnitude();
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

#pragma warning ( pop )

//*****************************************************************************
//	GetDeterminantAdjoint
//*****************************************************************************
//
//! \brief	Determines the determinant for the matrix adjoint to the specified element
//! \param	[in] The index of the row to omit
//! \param	[in] The index of the column to omit
//
//*****************************************************************************
float CMatrix3::GetDeterminantAdjoint( int a_iRow, int a_iColumn ) const 
{
	U32 iRowNext    = ( a_iRow                  + 1 ) % ERow_Count;
	U32 iRowPrev    = ( a_iRow + ERow_Count		- 1 ) % ERow_Count;
	U32 iColumnNext = ( a_iColumn				+ 1 ) % ERow_Count;
	U32 iColumnPrev = ( a_iColumn + ERow_Count	- 1 ) % ERow_Count;

	float fDet = m_pfRowColumn[iRowNext][iColumnNext] * m_pfRowColumn[iRowPrev][iColumnPrev]
				-  m_pfRowColumn[iRowNext][iColumnPrev] * m_pfRowColumn[iRowPrev][iColumnNext];

	return ( 0 == ( ( a_iRow + a_iColumn ) & 1 ) ?  fDet : -fDet );
}

//*****************************************************************************
//	SetToRotation
//*****************************************************************************
//
//! \brief	Sets the matrix to the specified euler angles
//! \param	[in] The euler angles to set
//
//*****************************************************************************
void CMatrix3::SetToRotation( const CEulerAngles& a_Rotation ) 
{
	float fSinX = ::g_sinf( a_Rotation.GetPitch	() );
	float fCosX = ::g_cosf( a_Rotation.GetPitch	() );
	float fSinY = ::g_sinf( a_Rotation.GetYaw	() );
	float fCosY = ::g_cosf( a_Rotation.GetYaw	() );	
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
//	SetToRotation
//*****************************************************************************
//
//! \brief	SetTos the matrix to the specified quaternion
//! \param	[in] The quaternion to set
//
//*****************************************************************************
void CMatrix3::SetToRotation( const CQuaternion& a_quatRotation ) 
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
//	Scale
//*****************************************************************************
//
//! \brief	Scales the matrix by the specified factor
//! \param	[in] The factor to scale by
//
//*****************************************************************************
void CMatrix3::Scale( float a_fScaling ) 
{
	CMatrix3	matScale;
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
void CMatrix3::Scale( const CVector3& a_vecScaling ) 
{
	CMatrix3	matScale;
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
void CMatrix3::Rotate( const CQuaternion& a_quatRotation ) 
{
	CMatrix3	matRotation;
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
void CMatrix3::Rotate( const CAxisAngle& a_Rotation ) 
{
	CMatrix3	matRotation;
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
void CMatrix3::Rotate( const CEulerAngles& a_Rotation ) 
{
	CMatrix3	matRotation;
				matRotation.SetToRotation( a_Rotation );

	ReverseMultiply( matRotation );
}

//*****************************************************************************
//	RotateAxisX
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified angle along the x axis 
//! \param	[in] The angle to rotate by along the x axis
//
//*****************************************************************************
void CMatrix3::RotateAxisX( float a_fAngle ) 
{
	CMatrix3	matRotation;
				matRotation.SetToRotationAxisX( a_fAngle );

	ReverseMultiply( matRotation );
}

//*****************************************************************************
//	RotateAxisY
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified angle along the y axis 
//! \param	[in] The angle to rotate by along the y axis
//
//*****************************************************************************
void CMatrix3::RotateAxisY( float a_fAngle ) 
{
	CMatrix3	matRotation;
				matRotation.SetToRotationAxisY( a_fAngle );

	ReverseMultiply( matRotation );
}

//*****************************************************************************
//	RotateAxisZ
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified angle along the z axis 
//! \param	[in] The angle to rotate by along the z axis
//
//*****************************************************************************
void CMatrix3::RotateAxisZ( float a_fAngle ) 
{
	CMatrix3	matRotation;
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
void CMatrix3::RotateLocal( const CQuaternion& a_quatRotation ) 
{
	CMatrix3	matRotation;
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
void CMatrix3::RotateLocal( const CAxisAngle& a_Rotation ) 
{
	CMatrix3	matRotation;
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
void CMatrix3::RotateLocal( const CEulerAngles& a_Rotation ) 
{
	CMatrix3	matRotation;
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
void CMatrix3::RotateLocalAxisX( float a_fAngle ) 
{
	CMatrix3	matRotation;
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
void CMatrix3::RotateLocalAxisY( float a_fAngle ) 
{
	CMatrix3	matRotation;
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
void CMatrix3::RotateLocalAxisZ( float a_fAngle ) 
{
	CMatrix3	matRotation;
				matRotation.SetToRotationAxisZ( a_fAngle );

	Multiply( matRotation );
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
bool CMatrix3::IsIdentity() const
{
	float fIdentity;

	FOR_EACH_ROW_COLUMN_CMatrix3( iRow, iColumn )
	{
		fIdentity = ( ( iRow == iColumn ) ? 1.0f : 0.0f );	// Diagonal elements must be 1.0f, all other elements must be zero

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
bool CMatrix3::IsEqual( const CMatrix3& a_matOther ) const
{
	FOR_EACH_ELEMENT_CMatrix3( iElement )
	{
		if ( GetElement( iElement ) != a_matOther.GetElement( iElement ) )
		{
			return false;
		}
	}

	return true;
}

//*****************************************************************************
//	GetIdentity
//*****************************************************************************
//
//! \brief	Returns an identity matrix
//! \return	An identity matrix
//
//*****************************************************************************
CMatrix3 CMatrix3::GetIdentity()
{
	CMatrix3	matIdentity;
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
CMatrix3 CMatrix3::GetNegated() const
{
	CMatrix3	matNegated( *this );
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
CMatrix3 CMatrix3::GetInverted() const
{
	CMatrix3 matInverted;

	GetInverted( matInverted );

	return matInverted;
}

//*****************************************************************************
//	GetTransposed
//*****************************************************************************
//
//! \brief	Returns a transposed copy of the matrix
//! \return	The transposed copy of the matrix
//
//*****************************************************************************
CMatrix3 CMatrix3::GetTransposed() const
{
	CMatrix3 matTransposed;

	GetTransposed( matTransposed );

	return matTransposed;
}

//*****************************************************************************
//	GetOrthogonalized
//*****************************************************************************
//
//! \brief	Returns an orthogonalized copy of the matrix
//! \return	The orthogonalized copy of the matrix
//
//*****************************************************************************
CMatrix3 CMatrix3::GetOrthogonalized( void ) const 
{
	CMatrix3	matOrthogonalized( *this );
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
CMatrix3 CMatrix3::GetOrthonormalized( void ) const 
{
	CMatrix3	matOrthonormalized( *this );
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
CMatrix3 CMatrix3::GetSum( const CMatrix3& a_matOffset ) const 
{
	CMatrix3	matSum( *this );
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
CMatrix3 CMatrix3::GetDifference( const CMatrix3& a_matOffset ) const 
{
	CMatrix3	matDifference( *this );
				matDifference.Sub( a_matOffset );

	return matDifference;
}

//*****************************************************************************
//	GetProductPrefix
//*****************************************************************************
//
//! \brief	Returns the matrix product of the specified matrix and this matrix 
//! \param	[in] The matrix to multiply
//! \return	The product
//
//*****************************************************************************
CMatrix3 CMatrix3::GetProductPrefix( const CMatrix3& a_matPrefix ) const 
{
	CMatrix3 matProduct;

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
CMatrix3 CMatrix3::GetProduct( const CMatrix3& a_matFactor ) const 
{
	CMatrix3 matProduct;

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
CMatrix3 CMatrix3::GetProduct( float a_fFactor ) const 
{
	CMatrix3	matProduct( *this );
				matProduct.Multiply( a_fFactor );

	return matProduct;
}

//*****************************************************************************
//	GetQuotient
//*****************************************************************************
//
//! \brief	Returns the scalar product of the matrix and the reciprocal of the specified scalar divisor
//! \param	[in] The factor to divide by
//! \return	The quotient
//
//*****************************************************************************
CMatrix3 CMatrix3::GetQuotient( float a_fDivisor ) const 
{
	CMatrix3	matQuotient( *this );
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
CMatrix3 CMatrix3::GetRotated( const CQuaternion& a_quatRotation ) const 
{
	CMatrix3	matRotated( *this );
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
CMatrix3 CMatrix3::GetRotated( const CAxisAngle& a_Rotation ) const 
{
	CMatrix3	matRotated( *this );
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
CMatrix3 CMatrix3::GetRotated( const CEulerAngles& a_Rotation ) const 
{
	CMatrix3	matRotated( *this );
				matRotated.Rotate( a_Rotation );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the x axis
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
CMatrix3 CMatrix3::GetRotatedAxisX( float a_fAngle ) const 
{
	CMatrix3	matRotated( *this );
				matRotated.RotateAxisX( a_fAngle );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the y axis
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
CMatrix3 CMatrix3::GetRotatedAxisY( float a_fAngle ) const 
{
	CMatrix3	matRotated( *this );
				matRotated.RotateAxisY( a_fAngle );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the z axis
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
CMatrix3 CMatrix3::GetRotatedAxisZ( float a_fAngle ) const 
{
	CMatrix3	matRotated( *this );
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
CMatrix3 CMatrix3::GetRotatedLocal( const CQuaternion& a_quatRotation ) const 
{
	CMatrix3	matRotated( *this );
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
CMatrix3 CMatrix3::GetRotatedLocal( const CAxisAngle& a_Rotation ) const 
{
	CMatrix3	matRotated( *this );
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
CMatrix3 CMatrix3::GetRotatedLocal( const CEulerAngles& a_Rotation ) const 
{
	CMatrix3	matRotated( *this );
				matRotated.RotateLocal( a_Rotation );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedLocalAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the x axis
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
CMatrix3 CMatrix3::GetRotatedLocalAxisX( float a_fAngle ) const 
{
	CMatrix3	matRotated( *this );
				matRotated.RotateLocalAxisX( a_fAngle );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedLocalAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the y axis
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
CMatrix3 CMatrix3::GetRotatedLocalAxisY( float a_fAngle ) const 
{
	CMatrix3	matRotated( *this );
				matRotated.RotateLocalAxisY( a_fAngle );

	return matRotated;
}

//*****************************************************************************
//	GetRotatedLocalAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the z axis
//! \param	[in] The angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
CMatrix3 CMatrix3::GetRotatedLocalAxisZ( float a_fAngle ) const 
{
	CMatrix3	matRotated( *this );
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
void CMatrix3::GetNegated( CMatrix3& a_matNegated ) const
{
	a_matNegated.SetMatrix( *this );
	a_matNegated.Negate();
}

//*****************************************************************************
//	GetTransposed
//*****************************************************************************
//
//! \brief	Returns an transposed copy of the matrix
//! \param	[out] The transposed copy of the matrix
//
//*****************************************************************************
void CMatrix3::GetTransposed( CMatrix3& a_matTransposed ) const
{
	a_matTransposed.SetMatrix( *this );
	a_matTransposed.Transpose();
}

//*****************************************************************************
//	GetOrthogonalized
//*****************************************************************************
//
//! \brief	Returns an orthogonalized copy of the matrix
//! \param	[out] The orthogonalized copy of the matrix
//
//*****************************************************************************
void CMatrix3::GetOrthogonalized( CMatrix3& a_matOrthogonalized ) const 
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
void CMatrix3::GetOrthonormalized( CMatrix3& a_matOrthonormalized ) const 
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
void CMatrix3::GetSum( CMatrix3& a_matSum, const CMatrix3& a_matOffset ) const 
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
void CMatrix3::GetDifference( CMatrix3& a_matDifference, const CMatrix3& a_matOffset ) const 
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
void CMatrix3::GetProduct( CMatrix3& a_matProduct, float a_fFactor ) const 
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
void CMatrix3::GetQuotient( CMatrix3& a_matQuotient, float a_fDivisor ) const 
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
void CMatrix3::GetRotated( CMatrix3& a_matRotated, const CQuaternion& a_quatRotation ) const 
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
void CMatrix3::GetRotated( CMatrix3& a_matRotated, const CAxisAngle& a_Rotation ) const 
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
void CMatrix3::GetRotated( CMatrix3& a_matRotated, const CEulerAngles& a_Rotation ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.Rotate( a_Rotation );
}

//*****************************************************************************
//	GetRotatedAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the x axis
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
void CMatrix3::GetRotatedAxisX( CMatrix3& a_matRotated, float a_fAngle ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateAxisX( a_fAngle );
}

//*****************************************************************************
//	GetRotatedAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the y axis
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
void CMatrix3::GetRotatedAxisY( CMatrix3& a_matRotated, float a_fAngle ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateAxisY( a_fAngle );
}

//*****************************************************************************
//	GetRotatedAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the z axis
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
void CMatrix3::GetRotatedAxisZ( CMatrix3& a_matRotated, float a_fAngle ) const 
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
void CMatrix3::GetRotatedLocal( CMatrix3& a_matRotated, const CQuaternion& a_quatRotation ) const 
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
void CMatrix3::GetRotatedLocal( CMatrix3& a_matRotated, const CAxisAngle& a_Rotation ) const 
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
void CMatrix3::GetRotatedLocal( CMatrix3& a_matRotated, const CEulerAngles& a_Rotation ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateLocal( a_Rotation );
}

//*****************************************************************************
//	GetRotatedLocalAxisX
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the x axis
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
void CMatrix3::GetRotatedLocalAxisX( CMatrix3& a_matRotated, float a_fAngle ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateLocalAxisX( a_fAngle );
}

//*****************************************************************************
//	GetRotatedLocalAxisY
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the y axis
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
void CMatrix3::GetRotatedLocalAxisY( CMatrix3& a_matRotated, float a_fAngle ) const 
{
	a_matRotated.SetMatrix( *this );
	a_matRotated.RotateLocalAxisY( a_fAngle );
}

//*****************************************************************************
//	GetRotatedLocalAxisZ
//*****************************************************************************
//
//! \brief	Returns a rotated copy of the matrix by the specified angle along the z axis
//! \param	[out] The rotated matrix
//! \param	[in] The angle to rotate by
//
//*****************************************************************************
void CMatrix3::GetRotatedLocalAxisZ( CMatrix3& a_matRotated, float a_fAngle ) const 
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
CMatrix3& CMatrix3::operator = ( const CMatrix3& a_matOther ) 
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
CMatrix3& CMatrix3::operator += ( const CMatrix3& a_matOffset ) 
{
	Add( a_matOffset );

	return (*this);
}

//*****************************************************************************
//	operator -=
//*****************************************************************************
//
//! \brief	Subtracts the specified matrix from this one
//! \param	[in] The matrix to subtract
//! \return	A reference to self
//
//*****************************************************************************
CMatrix3& CMatrix3::operator -= ( const CMatrix3& a_matOffset ) 
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
CMatrix3& CMatrix3::operator *= ( float a_fFactor ) 
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
CMatrix3& CMatrix3::operator /= ( float a_fFactor ) 
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
CMatrix3& CMatrix3::operator *= ( const CMatrix3& a_matFactor ) 
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
CMatrix3& CMatrix3::operator *= ( const CAxisAngle& a_Rotation ) 
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
CMatrix3& CMatrix3::operator *= ( const CEulerAngles& a_Rotation ) 
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
CMatrix3& CMatrix3::operator *= ( const CQuaternion& a_quatRotation ) 
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
CVector3 CMatrix3::operator * ( const CVector3& a_vec ) const 
{
	CVector3	vecTranformed( a_vec );
				vecTranformed.Transform( *this );

	return vecTranformed;
}
