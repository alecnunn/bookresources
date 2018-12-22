/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */

#include "TomsD3DLib.h"

#define STRICT
#define D3D_OVERLOADS
#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <D3DX8.h>
#include <DInput.h>
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "resource.h"
#include "commctrl.h"
#include <list>



extern LPDIRECT3DDEVICE8 g_pd3dDevice;



// Incremented by the draw routs. Display + zero whenever you want.
extern int g_iNumOfObjectTrisDrawn;
extern int g_iNumOfObjectVertsDrawn;
extern int g_iMaxNumTrisDrawn;
extern BOOL g_bOptimiseVertexOrder;
extern BOOL g_bShowVIPMInfo;
extern BOOL g_bUseFastButBadOptimise;


// Set to 1 to allow the progress bars, or 0 if the libraries
// are unavailable or something.
#define ALLOW_PROGRESS_BARS 1


#if ALLOW_PROGRESS_BARS
extern HWND g_hWndApp;					// The main app window - app must fill this in.
#endif



struct STDVERTEX
{
    D3DXVECTOR3 v;
    D3DXVECTOR3 norm;
    FLOAT       tu, tv;
};

#define STDVERTEX_FVF (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)



// The data that gets stored inside mesh.h's tris, pts and edges.

class MeshPt;
class MeshEdge;
class MeshTri;



struct MyPt
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNorm;
	float fU, fV;

	DWORD dwIndex;

	int iMaterialNumber;	// Which material this belongs to.



	// Temporary data.

	MeshPt *pTempPt;	// Temporary data.

	D3DXVECTOR4		v4ScrPos;		// Screen pos of this vert.
	BOOL			bFrontFaced;	// TRUE if not backfaced.

};

struct MyEdge
{
	int iMaterialNumber;	// Which material this belongs to.

	// Temporary data.
	BOOL			bFrontFaced;	// TRUE if not backfaced.
};

struct MyTri
{
	int iMaterialNumber;	// Which material this belongs to.

	// Temporary data.
	BOOL			bFrontFaced;	// TRUE if not backfaced.

	int iSlidingWindowLevel;			// Which sliding window level this tri belongs to.

	DWORD dwIndex;
	MeshTri *pOriginalTri;
};


// Some defines that mesh.h wants.
#define MESHCTRL_EDGES_ALWAYS_ADDED_BEFORE_TRIS 1
#define MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_TRIS 1
#define MESHCTRL_PTS_ALWAYS_ADDED_BEFORE_EDGES 1

// The data that gets stored inside mesh.h's tris, pts and edges.
#define MESHTRI_APP_DEFINED		MyTri	mytri;
#define MESHEDGE_APP_DEFINED	MyEdge	myedge;
#define MESHPT_APP_DEFINED		MyPt	mypt;

#include "mesh.h"



struct GeneralTriInfo
{
	MeshPt		*ppt[3];
};

struct GeneralCollapseInfo
{
	DlinkDefine(GeneralCollapseInfo,List);
	
	ArbitraryList<GeneralTriInfo>		TriOriginal;
	ArbitraryList<GeneralTriInfo>		TriCollapsed;

	int			iSlidingWindowLevel;			// Which sliding window level the binned tris will belong to.
	ArbitraryList<GeneralTriInfo>		TriNextLevel;	// On collapses that change levels, lists the tris that were on the next level.

	MeshPt		*pptBin;
	MeshPt		*pptKeep;

	float		fError;					// Error of this collapse.
	int			iNumTris;				// Number of tris after this collapse has been made.


	DlinkMethods(GeneralCollapseInfo,List);


	GeneralCollapseInfo()
	{
		ListInit();
	}

	GeneralCollapseInfo ( GeneralCollapseInfo *pPrev )
	{
		ListInit();
		ListAddAfter ( pPrev );
	}

	~GeneralCollapseInfo()
	{
		ListDel();
	}
};



// This Quad only records the error due to the vertex positions.
// A real implementation needs to use smarter QEMs that take account of
// other vertex attrbiutes such as normals, texture coords, vertex colours,
// etc. Hugues Hoppe's version of this looks like the most complete and simple
// version.
//  The algo also needs to do smarter things than just look at QEMs,
// such as prevent normal-flipping, deal with degenerate collapses,
// deal with non-manifold meshes, etc.
//
// But this will do for my purposes because:
// (a) it's simple.
// (b) it's flexible - all vertices have a position.
// (c) it is good enough to give plausable collapse orders.

struct Quad
{
	float A00, A01, A02;
	float      A11, A12;
	float           A22;
	float B0, B1, B2;
	float C;


	Quad ( void )
	{
		A00 = A01 = A02 = A11 = A12 = A22 = 0.0f;
		B0 = B1 = B2 = 0.0f;
		C = 0.0f;
	}

	// Create a quad from a triangle (numbered clockwise).
	Quad ( const D3DXVECTOR3 &vec1, const D3DXVECTOR3 &vec2, const D3DXVECTOR3 &vec3 )
	{
		D3DXVECTOR3 vec12 = vec2 - vec1;
		D3DXVECTOR3 vec13 = vec3 - vec1;

		D3DXVECTOR3 vNorm;
		D3DXVec3Cross ( &vNorm, &vec12, &vec13 );
		float fArea = D3DXVec3Length ( &vNorm );
		vNorm = vNorm / fArea;
		// Use the area of the tri, not the parallelogram.
		fArea *= 0.5f;

		// Find the distance of the origin from the plane, so that
		// P*N + D = 0
		// => D = -P*N
		float fDist = - D3DXVec3Dot ( &vNorm, &vec1 );

		// And now form the Quadric.
		// A = NNt (and bin the lower half, since it is symmetrical).
		// B = D*N
		// C = D^2
		// The quadric is weighted by the area of the tri.
		A00 = fArea * vNorm.x * vNorm.x;
		A01 = fArea * vNorm.x * vNorm.y;
		A02 = fArea * vNorm.x * vNorm.z;
		A11 = fArea * vNorm.y * vNorm.y;
		A12 = fArea * vNorm.y * vNorm.z;
		A22 = fArea * vNorm.z * vNorm.z;
		B0  = fArea * vNorm.x * fDist;
		B1  = fArea * vNorm.y * fDist;
		B2  = fArea * vNorm.z * fDist;
		C   = fArea * fDist   * fDist;
	}

	const float FindError ( const D3DXVECTOR3 &vec )
	{
		return (
			A00 * vec.x * vec.x +
			A01 * vec.x * vec.y * 2 +
			A02 * vec.x * vec.z * 2 +
			A11 * vec.y * vec.y +
			A12 * vec.y * vec.z * 2 +
			A22 * vec.z * vec.z +
			B0  * vec.x * 2 +
			B1  * vec.y * 2 +
			B2  * vec.z * 2 +
			C
				);
	}

	Quad operator+ ( const Quad &q )
	{
		Quad rq;
		rq.A00 = A00 + q.A00;
		rq.A01 = A01 + q.A01;
		rq.A02 = A02 + q.A02;
		rq.A11 = A11 + q.A11;
		rq.A12 = A12 + q.A12;
		rq.A22 = A22 + q.A22;
		rq.B0  = B0  + q.B0 ;
		rq.B1  = B1  + q.B1 ;
		rq.B2  = B2  + q.B2 ;
		rq.C   = C   + q.C  ;
		return rq;
	}

	Quad &operator+= ( const Quad &q )
	{
		A00 += q.A00;
		A01 += q.A01;
		A02 += q.A02;
		A11 += q.A11;
		A12 += q.A12;
		A22 += q.A22;
		B0  += q.B0 ;
		B1  += q.B1 ;
		B2  += q.B2 ;
		C   += q.C  ;
		return (*this);
	}
};






// The various types of VIPM
enum VIPMTypeEnum
{
	VIPMType_Vanilla = 0,		// 0 must always be the first one.
	VIPMType_SlidingWindow,
	VIPMType_MultilevelSkiplist,
	VIPMType_Last,				// bogus one to show the end of the list.
};

char *VIPMTypeName ( VIPMTypeEnum type );





struct ObjectInstance;
struct Object;


class OptimisedMeshInstance;

class OptimisedMesh
{
protected:
	OptimisedMesh ( void );

	int iVersion;				// Current version number.
	BOOL bDirty;				// TRUE if dirty.
	BOOL bWillGetInfo;
public:
	


	virtual ~OptimisedMesh ( void ) = 0;

	virtual VIPMTypeEnum GetType ( void ) = 0;
	virtual char *GetTypeName ( void ) = 0;

	// Tell this method that the underlying mesh has changed.
	// bWillGetInfo = TRUE if you are going to call any of the Info* functions.
	// This causes a speed hit, so only do it when necessary.
	virtual void MarkAsDirty ( BOOL bWillGetInfo );

	// Create an instance of this optimised mesh, and returns the pointer to it.
	// Pass in the object instance you wish to associate it with.
	virtual OptimisedMeshInstance *CreateInstance ( ObjectInstance *pObjectInstance ) = 0;

	// Actually updates the OptimisedMesh to match the real mesh.
	virtual void Update ( void ) = 0;

	// Debugging check.
	virtual void Check ( void ) = 0;

	// Call before changing D3D device.
	virtual void AboutToChangeDevice ( void ) = 0;


	// Creates the given type of optimised mesh, and returns the pointer to it.
	static OptimisedMesh *Create ( VIPMTypeEnum type, Object *pObject );
};


// This is an instance of an OptimisedMesh.
class OptimisedMeshInstance
{
protected:

	int iVersion;		// Current version number;

	OptimisedMeshInstance ( void );

public:
	virtual ~OptimisedMeshInstance ( void ) = 0;

	virtual VIPMTypeEnum GetType ( void ) = 0;
	virtual char *GetTypeName ( void ) = 0;

	// Renders the given material of the object with the given level of detail.
	virtual void RenderCurrentObject ( LPDIRECT3DDEVICE8 pd3ddev, int iMaterialNumber, int iLoD ) = 0;

	// True if this instance needs to redo its data, because the related optimesh has changed.
	virtual BOOL bNeedsUpdate ( void ) = 0;

	// Update this instance to match the related optimesh, if it has changed.
	virtual void Update ( void ) = 0;

	// Call before changing D3D device.
	virtual void AboutToChangeDevice ( void ) = 0;

	

	// Gets some info about this instance.
	// Returns data about the last-rendered version.
	// Should only be called if bWillGetInfo was TRUE
	// in the last MarkAsDirty() call.

	// *pdwMemoryUsed will be the total amount of memory used by the global data.
	// *pdwOfWhichAGP will the amount of that memory in index and vertex buffers (i.e. all the data the card needs to see).
	virtual const void InfoGetGlobal ( DWORD *pdwMemoryUsed, DWORD *pdwOfWhichAGP ) = 0;

	// *pdwRealTrisDrawn will contain the number of non-degenerate tris drawn.
	virtual const void InfoGetInstance ( DWORD *pdwMemoryUsed, DWORD *pdwOfWhichAGP, DWORD *pdwVerticesLoaded, DWORD *pdwRealTrisDrawn, DWORD *pdwTotalVertices ) = 0;


	// Debugging check.
	virtual void Check ( void ) = 0;
};






struct Object
{
	// The permanent shape.
	MeshPt		PermPtRoot;
	MeshTri		PermTriRoot;
	MeshEdge	PermEdgeRoot;

	// The collapse list is ordered backwards,
	// so ptr->ListNext() is the _previous_ collapse to ptr.
	GeneralCollapseInfo		CollapseRoot;

	// The current shape.
	MeshPt		CurPtRoot;
	MeshTri		CurTriRoot;
	MeshEdge	CurEdgeRoot;

	// pNextCollapse points to the _next_ collapse to do.
	// pNextCollapse->ListNext() is the collapse that's just been done.
	// &CollapseRoot = no more collapses to do.
	GeneralCollapseInfo		*pNextCollapse;

	int			iFullNumTris;		// How many tris with no collapses.
	int			iNumCollapses;		// Total number of collapses.


	int			iCurSlidingWindowLevel;

	BOOL			bSomethingHappened;		// Set to TRUE when interesting things happen. Cleared by main app.



	OptimisedMesh		*pOptMesh[VIPMType_Last];


	Object();

	~Object();

	// Anooyingly, this requires a D3D object, even if only temporarily.
	void Object::CreateTestObject ( LPDIRECT3DDEVICE8 pd3dDevice );

	// Check that this is sensible.
	void CheckObject ( void );

	// Bins all the current data.
	void BinCurrentObject ( void );

	// Creates the current data from the permanent data.
	void MakeCurrentObjectFromPerm ( void );

	// Renders the given material of the current state of the object.
	// Set iSlidingWindowLevel to -1 if you don't care about level numbers.
	void RenderCurrentObject ( LPDIRECT3DDEVICE8 pd3ddev, int iMaterialNumber, int iSlidingWindowLevel = -1 );

	void RenderCurrentEdges ( LPDIRECT3DDEVICE8 pd3ddev, int iMaterialNumber, BOOL bIgnoreBackFaced );

	// Creates and performs a collapse of pptBinned to pptKept.
	// Make sure they actually share an edge!
	// Make sure the object is fully collapsed already.
	void CreateEdgeCollapse ( MeshPt *pptBinned, MeshPt *pptKept );

	// Bin the last collapse.
	// Returns TRUE if these was a last collapse to do.
	BOOL BinEdgeCollapse ( void );

	// Returns TRUE if a collapse was undone.
	BOOL UndoCollapse ( void );

	// Returns TRUE if a collapse was done.
	BOOL DoCollapse ( void );

	
	void SetNewLevel ( int iLevel );

	BOOL CollapseAllowedForLevel ( MeshPt *pptBinned, int iLevel );

	// Return the error from this edge collapse.
	// Set bTryToCacheResult=TRUE if you can pass pptBinned in multiple times.
	// Make sure you call this with bTryToCacheResult=FALSE if any data changes,
	//	or you'll confuse the poor thing.
	float FindCollapseError ( MeshPt *pptBinned, MeshEdge *pedgeCollapse, BOOL bTryToCacheResult = FALSE );

	// Call this if you make a change to the mesh.
	// It will mark all the OptimisedMeshes hanging off it as dirty.
	void MarkAsDirty ( void );

	// Call before D3D leaves.
	void AboutToChangeDevice ( void );

};



struct ObjectInstance
{
	DlinkDefine(ObjectInstance,List);

	// Orientation of instance;
	D3DXMATRIX		matOrn;
	// The parent object.
	Object			*pObj;

	int				iRenderMethod;		// -1 = naive, >=0 is an OptMesh type.
	int				iCurCollapses;		// Current number of collapses done (from full-rez).


	OptimisedMeshInstance		*pOptMeshInst[VIPMType_Last];



	DlinkMethods(ObjectInstance,List);


	ObjectInstance ( Object *pObject = NULL, ObjectInstance *pRoot = NULL );

	~ObjectInstance ( void );


	void RenderCurrentObject ( LPDIRECT3DDEVICE8 pd3ddev, int iMaterialNumber, int iSlidingWindowLevel = -1, BOOL bShowOptiMesh = FALSE );

	void RenderCurrentEdges ( LPDIRECT3DDEVICE8 pd3ddev, int iMaterialNumber, BOOL bIgnoreBackFaced )
	{
		pObj->RenderCurrentEdges ( pd3ddev, iMaterialNumber, bIgnoreBackFaced );
	}

	void SetNumCollapses ( int iNum );

	int GetNumCollapses ( void );


	// This shortcut breaks data-hiding. Careful.
	OptimisedMeshInstance *GetOpiMeshInst ( void )
	{
		if ( iRenderMethod >= 0 )
		{
			return pOptMeshInst[iRenderMethod];
		}
		else
		{
			return NULL;
		}
	}



	// Call before D3D leaves.
	void AboutToChangeDevice ( void );

};








