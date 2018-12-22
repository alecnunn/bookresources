/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "baseos.h"
#include "csg_bsp.h"
#include "Poly.h"
#include "collections.h"
#include "MiniBsp.h"
#include "Mainfrm.h"

//------------------------------------------------------------------------------------------
static  int			Gdepth;             // Keep track of current tree depth
static  int			GMaxDepth;          // Stores maximum deph reached in bsp compilation

//------------------------------------------------------------------------------------------
// Returns a pointer to back node of current node 
CMiniNode* CMiniNode::Back()
{
    if(_nodeidx[0]>=0)
        return _pBsp->_nodesPtr[_nodeidx[0]];
    return 0;
}

//------------------------------------------------------------------------------------------
// Returns a pointer to front node of current node or 0
CMiniNode* CMiniNode::Front()
{
    if(_nodeidx[1]>=0)
        return _pBsp->_nodesPtr[_nodeidx[1]];
    return 0;
}

//------------------------------------------------------------------------------------------
// Returns a pointer to the parent node of current node or 0 if current node is the root node
CMiniNode* CMiniNode::Parent()
{
    if(_idxParent>=0)
        return _pBsp->_nodesPtr[_idxParent];
    return 0;
}

//------------------------------------------------------------------------------------------
// Compiles the BSP
BOOL MiniBsp::Compile(vvector<Poly>& polys, BOOL bremES)
{
    static    vvector<Poly>  loco;           // make it static to be heap friendly
    Gdepth            = 0;
    GMaxDepth         = polys.size()*2;      // Limith a deph for the time being
    loco.clear();                            // Clear it from previous call   
    loco              = polys;               // Copy all polygons in the local array
    _bComplErr        = FALSE;
    Clear();
    MakeRoot();                              // Make default root node
    BuildPlaneArray(loco);                   // Collect planes in one array
    try{
        R_Compile(0, loco);
    }
    catch(int i){
        i;
        return FALSE;
    }
    // Uncomment the lines to trace the trees.
    /**
    TRACE("*********************************\r\n");
    Trace(0);
    */
    return TRUE;
}

//------------------------------------------------------------------------------------------
// Recusrsive call. Compiles the BSP tree.
void MiniBsp::R_Compile(int nodeIdx, vvector<Poly>& polys)
{
    if(++Gdepth > GMaxDepth)        throw(1);     // break if BSP canot be solved 
                                                  // (tiny polys almost layin on the same plane) 
    vvector<Poly>	    frontPolys(polys.size()); // reserve space for front polygons
    vvector<Poly>	    backPolys(polys.size());  // and back polygons  
    CMiniNode*	pNode = _nodesPtr[nodeIdx];       
    pNode->_planeIdx  = GetBestSplitter(polys);   // choose the splitter plane

    // Here is the standard BSP partitioning
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
            if(ISZERO(VDp(curPoly._normal,  pNode->GetPlane()._normal)-1.f))
                frontPolys.push_back(curPoly);
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
                Poly	frontPoly, backPoly;

                curPoly.Split(pNode->GetPlane(), frontPoly, backPoly);
                backPolys.push_back(backPoly);
                frontPolys.push_back(frontPoly);
            }
            break;
        default:
            break;
        }
        polys.pop_back();
    }

    UpdateNodeBB(pNode, frontPolys);
    UpdateNodeBB(pNode, backPolys);
    
    if(frontPolys.size()){
        if(splitters ==  0 || frontPolys.size() == 1/*allow convex*/){
            CMiniNode* fn			 = CreateNode(NODE_LEAF);
            pNode->_nodeidx[N_FRONT] = _nodesPtr.size();
            fn->_polyIdx			 = _polys.size();
            fn->_nPolys				 =  frontPolys.size();
            fn->_idxParent           =  pNode->_idxNodeThis;
            AddNode(fn);
            colex::append(_polys, frontPolys);
            UpdateNodeBB(fn, frontPolys);
            frontPolys.clear();
            if(_bComplErr){
                _bComplErr=FALSE;
            }
        }
        else{
            CMiniNode*  fn = CreateNode(0);
            pNode->_nodeidx[N_FRONT]=_nodesPtr.size();
            fn->_idxParent = pNode->_idxNodeThis;
            AddNode(fn);
            R_Compile(pNode->_nodeidx[N_FRONT], frontPolys);
        }
    }
    if(backPolys.size()){
        CMiniNode* bn = CreateNode(0);
        pNode->_nodeidx[N_BACK] = _nodesPtr.size();
        bn->_idxParent = pNode->_idxNodeThis;
        AddNode(bn);
        R_Compile(pNode->_nodeidx[N_BACK], backPolys);
    }
    else{
        CMiniNode* bn = CreateNode(NODE_LEAF|NODE_SOLID);
        pNode->_nodeidx[N_BACK]=_nodesPtr.size();
        bn->_idxParent = pNode->_idxNodeThis;
        AddNode(bn);
    }
    Gdepth--;
}

//------------------------------------------------------------------------------------------
// Select the best splitter index in plSpliter list. 
// Walks in polys and pick most suitable splitter which is in the score range
// score is a factor to choose the splitter (BSP balance versus splits)
int MiniBsp::GetBestSplitter( vvector<Poly>& polys)
{
    int                     splitCnt    = 0;
    int		                retval		= -1;
    int		                bestscore	= INFINIT;
    vvector<Poly>::iterator  bpIt       = polys.end();

    FOREACH(vvector<Poly>, polys, pPoly1){
        DWORD	score		= 0;
        DWORD	boths		= 0;
        DWORD	backs		= 0;
        DWORD	fronts		= 0;
        Poly&	polySpliter = *pPoly1;

        if(!polySpliter.IsSplitter()){
            FOREACH(vvector<Poly>, polys, pPoly2)
            {
                REL_POS   whereIs;
                Poly& poly = *pPoly2;

                if(pPoly2 == pPoly1)
                    continue;
                whereIs = poly.Classify(polySpliter);
                switch(whereIs){
                case ON_PLANE:
                    break;
                case ON_FRONT:
                    fronts++;
                    break;
                case ON_BACK:
                    backs++;
                    break;
                case ON_SPLIT:
                    boths++;
                    break;
                }
            }
            score = abs(fronts-backs)+(boths*4/*8*/);
            if(score < bestscore){
                bestscore = score;
                bpIt  = pPoly1;
            }
            splitCnt++;
        }//if
    }
    if(bpIt != polys.end()){
        Poly& rpoly = (*bpIt);
        Poly* poly = &rpoly;
        poly->SetSplitter();		// mark it as a splitter poly
        return poly->_planeIdx;
    }
    _bComplErr        = TRUE;
    return polys[0]._planeIdx;      // allow convex; (more appropriate is to build a virtual
                                    // plane whos is laying behind on all remainig polygons)
}

//------------------------------------------------------------------------------------------
// Walks recursively and prints out nodes and leafs
void MiniBsp::Trace(int idx)
{
    CMiniNode* pNode = _nodesPtr[idx];
    if(pNode->_flags & NODE_LEAF){
        if(pNode->_flags & NODE_SOLID){
            TRACE("X %");
        }
        else{
            TRACE("[%d]",pNode->_nPolys);
        }
        TRACE("\r\n");
    }
    if(pNode->_nodeidx[1]>=0){
        TRACE(">");
        Trace(pNode->_nodeidx[1]);
    }
    if(pNode->_nodeidx[0]>=0){
        TRACE("<");
        Trace(pNode->_nodeidx[0]);
    }
    TRACE("-");
}


//------------------------------------------------------------------------------------------
// Collects all planes in one single array. 
// BSP nodes will hold the plane index in this array
void	MiniBsp::BuildPlaneArray(vvector<Poly>& polys)
{
    ASSERT(_planes.size()==0);

    FOREACH(vvector<Poly>, polys, pPoly){
        Plane   plane(pPoly->_normal, pPoly->_const);
        pPoly->_planeIdx = -1;
        vvector<Plane>::iterator fp = _planes.findelement(plane);
        if(fp == _planes.end()){
            pPoly->_planeIdx = _planes.size();
            _planes << plane;
        }else{
            pPoly->_planeIdx = fp - _planes.begin();
        }
        ASSERT(pPoly->_planeIdx != -1);
    }
}

//------------------------------------------------------------------------------------------
// This is the main CSG clipping function. It receives a soup of polygons and
// 3 flags. The flags are settings how clipping should go.
// Split repair goes like this:
/*
    Initially each polygon references himself as its parent.
    Each time is being split the '_fragments' member get's incremented
    and resulted fragments will point to the original parent.
    Finally the parent has the the counter '_fragments' equal with the
    number of its fragmens. If the number of fragments having the same 
    parent is equal with the parent '_fragments' counter then all fragments 
    are being replaced with the original polygon.
*/

void    MiniBsp::ClipBrPolys(vvector<Poly>& polySoup,
                             vvector<Poly>&    outPlys,
                             BOOL bRemCoplan,  // remove any coplanars poly
                             BOOL bRemSolid,   // removes the polygons ending up in a solid space
                             BOOL bremoveUs)   // removes any unnecesarely splits after operation
{
    int idx = 0;
    FOREACH(vvector<Poly>, polySoup, pp)    // prepare for unnecesarely split repair anyway
    {
        pp->_flags          &=  ~POLY_DELETED;
        pp->_idxSplitter    =  -1;          // node idx who split this polygon
        pp->_idxParent      =  idx++;       // hold in each polygon it's own index. This is the parent index
        pp->_fragments		= 0;            // the polyogn initially has no fragments
    }

    _pInPolys   = &polySoup;                // hold a pointer to input data set
    _bRemUS		= bremoveUs;                
    _bRemCoplan = bRemCoplan;
    _bClipSolid = bRemSolid;

    R_ClipPolyList(0, polySoup, outPlys);

    //
    // Repair the slipts as described in the function header
    //
    if(bremoveUs)
    {
        RepairSplits(polySoup, outPlys);
    }
}

//------------------------------------------------------------------------------------------
// Recurvsive function . Clips the soup of polygons with this BSP
void MiniBsp::R_ClipPolyList(int idxNode,
                             vvector<Poly>& polySoup,
                             vvector<Poly>& outLst)
{
    REAL        fTemp;
    CMiniNode*	pNode = _nodesPtr[idxNode];

    if(pNode->IsLeaf()) {                           // Polygons have reached a leaf level                        
        if(pNode->IsSolid()){
            if(_bClipSolid)                         // Take apropriate operation 
                return;
            colex::append(outLst, polySoup);
        }
        else{
            if(!_bClipSolid)                        // Take apropriate operation 
                return;
            colex::append(outLst, polySoup);
        }
        return;
    }
    // Reserve  heap space for front and out polygons with minim memory fragmentation.
    vvector<Poly>    backPolys(polySoup.size());    
    vvector<Poly>    frontPolys(polySoup.size());

    FOREACH(vvector<Poly>, polySoup, ppSpPoly)       // For each poly in soup   
    {
        Poly& pSpPoly = *ppSpPoly;
        REL_POS rp1   = pSpPoly.Classify(pNode->GetPlane());

        switch (rp1){
        case ON_PLANE:
            fTemp = VDp(pSpPoly._normal, pNode->GetPlane()._normal)-1.f;
            if(ISZERO(fTemp)){      // Poly and splitter are on same plane
                if(_bRemCoplan) {   // Removing coplanar pass in back list
                    backPolys.push_back(pSpPoly);
                }
                else{               // Keeping coplanar, pass in front list
                    frontPolys.push_back(pSpPoly);
                }
            }
            else{                   // On same plane but facing oppozite, pass back 
                backPolys.push_back(pSpPoly);
            }
            break;
        case ON_FRONT:
            frontPolys.push_back(pSpPoly);
            break;
        case ON_BACK:
            backPolys.push_back(pSpPoly);
            break;
        case ON_SPLIT:
            {
                Poly fp;
                Poly bp;

                // The initial parent polygon keep track of how many times was it split 
                // Notice the first split increment is 2 and then only 1 
                if(_pInPolys->at(pSpPoly._idxParent)._fragments == 0)
                    _pInPolys->at(pSpPoly._idxParent)._fragments+=2;
                else
                    _pInPolys->at(pSpPoly._idxParent)._fragments++;

                pSpPoly.Split(pNode->GetPlane(),fp,bp);

                // propagate the fragments parent index back to original parent
                fp._idxParent = pSpPoly._idxParent;
                bp._idxParent = pSpPoly._idxParent;
                // And the node who cut them (for future use)
                fp._idxSplitter = idxNode;
                bp._idxSplitter = idxNode;

                // Mark parent polygon as deleted as long we have the fragments.
                // If all fragments of original polygon survived we undelete the polygon
                // and place it back in the out polygon list.
                pSpPoly._flags |= POLY_DELETED;
                frontPolys.push_back(fp);
                backPolys.push_back(bp);
            }
            break;
        }
    }

    if(backPolys.size()){   //  send back fragments on back
        R_ClipPolyList(pNode->_nodeidx[N_BACK], backPolys, outLst);
    }
    if(frontPolys.size()){  //  send front fragments on front
        R_ClipPolyList(pNode->_nodeidx[N_FRONT], frontPolys, outLst);
    }
    return;
}

//------------------------------------------------------------------------------------------
// Helping foo: Collects all fragments having the same parent
void collect(int index, vvector<Poly*>& fragments, vvector<Poly>& outs)
{
    FOREACH(vvector<Poly>, outs,ppop){
        Poly& rp = *ppop;
        if(ppop->_idxParent==-1)
            continue;
        if(ppop->_idxParent!=index)
            continue;
        fragments.push_back(&rp);
    }
}

//------------------------------------------------------------------------------------------
// Walks in fragments and check if all of them have survived. 
// if the case replace them with the original polygon. 
void	MiniBsp::RepairSplits(vvector<Poly>& ins , vvector<Poly>& outs)
{
    FOREACH(vvector<Poly>, ins ,pip){
        if(pip->_fragments){
            vvector<Poly*>	fragments;
            collect(pip->_idxParent, fragments, outs);  // get fragments for pip
            if(fragments.size() == pip->_fragments){    // see if all survived
                FOREACH(vvector<Poly*>,fragments,ppp){
                    (*ppp)->_flags|=POLY_DELNAILED;    // mark them deleted
                }
                outs.push_back(*pip);                  
            }
        }
    }
    // delete the marked ones
AGAIN:
    FOREACH(vvector<Poly>, outs, ppo){
        if(ppo->_flags & POLY_DELNAILED){
            outs.erase(ppo);
            goto AGAIN;
        }
    }
}

