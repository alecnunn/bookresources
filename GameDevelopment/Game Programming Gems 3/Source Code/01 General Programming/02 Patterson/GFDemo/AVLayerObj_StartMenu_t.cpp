/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerObj_StartMenu_t.cpp: implementation of the AVLayerObj_StartMenu_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVLayerObj_StartMenu_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/VisualSys_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVLayerObj_StartMenu_t::AVLayerObj_StartMenu_t()
{
	m_pSystems = 0;
}

AVLayerObj_StartMenu_t::~AVLayerObj_StartMenu_t()
{

}

bool AVLayerObj_StartMenu_t::Connect( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void AVLayerObj_StartMenu_t::Disconnect()
{
}

void AVLayerObj_StartMenu_t::Update()
{
}

void AVLayerObj_StartMenu_t::RenderVisual()
{
	m_pSystems->GetVisualSys()->RenderTextBox( &m_ModeTextBox );
	m_pSystems->GetVisualSys()->RenderTextBox( &m_InfoTextBox );
	m_pSystems->GetVisualSys()->RenderMenuBox( &m_MenuBox );
}

void AVLayerObj_StartMenu_t::RenderAudio()
{
}

