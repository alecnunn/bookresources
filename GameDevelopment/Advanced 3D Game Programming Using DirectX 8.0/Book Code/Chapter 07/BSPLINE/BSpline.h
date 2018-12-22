/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: BSpline.h
 *    Desc: Code to implement b-spline curves
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _BSPLINE_H
#define _BSPLINE_H

#include <vector>
using std::vector;

class cBSpline  
{
	vector< point3 > m_ctrlPoints;

	static matrix4	m_baseMatrix;
public:

	void AddCtrlPoint( point3& pt )
	{
		m_ctrlPoints.push_back( pt );
	}

	point3 Calc( float t, int i0 );
	point3 CalcAbs( float t );

	int NumPoints()
	{
		return m_ctrlPoints.size();
	}
};

#endif //_BSPLINE_H
