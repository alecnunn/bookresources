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

#ifndef GPGVector4_h
#define GPGVector4_h

#include "GPGDebug.h"
#include "GPGDataTypes.h"
#include "GPGVector3.h"


class GPGVector4
{
	public:

					GPGVector4(void)					{}
					GPGVector4(F32 r,F32 g,F32 b)		{ Set(r,g,b); }
					GPGVector4(F32 r,F32 g,F32 b,F32 a)	{ Set(r,g,b,a); }
					GPGVector4(const F32 *floats)		{ operator=(floats); }
					GPGVector4(const GPGVector3 &operand) {operator=(operand);}
					GPGVector4(const GPGVector4 &operand) {operator=(operand);}

		GPGVector4	&operator=(const F32 *floats);
		GPGVector4	&operator=(const GPGVector3 &operand);
		GPGVector4	&operator=(const GPGVector4 &operand);

		BOOL		operator==(const GPGVector4& operand) const;
		BOOL		operator!=(const GPGVector4& operand) const;
		BOOL		NearlyEquals(const GPGVector4& operand,F32 margin) const;

					/// Reset vector to 0,0,0,1
		void		Reset(void);
					/// Set all 4 RGBA or XYZH components (4th defaults to 1.0)
		void		Set(F32 r, F32 g, F32 b, F32 a=1.0f);

					/// Access the Red component
		F32			R(void) const						{ return m_value[0]; }
					/// Access the Green component
		F32			G(void) const						{ return m_value[1]; }
					/// Access the Blue component
		F32			B(void) const						{ return m_value[2]; }
					/// Access the Alpha component
		F32			A(void) const						{ return m_value[3]; }
					/// Access the X component
		F32			X(void) const						{ return m_value[0]; }
					/// Access the Y component
		F32			Y(void) const						{ return m_value[1]; }
					/// Access the Z component
		F32			Z(void) const						{ return m_value[2]; }
					/// Access the H component (the 4th)
		F32			H(void) const						{ return m_value[3]; }
					/// Access the Radius component (the 4th)
		F32			Radius(void) const					{ return m_value[3]; }

					/// Access element using an index in the range 0 to 3
		F32			&Value(U32 index);

		F32			*Raw(void)							{ return m_value; };
const	F32			*RawConst(void) const				{ return m_value; };
		F32			&operator[](U32 n);

		GPGVector4	&Add(const GPGVector4& right);
		GPGVector4	&Add(const GPGVector4& left, const GPGVector4& right);
		GPGVector4	&Subtract(const GPGVector4& right);
		GPGVector4	&Subtract(const GPGVector4& left, const GPGVector4& right);
		GPGVector4	&Multiply(const GPGVector4& right);
		GPGVector4	&Multiply(const GPGVector4& left, const GPGVector4& right);
		GPGVector4	&Scale3(const F32 fScale);
		GPGVector4	&Min(const GPGVector4& operand);
		GPGVector4	&Min(const F32* floats);
		GPGVector4	&Max(const GPGVector4& operand);
		GPGVector4	&Max(const F32* floats);
		F32			DotProduct3(const GPGVector4 &operand) const;
		GPGVector4	&CrossProduct3(const GPGVector4 &left,
                                                    const GPGVector4 &right);
		F32			CalcMagnitude3(void) const;
		F32			CalcMagnitudeSquared3(void) const;
		GPGVector4	&Normalize3(void);

					/** treating the forth element as radius, this function
					sets this "sphere" as the collective bounds of this
					current sphere and the given operand's sphere. */
		void		IncorporateSphere(const GPGVector4 &bound);

		GPGString	Out(BOOL verbose=false) const;

	private:
		F32 m_value[4];
};

GPGINLINE GPGVector4 &GPGVector4::operator=(const F32 *floats)
{
	// 4 floats assumed
	m_value[0]=floats[0];
	m_value[1]=floats[1];
	m_value[2]=floats[2];
	m_value[3]=floats[3];
	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::operator=(const GPGVector3 &operand)
{
	const F32 *v=operand.RawConst();
	m_value[0]=v[0];
	m_value[1]=v[1];
	m_value[2]=v[2];
	m_value[3]=1.0f;
	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::operator=(const GPGVector4 &operand)
{
	m_value[0]=operand.m_value[0];
	m_value[1]=operand.m_value[1];
	m_value[2]=operand.m_value[2];
	m_value[3]=operand.m_value[3];
	return *this;
}

GPGINLINE BOOL GPGVector4::operator==(const GPGVector4 &operand) const
{
	// DJ 8/14/00 - unrolled to reduce branch count to 1.
	U32 m = (*(U32*)&m_value[0]) - (*(U32*)&operand.m_value[0]);
	m |= (*(U32*)&m_value[1]) - (*(U32*)&operand.m_value[1]);
	m |= (*(U32*)&m_value[2]) - (*(U32*)&operand.m_value[2]);
	m |= (*(U32*)&m_value[3]) - (*(U32*)&operand.m_value[3]);

	if(m == 0)
		return true;

	return false;
}

GPGINLINE BOOL GPGVector4::operator!=(const GPGVector4 &operand) const
{
	return !(*this==operand);
}

GPGINLINE BOOL GPGVector4::NearlyEquals(const GPGVector4& operand,
														F32 margin) const
{
	return (fabs(m_value[0]-operand.m_value[0])<margin &&
			fabs(m_value[1]-operand.m_value[1])<margin &&
			fabs(m_value[2]-operand.m_value[2])<margin);
}

GPGINLINE void GPGVector4::Reset(void)
{
	m_value[0] = 0.0f;
	m_value[1] = 0.0f;
	m_value[2] = 0.0f;
	m_value[3] = 1.0f;
}

GPGINLINE void GPGVector4::Set(F32 r,F32 g,F32 b,F32 a)
{
	m_value[0] = r;
	m_value[1] = g;
	m_value[2] = b;
	m_value[3] = a;
}

GPGINLINE F32 &GPGVector4::operator[](U32 n)
{
	GPGASSERT(n<4);
	return m_value[n];
}

GPGINLINE F32& GPGVector4::Value(U32 index)
{
	GPGASSERT(index < 4);
	return m_value[index];
}

GPGINLINE GPGVector4 &GPGVector4::Multiply(const GPGVector4& right)
{
	m_value[0] *= right.m_value[0];
	m_value[1] *= right.m_value[1];
	m_value[2] *= right.m_value[2];
	m_value[3] *= right.m_value[3];

	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::Multiply(const GPGVector4& left,
											const GPGVector4& right)
{
	m_value[0] = left.m_value[0] * right.m_value[0];
	m_value[1] = left.m_value[1] * right.m_value[1];
	m_value[2] = left.m_value[2] * right.m_value[2];
	m_value[3] = left.m_value[3] * right.m_value[3];

	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::Add(const GPGVector4& right)
{
	m_value[0] += right.m_value[0];
	m_value[1] += right.m_value[1];
	m_value[2] += right.m_value[2];
	m_value[3] += right.m_value[3];

	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::Add(const GPGVector4& left,
										const GPGVector4& right)
{
	m_value[0] = left.m_value[0] + right.m_value[0];
	m_value[1] = left.m_value[1] + right.m_value[1];
	m_value[2] = left.m_value[2] + right.m_value[2];
	m_value[3] = left.m_value[3] + right.m_value[3];

	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::Min(const GPGVector4& operand)
{
	if(operand.m_value[0] < m_value[0]) m_value[0] = operand.m_value[0];
	if(operand.m_value[1] < m_value[1]) m_value[1] = operand.m_value[1];
	if(operand.m_value[2] < m_value[2]) m_value[2] = operand.m_value[2];
	if(operand.m_value[3] < m_value[3]) m_value[3] = operand.m_value[3];

	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::Min(const F32* floats)
{
	if(floats[0] < m_value[0]) m_value[0] = floats[0];
	if(floats[1] < m_value[1]) m_value[1] = floats[1];
	if(floats[2] < m_value[2]) m_value[2] = floats[2];
	if(floats[3] < m_value[3]) m_value[3] = floats[3];

	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::Max(const GPGVector4& operand)
{
	if(operand.m_value[0] > m_value[0]) m_value[0] = operand.m_value[0];
	if(operand.m_value[1] > m_value[1]) m_value[1] = operand.m_value[1];
	if(operand.m_value[2] > m_value[2]) m_value[2] = operand.m_value[2];
	if(operand.m_value[3] > m_value[3]) m_value[3] = operand.m_value[3];

	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::Max(const F32* floats)
{
	if(floats[0] > m_value[0]) m_value[0] = floats[0];
	if(floats[1] > m_value[1]) m_value[1] = floats[1];
	if(floats[2] > m_value[2]) m_value[2] = floats[2];
	if(floats[3] > m_value[3]) m_value[3] = floats[3];

	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::Subtract(const GPGVector4& right)
{
	m_value[0] -= right.m_value[0];
	m_value[1] -= right.m_value[1];
	m_value[2] -= right.m_value[2];
	m_value[3] -= right.m_value[3];

	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::Subtract(const GPGVector4& left,
											const GPGVector4& right)
{
	m_value[0] = left.m_value[0] - right.m_value[0];
	m_value[1] = left.m_value[1] - right.m_value[1];
	m_value[2] = left.m_value[2] - right.m_value[2];
	m_value[3] = left.m_value[3] - right.m_value[3];

	return *this;
}

GPGINLINE GPGVector4 &GPGVector4::Scale3(const F32 fScale)
{
	m_value[0] *= fScale;
	m_value[1] *= fScale;
	m_value[2] *= fScale;

	return *this;
}

GPGINLINE F32 GPGVector4::DotProduct3(const GPGVector4 &operand) const
{
	GPGVector3 left=RawConst(),right=operand.RawConst();
	return left.DotProduct(right);
}

GPGINLINE GPGVector4 &GPGVector4::CrossProduct3(const GPGVector4 &left,
                                                    const GPGVector4 &right)
{
	GPGVector3 result,left3=left.RawConst(),right3=right.RawConst();
	result.CrossProduct(left3,right3);
	operator=(result.RawConst());
	return *this;
}

GPGINLINE F32 GPGVector4::CalcMagnitude3(void) const
{
	GPGVector3 vector=m_value;
	return vector.CalcMagnitude();
}

GPGINLINE F32 GPGVector4::CalcMagnitudeSquared3(void) const
{
	GPGVector3 vector=m_value;
	return vector.CalcMagnitudeSquared();
}

GPGINLINE GPGVector4 &GPGVector4::Normalize3(void)
{
	GPGVector3 vector=m_value;
	if(vector.IsNonZero())
		return Scale3(1.0f/vector.CalcMagnitude());

	m_value[0]=0.0f;
	m_value[1]=0.0f;
	m_value[2]=0.0f;
	return *this;
}

#endif
