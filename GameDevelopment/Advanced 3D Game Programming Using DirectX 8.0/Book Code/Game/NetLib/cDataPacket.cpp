// cDataPacket.cpp: implementation of the cDataPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "cDataPacket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cDataPacket::cDataPacket()
{
  d_timesSent = 1;
}

cDataPacket::~cDataPacket()
{

}


void cDataPacket::Init( DWORD time, DWORD id, unsigned short len, char *pData )
{
  d_firstTime = time;
  d_lastTime = time;
  d_id = id;
  d_length = len;
  memcpy( d_data, pData, len );
}


cDataPacket &cDataPacket::operator=( const cDataPacket &otherPacket )
{
  d_timesSent = otherPacket.d_timesSent;
  d_firstTime = otherPacket.d_firstTime;
  d_lastTime = otherPacket.d_lastTime;
  d_id = otherPacket.d_id;
  d_length = otherPacket.d_length;
  memcpy( d_data, otherPacket.d_data, d_length );

  return *this;
}

