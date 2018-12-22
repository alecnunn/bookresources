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

#ifndef	_Address_H
#define	_Address_H

//---------------------------------------------------------------------

#include "Network/NetworkPlatform.h"
#include <string>

namespace Network {

//---------------------------------------------------------------------
/** @brief Abstract IPv4 Network Addresses

	The Address class encapsulates IPv4 addressing. It will build an
	IPv4 compliant socket address from source Address objects, 
	human-readable dotted-deciman or hostname ip strings or other
	BSD sockaddr structs.

	All components of the network that send messages, or connect to
	an endpoint use the Address class to identify targets.

	\code
	void foo()
	{
		Network::Address myAddress("www.somegameplace.com", 4343);
		Network::TcpClient t(myAddress); // connect to remote
	}
	\endcode

	@author Justin Randall
*/
class Address
{
public:
	Address();
	explicit Address(const Address & source);
	Address(const struct sockaddr_in & source);
	Address(const std::string & humanReadableAddress, const unsigned short port);
	virtual ~Address();
	Address & operator = (const Address &  rhs);
	Address & operator = (const struct sockaddr_in & rhs);
	const bool operator == (const Address & rhs) const;
	const bool operator != (const Address & rhs) const;
	const bool operator < (const Address & rhs) const;
	const std::string &         getHumanReadable() const;
	const unsigned short        getPort() const;
	const struct sockaddr_in &  getSockAddr() const;

	struct Hash
	{
		const size_t operator () (const Address & source) const 
		{ 
			return source.getSockAddr().sin_addr.s_addr; 
		};
	};

private:
	std::string         humanReadableAddress;
	struct sockaddr_in  sockAddr;
};

//---------------------------------------------------------------------

inline const bool Address::operator < (const Address & rhs) const
{
	return (sockAddr.sin_addr.s_addr < rhs.sockAddr.sin_addr.s_addr && 
		sockAddr.sin_port < rhs.sockAddr.sin_port);
}

//---------------------------------------------------------------------

inline const bool Address::operator == (const Address & rhs) const
{
	return(
		sockAddr.sin_addr.s_addr == rhs.sockAddr.sin_addr.s_addr &&
		sockAddr.sin_family == rhs.sockAddr.sin_family &&
		sockAddr.sin_port == rhs.sockAddr.sin_port &&
		sockAddr.sin_zero == rhs.sockAddr.sin_zero
		);
}

//---------------------------------------------------------------------

inline const bool Address::operator != (const Address & rhs) const
{
	return(
		sockAddr.sin_addr.s_addr != rhs.sockAddr.sin_addr.s_addr ||
		sockAddr.sin_family != rhs.sockAddr.sin_family ||
		sockAddr.sin_port != rhs.sockAddr.sin_port ||
		sockAddr.sin_zero != rhs.sockAddr.sin_zero
		);
}

//---------------------------------------------------------------------

inline const std::string & Address::getHumanReadable() const
{
	return humanReadableAddress;
}

//---------------------------------------------------------------------

inline const unsigned short Address::getPort() const
{
	return ntohs(sockAddr.sin_port);
}

//---------------------------------------------------------------------

inline const struct sockaddr_in & Address::getSockAddr() const
{
	return sockAddr;
}

//---------------------------------------------------------------------

}//namespace Network

#endif	// _Address_H
