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

#include "gamelibrary/objectfactories.h"
#include "imagelibrary/csbitmap.h"
#include "gamelibrary/resourcemanager.h"
#include "gameworld.h"
#include "math.h"
#include "gamelibrary/microthreads.h"
using namespace MicroThread;

// This game object is implemented _without_ micro threads.
class RotatingObject : public GameObject
{
public:
	RotatingObject()
	{
		m_pSprite = LoadBitmapImage("blue piece.bmp");
		m_motionRadius = 60;
		m_motionCenterX = m_motionRadius + UpdateLoopRand() % (k_viewWidth - m_motionRadius * 2 - m_pSprite->GetWidth());
		m_motionCenterY = m_motionRadius + UpdateLoopRand() % (k_viewHeight - m_motionRadius * 2 - m_pSprite->GetHeight());
		m_theta = 0;
		m_dTheta = 1.5 / k_updateFrameRate;	// Approximately 90 degrees per second.
		SetPosition();
	}
	inline void SetPosition()
	{
		m_x = m_motionCenterX + cos(m_theta) * m_motionRadius;
		m_y = m_motionCenterY + sin(m_theta) * m_motionRadius;
	}
	virtual void ProcessFrame()
	{
		m_theta += m_dTheta;
		SetPosition();
	}
private:
	double m_motionCenterX, m_motionCenterY;
	int m_motionRadius;
	double m_vX, m_vY;	// Velocity
	double m_theta, m_dTheta;
};

DECLARE_OBJECT_FACTORY(RotatingObject);



// This game object is implemented _without_ micro threads.
class SimpleBouncer : public GameObject
{
public:
	SimpleBouncer()
	{
		m_pSprite = LoadBitmapImage("blue piece.bmp");
		m_x = UpdateLoopRand() % (k_viewWidth - m_pSprite->GetWidth());;
		m_y = UpdateLoopRand() % (k_viewHeight - m_pSprite->GetHeight());
		m_dX = 200.0 / k_updateFrameRate;
	}
	virtual void ProcessFrame()
	{
		// Our sprite wants to move on a line from m_x, m_y
		// to m_x + m_vX, m_y + m_vY. If it hits one of the
		// edges then we need to... ah, fuck it.
		m_x += m_dX;
		bool Clipped = true;
		const int maxX = k_viewWidth - m_pSprite->GetWidth();
		while (Clipped)
		{
			Clipped = false;
			if (m_x < 0)
			{
				m_x = -m_x;		// Bounce back with remaining motion.
				m_dX = -m_dX;	// Reverse direction.
				Clipped = true;
			}
			if (m_x > maxX)
			{
				m_x = maxX - (m_x - maxX);	// Bounce back with remaining motion.
				m_dX = -m_dX;	// Reverse direction.
				Clipped = true;
			}
		}
	}
private:
	double m_dX;
};

DECLARE_OBJECT_FACTORY(SimpleBouncer);




// This game object is implemented using micro threads.
class MicroThreadTest : public GameObject
{
public:
	MicroThreadTest()
	{
		m_pSprite = LoadBitmapImage("red piece.bmp");
		m_x = 40;
		m_y = UpdateLoopRand() % (k_viewHeight - m_pSprite->GetHeight());
	}
	void MicroThreadProcess()
	{
		MicroThreadSleep();
		double baseDX = 200.0 / k_updateFrameRate;
		double sign = 1;
		const int maxX = k_viewWidth - m_pSprite->GetWidth();
		while (true)
		{
			double dX = sign * baseDX * (0.2 + sin(m_x * 3.1415926535 / (k_viewWidth - m_pSprite->GetWidth())));
			m_x += dX;
			bool Clipped = true;
			while (Clipped)
			{
				Clipped = false;
				if (m_x < 0)
				{
					m_x = -m_x;		// Bounce back with remaining motion.
					dX = -dX;	// Reverse direction.
					Clipped = true;
				}
				if (m_x > maxX)
				{
					m_x = maxX - (m_x - maxX);	// Bounce back with remaining motion.
					dX = -dX;	// Reverse direction.
					Clipped = true;
				}
			}
			sign = 1;
			if (dX < 0)
				sign = -1;
			MicroThreadSleep();
		}
	}
private:
};

DECLARE_OBJECT_FACTORY(MicroThreadTest);




// This game object is implemented using micro threads.
class MouseFollower : public GameObject
{
public:
	MouseFollower()
	{
		m_pSprite = LoadBitmapImage("red piece.bmp");
		m_x = 40;
		m_y = UpdateLoopRand() % (k_viewHeight - m_pSprite->GetHeight());
	}
	void MicroThreadProcess()
	{
		GameWorld& world = GetGameWorld();
		const GameInput& input = GameInputManager::GetSingleton().GetCurrentInput();
		MicroThreadSleep();
		while (true)
		{
			int x, y;
			input.GetMousePos(x, y);
			x -= m_pSprite->GetWidth() / 2;
			y -= m_pSprite->GetHeight() / 2;
			if (x < 0)
				x = 0;
			if (y < 0)
				y = 0;
			const int maxX = world.GetWidth() - m_pSprite->GetWidth();
			if (x > maxX)
				x = maxX;
			const int maxY = world.GetHeight() - m_pSprite->GetHeight();
			if (y > maxY)
				y = maxY;
			m_x = x;
			m_y = y;
			MicroThreadSleep();
		}
	}
private:
};

DECLARE_OBJECT_FACTORY(MouseFollower);




// This game object is implemented using micro threads.
class LaggingMouseFollower : public GameObject
{
public:
	LaggingMouseFollower()
	{
		m_pSprite = LoadBitmapImage("red piece.bmp");
		m_x = 40;
		m_y = UpdateLoopRand() % (k_viewHeight - m_pSprite->GetHeight());
	}
	void MicroThreadProcess()
	{
		GameWorld& world = GetGameWorld();
		const GameInput& input = GameInputManager::GetSingleton().GetCurrentInput();
		MicroThreadSleep();
		while (true)
		{
			if (input.GetKeyEvent(VK_ESCAPE) > 0)
			{
				// Commit suicide. I hope that's legal...
				GameObject::DestroyObject(this);
				return;	// Terminate the micro thread - optional.
			}

			int x, y;
			input.GetMousePos(x, y);
			x -= m_pSprite->GetWidth() / 2;
			y -= m_pSprite->GetHeight() / 2;
			if (x < 0)
				x = 0;
			if (y < 0)
				y = 0;
			const int maxX = world.GetWidth() - m_pSprite->GetWidth();
			if (x > maxX)
				x = maxX;
			const int maxY = world.GetHeight() - m_pSprite->GetHeight();
			if (y > maxY)
				y = maxY;
			double deltaX = x - m_x;
			double deltaY = y - m_y;
			double distance = sqrt(deltaX * deltaX + deltaY * deltaY);
			const double maxDistance = 10;
			dprintf("Moving from %d, %d to %d, %d - distance = %1.3f\n", int(m_x), int(m_y), x, y, distance);
			if (distance > maxDistance)
			{
				double ratio = maxDistance / distance;
				dprintf("ratio = %1.3f\n", ratio);
				deltaX *= ratio;
				deltaY *= ratio;
			}
			m_x += deltaX;
			m_y += deltaY;
			MicroThreadSleep();
		}
	}
private:
};

DECLARE_OBJECT_FACTORY(LaggingMouseFollower);




// This game object is implemented using micro threads.
class PolyLineObject : public GameObject
{
public:
	PolyLineObject()
	{
		m_numEntries = 0;
	}
	void MicroThreadProcess()
	{
		GameWorld& world = GetGameWorld();
		const GameInput& input = GameInputManager::GetSingleton().GetCurrentInput();
		MicroThreadSleep();
		while (true)
		{
			int x, y;
			input.GetMousePos(x, y);
			// This check gets rid of redundant entries, which means that the tail of the
			// polyline only chases you if you keep moving.
			if (m_numEntries == 0 || m_xData[m_numEntries-1] != x || m_yData[m_numEntries-1] != y)
			{
				if (m_numEntries == NUMELEMENTS(m_xData))
				{
					--m_numEntries;
					// Which is better - a rotating list implemented by tracking the start
					// and end points (or just a count that is implicitly interpreted
					// modulo NUMELEMENTS) or shifting the data down when the array fills
					// up. Explicitly shifting the data will get expensive with long lists,
					// but it sure is simple...
					memmove(m_xData, m_xData + 1, sizeof(m_xData) - sizeof(m_xData[0]));
					memmove(m_yData, m_yData + 1, sizeof(m_yData) - sizeof(m_yData[0]));
				}
				m_xData[m_numEntries] = x;
				m_yData[m_numEntries] = y;
				++m_numEntries;
			}
			MicroThreadSleep();
		}
	}
	void RenderSelf(CSBitmap* pBitmap)
	{
		HDC hdc = pBitmap->GetDrawSurface();
		for (int i = 0; i < m_numEntries; ++i)
		{
			// Could use PolyLine I suppose. Oh well.
			if (i == 0)
				MoveToEx(hdc, m_xData[i], m_yData[i], 0);
			else
				LineTo(hdc, m_xData[i], m_yData[i]);
		}
	}
private:
	enum { k_numEntries = 100 };
	int	m_xData[k_numEntries];
	int	m_yData[k_numEntries];
	int m_numEntries;
};

DECLARE_OBJECT_FACTORY(PolyLineObject);
