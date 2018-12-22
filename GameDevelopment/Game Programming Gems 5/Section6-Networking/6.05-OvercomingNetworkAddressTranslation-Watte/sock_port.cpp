
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

#include "sock_port.h"

#if NEED_GETTIMEOFDAY
 #if defined( WIN32 )

#include <sys/types.h>
#include <sys/timeb.h>
#include <math.h>
#include <assert.h>

#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "ws2_32.lib" )

// This class could be made cheaper using RDTSC for short-term 
// measurement. But, whatever.
class init_gettimeofday
{
  public:
    init_gettimeofday()
    {
      timeBeginPeriod( 2 );
      __int64 rr;
      QueryPerformanceFrequency( (LARGE_INTEGER *)&rr );
      ticksPerSecInv_ = 1.0 / (double)((DWORD)rr & 0xffffffff);
      int watchdog = 0;
    again:
      lastTicks_ = timeGetTime();
      QueryPerformanceCounter( (LARGE_INTEGER *)&lastRead_ );
      timeb tb;
      ftime( &tb );
      timeOffset_ = tb.time + tb.millitm * 0.001 - lastRead_ * ticksPerSecInv_;
      lastTime_ = timeOffset_;
      //  make sure it didn't take too long
      if( watchdog++ < 10 && (timeGetTime() != lastTicks_) ) {
        goto again;
      }
    }
    ~init_gettimeofday()
    {
      timeEndPeriod( 2 );
    }

    void get( timeval * tv )
    {
      __int64 nu;
      int watchdog = 0;
    again:
      DWORD m = timeGetTime();
      QueryPerformanceCounter( (LARGE_INTEGER *)&nu );
      DWORD n = timeGetTime();
      // guard against pre-emption
      if( (watchdog++ < 10) && (n != m) ) {
        goto again;
      }
      double nuTime = nu * ticksPerSecInv_ + timeOffset_;
      if( (nu - lastRead_) & 0x7fffffff80000000ULL ) {
        //  there's a chance that we're seeing a jump-ahead
        double adjust = (nuTime - lastTime_ - (n - lastTicks_) * 0.001);
        if( adjust > 0.1f ) {
          timeOffset_ -= adjust;
          nuTime -= adjust;
          assert( nuTime >= lastTime_ );
        }
      }
      lastRead_ = nu;
      lastTicks_ = n;
      lastTime_ = nuTime;
      tv->tv_sec = (ulong)floor( nuTime );
      tv->tv_usec = (ulong)(1000000 * (nuTime - tv->tv_sec));
    }

    double ticksPerSecInv_;
    double timeOffset_;
    double lastTime_;
    __int64 lastRead_;
    DWORD lastTicks_;
};

void gettimeofday( timeval * tv, int )
{
  static init_gettimeofday data;
  data.get( tv );
}

 #else
  #error "don't know how to do this"
 #endif
#endif

#if TIME_MAIN

int main()
{
  struct timeval ta, tb;
  gettimeofday( &ta, 0 );
  while( 1 ) {
    fprintf( stderr, "0x%08lx:0x%08lx\n", ta.tv_sec, ta.tv_usec );
    do {
      gettimeofday( &tb, 0 );
    }
    while( ta.tv_sec == tb.tv_sec );
    ta = tb;
  }
  return 0;
}

#endif


#if NEED_WINDOWS_POLL
 #if defined( WIN32 )

#include <assert.h>
#include <winsock2.h>
#include <windows.h>

//  This is somewhat less than ideal -- better would be if we could 
//  abstract pollfd enough that it's non-copying on Windows.
int poll( pollfd * iofds, size_t count, int ms )
{
  FD_SET rd, wr, ex;
  FD_ZERO( &rd );
  FD_ZERO( &wr );
  FD_ZERO( &ex );
  SOCKET m = 0;
  for( size_t ix = 0; ix < count; ++ix ) {
    iofds[ix].revents = 0;
    if( iofds[ix].fd >= m ) {
      m = iofds[ix].fd + 1;
    }
    if( iofds[ix].events & (POLLIN | POLLPRI) ) {
      assert( rd.fd_count < FD_SETSIZE );
      rd.fd_array[ rd.fd_count++ ] = iofds[ix].fd;
    }
    if( iofds[ix].events & (POLLOUT) ) {
      assert( wr.fd_count < FD_SETSIZE );
      wr.fd_array[ wr.fd_count++ ] = iofds[ix].fd;
    }
    assert( ex.fd_count < FD_SETSIZE );
    ex.fd_array[ ex.fd_count++ ] = iofds[ix].fd;
  }
  timeval tv;
  tv.tv_sec = ms/1000;
  tv.tv_usec = (ms - (tv.tv_sec * 1000)) * 1000;
  int r = 0;
  if( m == 0 ) {
    ::Sleep( ms );
  }
  else {
    r = ::select( (int)m, (rd.fd_count ? &rd : 0), (wr.fd_count ? &wr : 0), (ex.fd_count ? &ex : 0), &tv );
  }
  if( r < 0 ) {
    int err = WSAGetLastError();
    errno = err;
    return r;
  }
  r = 0;
  for( size_t ix = 0; ix < count; ++ix ) {
    for( size_t iy = 0; iy < rd.fd_count; ++iy ) {
      if( rd.fd_array[ iy ] == iofds[ix].fd ) {
        iofds[ix].revents |= POLLIN;
        ++r;
        break;
      }
    }
    for( size_t iy = 0; iy < wr.fd_count; ++iy ) {
      if( wr.fd_array[ iy ] == iofds[ix].fd ) {
        iofds[ix].revents |= POLLOUT;
        ++r;
        break;
      }
    }
    for( size_t iy = 0; iy < ex.fd_count; ++iy ) {
      if( ex.fd_array[ iy ] == iofds[ix].fd ) {
        iofds[ix].revents |= POLLERR;
        ++r;
        break;
      }
    }
  }
  return r;
}

 #else
  #error "don't know how to do this"
 #endif
#endif
