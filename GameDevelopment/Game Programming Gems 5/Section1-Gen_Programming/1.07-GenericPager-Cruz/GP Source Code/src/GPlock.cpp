// +-----------------------------------------------------------------------
// |
// |                         File Name: GPlock.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This files contains the GPlock Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------
#include <GPpch.h>
#include <GPlock.h>

// +---------------------------
// |	Constructor & Destructor
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPlock::GPlock
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPlock::GPlock()
{
	InitializeCriticalSection( &criticalSection_ );
}

// +-------------------------------------------------------------
// |
// | Function        : GPlock::~GPlock
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPlock::~GPlock()
{
	DeleteCriticalSection( &criticalSection_ );
}

// +---------------------------
// |	Lock & UnLock
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPlock::Lock
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPlock::Lock()
{
	EnterCriticalSection( &criticalSection_ );
}

// +-------------------------------------------------------------
// |
// | Function        : GPlock::UnLock
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPlock::UnLock()
{
	LeaveCriticalSection( &criticalSection_ );
}

// +---------------------------
// |	CRITICAL_SECTION
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPlock::GetCriticalSection
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
CRITICAL_SECTION& GPlock::GetCriticalSection() const
{
	return ((CRITICAL_SECTION&)criticalSection_);
}


