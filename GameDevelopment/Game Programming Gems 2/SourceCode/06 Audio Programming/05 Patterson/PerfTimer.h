/* Copyright (C) Scott Patterson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2001"
 */

// PerfTimer.h

#ifndef PERFTIMER_H
#define PERFTIMER_H

class PerfTimer_t {
public:
	PerfTimer_t();
	~PerfTimer_t();

	// get the time in seconds
	float         GetTime();

private:
	double        m_fTime;

	LARGE_INTEGER m_StartCount;
	LARGE_INTEGER m_Frequency;
	double        m_fResolution;
	int	          m_nLowshift;
	bool          m_bPerformanceTimerEnabled;
};

#endif