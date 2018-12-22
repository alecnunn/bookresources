
/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/

#ifndef __GPGPlatformDefine_h__
#define __GPGPlatformDefine_h__

typedef U32 UWORD;
typedef I32 IWORD;

/* GPG_OSTYPE */
#define GPG_WIN32			1
#define GPG_UNIX			2
#define GPG_MAC				3
#define GPG_PALM			4

/* GPG_OS */
#define GPG_WINNT			1
#define GPG_WIN95			2
#define GPG_WIN98			3
#define GPG_WIN2K			4
#define GPG_LINUX			6
#define GPG_FREEBSD			7
#define GPG_MACOS			8
#define GPG_PALMOS			9

/* GPG_OSVER */
/* use the primary version number */

/* GPG_COMPILER */
#define GPG_MICROSOFT		1
#define GPG_ANSI			2
#define GPG_GNU				3
#define GPG_KR				4
#define GPG_INTEL			5

/* GPG_HW */
#define GPG_X86				1
#define GPG_PPC				2
#define GPG_68K				3
#define GPG_AXP				4
#define GPG_MIPS			5
#define GPG_SPARC			6
#define GPG_HPPA			7

/* GPG_ENDIAN */
#define GPG_BIG				1
#define GPG_LITTLE			2

/* GPG_WL */
#define GPG_WL_WIN32		1
#define GPG_WL_XWIN			2

/* GPG_2DGL */
#define GPG_2D_OPENGL		1
#define GPG_2D_DIRECTX		2
#define GPG_2D_GDI			3
#define GPG_2D_X_GFX		4
#define GPG_2D_MAC			5
#define GPG_2D_PALMOS		6

/* GPG_3DGL */
#define GPG_3D_OPENGL		1
#define GPG_3D_DIRECT3D		2

/************************************************************/
#ifndef GPG_PLATFORM_OVERRIDE
/* HARDCODED SETTINGS FOR NOW */
/* TODO: de-hardcode when we start doing multiple platforms */
#define GPG_OSTYPE		GPG_WIN32
#define GPG_OS			GPG_WINNT
#define GPG_OSVER		4
#ifdef __ICL
#define GPG_COMPILER	GPG_INTEL
#else
#define GPG_COMPILER	GPG_MICROSOFT
#endif
#define GPG_HW			GPG_X86
#define GPG_2DGL		GPG_2D_OPENGL
#define GPG_3DGL		GPG_3D_OPENGL
#endif
/************************************************************/

#if GPG_HW==GPG_X86 || GPG_HW==GPG_AXP
#define GPG_ENDIAN		GPG_LITTLE
#else
#define GPG_ENDIAN		GPG_BIG
#endif


/* SYSTEM DEPENDENT INCLUDES ETC */

/* GENERAL WINDOWS */
#if GPG_OSTYPE==GPG_WIN32
#include <windows.h>	//* causes wacky struct errors (???)
#include <winsock2.h>

#define GPG_ERRNO	GetLastError()
#define GPG_BEGIN_SIGNAL_REDO_LOOP();		do{
#define GPG_END_SIGNAL_REDO_LOOP(if_error)	} while ((if_error) && (GPG_ERRNO == WSAEINTR))
#define GPG_UNDEFINED_ERROR		0xFFF0
#define	GPG_EWOULDBLOCK			WSAEWOULDBLOCK
#define	GPG_EBADF				WSAEBADF
#define	GPG_ENOTSOCK			WSAENOTSOCK
#define	GPG_EINTR				WSAEINTR
#define	GPG_ENODEV				GPG_UNDEFINED_ERROR+0
#define	GPG_EOPNOTSUPP			WSAEOPNOTSUPP
#define	GPG_EPROTO				GPG_UNDEFINED_ERROR+1
#define	GPG_ENOMEM				GPG_UNDEFINED_ERROR+2
#define	GPG_ENOSR				GPG_UNDEFINED_ERROR+3
#define	GPG_EACCES				WSAEACCES
#define	GPG_EADDRINUSE			WSAEADDRINUSE
#define	GPG_EADDRNOTAVAIL		WSAEADDRNOTAVAIL
#define	GPG_EAFNOSUPPORT		WSAEAFNOSUPPORT
#define	GPG_ENOENT				GPG_UNDEFINED_ERROR+4
#define	GPG_ECONNREFUSED		WSAECONNREFUSED
#define	GPG_EINPROGRESS			WSAEINPROGRESS
#define	GPG_EINVAL				WSAEINVAL
#define	GPG_EIO					GPG_UNDEFINED_ERROR+5
#define	GPG_ENXIO				GPG_UNDEFINED_ERROR+6
#define	GPG_ECONNRESET			WSAECONNRESET
#define	GPG_EISCONN				WSAEISCONN
#define	GPG_ENETUNREACH			WSAENETUNREACH
#define	GPG_ETIMEDOUT			WSAETIMEDOUT
#define	GPG_EMFILE				WSAEMFILE
#define	GPG_EPROTONOSUPPORT		WSAEPROTONOSUPPORT
#define	GPG_ESTALE				WSAESTALE
#define	GPG_EMSGSIZE			WSAEMSGSIZE
#define GPG_EPIPE				GPG_UNDEFINED_ERROR+7
#endif

/* GENERAL UNIX */
#if GPG_OSTYPE==GPG_UNIX
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#define GPG_ERRNO	errno
#define GPG_BEGIN_SIGNAL_REDO_LOOP();		do{
#define GPG_END_SIGNAL_REDO_LOOP(if_error)	} while ((if_error) && (GPG_ERRNO == EINTR))
#define	GPG_EWOULDBLOCK			EWOULDBLOCK
#define	GPG_EBADF				EBADF
#define	GPG_ENOTSOCK			ENOTSOCK
#define	GPG_EINTR				EINTR
#define	GPG_ENODEV				ENODEV
#define	GPG_EOPNOTSUPP			EOPNOTSUPP
#define	GPG_EPROTO				EPROTO
#define	GPG_ENOMEM				ENOMEM
#define	GPG_ENOSR				ENOSR
#define	GPG_EACCES				EACCES
#define	GPG_EADDRINUSE			EADDRINUSE
#define	GPG_EADDRNOTAVAIL		EADDRNOTAVAIL
#define	GPG_EAFNOSUPPORT		EAFNOSUPPORT
#define	GPG_ENOENT				ENOENT
#define	GPG_ECONNREFUSED		ECONNREFUSED
#define	GPG_EINPROGRESS			EINPROGRESS
#define	GPG_EINVAL				EINVAL
#define	GPG_EIO					EIO
#define	GPG_ENXIO				ENXIO
#define	GPG_ECONNRESET			ECONNRESET
#define	GPG_EISCONN				EISCONN
#define	GPG_ENETUNREACH			ENETUNREACH
#define	GPG_ETIMEDOUT			ETIMEDOUT
#define	GPG_EMFILE				EMFILE
#define	GPG_EPROTONOSUPPORT		EPROTONOSUPPORT
#define	GPG_ESTALE				ESTALE
#define	GPG_EMSGSIZE			EMSGSIZE
#define GPG_EPIPE				EPIPE
#endif

/* PALMOS */
#if GPG_OS==GPG_PALMOS
#include <System/SysAll.h>
#include <UI/UIAll.h>
#endif

/* SYSTEM DEPENDENT FUNCTION NAMES */
#if GPG_OSTYPE==GPG_WIN32
#define GPG_VSNPRINTF _vsnprintf
#define GPG_SNPRINTF _snprintf
#endif

#if GPG_OSTYPE==GPG_UNIX
#define GPG_VSNPRINTF vsnprintf
#define GPG_SNPRINTF snprintf
#endif

#if GPG_OS==GPG_LINUX
#define GPGSignalHandler(x)		void x (...)

#elif GPG_OS==GPG_FREEBSD
#define GPGSignalHandler(x)		void x (int)

#else
#define GPGSignalHandler(x)		void x (...)

#endif


#if GPG_OSTYPE==GPG_UNIX
#define GPGSetSignalHandler(f,a) {											\
								struct sigaction t;							\
								t.sa_handler = f;							\
								sigfillset(&(t.sa_mask));					\
								t.sa_flags = 0;								\
								if(sigaction(a,&t,NULL)!=0)					\
								GPGLOG("could not set signal handler\n");	\
								}

#else
#define GPGSetSignalHandler(f,a) /*NOOP*/
#endif

#endif

