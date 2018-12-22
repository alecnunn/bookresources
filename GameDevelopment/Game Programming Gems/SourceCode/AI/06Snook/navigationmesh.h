/* Copyright (C) Greg Snook, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg Snook, 2000"
 */
#ifndef NAVIGATIONMESH_H
#define NAVIGATIONMESH_H
/****************************************************************************************\
	NavigationMesh.h

	NavigationMesh component interface for the Navimesh sample program.
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

#ifndef NAVIGATIONHEAP_H
#include "navigationheap.h"
#endif

#ifndef STD_VECTOR_H
#define STD_VECTOR_H
#pragma warning(disable : 4786)
#include <vector>
#endif

#include <GL/glut.h>

// forward declaration required
class NavigationPath;

/*	NavigationMesh
------------------------------------------------------------------------------------------
	
	A NavigationMesh is a collecion of NavigationCells used to control object movement while
	also providing path finding line-of-sight testing. It serves as a parent to all the
	Actor objects which exist upon it.
	
------------------------------------------------------------------------------------------
*/

class NavigationMesh
{
public:

	// ----- ENUMERATIONS & CONSTANTS -----
	typedef	std::vector<NavigationCell*> CELL_ARRAY;

	// ----- CREATORS ---------------------

	NavigationMesh();
	~NavigationMesh();

	// ----- OPERATORS --------------------

	// ----- MUTATORS ---------------------
	void Clear();
	void AddCell(const vector3& PointA, const vector3& PointB, const vector3& PointC);
	void LinkCells();

	vector3 SnapPointToCell(NavigationCell* Cell, const vector3& Point);
	vector3 SnapPointToMesh(NavigationCell** CellOut, const vector3& Point);
	NavigationCell* FindClosestCell(const vector3& Point)const;

	bool LineOfSightTest(NavigationCell* StartCell, const vector3& StartPos, NavigationCell* EndCell, const vector3& EndPos);
	bool BuildNavigationPath(NavigationPath& NavPath, NavigationCell* StartCell, const vector3& StartPos, NavigationCell* EndCell, const vector3& EndPos);

	void ResolveMotionOnMesh(const vector3& StartPos, NavigationCell* StartCell, vector3& EndPos, NavigationCell** EndCell);

	void Update(float elapsedTime = 1.0f);
	void Render();

	// ----- ACCESSORS --------------------
	int TotalCells()const;
	NavigationCell* Cell(int index);

private:

	// ----- DATA -------------------------
	CELL_ARRAY m_CellArray; // the cells that make up this mesh

	// path finding data...
	int m_PathSession;
	NavigationHeap m_NavHeap;

	// ----- HELPER FUNCTIONS -------------

	// ----- UNIMPLEMENTED FUNCTIONS ------

	NavigationMesh( const NavigationMesh& Src);
	NavigationMesh& operator=( const NavigationMesh& Src);

};

//- Inline Functions ---------------------------------------------------------------------

//= CREATORS =============================================================================

/*	NavigationMesh
------------------------------------------------------------------------------------------
	
	Default Object Constructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationMesh::NavigationMesh()
:m_PathSession(0)
{
	m_CellArray.clear();
}

/*	~NavigationMesh
------------------------------------------------------------------------------------------
	
	Default Object Destructor
	
------------------------------------------------------------------------------------------
*/
inline NavigationMesh::~NavigationMesh()
{
	Clear();
}

//= OPERATORS ============================================================================

//= MUTATORS =============================================================================

//:	Clear
//----------------------------------------------------------------------------------------
//
//	Delete all cells associated with this mesh 
//
//-------------------------------------------------------------------------------------://
inline void NavigationMesh::Clear()
{
	CELL_ARRAY::iterator	CellIter = m_CellArray.begin();
	for(;CellIter != m_CellArray.end(); ++CellIter)
	{
		NavigationCell* Cell = *CellIter;
		delete Cell;
	}

	m_CellArray.clear();
}

//:	AddCell
//----------------------------------------------------------------------------------------
//
//	Add a new cell, defined by the three vertices in clockwise order, to this mesh 
//
//-------------------------------------------------------------------------------------://
inline void NavigationMesh::AddCell(const vector3& PointA, const vector3& PointB, const vector3& PointC)
{
	NavigationCell* NewCell = new NavigationCell;
	NewCell->Initialize(PointA, PointB, PointC);
	m_CellArray.push_back(NewCell);
}

//:	Render
//----------------------------------------------------------------------------------------
//
//	Render the mesh geometry to screen. The mesh is assumed to exist in world corrdinates 
//	for the purpose of this demo 
//
//-------------------------------------------------------------------------------------://
inline void NavigationMesh::Render()
{
    glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);

	glBegin(GL_TRIANGLES);

	// render each cell triangle
	CELL_ARRAY::const_iterator	iter = m_CellArray.begin();
	for(;iter != m_CellArray.end(); ++iter)
	{
		const NavigationCell* Cell = *iter;
		int i;

		for (i=0;i<3;++i)
		{
			char shade = (char)Cell->Vertex(i).y;
			shade = 128+shade;

			glColor4ub(shade, shade, shade, 0xff);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(Cell->Vertex(i).x, Cell->Vertex(i).y, Cell->Vertex(i).z);
		}
	}

	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);

	// render cell edges as wireframe for added visibility
	iter = m_CellArray.begin();
	for(;iter != m_CellArray.end(); ++iter)
	{
		const NavigationCell* Cell = *iter;
		int i;

		for (i=0;i<3;++i)
		{
			char shade = (char)Cell->Vertex(i).y;
			shade = 64+shade;

			glColor4ub(shade, shade, shade, 0x80);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(Cell->Vertex(i).x, Cell->Vertex(i).y + 0.2, Cell->Vertex(i).z);
		}
	}

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   
	glPopMatrix();
}

//:	Update
//----------------------------------------------------------------------------------------
//
//	Does noting at this point. Stubbed for future use in animating the mesh 
//
//-------------------------------------------------------------------------------------://
inline void NavigationMesh::Update(float elapsedTime)
{
}


//= ACCESSORS ============================================================================
inline int NavigationMesh::TotalCells()const
{
	return((int)m_CellArray.size());
}

inline NavigationCell* NavigationMesh::Cell(int index)
{
	return(m_CellArray.at(index));
}


//- End of NavigationMesh ----------------------------------------------------------------

//****************************************************************************************

#endif  // end of file      ( NavigationMesh.h )

