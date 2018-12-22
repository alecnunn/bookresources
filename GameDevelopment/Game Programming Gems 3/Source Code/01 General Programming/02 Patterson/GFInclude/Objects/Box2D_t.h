/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// Box2D_t.h: interface for the Box2D_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOX2D_T_H__3ABD4E5C_11FF_40DF_97E6_4D76704CCEF7__INCLUDED_)
#define AFX_BOX2D_T_H__3ABD4E5C_11FF_40DF_97E6_4D76704CCEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Box2D_t  
{
public:
	Box2D_t();
	virtual ~Box2D_t();

	float m_fXPos;
	float m_fYPos;
	float m_fXSize;
	float m_fYSize;
	unsigned long m_nColor;
};

#endif // !defined(AFX_BOX2D_T_H__3ABD4E5C_11FF_40DF_97E6_4D76704CCEF7__INCLUDED_)
