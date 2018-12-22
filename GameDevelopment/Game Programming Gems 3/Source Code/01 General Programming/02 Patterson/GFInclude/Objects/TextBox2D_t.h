/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TextBox2D_t.h: interface for the TextBox2D_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTBOX2D_T_H__EEC592CC_A101_464A_804B_ED50D5805271__INCLUDED_)
#define AFX_TEXTBOX2D_T_H__EEC592CC_A101_464A_804B_ED50D5805271__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TextBox2D_t  
{
public:
	TextBox2D_t();
	virtual ~TextBox2D_t();

	float m_fXPos;
	float m_fYPos;
	float m_fXSize;
	float m_fYSize;

	bool m_bDrawText;
	unsigned long m_nTextColor;
	String_t m_Text;

	bool m_bDrawBox;
	unsigned long m_nBoxColor;
};

typedef std::vector<TextBox2D_t> TextBox2D_Vector_t;

#endif // !defined(AFX_TEXTBOX2D_T_H__EEC592CC_A101_464A_804B_ED50D5805271__INCLUDED_)
