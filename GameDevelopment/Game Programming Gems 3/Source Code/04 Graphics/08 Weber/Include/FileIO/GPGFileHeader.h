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
#ifndef GPGFILEHEADER_DOT_H
#define GPGFILEHEADER_DOT_H

#include "Base/GPGDataTypes.h"
#define STAMPSIZE 4

typedef struct GPGFileHeader_tag
{
	U32 Cookie;         // 4 byte field to identify file type
	U32 DataSize;       // length of data following this point in bytes

#if FALSE
	/*** Illegal: Stroustrup C++ 2nd Ed. pg551
		"Member functions of an enclosing class have no special access
		to members of a nested class; they obey usual access rules."
		So, addressing the nested struct's members as though they were 
		enclosing struct's members would be illegal. */
	union {
		U32 Version;    // File format version (major.minor)
		struct
		{
			U16 MinorVersion;
			U16 MajorVersion;
		};
	};
#else
	union {
		U32 Version;    // File format version (major.minor)
		struct
		{
			U16 MinorVersion;
			U16 MajorVersion;
		} VersionData;
	};
#endif

	U32 Stamp[STAMPSIZE];       // A unique stamp used to associate
	                    // separate files.
} GPGFileHeader;

#endif
