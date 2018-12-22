/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// PerfTimer.cpp

#include "StdAfx.h"
#include "PerfTimer.h"
#include "mmsystem.h"

PerfTimer_t::PerfTimer_t()
{
	m_fTime = 0.0f;

	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &m_Frequency)) {
		m_fResolution = 0.001f;
		m_bPerformanceTimerEnabled = false;
	} else { 
		m_bPerformanceTimerEnabled = true;

		// shift to a maximum resolution needed
		// in this example we will be accurate up to 1 microsecond
		__int64 nShift = m_Frequency.QuadPart;
		m_nLowshift = 0;
		while (nShift > 1000000)
		{
			m_nLowshift++;
			nShift >>= 1;
		}
		m_fResolution = 1.0/(double)nShift;

		QueryPerformanceCounter((LARGE_INTEGER *) &m_StartCount);
	}
}

PerfTimer_t::~PerfTimer_t()
{
}

float PerfTimer_t::GetTime()
{
	LARGE_INTEGER Count;
	__int64 nTemp;

	if(m_bPerformanceTimerEnabled)
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &Count);

		Count.QuadPart -= m_StartCount.QuadPart;

		nTemp = Count.QuadPart >> m_nLowshift;  

		m_fTime = (double)nTemp * m_fResolution;
	} else {
		m_fTime= timeGetTime() * m_fResolution;
	}
	return (float)m_fTime;
}