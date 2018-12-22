/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerObj_GameEditor_t.cpp: implementation of the AVLayerObj_GameEditor_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVLayerObj_GameEditor_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/VisualSys_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVLayerObj_GameEditor_t::AVLayerObj_GameEditor_t()
{
	m_pSystems = 0;
}

AVLayerObj_GameEditor_t::~AVLayerObj_GameEditor_t()
{

}

bool AVLayerObj_GameEditor_t::Connect( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void AVLayerObj_GameEditor_t::Disconnect()
{
}

void AVLayerObj_GameEditor_t::Update()
{
}

void AVLayerObj_GameEditor_t::RenderVisual()
{
	m_pSystems->GetVisualSys()->RenderTextBox( &m_ModeTextBox );
	m_pSystems->GetVisualSys()->RenderTextBox( &m_InfoTextBox );
}

void AVLayerObj_GameEditor_t::RenderAudio()
{
}

