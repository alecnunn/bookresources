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
#include "UdpSock.h"
#include "UpdateChain/UpdateChain.h"
#include "push_socket.h"

namespace Network {

//---------------------------------------------------------------------

class Updater : public UpdateChain::UpdateChain::UpdateObject
{
public:
	Updater();
	~Updater();
	void update();
};

//---------------------------------------------------------------------

Updater::Updater() :
UpdateObject()
{
}

//---------------------------------------------------------------------

Updater::~Updater()
{
}

//---------------------------------------------------------------------

void Updater::update()
{
	UdpSock::update();
}

//---------------------------------------------------------------------

UdpSock::UdpSock() :
group(UdpSock::getGroup())
{
	handle = ps_create_udp_socket(group, this);
}

//---------------------------------------------------------------------

UdpSock::UdpSock(const Address & bindAddress) :
group(UdpSock::getGroup)
{
	handle = ps_create_udp_socket(group, this);
	bind(bindAddress);
}

//---------------------------------------------------------------------

UdpSock::UdpSock(const unsigned short port) :
group(UdpSock::getGroup())
{
	handle = ps_create_udp_socket(group, this);
	bind(port);
}

//---------------------------------------------------------------------

UdpSock::~UdpSock()
{
}

//---------------------------------------------------------------------

const bool UdpSock::bind(const Address & bindAddress)
{
	bool result = false;
	result = ps_bind_socket(handle, &bindAddress.getSockAddr());
	if(result)
		ps_queue_recvfrom(handle, receiveBuffer, sizeof(receiveBuffer));
	return result;
}

//---------------------------------------------------------------------

const bool UdpSock::bind(const unsigned short port)
{
	Address bindAddr("0.0.0.0", port);
	return bind(bindAddr);
}

//---------------------------------------------------------------------

const bool UdpSock::broadcast(const unsigned short port, const Archive::Stream & data) const
{
	static Address broadcastAddress("255.255.255.255", port);
	return sendTo(broadcastAddress, data);
}

//---------------------------------------------------------------------

ps_group UdpSock::getGroup()
{
	static Updater u;
	static Sock::SockGroup s;
	return s.getGroup();
}

//---------------------------------------------------------------------

Dispatch::Transceiver<const UdpMessage &> & UdpSock::getInputHandler()
{
	return inputHandler;
}

//---------------------------------------------------------------------

void UdpSock::onReceive(const Address & sourceAddress, const Archive::Stream::ReadIterator & data) const
{
	inputHandler.emitMessage(UdpMessage(sourceAddress, data));
}

//---------------------------------------------------------------------

const bool UdpSock::sendTo(const Address & remoteAddress, const Archive::Stream & data) const
{
	return ps_queue_sendto(handle, &remoteAddress.getSockAddr(), data.getBuffer(), data.getSize());
}

//---------------------------------------------------------------------

void UdpSock::update()
{
	static ps_group g = UdpSock::getGroup();
	static ps_recvfrom_result recvResult;
	static Address remoteAddress;
	static Archive::Stream data;

	while(ps_query_recvfrom(g, &recvResult))
	{
		UdpSock * u = reinterpret_cast<UdpSock *>(recvResult.s->userData);
		remoteAddress = recvResult.remote;
		data.clear();
		data.put(recvResult.data, recvResult.length);
		u->onReceive(remoteAddress, data.begin());
		ps_queue_recvfrom(u->handle, u->receiveBuffer, sizeof(u->receiveBuffer));
	}

	static ps_error_result errors;
	while(ps_query_error(g, &errors))
	{
		if(errors.s)
		{
			UdpSock * u = (UdpSock *)(errors.s->userData);
			if(u)
			{
				u->errorHandler.emitMessage(errors);
			}
		}
		//assert(false); // handle errors!
	}
}

//---------------------------------------------------------------------

}//namespace Network
