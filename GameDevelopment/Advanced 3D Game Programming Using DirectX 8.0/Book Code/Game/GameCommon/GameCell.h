/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameCell.h
 *    Desc: convex cell that can contain a set of objects
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _GAMECELL_H
#define _GAMECELL_H

#include "..\gamelib\GameTypes.h"
#include "ResourceMgr.h"
#include "MsgDaemon.h"

#include <vector>
#include <list>
#include <algorithm>
using namespace std;

#include "..\math3d\point3.h"
#include "..\math3d\polygon.h"
#include "..\gamelib\file.h"
#include "ViewCone.h"

class cGameEnt;
class cGameWorld;

#define MAX_CELL_POLY_VERTS 12



/**
 * cGameCell
 * This class represents a cell in the world.  It is conceptually a 
 * convex space, but polygons may exist that make it un-convex 
 * (this is left for the physics engine to deal with)
 */
class cGameCell :
	public iGameObject
{
	friend bool DrawCell( cGameCell* pCell );

protected:
	int m_index; // for debugging

	list< cGameEnt* >	m_objectsInCell;

	struct sPolyIndex
	{
		ushort	m_ind;
		ulong	m_col;
		float			m_u;
		float			m_v;
	};

	struct sPolygon
	{
		sPolyIndex	m_vList[ MAX_CELL_POLY_VERTS ];
		int			m_nVerts;
		plane3		m_plane;
		resID		m_texID;
	};

	struct sPortal
	{
		sPolyIndex	m_vList[ MAX_CELL_POLY_VERTS ];
		int			m_nVerts;
		ulong		m_color;
		plane3		m_plane;
		objID		m_other;

		/**
		 * list of the planes that make up the edges.
		 * this is used in collision detection
		 */
		vector<plane3>	m_edgePlanes;
		// called after the portal is initialized
		void CalcEdgePlanes( cGameCell* pParent ); 
		bool ContainsSphere( const bSphere3& sphere );
		bool ContainsPoint( const point3& pt, float maxDist );

		int SnapInside( cGameEnt* pObj );

		cGameCell* GetOtherCell()
		{
			return (cGameCell*)MsgDaemon()->Get( m_other );
		}
	};

	vector< point3 >	m_ptList;
	vector< sPolygon >	m_polyList;
	vector< sPortal >	m_portalList;

	/**
	 * Before world traversal, we set all of these to false.
	 * since our graph can have cycles, this prevents 
	 * infintie recursion by only considering cells once,
	 * when they haven't been traversed.
	 */
	int m_travNum;

	/**
	 * When we're traversing the graph searching (for collision
	 * candidates, for example), only traverse a specified amount
	 */
	static int m_searchDepth;

	static int m_currTravNum;

	/**
	 * This is the unique ID that identifies this object
	 */
	objID	m_id;

public:

	cGameCell( cFile& file, int num );
	~cGameCell();

	//void Draw();
	void Tick( float tickLength );

	//==========--------------------------  Processing functions

	void GetCellObjects( list< cGameEnt* >* pObjList );

	void AddObject( cGameEnt* pObj );

	bool ListsObject( cGameEnt* pObj );
	bool ContainsObject( cGameEnt* pObj );
	
	void RemoveObject( cGameEnt* pObj );

	void PortalWalk( vector< cGameCell* >* pCellList, cViewCone& inCone, bool bFirst = true );
	void Traverse( list< cGameCell* >* pCellList, int depth, bool bFirst = true );

	/**
	 * When we intersect walls, we want to be exact, i.e. we
	 * don't let the sphere intersect the wall at all.  However,
	 * to allow passage between cells, we need to see if passage
	 * through a portal would put us in another cell, so the 
	 * inexact thing checks if /any/ part of the sphere lies in
	 * the cell
	 */
	bool ContainsSphere( bSphere3& sphere, bool exact );
	bool ContainsPoint( const point3& point );

	bool PortalContainsSphere( bSphere3& sphere, int portalNum )
	{
		return m_portalList[portalNum].ContainsSphere( sphere );
	}

	/**
	 * PointInAnyPortal:
	 * Assumes that the point is inside the cell, and that the
	 * sphere was intersecting some wall.  Pretty meaningless
	 * if the point is happily floating in the middle of the cell.
	 * this code path exists as a way to avoid the problem of
	 * objects needing to pass through doorways
	 */
	int PointInAnyPortal( const point3& pt, float maxDist );

	/**
	 * This function assumes that a sphere center is inside
	 * the cell.  It checks to see if it's standing inside 
	 * any of the doorways.  Used in the collision detection
	 * code.
	 */
	bool ContainedInPortal( bSphere3& sphere );

	/**
	 * Take an object and snap it's position
	 * such that it is still inside the cell.
	 * used by the collision detection code.
	 * -1 means that the object got destroyed
	 * as a result of the collision step and
	 * shouldn't be touched anymore.
	 */
	int SnapInside( cGameEnt* pObj );
	int SnapInsidePortal( cGameEnt* pObj, int portalNum )
	{
		return m_portalList[portalNum].SnapInside( pObj );
	}


	bool SphereInPortal( const bSphere3& sphere, int portalNum );

	friend class cGameWorld;
	friend struct sPortal;
	friend struct sPolygon;

	//==========--------------------------  iGameObject
	virtual objID GetID();
	virtual void SetID( objID id );
	virtual uint ProcMsg( const sMsg& msg );

	// Function for for_each use.
	static bool DrawCell( cGameCell* pCell )
	{
/*		if( pCell )
			pCell->Draw();*/
		return true;
	}

};


#endif // _GAMECELL_H
