
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

#ifndef __GPGMatrix_h__
#define __GPGMatrix_h__



#define	GPGNEARZERO			(1e-36f)	// allows scale of 1/3 the exponent

enum GPGAxisID				{GPG_X_AXIS,GPG_Y_AXIS,GPG_Z_AXIS};



/******************************************************************************
class GPGMatrix4x4

	Priorities:
		1. speed, not elegance (many loops unrolled)
		2. direct use in OpenGL

	Column major (x,y) where
		00	10	20	30
		01	11	21	31
		02	12	22	32
		03	13	23	33
	is stored as
		0	4	8	12
		1	5	9	13
		2	6	10	14
		3	7	11	15
	representing
		Rot	Rot	Rot	Tx
		Rot	Rot	Rot	Ty
		Rot	Rot	Rot	Tz
		0	0	0	1

	NOTE the bottom row was previously refered to as shear, which is incorrect.
	The bottom row really just contributes to a fourth vector element and
	has no shearing effect on the 3 primary vector elements.

	The fourth row is commonly used for a distance element in a
	perspective calcuation.

	NOTE Functions named with "3x4" treat the matrix as an affine 3x4 transform.

******************************************************************************/
class GPGMatrix4x4
	{
	public:
						GPGMatrix4x4(void)
							{
							// no default clear
							};

						GPGMatrix4x4(const float *pFloats)
							{
							// 16 floats assumed
							operator=(pFloats);
							};

						GPGMatrix4x4(const GPGMatrix4x4 &operand)
							{
							operator=(operand);
							};

						GPGMatrix4x4(const GPGQuaternion &quat)
							{
							ResetNonRotation();
							operator=(quat);
							}

						GPGMatrix4x4(const GPGVector3 &x,const GPGVector3 &y,
									const GPGVector3 &z,const GPGVector3 &t)
							{
							ResetNonRotation();
							Set(x,y,z,t);
							};

		void			Reset(void)
							{
							MakeIdentity();
							}
		GPGMatrix4x4	&MakeTensor(const GPGVector3 &left,
													const GPGVector3 &right);
		GPGMatrix4x4	&MakeIdentity(void);
		void			ResetNonRotation(void)
							{
							// resets data except 3x3 rotation
							ResetTranslation();
							ResetBottom();
							};

		void			ResetTranslation(void);
		void			ResetBottom(void);

		GPGMatrix4x4	&Set(const GPGVector3 &x,const GPGVector3 &y,
									const GPGVector3 &z,const GPGVector3 &t);
		GPGMatrix4x4	&operator=(const float *pFloats);
		GPGMatrix4x4	&CopyMatrix(const float ppFloats[4][4]);
		GPGMatrix4x4	&operator=(const GPGMatrix4x4 &operand);
		bool			operator==(const GPGMatrix4x4 &operand) const;

						// Raw() allows direct use in OpenGL
		F32				*Raw(void)					{ return m_data; };
const	F32				*RawConst(void) const		{ return m_data; };
		F32				&operator[](int n)			{ return m_data[n]; };

		void			SetTranslation(const GPGVector3 &vector)
							{
							const F32 *v=vector.RawConst();
							m_data[12]=v[0];
							m_data[13]=v[1];
							m_data[14]=v[2];
							}
		void			CopyTranslation(GPGVector3 &vector) const
							{ vector.Set(m_data[12],m_data[13],m_data[14]); };

		GPGMatrix4x4	&Translate3x4(const GPGVector3 &vector);
		GPGMatrix4x4	&Rotate3x4(F32 radians,GPGAxisID axis);
		GPGMatrix4x4	&Scale3x4(const GPGVector3 &vector);
		GPGMatrix4x4	&Scale(const GPGVector3 &vector);

		/// full algebraic 4x4 multiply
		GPGMatrix4x4	&Multiply(const GPGMatrix4x4 &left,
												const GPGMatrix4x4 &right);
		/// 3x4 transform-optimised reduced-calculation multiply
		GPGMatrix4x4	&Multiply3x4(const GPGMatrix4x4 &left,
												const GPGMatrix4x4 &right);

		/// full algebraic 4x4 inversion
		//* TODO fix: Invert() frequently has incorrect results
		GPGRESULT		Invert(const GPGMatrix4x4 &operand);

		/// Invert the affine 3x4 transform
		GPGRESULT		Invert3x4(const GPGMatrix4x4 &operand);

		/// full 4x4 determinant
		F32				CalcDeterminant(void) const;

		/** Find the determinant the affine 3x3 transform
				(translation does not contribute) */
		F32				CalcDeterminant3x3(void) const;
static	F32				CalcDeterminant3x3(F32 a1,F32 a2,F32 a3,
											F32 b1,F32 b2,F32 b3,
											F32 c1,F32 c2,F32 c3);

		void			TransformVector(const GPGVector3 &source,
													GPGVector3 &result) const;
		void			RotateVector(const GPGVector3 &source,
													GPGVector3 &result) const;
		void			ProjectVector(const GPGVector3 &source,
													GPGVector3 &result) const;

	// not inline:
		/// Scale signs are determined by quaternion conversion.
		GPGRESULT		CalcTRS(GPGVector3 *pTranslation,
							GPGQuaternion *pRotation,GPGVector3 *pScale) const;

#if FALSE
		/// Scale signs are determined by quaternion conversion.
		void			ExtractScale(GPGVector3 &scale,
											GPGQuaternion &pResult) const;
#endif
		/// Scale of unit axis vectors 
		void			CalcAxisScale(GPGVector3 &scale) const;

		/// round near-integer
		void			ApplyThreshold(F32 threshold);

		GPGMatrix4x4	&operator=(const GPGQuaternion &quat);

#if GPGALLOW_STREAMS
		std::ostream	&Print(std::ostream &out,long indention) const;
#endif

		GPGString		Out(bool verbose=false) const;


	protected:
		F32		m_data[16];

	private:
		GPGRESULT		Decompose(GPGVector3 &translation,
								GPGMatrix4x4 &rotation,
								GPGVector3 &scale) const;
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGMatrix4x4 &matrix);
#endif



GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::MakeIdentity(void)
	{
	// identity
	m_data[0]=1.0;
	m_data[1]=0.0;
	m_data[2]=0.0;
	m_data[3]=0.0;

	m_data[4]=0.0;
	m_data[5]=1.0;
	m_data[6]=0.0;
	m_data[7]=0.0;

	m_data[8]=0.0;
	m_data[9]=0.0;
	m_data[10]=1.0;
	m_data[11]=0.0;

	m_data[12]=0.0;
	m_data[13]=0.0;
	m_data[14]=0.0;
	m_data[15]=1.0;
	return *this;
	}
GPGINLINE void GPGMatrix4x4::ResetTranslation(void)
	{
	m_data[12]=0.0;
	m_data[13]=0.0;
	m_data[14]=0.0;
	}

GPGINLINE void GPGMatrix4x4::ResetBottom(void)
	{
	m_data[3]=0.0;
	m_data[7]=0.0;
	m_data[11]=0.0;
	m_data[15]=1.0;
	}

GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::MakeTensor(const GPGVector3 &left,
													const GPGVector3 &right)
{
	/// \todo check equations and test results

	const F32 *v1=left.RawConst();
	const F32 *v2=right.RawConst();

	m_data[0]=	v1[0]*v2[0];
	m_data[1]=	v1[1]*v2[0];
	m_data[2]=	v1[2]*v2[0];
	m_data[4]=	v1[0]*v2[1];
	m_data[5]=	v1[1]*v2[1];
	m_data[6]=	v1[2]*v2[1];
	m_data[8]=	v1[0]*v2[2];
	m_data[9]=	v1[1]*v2[2];
	m_data[10]=	v1[2]*v2[2];
	ResetNonRotation();
	return *this;
}

GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::operator=(const float *pFloats)
	{
	// 16 floats assumed

	long m;
	for(m=0;m<16;m++)
		m_data[m]=pFloats[m];

	return *this;
	}

GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::CopyMatrix(const float ppFloats[4][4])
	{
	long m,n,mn;;
	for(m=0;m<4;m++)
		{
		mn=m*4;
		for(n=0;n<4;n++)
			m_data[mn+n]=ppFloats[m][n];
		}
	return *this;
	}

GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::operator=(const GPGMatrix4x4 &operand)
	{
	long m;
	for(m=0;m<16;m++)
		m_data[m]=operand.m_data[m];

	return *this;
	}


GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::Set(const GPGVector3 &x,
				const GPGVector3 &y,const GPGVector3 &z,const GPGVector3 &t)
	{
	const F32 *v=x.RawConst();
	m_data[0 ]=v[0];
	m_data[1 ]=v[1];
	m_data[2 ]=v[2];

	v=y.RawConst();
	m_data[4 ]=v[0];
	m_data[5 ]=v[1];
	m_data[6 ]=v[2];

	v=z.RawConst();
	m_data[8 ]=v[0];
	m_data[9 ]=v[1];
	m_data[10]=v[2];

	v=t.RawConst();
	m_data[12]=v[0];
	m_data[13]=v[1];
	m_data[14]=v[2];

	return *this;
	}


GPGINLINE bool GPGMatrix4x4::operator==(const GPGMatrix4x4 &operand) const
	{
#if FALSE
	long m;
	for(m=0;m<16;m++)
		if(m_data[m]!=operand.m_data[m])
			return false;

	return true;
#else
	U32 m;
	m  = (*(U32*)&m_data[0 ]) - (*(U32*)&operand.m_data[0 ]);
	m |= (*(U32*)&m_data[1 ]) - (*(U32*)&operand.m_data[1 ]);
	m |= (*(U32*)&m_data[2 ]) - (*(U32*)&operand.m_data[2 ]);
	m |= (*(U32*)&m_data[3 ]) - (*(U32*)&operand.m_data[3 ]);

	m |= (*(U32*)&m_data[4 ]) - (*(U32*)&operand.m_data[4 ]);
	m |= (*(U32*)&m_data[5 ]) - (*(U32*)&operand.m_data[5 ]);
	m |= (*(U32*)&m_data[6 ]) - (*(U32*)&operand.m_data[6 ]);
	m |= (*(U32*)&m_data[7 ]) - (*(U32*)&operand.m_data[7 ]);

	m |= (*(U32*)&m_data[8 ]) - (*(U32*)&operand.m_data[8 ]);
	m |= (*(U32*)&m_data[9 ]) - (*(U32*)&operand.m_data[9 ]);
	m |= (*(U32*)&m_data[10]) - (*(U32*)&operand.m_data[10]);
	m |= (*(U32*)&m_data[11]) - (*(U32*)&operand.m_data[11]);

	m |= (*(U32*)&m_data[12]) - (*(U32*)&operand.m_data[12]);
	m |= (*(U32*)&m_data[13]) - (*(U32*)&operand.m_data[13]);
	m |= (*(U32*)&m_data[14]) - (*(U32*)&operand.m_data[14]);
	m |= (*(U32*)&m_data[15]) - (*(U32*)&operand.m_data[15]);

	if(m == 0)
		return true;

	return false;
#endif
	}

GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::Multiply(const GPGMatrix4x4 &left,
												const GPGMatrix4x4 &right)
	{
	GPGASSERT(this!=&left);
	GPGASSERT(this!=&right);
	// [i][j]=sum [i][k]*[k][j] for all k

#if FALSE
	long x,y,n;
	for(x=0;x<4;x++)
		for(y=0;y<4;y++)
			{
			m_data[x*4+y]=0.0f;

			for(n=0;n<4;n++)
				m_data[x*4+y]+=left.m_data[x*4+n]*right.m_data[n*4+y];
			}
#else
	const F32 *m2=left.RawConst();
	const F32 *m1=right.RawConst();

	m_data[ 0]=m1[ 0]*m2[ 0]+m1[ 1]*m2[ 4]+m1[ 2]*m2[ 8]+m1[ 3]*m2[12];
	m_data[ 1]=m1[ 0]*m2[ 1]+m1[ 1]*m2[ 5]+m1[ 2]*m2[ 9]+m1[ 3]*m2[13];
	m_data[ 2]=m1[ 0]*m2[ 2]+m1[ 1]*m2[ 6]+m1[ 2]*m2[10]+m1[ 3]*m2[14];
	m_data[ 3]=m1[ 0]*m2[ 3]+m1[ 1]*m2[ 7]+m1[ 2]*m2[11]+m1[ 3]*m2[15];
	m_data[ 4]=m1[ 4]*m2[ 0]+m1[ 5]*m2[ 4]+m1[ 6]*m2[ 8]+m1[ 7]*m2[12];
	m_data[ 5]=m1[ 4]*m2[ 1]+m1[ 5]*m2[ 5]+m1[ 6]*m2[ 9]+m1[ 7]*m2[13];
	m_data[ 6]=m1[ 4]*m2[ 2]+m1[ 5]*m2[ 6]+m1[ 6]*m2[10]+m1[ 7]*m2[14];
	m_data[ 7]=m1[ 4]*m2[ 3]+m1[ 5]*m2[ 7]+m1[ 6]*m2[11]+m1[ 7]*m2[15];
	m_data[ 8]=m1[ 8]*m2[ 0]+m1[ 9]*m2[ 4]+m1[10]*m2[ 8]+m1[11]*m2[12];
	m_data[ 9]=m1[ 8]*m2[ 1]+m1[ 9]*m2[ 5]+m1[10]*m2[ 9]+m1[11]*m2[13];
	m_data[10]=m1[ 8]*m2[ 2]+m1[ 9]*m2[ 6]+m1[10]*m2[10]+m1[11]*m2[14];
	m_data[11]=m1[ 8]*m2[ 3]+m1[ 9]*m2[ 7]+m1[10]*m2[11]+m1[11]*m2[15];
	m_data[12]=m1[12]*m2[ 0]+m1[13]*m2[ 4]+m1[14]*m2[ 8]+m1[15]*m2[12];
	m_data[13]=m1[12]*m2[ 1]+m1[13]*m2[ 5]+m1[14]*m2[ 9]+m1[15]*m2[13];
	m_data[14]=m1[12]*m2[ 2]+m1[13]*m2[ 6]+m1[14]*m2[10]+m1[15]*m2[14];
	m_data[15]=m1[12]*m2[ 3]+m1[13]*m2[ 7]+m1[14]*m2[11]+m1[15]*m2[15];
#endif

	return *this;
	}

GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::Multiply3x4(const GPGMatrix4x4 &left,
												const GPGMatrix4x4 &right)
	{
	GPGASSERT(this!=&left);
	GPGASSERT(this!=&right);
	// [i][j]=sum [i][k]*[k][j] for all k

	const F32 *m2=left.RawConst();
	const F32 *m1=right.RawConst();

	// reduced for transform-specific usage
	m_data[ 0]=m1[ 0]*m2[ 0]+m1[ 1]*m2[ 4]+m1[ 2]*m2[ 8];
	m_data[ 1]=m1[ 0]*m2[ 1]+m1[ 1]*m2[ 5]+m1[ 2]*m2[ 9];
	m_data[ 2]=m1[ 0]*m2[ 2]+m1[ 1]*m2[ 6]+m1[ 2]*m2[10];

	m_data[ 4]=m1[ 4]*m2[ 0]+m1[ 5]*m2[ 4]+m1[ 6]*m2[ 8];
	m_data[ 5]=m1[ 4]*m2[ 1]+m1[ 5]*m2[ 5]+m1[ 6]*m2[ 9];
	m_data[ 6]=m1[ 4]*m2[ 2]+m1[ 5]*m2[ 6]+m1[ 6]*m2[10];

	m_data[ 8]=m1[ 8]*m2[ 0]+m1[ 9]*m2[ 4]+m1[10]*m2[ 8];
	m_data[ 9]=m1[ 8]*m2[ 1]+m1[ 9]*m2[ 5]+m1[10]*m2[ 9];
	m_data[10]=m1[ 8]*m2[ 2]+m1[ 9]*m2[ 6]+m1[10]*m2[10];

	m_data[12]=m1[12]*m2[ 0]+m1[13]*m2[ 4]+m1[14]*m2[ 8]+		m2[12];
	m_data[13]=m1[12]*m2[ 1]+m1[13]*m2[ 5]+m1[14]*m2[ 9]+		m2[13];
	m_data[14]=m1[12]*m2[ 2]+m1[13]*m2[ 6]+m1[14]*m2[10]+		m2[14];

	return *this;
	}

GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::Translate3x4(const GPGVector3 &vector)
	{
	GPGVector3 result;
	const F32 *v=result.RawConst();

	RotateVector(vector,result);		//* equivalent when you do the math

	//* assuming 3x4
	m_data[12]+=v[0];
	m_data[13]+=v[1];
	m_data[14]+=v[2];

	return *this;
	}


GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::Rotate3x4(F32 radians,GPGAxisID axis)
	{

	F32 sina=GPGSIN(radians);
	F32 cosa=GPGCOS(radians);
	F32 b,c;

	static const int index[3][2]={{4,8},{8,0},{0,4}};
#if TRUE

	const int &x1=index[axis][0];
	const int &x2=index[axis][1];

	int y;
	for(y=0;y<3;y++)		//* would loop to 4 if using true 4x4
		{
		b=m_data[y+x1];
		c=m_data[y+x2];

		m_data[y+x1]=c*sina+b*cosa;
		m_data[y+x2]=c*cosa-b*sina;
		}
#else
	//* unrolled
	U32 x1=index[axis][0];
	U32 x2=index[axis][1];

	b=m_data[x1];
	c=m_data[x2];
	m_data[x1++]=c*sina+b*cosa;
	m_data[x2++]=c*cosa-b*sina;

	b=m_data[x1];
	c=m_data[x2];
	m_data[x1++]=c*sina+b*cosa;
	m_data[x2++]=c*cosa-b*sina;

	b=m_data[x1];
	c=m_data[x2];
	m_data[x1++]=c*sina+b*cosa;
	m_data[x2++]=c*cosa-b*sina;

	b=m_data[x1];
	c=m_data[x2];
	m_data[x1++]=c*sina+b*cosa;
	m_data[x2++]=c*cosa-b*sina;
#endif

	return *this;
	}

GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::Scale3x4(const GPGVector3 &vector)
	{
	const F32 *v=vector.RawConst();

	m_data[0]*=v[0];
	m_data[1]*=v[0];
	m_data[2]*=v[0];

	m_data[4]*=v[1];
	m_data[5]*=v[1];
	m_data[6]*=v[1];

	m_data[8]*=v[2];
	m_data[9]*=v[2];
	m_data[10]*=v[2];

	return *this;
	}

GPGINLINE GPGMatrix4x4 &GPGMatrix4x4::Scale(const GPGVector3 &vector)
	{
	const F32 *v=vector.RawConst();

	m_data[0]*=v[0];
	m_data[1]*=v[0];
	m_data[2]*=v[0];
	m_data[3]*=v[0];

	m_data[4]*=v[1];
	m_data[5]*=v[1];
	m_data[6]*=v[1];
	m_data[7]*=v[1];

	m_data[8]*=v[2];
	m_data[9]*=v[2];
	m_data[10]*=v[2];
	m_data[11]*=v[2];

	return *this;
	}

// NOTE limited 3x3+3 3-component multiplication, not 4x4 full multiplication
GPGINLINE void GPGMatrix4x4::TransformVector(const GPGVector3 &source,
													GPGVector3 &result) const
	{
	const F32 *in=source.RawConst();
	F32 *out=result.Raw();

	// unrolled
	out[0]=in[0]*m_data[0]+in[1]*m_data[4]+in[2]*m_data[8]+m_data[12];
	out[1]=in[0]*m_data[1]+in[1]*m_data[5]+in[2]*m_data[9]+m_data[13];
	out[2]=in[0]*m_data[2]+in[1]*m_data[6]+in[2]*m_data[10]+m_data[14];
	}

// NOTE limited 3x3+3 3-component multiplication, not 4x4 full multiplication
// Divides by transformed w
GPGINLINE void GPGMatrix4x4::ProjectVector(const GPGVector3 &source,
													GPGVector3 &result) const
	{
	const F32 *in=source.RawConst();
	F32 *out=result.Raw();
	F32 w;

	// unrolled - 
	w=1.0f/(in[0]*m_data[3]+in[1]*m_data[7]+in[2]*m_data[11]+m_data[15] );
	out[0]=in[0]*m_data[0]+in[1]*m_data[4]+in[2]*m_data[8]+m_data[12];
	out[1]=in[0]*m_data[1]+in[1]*m_data[5]+in[2]*m_data[9]+m_data[13];
	out[2]=in[0]*m_data[2]+in[1]*m_data[6]+in[2]*m_data[10]+m_data[14];

	out[0]*=w;
	out[1]*=w;
	out[2]*=w;

	}

GPGINLINE void GPGMatrix4x4::RotateVector(const GPGVector3 &source,
													GPGVector3 &result) const
	{
	const F32 *in=source.RawConst();
	F32 *out=result.Raw();

	// unrolled
	out[0]=in[0]*m_data[0]+in[1]*m_data[4]+in[2]*m_data[8];
	out[1]=in[0]*m_data[1]+in[1]*m_data[5]+in[2]*m_data[9];
	out[2]=in[0]*m_data[2]+in[1]*m_data[6]+in[2]*m_data[10];
	}

GPGINLINE GPGRESULT GPGMatrix4x4::Invert3x4(const GPGMatrix4x4 &operand)
{
	GPGASSERT(this!=&operand);
	F32 det = operand.CalcDeterminant3x3();

	const F32* src = operand.RawConst();
	F32* dst = Raw();

	if(fabsf(det) > GPGNEARZERO)
	{
		det = 1.0f / det;

		dst[0]  =  det*(src[5]*src[10] - src[6]*src[9]);
		dst[1]  = -det*(src[1]*src[10] - src[2]*src[9]);
		dst[2]  =  det*(src[1]*src[6]  - src[2]*src[5]);
		dst[3]  = 0;
		dst[4]  = -det*(src[4]*src[10] - src[6]*src[8]);
		dst[5]  =  det*(src[0]*src[10] - src[2]*src[8]);
		dst[6]  = -det*(src[0]*src[6]  - src[2]*src[4]);
		dst[7]  = 0;
		dst[8]  =  det*(src[4]*src[9]  - src[5]*src[8]);
		dst[9]  = -det*(src[0]*src[9]  - src[1]*src[8]);
		dst[10] =  det*(src[0]*src[5]  - src[1]*src[4]);
		dst[11] = 0;
		dst[12] = -(src[12]*dst[0]+src[13]*dst[4]+src[14]*dst[8]);
		dst[13] = -(src[12]*dst[1]+src[13]*dst[5]+src[14]*dst[9]);
		dst[14] = -(src[12]*dst[2]+src[13]*dst[6]+src[14]*dst[10]);
		dst[15] = 1;
		return GPG_OK;
	}

	return GPG_E_INVALID_RANGE;
}

// static
GPGINLINE F32 GPGMatrix4x4::CalcDeterminant3x3(F32 a1,F32 a2,F32 a3,
													F32 b1,F32 b2,F32 b3,
													F32 c1,F32 c2,F32 c3)
	{
	return	 a1*(b2*c3-b3*c2)
			-b1*(a2*c3-a3*c2)
			+c1*(a2*b3-a3*b2);
	}

GPGINLINE F32 GPGMatrix4x4::CalcDeterminant3x3(void) const
{
	F32 det;
	const F32* src = RawConst();

	F32 d1, d2, d3;
	d1 = src[5]*src[10] - src[6]*src[9];
	d2 = src[1]*src[10] - src[2]*src[9];
	d3 = src[1]*src[6]  - src[2]*src[5];
	det = src[0]*d1 - src[4]*d2 + src[8]*d3;

	return det;
}


GPGINLINE void GPGMatrix4x4::ApplyThreshold(F32 threshold)
	{
	U32 m;
	for(m=0;m<16;m++)
		{
		I32 ivalue=I32((m_data[m]<0.0f)? m_data[m]-0.5f: m_data[m]+0.5f);

		if(fabs(m_data[m]-(F32)ivalue) < threshold)
			m_data[m]=(F32)ivalue;
		}
	}


#endif /* __GPGMatrix_h__ */
