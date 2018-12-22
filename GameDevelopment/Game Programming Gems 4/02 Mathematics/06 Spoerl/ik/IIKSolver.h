#ifndef			IIKSOLVER_H_INCLUDED
#define			IIKSOLVER_H_INCLUDED
				
#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once	
#endif			
#endif			
				
class			CVector3;
class			CQuaternion;
class			CNode;

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	IIKSolver
//*****************************************************************************
//
//!	Interface for an inverse kinematic solver
//
//*****************************************************************************
class IIKSolver
{

public:

	// Target position property

	//! \brief	Sets the position the solver should reach for
	//! \param	[in] The target's new position
	virtual	void				SetTargetPosition		(const CVector3& a_vecPosition) = 0;
	//! \brief	Returns the position the solver should reach for
	//! \return	The target's position
	virtual	const CVector3&		GetTargetPosition		() const = 0;

	// Target threshold property

	//! \brief	Sets the error allowed to reach the target
	//! \param	[in] The new error
	virtual void				SetTargetThreshold		(float a_fThreshold) = 0;
	//! \brief	Returns the error allowed to reach the target
	//! \return	The allowed error
	virtual float				GetTargetThreshold		() const = 0;

	// Limit property

	//! \brief	Sets the maximum number of tries to reach the target
	//! \param	[in] The new limit
	virtual void				SetTryLimit				(int a_iLimit) = 0;
	//! \brief	Returns the maximum number of tries to reach the target
	//! \return	The limit used
	virtual int					GetTryLimit				() const = 0;

	// Initialize chain

	//! \brief	Sets the effector and initalizes the manipulator
	//! \param	[in/out]	The end effector node
	//! \param	[in]		The number of additional links/nodes in the chain
	virtual void				Initialize				(CNode& a_Effector, int a_iLinkCount) = 0;

	// Solve chain

	//! \brief	Calculates the new positions of the nodes
	//! \param	[in] Flag to indicate whether the computed rotations should be clamped or not
	virtual void				Solve					(bool a_bConstrainRotation) = 0;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // IIKSOLVER_H_INCLUDED
