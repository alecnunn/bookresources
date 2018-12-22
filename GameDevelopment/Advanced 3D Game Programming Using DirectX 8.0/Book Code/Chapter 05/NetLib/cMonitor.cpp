/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

// cMonitor.cpp: implementation of the cMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "cMonitor.h"
#include "cNetError.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cMonitor::cMonitor()
{
  // This mutex will help the two threads share their toys.
  d_mutex = CreateMutex( NULL, false, NULL );
  if( d_mutex == NULL )
    throw cNetError( "cMonitor() - Mutex creation failed." );
}

cMonitor::~cMonitor()
{
  if( d_mutex != NULL )
  {
    CloseHandle( d_mutex );
    d_mutex = NULL;
  }
}


void cMonitor::MutexOn() const
{
  WaitForSingleObject( d_mutex, INFINITE );  // To be safe...
}


void cMonitor::MutexOff() const
{
  ReleaseMutex( d_mutex );  // To be safe...
}

