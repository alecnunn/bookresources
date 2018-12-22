#include "stdafx.h"
#include "resource.h"
#include "compiler.h"
#include "Bsp_TechniquesDlg.h"
//---------------------------------------------------------------------------------------
int AllocBitsRoundByLong(int nSetCount)
{
    int rval =   (nSetCount + 7) / 8;    // bytes
    rval = (rval * 3 + 3) & 0xFFFFFFFC;  // round by mod4
    return rval;
}

//---------------------------------------------------------------------------------------
void PVS_Prc::Process(Bsp_Tree& tree, Portal_Prc& portalprc)
{
    TextOut(ST_OUT6, "Calculating PVS. Please Wait...");

    _pBspTree = &tree;
    _pPortPrc = &portalprc;
    Clear();
	_nleafs  = tree._leafs.size();

    dw_deltatime=GetTickCount();
    
    if(portalprc.HasPortals())
    {
        vvector<Portal>& rPortals = portalprc.GetPortals();
        InitalPortalVis(rPortals);
        GaterLeafsVis();
    }
    dw_deltatime = GetTickCount() - dw_deltatime;
    TextOut(ST_OUT3, "PVS Time: %d ms", dw_deltatime);
}

//---------------------------------------------------------------------------------------
BOOL    CompletteOnBack(Plane& pA, Portal& rP)
{
    int count = rP._vxes.size();
    FOREACH(vvector<V3>, rP._vxes, pvx)
    {
        if(pA.GetSide(*pvx) == 1)
            break;
        --count;
    }
    return (0==count);
}

//---------------------------------------------------------------------------------------
BOOL    CompletteOnFront(Plane& pA, Portal& rP)
{
    int count = rP._vxes.size();
    FOREACH(vvector<V3>, rP._vxes, pvx)
    {
        if(pA.GetSide(*pvx) == -1)
            break;
        --count;
    }
    return (0==count);
}


//---------------------------------------------------------------------------------------
BOOL    PVS_Prc::InitalPortalVis(vvector<Portal>& rPortals)
{
    BOOL    bRetVal= TRUE;
    BYTE*   pVis   = new BYTE[rPortals.size()];
    int     nCount = 0;
    int     nTotal = rPortals.size();

    ASSERT(pVis);

    FOREACH(vvector<Portal>, rPortals, portItA)
    {
        Portal&     prtA = *portItA;
        PortalData* pD  = new PortalData(_nleafs);
        _portVs << pD;                                    // this prtA pvs
        ::memset(pVis, 0, rPortals.size());               // reset the array for this prtA        
        

        TextOut(ST_OUT7, "Processing PVS for Portal : %d", nCount);

        FOREACH(vvector<Portal>, rPortals, portItB)
        {
            Portal& prtB = *portItB;
            if(portItB == portItA)
                continue;
            if(prtA._idxThis+1 == prtB._idxThis)   // portal sees iu'ts duplicate
            {
                pVis[prtB._idxThis] = 1;           // portal vis flag 
                continue;
            }

            if(CompletteOnBack(prtA, prtB))    // complette on back we dont see this
                continue;                      // so leave his byte on 0
        
            if(CompletteOnFront(prtB, prtA))   // are we complette on front 
                continue;                           

            pVis[prtB._idxThis] = 1;           // portal vis flag 
        }
        pD->_possibleVisCount = 0;
        PerformPVS(prtA, rPortals, pVis);
        R_PortalFlood(prtA, pVis, prtA._sideLIdx[1]);   // flod in neghbour leaf
        ++nCount;
    }
    delete []pVis;
    return bRetVal;
}

//---------------------------------------------------------------------------------------
void    PVS_Prc::R_PortalFlood(Portal& srcP, BYTE* pVis, int nLeaf)
{
    if(_portVs[srcP._idxThis]->_portArrPvs[nLeaf] == 1) // leaf vis bit
        return;

	_portVs[srcP._idxThis]->_portArrPvs.Set(nLeaf);
    _portVs[srcP._idxThis]->_possibleVisCount++;       // count how many portals this prtA can flood
    CLeaf*          pToLeaf = _pPortPrc->GetLeaf(nLeaf);
    vvector<int>&   iporrts = pToLeaf->GetPortalIdxes();

    FOREACH(vvector<int>, iporrts, pip)   
    {
        if(0 == pVis[*pip])                             // dont flood in this prtA
            continue;

        Portal& prtA = _pPortPrc->GetPortal(*pip);
        R_PortalFlood(srcP, pVis, prtA._sideLIdx[1]);
    }
}

//---------------------------------------------------------------------------------------
void    PVS_Prc::PerformPVS(Portal& portA, vvector<Portal>& rPortals, BYTE* prevPvs)
{
    FOREACH(vvector<Portal>, rPortals, portItB)
    {
        Portal& prtB = *portItB;
        if(prtB == portA)
            continue;

        if(prevPvs[prtB._idxThis] == 0)        
            continue;
        
        if(!PortalSeesPortal(portA,prtB))
            prevPvs[prtB._idxThis]=0;
    }
}


//---------------------------------------------------------------------------------------
void    PVS_Prc::GaterLeafsVis()
{
    int     dummy;
	if(_pvs) delete[] _pvs; _pvs = 0;
    int      nPerLeaf    = AllocBitsRoundByLong(_nleafs);
    _cPvss  = _nleafs * nPerLeaf;                           // all leafs        
	_pvs    = new BYTE[_cPvss];


	::memset(_pvs, 0, _cPvss);                              // zerooo them
    BYTE*    pWalkPvsPtr       = _pvs;                      // head of all bytes
    
    CBitArray pPerLeafPvs(_nleafs)  ; 

	FOREACH(vvector<CLeaf*>, _pBspTree->_leafs, ppLeaf)
	{
		CLeaf*          pLeaf = (*ppLeaf);
        vvector<int>&   pidxs = pLeaf->GetPortalIdxes();

        pPerLeafPvs.Reset();
        pLeaf->_pvsIdx  = pWalkPvsPtr - _pvs;

        pPerLeafPvs.Set((*ppLeaf)->_leafIdx);

        FOREACH(vvector<int>,pidxs,pidx)
        {
            Portal& portal = _pPortPrc->GetPortal(*pidx);
            PortalData* pD = _portVs[portal._idxThis];
            pPerLeafPvs.Merge(pD->_portArrPvs);
        }
		ASSERT(pPerLeafPvs.Size() == nPerLeaf);
        ::memcpy(pWalkPvsPtr, pPerLeafPvs.Buffer(dummy), nPerLeaf);
        pWalkPvsPtr += nPerLeaf;
	}
}


//---------------------------------------------------------------------------------------
BOOL    PVS_Prc::PortalSeesPortal(Portal& a, Portal& b)
{
    V3 dummy;

    V3  paa;
    V3  pab;
    
	FOREACH(vvector<V3>, a._vxes ,pv)
		paa+=*pv;

	_FOREACH(vvector<V3>, b._vxes ,pv)
		pab+=*pv;
	
	paa/=(REAL)a._vxes.size();
	pab/=(REAL)b._vxes.size();

    // test center to center
	V3   col;
    if(!_pBspTree->SegmentIntersect(paa, pab, 0, col))
    {
        return TRUE;
    }
    
    // test close to corner vertex to close to corner vertex
    FOREACH(vvector<V3>, a._vxes, pva)
    {
        FOREACH(vvector<V3>, b._vxes, pvb)
        {
            V3 c2c1; 
            V3 c2c2;

            c2c1.interpolate(*pva, paa, .3);
            c2c2.interpolate(*pvb, pab, .3);

            if(!_pBspTree->SegmentIntersect(c2c1, c2c2, 0, col))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

