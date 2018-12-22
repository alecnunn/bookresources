// ompiler.cpp: implementation of the Compiler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Bsp_Techniques.h"
#include "Bsp_TechniquesDlg.h"
#include "compiler.h"


//---------------------------------------------------------------------------------------
BOOL  Compiler::Compile(Brush& brush, CBsp_TechniquesDlg& theDlg, BOOL usethreads)
{
    p_brush      = &brush;
    p_dlg        = &theDlg;
    b_usethreads = usethreads;
    Start();
    return 1;
}


//---------------------------------------------------------------------------------------
int  Compiler::ThreadMain()
{
    TextOut(ST_OUT7, "");
    bsp_tree.Clear();
    portal_procesor.Clear();
    pvs_procesor.Clear();
    if(p_brush->_flags & BRSH_TERRAIN)
    {
        bsp_tree.SetType(Bsp_Tree::BSP_TERRAIN);
    }
    
    bsp_tree.Process(p_brush->_polys, b_usethreads);

    if(!(p_brush->_flags & BRSH_TERRAIN))
    {
        portal_procesor.Process(bsp_tree);
        pvs_procesor.Process(bsp_tree, portal_procesor);
    }
    p_dlg->PostMessage(WM_COMPILE_DONE);
    return 1;
}

//---------------------------------------------------------------------------------------
void  Compiler::Render(UINT mode, BOOL is3D)
{
    switch(mode)
    {
        case RB_BSP:
            RenderBSP(is3D);
            break;
        case RB_PORTALS:
            RenderPortals(is3D);
            break;
        case RB_PORTALSBSP:
            if(is3D)
            {
                RenderBSP(is3D);
                RenderPortals(is3D);
            }
            else
            {
                RenderBSP(is3D);
                RenderPortals(is3D);
            }
            break;
    }
}

//---------------------------------------------------------------------------------------
static BOOL	Local_IsLeafVisFromLeaf(vvector<CLeaf*>& leafs, int l1, int l2, BYTE* ppvs)
{
	if(l1<0||l2<0)return 1;
    BYTE* pPvs1 = &ppvs[leafs[l1]->_pvsIdx];
    return (pPvs1[l2 >> 3] & (1<<(l2 & 7)) )!=0;
}

//---------------------------------------------------------------------------------------
void Compiler::RenderBSP(BOOL is3D)
{
    V3  camPos  = p_dlg->gl_3d._cam._wpos;
    int curLeaf = bsp_tree.GetCurrentLeaf(camPos);
    BOOL          bvisible ;
    int           nVisPOlys=0;
    vvector<Poly*>  visibles(64);
 
    TextOut(ST_OUT, "Current Leaf=%d", curLeaf);

    if(!is3D)
    {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(0);
    }


    glColor4ubv(CLR_WHITE);
    glDisable(GL_BLEND);

    // from this leaf check the pvs and render all visible leafs
    FOREACH(vvector<CLeaf*>, bsp_tree._leafs, pNode)
    {
        bvisible  = TRUE;
        if(curLeaf!=-1) // if we are in interior of the BSP
        {
	        if((*pNode)->_pvsIdx >=0 && (*pNode)->_leafIdx != curLeaf)
	        {
		        if(!Local_IsLeafVisFromLeaf(bsp_tree._leafs, (*pNode)->_leafIdx, curLeaf, pvs_procesor._pvs))
                {
			        bvisible = FALSE;
                }
	        }

            if(Bsp_Tree::BSP_TERRAIN == bsp_tree._bsptype)
            {
	            if(!bvisible || p_dlg->gl_3d._cam.IsCulled(&(*pNode)->_bbox._min,&(*pNode)->_bbox._max))
                    bvisible = FALSE;
                else
                    bvisible = TRUE;
            }
        }
        if(FALSE == bvisible && is3D)
            continue;

        int nCount;
        Poly*   pPolyh = (*pNode)->GetPolys(nCount);
        for(int i=0;i<nCount;i++, pPolyh++)
        {
            if(!bvisible)
            {
                glColor4ubv(CLR_GREY);

                if(is3D)
                    p_dlg->gl_3d.DrawPolyVertexes(*pPolyh);
                else
                {
                    if((pPolyh)->_n.y<.9 && (pPolyh)->_n.y>-.9)
                        p_dlg->gl_2d.DrawPolyLines(*pPolyh);
                }
            }
            else
            {
                 visibles.push_back(pPolyh);
            }
        }
    }

    FOREACH(vvector<Poly*>, visibles, ppp)
    {
        glColor4ubv(CLR_WHITE);
        if(is3D)
            p_dlg->gl_3d.DrawPolyVertexes(**ppp);
        else
        {
            if((*ppp)->_n.y<.9 && (*ppp)->_n.y>-.9)
                p_dlg->gl_2d.DrawPolyLines(**ppp);
        }

    }

    glColor4ubv(CLR_WHITE);
    TextOut(ST_OUT2, "Vis =%d of %d (%02.2f%%)", visibles.size(), bsp_tree._polys.size(), 
            (REAL)visibles.size()*100.0/(REAL)bsp_tree._polys.size());
}

//---------------------------------------------------------------------------------------
void Compiler::RenderPortals(BOOL is3D)
{
    glDisable(GL_TEXTURE_2D);
    glColor4ubv(CLR_WHITE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    
    int ii=0;
    FOREACH(vvector<Portal>, portal_procesor._portals, pPrt)
    {
        if(is3D)
        {
            if(pPrt->_flags & PORT_DUP)
            {
                glColor4ubv(CLR_PORTALS1);
            }
            else
            {
                glColor4ubv(CLR_PORTALS);
            }

            glBegin(GL_POLYGON);
            FOREACH(vvector<V3>, pPrt->_vxes, pv)
            {
                glVertex3f(pv->x,pv->y,pv->z);
            }
            glEnd();
        }

        glColor4ubv(CLR_PORTALSLINE);
        glBegin(GL_LINE_LOOP);
        FOREACH(vvector<V3>, pPrt->_vxes, pv)
        {
            glVertex3f(pv->x,pv->y,pv->z);
        }
        glEnd();
    }
    glColor4ubv(CLR_WHITE);
    glDisable(GL_BLEND);
    glDepthMask(1);
    glEnable(GL_DEPTH_TEST);
    if(is3D)
        glEnable(GL_TEXTURE_2D);
}


void     Compiler::Clear()
{
    portal_procesor.Clear();  
    pvs_procesor.Clear();  
    bsp_tree.Clear();  
    p_brush = 0;

}