/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#ifndef NAVIGATIONPATH_H
#define NAVIGATIONPATH_H
/****************************************************************************************\
	NavigationPath.h

	NavigationPath component interface for the Navimesh sample program.
	Included as part of the Game Programming Gems sample code.

	Created 3/18/00 by Greg Snook
	greg@mightystudios.com
    ------------------------------------------------------------------------------------- 
	Notes/Revisions:

\****************************************************************************************/
#ifndef _MTXLIB_H
#include "mtxlib.h"
#endif

#ifndef NAVIGATIONCELL_H
#include "navigationcell.h"
#endif

#ifndef NAVIGATIONMESH_H
#include "navigationmesh.h"
#endif

#ifndef STD_LIST_H
#define STD_LIST_H
#pragma warning(disable : 4786)
#include <list>
#endif

/*	NavigationPath
------------------------------------------------------------------------------------------
	
	NavigationPath is a collection of waypoints that define a movement path for an Actor.
	This object is ownded by an Actor and filled by NavigationMesh::BuildNavigationPath().
	
------------------------------------------------------------------------------------------
*/

// forward declaration of our parents
class NavigationPath
{
public:

	// ----- ENUMERATIONS & CONSTANTS -----

	// definition of a waypoint
	struct WAYPOINT
	{
		vector3 Position;		// 3D position of waypoint	
		NavigationCell* Cell;	// The cell which owns the waypoint
	};

	typedef std::list <WAYPOINT> WAYPOINT_LIST;
	typedef WAYPOINT_LIST::const_iterator WayPointID;

	// ----- CREATORS ---------------------

	NavigationPath();
	~NavigationPath();

	// ----- OPERATORS --------------------

	// ----- MUTATORS ---------------------
	void Setup(NavigationMesh* Parent, const vector3& StartPoint, NavigationCell* StartCell, const vector3& EndPoint, NavigationCell* EndCell);
	void AddWayPoint(const vector3& Point, NavigationCell* Cell);
	void EndPath();

	// ----- ACCESSORS --------------------
	NavigationMesh*			Parent()const;
	const WAYPOINT&			StartPoint()const;
	const WAYPOINT&			EndPoint()const;
	WAYPOINT_LIST&			WaypointList();
	WayPointID				GetFurthestVisibleWayPoint(const WayPointID& VantagePoint)const;

private:

	// ----- DATA -------------------------
	NavigationMesh*		m_Parent;
	WAYPOINT			m_StartPoint;
	WAYPOINT			m_EndPoint;
	WayPointID			m_EndID;
	WAYPOINT_LIST		m_WaypointList;

	// ----- HELPER FUNCTIONS -------------

	// ----- UNIMPLEMENTED FUNCTIONS ------
	NavigationPath( const NavigationPath& Src);
	NavigationPath& operator=( const NavigationPath& Src);


};

//- Inline Functions ---------------------------------------------------------------------

//= CREATORS =============================================================================

/*	NavigationPath
------------------------------------------------------------------------------------------
	
	Default Object Constructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationPath::NavigationPath()
{
}

/*	~NavigationPath
------------------------------------------------------------------------------------------
	
	Default Object Destructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationPath::~NavigationPath()
{
}


//= OPERATORS ============================================================================

//= MUTATORS =============================================================================

//:	Setup
//----------------------------------------------------------------------------------------
//
//	Sets up a new path from StartPoint to EndPoint. It adds the StartPoint as the first 
//	waypoint in the list and waits for further calls to AddWayPoint and EndPath to 
//	complete the list 
//
//-------------------------------------------------------------------------------------://
inline void NavigationPath::Setup(NavigationMesh* Parent, const vector3& StartPoint, NavigationCell* StartCell, const vector3& EndPoint, NavigationCell* EndCell)
{
	m_WaypointList.clear();

	m_Parent = Parent;
	m_StartPoint.Position = StartPoint;
	m_StartPoint.Cell = StartCell;
	m_EndPoint.Position = EndPoint;
	m_EndPoint.Cell = EndCell;

	// setup the waypoint list with our start and end points
	m_WaypointList.push_back(m_StartPoint);
}

//:	AddWayPoint
//----------------------------------------------------------------------------------------
//
//	Adds a new waypoint to the end of the list 
//
//-------------------------------------------------------------------------------------://
inline void NavigationPath::AddWayPoint(const vector3& Point, NavigationCell* Cell)
{
	WAYPOINT NewPoint;

	NewPoint.Position = Point;
	NewPoint.Cell = Cell;

	m_WaypointList.push_back(NewPoint);
}

//:	EndPath
//----------------------------------------------------------------------------------------
//
//	Caps the end of the waypoint list by adding our final destination point 
//
//-------------------------------------------------------------------------------------://
inline void NavigationPath::EndPath()
{
	// cap the waypoint path with the last endpoint
	m_WaypointList.push_back(m_EndPoint);
}

//= ACCESSORS ============================================================================
inline NavigationMesh* NavigationPath::Parent()const
{
	return(m_Parent);
}

inline const NavigationPath::WAYPOINT& NavigationPath::StartPoint()const
{
	return(m_StartPoint);
}

inline const NavigationPath::WAYPOINT&	NavigationPath::EndPoint()const
{
	return(m_EndPoint);
}

inline NavigationPath::WAYPOINT_LIST& NavigationPath::WaypointList()
{
	return(m_WaypointList);
}

//:	GetFurthestVisibleWayPoint
//----------------------------------------------------------------------------------------
//
//	Find the furthest visible waypoint from the VantagePoint provided. This is used to 
//	smooth out irregular paths. 
//
//-------------------------------------------------------------------------------------://
inline NavigationPath::WayPointID NavigationPath::GetFurthestVisibleWayPoint(const WayPointID& VantagePoint)const
{
	// see if we are already talking about the last waypoint
	if (VantagePoint == m_WaypointList.end())
	{
		return(VantagePoint);
	}

	const WAYPOINT& Vantage = *VantagePoint;
	WayPointID TestWaypoint = VantagePoint;
	++TestWaypoint;

	if (TestWaypoint == m_WaypointList.end())
	{
		return(TestWaypoint);
	}

	WayPointID VisibleWaypoint = TestWaypoint;
	++TestWaypoint;

	while (TestWaypoint != m_WaypointList.end())
	{
		const WAYPOINT& Test = *TestWaypoint;
		if (!m_Parent->LineOfSightTest(Vantage.Cell, Vantage.Position, Test.Cell, Test.Position))
		{
			return(VisibleWaypoint);
		}
		VisibleWaypoint = TestWaypoint;
		++TestWaypoint;	
	}
	return(VisibleWaypoint);
}

//- End of NavigationPath ----------------------------------------------------------------

//****************************************************************************************

#endif  // end of file      ( NavigationPath.h )

