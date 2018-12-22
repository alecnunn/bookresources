/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/
#include "stdafx.h"
#include "poly.h"
#include "scene.h"

//---------------------------------------------------------------------------------------
void Poly::CalcNormal()
{
    assert(_vtci.size()>=3);
    Plane::CalcNormal(_vtci[0]._xyz,_vtci[1]._xyz,_vtci[2]._xyz);
}

//---------------------------------------------------------------------------------------
void Poly::operator<<(VTCIS& v)
{
    _vtci.push_back(v);
    if(_vtci.size() > 3)
        CalcNormal();
}

//---------------------------------------------------------------------------------------
BOOL Poly::operator == (const Poly& p)
{
    return (_normal       == p._normal &&
            _const        == p._const &&
            _vtci.size()  ==p._vtci.size() &&
            _pBrush       == p._pBrush &&
            _box._min     == p._box._min &&
            _box._max     == p._box._max &&
            _flags        == p._flags &&
            _props        == p._props &&
            _texcoord     == p._texcoord &&
            _idxParent    == p._idxParent &&
            _idxSplitter  == p._idxSplitter &&
            _planeIdx     == p._planeIdx &&
            _fragments    == p._fragments);
}

//---------------------------------------------------------------------------------------
void    Poly::Create(int points, VTCIS* pPoints, Brush* pBrush)
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
}

//---------------------------------------------------------------------------------------
void Poly::Create(int points, V3* pPoints, Brush* pBrush)
{
    _pBrush = pBrush;
    _vtci.clear();
    V3* pWalk = pPoints;

    if(points < 3)
        return;
    // see the maximum extend of the poly
    _box.Reset();
    for(int i=0; i < points;i++){
        VTCIS v;
        v._cmd = 0;
        v._rgb = CLR(255,255,255);
        v._xyz = *(pWalk++);
        _vtci.push_back(v);
        _box.AddPoint(v._xyz);
    }
    CalcNormal();
    CalcTexCoord(TRUE);
}

//---------------------------------------------------------------------------------------
BOOL Poly::ContainPoint(V3& pi)
{
    V3	    v1;
    V3	    v2;
    REAL    sum = DOIPI;
    VTCIS&  lastVx = *(_vtci.end()-1);

    v1.setval(lastVx._xyz);
    v1 -= pi;
    v1.norm();
    PVertex b = _vtci.begin();
    PVertex e = _vtci.end();
    for(;b!=e;b++){
        VTCIS& wv = *b;
        v2.setval(wv._xyz);
        if(vdist(v1, v2) < 1.f) // no less than 4 cm
            return FALSE;
        v2 -= pi;
        v2.norm();
        sum -= (REAL)acos(VDp(v1, v2));
        v1 = v2;
    }
    return ISZERO(sum);
}

//---------------------------------------------------------------------------------------
void Poly::Reverse()
{
    Vertexes   rev;

    rev.reserve(_vtci.size());
    _box.Reset();
    Vertexes::reverse_iterator b = _vtci.rbegin();
    Vertexes::reverse_iterator e = _vtci.rend();
    for(;b!=e;b++){
        rev.push_back(*b);
        _box.AddPoint((*b)._xyz);
    }
    _vtci = rev;
    CalcNormal();
}

//---------------------------------------------------------------------------------------
Poly::Poly(const Poly& p)
{
    this->operator=(p);
}

//---------------------------------------------------------------------------------------
Poly& Poly::operator=(const Poly& p)
{
    CopyProps((Poly&)p);
    _vtci       =   p._vtci;
    return *this;
}

//---------------------------------------------------------------------------------------
// no vertexes
void	Poly::CopyProps(Poly& p)
{
    _normal     = p._normal;
    _const      = p._const;
    _box        = p._box;
    _pBrush     = p._pBrush;
    _selected   = p._selected;
    _flags      = p._flags;
    _props      = p._props;
    _texcoord   = p._texcoord;
    _idxParent  = p._idxParent;
    _idxSplitter= p._idxSplitter;
    _planeIdx   = p._planeIdx;
    _color    = p._color;

}

//---------------------------------------------------------------------------------------
void    Poly::Recalc()
{
    _box.Reset();
    PVertex b = _vtci.begin();
    PVertex e = _vtci.end();
    for(;b!=e;b++)
        _box.AddPoint((*b)._xyz);
    CalcNormal();
}

//---------------------------------------------------------------------------------------
Poly::~Poly()
{
}

//---------------------------------------------------------------------------------------
void  Poly::Clear()
{
    _box.Reset();
    _vtci.clear();
    _normal = V0;
    _const  = 0;
}

//---------------------------------------------------------------------------------------
Poly&   Poly::GetWorldPos(M4& m)
{
    static Poly pr;
    pr.Clear();
    pr = *this;
    pr.Transform(m);
    return pr;
}

//---------------------------------------------------------------------------------------
void Poly::Transform(M4 m)
{
    _box.Reset();
    PVertex b = _vtci.begin();
    PVertex e = _vtci.end();
    for(;b!=e;b++){
        VTCIS& v = *b;
        m.transform(v._xyz);
        _box.AddPoint(v._xyz);
    }
    CalcNormal();
}

//---------------------------------------------------------------------------------------
// Splits a polygon by a plane
void    Poly::Split(Plane& plane, Poly& a, Poly& b)
{
    a.CopyProps(*this);
    b.CopyProps(*this);

    VTCIS  iv;
    VTCIS itxB = *_vtci.begin();
    VTCIS itxA = _vtci.back();

    REAL    fB;
    REAL	fA = plane.DistTo(itxA._xyz);
    //REAL	fA = plane.GetSide(itxA._xyz);

    FOREACH(vvector<VTCIS>, _vtci, vxI){
        itxB = *vxI;
        fB   = plane.DistTo(itxB._xyz);
        if(fB > GEpsilon){
            if(fA < -GEpsilon){
                REAL   t = -fA /(fB - fA);
                iv.interpolate(itxA,itxB,t);
                a << iv;
                b << iv;
            }
            a<<itxB;
        }
        else if(fB < -GEpsilon){
            if(fA > GEpsilon){
                REAL t = -fA /(fB - fA);           // t of segment
                iv.interpolate(itxA,itxB,t);
                a<<iv;
                b <<iv;
            }
            b <<itxB;
        }
        else{
            a << itxB;
            b << itxB;
        }
        itxA = itxB;
        fA   = fB;
    }

    assert(a._vtci.size() >= 3);
    assert(b._vtci.size() >= 3);
    a.Recalc();
    b.Recalc();
}

//---------------------------------------------------------------------------------------
// Clips a polygon by a plane
Poly&   Poly::Clip(Poly& plane)
{
    static Poly pout;

    pout._vtci.clear();
    pout._box.Reset();
    pout.CopyProps(*this);

    VTCIS  iv;
    VTCIS& itxB = *_vtci.begin();
    VTCIS& itxA = _vtci.back();

    REAL    fB;
    int     sB;
    REAL	fA = plane.DistTo(itxA._xyz);
    int     sA = plane.Classify(itxA._xyz);

    FOREACH(vvector<VTCIS>, _vtci, vxI){
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
        }
        else{
            pout << itxB;
        }
        itxA = itxB;
        fA   = fB;
    }
    pout.Recalc();
    assert(pout._vtci.size() >= 3);
    return pout;
}


//---------------------------------------------------------------------------------------
// classify a point with the respect of the polygon
int     Poly::Classify(V3& point)
{
    V3 vdir = point - _vtci[0]._xyz;
    REAL d = VDp(vdir, _normal);
    if (d < -GEpsilon)
        return -1;
    else if (d > GEpsilon)
        return 1;
    return 0;
}

//---------------------------------------------------------------------------------------
// Classify a polygon with the respect of a plane
REL_POS Poly::Classify(Plane& plane)
{
    int		fronts  = 0, backs = 0, coinciss = 0;
    int		vxes	= _vtci.size();
    REAL	rdp;

    FOREACH(vvector<VTCIS>, _vtci, ppct){
        rdp = plane.DistTo(ppct->_xyz);

        if(rdp > GEpsilon){
            fronts++;
        }
        else if(rdp < -GEpsilon){
            backs++;
        }
        else{
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

//---------------------------------------------------------------------------------------
// Test if a polygon intersects the other polygon
BOOL    Poly::IntersectsPoly(Poly& p2)
{
    V3   edge[2];
    REAL d2p[2];

    // thiis poly edges
    edge[0] = _vtci.back()._xyz;
    FOREACH(vvector<VTCIS>, _vtci, vxI){
        edge[1] = vxI->_xyz;
        // dist to p2
        d2p[0] = p2.DistTo(edge[0]);
        d2p[1] = p2.DistTo(edge[1]);

        if(d2p[0] * d2p[1] < 0) // they are on the oposite dirs of the plane
                                // we may have a intersection
        {
            V3   ip;
            V3   vray = edge[1]-edge[0];
            vray.norm();
            if(p2.RayIntersect(edge[0],vray,ip)){
                if(p2.ContainPoint(ip)){
                    return TRUE;
                }
            }
        }
        edge[0]=edge[1];
    }
    return p2.IntersectsPoly(*this);
}

//---------------------------------------------------------------------------------------
// 
void	Poly::ApplyNewTexCoord()
{
    Recalc();
    CalcTexCoord(FALSE);
}

//---------------------------------------------------------------------------------------
// 
void Poly::CalcTexCoord(BOOL bNew)
{
    static REAL	prevtc = 0.f;
    UV   maxuv[2];
    BBox box = _box;
    box.FlatIt(maxuv);

    for(int i=0; i < _vtci.size();i++)
    {
        V3& v   = _vtci[i]._xyz;
        UV& t   = _vtci[i]._uv[0];

        switch(_box.GetMinimAx())
        {
        case 'x':   //z,y
            t.v = (v.y-maxuv[0].v) / (maxuv[1].v-maxuv[0].v);
            t.u = (v.z-maxuv[0].u) / (maxuv[1].u-maxuv[0].u);

            t.v*=_texcoord.scaley;
            t.u*=_texcoord.scalex;


            t.v+=_texcoord.shifty;
            t.u+=_texcoord.shiftx;


            break;
        case 'y':   //x,z
            t.u = (v.x-maxuv[0].u) / (maxuv[1].u-maxuv[0].u);
            t.v = (v.z-maxuv[0].v) / (maxuv[1].v-maxuv[0].v);

            t.u*=_texcoord.scalex;
            t.v*=_texcoord.scaley;

            t.u+=_texcoord.shiftx;
            t.v+=_texcoord.shifty;

            break;
        case 'z':   //x,y
            t.u = (v.x-maxuv[0].u) / (maxuv[1].u-maxuv[0].u);
            t.v = (v.y-maxuv[0].v) / (maxuv[1].v-maxuv[0].v);

            t.u*=_texcoord.scalex;
            t.v*=_texcoord.scaley;

            t.u+=_texcoord.shiftx;
            t.v+=_texcoord.shifty;

            break;
        }

        _vtci[i]._uv[1] = _vtci[i]._uv[0];

    }
    if(bNew)
    {
        RotateTC();
    }
}

//---------------------------------------------------------------------------------------
// 
void    Poly::RotateTC(int step)
{
    if(!step)
        _texcoord.rotate++;
    _texcoord.rotate %= _vtci.size();

    if(0 == _texcoord.rotate)
        return;
    for(int i=0; i< _texcoord.rotate; i++){
        UV prev = _vtci.back()._uv[0];
        UV tmp;
        for(int j=0; j < _vtci.size(); j++){
            tmp = _vtci[j]._uv[0];
            _vtci[j]._uv[0] = prev;
            prev=tmp;
        }
    }

}

//---------------------------------------------------------------------------------------
// flips tex coord vertically or horizontally
void    Poly::FlipTC(char cd /*V or H*/)
{
    //calc bbox of the texture. use a 3box (only xy)
    BBox box;
    FOREACH(vvector<VTCIS>,_vtci, pvtci){
        VTCIS& rvtci = *pvtci;
        box.AddPoint(V3(rvtci._uv[0].u,rvtci._uv[0].v,0));
    }
    V3& centre = box.GetCenter();

    //calc the bbox of the texture
    _FOREACH(vvector<VTCIS>,_vtci, pvtci){
        VTCIS& rvtci = *pvtci;
        UV& tc = rvtci._uv[0];
        tc -= UV(centre.x,centre.y); // shift that centre to make it 0 based
        if(cd=='H'){
            tc.u=-tc.u;
        }
        else{
            tc.v=-tc.v;
        }
        tc += UV(centre.x,centre.y); //shift back
    }
}




