/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfigObj_GameHud_t.cpp: implementation of the AVLayerConfigObj_GameHud_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVLayerConfigObj_GameHud_t.h"
#include "AVLayerObj_GameHud_t.h"
#include "GameConstants.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVLayerConfigObj_GameHud_t::AVLayerConfigObj_GameHud_t()
{

}

AVLayerConfigObj_GameHud_t::~AVLayerConfigObj_GameHud_t()
{

}

void AVLayerConfigObj_GameHud_t::Configure( AVLayer_t *pAVLayer )
{
	AVLayerObj_GameHud_t *pAVLayerObj = (AVLayerObj_GameHud_t *)pAVLayer;

	pAVLayerObj->m_ModeTextBox.m_fXPos = MODE_BOX_XPOS+MODE_BOX_XINC;
	pAVLayerObj->m_ModeTextBox.m_fYPos = MODE_BOX_YPOS-MODE_BOX_YINC;
	pAVLayerObj->m_ModeTextBox.m_fXSize = MODE_BOX_XSIZE;
	pAVLayerObj->m_ModeTextBox.m_fYSize = MODE_BOX_YSIZE;
	pAVLayerObj->m_ModeTextBox.m_Text = "GameHud";
}
