/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/
#ifndef __BASEGEOM_H__
#define __BASEGEOM_H__

#include "baseos.h"
#include "basemath.h"


//---------------------------------------------------------------------------------------
// Plane Class
class Plane
{
public:
    Plane():_const(0.0){};
    Plane& operator=(const Plane& pl){
        _normal = pl._normal;
        _const = pl._const;
        return *this;
    };
    BOOL operator ==(const Plane& p){
        if(ISZERO(p._const - _const)){
            return ISZERO( VDp(_normal , p._normal)-1.0,  EPSILON);
        }
        return FALSE;
    }
    Plane(const V3& vNormal, const REAL dist):_normal(vNormal),_const(dist){}
    Plane(const V3& vNormal, const V3& point):_normal(vNormal){
        _const = VDp(_normal, point);
    }
    Plane(const V3& p0, const V3& p1, const V3& p2){
        CalcNormal(p0, p1, p2);
    }
    Plane(REAL a, REAL b , REAL c, REAL d){
        _normal = V3(a,b,c);
        _const = d;
    }
    INLN int GetSide(const V3& pct){
        REAL r =  DistTo(pct);
        if(r>EPSILON)
            return 1;
        else if(r<-EPSILON)
            return -1;
        return 0;
    }
    INLN REAL   DistTo(const V3& pct){
        return (VDp(pct, _normal) - _const);
    }
    INLN REAL   FastDistTo(const V3& pct){
        return (VDp(pct, _normal) - _const);
    }
    INLN REAL   DistTo(REAL a, REAL b, REAL c){
        return DistTo(V3(a,b,c));
    }
    INLN REAL   DistTo(int a, int b, int c){
        return DistTo(V3((REAL)a,(REAL)b,(REAL)c));
    }
    INLN void Set(const V3& vNormal, V3& pct){
        _normal = vNormal;
        _const  = VDp(_normal, pct);
    }
    INLN void Set(const V3& vNormal, REAL cnst){
        _normal = vNormal; _const=cnst;
    }
    INLN void	Reverse(){
        _normal.negate();
        _const=-_const;
    }
    INLN void CalcNormal(const V3& p0, const  V3& p1, const  V3& p2){
        V3 vV1 = p0-p1;
        V3 vV2 = p0-p2;
        _normal = VCp(vV1, vV2);
        _normal.norm();
        _const = VDp(p0,_normal);
    }
    INLN BOOL IsYAligned(){return _normal.y == 1.0;}
    INLN BOOL IsFloor(){return _normal.y >.55f;}
    INLN BOOL IsCeiling(){return _normal.y <-.55f;}
    INLN char GetApproxAlignment(){
        if (RABS(_normal.x) > RABS(_normal.y) && RABS(_normal.x) > RABS(_normal.z))
            return 'x';
        if (RABS(_normal.y) > RABS(_normal.x) && RABS(_normal.y) > RABS(_normal.z))
            return 'y';
        return 'z';
    }
    INLN BOOL RayIntersect(const V3& vpos, const V3& vdir, V3& ip){
        REAL dp = VDp(_normal, vdir);
        if(dp  == 0.0)
        {
            ip.setval(INFINIT,INFINIT,INFINIT);
            return FALSE;
        }
        ip = vpos - ((vdir * DistTo(vpos)) / dp);
        return TRUE;
    }
    V3		_normal;
    REAL	_const;
};

//------------------------------------------------------------------------------------
// Axis aligned bounding box
class   NO_VT BBox
{
public:
    BBox():_min(INFINIT),_max(-INFINIT){}
    BBox(V3& m, V3& M){
        AddPoint(m);
        AddPoint(M);
    }
    BBox(const BBox& box):_min(box._min),_max(box._max){}
    void Reset(){_min =INFINIT;_max=-INFINIT;}
    BBox& operator=(const BBox& box){
        _min=box._min;
        _max=box._max;
        return *this;
    }
    BBox& operator+=(V3& pos){
        _min+=pos;
        _max+=pos;
        return *this;
    }
    BBox& operator+=(REAL val){
        _max+=val;
        _max+=-val;
        return *this;
    }
    BBox& operator*=(REAL val){
        _max*=val;
        _max*=val;
        return *this;
    }
    void Union(BBox& b){
        _max.x = max(_max.x, b._max.x);
        _max.y = max(_max.y, b._max.y);
        _max.z = max(_max.z, b._max.z);
        _min.x = min(_min.x, b._min.x);
        _min.y = min(_min.y, b._min.y);
        _min.z = min(_min.z, b._min.z);
    }
    void AddPoint(V3& a){
        _max.x = max(_max.x, a.x);
        _max.y = max(_max.y, a.y);
        _max.z = max(_max.z, a.z);
        _min.x = min(_min.x, a.x);
        _min.y = min(_min.y, a.y);
        _min.z = min(_min.z, a.z);
    }
    void FlatIt(UV* puv){           // lowest extend is nulled out
        REAL mx = _max.x-_min.x;
        REAL my = _max.y-_min.y;
        REAL mz = _max.z-_min.z;
        REAL me =  min(min(mx,my),mz);
        if(me == mx){
            _max.x = _min.x =0;
            puv[0] = UV(_min.z,_min.y);
            puv[1] = UV(_max.z,_max.y);
        }
        else if(me == my){
            _max.y = _min.y =0;
            puv[0] = UV(_min.x,_min.z);
            puv[1] = UV(_max.x,_max.z);
        }
        else if(me == mz){
            _max.z = _min.z =0;
            puv[0] = UV(_min.x,_min.y);
            puv[1] = UV(_max.x,_max.y);
        }
    }
    char GetMinimAx(){
        REAL mx = _max.x-_min.x;
        REAL my = _max.y-_min.y;
        REAL mz = _max.z-_min.z;
        REAL me =  min(min(mx,my),mz);
        if(me == mx){
            return 'x';
        }
        else if(me == my){
            return 'y';
        }
        else if(me == mz){
            return 'z';
        }
        return 0;
    }
    REAL GetMinExtend(){
        REAL mx = _max.x-_min.x;
        REAL my = _max.y-_min.y;
        REAL mz = _max.z-_min.z;
        return min(min(mx,my),mz);
    }
    REAL GetMaxExtend(){
        REAL mx = _max.x-_min.x;
        REAL my = _max.y-_min.y;
        REAL mz = _max.z-_min.z;
        return max(max(mx,my),mz);
    }
    V3 GetExtends(){
        return (_max-_min);
    }
    void Transform(M4& m){
        m.transform(_max);
        m.transform(_min);
    }
    V3  GetCenter(){
        return (_min + _max)/2;
    }
    BOOL ContainPoint(V3& pct){
        return  (pct.x>_min.x && pct.x<_max.x &&
                 pct.y>_min.y && pct.y<_max.y &&
                 pct.z>_min.z && pct.z<_max.z );
    }
    void Expand0(REAL ammnt = .1f){
        V3 ex = _max-_min;
        if(ex.x==0){
            _max.x+=ammnt;
            _min.x-=ammnt;
        }
        if(ex.y==0){
            _max.y+=ammnt;
            _min.y-=ammnt;
        }
        if(ex.z==0){
            _max.z+=ammnt;
            _min.z-=ammnt;
        }
    }
    void Expand(REAL val){
        _min-=val;
        _max+=val;
    }
    friend void ExpandBb(V3& m, V3& M, REAL val){
        m-=val;
        M+=val;
    }
    BOOL IsTouchesBox(const BBox& other){
        if(_max.x < other._min.x)
            return FALSE;
        if(_min.x > other._max.x)
            return FALSE;
        if(_max.y < other._min.y)
            return FALSE;
        if(_min.y > other._max.y)
            return FALSE;
        if(_max.z < other._min.z)
            return FALSE;
        if(_min.z > other._max.z)
            return FALSE;
        return TRUE;
    }
    // shifts the plane into the box with the effective radius
    REAL PlaneShift(Plane& plan){   
        REAL     r = plan._const;
        V3&	pn = plan._normal;
        if (pn.x > 0)
            plan._const -= pn.x * _min.x;
        else
            plan._const -= pn.x * _max.x;
        if (pn.y > 0)
            plan._const -= pn.y * _min.y;
        else
            plan._const -= pn.y * _max.y;
        if (pn.z > 0)
            plan._const -= pn.z * _min.z;
        else
            plan._const -= pn.z * _max.z;

        return r-plan._const;
    }

    BOOL PointInside(V3& pct){
        return ((pct.x >= _min.x) && (pct.x <= _max.x) &&
                (pct.y >= _min.y) && (pct.y <= _max.y) &&
                (pct.z >= _min.z) && (pct.z <= _max.z));
    }

    V3	 _min;
    V3	 _max;
};

//---------------------------------------------------------------------------------------
// Euler angles and associated forward, side and up vectors
class NO_VT OrtoPos
{
public:
    virtual ~OrtoPos(){}
    OrtoPos(REAL *pos){
        _wpos.setval(pos[0],pos[1],pos[2]);
        SetAngles(pos[3], pos[4], pos[5]);
    }
    OrtoPos(){
        SetZero();
    }

    void SetZero(){
        _pwpos.setval(V0);
        _wpos.setval(V0);
        _ldir.setval(-VZ);
        _lup.setval(-VFALL);
        _lside.setval(VX);
    }

    INLN void	SetAngles(REAL a, REAL e, REAL r){
        ROLLPI(a);
        BLOCKRANGE(e);
        _angles.y=a; _angles.x=e; _angles.z=r;
        AnglesToDir();
    }
    void MoveAndRot(REAL* pr){
        MoveFwd(pr[2]);
        MoveSide(pr[0]);
        MoveUp(pr[1]);
        Rotate( pr[3],pr[4], pr[5]);
    }
    void MoveAndRot(REAL f, REAL s, REAL u, REAL a, REAL e, REAL r){
        if(u)MoveUp(u);
        if(s)MoveSide(s);
        if(f)MoveFwd(f);
        Rotate( a, e, r);
    }
    void Rotate(REAL a, REAL e, REAL r);
    INLN 	void	MoveUp(REAL r){
        _wpos += r*_lup;
    }
    INLN 	void	MoveSide(REAL r){
        _wpos += r*_lside;
    }
    INLN 	void	MoveFwd(REAL r){
        _wpos += r*_ldir;
    }
    INLN 	M4&	MakeLookAtMatrix(){
        _trmat = MLook(_wpos, _ldir, _lup, _lside);
        return _trmat;
    }
    void  DirToAngles(BOOL toDeg);
    void  AnglesToDir();
    void  BuildTrMatrix();

    V3	    _angles;
    V3		_pwpos;
    V3		_wpos;
    V3		_ldir;
    V3		_lup;
    V3		_lside;
    M4		_trmat;

};

//---------------------------------------------------------------------------------------
// Returns intersection point 'ip' from segment a-b and distance from a to the plane in 
// 'p'
INLN BOOL SegIntersectPlane(V3& s, V3& e, V3& ponp, V3& pn, V3& ip, REAL& p)
{
    V3		dr  = e - s;
    REAL	ll  = VDp(dr, pn);

    if ( RABS( ll ) < EPSILON)
        return FALSE;

    V3		l1 = ponp - s;
    REAL	d2p = VDp(l1, pn);

    p	= d2p / ll;
    if (p < 0.0 || p > 1.0)
        return FALSE;
    ip = s + dr * p;
    return TRUE;
}

//---------------------------------------------------------------------------------------
INLN void    GetCornersOfPlane(V3& mins, V3&  maxs, V3& normal, V3* corners)
{
    for (REG int i=0 ; i<3 ; i++){
        if (normal[i] < 0){
            corners[0][i] = mins[i];
            corners[1][i] = maxs[i];
        }
        else{
            corners[1][i] = mins[i];
            corners[0][i] = maxs[i];
        }
    }
}
#endif // __BASEGEOM_H__
