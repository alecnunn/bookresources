/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerObj_StartMenu_t.h: interface for the AVLayerObj_StartMenu_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYEROBJ_STARTMENU_T_H__E675437F_4E63_484D_BE7A_BE2A5EB72024__INCLUDED_)
#define AFX_AVLAYEROBJ_STARTMENU_T_H__E675437F_4E63_484D_BE7A_BE2A5EB72024__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayer_t.h"
#include "GFInclude/Objects/TextBox2D_t.h"
#include "GFInclude/Objects/MenuBox2D_t.h"

class AVLayerObj_StartMenu_t : public AVLayer_t  
{
public:
	AVLayerObj_StartMenu_t();
	virtual ~AVLayerObj_StartMenu_t();

	virtual bool Connect( Systems_t *pSystems );
	virtual void Disconnect();

	virtual void Update();
	virtual void RenderVisual();
	virtual void RenderAudio();

	// configurable
	TextBox2D_t m_ModeTextBox;
	TextBox2D_t m_InfoTextBox;
	MenuBox2D_t m_MenuBox;
protected:
	Systems_t *m_pSystems;
};

#endif // !defined(AFX_AVLAYEROBJ_STARTMENU_T_H__E675437F_4E63_484D_BE7A_BE2A5EB72024__INCLUDED_)
