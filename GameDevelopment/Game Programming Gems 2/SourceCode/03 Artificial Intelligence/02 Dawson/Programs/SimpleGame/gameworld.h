/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#ifndef	GAMEWORLD_H
#define	GAMEWORLD_H

#include "core/randomnumbers.h"
#include "gamelibrary/gameinput.h"

const int k_viewWidth = 400;
const int k_viewHeight = 300;
const int k_viewDepth = 3;	// Number of bytes per pixel.
const int k_updateFrameRate = 20;

class CSBitmap;

class GameWorld;

// Random number function for the game update loop.
// Do not use from the rendering code.
int UpdateLoopRand();

GameWorld& GetGameWorld();

class GameWorld
{
public:
	int GetFrameNum() const {return m_frameNum;}
	// It is important that the game threads always use GetGameTime()
	// rather than asking the OS or C run time for the current time -
	// otherwise your game results will be indeterminate.
	double GetGameTime() const {return m_frameNum / double(k_updateFrameRate);}

	void ProcessFrame();
	void RenderFrame();
	const CSBitmap* GetBitmap() const {return m_bitmap;}
	// Don't use this random number generator in the draw loop - only in
	// the update loop.
	const RandomNumbers& GetRandomGenerator() const {return m_random;}
	void SetOffset(int xOffset, int yOffset);
	void StartRecordingInput(HWND hWnd);

	int GetWidth() const;
	int GetHeight() const;

private:
	GameWorld();	// Make private - so this is a singleton class.
	friend GameWorld& GetGameWorld();
	// At ~60fps we will wrap our frame counter after about one year.
	// I'm not going to worry about it.
	int			m_frameNum;
	CSBitmap*	m_bitmap;
	RandomNumbers m_random;
};

#endif
