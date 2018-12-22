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

#include "NetworkPCH.h"
#include "TcpClientSock.h"
#include "UpdateChain/UpdateChain.h"

namespace Network {

//---------------------------------------------------------------------

TcpClientSock::TcpClientSock(const Address & r) :
TcpSock()
{
	// establish a connection with the remote
	handle = ps_create_tcp_socket(group, this);
	ps_queue_connect(handle, &r.getSockAddr());
}

//---------------------------------------------------------------------

TcpClientSock::TcpClientSock(const ps_socket * h, const Address & r, const Address & l) :
TcpSock(l),
remoteAddress(r)
{
	handle = h;
	ps_queue_recv(handle, receiveBuffer, sizeof(receiveBuffer));
}

//---------------------------------------------------------------------

TcpClientSock::~TcpClientSock()
{
	disconnect();
}

//---------------------------------------------------------------------

void TcpClientSock::disconnect() const
{
	ps_destroy_socket(handle);
}

//---------------------------------------------------------------------

void TcpClientSock::onConnected(const Address & r)
{
	remoteAddress = r;
	onOpened();
	TcpEvent e(TcpEvent::OPENED, *this);
	eventHandler.emitMessage(e);
}

//---------------------------------------------------------------------

void TcpClientSock::onReceive(const Archive::Stream::ReadIterator & data)
{
	inputHandler.emitMessage(TcpMessage(*this, data));
}

//---------------------------------------------------------------------

const bool TcpClientSock::send(const Archive::Stream & data) const
{
	return ps_queue_send(handle, data.getBuffer(), data.getSize());
}

//---------------------------------------------------------------------

void TcpClientSock::update()
{
	static ps_group g = TcpSock::getGroup();
	static ps_recv_result recvResult;
	static Archive::Stream data;

	if(ps_has_events(g))
	{
		ps_connect_result result;
		if(ps_query_connect(g, &result))
		{
			TcpClientSock * c = reinterpret_cast<TcpClientSock *>(result.s->userData);
			c->onConnected(result.remote);				
		}
		while(ps_query_recv(g, &recvResult))
		{
			TcpClientSock * t = reinterpret_cast<TcpClientSock *>(recvResult.s->userData);
			data.clear();
			data.put(recvResult.data, recvResult.length);
			t->onReceive(data.begin());
			ps_queue_recv(t->handle, t->receiveBuffer, sizeof(t->receiveBuffer));
		}
	}

	static ps_error_result errors;
	while(ps_query_error(g, &errors))
	{
		switch(errors.err)
		{
		case ps_error_result::NO_PENDING_OPERATIONS:
			break;
		case ps_error_result::CONNECTION_CLOSED:
			{
				if(errors.s)
				{
					TcpClientSock * s = reinterpret_cast<TcpClientSock *>(errors.s->userData);
					if(s)
					{
						TcpEvent e(TcpEvent::CLOSED, *s);
						s->eventHandler.emitMessage(e);
						s->onClosed();
					}
				}
			}
			break;
		default:
			assert(false); // handle errors!
		}
	}
}

//---------------------------------------------------------------------

}//namespace Network
