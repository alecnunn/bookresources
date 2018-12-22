/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

// cNetClock.h: interface for the cNetClock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CNETCLOCK_H__1E011A05_D341_11D3_AE4F_00E029031C67__INCLUDED_)
#define AFX_CNETCLOCK_H__1E011A05_D341_11D3_AE4F_00E029031C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "cMonitor.h"


struct cTimePair
{
public:
  DWORD d_actual,  // The actual time as reported by GetTickCount()
        d_clock;  // The clock time as determined by the server.
};


class cNetClock : public cMonitor  
{
  cTimePair d_start,  // The first time set by the server.
            d_lastUpdate;  // the last updated time set by the server.
  bool      d_bInitialized;  // first time has been received.
  double    d_ratio;
  // The difference as measured by
  //  d_lastUpdated.d_clock - d_start.d_clock
  // -----------------------------------------
  // d_lastUpdated.d_actual - d_start.d_actual

public:
	cNetClock();
	virtual ~cNetClock();

  void  Init();
  void  Synchronize( DWORD serverTime, DWORD packetSendTime, DWORD packetACKTime, float ping );
  DWORD GetTime() const;
  DWORD TranslateTime( DWORD time ) const;
};

#endif // !defined(AFX_CNETCLOCK_H__1E011A05_D341_11D3_AE4F_00E029031C67__INCLUDED_)
