#ifndef			CTARGET_H_INCLUDED
#define			CTARGET_H_INCLUDED

#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once
#endif 
#endif 

#include		"../math/CVector3.h"
#include		"../math/CQuaternion.h"

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CTarget
//*****************************************************************************
//
//!	Target object for the IK-chain
//
//*****************************************************************************
class CTarget
{

private:

						CTarget			(const CTarget& a_Other);	//!< Private declaration (only) to prevent automatic generation
	CTarget&			operator=		(const CTarget& a_Other);	//!< Private declaration (only) to prevent automatic generation

protected:

	// Target position and orientation

	CVector3			m_vecPosition;			//!< Target's current position

	// Threshold to reach target

	float				m_fThreshold;			//!< Threshold allowed to reach the target

public:

	// Con-/Destruction

						CTarget			();
						~CTarget		();

	// Transform target

	void				Translate		(const CVector3& a_vecTranslation);

	// Position property

	void				SetPosition		(const CVector3& a_vecPosition);
	const CVector3&		GetPosition		() const;

	// Threshold property

	void				SetThreshold	(float a_fThreshold);
	const float&		GetThreshold	() const;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************
	
//*****************************************************************************
//	Translate
//*****************************************************************************
//
//!	\brief	Moves the target by the given vector
//!	\param	[in] The movement vector
//
//*****************************************************************************
inline void	CTarget::Translate(const CVector3& a_vecTranslation)
{
	m_vecPosition += a_vecTranslation;
}

//*****************************************************************************
//	SetPosition
//*****************************************************************************
//
//!	\brief	Sets the target's current position
//!	\param	[in] The new position
//
//*****************************************************************************
inline void	CTarget::SetPosition(const CVector3& a_vecPosition)
{
	m_vecPosition = a_vecPosition;
}

//*****************************************************************************
//	GetPosition
//*****************************************************************************
//
//!	\brief	Returns the target's current position
//!	\return	The current position vector
//
//*****************************************************************************
inline const CVector3& CTarget::GetPosition() const
{
	return m_vecPosition;
}

//*****************************************************************************
//	SetThreshold
//*****************************************************************************
//
//!	\brief	Sets the threshold to reach the target
//!	\param	[in] The new threshold
//
//*****************************************************************************
inline void CTarget::SetThreshold(float a_fThreshold)
{
	m_fThreshold = a_fThreshold;
}

//*****************************************************************************
//	GetThreshold
//*****************************************************************************
//
//!	\brief	Returns the threshold to reach the target
//!	\return	The current threshold
//
//*****************************************************************************
inline const float& CTarget::GetThreshold() const
{
	return m_fThreshold;
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // CTARGET_H_INCLUDED
