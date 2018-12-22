
#include "anim.h"
#include "gametime.h"
#include <stdio.h>

Anim::Anim ( float time, bool loop )
{
    m_time  = time;
    m_clock = 0.0f;
    m_loop  = loop;
}

void Anim::start ()
{
    m_clock = 0.0f;
}

bool Anim::update ()
{
    m_clock += GameTime::dt;

    if ( m_loop && m_clock >= m_time )
        m_clock = 0.0f;

    return done();
}

bool Anim::done ()
{
    if ( m_clock >= m_time && ! m_loop )
    {
        return true;
    }
    else
        return false;
}