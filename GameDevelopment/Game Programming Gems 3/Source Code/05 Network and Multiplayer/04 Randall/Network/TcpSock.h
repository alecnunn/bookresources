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

#ifndef	_TcpSock_H
#define	_TcpSock_H

//---------------------------------------------------------------------

#include "Dispatch/Transceiver.h"
#include "Network/Address.h"
#include "Network/Message.h"
#include "Network/Sock.h"
#include "Archive/Stream.h"

namespace Network {

class TcpClientSock;
class TcpServerSock;

//---------------------------------------------------------------------

struct TcpClientConnected
{
	TcpClientConnected(TcpClientSock & c) : client(c) {};
	TcpClientConnected & operator = (const TcpClientConnected);
	TcpClientSock & client;
};

//---------------------------------------------------------------------

struct TcpMessage
{
	TcpMessage(const class TcpSock & source, const Archive::Stream::ReadIterator & data);
	TcpMessage & operator = (const TcpMessage & rhs);
	const Archive::Stream::ReadIterator & data;
	const class TcpSock & source;
};

struct TcpEvent
{
	enum Events
	{
		OPENED,
		CLOSED
	};
	TcpEvent(const Events e, TcpClientSock & s) : event(e), sock(s) {};
	TcpEvent & operator = (const TcpEvent & rhs);

	Events     event;
	TcpClientSock &  sock;
};

//---------------------------------------------------------------------

class TcpSock : public Sock
{
public:
	Dispatch::Transceiver<TcpEvent &> &            getEventHandler    ();
	Dispatch::Transceiver<const TcpMessage &> &    getInputHandler    ();
	const bool                                     isConnected        () const;
//	virtual const bool                             send               (const Archive::Stream & data) const;

protected:
	void          close         ();
	static void   flushSockets  ();

protected:
	virtual             ~TcpSock      () = 0;
	                    TcpSock       ();
	explicit			TcpSock       (const Address & local);
	static ps_group     getGroup      ();
	void                onClosed      ();
	void                onOpened      ();
	void                setConnected  (const bool connectedState);

protected:
	Dispatch::Transceiver<TcpEvent &>    eventHandler;
	ps_group         group;
	Dispatch::Transceiver<const TcpMessage &>  inputHandler;
	unsigned char    receiveBuffer[1400];

private:
	              TcpSock     (const TcpSock & source);
	TcpSock &     operator =  (const TcpSock &  rhs);

private:
	bool                                       connected;
	struct Packet
	{
		Packet();
		unsigned long    length;
		Archive::Stream  data;
	};

};

//---------------------------------------------------------------------

inline const bool TcpSock::isConnected() const
{
	return connected;
}

//---------------------------------------------------------------------

inline Dispatch::Transceiver<TcpEvent &> & TcpSock::getEventHandler()
{
	return eventHandler;
}

//---------------------------------------------------------------------

inline Dispatch::Transceiver<const TcpMessage &> & TcpSock::getInputHandler()
{
	return inputHandler;
}

//---------------------------------------------------------------------

inline void TcpSock::setConnected(const bool connectedState)
{
	connected = connectedState;
}

//---------------------------------------------------------------------

}//namespace Network

#endif	// _TcpSock_H
