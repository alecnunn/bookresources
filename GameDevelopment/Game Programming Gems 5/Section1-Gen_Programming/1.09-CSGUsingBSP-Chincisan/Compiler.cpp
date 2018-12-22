/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "csg_bsp.h"
#include "Scene.h"
#include "z_ed2Doc.h"
#include "BspTree.h"
#include "z_ed3View.h"
#include "MainFrm.h"


//---------------------------------------------------------------------------------------
// Global var to access the compiler. Compiler Wraps the CSG operations
Compiler* Compiler::PCompiler;

//---------------------------------------------------------------------------------------
// Compiler Constructor
Compiler::Compiler()
{
    PCompiler          = this;
};

//---------------------------------------------------------------------------------------
//  Hidden Surfaces Removal. Performs CSG Union Between all in-touch results
BOOL    Compiler::PerformHSR(Scene& scene)
{
    BOOL brv = FALSE;

    PBrushes* pBrushes =  scene.GetPrimitives();
    if( 0 == pBrushes->size())
        return brv;

    PBrushes    brshColl;
    AfxGetApp()->DoWaitCursor(1);
    
    // Clear prev output brushes
    scene.ClearBrushes();

    // Create the new HSR-ed brushes (Hidden Surface Removal By CSG Union)
    FOREACH(PBrushes, (*pBrushes), ppBrsh)
    {
        Brush* pBrush = (*ppBrsh)->_pResult;
        // collect all of them in one collection
        if((*ppBrsh)->_flags & BRSH_SOLID)
        {
            pBrush->_flags |= (BRSH_SOLID);
            pBrush->Recalc();
            brshColl.push_back(pBrush);
        }
    }

    if(brshColl.size() != 0)
    {
        Brush* pResultSceneBrsh = new Brush();
        pResultSceneBrsh->_flags |= BRSH_SOLID;
        
        // Union the solids into  'pResultSceneBrsh'
        if(Union(brshColl, *pResultSceneBrsh,1)){
            scene.AddBrush(pResultSceneBrsh);
            brv = TRUE;
        }
        else{
            delete pResultSceneBrsh;
            pResultSceneBrsh = 0;
        }
    }
    AfxGetApp()->DoWaitCursor(0);
    return brv ;
}

//---------------------------------------------------------------------------------------
// Unions all solids in one big solid brush 'brush'. 
BOOL    Compiler::Union(PBrushes& pPrimitives, Brush& brush, BOOL sgow)
{
    Polys       firstPolys;
    Polys       pSplitters;
    Polys       splittedPolys;
    Polys       outPolys;
    int	        bClipIt  = FALSE;
    Brush* pB1      = NULL;
    Brush* pB2      = NULL;

    // counters for progress counter UI
    int counter[3] = {0};
    counter[2]     = pPrimitives.size();

    int         cntInPolys = 0;
    int         cntOutPolys = 0;

    // iterate each solid
    FOREACH(PBrushes, pPrimitives, ppPrim)
    {
        bClipIt = FALSE;
        pB1     = *ppPrim;

        // get polygons copy 
        pB1->GetTxPolysCopy(firstPolys);    
        cntInPolys += firstPolys.size();

        // and compare it with another one
        FOREACH(PBrushes, pPrimitives, ppPrim1){
            // skip the same
            pB2 = *ppPrim1;
            if (pB1 == pB2){
                bClipIt = TRUE;
                continue;
            }
            // Fast touch elimination between ppPrim  and ppPrim1 brush            
            if(!pB1->_box.IsTouchesBox(pB2->_box)){
                continue;
            }
            // BSP the polygons and clip them acording to the union operation
            do{                              // scope this for bsp clean up
                MiniBsp bsp;
                pB2->GetTxPolysCopy(pSplitters);
                if(!bsp.Compile(pSplitters,FALSE))
                    return FALSE;
                bsp.ClipBrPolys(firstPolys, splittedPolys, bClipIt,1,1);
                firstPolys.clear();
                firstPolys = splittedPolys;
                pSplitters.clear();
                splittedPolys.clear();
            }while(0);
            counter[0]++;
        }
        // collect remaining polygons
        colex::append(outPolys, firstPolys);
        firstPolys.clear();
        counter[1]++;
    }

    // add them to final solid brush
    if (outPolys.size()){
        FOREACH(Polys, outPolys, op){
            op->Recalc();
            op->_pBrush = &brush;
            brush._polys.push_back(*op);
            cntOutPolys++;
        }
        brush.Recalc();
        brush._flags |= BRSH_SOLID;
    }
    return TRUE;
}

