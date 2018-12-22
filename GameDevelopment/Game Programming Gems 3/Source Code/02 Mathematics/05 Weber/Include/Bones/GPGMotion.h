
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

#ifndef __GPGMotion_h__
#define __GPGMotion_h__



/******************************************************************************
class GPGMotion

	Motion of the various parts is through indexed "tracks"

******************************************************************************/
class GPGMotion
	{
	public:
						GPGMotion(void)
							{
							SetName("motion");
							Reset();
							};

virtual					~GPGMotion(void)
							{
							Reset();
							};

		void			Reset(void)
							{
							m_tracks.Clear();
							};

		GPGString		GetName(void)					{ return m_name; };
const	GPGString		&GetNameConst(void) const		{ return m_name; };
		void			SetName(const GPGString &name)	{ m_name=name; };

		void			CalcInstantConst(float time,U32 track,
									GPGInstant *instant,
									GPGListContext *context=NULL) const
							{ GetTrackConst(track)
									.CalcInstantConst(time,instant,context); };

						// returns -1 when not found
		long			GetTrackIndexByName(const GPGString &name);

		U32				GetNumberTracks(void)
							{ return m_tracks.GetNumberElements(); };
		GPGKeyTrack		&GetTrack(U32 index)
							{
							GPGASSERT(index<m_tracks.GetNumberElements());
							return m_tracks[index];
							};
const	GPGKeyTrack		&GetTrackConst(U32 index) const
							{
							GPGASSERT(index<m_tracks.GetNumberElements());
							return m_tracks[index];
							};

		GPGRESULT		GetTimeLimits(float *min,float *max);

		U32				CreateTrack(const GPGString &name)
							{
							GPGKeyTrack &track=m_tracks.CreateNewElement();
							track.SetName(name);

							return m_tracks.GetNumberElements()-1;
							};

							// only parser should need this
		GPGKeyTrackArray	&GetTracks(void)		{ return m_tracks; };

		void			FilterAllTracks(float deltatime)
							{
							U32 m,numtracks=m_tracks.GetNumberElements();
							for(m=0;m<numtracks;m++)
								m_tracks[m].Filter(deltatime);
							};
		void			CompressAllTracks(float deltaposition,
														float deltarotation)
							{
							U32 m,numtracks=m_tracks.GetNumberElements();
							for(m=0;m<numtracks;m++)
								m_tracks[m].Compress(deltaposition,
																deltarotation);
							};

	private:

		GPGKeyTrackArray	m_tracks;
		GPGString			m_name;
	};



#endif /* __GPGMotion_h__ */
