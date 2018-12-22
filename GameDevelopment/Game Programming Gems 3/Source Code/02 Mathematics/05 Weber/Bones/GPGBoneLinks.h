
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

#ifndef __GPGBoneLinks_h__
#define __GPGBoneLinks_h__



/******************************************************************************
class GPGBoneLinks

	link length is for display purposes

******************************************************************************/
class GPGBoneLinks
	{
	public:
						GPGBoneLinks(void)
							{
							m_numberlinks=0;
							m_linklength=1.0f;
//							ResetSamples();
							};

		GPGBoneNode		*GetLink(I32 index)	{ return m_linkarray[index]; };
						// if index<0, enter into next NULL location
		void			SetLink(I32 index,GPGBoneNode *set)
							{
							if(index>=0)
								{
								m_linkarray[index]=set;
								return;
								}

							U32 m;
							for(m=0;m<(U32)m_numberlinks;m++)
								if(!m_linkarray[m])
									{
									m_linkarray[m]=set;
									return;
									}
							GPGASSERT(false);	// more links than m_numberlinks
							};

		void			SetNumberLinks(I32 set)	{ m_numberlinks=set; };
		I32				&GetNumberLinks(void)	{ return m_numberlinks; };

		void			AllocateArray(void)
							{
							m_linkarray.ResizeToExactly(m_numberlinks);
							U32 m;
							for(m=0;m<(U32)m_numberlinks;m++)
								m_linkarray[m]=NULL;
							};
		I32				GetArraySize(void)
							{ return m_linkarray.GetNumberElements(); };

		float			&GetLinkLength(void)	{ return m_linklength; };
		void			SetLinkLength(float set)
							{
							m_linklength=set;
//							m_samples= -1;
							};

#if FALSE
		void			ResetSamples(void)
							{
							m_linklength=0.0f;
							m_samples=0;
							};
		void			SampleLength(float sample)
							{
							if(m_samples<0)
								return;

// GPGLOG("sample=%.6G length=%.6G m_samples=%d\n",sample,m_linklength,m_samples);

							m_linklength=(m_linklength*m_samples+sample)/
														(float)(m_samples+1);
							m_samples++;

// GPGLOG("            length=%.6G m_samples=%d\n",m_linklength,m_samples);
							};
#endif

	private:

		I32						m_numberlinks;
		GPGArray<GPGBoneNode *>	m_linkarray;
		F32						m_linklength;

//		I32						m_samples;
	};



#endif /* __GPGBoneLinks_h__ */
