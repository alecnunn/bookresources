/*********************************************************************\
 *
 * Copyright Stormfront Studios, Inc.  Confidential.
 *
 *   This source code is proprietary and confidential under
 * appropriate laws.  No duplication is allowed without the
 * express written permission of Stormfront Studios, Inc.
 * All Rights Reserved.
 *
 *
 * $Workfile: AIUpdate.cpp $
 *
 * $Modtime: 7/02/01 4:05p $
 *
 * $Author: Balexander $
 *
 *
 * Description:
 *
 *   This source file contains the AI update module class.
 *
 *
\*********************************************************************/

#include <stdio.h>
#include <float.h>

#include "LoadBalance.h"

///////////////////////////////////////////////////////////////////////
// The AI Update Module

ExecutionModule			gAIExecutionModule;

///////////////////////////////////////////////////////////////////////
// AI Update Module Profiling

#ifdef _DEBUG

#define PROFILE_INIT(data)			\
	{								\
		data.TotalTime = 0;			\
		data.MinTime = FLT_MAX;		\
		data.MaxTime = -FLT_MAX;	\
		data.Count = 0;				\
		data.Name = NULL;			\
	}

#define PROFILE_RELEASE(data)	((void)0)

#define PROFILE_START(data)	// TBD - Record system ticks

#define PROFILE_END(data)											\
	{																\
		float ExecTime = 0; /* Get system ticks */					\
		data.TotalTime += ExecTime;									\
		if (ExecTime < data.MinTime)	data.MinTime = ExecTime;	\
		if (ExecTime > data.MaxTime)	data.MaxTime = ExecTime;	\
		data.Count++;												\
	}

#define PROFILE_GROUP_INIT(data)		\
	{									\
		data.TotalTime = 0;				\
		data.MaxTime = -FLT_MAX;		\
		data.FrameCount = 0;			\
		data.LastFrame = ~0;			\
		data.LastFrameTotalTime = 0;	\
	}

#define PROFILE_GROUP_RELEASE(data)	((void)0)

#define PROFILE_GROUP_FRAME_START(data)								\
	{																\
		data.FrameTaskCount = 0;								\
	}

#define PROFILE_GROUP_FRAME_END(data)	((void)0)

#define PROFILE_GROUP_START(data)									\
	{																\
		if (data.LastFrame != gAIExecutionModule.GetFrames())		\
		{															\
			data.LastFrame = gAIExecutionModule.GetFrames();		\
			data.LastFrameTotalTime = data.TotalTime;				\
			data.FrameCount++;										\
		}															\
		/* Record system ticks */									\
	}

#define PROFILE_GROUP_END(data)										\
	{																\
		float ExecTime = 0; /* Get system ticks */					\
		data.TotalTime += ExecTime;									\
		data.FrameTaskCount++;										\
		ExecTime = data.TotalTime - data.LastFrameTotalTime;		\
		if (ExecTime > data.MaxTime)	data.MaxTime = ExecTime;	\
	}

void PROFILE_INIT_MODULE( void )
{
	// Add code here
}

void PROFILE_RELEASE_MODULE( void )
{
	// Add code here
}



#else

#define PROFILE_INIT_MODULE()		((void)0)
#define PROFILE_RELEASE_MODULE()	((void)0)
#define PROFILE_INIT(data)			((void)0)
#define PROFILE_RELEASE(data)		((void)0)
#define PROFILE_START(data)			((void)0)
#define PROFILE_END(data)			((void)0)

#define PROFILE_GROUP_INIT(data)		((void)0)
#define PROFILE_GROUP_RELEASE(data)		((void)0)
#define PROFILE_GROUP_FRAME_START(data)	((void)0)
#define PROFILE_GROUP_FRAME_END(data)	((void)0)
#define PROFILE_GROUP_START(data)		((void)0)
#define PROFILE_GROUP_END(data)			((void)0)

#endif


///////////////////////////////////////////////////////////////////////
// AI update module class

//
//  Construction
//

//
//  This function handles AI update initialization.
//

bool ExecutionModule::Init( void )
{
	mTime = 0;
	mTicks = 0;
	mFrames = 0;

	mpScheduledTasks = NULL;
	mpExecutingTask = NULL;

	// Init profiling
	PROFILE_INIT_MODULE();
	PROFILE_INIT( GetProfileData() );
	AI_UPDATE_SET_PROFILE_NAME( *this, "AI Update Module" );

	return( true );
}


//
//  This function handles AI update release.
//

bool ExecutionModule::Release( void )
{
	// Clear Task schedule
	while (mpScheduledTasks != NULL)
	{
		UnscheduleTask( mpScheduledTasks );
	}

	// Remove all groups
	while (mpFirstGroup != NULL)
	{
		DetachGroup( mpFirstGroup );
	}

	PROFILE_RELEASE( GetProfileData() );
	PROFILE_RELEASE_MODULE();

	return( true );
}


//
//  Update
//

void ExecutionModule::Update( unsigned long dt )
{
	PROFILE_START( GetProfileData() );
	if (dt)
	{
		float fdt = (float)dt/1000.0f;

		mTime += fdt;
		mTicks += dt;
		mFrames++;

		#ifdef _DEBUG
		{
			for (SchedulingGroup * group = mpFirstGroup; group != NULL; group = group->GetNext())
			{
				PROFILE_GROUP_FRAME_START( group->GetGroupProfileData() );
			}
		}
		#endif

		// Run all expired scheduled Tasks
		while (		(mpScheduledTasks != NULL)
				&&	(mpScheduledTasks->GetTime() < mTime) )
		{
			// Remove Task from queue
			mpExecutingTask = mpScheduledTasks;
			mpScheduledTasks = mpScheduledTasks->mpScheduledNext;

			mpExecutingTask->mScheduled = false;

			SchedulingGroup * executingGroup = mpExecutingTask->GetGroup();

			if (executingGroup != NULL)
			{
				PROFILE_GROUP_START( executingGroup->GetGroupProfileData() );
			}

				PROFILE_START( mpExecutingTask->GetProfileData() );

					mpExecutingTask->Run( (mpExecutingTask->mLastTime != 0) ? (mTime - mpExecutingTask->mLastTime) : 0 );

				PROFILE_END( mpExecutingTask->GetProfileData() );

			if (executingGroup != NULL)
			{
				PROFILE_GROUP_END( executingGroup->GetGroupProfileData() );
			}

			// If the Task has not gone away, remove it from the schedule.
			mpExecutingTask->mLastTime = mTime;

			mpExecutingTask = NULL;		// JIC
		}

		// Run all groups to rescedule Tasks.
		for (SchedulingGroup * group = mpFirstGroup; group != NULL; group = group->GetNext())
		{
			PROFILE_START( group->GetProfileData() );
			group->Schedule( this );
			PROFILE_END( group->GetProfileData() );
			PROFILE_GROUP_FRAME_END( group->GetProfileData() );
		}
	}
	PROFILE_END( GetProfileData() );
}


//
//  Attach/Detach groups
//

bool ExecutionModule::AttachGroup( SchedulingGroup * group )
{
	for (SchedulingGroup * curGroup = mpFirstGroup; curGroup != NULL; curGroup = curGroup->mpNext)
	{
		if (curGroup == group)
		{
			// Already in the group.
			return false;
		}
	}

	// Add to front of list.
	group->mpNext = mpFirstGroup;
	mpFirstGroup = group;

	return true;
}

bool ExecutionModule::DetachGroup( SchedulingGroup * group )
{
	for (SchedulingGroup ** DelAt = &mpFirstGroup; *DelAt != NULL; DelAt = &((*DelAt)->mpNext))
	{
		if (*DelAt == group)
		{
			*DelAt = group->mpNext;
			group->mpNext = NULL;
			return true;
		}
	}

	return false;
}

//
//  Schedule a Task
//

bool ExecutionModule::ScheduleTask( Task * task )
{
	// Insert sort the task by time.
	if (task->IsScheduled())
	{
		return false;
	}

	for (Task ** AddAt = &mpScheduledTasks; *AddAt != NULL; AddAt = &((*AddAt)->mpScheduledNext))
	{
		if ((*AddAt)->GetTime() > task->GetTime())
		{
			break;
		}
	}

	// Add at location found.
	task->mpScheduledNext = *AddAt;
	*AddAt = task;
	task->mScheduled = true;
	return true;
}

//
//  Unschedule a previously scheduled Task
//

bool ExecutionModule::UnscheduleTask( Task * task )
{
	if (!task->IsScheduled())
	{
		return false;
	}

	for (Task ** DelAt = &mpScheduledTasks; *DelAt != NULL; DelAt = &((*DelAt)->mpScheduledNext))
	{
		if (*DelAt == task)
		{
			*DelAt = task->mpScheduledNext;
			task->mpScheduledNext = NULL;
			task->mScheduled = false;
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////
// Scheduling Group Class

// initialization/release
bool SchedulingGroup::Init( void )
{
	PROFILE_INIT( GetProfileData() );
	PROFILE_GROUP_INIT( GetGroupProfileData() );
	mpFirstTask = NULL;
	mTaskCount = 0;
	return true;
}

bool SchedulingGroup::Release( void )
{
	gAIExecutionModule.DetachGroup( this );
	while (mpFirstTask != NULL)
	{
		DetachTask( mpFirstTask );
	}
	PROFILE_GROUP_RELEASE( GetGroupProfileData() );
	PROFILE_RELEASE( GetProfileData() );
	return true;
}

bool SchedulingGroup::AttachTask( Task * task )
{
	for (Task * curTask = mpFirstTask; curTask != NULL; curTask = curTask->mpGroupNext)
	{
		if (curTask == task)
		{
			// Already in the group.
			return false;
		}
	}

	// Add to front of list.
	task->mpGroupNext = mpFirstTask;
	mpFirstTask = task;
	task->mpGroup  = this;
	mTaskCount++;

	return true;
}

bool SchedulingGroup::DetachTask( Task * task )
{
	for (Task ** DelAt = &mpFirstTask; *DelAt != NULL; DelAt = &((*DelAt)->mpGroupNext))
	{
		if (*DelAt == task)
		{
			*DelAt = task->mpGroupNext;
			task->mpGroupNext = NULL;
			task->mpGroup = NULL;
			mTaskCount--;
			return true;
		}
	}

	return false;
}

void SchedulingGroup::SetTaskTime( Task * task, float time )
{
	task->mTime = time;
}

///////////////////////////////////////////////////////////////////////
// Task Class

bool Task::Init( void )
{
	PROFILE_INIT( GetProfileData() );
	mTime = 0;
	mLastTime = 0;
	mScheduled = false;
	mpGroup = NULL;
	mpGroupNext = NULL;
	mpScheduledNext = NULL;
	return true;
}

bool Task::Release( void )
{
	if (mpGroup != NULL)
	{
		mpGroup->DetachTask( this );
	}
	gAIExecutionModule.UnscheduleTask( this );
	PROFILE_RELEASE( GetProfileData() );
	return true;
}



///////////////////////////////////////////////////////////////////////
// AI Update Every Frame

void EveryFrameGroup::Schedule( ExecutionModule * module )
{
	// Schedule all Tasks, setting their fire times to 0.
	// This will ensure that they are fired next Update module call.

	for (Task * task = mpFirstTask; task != NULL; task = task->GetGroupNext())
	{
		if (!task->IsScheduled())
		{
			SetTaskTime( task, 0 );
			module->ScheduleTask( task );
		}
	}
}


///////////////////////////////////////////////////////////////////////
// AI Update Count

bool CountGroup::Init( int count )
{
	mCount = count;
	mpCurrentTask = NULL;
	return SchedulingGroup::Init();
}

bool CountGroup::Release( void )
{
	mpCurrentTask = NULL;
	return SchedulingGroup::Release();
}

bool CountGroup::DetachTask( Task * task )
{
	// If we are detaching a 
	if (mpCurrentTask == task)
	{
		mpCurrentTask = mpCurrentTask->GetGroupNext();
		if (mpCurrentTask == NULL)
		{
			mpCurrentTask = GetFirstTask();
		}
	}

	return SchedulingGroup::DetachTask( task );
}

void CountGroup::Schedule( ExecutionModule * module )
{
	if (GetFirstTask() != NULL)
	{
		if (mpCurrentTask == NULL)
		{
			// Initialize current Task
			mpCurrentTask = GetFirstTask();
		}

		Task * start = mpCurrentTask;

		int count = mCount;
		while (count)
		{
			if (!mpCurrentTask->IsScheduled())
			{
				// NOt scheduled, so schedule it.
				SetTaskTime( mpCurrentTask, 0 );
				module->ScheduleTask( mpCurrentTask );
				count--;
			}

			// Move to next Task.
			mpCurrentTask = mpCurrentTask->GetGroupNext();
			if (mpCurrentTask == NULL)
			{
				// Wrap around
				mpCurrentTask = GetFirstTask();
			}
			if (mpCurrentTask == start)
			{
				// Back at start, so stop.
				break;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////
// AI Update Spread

bool SpreadGroup::Init( float period )
{
	mPeriod = period;
	mTime = 0;
	return SchedulingGroup::Init();
}

bool SpreadGroup::AttachTask( Task * task )
{
	if (SchedulingGroup::AttachTask( task ))
	{
		return true;
	}

	return false;
}

bool SpreadGroup::DetachTask( Task * task )
{
	if (SchedulingGroup::DetachTask( task ))
	{
		return true;
	}

	return false;
}

void SpreadGroup::Schedule( ExecutionModule * module )
{
	if (GetTaskCount() > 0)
	{
		float dt = mPeriod / GetTaskCount();

		// Adjust time to make sure its past the current time.
		mTime += (int((module->GetTime() - mTime) / dt) + 1) * dt;

		for (Task * task = mpFirstTask; task != NULL; task = task->GetGroupNext())
		{
			if (!task->IsScheduled())
			{
				SetTaskTime( task, mTime );
				module->ScheduleTask( task );
				mTime += dt;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////
// Timed Update Task

bool TimedTask::Init()
{
	Reset();
	return Task::Init();
}

//
// Reset all stats.
//

void TimedTask::Reset( void )
{
	mLastTime = 0;
	mTotalTime = 0;
	mExecutionCount = 0;
}

//
// This Task will time the run of the Execute Task below.
//

void TimedTask::Run( float dt )
{
	// Record system ticks

	Execute( dt );

	mLastTime = 0; // Get system ticks

	mTotalTime += mLastTime;
	mExecutionCount++;
}


///////////////////////////////////////////////////////////////////////
// AI Max Time Update

bool MaxTimeGroup::Init( float period )
{
	mMaxTime = period;
	mpCurrentTask = NULL;
	return SchedulingGroup::Init();
}

bool MaxTimeGroup::Release( void )
{
	mpCurrentTask = NULL;
	return SchedulingGroup::Release();
}

bool MaxTimeGroup::AttachTask( TimedTask * task )
{
	// Just call the protected version
	return AttachTask( (Task *)task );
}

bool MaxTimeGroup::DetachTask( TimedTask * task )
{
	// Just call the protected version
	return DetachTask( (Task *)task );
}

bool MaxTimeGroup::AttachTask( Task * task )
{
	return SchedulingGroup::AttachTask( task );
}

bool MaxTimeGroup::DetachTask( Task * task )
{
	if (mpCurrentTask == task)
	{
		mpCurrentTask = mpCurrentTask->GetGroupNext();
		if (mpCurrentTask == NULL)
		{
			mpCurrentTask = GetFirstTask();
		}
	}

	return SchedulingGroup::DetachTask( task );
}

void MaxTimeGroup::Schedule( ExecutionModule * module )
{
	if (GetFirstTask() != NULL)
	{
		float ScheduledTime = 0;
		int ScheduleCount = 0;

		if (mpCurrentTask == NULL)
		{
			// Initialize current Task
			mpCurrentTask = GetFirstTask();
		}

		Task * start = mpCurrentTask;

		while (ScheduledTime < mMaxTime)
		{
			if (!mpCurrentTask->IsScheduled())
			{
				// Not scheduled, so schedule it.
				TimedTask * timedTask = (TimedTask *)mpCurrentTask;
				
				float NewTotal = ScheduledTime + timedTask->GetLastExecutionTime();

				if (	(ScheduledTime < mMaxTime)	// We do not exceed the time limit.
					||	(ScheduleCount == 0) )	// It's the only thing to schedule.
				{
					SetTaskTime( mpCurrentTask, 0 );
					module->ScheduleTask( mpCurrentTask );
					ScheduledTime = NewTotal;
					ScheduleCount++;
				}
			}

			// Move to next Task.
			mpCurrentTask = mpCurrentTask->GetGroupNext();
			if (mpCurrentTask == NULL)
			{
				// Wrap around
				mpCurrentTask = GetFirstTask();
			}
			if (mpCurrentTask == start)
			{
				// Back at start, so stop.
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////
// Variable Delay Task

bool VarDelayTask::Init()
{
	mDelay = 1.0f / 4.0f;	// Default delay of 1/4 of a second.
	return Task::Init();
}


///////////////////////////////////////////////////////////////////////
// AI Variable Delay Update Group

bool VarDelayGroup::Init( void )
{
	mTime = 0;
	return SchedulingGroup::Init();
}

bool VarDelayGroup::AttachTask( VarDelayTask * task )
{
	// Just call the protected version
	return AttachTask( (Task *)task );
}

bool VarDelayGroup::DetachTask( VarDelayTask * task )
{
	// Just call the protected version
	return DetachTask( (Task *)task );
}

bool VarDelayGroup::AttachTask( Task * task )
{
	return SchedulingGroup::AttachTask( task );
}

bool VarDelayGroup::DetachTask( Task * task )
{
	return SchedulingGroup::DetachTask( task );
}

void VarDelayGroup::Schedule( ExecutionModule * module )
{
	if (mpFirstTask != NULL)
	{
		const float dt = 1.0f / 30.0f;

		// Adjust time to make sure its past the current time.
		mTime += (int((module->GetTime() - mTime) / dt) + 1) * dt;

		for (Task * task = mpFirstTask; task != NULL; task = task->GetGroupNext())
		{
			if (!task->IsScheduled())
			{
				if (task->GetTime() == 0)
				{
					// First time the task is scheduled.
					// In order to attempt to prevent tasks from executing all on
					// the same frame, use the mTime to stagger the tasks
					SetTaskTime( task, mTime );
					mTime += dt;
				}
				else
				{
					// Task has been schedule before, use task delay to 
					// reschedule.
					VarDelayTask * DelayTask = (VarDelayTask *)task;
					float NewTime = task->GetTime();
					NewTime += (int((module->GetTime() - task->GetTime()) / DelayTask->GetDelay()) + 1) * DelayTask->GetDelay();
					SetTaskTime( task, NewTime );
				}
				module->ScheduleTask( task );
			}
		}
	}
}


