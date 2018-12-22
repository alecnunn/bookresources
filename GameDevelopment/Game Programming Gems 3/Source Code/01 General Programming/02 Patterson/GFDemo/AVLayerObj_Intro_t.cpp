/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AVLayerObj_Intro_t.cpp: implementation of the AVLayerObj_Intro_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVLayerObj_Intro_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/VisualSys_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVLayerObj_Intro_t::AVLayerObj_Intro_t()
{
	m_pSystems = 0;
}

AVLayerObj_Intro_t::~AVLayerObj_Intro_t()
{

}

bool AVLayerObj_Intro_t::Connect( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void AVLayerObj_Intro_t::Disconnect()
{
}

void AVLayerObj_Intro_t::Update()
{
}

void AVLayerObj_Intro_t::RenderVisual()
{
	m_pSystems->GetVisualSys()->RenderTextBox( &m_ModeTextBox );
	m_pSystems->GetVisualSys()->RenderTextBox( &m_InfoTextBox );
	m_pSystems->GetVisualSys()->RenderStats();
}

void AVLayerObj_Intro_t::RenderAudio()
{
}

