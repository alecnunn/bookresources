// +-----------------------------------------------------------------------
// |
// |                         File Name: GPsync.cpp
// |
// |                             GENERIC PAGER
// |
// |             Developed by Ignacio Incera Cruz, August, 2004
// | 
// | 
// | Description:
// |							This file contains the GPsync Class Implementation.
// | 
// +-----------------------------------------------------------------------

// +---------------------------
// |	Includes
// +---------------------------

#include <GPpch.h>
#include <GPsync.h>

// +--------------------
// |	Constructors
// +--------------------

// +-------------------------------------------------------------
// |
// | Function        : GPsync::GPsync
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPsync::GPsync():
ownCriticalSection_ ( true ),
pCriticalSection_ (NULL)
{
	pCriticalSection_ = new CRITICAL_SECTION();
	InitializeCriticalSection( pCriticalSection_ );
}

// +--------------------
// |	Destructor
// +--------------------

// +-------------------------------------------------------------
// |
// | Function        : GPsync::~GPsync
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPsync::~GPsync()
{
	if ( ownCriticalSection_ && pCriticalSection_)
	{
		DeleteCriticalSection( pCriticalSection_ );
		delete ( pCriticalSection_ );
	}

	pCriticalSection_ = NULL;
	ownCriticalSection_ = false;
}

// +---------------------------
// |	Instance. Singleton Method
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPsync::GetInstance
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
GPsync& GPsync::GetInstance()
{
	static GPsync instance;
	return instance;
}

// +---------------------------
// |	Synchronization
// +---------------------------

// +-------------------------------------------------------------
// |
// | Function        : GPsync::SetCriticalSection
// | Description     : 
// |
// | Parameters : 
// |   pCriticalSection : 
// | 
// +-------------------------------------------------------------
void GPsync::SetCriticalSection(CRITICAL_SECTION* pCriticalSection)
{
	if ( 
			pCriticalSection			&& 
			(pCriticalSection!=pCriticalSection_)
		 )
	{
		if ( ownCriticalSection_ && pCriticalSection_)
		{
			DeleteCriticalSection( pCriticalSection_ );
			delete ( pCriticalSection_ );
			pCriticalSection_ = NULL;
			ownCriticalSection_ = false;
		}
		ownCriticalSection_ = false;
		pCriticalSection_ = pCriticalSection;
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPsync::GetCriticalSection
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
CRITICAL_SECTION* GPsync::GetCriticalSection() const
{
	return pCriticalSection_;
}

// +-------------------------------------------------------------
// |
// | Function        : GPsync::Lock
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPsync::Lock()
{
	if ( pCriticalSection_ )
	{
		EnterCriticalSection(pCriticalSection_ );
	}
}

// +-------------------------------------------------------------
// |
// | Function        : GPsync::UnLock
// | Description     : 
// |
// | Parameters : 
// | 
// +-------------------------------------------------------------
void GPsync::UnLock()
{
	if ( pCriticalSection_ )
	{
		LeaveCriticalSection( pCriticalSection_ );
	}
}
