// cUnreliableQueueOut.h: interface for the cUnreliableQueueOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUNRELIABLEQUEUEOUT_H__8E4CCAC2_D4DB_11D3_AE4F_00E029031C67__INCLUDED_)
#define AFX_CUNRELIABLEQUEUEOUT_H__8E4CCAC2_D4DB_11D3_AE4F_00E029031C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "cMonitor.h"
#include "cDataPacket.h"
#include <list>
using namespace std;


class cUnreliableQueueOut : public cMonitor
{
  list<cDataPacket *> d_packets;
  DWORD               d_currentPacketID;
  unsigned char       d_maxPackets,
                      d_numPackets;

public:
	cUnreliableQueueOut();
	virtual ~cUnreliableQueueOut();

	void  Clear();
	void  AddPacket( char *pData, unsigned short len );
  bool  GetPreviousPacket( DWORD packetID, cDataPacket *pPacket );
  void  SetMaxPackets( unsigned char maxPackets );


  DWORD GetCurrentID()
  {
    return d_currentPacketID;
  }
};

#endif // !defined(AFX_CUNRELIABLEQUEUEOUT_H__8E4CCAC2_D4DB_11D3_AE4F_00E029031C67__INCLUDED_)
