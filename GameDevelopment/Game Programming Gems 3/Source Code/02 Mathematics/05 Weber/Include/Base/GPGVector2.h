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
//    Interface for a 2-dimensional vector of floats
//

#ifndef GPGVector2_h
#define GPGVector2_h

#include "GPGDebug.h"
#include "GPGDataTypes.h"


class GPGVector2
{
public:
	// Set the u and v components
	void Set(F32 u, F32 v);
	
	F32& X();   // Get or set component 0 (X component)
	F32& Y();	// Get or set component 1 (Y component)	

    F32& U();   // Get or set component 0 (U component)
	F32& V();   // Get or set component 1 (V component)

	// Get or set values using index in the range 0..1
	F32& Value(U32 index);

    GPGVector2(void)			{ };
	GPGVector2(F32 x,F32 y) { Set(x,y); };

	// 2 floats assumed
	GPGVector2(const float *floats) { operator=(floats); };
	GPGVector2(const GPGVector2 &operand) { operator=(operand); };

	void			Reset(void);
	GPGVector2		&operator=(const float *floats);
	GPGVector2		&operator=(const GPGVector2 &operand);
	bool			operator==(const GPGVector2 &operand) const;
	bool			IsApproximately(const GPGVector2 &operand,
													F32 margin) const;

		F32		*Raw(void)					{ return m_value; };
const	F32		*RawConst(void) const		{ return m_value; };
		F32		&operator[](const int n)	{ GPGASSERT(n < 2); return m_value[n]; };

		GPGVector2	&Add(const GPGVector2 &operand);
		GPGVector2	&Add(const GPGVector2 &left, const GPGVector2 &right);

		GPGVector2	&Subtract(const GPGVector2 &operand);
		GPGVector2	&Subtract(const GPGVector2 &left, const GPGVector2 &right);

		GPGVector2	&Multiply(const GPGVector2 &operand);
		GPGVector2	&Multiply(const GPGVector2 &left, const GPGVector2 &right);

		GPGVector2	&Divide(const GPGVector2 &operand);
		GPGVector2	&Divide(const GPGVector2 &left, const GPGVector2 &right);

		GPGVector2	&Scale(F32 scalar);
						
        // Rescale = Normalize then Scale
		GPGVector2	&Rescale(F32 magnitude);
		GPGVector2	&Invert(void);
		F32		    DotProduct(const GPGVector2 &operand);
static	F32		    DotProduct(const GPGVector2 &left, const GPGVector2 &right);
		
		GPGVector2	&Normalize(void);
		F32		    CalcMagnitude(void);
		F32		    CalcMagnitudeSquared(void);	// faster!

private:
	F32 m_value[2];
};




inline void GPGVector2::Set(F32 u, F32 v)
{
	m_value[0] = u;
	m_value[1] = v;
}

inline F32& GPGVector2::X()
{
	return m_value[0];
}

inline F32& GPGVector2::Y()
{
	return m_value[1];
}

inline F32& GPGVector2::U()
{
	return m_value[0];
}

inline F32& GPGVector2::V()
{
	return m_value[1];
}

inline F32& GPGVector2::Value(U32 index)
{
	GPGASSERT(index < 2);
	return m_value[index];
}







inline void GPGVector2::Reset(void)
	{
	m_value[0]=0.0f;
	m_value[1]=0.0f;
	}


inline GPGVector2 &GPGVector2::operator=(const float *floats)
	{
	// 3 floats assumed

	m_value[0]=floats[0];
	m_value[1]=floats[1];

	return *this;
	}

inline GPGVector2 &GPGVector2::operator=(const GPGVector2 &operand)
	{
	m_value[0]=operand.m_value[0];
	m_value[1]=operand.m_value[1];

	return *this;
	}

inline bool GPGVector2::operator==(const GPGVector2 &operand) const
	{
	// DJ 8/14/00 - unrolled to reduce branch count to 1.
	U32 m = (*(U32*)&m_value[0]) - (*(U32*)&operand.m_value[0]);
	m |= (*(U32*)&m_value[1]) - (*(U32*)&operand.m_value[1]);

	
	if(m == 0)
		return true;

	return false;
	}

inline bool GPGVector2::IsApproximately(const GPGVector2 &operand,
													F32 margin) const
	{
	if(	fabs(m_value[0]-operand.m_value[0])<margin &&
		fabs(m_value[1]-operand.m_value[1])<margin)
		return true;

	return false;
	}

inline GPGVector2 &GPGVector2::Add(const GPGVector2 &operand)
	{
	m_value[0]+=operand.m_value[0];
	m_value[1]+=operand.m_value[1];
	
	return *this;
	}

inline GPGVector2 &GPGVector2::Add(const GPGVector2 &left,
													const GPGVector2 &right)
	{
	m_value[0]=left.m_value[0]+right.m_value[0];
	m_value[1]=left.m_value[1]+right.m_value[1];

	return *this;
	}

inline GPGVector2 &GPGVector2::Subtract(const GPGVector2 &operand)
	{
	m_value[0]-=operand.m_value[0];
	m_value[1]-=operand.m_value[1];

	return *this;
	}

inline GPGVector2 &GPGVector2::Subtract(const GPGVector2 &left,
													const GPGVector2 &right)
	{
	m_value[0]=left.m_value[0]-right.m_value[0];
	m_value[1]=left.m_value[1]-right.m_value[1];

	return *this;
	}

inline GPGVector2 &GPGVector2::Multiply(const GPGVector2 &operand)
	{
	m_value[0]*=operand.m_value[0];
	m_value[1]*=operand.m_value[1];

	return *this;
	}

inline GPGVector2 &GPGVector2::Multiply(const GPGVector2 &left,
													const GPGVector2 &right)
	{
	m_value[0]=left.m_value[0]*right.m_value[0];
	m_value[1]=left.m_value[1]*right.m_value[1];

	return *this;
	}

inline GPGVector2 &GPGVector2::Divide(const GPGVector2 &operand)
	{
	GPGASSERT(operand.m_value[0] != 0.0f);
	GPGASSERT(operand.m_value[1] != 0.0f);

	m_value[0]/=operand.m_value[0];
	m_value[1]/=operand.m_value[1];

	return *this;
	}

inline GPGVector2 &GPGVector2::Divide(const GPGVector2 &left,
													const GPGVector2 &right)
	{
	GPGASSERT(right.m_value[0] != 0.0f);
	GPGASSERT(right.m_value[1] != 0.0f);

	m_value[0]=left.m_value[0]/right.m_value[0];
	m_value[1]=left.m_value[1]/right.m_value[1];

	return *this;
	}


// static
inline F32 GPGVector2::DotProduct(const GPGVector2 &left,
												const GPGVector2 &right)
	{
	return	left.m_value[0]*right.m_value[0]+
			left.m_value[1]*right.m_value[1];
	}

inline F32 GPGVector2::DotProduct(const GPGVector2 &operand)
	{
	return	m_value[0]*operand.m_value[0]+
			m_value[1]*operand.m_value[1];
	}

inline GPGVector2 &GPGVector2::Scale(F32 scalar)
	{
	m_value[0]*=scalar;
	m_value[1]*=scalar;

	return *this;
	}

inline GPGVector2 &GPGVector2::Invert(void)
	{
	m_value[0]= -m_value[0];
	m_value[1]= -m_value[1];

	return *this;
	}

inline F32 GPGVector2::CalcMagnitude(void)
	{
#if 0
	return ifxFastMagnitude(m_value);
#else
	// Note: sqrtf calls sqrt
	return (float)sqrt(m_value[0]*m_value[0]+m_value[1]*m_value[1]);
#endif
	}

inline F32 GPGVector2::CalcMagnitudeSquared(void)
	{
	return m_value[0]*m_value[0]+m_value[1]*m_value[1];
	}

inline GPGVector2 &GPGVector2::Normalize(void)
	{
	return Scale(1.0f/CalcMagnitude());
	}

inline GPGVector2 &GPGVector2::Rescale(F32 magnitude)
	{
	return Scale(magnitude/CalcMagnitude());
	}


#endif
