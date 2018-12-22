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

#ifndef	_UdpSock_H
#define	_UdpSock_H

//---------------------------------------------------------------------

#include "Archive/Stream.h"
#include "Network/Address.h"
#include "Network/Sock.h"
#include "Dispatch/Transceiver.h"

//---------------------------------------------------------------------

namespace Network {

struct UdpMessage
{
	UdpMessage(const Address & remoteAddress, const Archive::Stream::ReadIterator & data);
	UdpMessage & operator = (const UdpMessage & rhs);
	const Address & remoteAddress;
	const Archive::Stream::ReadIterator & data;
};

//---------------------------------------------------------------------

inline UdpMessage::UdpMessage(const Address & a, const Archive::Stream::ReadIterator & d) :
remoteAddress(a),
data(d)
{
}

//---------------------------------------------------------------------

/**
	@brief An IP datagram socket

	The UdpSock class implements asynchronous I/O. The interface is 
	platform independant. The underlying implementation exploits 
	platform-specific optimizations for efficient asynchronous I/O.

	To employ raw IP datagram sockets in an application, simply
	derive from this class, implement the onReceive() method, and
	periodically call update(). Sockets with data pending will
	invoke onReceive and provide the source address of the datagram
	and the payload in the Archive::Stream parameter.

	Reading is implicit on the socket. When update() is invoked, 
	the UdpSock will continue to enqueue receives until no more
	data is available on the socket. update() will return when
	all UdpSock objects have pumped their incoming data.

*/
class UdpSock : public Sock
{
public:
	              UdpSock   ();
	explicit      UdpSock   (const Address & bindAddress);
	explicit      UdpSock   (const unsigned short port);
	virtual       ~UdpSock  ();
	const bool    broadcast (const unsigned short port, const Archive::Stream & data) const;
	const bool    bind      (const Address & bindAddress);
	const bool    bind      (const unsigned short port);
	const bool    sendTo    (const Address & remoteAddress, const Archive::Stream & data) const;
	static void   update    ();

	Dispatch::Transceiver<const UdpMessage &> &  getInputHandler  ();

protected:
	virtual void  onReceive(const Address & sourceAddress, const Archive::Stream::ReadIterator & data) const;

private:
	                 UdpSock(const UdpSock & source);
	UdpSock &        operator = (const UdpSock &  rhs);
	static ps_group  getGroup();

private:
	unsigned char                              receiveBuffer[1400];
	ps_group                                   group;
	Dispatch::Transceiver<const UdpMessage &>  inputHandler;
	Dispatch::Transceiver<ps_error_result>     errorHandler;
	bool                                       bound;
};

//---------------------------------------------------------------------

}//namespace Network

#endif	// _UdpSock_H
