
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

#ifndef __GPGEuler_h__
#define __GPGEuler_h__



/******************************************************************************
class GPGEuler

	The only intent of this function is for conversions to and from
	GPGQuaternion.

	The order of rotations are Z->Y->X
	So given a GPGQuaternion quaternion1,

		GPGEuler euler=quaternion1;

	creates an euler whose three angle values can be used like

		GPGQuaternion quaternion2;
		quaternion2.MakeRotation(euler[0],Z_AXIS);
		quaternion2.Rotate(euler[1],Y_AXIS);
		quaternion2.Rotate(euler[2],X_AXIS);

	where quaternion2 == quaternion1.

	Note that there can be more than one Euler solution for any quaternion.
	An euler->quaternion->euler conversion may not result in the same
	euler angles.

	But a quaternion->euler->quaternion conversion will always return
	to the same quaternion, since there is only one quaternion solution
	for any aribitrary-axis rotation.  (Exceptional cases may occur
	around Euler problem areas.)

	Note that every quaternion has an exactly equivalent twin,
	with every element of opposite sign.   This pair is referred to
	as one solution.

	Note that some cases of 90 degree rotations can create Gimbal Lock
	in Euler angles by aligning rotation axes.  Quaternions are not
	suceptable to this, but conversions may not be accurate in these cases.

	Specifically, the Y result of a quaternion->euler conversion is limited
	to +-90 degrees.  Solution near the limits of Y are very unstable in
	X and Z.

******************************************************************************/
class GPGEuler
	{
	public:

						GPGEuler(void)		{ };

		void			Set(F32 x,F32 y,F32 z);
		void			Reset(void);

						GPGEuler(const GPGEuler &operand)
													{ operator=(operand); };
						GPGEuler(const GPGQuaternion &operand)
													{ operator=(operand); };

		GPGEuler		&operator=(const GPGEuler &operand);
		GPGEuler		&operator=(const GPGQuaternion &operand);

		bool			operator==(const GPGEuler &operand) const;

		void			ConvertToQuaternion(GPGQuaternion &operand) const;

		F32				&operator[](const int x)	{ return m_data[x]; };
		F32				*Raw(void)					{ return m_data; };
const	F32				*RawConst(void) const		{ return m_data; };

#if GPGALLOW_STREAMS
		std::ostream	&Print(std::ostream &out) const;
#endif

		GPGString		Out(bool verbose=false) const;

	private:
		F32				m_data[3];
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGEuler &quat);
#endif



GPGINLINE void GPGEuler::Set(F32 x,F32 y,F32 z)
	{
	m_data[0]=x;
	m_data[1]=y;
	m_data[2]=z;
	}

GPGINLINE void GPGEuler::Reset(void)
	{
	m_data[0]=0.0f;
	m_data[1]=0.0f;
	m_data[2]=0.0f;
	}

GPGINLINE GPGEuler &GPGEuler::operator=(const GPGEuler &operand)
	{
	m_data[0]=operand.m_data[0];
	m_data[1]=operand.m_data[1];
	m_data[2]=operand.m_data[2];

	return *this;
	}

GPGINLINE bool GPGEuler::operator==(const GPGEuler &operand) const
	{
	return (	m_data[0]==operand.m_data[0]	&&
				m_data[1]==operand.m_data[1]	&&
				m_data[2]==operand.m_data[2]	);
	}

#endif /* __GPGEuler_h__ */
