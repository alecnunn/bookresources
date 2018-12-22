/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// AudioSysDSoundImpl_t.cpp: implementation of the AudioSysDSoundImpl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Win32/AudioSysDSoundImpl_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AudioSysDSoundImpl_t::AudioSysDSoundImpl_t()
{

}

AudioSysDSoundImpl_t::~AudioSysDSoundImpl_t()
{

}

bool AudioSysDSoundImpl_t::Init( Systems_t *pSystems )
{
	return true;
}

void AudioSysDSoundImpl_t::Shutdown()
{
}

bool AudioSysDSoundImpl_t::BeginRender()
{
	return true;
}

void AudioSysDSoundImpl_t::EndRender()
{
}

