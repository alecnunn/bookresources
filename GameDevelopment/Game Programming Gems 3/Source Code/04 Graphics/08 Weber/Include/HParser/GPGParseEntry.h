
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

#ifndef __GPGParseEntry_h__
#define __GPGParseEntry_h__



class GPGParser;
class GPGParseEntry;
typedef void (GPGParser::* GPGTransferFunction)(GPGParseEntry &entry);


/******************************************************************************
class GPGParseEntry

	size is space in buffer
	length is number in file

	for output, size elements are to be written,
		but the block-length attribute is still from length

******************************************************************************/
class GPGParseEntry
	{
	public:
				GPGParseEntry(void)			{ };

		void	Set(GPGTransferFunction fp,
							const GPGVoidStar &instance,
							const char *type,const char *name,
							long size=1,long bits=0,
							const char *indexname=NULL,long granularity=1)
					{
					m_fp=fp;
					m_instance=instance;
					m_pinstance=(void *)NULL;
					m_length=0;
					m_size=size;
					m_granularity=granularity;
					m_bits=bits;
					m_indention=0;
					m_array=false;
					m_list=false;
					m_repeatable=false;
					m_elemental=false;
					m_elementsize=1;
					m_elementcount=0;

					if(type)
						SetType(type);
					else
						m_type[0]=0;

					if(name)
						SetName(name);
					else
						m_name[0]=0;

					if(indexname)
						SetIndexName(indexname);
					else
						m_indexname[0]=0;
					};

				GPGParseEntry(GPGParseEntry &operand)
					{
					m_fp=operand.m_fp;
					m_instance=operand.m_instance;
					m_pinstance=operand.m_pinstance;
					m_length=operand.m_length;
					m_size=operand.m_size;
					m_granularity=operand.m_granularity;
					m_bits=operand.m_bits;
					m_indention=operand.m_indention;
					m_array=operand.m_array;
					m_list=operand.m_list;
					m_repeatable=operand.m_repeatable;
					m_elemental=operand.m_elemental;
					m_elementsize=operand.m_elementsize;
					m_elementcount=operand.m_elementcount;
					SetType(operand.m_type);
					SetName(operand.m_name);
					SetIndexName(operand.m_indexname);
					};

		void	CallFunction(GPGParser &parser,long length,long bits=0)
									{
									m_length=length;
									m_bits=bits;
									(parser.*m_fp)(*this);
									};

const	char	*GetType(void) const		{ return m_type; };
		void	SetType(const char *set)
						{
						strncpy(m_type,set,GPGPARSE_NAMELENGTH);
						m_type[GPGPARSE_NAMELENGTH-1]=0;
						}

const	char	*GetName(void) const		{ return m_name; };
		void	SetName(const char *set)
					{
					strncpy(m_name,set,GPGPARSE_NAMELENGTH);
					m_name[GPGPARSE_NAMELENGTH-1]=0;
					}

const	char	*GetIndexName(void) const		{ return m_indexname; };
		void	SetIndexName(const char *set)
					{
					strncpy(m_indexname,set,GPGPARSE_NAMELENGTH);
					m_indexname[GPGPARSE_NAMELENGTH-1]=0;
					}

		void		SetInstance(const GPGVoidStar &set)	{ m_instance=set; };
//		GPGVoidStar	GetInstance(void) const				{ return m_instance; };
		GPGVoidStar	&GetInstance(void)					{ return m_instance; };
		GPGVoidStar	&GetInstanceReference(void)			{ return m_instance; };

		void		SetPointerToInstance(const GPGVoidStar &set)
														{ m_pinstance=set; };
		GPGVoidStar	&GetPointerToInstance(void)			{ return m_pinstance; };

		long	GetSize(void) const			{ return m_size; };
		void	SetSize(long set)			{ m_size=set; };

		long	GetGranularity(void) const	{ return m_granularity; };
		void	SetGranularity(long set)	{ m_granularity=set; };

		long	GetBits(void) const			{ return m_bits; };
		void	SetBits(long set)			{ m_bits=set; };

		long	GetLength(void) const		{ return m_length; };
		void	SetLength(long set)			{ m_length=set; };

		long	GetIndention(void) const	{ return m_indention; };
		void	SetIndention(long set)		{ m_indention=set; };

		bool	GetIsArray(void) const		{ return m_array; };
		void	SetIsArray(bool set)		{ m_array=set; };

		bool	GetIsList(void) const		{ return m_list; };
		void	SetIsList(bool set)			{ m_list=set; };

		bool	GetIsElemental(void) const	{ return m_elemental; };
		void	SetIsElemental(bool set)	{ m_elemental=set; };

		bool	GetIsRepeatable(void) const	{ return m_repeatable; };
		void	SetIsRepeatable(bool set)	{ m_repeatable=set; };

		long	GetElementSize(void) const	{ return m_elementsize; };
		void	SetElementSize(long set)	{ m_elementsize=set; };

		long	GetElementCount(void) const	{ return m_elementcount; };
		void	SetElementCount(long set)	{ m_elementcount=set; };
		void	IncElementCount(void)		{ m_elementcount++; };

		bool	TypeMatches(const char *type)
					{ return (!m_type[0] || !strcmp(m_type,type)); };
		bool	NameMatches(const char *name)
					{ return (!m_name[0] || !strcmp(m_name,name)); };


	private:

		GPGTransferFunction		m_fp;
		GPGVoidStar				m_instance;
		GPGVoidStar				m_pinstance;	// used to allocate elemental
		char					m_type[GPGPARSE_NAMELENGTH];
		char					m_name[GPGPARSE_NAMELENGTH];
		char					m_indexname[GPGPARSE_NAMELENGTH];
		long					m_length;
		long					m_size;
		long					m_granularity;
		long					m_bits;
		long					m_indention;
		bool					m_array;
		bool					m_list;
		bool					m_repeatable;

		bool					m_elemental;
		long					m_elementsize;
		long					m_elementcount;
	};



#endif /* __GPGParseEntry_h__ */
