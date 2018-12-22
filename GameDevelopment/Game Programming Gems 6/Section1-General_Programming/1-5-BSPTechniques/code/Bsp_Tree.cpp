#include "stdafx.h"
#include "resource.h"
#include "geometry.h"
#include "compiler.h"
#include "Bsp_TechniquesDlg.h"

#ifndef _THREAD_WAIT_BRANCH_
#pragma message("Complation: Thread Waiting for Branch")
#else
#pragma message("Complation: Thread Not Waitong for Branch")
#endif //

//------------------------------------------------------------------------------------------
CBspNode* CBspNode::Back()
{
    if(_nodeidx[0]>=0)
        return _pBsp->_nodes[_nodeidx[0]];
    return 0;
}

//------------------------------------------------------------------------------------------
CBspNode* CBspNode::Front()
{
    if(_nodeidx[1]>=0)
        return _pBsp->_nodes[_nodeidx[1]];
    return 0;
}

//------------------------------------------------------------------------------------------
CBspNode* CBspNode::Parent()
{
	if(_idxParent>=0)
        return _pBsp->_nodes[_idxParent];
    return 0;
}

//------------------------------------------------------------------------------------------
void Bsp_Tree::Process(vvector<Poly>& polys, BOOL busethreads)
{

    Clear(); 
    list<Poly>		loco;

    b_usethreads = busethreads;

    TextOut(ST_OUT6, "Building BSP Tree. Please Wait...");

    FOREACH(vvector<Poly>, polys, pp)
	{
		pp->_flags  &= ~SPLITTER_POLY;
		loco.push_back(*pp);
	}
	_pInPolys  = &polys;				   // hold a pointer to input data set	
    

    MakeRoot();                         // sdd default root node                    

    TextOut(ST_OUT6, "Preparing BSP. Please Wait...");

    UpdateNodeBB(Root(), polys);
    V3 center = Root()->_bbox.GetCenter();
    _balance = 6;

    dw_deltatime = GetTickCount();
    BuildPlaneArray(loco);
    CBspNode*  pNode = _nodes[0];
	pNode->_planeIdx = 0;
    n_thrcount = 0;
    n_threadsup  = 0;
    if(BSP_TERRAIN == _bsptype)
	{
		R_TerrainCompile(0, loco);
	}
	else
    {
		R_Compile(0, loco);

#ifndef _THREAD_WAIT_BRANCH_
        while(n_threadsup)
        {
            Sleep(1);
        }
        ::WaitForMultipleObjects(n_thrcount, h_handles,1,INFINITE);
        while(--n_thrcount>=0)
            ::CloseHandle(h_handles[n_thrcount]);
#endif //
    }
    dw_deltatime = GetTickCount() - dw_deltatime;

    TextOut(ST_OUT, "BSP Time: %d ms", dw_deltatime );
}

//------------------------------------------------------------------------------------------
void Bsp_Tree::R_TerrainCompile(int nodeIdx, list<Poly>& polys)
{
	static          Deepth = 0;
	list<Poly>	    frontPolys; 
	list<Poly>	    backPolys;
	CBspNode*		pNode = _nodes[nodeIdx];
	Deepth++;

    FindSplitterPlane(polys, pNode);
	PartitionTerrain(pNode, polys, frontPolys, backPolys);
	if(frontPolys.size())
	{
		if(0==backPolys.size())
		{
			CBspNode* fn			 = CreateNode(NODE_LEAF);

            if(Lock()) 
            {
			    pNode->_nodeidx[N_FRONT] = _nodes.size();
			    fn->_polyIdx			 = _polys.size();
			    fn->_nPolys				 =  frontPolys.size() + backPolys.size();
                fn->_idxParent           =  pNode->_idxNodeThis;
			    AddNode(fn);


                FOREACH(list<Poly>, frontPolys, pp)
				    _polys.push_back(*pp);

			    if(backPolys.size())		
			    {
				    FOREACH(list<Poly>, backPolys, pp)
					    _polys.push_back(*pp);
			    }
                UnLock();
            }


			UpdateNodeBB(fn, frontPolys);
			frontPolys.clear();
		}
        else                                            // continue
		{
			CBspNode*  fn = CreateNode(0);
            if(Lock()) 
            {
			    pNode->_nodeidx[N_FRONT]=_nodes.size();
                fn->_idxParent = pNode->_idxNodeThis;
			    AddNode(fn);
                UnLock();
            }
			R_TerrainCompile(pNode->_nodeidx[N_FRONT], frontPolys);
		}
	}
	if(backPolys.size())	                            // make a solid leaf
	{
		CBspNode* bn = CreateNode(0);
        if(Lock()) 
        {
		    pNode->_nodeidx[N_BACK] = _nodes.size();
		    bn->_idxParent = pNode->_idxNodeThis;
		    AddNode(bn);
            UnLock();
        }
		R_TerrainCompile(pNode->_nodeidx[N_BACK], backPolys);
	}
	else                                                // continue
	{
		CBspNode* bn = CreateNode(NODE_LEAF|NODE_SOLID);
        if(Lock()) 
        {
		    pNode->_nodeidx[N_BACK]=_nodes.size();
            bn->_idxParent = pNode->_idxNodeThis;
		    AddNode(bn);
            UnLock();
        }
	}
}

//------------------------------------------------------------------------------------------
BOOL Bsp_Tree::FormConvex(list<Poly>& frontPolys)
{
	Poly&		fp = frontPolys.front();
	int			j  = 0;

	FOREACH(list<Poly>, frontPolys, ppoly)
	{
		if(j!=0)
		{
			if(ON_BACK == fp.Classify(static_cast<Plane>(*ppoly)))
				return FALSE;
		}
		++j;
	}
	return TRUE;
}

//------------------------------------------------------------------------------------------
void Bsp_Tree::Recurs_Or_Thread(int nodeIdx, list<Poly>& polys)
{
    if(b_usethreads && polys.size() > 256) //1 thread per 256 polygons
    {
        long cv = InterlockedIncrement(&n_threadsup);
        TextOut(ST_OUT7, "Running Threads: %d", cv); 

        DWORD   uid;

#ifdef _THREAD_WAIT_BRANCH_
        ThrData td = {nodeIdx, &polys, this};
        HANDLE h        = ::CreateThread(0, 0, 
                                         reinterpret_cast<LPTHREAD_START_ROUTINE>(Bsp_Tree::T_Compile), 
                                         (void*)&td, 0, &uid);
        ::WaitForSingleObject(h, INFINITE);
        ::CloseHandle(h);
#else
        ThrData* td = new ThrData();
        td->nodeIdx = nodeIdx;
        td->polys   = polys;
        td->ptree   = this;
        polys.clear();
        h_handles[n_thrcount++]= ::CreateThread(0, 0, 
                                                reinterpret_cast<LPTHREAD_START_ROUTINE>(Bsp_Tree::T_Compile), 
                                                (void*)td, 0, &uid);
#endif //

    }
    else
    {
        R_Compile(nodeIdx, polys);
    }
}

//------------------------------------------------------------------------------------------
int      Bsp_Tree::T_Compile(void* pvoid)
{
    ThrData* pThrData = reinterpret_cast<ThrData*>(pvoid);
#ifdef _THREAD_WAIT_BRANCH_
    pThrData->ptree->R_Compile(pThrData->nodeIdx, *pThrData->polys);
#else
    pThrData->ptree->R_Compile(pThrData->nodeIdx, pThrData->polys);
#endif //
    long dv = InterlockedDecrement(&pThrData->ptree->n_threadsup);
    TextOut(ST_OUT7, "Running Threads: %d", dv);

#ifndef _THREAD_WAIT_BRANCH_
    delete pThrData;
#endif //
    return 0;
}

//------------------------------------------------------------------------------------------
void Bsp_Tree::R_Compile(int nodeIdx, list<Poly>& polys)
{
	list<Poly>	    frontPolys;         
	list<Poly>	    backPolys;
	CBspNode*		pNode     = _nodes[nodeIdx];
	int             splitters = Partition(pNode, polys, frontPolys, backPolys);

    if(0 == splitters && backPolys.size())                      // not good
    {
        append(frontPolys, backPolys);                          // fake leaf
        backPolys.clear();
    }
    
    if(frontPolys.size())
	{
		if(splitters ==  0 )           
		{
			CBspNode* fn			 = CreateNode(NODE_LEAF);
            if(Lock()) 
            {
			    pNode->_nodeidx[N_FRONT] = _nodes.size();
			    fn->_polyIdx			 = _polys.size();
			    fn->_nPolys				 =  frontPolys.size();
                fn->_idxParent           =  pNode->_idxNodeThis;
			    AddNode(fn);
			    FOREACH(list<Poly>, frontPolys, pp)
				    _polys.push_back(*pp);
                UnLock();
            }

            UpdateNodeBB(fn, frontPolys);
			frontPolys.clear();
		}
        else                                                    //  continue
		{
			CBspNode*  fn = CreateNode(0);
            if(Lock()) 
            {
    			pNode->_nodeidx[N_FRONT]=_nodes.size();
                fn->_idxParent = pNode->_idxNodeThis;
                AddNode(fn);
                UnLock();
            }
            UpdateNodeBB(fn, frontPolys);
            Recurs_Or_Thread(pNode->_nodeidx[N_FRONT], frontPolys);
		}
	}

    if(backPolys.size())	                        // make a solid leaf
	{
		CBspNode* bn = CreateNode(0);
        if(Lock()) 
        {
            pNode->_nodeidx[N_BACK] = _nodes.size();
            bn->_idxParent = pNode->_idxNodeThis;
		    AddNode(bn);
            UnLock();
        }
        UpdateNodeBB(bn, backPolys);
        Recurs_Or_Thread(pNode->_nodeidx[N_BACK], backPolys);
	}
	else
	{
		CBspNode* bn = CreateNode(NODE_LEAF|NODE_SOLID);
        if(Lock()) 
        {
    		pNode->_nodeidx[N_BACK]=_nodes.size();
            bn->_idxParent = pNode->_idxNodeThis;
            bn->_bbox._max.setval(0.0); bn->_bbox._min.setval(0.0);
		    AddNode(bn);
            UnLock();
        }
	}
}

//------------------------------------------------------------------------------------------
// Select the best splitter 
int Bsp_Tree::GetBestSplitter(list<Poly>& polys, Poly* pWantPoly)
{
    int                     splitCnt    = 0;
	int		                retval		= -1;
	int		                bestscore	= INFINIT;
    list<Poly>::iterator    bpIt        = polys.end();
    int                     maxbacks    = 0;
    int                     maxfronts   = 0;

	FOREACH(list<Poly>, polys, pPoly1)
	{
		DWORD	score		= 0; 
		DWORD	boths		= 0;
        DWORD   planars     = 0;
		DWORD	backs		= 0;
		DWORD	fronts		= 0;
		Poly&	polySpliter = *pPoly1;

		if(!polySpliter.IsSplitter())
        {
		    FOREACH(list<Poly>, polys, pPoly2)
		    {
			    REL_POS     whereIs;
			    Poly&       poly = *pPoly2;

                if(pPoly2 == pPoly1)
                    continue;
			    whereIs = poly.Classify(polySpliter);
			    switch(whereIs)
			    {
				    case ON_PLANE:
                        ++planars;
					    break;
				    case ON_FRONT:
					    ++fronts;
					    break;
				    case ON_BACK:
					    ++backs;
					    break;
				    case ON_SPLIT:
					    ++boths;
					    break;
			    }
		    }
            

		    score = abs(fronts-backs)+(boths*_balance);
		    if(score < bestscore)
		    {
		        bestscore = score;
                bpIt  = pPoly1;
		    }
            splitCnt++;
            maxfronts = tmax((ULONG)maxfronts, (ULONG)fronts);
            maxbacks =  tmax((ULONG)maxbacks,  (ULONG)backs);

        }//if
	}
	if(bpIt != polys.end())
	{
        Poly& rpoly = (*bpIt);
		Poly* poly = &rpoly;
		poly->SetSplitter();		        // mark it as a splitter poly
		return poly->_planeIdx;
	}
   	_FOREACH(list<Poly>, polys, pPoly1)
	{
		if(!pPoly1->IsSplitter())
        {
            Poly& rpoly = (*pPoly1);
		    Poly* poly = &rpoly;
		    poly->SetSplitter();		    // mark it as a splitter poly
		    return poly->_planeIdx;
        }
    }
	return polys.begin()->_planeIdx;        // all are on front so dosen't matter
}

//------------------------------------------------------------------------------------------
void	Bsp_Tree::BuildPlaneArray(list<Poly>& polys)
{
    ASSERT(_planes.size()==0);
    
    // init the splitting planes
AGAIN:
    FOREACH(list<Poly>, polys, pPoly)
    {
        Poly& poly = (Poly&)(*pPoly);
        poly.Recalc();
        if(poly._n.isnot0() )
        {
            Plane   plane(poly._n, poly._c);
            poly._planeIdx = -1;
            vvector<Plane>::iterator fp = _planes.findelement(plane);
            if(fp == _planes.end())
            {
                pPoly->_planeIdx = _planes.size();
                _planes << plane;
            }
            else
            {
                pPoly->_planeIdx = fp - _planes.begin();
            }
            ASSERT(pPoly->_planeIdx != -1);
        }
        else
        {
            polys.erase(pPoly);
            goto AGAIN;
        }
    }
}

//------------------------------------------------------------------------------------------
void Bsp_Tree::PartitionTerrain(CBspNode*  pNode, 
							   list<Poly>& polys, 
							   list<Poly>& frontPolys, 
							   list<Poly>& backPolys)
{
	while(polys.size())
	{
		Poly& curPoly = polys.back();
	    REL_POS   whereIs = curPoly.Classify(pNode->GetPlane());
		switch(whereIs)
		{
			case ON_PLANE:
				if(IsZero(Vdp(curPoly._n,  pNode->GetPlane()._n)-1.f))
					frontPolys.push_back(curPoly);
				else
					backPolys.push_back(curPoly);
				break;
            case ON_SPLIT:
                {
				    Poly	frontPoly;
				    Poly	backPoly;
                    curPoly.Split(pNode->GetPlane(), frontPoly, backPoly);
                    backPolys.push_back(backPoly);
				    frontPolys.push_back(frontPoly);
                }
                break;
			case ON_FRONT:
				frontPolys.push_back(curPoly);
				break;
			case ON_BACK:
				backPolys.push_back(curPoly);
				break;
            default:
                break;
		}
        polys.pop_back();
	}
    UpdateNodeBB(pNode, frontPolys);
    UpdateNodeBB(pNode, backPolys);
}

//------------------------------------------------------------------------------------------
int	 Bsp_Tree::Partition(CBspNode* pNode, 
                         list<Poly>& polys, 
                         list<Poly>&  frontPolys, 
                         list<Poly>& backPolys)
{
	pNode->_planeIdx   = GetBestSplitter(polys);

	int	 splitters    = polys.size();	      
	while(polys.size())
	{
		Poly& curPoly = polys.back();
		if(curPoly.IsSplitter())
			splitters--;

        REL_POS   whereIs = curPoly.Classify(pNode->GetPlane());
		switch(whereIs)
		{
			case ON_PLANE:
				if(IsZero(Vdp(curPoly._n,  pNode->GetPlane()._n)-1.f))
				{
					curPoly.SetSplitter();
					frontPolys.push_back(curPoly);
				}
				else
					backPolys.push_back(curPoly);
				break;
			case ON_FRONT:
				frontPolys.push_back(curPoly);
				break;
			case ON_BACK:
				backPolys.push_back(curPoly);
				break;
			case ON_SPLIT:
				{
                    Poly	frontPoly;
                    Poly	backPoly;

                    curPoly.Split(pNode->GetPlane(), frontPoly, backPoly);
					if(curPoly.IsSplitter())
					{
						frontPoly.SetSplitter();
						backPoly.SetSplitter();
					}
                    backPolys.push_back(backPoly);
                    frontPolys.push_back(frontPoly);
				}
				break;
            default:
                break;
		}
        polys.pop_back();
	}
	return splitters;

}

//------------------------------------------------------------------------------------------
Plane   Bsp_Tree::GetOptimSpliterPlane(list<Poly>& polys, int moe)
{
    Box allBox;
	Box oneBox(V0,V3(100, 100, 100));

    FOREACH(list<Poly>, polys, pp){
        pp->Recalc();
        allBox.Union(pp->_box);
    }

	V3 allEx  = allBox.GetExtends();
	V3 oneEx  = oneBox.GetExtends();
    V3 ex     = allBox.GetExtends();
    V3 c      = allBox.GetCenter();

    REAL    mx = tmax(ex.x , ex.y);
    mx         = tmax(mx,    ex.z);

    
    if(ex.x  == mx)
    {
        ex.x = 1;
        ex.y = 0;
        ex.z = 0;
    }
    else if(ex.y == mx)
    {
        ex.x = 0;
        ex.y = 1;
        ex.z = 0;
    }else
    {
        ex.x = 0;
        ex.y = 0;
        ex.z = 1;
    }

    // make all of them on front to nail down the tree
    if(moe)
    {
	    oneEx     *= 8;
	    if((allEx.x <=  oneEx.x &&  
           allEx.y <= oneEx.y   && 
           allEx.z <=  oneEx.z) || polys.size()<=moe)
        {
		    c = allBox._min; // one unit down
            c.x -= 1;
            c.y -= 1;
            c.z -= 1;
	    }
    }
    return Plane(ex,c);
}

//------------------------------------------------------------------------------------------
void Bsp_Tree::FindSplitterPlane(list<Poly>& polys, CBspNode* pNode)
{
    Plane& plane = GetOptimSpliterPlane(polys);
    pNode->_planeIdx = _planes.size();
	_planes << plane;

}

//------------------------------------------------------------------------------------------
void Bsp_Tree::UpdateNodeBB(CBspNode* pNode, list<Poly>& polys)
{
    FOREACH(list<Poly>,  polys, poly)
    {
        poly->Recalc();
        pNode->_bbox.Union(poly->_box);
        Root()->_bbox.Union(poly->_box);
    }
}

//------------------------------------------------------------------------------------------
void Bsp_Tree::UpdateNodeBB(CBspNode* pNode, vvector<Poly>& polys)
{
    FOREACH(vvector<Poly>,  polys, poly)
    {
        poly->Recalc();
        pNode->_bbox.Union(poly->_box);
        Root()->_bbox.Union(poly->_box);
    }
}

//------------------------------------------------------------------------------------------
void	Bsp_Tree::Clear()
{
	_nodes.deleteelements();
	_planes.clear();
	_polys.clear();
	_nodes.clear();
    _leafs.clear();
    dw_deltatime = GetTickCount();
}

//------------------------------------------------------------------------------------------
BOOL Bsp_Tree::R_SegmentIntersect(int nodeIdx, V3& a, V3& b, V3& col)
{
	CBspNode* pNode = _nodes[nodeIdx];
    if (pNode->IsLeaf())
	{
        if(pNode->IsSolid())
        {
            return TRUE;
        }
		return FALSE;
	}
	    
    Plane& plane = _planes[pNode->_planeIdx]; 
    REAL da = plane.DistTo(a);
    REAL db = plane.DistTo(b);

    if(da > -EPSILON  && db > - EPSILON )
        return R_SegmentIntersect(pNode->_nodeidx[N_FRONT], a, b, col);
    else if(da < EPSILON && db < EPSILON )
        return R_SegmentIntersect(pNode->_nodeidx[N_BACK], a, b, col);

	int  side = da >= 0;
	REAL dist = da / (da - db);
	V3 midPct = a + ((b-a)*dist);

	if(R_SegmentIntersect(pNode->_nodeidx[side], a, midPct,  col))
	{
		return TRUE;
	}
	else if (R_SegmentIntersect(pNode->_nodeidx[!side], midPct, b,  col))
	{
		if(!_hitTest)
		{
			_hitTest		 = TRUE;
			col	             = midPct;
		}
		return TRUE;
	}
    return FALSE;
}

//------------------------------------------------------------------------------------------
BOOL   Bsp_Tree::SegmentIntersect(V3& paa, V3& pab, int node , V3& col)
{
    _hitTest = FALSE;
    R_SegmentIntersect(0, paa, pab, col);
    return _hitTest;
}

//------------------------------------------------------------------------------------------
int	Bsp_Tree::GetCurrentLeaf(V3& pov, int nodeIdx)
{
    REAL    rdist;
    CBspNode* pNode = _nodes[nodeIdx];
    while(!pNode->IsLeaf())
    {
        Plane& plane = _planes[pNode->_planeIdx];
        rdist = plane.DistTo(pov);
        if (rdist <0 ) 
            nodeIdx = pNode->_nodeidx[0];
		else
            nodeIdx = pNode->_nodeidx[1];
        pNode = _nodes[nodeIdx];
    }
    return _nodes[nodeIdx]->_leafIdx;
}

