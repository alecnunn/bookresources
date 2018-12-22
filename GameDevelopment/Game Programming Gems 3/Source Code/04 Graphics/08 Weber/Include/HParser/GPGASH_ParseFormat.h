
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

#ifndef __GPGASH_ParseFormat_h__
#define __GPGASH_ParseFormat_h__



/******************************************************************************
class GPGASH_ParseFormat: public GPGParseFormat

	begins with GPG 24-byte header and ASH identifier:

		TYP\0xxyyzzzzzzzzzzzzzzzz
		//ASH 1.0

		struct ...

	arrays of variable size must be IMMEDIATELY preceeded by that variable:

		struct stuff
			{
			int entries;
			float[entries] data;
			}

******************************************************************************/
class GPGASH_ParseFormat: public GPGParseFormat
	{
	public:
				GPGASH_ParseFormat(void)
					{
					Reset();
					ResetGPGData();
					};

virtual			~GPGASH_ParseFormat(void)	{};

virtual	void	Reset(void)
					{
					m_level=0;
					m_stage=0;
					m_delimitations=1;
					m_structures.Reset();
					m_context.Clear();
					m_layoutlevel=0;
					};

		void	ResetGPGData(void)
					{
					strncpy(m_ifxcookie,"ASH",4);
					m_ifxmajor=1;
					m_ifxminor=0;

					// theoretically a unique id, clear to 0
					long m;
					for(m=0;m<4;m++)
						m_ifxstamp[m]=0;

					m_FileHeaderUtil.Reset();
					};

		void	SetGPGData(char *cookie,long major=1,long minor=0)
					{
					memset(m_ifxcookie,0,4);
					strncpy(m_ifxcookie,cookie,4);
					m_ifxmajor=(unsigned short)major;
					m_ifxminor=(unsigned short)minor;

					m_FileHeaderUtil.Reset();
					m_FileHeaderUtil.Set(*(U32*)cookie,m_ifxmajor,m_ifxminor);
					};

		GPGFileHeaderUtil *GetFileHeader()
					{
					return &m_FileHeaderUtil;
					}

//virtual bool	IsDelimiter(char c)		{ return GPGParser::IsWhiteSpace(c); };
virtual bool	IsDelimiter(char c)		{ return false; };

virtual long	GetDelimitations(void)		{ return m_delimitations; };
virtual void	ConsumeDelimitation(void)	{ m_delimitations--; };
		void	SetDelimitations(long set)	{ m_delimitations=set; };

virtual	GPGParseType	InputElement(GPGParser *parser,char *type,char *name,
													long &length,long &bits);

virtual	void	Indent(GPGParser *parser,long indention)	{};

virtual GPGRESULT InputFormatHeader(GPGParser *parser);
virtual void	OutputFormatHeader(GPGParser *parser);

virtual	void	OutputHeader(GPGParser *parser,long indention,
						const char *type,const char *name=NULL,long length=1)
					{
					if(indention<1)
						{
						if(parser->GetBinaryWrite())
							parser->OutputRawText("~");
						parser->OutputQuoteableText(type);
						if(!parser->GetBinaryWrite())
							parser->OutputRawText(" ");

						if(name && name[0])
							parser->OutputQuoteableText(name);
						else if(parser->GetBinaryWrite())
							{
							char nullbyte=0;
							parser->OutputRawBytes(&nullbyte,1);
							}
						else
							parser->OutputRawText("\"\"");
						}
					};
virtual	void	OutputFooter(GPGParser *parser,long indention,
					const char *type,const char *name=NULL,long length=1)
					{
					if(indention<1)
						parser->OutputRawText("\n");
					};

virtual	void	OutputCommentStart(GPGParser *parser)
					{
					parser->OutputRawText("# ");
					};
virtual	void	OutputCommentEnd(GPGParser *parser)
					{
					parser->OutputRawText("\n");
					};

virtual void	ReportInteger(bool indata,const char *name,long value)
					{
					GPGASSERT((m_level-indata)>=0);
					m_lastintmap[m_level-indata].SetLastInt(name,value);
					};

virtual	void	SkipBytes(GPGParser *parser,long bytes);
		void	EndBlock(GPGParser *parser);

		void	ReadWord(GPGParser *parser,char *word,bool binary);
		bool	ReadStructLine(GPGParser *parser,char *type,
											char *name,long *size,long *bits);
		void	ReadStructure(GPGParser *parser);

		void	ExpandArrays(long depth)
					{
					m_context.ResizeToAtLeast(depth);
					m_lastintmap.ResizeToAtLeast(depth+2);
					};

		long	ExtractRepetitions(char *string);

virtual void	ReportLayout(GPGParseEntry &entry);
virtual void	PushLayout(void);
virtual void	PopLayout(void);
virtual void	ResolveLayout(GPGParser *parser);

	private:

		GPGASH_StructureArray			m_structures;
		GPGArray<GPGASH_Context>		m_context;
		GPGArray<GPGStringIntMap>		m_lastintmap;

		// every class derived from this one must have a file header
		GPGFileHeaderUtil				m_FileHeaderUtil;

		// input verification data
		char							m_ifxcookie[4];
		unsigned short					m_ifxmajor;
		unsigned short					m_ifxminor;
		unsigned long					m_ifxstamp[4];

		long m_level;			// 0=base
		long m_stage;			// 0=header 1=data 2=footer
		long m_delimitations;	// number of space-delimited values to be read

		long							m_layoutlevel;
		GPGArray<long>					m_layoutindex;
		GPGArray<GPGStringStringMap>	m_layoutmap;
		GPGString						m_layouttype;
	};



#endif /* __GPGASH_ParseFormat_h__ */
