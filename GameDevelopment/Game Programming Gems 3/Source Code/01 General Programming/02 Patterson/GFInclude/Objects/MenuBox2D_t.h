/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// MenuBox2D_t.h: interface for the MenuBox2D_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUBOX2D_T_H__EE7AB82E_D099_45B5_912D_900BE1DF662C__INCLUDED_)
#define AFX_MENUBOX2D_T_H__EE7AB82E_D099_45B5_912D_900BE1DF662C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Objects/TextBox2D_t.h"
class VisualSys_t;

class MenuBox2D_t  
{
public:
	MenuBox2D_t();
	virtual ~MenuBox2D_t();

	void PushBackChoice( TextBox2D_t &TextBox );
	void SetChoice( int nChoice );
	int GetChoice();
	void NextChoice();
	void PrevChoice();

	int m_nChoice;
	float m_fXPos;
	float m_fYPos;
	float m_fXSize;
	float m_fYSize;
	unsigned long m_nTextHighlightColor;
	unsigned long m_nTextNormalColor;
	unsigned long m_nBoxHighlightColor;
	unsigned long m_nBoxNormalColor;
	TextBox2D_Vector_t m_TextBoxVector;
};

#endif // !defined(AFX_MENUBOX2D_T_H__EE7AB82E_D099_45B5_912D_900BE1DF662C__INCLUDED_)
