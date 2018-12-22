/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

// cNetClock.cpp: implementation of the cNetClock class.
//
//////////////////////////////////////////////////////////////////////

#include "cNetClock.h"
#include "Macros.h"
#if defined( _DEBUG_CLOCK )
#include <stdio.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cNetClock::cNetClock() : cMonitor()
{
  Init();
}

cNetClock::~cNetClock()
{

}


void cNetClock::Init()
{
  d_bInitialized = false;
  d_ratio = 1.0;

  d_start.d_actual = GetTickCount();
  d_start.d_clock = 0;
  d_lastUpdate.d_actual = d_start.d_actual;
  d_lastUpdate.d_clock = d_start.d_clock;
}


void cNetClock::Synchronize( DWORD serverTime, DWORD packetSendTime, DWORD packetACKTime, float ping )
{
  cMonitor::MutexOn();

  DWORD dt;

  dt = packetACKTime - packetSendTime;

  if( dt > 10000 )
  {
    cMonitor::MutexOff();
    return;
  }

  if( d_bInitialized == true )
  {
    // if the packet ACK time was too long OR the clock is close enough
    // then do not update the clock.
    if( abs( serverTime + ( dt / 2 ) - GetTime() ) <= 5 )
    {
      cMonitor::MutexOff();
      return;
    }

    d_lastUpdate.d_actual = packetACKTime;
    d_lastUpdate.d_clock = serverTime + (DWORD)( ping / 2 );
    d_ratio = (double)( d_lastUpdate.d_clock - d_start.d_clock ) / 
              (double)( d_lastUpdate.d_actual - d_start.d_actual );

#if defined( _DEBUG_CLOCK )
    OUTPUTREPORT1( "clock: synch to %d\n", d_lastUpdate.d_clock );
#endif
  }
  else
  {
    d_lastUpdate.d_actual = packetACKTime;
    d_lastUpdate.d_clock = serverTime + ( dt / 2 );
    d_start.d_actual = d_lastUpdate.d_actual;
    d_start.d_clock = d_lastUpdate.d_clock;
    d_bInitialized = true;

#if defined( _DEBUG_CLOCK )
    OUTPUTREPORT1( "clock: first synch to %d\n", d_lastUpdate.d_clock );
#endif
  }

  cMonitor::MutexOff();
}


DWORD cNetClock::GetTime() const
{
  DWORD serverTime;

  cMonitor::MutexOn();

  serverTime = (DWORD)( (double)( GetTickCount() - d_lastUpdate.d_actual ) /** d_ratio*/ ) + d_lastUpdate.d_clock;

  cMonitor::MutexOff();

  return serverTime;
}


DWORD cNetClock::TranslateTime( DWORD localTime ) const
{
  DWORD serverTime;

  cMonitor::MutexOn();

  serverTime = (DWORD)( (double)( localTime - d_lastUpdate.d_actual ) /** d_ratio*/ ) + d_lastUpdate.d_clock;

  cMonitor::MutexOff();

  return serverTime;
}
