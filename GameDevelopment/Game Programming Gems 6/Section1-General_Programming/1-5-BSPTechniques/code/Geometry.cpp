/*
    Game Programming Gems: Vol 6, General Programming
    Gem:    BSP Techniques
    Author: Octavian Marius Chincisan
*/
#include "stdafx.h"
#include "types.h"
#include "Geometry.h"
#include "Bsp_Techniques.h"
#include <set>
using namespace std;

Poly::Poly()
{
    _vtci.reserve(4);
    _planeIdx        = -1;
    _pBrush		     = 0;
    _flags           = 0;
}


void Poly::CalcNormal()
{
    ASSERT(_vtci.size()>=3);
    GCalcNormal(this, _vtci[0]._xyz,_vtci[1]._xyz,_vtci[2]._xyz);
}

Poly& Poly::operator<<(Vtx& v)
{
    _vtci.push_back(v);
    if(_vtci.size() > 3)
        CalcNormal();
    return *this;
}

BOOL Poly::operator==(const Poly& p)
{
    if(_n == p._n &&
       _c == p._c &&
       _vtci.size()  == p._vtci.size()){
        return !memcmp(&_vtci[0], &p._vtci[0], sizeof(Vtx)*_vtci.size());
    }
    return FALSE;
}

void    Poly::Create(int points, Vtx* pPoints, Brush* pBrush)
{
    _pBrush = pBrush;
    _vtci.clear();
    if(points < 3)
        return;
    _box.Reset();
    for(int i=0; i < points;i++){
        _vtci.push_back(pPoints[i]);
        _box.AddPoint(pPoints[i]._xyz);
    }
    CalcNormal();
    SortVertexes();
}

void Poly::Create(int points, V3* pPoints, Brush* pBrush)
{
    _pBrush = pBrush;
    _vtci.clear();
    V3* pWalk = pPoints;
    if(points < 3)
        return;
    _box.Reset();
    for(int i=0; i < points;i++){
        Vtx v;
        v._user = 0;
        v._rgb = CLR(255,255,255);
        v._xyz = *(pWalk++);
        _vtci.push_back(v);
        _box.AddPoint(v._xyz);
    }
    CalcNormal();
    SortVertexes();
}

void	Poly::SortVertexes()
{
    return;
}

BOOL Poly::ContainPoint(V3& pi)
{
    V3	v1; 
    V3	v2;
    REAL sum = DOIPI;
    Vtx&  lastVx = *(_vtci.end()-1);
    v1.setval(lastVx._xyz);
    v1 -= pi;
    v1.norm();
    PVertex b = _vtci.begin();
    PVertex e = _vtci.end();
    for(;b!=e;b++){
        Vtx& wv = *b;
        v2.setval(wv._xyz);
        if(vdist(v1, v2) < 1.f) 
            return FALSE;
        v2 -= pi;
        v2.norm();
        sum -= (REAL)acos(Vdp(v1, v2));
        v1 = v2;
    }
    return IsZero(sum);
}

Poly::Poly(const Poly& p)
{
    this->operator=(p);
}

Poly& Poly::operator=(const Poly& p){
    if(this != &p){
        _vtci.clear();
        CopyProps((Poly&)p);
        FOREACH(vvector<Vtx>, ((Poly&)p)._vtci, vxI){
            _vtci << *vxI;
        }
    }
    return *this;
}

void	Poly::CopyProps(Poly& p)
{
    _n          = p._n;
    _t          = p._t;
    _c          = p._c;
    _box        = p._box;
    _pBrush     = p._pBrush; 
    _flags      = p._flags;
    _planeIdx   = p._planeIdx;
}

void    Poly::Recalc()
{
    _box.Reset();
    PVertex b = _vtci.begin();
    PVertex e = _vtci.end();
    for(;b!=e;b++){
        _box.AddPoint((*b)._xyz);
    }
    CalcNormal();
}


Poly::~Poly()
{
    _vtci.clear();	    // vertexex array
}


void  Poly::Clear()
{
    _box.Reset();
    _vtci.clear();
    _n = V0;        // invalid normal
    _c = 0;
    _t = 0;
}


void    Poly::Split(Plane& plane, Poly& a, Poly& b, BOOL bAnyway)
{
    a.CopyProps(*this);
    b.CopyProps(*this);
    Vtx  iv;
    Vtx itxB = *_vtci.begin();
    Vtx itxA = _vtci.back();
    
    REAL    fB;
    REAL	fA = plane.DistTo(itxA._xyz);
    
    FOREACH(vvector<Vtx>, _vtci, vxI){
        itxB = *vxI;
        fB   = plane.DistTo(itxB._xyz);
        if(fB > .2f){
            if(fA < -.2f){
                REAL   t = -fA /(fB - fA);
                iv.interpolate(itxA,itxB,t);
                a << iv;
                b << iv;
            }
            a<<itxB;
        }
        else if(fB < -.2f){
            if(fA > .2f){
                REAL t = -fA /(fB - fA);           // t of segment
                iv.interpolate(itxA,itxB,t);
                a<<iv;
                b <<iv;
            }
            b <<itxB;
        }else{
            a << itxB;
            b << itxB;
        }
        itxA = itxB;
        fA   = fB;
    }
    
    if(!bAnyway){
        
        if(b._vtci.size() == _vtci.size() && a._vtci.size()<3)
            a.Clear();
        if(a._vtci.size() == _vtci.size() && b._vtci.size()<3)
            b.Clear();
        if(a._vtci.size()<3)
            a.Clear();
        if(b._vtci.size()<3)
            b.Clear();
    }
    
    if(a._vtci.size()>=3)
        a.Recalc();
    if(b._vtci.size()>=3)
        b.Recalc();
}


void   Poly::Clip(Poly& plane, Poly& pout)
{
    pout._vtci.clear();
    pout._box.Reset();
    pout.CopyProps(*this);
    Vtx  iv;
    Vtx& itxB = *_vtci.begin();
    Vtx& itxA = _vtci.back();
    REAL fB;
    int  sB;
    REAL fA = plane.DistTo(itxA._xyz);
    int  sA = plane.Classify(itxA._xyz);
    
    FOREACH(vvector<Vtx>, _vtci, vxI){
        itxB = *vxI;
        fB   = plane.DistTo(itxB._xyz);
        sB   = plane.Classify(itxB._xyz);
        if(sB > 0){
            if(sA < 0){
                REAL   t = -fA /(fB - fA);
                iv.interpolate(itxA,itxB,t);
                pout << iv;
            }
            pout<<itxB;
        }
        else if(sB < 0){
            if(sA > 0){
                REAL t = -fA /(fB - fA);           // t of segment
                iv.interpolate(itxA,itxB,t);
                pout<<iv;
            }
        }else{
            pout << itxB;
        }
        itxA = itxB;
        fA   = fB;
    }
    pout.Recalc();
    ASSERT(pout._vtci.size() >= 3);
}

int     Poly::Classify(V3& point)
{
    V3 vdir = point - _vtci[0]._xyz;
    REAL d = Vdp(vdir, _n);
    
    if (d < -.2f)
        return -1;
    else
        if (d > .2f)
            return 1;
    return 0;
}

REL_POS Poly::Classify(Plane& plane)
{
    int		fronts  = 0, backs = 0, coinciss = 0;
    int		vxes	= _vtci.size();
    REAL	rdp;
    
    FOREACH(vvector<Vtx>, _vtci, ppct){
        rdp = plane.DistTo(ppct->_xyz);
        
        if(rdp > .2f){
            fronts++;
        }
        else if(rdp < -.2f){
            backs++;
        }else{
            coinciss++;
            backs++;
            fronts++;
        }
    }
    if (coinciss == vxes){
        return ON_PLANE;
    }
    if (fronts == vxes){
        return ON_FRONT;
    }
    if (backs  == vxes){
        return ON_BACK;
    }
    return ON_SPLIT;
}

Poly&    Poly::operator<<(V3& v)
{
    Vtx vt;
    vt._xyz = v;
    (*this)<<vt;
    return *this;
};

V3      Poly::GetCenter()
{
    V3 ret;
    for(UINT i=0; i < _vtci.size(); ++i){
        ret+=_vtci[i]._xyz;
    }
    ret/= (REAL)_vtci.size();
    return ret;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Brush::Brush()
{
    _polys.reserve(8);
    _flags = BRSH_DIRTY;
};

Brush::~Brush()
{
    _polys.clear();
}

Brush::Brush(const Brush& b)
{
    Equal(b);
}

Brush&  Brush::operator=(const Brush& b)
{
    if(this == &b)
        return *this;
    Equal(b);
    return *this;
}


void  Brush::Equal(const Brush& b, BOOL polys)
{
    _flags     = b._flags;
    _box       = b._box; 
    Polys::iterator p  = ((Brush&)b)._polys.begin();
    Polys::iterator e  = ((Brush&)b)._polys.end();
    for(;p!=e;p++){
        (*p)._pBrush = this;
        _polys.push_back(*p);
    }
}

Brush&  Brush::operator<<(vvector<Poly>& pls)
{
    Poly pn;
    Polys::iterator p  = pls.begin();
    Polys::iterator e  = pls.end();
    for(;p!=e;p++){
        pn = *p;
        pn._pBrush = this;
        _polys.push_back(pn);
    }
    Dirty(1);
    return *this;
}

Brush&  Brush::operator<<(const Brush& b)
{
    Poly pn;
    Polys::iterator p  = ((Brush&)b)._polys.begin();
    Polys::iterator e  = ((Brush&)b)._polys.end();
    for(;p!=e;p++){
        pn = *p;
        pn._pBrush = this;
        _polys.push_back(pn);
    }
    Dirty(1);
    return *this;
}

Brush&   Brush::operator<<(Poly& p)
{
    _box.Union(p._box);
    _polys.push_back(p);
    p._pBrush = this;
    return *this;
}

void    Brush::Clear()
{
    _polys.clear();
    _box.Reset();
    _flags = 0;
}

void    Brush::Recalc()
{
    DELAG:
    FOREACH(Polys, _polys, p){
        if(p->_flags & POLY_DELETED)
        {
            _polys.erase(p);
            goto DELAG; // safer than p = _polys.erase(p)
        }
    }
    do{ _box.Reset();
        FOREACH(Polys, _polys, p){
            p->_box.Reset();
            p->CalcNormal();
            FOREACH(vvector<Vtx>, p->_vtci, vt){
                vt->_nrm = (p->_n);
                p->_box.AddPoint(vt->_xyz);
                _box.AddPoint(vt->_xyz);
            }
            p->_pBrush = this;
        }
    }while(0);                              // scope
    _flags &= ~BRSH_DIRTY;
}


