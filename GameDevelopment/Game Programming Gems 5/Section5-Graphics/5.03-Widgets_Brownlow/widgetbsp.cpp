//-----------------------------------------------------------------
//	widget bsp
//-----------------------------------------------------------------
#include <stdafx.h>
#include <assert.h>
#include "widgetbsp.h"


//-----------------------------------------------------------------
//	widget bsp
//-----------------------------------------------------------------
CWidgetBSP::CWidgetBSP()
{
	m_uTreeLength = 0;
	m_pMesh = 0;
	m_pTree = 0;
}

CWidgetBSP::~CWidgetBSP()
{
	DestroyTree();
}

//-----------------------------------------------------------------
//	destroy the tree
//-----------------------------------------------------------------
void CWidgetBSP::DestroyTree()
{
	FREE(m_pTree);
	m_uTreeLength = 0;
	m_pMesh = 0;
}


//-----------------------------------------------------------------
//	create a tree
//-----------------------------------------------------------------
static u32	_uSortAxis = 0;
static int _sortLeaves( const void *pa, const void *pb )
{
	WIDGETLEAF	*p1 = (WIDGETLEAF*)pa;
	WIDGETLEAF	*p2 = (WIDGETLEAF*)pb;
	if( p1->position[_uSortAxis] > p2->position[_uSortAxis] )
		return 1;
	if( p1->position[_uSortAxis] < p2->position[_uSortAxis] )
		return -1;
	return 0;
}

static void *_CreateTree( void *ptr, WIDGETLEAF *pLeaves, u32 uNumLeaves )
{
	float		bbox[2][3];
	u32			i,j,uSplit;
	WIDGETNODE	*pNode;

	// make room for this widget node
	pNode = (WIDGETNODE*)ptr;
	ptr   = IncPointer(ptr,sizeof(WIDGETNODE));

	// get the bounding box of these leaves
	bbox[0][0] = bbox[1][0] = pLeaves[0].position[0];
	bbox[0][1] = bbox[1][1] = pLeaves[0].position[1];
	bbox[0][2] = bbox[1][2] = pLeaves[0].position[2];
	for( i=1;i<uNumLeaves;i++ )
	{
		for( j=0;j<3;j++ )
		{
			if( pLeaves[i].position[j]<bbox[0][j] )
			{
				bbox[0][j] = pLeaves[i].position[j];
			}
			else if( pLeaves[i].position[j]>bbox[1][j] )
			{
				bbox[1][j] = pLeaves[i].position[j];
			}
		}
	}

	// find the largest axis
	bbox[1][0] -= bbox[0][0];
	bbox[1][1] -= bbox[0][1];
	bbox[1][2] -= bbox[0][2];
	if( bbox[1][0] > bbox[1][1] && bbox[1][0] > bbox[1][2] )
		pNode->axis = 0;
	else if( bbox[1][1] > bbox[1][2] )
		pNode->axis = 1;
	else
		pNode->axis = 2;

	// sort the leaves on this axis
	_uSortAxis = pNode->axis;
	qsort(pLeaves,uNumLeaves,sizeof(WIDGETLEAF),_sortLeaves);

	// find the median leaf
	uSplit = uNumLeaves>>1;
	pNode->distance = pLeaves[uSplit].position[pNode->axis];

	// split the leaves at this point
	// create the front tree
	if( uSplit>LEAVESPERNODE )
	{
		pNode->numFrontLeaves = 0;
		ptr = _CreateTree(ptr,pLeaves,uSplit);
	}
	else
	{
		pNode->numFrontLeaves = uSplit;
		memcpy(ptr,pLeaves,uSplit*sizeof(WIDGETLEAF));
		ptr = IncPointer(ptr,uSplit*sizeof(WIDGETLEAF));
	}

	// create the back tree
	pLeaves += uSplit;
	uSplit   = uNumLeaves - uSplit;

	// check we haven't overflowed the offset amount
	pNode->backNodeOffset = ((WIDGETNODE*)ptr)-pNode;
	assert(ptr==(void*)&pNode[pNode->backNodeOffset]);

	if( uSplit>LEAVESPERNODE )
	{
		pNode->numBackLeaves = 0;
		ptr = _CreateTree(ptr,pLeaves,uSplit);
	}
	else
	{
		pNode->numBackLeaves = uSplit;
		memcpy(ptr,pLeaves,uSplit*sizeof(WIDGETLEAF));
		ptr = IncPointer(ptr,uSplit*sizeof(WIDGETLEAF));
	}
	return ptr;
}

//-----------------------------------------------------------------
//	create the tree
//-----------------------------------------------------------------
void CWidgetBSP::CreateTree( CWidgetMesh *pMesh, WIDGETLEAF *pLeaves, u32 uNumLeaves )
{
	u32		uNumNodes,uLength;
	void	*ptr;

	assert(sizeof(WIDGETNODE)==8);
	assert(sizeof(WIDGETLEAF)==16);

	DestroyTree();

	// there MUST be at least 2 leaves to make a tree...
	if( uNumLeaves>1 )
	{
		uNumNodes = (uNumLeaves+(LEAVESPERNODE-1))/LEAVESPERNODE;
		uNumNodes = uNumNodes*2;
		m_uTreeLength = uNumNodes*sizeof(WIDGETNODE) + uNumLeaves*sizeof(WIDGETLEAF);

		m_pTree = (WIDGETNODE*)malloc(m_uTreeLength);
		if( m_pTree )
		{
			m_pMesh = pMesh;

			// create the tree
			ptr = _CreateTree((void*)m_pTree,pLeaves,uNumLeaves);

			uLength = ((u8*)ptr) - ((u8*)m_pTree);
			assert(uLength<=m_uTreeLength);
		}
	}
}

//-----------------------------------------------------------------
//	calculate the view box
//-----------------------------------------------------------------
void CWidgetBSP::CalculateViewBox( const D3DXMATRIX &rViewMatrix, const float fFov[2], float fFar )
{
	float		pos;
	float		tanfov[2];
	float		factor[2];
	u32			i,j;

	// calculate a view box
	tanfov[0] = tanf(fFov[0]*0.5f);
	tanfov[1] = tanf(fFov[1]*0.5f);

	m_viewBox[0][0] = m_viewBox[0][1] = rViewMatrix(3,0);
	m_viewBox[1][0] = m_viewBox[1][1] = rViewMatrix(3,1);
	m_viewBox[2][0] = m_viewBox[2][1] = rViewMatrix(3,2);
	for( i=0;i<4;i++ )
	{
		factor[0] = tanfov[0]*((i&1)?1.0f:-1.0f);
		factor[1] = tanfov[1]*((i&2)?1.0f:-1.0f);

		for( j=0;j<3;j++ )
		{
			pos  = rViewMatrix(3,j) + rViewMatrix(2,j)*fFar;
			pos += rViewMatrix(0,j)*factor[0]*fFar;
			pos += rViewMatrix(1,j)*factor[1]*fFar;

			if( pos < m_viewBox[j][0] )
				m_viewBox[j][0] = pos;
			else if( pos > m_viewBox[j][1] )
				m_viewBox[j][1] = pos;
		}
	}
}

//-----------------------------------------------------------------
//	draw the tree
//-----------------------------------------------------------------
void CWidgetBSP::Draw( const D3DXMATRIX &rViewMatrix, const float fFov[2], float fFar )
{
	if( m_pMesh && m_pTree )
	{
		CalculateViewBox(rViewMatrix,fFov,fFar);

		m_pMesh->BeginDraw();
		DrawNode(m_pTree);
		m_pMesh->EndDraw();
	}
}

//-----------------------------------------------------------------
//	draw a node in the tree
//-----------------------------------------------------------------
void CWidgetBSP::DrawNode( const WIDGETNODE *pNode )
{
	const WIDGETNODE	*nextNode;

	// draw front node?
	if( pNode->distance >= m_viewBox[pNode->axis][0] )
	{
		nextNode = &pNode[1];
		if( pNode->numFrontLeaves!=0 )
		{
			DrawLeaves((WIDGETLEAF*)nextNode,pNode->numFrontLeaves);
		}
		else
		{
			DrawNode(nextNode);
		}
	}

	// draw back node?
	if( pNode->distance <= m_viewBox[pNode->axis][1] )
	{
		nextNode = &pNode[pNode->backNodeOffset];
		if( pNode->numBackLeaves!=0 )
		{
			DrawLeaves((WIDGETLEAF*)nextNode,pNode->numBackLeaves);
		}
		else
		{
			DrawNode(nextNode);
		}
	}
}


//-----------------------------------------------------------------
//	draw a node in the tree
//-----------------------------------------------------------------
void CWidgetBSP::DrawLeaves( const WIDGETLEAF *pLeaves, u32 uNumLeaves ) 
{
	while( uNumLeaves-- )
	{
		m_pMesh->AddInstance(	pLeaves->position,
								pLeaves->scale/32.0f,
								pLeaves->sinAngle/127.0f,
								pLeaves->cosAngle/127.0f);
		pLeaves++;
	}
}
