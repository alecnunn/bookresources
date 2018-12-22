#ifndef			CMATRIX4_H_INCLUDED
#define			CMATRIX4_H_INCLUDED

#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once
#endif 
#endif 

#include		"Basics.h"
#include		"CVector3.h"
#include		"CVector4.h"
#include		"CAxisAngle.h"
#include		"CEulerAngles.h"
#include		"CSphericalRotation.h"
#include		"CQuaternion.h"
#include		"CMatrix3.h"

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CMatrix4
//*****************************************************************************
//
//!	Representation of a 4x4 matrix
//
//*****************************************************************************
class CMatrix4
{

public:

	// Enums

	enum	ERow											//! Matrix' row indices
	{
		ERow_RotationAxisX								,	//!< Row containing x axis rotation scalars
		ERow_RotationAxisY								,	//!< Row containing y axis rotation scalars
		ERow_RotationAxisZ								,	//!< Row containing z axis rotation scalars
		ERow_Translation								,	//!< Row containing translation
		ERow_Count										,	//!< The number of rows
		ERow_Min				= ERow_RotationAxisX	,	//!< The first row
		ERow_Max				= ERow_Translation		,	//!< The last row
		ERow_ForceDWORD			= FORCE_DWORD				//!< Forces all compilers to reserve 32 bits for the enumeration
	};																	

	enum	EColumn											//! Matrix' column indices
	{
		EColumn_X									,		//!< Column containing x coordinate scalars / offsets
		EColumn_Y									,		//!< Column containing y coordinate scalars / offsets
		EColumn_Z									,		//!< Column containing z coordinate scalars / offsets
		EColumn_Reserved							,		//!< Column containing reserved values
		EColumn_Count								,		//!< The number of columns
		EColumn_Min				= EColumn_X			,		//!< The first column
		EColumn_Max				= EColumn_Reserved	,		//!< The last column
		EColumn_ForceDWORD		= FORCE_DWORD				//!< Forces all compilers to reserve 32 bits for the enumeration
	};																	

	enum	EElement										//! Matrix' element indices
	{
		EElement_11								,			//!< x axis rotation scalar for x coordinate 
		EElement_12								,			//!< x axis rotation scalar for y coordinate 
		EElement_13								,			//!< x axis rotation scalar for z coordinate 
		EElement_14								,			//!< reserved (must be set to 0.0f for multiplication to work properly) 
		EElement_21								,			//!< y axis rotation scalar for x coordinate 
		EElement_22								,			//!< y axis rotation scalar for y coordinate 
		EElement_23								,			//!< y axis rotation scalar for z coordinate
		EElement_24								,			//!< reserved (must be set to 0.0f for multiplication to work properly) 
		EElement_31								,			//!< z axis rotation scalar for x coordinate  
		EElement_32								,			//!< z axis rotation scalar for y coordinate  
		EElement_33								,			//!< z axis rotation scalar for z coordinate 
		EElement_34								,			//!< reserved (must be set to 0.0f for multiplication to work properly)
		EElement_41								,			//!< x axis translation offset (position)
		EElement_42								,			//!< y axis translation offset (position)
		EElement_43								,			//!< z axis translation offset (position)
		EElement_44								,			//!< reserved (must be set to 1.0f for multiplication to work properly)
		EElement_Count							,			//!< The number of elements
		EElement_Min			= EElement_11	,			//!< The first element
		EElement_Max			= EElement_44	,			//!< The last element
		EElement_ForceDWORD		= FORCE_DWORD				//!< Forces all compilers to reserve 32 bits for the enumeration
	};																																								

protected:

	// Attribs

	union
	{
		float	m_pfElements [ EElement_Count ];				//!< Matrix' elements 
		float	m_pfRowColumn[ ERow_Count ][ EColumn_Count ];	//!< Matrix' elements split into row and column
	};																																

	// Get/Set

			CVector3&	AccessRow				(		int				a_iRow		)														;
			CVector4&	AccessRow4				(		int				a_iRow		)														;
	const	CVector3&	GetRow					(		int				a_iRow		)												const	;
	const	CVector4&	GetRow4					(		int				a_iRow		)												const	;
			CVector3	GetColumn				(		int				a_iColumn	)												const	;
			CVector4	GetColumn4				(		int				a_iColumn	)												const	;

			void		GetRow					(		int				a_iRow 														
												,		CVector3&		a_vecRow										)			const	;
			void		GetRow					(		int				a_iRow 														
												,		CVector4&		a_vecRow										)			const	;
			void		GetColumn				(		int				a_iColumn 													
												,		CVector3&		a_vecColumn										)			const	;
			void		GetColumn				(		int				a_iColumn 													
												,		CVector4&		a_vecColumn										)			const	;

	// Modification

			void		ValidateReserved		( void )																					;
			void		InvertExcentric			( void )																					;
			CMatrix4	GetInvertedExcentric	( void )																			const	;
			void		GetInvertedExcentric	(		CMatrix4&		a_matInverted	)											const	;

public:																																		

	// Con-/Destruction

			 CMatrix4(	const	CAxisAngle&		a_Rotation		, const CVector3&	a_vecTranslation	)									;
			 CMatrix4(	const	CEulerAngles&	a_Rotation		, const CVector3&	a_vecTranslation	)									;
			 CMatrix4(	const	CQuaternion&	a_quatRotation	, const CVector3&	a_vecTranslation	)									;
	explicit CMatrix4(	const	float			a_pfElements	[ EElement_Count ]						)									;
	explicit CMatrix4(	const	float			a_pfRowColumn	[ ERow_Count ][ EColumn_Count ]			)									;
	explicit CMatrix4(	const	CVector4	    a_pvecRows		[ ERow_Count ]							)									;
	explicit CMatrix4(	const	CVector3	    a_pvecRows		[ ERow_Count ]							)									;
	explicit CMatrix4(			float			a_fElements	)																				;
			 CMatrix4(	const	CMatrix4&		a_matSource )																				;
			 CMatrix4()																														;
			~CMatrix4()																														;

	// Get/Set

			float&	AccessElement				(		int				a_iRow														
												,		int				a_iColumn	)														;
			float&	AccessElement				(		int				a_iElement	)														;
			CVector3&	AccessXAxis				( void )																					;
			CVector4&	AccessXAxis4			( void )																					;
			CVector3&	AccessYAxis				( void )																					;
			CVector4&	AccessYAxis4			( void )																					;
			CVector3&	AccessZAxis				( void )																					;
			CVector4&	AccessZAxis4			( void )																					;
			CVector3&	AccessTranslation		( void )																					;
			CVector4&	AccessTranslation4		( void )																					;
																																	
			float		GetElement				(		int				a_iRow														
												,		int				a_iColumn	)												const	;
			float		GetElement				(		int				a_iElement	)												const	;
	const	CVector3&	GetXAxis				( void )																			const	;
	const	CVector4&	GetXAxis4				( void )																			const	;
	const	CVector3&	GetYAxis				( void )																			const	;
	const	CVector4&	GetYAxis4				( void )																			const	;
	const	CVector3&	GetZAxis				( void )																			const	;
	const	CVector4&	GetZAxis4				( void )																			const	;
	const	CVector3&	GetTranslation			( void )																			const	;
	const	CVector4&	GetTranslation4			( void )																			const	;
			CMatrix4	GetRotation				( void )																			const	;
			CMatrix3	GetRotation3			( void )																			const	;
																																	
			void		GetElement				(		int				a_iRow														
												,		int				a_iComumn													
												,		float&			a_fElement										)			const	;
			void		GetElement				(		int				a_iElement													
												,		float&			a_fElement										)			const	;
			void		GetRow					(		int				a_iRow 														
												,		float			a_pfRow				[ EColumn_Count ]			)			const	;
			void		GetColumn				(		int				a_iColumn 													
												,		float			a_pfColumn			[ ERow_Count ]				)			const	;								
			void		GetXAxis				(		CVector3&		a_vecAxisX										)			const	;
			void		GetXAxis				(		CVector4&		a_vecAxisX										)			const	;
			void		GetYAxis				(		CVector3&		a_vecAxisY										)			const	;
			void		GetYAxis				(		CVector4&		a_vecAxisY										)			const	;
			void		GetZAxis				(		CVector3&		a_vecAxisZ										)			const	;
			void		GetZAxis				(		CVector4&		a_vecAxisZ										)			const	;
			void		GetTranslation			(		CVector3&		a_vecTranslation								)			const	;
			void		GetTranslation			(		CVector4&		a_vecTranslation								)			const	;
			void		GetRotation				(		CMatrix4&		a_matRotation									)			const	;
			void		GetRotation				(		CMatrix3&		a_matRotation									)			const	;
																																	
			void		SetIdentityRotation		( void )																					;
			void		SetIdentity				( void )																					;
			void		SetElement				(		int				a_iElement													
												,		int				a_iColumn													
												,		float			a_fElement										)					;
			void		SetElement				(		int				a_iElement													
												,		float			a_fElement										)					;
			void		SetRow					(		int				a_iRow 														
												, const	CVector3&		a_vecRow										)					;
			void		SetRow					(		int				a_iRow 														
												, const	CVector4&		a_vecRow										)					;
			void		SetRow					(		int				a_iRow														
												, const	float			a_pfRow				[ EColumn_Count ]			)					;
			void		ModifyXAxis				( const	CVector3&		a_vecAxisX										)					;
			void		ModifyXAxis				( const	CVector4&		a_vecAxisX										)					;
			void		ModifyYAxis				( const	CVector3&		a_vecAxisY										)					;
			void		ModifyYAxis				( const	CVector4&		a_vecAxisY										)					;
			void		ModifyZAxis				( const	CVector3&		a_vecAxisZ										)					;
			void		ModifyZAxis				( const	CVector4&		a_vecAxisZ										)					;
			void		ModifyScaling			(		float			a_fScaling										)					;
			void		ModifyScaling			( const	CVector3&		a_vecScaling									)					;
			void		ModifyTranslation		( const	CVector3&		a_vecTranslation								)					;
			void		ModifyRotation			( const	CQuaternion&	a_quatRotation									)					;
			void		ModifyRotation			( const	CMatrix3&		a_matRotation									)					;
			void		ModifyRotation			( const	CAxisAngle&		a_Rotation										)					;
			void		ModifyRotation			( const	CEulerAngles&	a_Rotation										)					;
			void		ModifyRotationAxisX		(		float			a_fAngle										)					;
			void		ModifyRotationAxisY		(		float			a_fAngle										)					;
			void		ModifyRotationAxisZ		(		float			a_fAngle										)					;
																																	
			void		SetToScaling			(		float			a_fScaling										)					;
			void		SetToScaling			( const	CVector3&		a_vecScaling									)					;
			void		SetToTranslation		( const	CVector3&		a_vecTranslation								)					;
			void		SetToRotation			( const	CQuaternion&	a_quatRotation									)					;
			void		SetToRotation			( const	CAxisAngle&		a_Rotation										)					;
			void		SetToRotation			( const	CEulerAngles&	a_Rotation										)					;
			void		SetToRotationAxisX		(		float			a_fAngle										)					;
			void		SetToRotationAxisY		(		float			a_fAngle										)					;
			void		SetToRotationAxisZ		(		float			a_fAngle										)					;
			void		SetToCrossProduct		( const	CVector3&		a_vecFactor										)					;
			void		SetToProduct			( const	CMatrix4&		a_matLeft															
												, const	CMatrix4&		a_matRight										)					;
																																	
			void		SetMatrix				( const CMatrix4&		a_matSource										)					;
			void		SetMatrix				(		float			a_fElements										)					;
			void		SetMatrix				( const	CVector3	    a_pvecRows	 [ ERow_Count ]						)					;
			void		SetMatrix				( const	CVector4	    a_pvecRows	 [ ERow_Count ]						)					;
			void		SetMatrix				( const	float			a_pfRowColumn[ ERow_Count ][ EColumn_Count ]	)					;
			void		SetMatrix				( const	float			a_pfElements [ EElement_Count ]					)					;

			void		SetMatrix				( const	CQuaternion&	a_quatRotation														
												, const CVector3&		a_vecTranslation								)					;
			void		SetMatrix				( const	CEulerAngles&	a_Rotation															
												, const CVector3&		a_vecTranslation								)					;
			void		SetMatrix				( const	CAxisAngle&		a_Rotation															
												, const CVector3&		a_vecTranslation								)					;
			void		SetMatrix				( const CVector3&		a_vecTranslation
												, const CQuaternion&	a_quatRotation
												, const CVector3&		a_vecScale										)					;
			void		SetMatrix				( const CVector3&		a_vecTranslation
												, const CEulerAngles&	a_eulerRotation
												, const CVector3&		a_vecScale										)					;

	// Modification

			void		Clear					( void )																					;
			void		ClearRotation			( void )																					;
			void		TransposeRotation		( void )																					;
			void		Transpose				( void )																					;
			void		Negate					( void )																					;
			void		Invert					( void )																					;
			void		Orthogonalize			( void )																					;
			void		Orthonormalize			( void )																					;
			void		Add						( const	CMatrix4&		a_mat			)													;
			void		Sub						( const	CMatrix4&		a_mat			)													;
			void		Multiply				(		float			a_fFactor		)													;
			void		Divide					(		float			a_fDivisor		)													;
			void		ReverseMultiply			( const CMatrix4&		a_matPrefix		)													;
			void		Multiply				( const CMatrix4&		a_matFactor		)													;
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
			void		LookAt					( const CVector3&		a_vecLookAt		)													;
			void		LookAt					( const CVector3&		a_vecLookAt
												, const CVector3&		a_vecUp			)													;

	// Computations

			bool		IsExcentric				( void )																			const	;
			bool		IsIdentity				( void )																			const	;
																																	
			bool		IsEqual					( const	CMatrix4&		a_matOther		)											const	;
																																	
			float		GetDeterminant			( void )																			const	;
			float		GetDeterminantRotation	( void )																			const	;
			float		GetDeterminantAdjoint	(		int				a_iRow														
												,		int				a_iColumn		)											const	;														
	static	CMatrix4	GetIdentity				( void )																					;
			CMatrix4	GetNegated				( void )																			const	;
			CMatrix4	GetInverted				( void )																			const	;
			CMatrix4	GetOrthogonalized		( void )																			const	;
			CMatrix4	GetOrthonormalized		( void )																			const	;
																																			
			CMatrix4	GetSum					( const CMatrix4&		a_matOffset		)											const	;
			CMatrix4	GetDifference			( const CMatrix4&		a_matOffset		)											const	;
			CMatrix4	GetProductPrefix		( const CMatrix4&		a_matPrefix		)											const	;
			CMatrix4	GetProduct				( const CMatrix4&		a_matFactor		)											const	;
			CMatrix4	GetProduct				(		float			a_fFactor		)											const	;
			CMatrix4	GetQuotient				(		float			a_fDivisor		)											const	;
			CMatrix4	GetRotated				( const	CQuaternion&	a_quatRotation	)											const	;
			CMatrix4	GetRotated				( const	CAxisAngle&		a_Rotation		)											const	;
			CMatrix4	GetRotated				( const	CEulerAngles&	a_Rotation		)											const	;
			CMatrix4	GetRotatedByAxisX		(		float			a_fAngle		)											const	;
			CMatrix4	GetRotatedByAxisY		(		float			a_fAngle		)											const	;
			CMatrix4	GetRotatedByAxisZ		(		float			a_fAngle		)											const	;
			CMatrix4	GetRotatedLocal			( const	CQuaternion&	a_quatRotation	)											const	;
			CMatrix4	GetRotatedLocal			( const	CAxisAngle&		a_Rotation		)											const	;
			CMatrix4	GetRotatedLocal			( const	CEulerAngles&	a_Rotation		)											const	;
			CMatrix4	GetRotatedByLocalAxisX	(		float			a_fAngle		)											const	;
			CMatrix4	GetRotatedByLocalAxisY	(		float			a_fAngle		)											const	;
			CMatrix4	GetRotatedByLocalAxisZ	(		float			a_fAngle		)											const	;
																																	
	static	void		GetIdentity				(		CMatrix4&		a_matIdentity	)													;
			void		GetNegated				(		CMatrix4&		a_matNegated	)											const	;
			void		GetInverted				(		CMatrix4&		a_matInverted	)											const	;
			void		GetOrthogonalized		(		CMatrix4&		a_matOrthogon	)											const	;
			void		GetOrthonormalized		(		CMatrix4&		a_matOrthonorm	)											const	;
																																			
			void		GetSum					(		CMatrix4&		a_matSum		, const CMatrix4&		a_matOffset		)	const	;
			void		GetDifference			(		CMatrix4&		a_matDifference	, const CMatrix4&		a_matOffset		)	const	;
			void		GetProductPrefix		(		CMatrix4&		a_matProduct	, const CMatrix4&		a_matPrefix		)	const	;
			void		GetProduct				(		CMatrix4&		a_matProduct	, const CMatrix4&		a_matFactor		)	const	;
			void		GetProduct				(		CMatrix4&		a_matProduct	,		float			a_fFactor		)	const	;
			void		GetQuotient				(		CMatrix4&		a_matQuotient	,		float			a_fDivisor		)	const	;
			void		GetRotated				(		CMatrix4&		a_matRotated	, const	CQuaternion&	a_quatRotation	)	const	;
			void		GetRotated				(		CMatrix4&		a_matRotated	, const	CAxisAngle&		a_Rotation		)	const	;
			void		GetRotated				(		CMatrix4&		a_matRotated	, const	CEulerAngles&	a_Rotation		)	const	;
			void		GetRotatedByAxisX		(		CMatrix4&		a_matRotated	,		float			a_fAngle		)	const	;
			void		GetRotatedByAxisY		(		CMatrix4&		a_matRotated	,		float			a_fAngle		)	const	;
			void		GetRotatedByAxisZ		(		CMatrix4&		a_matRotated	,		float			a_fAngle		)	const	;
			void		GetRotatedLocal			(		CMatrix4&		a_matRotated	, const	CQuaternion&	a_quatRotation	)	const	;
			void		GetRotatedLocal			(		CMatrix4&		a_matRotated	, const	CAxisAngle&		a_Rotation		)	const	;
			void		GetRotatedLocal			(		CMatrix4&		a_matRotated	, const	CEulerAngles&	a_Rotation		)	const	;
			void		GetRotatedByLocalAxisX	(		CMatrix4&		a_matRotated	,		float			a_fAngle		)	const	;
			void		GetRotatedByLocalAxisY	(		CMatrix4&		a_matRotated	,		float			a_fAngle		)	const	;
			void		GetRotatedByLocalAxisZ	(		CMatrix4&		a_matRotated	,		float			a_fAngle		)	const	;

	// Operators

			bool		operator	==			( const CMatrix4&		a_matOther		)											const	;
			bool		operator	!=			( const CMatrix4&		a_matOther		)											const	;
			CMatrix4&	operator	=			( const	CMatrix4&		a_matOther		)													;
			CMatrix4&	operator	+=			( const CMatrix4&		a_matOffset		)													;
			CMatrix4&	operator	-=			( const CMatrix4&		a_matOffset		)													;
			CMatrix4&	operator	*=			(		float			a_fFactor		)													;
			CMatrix4&	operator	/=			(		float			a_fFactor		)													;
			CMatrix4&	operator	*=			( const CMatrix4&		a_matFactor		)													;
			CMatrix4&	operator	*=			( const CAxisAngle&		a_Rotation		)													;
			CMatrix4&	operator	*=			( const CEulerAngles&	a_Rotation		)													;
			CMatrix4&	operator	*=			( const CQuaternion&	a_quatRotation	)													;
			CMatrix4	operator	~			( void )																			const	;
			CMatrix4	operator	-			( void )																			const	;
			CMatrix4	operator	+			( const CMatrix4&		a_matOffset		)											const	;
			CMatrix4	operator	-			( const CMatrix4&		a_matOffset		)											const	;
			CMatrix4	operator	*			(		float			a_fFactor		)											const	;
			CMatrix4	operator	/			(		float			a_fFactor		)											const	;
			CMatrix4	operator	*			( const CMatrix4&		a_matFactor		)											const	;
			CVector3	operator	*			( const CVector3&		a_vec			)											const	;
			CMatrix4	operator	*			( const CAxisAngle&		a_Rotation		)											const	;
			CMatrix4	operator	*			( const CEulerAngles&	a_Rotation		)											const	;
			CMatrix4	operator	*			( const CQuaternion&	a_quatRotation	)											const	;
	friend	CMatrix4	operator	*			(		float			a_fFactor		, const CMatrix4&		a_matFactor		)			;
			float		operator	[]			(		int				a_iElement		)											const	;
			float&		operator	[]			(		int				a_iElement		)													;
	const	CVector3&	operator	()			(		int				a_iRow			)											const	;
			CVector3&	operator	()			(		int				a_iRow			)													;
			float		operator	()			(		int				a_iRow			,		int				a_iColumn		)	const	;
			float&		operator	()			(		int				a_iRow			,		int				a_iColumn		)			;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#define	FOR_EACH_ROW_CMatrix4( iRow )					for( int iRow = ERow_Min; iRow < ERow_Count; ++iRow )		

#define	FOR_EACH_ROW_COLUMN_CMatrix4( iRow, iColumn )	for( int iRow = ERow_Min; iRow < ERow_Count; ++iRow )		\
														for( int iColumn  = EColumn_Min ; iColumn	< EColumn_Count	; ++iColumn )			

#define	FOR_EACH_ELEMENT_CMatrix4( iElement )			for( int iElement = EElement_Min; iElement < EElement_Count; ++iElement )

#define SWAP_CMatrix4( fSwap, iRow, iColumn )			fSwap = m_pfRowColumn[ iRow ][ iColumn ];								\
														m_pfRowColumn[ iRow ][ iColumn ] = m_pfRowColumn[ iColumn ][ iRow ];	\
														m_pfRowColumn[ iColumn ][ iRow ] = fSwap;									 

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#include	<memory>

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
inline float& CMatrix4::AccessElement( int a_iRow, int a_iColumn ) 
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
inline float& CMatrix4::AccessElement( int a_iElement ) 
{
	return m_pfElements[ a_iElement ];
}

//*****************************************************************************
//	AccessRow
//*****************************************************************************
//
//! \brief	Returns a 3D vector representation of the specified row for modification purposes
//! \param	[in] The desired row's index 
//! \return	The vector representation of the specified row 
//
//*****************************************************************************
inline CVector3& CMatrix4::AccessRow( int a_iRow ) 
{
	return ( (CVector3&) ( m_pfRowColumn[ a_iRow ] [ EColumn_X ] ) );
}

//*****************************************************************************
//	AccessRow
//*****************************************************************************
//
//! \brief	Returns a 4D vector representation of the specified row for modification purposes
//! \param	[in] The desired row's index 
//! \return	The vector representation of the specified row 
//
//****************************************************************************
inline CVector4& CMatrix4::AccessRow4( int a_iRow ) 
{
	return ( (CVector4&) ( m_pfRowColumn[ a_iRow ] [ EColumn_X ] ) );
}

//*****************************************************************************
//	AccessXAxis
//*****************************************************************************
//
//! \brief	Returns a 3D vector representation of the matrix' rotation's x axis
//! \return	The vector representation of the matrix' rotation's x axis
//
//*****************************************************************************
inline CVector3& CMatrix4::AccessXAxis()
{
	return AccessRow( ERow_RotationAxisX );
}

//*****************************************************************************
//	AccessXAxis
//*****************************************************************************
//
//! \brief	Returns a 4D vector representation of the matrix' rotation's x axis
//! \return	The vector representation of the matrix' rotation's x axis
//
//*****************************************************************************
inline CVector4& CMatrix4::AccessXAxis4()
{
	return AccessRow4( ERow_RotationAxisX );
}

//*****************************************************************************
//	AccessYAxis
//*****************************************************************************
//
//! \brief	Returns a 3D vector representation of the matrix' rotation's y axis
//! \return	The vector representation of the matrix' rotation's y axis
//
//*****************************************************************************
inline CVector3& CMatrix4::AccessYAxis()
{
	return AccessRow( ERow_RotationAxisY );
}

//*****************************************************************************
//	AccessYAxis
//*****************************************************************************
//
//! \brief	Returns a 4D vector representation of the matrix' rotation's y axis
//! \return	The vector representation of the matrix' rotation's y axis
//
//*****************************************************************************
inline CVector4& CMatrix4::AccessYAxis4()
{
	return AccessRow4( ERow_RotationAxisY );
}

//*****************************************************************************
//	AccessZAxis
//*****************************************************************************
//
//! \brief	Returns a 3D vector representation of the matrix' rotation's z axis
//! \return	The vector representation of the matrix' rotation's z axis
//
//*****************************************************************************
inline CVector3& CMatrix4::AccessZAxis()
{
	return AccessRow( ERow_RotationAxisZ );
}

//*****************************************************************************
//	AccessZAxis
//*****************************************************************************
//
//! \brief	Returns a 4D vector representation of the matrix' rotation's z axis
//! \return	The vector representation of the matrix' rotation's z axis
//
//*****************************************************************************
inline CVector4& CMatrix4::AccessZAxis4()
{
	return AccessRow4( ERow_RotationAxisZ );
}

//*****************************************************************************
//	AccessTranslation
//*****************************************************************************
//
//! \brief	Returns a 3D vector representation of the translation offsets for modification purposes
//! \return	The vector representation of the translation offsets 
//
//*****************************************************************************
inline CVector3& CMatrix4::AccessTranslation( void ) 
{
	return AccessRow( ERow_Translation );
} 

//*****************************************************************************
//	AccessTranslation
//*****************************************************************************
//
//! \brief	Returns a 4D vector representation of the translation offsets for modification purposes
//! \return	The vector representation of the translation offsets 
//
//*****************************************************************************
inline CVector4& CMatrix4::AccessTranslation4( void ) 
{
	return AccessRow4( ERow_Translation );
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
inline float CMatrix4::GetElement( int a_iRow, int a_iColumn ) const 
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
inline float CMatrix4::GetElement( int a_iElement ) const 
{
	return m_pfElements[ a_iElement ];
}

//*****************************************************************************
//	GetRow
//*****************************************************************************
//
//! \brief	Returns a 3D vector representation of the specified row for look-up purposes
//! \param	[in] The desired row's index
//! \return	The vector representation of the specified row
//
//*****************************************************************************
inline const CVector3& CMatrix4::GetRow( int a_iRow ) const 
{
	return ( (CVector3&) ( m_pfRowColumn[ a_iRow ] [ EColumn_X ] ) );
}

//*****************************************************************************
//	GetRow
//*****************************************************************************
//
//! \brief	Returns a 4D vector representation of the specified row for look-up purposes
//! \param	[in] The desired row's index
//! \return	The vector representation of the specified row
//
//*****************************************************************************
inline const CVector4& CMatrix4::GetRow4( int a_iRow ) const 
{
	return ( (CVector4&) ( m_pfRowColumn[ a_iRow ] [ EColumn_X ] ) );
}

//*****************************************************************************
//	GetXAxis
//*****************************************************************************
//
//! \brief	Returns a 3D vector representation of the matrix' rotation's x axis
//! \return	The vector representation of the matrix' rotation's x axis
//
//*****************************************************************************
inline const CVector3& CMatrix4::GetXAxis() const 
{
	return GetRow( ERow_RotationAxisX );
}

//*****************************************************************************
//	GetXAxis
//*****************************************************************************
//
//! \brief	Returns a 4D vector representation of the matrix' rotation's x axis
//! \return	The vector representation of the matrix' rotation's x axis
//
//*****************************************************************************
inline const CVector4& CMatrix4::GetXAxis4() const 
{
	return GetRow4( ERow_RotationAxisX );
}

//*****************************************************************************
//	GetYAxis
//*****************************************************************************
//
//! \brief	Returns a 3D vector representation of the matrix' rotation's y axis
//! \return	The vector representation of the matrix' rotation's y axis
//
//*****************************************************************************
inline const CVector3& CMatrix4::GetYAxis() const 
{
	return GetRow( ERow_RotationAxisY );
}

//*****************************************************************************
//	GetYAxis
//*****************************************************************************
//
//! \brief	Returns a 4D vector representation of the matrix' rotation's y axis
//! \return	The vector representation of the matrix' rotation's y axis
//
//*****************************************************************************
inline const CVector4& CMatrix4::GetYAxis4() const 
{
	return GetRow4( ERow_RotationAxisY );
}

//*****************************************************************************
//	GetZAxis
//*****************************************************************************
//
//! \brief	Returns a 3D vector representation of the matrix' rotation's z axis
//! \return	The vector representation of the matrix' rotation's z axis
//
//*****************************************************************************
inline const CVector3& CMatrix4::GetZAxis() const 
{
	return GetRow( ERow_RotationAxisZ );
}

//*****************************************************************************
//	GetZAxis
//*****************************************************************************
//
//! \brief	Returns a 4D vector representation of the matrix' rotation's z axis
//! \return	The vector representation of the matrix' rotation's z axis
//
//*****************************************************************************
inline const CVector4& CMatrix4::GetZAxis4() const 
{
	return GetRow4( ERow_RotationAxisZ );
}

//*****************************************************************************
//	GetTranslation
//*****************************************************************************
//
//! \brief	Returns a 3D vector representation of the translation offsets for look-up purposes
//! \return	The vector representation of the translation offsets
//
//*****************************************************************************
inline const CVector3& CMatrix4::GetTranslation( void ) const 
{
	return GetRow( ERow_Translation );
} 

//*****************************************************************************
//	GetTranslation
//*****************************************************************************
//
//! \brief	Returns a 4D vector representation of the translation offsets for look-up purposes
//! \return	The vector representation of the translation offsets
//
//*****************************************************************************
inline const CVector4& CMatrix4::GetTranslation4( void ) const 
{
	return GetRow4( ERow_Translation );
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
inline void CMatrix4::GetElement( int a_iRow, int a_iColumn, float& a_fElement ) const 
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
inline void CMatrix4::GetElement( int a_iElement, float& a_fElement ) const 
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
inline void CMatrix4::GetRow( int a_iRow, CVector3& a_vecRow ) const 
{
	a_vecRow = GetRow( a_iRow );
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
inline void CMatrix4::GetRow( int a_iRow, CVector4& a_vecRow ) const 
{
	a_vecRow = GetRow4( a_iRow );
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
inline void CMatrix4::GetRow( int a_iRow, float a_pfRow [ CMatrix4::EColumn_Count ] ) const 
{
	memcpy(	a_pfRow							,	
			m_pfRowColumn[ a_iRow ]			,
			EColumn_Count * sizeof(float)	);
}

//*****************************************************************************
//	GetTranslation
//*****************************************************************************
//
//! \brief	Returns a vector representation of the translation for look-up purposes	
//! \return	[out] The vector representation of the translation 
//
//*****************************************************************************
inline void CMatrix4::GetTranslation( CVector3& a_vecTranslation ) const 
{
	a_vecTranslation = GetTranslation();
}

//*****************************************************************************
//	GetTranslation
//*****************************************************************************
//
//! \brief	Returns a vector representation of the translation for look-up purposes	
//! \return	[out] The vector representation of the translation 
//
//*****************************************************************************
inline void CMatrix4::GetTranslation( CVector4& a_vecTranslation ) const 
{
	a_vecTranslation = GetTranslation4();
}

//*****************************************************************************
//	GetXAxis
//*****************************************************************************
//
//! \brief	Returns the 3D vector representation of the matrix' rotation's x axis
//! \param	[out] The vector representation of the matrix' rotation's x axis
//
//*****************************************************************************
inline void CMatrix4::GetXAxis( CVector3& a_vecAxisX ) const 
{
	GetRow( ERow_RotationAxisX, a_vecAxisX );
}

//*****************************************************************************
//	GetXAxis
//*****************************************************************************
//
//! \brief	Returns the 4D vector representation of the matrix' rotation's x axis
//! \param	[out] The vector representation of the matrix' rotation's x axis
//
//*****************************************************************************
inline void CMatrix4::GetXAxis( CVector4& a_vecAxisX ) const 
{
	GetRow( ERow_RotationAxisX, a_vecAxisX );
}

//*****************************************************************************
//	 GetYAxis
//*****************************************************************************
//
//! \brief	Returns the 3D vector representation of the matrix' rotation's y axis
//! \param	[out] The vector representation of the matrix' rotation's y axis
//
//*****************************************************************************
inline void CMatrix4::GetYAxis( CVector3& a_vecAxisY ) const 
{
	GetRow( ERow_RotationAxisY, a_vecAxisY );
}

//*****************************************************************************
//	GetYAxis
//*****************************************************************************
//
//! \brief	Returns the 4D vector representation of the matrix' rotation's y axis
//! \param	[out] The vector representation of the matrix' rotation's y axis
//
//*****************************************************************************
inline void CMatrix4::GetYAxis( CVector4& a_vecAxisY ) const 
{
	GetRow( ERow_RotationAxisY, a_vecAxisY );
}

//*****************************************************************************
//	GetZAxis
//*****************************************************************************
//
//! \brief	Returns the 3D vector representation of the matrix' rotation's z axis
//! \param	[out] The vector representation of the matrix' rotation's z axis
//
//*****************************************************************************
inline void CMatrix4::GetZAxis( CVector3& a_vecAxisZ ) const 
{
	GetRow( ERow_RotationAxisZ, a_vecAxisZ );
}

//*****************************************************************************
//	GetZAxis
//*****************************************************************************
//
//! \brief	Returns the 4D vector representation of the matrix' rotation's z axis
//! \param	[out] The vector representation of the matrix' rotation's z axis
//
//*****************************************************************************
inline void CMatrix4::GetZAxis( CVector4& a_vecAxisZ ) const 
{
	GetRow( ERow_RotationAxisZ, a_vecAxisZ );
}

//*****************************************************************************
//	SetElement
//*****************************************************************************
//
//! \brief	Modifies the element at the specified row and column
//! \param	[in] The row index of the element to set
//! \param	[in] The column index of the element to set
//! \return	[in] The element's new value
//
//*****************************************************************************
inline void CMatrix4::SetElement( int a_iRow, int a_iColumn, float a_fElement ) 
{
	AccessElement( a_iRow, a_iColumn ) = a_fElement;
}

//*****************************************************************************
//	SetElement
//*****************************************************************************
//
//! \brief	Modifies the element at the specified index
//! \param	[in] The index of the element to set
//! \return	[in] The element's new value
//
//*****************************************************************************
inline void CMatrix4::SetElement( int a_iElement, float a_fElement ) 
{
	AccessElement( a_iElement ) = a_fElement;
}

//*****************************************************************************
//	SetRow
//*****************************************************************************
//
//! \brief	Modifies the specified row vector
//! \param	[in] The index of the row to set
//! \param	[in] The vector representation of the row to set
//
//*****************************************************************************
inline void CMatrix4::SetRow( int a_iRow, const CVector3& a_vecRow ) 
{
	AccessRow( a_iRow ) = a_vecRow;
}

//*****************************************************************************
//	SetRow
//*****************************************************************************
//
//! \brief	Modifies the specified row vector
//! \param	[in] The index of the row to set
//! \param	[in] The vector representation of the row to set
//
//*****************************************************************************
inline void CMatrix4::SetRow( int a_iRow, const CVector4& a_vecRow ) 
{
	AccessRow4( a_iRow ) = a_vecRow;
}

//*****************************************************************************
//	SetRow
//*****************************************************************************
//
//! \brief	Modifies the specified row's columns 
//! \param	[in] The index of the row to set
//! \param	[in] The values to set for the row
//
//*****************************************************************************
inline void CMatrix4::SetRow( int a_iRow, const float a_pfRow [ CMatrix4::EColumn_Count ] ) 
{
	memcpy(	m_pfRowColumn[ a_iRow ]			, 
			a_pfRow							, 
			EColumn_Count * sizeof(float)	);
}

//*****************************************************************************
//	ModifyXAxis
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation's x axis
//! \param	[in] The matrix' rotation's x axis
//
//*****************************************************************************
inline void CMatrix4::ModifyXAxis( const CVector3& a_vecAxisX )
{
	SetRow( ERow_RotationAxisX, a_vecAxisX );
}

//*****************************************************************************
//	ModifyXAxis
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation's x axis
//! \param	[in] The matrix' rotation's x axis
//
//*****************************************************************************
inline void CMatrix4::ModifyXAxis( const CVector4& a_vecAxisX )
{
	SetRow( ERow_RotationAxisX, a_vecAxisX );
}

//*****************************************************************************
//	ModifyYAxis
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation's y axis
//! \param	[in] The matrix' rotation's y axis
//
//*****************************************************************************
inline void CMatrix4::ModifyYAxis( const CVector3& a_vecAxisY )
{
	SetRow( ERow_RotationAxisY, a_vecAxisY );
}

//*****************************************************************************
//	ModifyYAxis
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation's y axis
//! \param	[in] The matrix' rotation's y axis
//
//*****************************************************************************
inline void CMatrix4::ModifyYAxis( const CVector4& a_vecAxisY )
{
	SetRow( ERow_RotationAxisY, a_vecAxisY );
}

//*****************************************************************************
//	ModifyZAxis
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation's z axis
//! \param	[in] The matrix' rotation's z axis
//
//*****************************************************************************
inline void CMatrix4::ModifyZAxis( const CVector3& a_vecAxisZ )
{
	SetRow( ERow_RotationAxisZ, a_vecAxisZ );
}

//*****************************************************************************
//	ModifyZAxis
//*****************************************************************************
//
//! \brief	Modifies the matrix' rotation's z axis
//! \param	[in] The matrix' rotation's z axis
//
//*****************************************************************************
inline void CMatrix4::ModifyZAxis( const CVector4& a_vecAxisZ )
{
	SetRow( ERow_RotationAxisZ, a_vecAxisZ );
}

//*****************************************************************************
//	ModifyTranslation
//*****************************************************************************
//
//! \brief	Modifies the translation offsets 
//! \param	[in] The translation to set
//
//*****************************************************************************
inline void CMatrix4::ModifyTranslation( const CVector3& a_vecTranslation )
{
	AccessTranslation() = a_vecTranslation;
}

//*****************************************************************************
//	SetMatrix
//*****************************************************************************
//
//! \brief	Copies the specified matrix
//! \param	[in] The matrix to copy
//
//*****************************************************************************
inline void CMatrix4::SetMatrix ( const CMatrix4& a_matSource ) 
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
inline void CMatrix4::SetMatrix ( const float a_pfRowColumn [ CMatrix4::ERow_Count ][ CMatrix4::EColumn_Count ] ) 
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
inline void CMatrix4::SetMatrix ( const float a_pfElements [ CMatrix4::EElement_Count ] ) 
{
	memcpy(	m_pfElements					,
			a_pfElements					,
			EElement_Count * sizeof(float)	);
}

//*****************************************************************************
//	InvertExcentric								 
//*****************************************************************************
//
//! \brief	Completely inverts the matrix	(regular matrix inversion)
//
//*****************************************************************************
inline void CMatrix4::InvertExcentric( void ) 
{
	SetMatrix( GetInvertedExcentric() );
}

//*****************************************************************************
//	GetInvertedExcentric
//*****************************************************************************
//
//! \brief	Returns a completely inverted copy of the matrix
//! \param	[out]The completely inverted copy of the matrix
//
//*****************************************************************************
inline void CMatrix4::GetInvertedExcentric( CMatrix4& a_matInverted ) const 
{
	a_matInverted = GetInvertedExcentric();
}

//*****************************************************************************
//	Clear
//*****************************************************************************
//
//! \brief	Clears the matrix (sets all elements to 0.0f)
//
//*****************************************************************************
inline void CMatrix4::Clear( void ) 
{
	SetMatrix( 0.0f );
}

//*****************************************************************************
//	Divide
//*****************************************************************************
//
//! \brief	Divides each element by the specified factor
//! \param	[in] The factor to divide by
//
//*****************************************************************************
inline void CMatrix4::Divide( float a_fDivisor ) 
{
	Multiply( 1.0f / a_fDivisor );
}

//*****************************************************************************
//	ReverseMultiply
//*****************************************************************************
//
//! \brief	Returns the matrix product of this matrix and the specified one
//! \param	[in] The matrix to multiply by this one
//
//*****************************************************************************
inline void CMatrix4::ReverseMultiply( const CMatrix4& a_matPrefix )
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
inline void CMatrix4::Multiply( const CMatrix4& a_matFactor ) 
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
inline float CMatrix4::GetDeterminant( void ) const 
{
	return	  GetElement( 3, 3 ) * GetDeterminantAdjoint( 3, 3 )
			- GetElement( 3, 2 ) * GetDeterminantAdjoint( 3, 2 )
			+ GetElement( 3, 1 ) * GetDeterminantAdjoint( 3, 1 )
			- GetElement( 3, 0 ) * GetDeterminantAdjoint( 3, 0 );
}

//*****************************************************************************
//	GetDeterminantRotation
//*****************************************************************************
//
//! \brief	Determines the matrix' rotation's determinant
//! \return	The matrix' rotation's determinant
//
//*****************************************************************************
inline float CMatrix4::GetDeterminantRotation( void ) const 
{
	return GetDeterminantAdjoint( 3, 3 );
}

//*****************************************************************************
//	GetIdentity
//*****************************************************************************
//
//! \brief	Returns an identity matrix
//! \param	[out] An identity matrix
//
//*****************************************************************************
inline void CMatrix4::GetIdentity( CMatrix4& a_matIdentity )
{
	a_matIdentity.SetIdentity();
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
inline void CMatrix4::GetProductPrefix( CMatrix4& a_matProduct, const CMatrix4& a_matPrefix ) const 
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
inline void CMatrix4::GetProduct( CMatrix4& a_matProduct, const CMatrix4& a_matFactor ) const 
{
	a_matProduct.SetToProduct( (*this), a_matFactor );
}

//*****************************************************************************
//	Invert
//*****************************************************************************
//
//! \brief	Inverts the matrix
//
//*****************************************************************************
inline void CMatrix4::Invert( void ) 
{
	InvertExcentric();
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
inline bool CMatrix4::operator == ( const CMatrix4& a_matOther ) const 
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
inline bool CMatrix4::operator != ( const CMatrix4& a_matOther ) const 
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
inline CMatrix4 CMatrix4::operator ~ ( void ) const 
{
	return GetInverted();
}

//*****************************************************************************
//	operator -
//*****************************************************************************
//
//! \brief	Negates the matrix
//! \return	The negated matrix	
//
//*****************************************************************************
inline CMatrix4 CMatrix4::operator - ( void ) const 
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
inline CMatrix4 CMatrix4::operator + ( const CMatrix4& a_matOffset ) const 
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
inline CMatrix4 CMatrix4::operator - ( const CMatrix4& a_matOffset ) const 
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
inline CMatrix4 CMatrix4::operator * ( float a_fFactor ) const 
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
inline CMatrix4 CMatrix4::operator / ( float a_fFactor ) const 
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
inline CMatrix4 CMatrix4::operator * ( const CMatrix4& a_matFactor ) const 
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
inline CMatrix4 CMatrix4::operator * ( const CAxisAngle& a_Rotation ) const 
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
inline CMatrix4 CMatrix4::operator * ( const CEulerAngles& a_Rotation ) const 
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
inline CMatrix4 CMatrix4::operator * ( const CQuaternion& a_quatRotation ) const 
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
inline CMatrix4 operator * ( float a_fFactor, const CMatrix4& a_matFactor ) 
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
//*****************************************************************************
inline float CMatrix4::operator [] ( int a_iElement ) const 
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
inline float& CMatrix4::operator [] ( int a_iElement ) 
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
inline const CVector3& CMatrix4::operator () ( int a_iRow ) const
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
inline CVector3& CMatrix4::operator () ( int a_iRow ) 
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
inline float CMatrix4::operator () ( int a_iRow, int a_iColumn ) const 
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
inline float& CMatrix4::operator () ( int a_iRow, int a_iColumn ) 
{
	return AccessElement( a_iRow, a_iColumn );
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif	// CMATRIX4_H_INCLUDED
