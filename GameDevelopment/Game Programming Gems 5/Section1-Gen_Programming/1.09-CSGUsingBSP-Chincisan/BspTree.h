/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __BSPTREE_H__
#define __BSPTREE_H__

#include "baseos.h"
#include "baseutils.h"
#include "poly.h"
#include "collections.h"
#include "MiniBsp.h"

//---------------------------------------------------------------------------------------
// Leaf class
class CLeaf : public CMiniNode
{
public:
    CLeaf(MiniBsp* pBsp, DWORD dw=0):CMiniNode(pBsp, dw)
    {
    }
    // add your leaf specific members
};

//---------------------------------------------------------------------------------------
// Mini BSP tree class.
class z_ed3View;
class CBspTree  : public MiniBsp
{
public:
    BOOL BuildPortals();
    CBspTree();
    virtual ~CBspTree();
    virtual void Clear();                       
    virtual BOOL Compile(vvector<Poly>&, BOOL bremES=FALSE); 
    CLeaf*  GetLeaf(int idx){
        ASSERT(idx < _leafs.size()); return _leafs[idx];
    }

protected:
    virtual void UpdateNodeBB(CMiniNode*, vvector<Poly>&);
    CMiniNode*  CreateNode(DWORD dw){
        if(dw & NODE_SOLID)
            return MiniBsp::CreateNode(dw);
        return new CLeaf(this, dw);
    }
    void   AddNode(CMiniNode* pNn){
        MiniBsp::AddNode(pNn);
        if(pNn->IsEmptyLeaf()){
            pNn->_leafIdx = _leafs.size();
            _leafs << (CLeaf*)pNn;
        }
    }
public:
    int             _thisIdx;
    vvector<CLeaf*> _leafs;
    DWORD           _props;
private:
    BOOL            _mainBsp;
};

#endif // !__BSPTREE_H__

