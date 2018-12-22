//----------------------------------------------------------------------------
//
//mth_quaternion.cpp
//
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------

//··········································································//
//    Source :: Inclusions
//··········································································//

#include <math.h>
#include "mth_angle.h"
#include "mth_quaternion.h"

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
//mth_quat ·································································//
//··········································································//

mth_quat::mth_quat(void)
{
	set_identity();
}
mth_quat::mth_quat(const float * quat)
{
	if(quat)
		set_quat(quat);
	else
		set_identity();
}
mth_quat::~mth_quat(void)
{
}

void mth_quat::set_quat(const float * quat)
{
	if(!quat)
		return;

	m_quat[0] = quat[0];
	m_quat[1] = quat[1];
	m_quat[2] = quat[2];
	m_quat[3] = quat[3];
}
void mth_quat::set_quat(const mth_quat & quat)
{
	set_quat(quat.get_quat());
}
void mth_quat::set_quat(const mth_quat_wrp & quat)
{
	set_quat(quat.get_quat());
}

void mth_quat::set_matrix(float * mtx)
{
	if(!mtx)
		return;

	mtx[0] =  1 - 2*m_quat[1]*m_quat[1] - 2*m_quat[2]*m_quat[2];
	mtx[1] =  2*m_quat[0]*m_quat[1] + 2*m_quat[3]*m_quat[2];
	mtx[2] =  2*m_quat[0]*m_quat[2] - 2*m_quat[3]*m_quat[1];

	mtx[4] =  2*m_quat[0]*m_quat[1] - 2*m_quat[3]*m_quat[2];
	mtx[5] =  1 - 2*m_quat[0]*m_quat[0] - 2*m_quat[2]*m_quat[2];
	mtx[6] =  2*m_quat[1]*m_quat[2] + 2*m_quat[3]*m_quat[0];

	mtx[8] =  2*m_quat[0]*m_quat[2] + 2*m_quat[3]*m_quat[1];
	mtx[9] =  2*m_quat[1]*m_quat[2] - 2*m_quat[3]*m_quat[0];
	mtx[10] = 1 - 2*m_quat[0]*m_quat[0] - 2*m_quat[1]*m_quat[1];
}
void mth_quat::set_slerp(const float * q1, const float * q2, float i_val)
{
	if(!q1 || !q2)
		return;

	if(i_val < 0.0f)
		i_val = 0.0f;

	if(!i_val)
	{
		set_quat(q1);
		return;
	}

	if(i_val > 1.0f)
		i_val = 1.0f;

	int i;
	float a = 0, b = 0;
	for ( i = 0; i < 4; i++ )
	{
		a += ( q1[i]-q2[i] )*( q1[i]-q2[i] );
		b += ( q1[i]+q2[i] )*( q1[i]+q2[i] );
	}
	
	float cosom = q1[0]*q2[0]+q1[1]*q2[1]+q1[2]*q2[2];
	double sclq1, sclq2;

	if ( a > b )
		cosom -= q1[3]*q2[3];
	else
		cosom += q1[3]*q2[3];

	if (( 1.0+cosom ) > 0.00000001 )
	{
		if (( 1.0-cosom ) > 0.00000001 )
		{
			double omega = acos( cosom );
			double sinom = sin( omega );
			sclq1 = sin(( 1.0-i_val )*omega );
			sclq2 = sin( i_val*omega );
			if(sinom)
			{
				sclq1 /= sinom;
				sclq2 /= sinom;
			}			
		}
		else
		{
			sclq1 = 1.0-i_val;
			sclq2 = i_val;
		}
		for ( i = 0; i < 3; i++ )
			m_quat[i] = ( float )( sclq1*q1[i]+sclq2*q2[i] );

		if ( a > b )
			m_quat[3] = ( float )( sclq1*q1[3]-sclq2*q2[3] );
		else
			m_quat[3] = ( float )( sclq1*q1[3]+sclq2*q2[3] );
	}
	else
	{
		m_quat[0] = -q1[1];
		m_quat[1] = q1[0];
		m_quat[2] = -q1[3];
		m_quat[3] = q1[2];

		sclq1 = sin(( 1.0-i_val )*0.5*MTH_PI );
		sclq2 = sin( i_val*0.5*MTH_PI );
		for ( i = 0; i < 3; i++ )
			m_quat[i] = ( float )( sclq1*q1[i]+sclq2*m_quat[i] );
	}
}

void mth_quat::set_identity(void)
{
	m_quat[0] = 0.0f;
	m_quat[1] = 0.0f;
	m_quat[2] = 0.0f;
	m_quat[3] = 1.0f;
}
void mth_quat::set_inv(void)
{
	m_quat[3] *= -1;
}

const float * mth_quat::get_quat(void) const
{
	return m_quat;
}

//··········································································//
//mth_quat wrapper··························································//
//··········································································//

mth_quat_wrp::mth_quat_wrp(void)
{
	m_quat = 0;
}
mth_quat_wrp::mth_quat_wrp(float * quat)
{
	m_quat = quat;
}
mth_quat_wrp::~mth_quat_wrp(void)
{
}

void mth_quat_wrp::wrp_quat(float * quat)
{
	m_quat = quat;
}
void mth_quat_wrp::set_quat(const float * quat)
{
	if(!m_quat || !quat)
		return;

	m_quat[0] = quat[0];
	m_quat[1] = quat[1];
	m_quat[2] = quat[2];
	m_quat[3] = quat[3];
}
void mth_quat_wrp::set_quat(const mth_quat & quat)
{
	set_quat(quat.get_quat());
}
void mth_quat_wrp::set_quat(const mth_quat_wrp & quat)
{
	set_quat(quat.get_quat());
}

void mth_quat_wrp::set_matrix(float * mtx)
{
	if(!m_quat || !mtx)
		return;

	mtx[0] =  1 - 2*m_quat[1]*m_quat[1] - 2*m_quat[2]*m_quat[2];
	mtx[1] =  2*m_quat[0]*m_quat[1] + 2*m_quat[3]*m_quat[2];
	mtx[2] =  2*m_quat[0]*m_quat[2] - 2*m_quat[3]*m_quat[1];

	mtx[4] =  2*m_quat[0]*m_quat[1] - 2*m_quat[3]*m_quat[2];
	mtx[5] =  1 - 2*m_quat[0]*m_quat[0] - 2*m_quat[2]*m_quat[2];
	mtx[6] =  2*m_quat[1]*m_quat[2] + 2*m_quat[3]*m_quat[0];

	mtx[8] =  2*m_quat[0]*m_quat[2] + 2*m_quat[3]*m_quat[1];
	mtx[9] =  2*m_quat[1]*m_quat[2] - 2*m_quat[3]*m_quat[0];
	mtx[10] = 1 - 2*m_quat[0]*m_quat[0] - 2*m_quat[1]*m_quat[1];
}
void mth_quat_wrp::set_slerp(const float * q1, const float * q2, float i_val)
{
	if(!m_quat || !q1 || !q2)
		return;

	if(i_val < 0.0f)
		i_val = 0.0f;

	if(!i_val)
	{
		set_quat(q1);
		return;
	}

	if(i_val > 1.0f)
		i_val = 1.0f;

	int i;
	float a = 0, b = 0;
	for ( i = 0; i < 4; i++ )
	{
		a += ( q1[i]-q2[i] )*( q1[i]-q2[i] );
		b += ( q1[i]+q2[i] )*( q1[i]+q2[i] );
	}
	
	float cosom = q1[0]*q2[0]+q1[1]*q2[1]+q1[2]*q2[2];
	double sclq1, sclq2;

	if ( a > b )
		cosom -= q1[3]*q2[3];
	else
		cosom += q1[3]*q2[3];

	if (( 1.0+cosom ) > 0.00000001 )
	{
		if (( 1.0-cosom ) > 0.00000001 )
		{
			double omega = acos( cosom );
			double sinom = sin( omega );
			sclq1 = sin(( 1.0-i_val )*omega );
			sclq2 = sin( i_val*omega );
			if(sinom)
			{
				sclq1 /= sinom;
				sclq2 /= sinom;
			}			
		}
		else
		{
			sclq1 = 1.0-i_val;
			sclq2 = i_val;
		}
		for ( i = 0; i < 3; i++ )
			m_quat[i] = ( float )( sclq1*q1[i]+sclq2*q2[i] );

		if ( a > b )
			m_quat[3] = ( float )( sclq1*q1[3]-sclq2*q2[3] );
		else
			m_quat[3] = ( float )( sclq1*q1[3]+sclq2*q2[3] );
	}
	else
	{
		m_quat[0] = -q1[1];
		m_quat[1] = q1[0];
		m_quat[2] = -q1[3];
		m_quat[3] = q1[2];

		sclq1 = sin(( 1.0-i_val )*0.5*MTH_PI );
		sclq2 = sin( i_val*0.5*MTH_PI );
		for ( i = 0; i < 3; i++ )
			m_quat[i] = ( float )( sclq1*q1[i]+sclq2*m_quat[i] );
	}
}

void mth_quat_wrp::set_identity(void)
{
	m_quat[0] = 0.0f;
	m_quat[1] = 0.0f;
	m_quat[2] = 0.0f;
	m_quat[3] = 1.0f;
}
void mth_quat_wrp::set_inv(void)
{
	m_quat[3] *= -1;
}

const float * mth_quat_wrp::get_quat(void) const
{
	return m_quat;
}

//··········································································//
//    Source :: Internal Function Implementation
//··········································································//

//··········································································//
//    Source :: Internal Class Implementation
//··········································································//

//----------------------------------------------------------------------------
//mth_quaternion.cpp
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
