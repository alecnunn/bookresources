
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

#ifndef __GPGConstraints_h__
#define __GPGConstraints_h__



/******************************************************************************
class GPGConstraints

******************************************************************************/
class GPGConstraints
	{
	public:
						GPGConstraints(void)
							{
							m_world_relative=true;

							m_active[0]=true;
							m_active[1]=true;
							m_active[2]=true;

							m_limited[0]=false;
							m_limited[1]=false;
							m_limited[2]=false;

							m_bias[0]=2;
							m_bias[1]=2;
							m_bias[2]=2;
							m_lastEuler.Reset();

							m_min.Reset();
							m_max.Reset();
							};

		void			SetActive(bool x,bool y,bool z)
							{
							m_active[0]=x;
							m_active[1]=y;
							m_active[2]=z;
							};
		void			SetLimited(bool x,bool y,bool z)
							{
							m_limited[0]=x;
							m_limited[1]=y;
							m_limited[2]=z;
							};
		void			SetMin(float x,float y,float z)
							{ m_min.Set(x,y,z); };
		void			SetMax(float x,float y,float z)
							{ m_max.Set(x,y,z); };

		bool			&WorldRelative(void)	{ return m_world_relative; };
const	bool			&WorldRelativeConst(void) const
												{ return m_world_relative; };

		bool			*Active(void)					{ return m_active; };
const	bool			*ActiveConst(void) const		{ return m_active; };
		bool			*Limited(void)					{ return m_limited; };
const	bool			*LimitedConst(void) const		{ return m_limited; };

		char			*Bias(void)						{ return m_bias; };
		GPGEuler		&LastEuler(void)				{ return m_lastEuler; };

		GPGVector3		&Min(void)						{ return m_min; };
const	GPGVector3		&MinConst(void) const			{ return m_min; };
		GPGVector3		&Max(void)						{ return m_max; };
const	GPGVector3		&MaxConst(void) const			{ return m_max; };

#if GPGALLOW_STREAMS
		std::ostream	&Print(std::ostream &out,long indention) const;
#endif

		GPGString		Out(bool verbose=false) const;

	private:
		bool			m_world_relative;
		bool			m_active[3];
		bool			m_limited[3];
		GPGVector3		m_min;
		GPGVector3		m_max;

		char			m_bias[3];		// dynamic preference to stablize IK
										// trinary: 0=min 1=max 2=don't care
		GPGEuler		m_lastEuler;	// history
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGCharacter &character);
#endif



#endif /* __GPGConstraints_h__ */
