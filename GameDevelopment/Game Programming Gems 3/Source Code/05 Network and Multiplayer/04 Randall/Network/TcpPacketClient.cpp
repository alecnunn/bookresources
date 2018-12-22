/*
Copyright (c) 2001 by Justin Randall.  All rights reserved.
This copyright applies only to this particular software distribution 
package, and does not imply a copyright on any particular module in 
the library.

Permission to use, copy, modify, and distribute this compilation for
non-commercial use is hereby granted without fee, subject to 
the following license:

1. Any copy or modification of this compilation must include the above
copyright notice and this license.

2. Users of the software included in this compilation agree to use
their best efforts to provide Justin Randall with any modifications containing
improvements or extensions and hereby grant Justin Randall a perpetual,
royalty-free license to use and distribute such modifications under
the terms of this license.

3. Justin Randall makes no warranty or representation that the operation of the
software in this compilation will be error-free, and Justin Randall is under no
obligation to provide any services, by way of maintenance, update, or
otherwise.  THE SOFTWARE AND ANY DOCUMENTATION ARE PROVIDED 'AS IS'
WITHOUT EXPRESS OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. IN NO EVENT WILL JUSTIN RANDALL OR ANY OTHER CONTRIBUTOR BE LIABLE FOR
DIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

4. Users will not use Justin Randall or any other contributor's name in any 
publicity or advertising, without prior written consent in each case.

5. Export of this software from the United States may require a
specific license from the United States Government.  It is the
responsibility of any person or organization contemplating export
to obtain such a license before exporting.

6. Certain parts of this software may be protected by patents.  It
is the users' responsibility to obtain the appropriate
licenses before using those parts.
*/

//---------------------------------------------------------------------

#include "NetworkPCH.h"
#include "TcpPacketClient.h"

namespace Network {

//---------------------------------------------------------------------

TcpPacketClient::TcpPacketClient(const ps_socket * h, const Address & r, const Address & l) :
TcpClientSock(h, r, l),
inFragment(),
inFragmentSize(0),
inPacket(),
outPacket()
{
}

//---------------------------------------------------------------------

TcpPacketClient::TcpPacketClient(const Address & r) :
TcpClientSock(r),
inFragment(),
inFragmentSize(0),
inPacket(),
outPacket()
{
}

//---------------------------------------------------------------------

TcpPacketClient::~TcpPacketClient()
{
}

//---------------------------------------------------------------------

void TcpPacketClient::onReceive(const Archive::Stream::ReadIterator & data)
{
	Archive::Stream::ReadIterator r = data;
	while(r.getSize() > 0)
	{
		if(inPacket.length == 0)
		{
			if(inFragmentSize > 0)
			{
				// It is the worst case (computationally) and a rare 
				// occurrance when a Packet is fragmented on the 4 
				// bytes that indicate Packet size. TcpSock handles 
				// this by keeping a 4 byte fragment buffer for the 
				// Packet header, and building the buffer until the 
				// Packet size is determined.
				unsigned long remaining = 4 - inFragmentSize;
				if(remaining <= r.getSize())
				{
					memcpy(&inFragment[inFragmentSize], r.getBuffer(), remaining);
					r.advance(remaining);

					// set inPacket length
					memcpy(&inPacket.length, inFragment, 4);
					inFragmentSize = 0;
					continue;
				}
				else
				{
					memcpy(&inFragment[inFragmentSize], r.getBuffer(), r.getSize());
					inFragmentSize += r.getSize();
				}
			}
			else
			{
				// there was no remaining Packet size fragment from a 
				// previous partial receive. Perhaps there will be no 
				// fragment now either.
				if(r.getSize() >= 4)
				{
					// no fragment on Packet size, this should be
					// the common path for a new Packet received
					Archive::get(r, inPacket.length);
					continue;
				}
				else
				{
					// no fragments, the remaining data
					// does not contain the whole Packet
					// header
					inFragmentSize = r.getSize();
					memcpy(inFragment, r.getBuffer(), inFragmentSize);
					r.advance(inFragmentSize);
					continue;
				}
			}
		}
		else
		{
			// the size of the Packet is known, put the data into
			// the inPacket data until the whole Packet is received
			// or put as much data as is available into the input
			// buffer and defer delivery until the whole Packet
			// has arrived.
			unsigned long bytesRemaining = inPacket.length - inPacket.data.getSize();
			if(r.getSize() >= bytesRemaining)
			{
				// it's possible to build a complete Packet to deliver
				// to the application.
				inPacket.data.put(r.getBuffer(), bytesRemaining);
				r.advance(bytesRemaining);
				inputHandler.emitMessage(TcpMessage(*this, inPacket.data.begin()));
				inPacket.length = 0;
				inPacket.data.clear();
				continue;
			}
			else
			{
				// only a fragment is available, put it in the 
				// input Packet data archive and continue
				Archive::put(inPacket.data, r);
				continue;
			}
		}
	}
}

//---------------------------------------------------------------------

const bool TcpPacketClient::send(const Archive::Stream & data) const
{
	outMessage.clear();
	Archive::put(outMessage, data.getSize());
	outMessageReadIterator = data.begin();
	Archive::put(outMessage, outMessageReadIterator);
	return TcpClientSock::send(outMessage);	
}

//---------------------------------------------------------------------

}//namespace Network
