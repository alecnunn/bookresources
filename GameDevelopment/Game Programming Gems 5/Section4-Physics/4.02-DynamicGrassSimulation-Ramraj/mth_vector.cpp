//----------------------------------------------------------------------------
//
//mth_vector.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <math.h>
#include "mth_vector.h"

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
//mth_vector································································//
//··········································································//

mth_vector::mth_vector(void)
{
	m_vec[0] = 0.0f;
	m_vec[1] = 0.0f;
	m_vec[2] = 0.0f;
}
mth_vector::mth_vector(const float * vec)
{
	if(vec)
	{
		m_vec[0] = vec[0];
		m_vec[1] = vec[1];
		m_vec[2] = vec[2];
	}
}
mth_vector::~mth_vector(void)
{
}

void mth_vector::set_vector(const float * vec)
{
	if(!vec)
		return;

	m_vec[0] = vec[0];
	m_vec[1] = vec[1];
	m_vec[2] = vec[2];
}
void mth_vector::set_vector(const float x,const float y,const float z)
{
	m_vec[0] = x;
	m_vec[1] = y;
	m_vec[2] = z;
}
void mth_vector::set_matrix(const float * mtx)
{
	if(!mtx)
		return;

	float vec[3];

	vec[0] = m_vec[0]*mtx[0] + m_vec[1]*mtx[4] + m_vec[2]*mtx[8] + mtx[12];
	vec[1] = m_vec[0]*mtx[1] + m_vec[1]*mtx[5] + m_vec[2]*mtx[9] + mtx[13];
	vec[2] = m_vec[0]*mtx[2] + m_vec[1]*mtx[6] + m_vec[2]*mtx[10] + mtx[14];

	m_vec[0] = vec[0];
	m_vec[1] = vec[1];
	m_vec[2] = vec[2];
}
void mth_vector::set_matrix_no_trans(const float * mtx)
{
	if(!mtx)
		return;

	float vec[3];

	vec[0] = m_vec[0]*mtx[0] + m_vec[1]*mtx[4] + m_vec[2]*mtx[8];
	vec[1] = m_vec[0]*mtx[1] + m_vec[1]*mtx[5] + m_vec[2]*mtx[9];
	vec[2] = m_vec[0]*mtx[2] + m_vec[1]*mtx[6] + m_vec[2]*mtx[10];

	m_vec[0] = vec[0];
	m_vec[1] = vec[1];
	m_vec[2] = vec[2];
}
void mth_vector::set_matrix_inv(const float * mtx)
{
	if(!mtx)
		return;

	float vec[3];

	vec[0] = m_vec[0]*mtx[0] + m_vec[1]*mtx[1] + m_vec[2]*mtx[2];
	vec[1] = m_vec[0]*mtx[4] + m_vec[1]*mtx[5] + m_vec[2]*mtx[6];
	vec[2] = m_vec[0]*mtx[8] + m_vec[1]*mtx[9] + m_vec[2]*mtx[10];

	m_vec[0] = vec[0] - mtx[12];
	m_vec[1] = vec[1] - mtx[13];
	m_vec[2] = vec[2] - mtx[14];
}
void mth_vector::set_matrix_inv_no_trans(const float * mtx)
{
	if(!mtx)
		return;

	float vec[3];

	vec[0] = m_vec[0]*mtx[0] + m_vec[1]*mtx[1] + m_vec[2]*mtx[2];
	vec[1] = m_vec[0]*mtx[4] + m_vec[1]*mtx[5] + m_vec[2]*mtx[6];
	vec[2] = m_vec[0]*mtx[8] + m_vec[1]*mtx[9] + m_vec[2]*mtx[10];

	m_vec[0] = vec[0];
	m_vec[1] = vec[1];
	m_vec[2] = vec[2];
}
void mth_vector::set_matrix_inv_trans(const float * mtx)
{
	if(!mtx)
		return;

	m_vec[0] -= mtx[12];
	m_vec[1] -= mtx[13];
	m_vec[2] -= mtx[14];
}
void mth_vector::set_normalize(void)
{
	float len = get_len();
	m_vec[0] /= len;
	m_vec[1] /= len;
	m_vec[2] /= len;
}
 
const float * mth_vector::get_vec(void) const 
{
	return (float *)m_vec;
}
float mth_vector::get_len(void) const
{
	return (float)sqrt(m_vec[0] * m_vec[0] + m_vec[1] * m_vec[1] + m_vec[2] * m_vec[2]);
}
float mth_vector::get_x(void) const
{
	return m_vec[0];
}
float mth_vector::get_y(void) const
{
	return m_vec[1];
}
float mth_vector::get_z(void) const
{
	return m_vec[2];
}

const float mth_vector::operator[](const int index) const
{
	if(index < 3)
		return m_vec[index];
	else
		return 0.0f;
}

//··········································································//
//mth_vector wrapper························································//
//··········································································//

mth_vector_wrp::mth_vector_wrp(void)
{
	m_vec = 0;
}
mth_vector_wrp::mth_vector_wrp(float * vec)
{
	m_vec = vec;
}
mth_vector_wrp::~mth_vector_wrp(void)
{
}

void mth_vector_wrp::wrp_vector(float * vec)
{
	m_vec = vec;
}
void mth_vector_wrp::set_vector(const float * vec)
{
	if(!m_vec || !vec)
		return;

	m_vec[0] = vec[0];
	m_vec[1] = vec[1];
	m_vec[2] = vec[2];
}
void mth_vector_wrp::set_vector(const float x,const float y,const float z)
{
	if(!m_vec)
		return;

	m_vec[0] = x;
	m_vec[1] = y;
	m_vec[2] = z;
}
void mth_vector_wrp::set_matrix(const float * mtx)
{
	if(!m_vec || !mtx)
		return;

	float vec[3];

	vec[0] = m_vec[0]*mtx[0] + m_vec[1]*mtx[4] + m_vec[2]*mtx[8] + mtx[12];
	vec[1] = m_vec[0]*mtx[1] + m_vec[1]*mtx[5] + m_vec[2]*mtx[9] + mtx[13];
	vec[2] = m_vec[0]*mtx[2] + m_vec[1]*mtx[6] + m_vec[2]*mtx[10] + mtx[14];

	m_vec[0] = vec[0];
	m_vec[1] = vec[1];
	m_vec[2] = vec[2];
}
void mth_vector_wrp::set_matrix_no_trans(const float * mtx)
{
	if(!m_vec || !mtx)
		return;

	float vec[3];

	vec[0] = m_vec[0]*mtx[0] + m_vec[1]*mtx[4] + m_vec[2]*mtx[8];
	vec[1] = m_vec[0]*mtx[1] + m_vec[1]*mtx[5] + m_vec[2]*mtx[9];
	vec[2] = m_vec[0]*mtx[2] + m_vec[1]*mtx[6] + m_vec[2]*mtx[10];

	m_vec[0] = vec[0];
	m_vec[1] = vec[1];
	m_vec[2] = vec[2];
}
void mth_vector_wrp::set_matrix_inv(const float * mtx)
{
	if(!m_vec || !mtx)
		return;

	m_vec[0] -= mtx[12];
	m_vec[1] -= mtx[13];
	m_vec[2] -= mtx[14];

	float vec[3];

	vec[0] = m_vec[0]*mtx[0] + m_vec[1]*mtx[1] + m_vec[2]*mtx[2];
	vec[1] = m_vec[0]*mtx[4] + m_vec[1]*mtx[5] + m_vec[2]*mtx[6];
	vec[2] = m_vec[0]*mtx[8] + m_vec[1]*mtx[9] + m_vec[2]*mtx[10];

	m_vec[0] = vec[0];
	m_vec[1] = vec[1];
	m_vec[2] = vec[2];
}
void mth_vector_wrp::set_matrix_inv_no_trans(const float * mtx)
{
	if(!m_vec || !mtx)
		return;

	float vec[3];

	vec[0] = m_vec[0]*mtx[0] + m_vec[1]*mtx[1] + m_vec[2]*mtx[2];
	vec[1] = m_vec[0]*mtx[4] + m_vec[1]*mtx[5] + m_vec[2]*mtx[6];
	vec[2] = m_vec[0]*mtx[8] + m_vec[1]*mtx[9] + m_vec[2]*mtx[10];

	m_vec[0] = vec[0];
	m_vec[1] = vec[1];
	m_vec[2] = vec[2];
}
void mth_vector_wrp::set_matrix_inv_trans(const float * mtx)
{
	if(!m_vec || !mtx)
		return;

	m_vec[0] -= mtx[12];
	m_vec[1] -= mtx[13];
	m_vec[2] -= mtx[14];
}
void mth_vector_wrp::set_normalize(void)
{
	if(!m_vec)
		return;

	float len = get_len();
	m_vec[0] /= len;
	m_vec[1] /= len;
	m_vec[2] /= len;
}

const float * mth_vector_wrp::get_vec(void) const 
{
	return m_vec;
}
float mth_vector_wrp::get_len(void) const
{
	if(m_vec)
		return (float)sqrt(m_vec[0] * m_vec[0] + m_vec[1] * m_vec[1] + m_vec[2] * m_vec[2]);
	else
		return 1.0f;
}
float mth_vector_wrp::get_x(void) const
{
	if(m_vec)
		return m_vec[0];
	else
		return 1.0f;
}
float mth_vector_wrp::get_y(void) const
{
	if(m_vec)
		return m_vec[1];
	else
		return 1.0f;
}
float mth_vector_wrp::get_z(void) const
{
	if(m_vec)
		return m_vec[2];
	else
		return 1.0f;
}

const float mth_vector_wrp::operator[](const int index) const
{
	if(!m_vec)
		return 0.0f;

	if(index < 3)
		return m_vec[index];
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
//mth_vector.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
