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
//
//	GPGDebug.H
//
//	DESCRIPTION
//		This module defines some useful debug macros for performing asserts
//	and outputting debug messages according to a user selected debug level.
//
//		Usage of the GPGASSERT and GPGLOG  macros are guaranteed to compile 
//	out of the release build.
//
//		Usage of all macros should terminate with a semicolon.  The semicolon
//	doesn't compile out of the release build so it is possible to use these
//	macros as a single statement in an if/else block that contains no
//	parenthesis.
//
//	EXAMPLE USAGE
//
//		void ExampleFunction( void )
//		{
//			GPGASSERT( ThisFunctionShouldReturnTrueToSucceedTheAssert() );
//		
//			GPGLOG( "Help, I've falled %d times and can't get up!\n", 10);
//		}
//


#ifndef GPGDebug_H
#define GPGDebug_H

#ifdef WIN32
#include <CRTDbg.h>
#endif

// 21nov00 grandma
//  I've pasted the contents of dbgnew.h here,
//  as xtras/include isn't visible here.
#ifndef _DBGNEW_H
#define _DBGNEW_H
#if defined( DEBUG_ALLOC )

#include <new.h>

void *operator new(size_t, char*, int);
void *operator new[](size_t, char*, int);

#define new _dbg_NEW
#define _dbg_NEW new(__FILE__, __LINE__)

#endif // DEBUG_ALLOC
#endif // _DBGNEW_H


#ifdef __cplusplus
extern "C"
{
#endif

typedef int ( GPGDebugOutputFunction )( const char* pFormatString, ... );

#ifdef __cplusplus
}
#endif

extern GPGDebugOutputFunction   *gs_pDebugOutputFunction;


//
// Debug version of GPGDebug.H.
//
#ifdef _DEBUG

//---------------------------------------------------------------------------
//	GPGASSERT
//
//	This macro behaves like the standard assert.  It was added so that ours
//	has a unique name that doesn't clash with other versions of assert.
//---------------------------------------------------------------------------

// DO NOT CALL THIS PROCEDURE; USE GPGLOG INSTEAD.
// These should only be used by the macros defined in this header file.
#ifndef __linux__
int GPGDebugTrace(const char* pFormatString, ...);
#endif

#ifdef WIN32

#define GPGASSERT(condition) _ASSERT(condition)

//---------------------------------------------------------------------------
//	GPGLOG
//
//	This macro is used to display to the output debug window.
//	The parameters to GPGLOG are exactly the same as the standard 
//	printf function (i.e. a format string followed by a variable
//	arguement list).
//
//	Note:  Make sure the final message doesn't get more than 512 characters.
//---------------------------------------------------------------------------

// DO NOT CALL THIS PROCEDURE; USE GPGTRACE INSTEAD.
// These should only be used by the macros defined in this header file.
//int GPGDebugTrace(const char* pFormatString, ...);

#ifndef GPG_CONSOLE_LOG
#define GPGLOG ::GPGDebugTrace
#else
#define GPGLOG ::gs_pDebugOutputFunction
#endif

#elif defined(MACINTOSH)

#define GPGLOG	::GPGDebugTrace

#define GPGASSERT(condition) \
	do { if (!(condition)) GPGDebugTrace((const char *)"\pAsserted at line %d, in file %s.\n",__LINE__, __FILE__); }	while(0)	

#else // Unix

#include <assert.h>
#define GPGASSERT(condition) assert(condition)

#define GPGLOG ::gs_pDebugOutputFunction

#endif


//
// Release version of GPGDebug.H.
//
#else  //_DEBUG


// inline void GPGDebugTrace(const char* pFormatString, ...) { }
int GPGDebugTrace(const char* pFormatString, ...);


#define GPGASSERT( condition )

#define GPGLOG 1 ? 0 : ::GPGDebugTrace


#endif // Release version of Debug.H.


#endif //GPGDebug_H
