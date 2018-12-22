/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerConfigObj_Intro_t.cpp: implementation of the AVLayerConfigObj_Intro_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVLayerConfigObj_Intro_t.h"
#include "AVLayerObj_Intro_t.h"
#include "GameConstants.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVLayerConfigObj_Intro_t::AVLayerConfigObj_Intro_t()
{

}

AVLayerConfigObj_Intro_t::~AVLayerConfigObj_Intro_t()
{

}

void AVLayerConfigObj_Intro_t::Configure( AVLayer_t *pAVLayer )
{
	AVLayerObj_Intro_t *pAVLayerObj = (AVLayerObj_Intro_t *)pAVLayer;

	pAVLayerObj->m_ModeTextBox.m_fXPos = MODE_BOX_XPOS;
	pAVLayerObj->m_ModeTextBox.m_fYPos = MODE_BOX_YPOS;
	pAVLayerObj->m_ModeTextBox.m_fXSize = MODE_BOX_XSIZE;
	pAVLayerObj->m_ModeTextBox.m_fYSize = MODE_BOX_YSIZE;
	pAVLayerObj->m_ModeTextBox.m_Text = "Intro";

	pAVLayerObj->m_InfoTextBox.m_fXPos = INFO_BOX_XPOS;
	pAVLayerObj->m_InfoTextBox.m_fYPos = INFO_BOX_YPOS;
	pAVLayerObj->m_InfoTextBox.m_fXSize = INFO_BOX_XSIZE;
	pAVLayerObj->m_InfoTextBox.m_fYSize = INFO_BOX_YSIZE;
	pAVLayerObj->m_InfoTextBox.m_Text = "Hit Enter key for start menu";
}
