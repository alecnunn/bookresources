/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerObj_GameViewer_t.cpp: implementation of the AVLayerObj_GameViewer_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVLayerObj_GameViewer_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/VisualSys_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVLayerObj_GameViewer_t::AVLayerObj_GameViewer_t()
{
	m_pSystems = 0;
}

AVLayerObj_GameViewer_t::~AVLayerObj_GameViewer_t()
{

}

bool AVLayerObj_GameViewer_t::Connect( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void AVLayerObj_GameViewer_t::Disconnect()
{
}

void AVLayerObj_GameViewer_t::Update()
{
}

void AVLayerObj_GameViewer_t::RenderVisual()
{
	m_pSystems->GetVisualSys()->RenderTextBox( &m_ModeTextBox );
	m_pSystems->GetVisualSys()->RenderTextBox( &m_InfoTextBox );
}

void AVLayerObj_GameViewer_t::RenderAudio()
{
}

