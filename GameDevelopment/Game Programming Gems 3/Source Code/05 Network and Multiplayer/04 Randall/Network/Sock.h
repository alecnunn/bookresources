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

#ifndef	_Sock_H
#define	_Sock_H

//---------------------------------------------------------------------

#include "NetworkPlatform.h"
#include "Network/push_socket.h"

namespace Network {

class Address;

//---------------------------------------------------------------------

class Sock 
{
public:
	                         Sock             ();
	explicit				 Sock             (const Address & bindAddress);
	virtual                  ~Sock            () = 0;
	virtual const bool       bind             ();
	const Network::Address & getBindAddress   () const;
	const unsigned long int  getBytesReceived () const;
	const unsigned long int  getBytesSent     () const;
	const ps_socket *        getHandle        () const;
	const unsigned int       getMtu           () const;

protected:
	void  addBytesReceived  (const unsigned long int bytesReceived) const; // changes mutable, make it const
	void  addBytesSent      (const unsigned long int bytesSent) const;         // changes mutable, make it const

protected:
	Address                    bindAddress;
	const ps_socket *  handle;
	class SockGroup
	{
	public:
		SockGroup();
		~SockGroup();
		ps_group getGroup() const;
	private:
		ps_group group;
	};
	
private:
	        Sock        (const Sock & source);
	Sock &  operator =  (const Sock &  rhs);

private:
	mutable unsigned long int  bytesReceived;
	mutable unsigned long int  bytesSent;
	unsigned int               mtu;
};

//---------------------------------------------------------------------

inline void Sock::addBytesReceived(const unsigned long int moreBytesReceived) const
{
	bytesReceived += moreBytesReceived;
}

//---------------------------------------------------------------------

inline void Sock::addBytesSent(const unsigned long int moreBytesSent) const
{
	bytesSent += moreBytesSent;
}

//---------------------------------------------------------------------

inline const Address & Sock::getBindAddress() const
{
	return bindAddress;
}

//---------------------------------------------------------------------

inline const unsigned long int Sock::getBytesReceived() const
{
	return bytesReceived;
}

//---------------------------------------------------------------------

inline const unsigned long int Sock::getBytesSent() const
{
	return bytesSent;
}

//---------------------------------------------------------------------

inline const ps_socket * Sock::getHandle() const
{
	return handle;
}

//---------------------------------------------------------------------

inline const unsigned int Sock::getMtu() const
{
	return mtu;
}

//---------------------------------------------------------------------

}//namespace Network

#endif	// _Sock_H
