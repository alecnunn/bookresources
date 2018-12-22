/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameCell.cpp
 *    Desc: convex cell that can contain a set of objects
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#include "stdafx.h"
#include "GameCell.h"
#include "GameWorld.h"
#include "Ents\GameEnt.h"

int cGameCell::m_searchDepth = 1;
int cGameCell::m_currTravNum = 0;

#include <string>

#define nextTok(x) x.front();assert(!x.empty());x.pop();

cGameCell::cGameCell( cFile& file, int num ) :
	m_index( num )
{
	queue<string> tokQueue;

	SetID( MakeID( c_cellSegment, num ) );
	MsgDaemon()->RegObject( GetID(), this );

	string tok;
	do
	{
		file.TokenizeNextNCLine( &tokQueue, '#' );

		tok = nextTok( tokQueue );

		if( tok == "CELL_START" )
		{
			// do nothing
		}

		else if( tok == "CELL_END" )
		{
			break;
		}

		else if( tok == "PT" )
		{
			point3 pt;

			// Read in the point
			tok = nextTok( tokQueue );
			pt.x = atof( tok.c_str() );

			tok = nextTok( tokQueue );
			pt.y = atof( tok.c_str() );

			tok = nextTok( tokQueue );
			pt.z = atof( tok.c_str() );

			// Append it to the list
			m_ptList.push_back( pt );
		}

		else if( tok == "FACE" )
		{
			tok = nextTok( tokQueue );

			int nInd = atoi( tok.c_str() );

			// Create the polygon structure to fill
			sPolygon currPoly;
			currPoly.m_nVerts = nInd;

			/**
			 * The next line is the indices.
			 */
			file.TokenizeNextNCLine( &tokQueue, '#' );

			// Fill in the indices
			for( int i=0; i< nInd; i++ )
			{
				tok = nextTok( tokQueue );
				currPoly.m_vList[i].m_ind = atoi( tok.c_str() );
			}

			// Now we can build the plane.

			currPoly.m_plane = plane3( 
				m_ptList[currPoly.m_vList[0].m_ind],
				m_ptList[currPoly.m_vList[1].m_ind],
				m_ptList[currPoly.m_vList[2].m_ind] );

			/**
			 * Next line is the colors
			 */
			file.TokenizeNextNCLine( &tokQueue, '#' );

			// Fill in the colors
			for( i=0; i< nInd; i++ )
			{
				tok = nextTok( tokQueue );
				sscanf( tok.c_str(), "%x", &currPoly.m_vList[i].m_col );
			}

			/**
			 * Final line is the texture info.
			 */
			file.TokenizeNextNCLine( &tokQueue, '#' );

			point3 m, n, p;

			// First token says whether to auto generate textures
			// Or explicitly read them.
			tok = nextTok( tokQueue );

			if( tok == "AUTO" )
			{
				// Automatically generate the texture vectors
				p = (m_ptList[currPoly.m_vList[0].m_ind]);

				tok = nextTok( tokQueue );
				currPoly.m_texID = atoi( tok.c_str() );

				tok = nextTok( tokQueue );
				float mScale = atof( tok.c_str() );

				tok = nextTok( tokQueue );
				float nScale = atof( tok.c_str() );

				// Run tests to figure out which way the plane faces
				if( point3::i == currPoly.m_plane.n || -1*point3::i == currPoly.m_plane.n )
				{
					// plane points in the +/-x direction
					m.Assign(0,0,1);
					n.Assign(0,1,0);
				}
				else if( point3::j == currPoly.m_plane.n || -1*point3::j == currPoly.m_plane.n )
				{
					// plane points in the +/-y direction
					m.Assign(0,0,1);
					n.Assign(1,0,0);
				}
				else if( point3::k == currPoly.m_plane.n || -1*point3::k == currPoly.m_plane.n )
				{
					// plane points in the +/-z direction
					m.Assign(1,0,0);
					n.Assign(0,1,0);
				}

				// No easy guess... just estimate using the first two points as one vector
				// and the cross as the other.
				else
				{
					m = m_ptList[currPoly.m_vList[1].m_ind] - m_ptList[currPoly.m_vList[0].m_ind];
					m.Normalize();
					n = m ^ currPoly.m_plane.n;
				}

				// scale the vectors by the provided scaling values.
				m *= mScale;
				n *= nScale;
			}
			else if( tok == "EXP" )
			{
				// Explicit tex-gen.  texture ID and then 9 floats (p,m,n).
				tok = nextTok( tokQueue );
				currPoly.m_texID = atoi( tok.c_str() );

				// P
				tok = nextTok( tokQueue );
				p.x = atof( tok.c_str() );

				tok = nextTok( tokQueue );
				p.y = atof( tok.c_str() );

				tok = nextTok( tokQueue );
				p.z = atof( tok.c_str() );

				// M
				tok = nextTok( tokQueue );
				m.x = atof( tok.c_str() );

				tok = nextTok( tokQueue );
				m.y = atof( tok.c_str() );

				tok = nextTok( tokQueue );
				m.z = atof( tok.c_str() );

				// N
				tok = nextTok( tokQueue );
				n.x = atof( tok.c_str() );

				tok = nextTok( tokQueue );
				n.y = atof( tok.c_str() );

				tok = nextTok( tokQueue );
				n.z = atof( tok.c_str() );

			}
			else throw cGameError( "Bad texture gen token" );

			float mMag = m.Mag();
			m /= mMag;

			float nMag = n.Mag();
			n /= nMag;

			// We have the n,m,p vectors; let's generate the coordinates.
			for( i=0; i<nInd; i++ )
			{
				point3 pt = m_ptList[currPoly.m_vList[i].m_ind] - p;

				float u = (pt * m);
				float v = (pt * n);

				currPoly.m_vList[i].m_u = u / mMag;
				currPoly.m_vList[i].m_v = v / nMag;
			}

			// Add the poly
			m_polyList.push_back( currPoly );

		}
		else if( 0 == tok.compare( "PORTAL" ) )
		{
			tok = nextTok( tokQueue );
			int other = atoi( tok.c_str() );

			tok = nextTok( tokQueue );
			int nInd = atoi( tok.c_str() );

			// Create the polygon structure to fill
			sPortal currPortal;
			currPortal.m_nVerts = nInd;

			// first # in the file is 1, here is 0
			currPortal.m_other = MakeID( c_cellSegment, other - 1);

			// Fill in the indices
			for( int i=0; i< nInd; i++ )
			{
				tok = nextTok( tokQueue );

				currPortal.m_vList[i].m_ind = atoi( tok.c_str() );
			}

			currPortal.m_plane = plane3( 
				m_ptList[currPortal.m_vList[0].m_ind],
				m_ptList[currPortal.m_vList[1].m_ind],
				m_ptList[currPortal.m_vList[2].m_ind] );

			// build the edge planes for this portal
			currPortal.CalcEdgePlanes( this );

			// Add the poly
			m_portalList.push_back( currPortal );
		}

	} while( 1 );
}

cGameCell::~cGameCell()
{
}

void cGameCell::GetCellObjects( list< cGameEnt* >* pObjList )
{
	/**
	 * Step through the list, adding to the array
	 */
	list<cGameEnt*>::iterator iter;
	for( iter = m_objectsInCell.begin(); iter != m_objectsInCell.end(); iter++ )
	{
		pObjList->push_back( *iter );
	}
}

void cGameCell::AddObject( cGameEnt* pObj )
{
	/**
	 * Make sure we don't have the object yet.
	 */
	assert( 0 == count(
		m_objectsInCell.begin(),
		m_objectsInCell.end(),
		pObj ) );

	m_objectsInCell.push_back( pObj );

	DP1("[cGameCell::AddObject]: Adding object %x\n", pObj );
}



bool cGameCell::ListsObject( cGameEnt* pObj )
{
	return (0==count(
		m_objectsInCell.begin(),
		m_objectsInCell.end(),
		pObj) ) ? false : true;
}


bool cGameCell::ContainsObject( cGameEnt* pObj )
{
	int i;
	point3 loc = MRetToPoint( pObj->ProcMsg( sMsg( msgGetLoc ) ) );
	float radius = 2.f;

	// FIXME: not checking to see if we're inside a portal.
	for( i=0; i<m_polyList.size(); i++ )
	{
		if( ptBack == m_polyList[i].m_plane.TestPoint(loc) )
		{
			return false;
		}
	}
	for( i=0; i<m_portalList.size(); i++ )
	{
		if( ptBack == m_portalList[i].m_plane.TestPoint(loc) )
		{
			return false;
		}
	}
	return true;
}



void cGameCell::RemoveObject( cGameEnt* pObj )
{
	m_objectsInCell.remove( pObj );
}

void cGameCell::PortalWalk( 
	vector< cGameCell* >* pCellList, 
	cViewCone& inCone, 
	bool bFirst )
{
	if( bFirst )
	{
		m_currTravNum++;
	}

	/**
	 * First, change out state to say that we have been traversed
	 */
	m_travNum = m_currTravNum;

	/**
	 * Add ourselves to the list
	 */
	pCellList->push_back( this );
	
	/**
	 * Test each portal against the view cone. 
	 * if the portal passes, generate a new view cone, add the neighbor
	 * to the list, and traverse it.
	 */
	static polygon< point3 > poly1( MAX_CELL_POLY_VERTS );
	static polygon< point3 > poly2( MAX_CELL_POLY_VERTS );

	for( int i=0; i< m_portalList.size(); i++ )
	{
		cGameCell* pOther = (cGameCell*)MsgDaemon()->Get( m_portalList[i].m_other );

		if( !pOther )
		{
			LogPrint("cGameCell::PortalWalk: bad cell");
			return;
		}
		
		if(	pOther->m_travNum == m_currTravNum )
		{
			// The other cell was already traversed.  skip it.
			continue;
		}

		// if the camera is too close to the portal, Assume that it's
		// not clipping the frustum and just traverse into the next 
		// cell.  This gets rid of the annoying popping effects.
		if( m_portalList[i].m_plane.TestPoint( inCone.GetLoc() ) == ptCoplanar )
		{
			pOther->PortalWalk( 
				pCellList,
				inCone,
				false );
		}

		/**
		 * Fill the polygon structure with the points of the portal
		 */
		poly1.nElem = m_portalList[i].m_nVerts;
		for( int j=0; j<poly1.nElem; j++ )
		{
			poly1.pList[j] = m_ptList[ m_portalList[i].m_vList[j].m_ind ];
		}

		/**
		 * Test the newly created polygon against the view cone
		 */
		if( inCone.Clip( poly1, &poly2 ) )
		{
			/**
			 * If we get in here, the polygon was visible from 
			 * the view cone.  poly2 is the part of the polygon that was
			 * visible.  Traverse into it.
			 */
			pOther->PortalWalk( 
				pCellList,
				cViewCone( inCone.GetLoc(), poly2 ),
				false );
		}
	}
}


void cGameCell::Traverse( list< cGameCell* >* pCellList, int depth, bool bFirst )
{
	if( bFirst )
	{
		m_currTravNum++; 
	}

	m_travNum = m_currTravNum;

	pCellList->push_back( this );
	if( depth <= 0 )
		return;
	for( int i=0; i<m_portalList.size(); i++ )
	{
		cGameCell* pCell = m_portalList[i].GetOtherCell();
		if( pCell->m_travNum != m_currTravNum )
		{
			pCell->Traverse( pCellList, depth-1, false );
		}
	}
}


bool cGameCell::ContainsPoint( const point3& point )
{
	int i;
	ePointLoc res;
	for( i=0; i<m_polyList.size(); i++ )
	{
		res = m_polyList[i].m_plane.TestPoint( point );
		if( res == ptBack )
		{
			return false;
		}
	}
	for( i=0; i<m_portalList.size(); i++ )
	{
		res = m_portalList[i].m_plane.TestPoint( point );
		if( res == ptBack )
		{
			return false;
		}
	}
	return true;
}


bool cGameCell::ContainsSphere( bSphere3& sphere, bool exact )
{
	int i;
	eSphereLoc res;
	for( i=0; i<m_polyList.size(); i++ )
	{
		res = m_polyList[i].m_plane.TestBSphere( sphere );
		if( exact )
		{
			if( res != sphereFront )
			{
				return false;
			}
		}
		else
		{
			if( res == sphereBack )
			{
				return false;
			}
		}
	}
	for( i=0; i<m_portalList.size(); i++ )
	{
		res = m_portalList[i].m_plane.TestBSphere( sphere );
		if( exact )
		{
			if( res != sphereFront )
			{
				return false;
			}
		}
		else
		{
			if( res == sphereBack )
			{
				return false;
			}
		}
	}
	return true;
}


int cGameCell::SnapInside( cGameEnt* pObj )
{
	plane3* pPlane;

	int i;

	for( i=0; i< m_polyList.size(); i++ )
	{
		pPlane = &m_polyList[i].m_plane;

		if( 1 == pObj->ProcMsg( sMsg( msgQueryInPlane, 0, *pPlane ) ) )
		{
			/**
			 * Tell the object to correct itself.
			 */
			if( -1 == pObj->ProcMsg( sMsg( msgNotifyPlaneCollision, 0, *pPlane ) ) )
			{
				// the object destroyed itself.
				return -1;
			}
		}
	}

	for( i=0; i< m_portalList.size(); i++ )
	{
		pPlane = &m_portalList[i].m_plane;

		if( 1 == pObj->ProcMsg( sMsg( msgQueryInPlane, 0, *pPlane ) ) )
		{
			/**
			 * Tell the object to correct itself.
			 */
			if( -1 == pObj->ProcMsg( sMsg( msgNotifyPlaneCollision, 0, *pPlane ) ) )
			{
				// the object destroyed itself.
				return -1;
			}
		}
	}
	return 0; // the object survived.
}


bool cGameCell::ContainedInPortal( bSphere3& sphere )
{
	int i;
	for( i=0; i<m_portalList.size(); i++ )
	{
		if( m_portalList[i].ContainsSphere( sphere ) )
		{
			return true;
		}
	}
	return false;
}

void cGameCell::sPortal::CalcEdgePlanes( cGameCell* pParent )
{
	int curr, next;
	for( curr=0; curr<m_nVerts; curr++ )
	{
		next = (curr+1) % m_nVerts;
		point3 edgeVec = pParent->m_ptList[ m_vList[next].m_ind ] - pParent->m_ptList[ m_vList[curr].m_ind ];

		point3 edgeNorm = m_plane.n ^ edgeVec;
		edgeNorm.Normalize();

		plane3 edgePlane = plane3( edgeNorm, pParent->m_ptList[ m_vList[curr].m_ind ] );
		m_edgePlanes.push_back( edgePlane );
	}
}

bool cGameCell::sPortal::ContainsSphere( const bSphere3& sphere )
{
	// skip any portals we're not close to.
	if( sphereCoplanar != m_plane.TestBSphere( sphere ) )
	{
		return false;
	}

	for( int i=0; i<m_nVerts; i++ )
	{
		if( sphereFront != m_edgePlanes[i].TestBSphere( sphere ) )
		{
			return false;
		}
	}
	return true;
}

bool cGameCell::sPortal::ContainsPoint( const point3& pt, float maxDist )
{
	// skip any portals we're not close to.
	float dist = m_plane.d + m_plane.n * pt;
	if( dist > maxDist || dist < 0 )
	{
		return false;
	}

	for( int i=0; i<m_nVerts; i++ )
	{
		if( ptFront != m_edgePlanes[i].TestPoint( pt ) )
		{
			return false;
		}
	}
	return true;
}

objID cGameCell::GetID()
{
	return m_id;
}


void cGameCell::SetID( objID id )
{
	m_id = id;
}


uint cGameCell::ProcMsg( const sMsg& msg )
{
	switch( msg.m_type )
	{
	case msgAddObjRef:
		AddObject( (cGameEnt*)MsgDaemon()->Get( msg.m_i[0] ) );
		break;
	case msgRemObjRef:
		RemoveObject( (cGameEnt*)MsgDaemon()->Get( msg.m_i[0] ) );
		break;
	default:
		DP0("Bad Message got to cGameCell\n");
		return -1;
	}
	return 0;
}


void Tick( float tickLength )
{

}


int cGameCell::PointInAnyPortal( const point3& pt, float maxDist  )
{
	for( int i=0; i<m_portalList.size(); i++ )
	{
		if( m_portalList[i].ContainsPoint( pt, maxDist ) )
		{
			return i;
		}
	}
	return -1; // not in any portal
}


int cGameCell::sPortal::SnapInside( cGameEnt* pObj )
{
	bSphere3 sphere = pObj->GetSphere();

	float rad = sphere.m_radius;
	point3 loc = sphere.m_loc;

	plane3* pPlane;

	int i;

	for( i=0; i< m_edgePlanes.size(); i++ )
	{
		pPlane = &m_edgePlanes[i];

		float dp = (loc * pPlane->n) + pPlane->d;

		if( 1 == pObj->ProcMsg( sMsg( msgQueryInPlane, 0, *pPlane ) ) )
		{
			/**
			 * Tell the object to correct itself.
			 */
			if( -1 == pObj->ProcMsg( sMsg( msgNotifyPlaneCollision, 0, *pPlane ) ) )
			{
				// The object destroyed itself
				return -1;
			}
		}
	}
	return 0;
}



bool cGameCell::SphereInPortal( const bSphere3& sphere, int portalNum )
{
	return m_portalList[portalNum].ContainsSphere( sphere );
}