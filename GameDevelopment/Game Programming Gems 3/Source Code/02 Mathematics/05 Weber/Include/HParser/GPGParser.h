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

#ifndef __GPGParser_h__
#define __GPGParser_h__


#if _DEBUG
#include <errno.h>
#include <string.h>
#endif


#define GPGPARSE_ASSERT_BYTES	FALSE	// really slow
#define GPGPARSE_FUNBUFFER_SIZE	1024	// for InputFundamental()



enum GPGParserMode	{ INPUT, OUTPUT };

const char GPGParserTypeName[][16]={ "bool", "U8 (HEX)", "I16", "U16",
							"I32", "U32", "U32 (SKIP)", "F32", "F64" };


/******************************************************************************

	char buffers are generally expected to be of m_length GPGPARSE_NAMELENGTH
	or greater, specifically type and name strings

******************************************************************************/



/******************************************************************************
class GPGParser

	16-bit float is a float from -1 to 1 stored as an int

******************************************************************************/
class GPGParser
	{
	public:

//		enum	FundamentalType	{ BOOLEAN, LONG, FLOAT, HEX, SKIP };
		enum	FundamentalType	{ eBOOL, eHEX, eI16, eU16, eI32, eU32,
									eSKIP, eF32, eF64 };

				GPGParser(void)	{ Reset(); };

		void	Reset(void)
					{
#if GPGPARSE_USE_STREAMS
					m_in=NULL;
					m_out=NULL;
					m_streammode=true;
#else
					m_file=NULL;
					m_streammode=false;
#endif
					m_indention=0;
					m_binaryread=false;
					m_binarywrite=false;
					m_layoutmode=false;
					m_abort=GPG_OK;
					};

		GPGParseFormat	*GetParseFormat(void)			{ return m_format; };
		void			SetParseFormat(GPGParseFormat *set) { m_format=set; };

		void	SetBinaryRead(bool set)		{ m_binaryread=set; };
		bool	GetBinaryRead(void) const	{ return m_binaryread; };

		void	SetBinaryWrite(bool set)	{ m_binarywrite=set; };
		bool	GetBinaryWrite(void) const	{ return m_binarywrite; };

		void	SetLayoutMode(bool set)		{ m_layoutmode=set; };
		bool	GetLayoutMode(void) const	{ return m_layoutmode; };

		FILE	*GetFilePointer(void)
					{
#if GPGPARSE_USE_STREAMS
					return NULL;
#else
					return m_file;
#endif
					};

#if GPGPARSE_USE_STREAMS
		void	SetInputStream(std::istream *set)
					{
					if(m_format)
						m_format->Reset();
					m_abort=GPG_OK;
					SetParserMode(INPUT);
					m_in=set;
					};
		void	SetOutputStream(std::ostream *set)
					{
					if(m_format)
						m_format->Reset();
					m_abort=GPG_OK;
					SetParserMode(OUTPUT);
					m_out=set;
					};
#endif

		virtual GPGRESULT OpenInputHeader(const char *filename)
					{
						// GPGFileHeaderUtil doesn't know about streams
						m_file=fopen(filename,"rb");
						if (!m_file)
							return GPG_E_INVALID_FILE;
						GPGRESULT result = m_format->InputFormatHeader(this);
						fclose(m_file);
						return result;
					}


		bool	OpenInputFile(const char *filename)
					{
					bool result=false;
					if(m_format)
						m_format->Reset();
					m_abort=GPG_OK;
					SetParserMode(INPUT);

#if GPGPARSE_USE_STREAMS
					m_local_in=new std::ifstream;
					m_in=m_local_in;
					m_in->open(filename);
					result=m_in->is_open();
#else
					// the b is required under Win32 to force binary
					// it has no effect under proper OS's
					m_file=fopen(filename,"rb");
//					GPGASSERT(m_file);
					result=(m_file!=NULL);
					if(result)
						m_format->InputFormatHeader(this);
#endif
					return result;
					};

		bool	CloseInputFile(void)
					{
					if(m_format)
						m_format->Reset();
#if GPGPARSE_USE_STREAMS
					m_in->close();
					m_in=NULL;
					delete m_local_in;
					return true;
#else
					int result=fclose(m_file);
					m_file=NULL;
					GPGASSERT(!result);
					return (!result);
#endif
					};

		bool	OpenOutputFile(const char *filename)
					{
					if(m_format)
						m_format->Reset();
					m_abort=GPG_OK;
					bool result=false;
					SetParserMode(OUTPUT);
#if GPGPARSE_USE_STREAMS
					m_local_out=new std::ofstream;
					m_out=m_local_out;
					m_out->open(filename);
					result=m_out->is_open();
#else
					m_file=fopen(filename,"wb");
//					GPGASSERT(m_file);
					result=(m_file!=NULL);
					if(result)
						m_format->OutputFormatHeader(this);
#endif
					return result;
					};

		bool	CloseOutputFile(void)
					{
					if(m_format)
						m_format->Reset();
#if GPGPARSE_USE_STREAMS
					m_out->close();
					m_out=NULL;
					delete m_local_out;
					return true;
#else
					int result=fclose(m_file);
					m_file=NULL;
					GPGASSERT(!result);
					return (!result);
#endif
					};

/*****************************************************************************/

#if GPGPARSE_USE_STREAMS
		long	GetPosition(void)
					{
					GPGLOG("GPGParser::GetPosition() denied in stream mode");
					GPGASSERT(FALSE);
					return 0;
					};
		bool	SetPosition(long set)
					{
					GPGLOG("GPGParser::SetPosition() denied in stream mode");
					GPGASSERT(FALSE);
					return false;
					};

				// NULL permitted
		void	OutputRawBytes(const char *buffer,long length)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_out);
#endif
					if(m_layoutmode)
						return;
					(*m_out).write(buffer,length);
					};

				// NULL termm_inated
		void	OutputRawText(const char *buffer)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_out);
#endif
					if(m_layoutmode)
						return;
					*m_out<<buffer;
					};

				// don't try to put back more than last one read
		bool	PutBackRawByte(char byte)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_in);
#endif
					(*m_in).putback(byte);

					// TODO can putback return success?
					return true;
					};

				// whitespace/NULL m_included
		long	InputRawBytes(char *buffer,long length)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_in);
#endif
					(*m_in).read(buffer,length);

					// TODO return actual number read
					return length;
					};

				// whitespace/NULL m_included
		char	InputRawByte(void)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_in);
#endif
					return (*m_in).get();
					};

				// whitespace ignored
		char	InputNextChar(void)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_in);
#endif
					char m_byte;
					*m_in>>m_byte;
					return m_byte;
					};

		bool	IsAtEndOfFile(void)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_in);
#endif
					return (*m_in).eof();
					};
#else
				//* non-stream

		long	GetPosition(void)
					{
					return ftell(m_file);
					};
		bool	SetPosition(long set)
					{
					if(set<0)
						return (fseek(m_file,0,SEEK_END)==0);
					else
						return (fseek(m_file,set,SEEK_SET)==0);
					};

				// NULL permitted
		void	OutputRawBytes(const char *buffer,long len)
					{
					if(m_layoutmode)
						return;
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_file);
					GPGASSERT(m_parsermode==OUTPUT);
#endif

					m_bytes=fwrite(buffer,1,len,m_file);
					if(m_bytes!=len)
						Abort(GPG_E_WRITE_FAILED);
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_bytes==len);
#endif
					};

				// NULL termm_inated
		void	OutputRawText(const char *buffer)
					{
					if(m_layoutmode)
						return;
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_file);
					GPGASSERT(m_parsermode==OUTPUT);
#endif

					m_length=strlen(buffer);
					m_bytes=fwrite(buffer,1,m_length,m_file);
					if(m_bytes!=m_length)
						Abort(GPG_E_WRITE_FAILED);
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_bytes==m_length);
#endif
					};

				// don't try to put back more than last one read
		bool	PutBackRawByte(char cbyte)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_file);
					GPGASSERT(m_parsermode==INPUT);
#endif

					m_byte=ungetc(cbyte,m_file);
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_byte!=EOF);
#endif
					return (m_byte!=EOF);
					};

				// whitespace/NULL m_included
		long	InputRawBytes(char *buffer,long length)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_file);
					GPGASSERT(m_parsermode==INPUT);
#endif

#if _DEBUG
errno=0;
#endif
					m_bytes=fread(buffer,1,length,m_file);
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_bytes==length);
#endif

#if _DEBUG
if(m_bytes!=length)
	GPGLOG("length=%d m_bytes=%d eof=%d %s",length,m_bytes,(long)IsAtEndOfFile(),strerror(errno));
#endif
					return m_bytes;
					};

				// whitespace/NULL included
		char	InputRawByte(void)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_file);
					GPGASSERT(m_parsermode==INPUT);
#endif

					if((m_byte=fgetc(m_file)) != EOF)
						return m_byte;
					return 0;
					};

				// whitespace ignored
		char	InputNextChar(void)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_file);
					GPGASSERT(m_parsermode==INPUT);
#endif

					while((m_byte=fgetc(m_file))!=EOF && IsWhiteSpace(m_byte));

					return (m_byte==EOF)? 0: m_byte;
					};

		bool	IsAtEndOfFile(void)
					{
#if GPGPARSE_ASSERT_BYTES
					GPGASSERT(m_file);
					GPGASSERT(m_parsermode==INPUT);
#endif

					return (feof(m_file)!=0);
					};
#endif

/*****************************************************************************/

		bool			GetStreamMode(void)			{ return m_streammode; };

		GPGParserMode	GetParserMode(void)			{ return m_parsermode; };
		void			SetParserMode(GPGParserMode set)
													{ m_parsermode=set; };

		GPGRESULT		TransferBlock(GPGParseTable &parsetable);

		GPGRESULT		GetAbortCode(void)			{ return m_abort; };
		void			Abort(GPGRESULT result=GPG_E_ABORTED)
													{ m_abort=result; };

		long	InputDataBlock(char *buffer,long size,bool complete=TRUE);

				// skip past null-terminated data in input (presumably string)
		long	SkipNullTerminated(void)
					{
//					GPGASSERT(m_binaryread);
					long m=0;
					char m_byte;
					while( (m_byte=InputRawByte()) )
						m++;
					return m;
					}

				// skip past data in input of known number of bytes
		bool	SkipBytes(long bytes)
					{
#if GPGPARSE_USE_STREAMS
//					GPGASSERT(m_binaryread);
					char *buffer=new char[bytes];
					long result=InputRawBytes(buffer,bytes);
					delete[] buffer;
					return (result==bytes);
#else
//					GPGASSERT(m_bytes>=0);
					return (fseek(m_file,bytes,SEEK_CUR)==0);
#endif
					}

static	void	TrimString(char *inbuffer,char *outbuffer,long size);
static	bool	IsWhiteSpace(char c)
					{ return (c==' ' || c=='\t' || c=='\n' || c=='\r'); };

				// WhenOutput*() is for m_formattm_ing ONLY
		void	WhenOutput_Indent(long indention)
					{
					if(GetParserMode()==OUTPUT && !GetBinaryWrite())
						m_format->Indent(this,indention);
					};
		void	WhenOutput_Append(char *string)
					{
					if(GetParserMode()==OUTPUT && !GetBinaryWrite())
						OutputRawText(string);
					};

		void	OutputQuoteableText(const char *buffer);

		GPGParseType	InputElement(char *type,char *name,
									long &m_length,long &bits)
							{
							return m_format->InputElement(this,type,name,
														m_length,bits);
							};

		void	OutputCommentHex(long value,long indention);

//*				GPGParseFormat		(save these for GPGThinParseFormat)
/*
virtual	GPGParseType	InputElement(char *type,char *name,long &length)
													{ return BAD; };

virtual	void	OutputHeader(const char *type,const char *name=NULL,
															long length=1)
					{
					GPGASSERT(m_out);
					if(name)
						*m_out<<'\"'<<name<<"\" ";
					*m_out<<type;
					if(m_length!=1)
						*m_out<<'['<<std::dec<<m_length<<"]";
					*m_out<<" {";
					};
virtual	void	OutputFooter(const char *type,const char *name=NULL,
															long length=1)
					{
					GPGASSERT(m_out);
					*m_out<<'}';
					};

virtual	void	OutputCommentStart(void)
					{
					GPGASSERT(m_out);
					*m_out<<'(';
					};
virtual	void	OutputCommentEnd(void)
					{
					GPGASSERT(m_out);
					*m_out<<')';
					};
*/

		void	InputStringArg(char *buffer,long size,bool complete);

				// for use on bool, longs, and floats
		void	InputFundamental(GPGParseEntry &entry,FundamentalType ftype);
		void	OutputFundamental(GPGParseEntry &entry,FundamentalType ftype);
		void	TransferFundamental(GPGParseEntry &entry,FundamentalType ftype)
					{
					if(m_parsermode==INPUT)
						InputFundamental(entry,ftype);
					else
						OutputFundamental(entry,ftype);
					};

		void	TransferBools(GPGParseEntry &entry)
										{ TransferFundamental(entry,eBOOL); };
		void	TransferHex(GPGParseEntry &entry)
										{ TransferFundamental(entry,eHEX); };
		void	TransferI32s(GPGParseEntry &entry)
										{ TransferFundamental(entry,eI32); };
		void	TransferU32s(GPGParseEntry &entry)
										{ TransferFundamental(entry,eU32); };
		void	TransferF32s(GPGParseEntry &entry)
										{ TransferFundamental(entry,eF32); };
		void	TransferF64s(GPGParseEntry &entry)
										{ TransferFundamental(entry,eF64); };
		void	TransferSkip(GPGParseEntry &entry)
										{ TransferFundamental(entry,eSKIP); };


				// string are a little different and handled separately
		void	InputString(GPGParseEntry &entry);
		void	OutputString(GPGParseEntry &entry);
		void	TransferString(GPGParseEntry &entry)
					{
					if(m_parsermode==INPUT)
						InputString(entry);
					else
						OutputString(entry);
					};

				// derived from float transfer
		void	TransferVector3(GPGParseEntry &entry);
		void	TransferQuaternion(GPGParseEntry &entry);


	protected:

		GPGParseFormat			*m_format;

		GPGParserMode			m_parsermode;	// INPUT or OUTPUT
		long					m_indention;
		bool					m_binaryread;
		bool					m_binarywrite;
		bool					m_layoutmode;
		GPGRESULT				m_abort;

#if GPGPARSE_USE_STREAMS
		std::istream			*m_in;
		std::ostream			*m_out;
		std::ifstream			*m_local_in;
		std::ofstream			*m_local_out;
#else
		FILE					*m_file;
#endif

		char					m_funbuffer[GPGPARSE_FUNBUFFER_SIZE];

	private:
		bool					m_streammode;

								// reusable variables, like statics
		int						m_byte;
		long					m_bytes,m_length;
	};


#endif /* __GPGParser_h__ */
