/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __POLY_H__
#define __POLY_H__

//---------------------------------------------------------------------------------------
#include "basemath.h"
#include "basegeom.h"
#include "collections.h"
typedef enum _REL_POS
{
    ON_PLANE,
    ON_FRONT,
    ON_BACK,
    ON_SPLIT
}REL_POS;

//---------------------------------------------------------------------------------------
// node flags
#define	NODE_EMPTY      0x0             // node is leaf ending in a empty space (it has content ocupable)    
#define	NODE_SOLID      0x1             // node is leaf engind in a solid space
#define	LEAF_DELETED    0x2             // temp flag indicating node has been deleted
#define	LEAF_MERGED     0x4             // temp flag indicating leaf is merged into next leaf
#define	NODE_LEAF       0x80000000      // node is pointing to a leaf
#define	N_FRONT		    1               // front node index
#define	N_BACK		    0               // back node index


//---------------------------------------------------------------------------------------
// Temporary polygon flags during CSG and BSP construcyion
#define	BACK_CUT_POLY	0x1
#define	SPLITTER_POLY	0x2
#define	POLY_DELNAILED  0x4
#define	POLY_DELETED	0x8

//---------------------------------------------------------------------------------------
// Brush flags
#define BRSH_SOLID      0x1     // brush is solid
#define BRSH_REV        0x2     // brush is reversed
#define BRSH_DETAIL		0x4     // brush is detail
#define BRSH_DIRTY      0x8     // brush is dirty (properties have changed)
#define BRSH_SELBOX     0x10    // selection box
#define BRSH_SELVIS     0x20    // selbox visible
#define BRSH_NEW        0x40    // brush is not yet comited
#define BRSH_CUTALL     0x80    // brush cut's all solids
#define BRSH_REF        0x80000000  // brush is a reference to anbother brush


//---------------------------------------------------------------------------------------
// Texture coordinates for each polygon
struct TexCoords
{
    TexCoords& operator=(const TexCoords& tc){
        ::memcpy(this,&tc,sizeof(TexCoords));
        return *this;
    }
    operator==(const TexCoords& tc){
        return !::memcmp(this,&tc,sizeof(TexCoords));
    }
    int 	rotate;
    REAL	scalex;
    REAL	scaley;
    REAL	shiftx;
    REAL	shifty;
    V3      texNorm[2];
};

//---------------------------------------------------------------------------------------
// Cursor position
#define	SIZE_0	    0x0
#define	SIZE_N	    0x1
#define	SIZE_S	    0x2
#define	SIZE_E	    0x4
#define	SIZE_W	    0x8
#define	SIZE_MOVE	0x10
#define	SIZE_SW	(SIZE_S|SIZE_W)
#define	SIZE_SE	(SIZE_S|SIZE_E)
#define	SIZE_NW	(SIZE_N|SIZE_W)
#define	SIZE_NE	(SIZE_N|SIZE_E)

//---------------------------------------------------------------------------------------
// Extended vertex  
class VTCIS : public VTCI
{
public:
    VTCIS(){}
    VTCIS(const VTCIS& v):VTCI(v){}
    INLN  VTCIS& operator = (const VTCIS& r){ VTCI::operator=(r); return *this;}
    void  Select(){_cmd = 1;}
    void  DeSelect(){_cmd = 0;}
    BOOL  IsSelected(){return _cmd == 1;}
    int   _dummy[2];
};

//---------------------------------------------------------------------------------------
typedef vvector<VTCIS>   Vertexes;
typedef vvector<VTCIS*>  PVertexes;
typedef vvector<VTCIS>::iterator   PVertex;
typedef vvector<VTCIS*>::iterator  PPVertex;


//---------------------------------------------------------------------------------------
// Polygon class.
class Brush;
class Poly : public Plane
{
public:
    // temporary variables (it should go elswhere)
    int             _idxSplitter;   
    int             _idxParent;     
    int             _planeIdx;
    int				_fragments;

    BBox            _box;       // polygon bounding box
    CLR             _color;     // polygon color

    BOOL            _selected;  // polygon is selected
    DWORD           _flags;     // polygon flags
    TexCoords		_texcoord;  // polygon textures coordinates
    Brush*          _pBrush;    // brush this polygon belongs to
    Vertexes        _vtci;      // this polygon vertexes
    DWORD           _props;     // polygon properties (unused)
public:
    Poly():_selected(0){
        _vtci.reserve(4);
        _flags       = 0;
        _idxSplitter = -1;
        _idxParent   = -1;
        _pBrush		 = 0;
        _planeIdx    = -1;
        _fragments   = 0;
        _texcoord.rotate=0;
        _texcoord.scalex=1;
        _texcoord.scaley=1;
        _texcoord.shiftx=0;
        _texcoord.shifty=0;
        _props = 0;
        _color = ZZWHITE;
    }
    ~Poly();
    Poly(const Poly& p);
    void    Clear();
    void    Create(int points, V3* pPoints, Brush* pBrush);
    void    Create(int points, VTCIS* pPoints, Brush* pBrush);
    void	Transform(M4 m);
    Poly&   GetWorldPos(M4& m);
    void	ApplyNewTexCoord();
    void    Recalc();
    BOOL    ContainPoint(V3& pi);
    void    Reverse();
    void    RotateTC(int steps=0);
    void    DeSelect(){
        _selected=FALSE;
        Vertexes::iterator b = _vtci.begin();
        Vertexes::iterator e = _vtci.end();
        for(;b!=e;b++)
            b->Select();
    }
    void    Select(){
        _selected=TRUE;
        Vertexes::iterator b = _vtci.begin();
        Vertexes::iterator e = _vtci.end();
        for(;b!=e;b++)
            b->DeSelect();
    }
    void	CalcTexCoord(BOOL bNew);
    BOOL    IsSelected(){return _selected;}
    void    CopyProps(Poly& p);
    BOOL    IntersectsPoly(Poly& p2);
    void    Split(Plane& plane, Poly& a, Poly& b);
    Poly&   Clip(Poly& plane);
    REL_POS Classify(Plane& plane);
    int     Classify(V3& point);
    BOOL	IsSplitter(){return (_flags & SPLITTER_POLY);}
    void	SetSplitter(){_flags |= SPLITTER_POLY;}
    Poly&   operator=(const Poly& p);
    void    CalcNormal();
    void    FlipTC(char cd);
    void    operator<<(VTCIS& v);
    void    operator<<(V3& v){
        VTCIS vt;
        vt._xyz = v;
        (*this)<<vt;
    };
    BOOL    operator == (const Poly& p);
};

typedef vvector<Poly>              Polys;
typedef vvector<Poly>::iterator    PPoly;
typedef vvector<Poly*>             PPolys;
typedef vvector<Poly*>::iterator   PPPoly;

#endif //__POLY_H__
