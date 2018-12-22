/*
	Csg by Bsp.
	Author:			Marius Chincisan, 
	Developed for	Game Programming Gems 5
*/
#include "stdafx.h"
#include "csg_bsp.h"
#include "z_ed2Doc.h"
#include "mainfrm.h"
#include "Z_ed2View.h"
#include "Z_ed3View.h"
#include "baseutils.h"
#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------------------
BOOL    GSuported=0;

//---------------------------------------------------------------------------------------
BBox   CZ_ed2Doc::GetReferenceBrushBox()
{
    DeleteUncomittedNewBrushes();
    return _pSelBrushes.back()->_box;
}

//---------------------------------------------------------------------------------------
static BBox GetSelBrushesBBox(PBrushes& selbrushes)
{
    BBox box;
    FOREACH(PBrushes, selbrushes, ppb)
    {
        box.Union((*ppb)->_box);
    }
    return box;
}

//---------------------------------------------------------------------------------------
static V3    GetFilledSpace(PBrushes& selbrushes)
{
    V3 allDims;
    FOREACH(PBrushes, selbrushes, ppb)
    {
        allDims+=(*ppb)->_box.GetExtends();
    }
    return allDims;
}
//---------------------------------------------------------------------------------------
// sorts brush by x displacement
int SortByX(const void* b1, const void* b2)
{
    REAL displ = ((Brush*)b1)->_box.GetCenter().x-((Brush*)b2)->_box.GetCenter().x;
    if(displ > 0)
        return 1;
    else if(displ < 0)
        return -1;
    return 0;
}

//---------------------------------------------------------------------------------------
// sorts brush by x displacement
int SortByY(const void* b1, const void* b2)
{
    REAL displ = ((Brush*)b1)->_box.GetCenter().y-((Brush*)b2)->_box.GetCenter().y;
    if(displ > 0)
        return 1;
    else if(displ < 0)
        return -1;
    return 0;

}

//---------------------------------------------------------------------------------------
// sorts brush by x displacement
int SortByZ(const void* b1, const void* b2)
{
    REAL displ = ((Brush*)b1)->_box.GetCenter().z-((Brush*)b2)->_box.GetCenter().z;
    if(displ > 0)
        return 1;
    else if(displ < 0)
        return -1;
    return 0;
}

//---------------------------------------------------------------------------------------
// more brushes. the other operations can be done by acting on specific brush
void    CZ_ed2Doc::UndoUncomittedBrushes()
{
    FOREACH(PBrushes,  _pSelBrushes, ppb)
    {
        if((*ppb)->IsDirty())
        {
            (*ppb)->Move(-(*ppb)->_t);
        }
        (*ppb)->Recalc();
        (*ppb)->Dirty(0);
    }
    Recut();
}


