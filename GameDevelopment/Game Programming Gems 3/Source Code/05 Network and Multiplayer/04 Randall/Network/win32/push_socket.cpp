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
#pragma warning ( disable : 4786 )
#include "NetworkPCH.h"
#include "Network/push_socket.h"
#include <map>
#include <mswsock.h>
#include <hash_map>
#include <set>
#include <vector>

//---------------------------------------------------------------------

// Something, somewhere in Micorosoft headers prevents the use of
// C with this winsock2, so it's C++ with C linkage.. 

//---------------------------------------------------------------------
class WinsockInstallationObject
{
public:
	WinsockInstallationObject();
	~WinsockInstallationObject();
static WinsockInstallationObject w;
};

WinsockInstallationObject::WinsockInstallationObject()
{
	WORD wVersionRequested = MAKEWORD(2,2);
	WSADATA wsaData;
	int result = WSAStartup(wVersionRequested, &wsaData);
	if(result != 0)
	{
		switch(result)
		{
		case WSASYSNOTREADY:
			OutputDebugString("Indicates that the underlying network subsystem is not ready for network communication. ");
			break;
		case WSAVERNOTSUPPORTED:
			OutputDebugString("The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation. ");
			break;
		case WSAEINPROGRESS:
			OutputDebugString("A blocking Windows Sockets 1.1 operation is in progress. ");
			break;
		case WSAEPROCLIM:
			OutputDebugString("Limit on the number of tasks supported by the Windows Sockets implementation has been reached. ");
			break;
		case WSAEFAULT:
			OutputDebugString("The lpWSAData is not a valid pointer. ");
			break;
		default:
			OutputDebugString("An unknown error occurred during WSAStartup()");
			break;
		}
	}
}

//---------------------------------------------------------------------

WinsockInstallationObject::~WinsockInstallationObject()
{
	int result = WSACleanup();
	if(result == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		switch(errCode)
		{
		case WSANOTINITIALISED:
			OutputDebugString("A successful WSAStartup must occur before using this function. ");
			break;
		case WSAENETDOWN:
			OutputDebugString("The network subsystem has failed. ");
			break;
		case WSAEINPROGRESS:
			OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ");
			break;
		default:
			OutputDebugString("An unknown error occurred during WSACleanup");
			break;
		}
	}
}

//---------------------------------------------------------------------

typedef struct _ps_overlapped
{
	OVERLAPPED op;
	struct _ps_socket_win32 *  sock;
	struct _ps_socket_win32 *  acceptedSock;
	unsigned char *            acceptData;
	DWORD                      bytes;
	struct sockaddr_in         addr;
	int                        addrLen;	
	WSABUF                     recvBuf;

	enum ps_socket_operations
	{
		RECV,
		RECVFROM,
		SEND,
		SENDTO,
		ACCEPT
	};
	ps_socket_operations  operation;	
} ps_overlapped;

//---------------------------------------------------------------------

struct OverlappedFreeList
{
	OverlappedFreeList();
	~OverlappedFreeList();
	std::vector<ps_overlapped *> freeList;
};

//---------------------------------------------------------------------

OverlappedFreeList::OverlappedFreeList() :
freeList()
{
}

//---------------------------------------------------------------------

OverlappedFreeList::~OverlappedFreeList()
{
}

//---------------------------------------------------------------------

struct OverlappedAllocatedList
{
	OverlappedAllocatedList();
	~OverlappedAllocatedList();
	std::vector<ps_overlapped *> overlappedAllocatedList;
};

//---------------------------------------------------------------------

typedef struct _ps_socket_win32
{
	ps_socket      sock;
	HANDLE         iocp;
	std::set<ps_overlapped *> op;
} ps_socket_win32;

//---------------------------------------------------------------------

OverlappedAllocatedList::OverlappedAllocatedList() :
overlappedAllocatedList()
{
}

//---------------------------------------------------------------------

OverlappedAllocatedList::~OverlappedAllocatedList()
{
	std::vector<ps_overlapped *>::iterator i;
	for(i = overlappedAllocatedList.begin(); i != overlappedAllocatedList.end(); ++i)
	{
		ps_overlapped * p = (*i);
		if(p->sock != 0)
			closesocket(p->sock->sock.s);
		delete p->acceptData;
		delete p;
	}
}

//---------------------------------------------------------------------

typedef struct _ps_group_win32
{
	HANDLE                                           iocp;

	std::hash_map<SOCKET_HANDLE, ps_socket_win32 *>  members;
	std::vector<ps_accept_result>                    completedAccepts;
	std::vector<ps_connect_result>                   completedConnects;
	std::vector<ps_recv_result>                      completedRecvs;
	std::vector<ps_recvfrom_result>                  completedRecvFroms;
	std::vector<ps_error_result>                     errors;
	std::map<WSAEVENT, const ps_socket *>            pendingConnectEvents;
	unsigned int                                     eventCount;
	unsigned int                                     queuedAccepts;
	unsigned int                                     queuedConnects;
	unsigned int                                     queuedRecvs;
	unsigned int                                     queuedRecvFroms;
} ps_group_win32;

//---------------------------------------------------------------------

void internal_post_error(ps_group group, const ps_socket * s, const int err, const char * const errString)
{
	ps_error_result e = {s, err, errString};
	ps_group_win32 * g = (ps_group_win32 *)group; //lint !e1924
	g->errors.push_back(e);
	g->eventCount++;
}

//---------------------------------------------------------------------

const bool ps_bind_socket(const ps_socket * sock, const struct sockaddr_in * bindAddress)
{
	int result = bind(sock->s, reinterpret_cast<const struct sockaddr *>(bindAddress), sizeof(struct sockaddr_in));
	if(result != 0)
	{
		int errCode = WSAGetLastError();
		switch(errCode)
		{
		case WSANOTINITIALISED:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "A successful WSAStartup must occur before using this function.");
			break;
		case WSAENETDOWN:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "The network subsystem has failed.");
			break;
		case WSAEADDRINUSE:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "A process on the machine is already bound to the same fully-qualified address and the socket has not been marked to allow address re-use with SO_REUSEADDR. For example, IP address and port are bound in the af_inet case) . (See the SO_REUSEADDR socket option under setsockopt.)");
			break;
		case WSAEADDRNOTAVAIL:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "The specified address is not a valid address for this machine");
			break;
		case WSAEFAULT:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the name parameter contains incorrect address format for the associated address family, or the first two bytes of the memory block specified by name does not match the address family associated with the socket descriptor s.");
			break;
		case WSAEINPROGRESS:
			OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
			break;
		case WSAEINVAL:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "The socket is already bound to an address.");
			break;
		case WSAENOBUFS:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "Not enough buffers available, too many connections.");
			break;
		case WSAENOTSOCK:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "The descriptor is not a socket.");
			break;
		default:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "An unknown error occurred while processing bind()");
			break;
		}
	}
	return (result == 0);
}

//---------------------------------------------------------------------

std::vector<ps_overlapped *> & internal_get_overlapped_freelist(void)
{ //lint !e1929
	static OverlappedFreeList f;
	return f.freeList;
}

//---------------------------------------------------------------------

std::vector<ps_overlapped *> & internal_get_overlapped_allocated_list(void)
{
	static OverlappedAllocatedList l;
	return l.overlappedAllocatedList;
}

//---------------------------------------------------------------------

ps_overlapped * internal_get_free_overlapped(ps_socket_win32 * sock)
{
	ps_overlapped * result = 0;

	if(!internal_get_overlapped_freelist().empty())
	{
		result = internal_get_overlapped_freelist().back();
		internal_get_overlapped_freelist().pop_back();
	}
	else
	{
		result = new ps_overlapped;
		internal_get_overlapped_allocated_list().push_back(result);
	}
	memset(result, 0, sizeof(ps_overlapped));
	sock->op.insert(result);
	result->sock = sock;
	result->addrLen = sizeof(struct sockaddr_in);
	return result;
}

//---------------------------------------------------------------------

void internal_free_overlapped(ps_overlapped * overlapped)
{
	if(overlapped->sock)
	{
		std::set<ps_overlapped *>::iterator f = overlapped->sock->op.find(overlapped);
		if(f != overlapped->sock->op.end())
			overlapped->sock->op.erase(f);
	}
	internal_get_overlapped_freelist().push_back(overlapped);
}
extern "C" {

//---------------------------------------------------------------------

const bool internal_query_completions(ps_group_win32 * group)
{
	unsigned long int bytesTransferred = 0;
	unsigned long int completionKey = 0;
	OVERLAPPED * overlapped = 0;
	ps_overlapped * op = 0;
	bool success = true;
	ps_group_win32 * g = (ps_group_win32 *)group; //lint !e1924

	if(!g->pendingConnectEvents.empty())
	{
		// query pending connect events for completion 
		// there are no IOCP/Overlapped IO operations for connect 
		// events
		std::map<WSAEVENT, const ps_socket *>::iterator i;
		std::vector<WSAEVENT> eraseList;

		for(i = g->pendingConnectEvents.begin(); i != g->pendingConnectEvents.end(); ++i)
		{
			WSANETWORKEVENTS w;
			int err = WSAEnumNetworkEvents((*i).second->s, (*i).first, &w);
			if(err == SOCKET_ERROR)
			{
				int errCode = WSAGetLastError();
				switch(errCode)
				{
				case WSANOTINITIALISED:
					internal_post_error((*i).second->group, (*i).second, ps_error_result::CONNECTION_FAILED, "A successful WSAStartup must occur before using this function.");
					break;
				case WSAENETDOWN:
					internal_post_error((*i).second->group, (*i).second, ps_error_result::CONNECTION_FAILED, "The network subsystem has failed.");
					break;
				case WSAEINVAL:
					internal_post_error((*i).second->group, (*i).second, ps_error_result::CONNECTION_FAILED, "Indicates that one of the specified parameters was invalid.");
					break;
				case WSAEINPROGRESS:
					OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
					break;
				case WSAENOTSOCK:
					internal_post_error((*i).second->group, (*i).second, ps_error_result::CONNECTION_FAILED, "The descriptor is not a socket.");
					break;
				case WSAEFAULT:
					internal_post_error((*i).second->group, (*i).second, ps_error_result::CONNECTION_FAILED, "The lpNetworkEvents argument is not a valid part of the user address space.");
					break;
				default:
					internal_post_error((*i).second->group, (*i).second, ps_error_result::CONNECTION_FAILED, "An unknown error occurred while processing WSAEnumNetworkEvents for pending connect events.");
					break;
				}
			}
			else if(w.lNetworkEvents == FD_CONNECT)
			{
				// success!!
				const ps_socket * t = (*i).second;

				eraseList.push_back((*i).first);
				// handle event
				if(w.iErrorCode[FD_CONNECT_BIT] != 0)
				{
					switch(w.iErrorCode[FD_CONNECT_BIT])
					{
					case WSAEAFNOSUPPORT:
						internal_post_error(group, (*i).second, ps_error_result::CONNECTION_FAILED, "Addresses in the specified family cannot be used with this socket.");
						break;
					case WSAECONNREFUSED:
						internal_post_error(group, (*i).second, ps_error_result::CONNECTION_FAILED, "The attempt to connect was forcefully rejected.");
						break;
					case WSAENETUNREACH:
						internal_post_error(group, (*i).second, ps_error_result::CONNECTION_FAILED, "The network cannot be reached from this host at this time.");
						break;
					case WSAENOBUFS:
						internal_post_error(group, (*i).second, ps_error_result::CONNECTION_FAILED, "No buffer space is available. The socket cannot be connected.");
						break;
					case WSAETIMEDOUT:
						internal_post_error(group, (*i).second, ps_error_result::CONNECTION_FAILED, "Attempt to connect timed out without establishing a connection.");
						break;
					default:
						internal_post_error(group, (*i).second, ps_error_result::CONNECTION_FAILED, "An unknown error occurred while processing a connect event from WSAEnumNetworkEvents.");
						break;
					}
				}
				else
				{
					// success!!
					ps_connect_result r;
					r.s = t;
					int remoteLen = sizeof(struct sockaddr_in);
					getpeername(t->s, reinterpret_cast<struct sockaddr *>(&r.remote), &remoteLen);
					g->completedConnects.push_back(r);
					g->eventCount++;
				}
				int closeErr;
				closeErr = CloseHandle((*i).first);
			}
		}
		std::vector<WSAEVENT>::iterator ei;
		for(ei = eraseList.begin(); ei != eraseList.end(); ++ei)
		{
			g->pendingConnectEvents.erase(g->pendingConnectEvents.find((*ei)));
		}
	}

	while(success)
	{
		int ok = GetQueuedCompletionStatus(
			group->iocp, // completion port of interest
			&bytesTransferred, // number of bytes sent or received
			&completionKey,
			&overlapped,
			0 // timeout immediately if there are no completions
			);
		op = reinterpret_cast<ps_overlapped *>(overlapped);
		if(ok)
		{
			success = true;
			//op = CONTAINING_RECORD(overlapped, OVERLAPPED_TCP_SERVER_S, ol); //lint !e1924 !e413 // C-style cast in Microsoft's own macro //lint !e413 potential use of NULL in Microsoft's macro
			if(op && op->sock)
			{
				switch(op->operation)
				{
				case ps_overlapped::RECV:
					{
						if(bytesTransferred > 0)
						{
							g->eventCount++;
							ps_recv_result r;
							r.data = op->recvBuf.buf;
							r.length = bytesTransferred;
							r.s = &op->sock->sock;
							g->completedRecvs.push_back(r);
						}
						else
						{
							internal_post_error(group, &op->sock->sock, ps_error_result::CONNECTION_CLOSED, "The connection was closed on the remote end.");
						}
					}
					break;
				case ps_overlapped::RECVFROM:
					{
						if(bytesTransferred > 0)
						{
							g->eventCount++;
							ps_recvfrom_result r;
							r.data = op->recvBuf.buf;
							r.length = bytesTransferred;
							r.s = &op->sock->sock;
							memcpy(&r.remote, &op->addr, sizeof(struct sockaddr_in));
							g->completedRecvFroms.push_back(r);							
						}
						else
						{
						}
					}
					break;
				case ps_overlapped::SEND:
					break;
				case ps_overlapped::ACCEPT:
					{
						if(op->acceptData != 0)
						{
							// get the addresses
							//struct sockaddr_in * local;
							//struct sockaddr_in * remote;
							// According to Microsoft, this is the correct way to 
							// get socket addresses on accept.
							// According to bounds checker, this routine trashes the
							// stack.
							//
							//int localSize = sizeof(struct sockaddr_in);
							//int remoteSize = sizeof(struct sockaddr_in);

							//GetAcceptExSockaddrs
							//(
							//	op->acceptData, 
							//	0, 
							//	sizeof(struct sockaddr_in) + 16, 
							//	sizeof(struct sockaddr_in) + 16, 
							//	reinterpret_cast<struct sockaddr **>(&local), 
							//	&localSize, 
							//	reinterpret_cast<struct sockaddr **>(&remote), 
							//	&remoteSize
							//);
							
							// Extremely lame hack to keep things safe from Winsock stacktrashing
							ps_accept_result r;
							
							memcpy(&r.local, reinterpret_cast<struct sockaddr_in *>(op->acceptData + 10), sizeof(struct sockaddr_in));
							memcpy(&r.remote, reinterpret_cast<struct sockaddr_in *>(op->acceptData + 38), sizeof(struct sockaddr_in));
							delete[] op->acceptData;
							r.listener = &op->sock->sock;
							r.newSocket = &op->acceptedSock->sock;
							g->completedAccepts.push_back(r);
							
							g->eventCount++;
						}
					}
					break;
					
				default:
					OutputDebugString("unknown socket operation in TcpServerSock!\n");
				}
			}
		}
		else if(op != 0)
		{
			OutputDebugString("failed iocp operation!\n");
			switch(op->operation)
			{
			case ps_overlapped::RECV:
				{
					internal_post_error(group, &op->sock->sock, ps_error_result::CONNECTION_CLOSED, "The connection was closed on the remote end.");
				}
				break;
			default:
				break;
			}
		}
		else
		{
			success = false;
		}
		if(op)
		{
			internal_free_overlapped(op);
		}
	}
	return success;
}

//---------------------------------------------------------------------

void internal_set_nonblocking(const ps_socket * sock)
{
	unsigned long int nb = 1;
	int err;
	err = ioctlsocket(sock->s, FIONBIO, &nb); //lint !e569 // loss of precision in the FIONBIO macro, beyond my control
	if(err == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		switch(errCode)
		{
		case WSANOTINITIALISED:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_INIT_FAILED, "A successful WSAStartup must occur before using this function. \n");
			break;
		case WSAENETDOWN:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_INIT_FAILED, "The network subsystem has failed. \n");
			break;
		case WSAEINPROGRESS:
			OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
			break;
		case WSAENOTSOCK:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_INIT_FAILED, "The descriptor s is not a socket. \n");
			break;
		case WSAEFAULT:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_INIT_FAILED, "The argp parameter is not a valid part of the user address space. \n");
			break;
		default:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_INIT_FAILED, "An unknown error occurred while processing ioctl()\n");
			break;
		}
	}
}

//---------------------------------------------------------------------

ps_group ps_create_group(void)
{
	static WinsockInstallationObject w;
	ps_group_win32 * result;
	result = new ps_group_win32;
	result->eventCount = 0;
	result->queuedAccepts = 0;
	result->queuedConnects = 0;
	result->queuedRecvFroms = 0;
	result->queuedRecvs = 0;
	result->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); //lint !e1924
	return result;
}

//---------------------------------------------------------------------

const ps_socket * ps_create_tcp_socket(ps_group group, void * userData)
{
	ps_socket_win32 * result = new ps_socket_win32;

	// allocate socket
	result->sock.s = INVALID_SOCKET;
	PROTOENT * protoEntry = getprotobyname("tcp");
	if(protoEntry)
	{
		result->sock.s = WSASocket(AF_INET, SOCK_STREAM, protoEntry->p_proto, NULL, 0, WSA_FLAG_OVERLAPPED);
		if(result->sock.s == INVALID_SOCKET)
		{
			int errCode = WSAGetLastError();
			switch(errCode)
			{
			case WSANOTINITIALISED:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "A successful WSAStartup must occur before using this function.");
				break;
			case WSAENETDOWN:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The network subsystem has failed.");
				break;
			case WSAEAFNOSUPPORT:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The specified address family is not supported.");
				break;
			case WSAEINPROGRESS:
				OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
				break;
			case WSAEMFILE:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "No more socket descriptors are available.");
				break;
			case WSAENOBUFS:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "No buffer space is available. The socket cannot be created.");
				break;
			case WSAEPROTONOSUPPORT:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The specified protocol is not supported.");
				break;
			case WSAEPROTOTYPE:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The specified protocol is the wrong type for this socket.");
				break;
			case WSAESOCKTNOSUPPORT:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The specified socket type is not supported in this address family.");
				break;
			case WSAEINVAL:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The parameter g specified is not valid, or the WSAPROTOCOL_INFO structure that lpProtocolInfo points to is incomplete, the contents are invalid or the WSAPROTOCOL_INFO structurehas already been used in an earlier duplicate socket operation.");
				break;
			case WSAEFAULT:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "lpProtocolInfo argument is not in a valid part of the process address space.");
				break;
			default:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "An unknown error has occurred while processing WSASocket().");
				break;
			}
			delete result;
			return 0;
		}
	}
	else
	{
		int errCode = WSAGetLastError();
		switch(errCode)
		{
		case WSANOTINITIALISED:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "A successful WSAStartup must occur before using this function.");
			break;
		case WSAENETDOWN:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The network subsystem has failed.");
			break;
		case WSAHOST_NOT_FOUND:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Authoritative Answer Protocol not found.");
			break;
		case WSATRY_AGAIN:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Non-Authoritative Protocol not found, or server failure.");
			break;
		case WSANO_RECOVERY:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Nonrecoverable errors, the protocols database is not accessible.");
			break;
		case WSANO_DATA:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Valid name, no data record of requested type.");
			break;
		case WSAEINPROGRESS:
			OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
			break;
		case WSAEFAULT:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The name parameter is not a valid part of the user address space.");
			break;
		case WSAEINTR:
			OutputDebugString("A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall. \n");
			break;
		default:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "An unknown error has occurred while processing getprotobyname()");
			break;
		}
		delete result;
		return 0;
	}
	
	internal_set_nonblocking(&result->sock);

	result->sock.userData = userData;
	result->sock.group = group;
	result->iocp = INVALID_HANDLE_VALUE; //lint !e1924
	result->sock.reserved_internal = result;

	struct _ps_group_win32 * g = (struct _ps_group_win32 *)group; //lint !e1924

	// get completion port for the socket
	result->iocp = CreateIoCompletionPort(reinterpret_cast<HANDLE>(result->sock.s), g->iocp, 0, 0);
	g->members[result->sock.s] = result;

	return &result->sock;
}

//---------------------------------------------------------------------

const ps_socket * ps_create_udp_socket(ps_group group, void * userData)
{
	ps_socket_win32 * result = new ps_socket_win32;
	// allocate socket
	
	PROTOENT * protoEntry = getprotobyname("udp");
	if(protoEntry)
	{
		result->sock.s = WSASocket(AF_INET, SOCK_DGRAM, protoEntry->p_proto, NULL, 0, WSA_FLAG_OVERLAPPED);
		if(result->sock.s == INVALID_SOCKET)
		{
			int errCode = WSAGetLastError();
			switch(errCode)
			{
			case WSANOTINITIALISED:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "A successful WSAStartup must occur before using this function.");
				break;
			case WSAENETDOWN:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The network subsystem has failed.");
				break;
			case WSAEAFNOSUPPORT:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The specified address family is not supported.");
				break;
			case WSAEINPROGRESS:
				OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
				break;
			case WSAEMFILE:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "No more socket descriptors are available.");
				break;
			case WSAENOBUFS:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "No buffer space is available. The socket cannot be created.");
				break;
			case WSAEPROTONOSUPPORT:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The specified protocol is not supported.");
				break;
			case WSAEPROTOTYPE:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The specified protocol is the wrong type for this socket.");
				break;
			case WSAESOCKTNOSUPPORT:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The specified socket type is not supported in this address family.");
				break;
			case WSAEINVAL:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The parameter g specified is not valid, or the WSAPROTOCOL_INFO structure that lpProtocolInfo points to is incomplete, the contents are invalid or the WSAPROTOCOL_INFO structurehas already been used in an earlier duplicate socket operation.");
				break;
			case WSAEFAULT:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "lpProtocolInfo argument is not in a valid part of the process address space.");
				break;
			default:
				internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "An unknown error has occurred while processing WSASocket()");
				break;
			}
			//@todo post error!
			delete result;
			return 0;
		}
	}
	else
	{
		int errCode = WSAGetLastError();
		switch(errCode)
		{
		case WSANOTINITIALISED:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "A successful WSAStartup must occur before using this function.");
			break;
		case WSAENETDOWN:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The network subsystem has failed.");
			break;
		case WSAHOST_NOT_FOUND:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Authoritative Answer Protocol not found.");
			break;
		case WSATRY_AGAIN:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Non-Authoritative Protocol not found, or server failure.");
			break;
		case WSANO_RECOVERY:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Nonrecoverable errors, the protocols database is not accessible.");
			break;
		case WSANO_DATA:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Valid name, no data record of requested type.");
			break;
		case WSAEINPROGRESS:
			OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
			break;
		case WSAEFAULT:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The name parameter is not a valid part of the user address space.");
			break;
		case WSAEINTR:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.");
			break;
		default:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "An unknown error has occurred while processing getprotobyname()");
			break;
		}
		//@todo post error!
		delete result;
		return 0;
	}
	
	// enable broadcast from this socket
	BOOL bcast = true;
	int err = setsockopt(result->sock.s, SOL_SOCKET, SO_BROADCAST, (char *)&bcast, sizeof( BOOL ));
	if(err == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		switch(errCode)
		{
		case WSANOTINITIALISED:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "A successful WSAStartup must occur before using this function. ");
			break;
		case WSAENETDOWN:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The network subsystem has failed. ");
			break;
		case WSAEFAULT:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "optval is not in a valid part of the process address space or optlen parameter is too small. ");
			break;
		case WSAEINPROGRESS:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ");
			break;
		case WSAEINVAL:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "level is not valid, or the information in optval is not valid. ");
			break;
		case WSAENETRESET:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Connection has timed out when SO_KEEPALIVE is set. ");
			break;
		case WSAENOPROTOOPT:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The option is unknown or unsupported for the specified provider or socket (see SO_GROUP_PRIORITY limitations). ");
			break;
		case WSAENOTCONN:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Connection has been reset when SO_KEEPALIVE is set. ");
			break;
		case WSAENOTSOCK:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The descriptor is not a socket. ");
		default:
			internal_post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "An unknown error occurred while processsing setsockopt()");
			break;
		}
	}

	internal_set_nonblocking(&result->sock);

	result->sock.userData = userData;
	result->sock.group = group;
	result->iocp = INVALID_HANDLE_VALUE; //lint !e1924
	result->sock.reserved_internal = result;

	struct _ps_group_win32 * g = (struct _ps_group_win32 *)group; //lint !e1924

	// get completion port for the socket
	result->iocp = CreateIoCompletionPort(reinterpret_cast<HANDLE>(result->sock.s), g->iocp, 0, 0);
	g->members[result->sock.s] = result;

	return &result->sock;
}

//---------------------------------------------------------------------

void ps_destroy_group(ps_group group)
{
	ps_group_win32 * g = (ps_group_win32 *)group; //lint !e1924
	BOOL result;
	result = CloseHandle(g->iocp);

	std::hash_map<SOCKET_HANDLE, ps_socket_win32 *>::iterator i;
	for(i = g->members.begin(); i != g->members.end(); ++i)
	{
		ps_socket_win32 * s = (*i).second;
		closesocket((*i).first);
		delete s;
	}
	delete g;
}

//---------------------------------------------------------------------

void ps_destroy_socket(const ps_socket * sock)
{
	if(sock)
	{
		ps_group_win32 * g = (ps_group_win32 *)sock->group; //lint !e1924
		std::hash_map<SOCKET_HANDLE, ps_socket_win32 *>::iterator i = g->members.find(sock->s);
		if(i != g->members.end())
		{
			// remove from completions
			ps_socket_win32 * s = (*i).second;
			std::set<ps_overlapped *>::iterator o;
			ps_overlapped * p;
			for(o = s->op.begin(); o != s->op.end(); ++o)
			{
				p = (*o);
				p->sock = 0;
			}

			s->iocp = INVALID_HANDLE_VALUE; //lint !e1924
			const_cast<ps_socket *>(sock)->reserved_internal = 0;
			SOCKET_HANDLE h = sock->s;
			shutdown(sock->s, 1);
			closesocket(h);
			g->members.erase(i);
			delete s;
		}
	}
}

//---------------------------------------------------------------------

const unsigned long ps_get_host_address(void)
{
	unsigned long result = 0;
	char name[128];
	int err = gethostname(name, sizeof(name));
	if(err != -1)
	{
		struct hostent * phe = gethostbyname(name);
		if(phe)
		{
			result = (unsigned long)phe->h_addr;
		}
	}
	return result;
}

//---------------------------------------------------------------------

const bool ps_get_local_address(const ps_socket * sock, struct sockaddr_in & result)
{
	int addrLen = sizeof(struct sockaddr_in);
	int r = getsockname(sock->s, (struct sockaddr *)&result, &addrLen);
	return (r == 0);
}

//---------------------------------------------------------------------

const bool ps_has_events(ps_group group)
{
	ps_group_win32 * g = (ps_group_win32 *)group; //lint !e1924
	internal_query_completions(g);
	return (g->eventCount > 0);
}

//---------------------------------------------------------------------

const bool ps_listen(const ps_socket * sock)
{
	int result = listen(sock->s, 256);
	if(result == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		switch(errCode)
		{
		case WSANOTINITIALISED:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "A successful WSAStartup must occur before using this function.");
			break;
		case WSAENETDOWN:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "The network subsystem has failed.");
			break;
		case WSAEADDRINUSE:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs during execution of the bind function, but could be delayed until this function if the bind was to a partially wild-card address (involving ADDR_ANY) and if a specific address needs to be \"committed\" at the time of this function.");
			break;
		case WSAEINPROGRESS:
			OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
			break;
		case WSAEINVAL:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "The socket has not been bound with bind.");
			break;
		case WSAEISCONN:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "The socket is already connected.");
			break;
		case WSAEMFILE:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "No more socket descriptors are available.");
			break;
		case WSAENOBUFS:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "No buffer space is available.");
			break;
		case WSAENOTSOCK:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "The descriptor is not a socket.");
			break;
		case WSAEOPNOTSUPP:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "The referenced socket is not of a type that supports the listen operation.");
			break;
		default:
			internal_post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "An unknown error occurred while processing listen()");
			break;
		}
	}
	return (result == 0);
}

//---------------------------------------------------------------------

const bool ps_query_accept(ps_group group, ps_accept_result * target)
{
	bool result = false;
	ps_group_win32 * g = (ps_group_win32 *)group; //lint !e1924
	if(g->queuedAccepts == 0)
	{
		internal_post_error(group, 0, ps_error_result::NO_PENDING_OPERATIONS, "An accept operation was queried, but there was no previous call to ps_queue_accept()");
	}
	else
	{
		internal_query_completions(g);
		if(!g->completedAccepts.empty())
		{
			std::vector<ps_accept_result>::iterator i = g->completedAccepts.begin();
			target->listener = (*i).listener;
			target->newSocket = (*i).newSocket;
			target->remote = (*i).remote;
			target->local = (*i).local;
			result = true;
			g->eventCount--;
			g->queuedAccepts--;
			g->completedAccepts.erase(i); //lint !e534

		}
	}
	return result;
}

//---------------------------------------------------------------------

const bool ps_query_connect(ps_group group, ps_connect_result * target)
{
	bool result = false;
	ps_group_win32 * g = (ps_group_win32 *)group; //lint !e1924
	if(g->queuedConnects == 0)
	{
		internal_post_error(group, 0, ps_error_result::NO_PENDING_OPERATIONS, "A connect operations was queried, but there was no previous call to ps_queue_connect()");
	}
	else
	{
		internal_query_completions(g);
		if(!g->completedConnects.empty())
		{
			std::vector<ps_connect_result>::iterator i = g->completedConnects.begin();
			target->remote = (*i).remote;
			target->s = (*i).s;
			result = true;
			g->eventCount--;
			g->queuedConnects--;
			g->completedConnects.erase(i); //lint !e534
		}
	}		
	return result;
}

//---------------------------------------------------------------------

const bool ps_query_error(ps_group group, ps_error_result * target)
{
	bool result = false;
	ps_group_win32 * g = (ps_group_win32 *)group; //lint !e1924
	internal_query_completions(g);
	if(!g->errors.empty())
	{
		std::vector<ps_error_result>::iterator i = g->errors.begin();
		target->err = (*i).err;
		target->s = (*i).s;
		target->errString = (*i).errString;
		result = true;
		g->eventCount--;
		g->errors.erase(i); //lint !e534
	}
	return result;
}

//---------------------------------------------------------------------

const bool ps_query_recvfrom(ps_group group, ps_recvfrom_result * target)
{
	bool result = false;
	ps_group_win32 * g = (ps_group_win32 *)group; //lint !e1924
	if(g->queuedRecvFroms == 0)
	{
		internal_post_error(group, 0, ps_error_result::NO_PENDING_OPERATIONS, "A recvfrom operation was queried, but there was no prior call to ps_queue_recvfrom()");
	}
	else
	{
		internal_query_completions(g);

		if(!g->completedRecvFroms.empty())
		{
			std::vector<ps_recvfrom_result>::iterator i = g->completedRecvFroms.begin();
			target->data = (*i).data;
			target->length = (*i).length;
			target->s = (*i).s;
			
			memcpy(&target->remote, &(*i).remote, sizeof(struct sockaddr_in));
			result = true;
			g->eventCount--;
			g->queuedRecvFroms--;
			g->completedRecvFroms.erase(i); //lint !e534
		}
	}
	return result;
	
}

//---------------------------------------------------------------------

const bool ps_query_recv(ps_group group, ps_recv_result * target)
{
	bool result = false;
	ps_group_win32 * g = (ps_group_win32 *)group; //lint !e1924
	if(g->queuedRecvs == 0)
	{
		internal_post_error(group, 0, ps_error_result::NO_PENDING_OPERATIONS, "A recv operation was queried, but there was no prior call to ps_queue_recv()");
	}
	else
	{
		internal_query_completions(g);

		if(!g->completedRecvs.empty())
		{
			std::vector<ps_recv_result>::iterator i = g->completedRecvs.begin();
			target->data = (*i).data;
			target->length = (*i).length;
			target->s = (*i).s;
			result = true;
			g->eventCount--;
			g->queuedRecvs--;
			g->completedRecvs.erase(i); //lint !e534
		}
	}
	return result;
}

//---------------------------------------------------------------------

const bool ps_queue_accept(const ps_socket * listener, const ps_socket * acceptSock)
{
	bool r = false;
	ps_socket_win32 * s = (ps_socket_win32 *)listener->reserved_internal; //lint !e1924
	ps_group_win32 * g = (ps_group_win32 *)listener->group; //lint !e1924
	if(s)
	{
		ps_overlapped * op = internal_get_free_overlapped(s);
		op->acceptData = new unsigned char[(sizeof( struct sockaddr_in ) + 16 ) * 2];
		op->operation = ps_overlapped::ACCEPT;
		ps_socket_win32 * a = (ps_socket_win32 *)acceptSock->reserved_internal; //lint !e1924
		op->acceptedSock = a;

		int result = AcceptEx(
			listener->s, 
			acceptSock->s,
			op->acceptData,
			0, 
			sizeof(struct sockaddr_in) + 16, 
			sizeof(struct sockaddr_in) + 16, 
			&op->bytes,
			&op->op);
		if(result == 0)
		{
			int errCode = WSAGetLastError();
			if(errCode != ERROR_IO_PENDING)
			{
				//@todo post error!
			}
			else
			{
				r = true;
			}
		}
		else
		{
			r = true;
		}
	}
	
	if(r)
		g->queuedAccepts++;
	
	return r;
}

//---------------------------------------------------------------------

const bool ps_queue_connect(const ps_socket * connector, const struct sockaddr_in * remote)
{
	bool success = false;
	ps_group_win32 * g = (ps_group_win32 *)connector->group; //lint !e1924
	int nameLen = sizeof(struct sockaddr_in);
	
	static WSABUF emptyBuf;
	emptyBuf.buf = 0;
	emptyBuf.len = 0;
	QOS sqos;
	QOS gqos;
	memset(&sqos, 0, sizeof(QOS));
	memset(&gqos, 0, sizeof(QOS));

	int result = WSAConnect(connector->s, reinterpret_cast<const struct sockaddr *>(remote), nameLen, 0, &emptyBuf, &sqos, &gqos);

	if(result == SOCKET_ERROR)
	{
		int errCode = WSAGetLastError();
		if(errCode != WSAEWOULDBLOCK)
		{
			switch(errCode)
			{
			case WSANOTINITIALISED:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "A successful WSAStartup must occur before using this function.");
				break;
			case WSAENETDOWN:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The network subsystem has failed. \n");
				break;
			case WSAEADDRINUSE:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The local address of the socket is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs during the execution of bind, but could be delayed until this function if the bind function operates on a partially wild-card address (involving ADDR_ANY) and if a specific address needs to be \"committed\" at the time of this function. ");
				break;
			case WSAEINTR:
				OutputDebugString("The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall. \n");
				break;
			case WSAEINPROGRESS:
				OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
				break;
			case WSAEALREADY:
				OutputDebugString("A nonblocking connect/WSAConnect call is in progress on the specified socket. \n");
				break;
			case WSAEADDRNOTAVAIL:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The remote :address is not a valid address (such as ADDR_ANY). ");
				break;
			case WSAEAFNOSUPPORT:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "Addresses in the specified family cannot be used with this socket. ");
				break;
			case WSAECONNREFUSED:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The attempt to connect was rejected.");
				break;
			case WSAEFAULT:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the buffer length for lpCalleeData, lpSQOS, and lpGQOS are too small, or the buffer length for lpCallerData is too large. ");
				break;
			case WSAEINVAL:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The parameter s is a listening socket, or the destination address specified is not consistent with that of the constrained group the socket belongs to. ");
				break;
			case WSAEISCONN:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The socket is already connected (connection-oriented sockets only). ");
				break;
			case WSAENETUNREACH:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The network cannot be reached from this host at this time. ");
				break;
			case WSAENOBUFS:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "No buffer space is available. The socket cannot be connected. ");
				break;
			case WSAENOTSOCK:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The descriptor is not a socket. ");
				break;
			case WSAEOPNOTSUPP:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The FLOWSPEC structures specified in lpSQOS and lpGQOS cannot be satisfied. ");
				break;
			case WSAEPROTONOSUPPORT:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The lpCallerData argument is not supported by the service provider. ");
				break;
			case WSAETIMEDOUT:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "Attempt to connect timed out without establishing a connection. ");
				break;
			case WSAEACCES:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "Attempt to connect datagram socket to broadcast address failed because setsockopt SO_BROADCAST is not enabled.");
				break;
			default:
				internal_post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "An unknown error has occurred while prcoessing connect()\n");
				break;
			}
		}
		else
		{
			success = true;
		}
	}
	else
	{
		success = true;
	}

	WSAEVENT c = WSACreateEvent();
	int eventResult;
	eventResult = WSAEventSelect(connector->s, c, FD_CONNECT);
	if(eventResult != 0)
		success = false;
	g->pendingConnectEvents.insert(std::pair<WSAEVENT, const ps_socket *>(c, connector)); //lint !e534 don't care about iterator returned
	if(success)
		g->queuedConnects++;
	return success;
}

//---------------------------------------------------------------------

const bool ps_queue_recvfrom(const ps_socket * ps, void * recvBuffer, const unsigned int recvBufferSize)
{
	bool r = false;
	DWORD flags = 0;
	ps_socket_win32 * s = (ps_socket_win32 *)ps->reserved_internal;	 //lint !e1924
	if(s)
	{
		ps_overlapped * op = internal_get_free_overlapped(s);
		op->operation = ps_overlapped::RECVFROM;
		op->recvBuf.buf = (char *)recvBuffer;
		op->recvBuf.len = recvBufferSize;
		int result = WSARecvFrom(
			ps->s, 
			&op->recvBuf, 
			1, 
			&op->bytes, 
			&flags,
			reinterpret_cast<struct sockaddr *>(&op->addr), 
			&op->addrLen, 
			&op->op, 
			NULL);

		// if an error occurred and it was not an IO PENDING error (e.g.
		// queued for completion later), bark a message to the debug window
		if(result == SOCKET_ERROR)
		{
			int errCode = WSAGetLastError();
			if(errCode != WSA_IO_PENDING)
			{
				switch(errCode)
				{
				case WSANOTINITIALISED:
					OutputDebugString("A successful WSAStartup must occur before using this function.");
					break;
				case WSAENETDOWN: 
					OutputDebugString("The network subsystem has failed.");
					break;
				case WSAEFAULT:
					OutputDebugString("The lpBuffers, lpFlags, lpFrom, lpNumberOfBytesRecvd, lpFromlen, lpOverlapped, or lpCompletionRoutine argument is not totally contained in a valid part of the user address space: the lpFrom buffer was too small to accommodate the peer address.");
					break;
				case WSAEINTR:
					OutputDebugString("A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall. \n");
					break;
				case WSAEINPROGRESS:
					OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
					break;
				case WSAEINVAL:
					OutputDebugString("The socket has not been bound (with bind, for example).");
					break;
				case WSAEISCONN:
					OutputDebugString("The socket is connected. This function is not permitted with a connected socket, whether the socket is connection-oriented or connectionless.");
					break;
				case WSAENETRESET:
					OutputDebugString("The connection has been broken due to \"keep-alive\" activity detecting a failure while the operation was in progress.");
					break;
				case WSAENOTCONN:
					OutputDebugString("The connection was closed on the remote end.");
					break;
				case WSAEOPNOTSUPP:
					OutputDebugString("MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, out-of-band data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.");
					break;
				case WSAESHUTDOWN:
					OutputDebugString("The socket has been shut down; it is not possible to WSARecvFrom on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH. \n");
					break;
				case WSAEWOULDBLOCK:
					OutputDebugString("Overlapped sockets: There are too many outstanding overlapped I/O requests. Nonoverlapped sockets: The socket is marked as nonblocking and the receive operation cannot be completed immediately. \n");
					break;
				case WSAEMSGSIZE:
					OutputDebugString("The message was too large to fit into the specified buffer and (for unreliable protocols only) any trailing portion of the message that did not fit into the buffer has been discarded.");
					break;
				case WSAECONNRESET:
					OutputDebugString("The virtual circuit was reset by the remote side executing a \"hard\" or \"abortive\" close. The application should close the socket as it is no longer useable. On a UDP datagram socket this error would indicate that a previous send operation resulted in an ICMP \"Port Unreachable\" message. \n");
					break;
				case WSAEDISCON: 
					OutputDebugString("Socket s is message oriented and the virtual circuit was gracefully closed by the remote side. \n");
					break;
				case WSA_OPERATION_ABORTED:
					OutputDebugString("The overlapped operation has been canceled due to the closure of the socket. \n");
					break;
				case WSAENOTSOCK:
					OutputDebugString("Socket operation on non-socket. An operation was attempted on something that is not a socket. Either the socket handle parameter did not reference a valid socket, or for select, a member of an fd_set was not valid.");
					break;
				default:
					OutputDebugString("An unknown error occurred while processing WSASendTo()\n");
					break;
				}
			}
			else
			{
				r = true;
			}
		}
		else
		{
			r = true;
		}
	}
	
	if(r)
	{
		ps_group_win32 * g = (ps_group_win32 *)ps->group; //lint !e1924
		g->queuedRecvFroms++;
	}

	return r;
}

//---------------------------------------------------------------------

const bool ps_queue_recv(const ps_socket * ps, void * recvBuffer, const unsigned int recvBufferSize)
{
	bool r = false;
	DWORD flags = 0;
	ps_socket_win32 * s = (ps_socket_win32 *)ps->reserved_internal;	 //lint !e1924
	if(s)
	{
		ps_overlapped * op = internal_get_free_overlapped(s);
		op->operation = ps_overlapped::RECV;
		op->recvBuf.buf = (char *)recvBuffer;
		op->recvBuf.len = recvBufferSize;
		int result = WSARecv(
			ps->s,//sock.getHandle(), 
			&op->recvBuf, 
			1, 
			&op->bytes, //bytes, 
			&flags, 
			&op->op, //overlapped, 
			NULL);
		if(result == SOCKET_ERROR)
		{
			int errCode = WSAGetLastError();
			if(errCode != WSA_IO_PENDING)
			{
				switch(errCode)
				{
				case WSANOTINITIALISED:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "A successful WSAStartup must occur before using this function.");
					break;
				case WSAENETDOWN:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The network subsystem has failed.");
					break;
				case WSAENOTCONN:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The socket is not connected.");
					break;
				case WSAEINTR:
					OutputDebugString("The (blocking) call was canceled through WSACancelBlockingCall. \n");
					break;
				case WSAEINPROGRESS:
					OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
					break;
				case WSAENETRESET:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The connection has been broken due to \"keep-alive\" activity detecting a failure while the operation was in progress.");
					break;
				case WSAENOTSOCK:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The descriptor is not a socket.");
					break;
				case WSAEFAULT:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The lpBuffers parameter is not completely contained in a valid part of the user address space.");
					break;
				case WSAEOPNOTSUPP:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, out-of-band data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.");
					break;
				case WSAESHUTDOWN:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The socket has been shut down; it is not possible to call WSARecv on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH. \n");
					break;
				case WSAEWOULDBLOCK:
					OutputDebugString("Overlapped sockets: There are too many outstanding overlapped I/O requests. Nonoverlapped sockets: The socket is marked as nonblocking and the receive operation cannot be completed immediately. \n");
					break;
				case WSAEMSGSIZE:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The message was too large to fit into the specified buffer and (for unreliable protocols only) any trailing portion of the message that did not fit into the buffer has been discarded.");
					break;
				case WSAEINVAL:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The socket has not been bound (for example, with bind).");
					break;
				case WSAECONNABORTED:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The virtual circuit was terminated due to a time-out or other failure. \n");
					break;
				case WSAECONNRESET:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The virtual circuit was reset by the remote side. \n");
					break;
				case WSAEDISCON:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "Socket s is message oriented and the virtual circuit was gracefully closed by the remote side. \n");
					break;
				case WSA_IO_PENDING:
					OutputDebugString("An overlapped operation was successfully initiated and completion will be indicated at a later time. \n");
					break;
				case WSA_OPERATION_ABORTED:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The overlapped operation has been canceled due to the closure of the socket. \n");
					break;
				default:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "An unknown error occured while processing WSARecv().");
					break;
				}
			}
			else
			{
				result = 0;
				r = true;
			}
		}
		else
		{
			r = true;
		}
	}

	if(r)
	{
		ps_group_win32 * g = (ps_group_win32 *)ps->group; //lint !e1924
		g->queuedRecvs++;
	}

	return r;
}

//---------------------------------------------------------------------

const bool ps_queue_send(const ps_socket * ps, const void * const data, const unsigned int length)
{
	bool result = false;
	int sent = 0;
	WSABUF wsaBuf;
	wsaBuf.buf = (char *)data; //lint !e1924
	wsaBuf.len = length;
	ps_socket_win32 * s = (ps_socket_win32 *)ps->reserved_internal; //lint !e1924
	if(s)
	{
		ps_overlapped * op = internal_get_free_overlapped(s);
		op->operation = ps_overlapped::SEND;
		sent = WSASend(
			ps->s, //sock.getHandle(), 
			&wsaBuf, 
			1, 
			&op->bytes, //bytes, 
			0, 
			&op->op, //overlapped, 
			NULL);
		if(sent != SOCKET_ERROR)
		{
			result = true;
		}
		else
		{
			int errCode = WSAGetLastError();
			if(errCode == WSA_IO_PENDING)
			{
				result = true;
			}
			else
			{
				switch(errCode)
				{
				case WSANOTINITIALISED:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "A successful WSAStartup must occur before using this function.");
					break;
				case WSAENETDOWN:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The network subsystem has failed.");
					break;
				case WSAEACCES:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The requested address is a broadcast address, but the appropriate flag was not set.");
					break;
				case WSAEINTR:
					OutputDebugString("A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall. \n");
					break;
				case WSAEINPROGRESS:
					OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
					break;
				case WSAEFAULT:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The lpBuffers, lpNumberOfBytesSent, lpOverlapped, lpCompletionRoutine argument is not totally contained in a valid part of the user address space.");
					break;
				case WSAENETRESET:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The connection has been broken due to \"keep-alive\" activity detecting a failure while the operation was in progress.");
					break;
				case WSAENOBUFS:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The Windows Sockets provider reports a buffer deadlock.");
					break;
				case WSAENOTCONN:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The socket is not connected.");
					break;
				case WSAENOTSOCK:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The descriptor is not a socket.");
					break;
				case WSAEOPNOTSUPP:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, out-of-band data is not supported in the communication domain associated with this socket, MSG_PARTIAL is not supported, or the socket is unidirectional and supports only receive operations.");
					break;
				case WSAESHUTDOWN:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The socket has been shut down; it is not possible to WSASend on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH.");
					break;
				case WSAEWOULDBLOCK:
					OutputDebugString("Overlapped sockets: There are too many outstanding overlapped I/O requests. Nonoverlapped sockets: The socket is marked as nonblocking and the send operation cannot be completed immediately. \n");
					break;
				case WSAEMSGSIZE:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The socket is message oriented, and the message is larger than the maximum supported by the underlying transport.");
					break;
				case WSAEINVAL:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The socket has not been bound with bind, or the socket is not created with the overlapped flag.");
					break;
				case WSAECONNABORTED:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The virtual circuit was terminated due to a time-out or other failure.");
					break;
				case WSAECONNRESET:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The virtual circuit was reset by the remote side. \n");
					break;
				case WSA_OPERATION_ABORTED:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "The overlapped operation has been canceled due to the closure of the socket, or the execution of the SIO_FLUSH command in WSAIoctl.");
					break;
				default:
					internal_post_error(ps->group, ps, ps_error_result::CONNECTION_CLOSED, "An unknown error occurred while processing WSASend.");
					break;
				}
			}
		}
	}
	return result;
}

//---------------------------------------------------------------------

const bool ps_queue_sendto(const ps_socket * ps, const struct sockaddr_in * to, const void * data, const unsigned int length)
{
	bool r = false;
	static const int toLen = sizeof(struct sockaddr_in);
	WSABUF wsaBuf;
	wsaBuf.buf = (char *)data; //lint !e1924
	wsaBuf.len = length;
	ps_socket_win32 * s = (ps_socket_win32 *)ps->reserved_internal; //lint !e1924
	if(s)
	{
		ps_overlapped * op = internal_get_free_overlapped(s);
		op->operation = ps_overlapped::SEND;
		int result = WSASendTo(
			ps->s, //op->sock, 
			&wsaBuf, 
			1, 
			&op->bytes, 
			0, 
			reinterpret_cast<const struct sockaddr *>(to), 
			toLen, 
			&op->op, 
			NULL);
		if(result == SOCKET_ERROR)
		{
			int errCode = WSAGetLastError();
			if(errCode != WSA_IO_PENDING)
			{
				switch (errCode)
				{
				case WSANOTINITIALISED:
					OutputDebugString("A successful WSAStartup must occur before using this function. \n");
					break;
				case WSAENETDOWN:
					OutputDebugString("The network subsystem has failed. \n");
					break;
				case WSAEACCES:
					OutputDebugString("The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST parameter to allow the use of the broadcast address. \n");
					break;
				case WSAEINVAL:
					OutputDebugString("An unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled. \n");
					break;
				case WSAEINTR:
					OutputDebugString("A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall. \n");
					break;
				case WSAEINPROGRESS:
					OutputDebugString("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n");
					break;
				case WSAEFAULT:
					OutputDebugString("The buf or to parameters are not part of the user address space, or the tolen parameter is too small. \n");
					break;
				case WSAENETRESET:
					OutputDebugString("The connection has been broken due to \"keep-alive\" activity detecting a failure while the operation was in progress. \n");
					break;
				case WSAENOBUFS: 
					OutputDebugString("No buffer space is available. \n");
					break;
				case WSAENOTCONN:
					OutputDebugString("The socket is not connected (connection-oriented sockets only) \n");
					break;
				case WSAENOTSOCK:
					OutputDebugString("The descriptor is not a socket. \n");
					break;
				case WSAEOPNOTSUPP:
					OutputDebugString("MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, out-of-band data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations. \n");
					break;
				case WSAESHUTDOWN:
					OutputDebugString("The socket has been shut down; it is not possible to sendto on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH. \n");
					break;
				case WSAEWOULDBLOCK:
					OutputDebugString("The socket is marked as nonblocking and the requested operation would block. \n");
					break;
				case WSAEMSGSIZE:
					OutputDebugString("The socket is message oriented, and the message is larger than the maximum supported by the underlying transport. \n");
					break;
				case WSAEHOSTUNREACH:
					OutputDebugString("The remote host cannot be reached from this host at this time. \n");
					break;
				case WSAECONNABORTED:
					OutputDebugString("The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable. \n");
					break;
				case WSAECONNRESET:
					OutputDebugString("The virtual circuit was reset by the remote side executing a \"hard\" or \"abortive\" close. For UPD sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a \"Port Unreachable\" ICMP packet. The application should close the socket as it is no longer usable. \n");
					break;
				case WSAEADDRNOTAVAIL:
					OutputDebugString("The remote address is not a valid address, for example, ADDR_ANY. \n");
					break;
				case WSAEAFNOSUPPORT:
					OutputDebugString("Addresses in the specified family cannot be used with this socket. \n");
					break;
				case WSAEDESTADDRREQ:
					OutputDebugString("A destination address is required. \n");
					break;
				case WSAENETUNREACH:
					OutputDebugString("The network cannot be reached from this host at this time. \n");
					break;
				case WSAETIMEDOUT:
					OutputDebugString("The connection has been dropped, because of a network failure or because the system on the other end went down without notice. \n");
					break;
				default:
					OutputDebugString("An unkown error has occurred while processing sendto()\n");
					break;
				}
			}
			else
			{
				r = true;
			}
		}
		else
		{
			r = true;
		}
	}
	return r;
}

//---------------------------------------------------------------------

}
