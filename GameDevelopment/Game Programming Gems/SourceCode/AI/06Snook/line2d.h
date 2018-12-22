/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#ifndef LINE2D_H
#define LINE2D_H
/****************************************************************************************\
	Line2D.h

	Line2D component interface for the Navimesh sample program.
	Included as part of the Game Programming Gems sample code.

	Created 3/18/00 by Greg Snook
	greg@mightystudios.com
    ------------------------------------------------------------------------------------- 
	Notes/Revisions:

\****************************************************************************************/
#ifndef _MTXLIB_H
#include "mtxlib.h"
#endif

#include <cmath>

/*	Line2D
------------------------------------------------------------------------------------------
	
	Line2D represents a line in 2D space. Line data is held as a line segment having two 
	endpoints and as a fictional 3D plane extending verticaly. The Plane is then used for
	spanning and point clasification tests. A Normal vector is used internally to represent
	the fictional plane.
	
------------------------------------------------------------------------------------------
*/

class Line2D
{
public:

	// ----- ENUMERATIONS & CONSTANTS -----
	enum POINT_CLASSIFICATION
	{
		ON_LINE,		// The point is on, or very near, the line
		LEFT_SIDE,		// looking from endpoint A to B, the test point is on the left
		RIGHT_SIDE		// looking from endpoint A to B, the test point is on the right
	};

	enum LINE_CLASSIFICATION
	{
		COLLINEAR,			// both lines are parallel and overlap each other
		LINES_INTERSECT,	// lines intersect, but their segments do not
		SEGMENTS_INTERSECT,	// both line segments bisect each other
		A_BISECTS_B,		// line segment B is crossed by line A
		B_BISECTS_A,		// line segment A is crossed by line B
		PARALELL			// the lines are paralell
	};

	// ----- CREATORS ---------------------

	Line2D();
	Line2D( const Line2D& Src);
	Line2D( const vector2& PointA, const vector2& PointB);
	~Line2D();

	// ----- OPERATORS --------------------
	Line2D& operator=( const Line2D& Src);

	// ----- MUTATORS ---------------------
	void SetEndPointA(const vector2& Point);
	void SetEndPointB(const vector2& Point);
	void SetPoints(const vector2& PointA, const vector2& PointB);
	void SetPoints(float PointAx, float PointAy, float PointBx, float PointBy);
	
	float SignedDistance(const vector2& Point) const;
	POINT_CLASSIFICATION ClassifyPoint(const vector2& Point, float Epsilon = 0.0f) const;
	LINE_CLASSIFICATION Intersection(const Line2D& Line, vector2* pIntersectPoint=0)const;

	void GetDirection(vector2& Direction)const;

	// ----- ACCESSORS --------------------
	const vector2& EndPointA()const;
	const vector2& EndPointB()const;
	const vector2& Normal()const;
	float Length() const;

private:

	// ----- DATA -------------------------
	vector2 m_PointA;	// Endpoint A of our line segment
	vector2 m_PointB;	// Endpoint B of our line segment

	mutable vector2 m_Normal;	// 'normal' of the ray. 
								// a vector pointing to the right-hand side of the line
								// when viewed from PointA towards PointB
	mutable bool m_NormalCalculated; // normals are only calculated on demand

	// ----- HELPER FUNCTIONS -------------
	void ComputeNormal() const;

	// ----- UNIMPLEMENTED FUNCTIONS ------


};

//- Inline Functions ---------------------------------------------------------------------

//= CREATORS =============================================================================

/*	Line2D
------------------------------------------------------------------------------------------
	
	Default Object Constructor
	
------------------------------------------------------------------------------------------
*/

inline Line2D::Line2D()
: m_NormalCalculated(false)
{
}

inline Line2D::Line2D( const vector2& PointA, const vector2& PointB)
{
	m_PointA = PointA;
	m_PointB = PointB;
	m_NormalCalculated = false;
}


/*	Line2D
------------------------------------------------------------------------------------------
	
	Default Copy Constructor
	
------------------------------------------------------------------------------------------
*/

inline Line2D::Line2D( const Line2D& Src)
:m_NormalCalculated(false)
{
	*this = Src;
}


/*	~Line2D
------------------------------------------------------------------------------------------
	
	Default Object Destructor
	
------------------------------------------------------------------------------------------
*/

inline Line2D::~Line2D()
{
}

//= OPERATORS ============================================================================


inline Line2D& Line2D::operator=( const Line2D& Src)
{
	m_PointA = Src.m_PointA;
	m_PointB = Src.m_PointB;
	m_Normal = Src.m_Normal;
	m_NormalCalculated = Src.m_NormalCalculated;
	return (*this);
}

//= MUTATORS =============================================================================
inline void Line2D::SetEndPointA(const vector2& Point)
{
	m_PointA = Point;
	m_NormalCalculated = false;

}

inline void Line2D::SetEndPointB(const vector2& Point)
{
	m_PointB = Point;
	m_NormalCalculated = false;
}

inline const vector2& Line2D::Normal()const
{
	if (!m_NormalCalculated)
	{
		ComputeNormal();
	}

	return (m_Normal);
}

inline void Line2D::SetPoints(const vector2& PointA, const vector2& PointB)
{
	m_PointA = PointA;
	m_PointB = PointB;
	m_NormalCalculated = false;
}


inline void Line2D::SetPoints(float PointAx, float PointAy, float PointBx, float PointBy)
{
	m_PointA.x=PointAx;
	m_PointA.y=PointAy;
	m_PointB.x=PointBx;
	m_PointB.y=PointBy;
	m_NormalCalculated = false;
}

/*	SignedDistance
------------------------------------------------------------------------------------------
	
	Determines the signed distance from a point to this line. Consider the line as
	if you were standing on PointA of the line looking towards PointB. Posative distances
	are to the right of the line, negative distances are to the left.

------------------------------------------------------------------------------------------
*/

inline float Line2D::SignedDistance(const vector2& Point) const
{
	if (!m_NormalCalculated)
	{
		ComputeNormal();
	}

	vector2 TestVector(Point - m_PointA);
	
	return DotProduct(TestVector,m_Normal);

}

/*	ClassifyPoint
------------------------------------------------------------------------------------------
	
	Determines where a point lies in relation to this line. Consider the line as
	if you were standing on PointA of the line looking towards PointB. The incomming
	point is then classified as being on the Left, Right or Centered on the line.

------------------------------------------------------------------------------------------
*/

inline Line2D::POINT_CLASSIFICATION Line2D::ClassifyPoint(const vector2& Point, float Epsilon) const
{
    POINT_CLASSIFICATION      Result = ON_LINE;
    float          Distance = SignedDistance(Point);
    
    if (Distance > Epsilon)
    {
        Result = RIGHT_SIDE;
    }
    else if (Distance < -Epsilon)
    {
        Result = LEFT_SIDE;
    }

    return(Result);
}

/*	SegmentIntersection
------------------------------------------------------------------------------------------
	
	Determines if two segments intersect, and if so the point of intersection. The current
	member line is considered line AB and the incomming parameter is considered line CD for
	the purpose of the utilized equations.

	A = PointA of the member line
	B = PointB of the member line
	C = PointA of the provided line	
	D = PointB of the provided line	

------------------------------------------------------------------------------------------
*/

inline Line2D::LINE_CLASSIFICATION Line2D::Intersection(const Line2D& Line, vector2* pIntersectPoint)const
{
	float Ay_minus_Cy = m_PointA.y - Line.EndPointA().y;	
	float Dx_minus_Cx = Line.EndPointB().x - Line.EndPointA().x;	
	float Ax_minus_Cx = m_PointA.x - Line.EndPointA().x;	
	float Dy_minus_Cy = Line.EndPointB().y - Line.EndPointA().y;	
	float Bx_minus_Ax = m_PointB.x - m_PointA.x;	
	float By_minus_Ay = m_PointB.y - m_PointA.y;	

	float Numerator = (Ay_minus_Cy * Dx_minus_Cx) - (Ax_minus_Cx * Dy_minus_Cy);
	float Denominator = (Bx_minus_Ax * Dy_minus_Cy) - (By_minus_Ay * Dx_minus_Cx);

	// if lines do not intersect, return now
	if (!Denominator)
	{
		if (!Numerator)
		{
			return COLLINEAR;
		}

		return PARALELL;
	}

	float FactorAB = Numerator / Denominator;
	float FactorCD = ((Ay_minus_Cy * Bx_minus_Ax) - (Ax_minus_Cx * By_minus_Ay)) / Denominator;

	// posting (hitting a vertex exactly) is not allowed, shift the results
	// if they are within a minute range of the end vertecies
/*	if (fabs(FactorCD) < 1.0e-6f)
	{
		FactorCD = 1.0e-6f;
	}
	if (fabs(FactorCD - 1.0f) < 1.0e-6f)
	{
		FactorCD = 1.0f - 1.0e-6f;
	}
*/

	// if an interection point was provided, fill it in now
	if (pIntersectPoint)
	{
		pIntersectPoint->x = (m_PointA.x + (FactorAB * Bx_minus_Ax));
		pIntersectPoint->y = (m_PointA.y + (FactorAB * By_minus_Ay));
	}

	// now determine the type of intersection
	if ((FactorAB >= 0.0f) && (FactorAB <= 1.0f) && (FactorCD >= 0.0f) && (FactorCD <= 1.0f))
	{
		return SEGMENTS_INTERSECT;
	}
	else if ((FactorCD >= 0.0f) && (FactorCD <= 1.0f))
	{
		return (A_BISECTS_B);
	}
	else if ((FactorAB >= 0.0f) && (FactorAB <= 1.0f))
	{
		return (B_BISECTS_A);
	}

	return LINES_INTERSECT;
}

//= ACCESSORS ============================================================================

inline const vector2& Line2D::EndPointA()const
{
	return (m_PointA);
}


inline const vector2& Line2D::EndPointB()const
{
	return (m_PointB);
}


inline float Line2D::Length() const
{
	float xdist = m_PointB.x-m_PointA.x;
	float ydist = m_PointB.y-m_PointA.y;

	xdist *= xdist;
	ydist *= ydist;

	return static_cast<float>(sqrt(xdist + ydist));
}

inline void Line2D::GetDirection(vector2& Direction)const
{
	Direction = (m_PointB - m_PointA);
	Direction.normalize();
}

inline void Line2D::ComputeNormal() const
{
	//
	// Get Normailized direction from A to B
	//
	GetDirection(m_Normal);

	//
	// Rotate by -90 degrees to get normal of line
	//
	float OldYValue = m_Normal[1];
	m_Normal[1] = -m_Normal[0];
	m_Normal[0] = OldYValue;
	m_NormalCalculated = true;

}

//- End of Line2D -----------------------------------------------------------------------

//****************************************************************************************

#endif  // end of file      ( Line2D.h )

