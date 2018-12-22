#ifndef			CMATRIX3_H_INCLUDED
#define			CMATRIX3_H_INCLUDED

#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once
#endif 
#endif 

#include		"Basics.h"
#include		<memory>

class			CVector3			;
class			CMatrix4			;
class			CAxisAngle			;
class			CQuaternion			;
class			CEulerAngles		;
class			CSphericalRotation	;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CMatrix3
//*****************************************************************************
//
//!	Representation of a 3x3 matrix
//
//*****************************************************************************
class CMatrix3
{

public:

	// Enums

	enum	ERow											//! Matrix' row indices
	{
		ERow_RotationAxisX							,		//!< Row containing x axis rotation scalars
		ERow_RotationAxisY							,		//!< Row containing y axis rotation scalars
		ERow_RotationAxisZ							,		//!< Row containing z axis rotation scalars
		ERow_Count									,		//!< The number of rows
		ERow_Min			= ERow_RotationAxisX	,		//!< The first row
		ERow_Max			= ERow_RotationAxisZ	,		//!< The last row
		ERow_ForceDWORD		= FORCE_DWORD					//!< Forces all compilers to reserve 32 bits for the enumeration
	};

	enum	EColumn											//! Matrix' column indices
	{
		EColumn_X							,				//!< Column containing x coordinate scalars / offsets
		EColumn_Y							,				//!< Column containing y coordinate scalars / offsets
		EColumn_Z							,				//!< Column containing z coordinate scalars / offsets
		EColumn_Count						,				//!< The number of columns
		EColumn_Min			= EColumn_X		,				//!< The first column
		EColumn_Max			= EColumn_Z		,				//!< The last column
		EColumn_ForceDWORD	= FORCE_DWORD					//!< Forces all compilers to reserve 32 bits for the enumeration
	};

	enum	EElement										//! Matrix' element indices
	{
		EElement_11							,				//!< x axis rotation scalar for x coordinate 
		EElement_12							,				//!< x axis rotation scalar for y coordinate 
		EElement_13							,				//!< x axis rotation scalar for z coordinate 
		EElement_21							,				//!< y axis rotation scalar for x coordinate 
		EElement_22							,				//!< y axis rotation scalar for y coordinate 
		EElement_23							,				//!< y axis rotation scalar for z coordinate
		EElement_31							,				//!< z axis rotation scalar for x coordinate  
		EElement_32							,				//!< z axis rotation scalar for y coordinate  
		EElement_33							,				//!< z axis rotation scalar for z coordinate 
		EElement_Count						,				//!< The number of elements
		EElement_Min		= EElement_11	,				//!< The first element
		EElement_Max		= EElement_33	,				//!< The last element
		EElement_ForceDWORD	= FORCE_DWORD					//!< Forces all compilers to reserve 32 bits for the enumeration
	};

protected:

	// Attribs

	union
	{
		float	m_pfElements [ EElement_Count ];				//!< Matrix' elements 
		float	m_pfRowColumn[ ERow_Count ][ EColumn_Count ];	//!< Matrix' elements split into row and column
	};																																
																																	
public:

	// Con-/Destruction

	explicit CMatrix3( const	float			a_pfElements	[ EElement_Count ]				)											;
	explicit CMatrix3( const	float			a_pfRowColumn	[ ERow_Count ][ EColumn_Count ]	)											;
	explicit CMatrix3( const	CVector3	    a_pvecRows		[ ERow_Count ]					)											;
	explicit CMatrix3(			float			a_fElements	)																				;
	explicit CMatrix3( const	CMatrix4&		a_matSource )																				;
			 CMatrix3( const	CMatrix3&		a_matSource )																				;
			 CMatrix3()																														;	
			~CMatrix3()																														;

	// Get/Set

			float&		AccessElement			(		int				a_iRow														
												,		int				a_iColumn	)														;
			float&		AccessElement			(		int				a_iElement	)														;
			CVector3&	AccessRow				(		int				a_iRow		)														;
			CVector3&	AccessXAxis				( void )																					;
			CVector3&	AccessYAxis				( void )																					;
			CVector3&	AccessZAxis				( void )																					;
																																	
			float		GetElement				(		int				a_iRow														
												,		int				a_iColumn	)												const	;
			float		GetElement				(		int				a_iElement	)												const	;
	const	CVector3&	GetRow					(		int				a_iRow		)												const	;
			CVector3	GetColumn				(		int				a_iColumn	)												const	;
	const	CVector3&	GetXAxis				( void )																			const	;
	const	CVector3&	GetYAxis				( void )																			const	;
	const	CVector3&	GetZAxis				( void )																			const	;
																																	
			void		GetElement				(		int				a_iRow														
												,		int				a_iComumn													
												,		float&			a_fElement										)			const	;
			void		GetElement				(		int				a_iElement													
												,		float&			a_fElement										)			const	;
			void		GetRow					(		int				a_iRow 														
												,		CVector3&		a_vecRow										)			const	;
			void		GetRow					(		int				a_iRow 														
												,		float			a_pfRow			[ EColumn_Count ]				)			const	;
			void		GetColumn				(		int				a_iColumn 													
												,		CVector3&		a_vecColumn										)			const	;
			void		GetColumn				(		int				a_iColumn 													
												,		float			a_pfColumn		[ ERow_Count ]					)			const	;									
			void		GetXAxis				(		CVector3&		a_vecAxisX										)			const	;
			void		GetYAxis				(		CVector3&		a_vecAxisY										)			const	;
			void		GetZAxis				(		CVector3&		a_vecAxisZ										)			const	;
																																	
			void		SetIdentity				( void )																					;
			void		SetElement				(		int				a_iElement													
												,		int				a_iColumn													
												,		float			a_fElement										)					;
			void		SetElement				(		int				a_iElement													
												,		float			a_fElement										)					;
			void		SetRow					(		int				a_iRow 														
												, const	CVector3&		a_vecRow										)					;
			void		SetRow					(		int				a_iRow														
												, const	float			a_pfRow			[ EColumn_Count ]				)					;
			void		ModifyXAxis				( const	CVector3&		a_vecAxisX										)					;
			void		ModifyYAxis				( const	CVector3&		a_vecAxisY										)					;
			void		ModifyZAxis				( const	CVector3&		a_vecAxisZ										)					;
			void		SetToScaling			(		float			a_fScaling										)					;
			void		SetToScaling			( const	CVector3&		a_vecScaling									)					;
			void		SetToRotation			( const	CQuaternion&	a_quatRotation									)					;
			void		SetToRotation			( const	CAxisAngle&		a_Rotation										)					;
			void		SetToRotation			( const	CEulerAngles&	a_Rotation										)					;
			void		SetToRotationAxisX		(		float			a_fAngle										)					;
			void		SetToRotationAxisY		(		float			a_fAngle										)					;
			void		SetToRotationAxisZ		(		float			a_fAngle										)					;
			void		SetToCrossProduct		( const	CVector3&		a_vecFactor										)					;
			void		SetToProduct			( const	CMatrix3&		a_matLeft															
												, const	CMatrix3&		a_matRight										)					;
			void		SetToInverse			( const	CMatrix3&		a_matSource										)					;
																																	
			void		SetMatrix				( const CMatrix4&		a_matSource										)					;
			void		SetMatrix				( const CMatrix3&		a_matSource										)					;
			void		SetMatrix				(		float			a_fElements										)					;
			void		SetMatrix				( const	CVector3	    a_pvecRows	 [ ERow_Count ]						)					;
			void		SetMatrix				( const	float			a_pfRowColumn[ ERow_Count ][ EColumn_Count ]	)					;
			void		SetMatrix				( const	float			a_pfElements [ EElement_Count ]					)					;

	// Modification

			void		Clear					( void )																					;
			void		Negate					( void )																					;
			void		Invert					( void )																					;
			void		Transpose				( void )																					;
			void		Orthogonalize			( void )																					;
			void		Orthonormalize			( void )																					;
																																			
			void		Add						( const	CMatrix3&		a_mat			)													;
			void		Sub						( const	CMatrix3&		a_mat			)													;
			void		Multiply				(		float			a_fFactor		)													;
			void		Divide					(		float			a_fDivisor		)													;
			void		ReverseMultiply			( const CMatrix3&		a_matPrefix		)													;
			void		Multiply				( const CMatrix3&		a_matFactor		)													;
			void		Scale					(		float			a_fScaling		)													;
			void		Scale					( const CVector3&		a_vecScaling	)													;
			void		Rotate					( const	CQuaternion&	a_quatRotation	)													;
			void		Rotate					( const	CAxisAngle&		a_Rotation		)													;
			void		Rotate					( const	CEulerAngles&	a_Rotation		)													;
			void		RotateAxisX				(		float			a_fAngle		)													;
			void		RotateAxisY				(		float			a_fAngle		)													;
			void		RotateAxisZ				(		float			a_fAngle		)													;
			void		RotateLocal				( const	CQuaternion&	a_quatRotation	)													;
			void		RotateLocal				( const	CAxisAngle&		a_Rotation		)													;
			void		RotateLocal				( const	CEulerAngles&	a_Rotation		)													;
			void		RotateLocalAxisX		(		float			a_fAngle		)													;
			void		RotateLocalAxisY		(		float			a_fAngle		)													;
			void		RotateLocalAxisZ		(		float			a_fAngle		)													;

	// Computations

			bool		IsIdentity				( void )																			const	;
			bool		IsEqual					( const	CMatrix3&		a_matOther		)											const	;
																																	
			float		GetDeterminant			( void )																			const	;
			float		GetDeterminantAdjoint	(		int				a_iRow														
												,		int				a_iColumn		)											const	;																
	static	CMatrix3	GetIdentity				( void )																					;
			CMatrix3	GetNegated				( void )																			const	;
			CMatrix3	GetInverted				( void )																			const	;
			CMatrix3	GetTransposed			( void )																			const	;
			CMatrix3	GetOrthogonalized		( void )																			const	;
			CMatrix3	GetOrthonormalized		( void )																			const	;
																																			
			CMatrix3	GetSum					( const CMatrix3&		a_matOffset		)											const	;
			CMatrix3	GetDifference			( const CMatrix3&		a_matOffset		)											const	;
			CMatrix3	GetProductPrefix		( const CMatrix3&		a_matPrefix		)											const	;
			CMatrix3	GetProduct				( const CMatrix3&		a_matFactor		)											const	;
			CMatrix3	GetProduct				(		float			a_fFactor		)											const	;
			CMatrix3	GetQuotient				(		float			a_fDivisor		)											const	;
			CMatrix3	GetRotated				( const	CQuaternion&	a_quatRotation	)											const	;
			CMatrix3	GetRotated				( const	CAxisAngle&		a_Rotation		)											const	;
			CMatrix3	GetRotated				( const	CEulerAngles&	a_Rotation		)											const	;
			CMatrix3	GetRotatedAxisX			(		float			a_fAngle		)											const	;
			CMatrix3	GetRotatedAxisY			(		float			a_fAngle		)											const	;
			CMatrix3	GetRotatedAxisZ			(		float			a_fAngle		)											const	;
			CMatrix3	GetRotatedLocal			( const	CQuaternion&	a_quatRotation	)											const	;
			CMatrix3	GetRotatedLocal			( const	CAxisAngle&		a_Rotation		)											const	;
			CMatrix3	GetRotatedLocal			( const	CEulerAngles&	a_Rotation		)											const	;
			CMatrix3	GetRotatedLocalAxisX	(		float			a_fAngle		)											const	;
			CMatrix3	GetRotatedLocalAxisY	(		float			a_fAngle		)											const	;
			CMatrix3	GetRotatedLocalAxisZ	(		float			a_fAngle		)											const	;
																															
	static	void		GetIdentity				(		CMatrix3&		a_matIdentity			)											;
			void		GetNegated				(		CMatrix3&		a_matNegated			)									const	;
			void		GetInverted				(		CMatrix3&		a_matInverted			)									const	;
			void		GetTransposed			(		CMatrix3&		a_matTransposed			)									const	;
			void		GetOrthogonalized		(		CMatrix3&		a_matOrthogonalized		)									const	;
			void		GetOrthonormalized		(		CMatrix3&		a_matOrthonormalized	)									const	;
																																			
			void		GetSum					(		CMatrix3&		a_matSum		, const CMatrix3&		a_matOffset		)	const	;
			void		GetDifference			(		CMatrix3&		a_matDifference	, const CMatrix3&		a_matOffset		)	const	;
			void		GetProductPrefix		(		CMatrix3&		a_matProduct	, const CMatrix3&		a_matPrefix		)	const	;
			void		GetProduct				(		CMatrix3&		a_matProduct	, const CMatrix3&		a_matFactor		)	const	;
			void		GetProduct				(		CMatrix3&		a_matProduct	,		float			a_fFactor		)	const	;
			void		GetQuotient				(		CMatrix3&		a_matQuotient	,		float			a_fDivisor		)	const	;
			void		GetRotated				(		CMatrix3&		a_matRotated	, const	CQuaternion&	a_quatRotation	)	const	;
			void		GetRotated				(		CMatrix3&		a_matRotated	, const	CAxisAngle&		a_Rotation		)	const	;
			void		GetRotated				(		CMatrix3&		a_matRotated	, const	CEulerAngles&	a_Rotation		)	const	;
			void		GetRotatedAxisX			(		CMatrix3&		a_matRotated	,		float			a_fAngle		)	const	;
			void		GetRotatedAxisY			(		CMatrix3&		a_matRotated	,		float			a_fAngle		)	const	;
			void		GetRotatedAxisZ			(		CMatrix3&		a_matRotated	,		float			a_fAngle		)	const	;
			void		GetRotatedLocal			(		CMatrix3&		a_matRotated	, const	CQuaternion&	a_quatRotation	)	const	;
			void		GetRotatedLocal			(		CMatrix3&		a_matRotated	, const	CAxisAngle&		a_Rotation		)	const	;
			void		GetRotatedLocal			(		CMatrix3&		a_matRotated	, const	CEulerAngles&	a_Rotation		)	const	;
			void		GetRotatedLocalAxisX	(		CMatrix3&		a_matRotated	,		float			a_fAngle		)	const	;
			void		GetRotatedLocalAxisY	(		CMatrix3&		a_matRotated	,		float			a_fAngle		)	const	;
			void		GetRotatedLocalAxisZ	(		CMatrix3&		a_matRotated	,		float			a_fAngle		)	const	;

	// Operators

			bool		operator	==			( const CMatrix3&		a_matOther		)											const	;
			bool		operator	!=			( const CMatrix3&		a_matOther		)											const	;
			CMatrix3&	operator	=			( const	CMatrix3&		a_matOther		)													;
			CMatrix3&	operator	+=			( const CMatrix3&		a_matOffset		)													;
			CMatrix3&	operator	-=			( const CMatrix3&		a_matOffset		)													;
			CMatrix3&	operator	*=			(		float			a_fFactor		)													;
			CMatrix3&	operator	/=			(		float			a_fFactor		)													;
			CMatrix3&	operator	*=			( const CMatrix3&		a_matFactor		)													;
			CMatrix3&	operator	*=			( const CAxisAngle&		a_Rotation		)													;
			CMatrix3&	operator	*=			( const CEulerAngles&	a_Rotation		)													;
			CMatrix3&	operator	*=			( const CQuaternion&	a_quatRotation	)													;
			CMatrix3	operator	~			( void )																			const	;
			CMatrix3	operator	-			( void )																			const	;
			CMatrix3	operator	+			( const CMatrix3&		a_matOffset		)											const	;
			CMatrix3	operator	-			( const CMatrix3&		a_matOffset		)											const	;
			CMatrix3	operator	*			(		float			a_fFactor		)											const	;
			CMatrix3	operator	/			(		float			a_fFactor		)											const	;
			CMatrix3	operator	*			( const CMatrix3&		a_matFactor		)											const	;
			CVector3	operator	*			( const CVector3&		a_vec			)											const	;
			CMatrix3	operator	*			( const CAxisAngle&		a_Rotation		)											const	;
			CMatrix3	operator	*			( const CEulerAngles&	a_Rotation		)											const	;
			CMatrix3	operator	*			( const CQuaternion&	a_quatRotation	)											const	;
	friend	CMatrix3	operator	*			(		float			a_fFactor
												, const CMatrix3&		a_matFactor		)													;
			float		operator	[]			(		int				a_iElement		)											const	;
			float&		operator	[]			(		int				a_iElement		)													;
	const	CVector3&	operator	()			(		int				a_iRow			)											const	;
			CVector3&	operator	()			(		int				a_iRow			)													;
			float		operator	()			(		int				a_iRow																
												,		int				a_iColumn		)											const	;
			float&		operator	()			(		int				a_iRow														
												,		int				a_iColumn		)													;											

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#define	FOR_EACH_ROW_CMatrix3( iRow )					for( int iRow = ERow_Min; iRow < ERow_Count; ++iRow )

#define	FOR_EACH_ROW_COLUMN_CMatrix3( iRow, iColumn )	for( int iRow = ERow_Min; iRow < ERow_Count; ++iRow )		\
														for( int iColumn = EColumn_Min; iColumn	< EColumn_Count; ++iColumn )

#define	FOR_EACH_ELEMENT_CMatrix3( iElement )			for( int iElement = EElement_Min; iElement < EElement_Count; ++iElement )

#define SWAP_CMatrix3( fSwap, iRow, iColumn )			fSwap = m_pfRowColumn[ iRow ][ iColumn ];								\
														m_pfRowColumn[ iRow ][ iColumn ] = m_pfRowColumn[ iColumn ][ iRow ];	\
														m_pfRowColumn[ iColumn ][ iRow ] = fSwap;									 

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#include	"CVector3.h"
#include	"CAxisAngle.h"
#include	"CEulerAngles.h"
#include	"CSphericalRotation.h"
#include	"CQuaternion.h"

//*****************************************************************************
//	AccessElement
//*****************************************************************************
//
//! \brief	Returns the element at the specified row and column for modification purposes
//! \param	[in] The desired element's row index
//! \param	[in] The desired element's column index
//! \return	A reference to the desired element
//
//*****************************************************************************
inline float& CMatrix3::AccessElement( int a_iRow, int a_iColumn ) 
{
	return m_pfRowColumn[ a_iRow ][ a_iColumn ];
}

//*****************************************************************************
//	AccessElement
//*****************************************************************************
//
//! \brief	Returns the element at the specified index for modification purposes
//! \param	[in] The desired element's index
//! \return	The element at the specified index 
//
//*****************************************************************************
inline float& CMatrix3::AccessElement( int a_iElement ) 
{
	return m_pfElements[ a_iElement ];
}

//*****************************************************************************
//	AccessRow
//*****************************************************************************
//
//! \brief	Returns a vector representation of the specified row for modification purposes
//! \param	[in] The desired row's index 
//! \return	The vector representation of the specified row 
//
//*****************************************************************************
inline CVector3& CMatrix3::AccessRow( int a_iRow ) 
{
	return ( (CVector3&) ( m_pfRowColumn[ a_iRow ] [ EColumn_X ] ) );
}

//*****************************************************************************
//	AccessXAxis
//*****************************************************************************
//
//! \brief	Returns a vector representation of the matrix' rotation's x axis
//! \return	The vector representation of the matrix' rotation's x axis
//
//*****************************************************************************
inline CVector3& CMatrix3::AccessXAxis()
{
	return AccessRow( ERow_RotationAxisX );
}

//*****************************************************************************
//	AccessYAxis
//*****************************************************************************
//
//! \brief	Returns a vector representation of the matrix' rotation's y axis
//! \return	The vector representation of the matrix' rotation's y axis
//
//*****************************************************************************
inline CVector3& CMatrix3::AccessYAxis()
{
	return AccessRow( ERow_RotationAxisY );
}

//*****************************************************************************
//	AccessZAxis
//*****************************************************************************
//
//! \brief	Returns a vector representation of the matrix' rotation's z axis
//! \return	The vector representation of the matrix' rotation's z axis
//
//*****************************************************************************
inline CVector3& CMatrix3::AccessZAxis()
{
	return AccessRow( ERow_RotationAxisZ );
}

//*****************************************************************************
//	GetElement
//*****************************************************************************
//
//! \brief	Returns the element at the specified row and column for look-up purposes
//! \param	[in] The desired element's row index
//! \param	[in] The desired element's column index
//! \return	The element at the specified row and column 
//
//*****************************************************************************
inline float CMatrix3::GetElement( int a_iRow, int a_iColumn ) const 
{
	return m_pfRowColumn[ a_iRow ][ a_iColumn ];
}

//*****************************************************************************
//	GetElement
//*****************************************************************************
//
//! \brief	Returns the element at the specified index for look-up purposes
//! \param	[in] The desired element's index
//! \return	The element at the specified row and column 
//
//*****************************************************************************
inline float CMatrix3::GetElement( int a_iElement ) const 
{
	return m_pfElements[ a_iElement ];
}

//*****************************************************************************
//	GetRow
//*****************************************************************************
//
//! \brief	Returns a vector representation of the specified row for look-up purposes
//! \param	[in] The desired row's index
//! \return	The vector representation of the specified row
//
//*****************************************************************************
inline const CVector3& CMatrix3::GetRow( int a_iRow ) const 
{
	return ( (CVector3&) ( m_pfRowColumn[ a_iRow ] [ EColumn_X ] ) );
}

//*****************************************************************************
//	GetXAxis
//*****************************************************************************
//
//! \brief	Returns a vector representation of the matrix' rotation's x axis
//! \return	The vector representation of the matrix' rotation's x axis
//
//*****************************************************************************
inline const CVector3& CMatrix3::GetXAxis() const 
{
	return GetRow( ERow_RotationAxisX );
}

//*****************************************************************************
//	GetYAxis
//*****************************************************************************
//
//! \brief	Returns a vector representation of the matrix' rotation's y axis
//! \return	The vector representation of the matrix' rotation's y axis
//
//*****************************************************************************
inline const CVector3& CMatrix3::GetYAxis() const 
{
	return GetRow( ERow_RotationAxisY );
}

//*****************************************************************************
//	GetZAxis
//*****************************************************************************
//
//! \brief	Returns a vector representation of the matrix' rotation's z axis
//! \return	The vector representation of the matrix' rotation's z axis
//
//*****************************************************************************
inline const CVector3& CMatrix3::GetZAxis() const 
{
	return GetRow( ERow_RotationAxisZ );
}

//*****************************************************************************
//	GetElement
//*****************************************************************************
//
//! \brief	Returns the element at the specified row and column 
//! \param	[in] The desired element's row index
//! \param	[in] The desired element's column index
//! \param	[out] The element at the specified row and column 
//
//*****************************************************************************
inline void CMatrix3::GetElement( int a_iRow, int a_iColumn, float& a_fElement ) const 
{
	a_fElement = GetElement( a_iRow, a_iColumn );
}

//*****************************************************************************
//	GetElement
//*****************************************************************************
//
//! \brief	Returns the element at the specified index
//! \param	[in] The desired element's index
//! \param	[out] The element at the specified index
//
//*****************************************************************************
inline void CMatrix3::GetElement( int a_iElement, float& a_fElement ) const 
{
	a_fElement = GetElement( a_iElement );
}

//*****************************************************************************
//	GetRow
//*****************************************************************************
//
//! \brief	Returns a vector representation of the specified row for look-up purposes
//! \param	[in] The desired row's index
//! \param	[out] The vector representation of the specified row 
//
//*****************************************************************************
inline void CMatrix3::GetRow( int a_iRow, CVector3& a_vecRow ) const 
{
	a_vecRow = GetRow( a_iRow );
}

//*****************************************************************************
//	GetRow
//*****************************************************************************
//
//! \brief	Returns the specified row's columns for look-up purposes
//! \param	[in] The desired row's index
//! \param	[out] The specified row's columns 
//
//*****************************************************************************
inline void CMatrix3::GetRow( int a_iRow, float a_pfRow [ CMatrix3::EColumn_Count ] ) const 
{
	memcpy(	a_pfRow							,	
			m_pfRowColumn[ a_iRow ]			,
			EColumn_Count * sizeof(float)	);
}

//*****************************************************************************
//	GetXAxis
//*****************************************************************************
//
//! \brief	Returns the vector representation of the matrix' rotation's x axis
//! \param	[out] The vector representation of the matrix' rotation's x axis
//
//*****************************************************************************
inline void CMatrix3::GetXAxis( CVector3& a_vecAxisX ) const 
{
	GetRow( ERow_RotationAxisX, a_vecAxisX );
}

//*****************************************************************************
//	GetYAxis
//*****************************************************************************
//
//! \brief	Returns the vector representation of the matrix' rotation's y axis
//! \param	[out] The vector representation of the matrix' rotation's y axis
//
//*****************************************************************************
inline void CMatrix3::GetYAxis( CVector3& a_vecAxisY ) const 
{
	GetRow( ERow_RotationAxisY, a_vecAxisY );
}

//*****************************************************************************
//	GetZAxis
//*****************************************************************************
//
//! \brief	Returns the vector representation of the matrix' rotation's z axis
//! \param	[out] The vector representation of the matrix' rotation's z axis
//
//*****************************************************************************
inline void CMatrix3::GetZAxis( CVector3& a_vecAxisZ ) const 
{
	GetRow( ERow_RotationAxisZ, a_vecAxisZ );
}

//*****************************************************************************
//	SetElement
//*****************************************************************************
//
//! \brief	Sets the element at the specified row and column
//! \param	[in] The row index of the element to set
//! \param	[in] The column index of the element to set
//! \param	[in] The element's new value
//
//*****************************************************************************
inline void CMatrix3::SetElement( int a_iRow, int a_iColumn, float a_fElement ) 
{
	AccessElement( a_iRow, a_iColumn ) = a_fElement;
}

//*****************************************************************************
//	SetElement
//*****************************************************************************
//
//! \brief	Sets the element at the specified index
//! \param	[in] The index of the element to set
//! \param	[in] The element's new value
//
//*****************************************************************************
inline void CMatrix3::SetElement( int a_iElement, float a_fElement ) 
{
	AccessElement( a_iElement ) = a_fElement;
}

//*****************************************************************************
//	SetRow
//*****************************************************************************
//
//! \brief	Sets the specified row vector
//! \param	[in] The index of the row to set
//! \param	[in] The vector representation of the row to set
//
//*****************************************************************************
inline void CMatrix3::SetRow( int a_iRow, const CVector3& a_vecRow ) 
{
	AccessRow( a_iRow ) = a_vecRow;
}

//*****************************************************************************
//	SetRow
//*****************************************************************************
//
//! \brief	Sets the specified row's columns 
//! \param	[in] The index of the row to set
//! \param	[in] The values to set for the row
//
//*****************************************************************************
inline void CMatrix3::SetRow( int a_iRow, const float a_pfRow [ CMatrix3::EColumn_Count ] ) 
{
	memcpy(	m_pfRowColumn[ a_iRow ]			, 
			a_pfRow							, 
			EColumn_Count * sizeof(float)	);
}

//*****************************************************************************
//	ModifyXAxis
//*****************************************************************************
//
//! \brief	Sets the matrix' rotation's x axis
//! \param	[in] The matrix' rotation's x axis
//
//*****************************************************************************
inline void CMatrix3::ModifyXAxis( const CVector3& a_vecAxisX )
{
	SetRow( ERow_RotationAxisX, a_vecAxisX );
}

//*****************************************************************************
//	ModifyYAxis
//*****************************************************************************
//
//! \brief	Sets the matrix' rotation's y axis
//! \param	[in] The matrix' rotation's y axis
//
//*****************************************************************************
inline void CMatrix3::ModifyYAxis( const CVector3& a_vecAxisY )
{
	SetRow( ERow_RotationAxisY, a_vecAxisY );
}

//*****************************************************************************
//	ModifyZAxis
//*****************************************************************************
//
//! \brief	Sets the matrix' rotation's z axis
//! \param	[in] The matrix' rotation's z axis
//
//*****************************************************************************
inline void CMatrix3::ModifyZAxis( const CVector3& a_vecAxisZ )
{
	SetRow( ERow_RotationAxisZ, a_vecAxisZ );
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Copies the specified matrix
//! \param	[in] The matrix to copy
//
//*****************************************************************************
inline void CMatrix3::SetMatrix( const CMatrix3& a_matSource ) 
{
	memcpy(	m_pfElements					,
			a_matSource.m_pfElements		,
			EElement_Count * sizeof(float)	);
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Copies the specified array
//! \param	[in] The array to copy
//
//*****************************************************************************
inline void CMatrix3::SetMatrix( const float a_pfRowColumn [ CMatrix3::ERow_Count ][ CMatrix3::EColumn_Count ] ) 
{
	memcpy(	m_pfRowColumn								,
			a_pfRowColumn								,
			ERow_Count * EColumn_Count * sizeof(float)	);
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Copies the specified array
//! \param	[in] The array to copy
//
//*****************************************************************************
inline void CMatrix3::SetMatrix( const float a_pfElements [ CMatrix3::EElement_Count ] ) 
{
	memcpy(	m_pfElements					,
			a_pfElements					,
			EElement_Count * sizeof(float)	);
}

//*****************************************************************************
//	Clear
//*****************************************************************************
//
//! \brief	Clears the matrix (sets all elements to 0.0f)
//
//*****************************************************************************
inline void CMatrix3::Clear( void ) 
{
	SetMatrix( 0.0f );
}

//*****************************************************************************
//	Invert
//*****************************************************************************
//
//! \brief	Inverts the matrix
//
//*****************************************************************************
inline void CMatrix3::Invert( void ) 
{
	SetToInverse( *this );
}

//*****************************************************************************
//	Divide
//*****************************************************************************
//
//! \brief	Divides each element by the specified factor
//! \param	[in] The factor to divide by
//
//*****************************************************************************
inline void CMatrix3::Divide( float a_fDivisor ) 
{
	Multiply( 1.0f / a_fDivisor );
}

//*****************************************************************************
//	ReverseMultiply
//*****************************************************************************
//
//! \brief	Multiplies the specified matrix by this matrix and sets this matrix to the product
//! \param	[in] The matrix to multiply by this one
//
//*****************************************************************************
inline void CMatrix3::ReverseMultiply( const CMatrix3& a_matPrefix )
{
	SetToProduct( a_matPrefix, *this );
}

//*****************************************************************************
//	Multiply
//*****************************************************************************
//
//! \brief	Multiplies the matrix by the specified matrix
//! \param	[in] The matrix to multiply this one by
//
//*****************************************************************************
inline void CMatrix3::Multiply( const CMatrix3& a_matFactor ) 
{
	SetToProduct( *this, a_matFactor );
}

//*****************************************************************************
//	GetDeterminant
//*****************************************************************************
//
//! \brief	Determines the matrix' determinant
//! \return	The matrix' determinant
//
//*****************************************************************************
inline float CMatrix3::GetDeterminant( void ) const 
{
    return	 (	GetElement(0,0) * ( GetElement(1,1) * GetElement(2,2) - GetElement(1,2) * GetElement(2,1) ) 
			 -	GetElement(0,1) * ( GetElement(1,0) * GetElement(2,2) - GetElement(1,2) * GetElement(2,0) ) 
			 +	GetElement(0,2) * ( GetElement(1,0) * GetElement(2,1) - GetElement(1,1) * GetElement(2,0) ) );
}

//*****************************************************************************
//	GetIdentity
//*****************************************************************************
//
//! \brief	Returns an identity matrix
//! \param	[out] An identity matrix
//
//*****************************************************************************
inline void CMatrix3::GetIdentity( CMatrix3& a_matIdentity )
{
	a_matIdentity.SetIdentity();
}

//*****************************************************************************
//	GetInverted
//*****************************************************************************
//
//! \brief	Returns an inverted copy of the matrix
//! \param	[out] The inverted copy of the matrix
//
//*****************************************************************************
inline void CMatrix3::GetInverted( CMatrix3& a_matInverted ) const
{
	a_matInverted.SetToInverse( *this );
}

//*****************************************************************************
//	GetProductPrefix
//*****************************************************************************
//
//! \brief	Returns the product of the specified matrix and this one
//! \param	[out] The product
//! \param	[in] The matrix to multiply by this one
//
//*****************************************************************************
inline void CMatrix3::GetProductPrefix( CMatrix3& a_matProduct, const CMatrix3& a_matPrefix ) const 
{
	a_matProduct.SetToProduct( a_matPrefix, (*this) );
}

//*****************************************************************************
//	GetProduct
//*****************************************************************************
//
//! \brief	Returns the product of this matrix and the specified one
//! \param	[out] The product
//! \param	[in] The matrix to multiply
//
//*****************************************************************************
inline void CMatrix3::GetProduct( CMatrix3& a_matProduct, const CMatrix3& a_matFactor ) const 
{
	a_matProduct.SetToProduct( (*this), a_matFactor );
}

//*****************************************************************************
//	operator ==
//*****************************************************************************
//
//! \brief	Determines whether or not the matrix equals the specified matrix
//! \param	[in] The matrix to compare to
//! \return	The method returns either of the following values
//! \retval	true	| The matrices are equal
//! \retval	false	| The matrices differ
//
//*****************************************************************************
inline bool CMatrix3::operator == ( const CMatrix3& a_matOther ) const 
{
	return IsEqual( a_matOther );
}

//*****************************************************************************
//	operator != 
//*****************************************************************************
//
//! \brief	Determines whether or not the matrix differs the specified matrix
//! \param	[in] The matrix to compare to
//! \return	The method returns either of the following values
//! \retval	true	| The matrices differ
//! \retval	false	| The matrices are equal
//
//*****************************************************************************
inline bool CMatrix3::operator != ( const CMatrix3& a_matOther ) const 
{
	return ! ( IsEqual( a_matOther ) );
}

//*****************************************************************************
//	operator ~
//*****************************************************************************
//
//! \brief	Inverts the matrix
//! \return	The inverted matrix	
//
//*****************************************************************************
inline CMatrix3 CMatrix3::operator ~ ( void ) const 
{
	return GetInverted();
}

//*****************************************************************************
//	operator -
//*****************************************************************************
//
//! \brief	Negates the matrix
//! \return	The negates matrix	
//
//*****************************************************************************
inline CMatrix3 CMatrix3::operator - ( void ) const 
{
	return GetNegated();
}

//*****************************************************************************
//	operator +
//*****************************************************************************
//
//! \brief	Adds another matrix to this one
//! \param	[in] The matrix to add
//! \param	The sum matrix
//
//*****************************************************************************
inline CMatrix3 CMatrix3::operator + ( const CMatrix3& a_matOffset ) const 
{
	return GetSum( a_matOffset );
}

//*****************************************************************************
//	operator -
//*****************************************************************************
//
//! \brief	Subtracts another matrix from this one
//! \param	[in] The matrix to subtract
//! \param	The difference matrix
//
//*****************************************************************************
inline CMatrix3 CMatrix3::operator - ( const CMatrix3& a_matOffset ) const 
{
	return GetDifference( a_matOffset );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief	Multiplies the matrix by the specified scalar factor
//! \param	[in] The factor to multiply by
//! \return	The product matrix
//
//*****************************************************************************
inline CMatrix3 CMatrix3::operator * ( float a_fFactor ) const 
{
	return GetProduct( a_fFactor );
}

//*****************************************************************************
//	operator /
//*****************************************************************************
//
//! \brief	Multiplies the matrix by the reciprocal of the specified scalar factor
//! \param	[in] The factor
//! \return	The quotient matrix
//
//*****************************************************************************
inline CMatrix3 CMatrix3::operator / ( float a_fFactor ) const 
{
	return GetQuotient( a_fFactor );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief	Multiplies the matrix by the specified matrix
//! \param	[in] The matrix to multiply by
//! \return	The product matrix
//
//*****************************************************************************
inline CMatrix3 CMatrix3::operator * ( const CMatrix3& a_matFactor ) const 
{
	return GetProduct( a_matFactor );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified axis and angle
//! \param	[in] The axis and angle to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
inline CMatrix3 CMatrix3::operator * ( const CAxisAngle& a_Rotation ) const 
{
	return GetRotated( a_Rotation );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified euler angles
//! \param	[in] The euler angles to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
inline CMatrix3 CMatrix3::operator * ( const CEulerAngles& a_Rotation ) const 
{
	return GetRotated( a_Rotation );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief	Rotates the matrix by the specified quaternion
//! \param	[in] The quaternion to rotate by
//! \return	The rotated matrix
//
//*****************************************************************************
inline CMatrix3 CMatrix3::operator * ( const CQuaternion& a_quatRotation ) const 
{
	return GetRotated( a_quatRotation );
}

//*****************************************************************************
//	operator *
//*****************************************************************************
//
//! \brief	Multiplies the specified scalar with the specified matrix
//! \param	[in] The scalar factor
//! \param	[in] The matrix
//! \return	The product matrix
//
//*****************************************************************************
inline CMatrix3 operator * ( float a_fFactor, const CMatrix3& a_matFactor ) 
{
	return ( a_matFactor * a_fFactor );
}

//*****************************************************************************
//	operator []
//*****************************************************************************
//
//! \brief	Returns the element at the specified index
//! \param	[in] The element's index
//! \return	The element at the specified index
//
//****************************************************************************
inline float CMatrix3::operator [] ( int a_iElement ) const 
{
	return GetElement( a_iElement );
}

//*****************************************************************************
//	operator []
//*****************************************************************************
//
//! \brief	Returns the element at the specified index
//! \param	[in] The element's index
//! \return	The element at the specified index
//
//*****************************************************************************
inline float& CMatrix3::operator [] ( int a_iElement ) 
{
	return AccessElement( a_iElement );
}

//*****************************************************************************
//	operator ()
//*****************************************************************************
//
//! \brief	Returns a vector representation of the specified row
//! \param	[in] The desired row's index
//! \return	A reference to the vector representation of the specified row
//
//*****************************************************************************
inline const CVector3& CMatrix3::operator () ( int a_iRow ) const
{
	return GetRow( a_iRow );
}

//*****************************************************************************
//	operator ()
//*****************************************************************************
//
//! \brief	Returns a vector representation of the specified row
//! \param	[in] The desired row's index
//! \return	A reference to the vector representation of the specified row
//
//*****************************************************************************
inline CVector3& CMatrix3::operator () ( int a_iRow ) 
{
	return AccessRow( a_iRow );
}

//*****************************************************************************
//	operator ()
//*****************************************************************************
//
//! \brief	Returns the element at the specified row and column
//! \param	[in] The desired element's row index
//! \param	[in] The desired element's column index
//! \return	A reference to the desired element
//
//*****************************************************************************
inline float CMatrix3::operator () ( int a_iRow, int a_iColumn ) const 
{
	return GetElement( a_iRow, a_iColumn );
}

//*****************************************************************************
//	operator ()
//*****************************************************************************
//
//! \brief	Returns the element at the specified row and column
//! \param	[in] The desired element's row index
//! \param	[in] The desired element's column index
//! \return	A reference to the desired element
//
//*****************************************************************************
inline float& CMatrix3::operator () ( int a_iRow, int a_iColumn ) 
{
	return AccessElement( a_iRow, a_iColumn );
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif	// CMATRIX3_H_INCLUDED
