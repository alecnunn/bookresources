/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: bSphere3.h
 *    Desc: bounding sphere code
 *          treated as a primitive
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _BSPHERE3_H
#define _BSPHERE3_H

#include "point3.h"

struct bSphere3
{
	float m_radius;
	point3 m_loc;

	bSphere3(){}

	bSphere3( float radius, point3 loc ) :
		m_radius( radius ), m_loc( loc )
	{
	}

	bSphere3( point3 loc, int nVerts, point3* pList )
	{
		m_loc = loc;
		m_radius = 0.f;
		float currRad;
		for( int i=0; i< nVerts; i++ )
		{
			currRad = pList[i].Mag();
			if( currRad > m_radius )
			{
				m_radius = currRad;
			}
		}
	}

	template< class iter >
	bSphere3( point3 loc, iter& begin, iter& end )
	{
		iter i = begin;
		m_loc = loc;
		m_radius = 0.f;
		float currRad;
		while( i != end )
		{
			currRad = (*i).Mag();
			if( currRad > m_radius )
			{
				m_radius = currRad;
			}
			i++;
		}
	}

	bSphere3& operator += ( const point3& in );
	bSphere3& operator -= ( const point3& in );

	static bool Intersect( bSphere3& a, bSphere3& b )
	{
		// avoid a square root by squaring both sides of the equation
		float magSqrd = a.m_radius * a.m_radius + b.m_radius * b.m_radius;
		if( (b.m_loc - a.m_loc).MagSquared() > magSqrd )
		{
			return false;
		}
		return true;
	}
};

//==========--------------------------  A few useful operators

inline bSphere3& bSphere3::operator += ( const point3& in )
{
	m_loc += in;
	return *this;
}

inline bSphere3& bSphere3::operator -= ( const point3& in )
{
	m_loc -= in;
	return *this;
}

inline const bSphere3 operator+(bSphere3 const &a, point3 const &b)
{
	return bSphere3( a.m_radius, b+a.m_loc ); 
}

inline const bSphere3 operator-(bSphere3 const &a, point3 const &b)
{
	return bSphere3( a.m_radius, b-a.m_loc ); 
}


#endif //_BSPHERE3_H