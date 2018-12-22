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

#ifndef _Clock_H_
#define _Clock_H_

class Clock {
	int m_running;

	unsigned m_thisTime;
	unsigned m_lastTime;

	unsigned m_systemTime;
	unsigned m_systemOffset;
	unsigned m_pauseTime;

	unsigned m_frameCount;
	unsigned m_frameStart;
	unsigned m_frameEnd;

	unsigned m_simTime;
	unsigned m_simOffset;

public:

	Clock();
	~Clock();

	void Reset();
	int IsRunning() { return m_running; }
	void Run();
	void Stop();

	void Update();
	void BeginFrame();

	unsigned GetSystem() { Update(); return m_systemTime; }
	unsigned GetTime() { return m_simTime; }
	unsigned GetFrame() { return m_frameCount; }
	unsigned GetFrameStart() { return m_frameStart; }
	unsigned GetFrameEnd() { return m_frameEnd; }

	void AdvanceTo(unsigned newTime);
	void AdvanceToEnd();
};

#endif

