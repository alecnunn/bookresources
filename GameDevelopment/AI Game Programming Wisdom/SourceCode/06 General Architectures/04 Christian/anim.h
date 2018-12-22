
#ifndef _ANIM_H_
#define _ANIM_H_

class Anim
{

public:
    Anim( float time, bool loop = false );

    void start   ();
    bool update  ();
    bool done    ();

private:

    float  m_time;
    double m_clock;
    float  m_loop;
};

#endif