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
#include <conio.h>
#include <stdio.h>

#include "Scheduler.h"
#include "ITask.h"

Scheduler sched;

//
// two very simple task implementations
//

// this class just prints a message when executed
class PrintTask : public ITask {
public:
	virtual void Execute(int id, int time, void * pUser) {
		printf("Task[%u] time=%-3u  %s\n", id, time, pUser?pUser:"");
		Sleep(2); // waste some time to simulate doing actual work
	}
};

// this class terminates another task
class KillTask : public ITask {
public:
	virtual void Execute(int id, int time, void * pUser) {
		printf("Task[%u] time=%-3u  Kill %d\n", id, time, (int) pUser);
		sched.Terminate((int) pUser);
	}
};

PrintTask printTask;
KillTask killTask;

int main()
{
	int id, frame_id;

	// the renderer doesn't do anything interesting; it's just here
	// as an example
	sched.Schedule(TASK_RENDER, 0, 0, 0, &printTask, "Renderer", &id);

	// a few time tasks to demonstrate execution sequence
	sched.Schedule(TASK_TIME, 0, 15, 150, &printTask, "Time1", &id);
	sched.Schedule(TASK_TIME, 0, 50, 150, &printTask, "Time2", &id);
	sched.Schedule(TASK_TIME, 200, 14, 140, &printTask, "Time3", &id);

	// some frame tasks to mix with the time tasks
	sched.Schedule(TASK_FRAME, 4,  2, 3, &printTask, "Frame2", &id);
	sched.Schedule(TASK_FRAME, 2,  1, 0, &printTask, "Infinite Frame", &frame_id);

	// the second frame task has infinite duration, so this task will
	// kill it at frame 20. you could do this anywhere in the program.
	// this is an example of one task affecting another.
	sched.Schedule(TASK_FRAME, 20, 1, 1, &killTask, (void*) frame_id, &id);


	//
	// simple and stupid main loop
	//
	// run the scheduler for 1000 ms, in 33 ms "frames"
	// hitting the spacebar will pause/resume the program
	//
	unsigned time = 33;
	sched.Run();
	do
	{
		if (_kbhit())
		{
			int c = _getch();
			if (c == ' ')
				if (sched.IsRunning())
					sched.Stop();
				else
					sched.Run();
		}
		
		while (sched.GetClock().GetSystem() < time) Sleep(1);
		time += 33;
		sched.ExecuteFrame();
	}
	while (sched.GetClock().GetTime() < 1000);

	return 0;
}

