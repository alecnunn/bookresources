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
//	GPGDataTypes
//
//	DESCRIPTION
//    Defines basic data types and precompiler stuff used by the Toolkit.
//


#ifndef GPGDataTypes_h
#define GPGDataTypes_h

#ifdef MACINTOSH
#pragma options align = power
#endif // MACINTOSH


#ifdef MACINTOSH
typedef unsigned long	U32;
#else
typedef unsigned int	U32;
#endif
typedef unsigned short  U16;
typedef unsigned char   U8;

#ifdef MACINTOSH
typedef long			I32;
#else
typedef int				I32;
#endif
typedef short			I16;
typedef signed char		I8;

typedef float			F32;
typedef double			F64;

typedef int				BOOL;		// Typedef matches Windows.H.
typedef char			CHAR;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef NULL
#define NULL 0
#endif

// Macros
#define GPGDELETE(p)		if ((p)) {delete (p); (p) = NULL;}
#define GPGDELETE_ARRAY(p)	if ((p)) {delete [] (p); (p) = NULL;}
#define GPGRELEASE(p)		if ((p)) {(p)->Release(); (p) = NULL;}

#define GPGINLINE inline
#ifdef MACINTOSH
#define GPGFORCEINLINE inline
#else
#define GPGFORCEINLINE __forceinline
#endif

// Define GPG_U16_INDEX on compiler cmd line to 
// change INDEX to a 16-bit index (useful for d3d)
#ifdef GPG_U16_INDEX
typedef U16		GPGGeomIndex;	
#define 		GPG_MAX_GEOMINDEX		USHRT_MAX
#else
typedef U32		GPGGeomIndex;	
#define 		GPG_MAX_GEOMINDEX		UINT_MAX
#endif

#endif
