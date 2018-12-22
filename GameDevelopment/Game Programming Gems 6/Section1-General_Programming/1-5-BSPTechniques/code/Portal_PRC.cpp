#pragma warning (disable, 4786)
#include "stdafx.h"
#include "resource.h"
#include "compiler.h"
#include <set>
#include "Bsp_TechniquesDlg.h"
//---------------------------------------------------------------------------------------
static REAL GEpsilon =  1.0f/8.0f;
//---------------------------------------------------------------------------------------
Box GetVxesBbox(vvector<V3>& vxes)
{
    Box b;
    FOREACH(vvector<V3>, vxes, pv)
        b.AddPoint(*pv);
    return b;
}

//---------------------------------------------------------------------------------------
BOOL    R_SearchLeaf(vvector<CBspNode*>& nodes, CBspNode* pStartNode, CBspNode* p2Find)
{
    if(pStartNode->IsLeaf())
    {
        return (pStartNode == p2Find);
    }
    
    if(R_SearchLeaf(nodes, nodes[pStartNode->_nodeidx[0]], p2Find))
        return TRUE;
    if(R_SearchLeaf(nodes, nodes[pStartNode->_nodeidx[1]], p2Find))
        return TRUE;
    return FALSE;
}

//---------------------------------------------------------------------------------------
CBspNode* R_FindCommonParent(vvector<CBspNode*>& nodes,  
                             CBspNode* pNotGo,
                             CBspNode* pUpNode, 
                             CBspNode* pSec)
{
    CBspNode* p2Go;
    
    if(pNotGo == pUpNode->Front())
        p2Go = pUpNode->Back();
    else
        p2Go = pUpNode->Front();
    
    if(R_SearchLeaf(nodes, p2Go, pSec))
        return pUpNode;
    if(pUpNode->_idxNodeThis == 0)
    {
        if(R_SearchLeaf(nodes, pUpNode, pSec))
            return pUpNode;
    }
    return R_FindCommonParent(nodes,  pUpNode, nodes[pUpNode->_idxParent], pSec);
}

//---------------------------------------------------------------------------------------
CBspNode* FindCommonParent(vvector<CBspNode*>& nodes,  CBspNode* pLa, CBspNode* pLb)
{
    return R_FindCommonParent(nodes,  pLa, nodes[pLa->_idxParent], pLb);
}

//---------------------------------------------------------------------------------------
void    Portal::Split(Plane& plane, Portal& a, Portal& b)
{
    a._vxes.clear();
    b._vxes.clear();
    a.CopyProps(*this);
    b.CopyProps(*this);

    if(IsZero(plane._c - _c))
    {
        if(IsZero(Vdp(_n, plane._n)-1))
        {
            a._vxes = _vxes;
            return ;
        }
    }
    

	V3  iv;
    V3 itxB = *_vxes.begin();
    V3 itxA = _vxes.back();

	REAL    fB;
    REAL	fA = plane.DistTo(itxA);
    
    FOREACH(vvector<V3>, _vxes, vxI)
    {
        itxB = *vxI;
        fB   = plane.DistTo(itxB);
        if(fA < GEpsilon && fA > -GEpsilon)fA=0;
        if(fB < GEpsilon && fB > -GEpsilon)fB=0;
        if(fB > GEpsilon)
        {
            if(fA < -GEpsilon)
            {
                REAL   t = -fA /(fB - fA);
				iv.interpolate(itxA,itxB,t);
                a._vxes << iv;
                b._vxes << iv;
            }
            a._vxes<<itxB;
        }
        else if(fB < -GEpsilon)
        {
            if(fA > GEpsilon)
            {
                REAL t = -fA /(fB - fA);           // t of segment
				iv.interpolate(itxA,itxB,t);
                a._vxes<<iv;
                b._vxes <<iv;
            }
            b._vxes <<itxB;
        }
		else
		{
			a._vxes << itxB;
            b._vxes << itxB;
		}

        itxA = itxB;
        fA   = fB;
    }

    if(a._vxes.size()<=3)
    {
        if(b._vxes.size())
            b._vxes = _vxes;
    }
    else if(b._vxes.size()<=3)
    {
        if(a._vxes.size())
            a._vxes = _vxes;
    }
}

//---------------------------------------------------------------------------------------
void    Portal_Prc::Clear()
{
    _portals.clear();
    dw_deltatime = GetTickCount();
}

//---------------------------------------------------------------------------------------
// build the new leafs tha will hold additional flags. we do not copy the
void    Portal_Prc::Process(Bsp_Tree& tree)
{
    int                 cleafIdx1 = 0;
    vvector<Portal>     portals;
    
    vector<pair<int,int> >  uniques;

    TextOut(ST_OUT6, "Generating Portals. Please Wait...");

    _pTree = &tree;
    Clear();
    dw_deltatime=GetTickCount();
    FOREACH(vvector<CLeaf*>, tree._leafs, pp1)
    {
        TextOut(ST_OUT7, "Processing Portals for Leaf : %d", cleafIdx1);

        FOREACH(vvector<CLeaf*>, tree._leafs, pp2)
        {
            if(pp1 == pp2) 
                continue;
            CLeaf* pl1 = *pp1;
            CLeaf* pl2 = *pp2;
            
            pair<int,int> p(pl1->_leafIdx,pl2->_leafIdx);
            if(find(uniques.begin(), uniques.end(), p) != uniques.end())
                continue;

            Box b1 = pl1->_bbox;
            Box b2 = pl2->_bbox;

            b1.Expand(4);
            b1.Expand(4);
            if(b1.IsTouchesBox(b2))
            {
                CBspNode* pNode = FindCommonParent(tree._nodes,  pl1, pl2);
                Portal    portal(this);
            
                CalculateInitialPortal(portal, pNode);
            
                ClipWithLeafSides(pl1, portal);
                ClipWithLeafSides(pl2, portal);

                portal.AddSideLeafIdx(pl1->_leafIdx);
                portal.AddSideLeafIdx(pl2->_leafIdx);
                if(ValidatePortal(portal))
                {
                    portals << portal;
                }
                p.first = pl1->_leafIdx;
                p.second= pl2->_leafIdx;
                uniques.push_back(p);

                p.first = pl2->_leafIdx;
                p.second= pl1->_leafIdx;
                uniques.push_back(p);

            }
        }
        ++cleafIdx1;    
    }
    DuplicatePortals(portals, _portals);
    dw_deltatime = GetTickCount() - dw_deltatime;
    TextOut(ST_OUT2, "Portal Time: %d ms", dw_deltatime);
}

//---------------------------------------------------------------------------------------
BOOL    Portal_Prc::ValidatePortal(Portal& portal)    
{
    V3  points[2];
    V3 center = portal.GetCenter();
    points[0] = center - portal._n * 8;
    points[1] = center + portal._n * 8;

    int nleal1 = _pTree->GetCurrentLeaf(points[0]);
    int nleal2 = _pTree->GetCurrentLeaf(points[1]);

    if((nleal1 == portal._sideLIdx[0] && nleal2 == portal._sideLIdx[1]) ||
       (nleal1 == portal._sideLIdx[1] && nleal2 == portal._sideLIdx[0]))
    {

        if(nleal1 == portal._sideLIdx[0])
            return TRUE;

        int temp = portal._sideLIdx[0];
        portal._sideLIdx[0]  = portal._sideLIdx[1];
        portal._sideLIdx[1] = temp;
        return TRUE;

    }
    return FALSE;
}

//---------------------------------------------------------------------------------------
static V3	GetMajorAxes(V3& normal)
{
    V3	rv;
    if( Rabs(normal.y) > Rabs(normal.z) ) 
    {
        if( Rabs(normal.z) < Rabs(normal.x) ) 
            rv.z = 1.f;
        else  
            rv.x = 1.f;
    } 
    else 
    {
        if (Rabs(normal.y )<= Rabs(normal.x) ) 
            rv.y = 1;
        else 
            rv.x = 1;
    }
    return rv;
}

//---------------------------------------------------------------------------------------
BOOL	Portal_Prc::CalculateInitialPortal(Portal& portal, CBspNode* pNode)
{
    Plane&  plane  = pNode->GetPlane();
    V3&		normal = plane._n;
    Box	bb         = _pTree->Root()->_bbox;//pNode->_bbox;				    	// bbox
    
    V3		bbCenter = bb.GetCenter();					// bbox center
    REAL	d2plan   = -plane.DistTo(bbCenter);			// dist to plane
    V3		portOrig = bbCenter + (normal * d2plan);	// plane center id bb center projeccted on plane
    V3		ax       = GetMajorAxes(plane._n);    		// get to see what wax we are using for rotation cp
    ax.norm();
    
    // build a polygon from plane
    V3		bbEx = (bb._max - bbCenter);				// bbox extends
    V3		uDr  = Vcp(ax, normal);						// to corner vectors;
    V3		vDr  = Vcp(uDr, normal);
    
    uDr		*= bbEx.getmax() ;
    vDr		*= bbEx.getmax() ;
    
    REAL vdp = Vdp(normal, ax);
    
    // enlarge the bogus polygon
    if(!IsZero(vdp,.001f))
    {
        uDr *= 8.0;
        vDr *= 8.0;
    }
    
    V3		corners[4];									// the 4 corners
    corners[0] = portOrig + (uDr-vDr);
    corners[1] = portOrig + (uDr+vDr);
    corners[2] = portOrig - (uDr-vDr);					// switch this to be able to rotate it
    corners[3] = portOrig - (uDr+vDr);
    
    // populate the portal structure
    portal._planeIdx = pNode->_planeIdx;
    portal._flags    = PORT_INITAL;
    
    portal._vxes.clear();
    for(int i=0; i < 4; i++)
        portal._vxes << corners[i];
    
    // see if bnew generated portal rely on the same plane it was generated (due)
    // 2 floating point errors
    GCalcNormal(&portal, portal._vxes[0],portal._vxes[1], portal._vxes[2]);
    if(0==portal._vxes.size())
        return FALSE;
    
    // cut initial portal with BSP bounding box
    CutPortalWithNodeBox(portal, _pTree->Root()->_bbox, 0);
    if(portal._vxes.size() == 0)
        return TRUE;
    
    CBspNode* pNodeP = pNode;//->Parent();
    Box lb = pNodeP->_bbox;
    lb.Expand(128);
    CutPortalWithNodeBox(portal, pNodeP->_bbox);
    
    portal._n  = normal;
    portal._c  = plane._c;
    return portal._vxes.size() >= 3;
    
}

//---------------------------------------------------------------------------------------
void	Portal_Prc::ClipWithLeafSides(CLeaf* pLeaf, Portal& portal)
{
    set<int>     sides;
    Portal       ap((const Portal&)portal), bp;
    
    for(int i=pLeaf->_polyIdx; i < pLeaf->_polyIdx + pLeaf->_nPolys; i++)
    {
        Poly& poly = _pTree->_polys[i];
        
        if(sides.find(poly._planeIdx) == sides.end())
        {
            sides.insert(poly._planeIdx);
            portal.Split(poly, ap, bp);
            portal.Clear();
            portal = ap;
        }
    }
}

//---------------------------------------------------------------------------------------
/*
BackLeaf->Portal->FrontLeaf    P->[0][1]
|  - front leaf  - back leaf
Leaf->[portals] these portals has back leaf this 'Leaf'

  Duplication rplaces any portal makeing it 2 way portal
  [Lo]->P->[l1]
  
*/
BOOL	Portal_Prc::DuplicatePortals(vvector<Portal>& portalsI, 
                                     vvector<Portal>& portalsO)
{
    V3 nodeC0;  // portal side leafs bbox centers
    V3 nodeC1;
    
    portalsO.clear();
AG:
    FOREACH(vvector<Portal>, portalsI, pPrt)
    {
        if(pPrt->_vxes.size()<3)
        {
            portalsI.erase(pPrt);
            goto AG;
        }
        ASSERT(pPrt->_sideLIdx.size() == 2);   // indexes in node structure
        
        Portal dupP = (*pPrt);
        dupP.Reverse();
        dupP._flags |= PORT_DUP;
        int temp = dupP._sideLIdx[0];
        dupP._sideLIdx[0] = dupP._sideLIdx[1];
        dupP._sideLIdx[1] = temp;
/*        
        int iLf0 = pPrt->_sideLIdx[0];             // see wich leaf is back and wich is front
        int iLf1 = pPrt->_sideLIdx[1];
        nodeC0   = GetLeaf(iLf0)->_bbox.GetCenter();
        nodeC1   = GetLeaf(iLf1)->_bbox.GetCenter();
        int side1;
        int side0 = pPrt->GetSide(nodeC0);
        side1 = -side0;
        if(0 == side0)                          // it may end up on portal plane
        {                                       // so take the other one
            side1 = pPrt->GetSide(nodeC1);
            //ASSERT(side1 != 0);
            if(0==side1)
            {
                continue;
            }
            side0 = -side1;
        }
        // make [0]P[1]
        if(side0 > 0) // if leaf [0] id on front switch the order
        {
            SWITCHI(dupP._sideLIdx[0] , dupP._sideLIdx[1]);
            SWITCHI(pPrt->_sideLIdx[0] , pPrt->_sideLIdx[1]);
        }
  */      
        //
        // _portIdxes
        //
        pPrt->BackLeaf()->AddPortalIdx(portalsO.size());
        AddPortal(*pPrt);
        
        pPrt->FrontLeaf()->AddPortalIdx(portalsO.size());
        AddPortal(dupP);
    }
    
    return TRUE;
}

//--------------------------------------------------------------------------------
void Portal_Prc::CutPortalWithNodeBox(Portal& portal, Box& box, BOOL reverse)
{
    Plane cutPlanes[6];
    
    if(reverse)
    {
        cutPlanes[0] = Plane(-VY,  box._max);
        cutPlanes[1] = Plane(VY,   box._min);
        cutPlanes[2] = Plane(-VX,  box._max);
        cutPlanes[3] = Plane(VX,   box._min);
        cutPlanes[4] = Plane(-VZ,  box._max);
        cutPlanes[5] = Plane(VZ,   box._min);
        
    }
    else
    {
        cutPlanes[0] = Plane(VY,box._max);
        cutPlanes[1] = Plane(-VY,box._min);
        cutPlanes[2] = Plane(VX,box._max);
        cutPlanes[3] = Plane(-VX,box._min);
        cutPlanes[4] = Plane(VZ,box._max);
        cutPlanes[5] = Plane(-VZ,box._min);
    }
    
    Portal	rp = portal;
    Portal	a(this);
    Portal	b(this);
    
    for(int i=0; i<6 && rp._vxes.size()>2; i++)
    {
        rp.Split(cutPlanes[i],a,b);
        rp = b;
        b.Clear();
        a.Clear();
    }
    portal.Clear();
    portal = rp;
}


