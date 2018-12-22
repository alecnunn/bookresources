/******************************************************************************

  Copyright (C) 1999, 2000 NVIDIA Corporation
  This file is provided without support, instruction, or implied warranty of any
  kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
  not liable under any circumstances for any damages or loss whatsoever arising
  from the use or inability to use this file or items derived from it.
  
    Comments:
    
      
        
******************************************************************************/

#ifndef __EB_TIMER_H
#define __EB_TIMER_H
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
typedef unsigned __int64 UINT64;

class EBTimer
{
public:
	EBTimer()
	{
		if(!QueryPerformanceFrequency((LARGE_INTEGER *) &m_qwTicksPerSec))
			m_qwTicksPerSec = 1000;
		m_fTicksPerSec  = (float) (__int64) m_qwTicksPerSec;
	}

	virtual void Start(float fFramesPerSec)
	{
		m_fLastInterval = 0.0f;
		m_fLastTime = 0.0f;
		m_dwFrames = 0;
		m_fAverageFramesPerSec = 0.0f;
		m_fFramesPerSec = fFramesPerSec;
		m_fSecsPerFrame = 1.0f / fFramesPerSec;
		m_qwTicksPerFrame = m_qwTicksPerSec / (__int64) fFramesPerSec;
		m_qwStartTicks = m_qwTicks = GetTicks();
	}

    virtual void Frame()
	{
	    UINT64 qw;
		qw = GetTicks();
	    if(qw != m_qwTicks)
		{
			m_qwTicksPerFrame = qw - m_qwTicks;
			m_qwTicks = qw;
			m_fFramesPerSec = 0.75f * m_fFramesPerSec + 0.25f * (m_fTicksPerSec / (float) (__int64) m_qwTicksPerFrame);
	        if(m_fFramesPerSec < 1.0f)
				m_fFramesPerSec = 1.0f;
	        m_fSecsPerFrame = 1.0f / m_fFramesPerSec;
		}

	    m_dwFrames++;

		float fTime = GetDuration();
		m_fInterval = fTime - m_fLastInterval;
		m_fLastInterval = fTime;

		// Update the scene stats once per second
		if((fTime - m_fLastTime) > 1.5f )
		{
			m_fAverageFramesPerSec = m_dwFrames / (fTime - m_fLastTime);
			m_fLastTime = fTime;
			m_dwFrames = 0;
		}
	}
	
	virtual UINT64 GetTicks()
	{
		UINT64 qw;

		if(QueryPerformanceCounter((LARGE_INTEGER *) &qw))
		{
			while(!qw)
				QueryPerformanceCounter((LARGE_INTEGER *) &qw);
		} 
		else
		{
			qw = (UINT64) timeGetTime();
		}
		return qw;
	}
	float GetDuration()
	{
		INT64 qwDuration = (INT64)(GetTicks() - m_qwStartTicks);
		float fDuration = (float)qwDuration / (float)(INT64)m_qwTicksPerSec;
		return fDuration;
	}

	virtual float GetInterval() const { return m_fInterval; }
    virtual float GetFramesPerSec() const { return m_fFramesPerSec; }
	virtual float GetAverageFramesPerSec() const { return m_fAverageFramesPerSec; }
    virtual float GetSecsPerFrame() const { return m_fSecsPerFrame; }
    virtual UINT64 GetTicksPerSec() const { return m_qwTicksPerSec; }
    virtual UINT64 GetTicksPerFrame() const { return m_qwTicksPerFrame; }

protected:
    float m_fTicksPerSec;
    float m_fFramesPerSec;
	float m_fAverageFramesPerSec;
    float m_fSecsPerFrame;
	DWORD m_dwFrames;
	float m_fLastTime;
	float m_fInterval;
	float m_fLastInterval;

    UINT64 m_qwTicks;
	UINT64 m_qwStartTicks;
    UINT64 m_qwTicksPerSec;
    UINT64 m_qwTicksPerFrame;
};

#endif __EB_TIMER_H