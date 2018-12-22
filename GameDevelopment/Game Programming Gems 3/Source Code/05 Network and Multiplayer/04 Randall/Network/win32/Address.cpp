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
#include "Network/Address.h"
#include <winsock2.h>

namespace Network {

//---------------------------------------------------------------------

void addressToString(const struct sockaddr_in & address, std::string & target)
{
	char addr[24];
	const unsigned char * bytes = reinterpret_cast<const unsigned char *>(&address.sin_addr.s_addr);
	int result;
	result = _snprintf(
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
	
	if(!isalpha(addressName.c_str()[0]))
	{
		// it may be a dotted-decimal IP address...
		int addressLen = sizeof(struct sockaddr_in);
		if(addressName == "255.255.255.255")
		{
			sockAddr.sin_addr.s_addr = INADDR_BROADCAST;
		}
		else if (addressName.length() == 0 || addressName == "0.0.0.0")
		{
			sockAddr.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			// WSAStringToAddress is NOT const correct! Docs say that 
			// address string is an [in] parameter and is not changed
			int result = WSAStringToAddress(const_cast<char *>(addressName.c_str()), AF_INET, NULL, reinterpret_cast<struct sockaddr *>(&sockAddr), &addressLen);
			if(result != 0)
			{
				int errCode = WSAGetLastError();
				switch(errCode)
				{
				case WSAEFAULT:
					OutputDebugString("The specified Address buffer is too small. Pass in a larger buffer.\n");
					break;
				case WSAEINVAL:
					OutputDebugString("Unable to translate the string into a SOCKADDR.\n");
					break;
				case WSANOTINITIALISED:
					OutputDebugString("The WS2_32.DLL has not been initialized. The application must first call WSAStartup before calling any Windows Socket functions.\n");
					break;
				default:
					OutputDebugString("An unknown error occurred while processing WSAStringToAddress().\n");
					break;
				}
			}
			assert(result == 0);
		}
	}
	else
	{
		HOSTENT * h = gethostbyname(addressName.c_str());
		if(h == 0)
		{
			int errCode = WSAGetLastError();
			switch(errCode)
			{
			case WSANOTINITIALISED:
				OutputDebugString("A successful WSAStartup must occur before using this function. \n");
				break;
			case WSAENETDOWN:
				OutputDebugString("The network subsystem has failed. \n");
				break;
			case WSAHOST_NOT_FOUND:
				OutputDebugString("Authoritative Answer Host not found. \n");
				break;
			case WSATRY_AGAIN:
				OutputDebugString("Non-Authoritative Host not found, or server failure. \n");
				break;
			case WSANO_RECOVERY: 
				OutputDebugString("Nonrecoverable error occurred. \n");
				break;
			case WSANO_DATA:
				OutputDebugString("Valid name, no data record of requested type. \n");
				break;
			case WSAEINPROGRESS:
				OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
				break;
			case WSAEFAULT:
				OutputDebugString("The name parameter is not a valid part of the user address space. \n");
				break;
			case WSAEINTR:
				OutputDebugString("A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall. \n");
				break;
			default:
				OutputDebugString("An unkown error occurred while processing gethostbyname()\n");
				break;
			}
		}
		else
		{
			memcpy(&sockAddr.sin_addr, h->h_addr_list[0], sizeof(sockAddr.sin_addr));
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

//---------------------------------------------------------------------

Address & Address::operator = (const struct sockaddr_in & rhs)
{
	sockAddr = rhs;
	addressToString(sockAddr, humanReadableAddress);
	return *this;
}

//---------------------------------------------------------------------

}//namespace Network
