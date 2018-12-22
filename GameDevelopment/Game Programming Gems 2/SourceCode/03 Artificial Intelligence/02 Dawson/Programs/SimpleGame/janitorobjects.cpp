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
#include <vector>
using namespace std;
#include "gamelibrary/microthreads.h"
using namespace MicroThread;



// Helper functions for our two janitor classes.

const double k_collisionTolerance = 5;

static GameObject* GetNewTarget(GameObject* pNotMe)
{
    // Get the list of game objects.
    const vector<GameObject*>& objects = GetGameObjects();
    GameObject* pObject = 0;
    if (!objects.empty())
    {
        // Choose a random game object.
        pObject = objects[UpdateLoopRand() % objects.size()];
    }
    if (!pObject || pObject == pNotMe)
        pObject = CreateGameObject(GetObjectTypes()[UpdateLoopRand() % GetNumObjectTypes()]);

    return pObject;
}

static double Distance(GameObject* mover, GameObject* target)
{
    double dx = mover->m_x - target->m_x;
    double dy = mover->m_y - target->m_y;
    return sqrt(dx * dx + dy * dy);
}

static void MoveABitTowards(GameObject* mover, GameObject* target, double slerpAmount = 0.1)
{
    double deltax = (target->m_x - mover->m_x) * slerpAmount;
    double deltay = (target->m_y - mover->m_y) * slerpAmount;
    // If we just add deltax and deltay then we may never catch up because
    // the closer we get, the slower we move.
    double length = sqrt(deltax * deltax + deltay * deltay);
    const double k_accelerationTolerance = 7;
    if (length < k_accelerationTolerance)
    {
        double ratio = k_accelerationTolerance / length;
        deltax *= ratio;
        deltay *= ratio;
    }
    mover->m_x = mover->m_x + deltax;
    mover->m_y = mover->m_y + deltay;
}

static void Dispose(GameObject* user, GameObject* used)
{
    GameObject::DestroyObject(used);
}

inline void WaitOneFrame()
{
    MicroThreadSleep();
}



// Janitor object, implemented with micro threads.

class MicroThreadJanitor : public GameObject
{
public:
    MicroThreadJanitor()
    {
        m_pSprite = LoadBitmapImage("red piece.bmp");
        m_x = UpdateLoopRand() % (k_viewWidth - m_pSprite->GetWidth());;
        m_y = UpdateLoopRand() % (k_viewHeight - m_pSprite->GetHeight());
    }
    void MicroThreadProcess();

private:
};

void MicroThreadJanitor::MicroThreadProcess()
{
    while (true)
    {
        GameObject* target = GetNewTarget(this);
        while (Distance(this, target) > k_collisionTolerance)
        {
            WaitOneFrame();
            // Our target might be killed by somebody else, so
            // we have to check for this. This code was omitted
			// from the article text for simplicity.
            if (target->IsDead())
            {
                target = 0;
                break;
            }
            MoveABitTowards(this, target);
        }
        if (target)
            Dispose(this, target);
    }
}

DECLARE_OBJECT_FACTORY(MicroThreadJanitor);




// Janitor object, implemented with a virtual function callback.

enum EJanitorStates
{
    k_NeedsGoal,
    k_MovingTowardsTarget,
    k_DisposeObject
};

class Janitor : public GameObject
{
public:
    Janitor();
    void ProcessFrame();
private:
    GameObject*     m_target;
    EJanitorStates    m_state;
};

Janitor::Janitor()
: m_target(0), m_state(k_NeedsGoal)
{
    m_pSprite = LoadBitmapImage("red piece.bmp");
    m_x = UpdateLoopRand() % (k_viewWidth - m_pSprite->GetWidth());;
    m_y = UpdateLoopRand() % (k_viewHeight - m_pSprite->GetHeight());
}

void Janitor::ProcessFrame()
{
    switch (m_state)
    {
        case k_NeedsGoal:
            m_target = GetNewTarget(this);
            if (Distance(this, m_target) <= k_collisionTolerance)
            {
                m_state = k_DisposeObject;
                ProcessFrame();    // Call ourselves.
                return;
            }
            m_state = k_MovingTowardsTarget;
            // Intentionally missing break.
        case k_MovingTowardsTarget:
            // Our target might be killed by somebody else, so
            // we have to check for this. This code was omitted
			// from the article text for simplicity.
            if (m_target->IsDead())
            {
                m_state = k_NeedsGoal;
                break;
            }
            MoveABitTowards(this, m_target);
            if (Distance(this, m_target) > k_collisionTolerance)
            {
                break;
            }
            else
            {
                m_state = k_DisposeObject;
                // Intentionally missing break.
            }
        case k_DisposeObject:
            Dispose(this, m_target);
            m_state = k_NeedsGoal; 
            break;
    }
}

DECLARE_OBJECT_FACTORY(Janitor);
