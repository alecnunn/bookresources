/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#include "stdafx.h"
#include "gameworld.h"
#include "imagelibrary/csbitmap.h"
#include "gamelibrary/objectfactories.h"

#include <vector>
using namespace std;

GameWorld::GameWorld()
	: m_frameNum(-1), m_bitmap(new CSBitmap)
{
	m_bitmap->SetSize(k_viewWidth, k_viewHeight, k_viewDepth);
	// Make sure the bitmap starts out in a good state.
	RECT drawRect = {0, 0, m_bitmap->GetWidth(), m_bitmap->GetHeight()};
	FillRect(m_bitmap->GetDrawSurface(), &drawRect, (HBRUSH)GetStockObject(WHITE_BRUSH));

	CreateGameObject("RotatingObject");
	CreateGameObject("SimpleBouncer");
	//CreateGameObject("MicroThreadJanitor");
}

void GameWorld::ProcessFrame()
{
	// This frame number should really be stored in the input object.
	// I'll move it there some day...
	++m_frameNum;

	const vector<GameObject*>& gameObjects = GetGameObjects();
	const int numObjects = gameObjects.size();
	for (int i = 0; i < numObjects; ++i)
	{
		gameObjects[i]->ProcessFrame();
		gameObjects[i]->RunThread();
	}
}

void GameWorld::RenderFrame()
{
	const vector<GameObject*>& gameObjects = GetGameObjects();
	const int numObjects = gameObjects.size();

	// Initialize the bitmap to some background image.
	RECT drawRect = {0, 0, m_bitmap->GetWidth(), m_bitmap->GetHeight()};
	FillRect(m_bitmap->GetDrawSurface(), &drawRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	for (int i = 0; i < numObjects; ++i)
	{
		gameObjects[i]->RenderSelf(m_bitmap);
	}

	char	textBuffer[50];
	sprintf(textBuffer, "frame %d", m_frameNum);
	DrawText(m_bitmap->GetDrawSurface(), textBuffer, -1, &drawRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void GameWorld::SetOffset(int xOffset, int yOffset)
{
	GameInputManager::GetSingleton().SetOffset(xOffset, yOffset);
}

void GameWorld::StartRecordingInput(HWND hWnd)
{
	GameInputManager::GetSingleton().StartRecordingInput(hWnd, k_updateFrameRate);
}

int GameWorld::GetWidth() const
{
	return m_bitmap->GetWidth();
}

int GameWorld::GetHeight() const
{
	return m_bitmap->GetHeight();
}

GameWorld& GetGameWorld()
{
	// Declared in this function to ensure a particular order of initialization -
	// this makes it impossible to use it before it is initialized.
	static GameWorld s_gameWorld;
	return s_gameWorld;
}

int UpdateLoopRand()
{
	// I know - too many 
	static const RandomNumbers& s_random = GetGameWorld().GetRandomGenerator();
	return s_random.GetRandom();
}
