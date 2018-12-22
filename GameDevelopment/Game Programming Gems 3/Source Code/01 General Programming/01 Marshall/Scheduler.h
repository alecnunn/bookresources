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

#ifndef _Scheduler_H_
#define _Scheduler_H_

#include "ITask.h"
#include "Clock.h"

enum TaskType { TASK_TIME, TASK_FRAME, TASK_RENDER };
enum TaskState { TASK_ACTIVE, TASK_PAUSE, TASK_DELETE };

struct TaskInfo
{
	ITask * pTask;
	TaskInfo * pNext;
	int status; // 0 = ok, 1 = pause, 2 = delete
	int id;
	void * pUser;

	struct {
		unsigned start;
		unsigned period;
		unsigned duration;

		unsigned last;
		unsigned next;
	} time;
};



class Scheduler
{
	TaskInfo renderTask;
	TaskInfo * m_pTaskList;
	TaskInfo * m_pFrameList;
	int m_nextId;
	Clock m_clock;

public:

	Scheduler();
	~Scheduler();

	int IsRunning() { return m_clock.IsRunning(); }
	void Run() { m_clock.Run(); }
	void Stop() { m_clock.Stop(); }

	int ExecuteFrame();

	void InsertTimeTask(TaskInfo *pTaskInfo);
	void InsertFrameTask(TaskInfo *pTaskInfo);
	void GetNextTimeTask(TaskInfo **ppTaskInfo);
	void GetNextFrameTask(TaskInfo **ppTaskInfo);

	int Schedule(TaskType type, int start, int period, int duration, ITask * pTask, void * pUser, int *pID);

	void Terminate(int id);
	void SweepGarbage();

	Clock& GetClock()
	{
		return m_clock;
	}
};

#endif


