
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

#ifndef __GPGInstant_h__
#define __GPGInstant_h__



/******************************************************************************
	This represents an instantaneous state in time including location,
	rotation, and scale.
******************************************************************************/
class GPGInstant
	{
	public:
						GPGInstant(void)	{ Reset(); };

						/// Reset data to identity values
		void			Reset(void)
							{
							m_location.Reset();
							m_rotation.MakeIdentity();
							m_scale.Set(1.0f,1.0f,1.0f);
							};

						/// Standard copy constructor
						GPGInstant(const GPGInstant &operand)
							{ operator=(operand); };

						/// Access the location component
		GPGVector3		&Location(void)					{ return m_location; };
						/// Access the location component disallowing changes
const	GPGVector3		&LocationConst(void)	const	{ return m_location; };

						/// Access the rotation component
		GPGQuaternion	&Rotation(void)					{ return m_rotation; };
						/// Access the rotation component disallowing changes
const	GPGQuaternion	&RotationConst(void)	const	{ return m_rotation; };

						/// Access the scale component
		GPGVector3		&Scale(void)					{ return m_scale; };
						/// Access the scale component disallowing changes
const	GPGVector3		&ScaleConst(void)		const	{ return m_scale; };

						/// Standard deep copy
		GPGInstant		&operator=(const GPGInstant &operand)
							{
							m_location=operand.m_location;
							m_rotation=operand.m_rotation;
							m_scale=operand.m_scale;

							return *this;
							};

#if GPGALLOW_STREAMS
		std::ostream	&Print(std::ostream &out,long indention) const;
#endif

						/// Return a text representation of current state
		GPGString		Out(bool verbose=false) const;

	private:
		GPGVector3		m_location;
		GPGQuaternion	m_rotation;
		GPGVector3		m_scale;
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGInstant &instant);
#endif



#endif /* __GPGInstant_h__ */
