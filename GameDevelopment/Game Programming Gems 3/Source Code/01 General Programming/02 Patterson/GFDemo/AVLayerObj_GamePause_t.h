/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerObj_GamePause_t.h: interface for the AVLayerObj_GamePause_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVLAYEROBJ_GAMEPAUSE_T_H__8B56B553_42B2_447B_8843_6EAAA2A6806B__INCLUDED_)
#define AFX_AVLAYEROBJ_GAMEPAUSE_T_H__8B56B553_42B2_447B_8843_6EAAA2A6806B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFInclude/Framework/AVLayer_t.h"
#include "GFInclude/Objects/TextBox2D_t.h"
#include "GFInclude/Objects/MenuBox2D_t.h"

class AVLayerObj_GamePause_t : public AVLayer_t  
{
public:
	AVLayerObj_GamePause_t();
	virtual ~AVLayerObj_GamePause_t();

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

#endif // !defined(AFX_AVLAYEROBJ_GAMEPAUSE_T_H__8B56B553_42B2_447B_8843_6EAAA2A6806B__INCLUDED_)
