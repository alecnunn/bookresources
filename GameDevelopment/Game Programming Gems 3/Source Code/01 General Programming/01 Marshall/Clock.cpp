/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Game Scheduler Gem sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
* Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer. 
* Redistributions in binary form must reproduce the above copyright notice, this 
list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution. 
* Neither the name of Intel Corporation nor the names of its contributors may be 
used to endorse or promote products derived from this software without specific 
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS'' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR 
JURISDICTION. It is each licensee's responsibility to comply with any export 
regulations applicable in each licensee's jurisdiction. 
*/

#include <windows.h>

#include "Clock.h"

#include <stdio.h>

//
// This class manages time for the scheduler
//
// For clarity, this keeps tracks of both real time and virtual time.
//
// System time ....... real time (zero-based)
// Simulation time ... virtual time (zero-based)
// Frame ............. current frame


Clock::Clock()
{
	timeBeginPeriod(1); // set win32 time resolution
	Reset();
}

Clock::~Clock()
{
	timeEndPeriod(1); // release win32 time resolution
}

void Clock::Reset()
{
	//
	// reset clock to startup conditions
	//
	m_running = FALSE;

	m_thisTime = timeGetTime();
	m_lastTime = m_thisTime;

	m_systemTime = 0;
	m_pauseTime = 0;
	m_systemOffset = m_thisTime;

	m_frameCount = 0;
	m_frameStart = 0;
	m_frameEnd = 0;

	m_simTime = 0;
	m_simOffset = 0;
}

void Clock::Run()
{
	if (!m_running)
	{
		Update();
		m_simOffset += (m_systemTime - m_pauseTime);
		printf("Clock: started, %u ms elapsed since last stop\n", m_systemTime - m_pauseTime);
	}
	m_running = TRUE;
}
void Clock::Stop()
{
	if (m_running)
	{
		Update();
		m_pauseTime = m_systemTime;
		printf("Clock: stopped, paused at %u ms\n", m_pauseTime);
	}
	m_running = FALSE;
}

void Clock::Update()
{
	// get windows' idea of current time
	unsigned elapsed = 0;
	m_lastTime = m_thisTime;
	m_thisTime = timeGetTime();

	// convert to elapsed time
	// also handle rollover, which happens every 2^32 milliseconds
	if (m_thisTime < m_lastTime)
		elapsed = m_lastTime - m_thisTime;
	else
		elapsed = m_thisTime - m_lastTime;

	// system time is real time and never pauses
	m_systemTime += elapsed;
//	printf("<CLOCK> update, system=%u, elapsed=%u\n", m_systemTime, elapsed);
}

void Clock::BeginFrame()
{
	// Begin a new frame. This method is normally called
	// immediately AFTER rendering the previous frame, and
	// sets the context for all work which will be rendered
	// on the next frame.
	//
	// This method increments the frame count and samples real
	// time to determine the elapsed time since the last frame.
	//
	// <Render frame n>
	// <Begin frame n+1>
	// <Task1>
	// <Task2>
	// <Task3...>
	// <Render frame n+1>
	//

	m_frameCount++;

	Update(); // update system time


	if (m_running)
	{
		m_frameStart = m_frameEnd; // start of this frame = end of last frame
		m_frameEnd = m_systemTime - m_simOffset; // end of this frame = current time
		m_simTime = m_frameStart; // set simulation time to start of frame
	}
	printf("Clock: begin frame %u (begin %u, end %u), sys=%u, sim=%u (%u)\n", m_frameCount, m_frameStart, m_frameEnd, m_systemTime, m_simTime, m_simOffset);
}

void Clock::AdvanceTo(unsigned newTime)
{
	if (m_running && newTime >= m_simTime)
	{
		m_simTime = newTime;
//		printf("<CLOCK> advance to %u\n", m_simTime);
	}
}

void Clock::AdvanceToEnd()
{
	if (m_running)
	{
		m_simTime = m_frameEnd;
//		printf("<CLOCK> advance to %u (end)\n", m_simTime);
	}
}



