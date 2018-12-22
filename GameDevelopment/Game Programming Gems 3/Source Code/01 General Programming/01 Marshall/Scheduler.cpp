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

#include "Scheduler.h"
#include "ITask.h"

#include <stdio.h>

#define TRUE 1
#define FALSE 0

#define RENDER_TASK_ID 1

Scheduler::Scheduler()
{
	m_pTaskList = NULL;
	m_pFrameList = NULL;
	renderTask.pTask = NULL;
	m_nextId = RENDER_TASK_ID + 1;
}

Scheduler::~Scheduler()
{
	// release our allocated memory
	TaskInfo * pTaskInfo = m_pTaskList;
	while (pTaskInfo)
	{
		TaskInfo * pTemp = pTaskInfo;
		pTaskInfo = pTaskInfo->pNext;
		delete pTemp;
	}

	pTaskInfo = m_pFrameList;
	while (pTaskInfo)
	{
		TaskInfo * pTemp = pTaskInfo;
		pTaskInfo = pTaskInfo->pNext;
		delete pTemp;
	}
}

void Scheduler::InsertFrameTask(TaskInfo *pTaskInfo)
{
	//
	// inserted into list in time order
	//
	if (!m_pFrameList)
	{
		pTaskInfo->pNext = NULL;
		m_pFrameList = pTaskInfo;
	}
	else if (m_pFrameList->time.next > pTaskInfo->time.next)
	{
		pTaskInfo->pNext = m_pFrameList;
		m_pFrameList = pTaskInfo;
	}
	else
	{
		TaskInfo * pInfo = m_pFrameList;
		while (pInfo)
		{
			if (pInfo->pNext == NULL)
			{
				pTaskInfo->pNext = NULL;
				pInfo->pNext = pTaskInfo;
				break;
			}
			else if (pInfo->pNext->time.next > pTaskInfo->time.next)
			{
				pTaskInfo->pNext = pInfo->pNext;
				pInfo->pNext = pTaskInfo;
				break;
			}
			pInfo = pInfo->pNext;
		}
	}
}

void Scheduler::InsertTimeTask(TaskInfo *pTaskInfo)
{
	//
	// inserted into list in time order
	//
	if (!m_pTaskList)
	{
		pTaskInfo->pNext = NULL;
		m_pTaskList = pTaskInfo;
	}
	else if (m_pTaskList->time.next > pTaskInfo->time.next)
	{
		pTaskInfo->pNext = m_pTaskList;
		m_pTaskList = pTaskInfo;
	}
	else
	{
		TaskInfo * pInfo = m_pTaskList;
		while (pInfo)
		{
			if (pInfo->pNext == NULL)
			{
				pTaskInfo->pNext = NULL;
				pInfo->pNext = pTaskInfo;
				break;
			}
			else if (pInfo->pNext->time.next > pTaskInfo->time.next)
			{
				pTaskInfo->pNext = pInfo->pNext;
				pInfo->pNext = pTaskInfo;
				break;
			}
			pInfo = pInfo->pNext;
		}
	}
}

void Scheduler::GetNextFrameTask(TaskInfo **ppTaskInfo)
{
	// return the next frame task. If no frame task is scheduled to run this frame,
	// return null. note that this method REMOVES the task from the list, so the
	// caller is responsible for either adding it back in (rescheduling) or
	// deleting it.
	TaskInfo *pNextTask = m_pFrameList;
	if (pNextTask && pNextTask->time.next <= m_clock.GetFrame())
	{
		m_pFrameList = pNextTask->pNext;
		pNextTask->pNext = NULL;
		*ppTaskInfo = pNextTask;
		return;
	}
	*ppTaskInfo=NULL;
}

void Scheduler::GetNextTimeTask(TaskInfo **ppTaskInfo)
{
	// return the next time task. If no time task is scheduled to run this frame,
	// return null. note that this method REMOVES the task from the list, so the
	// caller is responsible for either adding it back in (rescheduling) or
	// deleting it.
	TaskInfo *pNextTask = m_pTaskList;
	if (pNextTask && pNextTask->time.next <= m_clock.GetFrameEnd())
	{
		m_pTaskList = pNextTask->pNext;
		pNextTask->pNext = NULL;
		*ppTaskInfo = pNextTask;
		return;
	}
	*ppTaskInfo=NULL;
}

int Scheduler::Schedule(TaskType type, int start, int period, int duration, ITask * pTask, void * pUser, int *pID)
{
	//
	// this schedules a task of the appropriate type (RENDER, FRAME, or TIME). Time tasks and
	// frame tasks are kept in separate lists for ease of handling. Render tasks are stored
	// as a single special-case entry.
	//
	// time and frame tasks have a start time, a duration, and a period.
	// the duration is relative to the start time, except for duration 0 which is a special case.
	// since the scheduler doesn't care about the duration itself, it converts it into an end time
	// and stores that instead. the render task does ignores start/duration/end.
	//
	// pUser is a user data pointer.
	//
	// a unique task id is generated and returned in pID. if you don't care about an ID,
	// pass in NULL instead.
	//
	if (type == TASK_RENDER)
	{
		renderTask.pTask = pTask;
		renderTask.pUser = pUser;
		renderTask.id = RENDER_TASK_ID;
		return 1;
	}
	else
	{
		//
		// Allocating memory like this has high overhead. It would be much better to use a
		// fast allocator which preallocates and/or reuses TaskInfo structures.
		//
		TaskInfo *pTaskInfo = new TaskInfo;

		if (pTaskInfo)
		{
			pTaskInfo->pTask = pTask;
			pTaskInfo->pNext = m_pTaskList;
			pTaskInfo->status = 0;
			pTaskInfo->id = m_nextId++;
			if (pID)
				*pID = pTaskInfo->id;
			pTaskInfo->pUser = pUser;
			pTaskInfo->time.start = start;
			pTaskInfo->time.period = period;
			if (duration == 0)
				pTaskInfo->time.duration = 0; // infinite
			else
				pTaskInfo->time.duration = start + duration - 1; // compute end time
			pTaskInfo->time.next = start;

			printf("Scheduling %s task %u from %3u to %3u, every %2u %s\n",
				type==TASK_TIME?"time ":"frame",
				pTaskInfo->id,
				pTaskInfo->time.start,
				pTaskInfo->time.duration,
				pTaskInfo->time.period,
				type==TASK_TIME?"ms":"frames");

			if (type == TASK_TIME)
				InsertTimeTask(pTaskInfo);
			else if (type == TASK_FRAME)
				InsertFrameTask(pTaskInfo);
			return 1;
		}
		else return 0;
	}
}

void Scheduler::Terminate(int id)
{
	//
	// Terminate a task. This is most useful with infinite-duration tasks, but
	// it is also handy to delete finite tasks prematurely. It would be a simple
	// matter to add a "suspend task" option which suspends a task instead of
	// removing it.
	//
	// Terminate marks the task for deletion rather than just removing it,
	// since it may be called while ExecuteFrame is iterating through the list.
	// Marked tasks are swept up at the end of each frame.
	//
	// Since all task ID's are unique, this method stops looking after it finds
	// a matching task.
	// 
	int found = FALSE;

	if (id == RENDER_TASK_ID)
	{
		renderTask.pTask = NULL;
		found = TRUE;
	}

	TaskInfo * pTaskInfo = m_pTaskList;
	while (pTaskInfo && !found)
	{
		if (pTaskInfo->id == id)
		{
			pTaskInfo->status = TASK_DELETE;
			found = TRUE;
			break;
		}
		pTaskInfo = pTaskInfo->pNext;
	}
	pTaskInfo = m_pFrameList;
	while (pTaskInfo && !found)
	{
		if (pTaskInfo->id == id)
		{
			pTaskInfo->status = TASK_DELETE;
			found = TRUE;
			break;
		}
		pTaskInfo = pTaskInfo->pNext;
	}
}

void Scheduler::SweepGarbage()
{
	//
	// Check both tasklist and frame list for tasks that were marked
	// for deletion by the Terminate() method. This implementation is
	// pretty brute-force; garbage collection could be run as an
	// idle processing task.
	//
	printf("SWEEP BEGIN\n");
	TaskInfo * pTaskInfo = m_pTaskList;
	TaskInfo * pPrev = NULL;
	while (pTaskInfo)
	{
		if (pTaskInfo->status == TASK_DELETE)
		{
			TaskInfo * pTemp = pTaskInfo;
			pTaskInfo = pTaskInfo->pNext;
			if (pTemp == m_pTaskList)
				m_pTaskList = pTaskInfo;
			else
				pPrev->pNext = pTaskInfo;
			printf("Sched: Deleted %d\n",pTemp->id);
			delete pTemp;
		}
		else
		{
			pPrev = pTaskInfo;
			pTaskInfo = pTaskInfo->pNext;
		}
	}

	pTaskInfo = m_pFrameList;
	pPrev = NULL;
	while (pTaskInfo)
	{
		if (pTaskInfo->status == TASK_DELETE)
		{
			TaskInfo * pTemp = pTaskInfo;
			pTaskInfo = pTaskInfo->pNext;
			if (pTemp == m_pFrameList)
				m_pFrameList = pTaskInfo;
			else
				pPrev->pNext = pTaskInfo;
			printf("Sched: Deleted %d\n",pTemp->id);
			delete pTemp;
		}
		else
		{
			pPrev = pTaskInfo;
			pTaskInfo = pTaskInfo->pNext;
		}
	}
	printf("SWEEP END\n");
}

int Scheduler::ExecuteFrame()
{
	printf("EXEC BEGIN\n");
	//
	// Run one frame. This takes the time stamp marking the end of the frame
	// and then processes events for that frame retroactively. This method has
	// the advantage of flexibility, especially if the frame rate fluctuates.
	// However it is always a little behind, because it can't compute the
	// frame length until the end of the frame is reached. With a fixed known
	// frame rate you could optimize things a bit and make the start/end times
	// correspond exactly with real time.
	//
	m_clock.BeginFrame();
	unsigned started = m_clock.GetSystem();

	//
	// Execute any time-based tasks
	//
	// (1) Pop the next task off the list. Since the list is always
	//     sorted, the first item in the list is always the next task.
	// (2) Execute it and update times
	// (3) If it's expired, delete it
	//     Otherwise, insert it into the list in its new position
	//
	TaskInfo * pTaskInfo = NULL;
	GetNextTimeTask(&pTaskInfo);
	while (pTaskInfo)
	{
		m_clock.AdvanceTo(pTaskInfo->time.next);

		pTaskInfo->pTask->Execute(pTaskInfo->id, m_clock.GetTime(), pTaskInfo->pUser);
		pTaskInfo->time.last = pTaskInfo->time.next;
		pTaskInfo->time.next += pTaskInfo->time.period;

		if (pTaskInfo->time.duration == 0 ||
			pTaskInfo->time.duration >= pTaskInfo->time.next)
		{
			// re-insert into list with updated time
			InsertTimeTask(pTaskInfo);
		}
		else
		{
			// task is expired, delete it
			printf("Sched: Expired %d\n",pTaskInfo->id);
			delete pTaskInfo;
		}
		GetNextTimeTask(&pTaskInfo);
	}

	//
	// Advance simulation clock to end of frame
	//
	m_clock.AdvanceToEnd();

	//
	// Now execute all frame tasks in round-robin fashion.
	// Frame tasks always execute at the end of the frame just
	// before rendering. A priority scheme could be used to
	// control sequence. It would be more efficient to keep the
	// list sorted, the same as with time tasks (exe
	//
	pTaskInfo = m_pFrameList;
	GetNextFrameTask(&pTaskInfo);
//	TaskInfo * pPrev = NULL;
	while (pTaskInfo)
	{
		pTaskInfo->pTask->Execute(pTaskInfo->id, m_clock.GetTime(), pTaskInfo->pUser);
		pTaskInfo->time.last = pTaskInfo->time.next;
		pTaskInfo->time.next += pTaskInfo->time.period;

		if (pTaskInfo->time.duration == 0 ||
			pTaskInfo->time.duration >= pTaskInfo->time.next)
		{
			// re-insert into list with updated time
			InsertFrameTask(pTaskInfo);
		}
		else
		{
			// task is expired, delete it
			printf("Sched: Expired %d\n",pTaskInfo->id);
			delete pTaskInfo;
		}
		GetNextFrameTask(&pTaskInfo);
	}

	//
	// render
	//
	renderTask.pTask->Execute(renderTask.id, m_clock.GetFrame(), renderTask.pUser);

	//
	// here is where we could do idle processing or load balancing
	//
	unsigned elapsed = m_clock.GetSystem() - started;
	unsigned frameLength = m_clock.GetFrameEnd() - m_clock.GetFrameStart();
//	printf("Busy %u ms, idle %u ms\n", elapsed, frameLength - elapsed);

	//
	// If any tasks are terminated during execution, it is easier to leave
	// them in the list until we're finished iterating through it, then sweep
	// them out later.
	//
	printf("EXEC END\n");
	SweepGarbage();

	return 0;
}


