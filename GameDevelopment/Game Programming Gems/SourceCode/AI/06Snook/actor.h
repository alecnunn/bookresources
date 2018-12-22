/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#ifndef Actor_H
#define Actor_H
/****************************************************************************************\
	Actor.h

	Actor component interface for the Navimesh sample program.
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

#ifndef NAVIGATIONPATH_H
#include "navigationpath.h"
#endif

#ifndef NAVIGATIONMESH_H
#include "navigationmesh.h"
#endif

#ifndef STD_VECTOR_H
#define STD_VECTOR_H
#pragma warning(disable : 4786)
#include <vector>
#endif

#include <GL/glut.h>

/*	Actor
------------------------------------------------------------------------------------------
	
	An Actor represents a mobile game object attached to a NavigationMesh. This sample 
	implementation demonstrates how to manuver an object on a NavigationMesh and request
	path information from the mesh.
	
------------------------------------------------------------------------------------------
*/

class Actor
{
public:

	// ----- ENUMERATIONS & CONSTANTS -----
	struct Triangle
	{
		vector3 vert[3];			// triangle vertices in clockwise order
		unsigned char color[3][4];	// 4 color values (RGBA) for each vertex
	};

	typedef	std::vector<Triangle> TRIANGLE_POOL;

	// ----- CREATORS ---------------------

	Actor();
	~Actor();

	// ----- OPERATORS --------------------

	// ----- MUTATORS ---------------------
	virtual void Create(NavigationMesh* Parent, const vector3& Position, NavigationCell* CurrentCell);
	virtual void AddTriangle(const Triangle& triangle);

	virtual void Update(float elapsedTime = 1.0f);
	virtual void Render();

	virtual void GotoRandomLocation();
	virtual void GotoLocation(const vector3& Position, NavigationCell* Cell);
	virtual void AddMovement(const vector3& Movement);
	virtual void SetMovement(const vector3& Movement);
	virtual void SetMovementX(float X);
	virtual void SetMovementY(float Y);
	virtual void SetMovementZ(float Z);
	virtual void SetMaxSpeed(float speed);

	// ----- ACCESSORS --------------------
	bool PathIsActive()const;
	const vector3& Position()const;
	const vector3& Movement()const;
	NavigationCell* CurrentCell()const;

private:

	// ----- DATA -------------------------
	NavigationMesh* m_Parent;		// the mesh we are sitting on
	NavigationCell* m_CurrentCell;	// our current cell on the mesh
	vector3 m_Position;				// our 3D control point position
	vector3 m_Movement;				// the current movement vector
	float m_MaxSpeed;				// our maximum traveling distance per frame

	TRIANGLE_POOL m_Geometry;		// a pile of triangles representing our object
	bool m_PathActive;				// true when we are using a path to navigate
	NavigationPath m_Path;			// our path object
	NavigationPath::WayPointID m_NextWaypoint; // ID of the next waypoint we will move to

	// ----- HELPER FUNCTIONS -------------

	// ----- UNIMPLEMENTED FUNCTIONS ------

	Actor( const Actor& Src);
	Actor& operator=( const Actor& Src);

};

//- Inline Functions ---------------------------------------------------------------------

//= CREATORS =============================================================================

/*	Actor
------------------------------------------------------------------------------------------
	
	Default Object Constructor
	
------------------------------------------------------------------------------------------
*/
inline Actor::Actor()
:m_PathActive(false)
,m_CurrentCell(0)
,m_Movement(0.0f,0.0f,0.0f)
,m_Position(0.0f,0.0f,0.0f)
,m_MaxSpeed(5.0f)
{
}

/*	~Actor
------------------------------------------------------------------------------------------
	
	Default Object Destructor
	
------------------------------------------------------------------------------------------
*/
inline Actor::~Actor()
{
}

//= OPERATORS ============================================================================

//= MUTATORS =============================================================================

//:	Create
//----------------------------------------------------------------------------------------
//
//	Initializes the Actor object, recording data about it's parent mesh, position and 
//	current cell 
//
//-------------------------------------------------------------------------------------://
inline void Actor::Create(NavigationMesh* Parent, const vector3& Position, NavigationCell* CurrentCell)
{
	m_Parent = Parent;
	m_Position = Position;
	m_CurrentCell = CurrentCell;

	m_Movement.x=m_Movement.y=m_Movement.z=0.0f;
	m_Geometry.clear();

	if (m_Parent)
	{
		// if no starting cell is provided, find one by searching the mesh
		if (!m_CurrentCell)
		{
			m_CurrentCell = m_Parent->FindClosestCell(m_Position);
		}

		// make sure our position is within the current cell
		m_Position =  m_Parent->SnapPointToCell(m_CurrentCell, m_Position);
	}
}

//:	AddTriangle
//----------------------------------------------------------------------------------------
//
//	Adds a Triangle to our Geometry pool 
//
//-------------------------------------------------------------------------------------://
inline void Actor::AddTriangle(const Triangle& triangle)
{
	m_Geometry.push_back(triangle);
}

//:	Render
//----------------------------------------------------------------------------------------
//
//	Renders the actor geometry, as well as the current path in use. 
//
//-------------------------------------------------------------------------------------://
inline void Actor::Render()
{
    glPushMatrix();
	glTranslatef(m_Position.x, m_Position.y, m_Position.z);
	glBegin(GL_TRIANGLES);

	//
	// Draw each triangle held in the pool
	//
	TRIANGLE_POOL::const_iterator	iter = m_Geometry.begin();
	for(;iter != m_Geometry.end(); ++iter)
	{
		const Triangle& Poly = *iter;

		// add each vertex of the polygon in clockwise order
		for (int i=0;i<3;++i)
		{
			glColor4ub(Poly.color[i][0], Poly.color[i][1], Poly.color[i][2], Poly.color[i][3]);
			glVertex3f(Poly.vert[i].x, Poly.vert[i].y, Poly.vert[i].z);
		}
	}

	glEnd();
    glPopMatrix();

	//
	// render the waypoint path if active
	//
	if (m_PathActive)
	{
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.0f); 
		glBegin(GL_LINES);

		//
		// For line drawing, we add 0.1 to all y values to make sure 
		// the lines are visible above the Navigation Mesh
		//

		// connect the waypoins to draw lines in blue
		NavigationPath::WAYPOINT_LIST::const_iterator iter = m_Path.WaypointList().begin();
		if (iter != m_Path.WaypointList().end())
		{
			NavigationPath::WAYPOINT LastWaypoint = *iter;
			++iter;
			for (;iter != m_Path.WaypointList().end();++iter)
			{
				const NavigationPath::WAYPOINT& waypoint= *iter;

				glColor4ub(0x00, 0x00, 0xff, 0xff);
				glVertex3f(LastWaypoint.Position.x, LastWaypoint.Position.y + 0.1, LastWaypoint.Position.z);

				glColor4ub(0x00, 0x00, 0xff, 0xff);
				glVertex3f(waypoint.Position.x, waypoint.Position.y + 0.1, waypoint.Position.z);

				LastWaypoint = waypoint;
			}
		}

		// draw our current heading in red
		glColor4ub(0xff, 0x00, 0x00, 0xff);
		glVertex3f(m_Position.x, m_Position.y + 0.1, m_Position.z);

		const NavigationPath::WAYPOINT& waypoint= *m_NextWaypoint;

		glColor4ub(0xff, 0x00, 0x00, 0xff);
		glVertex3f(waypoint.Position.x, waypoint.Position.y + 0.1, waypoint.Position.z);

		glEnd();
		glPopMatrix();
	}
}

//:	Update
//----------------------------------------------------------------------------------------
//
//	Move ourselves along the current path or movement vector 
//
//-------------------------------------------------------------------------------------://
inline void Actor::Update(float elapsedTime)
{
	float distance;
	float max_distance = m_MaxSpeed * elapsedTime;

	// if we have no parent mesh, return. We are simply a static object.
	if (!m_Parent)
	{
		return;
	}

	//
	// Figure out where we are going
	//
	if (m_PathActive)
	{
		//
		// Move along the waypoint path
		//
		if (m_NextWaypoint != m_Path.WaypointList().end())
		{
			// determine our desired movement vector
			m_Movement = (*m_NextWaypoint).Position;
			m_Movement -= m_Position;
		}
		else
		{
			// we have reached the end of the path
			m_PathActive = false;
			m_Movement.x=m_Movement.y=m_Movement.z=0.0f;
		}
	}
	else
	{
		// apply some friction to our current movement (if any)
		m_Movement.x *= 0.75f;
		m_Movement.y *= 0.75f;
		m_Movement.z *= 0.75f;
	}

	//
	// Adjust Movement
	//

	// scale back movement by our max speed if needed
	distance = m_Movement.length();
	if (distance > max_distance)
	{
		m_Movement.normalize();
		m_Movement *= max_distance;
	}

	// come to a full stop when we go below a certain threshold
	if (fabs(m_Movement.x) < 0.005f) m_Movement.x = 0.0f;
	if (fabs(m_Movement.y) < 0.005f) m_Movement.y = 0.0f;
	if (fabs(m_Movement.z) < 0.005f) m_Movement.z = 0.0f;

	//
	// Constrain any remaining Horizontal movement to the parent navigation rink
	//
	if (m_Movement.x || m_Movement.z)
	{
		//
		// Constrain any remaining Horizontal movement to the parent navigation rink
		//
		if (m_Parent)
		{
			// compute the next desired location
			vector3 NextPosition = m_Position + m_Movement;
			NavigationCell* NextCell;

			// test location on the NavigationMesh and resolve collisions
			m_Parent->ResolveMotionOnMesh(m_Position, m_CurrentCell, NextPosition, &NextCell);

			m_Position = NextPosition;
			m_CurrentCell = NextCell;
		}
	}
	else if (m_PathActive)
	{
		// If we have no remaining movement, but the path is active,
		// we have arrived at our desired waypoint.
		// Snap to it's position and figure out where to go next
		m_Position = (*m_NextWaypoint).Position;
		m_Movement.x = m_Movement.y = m_Movement.z = 0.0f;
		distance = 0.0f;
		m_NextWaypoint= m_Path.GetFurthestVisibleWayPoint(m_NextWaypoint);

		if (m_NextWaypoint == m_Path.WaypointList().end())
		{
			m_PathActive = false;
			m_Movement.x=m_Movement.y=m_Movement.z=0.0f;
		}
	}

}

//:	GotoLocation
//----------------------------------------------------------------------------------------
//
//	Find a path to the designated location on the mesh 
//
//-------------------------------------------------------------------------------------://
inline void Actor::GotoLocation(const vector3& Position, NavigationCell* Cell)
{
	if (m_Parent)
	{
		m_Movement.x = m_Movement.y=m_Movement.z = 0.0f;

		m_PathActive=m_Parent->BuildNavigationPath(m_Path, m_CurrentCell, m_Position, Cell, Position);

		if(m_PathActive)
		{
			m_NextWaypoint=m_Path.WaypointList().begin();
		}
	}
}

//:	GotoRandomLocation
//----------------------------------------------------------------------------------------
//
//	Pick a random location to move to on the mesh 
//
//-------------------------------------------------------------------------------------://
inline void Actor::GotoRandomLocation()
{
	if (m_Parent)
	{
		// pick a random cell and go there
		int index = rand() % m_Parent->TotalCells();
		NavigationCell* pCell = m_Parent->Cell(index);

		GotoLocation(pCell->CenterPoint(), pCell);
	}
}

//:	AddMovement
//----------------------------------------------------------------------------------------
//
//	Add a movement vector to our current motion 
//
//-------------------------------------------------------------------------------------://
inline void Actor::AddMovement(const vector3& Movement)
{
	m_Movement += Movement;
}

//:	SetMovement
//----------------------------------------------------------------------------------------
//
//	Set a movement vector as our current motion 
//
//-------------------------------------------------------------------------------------://
inline void Actor::SetMovement(const vector3& Movement)
{
	m_Movement += Movement;
}

//:	SetMovementX
//----------------------------------------------------------------------------------------
//
//	Add a movement value to our current motion in the x direction 
//
//-------------------------------------------------------------------------------------://
inline void Actor::SetMovementX(float X)
{
	m_Movement.x += X;
}

//:	SetMovementY
//----------------------------------------------------------------------------------------
//
//	Add a movement value to our current motion in the y direction 
//
//-------------------------------------------------------------------------------------://
inline void Actor::SetMovementY(float Y)
{
	m_Movement.y += Y;
}

//:	SetMovementZ
//----------------------------------------------------------------------------------------
//
//	Add a movement value to our current motion in the z direction 
//
//-------------------------------------------------------------------------------------://
inline void Actor::SetMovementZ(float Z)
{
	m_Movement.z += Z;
}

//:	SetMaxSpeed
//----------------------------------------------------------------------------------------
//
//	Set our maximum speed. This is the distance in world units we may travel per second. 
//
//-------------------------------------------------------------------------------------://
inline void Actor::SetMaxSpeed(float speed)
{
	m_MaxSpeed = speed;
}

//= ACCESSORS ============================================================================
inline bool Actor::PathIsActive()const
{
	return(m_PathActive);
}

inline const vector3& Actor::Position()const
{
	return(m_Position);
}

inline const vector3& Actor::Movement()const
{
	return(m_Movement);
}

inline NavigationCell* Actor::CurrentCell()const
{
	return(m_CurrentCell);
}

//- End of Actor ------------------------------------------------------------------

//****************************************************************************************

#endif  // end of file      ( Actor.h )

