
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
//
//	DESCRIPTION
//    Interface for a 3-dimensional vector of floats
//

#ifndef __GPGVector3_h__
#define __GPGVector3_h__

#include "Base/GPGDebug.h"
#include "Base/GPGDataTypes.h"
#include "Base/GPGString.h"
#include <math.h>
#include "Base/GPGVector2.h"

#ifndef GPGALLOW_STREAMS
#define GPGALLOW_STREAMS 0
#endif


class GPGVector3
	{
	public:
						// no default clear
						GPGVector3(void)				{ };
						GPGVector3(F32 x,F32 y,F32 z)	{ Set(x,y,z); };

						// 3 floats assumed
						GPGVector3(const float *floats)
													{ operator=(floats); };

						GPGVector3(const GPGVector3 &operand)
													{ operator=(operand); };
						GPGVector3(const GPGVector2 &operand)
													{ operator=(operand); };

		void			Reset(void);
		void			Set(F32 x,F32 y,F32 z);
		GPGVector3		&operator=(const F32 *floats);
		GPGVector3		&operator=(const GPGVector3 &operand);
		GPGVector3		&operator=(const GPGVector2 &operand);
		BOOL			operator==(const GPGVector3 &operand) const;
		BOOL			IsApproximately(const GPGVector3 &operand,
															F32 margin) const;

		F32				*Raw(void)					{ return m_value; };
const	F32				*RawConst(void) const		{ return m_value; };
		F32				&operator[](U32 n);

		F32				&X(void)					{ return m_value[0]; }
		F32				&Y(void)					{ return m_value[1]; }
		F32				&Z(void)					{ return m_value[2]; }
		F32				&Value(U32 index);

						/// returns true if any componnent is non-zero
		BOOL			IsNonZero(void) const;
						/// returns true if any componnent is zero
		BOOL			HasZeroComponent(void) const;

		GPGVector3		&Add(const GPGVector3 &operand);
		GPGVector3		&Add(const GPGVector3 &left,const GPGVector3 &right);
		GPGVector3		&AddScaled(const GPGVector3 &operand,F32 scalar);

		GPGVector3		&Subtract(const GPGVector3 &operand);
		GPGVector3		&Subtract(const GPGVector3 &left,
													const GPGVector3 &right);

		GPGVector3		&Multiply(const GPGVector3 &operand);
		GPGVector3		&Multiply(const GPGVector3 &left,
												const GPGVector3 &right);

						/** divide each element in the vector with the
							correspnding element in the operand.
							Attempted division by zero results in
							no operation (on a per-element basis). */
		GPGVector3		&Divide(const GPGVector3 &operand);
						/** divide each element in the left vector with the
							correspnding element in the right vector and
							store the result in this vector.
							Attempted division by zero results in
							no operation (on a per-element basis). */
		GPGVector3		&Divide(const GPGVector3 &left,const GPGVector3 &right);

		GPGVector3		&Scale(F32 scalar);

						/// Normalize, then Scale
		GPGVector3		&Rescale(F32 magnitude);
						/// additive inverse
		GPGVector3		&Negate(void);
						/// multiplicative inverse
		GPGVector3		&Reciprocate(void);
						/// just calls Negate()
		GPGVector3		&Invert(void)		{ return Negate(); };

						// WARNING for cos(angle)=DotProduct(), both operands
						// must be pre-normalized
						// use CalcCosAngleFrom() if not normalized
		F32				DotProduct(const GPGVector3 &operand) const;
static	F32				DotProduct(const GPGVector3 &left,
													const GPGVector3 &right);
		GPGVector3		&CrossProduct(const GPGVector3 &left,
													const GPGVector3 &right);
		GPGVector3		&Normalize(void);

		F32				CalcMagnitude(void) const;
		F32				CalcMagnitudeSquared(void) const;	// faster

		F32				CalcDistanceFrom(const GPGVector3 &operand) const;
		F32				CalcDistanceSquaredFrom(const GPGVector3 &operand)const;

						// presumes vectors are not pre-normalized
						// just use DotProduct() if already normalized
		F32				CalcCosAngleFrom(const GPGVector3 &operand)const;
		F32				CalcCosAngleSquaredFrom(const GPGVector3 &operand)const;

		void			Interpolate(F32 t,const GPGVector3 &from,
														const GPGVector3 &to);
	// not GPGINLINE:

#if GPGALLOW_STREAMS
		std::ostream	&Print(std::ostream &out,U32 indention) const;
#endif

		GPGString		Out(BOOL verbose=false) const;

	private:
		F32				m_value[3];
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGVector3 &vector);
#endif

GPGINLINE void GPGVector3::Reset(void)
	{
	m_value[0]=0.0f;
	m_value[1]=0.0f;
	m_value[2]=0.0f;
	}

GPGINLINE void GPGVector3::Set(F32 x,F32 y,F32 z)
	{
	m_value[0]=x;
	m_value[1]=y;
	m_value[2]=z;
	}

GPGINLINE F32 &GPGVector3::operator[](U32 n)
	{
	GPGASSERT(n<3);
	return m_value[n];
	}

GPGINLINE F32 &GPGVector3::Value(U32 index)
	{
	GPGASSERT(index<3);
	return m_value[index];
	}

GPGINLINE BOOL GPGVector3::IsNonZero(void) const
{
	return (m_value[0]!=0.0f || m_value[1]!=0.0f || m_value[2]!=0.0f);
}

GPGINLINE BOOL GPGVector3::HasZeroComponent(void) const
{
	return (m_value[0]==0.0f || m_value[1]==0.0f || m_value[2]==0.0f);
}

GPGINLINE GPGVector3 &GPGVector3::operator=(const F32 *floats)
	{
	// 3 floats assumed

	m_value[0]=floats[0];
	m_value[1]=floats[1];
	m_value[2]=floats[2];

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::operator=(const GPGVector3 &operand)
	{
	m_value[0]=operand.m_value[0];
	m_value[1]=operand.m_value[1];
	m_value[2]=operand.m_value[2];

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::operator=(const GPGVector2 &operand)
	{
	m_value[0]=operand.RawConst()[0];
	m_value[1]=operand.RawConst()[1];
	m_value[2]=0.0f;

	return *this;
	}

// CLG - Watch out when comparing floating point values:
//         0x80000000 is negative zero (-1.0 * 2**-127), and 
//         0x00000000 is positive zero (1.0 * 2**-127)
// This method will return false when comparing negative zero with positive zero
GPGINLINE BOOL GPGVector3::operator==(const GPGVector3 &operand) const
	{
	// DJ 8/14/00 - unrolled to reduce branch count to 1.
	U32 m = (*(U32*)&m_value[0]) - (*(U32*)&operand.m_value[0]);
	m |= (*(U32*)&m_value[1]) - (*(U32*)&operand.m_value[1]);
	m |= (*(U32*)&m_value[2]) - (*(U32*)&operand.m_value[2]);

	
	if(m == 0)
		return true;

	return false;
	}

GPGINLINE BOOL GPGVector3::IsApproximately(const GPGVector3 &operand,
															F32 margin) const
	{
	if(	fabs(m_value[0]-operand.m_value[0])<margin &&
		fabs(m_value[1]-operand.m_value[1])<margin &&
		fabs(m_value[2]-operand.m_value[2])<margin )
			return true;

	return false;
	}

GPGINLINE GPGVector3 &GPGVector3::Add(const GPGVector3 &operand)
	{
	m_value[0]+=operand.m_value[0];
	m_value[1]+=operand.m_value[1];
	m_value[2]+=operand.m_value[2];

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::Add(const GPGVector3 &left,
													const GPGVector3 &right)
	{
	m_value[0]=left.m_value[0]+right.m_value[0];
	m_value[1]=left.m_value[1]+right.m_value[1];
	m_value[2]=left.m_value[2]+right.m_value[2];

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::AddScaled(const GPGVector3 &operand,
																	F32 scalar)
	{
	m_value[0]+=operand.m_value[0]*scalar;
	m_value[1]+=operand.m_value[1]*scalar;
	m_value[2]+=operand.m_value[2]*scalar;

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::Subtract(const GPGVector3 &operand)
	{
	m_value[0]-=operand.m_value[0];
	m_value[1]-=operand.m_value[1];
	m_value[2]-=operand.m_value[2];

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::Subtract(const GPGVector3 &left,
													const GPGVector3 &right)
	{
	m_value[0]=left.m_value[0]-right.m_value[0];
	m_value[1]=left.m_value[1]-right.m_value[1];
	m_value[2]=left.m_value[2]-right.m_value[2];

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::Multiply(const GPGVector3 &operand)
	{
	m_value[0]*=operand.m_value[0];
	m_value[1]*=operand.m_value[1];
	m_value[2]*=operand.m_value[2];

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::Multiply(const GPGVector3 &left,
													const GPGVector3 &right)
	{
	m_value[0]=left.m_value[0]*right.m_value[0];
	m_value[1]=left.m_value[1]*right.m_value[1];
	m_value[2]=left.m_value[2]*right.m_value[2];

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::Divide(const GPGVector3 &operand)
	{
	GPGASSERT(!operand.HasZeroComponent());

	if(operand.m_value[0]!=0.0f)
		m_value[0]/=operand.m_value[0];
	if(operand.m_value[1]!=0.0f)
		m_value[1]/=operand.m_value[1];
	if(operand.m_value[2]!=0.0f)
		m_value[2]/=operand.m_value[2];

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::Divide(const GPGVector3 &left,
													const GPGVector3 &right)
	{
	GPGASSERT(!right.HasZeroComponent());

	if(right.m_value[0]!=0.0f)
		m_value[0]=left.m_value[0]/right.m_value[0];
	if(right.m_value[1]!=0.0f)
		m_value[1]=left.m_value[1]/right.m_value[1];
	if(right.m_value[2]!=0.0f)
		m_value[2]=left.m_value[2]/right.m_value[2];

	return *this;
	}


GPGINLINE GPGVector3 &GPGVector3::CrossProduct(const GPGVector3 &left,
													const GPGVector3 &right)
	{
	GPGASSERT(this!=&left);
	GPGASSERT(this!=&right);
	m_value[0]=	left.m_value[1]*right.m_value[2]-
								left.m_value[2]*right.m_value[1];
	m_value[1]=	left.m_value[2]*right.m_value[0]-
								left.m_value[0]*right.m_value[2];
	m_value[2]=	left.m_value[0]*right.m_value[1]-
								left.m_value[1]*right.m_value[0];

	return *this;
	}

GPGINLINE F32 GPGVector3::DotProduct(const GPGVector3 &left,
													const GPGVector3 &right)
	{
	return	left.m_value[0]*right.m_value[0]+
			left.m_value[1]*right.m_value[1]+
			left.m_value[2]*right.m_value[2];
	}

GPGINLINE F32 GPGVector3::DotProduct(const GPGVector3 &operand) const
	{
	return	m_value[0]*operand.m_value[0]+
			m_value[1]*operand.m_value[1]+
			m_value[2]*operand.m_value[2];
	}

GPGINLINE GPGVector3 &GPGVector3::Scale(F32 scalar)
	{
	m_value[0]*=scalar;
	m_value[1]*=scalar;
	m_value[2]*=scalar;

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::Negate(void)
	{
	m_value[0]= -m_value[0];
	m_value[1]= -m_value[1];
	m_value[2]= -m_value[2];

	return *this;
	}

GPGINLINE GPGVector3 &GPGVector3::Reciprocate(void)
	{
	m_value[0]= (m_value[0]==0.0f)? 0.0f: 1.0f/m_value[0];
	m_value[1]= (m_value[1]==0.0f)? 0.0f: 1.0f/m_value[1];
	m_value[2]= (m_value[2]==0.0f)? 0.0f: 1.0f/m_value[2];

	return *this;
	}

GPGINLINE F32 GPGVector3::CalcMagnitude(void) const
	{
#if FALSE
	return ifxFastMagnitude(m_value);
#else
	return sqrtf(m_value[0]*m_value[0]+
					m_value[1]*m_value[1]+
					m_value[2]*m_value[2]);
#endif
	}

GPGINLINE F32 GPGVector3::CalcMagnitudeSquared(void) const
	{
	return m_value[0]*m_value[0]+m_value[1]*m_value[1]+m_value[2]*m_value[2];
	}

GPGINLINE F32 GPGVector3::CalcDistanceFrom(
											const GPGVector3 &operand) const
	{
	return sqrtf(CalcDistanceSquaredFrom(operand));
	}

GPGINLINE F32 GPGVector3::CalcDistanceSquaredFrom(
											const GPGVector3 &operand) const
	{
	F32 part=m_value[0]-operand.m_value[0];
	F32 sum=part*part;

	part=m_value[1]-operand.m_value[1];
	sum+=part*part;

	part=m_value[2]-operand.m_value[2];
	return sum+part*part;
	}

GPGINLINE F32 GPGVector3::CalcCosAngleFrom(const GPGVector3 &operand) const
	{
	return sqrtf(CalcCosAngleSquaredFrom(operand));
	}

GPGINLINE F32 GPGVector3::CalcCosAngleSquaredFrom(
											const GPGVector3 &operand) const
	{
	GPGASSERT(IsNonZero());
	GPGASSERT(operand.IsNonZero());

	F32 dot=DotProduct(operand);
	return ( (dot*dot) /
				(CalcMagnitudeSquared()*operand.CalcMagnitudeSquared()) );
	}

GPGINLINE GPGVector3 &GPGVector3::Normalize(void)
	{
	GPGASSERT(IsNonZero());
	return Scale(1.0f/CalcMagnitude());
	}

GPGINLINE GPGVector3 &GPGVector3::Rescale(F32 magnitude)
	{
	GPGASSERT(IsNonZero());
	return Scale(magnitude/CalcMagnitude());
	}

#endif /* __GPGVector3_h__ */
