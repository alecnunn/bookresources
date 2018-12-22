
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

#ifndef __GPGKeyTrack_h__
#define __GPGKeyTrack_h__



/******************************************************************************
class GPGKeyTrack: public GPGList<GPGKeyFrame>

******************************************************************************/
class GPGKeyTrack: public GPGList<GPGKeyFrame>
	{
	public:
						GPGKeyTrack(void)
						{
							SetName("Track");
							ToHead(m_current);
							SetAutoDestruct(true);
						}

		GPGKeyTrack		&operator=(const GPGKeyTrack &other)
						{
							GPGList<GPGKeyFrame>::operator=(other);
							return *this;
						}

virtual	void			AllocElement(GPGVariant *get)
						{
							GPGKeyFrame *entry=new GPGKeyFrame();
							Append(entry);
							*get=entry;
						}

		void			CalcInstantConst(float time,GPGInstant *instant,
									GPGListContext *context=NULL) const;

		void			InsertNewKeyFrame(float time,const GPGInstant &instant,
											GPGListContext *context=NULL);

		GPGString		GetName(void)				{ return m_name; };
const	GPGString		&GetNameConst(void) const	{ return m_name; };
		void			SetName(GPGString set)		{ m_name=set; };

		void			Filter(F32 deltatime);
		void			Compress(F32 deltaposition,F32 deltarotation,
														F32	deltascale=0.01f);

#if GPGALLOW_STREAMS
		std::ostream	&Print(std::ostream &out,long indention) const;
#endif

		GPGString		Out(bool verbose=false) const;


	private:

		void			Sync(float time,GPGListContext *context) const;

		GPGString		m_name;
		GPGListContext	m_current;
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGKeyTrack &track);
#endif

#endif /* __GPGKeyTrack_h__ */
