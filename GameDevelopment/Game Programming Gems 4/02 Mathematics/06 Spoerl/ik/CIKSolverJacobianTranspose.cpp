#include	"CIKSolverJacobianTranspose.h"

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
CIKSolverJacobianTranspose::CIKSolverJacobianTranspose()
: CIKSolver()
{}

//*****************************************************************************
//	Destructor
//*****************************************************************************
//
//!	\brief	Destructor
//
//*****************************************************************************
CIKSolverJacobianTranspose::~CIKSolverJacobianTranspose()
{}

//*****************************************************************************
//	Solve
//*****************************************************************************
void CIKSolverJacobianTranspose::Solve(bool a_bConstrainRotation)
{
	// Update transformations

	UpdateTempData();

	// Local variables

	CQuaternion	quatAlign;
	CAxisAngle	axisAngle;
	CMatrix4	matRelative;
	float		farrForce[6];
	float		fError;
	int			iTries	= 0;
	int			iLevel = m_arrSourceNodes.size() - 1;

	// Prepare JT container
	// ToDo: Replace STL with something more sophisticated

	m_arrJacobian			.resize(iLevel * 6);
	m_arrJacobianTransposed	.resize(iLevel * 6);
	m_arrQDerivate			.resize(iLevel);
	m_arrAxis				.resize(iLevel);

	//
	// Jacobian Transpose
	//

	do
	{
		int			iLinkIndex, iRow, iColumn, iIndex;
		CVector3	vecEnd, vecDifference, vecLink, vecLinkTarget, vecLinkEnd, vecEntry;

		m_arrCurrentTM[0]	.GetTranslation(vecEnd);		// Position of end effector
		vecDifference		= vecEnd - m_vecTargetPosition;	// What do we have to do to reach that?
		fError				= vecDifference.GetMagnitude();	// How far is that?

		if( fError > m_fTargetThreshold )
		{
			//
			// Build Jacobian
			//

			for( iColumn = 0, iLinkIndex = 1; iColumn < iLevel; ++iColumn, ++iLinkIndex )
			{
				m_arrCurrentTM[iLinkIndex].GetTranslation(vecLink);	// Position of current node

				vecLinkTarget	= m_vecTargetPosition - vecLink;	// Vector current link -> target
				vecLinkEnd		= vecEnd - vecLink;					// Vector current link -> current effector position

				m_arrAxis[iColumn] = CVector3::CrossProduct(vecLinkTarget, vecLinkEnd);
				m_arrAxis[iColumn] .Normalize();

				vecEntry = CVector3::CrossProduct(vecLinkEnd, m_arrAxis[iColumn]);

				for( iRow = 0; iRow < 3; ++iRow )
				{
					m_arrJacobian[iRow		* iLevel + iColumn]	= vecEntry[iRow];
					m_arrJacobian[(iRow+3)	* iLevel + iColumn]	= m_arrAxis[iColumn][iRow];
				}
			}

			//
			// Transpose Jacobian
			//

			for( iRow = 0; iRow < 6; ++iRow )
				for( iColumn = 0; iColumn < iLevel; ++iColumn )
					m_arrJacobianTransposed[iColumn * 6 + iRow] = m_arrJacobian[iRow * iLevel + iColumn];

			//
			// Set force vector
			//

			farrForce[0] = vecDifference.GetX();
			farrForce[1] = vecDifference.GetY();
			farrForce[2] = vecDifference.GetZ();
			farrForce[3] = 0.0f;
			farrForce[4] = 0.0f;
			farrForce[5] = 0.0f;

			//
			// Compute q'
			//

			for( iRow = 0; iRow < iLevel; ++iRow )
			{
				m_arrQDerivate[iRow] = 0.0f;

				for( iColumn = 0; iColumn < 6; ++iColumn )
					m_arrQDerivate[iRow] += m_arrJacobianTransposed[iRow * 6 + iColumn] * farrForce[iColumn];
			}

			//
			// Integrate and apply changes
			//

			for( iIndex = 0, iLinkIndex = 1; iIndex < iLevel; ++iIndex, ++iLinkIndex )
			{
				axisAngle.SetAxis(m_arrAxis[iIndex]);
				axisAngle.SetAngle(m_arrQDerivate[iIndex] * 0.01f);	// Simple Euler integration step
				quatAlign.SetQuaternion(axisAngle);
				quatAlign.Normalize();

				StoreRelativeQuaternion(quatAlign, iLinkIndex, a_bConstrainRotation);
			}

			RecalcMatrices(iLevel);		// Recalculate transformation matrices for the next loop
		}

	}while( ++iTries < m_iTryLimit && fError > m_fTargetThreshold );	// Check criteria for abort

	// Set result

	UpdateSourceNodes();	// Update original chain with calculated values
}
