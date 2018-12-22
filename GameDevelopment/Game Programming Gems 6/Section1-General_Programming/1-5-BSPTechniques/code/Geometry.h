/*
    Game Programming Gems: Vol 6, General Programming
    Gem:    BSP Techniques
    Author: Octavian Marius Chincisan
*/
#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "types.h"


#define	SPLITTER_POLY	    0x2
#define	POLY_DELETED	    0x8
#define	POLY_BT_ISSPLITER	0x10
#define POLY_PORTAL0        0x20
#define POLY_PORTAL1        0x40
#define	POLY_PROCESSED		0x80
#define POLY_HIDDEN         0x100
#define POLY_DIRTY          0x800
#define POLY_SELECTED       0x2000      
#define BRSH_SOLID          0x1
#define BRSH_REV            0x2
#define BRSH_DIRTY          0x8
#define BRSH_TERRAIN        0x10


INLN BOOL IsOrtogonal(const V3& v)
{
    return v.x==1 ||v.y==1||v.z==1||v.x==-1||v.y==-1||v.z==-1;
}

class   Brush;
typedef vvector<Vtx>             Vertexes;
typedef vvector<Vtx*>            PVertexes;
typedef vvector<Vtx>::iterator   PVertex;

class __declspec (novtable) Poly : public Plane
{
public:
    int             _planeIdx;      
    Box             _box;           
    Brush*          _pBrush;        
    Vertexes        _vtci;	        
    DWORD           _flags;

public:
    Poly();
    ~Poly();
    Poly(const Poly& p);
    void    Clear();
    void    Create(int points, V3* pPoints, Brush* pBrush);
    void    Create(int points, Vtx* pPoints, Brush* pBrush);
    void    Recalc();
    void    AddPoly(Poly& p);
    BOOL    ContainPoint(V3& pi);
    void    CopyProps(Poly& p);
    BOOL    IntersectsPoly(Poly& p2);
    void	SortVertexes();
    REL_POS Classify(Plane& plane);
    int     Classify(V3& point);
    void    Split(Plane& plane, Poly& a, Poly& b, BOOL bAnyway=0);
    V3      GetCenter();
    void    Clip(Poly& plane, Poly& pout);
    BOOL	IsSplitter(){return (_flags & SPLITTER_POLY);}
    void	SetSplitter(){_flags |= SPLITTER_POLY;}
    Poly&   operator=(const Poly& p);
    void    CalcNormal();
    Poly&   operator<<(Vtx& v);
    Poly&   operator<<(V3& v);
    BOOL    operator == (const Poly& p);
    BOOL    IsInnerPoint(V3& pi);
};

typedef vvector<Poly>             Polys;
typedef vvector<Poly*>            PPolys;
typedef vvector<Poly>::iterator   PPoly;
class Brush  
{
public:
    Polys	 _polys;
    Box      _box;
    DWORD    _flags;

public:  
    Brush();
    Brush(const Brush& b);
    virtual ~Brush();
    Brush&  operator<<(const Brush& b);
    Brush&  operator=(const Brush& b);
    Brush&  operator<<(Poly& p);
    Brush&  operator<<(vvector<Poly>& p);
    void    Recalc();
    void    Clear();
    void    Equal(const Brush& b, BOOL polys=1);
    BOOL	IsDirty(){return _flags&BRSH_DIRTY;};
    void	Dirty(BOOL b){
        if(b) _flags|=BRSH_DIRTY; 
        else  _flags&=~BRSH_DIRTY;
    };
    
};

INLN V3 GCalcNormal(const V3& p0, const  V3& p1, const  V3& p2)
{
    V3 vV1 = p0-p1; 
    V3 vV2 = p0-p2;
    V3 n = Vcp(vV1, vV2);	
    n.norm();	
    return n;
}

INLN void	GCalcNormal(Plane* p, V3& a, V3& b, V3& c)
{
    p->CalcNormal(c,b,a);
}

#endif // !__GEOMETRY_H__
