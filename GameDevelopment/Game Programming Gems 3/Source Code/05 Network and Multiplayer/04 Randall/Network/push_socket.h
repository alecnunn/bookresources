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

#ifndef	_PushSocket_H
#define	_PushSocket_H

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------

#include "Network/NetworkPlatform.h"

//---------------------------------------------------------------------

typedef const void * ps_group;

//---------------------------------------------------------------------

typedef struct _ps_socket
{
	SOCKET_HANDLE  s;
	void *         userData;
	ps_group       group;

	const void *   reserved_internal;

} ps_socket;

//---------------------------------------------------------------------

typedef struct _ps_accept_result
{
	const ps_socket *   listener;
	const ps_socket *   newSocket;
	struct sockaddr_in  remote;
	struct sockaddr_in  local;
} ps_accept_result;

//---------------------------------------------------------------------

typedef struct _ps_connect_result
{
	const ps_socket *   s;
	struct sockaddr_in  remote;
} ps_connect_result;

//---------------------------------------------------------------------

typedef struct _ps_error_result
{
	enum
	{
		CONNECTION_CLOSED,
		CONNECTION_FAILED,
		NO_PENDING_OPERATIONS,
		SOCKET_NOT_BOUND,
		SOCKET_NOT_LISTENING,
		SOCKET_NOT_CONNECTED,
		SOCKET_INIT_FAILED,
		SOCKET_ACCEPT_FAILED
	};
	const ps_socket *  s;
	int                err;
	const char *       errString;
} ps_error_result;

//---------------------------------------------------------------------

typedef struct _ps_recvfrom_result
{
	const ps_socket *   s;
	struct sockaddr_in  remote;
	const void *        data;
	unsigned int        length;
} ps_recvfrom_result;

//---------------------------------------------------------------------

typedef struct _ps_recv_result
{
	const ps_socket *   s;
	const void *        data;
	unsigned int        length;
} ps_recv_result;

//---------------------------------------------------------------------

const bool          ps_bind_socket        (const ps_socket * sock, const struct sockaddr_in * bindAddress);
ps_group            ps_create_group       (void);
const ps_socket *   ps_create_tcp_socket  (ps_group group, void * userData);
const ps_socket *   ps_create_udp_socket  (ps_group group, void * userData);
void                ps_destroy_group      (ps_group group);
void                ps_destroy_socket     (const ps_socket * sock);
const unsigned long ps_get_host_address   (void);
const bool          ps_has_events         (ps_group group);
const bool          ps_listen             (const ps_socket * sock);
const bool          ps_get_local_address  (const ps_socket * sock, struct sockaddr_in & result);
const bool          ps_query_accept       (ps_group group, ps_accept_result * target);
const bool          ps_query_connect      (ps_group group, ps_connect_result * target);
const bool          ps_query_error        (ps_group group, ps_error_result * target);
const bool          ps_query_recv         (ps_group group, ps_recv_result * target);
const bool          ps_query_recvfrom     (ps_group group, ps_recvfrom_result * target);
const bool          ps_queue_accept       (const ps_socket * listener, const ps_socket * acceptSock);
const bool          ps_queue_connect      (const ps_socket * connector, const struct sockaddr_in * remote);
const bool          ps_queue_recv         (const ps_socket * ps, void * recvBuffer, const unsigned int recvBufferSize);
const bool          ps_queue_recvfrom     (const ps_socket * ps, void * recvBuffer, const unsigned int recvBufferSize);
const bool          ps_queue_send         (const ps_socket * ps, const void * data, const unsigned int length);
const bool          ps_queue_sendto       (const ps_socket * ps, const struct sockaddr_in * to, const void * data, const unsigned int length);

//---------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------

#endif	// _PushSocket_H
