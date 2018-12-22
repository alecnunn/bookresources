/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// MenuBox2D_t.cpp: implementation of the MenuBox2D_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Objects/MenuBox2D_t.h"
#include "GFInclude/Systems/VisualSys_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MenuBox2D_t::MenuBox2D_t()
{
	m_nChoice = 0;
	m_fXPos = 0.0f;
	m_fYPos = 0.0f;
	m_fXSize = 100.0f;
	m_fYSize = 100.0f;
	m_nTextHighlightColor = 0xffffffff;
	m_nTextNormalColor = 0xff7f7f7f;
	m_nBoxHighlightColor = 0xff7f7f7f;
	m_nBoxNormalColor = 0xff000000;
}

MenuBox2D_t::~MenuBox2D_t()
{

}

void MenuBox2D_t::PushBackChoice( TextBox2D_t &TextBox )
{
	m_TextBoxVector.push_back( TextBox );
}

void MenuBox2D_t::SetChoice( int nChoice )
{
	m_nChoice = nChoice;
}

int MenuBox2D_t::GetChoice()
{
	return m_nChoice;
}

void MenuBox2D_t::NextChoice()
{
	if( m_nChoice < (m_TextBoxVector.size() - 1) )
	{
		m_nChoice++;
	}
}

void MenuBox2D_t::PrevChoice()
{
	if( m_nChoice > 0 )
	{
		m_nChoice--;
	}
}

