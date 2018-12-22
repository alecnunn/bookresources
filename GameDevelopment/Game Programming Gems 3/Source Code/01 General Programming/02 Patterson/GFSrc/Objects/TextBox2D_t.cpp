/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TextBox2D_t.cpp: implementation of the TextBox2D_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Objects/TextBox2D_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TextBox2D_t::TextBox2D_t()
{
	m_fXPos = 0.0f;
	m_fYPos = 0.0f;
	m_fXSize = 100.0f;
	m_fYSize = 100.0f;

	m_bDrawText = true;
	m_nTextColor = 0xff7f7f7f;

	m_bDrawBox = false;
	m_nBoxColor = 0xff0000ff;
}

TextBox2D_t::~TextBox2D_t()
{

}
