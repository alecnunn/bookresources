#include	"CIKSolver.h"
#include	"CNode.h"

//*********************************************************************************************************************
//*********************************************************************************************************************
//**	Protected methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//*****************************************************************************
//	UpdateTempData
//*****************************************************************************
//
//!	\brief	Updates the stored node transformations
//
//*****************************************************************************
void CIKSolver::UpdateTempData()
{
	for( int iIndex = 0; iIndex < m_arrSourceNodes.size(); ++iIndex )
	{
		m_arrCurrentQuat[iIndex]		= m_arrSourceNodes[iIndex]->GetRotation();
		m_arrCurrentTM[iIndex]			= m_arrSourceNodes[iIndex]->GetMatrix();
		m_arrCurrentTMInverse[iIndex]	= m_arrSourceNodes[iIndex]->GetMatrix().GetInverted();
	}
}

//*****************************************************************************
//	StoreRelativeQuaternion
//*****************************************************************************
//
//!	\brief	Makes the specified quaterion relative and stores it
//! \param	[in] The quaternion to convert and store
//! \param	[in] The index to store the result to
//! \param	[in] Flag to indicate whether the rotation should be clamped or not
//
//*****************************************************************************
void CIKSolver::StoreRelativeQuaternion(const CQuaternion& a_Quaternion, int a_iIndex, bool a_bConstrainRotation)
{
	// ToDo: Find a better way to this quick'n'dirty way to make the quaternion relative

	CMatrix4 matRelative = a_Quaternion.GetMatrix() * m_arrCurrentTM[a_iIndex];

	if( a_iIndex == m_arrSourceNodes.size() - 1 )
		matRelative = m_matWTMInverse * matRelative;
	else
		matRelative = m_arrCurrentTMInverse[a_iIndex + 1] * matRelative;

	// Store unconstrained quaternion

	if( false == a_bConstrainRotation )
	{
		m_arrCurrentQuat[a_iIndex].SetQuaternion(matRelative);
		m_arrCurrentQuat[a_iIndex].Normalize();

		return;
	}

	// Clamp the rotation using the evil Euler angle conversion

	CQuaternion		quatRotation(matRelative);
	CEulerAngles	eulerAngles	(quatRotation);

	eulerAngles.AccessPitch()	= Clamp( eulerAngles.GetPitch(), m_arrSourceNodes[a_iIndex]->GetNegativeLimits().GetX(), m_arrSourceNodes[a_iIndex]->GetPositiveLimits().GetX() );
	eulerAngles.AccessYaw()		= Clamp( eulerAngles.GetYaw(), m_arrSourceNodes[a_iIndex]->GetNegativeLimits().GetY(), m_arrSourceNodes[a_iIndex]->GetPositiveLimits().GetY() );
	eulerAngles.AccessRoll()	= Clamp( eulerAngles.GetRoll(), m_arrSourceNodes[a_iIndex]->GetNegativeLimits().GetZ(), m_arrSourceNodes[a_iIndex]->GetPositiveLimits().GetZ() );

	quatRotation.SetQuaternion(eulerAngles);
	quatRotation.Normalize();

	m_arrCurrentQuat[a_iIndex] = quatRotation;
/*
	// Clamp the rotation in quaternion space

	CQuaternion quatRotation(matRelative);

	if( quatRotation.GetW() < 0.0f )
		quatRotation.Invert();

	float fAngleX = Clamp( g_asinf(quatRotation.GetX()) * 2.0f, m_arrSourceNodes[a_iIndex]->GetNegativeLimits().GetX(), m_arrSourceNodes[a_iIndex]->GetPositiveLimits().GetX() );
	float fAngleY = Clamp( g_asinf(quatRotation.GetY()) * 2.0f, m_arrSourceNodes[a_iIndex]->GetNegativeLimits().GetY(), m_arrSourceNodes[a_iIndex]->GetPositiveLimits().GetY() );
	float fAngleZ = Clamp( g_asinf(quatRotation.GetZ()) * 2.0f, m_arrSourceNodes[a_iIndex]->GetNegativeLimits().GetZ(), m_arrSourceNodes[a_iIndex]->GetPositiveLimits().GetZ() );

	quatRotation.AccessX() = g_sinf	(fAngleX * 0.5f);
	quatRotation.AccessY() = g_sinf	(fAngleY * 0.5f);
	quatRotation.AccessZ() = g_sinf	(fAngleZ * 0.5f);
	quatRotation.AccessW() = g_sqrtf( 1.0f	- quatRotation.GetX() * quatRotation.GetX()
											+ quatRotation.GetY() * quatRotation.GetY() 
											+ quatRotation.GetZ() * quatRotation.GetZ() );

	quatRotation.Normalize();

	m_arrCurrentQuat[a_iIndex] = quatRotation;
*/
}

//*****************************************************************************
//	RecalcMatrices
//*****************************************************************************
//
//!	\brief	Calculates the local transformation matrices
//! \param	[in] The node index to start from
//
//*****************************************************************************
void CIKSolver::RecalcMatrices(int a_iStartIndex)
{
	CMatrix4 matTemp;

	// Counting down because the 0th element is the effector

	int iNode = a_iStartIndex + 1;

	if( iNode > 0 ) while( iNode-- )
	{
		if( iNode == m_arrSourceNodes.size() - 1 )
			m_arrCurrentTM[iNode] = m_matWTM;
		else
			m_arrCurrentTM[iNode] = m_arrCurrentTM[iNode + 1];

		matTemp							.SetMatrix(m_arrCurrentQuat[iNode], m_arrSourceNodes[iNode]->GetTranslation());
		m_arrCurrentTM[iNode]			*= matTemp;
		m_arrCurrentTMInverse[iNode]	= m_arrCurrentTM[iNode].GetInverted();
	}
}

//*****************************************************************************
//	UpdateSourceNodes
//*****************************************************************************
//
//!	\brief	Sets the nodes rotations to the intermediate rotation quaternions
//
//*****************************************************************************
void CIKSolver::UpdateSourceNodes()
{
	for( int iNode = 0; iNode < m_arrSourceNodes.size(); ++iNode )
		m_arrSourceNodes[iNode]->SetRotation(m_arrCurrentQuat[iNode]);
}

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
CIKSolver::CIKSolver()
: m_vecTargetPosition(0.0f, 0.0f, 0.0f), m_fTargetThreshold(1.0f), m_iTryLimit(100)
{}

//*****************************************************************************
//	Destructor
//*****************************************************************************
//
//!	\brief	Destructor
//
//*****************************************************************************
CIKSolver::~CIKSolver()
{}

//*****************************************************************************
//	Solve
//*****************************************************************************
void CIKSolver::Solve()
{}

//*****************************************************************************
//	Initialize
//*****************************************************************************
void CIKSolver::Initialize(CNode& a_Effector, int a_iLinkCount)
{
	// Collect nodes in chain

	m_arrSourceNodes.clear();
	m_arrSourceNodes.push_back(&a_Effector);

	CNode* pParent = a_Effector.AccessParent();

	while( NULL != pParent && a_iLinkCount > 0 )
	{
		m_arrSourceNodes.push_back(pParent);
		--a_iLinkCount;
		pParent = pParent->AccessParent();
	}

	// Prepare quaternion and matrix container

	m_arrCurrentQuat		.resize(m_arrSourceNodes.size());
	m_arrCurrentTM			.resize(m_arrSourceNodes.size());
	m_arrCurrentTMInverse	.resize(m_arrSourceNodes.size());

	for( int iNode = 0; iNode < m_arrSourceNodes.size(); ++iNode )
	{
		m_arrCurrentQuat[iNode]			= m_arrSourceNodes[iNode]->GetRotation();
		m_arrCurrentTM[iNode]			= m_arrSourceNodes[iNode]->GetMatrix();
		m_arrCurrentTMInverse[iNode]	= m_arrSourceNodes[iNode]->GetMatrix().GetInverted();
	}

	// Get basic world transformation matrix

	pParent = m_arrSourceNodes[m_arrSourceNodes.size() - 1]->AccessParent();

	if( NULL != pParent )
	{
		m_matWTM		= pParent->GetMatrix();
		m_matWTMInverse	= pParent->GetMatrix().GetInverted();
	}
	else
	{
		m_matWTM		.SetIdentity();
		m_matWTMInverse	.SetIdentity();
	}
}
