/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "baseos.h"
#include "basemath.h"
#include "scene.h"
#include "bsptree.h"
#include <gl/glu.h>
#include <assert.h>
#include "z_ed3View.h"
#include "Mainfrm.h"
#include "z_ed2Doc.h"

//---------------------------------------------------------------------------------------
// Relative positions of a face with the respoct of a plane
#define	PON_FRONT   	1
#define	PON_BACK		2
#define	PON_BOTH		(PON_FRONT|PON_BACK)
#define	PON_FACE		4           


//---------------------------------------------------------------------------------------
// Returns the bounding box of 'pcts' vertexes
static BBox GetVxesBbox(vvector<VTCIS>& pcts)
{
    BBox b;
    FOREACH(vvector<VTCIS>, pcts, pp){
        b.AddPoint(pp->_xyz);
    }
    return b;
}


//----------------------------------------------------------------------------------------
// Bsp tree class default constructor
CBspTree::CBspTree():_mainBsp(FALSE)
{
}

//----------------------------------------------------------------------------------------
// Destructor
CBspTree::~CBspTree()
{
    Clear();
}

//----------------------------------------------------------------------------------------
// Clear all data from the bsp
void    CBspTree::Clear()
{
    _leafs.clear();
    MiniBsp::Clear();
}

//----------------------------------------------------------------------------------------
// Compile the bsp. Pass the compilation to the base class
BOOL CBspTree::Compile(vvector<Poly>& polys, BOOL bLog) 
{
    return MiniBsp::Compile(polys, bLog);
}

//----------------------------------------------------------------------------------------
// Iterates all polygons and calculates the bounding box.
void	CBspTree::UpdateNodeBB(CMiniNode* pNode, vvector<Poly>& polys)
{
    FOREACH(vvector<Poly>, 	polys, poly){
        poly->Recalc();
        pNode->_bbox.Union(poly->_box);
    }
}
