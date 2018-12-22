//
// Polygon.h
//

#ifndef _POLYGON_H_
#define _POLYGON_H_

#include "MathStructs.h"

// Width of grid used when snapping vertices
const float	GRID_SCALE				= 0.01f;

// Polygon split result flags
const DWORD	HAS_FRONT				= 0x01;
const DWORD	HAS_BACK				= 0x02;

class CSphereTreeNode;
class CBSPNode;
class CBSPTree;

struct CTriangle
{

	bool		bValid;
	CVector		Vertices[3];
	CPlane		Plane;

};

// Polygon vertex - see CPolygon for details

class CPolygonVertex
{

public :

	CVector				m_vPosition;			// Vertex position

	CPolygonVertex*		m_pPrev;				// Previous vertex in polygon's vertex linked list
	CPolygonVertex*		m_pNext;				// Next vertex in polygon's vertex linked list

	// Constructor
	CPolygonVertex();

	// Equivalence operators
	inline bool			operator = ( const CPolygonVertex& Vertex );
	inline bool			operator == ( const CPolygonVertex& Vertex );

};

// Polygon class
//
// A polygon contains a linked list of individually allocated vertices - thus the number of vertices on each
// polygon is variable.
//
// The polygon stores its own bounding sphere and its facet plane.
//
// It has function to be split by a plane and functions for linking it to the sphere tree structures

class CPolygon
{

public :

	enum ePolyPlaneSides
	{
		epsFRONT	= 0,	// Polygon is in front of plane
		epsBACK		= 1,	// Polygon is in back of plane
		epsALIGNED	= 2,	// Polygon is aligned with plane
		epsBOTH		= 3,	// Polygon straddles the plane
		epsUNKNOWN	= 4		// Status unknown
	};

	// Constructor/Destructor
	CPolygon();
	virtual ~CPolygon();

	// Copies the vertices from another polygon.
			void	CopyFrom( CPolygon* pPolygon );

	// Computes the bounding sphere of the polygon.
			void	ComputeBounds();

	// Computes the facet plane of the polygon.
			void	ComputePlane();

	// Returns on which side of a plane the polygon lies.
	//
	// Paameters:
	//		Plane				Plane to test with
	//		bAlignedIsFront		Determines if vertices aligned with the plane will be considered as being
	//							on the front side of the plane
	ePolyPlaneSides	GetSide( const CPlane& Plane, bool bAlignedIsFront = true, float fEpsilon = EPSILON );

	// Allocates enough memory for the specified vertex count.
			bool	SetVertexCount( DWORD uNewVertexCount );

	// Snaps a float to grid of spacing GRID_SCALE
	static	void	SnapFloat( float& fValue );

	// Snaps a vector to grid of spacing GRID_SCALE
	static	void	SnapVertex( CVector& vVec );

	// Splits the polygon in two with the specified plane.
	//
	// Parameters:
	//		Plane				Plane to split with
	//		pFront				Resulting front polygon
	//		pBack				Resulting back polygon
	//
	// Returns a combination of the HAS_FRONT and HAS_BACK flags indicating whether the source polygon
	// has a front and/or back portion. Epsilon value canbe controlled through fEpsilon parameter.
			DWORD	Split(	const CPlane& Plane, CPolygon* pFront, CPolygon* pBack, 
							float fEpsilon = EPSILON );

	//---------------------------------------------------------------------------------------------
	// For debugging. Test if facet plane is well aligned with polygon
			bool	TestPlaneTolerance();

	//---------------------------------------------------------------------------------------------
			void	LinkAsPrev( CPolygon*& pNewNext, CPolygon*& pOldFirst );
			void	Unlink( CPolygon*& pOldFirst );

	DWORD				m_uVertexCount;		// Vertex count
	CPolygonVertex*		m_pVertices;		// Vertices
	
	CSphere				m_Bounds;			// Bounding sphere		:TODO: Verify this

	CPlane				m_Plane;			// Face plane

	CPolygon*			m_pPrev;			// Previous polygon in sphere tree linked list
	CPolygon*			m_pNext;			// Next polygon in sphere tree linked list
	CSphereTreeNode*		m_pParentNode;		// Parent sphere tree nodes

	CPolygon*			m_pNextCoplanar;	// Next coplanar polygon, if any

};

// Rounds a value to the nearest unit
inline int Round( float fValue )
{
	return (int)floor( fValue + 0.5 );
}

inline bool	CPolygonVertex::operator = ( const CPolygonVertex& Vertex )
{
	m_vPosition = Vertex.m_vPosition;
	return true;
}

inline bool CPolygonVertex::operator == ( const CPolygonVertex& Vertex )
{
	return m_vPosition == Vertex.m_vPosition;
}

inline void CPolygon::SnapFloat( float& fValue )
{
	int iValue;

	fValue *= 1.0f / GRID_SCALE;
	iValue = ( int ) floor (fValue + 0.5);

	fValue = ( float ) iValue;
	fValue *= GRID_SCALE;
}

inline void CPolygon::SnapVertex( CVector& vVec )
{
	SnapFloat( vVec.fX );
	SnapFloat( vVec.fY );
	SnapFloat( vVec.fZ );
}

#endif // _POLYGON_H_