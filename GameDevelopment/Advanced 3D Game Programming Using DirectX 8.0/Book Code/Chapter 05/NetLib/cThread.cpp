/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

// cThread.cpp: implementation of the cThread class.
//
//////////////////////////////////////////////////////////////////////

#include "cThread.h"
#include "cNetError.h"

//////////////////////////////////////////////////////////////////////

static DWORD WINAPI cThreadProc( cThread *pThis )
{
  return pThis->ThreadProc();
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cThread::cThread() : cMonitor()
{
  d_threadID = 0;
  d_threadHandle = NULL;
  d_bIsRunning = false;
}


cThread::~cThread()
{
  End();
}


void cThread::Begin()
{
  // Start the thread.
  d_threadHandle = CreateThread( NULL,
                                 0,
                                 (LPTHREAD_START_ROUTINE)cThreadProc,
                                 this,
                                 0,
                                 (LPDWORD)&d_threadID );
  if( d_threadHandle == NULL )
    throw cNetError( "cThread() - Thread creation failed." );
  d_bIsRunning = true;
}


void cThread::End()
{
  if( d_threadHandle != NULL )
  {
    d_bIsRunning = false;
    WaitForSingleObject( d_threadHandle, INFINITE );
    CloseHandle( d_threadHandle );
    d_threadHandle = NULL;
  }
}


DWORD cThread::ThreadProc()
{
  return 0;
}
////////////////////////////////////////////////////////////////////////////////

void cThread::SetError( const char *pBuffer )
{
  if( pBuffer == NULL )
    strcpy( d_errorBuffer, "Unspecified." );
  else
  {
    if( sizeof( d_errorBuffer ) > strlen( pBuffer ) )
      strcpy( d_errorBuffer, pBuffer );
    else
    {
      strncpy( d_errorBuffer, pBuffer, sizeof( d_errorBuffer ) );
      d_errorBuffer[ sizeof( d_errorBuffer ) - 1 ] = '\0';
    }
  }
}


const char *cThread::Error()
{
  return (const char *)d_errorBuffer;
}


bool cThread::IsRunning()
{
  return d_bIsRunning;
}

