#include	"CIKSolverCyclicCoordinateDescent.h"

//*********************************************************************************************************************
//*********************************************************************************************************************
//**	Public methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//*****************************************************************************
//	Default constructor
//*****************************************************************************
//
//!	\brief	Default constructor
//
//*****************************************************************************
CIKSolverCyclicCoordinateDescent::CIKSolverCyclicCoordinateDescent()
: CIKSolver()
{}

//*****************************************************************************
//	Destructor
//*****************************************************************************
//
//!	\brief	Destructor
//
//*****************************************************************************
CIKSolverCyclicCoordinateDescent::~CIKSolverCyclicCoordinateDescent()
{}

//*****************************************************************************
//	Solve
//*****************************************************************************
void CIKSolverCyclicCoordinateDescent::Solve(bool a_bConstrainRotation)
{
	// Update transformations

	UpdateTempData();

	// Local variables

	CVector3	vecEnd, vecLink, vecDifference, vecLinkTarget, vecLinkEnd;
	CQuaternion	quatAlign;
	float		fError;
	CMatrix4	matRelative;
	int			iTry	= 0;
	int			iLink	= 1;

	//
	// Cyclic Coordiante Descent
	//

	do
	{
		m_arrCurrentTM[0]		.GetTranslation(vecEnd);			// Position of end effektor
		m_arrCurrentTM[iLink]	.GetTranslation(vecLink);			// Position of current node
		vecDifference			= vecEnd - m_vecTargetPosition;		// What do we have to do to reach that?
		fError					= vecDifference.GetMagnitude();		// How far/much is that?

		if( fError > m_fTargetThreshold )							// Already within tolerable regions?
		{
			vecLinkTarget	= m_vecTargetPosition - vecLink;		// Vector current link -> target
			vecLinkTarget	.Normalize();
			vecLinkEnd		= vecEnd - vecLink;						// Vector current link -> current effektor position
			vecLinkEnd		.Normalize();

			quatAlign.SetQuaternion(vecLinkEnd, vecLinkTarget);		// Align the two vectors
			quatAlign.Normalize();

			StoreRelativeQuaternion(quatAlign, iLink, a_bConstrainRotation);

			RecalcMatrices(iLink);									// Recalculate transformation matrices for the next loop

			if( ++iLink > m_arrSourceNodes.size() - 1 )				// Next link
				iLink = 1;
		}

	}while( ++iTry < m_iTryLimit && fError > m_fTargetThreshold );	// Check criteria for abort

	// Set result

	UpdateSourceNodes();											// Update original chain with calculated values
}
