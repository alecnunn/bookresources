/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __MINIBSP_H__
#define	__MINIBSP_H__


#include "baseos.h"
#include "basemath.h"
#include "poly.h"
#include "brush.h"


//---------------------------------------------------------------------------------------
// CMiniNode Class
class MiniBsp ;
class CMiniNode
{
public:
    CMiniNode(MiniBsp* pBsp, DWORD dw=0)
    {
        _idxNodeThis  = -1;
        _nodeidx[0]	  = -1;
        _nodeidx[1]	  = -1;
        _planeIdx     = -1;
        _nPolys       = 0;
        _polyIdx      = -1;
        _flags        = dw;
        _pBsp         = pBsp;
        _leafIdx	  = -1;
        _idxParent	  = -1;

    }
    virtual ~CMiniNode(){}
    CMiniNode* Back();
    CMiniNode* Front();
    CMiniNode* Parent();
    int     FontIdx(){return _nodeidx[1];}
    int     BackIdx(){return _nodeidx[0];}
    BOOL	IsEmptyLeaf()  {return  (_flags & NODE_LEAF) && !(_flags & NODE_SOLID);}
    BOOL	IsLeaf()  {return  (_flags & NODE_LEAF);}
    BOOL	IsSolid() {return (_flags & NODE_SOLID);}
    BOOL	IsSpace() {return !(_flags & NODE_SOLID) && (_flags & NODE_LEAF);}
    BOOL	IsNode()  {return !(_flags & NODE_LEAF);}
    INLN    Plane&  GetPlane()const;//{
    INLN    Poly*   GetPolys(int& count);
public:
    int          _idxNodeThis;  // this idx in bsp array
    int          _idxParent;    // this idx in bsp array
    int		     _nodeidx[2];   // front back
    int		     _nPolys;       // leaf polys
    int		     _polyIdx;      // start poly leaf index
    int          _planeIdx;     // spliter plane index in bsp plane arrray
    int			 _leafIdx;
    DWORD	     _flags;
    MiniBsp*     _pBsp;
    BBox         _bbox;

};

//---------------------------------------------------------------------------------------
// MiniBsp class
class MiniBsp
{
public:
    friend class CMiniNode;
    MiniBsp(int expected=32){
        _polys.reserve(expected);
        _nodesPtr.reserve(expected*2);
        _bRemUS     =  FALSE;
        _bRemCoplan = TRUE;
        _bClipSolid = TRUE;
        _bremES     = FALSE;
    };
    virtual ~MiniBsp(){
        Clear();
    };
    virtual void	Clear(){
        _nodesPtr.deleteelements();
        _planes.clear();
        _polys.clear();
        _nodesPtr.clear();
    }
    vvector<Poly>&  GetPolys() {return _polys;};
    virtual BOOL    Compile(vvector<Poly>& polys, BOOL bLog=FALSE); // virtual from base
    void            ClipBrPolys(vvector<Poly>&, vvector<Poly>& , BOOL , BOOL ,BOOL);
    CMiniNode*      Root(){return _nodesPtr[0];}//       _pRoot;
    vvector<CMiniNode*>& GetNodes(){return _nodesPtr; }
    CMiniNode*      GetNode(int idx){ASSERT(idx < _nodesPtr.size()); return _nodesPtr[idx];}
    Plane&          GetPlane(int iplane){ASSERT(iplane < _planes.size()); return _planes[iplane];}
    Poly&           GetPoly(int idx) {ASSERT(idx < _polys.size()); return _polys[idx];}
    BOOL            GetError(){return _bComplErr;}
protected:
    virtual void   AddNode(CMiniNode* pNn){
        pNn->_idxNodeThis = _nodesPtr.size();
        _nodesPtr << pNn;
    }
    void    MakeRoot(){ASSERT(_nodesPtr.size()==0); AddNode(new CMiniNode(this));};
    void	BuildPlaneArray(vvector<Poly>& polys);
    void    R_ClipPolyList(int,  vvector<Poly>& , vvector<Poly>&);
    void	R_Compile(int nodeIdx, vvector<Poly>&);
    int 	GetBestSplitter(vvector<Poly>& polys);
    void	RepairSplits(vvector<Poly>& , vvector<Poly>& );
    void	Trace(int idxnode);
    virtual void UpdateNodeBB(CMiniNode* pNode, vvector<Poly>& polys){};
    virtual CMiniNode*  CreateNode(DWORD dw){return new CMiniNode(this, dw);}

public:
    vvector<CMiniNode*>	_nodesPtr;
    vvector<Poly>	    _polys;
    vvector<Plane>	    _planes;
    vvector<Poly>*	    _pInPolys;
    BOOL                _bRemUS;
    BOOL			    _bRemCoplan;
    BOOL			    _bClipSolid;
    BOOL                _bComplErr;
    BOOL                _bremES;
};

//------------------------------------------------------------------------------------------
INLN Plane& CMiniNode::GetPlane()const
{
    ASSERT(_planeIdx>=0 && _planeIdx < _pBsp->_planes.size());
    return _pBsp->_planes[_planeIdx];
};

//------------------------------------------------------------------------------------------
INLN Poly*   CMiniNode::GetPolys(int& count){
    vvector<Poly>& polys = _pBsp->GetPolys();
    count = _nPolys;
    return &polys[_polyIdx];
};



#endif // !__MINIBSP_H__
