#include "stdafx.h"
#include "MathStructs.h"

//-------------------------------------------------------------------------------------------------
void CPlane::Set( const CVector& vPoint0, const CVector& vPoint1, const CVector& vPoint2 )
{
	// Build plane from three points
	m_vNormal = ( vPoint1 - vPoint0 ).NormCrossProduct( vPoint2 - vPoint0 );
    m_fConstant = m_vNormal * vPoint0; 
}

// ------------------------------------------------------------------------------------------------
void CSphere::Merge( const CSphere& Sphere )
{
	// Distance between sphere centers
	float fSphereDist = ( Sphere.m_vPosition - m_vPosition ).Length();
	if( Sphere.m_fRadius < EPSILON || m_fRadius > fSphereDist + Sphere.m_fRadius )
	{
		// This sphere totally contains the Sphere or Sphere is null - do nothing, doesn't need to grow
	}
	else if( m_fRadius < EPSILON || Sphere.m_fRadius > fSphereDist + m_fRadius )
	{
		// Sphere totally contains this sphere
		*this = Sphere;
	}
	else
	{
		// Spheres partially overlap
		if( Sphere.m_vPosition != m_vPosition )
		{
			CVector vNormal = ( Sphere.m_vPosition - m_vPosition ) / fSphereDist;
			CVector vSegStart = Sphere.m_vPosition + vNormal * Sphere.m_fRadius;
			CVector vSegEnd = m_vPosition - vNormal * m_fRadius;
			m_vPosition = ( vSegEnd + vSegStart ) / 2.0f;
		}
		m_fRadius = ( fSphereDist + m_fRadius + Sphere.m_fRadius ) / 2.0f;
	}	
}