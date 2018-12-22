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
#include <cstdio>
#include <netdb.h>
#include "Network/Address.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Network {

//---------------------------------------------------------------------

void addressToString(const struct sockaddr_in & address, std::string & target)
{
	char addr[24];
	const unsigned char * bytes = reinterpret_cast<const unsigned char *>(&address.sin_addr.s_addr);
	int result = snprintf(
		addr, 
		24,
		"%i.%i.%i.%i:%i", 
		bytes[0],
		bytes[1],
		bytes[2],
		bytes[3],
		ntohs(address.sin_port));
	assert(result <=24);
	target = addr;
}

//---------------------------------------------------------------------

Address::Address() :
humanReadableAddress("UNINTIALIZED ADDRESS"),
sockAddr()
{
	memset(&sockAddr, 0, sizeof(struct sockaddr_in));
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = 0;
	addressToString(sockAddr, humanReadableAddress);
}

//---------------------------------------------------------------------

Address::Address(const Address & source) :
humanReadableAddress(source.humanReadableAddress),
sockAddr(source.sockAddr)
{
}

//---------------------------------------------------------------------

Address::Address(const struct sockaddr_in & source) :
humanReadableAddress("UNINITIALIZED ADDRESS"),
sockAddr(source)
{
	addressToString(sockAddr, humanReadableAddress);
}

//---------------------------------------------------------------------

Address::Address(const std::string & addressName, const unsigned short port) :
humanReadableAddress("UNINITIALIZED ADDRESS"),
sockAddr()
{
	memset(&sockAddr, 0, sizeof(struct sockaddr_in));
	sockAddr.sin_addr.s_addr = INADDR_ANY;

	if(addressName.length() > 0)
	{
		if(!isalpha(addressName.c_str()[0]))
		{
			// it may be a dotted-decimal IP address...
			sockAddr.sin_addr.s_addr = inet_addr(addressName.c_str());
		}
		else
		{
			struct hostent * h = gethostbyname(addressName.c_str());
			if(h == 0)
			{
				// @todo implement error handler!
				assert(false);
			}
			else
			{
				memcpy(&sockAddr.sin_addr, h->h_addr_list[0], sizeof(sockAddr.sin_addr));
			}
		}
	}
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	addressToString(sockAddr, humanReadableAddress);
}

//---------------------------------------------------------------------

Address::~Address()
{
}

//---------------------------------------------------------------------

Address & Address::operator = (const Address & rhs)
{
	if(&rhs != this)
	{
		humanReadableAddress = rhs.humanReadableAddress;
		sockAddr = rhs.sockAddr;
	}
	return *this;
}

// ----------------------------------------------------------------------

Address & Address::operator = (const struct sockaddr_in & rhs)
{
	addressToString(sockAddr, humanReadableAddress);
	sockAddr = rhs;
	return *this;
}

//---------------------------------------------------------------------

}//namespace Network
