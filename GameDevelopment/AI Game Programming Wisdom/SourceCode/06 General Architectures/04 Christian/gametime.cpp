

#include "gametime.h"
#include <stdio.h>

double GameTime::dt = 0.016f;

GameTime::GameTime ()
{
}

void GameTime::start ()
{
    m_startTime = clock();
}

void GameTime::end ()
{
    dt = 0.0f;

    // force dt to be slow for our text game
    while ( dt < 0.3f )
    {
        m_endTime = clock();

        dt = (double)(m_endTime - m_startTime) / CLOCKS_PER_SEC;
    }
}

