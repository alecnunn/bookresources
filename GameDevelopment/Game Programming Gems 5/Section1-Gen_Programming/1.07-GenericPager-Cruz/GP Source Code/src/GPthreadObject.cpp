// +-----------------------------------------------------------------------
// |
// |                         File Name: GPthreadObject.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPthreadObject Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPpch.h>
#include <GPthreadObject.h>

#include <GPthread.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::GPthreadObject
// | Description     : Constructor of the Active Object
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPthreadObject::GPthreadObject():
isFinishing_	(false),
thread_				(NULL),
exitFlag_			(false)
{
	thread_ = new GPthread(GPthreadObjectFuncPtr, this);
	thread_->SetPriority(THREAD_PRIORITY_NORMAL);
}

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::~GPthreadObject
// | Description     : Termination and clean-up
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPthreadObject::~GPthreadObject()
{
	if (thread_ != NULL)
  {
    thread_->WaitForFinish();
    delete(thread_);
    thread_ = NULL;
  }
}

// +---------------------------
// |	GPthread
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::GetThread
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPthread* GPthreadObject::GetThread() const
{
	return thread_;
}

// +---------------------------
// |	Priority
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::SetPriority
// | Description     : This method changes the priority of the thread
// |
// | Parameters : 
// |   priority      : 
// | 
// +-------------------------------------------------------------
void GPthreadObject::SetPriority(int priority)
{
	if( thread_ != NULL )
	{
    thread_->SetPriority(priority);
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::GetPriority
// | Description     : This method retrieves the priority of the thread
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
int GPthreadObject::GetPriority() const
{
	if( thread_ != NULL )
	{
		return thread_->GetPriority();
	}
	else 
	{
		return THREAD_PRIORITY_ERROR_RETURN;
	}
}

// +---------------------------
// |	Control
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::Start
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPthreadObject::Start()
{
	if( thread_ == NULL )
	{
		thread_ = new GPthread(GPthreadObjectFuncPtr, this);
	}

	thread_->Resume();
}

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::Pause
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPthreadObject::Pause()
{
	if( thread_ )
	{
		thread_->Suspend();
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::Finish
// | Description     : 
// |                   
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPthreadObject::Finish()
{
    isFinishing_ = true;

		Start();
		End();
		Sleep(0);

		if (thread_ != NULL)
		{
 			thread_->WaitForFinish();
			delete(thread_);
			thread_ = NULL;
		}
		
		isFinishing_ = false;
}

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::IsFinishing
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
bool GPthreadObject::IsFinishing() const
{
	return isFinishing_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::ProcessLoop
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPthreadObject::ProcessLoop()
{
	Init();

	while ( ! exitFlag_ )
	{
		Loop();
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::Init
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPthreadObject::Init()
{
	exitFlag_ = false;
}

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::End
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPthreadObject::End()
{
	exitFlag_ = true;
}

// +--------------------------------------------------
// |	Virtual Method to Implement by a derived class
// +--------------------------------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::Loop
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPthreadObject::Loop()
{
	//To Redefine
}

// +---------------------------
// |	GPthread Function
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPthreadObject::GPthreadObjectFuncPtr
// | Description     : 
// |
// | Parameters : 
// |   pArg          : 
// | 
// +-------------------------------------------------------------
DWORD WINAPI GPthreadObject::GPthreadObjectFuncPtr(void* pArg)
{
  GPthreadObject* pThreadObject = (GPthreadObject *) pArg;
  pThreadObject->ProcessLoop();
  return 0;
}





