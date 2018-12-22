//==============================================================
//= Vector.cpp
//= Original coders: Trent Polack (trent@voxelsoft.com) and
//-					 Evan Pipho	  (evan@codershq.com)
//==============================================================
//= A 3D vector class
//==============================================================
#include <cmath>

#include "Vector.h"
using namespace MathLib;

//calculate the vector's length/magnitude
float Vector::Length( void ) {
	return ( ( float )sqrt( m_vec[0]*m_vec[0] +
							m_vec[1]*m_vec[1] +
							m_vec[2]*m_vec[2] ) );
}

//normalize the vectors information
void Vector::Normalize( void ) {
	float f= 1/Length( );

	m_vec[0]*= f;
	m_vec[1]*= f;
	m_vec[2]*= f;
}

//get the normal for the vector
Vector Vector::GetNormal( void ) {
	Vector temp;
	float f= 1/Length( );

	temp.Set( m_vec );

	temp[0]*= f;
	temp[1]*= f;
	temp[2]*= f;

	return temp;
}

//calculate the dot product of two vectors
float Vector::DotProduct( Vector& vec ) {
	return ( m_vec[0]*vec.m_vec[0] + 
			 m_vec[1]*vec.m_vec[1] + 
			 m_vec[2]*vec.m_vec[2] );
}

//calculate the cross product of two vectors
Vector Vector::CrossProduct( Vector& vec ) {
	return Vector( m_vec[1]*vec.m_vec[2] - m_vec[2]*vec.m_vec[1],
				   m_vec[2]*vec.m_vec[0] - m_vec[0]*vec.m_vec[2],
				   m_vec[0]*vec.m_vec[1] - m_vec[1]*vec.m_vec[0] );
}