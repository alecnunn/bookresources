/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// Box2D_t.cpp: implementation of the Box2D_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Objects/Box2D_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Box2D_t::Box2D_t()
{
	m_fXPos = 0.0f;
	m_fYPos = 0.0f;
	m_fXSize = 100.0f;
	m_fYSize = 100.0f;
	m_nColor = 0xffff0000;
}

Box2D_t::~Box2D_t()
{

}
