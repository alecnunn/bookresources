/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfigObj_GameConfig_t.cpp: implementation of the AVLayerConfigObj_GameConfig_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVLayerConfigObj_GameConfig_t.h"
#include "AVLayerObj_GameConfig_t.h"
#include "GameConstants.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVLayerConfigObj_GameConfig_t::AVLayerConfigObj_GameConfig_t()
{

}

AVLayerConfigObj_GameConfig_t::~AVLayerConfigObj_GameConfig_t()
{

}

void AVLayerConfigObj_GameConfig_t::Configure( AVLayer_t *pAVLayer )
{
	AVLayerObj_GameConfig_t *pAVLayerObj = (AVLayerObj_GameConfig_t *)pAVLayer;

	pAVLayerObj->m_ModeTextBox.m_fXPos = MODE_BOX_XPOS;
	pAVLayerObj->m_ModeTextBox.m_fYPos = MODE_BOX_YPOS;
	pAVLayerObj->m_ModeTextBox.m_fXSize = MODE_BOX_XSIZE;
	pAVLayerObj->m_ModeTextBox.m_fYSize = MODE_BOX_YSIZE;
	pAVLayerObj->m_ModeTextBox.m_Text = "GameConfig";

	pAVLayerObj->m_InfoTextBox.m_fXPos = INFO_BOX_XPOS;
	pAVLayerObj->m_InfoTextBox.m_fYPos = INFO_BOX_YPOS;
	pAVLayerObj->m_InfoTextBox.m_fXSize = INFO_BOX_XSIZE;
	pAVLayerObj->m_InfoTextBox.m_fYSize = INFO_BOX_YSIZE;
	pAVLayerObj->m_InfoTextBox.m_Text = "Choose menu item and hit Enter key to enter game";

	pAVLayerObj->m_MenuBox.m_fXPos = MENU_BOX_XPOS;
	pAVLayerObj->m_MenuBox.m_fYPos = MENU_BOX_YPOS;
	pAVLayerObj->m_MenuBox.m_fXSize = MENU_BOX_XSIZE;
	pAVLayerObj->m_MenuBox.m_fYSize = MENU_BOX_YSIZE;

	TextBox2D_t TextBox;
	TextBox.m_fXPos = MENU_ITEM_XPOS;
	TextBox.m_fYPos = MENU_ITEM_YPOS;
	TextBox.m_fXSize = MENU_ITEM_XSIZE;
	TextBox.m_fYSize = MENU_ITEM_YSIZE;
	TextBox.m_Text = "Easy";
	pAVLayerObj->m_MenuBox.PushBackChoice( TextBox );
	TextBox.m_fYPos += MENU_ITEM_YINC;
	TextBox.m_Text = "Medium";
	pAVLayerObj->m_MenuBox.PushBackChoice( TextBox );
	TextBox.m_fYPos += MENU_ITEM_YINC;
	TextBox.m_Text = "Hard";
	pAVLayerObj->m_MenuBox.PushBackChoice( TextBox );
}

