
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

#ifndef __GPGMotionReader_h__
#define __GPGMotionReader_h__



/******************************************************************************
class GPGMotionReader

	connection from GPGCoreNode to potentially-shared single track on GPGMotion

******************************************************************************/
class GPGMotionReader
	{
	public:
					GPGMotionReader(void)		{ Reset(); };
virtual				~GPGMotionReader(void)
						{ m_context.SetCurrent(NULL); };

		void		Reset(void)
						{
						m_motion=NULL;
						m_track=0;

						m_timescale=1.0f;
						m_offset=0.0f;
						m_start=0.0f;
						m_length=1.0f;
						m_loop=false;
						m_oscillate=false;
						m_ik=false;
						m_reference=false;
						};

		void		SetScale(float set)			{ m_timescale=set; };
		float		GetScale(void)				{ return m_timescale; };

		void		SetOffset(float set)		{ m_offset=set; };
		float		GetOffset(void)				{ return m_offset; };

		void		SetStart(float set)			{ m_start=set; };
		float		GetStart(void)				{ return m_start; };

		void		SetLength(float set)		{ m_length=set; };
		float		GetLength(void)				{ return m_length; };

		void		SetIK(bool set)				{ m_ik=set; };
		bool		GetIK(void)					{ return m_ik; };

		void		SetReference(bool set)		{ m_reference=set; };
		bool		GetReference(void)			{ return m_reference; };

		bool		GetLoop(void)				{ return m_loop; };
		void		SetLoop(bool set)
						{
						m_loop=set;
						if(m_loop)
							m_oscillate=false;
						};

		bool		GetOscillate(void)			{ return m_oscillate; };
		void		SetOscillate(bool set)
						{
						m_oscillate=set;
						if(m_oscillate)
							m_loop=false;
						};

		bool		AssignByTrackID(GPGMotion *mot,U32 id)
						{
						if(id>=mot->GetTracks().GetNumberElements())
							{
							m_track=0;
							m_motion=NULL;
							return false;
							}

						m_motion=mot;
						m_track=id;
						return true;
						};

		bool		AssignByTrackName(GPGMotion *mot,GPGString name)
						{
						long track=mot->GetTrackIndexByName(name);
						if(track<0)
							{
							m_track=0;
							m_motion=NULL;
							return false;
							}

						m_track=track;
						m_motion=mot;
						return true;
						};

		bool		IsValid(void) const
						{
						return (m_motion!=NULL &&
										m_track<m_motion->GetNumberTracks());
						};

		bool		Read(float time,GPGInstant *instant)
						{
						if(!IsValid())
							{
							instant->Reset();
							return false;
							}

						GPGASSERT(m_motion);
						GPGASSERT(m_track<m_motion->GetTracks()
														.GetNumberElements());
						m_motion->CalcInstantConst(WarpTime(time),m_track,
														instant,&m_context);
						return true;
						};

		bool		GetTimeLimits(float *min,float *max)
						{
						if(!m_motion)
							return false;

						GPGASSERT(m_track<m_motion->GetTracks()
														.GetNumberElements());
						*min=m_motion->GetTrackConst(m_track).GetHead()->Time();
						*max=m_motion->GetTrackConst(m_track).GetTail()->Time();

						*min=WarpTime(*min);
						*max=WarpTime(*max);
						return true;
						}


	private:
		float		WarpTime(float time) const
						{
//						GPGLOG("%.6G ",time);

						time*=m_timescale;
						time+=m_offset;

						if(m_loop || m_oscillate)
							{
							long reps;
							if(time<m_start)
								reps=(long)((time-m_start)/m_length-0.9999f);
							else
								reps=(long)((time-m_start)/m_length);

							time-=reps*m_length;

							if(m_oscillate)
								{
								if( (reps%2) != (reps<0) )
									{
									time=(m_start+m_length)-(time-m_start);
									}
								}
							}

						return time;
						};

		float				m_timescale;
		float				m_offset;
		float				m_start;
		float				m_length;
		bool				m_loop;
		bool				m_oscillate;
		bool				m_ik;
		bool				m_reference;

		GPGMotion			*m_motion;
		U32					m_track;
		GPGListContext		m_context;
	};



#endif /* __GPGMotionReader_h__ */
