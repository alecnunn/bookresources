
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

#ifndef __GPGQuaternion_h__
#define __GPGQuaternion_h__


#define GPGUSE_SLERP	TRUE		// use non-linear interpolation for quats
#define GPGSLERP_DELTA	(1e-6f)		// margin under which use linear always
#define GPGDELTA		(1e-4f)		// for equivelence comparisons
#define GPGALMOST1		(0.999999f)	// for trivial dot products




/******************************************************************************
class GPGQuaternion

	m_data[4]= w x y z	right-handed

	Reference:  Non-trivial operations related to quaternions draw heavily
		from Nick Bobic's GLquat source code and his Feb 98
		Game Developer article.

******************************************************************************/
class GPGQuaternion
	{
	public:

						GPGQuaternion(void)		{ };

		void			Set(F32 w,F32 x,F32 y,F32 z);
		void			MakeIdentity(void);
		void			MakeRotation(F32 radians,GPGAxisID axis);

						// axis/angle
		void			MakeRotation(F32 radians,const GPGVector3 &axis);

						// quaternion between NORMALIZED vectors
		void			MakeRotation(const GPGVector3 &from,
													const GPGVector3 &to);

						GPGQuaternion(F32 w,F32 x,F32 y,
																F32 z)
													{ Set(w,x,y,z); };
						GPGQuaternion(const float *floats)
													{ operator=(floats); };
						GPGQuaternion(const GPGVector3 &operand)
													{ operator=(operand); };
						GPGQuaternion(const GPGVector4 &operand)
													{ operator=(operand); };
						GPGQuaternion(const GPGQuaternion &operand)
													{ operator=(operand); };
						GPGQuaternion(const GPGMatrix4x4 &matrix)
													{ operator=(matrix); };

		GPGQuaternion	&operator=(const float *floats);
		GPGQuaternion	&operator=(const GPGQuaternion &operand);
		GPGQuaternion	&operator=(const GPGVector3 &operand);
		GPGQuaternion	&operator=(const GPGVector4 &operand);

		bool			operator==(const GPGQuaternion &operand) const;

		bool			IsApproximately(const GPGQuaternion &operand,
													F32 margin) const;

static
const	GPGQuaternion	&Identity(void);

		F32				&operator[](const int x)	{ return m_data[x]; };
		F32				*Raw(void)					{ return m_data; };
const	F32				*RawConst(void) const		{ return m_data; };

		void			ComputeAngleAxis(F32 &radians,
													GPGVector3 &axis) const;

		GPGQuaternion	&Invert(const GPGQuaternion &operand);
		GPGQuaternion	&Invert(void);
		F32				CalcMagnitudeSquared(void);
		bool			IsValid(void);
		void			Normalize(void);

		GPGQuaternion	&Multiply(const GPGQuaternion &left,
												const GPGQuaternion &right);
		void			Multiply(const GPGQuaternion &operand)
													{ Rotate(operand); };
		void			Rotate(const GPGQuaternion &operand);
		void			Rotate(F32 radians,GPGAxisID axis);
						/// preferred
		void			RotateVector(const GPGVector3 &input,
														GPGVector3 &output);
		void			RotateVector(const F32 *input,F32 *output);

		void			ScaleAngle(F32 scale);

		void			ForcePositiveW(void);

	// not inline:
		GPGQuaternion	&operator=(const GPGMatrix4x4 &operand);

		void			Interpolate(F32 t,const GPGQuaternion &from,
											const GPGQuaternion &to);

						// computes and removes from quat
		F32				ExtractEulerZ(void);

#if GPGALLOW_STREAMS
		std::ostream	&Print(std::ostream &out,long indention) const;
#endif
		GPGString		Out(bool verbose=false) const;

	private:
		F32				m_data[4];
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGQuaternion &quat);
#endif


GPGINLINE void GPGQuaternion::Set(F32 w,F32 x,F32 y,F32 z)
	{
	m_data[0]=w;
	m_data[1]=x;
	m_data[2]=y;
	m_data[3]=z;
	}

GPGINLINE void GPGQuaternion::MakeIdentity(void)
	{
	m_data[0]=1.0f;
	m_data[1]=0.0f;
	m_data[2]=0.0f;
	m_data[3]=0.0f;
	}

// static
GPGINLINE const GPGQuaternion &GPGQuaternion::Identity(void)
	{
	static const GPGQuaternion identity(1.0f,0.0f,0.0f,0.0f);
	return identity;
	}

GPGINLINE bool GPGQuaternion::IsApproximately(const GPGQuaternion &operand,
													F32 margin) const
	{
	if( fabs(m_data[0]-operand.m_data[0])<margin &&
		fabs(m_data[1]-operand.m_data[1])<margin &&
		fabs(m_data[2]-operand.m_data[2])<margin &&
		fabs(m_data[3]-operand.m_data[3])<margin )
			return true;

	return false;
	}

GPGINLINE void GPGQuaternion::ForcePositiveW(void)
	{
	// negative W is the alternate twin, so X,Y,Z may need to be inverted
	if(m_data[0]<0)
		{
		m_data[0]= -m_data[0];
		m_data[1]= -m_data[1];
		m_data[2]= -m_data[2];
		m_data[3]= -m_data[3];
		}
	}

GPGINLINE void GPGQuaternion::MakeRotation(F32 radians,GPGAxisID axis)
	{
//	GPGASSERT(radians>= -(GPGPI+0.0001f));
//	GPGASSERT(radians<=  (GPGPI+0.0001f));

	F32 r2=radians*0.5f;

	m_data[0]=GPGCOS(r2);
	m_data[1]=0.0f;
	m_data[2]=0.0f;
	m_data[3]=0.0f;
	m_data[1+axis]=GPGSIN(r2);
	}

GPGINLINE void GPGQuaternion::MakeRotation(F32 radians,const GPGVector3 &axis)
	{
//	GPGASSERT(radians>= -(GPGPI+0.0001f));
//	GPGASSERT(radians<=  (GPGPI+0.0001f));

	const F32 *v=axis.RawConst();
	if(fabs(v[0])+fabs(v[1])+fabs(v[2])<GPGDELTA)
		{
		MakeIdentity();
		return;
		}

	F32 r2=radians*0.5f;
	GPGVector3 temp=axis;

	temp.Normalize();
	temp.Scale(GPGSIN(r2));

	operator=(temp);
	m_data[0]=GPGCOS(r2);
	}

GPGINLINE void GPGQuaternion::MakeRotation(const GPGVector3 &from,
													const GPGVector3 &to)
	{
	//* NOTE: assumes normalized input

	const F32 *v1=from.RawConst();
	const F32 *v2=to.RawConst();

	F32 tx,ty,tz,temp,dist;
	F32 cost,len,ss;

	// get dot product of two vectors
	cost=v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];

	// check if parallel
	if (cost > GPGALMOST1)
		{
		MakeIdentity();
		return;
		}
	else if (cost < -GPGALMOST1)
		{
		// check if opposite

		// check if we can use cross product of from vector with [1, 0, 0]
		tx=0.0f;
		ty=v1[0];
		tz= -v1[1];

		len=GPGSQRT(ty*ty + tz*tz);

		if (len < GPGSLERP_DELTA)
			{
			// no, we need cross product of from vector with [0, 1, 0]
			tx= -v1[2];
			ty=0.0f;
			tz=v1[0];
			}

		// normalize
		temp=tx*tx + ty*ty + tz*tz;

		dist=(F32)(1.0f/GPGSQRT(temp));

		tx*=dist;
		ty*=dist;
		tz*=dist;

		Set(0.0f,tx,ty,tz);

		return;
		}

	// else just cross two vectors

	tx=v1[1]*v2[2] - v1[2]*v2[1];
	ty=v1[2]*v2[0] - v1[0]*v2[2];
	tz=v1[0]*v2[1] - v1[1]*v2[0];

	temp=tx*tx + ty*ty + tz*tz;
	dist=1.0f/GPGSQRT(temp);

	tx*=dist;
	ty*=dist;
	tz*=dist;

	// use half-angle formula (sin^2 t=( 1 - cos(2t) )/2)

	ss=GPGSQRT(0.5f * (1.0f - cost));

	tx*=ss;
	ty*=ss;
	tz*=ss;

	// cos^2 t=( 1 + cos (2t) ) / 2
	// w part is cosine of half the rotation angle

	Set(GPGSQRT(0.5f * (1.0f + cost)),tx,ty,tz);
	}



GPGINLINE GPGQuaternion &GPGQuaternion::operator=(const GPGQuaternion &operand)
	{
	m_data[0]=operand.m_data[0];
	m_data[1]=operand.m_data[1];
	m_data[2]=operand.m_data[2];
	m_data[3]=operand.m_data[3];

	return *this;
	}

GPGINLINE GPGQuaternion &GPGQuaternion::operator=(const float *floats)
	{
	m_data[0]=floats[0];
	m_data[1]=floats[1];
	m_data[2]=floats[2];
	m_data[3]=floats[3];

	return *this;
	}

GPGINLINE GPGQuaternion &GPGQuaternion::operator=(const GPGVector3 &operand)
	{
	const F32 *v=operand.RawConst();

	m_data[0]=0.0f;
	m_data[1]=v[0];
	m_data[2]=v[1];
	m_data[3]=v[2];

	return *this;
	}

GPGINLINE GPGQuaternion &GPGQuaternion::operator=(const GPGVector4 &operand)
	{
	const F32 *v=operand.RawConst();

	m_data[0]=v[0];
	m_data[1]=v[1];
	m_data[2]=v[2];
	m_data[3]=v[3];

	return *this;
	}

GPGINLINE bool GPGQuaternion::operator==(const GPGQuaternion &operand) const
	{
	return (	(m_data[0]-operand.m_data[0]<GPGDELTA)	&&
				(m_data[1]-operand.m_data[1]<GPGDELTA)	&&
				(m_data[2]-operand.m_data[2]<GPGDELTA)	&&
				(m_data[3]-operand.m_data[3]<GPGDELTA)	);
	}

GPGINLINE void GPGQuaternion::Rotate(const GPGQuaternion &operand)
	{
	GPGQuaternion temp=*this;		// copy

	Multiply(temp,operand);
	}

GPGINLINE void GPGQuaternion::Rotate(F32 radians,GPGAxisID axis)
	{
	GPGQuaternion rot;
	rot.MakeRotation(radians,axis);

	Rotate(rot);
	}

GPGINLINE void GPGQuaternion::ComputeAngleAxis(F32 &radians,
													GPGVector3 &axis) const
	{
//	F32 len=m_data[1]*m_data[1] + m_data[2]*m_data[2] + m_data[3]*m_data[3];

	F32 len=m_data[1];
	len*=m_data[1];
	F32 len2=m_data[2];
	len2*=m_data[2];
	len+=len2;
	F32 len3=m_data[3];
	len3*=m_data[3];
	len+=len3;

//	if(len<GPGSLERP_DELTA || m_data[0]>(1.0f-GPGSLERP_DELTA))
	if(len==0.0f)
		{
		axis.Set(0.0f,0.0f,1.0f);
		radians=0.0f;
		return;
		}

	// NOTE example code didn't sqrt (why?)

	F32 inv=1.0f/GPGSQRT(len);
	if(m_data[0]<0.0f)
		inv= -inv;

	axis[0]=m_data[1]*inv;
	axis[1]=m_data[2]*inv;
	axis[2]=m_data[3]*inv;

	radians=2.0f*GPGACOS((F32)fabs(m_data[0]));
	}

GPGINLINE void GPGQuaternion::ScaleAngle(F32 scale)
	{
	F32 angle;
	GPGVector3 axis;

	ComputeAngleAxis(angle,axis);
	MakeRotation(angle*scale,axis);
	}

GPGINLINE GPGQuaternion &GPGQuaternion::Invert(const GPGQuaternion &operand)
	{
	GPGASSERT(this!=&operand);
	const F32 *q=operand.RawConst();

#if FALSE
	F32 norm,invNorm;

	norm=q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3];
	invNorm= 1.0f/norm;

	m_data[0]=  q[0]*invNorm;
	m_data[1]= -q[1]*invNorm;
	m_data[2]= -q[2]*invNorm;
	m_data[3]= -q[3]*invNorm;
#else
	m_data[0]=  q[0];
	m_data[1]= -q[1];
	m_data[2]= -q[2];
	m_data[3]= -q[3];
#endif
	return *this;
	}

GPGINLINE GPGQuaternion &GPGQuaternion::Invert(void)
	{
	m_data[1]= -m_data[1];
	m_data[2]= -m_data[2];
	m_data[3]= -m_data[3];
	return *this;
	}

GPGINLINE F32 GPGQuaternion::CalcMagnitudeSquared(void)
	{
	return m_data[0]*m_data[0]+m_data[1]*m_data[1]+m_data[2]*m_data[2]+m_data[3]*m_data[3];
	}

GPGINLINE bool GPGQuaternion::IsValid(void)
	{
	F32 mag2=CalcMagnitudeSquared();
	return (mag2>0.99f && mag2<1.01f);
	}

GPGINLINE void GPGQuaternion::Normalize(void)
	{
	F32 invDist,square=CalcMagnitudeSquared();
	if(square==0.0f)
		{
		MakeIdentity();
		return;
		}

	invDist= 1.0f/GPGSQRT(square);

	m_data[0]*=invDist;
	m_data[1]*=invDist;
	m_data[2]*=invDist;
	m_data[3]*=invDist;
	}

// vector-specific version assuming w=0 (in and out) for speed
// this: 24mult,17add  quat_mult*2: 32mult,24add	matrix3x3_mult:9mult,6add
GPGINLINE void GPGQuaternion::RotateVector(
								const GPGVector3 &input,GPGVector3 &output)
	{
	RotateVector(input.RawConst(),output.Raw());
	}
GPGINLINE void GPGQuaternion::RotateVector(const F32 *in,F32 *out)
	{
	F32 mid[4];

	mid[0]= -m_data[1]*in[0]-m_data[2]*in[1]-m_data[3]*in[2];
	mid[1]=  m_data[0]*in[0]+m_data[2]*in[2]-m_data[3]*in[1];
	mid[2]=  m_data[0]*in[1]+m_data[3]*in[0]-m_data[1]*in[2];
	mid[3]=  m_data[0]*in[2]+m_data[1]*in[1]-m_data[2]*in[0];

#if FALSE
	GPGQuaternion inverse;
	inverse.Invert(*this);

	const F32 *inv=inverse.RawConst();

	out[0]=mid[0]*inv[1]+mid[1]*inv[0]+mid[2]*inv[3]-mid[3]*inv[2];
	out[1]=mid[0]*inv[2]+mid[2]*inv[0]+mid[3]*inv[1]-mid[1]*inv[3];
	out[2]=mid[0]*inv[3]+mid[3]*inv[0]+mid[1]*inv[2]-mid[2]*inv[1];
#else
	out[0]=-mid[0]*m_data[1]+mid[1]*m_data[0]-mid[2]*m_data[3]+mid[3]*m_data[2];
	out[1]=-mid[0]*m_data[2]+mid[2]*m_data[0]-mid[3]*m_data[1]+mid[1]*m_data[3];
	out[2]=-mid[0]*m_data[3]+mid[3]*m_data[0]-mid[1]*m_data[2]+mid[2]*m_data[1];
#endif
	}

GPGINLINE GPGQuaternion &GPGQuaternion::Multiply(const GPGQuaternion &left,
												const GPGQuaternion &right)
	{
	GPGASSERT(this!=&left);
	GPGASSERT(this!=&right);
	const F32 *a=left.RawConst();
	const F32 *b=right.RawConst();

#if TRUE
	// this: 16mult,12add  matrix3x3:27mult,18add
	m_data[1]=a[0]*b[1]+a[1]*b[0]+a[2]*b[3]-a[3]*b[2];
	m_data[2]=a[0]*b[2]+a[2]*b[0]+a[3]*b[1]-a[1]*b[3];
	m_data[3]=a[0]*b[3]+a[3]*b[0]+a[1]*b[2]-a[2]*b[1];
	m_data[0]=a[0]*b[0]-a[1]*b[1]-a[2]*b[2]-a[3]*b[3];
#else
	// WARNING untested
	// this: 12mult,35add  matrix3x3:27mult,18add
	F32 A=(a[0]+a[1])*(b[0]+b[1]);
	F32 B=(a[3]-a[2])*(b[2]-b[3]);
	F32 C=(a[1]-a[0])*(b[2]-b[3]);
	F32 D=(a[2]+a[3])*(b[1]-b[0]);
	F32 E=(a[1]+a[3])*(b[1]+b[2]);
	F32 F=(a[1]-a[3])*(b[1]-b[2]);
	F32 G=(a[0]+a[2])*(b[0]-b[3]);
	F32 H=(a[0]-a[2])*(b[0]+b[3]);

	m_data[0]=B+(-E-F+G+H)*0.5f;
	m_data[1]=A-(E+F+G+H)*0.5f;
	m_data[2]= -C+(E-F+G-H)*0.5f;
	m_data[3]= -D+(E-F-G+H)*0.5f;
#endif


	//* NOTE we use this mult for vector xforms, so DON'T NORMALIZE
//	Normalize();

	return *this;
	}

#endif /* __GPGQuaternion_h__ */
