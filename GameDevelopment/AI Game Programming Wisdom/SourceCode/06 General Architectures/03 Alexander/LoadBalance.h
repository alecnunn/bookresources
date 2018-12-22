/*********************************************************************\
 *
 * Workfile: LoadBalance.h
 *
 * Author: Balexander
 *
 *
 * Description:
 *
 *   This header file describes the AI update module class.
 *
 *
\*********************************************************************/


#ifndef __loadbalance_h__
#define __loadbalance_h__


class Task;
class SchedulingGroup;


///////////////////////////////////////////////////////////////////////
// AI Update Module Profiling
///////////////////////////////////////////////////////////////////////

#if _DEBUG

struct ProfileData 
{
	float					TotalTime;
	float					MinTime;
	float					MaxTime;
	unsigned long			Count;
	const char *			Name;
};

struct GroupProfileData 
{
	float					TotalTime;
	float					MaxTime;
	unsigned long			FrameCount;
	unsigned long			FrameTaskCount;
	unsigned long			LastFrame;
	float					LastFrameTotalTime;
};

#define	DECLARE_AI_UPDATE_PROFILE_ACCESS	ProfileData &	GetProfileData( void )	{ return mProfileData; }
#define	DECLARE_AI_UPDATE_PROFILE_DATA		ProfileData		mProfileData;

#define	DECLARE_AI_GROUP_UPDATE_PROFILE_ACCESS	GroupProfileData &	GetGroupProfileData( void )	{ return mGroupProfileData; }
#define	DECLARE_AI_GROUP_UPDATE_PROFILE_DATA	GroupProfileData	mGroupProfileData;

#define AI_UPDATE_SET_PROFILE_NAME( object, name )	((object).GetProfileData().Name = (name))

#else	//	_DEBUG

#define	DECLARE_AI_UPDATE_PROFILE_ACCESS
#define	DECLARE_AI_UPDATE_PROFILE_DATA

#define	DECLARE_AI_GROUP_UPDATE_PROFILE_ACCESS
#define	DECLARE_AI_GROUP_UPDATE_PROFILE_DATA

#define AI_UPDATE_SET_PROFILE_NAME( object, name )	((void)0)

#endif	//	_DEBUG

///////////////////////////////////////////////////////////////////////
// Execution module class
///////////////////////////////////////////////////////////////////////

class ExecutionModule
{
public:

	// initialization/release
	bool Init( void );
	bool Release( void );

	void Update( unsigned long dt );

	float			GetTime( void )		{ return mTime; }
	unsigned long	GetTicks( void )	{ return mTicks; }
	unsigned long	GetFrames( void )	{ return mFrames; }

	bool	AttachGroup( SchedulingGroup * group );
	bool	DetachGroup( SchedulingGroup * group );

	bool	ScheduleTask( Task * task );
	bool	UnscheduleTask( Task * task );

	Task *	GetFirstScheduledTask( void )	{ return mpScheduledTasks; }

	SchedulingGroup *	GetFirstGroup( void )	{ return mpFirstGroup; }

	DECLARE_AI_UPDATE_PROFILE_ACCESS

private:

	float						mTime;			// Current game time in seconds
	unsigned long				mTicks;			// Current game time in ms.
	unsigned long				mFrames;		// Current game frame count

	Task *						mpScheduledTasks;

	Task *						mpExecutingTask;

	SchedulingGroup *			mpFirstGroup;

	DECLARE_AI_UPDATE_PROFILE_DATA
};

extern ExecutionModule	gAIUpdateModule;

///////////////////////////////////////////////////////////////////////
// Scheduling Group Class

class SchedulingGroup
{
	friend bool ExecutionModule::AttachGroup( SchedulingGroup * group );
	friend bool ExecutionModule::DetachGroup( SchedulingGroup * group );

public:

	// initialization/release
	bool Init( void );
	bool Release( void );

	virtual bool		AttachTask( Task * task );
	virtual bool		DetachTask( Task * task );

	void				SetTaskTime( Task * task, float time );

	virtual void		Schedule( ExecutionModule * module ) = 0;

	Task *				GetFirstTask( void )	{ return mpFirstTask; }
	SchedulingGroup *	GetNext( void )			{ return mpNext; }

	int					GetTaskCount( void )	{ return mTaskCount; }

	DECLARE_AI_UPDATE_PROFILE_ACCESS
	DECLARE_AI_GROUP_UPDATE_PROFILE_ACCESS

protected:

	Task *				mpFirstTask;
	int					mTaskCount;

	SchedulingGroup *	mpNext;

	DECLARE_AI_UPDATE_PROFILE_DATA
	DECLARE_AI_GROUP_UPDATE_PROFILE_DATA
};

///////////////////////////////////////////////////////////////////////
// Task Class

class Task
{
	friend bool SchedulingGroup::AttachTask( Task * task );
	friend bool SchedulingGroup::DetachTask( Task * task );
	friend void SchedulingGroup::SetTaskTime( Task * task, float time );
	friend bool ExecutionModule::UnscheduleTask( Task * task );
	friend bool ExecutionModule::ScheduleTask( Task * task );
	friend void ExecutionModule::Update( unsigned long dt );

public:
	~Task()	{ Release(); }	// Just in case

	// initialization/release
	bool Init( void );
	bool Release( void );

	// This function gets called to do the actual work.
	virtual void Run( float dt ) = 0;

	bool	IsScheduled( void )		{ return mScheduled; }

	float GetTime( void)		{ return mTime; }
	float GetLastTime( void)	{ return mLastTime; }

	SchedulingGroup *		GetGroup( void )		{ return mpGroup; }

	Task *	GetGroupNext( void )	{ return mpGroupNext; }
	Task *	GetScheduleNext( void )	{ return mpScheduledNext; }
	
	DECLARE_AI_UPDATE_PROFILE_ACCESS

protected:

	float					mTime;			// Current schedule time in game seconds
	float					mLastTime;		// Last time the task was fired.
	bool					mScheduled;		// True if currently scheduled, o/w false

	SchedulingGroup *		mpGroup;

	Task *					mpGroupNext;
	Task *					mpScheduledNext;

	DECLARE_AI_UPDATE_PROFILE_DATA
};

///////////////////////////////////////////////////////////////////////
// Standard Scheduler Tasks
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// AI Timed Task

class TimedTask : public Task
{
public:

	// initialization/release
	bool Init( void );

	// Reset all stats.
	void Reset( void );

	// This function will time the run of the Execute function below.
	virtual void Run( float dt );

	virtual void Execute( float dt ) = 0;

	float			GetLastExecutionTime( void )	{ return mLastTime; }
	float			GetTotalExecutionTime( void )	{ return mTotalTime; }
	unsigned long	GetExecutionCount( void )		{ return mExecutionCount; }

protected:

	float			mLastTime;			// Last execution duration in seconds.
	float			mTotalTime;			// Total execution time in seconds for the current game.
	unsigned long	mExecutionCount;	// Number of times the function has executed in the current game.

};


///////////////////////////////////////////////////////////////////////
// AI Variable Delay Task

class VarDelayTask : public Task
{
public:

	// initialization/release
	bool Init( void );

	void	SetDelay( float seconds )	{ mDelay = seconds; }
	float	GetDelay( void )			{ return mDelay; }

protected:

	float		mDelay;		// Delay in seconds

};


///////////////////////////////////////////////////////////////////////
// Standard Scheduler Groups
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// AI Update Every Frame Group

class EveryFrameGroup : public SchedulingGroup
{
public:
	virtual void Schedule( ExecutionModule * module );
};


///////////////////////////////////////////////////////////////////////
// AI Count Update Group

class CountGroup : public SchedulingGroup
{
public:

	// initialization/release
	bool Init( int count = 1 );
	bool Release( void );

	virtual bool	DetachTask( Task * task );

	void	SetCount( int count )		{ mCount = count; }
	int		GetCount( void )			{ return mCount; }

	virtual void Schedule( ExecutionModule * module );

protected:

	int		mCount;		// Number of allowed executions per frame.
	Task *	mpCurrentTask;

};

///////////////////////////////////////////////////////////////////////
// AI Spread Update Group

class SpreadGroup : public SchedulingGroup
{
public:

	// initialization/release
	bool Init( float period = 1.0f/4.0f );

	virtual bool	AttachTask( Task * task );
	virtual bool	DetachTask( Task * task );

	void	SetPeriod( float period )	{ mPeriod = period; }
	float	GetPeriod( void )			{ return mPeriod; }

	virtual void Schedule( ExecutionModule * module );

protected:

	float		mTime;		// Current next scheduling time in seconds.
	float		mPeriod;	// Delay period (in seconds) between same task executions.
};

///////////////////////////////////////////////////////////////////////
// AI Max Time Update Group

class MaxTimeGroup : public SchedulingGroup
{
public:

	// initialization/release
	bool Init( float MaxTime );
	bool Release( void );

	// These make sure that only TimedTask's can be
	// attached to this group.
	bool	AttachTask( TimedTask * task );
	bool	DetachTask( TimedTask * task );

	void	SetMaxTime( float MaxTime )	{ mMaxTime = MaxTime; }
	float	GetMaxTime( float MaxTime )	{ return mMaxTime; }

	virtual void Schedule( ExecutionModule * module );

protected:

	// By making these protected, we make sure that only 
	// TimedTask's can be attached to this group.
	virtual bool	AttachTask( Task * task );
	virtual bool	DetachTask( Task * task );

	float	mMaxTime;		// Max group execution time (in seconds) per frame.
	Task *	mpCurrentTask;
};


///////////////////////////////////////////////////////////////////////
// AI Variable Delay Update Group

class VarDelayGroup : public SchedulingGroup
{
public:

	// initialization/release
	bool Init( void );

	// These make sure that only TimedTask's can be
	// attached to this group.
	bool	AttachTask( VarDelayTask * task );
	bool	DetachTask( VarDelayTask * task );

	virtual void Schedule( ExecutionModule * module );

protected:

	// By making these protected, we make sure that only 
	// TimedTask's can be attached to this group.
	virtual bool	AttachTask( Task * task );
	virtual bool	DetachTask( Task * task );

	float			mTime;		// Scheduling time for new tasks in seconds.
};


#endif __loadbalance_h__
