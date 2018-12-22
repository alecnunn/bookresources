#ifndef			CIKSOLVERJACOBIANTRANSPOSE_H_INCLUDED
#define			CIKSOLVERJACOBIANTRANSPOSE_H_INCLUDED
				
#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once	
#endif			
#endif			
				
#include		"CIKSolver.h"

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CIKSolverJacobianTranspose
//*****************************************************************************
//
//!	An inverse kinematic solver using the Jacobian Transpose method
//
//*****************************************************************************
class CIKSolverJacobianTranspose : public CIKSolver
{

private:

								CIKSolverJacobianTranspose	(const CIKSolverJacobianTranspose& a_Other);	//!< Private declaration (only) to prevent automatic generation
	CIKSolverJacobianTranspose	operator=					(const CIKSolverJacobianTranspose& a_Other);	//!< Private declaration (only) to prevent automatic generation

protected:

	std::vector<float>			m_arrJacobian;				//!< The Jacobi matrix
	std::vector<float>			m_arrJacobianTransposed;	//!< The transposed Jacobian
	std::vector<float>			m_arrQDerivate;				//!< Joint rotation angles
	std::vector<CVector3>		m_arrAxis;					//!< Joint rotation axes

public:

	// Con-/Destruction

								CIKSolverJacobianTranspose	();
	virtual						~CIKSolverJacobianTranspose	();

	// Base class overrides

	// Solve chain

	virtual void				Solve						(bool a_bConstrainRotation);

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // CIKSOLVERJACOBIANTRANSPOSE_H_INCLUDED
