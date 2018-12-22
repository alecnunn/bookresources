/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */
// Helper routines for manipulating large complex meshes.
// Pretty memory-hungry, but good for cross-referencing between
// edges, points and triangles.


#include "tomslib.h"


// Before including this file, #define the following:
//
// MESHPT_APP_DEFINED
// MESHTRI_APP_DEFINED
// MESHEDGE_APP_DEFINED
//
// These declarations are included in the declarations of the relevant
// classes. They may be defined to be nothing of course.
//
// For efficiency, also define some of the following according to use:
//
// MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS:
//		Set to 1 to enforce/assume that when adding a tri, all three edges
//		will exist. So all tris must have all three edge entries filled.
//		This is a speedup, and an error check, if that's what your app does.
//		The exception is if autocreation of edges is done for the MeshTri creator.
//		Similarly, before deleteing edges, make sure you have deleted all the
//		tris that use them
// MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_TRIS:
//		Ditto, but for pts.
// MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES:
//		Ditto, but pts must be added before edges are created.


#ifndef MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
#error Please define MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS to 0 or 1
#endif
#ifndef MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_TRIS
#error Please define MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_TRIS to 0 or 1
#endif
#ifndef MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES
#error Please define MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES to 0 or 1
#endif
#ifndef MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES
#error Please define MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES to 0 or 1
#endif



class MeshPt;
class MeshEdge;
class MeshTri;



class MeshTri
{
	friend class MeshPt;
	friend class MeshEdge;

private:
	DlinkDefine(MeshTri,List);
	DWORD		dwListId;	// For use when doing consistency checks.

	void InternalDelete ( BOOL bBinUnusedEdges );

public:
	MeshPt		*pPt1;		// Points.
	MeshPt		*pPt2;
	MeshPt		*pPt3;
	MeshEdge	*pEdge12;	// Edges between point numbers.
	MeshEdge	*pEdge23;
	MeshEdge	*pEdge31;

	DlinkMethods(MeshTri,List);

	MESHTRI_APP_DEFINED		// App-defined data.

	MeshTri ( void );
	// Set pEdgeListRoot to non-NULL to autocreate edges.
	MeshTri ( MeshPt *pNewPt1, MeshPt *pNewPt2, MeshPt *pNewPt3, MeshTri *pListRoot = NULL, MeshEdge *pEdgeListRoot = NULL );
	~MeshTri ( void );
	// Set bBinUnusedEdges to TRUE to autodestroy edges.
	Delete ( BOOL bBinUnusedEdges = FALSE );
	// Which list is this tri in?
	MeshTri *QueryList ( void );
	// Move this tri to this list.
	void SetList ( MeshTri *pListRoot );
	// Checks that all edges and pts refer back to this tri,
	// and that they're in the respective lists.
	// If the lists are NULL, the check is not made.
	bool ConsistencyCheck ( MeshPt *pPtRoot = NULL, MeshEdge *pEdgeRoot = NULL, MeshTri *pTriRoot = NULL );

#if MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
protected:
#else
public:
#endif
	// Add the edge to this tri.
	void AddEdge ( MeshEdge *pEdge );
	// Remove the edge from this tri.
	void RemoveEdge ( MeshEdge *pEdge );

#if MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_TRIS
protected:
#else
public:
#endif
	// Remove the point from this tri.
	// NOTE! This is probably not a good thing to do.
	void RemovePt ( MeshPt *pPt );

};


class MeshEdge
{
	friend class MeshPt;
	friend class MeshTri;

private:
	DlinkDefine(MeshEdge,List);
	DWORD		dwListId;	// For use when doing consistency checks.

public:
	MeshPt		*pPt1;
	MeshPt		*pPt2;
	MeshTri		*pTri12;		// Tri that numbers pt1, pt2 in that order.
	MeshTri		*pTri21;		// Tri that numbers pt2, pt1 in that order.

	MeshEdge	*pEdgeProx;		// The edge that this is close to, if any.

	DlinkMethods(MeshEdge,List);

	MESHEDGE_APP_DEFINED		// App-defined data.

	BINARY_HEAP_VARS();			// Helper stuff.

	MeshEdge ( void );
	MeshEdge ( MeshPt *pNewPt1, MeshPt *pNewPt2, MeshEdge *pListRoot = NULL );
	~MeshEdge ( void );
	// Find the other triangle that uses this edge.
	MeshTri *OtherTri ( MeshTri *pTri );
	// Find the other point that uses this edge.
	MeshPt *OtherPt ( MeshPt *pPt );
	// Try to merge these two edges. Result is TRUE if it succeeded - note that the other edge will be NOT deleted.
	bool bTryToMergeEdges ( MeshEdge *pedge );
	// Which list is this Edge in?
	MeshEdge *QueryList ( void );
	// Move this Edge to this list.
	void SetList ( MeshEdge *pListRoot );
	// Makes these two edges prox.
	// The point prox data must agree.
	// Returns TRUE on success, or FALSE if it failed.
	bool AddProx ( MeshEdge *pEdge );
	// Find the proximity edge, if any.
	// Relies on the point proximity values having been set up.
	// If one is found, it is returned.
	MeshEdge *DoProxMatch ( void );
	// Removes any edge prox data.
	// Returns TRUE if there was some.
	// The pt prox data can still agree - it is not touched.
	bool RemoveProx ( void );
	// Checks that all pts and tris refer back to this edge,
	// and that they're in the respective lists.
	// If the lists are NULL, the check is not made.
	bool ConsistencyCheck ( MeshPt *pPtRoot = NULL, MeshEdge *pEdgeRoot = NULL, MeshTri *pTriRoot = NULL );

#if MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
protected:
#else
public:
#endif
	// Remove the tri from this edge.
	void RemoveTri ( MeshTri *pTri );
	// Add the tri to this edge.
	void AddTri ( MeshTri *pTri );

#if MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES
protected:
#else
public:
#endif
	// Remove the pt from this edge.
	// NOTE! This is probably not a good thing to do.
	void RemovePt ( MeshPt *pPt );

};



class MeshPt
{
	friend class MeshEdge;
	friend class MeshTri;

private:
	ArbitraryList<MeshEdge *>	EdgeList;	// The list of edges that use this point (in no order).
	ArbitraryList<MeshTri *>	TriList;	// The list of tris that use this point (in no order).
	ArbitraryList<MeshPt *>		ProxPtList;	// The list of prox pts (in no order).

	int							iCurTriNum;		// Used with First/NextTri.
	int							iCurEdgeNum;	// Used with First/NextEdge.
	int							iCurProxNum;	// Used with First/NextProx.

	DlinkDefine(MeshPt,List);
	DWORD						dwListId;	// For use when doing consistency checks.

public:
	MESHPT_APP_DEFINED						// App-defined data.

	DlinkMethods(MeshPt,List);

	MeshPt ( MeshPt *pListRoot = NULL );
	~MeshPt ( void );
	// Find the edge that uses this pt and the other one given.
	// NOTE! You almost never want to use this - almost always,
	// use FindTriEdge, because edges are typically created on-demand
	// when creating tris, and using this rout means that no two
	// points can have more than two tris use them, which lots of models
	// violate.
	MeshEdge *FindEdge ( MeshPt *pPt );
	// Find the first edge that uses this pt and the other given, and
	// also has a free triangle entry, assuming that the points are
	// used in that clockwise order. This allows two edges that share
	// the same points to exist, e.g. where multiple triangles share
	// the same edge (think of the diagonal of the tris of a back-to-back
	// quad - same edge, four tris.
	// The tri will use the points in the order *this,*pPt.
	MeshEdge *FindTriEdge ( MeshPt *pPt );
	// Find the tri that uses this pt and the other two given.
	// They must be in the order this,pPt1,pPt2 - not the other way round.
	MeshTri *FindTri ( MeshPt *pPt1, MeshPt *pPt2 );

	// Retired - there may be several tris like this - call FirstTri/NextTri.
	// Return the first tri in the list. MUST be called before calling NextTri().
	// If a non-NULL pPt is supplied, only tris using this,pPt in that order
	// are returned, otherwise all tris are returned.
	MeshTri *FirstTri ( MeshPt *pPt = NULL );
	// Return the next tri in the list. 
	// If a non-NULL pPt is supplied, only tris using this,pPt in that order
	// are returned, otherwise all tris are returned.
	MeshTri *NextTri ( MeshPt *pPt = NULL );
	// Terminate the current First/Next loop.
	// No need to call this if NULL was returned from NextTri().
	void EndTri ( void );

	// Return the first Edge in the list. MUST be called before calling NextEdge().
	// If a non-NULL pPt is supplied, only edges using this and pPt
	// are returned, otherwise all edges are returned.
	MeshEdge *FirstEdge ( MeshPt *pPt = NULL );
	// Return the next Edge in the list. 
	// If a non-NULL pPt is supplied, only edges using this and pPt
	// are returned, otherwise all edges are returned.
	MeshEdge *NextEdge ( MeshPt *pPt = NULL );
	// Terminate the current First/Next loop.
	// No need to call this if NULL was returned from NextEdge().
	void EndEdge ( void );

	// Add the given pt to the prox list (and vice versa).
	// If the pt was not already there, returns TRUE;
	// If bProxEdges is set to TRUE (default is FALSE ),
	// the edges that these two pts use are made prox if possible.
	bool AddProx ( MeshPt *pPt, bool bProxEdges = FALSE );
	// Remove the given pt from the prox list (and vice versa).
	// If the pt was there, returns TRUE.
	bool RemoveProx ( MeshPt *pPt );
	// Returns TRUE if the two pts are marked as being in proximity.
	bool CheckProx ( MeshPt *pPt );
	// Return the first prox pt. MUST be called before calling NextProx().
	MeshPt *FirstProx ( void );
	// Return the next prox pt.
	MeshPt *NextProx ( void );
	// Terminate the current First/Next loop.
	// No need to call this if NULL was returned from NextProx().
	void EndProx ( void );

	// Which list is this Pt in?
	MeshPt *QueryList ( void );
	// Move this Pt to this list.
	void SetList ( MeshPt *pListRoot );
	// Checks that all edges and tris refer back to this pt,
	// and that they're in the respective lists.
	// If the lists are NULL, the check is not made.
	bool ConsistencyCheck ( MeshPt *pPtRoot = NULL, MeshEdge *pEdgeRoot = NULL, MeshTri *pTriRoot = NULL );

#if MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES
protected:
#else
public:
#endif
	// Remove the edge from this point.
	void RemoveEdge ( MeshEdge *pEdge );
	// Add the edge to this point.
	void AddEdge ( MeshEdge *pEdge );

#if MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_TRIS
protected:
#else
public:
#endif
	// Remove the tri from this point.
	void RemoveTri ( MeshTri *pTri );
	// Add the tri to this point.
	void AddTri ( MeshTri *pTri );

};



inline MeshTri::MeshTri ( void )
{
	// Should only be used for list roots.
	pPt1 = NULL;
	pPt2 = NULL;
	pPt3 = NULL;
	pEdge12 = NULL;
	pEdge23 = NULL;
	pEdge31 = NULL;
	ListInit();
}

inline MeshTri::MeshTri ( MeshPt *pNewPt1, MeshPt *pNewPt2, MeshPt *pNewPt3, MeshTri *pListRoot, MeshEdge *pEdgeListRoot )
{
	ASSERT ( pNewPt1 != NULL );
	ASSERT ( pNewPt2 != NULL );
	ASSERT ( pNewPt3 != NULL );
	pPt1 = pNewPt1;
	pPt2 = pNewPt2;
	pPt3 = pNewPt3;
	pEdge12 = NULL;
	pEdge23 = NULL;
	pEdge31 = NULL;

	pPt1->AddTri ( this );
	pPt2->AddTri ( this );
	pPt3->AddTri ( this );

	pEdge12 = pPt1->FindTriEdge ( pPt2 );
	if ( ( pEdge12 == NULL ) && ( pEdgeListRoot != NULL ) )
	{
		// Autocreate the edge.
		pEdge12 = new MeshEdge ( pPt1, pPt2, pEdgeListRoot );
	}
#if MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
	ASSERT ( pEdge12 != NULL );
#else
	if ( pEdge12 != NULL )
#endif
	{
		pEdge12->AddTri ( this );
	}

	pEdge23 = pPt2->FindTriEdge ( pPt3 );
	if ( ( pEdge23 == NULL ) && ( pEdgeListRoot != NULL ) )
	{
		// Autocreate the edge.
		pEdge23 = new MeshEdge ( pPt2, pPt3, pEdgeListRoot );
	}
#if MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
	ASSERT ( pEdge23 != NULL );
#else
	if ( pEdge23 != NULL )
#endif
	{
		pEdge23->AddTri ( this );
	}

	pEdge31 = pPt3->FindTriEdge ( pPt1 );
	if ( ( pEdge31 == NULL ) && ( pEdgeListRoot != NULL ) )
	{
		// Autocreate the edge.
		pEdge31 = new MeshEdge ( pPt3, pPt1, pEdgeListRoot );
	}
#if MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
	ASSERT ( pEdge31 != NULL );
#else
	if ( pEdge31 != NULL )
#endif
	{
		pEdge31->AddTri ( this );
	}

	ListInit();
	if ( pListRoot != NULL )
	{
		ListAddAfter ( pListRoot );
	}
}


inline void MeshTri::InternalDelete ( BOOL bBinUnusedEdges )
{
	// Remove edge references.
	if ( pEdge12 != NULL )
	{
		pEdge12->RemoveTri ( this );
		if ( bBinUnusedEdges && ( pEdge12->pTri12 == NULL ) && ( pEdge12->pTri21 == NULL ) )
		{
			// This edge is no longer in use.
			delete pEdge12;
		}
		pEdge12 = NULL;
	}
#if MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
	else
	{
		// The only good reason is if this is a local var,
		// in which case everything should be NULL.
		ASSERT ( pEdge12 == NULL );
		ASSERT ( pEdge23 == NULL );
		ASSERT ( pEdge31 == NULL );
		ASSERT ( pPt1 == NULL );
		ASSERT ( pPt2 == NULL );
		ASSERT ( pPt3 == NULL );
	}
#endif

	if ( pEdge23 != NULL )
	{
		pEdge23->RemoveTri ( this );
		if ( bBinUnusedEdges && ( pEdge23->pTri12 == NULL ) && ( pEdge23->pTri21 == NULL ) )
		{
			// This edge is no longer in use.
			delete pEdge23;
		}
		pEdge23 = NULL;
	}
#if MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
	else
	{
		// The only good reason is if this is a local var,
		// in which case everything should be NULL.
		ASSERT ( pEdge12 == NULL );
		ASSERT ( pEdge23 == NULL );
		ASSERT ( pEdge31 == NULL );
		ASSERT ( pPt1 == NULL );
		ASSERT ( pPt2 == NULL );
		ASSERT ( pPt3 == NULL );
	}
#endif

	if ( pEdge31 != NULL )
	{
		pEdge31->RemoveTri ( this );
		if ( bBinUnusedEdges && ( pEdge31->pTri12 == NULL ) && ( pEdge31->pTri21 == NULL ) )
		{
			// This edge is no longer in use.
			delete pEdge31;
		}
		pEdge31 = NULL;
	}
#if MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
	else
	{
		// The only good reason is if this is a local var,
		// in which case everything should be NULL.
		ASSERT ( pEdge12 == NULL );
		ASSERT ( pEdge23 == NULL );
		ASSERT ( pEdge31 == NULL );
		ASSERT ( pPt1 == NULL );
		ASSERT ( pPt2 == NULL );
		ASSERT ( pPt3 == NULL );
	}
#endif

	// Remove point references.
	if ( pPt1 != NULL )
	{
		pPt1->RemoveTri ( this );
		pPt1 = NULL;
	}
#if MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_TRIS
	else
	{
		// The only good reason is if this is a local var,
		// in which case everything should be NULL.
		ASSERT ( pEdge12 == NULL );
		ASSERT ( pEdge23 == NULL );
		ASSERT ( pEdge31 == NULL );
		ASSERT ( pPt1 == NULL );
		ASSERT ( pPt2 == NULL );
		ASSERT ( pPt3 == NULL );
	}
#endif

	if ( pPt2 != NULL )
	{
		pPt2->RemoveTri ( this );
		pPt2 = NULL;
	}
#if MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_TRIS
	else
	{
		// The only good reason is if this is a local var,
		// in which case everything should be NULL.
		ASSERT ( pEdge12 == NULL );
		ASSERT ( pEdge23 == NULL );
		ASSERT ( pEdge31 == NULL );
		ASSERT ( pPt1 == NULL );
		ASSERT ( pPt2 == NULL );
		ASSERT ( pPt3 == NULL );
	}
#endif

	if ( pPt3 != NULL )
	{
		pPt3->RemoveTri ( this );
		pPt3 = NULL;
	}
#if MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_TRIS
	else
	{
		// The only good reason is if this is a local var,
		// in which case everything should be NULL.
		ASSERT ( pEdge12 == NULL );
		ASSERT ( pEdge23 == NULL );
		ASSERT ( pEdge31 == NULL );
		ASSERT ( pPt1 == NULL );
		ASSERT ( pPt2 == NULL );
		ASSERT ( pPt3 == NULL );
	}
#endif

	ListDel();
}

inline MeshTri::~MeshTri ( void )
{
	InternalDelete ( FALSE );
}

inline MeshTri::Delete ( BOOL bBinUnusedEdges /*= FALSE*/ )
{
	InternalDelete ( bBinUnusedEdges );
	delete this;
}


inline void MeshTri::AddEdge ( MeshEdge *pEdge )
{
#if MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
	// Can't mess with a tri's edges once created.
	ASSERT ( FALSE );
#else

	ASSERT ( pEdge != NULL );
	if ( ( ( pEdge->pPt1 == pPt1 ) && ( pEdge->pPt2 == pPt2 ) ) ||
		 ( ( pEdge->pPt2 == pPt1 ) && ( pEdge->pPt1 == pPt2 ) ) )
	{
		ASSERT ( pEdge12 == NULL );
		pEdge12 = pEdge;
	}
	else if ( ( ( pEdge->pPt1 == pPt2 ) && ( pEdge->pPt2 == pPt3 ) ) ||
			  ( ( pEdge->pPt2 == pPt2 ) && ( pEdge->pPt1 == pPt3 ) ) )
	{
		ASSERT ( pEdge23 == NULL );
		pEdge23 = pEdge;
	}
	else
	{
		ASSERT ( ( ( pEdge->pPt1 == pPt3 ) && ( pEdge->pPt2 == pPt1 ) ) ||
				 ( ( pEdge->pPt2 == pPt3 ) && ( pEdge->pPt1 == pPt1 ) ) );
		ASSERT ( pEdge31 == NULL );
		pEdge31 = pEdge;
	}
#endif
}

inline void MeshTri::RemoveEdge ( MeshEdge *pEdge )
{
	ASSERT ( pEdge != NULL );
	if ( pEdge12 == pEdge )
	{
		pEdge12 = NULL;
	}
	else if ( pEdge23 == pEdge )
	{
		pEdge23 = NULL;
	}
	else
	{
		ASSERT ( pEdge31 == pEdge );
		pEdge31 = NULL;
	}
}

inline void MeshTri::RemovePt ( MeshPt *pPt )
{
	ASSERT ( pPt != NULL );
	if ( pPt1 == pPt )
	{
		pPt1 = NULL;
	}
	else if ( pPt2 == pPt )
	{
		pPt2 = NULL;
	}
	else
	{
		ASSERT ( pPt3 == pPt );
		pPt3 = NULL;
	}
}


inline MeshTri *MeshTri::QueryList ( void )
{
	MeshTri *pListRoot = ListFindFirst();
	if ( pListRoot == this )
	{
		ASSERT ( ListFindLast() == this );
		pListRoot = NULL;
	}
	return ( pListRoot );
}

inline void MeshTri::SetList ( MeshTri *pListRoot )
{
	ListDel();
	if ( pListRoot != NULL )
	{
		ListAddAfter ( pListRoot );
	}
}

#define FAIL_CHECK() bRes = FALSE; ASSERT ( FALSE )
inline bool MeshTri::ConsistencyCheck ( MeshPt *pPtRoot, MeshEdge *pEdgeRoot, MeshTri *pTriRoot )
{
	bool bRes = TRUE;
	if ( ( pTriRoot != NULL ) && ( QueryList() != pTriRoot ) )
	{
		FAIL_CHECK();
	}

	if ( pEdge12 != NULL )
	{
		if ( ( pEdgeRoot != NULL ) && ( pEdge12->QueryList() != pEdgeRoot ) )
		{
			FAIL_CHECK();
		}
		
		if ( pEdge12->pTri12 == this )
		{
			if ( ( pEdge12->pPt1 != pPt1 ) || ( pEdge12->pPt2 != pPt2 ) )
			{
				FAIL_CHECK();
			}
		}
		else if ( pEdge12->pTri21 == this )
		{
			if ( ( pEdge12->pPt1 != pPt2 ) || ( pEdge12->pPt2 != pPt1 ) )
			{
				FAIL_CHECK();
			}
		}
		else
		{
			FAIL_CHECK();
		}
	}
	else
	{
		FAIL_CHECK();
	}

	if ( pEdge23 != NULL )
	{
		if ( ( pEdgeRoot != NULL ) && ( pEdge23->QueryList() != pEdgeRoot ) )
		{
			FAIL_CHECK();
		}
		
		if ( pEdge23->pTri12 == this )
		{
			if ( ( pEdge23->pPt1 != pPt2 ) || ( pEdge23->pPt2 != pPt3 ) )
			{
				FAIL_CHECK();
			}
		}
		else if ( pEdge23->pTri21 == this )
		{
			if ( ( pEdge23->pPt1 != pPt3 ) || ( pEdge23->pPt2 != pPt2 ) )
			{
				FAIL_CHECK();
			}
		}
		else
		{
			FAIL_CHECK();
		}
	}
	else
	{
		FAIL_CHECK();
	}

	if ( pEdge31 != NULL )
	{
		if ( ( pEdgeRoot != NULL ) && ( pEdge31->QueryList() != pEdgeRoot ) )
		{
			FAIL_CHECK();
		}
		
		if ( pEdge31->pTri12 == this )
		{
			if ( ( pEdge31->pPt1 != pPt3 ) || ( pEdge31->pPt2 != pPt1 ) )
			{
				FAIL_CHECK();
			}
		}
		else if ( pEdge31->pTri21 == this )
		{
			if ( ( pEdge31->pPt1 != pPt1 ) || ( pEdge31->pPt2 != pPt3 ) )
			{
				FAIL_CHECK();
			}
		}
		else
		{
			FAIL_CHECK();
		}
	}
	else
	{
		FAIL_CHECK();
	}

	if ( ( pPt1 == NULL ) || ( pPt2 == NULL ) || ( pPt3 == NULL ) )
	{
		FAIL_CHECK();
	}
	else
	{
		if ( pPtRoot != NULL )
		{
			if ( pPt1->QueryList() != pPtRoot )
			{
				FAIL_CHECK();
			}
			if ( pPt2->QueryList() != pPtRoot )
			{
				FAIL_CHECK();
			}
			if ( pPt3->QueryList() != pPtRoot )
			{
				FAIL_CHECK();
			}
		}

// Multiple tris that have this order may exist - can't be arsed to
// do a FirstTri/NextTri search.
#if 0
		if ( pPt1->FindTri ( pPt2 ) != this )
		{
			FAIL_CHECK();
		}
		if ( pPt2->FindTri ( pPt3 ) != this )
		{
			FAIL_CHECK();
		}
		if ( pPt3->FindTri ( pPt1 ) != this )
		{
			FAIL_CHECK();
		}
#endif

// Falls over if more than one edge has these points.
// Can happen if more than two tris meet on one edge.
#if 0
		if ( pPt1->FindEdge ( pPt2 ) != pEdge12 )
		{
			FAIL_CHECK();
		}
		if ( pPt2->FindEdge ( pPt3 ) != pEdge23 )
		{
			FAIL_CHECK();
		}
		if ( pPt3->FindEdge ( pPt1 ) != pEdge31 )
		{
			FAIL_CHECK();
		}
#endif
	}

	return ( bRes );
}










inline MeshEdge::MeshEdge ( void )
{
	pPt1 = NULL;
	pPt2 = NULL;
	pTri12 = NULL;
	pTri21 = NULL;
	pEdgeProx = NULL;
	ListInit();
}

inline MeshEdge::MeshEdge ( MeshPt *pNewPt1, MeshPt *pNewPt2, MeshEdge *pListRoot )
{
	ASSERT ( pNewPt1 != NULL );
	ASSERT ( pNewPt2 != NULL );
	pPt1 = pNewPt1;
	pPt2 = pNewPt2;
	pTri12 = NULL;
	pTri21 = NULL;
	pEdgeProx = NULL;

	pPt1->AddEdge ( this );
	pPt2->AddEdge ( this );

#if MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
	// No auto-adding of tris.
#else
	pTri12 = pPt1->FindTri ( pPt2 );
	if ( pTri12 != NULL )
	{
		pTri12->AddEdge ( this );
	}
	pTri21 = pPt2->FindTri ( pPt1 );
	if ( pTri21 != NULL )
	{
		pTri21->AddEdge ( this );
	}
#endif

	ListInit();
	if ( pListRoot != NULL )
	{
		ListAddAfter ( pListRoot );
	}
}

inline MeshEdge::~MeshEdge ( void )
{
	RemoveProx();

	if ( pPt1 != NULL )
	{
		MeshPt *pPt = pPt1;
		RemovePt ( pPt );
		pPt->RemoveEdge ( this );
	}
#if MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES
	else
	{
		// The only good reason is if this is a local var,
		// in which case everything should be NULL.
		ASSERT ( pPt1 == NULL );
		ASSERT ( pPt2 == NULL );
		ASSERT ( pTri12 == NULL );
		ASSERT ( pTri21 == NULL );
	}
#endif


	if ( pPt2 != NULL )
	{
		MeshPt *pPt = pPt2;
		RemovePt ( pPt );
		pPt->RemoveEdge ( this );
	}
#if MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES
	else
	{
		// The only good reason is if this is a local var,
		// in which case everything should be NULL.
		ASSERT ( pPt1 == NULL );
		ASSERT ( pPt2 == NULL );
		ASSERT ( pTri12 == NULL );
		ASSERT ( pTri21 == NULL );
	}
#endif


#if MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS
	// Any tri should have been binned already
	ASSERT ( pTri12 == NULL );
	ASSERT ( pTri21 == NULL );
#else
	if ( pTri12 != NULL )
	{
		MeshTri *pTri = pTri12;
		RemoveTri ( pTri );
		pTri->RemoveEdge ( this );
	}

	if ( pTri21 != NULL )
	{
		MeshTri *pTri = pTri21;
		RemoveTri ( pTri );
		pTri->RemoveEdge ( this );
	}
#endif

	ListDel();
}


// Remove this edge from the tri.
inline void MeshEdge::RemoveTri ( MeshTri *pTri )
{
	ASSERT ( pTri != NULL );
	if ( pTri12 == pTri )
	{
		pTri12 = NULL;
	}
	else
	{
		ASSERT ( pTri21 == pTri );
		pTri21 = NULL;
	}
}

// Remove this edge from the pt.
inline void MeshEdge::RemovePt ( MeshPt *pPt )
{
	ASSERT ( pPt != NULL );
	if ( pPt1 == pPt )
	{
		pPt1 = NULL;
	}
	else
	{
		ASSERT ( pPt2 == pPt );
		pPt2 = NULL;
	}
}

inline void MeshEdge::AddTri ( MeshTri *pTri )
{
	ASSERT ( pTri != NULL );
	// Assumes the tri's pt pointers have already been set up.
	if ( ( ( pPt1 == pTri->pPt1 ) && ( pPt2 == pTri->pPt2 ) ) ||
		 ( ( pPt1 == pTri->pPt2 ) && ( pPt2 == pTri->pPt3 ) ) ||
		 ( ( pPt1 == pTri->pPt3 ) && ( pPt2 == pTri->pPt1 ) ) )
	{
		ASSERT ( pTri12 == NULL );
		pTri12 = pTri;
	}
	else
	{
		ASSERT ( ( ( pPt1 == pTri->pPt2 ) && ( pPt2 == pTri->pPt1 ) ) ||
				 ( ( pPt1 == pTri->pPt3 ) && ( pPt2 == pTri->pPt2 ) ) ||
				 ( ( pPt1 == pTri->pPt1 ) && ( pPt2 == pTri->pPt3 ) ) );
		ASSERT ( pTri21 == NULL );
		pTri21 = pTri;
	}
}

// Returns the other triangle that uses this edge.
inline MeshTri *MeshEdge::OtherTri ( MeshTri *pTri )
{
	ASSERT ( pTri != NULL );
	if ( pTri == pTri12 )
	{
		return ( pTri21 );
	}
	else
	{
		ASSERT ( pTri == pTri21 );
		return ( pTri12 );
	}
}

inline MeshPt *MeshEdge::OtherPt ( MeshPt *pPt )
{
	ASSERT ( pPt != NULL );
	if ( pPt == pPt1 )
	{
		return ( pPt2 );
	}
	else
	{
		ASSERT ( pPt == pPt2 );
		return ( pPt1 );
	}
}

// Try to merge these two edges. Result is TRUE if it succeeded - note that the other edge will be NOT deleted.
inline bool MeshEdge::bTryToMergeEdges ( MeshEdge *pedge )
{
	ASSERT ( pedge != this );
	if ( pPt1 == pedge->pPt1 )
	{
		ASSERT ( pPt2 == pedge->pPt2 );
		if ( ( pTri12 == NULL ) && ( pedge->pTri21 == NULL ) &&
			 ( ( pEdgeProx == NULL ) || ( pedge->pEdgeProx == NULL ) ) )
		{
			// Merge them.
			pTri12 = pedge->pTri12;
			if ( pTri12 != NULL )
			{
				if ( pTri12->pEdge12 == pedge )
				{
					pTri12->pEdge12 = this;
				}
				else if ( pTri12->pEdge23 == pedge )
				{
					pTri12->pEdge23 = this;
				}
				else
				{
					ASSERT ( pTri12->pEdge31 == pedge );
					pTri12->pEdge31 = this;
				}
			}
			pedge->pTri12 = NULL;
			if ( pedge->pEdgeProx != NULL )
			{
				ASSERT ( pEdgeProx == NULL );
				pEdgeProx = pedge->pEdgeProx;
				ASSERT ( pEdgeProx->pEdgeProx == pedge );
				pEdgeProx->pEdgeProx = this;
				pedge->pEdgeProx = NULL;
			}
			return TRUE;
		}
		else if ( ( pTri21 == NULL ) && ( pedge->pTri12 == NULL ) &&
				  ( ( pEdgeProx == NULL ) || ( pedge->pEdgeProx == NULL ) ) )
		{
			// Merge them.
			pTri21 = pedge->pTri21;
			if ( pTri21 != NULL )
			{
				if ( pTri21->pEdge12 == pedge )
				{
					pTri21->pEdge12 = this;
				}
				else if ( pTri21->pEdge23 == pedge )
				{
					pTri21->pEdge23 = this;
				}
				else
				{
					ASSERT ( pTri21->pEdge31 == pedge );
					pTri21->pEdge31 = this;
				}
			}
			pedge->pTri21 = NULL;
			if ( pedge->pEdgeProx != NULL )
			{
				ASSERT ( pEdgeProx == NULL );
				pEdgeProx = pedge->pEdgeProx;
				ASSERT ( pEdgeProx->pEdgeProx == pedge );
				pEdgeProx->pEdgeProx = this;
				pedge->pEdgeProx = NULL;
			}
			return TRUE;
		}
		else
		{
			// Nope - they don't match.
			return ( FALSE );
		}
	}
	else
	{
		ASSERT ( pPt1 == pedge->pPt2 );
		ASSERT ( pPt2 == pedge->pPt1 );
		if ( ( pTri12 == NULL ) && ( pedge->pTri12 == NULL ) &&
			 ( ( pEdgeProx == NULL ) || ( pedge->pEdgeProx == NULL ) ) )
		{
			// Merge them.
			pTri12 = pedge->pTri21;
			if ( pTri12 != NULL )
			{
				if ( pTri12->pEdge12 == pedge )
				{
					pTri12->pEdge12 = this;
				}
				else if ( pTri12->pEdge23 == pedge )
				{
					pTri12->pEdge23 = this;
				}
				else
				{
					ASSERT ( pTri12->pEdge31 == pedge );
					pTri12->pEdge31 = this;
				}
			}
			pedge->pTri21 = NULL;
			if ( pedge->pEdgeProx != NULL )
			{
				ASSERT ( pEdgeProx == NULL );
				pEdgeProx = pedge->pEdgeProx;
				ASSERT ( pEdgeProx->pEdgeProx == pedge );
				pEdgeProx->pEdgeProx = this;
				pedge->pEdgeProx = NULL;
			}
			return TRUE;
		}
		else if ( ( pTri21 == NULL ) && ( pedge->pTri21 == NULL ) &&
				  ( ( pEdgeProx == NULL ) || ( pedge->pEdgeProx == NULL ) ) )
		{
			// Merge them.
			pTri21 = pedge->pTri12;
			if ( pTri21 != NULL )
			{
				if ( pTri21->pEdge12 == pedge )
				{
					pTri21->pEdge12 = this;
				}
				else if ( pTri21->pEdge23 == pedge )
				{
					pTri21->pEdge23 = this;
				}
				else
				{
					ASSERT ( pTri21->pEdge31 == pedge );
					pTri21->pEdge31 = this;
				}
			}
			pedge->pTri12 = NULL;
			if ( pedge->pEdgeProx != NULL )
			{
				ASSERT ( pEdgeProx == NULL );
				pEdgeProx = pedge->pEdgeProx;
				ASSERT ( pEdgeProx->pEdgeProx == pedge );
				pEdgeProx->pEdgeProx = this;
				pedge->pEdgeProx = NULL;
			}
			return TRUE;
		}
		else
		{
			// Nope - they don't match.
			return ( FALSE );
		}
	}
}


inline MeshEdge *MeshEdge::QueryList ( void )
{
	MeshEdge *pListRoot = ListFindFirst();
	if ( pListRoot == this )
	{
		ASSERT ( ListFindLast() == this );
		pListRoot = NULL;
	}
	return ( pListRoot );
}

inline void MeshEdge::SetList ( MeshEdge *pListRoot )
{
	ListDel();
	if ( pListRoot != NULL )
	{
		ListAddAfter ( pListRoot );
	}
}




// Makes these two edges prox.
// The point prox data must agree.
// Returns TRUE on success, or FALSE if it failed.
inline bool MeshEdge::AddProx ( MeshEdge *pEdge )
{
	ASSERT ( pEdge != NULL );
	if ( pEdgeProx != NULL )
	{
		// Already got prox.
		return ( FALSE );
	}
	else if ( pEdge->pEdgeProx != NULL )
	{
		// Already got prox.
		return ( FALSE );
	}
	else
	{
		// Check that the pts agree.
		// Either pPt1<->pPt1 and pPt2<->pPt2, or the other way round.
		if ( pEdge->pPt1->CheckProx ( pPt1 ) )
		{
			if ( !pEdge->pPt2->CheckProx ( pPt2 ) )
			{
				return ( FALSE );
			}
		}
		else if ( pEdge->pPt1->CheckProx ( pPt2 ) )
		{
			if ( !pEdge->pPt2->CheckProx ( pPt1 ) )
			{
				return ( FALSE );
			}
		}
		else
		{
			return ( FALSE );
		}

		// OK, must have passed.
		pEdgeProx = pEdge;
		pEdge->pEdgeProx = this;
		return ( TRUE );
	}
}

// Find the proximity edge, if any.
// Relies on the point proximity values having been set up.
// If one is found, it is returned.
inline MeshEdge *MeshEdge::DoProxMatch ( void )
{
	// Loop through all the prox pts to pPt1
	//		Loop through all their edges.
	//			If the other pt is prox to pPt2, then we found a prox edge.
	int i;
	MeshPt **ppPt = pPt1->ProxPtList.Ptr();
	for ( i = 0; i < pPt1->ProxPtList.Size(); i++ )
	{
		MeshPt *pPtProx = ppPt[i];
		ASSERT ( pPtProx != NULL );

		MeshEdge *pEdgeOther = pPtProx->FirstEdge();
		while ( pEdgeOther != NULL )
		{
			MeshPt *pPtOther = pEdgeOther->OtherPt ( pPtProx );
			if ( pPtOther->CheckProx ( pPt2 ) )
			{
				// Yes - this is prox.
				bool bRes = AddProx ( pEdgeOther );
				if ( bRes )
				{
					pPtProx->EndEdge();
					return pEdgeOther;
				}
				else
				{
					// Too many edges trying to be prox!
					pPtProx->EndEdge();
					return NULL;
				}
			}

			pEdgeOther = pPtProx->NextEdge();
		}
	}
	return NULL;
}

// Removes any edge prox data.
// Returns TRUE if there was some.
// The pt prox data can still agree - it is not touched.
inline bool MeshEdge::RemoveProx ( void )
{
	if ( pEdgeProx == NULL )
	{
		return ( FALSE );
	}
	else
	{
		ASSERT ( pEdgeProx->pEdgeProx == this );
		pEdgeProx->pEdgeProx = NULL;
		pEdgeProx = NULL;
		return ( TRUE );
	}
}





inline bool MeshEdge::ConsistencyCheck ( MeshPt *pPtRoot, MeshEdge *pEdgeRoot, MeshTri *pTriRoot )
{
	bool bRes = TRUE;
	if ( ( pEdgeRoot != NULL ) && ( QueryList() != pEdgeRoot ) )
	{
		FAIL_CHECK();
	}

	if ( pEdgeProx != NULL )
	{
		if ( pEdgeProx->pEdgeProx != this )
		{
			FAIL_CHECK();
		}
		if ( ( pEdgeRoot != NULL ) && ( pEdgeProx->QueryList() != pEdgeRoot ) )
		{
			FAIL_CHECK();
		}
		// Either pPt1<->pPt1 and pPt2<->pPt2, or the other way round.
		if ( pEdgeProx->pPt1->CheckProx ( pPt1 ) )
		{
			if ( !pEdgeProx->pPt2->CheckProx ( pPt2 ) )
			{
				FAIL_CHECK();
			}
		}
		else if ( pEdgeProx->pPt1->CheckProx ( pPt2 ) )
		{
			if ( !pEdgeProx->pPt2->CheckProx ( pPt1 ) )
			{
				FAIL_CHECK();
			}
		}
		else
		{
			FAIL_CHECK();
		}
	}

	if ( pTri12 != NULL )
	{
		if ( ( pTriRoot != NULL ) && ( pTri12->QueryList() != pTriRoot ) )
		{
			FAIL_CHECK();
		}
		
		if ( pTri12->pEdge12 == this )
		{
			if ( ( pTri12->pPt1 != pPt1 ) || ( pTri12->pPt2 != pPt2 ) )
			{
				FAIL_CHECK();
			}
		}
		else if ( pTri12->pEdge23 == this )
		{
			if ( ( pTri12->pPt2 != pPt1 ) || ( pTri12->pPt3 != pPt2 ) )
			{
				FAIL_CHECK();
			}
		}
		else if ( pTri12->pEdge31 == this )
		{
			if ( ( pTri12->pPt3 != pPt1 ) || ( pTri12->pPt1 != pPt2 ) )
			{
				FAIL_CHECK();
			}
		}
		else
		{
			FAIL_CHECK();
		}
	}

	if ( pTri21 != NULL )
	{
		if ( ( pTriRoot != NULL ) && ( pTri21->QueryList() != pTriRoot ) )
		{
			FAIL_CHECK();
		}
		
		if ( pTri21->pEdge12 == this )
		{
			if ( ( pTri21->pPt1 != pPt2 ) || ( pTri21->pPt2 != pPt1 ) )
			{
				FAIL_CHECK();
			}
		}
		else if ( pTri21->pEdge23 == this )
		{
			if ( ( pTri21->pPt2 != pPt2 ) || ( pTri21->pPt3 != pPt1 ) )
			{
				FAIL_CHECK();
			}
		}
		else if ( pTri21->pEdge31 == this )
		{
			if ( ( pTri21->pPt3 != pPt2 ) || ( pTri21->pPt1 != pPt1 ) )
			{
				FAIL_CHECK();
			}
		}
		else
		{
			FAIL_CHECK();
		}
	}


	if ( ( pPt1 == NULL ) || ( pPt2 == NULL ) )
	{
		FAIL_CHECK();
	}
	else
	{
		if ( pPtRoot != NULL )
		{
			if ( pPt1->QueryList() != pPtRoot )
			{
				FAIL_CHECK();
			}
			if ( pPt2->QueryList() != pPtRoot )
			{
				FAIL_CHECK();
			}
		}

#if 0
		if ( pPt1->FindEdge ( pPt2 ) != this )
		{
			FAIL_CHECK();
		}
		if ( pPt2->FindEdge ( pPt1 ) != this )
		{
			FAIL_CHECK();
		}
#endif

// Should update to use FirstTri/NextTri until a verdict is reached.
#if 0
		// Note that pTri12 can be NULL, but if and only if
		// the FindTri is NULL, so this is a valid test.
		// Either both are NULL, or both are the same.
		if ( pPt1->FindTri ( pPt2 ) != pTri12 )
		{
			FAIL_CHECK();
		}
		if ( pPt2->FindTri ( pPt1 ) != pTri21 )
		{
			FAIL_CHECK();
		}
#endif
	}

	return ( bRes );
}







inline MeshPt::MeshPt ( MeshPt *pListRoot )
{
	iCurEdgeNum = -1;
	iCurTriNum = -1;
	iCurProxNum = -1;

	ListInit();
	if ( pListRoot != NULL )
	{
		ListAddAfter ( pListRoot );
	}
}

inline MeshPt::~MeshPt ( void )
{
#if !MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_TRIS || !MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES
	int i;
#endif

	// Can't just do a simple loop - RemoveProx modifies the list.
	while ( ProxPtList.Size() > 0 )
	{
		bool bRes = RemoveProx ( ProxPtList.Ptr()[0] );
		ASSERT ( bRes );
	}
 
#if MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_TRIS
	// Should not be any tris.
	ASSERT ( TriList.Size() == 0 );
#else
	MeshTri **ppTri = TriList.Ptr();
	for ( i = 0; i < TriList.Size(); i++ )
	{
		ASSERT ( ppTri[i] != NULL );
		ppTri[i]->RemovePt ( this );
	}
#endif

#if MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES
	// Should not be any edges.
	ASSERT ( EdgeList.Size() == 0 );
#else
	MeshEdge **ppEdge = EdgeList.Ptr();
	for ( i = 0; i < EdgeList.Size(); i++ )
	{
		ASSERT ( ppEdge[i] != NULL );
		ppEdge[i]->RemovePt ( this );
	}
#endif

	ListDel();
}

inline void MeshPt::RemoveEdge ( MeshEdge *pEdge )
{
	ASSERT ( pEdge != NULL );
	MeshEdge **ppEdgeList = EdgeList.Ptr();
	int i;
	for ( i = 0; i < EdgeList.Size(); i++ )
	{
		if ( ppEdgeList[i] == pEdge )
		{
			break;
		}
	}
	ASSERT ( i < EdgeList.Size() );
	// Bin this entry.
	EdgeList.RemoveItem ( i );
}

inline void MeshPt::RemoveTri ( MeshTri *pTri )
{
	ASSERT ( pTri != NULL );
	MeshTri **ppTriList = TriList.Ptr();
	int i;
	for ( i = 0; i < TriList.Size(); i++ )
	{
		if ( ppTriList[i] == pTri )
		{
			break;
		}
	}
	ASSERT ( i < TriList.Size() );
	// Bin this entry with the last entry.
	TriList.RemoveItem ( i );
}

inline void MeshPt::AddTri ( MeshTri *pTri )
{
	ASSERT ( pTri != NULL );
#ifdef DEBUG
	// Make sure this hasn't been added already.
	MeshTri **ppTriList = TriList.Ptr();
	int i;
	for ( i = 0; i < TriList.Size(); i++ )
	{
		ASSERT ( ppTriList[i] != NULL );
		ASSERT ( ppTriList[i] != pTri );
	}
#endif
	*(TriList.AddItem()) = pTri;
}

inline void MeshPt::AddEdge ( MeshEdge *pEdge )
{
	ASSERT ( pEdge != NULL );
#ifdef DEBUG
	// Make sure this hasn't been added already.
	int i;
	MeshEdge **ppEdgeList = EdgeList.Ptr();
	for ( i = 0; i < EdgeList.Size(); i++ )
	{
		ASSERT ( ppEdgeList[i] != NULL );
		ASSERT ( ppEdgeList[i] != pEdge );
	}
#endif
	*(EdgeList.AddItem()) = pEdge;
}


inline MeshEdge *MeshPt::FindEdge ( MeshPt *pPt )
{
	ASSERT ( pPt != NULL );
	ASSERT ( pPt != this );
	MeshEdge **ppEdgeList = EdgeList.Ptr();
	int i;
	for ( i = 0; i < EdgeList.Size(); i++ )
	{
		MeshEdge *pEdge = ppEdgeList[i];
		ASSERT ( pEdge != NULL );
		ASSERT ( ( pEdge->pPt1 == this ) || ( pEdge->pPt2 == this ) );
		if ( pEdge->pPt2 == pPt )
		{
			ASSERT ( pEdge->pPt1 == this );
			return ( pEdge );
		}
		if ( pEdge->pPt1 == pPt )
		{
			ASSERT ( pEdge->pPt2 == this );
			return ( pEdge );
		}
	}
	return ( NULL );
}

// Find the first edge that uses this pt and the other given, and
// also has a free triangle entry, assuming that the points are
// used in that clockwise order. This allows two edges that share
// the same points to exist, e.g. where multiple triangles share
// the same edge (think of the diagonal of the tris of a back-to-back
// quad - same edge, four tris.
// The tri will use the points in the order *this,*pPt.
inline MeshEdge *MeshPt::FindTriEdge ( MeshPt *pPt )
{
	ASSERT ( pPt != NULL );
	ASSERT ( pPt != this );
	MeshEdge **ppEdgeList = EdgeList.Ptr();
	int i;
	for ( i = 0; i < EdgeList.Size(); i++ )
	{
		MeshEdge *pEdge = ppEdgeList[i];
		ASSERT ( pEdge != NULL );
		ASSERT ( ( pEdge->pPt1 == this ) || ( pEdge->pPt2 == this ) );
		if ( pEdge->pPt2 == pPt )
		{
			ASSERT ( pEdge->pPt1 == this );
			// Check that it would be possible to add a tri to this.
			// The tri will use this,pPt in that order, so must be in
			// pTri12
			if ( pEdge->pTri12 == NULL )
			{
				return ( pEdge );
			}
			else
			{
				int bogus = 0;
			}
		}
		if ( pEdge->pPt1 == pPt )
		{
			ASSERT ( pEdge->pPt2 == this );
			// Check that it would be possible to add a tri to this.
			// The tri will use this,pPt in that order, so must be in
			// pTri21
			if ( pEdge->pTri21 == NULL )
			{
				return ( pEdge );
			}
			else
			{
				int bogus = 0;
			}
		}
	}
	return ( NULL );
}


inline MeshTri *MeshPt::FindTri ( MeshPt *pPt1, MeshPt *pPt2 )
{
	ASSERT ( pPt1 != NULL );
	ASSERT ( pPt2 != NULL );
	MeshTri **ppTriList = TriList.Ptr();
	int i;
	for ( i = 0; i < TriList.Size(); i++ )
	{
		MeshTri *pTri = ppTriList[i];
		ASSERT ( pTri != NULL );
		ASSERT ( ( pTri->pPt1 == this ) || ( pTri->pPt2 == this ) || ( pTri->pPt3 == this ) );
		if ( ( pTri->pPt1 == this ) && ( pTri->pPt2 == pPt1 ) && ( pTri->pPt3 == pPt2 ) )
		{
			return ( pTri );
		}
		if ( ( pTri->pPt2 == this ) && ( pTri->pPt3 == pPt1 ) && ( pTri->pPt1 == pPt2 ) )
		{
			return ( pTri );
		}
		if ( ( pTri->pPt3 == this ) && ( pTri->pPt1 == pPt1 ) && ( pTri->pPt2 == pPt2 ) )
		{
			return ( pTri );
		}
	}
	return ( NULL );
}


// Return the next tri in the list. 
// If a non-NULL pPt is supplied, only tris using this,pPt in that order
// are returned, otherwise all tris are returned.
inline MeshTri *MeshPt::NextTri ( MeshPt *pPt )
{
	ASSERT ( this != pPt );
	ASSERT ( iCurTriNum >= 0 );
	while ( TRUE )
	{
		if ( iCurTriNum < TriList.Size() )
		{
			MeshTri *pTri = (TriList.Ptr())[iCurTriNum++];
			ASSERT ( pTri != NULL );
			if ( pPt == NULL )
			{
				// Return all tris.
				return ( pTri );
			}

			// Return only tris that use this,pPt
			if ( ( ( pTri->pPt1 == this ) && ( pTri->pPt2 == pPt ) ) ||
				 ( ( pTri->pPt2 == this ) && ( pTri->pPt3 == pPt ) ) ||
				 ( ( pTri->pPt3 == this ) && ( pTri->pPt1 == pPt ) ) )
			{
				return ( pTri );
			}
		}
		else
		{
			// End of the list.
			iCurTriNum = -1;
			return ( NULL );
		}
	}
}

// Return the first tri in the list. MUST be called before calling NextTri().
// If a non-NULL pPt is supplied, only tris using this,pPt in that order
// are returned, otherwise all tris are returned.
inline MeshTri *MeshPt::FirstTri ( MeshPt *pPt )
{
	ASSERT ( iCurTriNum == -1 );
	iCurTriNum = 0;
	return ( NextTri ( pPt ) );
}

// Terminate the current First/Next loop.
inline void MeshPt::EndTri ( void )
{
	iCurTriNum = -1;
}




// Return the next Edge in the list. 
// If a non-NULL pPt is supplied, only edges using this and pPt
// are returned, otherwise all edges are returned.
inline MeshEdge *MeshPt::NextEdge ( MeshPt *pPt )
{
	ASSERT ( this != pPt );
	ASSERT ( iCurEdgeNum >= 0 );
	while ( TRUE )
	{
		if ( iCurEdgeNum < EdgeList.Size() )
		{
			MeshEdge *pEdge = (EdgeList.Ptr())[iCurEdgeNum++];
			ASSERT ( pEdge != NULL );
			if ( pPt == NULL )
			{
				// Return all edges.
				return ( pEdge );
			}

			// Return only the edges the use this & pPt.
			if ( ( pEdge->pPt1 == pPt ) || ( pEdge->pPt2 == pPt ) )
			{
				ASSERT ( ( pEdge->pPt1 == this ) || ( pEdge->pPt2 == this ) );
				return ( pEdge );
			}

		}
		else
		{
			// End of the list.
			iCurEdgeNum = -1;
			return ( NULL );
		}
	}
}

// Return the first Edge in the list. MUST be called before calling NextEdge().
// If a non-NULL pPt is supplied, only edges using this and pPt
// are returned, otherwise all edges are returned.
inline MeshEdge *MeshPt::FirstEdge ( MeshPt *pPt )
{
	ASSERT ( iCurEdgeNum == -1 );
	iCurEdgeNum = 0;
	return ( NextEdge ( pPt ) );
}

// Terminate the current First/Next loop.
inline void MeshPt::EndEdge ( void )
{
	iCurEdgeNum = -1;
}



// Returns TRUE if the two pts are marked as being in proximity.
inline bool MeshPt::CheckProx ( MeshPt *pPt )
{
	ASSERT ( pPt != NULL );
	MeshPt **ppPt = ProxPtList.Ptr();
	for ( int i = 0; i < ProxPtList.Size(); i++ )
	{
		ASSERT ( ppPt[i] != NULL );
		if ( ppPt[i] == pPt )
		{
			// Yes.
			return ( TRUE );
		}
	}
	return ( FALSE );
}


// Add the given pt to the prox list (and vice versa).
// If the pt was not already there, returns TRUE;
// If bProxEdges is set to TRUE (default is FALSE ),
// the edges that these two pts use are made prox if possible.
inline bool MeshPt::AddProx ( MeshPt *pPt, bool bProxEdges )
{
	bool bRes;

	ASSERT ( pPt != NULL );
	if ( CheckProx ( pPt ) )
	{
		// Already prox.
		ASSERT ( pPt->CheckProx ( this ) );
		bRes = FALSE;
	}
	else
	{
		ASSERT ( !pPt->CheckProx ( this ) );

		// Add to this pt.
		*(ProxPtList.AddItem()) = pPt;

		// Add to the other pt.
		*(pPt->ProxPtList.AddItem()) = this;

		bRes = TRUE;
	}

	// Now check all their edges for proximity.
	// For each edge of this.
	//		Find other pt and scan proxs of that.
	//			If those proxes form and edge with pPt, the edges are prox.
	MeshEdge *pedge = FirstEdge();
	while ( pedge != NULL )
	{
		MeshPt *pptOther = pedge->OtherPt ( this );
		MeshPt **ppPt = pptOther->ProxPtList.Ptr();
		for ( int i = 0; i < pptOther->ProxPtList.Size(); i++ )
		{
			ASSERT ( ppPt[i] != NULL );
			MeshEdge *pedgeProx = pPt->FindEdge ( ppPt[i] );
			if ( pedgeProx != NULL )
			{
				bool bRes = pedgeProx->AddProx ( pedge );
				ASSERT ( bRes );
				break;
			}
		}

		pedge = NextEdge();
	}

	return ( bRes );
}

// Remove the given pt from the prox list (and vice versa).
// If the pt was there, returns TRUE.
inline bool MeshPt::RemoveProx ( MeshPt *pPt )
{
	ASSERT ( pPt != NULL );
	if ( CheckProx ( pPt ) )
	{
		// Yep, they are prox.
		ASSERT ( pPt->CheckProx ( this ) );

		MeshPt **ppPtList;
		int i;

		// Remove pPt from this.
		ppPtList = ProxPtList.Ptr();
		for ( i = 0; i < ProxPtList.Size(); i++ )
		{
			if ( ppPtList[i] == pPt )
			{
				break;
			}
		}
		ASSERT ( i < ProxPtList.Size() );
		// Replace this entry with the last entry.
		ProxPtList.RemoveItem(i);

		// Remove this from pPt.
		ppPtList = pPt->ProxPtList.Ptr();
		for ( i = 0; i < pPt->ProxPtList.Size(); i++ )
		{
			if ( ppPtList[i] == this )
			{
				break;
			}
		}
		ASSERT ( i < pPt->ProxPtList.Size() );
		// Replace this entry with the last entry.
		ProxPtList.RemoveItem(i);

		return ( TRUE );
	}
	else
	{
		// No, they're not prox.
		ASSERT ( !pPt->CheckProx ( this ) );
		return ( FALSE );
	}

}




// Return the first prox pt. MUST be called before calling NextProx().
inline MeshPt *MeshPt::FirstProx ( void )
{
	ASSERT ( iCurProxNum == -1 );
	iCurProxNum = 0;
	return ( NextProx() );
}

// Return the next prox pt.
inline MeshPt *MeshPt::NextProx ( void )
{
	ASSERT ( iCurProxNum >= 0 );
	while ( TRUE )
	{
		if ( iCurProxNum < ProxPtList.Size() )
		{
			MeshPt *pptProx = (ProxPtList.Ptr())[iCurProxNum++];
			ASSERT ( pptProx != NULL );
			return ( pptProx );
		}
		else
		{
			// End of the list.
			iCurProxNum = -1;
			return ( NULL );
		}
	}
}

// Terminate the current First/Next loop.
inline void MeshPt::EndProx ( void )
{
	iCurProxNum = -1;
}



inline MeshPt *MeshPt::QueryList ( void )
{
	MeshPt *pListRoot = ListFindFirst();
	if ( pListRoot == this )
	{
		ASSERT ( ListFindLast() == this );
		pListRoot = NULL;
	}
	return ( pListRoot );
}

inline void MeshPt::SetList ( MeshPt *pListRoot )
{
	ListDel();
	if ( pListRoot != NULL )
	{
		ListAddAfter ( pListRoot );
	}
}

inline bool MeshPt::ConsistencyCheck ( MeshPt *pPtRoot, MeshEdge *pEdgeRoot, MeshTri *pTriRoot )
{
	bool bRes = TRUE;
	if ( ( pPtRoot != NULL ) && ( QueryList() != pPtRoot ) )
	{
		FAIL_CHECK();
	}

	// Check prox.
	MeshPt **ppPt = ProxPtList.Ptr();
	for ( int i = 0; i < ProxPtList.Size(); i++ )
	{
		ASSERT ( ppPt[i] != NULL );
		if ( !ppPt[i]->CheckProx ( this ) )
		{
			FAIL_CHECK();
		}
		if ( ( pPtRoot != NULL ) && ( ppPt[i]->QueryList() != pPtRoot ) )
		{
			FAIL_CHECK();
		}
	}

	// Just check the consistency of all tris and edges that use this.
	MeshEdge *pEdge = FirstEdge();
	while ( pEdge != NULL )
	{
		if ( !pEdge->ConsistencyCheck ( pPtRoot, pEdgeRoot, pTriRoot ) )
		{
			// Will have already ASSERTed.
			bRes = FALSE;
		}
		pEdge = NextEdge();
	}
	MeshTri *pTri = FirstTri();
	while ( pTri != NULL )
	{
		if ( !pTri->ConsistencyCheck ( pPtRoot, pEdgeRoot, pTriRoot ) )
		{
			// Will have already ASSERTed.
			bRes = FALSE;
		}
		pTri = NextTri();
	}
	return ( bRes );
}








#if 0

// Provides consistent typesafe access to the pMore field of the objects.
// Set them up like this:
//
// MESHPT_PMORE_SET(D3DVERTEX,D3DVert);
//
// And use them like this:
//
// D3DVERTEX *pVert = D3DVert(pPt);
//
// or this:
//
// D3DVert(pPt) = pVert;

#define MESHPT_PMORE_SET(mytype,accessname)							\
mytype *accessname ( MeshPt *pPt )									\
{																	\
	return ( (mytype *)( pPt->pMore ) );							\
}

#define MESHEDGE_PMORE_SET(mytype,accessname)						\
mytype *accessname ( MeshEdge *pEdge )								\
{																	\
	return ( (mytype *)( pEdge->pMore ) );							\
}

#define MESHTRI_PMORE_SET(mytype,accessname)						\
mytype *accessname ( MeshTri *pTri )								\
{																	\
	return ( (mytype *)( pTri->pMore ) );							\
}

#endif





