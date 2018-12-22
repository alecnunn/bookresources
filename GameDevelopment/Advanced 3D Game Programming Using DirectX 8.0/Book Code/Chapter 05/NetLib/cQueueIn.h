/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

// cQueueIn.h: interface for the cQueueIn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CQUEUEIN_H__4A115C42_D1D4_11D3_AE4F_00E029031C67__INCLUDED_)
#define AFX_CQUEUEIN_H__4A115C42_D1D4_11D3_AE4F_00E029031C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "cMonitor.h"
#include "cDataPacket.h"
#include <list>
using namespace std;


class cQueueIn : public cMonitor
{
protected:
  list<cDataPacket *> d_packetsOrdered;
  list<cDataPacket *> d_packetsUnordered;
  DWORD               d_currentPacketID,
                      d_count;  // number of packets added to this queue.

public:
	cQueueIn();
	virtual ~cQueueIn();

	void        Clear();
	void        AddPacket( DWORD packetID, char *pData, unsigned short len, DWORD receiveTime );
	cDataPacket *GetPacket();
  bool        UnorderedPacketIsQueued( DWORD packetID );
  DWORD       GetHighestID();


  DWORD GetCurrentID()
  {
    return d_currentPacketID;
  }


  DWORD GetCount()
  {
    return d_count;
  }
};

#endif // !defined(AFX_CQUEUEIN_H__4A115C42_D1D4_11D3_AE4F_00E029031C67__INCLUDED_)
