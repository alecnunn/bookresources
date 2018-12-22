#include	"CNode.h"

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
CNode::CNode()
: m_pParent(NULL), m_vecTranslation(0.0f, 1.0f, 0.0f), m_quatRotation(0.0f, 0.0f, 0.0f, 1.0f)
, m_vecPositiveLimits(g_fPIDiv4, g_fPIDiv4, g_fPIDiv4)
, m_vecNegativeLimits(-g_fPIDiv4, -g_fPIDiv4, -g_fPIDiv4)
{}

//*****************************************************************************
//	Destructor
//*****************************************************************************
//
//!	\brief	Destructor
//
//*****************************************************************************
CNode::~CNode()
{
	// Delete children

	for( CNodePtrArray::iterator iNode = m_arrChildren.begin(); iNode != m_arrChildren.end(); ++iNode )
		delete (*iNode);
}

//*****************************************************************************
//	AddChild
//*****************************************************************************
//
//!	\brief	Adds a new child node
//! \param	[in/out] The new child to add
//!	\return	Returns either of the following values
//! \retval	true - The operation succeeded
//! \retval	false - The operation failed
//
//*****************************************************************************
bool CNode::AddChild(CNode* a_pChild)
{
	// Check this

	if( this == a_pChild )
		return false;

	// Check for redundancy

	for( CNodePtrArray::const_iterator iNode = m_arrChildren.begin(); iNode != m_arrChildren.end(); ++iNode )
		if( (*iNode) == a_pChild )
			return false;

	// Add child and set its parent

	m_arrChildren.push_back(a_pChild);
	a_pChild->m_pParent = this;

	return true;
}

//*****************************************************************************
//	RemoveChild
//*****************************************************************************
//
//!	\brief	Removes the given child node
//! \param	[in/out]	The child to remove
//! \param	[in]		Flag whether the child should be deleted as well
//!	\return	Returns either of the following values
//! \retval	true - The operation succeeded
//! \retval	false - The operation failed
//
//*****************************************************************************
bool CNode::RemoveChild(CNode* a_pChild, bool a_bDeleteIt)
{
	// Check this

	if( this == a_pChild )
		return false;

	// Search for child

	for( CNodePtrArray::iterator iNode = m_arrChildren.begin(); iNode != m_arrChildren.end(); ++iNode )
	{
		// Delete child

		if( a_pChild == (*iNode) )
		{
			if( true == a_bDeleteIt )
			{
				delete a_pChild;
				a_pChild = NULL;
			}
			else
			{
				a_pChild->m_pParent = NULL;
			}

			m_arrChildren.erase(iNode);
			
			return true;
		}
	}

	return false;
}

//*****************************************************************************
//	CalculateMatrix
//*****************************************************************************
//
//!	\brief	Calculates the current transformation matrix
//! \param	[in] Flag whether the node's children should be processed as well
//
//*****************************************************************************
void CNode::CalculateMatrix(bool a_bDoChildren)
{
	// Set base matrix

	if( NULL != m_pParent )
		m_matCurrent = m_pParent->m_matCurrent;
	else
		m_matCurrent.SetIdentity();

	// Set local transformation

	CMatrix4	matTemp;
				matTemp.SetMatrix(m_quatRotation, m_vecTranslation);

	// Set global transformation

   	m_matCurrent *= matTemp;

	// Calculate child matrices

	if( true == a_bDoChildren )
		for( CNodePtrArray::iterator iNode = m_arrChildren.begin(); iNode != m_arrChildren.end(); ++iNode )
			(*iNode)->CalculateMatrix(a_bDoChildren);
}
