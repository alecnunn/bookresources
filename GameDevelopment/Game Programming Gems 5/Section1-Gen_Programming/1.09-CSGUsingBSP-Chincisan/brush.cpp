/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include <gl/gl.h>			
#include <gl/glu.h>			
#include <gl/glaux.h>	
#include "brush.h"
#include "collections.h"
#include "compiler.h"

//---------------------------------------------------------------------------------------
// Unique brush ID
int Brush::GID=0;

//---------------------------------------------------------------------------------------
// Constructor
Brush::Brush()
{
    _polys.reserve(16);         // Minimize minim mem frag at vec creation
    _pCutBrshes.reserve(16);    // (nest vec cap will be 32 if elements exceeds 16)
    _pSldBrushes.reserve(16);
    _unicID   = GID++;          // Unique ID for the brush
    _s        = V1;             // scalation is 1
    _selected = FALSE;      
    _copyed   = FALSE;
    _flags    = 0;
    _polySec  = 0;
    _thikness = 0;
    _pResult  = this;           // resulting CSG (subtract) initialy ponts here    
};

//---------------------------------------------------------------------------------------
// Destructor
Brush::~Brush()
{
    // If the result has been changed delete it.
    if(_pResult  != this)
        delete _pResult;
    _pResult = 0;
}

//---------------------------------------------------------------------------------------
// Copy constructor.
Brush::Brush(const Brush& b)
{
    Equal(b);
    if(b._pResult != &b){   
        _pResult  = new Brush(*b._pResult);
    }
    else{
        _pResult  = this;
    }
}

//---------------------------------------------------------------------------------------
// = operator
Brush&  Brush::operator=(const Brush& b)
{
    Equal(b);
    if(b._pResult != &b){
        _pResult  = new Brush(*b._pResult);
    }
    else{
        _pResult  = this;
    }
    return *this;
}

//---------------------------------------------------------------------------------------
// Equality function (for convenience of copy constructor and '=' op)
void  Brush::Equal(const Brush& b)
{
    _unicID    = b._unicID;
    _t         = b._t;
    _r         = b._r;
    _s         = b._s;
    _mt        = b._mt;
    _selected  = b._selected;
    _copyed    = b._copyed;
    _box       = b._box;
    _flags     = b._flags;
    _polySec   = b._polySec;
    _thikness  = b._thikness;
    _cf        = b._cf;
    _polys.clear();
    Polys::iterator p  = ((Brush&)b)._polys.begin();
    Polys::iterator e  = ((Brush&)b)._polys.end();
    for(;p!=e;p++){
        Poly pn = *p;
        pn._pBrush = this;
        _polys.push_back(pn);
    }
}

//---------------------------------------------------------------------------------------
// Unused:
void	Brush::MakeTm(){}

//---------------------------------------------------------------------------------------
// Retreive the polygons from this brush by copying them in the 'plout' container
void	Brush::GetTxPolysCopy(Polys& plOut)
{
    Poly pnew;
    plOut.clear();
    FOREACH(Polys, _polys, p){
        pnew.Clear();
        pnew.CopyProps(*p);
        FOREACH(vvector<VTCIS>,p->_vtci,vtci){
            VTCIS v = *vtci;
            pnew._vtci.push_back(v);
        }
        pnew.Recalc();
        plOut.push_back(pnew);
    }
}

//---------------------------------------------------------------------------------------
// Retreive all polygons and reverses them. (vertex order is reversed)
void	Brush::GetTxReversedPolysCopy(Polys& plOut)
{
    plOut.clear();
    plOut.reserve(_polys.size());
    int count  = _polys.size();
    FOREACH(Polys,_polys,p){
        Poly pnew;
        pnew.CopyProps(*p);
        vvector<VTCIS>::reverse_iterator b = p->_vtci.rbegin();
        vvector<VTCIS>::reverse_iterator e = p->_vtci.rend();
        for(;b!=e;b++){
            VTCIS v = *b;
            pnew._vtci.push_back(v);
        }
        pnew.Recalc();
        plOut.push_back(pnew);
    }
}

//---------------------------------------------------------------------------------------
// Adds 'pls' polygons to the this brush. Marks the brush 'dirty'
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

//---------------------------------------------------------------------------------------
// Merge 'b' brush with this brush
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

//---------------------------------------------------------------------------------------
// Adds one polygon to the this brush
Brush&   Brush::operator<<(Poly& p){
    _box.Union(p._box);
    _polys.push_back(p);
    p._pBrush = this;
    return *this;
}

//---------------------------------------------------------------------------------------
// Retreive the transformation matrix of this brush. 
// This is if brushes are allways in 0,0,0 (! Used in this demo)
M4& Brush::GetTrMatrix()
{
    return _mt;
}

//---------------------------------------------------------------------------------------
// Transfom the brush before painting it. (! Used in this demo)
void    Brush::GlTransform()
{
    glTranslatef(_t.x,_t.y,_t.z);
    glRotatef(_r.z,0,0,1);
    glRotatef(_r.y,0,1,0);
    glRotatef(_r.x,1,0,0);
    glScalef(_s.x,_s.y,_s.z);
}

//---------------------------------------------------------------------------------------
// Resets the internal flags and variables of the brush
void    Brush::Clear()
{
    _polys.clear();
    _box.Reset();
    _t        = V0;     // (! Used in this demo)
    _s        = V1;     // (! Used in this demo)
    _r        = V0;     // (! Used in this demo)
    _selected = FALSE;
    _copyed   = FALSE;
    _pCutBrshes.clear();
    _pSldBrushes.clear();
    if(_pResult!=this)
        delete _pResult;
    _pResult = this;
    _mt.identity();     // (! Used in this demo)
}

//---------------------------------------------------------------------------------------
// Returns current brush non CGS-ed
Brush&	Brush::GetPrimitive()
{
#ifdef _NO_PERM             // (! Used in this demo)
    return GetWorldPos();
#else//
    return *this;
#endif //
}

//---------------------------------------------------------------------------------------
// Returns the result brush. 
Brush&	Brush::GetResult()
{
#ifdef _NO_PERM
    return _pResult->GetWorldPos();
#else//
    return *_pResult;
#endif //
}

//---------------------------------------------------------------------------------------
// Runs again on the polygons and recalculates the brush bounding box
// and polygons laying planes
void    Brush::Recalc()
{
    _box.Reset();
    FOREACH(Polys, _polys, p){
        Poly& ps = *p;
        ps._box.Reset();
        FOREACH(vvector<VTCIS>, ps._vtci, vt){
            ps._box.AddPoint(vt->_xyz);
            _box.AddPoint(vt->_xyz);
        }
        ps.CalcNormal();
    }
}

//---------------------------------------------------------------------------------------
// Or's 'f' flag to all faces of this brush
void    Brush::FlagFaces(DWORD f){
    Polys::iterator b  = _polys.begin();
    Polys::iterator e  = _polys.end();
    for(;b!=e;b++){
        (*b)._flags|=f;
    }
}

//---------------------------------------------------------------------------------------
// Flips the vertex order of all polygons. 
void     Brush::Reverse()
{
    _box.Reset();
    Polys::iterator b  = _polys.begin();
    Polys::iterator e  = _polys.end();
    for(;b!=e;b++){
        Poly& ps = *b;
        ps.Reverse();
        _box.Union(ps._box);
    }
    if(_flags & BRSH_REV)
        _flags &= ~BRSH_REV;            // flag the brush 'nonreversed'
    else
        _flags |= BRSH_REV;             // flag the brush 'reversed'
    Dirty(1);
}

//---------------------------------------------------------------------------------------
// Adds a cut brush to this brush. 
void     Brush::AddCutter(Brush* pCutter)
{
    PBrushes::iterator f = find(_pCutBrshes.begin(),_pCutBrshes.end(),pCutter);
    if(f == _pCutBrshes.end())
        _pCutBrshes.push_back(pCutter);
}

//---------------------------------------------------------------------------------------
// Removes a cut brush from this brush 
void     Brush::RemoveCutter(Brush* pCutter)
{
    PBrushes::iterator f = find(_pCutBrshes.begin(),_pCutBrshes.end(),pCutter);
    if(f != _pCutBrshes.end())
        _pCutBrshes.erase(f);
}

//---------------------------------------------------------------------------------------
// Adds a solid brush to this 'cut' brush
void     Brush::AddBrush(Brush* pB)
{
    PBrushes::iterator f = find(_pSldBrushes.begin(),_pSldBrushes.end(),pB);
    if(f == _pSldBrushes.end())
        _pSldBrushes.push_back(pB);
}

//---------------------------------------------------------------------------------------
// Removes a solid brush from this 'cut' brush
void     Brush::RemoveBrush(Brush* pB)
{
    PBrushes::iterator f = find(_pSldBrushes.begin(),_pSldBrushes.end(),pB);
    if(f != _pSldBrushes.end())
        _pSldBrushes.erase(f);
}

//---------------------------------------------------------------------------------------
// Not used in this demo
void	Brush::RecalcTM()
{
    _mt  = MTranslate(_t.x,_t.y,_t.z);
    _mt  *= MRotate(_r.x,_r.y,_r.z);
    _mt	 *= MScale(_s.x,_s.y,_s.z);
}

//---------------------------------------------------------------------------------------
// Moves the brush by translating it by 'v'
void	Brush::Move(V3& v)
{
#ifdef _NO_PERM // (! Used in this demo)
    _t += v;
    RecalcTM();
#else           // Translates every polygon to its new location
    M4 ml = MTranslate(v.x,v.y,v.z);
    Polys::iterator b  = _polys.begin();
    Polys::iterator e  = _polys.end();
    for(;b!=e;b++){
        Poly& ps = *b;
        ps.Transform(ml);
    }
#endif //
    Dirty(1);
}

//---------------------------------------------------------------------------------------
// Limits the scalation vector in relation with the current zooming factor to avoid
// microscpic or too huge brushes
void    Brush::LimitScaleVector(V3& v, REAL zoom)
{
    BBox nBox(_box);
    REAL mindim = max(8, 64 * zoom);
    nBox._max.x += v.x;
    nBox._max.y += v.y;
    nBox._max.z += v.z;
    V3   ex =  nBox.GetExtends();
    if(ex.x < mindim)
        v.x=0;
    if(ex.y < mindim)
        v.y=0;
    if(ex.z < mindim)
        v.z=0;
    if(_flags & BRSH_SELBOX)
        return;
    if(ex.x > 3200.f)
        v.x=0;
    if(ex.y > 3200.f)
        v.y=0;
    if(ex.z > 3200.f)
        v.z=0;
}

//---------------------------------------------------------------------------------------
// Rescales the brush polygons due to a scalation operation.
void    Brush::RescalePolys(int s, int e, V3& sv, V3& tv, BBox& nBox, BBox& ob)
{
    nBox._max.x += sv.x;
    nBox._max.y += sv.y;
    nBox._max.z += sv.z;
    V3	rap = (nBox._max-nBox._min)/(ob._max-ob._min);
    for(int i=s; i< e;i++){
        Vertexes::iterator vb  = _polys[i]._vtci.begin();
        Vertexes::iterator ve  = _polys[i]._vtci.end();
        for(;vb!=ve;vb++){
            // recalc each point relative to new box
            V3&	vx  = vb->_xyz;
            vx     = ob._min + (vx-ob._min) * rap;
            vx     += tv;
        }
    }
}

//---------------------------------------------------------------------------------------
// Scales the current Brush
void	Brush::Scale(V3& s, V3& t, REAL zoom)
{
    if(_polySec){           // This is to preserve the thikness of a hallow brush
        BBox nBox;
        BBox nBox1;
        for(int i=0; i< _polySec;i++){
            nBox.Union(_polys[i]._box);
        }
        nBox1 = nBox;
        RescalePolys(0, _polySec, s, t, nBox, nBox1);   // exterioe polygons
        nBox.Reset();
        for( i=_polySec; i< _polys.size();i++){
            nBox.Union(_polys[i]._box);
        }
        nBox1 = nBox;
        RescalePolys(_polySec, _polys.size(), s, t ,nBox, nBox1); // interior
    }else{
        BBox nBox(_box);
        BBox nBox1(_box);
        RescalePolys(0, _polys.size(), s, t ,nBox, nBox1);
    }
    Dirty(1);
}

//---------------------------------------------------------------------------------------
// This function is unused.
void	Brush::Raise(V3& v, REAL zoom)
{
#ifdef _NO_PERM
    _f += v;
#else
    Shift(v);
#endif //
    Dirty(1);
}

//---------------------------------------------------------------------------------------
// This function is malfunctioning and has been taken out
void	Brush::Shift(V3& v)
{
    // v is the translation far point
    V3 ex = _box.GetExtends();
    Polys::iterator b  = _polys.begin();
    Polys::iterator e  = _polys.end();
    for(;b!=e;b++){
        Poly& ps = *b;

        Vertexes::iterator vb  = ps._vtci.begin();
        Vertexes::iterator ve  = ps._vtci.end();
        for(;vb!=ve;vb++){
            // recalc each point relative to new box
            V3&	vx   = vb->_xyz;
            V3	rap  = (vx-_box._min) / ex;
            vx.y     +=  v.x * rap.x;
            vx.z     +=  v.y * rap.y;
            vx.x     +=  v.z * rap.z;
        }
    }
}

//---------------------------------------------------------------------------------------
// Rotates the brush arount it's center by 'v' angle specified in radians
void	Brush::Rotate(V3& v)
{
#ifdef _NO_PERM
    _r+=v;
    RecalcTM();
#else //
    M4 m   = MRotate(v.x,v.y,v.z);
    V3	at = _box.GetCenter();
    Polys::iterator b  = _polys.begin();
    Polys::iterator e  = _polys.end();
    for(;b!=e;b++){
        Poly& ps = *b;
        Vertexes::iterator vb  = ps._vtci.begin();
        Vertexes::iterator ve  = ps._vtci.end();
        for(;vb!=ve;vb++){
            vb->_xyz-=at;           //  translate back in 0
            m.rotateonly(vb->_xyz); //  rotate
            vb->_xyz+=at;           //  translate back on position
        }
    }
#endif //
    Dirty(1);
}


//---------------------------------------------------------------------------------------
// Scan all brushes and link to this all possible cut touching brushes or the other way
// around
void    Brush::UpdateLinks()
{
AGAIN:  
    vvector<Brush*>::iterator ppCut  =  _pCutBrshes.begin();
    vvector<Brush*>::iterator ppCutE =  _pCutBrshes.end();
    for(;ppCut!=ppCutE;ppCut++){
        if(!_box.IsTouchesBox((*ppCut)->_box)){
            (*ppCut)->RemoveBrush(this);
            RemoveCutter((*ppCut));
            goto AGAIN;
        }
    }
    if(_pCutBrshes.size()==0){
        if(_pResult!=this)
            delete _pResult;
        _pResult=this;
    }
}

//---------------------------------------------------------------------------------------
// Gizmoing the result pointer.
void	Brush::AlocateResultPtr()
{
    if(_pCutBrshes.size()==0){
        if(_pResult != this){
            delete _pResult;
        }
        _pResult=this;
        return;
    }
    if(_pResult == this)
        _pResult = new Brush();
}

//---------------------------------------------------------------------------------------
// Replans a face if it's vertexes are out of initial plane. 
// (It should triangulate the face rather then replan it)
void    Brush::Replan()
{
    Polys::iterator b  = _polys.begin();
    Polys::iterator e  = _polys.end();
    for(;b!=e;b++){
        Poly& ps = *b;
        Plane plOut;
        Vertexes::iterator vb  = ps._vtci.begin();
        Vertexes::iterator ve  = ps._vtci.end();
        V3&  v1 = vb->_xyz;        vb++;
        V3&  v2 = vb->_xyz;        vb++;
        for(;vb!=ve;vb++){
            V3& v3 = vb->_xyz;
            plOut.CalcNormal(v1, v2, v3);
            if(!IsZero(plOut._normal - ps._normal)){
                V3 s = v3+ps._normal;
                ps.RayIntersect(s,-ps._normal,v3);    // replan the point
            }
        }
    }
}

//---------------------------------------------------------------------------------------
// Makes a cube 
/* Cube vertexes order for a given face
    0   1
    3   2    
*/
void      Brush::MakeCube(V3& dims, TEX_MAP tm)
{
    Poly p;

    V3  v[8];
    V3  fv[4];

    v[0]  =V3(-(dims.x/2), (dims.y/2), -(dims.z/2));
    v[1]  =V3(-(dims.x/2), (dims.y/2), (dims.z/2));
    v[2]  =V3((dims.x/2), (dims.y/2), (dims.z/2));
    v[3]  =V3((dims.x/2), (dims.y/2), -(dims.z/2));
    v[4]  =V3(-(dims.x/2), -(dims.y/2), -(dims.z/2));
    v[5]  =V3((dims.x/2), -(dims.y/2), -(dims.z/2));
    v[6]  =V3((dims.x/2), -(dims.y/2), (dims.z/2));
    v[7]  =V3(-(dims.x/2), -(dims.y/2), (dims.z/2));

    fv[0] = v[0];    fv[1] = v[1];    fv[2] = v[2];    fv[3] = v[3];
    p.Create(4, fv, this);
    *(this) << p;

    fv[0] = v[4];    fv[1] = v[5];    fv[2] = v[6];    fv[3] = v[7];
    p.Create(4, fv, this);
    *(this) << p;

    fv[0] = v[1];    fv[1] = v[7];    fv[2] = v[6];    fv[3] = v[2];

    p.Create(4, fv, this);
    *(this) << p;

    fv[0] = v[0];    fv[1] = v[3];    fv[2] = v[5];    fv[3] = v[4];
    p.Create(4, fv,this);
    *(this) << p;


    fv[0] = v[0];    fv[1] = v[4];    fv[2] = v[7];    fv[3] = v[1];
    p.Create(4, fv, this);
    *(this) << p;

    fv[0] = v[3];    fv[1] = v[2];    fv[2] = v[6];    fv[3] = v[5];
    p.Create(4, fv, this);
    *(this) << p;
    Recalc();
    Dirty(1);
}

//---------------------------------------------------------------------------------------
// Create a cylinder standing on ZOX. The rotation toolbar tool alows to reax the cylinder
// on any Xoyz planes
void Brush::MakeCylinder(REAL rad, REAL height, int radstrips, int vstrips, TEX_MAP tm)
{
    Brush    upper;
    Brush    lower;
    Brush    side;
    double   arcstep = DOIPI/radstrips;
    VTCIS    upPoly[36];
    VTCIS    dnPoly[36];
    VTCIS    vSide[4];
    double   arcPrev=0.0;
    Poly     pu,pd,ps;
    double   uplimit = DOIPI + arcstep/2.0;
    double   tuside = 0;
    double   tustep = 1.0/radstrips;
    V3		 prevUpDn[2];

    // Create top, bottom and side 
    int nSec = 0;
    for(double arc = arcstep; arc < uplimit; ){
        // up and down covers
        upPoly[nSec]._xyz.y   = height/2.0;
        upPoly[nSec]._xyz.x   = sin(arcPrev)*rad;
        upPoly[nSec]._xyz.z   = cos(arcPrev)*rad;
        upPoly[nSec]._uv[0].u = .5+sin(arcPrev)/2.0;
        upPoly[nSec]._uv[0].v = .5+cos(arcPrev)/2.0;
        dnPoly[nSec]._xyz   = upPoly[nSec]._xyz;
        dnPoly[nSec]._xyz.y = -height/2;
        dnPoly[nSec]._uv[0] = upPoly[nSec]._uv[0];

        if(nSec > 0){
            vSide[0]._xyz = upPoly[nSec]._xyz;
            vSide[1]._xyz = upPoly[nSec-1]._xyz;
            vSide[2]._xyz = dnPoly[nSec-1]._xyz;
            vSide[3]._xyz = dnPoly[nSec]._xyz;

            switch(tm){
            default:
            case TM_SPH:
            case TM_LIN:
                vSide[0]._uv[0] = UV(1,0);
                vSide[1]._uv[0] = UV(0,0);
                vSide[2]._uv[0] = UV(0,1);
                vSide[3]._uv[0] = UV(1,1);
                break;
            case TM_CYL:
                vSide[0]._uv[0] = UV(tuside   ,0);
                vSide[1]._uv[0] = UV(tuside-tustep,		  0);
                vSide[2]._uv[0] = UV(tuside-tustep,		  1);
                vSide[3]._uv[0] = UV(tuside    ,1);
                break;
            }

            vSide[0]._uv[1] = UV(1,0);
            vSide[1]._uv[1] = UV(0,0);
            vSide[2]._uv[1] = UV(0,1);
            vSide[3]._uv[1] = UV(1,1);

            ps.Create(4,vSide,this);
            side   << ps;
        }
        arcPrev =  arc;
        arc     += arcstep;
        tuside  += tustep;
        nSec++;
    }

    vSide[0]._xyz = upPoly[0]._xyz;
    vSide[1]._xyz = upPoly[nSec-1]._xyz;
    vSide[2]._xyz = dnPoly[nSec-1]._xyz;
    vSide[3]._xyz = dnPoly[0]._xyz;

    switch(tm){
    default:
    case TM_SPH:
    case TM_LIN:
        vSide[0]._uv[0] = UV(1,0);
        vSide[1]._uv[0] = UV(0,0);
        vSide[2]._uv[0] = UV(0,1);
        vSide[3]._uv[0] = UV(1,1);
        break;
    case TM_CYL:
        vSide[0]._uv[0] = UV(1,0);
        vSide[1]._uv[0] = UV(tuside-tustep     ,0);
        vSide[2]._uv[0] = UV(tuside-tustep     ,1);
        vSide[3]._uv[0] = UV(1,1);
        break;
    }
    vSide[0]._uv[1] = UV(1,0);
    vSide[1]._uv[1] = UV(0,0);
    vSide[2]._uv[1] = UV(0,1);
    vSide[3]._uv[1] = UV(1,1);

    ps.Create(4,vSide,this);
    side   << ps;

    pu.Create(nSec,upPoly,this);
    pd.Create(nSec,dnPoly,this);
    upper<<pu;
    lower<<pd;

    lower.Reverse();
    (*this)<<side;
    (*this)<<upper;
    (*this)<<lower;

    Recalc();
    Dirty(1);
}

//---------------------------------------------------------------------------------------
// Creates a cone standing on ZOX.
void    Brush::MakeCone(REAL rad, REAL height, int radstrips, int vstrips, TEX_MAP tm)
{
    Brush     baseBr;
    Brush     side;
    
    double   arcstep = DOIPI/radstrips;
    VTCIS    basePoly[36];
    VTCIS    vSide[3];
    double   arcPrev=0.0;
    Poly     pu,pd,ps;
    double   uplimit = DOIPI + arcstep/2.0;
    double   tuside = 0;
    double   tustep = 1.0/radstrips;
    V3		 prevUpDn[2];
    V3       topVct(0,height/2.0f,0);

    // create top cover and side in one shot
    int nSec = 0;
    for(double arc = arcstep; arc < uplimit; ){
        // down cap
        basePoly[nSec]._xyz.y   = -height/2.0;
        basePoly[nSec]._xyz.x   = sin(arcPrev)*rad;
        basePoly[nSec]._xyz.z   = cos(arcPrev)*rad;

        basePoly[nSec]._uv[0].u = .5+sin(arcPrev)/2.0;
        basePoly[nSec]._uv[0].v = .5+cos(arcPrev)/2.0;

        if(nSec > 0){
            vSide[0]._xyz = basePoly[nSec-1]._xyz;
            vSide[1]._xyz = basePoly[nSec]._xyz;
            vSide[2]._xyz = topVct;

            switch(tm){
            default:
            case TM_SPH:
            case TM_LIN:
                vSide[0]._uv[0] = UV(0.f, 0);
                vSide[1]._uv[0] = UV(0.f, 1);
                vSide[2]._uv[0] = UV(1.f, .5f);
                break;
            case TM_CYL:
                vSide[0]._uv[0] = UV(0.f, tuside-tustep);
                vSide[1]._uv[0] = UV(0.f, tuside);
                vSide[2]._uv[0] = UV(1.f, tuside-(tustep/2.f));
                break;
            }

            vSide[0]._uv[1] = UV(0.f, 0);
            vSide[1]._uv[1] = UV(0.f, 1);
            vSide[2]._uv[1] = UV(1.f, .5f);

            ps.Create(3,vSide,this);
            side   << ps;
        }

        arcPrev =  arc;
        arc     += arcstep;
        tuside  += tustep;
        nSec++;
    }

    vSide[1]._xyz = basePoly[0]._xyz;
    vSide[0]._xyz = basePoly[nSec-1]._xyz;
    vSide[2]._xyz = topVct;

    switch(tm){
    default:
    case TM_SPH:
    case TM_LIN:
        vSide[0]._uv[0] = UV(0.f, 0);
        vSide[1]._uv[0] = UV(0.f, 1);
        vSide[2]._uv[0] = UV(1.f, .5f);
        break;
    case TM_CYL:
        vSide[0]._uv[0] = UV(0.f, tuside-tustep);
        vSide[1]._uv[0] = UV(0.f, 1);
        vSide[2]._uv[0] = UV(1.f, tuside-(tustep/2.f));
        break;
    }
    vSide[0]._uv[1] = UV(0.f, 0);
    vSide[1]._uv[1] = UV(0.f, 1);
    vSide[2]._uv[1] = UV(1.f, .5f);

    ps.Create(3,vSide,this);
    side   << ps;

    pu.Create(nSec,basePoly,this);
    baseBr<<pu;

    baseBr.Reverse();
    (*this)<<side;
    (*this)<<baseBr;

    Recalc();
    Dirty(1);
}

//---------------------------------------------------------------------------------------
// Make stairs. Makes several Boxes and then applys CSG union between them.
void    Brush::MakeStairs(V3&   dims,                   
                          int   radstrips,
                          int   vstrips,
                          REAL  sThick,
                          TEX_MAP tm)
{
    // vstrips are number if stairs
    // hstrips is ignored
    double      stepY   = dims.y / (double)vstrips;
    double      stepZ   = -(dims.z-vstrips) / (double)vstrips;//allow union
    V3          nextOrg = -dims/2;

    Brush       bt[64];
    PBrushes    pbses;

    // build each stair as a full solid box
    for(int i=0; i < vstrips; i++){
        if(sThick > 0)
            bt[i].MakeCube(V3(dims.x, sThick, stepZ));
        else
            bt[i].MakeCube(V3(dims.x, stepY*(i+1), stepZ));
        bt[i].Reverse();
        bt[i].Move(nextOrg);
        pbses << &bt[i];
        nextOrg +=  V3(0,stepY/2.0,stepZ);
    }
    // Union all solids
    Compiler::Union(pbses,*this);
    Recalc();
    Dirty(1);
}

//---------------------------------------------------------------------------------------
BOOL    Brush::IsSealed()
{
    return TRUE;
}

//---------------------------------------------------------------------------------------
void    Brush::Seal()
{
}

//---------------------------------------------------------------------------------------
// Gets a copy of the brush in world coordinates
Brush&  Brush::GetWorldPos()
{
    static  Brush   locoBrsh;
    locoBrsh.Clear();
    locoBrsh = (*this);
    Polys::iterator b  = locoBrsh._polys.begin();
    Polys::iterator e  = locoBrsh._polys.end();
    for(;b!=e;b++){
        Poly& ps = *b;
        ps.Transform(_mt);
    }
    locoBrsh.Shift(_f);
    locoBrsh.Recalc();
    return locoBrsh;
}

//---------------------------------------------------------------------------------------
// Rotates texture coordinates
void    Brush::RotateTCOneStep()
{
    Polys::iterator b  = _polys.begin();
    Polys::iterator e  = _polys.end();
    for(;b!=e;b++){
        Poly& ps = *b;
        ps.RotateTC(1);
    }
}

//---------------------------------------------------------------------------------------
void    Brush::Snap2grid(REAL pgs, BOOL m)
{
}

//-----------------------------------------------------------------------------
//  Builds a sheet
void    Brush::MakeSheet(V3& dims, int vstrips, int hstrips, TEX_MAP tm)
{
    //// make sheet allways on xoy
    VTCIS v[4];
    REAL xstp = dims.x/(REAL)hstrips;
    REAL ystp = dims.y/(REAL)vstrips;
    REAL ys   = -dims.y/2.f;
    REAL vstp   = 1.0f/(REAL)vstrips;
    REAL us     = 0;
    REAL vs     = 1;
    Poly ps;
    // build faces facing up
    for(int i=0; i < vstrips; i++){
        REAL xs   = -dims.x/2.f;
        REAL ustp   = 1.0f/(REAL)hstrips;

        for(int j=0; j < hstrips; j++)
        {
            v[0]._xyz = V3(xs,ys,0);
            v[1]._xyz = V3(xs+xstp,ys,0);
            v[2]._xyz = V3(xs+xstp,ys+ystp,0);
            v[3]._xyz = V3(xs,ys+ystp,0);

            v[0]._uv[0] = UV(us, vs);
            v[1]._uv[0] = UV(us+ustp, vs);
            v[2]._uv[0] = UV(us+ustp, vs-vstp);
            v[3]._uv[0] = UV(us, vs-vstp);

            v[0]._uv[1] =  v[0]._uv[0];
            v[1]._uv[1] =  v[1]._uv[0];
            v[2]._uv[1] =  v[2]._uv[0];
            v[3]._uv[1] =  v[3]._uv[0];

            ps.Create(4,v,this);
            (*this)  << ps;
            ps.Clear();

            xs += xstp;
            us += ustp;
        }
        ys += ystp;
        vs -= vstp;
    }
    Recalc();
    Dirty(1);
}

//---------------------------------------------------------------------------------------
//
// Commins the CSG. Gets current brush and all it's list of cutters and applys the cuts
// by Subtracting any cut fromm this brush. The resulting brush is stored in _pResult
//
void    Brush::ApplyCuts()
{
    if(_pCutBrshes.size()){
        MiniBsp     cutsBsp;        // cuter brush
        MiniBsp     thisBsp;        // this brush bsp
        Brush       copyBrush;      // copy of this brush polygons 
        Brush       interBrsh;      // temporary brush
        Brush       allCuts;        // all cuts summed together in one brush

        AlocateResultPtr();         // make room for the result bsrush
        copyBrush.Equal(*this);     // make a copy of this
        interBrsh._flags = _flags;  
        Compiler::Union(_pCutBrshes, allCuts);   // union all cuts in a bigger one
        cutsBsp.Compile(allCuts._polys);         // compile the whole cuts bsp
        thisBsp.Compile(_polys);                 // compile this BSP

        //
        // Clip away all polygons ending up inside of cut brush 'cutsBsp'
        // and store the remains in interBrsh
        //
        cutsBsp.ClipBrPolys(copyBrush._polys, interBrsh._polys,1,1,1);

        //
        // Reverse all cuts and now do the other way around. Store all cuts
        // remains ending up inside of a solid. Store the remains in the interBrsh
        //
        allCuts.Reverse();
        thisBsp.ClipBrPolys(allCuts._polys, interBrsh._polys,1,0,0);

        //
        // well we are done
        //
        if(interBrsh._polys.size()){
            _pResult->Clear();
            _pResult->Equal(interBrsh); // place the result in the result pointer
        }
    }
    UpdateLinks();                      // remake the touching list of this brush.
}
