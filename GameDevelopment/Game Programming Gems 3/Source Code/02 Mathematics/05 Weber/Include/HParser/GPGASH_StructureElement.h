
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

#ifndef __GPGASH_StructureElement_h__
#define __GPGASH_StructureElement_h__



/******************************************************************************
class GPGASH_StructureElement

******************************************************************************/
class GPGASH_StructureElement
	{
	public:

		void	SetBits(long set)			{ m_bits=set; };
		long	GetBits(void)				{ return m_bits; };

		void	SetSize(long set)			{ m_size=set; };
		long	GetSize(void)				{ return m_size; };

		char	*GetType(void)				{ return m_type; };
		void	SetType(const char *set)
					{
					strncpy(m_type,set,GPGPARSE_NAMELENGTH-1);
					m_type[GPGPARSE_NAMELENGTH-1]=0;
					};

		char	*GetName(void)				{ return m_name; };
		void	SetName(const char *set)
					{
					strncpy(m_name,set,GPGPARSE_NAMELENGTH-1);
					m_name[GPGPARSE_NAMELENGTH-1]=0;
					};

	private:
		char	m_type[GPGPARSE_NAMELENGTH];
		char	m_name[GPGPARSE_NAMELENGTH];
		long	m_size;
		long	m_bits;
	};



#endif /* __GPGASH_StructureElement_h__ */
