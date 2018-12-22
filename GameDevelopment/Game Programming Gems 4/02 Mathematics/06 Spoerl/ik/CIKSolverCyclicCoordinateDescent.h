#ifndef			CIKSOLVERCYCLICCOORDINATEDESCENT_H_INCLUDED
#define			CIKSOLVERCYCLICCOORDINATEDESCENT_H_INCLUDED
				
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
//	CIKSolverCyclicCoordinateDescent
//*****************************************************************************
//
//!	An inverse kinematic solver using the Cyclic Coordiante Descent method
//
//*****************************************************************************
class CIKSolverCyclicCoordinateDescent : public CIKSolver
{

private:

										CIKSolverCyclicCoordinateDescent	(const CIKSolverCyclicCoordinateDescent& a_Other);		//!< Private declaration (only) to prevent automatic generation
	CIKSolverCyclicCoordinateDescent	operator=							(const CIKSolverCyclicCoordinateDescent& a_Other);		//!< Private declaration (only) to prevent automatic generation

public:

	// Con-/Destruction

										CIKSolverCyclicCoordinateDescent	();
	virtual								~CIKSolverCyclicCoordinateDescent	();

	// Base class overrides

	// Solve chain

	virtual void						Solve								(bool a_bConstrainRotation);

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // CIKSOLVERCYCLICCOORDINATEDESCENT_H_INCLUDED
