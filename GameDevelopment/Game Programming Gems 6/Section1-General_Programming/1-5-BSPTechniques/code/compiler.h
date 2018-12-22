#ifndef __COMPILER_H__
#define __COMPILER_H__

#include "geometry.h"
#include <list>

//--------------------------------------------------------------------------------
#define	NODE_EMPTY      0x0
#define	NODE_SOLID      0x1
#define	NODE_LEAF       0x80000000
#define	N_FRONT		    1
#define	N_BACK		    0
#define FLAG_PROCESSED  0x8000000

//--------------------------------------------------------------------------------
#define _THREAD_WAIT_BRANCH_    

//--------------------------------------------------------------------------------
class BaseThrCB
{
public:
    BaseThrCB(){};
    virtual ~BaseThrCB(){};
    virtual int ThreadMain()=0;
    static int ThreadFunction(void* p){return reinterpret_cast<BaseThrCB*>(p)->ThreadMain();}
    void Start(){
        DWORD uid;
        h_t = ::CreateThread(0,0,reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadFunction), this,0,&uid);
    }
    void Wait(){::WaitForSingleObject(h_t, INFINITE);}
    HANDLE  h_t;
};
//--------------------------------------------------------------------------------
class Bsp_Tree;
class CBspNode  
{
public:
    CBspNode(Bsp_Tree* pBsp, DWORD dw=0)
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
    virtual ~CBspNode(){}
    CBspNode* Back();
    CBspNode* Front();
	CBspNode* Parent();
    int     FontIdx(){return _nodeidx[1];}
    int     BackIdx(){return _nodeidx[0];}
    BOOL	IsEmptyLeaf()  {return  (_flags & NODE_LEAF) && !(_flags & NODE_SOLID);}
	BOOL	IsLeaf()  {return  (_flags & NODE_LEAF);}
	BOOL	IsSolid() {return (_flags & NODE_SOLID);}
    BOOL	IsSpace() {return !(_flags & NODE_SOLID) && (_flags & NODE_LEAF);}
	BOOL	IsNode()  {return !(_flags & NODE_LEAF);}
    INLN    Plane&  GetPlane()const;//
    INLN    Poly*   GetPolys(int& count);

public:
    int         _idxNodeThis;   
    int         _idxParent;     
    int		    _nodeidx[2];    
    int         _planeIdx;      
	int			_leafIdx;
	DWORD	    _flags;			
    int		    _nPolys;       
    int		    _polyIdx;      
    Bsp_Tree*   _pBsp; 
    Box         _bbox;
};

//--------------------------------------------------------------------------------
class CLeaf : public CBspNode
{
public:
    CLeaf(Bsp_Tree* pBsp, DWORD dw=0):CBspNode(pBsp, dw),
                                      _pvsIdx(-1),
                                      _portIdxes(4),
                                      _flags(0){
    }
    virtual ~CLeaf(){_portIdxes.clear();}
    void     AddPortalIdx(int idx){_portIdxes.push_back(idx);};
    vvector<int>&   GetPortalIdxes(){return _portIdxes;}
	int				_pvsIdx;
    vvector<int>    _portIdxes;      
    DWORD           _flags;
};

//--------------------------------------------------------------------------------
class Bsp_Tree ;
struct ThrData
{
    int          nodeIdx; 
#ifdef _THREAD_WAIT_BRANCH_
    list<Poly>*  polys;
#else
    list<Poly>   polys;
#endif //
    Bsp_Tree*    ptree;
};

//--------------------------------------------------------------------------------
class Compiler;
class Bsp_Tree 
{
public:
    enum _BSP_TYPE{BSP_LEAFY=0, BSP_TERRAIN};
public:
    friend class CBspNode;
    friend class Compiler;
	Bsp_Tree(int expected=128){
		_polys.reserve(expected);
		_nodes.reserve(expected*2);
        _balance      = 6;
        _bsptype      = BSP_LEAFY;
        ::InitializeCriticalSection(&c_s);
	};
	virtual ~Bsp_Tree(){
        ::DeleteCriticalSection(&c_s);
		Clear();
	};
    void            SetType(_BSP_TYPE bt){
        _bsptype    = bt;
    }
	void	        Clear();
    vvector<Poly>&  GetPolys() {return _polys;};
	void            Process(vvector<Poly>& polys, BOOL busethreads); // virtual from base
    int             ThreadMain();
	CBspNode*       Root(){return _nodes[0];}//       _pRoot;
    vvector<CBspNode*>& GetNodes(){return _nodes; }
    CBspNode*          GetNode(int idx){ASSERT(idx < (int)_nodes.size()); return _nodes[idx];}
    Plane&          GetPlane(int iplane){ASSERT(iplane < (int)_planes.size()); return _planes[iplane];}
    Poly&           GetPoly(int idx) {ASSERT(idx < (int)_polys.size()); return _polys[idx];}
    CLeaf*          GetLeaf(int idx){ASSERT(idx < _leafs.size()); return _leafs[idx];}
    BOOL            SegmentIntersect(V3& paa, V3& pab, int node , V3& col);
    int	            GetCurrentLeaf(V3& pov, int nodeIdx=0);
    void	        R_Compile(int nodeIdx, list<Poly>&);
    BOOL            Lock(){if(b_usethreads)::EnterCriticalSection(&c_s); return 1;}
    void            UnLock(){if(b_usethreads)::LeaveCriticalSection(&c_s); ;}
    static int      T_Compile(void*);
private:
    void    Recurs_Or_Thread(int nodeIdx, list<Poly>& polys);
    void    UpdateNodeBB(CBspNode* pNode, list<Poly>& polys);
    void    UpdateNodeBB(CBspNode* pNode, vvector<Poly>& polys);
    BOOL    BuildDummyBSP(vvector<Poly>& polys);
    void    MakeRoot(){ASSERT(_nodes.size()==0); AddNode(new CBspNode(this));};
	void	BuildPlaneArray(list<Poly>& polys);
	void	R_TerrainCompile(int nodeIdx, list<Poly>&);
	int 	GetBestSplitter(list<Poly>& polys, Poly* pWantPoly=0);
	int		Partition(CBspNode* pNode, list<Poly>&, list<Poly>&,list<Poly>&);
	void	PartitionTerrain(CBspNode* pNode, list<Poly>&, list<Poly>&,list<Poly>&);
    void    FindSplitterPlane(list<Poly>& polys, CBspNode* pNode);
    Plane   GetOptimSpliterPlane(list<Poly>& polys, int moe=16);
	BOOL	FormConvex(list<Poly>& frontPolys);
    BOOL    R_SegmentIntersect(int nodeIdx, V3& a, V3& b, V3& col);
    void   AddNode(CBspNode* pNn){
        pNn->_idxNodeThis = _nodes.size(); 
        _nodes << pNn;
        if(pNn->IsEmptyLeaf())
        {
            pNn->_leafIdx = _leafs.size();
            _leafs << (CLeaf*)pNn;
        }
    }
    CBspNode*  CreateNode(DWORD dw){
        if(dw & NODE_SOLID)
            return new CBspNode(this, dw);
        return new CLeaf(this, dw);
    }

public:
	vvector<CBspNode*>	_nodes;
    vvector<CLeaf*>     _leafs;
    vvector<Poly>	    _polys;
	vvector<Plane>	    _planes;
    vvector<Poly>*	    _pInPolys;
    _BSP_TYPE           _bsptype;
	int			        _reachedDepth;
    int                 _balance;
    BOOL                _hitTest;
    DWORD               dw_deltatime;
    BOOL                b_usethreads;
    int                 n_thrcount;
    long                n_threadsup;
    HANDLE              h_handles[64];
    CRITICAL_SECTION    c_s;
};

//------------------------------------------------------------------------------------------
INLN Plane& CBspNode::GetPlane()const
{
    ASSERT(_planeIdx>=0 && _planeIdx < (int)_pBsp->_planes.size());  
	if(_planeIdx>=0 && _planeIdx < (int)_pBsp->_planes.size())
		return _pBsp->_planes[_planeIdx];
	return _pBsp->_planes[0];
};

INLN Poly*   CBspNode::GetPolys(int& count){
    vvector<Poly>& polys = _pBsp->GetPolys();
    count = _nPolys;
    return &polys[_polyIdx];
};

//--------------------------------------------------------------------------------
#define PORT_INITAL     0x1
#define PORT_DUP        0x2
#define PORT_REVERSED   0x4
#define PORT_DELETED    0x8

//--------------------------------------------------------------------------------
class Portal_Prc;
class Portal : public Plane
{
public:
    Portal(){};
    ~Portal()
    {
        _vxes.clear();
        _sideLIdx.clear();
        _sideLIdxFinal.clear();
    }

    Portal(Portal_Prc* pp):_pPortPrc(pp),_flags(0),_planeIdx(-1){}
    Portal(const Portal& r){
        _planeIdx = r._planeIdx;
        _flags    = r._flags;
        _vxes     = r._vxes;
        _sideLIdx = r._sideLIdx;
        _n        = r._n;
        _c        = r._c;
        _pPortPrc = r._pPortPrc; 
        _idxThis  = r._idxThis;
    }
    void Reverse(){
        _flags |= PORT_REVERSED;
		_n.negate();
		_c=-_c;
        _vxes.reverse();
    }
    Portal& operator=(const Portal& r){
        if(this !=&r){
            _planeIdx   = r._planeIdx;
            _flags      = r._flags;
            _vxes       = r._vxes;
            _sideLIdx   = r._sideLIdx;
            _n          = r._n;
            _c          = r._c;
            _pPortPrc   = r._pPortPrc;
            _idxThis    = r._idxThis;
        }
        return *this;
    }
    V3      GetCenter()
    {
        V3 ret;
        for(UINT i=0; i < _vxes.size(); ++i)
        {
            ret+=_vxes[i];
        }
        ret/= (REAL)_vxes.size();
        return ret;
    }

    void AdjustVxOrder()
    {
        Plane tp;
		GCalcNormal(&tp,_vxes[0],_vxes[1],_vxes[2]); 
        if(!IsZero( Vdp(tp._n, _n)-1.0f,.00625f))
        {
            _vxes.reverse();
			GCalcNormal(&tp,_vxes[0],_vxes[1],_vxes[2]);
        }
    }

    REL_POS Classify(Plane& plane)
    {
	    int		fronts  = 0, backs = 0, coinciss = 0;
	    int		vxes	= _vxes.size();
	    REAL	rdp;
	    FOREACH(vvector<V3>, _vxes, vx)
	    {
		    rdp = plane.DistTo(*vx);
		    if(rdp > .5f)	        
				fronts++;
		    else if(rdp < -.5f)    
				backs++;
		    else{
				coinciss++;
				backs++;
				fronts++;}
	    }
	    if (coinciss == vxes) 
		    return ON_PLANE;
	    if (fronts == vxes) 
		    return ON_FRONT;
	    if (backs  == vxes) 
		    return ON_BACK;
	    return ON_SPLIT;
    }
    void CopyProps(Portal& pFrom){
        _planeIdx      = pFrom._planeIdx;
        _flags         = pFrom._flags;
        _sideLIdx      = pFrom._sideLIdx;
        _n             = pFrom._n;
        _c             = pFrom._c;
        _idxThis       = pFrom._idxThis;
    }
    void    AddSideLeafIdx(int idx){_sideLIdx<<idx;}
    void    Split(Plane& plane, Portal& a, Portal& b);
    INLN    CLeaf*  BackLeaf();
    INLN    CLeaf*  FrontLeaf();
    INLN    Plane&  GetPlane();
    void    Clear(){_vxes.clear(); }
public:
    int             _idxThis;
    int             _planeIdx;       // index in planes index of the BSP wich ortal belons too
    DWORD           _flags;
    vvector<V3>     _vxes;
    vvector<int>    _sideLIdx;
    vvector<int>    _sideLIdxFinal;
    Portal_Prc*     _pPortPrc;
};

//--------------------------------------------------------------------------------
class Portal_Prc
{
public:
    friend class Portal;
    friend class Compiler;
    
    Portal_Prc(){
        _pTree = 0;
    }
    ~Portal_Prc(){Clear();}
    void    Process(Bsp_Tree& tree); 
    void    Clear();
    CLeaf*  GetLeaf(int idx){
        CBspNode* pL = _pTree->GetLeaf(idx);
        ASSERT(pL->_flags & NODE_LEAF);
        return (CLeaf*)pL;
    }
    Portal& GetPortal(int idx){return _portals[idx];}
    vvector<Portal>& GetPortals(){return _portals;}
    BOOL    HasPortals(){return _portals.size()>0;}
private:
	void	CutPortalWithNodeBox(Portal& portal, Box& box,BOOL b=0);
    BOOL	CalculateInitialPortal(Portal&, CBspNode* );	
    BOOL    ValidatePortal(Portal&);    
    BOOL	DuplicatePortals(vvector<Portal>& i, vvector<Portal>& o);
    Plane&  GetPlane(int idx){return _pTree->GetPlane(idx);}
    void    AddPortal(Portal& p){
                p._idxThis = _portals.size();
                _portals << p;
            }
    void	ClipWithLeafSides(CLeaf* pLeaf, Portal& portal);
    
public:
    vvector<Portal>    _portals;
    Bsp_Tree*          _pTree;           
    DWORD               dw_deltatime;
};

//----------------------------------------------------------------------------------------
INLN CLeaf*  Portal::BackLeaf(){
    ASSERT(_sideLIdx.size()==2); 
    return _pPortPrc->GetLeaf(_sideLIdx[0]);
}

INLN CLeaf*  Portal::FrontLeaf(){
    ASSERT(_sideLIdx.size()==2); 
    return _pPortPrc->GetLeaf(_sideLIdx[1]);
}

Plane&  Portal::GetPlane(){
    return _pPortPrc->GetPlane(_planeIdx);
}

//--------------------------------------------------------------------------------
struct PortalData
{
    PortalData(int sz):_portArrPvs(sz)
	{
        _possibleVisCount = 0;
        _size   = _portArrPvs.Size();
    }
    ~PortalData(){}
    
    int			_size;
    int			_possibleVisCount;
    CBitArray   _portArrPvs;
};


//--------------------------------------------------------------------------------
class PVS_Prc
{
public:
    PVS_Prc():_nleafs(0),_pvs(0),_cPvss(0){}
    virtual ~PVS_Prc(){
        Clear();
    };
    
    void    Clear(){
		delete[] _pvs;  _pvs=0;
    	_cPvss = 0;
        _portVs.deleteelements();
        dw_deltatime = GetTickCount();

    };
    void Process(Bsp_Tree& tree, Portal_Prc& portalprc);

private:
    BOOL    InitalPortalVis(vvector<Portal>& rPortals);
    void    R_PortalFlood(Portal& portal, BYTE* pVis, int nLeaf);   // flood in neghbour leaf
    void    PerformPVS(Portal& pOrigin, vvector<Portal>& rPortals, BYTE* prevPvs);
	void    GaterLeafsVis();
    BOOL    PortalSeesPortal(Portal& a, Portal& b);

    Bsp_Tree*    _pBspTree;
    Portal_Prc*  _pPortPrc;
    int          _nleafs;

public:
	BYTE*		 _pvs;
	int			 _cPvss;
    vvector<PortalData*>  _portVs;
    DWORD        dw_deltatime;
};


//--------------------------------------------------------------------------------
class CBsp_TechniquesDlg;
class Compiler  : public BaseThrCB
{
public:
    Compiler(){};
    virtual ~Compiler(){};
    BOOL     Compile(Brush& brush, CBsp_TechniquesDlg& theDlg, BOOL usehthreads);
    void     Render(UINT mode, BOOL is3D);
    void     Clear();

private:
    int         ThreadMain();
    void        RenderBSP(BOOL is3D);
    void        RenderPortals(BOOL is3D);

public:
    Portal_Prc          portal_procesor;
    PVS_Prc             pvs_procesor;
    Bsp_Tree            bsp_tree;    
    CBsp_TechniquesDlg* p_dlg;
    Brush*              p_brush;
    BOOL                b_usethreads;
};

#endif // !__COMPILER_H__
