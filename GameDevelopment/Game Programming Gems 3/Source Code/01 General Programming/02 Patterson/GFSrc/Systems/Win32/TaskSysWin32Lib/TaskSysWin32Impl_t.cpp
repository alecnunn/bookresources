/* Copyright (C) Scott Patterson, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Patterson, 2002"
 */

// TaskSysWin32Impl_t1.cpp: implementation of the TaskSysWin32Impl_t class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GFInclude/Systems/Win32/TaskSysWin32Impl_t.h"
#include "GFInclude/Systems/Systems_t.h"
#include "GFInclude/Systems/FrameSys_t.h"
#include "GFInclude/Systems/InputSys_t.h"
#include "GFInclude/Framework/TaskCommand_t.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TaskSysWin32Impl_t::TaskSysWin32Impl_t()
{

}

TaskSysWin32Impl_t::~TaskSysWin32Impl_t()
{

}

bool TaskSysWin32Impl_t::Init( Systems_t *pSystems )
{
	m_pSystems = pSystems;
	return true;
}

void TaskSysWin32Impl_t::Shutdown()
{
}

void TaskSysWin32Impl_t::Post_TaskCommand( TaskCommand_t::TaskCommandID_t nID, Task_t *pTask )
{
	TaskCommand_t TC( nID, pTask );
	m_TaskCommandVector.push_back( TC );
}

bool TaskSysWin32Impl_t::Process_TaskCommands()
{
	// process any 
	TaskCommandVector_t::iterator iTCV;
	for( iTCV = m_TaskCommandVector.begin(); iTCV != m_TaskCommandVector.end(); ++iTCV )
	{
		switch( (*iTCV).m_nID )
		{
		case TaskCommand_t::FRAMESYNC_PUSH_BACK:
			(*iTCV).m_pTask->Connect( m_pSystems );
			m_FrameSyncTaskPtrList.push_back( (*iTCV).m_pTask );
			break;
		case TaskCommand_t::FRAMESYNC_PUSH_FRONT:
			(*iTCV).m_pTask->Connect( m_pSystems );
			m_FrameSyncTaskPtrList.push_front( (*iTCV).m_pTask );
			break;
		case TaskCommand_t::FRAMESYNC_REMOVE:
			m_FrameSyncTaskPtrList.remove( (*iTCV).m_pTask );
			(*iTCV).m_pTask->Disconnect();
			break;
		case TaskCommand_t::FRAMESYNC_REMOVE_ALL:
			{
				TaskPtrList_t::iterator iTPL;
				for( iTPL = m_FrameSyncTaskPtrList.begin(); iTPL != m_FrameSyncTaskPtrList.end(); ++iTPL )
				{
					Task_t *pTask = *iTPL;
					pTask->Disconnect();
				}
				m_FrameSyncTaskPtrList.clear();
			}
			break;
		case TaskCommand_t::ASYNC_PUSH_BACK:
			(*iTCV).m_pTask->Connect( m_pSystems );
			m_AsyncTaskPtrList.push_back( (*iTCV).m_pTask );
			break;
		case TaskCommand_t::ASYNC_PUSH_FRONT:
			(*iTCV).m_pTask->Connect( m_pSystems );
			m_AsyncTaskPtrList.push_front( (*iTCV).m_pTask );
			break;
		case TaskCommand_t::ASYNC_REMOVE:
			m_AsyncTaskPtrList.remove( (*iTCV).m_pTask );
			(*iTCV).m_pTask->Disconnect();
			break;
		case TaskCommand_t::ASYNC_REMOVE_ALL:
			{
				TaskPtrList_t::iterator iTPL;
				for( iTPL = m_AsyncTaskPtrList.begin(); iTPL != m_AsyncTaskPtrList.end(); ++iTPL )
				{
					Task_t *pTask = *iTPL;
					pTask->Disconnect();
				}
				m_AsyncTaskPtrList.clear();
			}
			break;
		default:
			LOGCALL((m_pSystems->GetLogSys()->WriteOutput(LOGSYS_CHANNEL_TASKSYS,LOGSYS_DETAIL_ERROR,"Unknown Task Command Was Skipped: %d\n",(*iTCV).m_nID));)
			break;
		}
	}

	// clear all of the commands
	m_TaskCommandVector.clear();

	// if no tasks exist, then we return false to say we are all done
	if( m_FrameSyncTaskPtrList.empty() &&
		m_AsyncTaskPtrList.empty() )
	{
		return false;
	}
	return true;
}

void TaskSysWin32Impl_t::Run()
{
	LOGCALL((m_pSystems->GetLogSys()->WriteOutput(LOGSYS_CHANNEL_TASKSYS,LOGSYS_DETAIL_HIGH,"Entered - TaskSysWin32Impl_t::Run\n"));)
	while(1)
	{
		// do any systems processing needed (gets keys for this frame)
		m_pSystems->Process();

		// do any requested operations on task lists
		if( !Process_TaskCommands() )
		{
			// return when there are no more tasks to perform
			return;
		}

		// check if ok to run frame sync tasks
		if( m_pSystems->GetFrameSys()->StartFrameSync() )
		{
			TaskPtrList_t::iterator iFSTPL;
			for( iFSTPL = m_FrameSyncTaskPtrList.begin(); iFSTPL != m_FrameSyncTaskPtrList.end(); ++iFSTPL )
			{
				Task_t *pFSTask = *iFSTPL;
				pFSTask->Run();
			}
			m_pSystems->GetFrameSys()->EndFrameSync();
		}

		// check if ok to run async tasks
		if( m_pSystems->GetFrameSys()->StartAsync() )
		{
			TaskPtrList_t::iterator iATPL;
			for( iATPL = m_AsyncTaskPtrList.begin(); iATPL != m_AsyncTaskPtrList.end(); ++iATPL )
			{
				Task_t *pATask = *iATPL;
				pATask->Run();
			}
			m_pSystems->GetFrameSys()->EndAsync();
		}

		// clear any keys not processed this frame
		m_pSystems->GetInputSys()->ClearKeyQueue();
	}
}

