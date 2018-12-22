
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

#ifndef __GPGParseTable_h__
#define __GPGParseTable_h__



/******************************************************************************
class GPGParseTable: public GPGList<GPGParseEntry>

******************************************************************************/
class GPGParseTable: public GPGArray<GPGParseEntry>
	{
	public:

						GPGParseTable(GPGParser &set);
virtual					~GPGParseTable(void);

		bool			GetNewlining(void)		{ return m_newlining; };
		void			SetNewlining(bool set)	{ m_newlining=set; };

		void			RememberOrder(void)
							{
							if(m_rememberflag)
								m_rememberstored=true;
							else
								m_rememberflag=true;
							if(m_remembermap==NULL)
								m_remembermap=new GPGArray<long>;
							m_remembercount=0;
							};

		void			Expect(GPGTransferFunction fp,GPGVoidStar instance,
								const char *type,const char *name,
								long size=1,long bits=0,
								const char *indexname=NULL,long granularity=1);
		void			ExpectRepeatable(GPGTransferFunction fp,
								GPGVoidStar instance,
								const char *type,const char *name,
								long size=1,long bits=0,
								const char *indexname=NULL,long granularity=1);
		void			ExpectElemental(long elementsize,GPGTransferFunction fp,
								GPGVoidStar pinstance,GPGVoidStar instance,
								const char *type,const char *name,
								long size=1,long bits=0,
								const char *indexname=NULL,long granularity=1);
		void			ExpectArray(GPGTransferFunction fp,GPGCoreArray *array,
								const char *type,const char *name,
								long size=1,long bits=0,
								const char *indexname=NULL,long granularity=1);
		void			ExpectList(GPGTransferFunction fp,GPGCoreList *list,
								const char *type,const char *name,
								long size=1,long bits=0,
								const char *indexname=NULL,long granularity=1);

		bool			InputBlock(void);

		GPGParseEntry	*Find(const char *type,const char *name);

		void			AlterPointer(U32 index,const GPGVoidStar &instance)
							{
							GPGASSERT(index<GetNumberElements());

							GetElement(index).SetInstance(instance);
							};

	private:
		GPGParser		*m_parser;
		bool			m_newlining;

		bool			m_rememberflag;
		bool			m_rememberstored;
		long			m_remembercount;
		GPGArray<long>	*m_remembermap;
	};



#endif /* __GPGParseTable_h__ */
