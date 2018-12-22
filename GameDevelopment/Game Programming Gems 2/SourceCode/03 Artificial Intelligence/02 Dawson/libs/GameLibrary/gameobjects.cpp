/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#include "priv_precompiled.h"

#include "gameobjects.h"
#include "imagelibrary/csbitmap.h"
#include "gamelibrary/microthreads.h"
using namespace MicroThread;

GameObject::GameObject()
	: m_pSprite(0), m_thread(0), m_x(0), m_y(0), m_deadFrames(0)
{
}

void GameObject::RenderSelf(CSBitmap* pBitmap)
{
	assert(pBitmap);
	assert(pBitmap->HasBitmap());
	if (m_pSprite)
		CompositeBitmap(pBitmap, m_pSprite, int(m_x), int(m_y));
}

static void MICROTHREADCALL RunMicroThread(void* pVoid)
{
	GameObject* pObject = (GameObject*)pVoid;
	pObject->MicroThreadProcess();
}

void GameObject::PostConstructorInit()
{
	m_thread = CreateMicroThread(RunMicroThread, this);
}

void GameObject::RunThread()
{
	SwitchToMicroThread(m_thread);
}

void GameObject::MicroThreadProcess()
{
	// Do nothing - just return.
}
