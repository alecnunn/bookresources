/*******************************************************************\
| ABT.h
|
|--------------------------------------------------------------------
| Copyright 2004/2005. Martin Fleisz.
| All rights reserved.
|
|--------------------------------------------------------------------
| CREATED:		2005/8/10
| AUTHOR:		Martin Fleisz
|
|--------------------------------------------------------------------
| DESCRIPTION:
| 
| ABT Compiler/Renderer class
\********************************************************************/
#ifndef __ABT_H
#define __ABT_H

#include "OGLExt.h"
#include "Types.h"
#include "DataLoader.h"


// defines the maximum number of primitives in an ABT node
#define ABT_MAX_NODEPRIMITIVES		5000

// defines the number of samples along each axis
#define ABT_PLANE_SAMPLES			  10

#define ABT_FRONT		1
#define ABT_BACK		2
#define ABT_COINCIDING	3
#define ABT_SPANNING	4

// this is defined because due to calculation errors with float variables
// a coinciding plane could have a non 0.0f result.
#define ABT_CLASSIFYPOINTDISTANCE_NULL	0.0000001f

// score weightings
#define ABT_WEIGHT_SPACELOCATION	0.15f
#define ABT_WEIGHT_CHILDVOLUMES		0.10f
#define ABT_WEIGHT_FACECOUNT		0.20f
#define ABT_WEIGHT_SPLITFACES		0.55f

// growth factor (10% here)
#define ABT_GROWTH_FACTOR			0.10f


//////////////////////////////////
// structure for a node in our ABT
typedef struct _ABTNode
{
	bool			bIsLeaf;		// true if this node is a leaf, false otherwise
	cAABoundingBox	aabb;			// axis-aligned bounding box enclosing the geometry in this node
    
	GLuint			iVertexBuffer;	// references to the vertex buffers object
	uint32			iVertexCount;	// number of vertices in the vertex buffer object

	_ABTNode		*pChildA;		// child nodes
	_ABTNode		*pChildB;
} ABTNode;


////////////
// ABT class
class cABT
{
	public:

		cABT();
		~cABT();

		bool	create();
		uint32	render();
		void	release();

	private:

		ABTNode*	r_create(vector<Vertex> &p_Vertex);
		void		r_render(ABTNode *p_Node);
		void		r_release(ABTNode *p_Node);


		void		getAABB(vector<Vertex> &p_Vertex, cAABoundingBox *p_AABB);
		void		getSplitter(vector<Vertex> &p_Vertex, cAABoundingBox *p_AABB, cPlane *p_Plane);
		void		splitTriangle(cPlane *p_Plane, Vertex *p_Vertex, vector<Vertex> &p_Front, vector<Vertex> &p_Back);
		uint32		getTriangleSide(cPlane *p_Plane, Vertex *p_Vertex);
		uint32		classifyPoint(cPlane *p_Plane, Vertex *p_Vertex);

		void		updateFrustum();
		bool		isInFrustum(cAABoundingBox &p_AABB);


		ABTNode		*m_Root;
		float32		m_Frustum[6][4];	// viewing frustum
		cOGLExt		m_OGLExt;			// opengl extension helper class
		uint32		m_NumRenderedTris;	// number of rendered triangles
		uint32		m_NumFinalTris;		// number of triangles in the final ABT
};



#endif  // __ABT_H
