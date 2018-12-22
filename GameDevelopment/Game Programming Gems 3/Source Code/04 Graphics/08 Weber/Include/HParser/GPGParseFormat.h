
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

#ifndef __GPGParseFormat_h__
#define __GPGParseFormat_h__

#include "Base/GPGResult.h"

/******************************************************************************
class GPGParseFormat

	if GetDelimitations()==0, complete parses read up to delimiter
	if GetDelimitations()>0, complete parses read through that number
		of whitespace-delimited segments

******************************************************************************/
class GPGParseFormat
	{
	public:
virtual	bool	IsDelimiter(char c)=0;
virtual	long	GetDelimitations(void)							{ return 0; };
virtual	void	ConsumeDelimitation(void)									{};

virtual	void	Reset(void)													{};


virtual	GPGParseType	InputElement(GPGParser *parser,char *type,char *name,
													long &length,long &bits)=0;

virtual	GPGRESULT InputFormatHeader(GPGParser *parser) {return GPG_OK;}
virtual	void	OutputFormatHeader(GPGParser *parser)						{};

virtual	void	OutputHeader(GPGParser *parser,long indention,
					const char *type,const char *name=NULL,long length=1)=0;
virtual	void	OutputFooter(GPGParser *parser,long indention,
					const char *type,const char *name=NULL,long length=1)=0;

virtual	void	OutputCommentStart(GPGParser *parser)=0;
virtual	void	OutputCommentEnd(GPGParser *parser)=0;

virtual	void	Indent(GPGParser *parser,long indention);

virtual	void	ReportInteger(bool indata,const char *name,long value)		{};
virtual	void	SkipBytes(GPGParser *parser,long bytes)						{};


virtual	void	ReportLayout(GPGParseEntry &entry)							{};
virtual	void	PushLayout(void)											{};
virtual	void	PopLayout(void)												{};
virtual	void	ResolveLayout(GPGParser *parser)							{};
	};



#endif /* __GPGParseFormat_h__ */
