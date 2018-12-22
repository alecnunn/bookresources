
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

#ifndef __GPGASH_Context_h__
#define __GPGASH_Context_h__



/******************************************************************************
class GPGASH_Context

******************************************************************************/
class GPGASH_Context
	{
	public:

							GPGASH_Context(void)		{ Reset(); };

		void				Reset(void)
								{
								m_repetitions=0;
								m_multiplier=1;
								m_lastintname[0]=0;
								m_lastint=0;
								};

		GPGASH_Structure	*GetCurrent(void)		{ return m_current; };
		void				SetCurrent(GPGASH_Structure *set)
													{ m_current=set; };

		long				GetCounter(void)		{ return m_counter; };
		void				SetCounter(long set)	{ m_counter=set; };
		void				IncrementCounter(void)	{ m_counter++; };

		long				GetMultiplier(void)		{ return m_multiplier; };
		void				SetMultiplier(long set)	{ m_multiplier=set; };

		long				GetRepetitions(void)	{ return m_repetitions; };
		void				SetRepetitions(long set){ m_repetitions=set; };
		long				DecrementRepetitions(void)
								{
								m_repetitions--;
								return m_repetitions;
								};

		void				GetLastInt(char *name,long *size)
								{
								strncpy(name,m_lastintname,
												GPGPARSE_NAMELENGTH-1);
								name[GPGPARSE_NAMELENGTH-1]=0;

								*size=m_lastint;
								};
		void				SetLastInt(const char *name,long size)
								{
								strncpy(m_lastintname,name,
												GPGPARSE_NAMELENGTH-1);
								m_lastintname[GPGPARSE_NAMELENGTH-1]=0;

								m_lastint=size;
								};

		void				SetLastExtraction(char *name,long rep)
								{
								strncpy(m_lastextractname,name,
												GPGPARSE_NAMELENGTH-1);
								m_lastextractname[GPGPARSE_NAMELENGTH-1]=0;

								m_lastextractrep=rep;
								};
		long				GetLastExtraction(char *name)
								{
								strncpy(name,m_lastextractname,
												GPGPARSE_NAMELENGTH-1);
								name[GPGPARSE_NAMELENGTH-1]=0;

								return m_lastextractrep;
								};

	private:

		GPGASH_Structure	*m_current;
		long				m_counter;
		long				m_repetitions;
		long				m_multiplier;

		char				m_lastintname[GPGPARSE_NAMELENGTH];
		long				m_lastint;
		char				m_lastextractname[GPGPARSE_NAMELENGTH];
		long				m_lastextractrep;
	};



#endif /* __GPGASH_Context_h__ */
