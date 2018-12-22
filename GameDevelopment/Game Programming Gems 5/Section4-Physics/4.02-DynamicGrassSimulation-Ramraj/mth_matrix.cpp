//----------------------------------------------------------------------------
//
//mth_matrix.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include "mth_matrix.h"

//··········································································//
//    Source :: Definitions
//··········································································//

//··········································································//
//    Source :: Structures
//··········································································//

//··········································································//
//    Source :: Class Defs
//··········································································//

//··········································································//
//    Source :: Private Function Defs
//··········································································//

//··········································································//
//    Source :: Private Vars
//··········································································//

//··········································································//
//    Source :: External Function Implementation
//··········································································//

//··········································································//
//    Source :: External Class Implementation
//··········································································//

//··········································································//
//mth_matrix ·······························································//
//··········································································//

mth_matrix::mth_matrix(void)
{
	set_identity();
}
mth_matrix::mth_matrix(const float * mtx)
{
	if(mtx)
		set_matrix(mtx);
	else
		set_identity();
}
mth_matrix::~mth_matrix(void)
{
}

void mth_matrix::set_matrix(const float * mtx)
{
	if(!mtx)
		return;

	m_mtx[0] = mtx[0];
	m_mtx[1] = mtx[1];
	m_mtx[2] = mtx[2];
	m_mtx[3] = mtx[3];
	m_mtx[4] = mtx[4];
	m_mtx[5] = mtx[5];
	m_mtx[6] = mtx[6];
	m_mtx[7] = mtx[7];
	m_mtx[8] = mtx[8];
	m_mtx[9] = mtx[9];
	m_mtx[10] = mtx[10];
	m_mtx[11] = mtx[11];
	m_mtx[12] = mtx[12];
	m_mtx[13] = mtx[13];
	m_mtx[14] = mtx[14];
	m_mtx[15] = mtx[15];
}

void mth_matrix::set_multiply(const float * mtx)
{
	if(!mtx)
		return;

	float res_mtx[16];

	res_mtx[0]  = m_mtx[0]*mtx[0]  + m_mtx[4]*mtx[1]  + m_mtx[8]*mtx[2];
	res_mtx[1]  = m_mtx[1]*mtx[0]  + m_mtx[5]*mtx[1]  + m_mtx[9]*mtx[2];
	res_mtx[2]  = m_mtx[2]*mtx[0]  + m_mtx[6]*mtx[1]  + m_mtx[10]*mtx[2];
	res_mtx[3]  = 0;

	res_mtx[4]  = m_mtx[0]*mtx[4]  + m_mtx[4]*mtx[5]  + m_mtx[8]*mtx[6];
	res_mtx[5]  = m_mtx[1]*mtx[4]  + m_mtx[5]*mtx[5]  + m_mtx[9]*mtx[6];
	res_mtx[6]  = m_mtx[2]*mtx[4]  + m_mtx[6]*mtx[5]  + m_mtx[10]*mtx[6];
	res_mtx[7]  = 0;

	res_mtx[8]  = m_mtx[0]*mtx[8]  + m_mtx[4]*mtx[9]  + m_mtx[8]*mtx[10];
	res_mtx[9]  = m_mtx[1]*mtx[8]  + m_mtx[5]*mtx[9]  + m_mtx[9]*mtx[10];
	res_mtx[10] = m_mtx[2]*mtx[8]  + m_mtx[6]*mtx[9]  + m_mtx[10]*mtx[10];
	res_mtx[11] = 0;

	res_mtx[12] = m_mtx[0]*mtx[12] + m_mtx[4]*mtx[13] + m_mtx[8]*mtx[14]  + m_mtx[12];
	res_mtx[13] = m_mtx[1]*mtx[12] + m_mtx[5]*mtx[13] + m_mtx[9]*mtx[14]  + m_mtx[13];
	res_mtx[14] = m_mtx[2]*mtx[12] + m_mtx[6]*mtx[13] + m_mtx[10]*mtx[14] + m_mtx[14];
	res_mtx[15] = 1;

	set_matrix(res_mtx);
}
void mth_matrix::set_multiply(const mth_matrix & mtx)
{
	set_multiply(mtx.m_mtx);
}
void mth_matrix::set_multiply(const mth_matrix_wrp & mtx)
{
	set_multiply(mtx.m_mtx);
}

void mth_matrix::set_identity(void)
{
	m_mtx[0] = 1.0f;
	m_mtx[1] = 0.0f;
	m_mtx[2] = 0.0f;
	m_mtx[3] = 0.0f;
	m_mtx[4] = 0.0f;
	m_mtx[5] = 1.0f;
	m_mtx[6] = 0.0f;
	m_mtx[7] = 0.0f;
	m_mtx[8] = 0.0f;
	m_mtx[9] = 0.0f;
	m_mtx[10] = 1.0f;
	m_mtx[11] = 0.0f;
	m_mtx[12] = 0.0f;
	m_mtx[13] = 0.0f;
	m_mtx[14] = 0.0f;
	m_mtx[15] = 1.0f;
}
void mth_matrix::set_trans(const float * vec)
{
	if(!vec)
		return;

	m_mtx[12] = vec[0];
	m_mtx[13] = vec[1];
	m_mtx[14] = vec[2];
}

float * mth_matrix::get_mtx(void) 
{
	return m_mtx;
}
float mth_matrix::get_ele(const int index) const
{
	if(index < 16)
		return m_mtx[index];
	else
		return 0.0f;
}

const float mth_matrix::operator[](const int index) const
{
	if(index < 16)
		return m_mtx[index];
	else
		return 0.0f;
}

//··········································································//
//mth_matrix wrapper························································//
//··········································································//

mth_matrix_wrp::mth_matrix_wrp(void)
{
	m_mtx = 0;
}
mth_matrix_wrp::mth_matrix_wrp(float * mtx)
{
	m_mtx = mtx;
}
mth_matrix_wrp::~mth_matrix_wrp(void)
{
}

void mth_matrix_wrp::wrp_matrix(float * mtx)
{
	m_mtx = mtx;
}
void mth_matrix_wrp::set_matrix(const float * mtx)
{
	if(!m_mtx || !mtx)
		return;

	m_mtx[0] = mtx[0];
	m_mtx[1] = mtx[1];
	m_mtx[2] = mtx[2];
	m_mtx[3] = mtx[3];
	m_mtx[4] = mtx[4];
	m_mtx[5] = mtx[5];
	m_mtx[6] = mtx[6];
	m_mtx[7] = mtx[7];
	m_mtx[8] = mtx[8];
	m_mtx[9] = mtx[9];
	m_mtx[10] = mtx[10];
	m_mtx[11] = mtx[11];
	m_mtx[12] = mtx[12];
	m_mtx[13] = mtx[13];
	m_mtx[14] = mtx[14];
	m_mtx[15] = mtx[15];
}

void mth_matrix_wrp::set_multiply(const float * mtx)
{
	if(!m_mtx || !mtx)
		return;

	float res_mtx[16];

	res_mtx[0]  = m_mtx[0]*mtx[0]  + m_mtx[4]*mtx[1]  + m_mtx[8]*mtx[2];
	res_mtx[1]  = m_mtx[1]*mtx[0]  + m_mtx[5]*mtx[1]  + m_mtx[9]*mtx[2];
	res_mtx[2]  = m_mtx[2]*mtx[0]  + m_mtx[6]*mtx[1]  + m_mtx[10]*mtx[2];
	res_mtx[3]  = 0;

	res_mtx[4]  = m_mtx[0]*mtx[4]  + m_mtx[4]*mtx[5]  + m_mtx[8]*mtx[6];
	res_mtx[5]  = m_mtx[1]*mtx[4]  + m_mtx[5]*mtx[5]  + m_mtx[9]*mtx[6];
	res_mtx[6]  = m_mtx[2]*mtx[4]  + m_mtx[6]*mtx[5]  + m_mtx[10]*mtx[6];
	res_mtx[7]  = 0;

	res_mtx[8]  = m_mtx[0]*mtx[8]  + m_mtx[4]*mtx[9]  + m_mtx[8]*mtx[10];
	res_mtx[9]  = m_mtx[1]*mtx[8]  + m_mtx[5]*mtx[9]  + m_mtx[9]*mtx[10];
	res_mtx[10] = m_mtx[2]*mtx[8]  + m_mtx[6]*mtx[9]  + m_mtx[10]*mtx[10];
	res_mtx[11] = 0;

	res_mtx[12] = m_mtx[0]*mtx[12] + m_mtx[4]*mtx[13] + m_mtx[8]*mtx[14]  + m_mtx[12];
	res_mtx[13] = m_mtx[1]*mtx[12] + m_mtx[5]*mtx[13] + m_mtx[9]*mtx[14]  + m_mtx[13];
	res_mtx[14] = m_mtx[2]*mtx[12] + m_mtx[6]*mtx[13] + m_mtx[10]*mtx[14] + m_mtx[14];
	res_mtx[15] = 1;

	set_matrix(res_mtx);
}
void mth_matrix_wrp::set_multiply(const mth_matrix & mtx)
{
	set_multiply(mtx.m_mtx);
}
void mth_matrix_wrp::set_multiply(const mth_matrix_wrp & mtx)
{
	set_multiply(mtx.m_mtx);
}

void mth_matrix_wrp::set_identity(void)
{
	if(!m_mtx)
		return;

	m_mtx[0] = 1.0f;
	m_mtx[1] = 0.0f;
	m_mtx[2] = 0.0f;
	m_mtx[3] = 0.0f;
	m_mtx[4] = 0.0f;
	m_mtx[5] = 1.0f;
	m_mtx[6] = 0.0f;
	m_mtx[7] = 0.0f;
	m_mtx[8] = 0.0f;
	m_mtx[9] = 0.0f;
	m_mtx[10] = 1.0f;
	m_mtx[11] = 0.0f;
	m_mtx[12] = 0.0f;
	m_mtx[13] = 0.0f;
	m_mtx[14] = 0.0f;
	m_mtx[15] = 1.0f;
}
void mth_matrix_wrp::set_trans(const float * vec)
{
	if(!m_mtx || !vec)
		return;

	m_mtx[12] = vec[0];
	m_mtx[13] = vec[1];
	m_mtx[14] = vec[2];
}

float * mth_matrix_wrp::get_mtx(void)
{
	return m_mtx;
}
float mth_matrix_wrp::get_ele(const int index) const
{
	if(m_mtx && index < 16)
		return m_mtx[index];
	else
		return 0.0f;
}

const float mth_matrix_wrp::operator[](const int index) const
{
	if(m_mtx && index < 16)
		return m_mtx[index];
	else
		return 0.0f;
}

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//mth_matrix.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
