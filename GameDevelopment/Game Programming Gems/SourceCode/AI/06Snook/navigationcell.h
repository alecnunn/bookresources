/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#ifndef NAVIGATIONCELL_H
#define NAVIGATIONCELL_H
/****************************************************************************************\
	NavigationCell.h

	NavigationCell component interface for the Navimesh sample program.
	Included as part of the Game Programming Gems sample code.

	Created 3/18/00 by Greg Snook
	greg@mightystudios.com
    ------------------------------------------------------------------------------------- 
	Notes/Revisions:

\****************************************************************************************/
#ifndef _MTXLIB_H
#include "mtxlib.h"
#endif

#ifndef PLANE_H
#include "plane.h"
#endif

#ifndef LINE2D_H
#include "line2d.h"
#endif

// a forward declaration for NavigationHeap is required
class NavigationHeap;


/*	NavigationCell
------------------------------------------------------------------------------------------
	
	A NavigationCell represents a single triangle within a NavigationMesh. It contains 
	functions for testing a path against the cell, and various ways to resolve collisions
	with the cell walls. Portions of the A* path finding algorythm are provided within this
	class as well, but the path finding process is managed by the parent Navigation Mesh.
	
------------------------------------------------------------------------------------------
*/

class NavigationCell
{
public:

	// ----- ENUMERATIONS & CONSTANTS -----

	enum CELL_VERT
	{
		VERT_A = 0,
		VERT_B,
		VERT_C
	};

	enum CELL_SIDE
	{
		SIDE_AB = 0,
		SIDE_BC,
		SIDE_CA
	};

	enum PATH_RESULT
	{
		NO_RELATIONSHIP,		// the path does not cross this cell
		ENDING_CELL,			// the path ends in this cell	
		EXITING_CELL,			// the path exits this cell through side X
	};

	// ----- CREATORS ---------------------

	inline NavigationCell():m_SessionID(0){};
	inline NavigationCell( const NavigationCell& Src){*this = Src;};
	inline ~NavigationCell(){};

	// ----- OPERATORS --------------------
	NavigationCell& operator=( const NavigationCell& Src);

	// ----- MUTATORS ---------------------
	void Initialize(const vector3& PointA, const vector3& PointB, const vector3& PointC);
	void ComputeCellData();
	bool RequestLink(const vector3& PointA, const vector3& PointB, NavigationCell* Caller);
	void SetLink(CELL_SIDE Side, NavigationCell* Caller);

	PATH_RESULT ClassifyPathToCell(const Line2D& MotionPath, NavigationCell** pNextCell, CELL_SIDE& Side, vector2* pPointOfIntersection)const;

	void ProjectPathOnCellWall(CELL_SIDE SideNumber, Line2D& MotionPath)const;

	void MapVectorHeightToCell(vector3& MotionPoint)const;

	bool ForcePointToCellCollumn(vector3& TestPoint)const;
	bool ForcePointToCellCollumn(vector2& TestPoint)const;
	bool ForcePointToWallInterior(CELL_SIDE SideNumber, vector2& TestPoint)const;
	bool ForcePointToWallInterior(CELL_SIDE SideNumber, vector3& TestPoint)const;

	bool ProcessCell(NavigationHeap* pHeap);
	bool QueryForPath(NavigationHeap* pHeap, NavigationCell* Caller, float arrivalcost);

	// ----- ACCESSORS --------------------

	bool IsPointInCellCollumn(const vector3& TestPoint)const;
	bool IsPointInCellCollumn(const vector2& TestPoint)const;
	const vector3& Vertex(int Vert)const;
	const vector3& CenterPoint()const;
	NavigationCell* Link(int Side)const;

	const vector3& LinkPoint()const;

	float ArrivalCost()const;
	float Heuristic()const;
	float PathfindingCost()const;

	int ArrivalWall()const;
	const vector3 WallMidpoint(int Side)const;

private:

	// ----- DATA -------------------------
	Plane	m_CellPlane;		// A plane containing the cell triangle
	vector3 m_Vertex[3];		// pointers to the verticies of this triangle held in the NavigationMesh's vertex pool
	vector3 m_CenterPoint;		// The center of the triangle
	Line2D	m_Side[3];			// a 2D line representing each cell Side
	NavigationCell* m_Link[3];// pointers to cells that attach to this cell. A NULL link denotes a solid edge.

	// Pathfinding Data...

	int		m_SessionID;		// an identifier for the current pathfinding session.
	float	m_ArrivalCost;		// total cost to use this cell as part of a path
	float	m_Heuristic;		// our estimated cost to the goal from here
	bool	m_Open;				// are we currently listed as an Open cell to revisit and test?
	int		m_ArrivalWall;		// the side we arrived through.
	vector3 m_WallMidpoint[3];	// the pre-computed midpoint of each wall.
	float	m_WallDistance[3];	// the distances between each wall midpoint of sides (0-1, 1-2, 2-0)
  
	// ----- HELPER FUNCTIONS -------------
	void ComputeHeuristic(const vector3& Goal); // estimates the distance to the goal for A*

	// ----- UNIMPLEMENTED FUNCTIONS ------

};

//- Inline Functions ---------------------------------------------------------------------

//= CREATORS =============================================================================

//= OPERATORS ============================================================================
inline NavigationCell& NavigationCell::operator=( const NavigationCell& Src)
{
	if (this != &Src)
	{
		m_CellPlane = Src.m_CellPlane;		
		m_CenterPoint = Src.m_CenterPoint;	
		m_SessionID= Src.m_SessionID;
		m_ArrivalCost= Src.m_ArrivalCost;
		m_Heuristic= Src.m_Heuristic;
		m_Open= Src.m_Open;
		m_ArrivalWall= Src.m_ArrivalWall;

		for (int i=0;i<3;i++)
		{
			m_Vertex[i] = Src.m_Vertex[i];
			m_Side[i] = Src.m_Side[i];
			m_Link[i] = Src.m_Link[i];
			m_WallMidpoint[i] = Src.m_WallMidpoint[i];
			m_WallDistance[i] = Src.m_WallDistance[i];
		}
	}
	return (*this);
}

//= MUTATORS =============================================================================

//:	Initialize
//----------------------------------------------------------------------------------------
//
//	Initialize our Cell object, given it's vertices in clockwise order 
//
//-------------------------------------------------------------------------------------://
inline void NavigationCell::Initialize(const vector3& PointA, const vector3& PointB, const vector3& PointC)
{
	m_Vertex[VERT_A] = PointA;
	m_Vertex[VERT_B] = PointB;
	m_Vertex[VERT_C] = PointC;

	// object must be re-linked
	m_Link[SIDE_AB] = 0;
	m_Link[SIDE_BC] = 0;
	m_Link[SIDE_CA] = 0;

	// now that the vertex pointers are set, compute additional data about the Cell
	ComputeCellData();
}

//:	ComputeCellData
//----------------------------------------------------------------------------------------
//
//	Compute additional data about the cell used for navigation tests and pathfinding 
//
//-------------------------------------------------------------------------------------://
inline void NavigationCell::ComputeCellData()
{
	// create 2D versions of our verticies
	vector2 Point1(m_Vertex[VERT_A].x,m_Vertex[VERT_A].z);
	vector2 Point2(m_Vertex[VERT_B].x,m_Vertex[VERT_B].z);
	vector2 Point3(m_Vertex[VERT_C].x,m_Vertex[VERT_C].z);

	// innitialize our sides
	m_Side[SIDE_AB].SetPoints(Point1,Point2);	// line AB
	m_Side[SIDE_BC].SetPoints(Point2,Point3);	// line BC
	m_Side[SIDE_CA].SetPoints(Point3,Point1);	// line CA

	m_CellPlane.Set(m_Vertex[VERT_A],m_Vertex[VERT_B],m_Vertex[VERT_C]);

	// compute midpoint as centroid of polygon
	m_CenterPoint.x=((m_Vertex[VERT_A].x + m_Vertex[VERT_B].x + m_Vertex[VERT_C].x)/3);
	m_CenterPoint.y=((m_Vertex[VERT_A].y + m_Vertex[VERT_B].y + m_Vertex[VERT_C].y)/3);
	m_CenterPoint.z=((m_Vertex[VERT_A].z + m_Vertex[VERT_B].z + m_Vertex[VERT_C].z)/3);

	// compute the midpoint of each cell wall
	m_WallMidpoint[0].x = (m_Vertex[VERT_A].x + m_Vertex[VERT_B].x)/2.0f;
	m_WallMidpoint[0].y = (m_Vertex[VERT_A].y + m_Vertex[VERT_B].y)/2.0f;
	m_WallMidpoint[0].z = (m_Vertex[VERT_A].z + m_Vertex[VERT_B].z)/2.0f;

	m_WallMidpoint[1].x = (m_Vertex[VERT_C].x + m_Vertex[VERT_B].x)/2.0f;
	m_WallMidpoint[1].y = (m_Vertex[VERT_C].y + m_Vertex[VERT_B].y)/2.0f;
	m_WallMidpoint[1].z = (m_Vertex[VERT_C].z + m_Vertex[VERT_B].z)/2.0f;

	m_WallMidpoint[2].x = (m_Vertex[VERT_C].x + m_Vertex[VERT_A].x)/2.0f;
	m_WallMidpoint[2].y = (m_Vertex[VERT_C].y + m_Vertex[VERT_A].y)/2.0f;
	m_WallMidpoint[2].z = (m_Vertex[VERT_C].z + m_Vertex[VERT_A].z)/2.0f;

	// compute the distances between the wall midpoints
	vector3 WallVector;
	WallVector = m_WallMidpoint[0] - m_WallMidpoint[1];
	m_WallDistance[0] = WallVector.length();

	WallVector = m_WallMidpoint[1] - m_WallMidpoint[2];
	m_WallDistance[1] = WallVector.length();

	WallVector = m_WallMidpoint[2] - m_WallMidpoint[0];
	m_WallDistance[2] = WallVector.length();

}

//:	RequestLink
//----------------------------------------------------------------------------------------
//
//	Navigation Mesh is created as a pool of raw cells. The cells are then compared against 
//	each other to find common edges and create links. This routine is called from a 
//	potentially adjacent cell to test if a link should exist between the two. 
//
//-------------------------------------------------------------------------------------://
inline bool NavigationCell::RequestLink(const vector3& PointA, const vector3& PointB, NavigationCell* Caller)
{
	// return true if we share the two provided verticies with the calling cell.
	if (m_Vertex[VERT_A] == PointA)
	{
		if (m_Vertex[VERT_B] == PointB)
		{
			m_Link[SIDE_AB] = Caller;
			return (true);
		}
		else if (m_Vertex[VERT_C] == PointB)
		{
			m_Link[SIDE_CA] = Caller;
			return (true);
		}
	}
	else if (m_Vertex[VERT_B] == PointA)
	{
		if (m_Vertex[VERT_A] == PointB)
		{
			m_Link[SIDE_AB] = Caller;
			return (true);
		}
		else if (m_Vertex[VERT_C] == PointB)
		{
			m_Link[SIDE_BC] = Caller;
			return (true);
		}
	}
	else if (m_Vertex[VERT_C] == PointA)
	{
		if (m_Vertex[VERT_A] == PointB)
		{
			m_Link[SIDE_CA] = Caller;
			return (true);
		}
		else if (m_Vertex[VERT_B] == PointB)
		{
			m_Link[SIDE_BC] = Caller;
			return (true);
		}
	}

	// we are not adjacent to the calling cell
	return (false);
}

//:	SetLink
//----------------------------------------------------------------------------------------
//
//	Sets a link to the calling cell on the enumerated edge. 
//
//-------------------------------------------------------------------------------------://
inline void NavigationCell::SetLink(CELL_SIDE Side, NavigationCell* Caller)
{
	m_Link[Side] = Caller;
}

//:	MapVectorHeightToCell
//----------------------------------------------------------------------------------------
//
//	Uses the X and Z information of the vector to calculate Y on the cell plane 
//
//-------------------------------------------------------------------------------------://
inline void NavigationCell::MapVectorHeightToCell(vector3& MotionPoint)const
{
	MotionPoint.y= m_CellPlane.SolveForY(MotionPoint.x, MotionPoint.z);
}

//= ACCESSORS ============================================================================

//:	IsPointInCellCollumn
//----------------------------------------------------------------------------------------
//
//	Test to see if a 2D point is within the cell. There are probably better ways to do 
//	this, but this seems plenty fast for the time being. 
//
//-------------------------------------------------------------------------------------://
inline bool NavigationCell::IsPointInCellCollumn(const vector2& TestPoint) const
{
	// we are "in" the cell if we are on the right hand side of all edge lines of the cell
	int InteriorCount = 0;

	for (int i=0;i<3;i++)
	{
		Line2D::POINT_CLASSIFICATION SideResult = m_Side[i].ClassifyPoint(TestPoint);

		if (SideResult != Line2D::LEFT_SIDE)
		{
			InteriorCount++;
		}
	}

	return (InteriorCount == 3);
}

//:	IsPointInCellCollumn
//----------------------------------------------------------------------------------------
//
//	Test to see if a 3D point is within the cell by projecting it down to 2D and calling 
//	the above method. 
//
//-------------------------------------------------------------------------------------://
inline bool NavigationCell::IsPointInCellCollumn(const vector3& TestPoint) const
{
	vector2 TestPoint2D(TestPoint.x,TestPoint.z);

	return (IsPointInCellCollumn(TestPoint2D));
}

inline const vector3& NavigationCell::Vertex(int Vert)const
{
	return(m_Vertex[Vert]);
}

inline const vector3& NavigationCell::CenterPoint()const
{
	return(m_CenterPoint);
}

inline NavigationCell* NavigationCell::Link(int Side)const
{
	return(m_Link[Side]);
}

inline float NavigationCell::ArrivalCost()const
{
	return(m_ArrivalCost);
}

inline float NavigationCell::Heuristic()const
{
	return(m_Heuristic);
}

inline float NavigationCell::PathfindingCost()const
{
	return(m_ArrivalCost + m_Heuristic);
}

inline int NavigationCell::ArrivalWall()const
{
	return(m_ArrivalWall);
}

inline const vector3 NavigationCell::WallMidpoint(int Side)const
{
	return(m_WallMidpoint[Side]);
}

//- End of NavigationCell ----------------------------------------------------------------

//****************************************************************************************

#endif  // end of file      ( NavigationCell.h )

