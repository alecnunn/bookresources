#ifndef _WIDGETBSP_H
#define _WIDGETBSP_H

#include "widgetmesh.h"

#define	LEAVESPERNODE	(8)

//-----------------------------------------------------------------
//	widget leaf
//-----------------------------------------------------------------
typedef struct
{
    float   position[3];   // position of the widget
    s8      sinAngle;      // sin of its orientation (*127)
    s8      cosAngle;      // cos of its orientation (*127)
    u8      scale;         // scale of this widget (*32)
    u8      count;         // # of consectutive leaves to come

} WIDGETLEAF;

//-----------------------------------------------------------------
//	widget bsp node
//-----------------------------------------------------------------
typedef struct
{
    u32     axis               : 2;
    u32     numFrontLeaves     : 4;
    u32     numBackLeaves      : 4;
    u32     backNodeOffset     : 22;
    float   distance;

} WIDGETNODE;

//-----------------------------------------------------------------
//	widget bsp tree class
//-----------------------------------------------------------------
class CWidgetBSP
{
public:
	CWidgetBSP();
	~CWidgetBSP();

	void	CreateTree( CWidgetMesh *pMesh, WIDGETLEAF *pLeaves, u32 uNumLeaves );
	void	DestroyTree();

	void	Draw( const D3DXMATRIX &rViewMtx, const float fov[2], float fFar=50.0f );

protected:

	void	DrawNode( const WIDGETNODE *pNode );
	void	DrawLeaves( const WIDGETLEAF *pLeaves, u32 uNumLeaves );
	void	CalculateViewBox( const D3DXMATRIX &rViewMatrix, const float fFov[2], float fFar );
	
	u32			m_uTreeLength;
	CWidgetMesh	*m_pMesh;
	WIDGETNODE	*m_pTree;

	float		m_viewBox[3][2];
};

#endif
