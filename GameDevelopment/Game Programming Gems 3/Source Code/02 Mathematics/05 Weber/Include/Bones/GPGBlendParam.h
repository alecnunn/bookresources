
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

#ifndef __GPGBlendParam_h__
#define __GPGBlendParam_h__



/******************************************************************************
class GPGBlendParam

	configuration for a motion blend

	StopLast:		true  = last motion stops its current time at start of blend
					false = last motion continues during blend

	Incremental:	true  = continuously uses last blend in place of
							computed last motion
							(alters linearity and makes StopLast irrelevant)
					false = actual last motion is used directly

	StartTime:		where in next motion the current time of last motion
					is started, basically where you plan to set the time
					for the first new frame

	Realignment:	an offset into the last motion where the next motion's
					StartTime aligns better.
					(time scaling and/or delay used to compensate)

	BlendTime:		how long from StartTime the blend transitions
					from last motion to next motion

	Exponent:		adjusts the linearity of the weighting over time
					weight_effective = weight ^ Exponent


******************************************************************************/
class GPGBlendParam
	{
	public:
				GPGBlendParam(void)
					{
					m_stoplast=true;
					m_incremental=true;
					m_starttime=0.0f;
					m_realignment=0.0f;
					m_blendtime=0.5f;
					m_exponent=1.0f;
					};

		void	operator=(const GPGBlendParam &other)
					{
					m_stoplast=other.m_stoplast;
					m_incremental=other.m_incremental;
					m_starttime=other.m_starttime;
					m_realignment=other.m_realignment;
					m_blendtime=other.m_blendtime;
					m_exponent=other.m_exponent;
					};

		bool	GetStopLastConst(void) const	{ return m_stoplast; };
		void	SetStopLast(bool set)			{ m_stoplast=set; };

		bool	GetIncrementalConst(void) const	{ return m_incremental; };
		void	SetIncremental(bool set)		{ m_incremental=set; };

		F32		GetStartTimeConst(void) const	{ return m_starttime; };
		void	SetStartTime(F32 set)			{ m_starttime=set; };

		F32		GetRealignmentConst(void) const	{ return m_realignment; };
		void	SetRealignment(F32 set)			{ m_realignment=set; };

		F32		GetBlendTimeConst(void) const	{ return m_blendtime; };
		void	SetBlendTime(F32 set)			{ m_blendtime=set; };

		F32		GetExponentConst(void) const	{ return m_exponent; };
		void	SetExponent(F32 set)			{ m_exponent=set; };

	private:
		bool	m_stoplast;
		bool	m_incremental;
		F32		m_starttime;
		F32		m_realignment;
		F32		m_blendtime;
		F32		m_exponent;
	};

#endif /* __GPGBlendParam_h__ */

