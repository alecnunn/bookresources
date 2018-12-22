#ifndef			CNODE_H_INCLUDED
#define			CNODE_H_INCLUDED
				
#ifdef			_MSC_VER
#if				_MSC_VER > 1000
#pragma once	
#endif			
#endif			
				
#include		"../math/CVector3.h"
#include		"../math/CQuaternion.h"
#include		"../math/CMatrix4.h"
#include		<vector>

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	CNode
//*****************************************************************************
//
//!	Basic node class to be used with an inverse kinematic solver.
//
//*****************************************************************************
class CNode
{

private:

								CNode				(const CNode& a_Other);	//!< Private declaration (only) to prevent automatic generation
	CNode&						operator=			(const CNode& a_Other);	//!< Private declaration (only) to prevent automatic generation

protected:

	// Types

	typedef std::vector<CNode*> CNodePtrArray;	//!< Node pointer array

	// Hierarchy

	CNode*			m_pParent;					//!< Pointer to the parent node
	CNodePtrArray	m_arrChildren;				//!< Array of pointer to the child nodes

	// Transformation

	CVector3		m_vecTranslation;			//!< Current translation
	CQuaternion		m_quatRotation;				//!< Current rotation
	CMatrix4		m_matCurrent;				//!< Current transformation matrix

	// Constraints

	CVector3		m_vecPositiveLimits;		//!< Positive rotation limits
	CVector3		m_vecNegativeLimits;		//!< Negative rotation limits

public:

	// Con-/Destruction

								CNode				();
	virtual						~CNode				();

	// Parent property

	virtual	const CNode*		GetParent			() const;
	virtual	CNode*				AccessParent		();

	// Children property

	virtual	bool				AddChild			(CNode* a_pChild);
	virtual	bool				RemoveChild			(CNode* a_pChild, bool a_bDeleteIt);
	virtual	int					GetChildCount		() const;
	virtual	const CNode*		GetChild			(int a_iIndex) const;
	virtual	CNode*				AccessChild			(int a_iIndex);

	// Translation property

	virtual	void				SetTranslation		(const CVector3& a_vecTranslation);
	virtual	const CVector3&		GetTranslation		() const;
	
	// Rotation property

	virtual	void				SetRotation			(const CQuaternion& a_quatRotation);
	virtual	const CQuaternion&	GetRotation			() const;

	// Current transformation matrix property

	virtual	void				CalculateMatrix		(bool a_bDoChildren);
	virtual const CMatrix4&		GetMatrix			() const;

	// Constraints properties

	virtual void				SetPositiveLimits	(const CVector3& a_vecLimits);
	virtual const CVector3&		GetPositiveLimits	() const;

	virtual void				SetNegativeLimits	(const CVector3& a_vecLimits);
	virtual const CVector3&		GetNegativeLimits	() const;

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//*****************************************************************************
//	GetParent
//*****************************************************************************
//
//!	\brief	Returns the node's parent for look-up purposes
//!	\return	The node's parent
//
//*****************************************************************************
inline const CNode*	CNode::GetParent() const
{
	return m_pParent;
}

//*****************************************************************************
//	AccessParent
//*****************************************************************************
//
//!	\brief	Returns the node's parent for modification purposes
//!	\return	The node's parent
//
//*****************************************************************************
inline CNode* CNode::AccessParent()
{
	return m_pParent;
}

//*****************************************************************************
//	GetChildCount
//*****************************************************************************
//
//!	\brief	Returns the node's number of children
//!	\return	The number of children
//
//*****************************************************************************
inline int CNode::GetChildCount() const
{
	return m_arrChildren.size();
}

//*****************************************************************************
//	GetChild
//*****************************************************************************
//
//!	\brief	Returns the specified child for look-up purposes
//! \param	[in] The child's index
//!	\return	The specified child
//
//*****************************************************************************
inline const CNode* CNode::GetChild(int a_iIndex) const
{
	return m_arrChildren[a_iIndex];
}

//*****************************************************************************
//	AccessChild
//*****************************************************************************
//
//!	\brief	Returns the specified child for modification purposes
//! \param	[in] The child's index
//!	\return	The specified child
//
//*****************************************************************************
inline CNode* CNode::AccessChild(int a_iIndex)
{
	return m_arrChildren[a_iIndex];
}

//*****************************************************************************
//	SetTranslation
//*****************************************************************************
//
//!	\brief	Sets the current translation
//! \param	[in] The new translation
//
//*****************************************************************************
inline void	CNode::SetTranslation(const CVector3& a_vecTranslation)
{
	m_vecTranslation = a_vecTranslation;
}

//*****************************************************************************
//	GetTranslation
//*****************************************************************************
//
//!	\brief	Returns the current translation
//! \return	The current translation vector
//
//*****************************************************************************
inline const CVector3& CNode::GetTranslation() const
{
	return m_vecTranslation;
}

//*****************************************************************************
//	SetRotation
//*****************************************************************************
//
//!	\brief	Sets the current rotation
//! \param	[in] The new rotation
//
//*****************************************************************************
inline void	CNode::SetRotation(const CQuaternion& a_quatRotation)
{
	m_quatRotation = a_quatRotation;
}

//*****************************************************************************
//	GetRotation
//*****************************************************************************
//
//!	\brief	Returns the current rotation
//! \return	The current rotation quaternion
//
//*****************************************************************************
inline const CQuaternion& CNode::GetRotation() const
{
	return m_quatRotation;
}

//*****************************************************************************
//	GetMatrix
//*****************************************************************************
//
//!	\brief	Returns the current transformation matrix
//! \return	The current transformation matrix
//
//*****************************************************************************
inline const CMatrix4& CNode::GetMatrix() const
{
	return m_matCurrent;
}

//*****************************************************************************
//	SetPositiveLimits
//*****************************************************************************
//
//!	\brief	Sets the positive rotation limits
//! \param	[in] The new limits
//
//*****************************************************************************
inline void CNode::SetPositiveLimits(const CVector3& a_vecLimits)
{
	m_vecPositiveLimits = a_vecLimits;
}

//*****************************************************************************
//	GetPositiveLimits
//*****************************************************************************
//
//!	\brief	Returns the positive rotation limits
//! \return	The current limits
//
//*****************************************************************************
inline const CVector3& CNode::GetPositiveLimits() const
{
	return m_vecPositiveLimits;
}

//*****************************************************************************
//	SetNegativeLimits
//*****************************************************************************
//
//!	\brief	Sets the negative rotation limits
//! \param	[in] The new limits
//
//*****************************************************************************
inline void CNode::SetNegativeLimits(const CVector3& a_vecLimits)
{
	m_vecNegativeLimits = a_vecLimits;
}

//*****************************************************************************
//	GetNegativeLimits
//*****************************************************************************
//
//!	\brief	Returns the negative rotation limits
//! \return	The current limits
//
//*****************************************************************************
inline const CVector3& CNode::GetNegativeLimits() const
{
	return m_vecNegativeLimits;
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // CNODE_H_INCLUDED
