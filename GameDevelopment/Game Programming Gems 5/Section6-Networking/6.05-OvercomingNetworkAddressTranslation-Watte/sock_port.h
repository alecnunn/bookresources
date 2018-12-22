
/*
 *
 * Copyright (c) 2003 Jon Watte
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */


#if !defined( sock_port_h )
#define sock_port_h

// There are differences between Linux/Berkeley sockets and WinSock2
// This file wraps some of the more common ones (as used by this library).

#include <stdarg.h>

#if defined( WIN32 )

 // Windows features go here
 #include <Winsock2.h>
 #include <stdio.h>
 
 #define NEED_SHORT_TYPES 1
 #define NEED_WINDOWS_POLL 1
 #define NEED_GETTIMEOFDAY 1
 #define NEED_SNPRINTF 1
 #define NEED_HSTRERROR 1
 #define NEED_GETLASTERROR 1

 typedef int socklen_t;
 #define MSG_NOSIGNAL 0
 #define MAKE_SOCKET_NONBLOCKING(x,r) \
   do { u_long _x = 1; (r) = ioctlsocket( (x), FIONBIO, &_x ); } while(0)
 #define NONBLOCK_MSG_SEND 0
 #define INIT_SOCKET_LIBRARY() \
   do { WSADATA wsaData; WSAStartup( MAKEWORD(2,2), &wsaData ); } while(0)

 #pragma warning( disable: 4250 )

 #define SIN_ADDR_UINT(x) \
   ((uint&)(x).S_un.S_addr)
 #define BAD_SOCKET_FD 0xffffffffU
 
#else

 // Linux features go here
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netdb.h>
 #include <sys/time.h>
 #include <sys/poll.h>

 #define NEED_CLOSESOCKET 1
 #define NEED_IOCTLSOCKET 1
 #define NEED_SHORT_TYPES 1
 #define NEED_ERRNO 1

 extern int h_errno;
 // hack -- we don't make it non-blocking; instead, we pass 
 // NONBLOCK_MSG_SEND for each call to sendto()
 #define MAKE_SOCKET_NONBLOCKING(x,r) do { (r) = 0; } while(0)
 #define NONBLOCK_MSG_SEND MSG_DONTWAIT
 #define INIT_SOCKET_LIBRARY() do {} while(0)

 #define SOCKET int
 #define SIN_ADDR_UINT(x) \
   ((uint&)(x).s_addr)
 #define BAD_SOCKET_FD -1

#endif

#if NEED_CLOSESOCKET
 #define closesocket close
#endif

#if NEED_PROTOENT
 struct protoent {
   int p_proto;
 };
#endif
#if NEED_GETPROTOENT
 inline protoent const * getprotobyname( char const * s ) {
   static protoent p;
   if( !strcmp( s, "udp" ) ) {
     p.p_proto = IPPROTO_UDP;
     return &p;
   }
   if( !strcmp( s, "tcp" ) ) {
     p.p_proto = IPPROTO_TCP;
     return &p;
   }
   return 0;
 }
#endif

#if NEED_SHORT_TYPES
 typedef unsigned long ulong;
 typedef unsigned int uint;
 typedef unsigned short ushort;
 typedef unsigned char uchar;
#endif

#if NEED_WINDOWS_POLL
 struct pollfd {
   SOCKET fd;
   unsigned short events;
   unsigned short revents;
 };
 #define POLLIN 0x1
 #define POLLPRI 0x2
 #define POLLOUT 0x4
 #define POLLERR 0x100
 #define POLLHUP 0x200
 #define POLLNVAL 0x8000
 int poll( pollfd * iofds, size_t count, int ms );
#endif

#if NEED_GETTIMEOFDAY
 // this is not thread safe!
 void gettimeofday( timeval * otv, int );
#endif

#if NEED_SNPRINTF
 inline int vsnprintf( char * buf, int size, char const * fmt, va_list vl ) {
  int r = _vsnprintf( buf, size, fmt, vl );
  if( r < 0 ) {
    buf[size-1] = 0;
    r = size-1;
  }
  return r;
 }
  inline int snprintf( char * buf, int size, char const * fmt, ... ) {
    va_list vl;
    va_start( vl, fmt );
    int r = vsnprintf( buf, size, fmt, vl );
    va_end( vl );
    return r;
  }
#endif

#if NEED_HSTRERROR
 // NOT thread safe!
 inline char const * hstrerror( ulong ec ) {
   static char err[ 128 ];
   snprintf( err, 128, "host error 0x%lx", ec );
   return err;
 }
#endif

#if NEED_GETLASTERROR
 #define SOCKET_ERRNO WSAGetLastError()
 inline bool SOCKET_WOULDBLOCK_ERROR( int e ) { return e == WSAEWOULDBLOCK; }
 inline bool SOCKET_NEED_REOPEN( int e ) { return e == WSAECONNRESET; }
#endif

#if NEED_ERRNO
 #include <errno.h>
 #define SOCKET_ERRNO errno
 inline bool SOCKET_WOULDBLOCK_ERROR( int e ) { return e == EWOULDBLOCK; }
 inline bool SOCKET_NEED_REOPEN( int e ) { return false; }
#endif

#endif  //  sock_port_h

