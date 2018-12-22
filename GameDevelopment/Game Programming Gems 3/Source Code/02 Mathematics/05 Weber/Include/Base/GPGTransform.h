
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

#ifndef __GPGTransform_h__
#define __GPGTransform_h__


#define GPGROTATE_BY_QUAT	TRUE	// stack rotate using quats vs. matrix
#define GPGXFORM_BY_QUAT	FALSE	// vector transform using quats vs. matrix




enum GPGReadWrite {READONLY,WRITEONLY,READWRITE};



/******************************************************************************
class GPGTransform

	WARNING Scaling is a recent feature with minimal testing.

	Wraps most GPGMatrix4x4 and GPGQuaternion functions
	to do automatic JIT conversions and control validity flags.
	The lack of virtuals is intentional for speed.

	Uses modeling-optimized 3x4 versions of GPGMatrix operations.

	To get data from the OpenGL current matrix:
		glGetFloatv(GL_MODELVIEW_MATRIX,transform.GetMatrixData(WRITEONLY));

	To put data into the OpenGL current matrix:
		glLoadMatrixf(transform.GetMatrixData(READONLY));

	To use relative to the OpenGL current matrix:
		glMultMatrixf(transform.GetMatrixData(READONLY));

	The last two require being in GL_MODELVIEW mode like with:
		glMatrixMode(GL_MODELVIEW);

	TODO check matrix inverse use of scaling
	TODO check GPGROTATE_BY_QUAT of FALSE

******************************************************************************/
class GPGTransform
	{
	public:
						// no default data
						GPGTransform(void)	{ Reset(); };

		void			Reset(void)
							{
							m_matrix_valid=TRUE;
							m_matinverse_valid=FALSE;
							m_quaternion_valid=FALSE;
							m_matrix.Reset();
							m_matrix_inverse.Reset();
							};

						GPGTransform(const GPGTransform &operand)
							{
							Reset();
							operator=(operand);
							};

						GPGTransform(const GPGMatrix4x4 &operand)
							{
							Reset();
							operator=(operand);
							};

						GPGTransform(const GPGQuaternion &operand)
							{
							Reset();
							operator=(operand);
							};

		GPGTransform	&operator=(const GPGTransform &operand);
		GPGTransform	&operator=(const GPGMatrix4x4 &operand);
		GPGTransform	&operator=(const GPGQuaternion &operand);

						// not const function, since JIT update
		bool			operator==(const GPGMatrix4x4 &operand)
							{
							UpdateMatrix();
							return m_matrix==operand;
							};

		void			UpdateMatrix(void);
		void			UpdateMatrixInverse(void);
		void			UpdateQuaternion(void);

		GPGTransform	&Multiply(const GPGMatrix4x4 &operand);
		GPGTransform	&Multiply(GPGTransform &operand)
							{ return Multiply(operand.MatrixConst()); };

		bool			Interpolate(F32 t,GPGTransform &from,
													GPGTransform &to);

		void			MakeIdentity(void);
		void			MakeTRS(const GPGVector3 &translation,
								const GPGQuaternion &rotation,
								const GPGVector3 &scale);
		GPGRESULT		CalcTRS(GPGVector3 *pTranslation,
								GPGQuaternion *pRotation,
								GPGVector3 *pScale);

		void			Translate(F32 x,F32 y,F32 z);
		void			Translate(const GPGVector3 &vector);
		void			Scale(const GPGVector3 &vector);
		void			Rotate(const GPGQuaternion &operand);
		void			Rotate(F32 radians,GPGAxisID axis);

		void			TransformVector(const F32 *source,
												GPGVector3 &result);
		void			TransformVector(const GPGVector3 &source,
												GPGVector3 &result);
		void			ReverseTransformVector(const F32 *source,
												GPGVector3 &result);
		void			ReverseTransformVector(const GPGVector3 &source,
												GPGVector3 &result);

						// float* version of *RotateVector() eliminates
						// potential auto-copy when data is already float*
		void			RotateVector(const F32 *source,
														GPGVector3 &result);
		void			RotateVector(const GPGVector3 &source,
														GPGVector3 &result);
		void			ReverseRotateVector(const F32 *source,
														GPGVector3 &result);
		void			ReverseRotateVector(const GPGVector3 &source,
														GPGVector3 &result);

						// For GetMatrixData() and Quaternion(),
						// note that WRITEONLY mode may be faster,
						// but doesn't make sure current data is valid.

						// WRITEONLY and READONLY are your intent
						// and can't be enforced such as with const.
						// The returned value should be used
						// immediately, then ignored.

						// for read-only, try GetMatrixDataConst()
		F32				*GetMatrixData(GPGReadWrite mode);
const	F32				*GetMatrixDataConst(void)
							{
							// read implied, so make sure it's valid
							UpdateMatrix();
							return m_matrix.RawConst();
							};
const	GPGMatrix4x4	&MatrixConst(void)
							{
							// read implied, so make sure it's valid
							UpdateMatrix();
							return m_matrix;
							};
const	F32				*GetMatrixInverseDataConst(void)
							{
							// read implied, so make sure it's valid
							UpdateMatrixInverse();
							return m_matrix_inverse.RawConst();
							};
const	GPGMatrix4x4	&MatrixInverseConst(void)
							{
							// read implied, so make sure it's valid
							UpdateMatrixInverse();
							return m_matrix_inverse;
							};

						// For read-only, try GetQuaternionConst()
		GPGQuaternion	&Quaternion(GPGReadWrite mode);
const	GPGQuaternion	&QuaternionConst(void)
							{
							// read implied, so make sure it's valid
							UpdateQuaternion();
							return m_quaternion;
							};

		void			SetTranslation(const GPGVector3 &vector)
							{ m_matrix.SetTranslation(vector); };
		void			CopyTranslation(GPGVector3 &vector) const
							{ m_matrix.CopyTranslation(vector); };

						// app shouldn't need to care, but for curiosity
		bool			QuaternionIsValid(void)	{ return m_quaternion_valid; };
		bool			MatrixIsValid(void)		{ return m_matrix_valid; };

#if FALSE
						// NOTE experimental
		void			LimitToEulerZ(void);
		F32				ExtractEulerZ(void);
#endif

	// not inline:

#if GPGALLOW_STREAMS
		std::ostream	&Print(std::ostream &out,long indention) const;
#endif

		GPGString		Out(bool verbose=false) const;

	private:
static	void			RotateVectorBy(const GPGMatrix4x4 &mat,
								const F32 *source,GPGVector3 &result);
		void			RotateVectorByQuat(const F32 *source,
														GPGVector3 &result);
		void			ReverseRotateVectorByQuat(const F32 *source,
														GPGVector3 &result);

		GPGMatrix4x4	m_matrix;
		GPGMatrix4x4	m_matrix_inverse;

		GPGQuaternion	m_quaternion;
		GPGVector3		m_scale;		// valid when m_quaternion_valid

		bool			m_matrix_valid;
		bool			m_matinverse_valid;
		bool			m_quaternion_valid;
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGTransform &transform);
#endif

GPGINLINE GPGTransform &GPGTransform::operator=(const GPGTransform &operand)
	{
	if(operand.m_matrix_valid)
		{
		m_matrix=operand.m_matrix;
		m_matrix_valid=TRUE;
		}
	else
		{
		const F32 *mat=operand.m_matrix.RawConst();

		// always copy translation data
		m_matrix.ResetBottom();
		m_matrix[12]=mat[12];
		m_matrix[13]=mat[13];
		m_matrix[14]=mat[14];
		m_matrix_valid=FALSE;
		}

	if(operand.m_quaternion_valid)
		{
		m_quaternion=operand.m_quaternion;
		m_scale=operand.m_scale;
		m_quaternion_valid=TRUE;
		}
	else
		m_quaternion_valid=FALSE;

	m_matinverse_valid=FALSE;

	return *this;
	}

GPGINLINE GPGTransform &GPGTransform::operator=(const GPGMatrix4x4 &operand)
	{
	m_matrix=operand;

	m_matrix_valid=TRUE;
	m_matinverse_valid=FALSE;
	m_quaternion_valid=FALSE;

	return *this;
	}

GPGINLINE GPGTransform &GPGTransform::operator=(const GPGQuaternion &operand)
	{
	m_quaternion=operand;
	m_scale.Set(1.0f,1.0f,1.0f);
	m_matrix.ResetNonRotation();

	m_matrix_valid=FALSE;
	m_matinverse_valid=FALSE;
	m_quaternion_valid=TRUE;

	return *this;
	}

GPGINLINE GPGTransform &GPGTransform::Multiply(const GPGMatrix4x4 &operand)
	{
	UpdateMatrix();
	GPGMatrix4x4 temp=m_matrix;
	m_matrix.Multiply3x4(temp,operand);

	m_matrix_valid=TRUE;
	m_matinverse_valid=FALSE;
	m_quaternion_valid=FALSE;

	return *this;
	}

GPGINLINE void GPGTransform::UpdateMatrix(void)
	{
//	if(!m_matrix_valid && m_quaternion_valid)
	if(!m_matrix_valid)
		{
		GPGVector3 translation= &m_matrix[12];
		GPGMatrix4x4 rotation=m_quaternion;

		GPGASSERT(m_quaternion_valid);
		m_matrix.MakeIdentity();
		m_matrix.Translate3x4(translation);
		GPGMatrix4x4 copy=m_matrix;
		m_matrix.Multiply3x4(copy,rotation);
		m_matrix.Scale3x4(m_scale);

		m_matrix_valid=TRUE;
		}
	}

GPGINLINE void GPGTransform::UpdateMatrixInverse(void)
	{
	if(!m_matinverse_valid)
		{
		UpdateQuaternion();

//		if(m_quaternion_valid)
			{
			//* temporary inversion
			m_quaternion.Invert();
			m_scale.Reciprocate();

			m_matrix_inverse=m_quaternion;
			m_matrix_inverse.Scale3x4(m_scale);

			m_quaternion.Invert();
			m_scale.Reciprocate();

			m_matrix_inverse.ResetBottom();
			GPGVector3 translation,result;
			translation[0]= -m_matrix[12];
			translation[1]= -m_matrix[13];
			translation[2]= -m_matrix[14];
			ReverseRotateVectorByQuat(translation.RawConst(),result);
			m_matrix_inverse[12]=result[0];
			m_matrix_inverse[13]=result[1];
			m_matrix_inverse[14]=result[2];

			m_matinverse_valid=TRUE;
			}
		}
	}

GPGINLINE void GPGTransform::UpdateQuaternion(void)
	{
//	if(!m_quaternion_valid && m_matrix_valid)
	if(!m_quaternion_valid)
		{
		GPGASSERT(m_matrix_valid);

		//* also computes scale
		m_matrix.CalcTRS(NULL,&m_quaternion,&m_scale);

		m_matinverse_valid=FALSE;
		m_quaternion_valid=TRUE;
		}
	}

GPGINLINE bool GPGTransform::Interpolate(F32 t,GPGTransform &from,
														GPGTransform &to)
	{
	GPGASSERT(this!=&from);
	GPGASSERT(this!=&to);
	// not const args, since JIT m_quat update

	from.UpdateQuaternion();
	to.UpdateQuaternion();

	if( !from.QuaternionIsValid() || !to.QuaternionIsValid() )
		return false;

	GPGVector3 translation;

	translation.Interpolate(t,GPGVector3(&from.m_matrix[12]),
								GPGVector3(&to.m_matrix[12]));
	m_scale.Interpolate(t,from.m_scale,to.m_scale);
	m_quaternion.Interpolate(t,from.QuaternionConst(),
								to.QuaternionConst());

	SetTranslation(translation);

	m_matrix_valid=FALSE;
	m_matinverse_valid=FALSE;
	m_quaternion_valid=TRUE;

	return true;
	}

GPGINLINE void GPGTransform::TransformVector(const GPGVector3 &source,
												GPGVector3 &result)
	{
	TransformVector(source.RawConst(),result);
	}

GPGINLINE void GPGTransform::TransformVector(const F32 *source,
												GPGVector3 &result)
	{
	F32 *v=result.Raw();
	const F32 *mat=m_matrix.RawConst();

	RotateVector(source,result);

	v[0]+=mat[12];
	v[1]+=mat[13];
	v[2]+=mat[14];
	}

GPGINLINE void GPGTransform::ReverseTransformVector(const GPGVector3 &source,
												GPGVector3 &result)
	{
	ReverseTransformVector(source.RawConst(),result);
	}
GPGINLINE void GPGTransform::ReverseTransformVector(const F32 *source,
												GPGVector3 &result)
	{
	GPGVector3 temp;
	const F32 *mat=m_matrix.RawConst();
	F32 *t=temp.Raw();

	t[0]=source[0]-mat[12];
	t[1]=source[1]-mat[13];
	t[2]=source[2]-mat[14];

	ReverseRotateVector(temp,result);
	}
GPGINLINE void GPGTransform::RotateVector(const F32 *source,
														GPGVector3 &result)
	{
#if GPGXFORM_BY_QUAT
	RotateVectorByQuat(source,result);
	result.Multiply(m_scale);
#else
	UpdateMatrix();
	RotateVectorBy(m_matrix,source,result);
#endif
	}
GPGINLINE void GPGTransform::RotateVector(const GPGVector3 &source,
														GPGVector3 &result)
	{
#if GPGXFORM_BY_QUAT
	RotateVectorByQuat(source.RawConst(),result);
	result.Multiply(m_scale);
#else
	UpdateMatrix();
	RotateVectorBy(m_matrix,source.RawConst(),result);
#endif
	}
GPGINLINE void GPGTransform::ReverseRotateVector(const F32 *source,
														GPGVector3 &result)
	{
#if GPGXFORM_BY_QUAT
	GPGVector3 temp(source);
	temp.Divide(m_scale);
	ReverseRotateVectorByQuat(temp,result);
#else
	UpdateMatrixInverse();
	RotateVectorBy(m_matrix_inverse,source,result);
#endif
	}
GPGINLINE void GPGTransform::ReverseRotateVector(const GPGVector3 &source,
														GPGVector3 &result)
	{
#if GPGXFORM_BY_QUAT
	GPGVector3 temp(source);
	temp.Divide(m_scale);
	ReverseRotateVectorByQuat(temp.RawConst(),result);
#else
	UpdateMatrixInverse();
	RotateVectorBy(m_matrix_inverse,source.RawConst(),result);
#endif
	}

GPGINLINE void GPGTransform::Translate(F32 x,F32 y,F32 z)
	{
	GPGVector3 vector(x,y,z);
	Translate(vector);
	}

GPGINLINE void GPGTransform::Translate(const GPGVector3 &vector)
	{
#if GPGROTATE_BY_QUAT
	// Quaternion version
	GPGVector3 rotated;
	RotateVectorByQuat(vector.RawConst(),rotated);
	rotated.Multiply(m_scale);
	m_matrix[12]+=rotated[0];
	m_matrix[13]+=rotated[1];
	m_matrix[14]+=rotated[2];
	m_matrix_valid=FALSE;
#else
	// Matrix version
	UpdateMatrix();
	m_matrix.Translate3x4(vector);
	m_quaternion_valid=FALSE;
#endif
	m_matinverse_valid=FALSE;
	}

GPGINLINE void GPGTransform::Scale(const GPGVector3 &vector)
	{
#if GPGROTATE_BY_QUAT
	// Quaternion version
	UpdateQuaternion();
	m_scale.Multiply(vector);
	m_matrix_valid=FALSE;
#else
	// Matrix version
	UpdateMatrix();
	m_matrix.Scale3x4(vector);
	m_quaternion_valid=FALSE;
#endif
	m_matinverse_valid=FALSE;
	}

GPGINLINE void GPGTransform::Rotate(const GPGQuaternion &operand)
	{
#if GPGROTATE_BY_QUAT
	// Quaternion version
	UpdateQuaternion();
	m_quaternion.Rotate(operand);
	m_matrix_valid=FALSE;
#else
	// Matrix version
	UpdateMatrix();
	GPGMatrix4x4 copy=m_matrix;
	m_matrix.Multiply(copy,GPGMatrix4x4(operand));
	m_quaternion_valid=FALSE;
#endif
	m_matinverse_valid=FALSE;
	}

GPGINLINE void GPGTransform::Rotate(F32 radians,GPGAxisID axis)
	{
#if GPGROTATE_BY_QUAT
	// Quaternion version
	UpdateQuaternion();
	m_quaternion.Rotate(radians,axis);
	m_matrix_valid=FALSE;
#else
	// Matrix version
	UpdateMatrix();
	m_matrix.Rotate3x4(radians,axis);
	m_quaternion_valid=FALSE;
#endif
	m_matinverse_valid=FALSE;
	}

GPGINLINE void GPGTransform::MakeIdentity(void)
	{
	m_matrix.ResetNonRotation();
	m_matrix.MakeIdentity();
	m_matrix_inverse.ResetNonRotation();
	m_matrix_inverse.MakeIdentity();
	m_quaternion.MakeIdentity();
	m_scale.Set(1.0f,1.0f,1.0f);

	m_matrix_valid=TRUE;
	m_matinverse_valid=TRUE;
	m_quaternion_valid=TRUE;
	}

GPGINLINE GPGRESULT GPGTransform::CalcTRS(GPGVector3 *pTranslation,
							GPGQuaternion *pRotation,GPGVector3 *pScale)
	{
	//* TODO UpdateQuaternion() should return a value (invert can fail)
	UpdateQuaternion();

	if(pTranslation)
		*pTranslation=&m_matrix[12];
	if(pRotation)
		*pRotation=m_quaternion;
	if(pScale)
		*pScale=m_scale;

	return GPG_OK;
	}

GPGINLINE void GPGTransform::MakeTRS(const GPGVector3 &translation,
									const GPGQuaternion &rotation,
									const GPGVector3 &scale)
	{
	SetTranslation(translation);
	m_quaternion=rotation;
	m_scale=scale;

	m_matrix_valid=FALSE;
	m_matinverse_valid=FALSE;
	m_quaternion_valid=TRUE;
	}

#if FALSE
GPGINLINE void GPGTransform::LimitToEulerZ(void)
	{
	UpdateQuaternion();
	F32 zangle=m_quaternion.ExtractEulerZ();
	m_quaternion.MakeRotation(zangle,GPG_Z_AXIS);

	m_matrix_valid=FALSE;
	m_matinverse_valid=FALSE;
	}

GPGINLINE F32 GPGTransform::ExtractEulerZ(void)
	{
	UpdateQuaternion();
	F32 zangle=m_quaternion.ExtractEulerZ();

	m_matrix_valid=FALSE;
	m_matinverse_valid=FALSE;

	return zangle;
	}
#endif

// static
GPGINLINE void GPGTransform::RotateVectorBy(const GPGMatrix4x4 &mat,
							const F32 *source,GPGVector3 &result)
	{
	const F32 *m=mat.RawConst();

	// unrolled
	result[0]=source[0]*m[0]+source[1]*m[4]+source[2]*m[8];
	result[1]=source[0]*m[1]+source[1]*m[5]+source[2]*m[9];
	result[2]=source[0]*m[2]+source[1]*m[6]+source[2]*m[10];
	}

GPGINLINE void GPGTransform::RotateVectorByQuat(const F32 *source,
														GPGVector3 &result)
	{
	// Note: auto-conversions between m_quat and vector

	UpdateQuaternion();
	m_quaternion.RotateVector(source,result.Raw());
	result.Multiply(m_scale);
	}

GPGINLINE void GPGTransform::ReverseRotateVectorByQuat(const F32 *source,
														GPGVector3 &result)
	{
	// Note: auto-conversions between m_quat and vector

	UpdateQuaternion();

	if(m_quaternion.Raw()[0] < 1.0f) 
		{

		GPGVector3 temp(source);
		temp.Divide(m_scale);

		//* temporary inversion
		m_quaternion.Invert();
		m_quaternion.RotateVector(temp.Raw(),result.Raw());
		m_quaternion.Invert();
		}
	else
		{
		GPGVector3 temp(source);
		result = temp;
		}
	}

GPGINLINE F32 *GPGTransform::GetMatrixData(GPGReadWrite mode)
	{
	// but don't waste the time if writing
	if(mode!=WRITEONLY)
		UpdateMatrix();

	if(mode!=READONLY)
		{
		m_matrix_valid=TRUE;
		m_matinverse_valid=FALSE;
		m_quaternion_valid=FALSE;
		}

	return m_matrix.Raw();
	}

GPGINLINE GPGQuaternion &GPGTransform::Quaternion(GPGReadWrite mode)
	{
	// don't waste the time if not reading
	if(mode!=WRITEONLY)
		UpdateQuaternion();

	if(mode!=READONLY)
		{
		m_matrix_valid=FALSE;
		m_matinverse_valid=FALSE;
		m_quaternion_valid=TRUE;

		//* TODO What should we do about the scale and pretranslation?
//		m_scale.Set(1.0f,1.0f,1.0f);
		}

	return m_quaternion;
	}

#endif /* __GPGTransform_h__ */
