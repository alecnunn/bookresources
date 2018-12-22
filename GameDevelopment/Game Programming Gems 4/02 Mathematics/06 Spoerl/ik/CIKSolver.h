#ifndef			CIKSOLVER_H_INCLUDED
#define			CIKSOLVER_H_INCLUDED
				
#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once	
#endif			
#endif			
				
#include		"IIKSolver.h"
#include		"../math/CVector3.h"
#include		"../math/CQuaternion.h"
#include		"../math/CMatrix4.h"
#include		<vector>

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CIKSolver
//*****************************************************************************
//
//!	Base class for an inverse kinematic solver
//
//*****************************************************************************
class CIKSolver : public IIKSolver
{

private:

								CIKSolver				(const CIKSolver& a_Other);		//!< Private declaration (only) to prevent automatic generation
	CIKSolver&					operator=				(const CIKSolver& a_Other);		//!< Private declaration (only) to prevent automatic generation

protected:

	// Basic IK paramter

	CVector3					m_vecTargetPosition;		//!< The position the solver should reach for
	float						m_fTargetThreshold;			//!< The error allowed to reach the target
	int							m_iTryLimit;				//!< The maximum number of tries to reach the target

	// Nodes in chain

	std::vector<CNode*>			m_arrSourceNodes;			//!< Array of pointers to the nodes in the chain

	// Transformation data

	CMatrix4					m_matWTM;					//!< Basic world transformation matrix
	CMatrix4					m_matWTMInverse;			//!< Inverse of the basic world transformation matrix

	std::vector<CQuaternion>	m_arrCurrentQuat;			//!< Array of intermediate rotation quaternions
	std::vector<CMatrix4>		m_arrCurrentTM;				//!< Array of intermediate local transformation matrices
	std::vector<CMatrix4>		m_arrCurrentTMInverse;		//!< Array of the inverse intermediate local transformation matrices

	// Helper

	virtual void				UpdateTempData			();
	virtual void				StoreRelativeQuaternion	(const CQuaternion& a_Quaternion, int a_iIndex, bool a_bConstrainRotation);
	virtual void				RecalcMatrices			(int a_iStartIndex);
	virtual void				UpdateSourceNodes		();

public:

	// Con-/Destruction

								CIKSolver				();
	virtual						~CIKSolver				() = 0;	// <- !!!

	// Interface base class overrides

	// Target position property

	virtual	void				SetTargetPosition		(const CVector3& a_vecPosition);
	virtual	const CVector3&		GetTargetPosition		() const;

	// Target threshold property

	virtual void				SetTargetThreshold		(float a_fThreshold);
	virtual float				GetTargetThreshold		() const;

	// Limit property

	virtual void				SetTryLimit				(int a_iLimit);
	virtual int					GetTryLimit				() const;

	// Initialize chain

	virtual void				Initialize				(CNode& a_Effector, int a_iLinkCount);

	// Solve chain

	virtual void				Solve					();

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	SetTargetPosition
//*****************************************************************************
inline void CIKSolver::SetTargetPosition(const CVector3& a_vecPosition)
{
	m_vecTargetPosition = a_vecPosition;
}

//*****************************************************************************
//	GetTargetPosition
//*****************************************************************************
inline const CVector3& CIKSolver::GetTargetPosition() const
{
	return m_vecTargetPosition;
}

//*****************************************************************************
//	SetTargetThreshold
//*****************************************************************************
inline void CIKSolver::SetTargetThreshold(float a_fThreshold)
{
	m_fTargetThreshold = a_fThreshold;
}

//*****************************************************************************
//	GetTargetThreshold
//*****************************************************************************
inline float CIKSolver::GetTargetThreshold() const
{
	return m_fTargetThreshold;
}

//*****************************************************************************
//	SetTryLimit
//*****************************************************************************
inline void CIKSolver::SetTryLimit(int a_iLimit)
{
	m_iTryLimit = a_iLimit;
}

//*****************************************************************************
//	GetTryLimit
//*****************************************************************************
inline int CIKSolver::GetTryLimit() const
{
	return m_iTryLimit;
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // CIKSOLVER_H_INCLUDED
