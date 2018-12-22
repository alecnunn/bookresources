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

#include "Network/push_socket.h"
#include <cstdio>
#include <errno.h>
#include <hash_map>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

// ----------------------------------------------------------------------

typedef struct _ps_group_linux
{
	std::hash_map<int, ps_socket *>  ps_sockets;

	std::pair<std::vector<struct pollfd>, std::vector<struct pollfd *> >  accept_poll;
	std::pair<std::vector<struct pollfd>, std::vector<struct pollfd *> >  connect_poll;
	std::pair<std::vector<struct pollfd>, std::vector<struct pollfd *> >  recv_poll;
	std::pair<std::vector<struct pollfd>, std::vector<struct pollfd *> >  recvfrom_poll;
	std::pair<std::vector<struct pollfd>, std::vector<struct pollfd *> >  send_poll;
	std::pair<std::vector<struct pollfd>, std::vector<struct pollfd *> >  sendto_poll;

	std::vector<ps_error_result>     errors;
	std::vector<ps_accept_result>    completed_accepts;
	std::vector<ps_connect_result>   completed_connects;
	std::vector<ps_recv_result>      completed_recvs;
	std::vector<ps_recvfrom_result>  completed_recvfroms;
	unsigned long int                event_count;
	unsigned long int                queued_accepts;
	unsigned long int                queued_connects;
	unsigned long int                queued_recvs;
	unsigned long int                queued_recvfroms;
} ps_group_linux;

// ----------------------------------------------------------------------

typedef struct _ps_socket_linux
{
	ps_socket *               sock;
	std::vector<ps_socket *>  accept_socks;
	struct pollfd *           tcp_init_index; // either connect or accept
	struct pollfd *           read_index;     // either recv or recvfrom (TCP or UDP respectively)
	struct pollfd *           write_index;    // either send or sendto (TCP or UDP res[ectively)
	std::vector<ps_recvfrom_result> recv_buffers;
	std::vector<unsigned char> pending_send_data;
	
} ps_socket_linux;

// ----------------------------------------------------------------------

struct pollfd * add_descriptor(std::pair<std::vector<struct pollfd>, std::vector<struct pollfd *> > * pollset, ps_socket_linux * sock)
{
	struct pollfd * i;
	std::vector<struct pollfd> & activeList = pollset->first;
	std::vector<struct pollfd *> & freeList = pollset->second;

	if(!freeList.empty())
	{
		i = freeList.back();
		freeList.pop_back();
	}
	else
	{
		struct pollfd fd;
		activeList.push_back(fd);
		i = &activeList.back();
	}

	i->revents = 0;
	i->fd = sock->sock->s;
	return i;
}

// ----------------------------------------------------------------------

void remove_descriptor(std::pair<std::vector<struct pollfd>, std::vector<struct pollfd *> > * pollset, struct pollfd * sockIndex)
{
	sockIndex->revents = 0;
	sockIndex->events = 0;
	pollset->second.push_back(sockIndex);
}

// ----------------------------------------------------------------------

void post_error(ps_group group, const ps_socket * s, const int err, const char * const errString)
{
	ps_error_result e = {s, err, errString};
	ps_group_linux * g = (ps_group_linux *)group; //lint !e1924
	g->errors.push_back(e);
}

// ----------------------------------------------------------------------

const char * ps_getpeername(int handle, struct sockaddr_in * remote)
{
	const char * result = 0;
	socklen_t remoteLen = sizeof(struct sockaddr_in);
	int peerErr = getpeername(handle, (struct sockaddr *)remote, &remoteLen);
	if(peerErr == -1)
	{
		switch(errno)
		{
			case EBADF:
				result = "The argument s is not a valid descriptor.";
				break;
			case ENOTSOCK:
				result = "The argument s is a file, not a socket.";
				break;
			case ENOTCONN:
				result = "The socket is not connected.";
				break;
			case ENOBUFS:
				result = "Insufficient resources were available in the system to perform the operation.";
				break;
			case EFAULT:
				result = "The name parameter points to memory not in a valid part of the process address space.";
				break;
			default:
				result = "An unknown error occured while processing getpeername().";
				break;
		}
	}
	return result;
}

// ----------------------------------------------------------------------

const int update_pollfds(struct pollfd * fds, const unsigned int count)
{
	int result = poll(fds, count, 0);
	if(result == -1)
	{
		switch(errno)
		{
			case ENOMEM:
				fprintf(stderr, "There was no space to allocate file descriptor tables.\n");
				break;
			case EFAULT:
				fprintf(stderr, "The array given as argument was not contained in the calling program's address space.\n");
				break;
			case EINTR:
				fprintf(stderr, "A signal occurred before any requested event.\n");
				break;
			default:
				fprintf(stderr, "An unknown error occurred during poll()\n");
		}
	}
	return result;
}

// ----------------------------------------------------------------------

void queue_pending_send_data(ps_socket_linux * sock, const unsigned char * data, const unsigned int dataLen)
{
	for(unsigned int i = 0 ; i < dataLen; ++i)
	{
		sock->pending_send_data.push_back(data[i]);
	}
}

// ----------------------------------------------------------------------

const bool flush_pending_send_data(ps_socket_linux * sock)
{
	bool result = false;
	
	const unsigned char * const data = sock->pending_send_data.begin();
	int bytesSent = send(sock->sock->s, data, sock->pending_send_data.size(), 0);
	if(bytesSent == -1)
	{
		if(errno != EAGAIN)
		{
            switch(errno)
			{
				case EBADF:
					post_error(sock->sock->group, sock->sock, ps_error_result::CONNECTION_CLOSED, "An invalid descriptor was specified.");
					break;
				case ENOTSOCK:
					post_error(sock->sock->group, sock->sock, ps_error_result::CONNECTION_CLOSED, "The argument s is not a socket.");
					break;
				case EFAULT:
					post_error(sock->sock->group, sock->sock, ps_error_result::CONNECTION_CLOSED, "An invalid user space address was specified for a parameter.");
					break;
				case EMSGSIZE:
					post_error(sock->sock->group, sock->sock, ps_error_result::CONNECTION_CLOSED, "The socket requires that message be sent atomically, and the size of the message to be sent made this impossile.");
					break;
				case ENOBUFS:
					post_error(sock->sock->group, sock->sock, ps_error_result::CONNECTION_CLOSED, "The output queue for a network interface was full.  This generally indicates that the interface has stopped snding, but may  be  caused  by  transient  congestion. (This cannot occur in Linux, packets are just silently dropped when a device queue overflows.)");
					break;
				case ENOMEM:
					post_error(sock->sock->group, sock->sock, ps_error_result::CONNECTION_CLOSED, "No memory available.");
					break;
				case EINVAL:
					post_error(sock->sock->group, sock->sock, ps_error_result::CONNECTION_CLOSED, "Invalid argument passed.");
					break;
				case EPIPE:
					post_error(sock->sock->group, sock->sock, ps_error_result::CONNECTION_CLOSED, "The local end has been shut down on a connection oriented socket.  In this case the process will also receivea SIGPIPE unless MSG_NOSIGNAL is set.");
					break;
				default:
					post_error(sock->sock->group, sock->sock, ps_error_result::CONNECTION_CLOSED, "An unknown error occurred while processing send()");
					break;
			}
		}
	}
	else if(bytesSent > 0 && bytesSent < sock->pending_send_data.size())
	{
		// erase the front range of data
		std::vector<unsigned char>::iterator last = sock->pending_send_data.begin() + bytesSent;
		sock->pending_send_data.erase(sock->pending_send_data.begin(), last);
	}
	else if(bytesSent == sock->pending_send_data.size())
	{
		result = true;
	}
	return result;
}

// ----------------------------------------------------------------------

const bool query_accept_completions(ps_group_linux * group)
{
	bool result = false;
	
	if(!group->accept_poll.first.empty())
	{
		int pollResult = update_pollfds(group->accept_poll.first.begin(), group->accept_poll.first.size());
		if(pollResult > 0)
		{
			// find descriptors with events
			std::vector<struct pollfd>::iterator i;
			int count = 0;
			for(i = group->accept_poll.first.begin(), count = 0; i != group->accept_poll.first.end() && count < pollResult; ++i)
			{
				struct pollfd & f = (*i);
				if(f.revents)
				{
					count++;
						// was there an error on accept?
					if(! (f.revents & POLLERR))
					{
						if(f.revents & POLLIN || f.revents & POLLOUT)
						{
							// find the listener
							std::hash_map<int, ps_socket *>::iterator findListener = group->ps_sockets.find(f.fd);
							if(findListener != group->ps_sockets.end())
							{
								ps_socket * listener = (*findListener).second;
								ps_socket_linux * l = (ps_socket_linux *)listener->reserved_internal;
								// grab an accept sock
								ps_socket * accepted = l->accept_socks.back();
								if(accepted)
								{
									struct sockaddr_in remote;
									socklen_t remoteLen = sizeof(struct sockaddr_in);
									accepted->s = accept(listener->s, (sockaddr *)&remote, &remoteLen);
									if(accepted->s == -1)
									{
										// error!
										if(errno != EAGAIN)
										{
											fprintf(stderr, "error polling accept()... ");
											switch(errno)
											{
												case EBADF:
													fprintf(stderr, "The descriptor is invalid.\n");
													break;
												case ENOTSOCK:
													fprintf(stderr, "The descriptor references a file, not a socket.\n");
													break;
												case EOPNOTSUPP:
													fprintf(stderr, "The referenced socket is not of type SOCK_STREAM.\n");
													break;
												case EFAULT:
													fprintf(stderr, "The addr parameter is not in a writable part of the user address space.\n");
													break;
												case EPERM:
													fprintf(stderr, "Firewall rules forbid connection.\n");
												case ENOMEM:
													fprintf(stderr, "Not enough free memory.  This often means that the memory allocation is limited by the socket buffer limits, not by the system memory.\n");
													break;
												default:
													fprintf(stderr, "An unknown error occurred while processing accept\n");
													break;
											}
										}			   
									}
									else
									{
										// have a valid connection now
										// get addresses
										struct sockaddr_in local;
										socklen_t localLen = sizeof(struct sockaddr_in);
										int peerResult = getsockname(accepted->s, (struct sockaddr *)&local, &localLen);
										if(peerResult == -1)
										{
											// error!
											fprintf(stderr, "Error in getsockname() while accepting a connection... ");
											switch(errno)
											{
												case EBADF:
													fprintf(stderr, "The argument s is not a valid descriptor.\n");
													break;
												case ENOTSOCK:
													fprintf(stderr, "The argument s is a file, not a socket.\n");
													break;
												case ENOBUFS:
													fprintf(stderr, "Insufficient resources were available in the system to perform the operation.\n");
													break;
												case EFAULT:
													fprintf(stderr, "The name parameter points to memory not in a valid part of the process address space.\n");
													break;
												default:
													fprintf(stderr, "An unknown error occurred while processing getsockname()\n");
													break;
											}														   
										}
										else
										{
											// add the new socket to the pollfds for read and write on TCP
											ps_socket_linux * al = (ps_socket_linux *)accepted->reserved_internal;
											al->read_index = add_descriptor(&group->recv_poll, al);
											al->write_index = add_descriptor(&group->send_poll, al);
											al->read_index->events = 0;
											al->write_index->events = 0;
											
											l->accept_socks.pop_back();
											ps_accept_result r;
											r.listener = listener;
											r.newSocket = accepted;
											r.remote = remote;
											r.local = local;
											result = true;
											group->completed_accepts.push_back(r);
											group->queued_accepts--;
											group->event_count++;
										}
									}
								}
							}
						}
					}
					else
					{
						perror("accept");
					}
					if(count == pollResult)
						break;
				}
			}
		}
	}
	return result;
}

// ----------------------------------------------------------------------

const bool query_connect_completions(ps_group_linux * group)
{
	bool result = false;
	
	if(!group->connect_poll.first.empty())
	{
		
		int pollResult = update_pollfds(group->connect_poll.first.begin(), group->connect_poll.first.size());
		if(pollResult > 0)
		{
			int count = 0;
			std::vector<struct pollfd>::iterator i;
            for(i = group->connect_poll.first.begin(); i != group->connect_poll.first.end(); ++i)
			{
				struct pollfd & f = (*i);
				if(f.revents)
				{
					count++;
					if(! (f.revents & POLLERR))
					{
						if(f.revents & POLLOUT)
						{
							// find the socket
							std::hash_map<int, ps_socket *>::iterator findConnect = group->ps_sockets.find(f.fd);
							if(findConnect != group->ps_sockets.end())
							{
								ps_socket * sock = (*findConnect).second;
								// check the socket for an error
								int optval;
								socklen_t socklen = sizeof(optval);
								getsockopt(f.fd, SOL_SOCKET, SO_ERROR, &optval, &socklen);
								if(optval != 0)
								{
									switch(optval)
									{
										case EBADF:
											post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, "The file descriptor is not a valid index in the descriptor table");
											break;
										case EFAULT:
											post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, "The socket structure address is outside the user's address space.\n");
											break;
										case ENOTSOCK:
											post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, "The file descriptor is not associated with a socket.\n");
											break;
										case EISCONN:
											post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, "The socket is already connected.\n");
											break;
										case ECONNREFUSED:
											post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, "No one listening on the remote address.\n");
											break;
										case ETIMEDOUT:
											post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, "Timeout while attempting connection. The server may be too busy to accept new connections. Note that for IP sockets the timeout may be very long when syncookies are enabled on the server.\n");
											break;
										case ENETUNREACH:
											post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, "Network is unreachable.\n");
											break;
										case EADDRINUSE:
											post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, "Local address is already in use.\n");
											break;
										case EINPROGRESS:
											optval = 0;
											break;
										case EALREADY:
											optval = 0;
											break;
										case EAGAIN:
											optval = 0;
											break;
										case EAFNOSUPPORT:
											post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, "The passed address didn't have the correct address family in its sa_family field.\n");
											break;
										case EPERM:
											post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, "The user tried to connect to a broadcast address without having the socket broadcast flag enabled or the connection request failed because of a local firewall rule.\n");
											break;
										default:
											post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, "An unknown error occurred while processing poll() in TcpClientSock::update()\n");
											break;
									}
								}
								else
								{
									// no errors
									ps_socket * connector = (*findConnect).second;
									struct sockaddr_in remote;
									socklen_t remoteLen;
									const char *  peerErr = ps_getpeername(f.fd, &remote);
									if(peerErr != 0)
									{
										post_error(group, sock, ps_error_result::SOCKET_NOT_CONNECTED, peerErr);									
									}
									else
									{
										ps_socket_linux * l = (ps_socket_linux *)connector->reserved_internal;
										l->read_index = add_descriptor(&group->recv_poll, l);
										l->write_index = add_descriptor(&group->send_poll, l);
										l->read_index->events = 0;
										l->write_index->events = 0;
										
										// remove the connecting socket from the connect poll set
										remove_descriptor(&group->connect_poll, l->tcp_init_index);
										
										ps_connect_result r;
										r.s = connector;
										r.remote = remote;
										group->completed_connects.push_back(r);
										group->event_count++;
										if(group->queued_connects > 0)
											group->queued_connects--;
										result = true;
									}
								}
							}
						}
					}
					else
					{
						perror("connect");
					}
					if(count == pollResult)
						break;
				}
			}
		}
	}
	
	return result;
}
// ----------------------------------------------------------------------

const bool query_recv_completions(ps_group_linux * group)
{
	bool result = false;
	
	if(!group->recv_poll.first.empty())
	{
		int pollResult = update_pollfds(group->recv_poll.first.begin(), group->recv_poll.first.size());
		if(pollResult > 0)
		{
			std::vector<struct pollfd>::iterator i;
			int count = 0;
			for(i = group->recv_poll.first.begin(); i != group->recv_poll.first.end(); ++i)
			{
				struct pollfd & f = (*i);
				if(! (f.revents & POLLERR))
				{
					if(f.revents == POLLIN)
					{
						// find the socket, use the user-supplied buffers
						std::hash_map<int, ps_socket *>::iterator findSocket = group->ps_sockets.find(f.fd);
						if(findSocket != group->ps_sockets.end())
						{
							// have an associated socket for this receive
							ps_socket * sock = (*findSocket).second;
							ps_socket_linux * sl = (ps_socket_linux *)sock->reserved_internal;
							if(! sl->recv_buffers.empty())
							{
								// have a receive buffer
								ps_recvfrom_result * rf = sl->recv_buffers.begin();
								int bytesReceived = recv(sock->s, (void *)rf->data, rf->length, 0);
								if(bytesReceived == -1)
								{
									// error or deferred
									if(errno != EAGAIN)
									{
										switch(errno)
										{
											case EBADF:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The argument s is an invalid descriptor.");
												break;
											case ECONNREFUSED:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "A remote host refused to allow the network connection (typically because it is not running the requested service).");
												break;
											case ENOTCONN:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).");
												break;
											case ENOTSOCK:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The argument s does not refer to a socket.");
												break;
											case EINTR:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The receive was interrupted by delivery of a signal before any data were available.");
												break;
											case EFAULT:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The receive buffer pointer(s) point outside the process's address space.");
												break;
											case EINVAL:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "Invalid argument passed.");
												break;
											default:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "An unknown error occured while processing recv().");
												break;
										}
									}
								}
								else if(bytesReceived == 0)
								{
									post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The connection has been closed by the remote host.");
								}
								else if(bytesReceived > 0)
								{
									// successful read
									ps_recv_result r;
									r.s = sock;
									r.data = rf->data;
									r.length = bytesReceived;
									if(group->queued_recvs > 0)
										group->queued_recvs--;
									group->event_count++;
									sl->recv_buffers.erase(sl->recv_buffers.begin());
									if(sl->recv_buffers.empty())
									{
										f.events = 0;
									}
									group->completed_recvs.push_back(r);
									result = true;
								}
							}
							else
							{
								f.events = 0;
							}
						}
						else
						{
							f.events = 0;
						}
					}
				}
				else
				{
					perror("recv");
				}
			}
		}
	}
	return result;
}

// ----------------------------------------------------------------------

const bool query_recvfrom_completions(ps_group_linux * group)
{
	bool result = false;
    if(!group->recvfrom_poll.first.empty())
	{
		int pollResult = update_pollfds(group->recvfrom_poll.first.begin(), group->recvfrom_poll.first.size());
		if(pollResult > 0)
		{
			std::vector<struct pollfd>::iterator i;
			int count = 0;
			for(i = group->recvfrom_poll.first.begin(); i != group->recvfrom_poll.first.end(); ++i)
			{
				struct pollfd & f = (*i);
				if(! (f.revents & POLLERR))
				{
					if(f.revents & POLLIN)
					{
						// find the socket, use the user-supplied buffers
						std::hash_map<int, ps_socket *>::iterator findSocket = group->ps_sockets.find(f.fd);
						if(findSocket != group->ps_sockets.end())
						{
							// have an associated socket for this receive
							ps_socket * sock = (*findSocket).second;
							ps_socket_linux * sl = (ps_socket_linux *)sock->reserved_internal;
							if(! sl->recv_buffers.empty())
							{
								// have a receive buffer
								ps_recvfrom_result * rf = sl->recv_buffers.begin();
								struct sockaddr_in remote;
								socklen_t remoteLen;
								int bytesReceived = recvfrom(sock->s, (void *)rf->data, rf->length, 0, (struct sockaddr *)&remote, &remoteLen);
								if(bytesReceived == -1)
								{
									// error or deferred
									if(errno != EAGAIN)
									{
										switch(errno)
										{
											case EBADF:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The argument s is an invalid descriptor.");
												break;
											case ECONNREFUSED:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "A remote host refused to allow the network connection (typically because it is not running the requested service).");
												break;
											case ENOTCONN:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).");
												break;
											case ENOTSOCK:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The argument s does not refer to a socket.");
												break;
											case EINTR:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The receive was interrupted by delivery of a signal before any data were available.");
												break;
											case EFAULT:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The receive buffer pointer(s) point outside the process's address space.");
												break;
											case EINVAL:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "Invalid argument passed.");
												break;
											default:
												post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "An unknown error occured while processing recvfrom().");
												break;
										}
									}
								}
								else if(bytesReceived == 0)
								{
									post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The connection has been closed by the remote host.");
								}
								else if(bytesReceived > 0)
								{
									// successful read
									ps_recvfrom_result r;
									r.s = sock;
									r.data = rf->data;
									r.length = bytesReceived;
									r.remote = remote;
									if(group->queued_recvfroms > 0)
										group->queued_recvfroms--;
									group->event_count++;
									sl->recv_buffers.erase(sl->recv_buffers.begin());
									if(sl->recv_buffers.empty())
									{
										f.events = 0;
									}
									group->completed_recvfroms.push_back(r);
									result = true;
								}
							}
							else
							{
								f.events = 0;
							}
						}
						else
						{
							f.events = 0;
						}
					}
				}
				else
				{
					perror("recvfrom");
				}
			}
		}
	}
	return result;
}

// ----------------------------------------------------------------------

const bool query_completions(ps_group_linux * group)
{
	bool result = false;

	if(query_accept_completions(group))
		result = true;

	if(query_connect_completions(group))
		result = true;

	if(query_recv_completions(group))
		result = true;

	if(query_recvfrom_completions(group))
		result = true;
	
	return result;
}

// ----------------------------------------------------------------------

const bool set_non_blocking(int handle)
{
	bool result = false;
	unsigned long int nb = 1;
	int err;
	err = ioctl(handle, FIONBIO, &nb); //lint !e569 // loss of precision in the FIONBIO macro, beyond my control
	if(err == -1)
	{
		switch(errno)
		{
			case EBADF:
				fprintf(stderr, "d is not a valid descriptor.\n");
				break;
			case EFAULT:
				fprintf(stderr, "argp references an inaccessible memory area.\n");
				break;
			case ENOTTY:
				fprintf(stderr, "The specified request does not apply to the kind of object that the descriptor d references.\n");
				break;
			case EINVAL:
				fprintf(stderr, "Request or argp is not valid.\n");
				break;
			default:
				fprintf(stderr, "An unknown error occured while processing ioctl() for nonblocking operation\n");
				break;
		}
	}
	else
	{
		result = true;
	}
	return result;
}

// ----------------------------------------------------------------------

const ps_socket * create_socket(ps_group group, void * userData, const int type, const int proto)
{
	ps_socket * result = 0;
	int s = socket(AF_INET, type, proto);
	if(s == -1)
	{
		switch(errno)
		{
			case EPROTONOSUPPORT:
				post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "The protocol type or the specified protocol is not supported within this domain.");
				break;
			case ENFILE:
				post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Not enough kernel memory to allocate a new socket structure.");
				break;
			case EMFILE:
				post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Process file table overflow.");
				break;
			case EACCES:
				post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Permission to create a socket of the specified type and/or protocol is denied.");
				break;
			case ENOBUFS:
				post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Insufficient memory is available.  The socket cannot be created until sufficient resources are freed.");
				break;
			case EINVAL:
				post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "Unknown protocol, or protocol family not available.");
				break;
			default:
				post_error(group, 0, ps_error_result::SOCKET_INIT_FAILED, "An unknown error occurred while processing socket().");
				perror("Error ");
				break;
		}
	}
	else
	{
		if(set_non_blocking(s))
		{
			ps_socket_linux * linux_result = new ps_socket_linux;
			
			result = new ps_socket;
			result->s = s;
			result->group = group;
			result->userData = userData;
			result->reserved_internal = linux_result;
			linux_result->sock = result;
			ps_group_linux * g = (ps_group_linux *)group;
			g->ps_sockets.insert(std::pair<int, ps_socket *>(s, result));
		}
	}
	return result;
}
		   
   
	
// ----------------------------------------------------------------------

const bool ps_bind_socket(const ps_socket * sock, const struct sockaddr_in * bindAddress)
{
	bool result = false;
	socklen_t addrLen = sizeof(struct sockaddr_in);
	int errCode = bind(sock->s, reinterpret_cast<const struct sockaddr *>(bindAddress), addrLen);
	if(errCode != 0)
    {
		switch(errno)
		{
			case EBADF:
				post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "sockfd is not a valid descriptor.");
				break;
			case EINVAL: 
				post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "The socket is already bound to an address.  This may change in the future: see linux/unix/sock.c for details.");
				break;
			case EACCES:
				post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "The address is protected, and the user is not the super-user.");
				break;
			case ENOTSOCK:
				post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "Argument is a descriptor for a file, not a socket.");
				break;
			default:
				post_error(sock->group, sock, ps_error_result::SOCKET_NOT_BOUND, "An unknown error occurred while processing bind().");
				break;
		}
		
    }
	else
    {
		result = true;
    }
	return result;
}

// ----------------------------------------------------------------------

ps_group ps_create_group(void)
{
	ps_group_linux * result = new ps_group_linux;
	result->event_count = 0;
    result->queued_accepts = 0;
	result->queued_connects = 0;
	result->queued_recvs = 0;
	result->queued_recvfroms = 0;
	return result;
}

// ----------------------------------------------------------------------

const ps_socket * ps_create_tcp_socket(ps_group group, void * userData)
{
	const ps_socket * result = 0;
	
	struct protoent * p = getprotobyname("tcp");
	if(p)
    {
		result = create_socket(group, userData, SOCK_STREAM, p->p_proto);
    }
	return result;
}

// ----------------------------------------------------------------------

const ps_socket * ps_create_udp_socket(ps_group group, void * userData)
{
	const ps_socket * result = 0;

	struct protoent * p = getprotobyname("udp");
	if(p)
	{
		result = create_socket(group, userData, SOCK_DGRAM, p->p_proto);
		ps_group_linux * g = (ps_group_linux *)group;
		ps_socket_linux * l = (ps_socket_linux *)result->reserved_internal;
		l->read_index = add_descriptor(&g->recvfrom_poll, l);
		l->write_index = add_descriptor(&g->sendto_poll, l);
		l->read_index->events = POLLIN;
		l->write_index->events = POLLOUT;
	}
	return result;
}

// ----------------------------------------------------------------------

void ps_destroy_group(ps_group group)
{
	ps_group_linux * g = (ps_group_linux *)group;
	std::hash_map<int, ps_socket *>::iterator i;
	for(i = g->ps_sockets.begin(); i != g->ps_sockets.end(); ++i)
	{
		ps_socket * p = (*i).second;
		close(p->s);
		ps_socket_linux * l = (ps_socket_linux *)p->reserved_internal;
		delete l;
		delete p;
	}
	delete g;
}

// ----------------------------------------------------------------------

void ps_destroy_socket(const ps_socket * sock)
{
	if(sock)
	{
		ps_group_linux * g = (ps_group_linux *)sock->group;
		std::hash_map<int, ps_socket *>::iterator i = g->ps_sockets.find(sock->s);
		if(i != g->ps_sockets.end())
		{
			close(sock->s);
			g->ps_sockets.erase(i);
			ps_socket_linux * l = (ps_socket_linux *)sock->reserved_internal;
			delete l;
			delete sock;
		}
	}
}

// ----------------------------------------------------------------------

const bool ps_has_events(ps_group group)
{
	ps_group_linux * g = (ps_group_linux *)group;
	return g->event_count > 0;
}

// ----------------------------------------------------------------------

const bool ps_listen(const ps_socket * sock)
{
	bool result = false;
	int err = listen(sock->s, 256);
	if(err == -1)
	{
		switch(errno)
		{
			case EBADF:
				post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "The argument s is not a valid descriptor.");
				break;
			case ENOTSOCK:
				post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "The argument s is not a socket.");
				break;
			case EOPNOTSUPP:
				post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "The socket is not of a type that supports the listen operation.");
				break;
			default:
				post_error(sock->group, sock, ps_error_result::SOCKET_NOT_LISTENING, "An unknown error was encountered while processing listen().");
				break;
		}   
	}
	else
	{
		result = true;
		ps_group_linux * g = (ps_group_linux *)sock->group;
		ps_socket_linux * l = (ps_socket_linux *)sock->reserved_internal;
		l->tcp_init_index = add_descriptor(&g->accept_poll, l);
		l->tcp_init_index->events = POLLIN | POLLOUT | POLLERR | POLLHUP;
	}
	return result;
}

// ----------------------------------------------------------------------

const bool ps_query_accept(ps_group group, ps_accept_result * target)
{
	bool result = false;
	ps_group_linux * g = (ps_group_linux *)group;
	if(g->queued_accepts == 0)
	{
		post_error(group, 0, ps_error_result::NO_PENDING_OPERATIONS, "An accept operation was queried, but there was no previous call to ps_queue_accept()");
	}
	else
	{
		query_accept_completions(g);
		if(!g->completed_accepts.empty())
		{
			std::vector<ps_accept_result>::iterator i = g->completed_accepts.begin();
			target->listener = (*i).listener;
			target->newSocket = (*i).newSocket;
			target->remote = (*i).remote;
			target->local = (*i).local;
			result = true;
			if(g->event_count > 0)
				g->event_count--;
			g->queued_accepts--;
			g->completed_accepts.erase(i); //lint !e534
		}
	}
	return result;
}

// ----------------------------------------------------------------------

const bool ps_query_connect(ps_group group, ps_connect_result * target)
{
	bool result = false;
	ps_group_linux * g = (ps_group_linux *)group; //lint !e1924
	if(g->queued_connects == 0)
	{
		post_error(group, 0, ps_error_result::NO_PENDING_OPERATIONS, "A connect operations was queried, but there was no previous call to ps_queue_connect()");
	}
	else
	{
		query_connect_completions(g);
		if(!g->completed_connects.empty())
		{
			std::vector<ps_connect_result>::iterator i = g->completed_connects.begin();
			target->remote = (*i).remote;
			target->s = (*i).s;
			result = true;
			if(g->event_count > 0)
				g->event_count--;
			g->queued_connects--;
			g->completed_connects.erase(i); //lint !e534
		}
	}
	return result;
}

// ----------------------------------------------------------------------

const bool ps_query_error(ps_group group, ps_error_result * target)
{
	bool result = false;
	ps_group_linux * g = (ps_group_linux *)group; //lint !e1924

	if(!g->errors.empty())
	{
		std::vector<ps_error_result>::iterator i = g->errors.begin();
		target->err = (*i).err;
		target->s = (*i).s;
		target->errString = (*i).errString;
		result = true;
		if(g->event_count > 0)
			g->event_count--;
		g->errors.erase(i); //lint !e534
	}
	return result;
}

// ----------------------------------------------------------------------

const bool ps_query_recvfrom(ps_group group, ps_recvfrom_result * target)
{
	bool result = false;
	ps_group_linux * g = (ps_group_linux *)group; //lint !e1924
	if(g->queued_recvfroms == 0)
	{
		post_error(group, 0, ps_error_result::NO_PENDING_OPERATIONS, "A recvfrom operation was queried, but there was no prior call to ps_queue_recvfrom()");
	}
	else
	{
		query_recvfrom_completions(g);
		if(!g->completed_recvfroms.empty())
		{
			std::vector<ps_recvfrom_result>::iterator i = g->completed_recvfroms.begin();
			target->data = (*i).data;
			target->length = (*i).length;
			target->s = (*i).s;
			memcpy(&target->remote, &(*i).remote, sizeof(struct sockaddr_in));
			result = true;
			if(g->event_count > 0)
				g->event_count--;
			g->queued_recvfroms--;
			g->completed_recvfroms.erase(i); //lint !e534
		}
	}
	return result;
}

//---------------------------------------------------------------------

const bool ps_query_recv(ps_group group, ps_recv_result * target)
{
	bool result = false;
	ps_group_linux * g = (ps_group_linux *)group; //lint !e1924
	if(g->queued_recvs == 0)
	{
		post_error(group, 0, ps_error_result::NO_PENDING_OPERATIONS, "A recv operation was queried, but there was no prior call to ps_queue_recv()");
	}
	else
	{
		query_recv_completions(g);

		if(!g->completed_recvs.empty())
		{
			std::vector<ps_recv_result>::iterator i = g->completed_recvs.begin();
			target->data = (*i).data;
			target->length = (*i).length;
			target->s = (*i).s;
			result = true;
			if(g->event_count > 0)
			g->event_count--;
			g->queued_recvs--;
			g->completed_recvs.erase(i); //lint !e534
		}
	}
	return result;
}

// ----------------------------------------------------------------------

const bool ps_queue_accept(const ps_socket * listener, const ps_socket * acceptSock)
{
	bool result = false;
	ps_socket_linux * l = (ps_socket_linux *)listener->reserved_internal;
	l->accept_socks.push_back((ps_socket *)acceptSock);
	ps_group_linux * g = (ps_group_linux *)listener->group;
	
	struct sockaddr_in remote;
	socklen_t remoteLen;

	// check for an immediate connection, which can potentially forego
	// poll() and lookup
	int err = accept(listener->s, (struct sockaddr *)&remote, &remoteLen);
	if(err == -1)
	{
		if(errno != EWOULDBLOCK)
		{
			switch(errno)
			{
				case EWOULDBLOCK:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "The socket is marked non-blocking and no connections are present to be accepted.");
					break;
				case EBADF:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "The descriptor is invalid.");
					break;
				case ENOTSOCK:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "The descriptor references a file, not a socket.");
					break;
				case EOPNOTSUPP:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "The referenced socket is not of type SOCK_STREAM.");
					break;
				case EFAULT:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "The addr parameter is not in a writable part of the user address space.");
					break;
				case EPERM:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "Firewall rules forbid connection.");
					break;
				case ENOMEM:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "Not enough free memory.  This often means that the memory allocation is limited by the socket buffer limits, not by the system memory.");
					break;
				default:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "An unknown error occurred while processing accept(),");
					break;
			}
		}
		else
		{
			l->tcp_init_index->events = POLLIN | POLLOUT | POLLERR | POLLHUP;
			result = true;
			g->queued_accepts++;
		}
	}
	else
	{
		// accept is immediately ready with a new socket, post the event
		ps_accept_result a;
		ps_socket * writeable = (ps_socket *)acceptSock;
		writeable->s = err;
		a.listener = listener;
		a.newSocket = acceptSock;
		a.remote = remote;
		struct sockaddr_in local;
		socklen_t localLen;
		int nameErr = getsockname(listener->s, (struct sockaddr *)&local, &localLen);
		if(nameErr == -1)
		{
			switch(errno)
			{
				case EBADF:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "The argument s is not a valid descriptor.");
					break;
				case ENOTSOCK:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "The argument s is a file, not a socket.");
					break;
				case ENOBUFS:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "Insufficient resources were available in the system to perform the operation.");
					break;
				case EFAULT:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "The name parameter points to memory not in a valid part of the process address space.");
					break;
				default:
					post_error(listener->group, listener, ps_error_result::SOCKET_ACCEPT_FAILED, "An unknown error occurred while processing getsockname().");
					break;
			}   
		}
		else
		{
			a.local = local;
			g->completed_accepts.push_back(a);
			g->event_count++;
			l->tcp_init_index->events = 0; // don't poll this socket unless another accept is attempted and would block
			result = true;
		}
	}
	return result;
}

// ----------------------------------------------------------------------

const bool ps_queue_connect(const ps_socket * connector, const struct sockaddr_in * remote)
{
	bool result = false;
	socklen_t addrLen = sizeof(struct sockaddr_in);
	int err = connect(connector->s, (struct sockaddr *)remote, addrLen);
	ps_group_linux * g = (ps_group_linux *)connector->group;
	ps_socket_linux * l = (ps_socket_linux *)connector->reserved_internal;
	
	if(err == -1)
	{
		if(errno != EINPROGRESS)
		{
			switch(errno)
			{
				case EBADF:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The file descriptor is not a valid index in the descriptor table.");
					break;
				case EFAULT:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The socket structure address is outside the user's address space.");
					break;
				case ENOTSOCK:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The file descriptor is not associated with a socket.");
					break;
				case EISCONN:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The socket is already connected.");
					break;
				case ECONNREFUSED:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "No one listening on the remote address.");
					break;
				case ETIMEDOUT:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "Timeout while attempting connection. The server may be too busy to accept new connections. Note that for IP sockets the timeout may be very long when syncookies are enabled on the server.");
					break;
				case ENETUNREACH:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "Network is unreachable.");
					break;
				case EADDRINUSE:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "Local address is already in use.");
					break;
				case EALREADY:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The socket is non-blocking and a previous connection attempt has not yet been completed.");
					break;
				case EAGAIN:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "No more free local ports or insufficient entries in the routing cache. For PF_INET see the net.ipv4.ip_local_port_range sysctl in ip(7) on how to increase the  number of local ports.");
					break;
				case EAFNOSUPPORT:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The passed address didn't have the correct address family in its sa_family field.");
				case EACCES:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "The user tried to connect to a broadcast address without having the socket broadcast flag enabled or the connection request failed because of a local firewall rule.");
					break;
				default:
					post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, "An unknown error occurred while processing connect().");
					break;
			}
		}
		else
		{
			struct pollfd fd;
			fd.fd = connector->s;
			fd.events = POLLOUT | POLLIN | POLLERR | POLLHUP;
			fd.revents = 0;
			l->tcp_init_index = add_descriptor(&g->connect_poll, l);
			result = true;
			g->queued_connects++;
		}
	}
	else
	{
		// connect completed immediately! Post the result!
		struct sockaddr_in remote;
		socklen_t remoteLen = sizeof(struct sockaddr_in);
		const char * peerErr = ps_getpeername(connector->s, &remote);
		if(peerErr != 0)
		{
			post_error(connector->group, connector, ps_error_result::CONNECTION_FAILED, peerErr);
		}
		else
		{
			ps_connect_result r;
			r.s = connector;
			r.remote = remote;
			g->completed_connects.push_back(r);
			g->event_count++;
			result = true;
		}
	}
	return result;
}

// ----------------------------------------------------------------------

const bool ps_queue_recv(const ps_socket * sock, void * recvBuffer, const unsigned int recvBufferSize)
{
	bool result = false;
	int recvBytes = recv(sock->s, recvBuffer, recvBufferSize, 0);
	if(recvBytes == -1)
	{
		// error or deferred
		if(errno != EAGAIN)
		{
			switch(errno)
			{
				case EBADF:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The argument s is an invalid descriptor.");
					break;
				case ECONNREFUSED:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "A remote host refused to allow the network connection (typically because it is not running the requested service).");
					break;
				case ENOTCONN:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and accept(2)).");
					break;
				case ENOTSOCK:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The argument s does not refer to a socket.");
					break;
				case EINTR:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The receive was interrupted by delivery of a signal before any data were available.");
					break;
				case EFAULT:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The receive buffer pointer(s) point outside the process's address space.");
					break;
				case EINVAL:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "Invalid argument passed.");
					break;
				default:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "An unknown error occured while processing recv().");
					break;
			}
		}
		else
		{
			// recv deferred
			result = true;
			ps_socket_linux * s = (ps_socket_linux *)sock->reserved_internal;
			ps_recvfrom_result r;
			r.s = sock;
			r.data = recvBuffer;
			r.length = recvBufferSize; // used until data arrives
			s->recv_buffers.push_back(r);
			ps_group_linux * l = (ps_group_linux *)sock->group;
			l->queued_recvs++;
			s->read_index->events = POLLIN;

		}
	}
	else if(recvBytes == 0)
	{
		post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The connection has been closed by the remote host.");
	}
	else if(recvBytes > 0)
	{
		result = true;
		ps_recv_result r;
		r.s = sock;
		r.data = recvBuffer;
		r.length = recvBytes;
		ps_group_linux * l = (ps_group_linux *)sock->group;
		l->completed_recvs.push_back(r);
		l->queued_recvs++;
		l->event_count++;
	}
	return result;
}

// ----------------------------------------------------------------------

const bool ps_queue_recvfrom(const ps_socket * sock, void * recvBuffer, const unsigned int recvBufferSize)
{
    bool result = false;
	struct sockaddr_in remote;
	socklen_t remoteLen = sizeof(struct sockaddr_in);
	int recvBytes = recvfrom(sock->s, recvBuffer, recvBufferSize, 0, (struct sockaddr *)&remote, &remoteLen);
	if(recvBytes == -1)
	{
		// error or deferred
		if(errno != EAGAIN)
		{
			switch(errno)
			{
				case EBADF:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The argument s is an invalid descriptor.");
					break;
				case ECONNREFUSED:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "A remote host refused to allow the network connection (typically because it is not running the requested servi\
ce).");
					break;
				case ENOTCONN:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The socket is associated with a connection-oriented protocol and has not been connected (see connect(2) and ac\
cept(2)).");
					break;
				case ENOTSOCK:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The argument s does not refer to a socket.");
					break;
				case EINTR:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The receive was interrupted by delivery of a signal before any data were available.");
					break;
				case EFAULT:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The receive buffer pointer(s) point outside the process's address space.");
					break;
				case EINVAL:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "Invalid argument passed.");
					break;
				default:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "An unknown error occured while processing recv().");
					break;
			}
		}
		else
		{
			// recv deferred
			result = true;
			ps_socket_linux * s = (ps_socket_linux *)sock->reserved_internal;
			ps_recvfrom_result r;
			r.s = sock;
			r.data = recvBuffer;
			r.length = recvBufferSize; // used until data arrives
			s->recv_buffers.push_back(r);
			ps_group_linux * l = (ps_group_linux *)sock->group;
			l->queued_recvfroms++;
			s->read_index->events = POLLIN;

		}
	}
	else if(recvBytes == 0)
	{
		post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The connection has been closed by the remote host.");
	}
	else if(recvBytes > 0)
	{
		result = true;
		ps_recvfrom_result r;
		r.s = sock;
		r.remote = remote;
		r.data = recvBuffer;
		r.length = recvBytes;
		ps_group_linux * l = (ps_group_linux *)sock->group;
		l->completed_recvfroms.push_back(r);
		l->queued_recvfroms++;
		l->event_count++;
	}
	return result;
}

// ----------------------------------------------------------------------

const bool ps_queue_send(const ps_socket * sock, const void * data, const unsigned int dataLen)
{
	bool result = false;

	ps_socket_linux * l = (ps_socket_linux *)sock->reserved_internal;

	// if there is pending data unsent from a previous ps_queue_send,
	// flush it. 
	if(l->pending_send_data.size())
	{
		if(! flush_pending_send_data(l))
		{
			// not all of the data could be flushed, queue the current
			// send buffer and bail out
			queue_pending_send_data(l, (unsigned char *)data, dataLen);
			return result;
		}
	}

	int bytesSent = send(sock->s, data, dataLen, 0);
	if(bytesSent == -1)
	{
		if(errno != EAGAIN)
		{
			switch(errno)
			{
				case EBADF:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "An invalid descriptor was specified.");
					break;
				case ENOTSOCK:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The argument s is not a socket.");
					break;
				case EFAULT:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "An invalid user space address was specified for a parameter.");
					break;
				case EMSGSIZE:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The socket requires that message be sent atomically, and the size of the message to be sent made this impossible.");
					break;
				case ENOBUFS:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The output queue for a network interface was full.  This generally indicates that the interface has stopped sending, but may  be  caused  by  transient  congestion. (This cannot occur in Linux, packets are just silently dropped when a device queue overflows.)");
					break;
				case ENOMEM:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "No memory available.");
					break;
				case EINVAL:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "Invalid argument passed.");
					break;
				case EPIPE:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "The local end has been shut down on a connection oriented socket.  In this case the process will also receive a SIGPIPE unless MSG_NOSIGNAL is set.");
					break;
				default:
					post_error(sock->group, sock, ps_error_result::CONNECTION_CLOSED, "An unknown error occurred while processing send()");
					break;
			}
		}
		else
		{
			// queue the whole buffer for a send when the socket becomes writeable
			result = true;
			queue_pending_send_data(l, (unsigned char *)data, dataLen);
			l->write_index->events = POLLOUT;
		}
	}
	else if(bytesSent < dataLen)
	{
		// only a partial send has occurred, queue the remainder and send
		// it when the socket becomes writeable
		result = true;
		unsigned char * c = (unsigned char *)data;
		queue_pending_send_data(l, &c[bytesSent], dataLen - bytesSent);
		l->write_index->events = POLLOUT;
	}
	else
	{
		result = true;
	}
	
	return result;
}

// ----------------------------------------------------------------------
						 
const bool ps_queue_sendto(const ps_socket * sock, const struct sockaddr_in * to, const void * data, const unsigned int dataLen)
{
	bool result = false;
	socklen_t toLen = sizeof(struct sockaddr_in);
	int bytesSent = sendto(sock->s, data, dataLen, 0, (const struct sockaddr *)to, toLen);
	if(bytesSent != -1)
	{
		result = true;
	}
	return result;
}
