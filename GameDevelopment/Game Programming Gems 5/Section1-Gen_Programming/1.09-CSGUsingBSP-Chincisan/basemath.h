/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/
#ifndef __BASEVMATH_H__
#define __BASEVMATH_H__

#include "baseos.h"

//---------------------------------------------------------------------------------------
// constants and values used by the editor
#define TRESH					1.f				                // minim world box dim
#define EPSILONBIG				0.0625F                         // power of 2 (uP=happy)
#define EPSILON					0.0009765625F
#define INFINIT					131072.F						// 1.3km power of 2 (uP=happy)
#define PI						3.14159265358900                // PI in radians    
#define DOIPI					6.28318530717900	            // 2*pi			
#define PIPE2					1.57079632679400                // pi/2
#define PIPE4					0.78539816339700                // pi/4
#define PIPE6					0.52359877559800                // pi/6
#define ONEDG                   0.0174532925199F                // 1 degree
#define G2R(a)					(a*0.0174532925F)               // Grades 2 Radians
#define R2G(a)					(a*57.295779513F)               // Radians 2  Grades 
#define ROLLPI(a)				{if(a < -(PI) ) a = (PI); else if (a > (PI)) a = -(PI);}
#define BLOCKRANGE(a)			if(a < - PIPE2-EPSILON) a =- (PIPE2-EPSILON); \
                                else if (a > PIPE2-EPSILON) a = PIPE2-EPSILON;  // blocks the range of angle to straight up/down
#define CLAMPVAL(_v_,m_,M_)     {if(_v_< m_) _v_= m_; else if (_v_> M_)  _v_=M_;}
#ifndef min
#define min(a_,b_)  (a_<b_ ? a_ : b_)
#define max(a_,b_)  (a_>=b_ ? a_ : b_)
#endif //

// returns the sinus of an angle
INLN REAL SIN(REAL rad){
    return (REAL)sin(rad);
}

// returns the cosinus of an angle
INLN REAL COS(REAL gr){
    return (REAL)cos(gr);
}

// Redefine SIN and COS postfixing with R (angle is in radians)
#define SINR    SIN
#define COSR    COS

// fast absolute value of a float.
INLN REAL RABS (REAL f){
    REG int i = *(int*) &f;
    i &= 0x7FFFFFFF;    // wipe out msb
    return *(REAL*)&i;
}

// Zero test of a floating point value
inline BOOL  ISZERO(REAL a , REAL b=EPSILONBIG){
    return ( a > -b && a < b );
}


// ---------------------------------------------------------------------------------------
// Vertex class. 
class NO_VT V3
{
public:
    REAL x, y, z;
public:
    V3():x(0.0),y(0.0),z(0.0) {};
    V3(REAL inX, REAL inY, REAL inZ): x(inX), y(inY), z(inZ) {}
    V3(const V3 &v): x(v.x), y(v.y), z(v.z) {}
    V3(const REAL f): x(f), y(f), z(f) {}
    V3(const REAL *f): x(*f), y(*(f+1)), z(*(f+2)) {}
    void RawPaste(REAL* pr){*pr =x; *(pr+1) = y;*(pr+2) = z;}
    void RawCopy(REAL* pr){x = *pr;y = *(pr+1);z = *(pr+2);}
    operator REAL*(){return &x;}
    INLN  V3& operator+=(const V3 &v){
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    INLN  V3& operator+=(const REAL r){
        x += r; y += r; z += r;
        return *this;
    }
    INLN  V3& operator-=(const V3 &v){
        x -= v.x ;y -= v.y; z -= v.z;
        return *this;
    }
    INLN  void negate(){
        x=-x; y=-y; z=-z;
    };
    INLN  V3& operator*=(REAL f) {
        x *= f;y *= f;z *= f;
        return *this;
    }
    INLN  V3& operator/=(REAL f) {
        x /= f;y /= f;z /= f;
        return *this;
    }
    REAL getmin(){
        return min((min(x,y)),z);
    }
    REAL getmax(){
        return max((max(x,y)),z);
    }
    INLN void domin(V3& v){
        x = min(x, v.x);
        y = min(y, v.y);
        z = min(z, v.z);
    }
    INLN  void domax(V3& v){
        x = max(x, v.x);
        y = max(y, v.y);
        z = max(z, v.z);
    }
    INLN  V3& operator/=(int f){
        x /= (REAL)f;
        y /= (REAL)f;
        z /= (REAL)f;
        return *this;
    }
    INLN  V3& operator*=(const V3& v){
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    INLN  friend BOOL operator==(const V3 &a, const V3 &b) {
        return(ISZERO(a.x-b.x) &&
               ISZERO(a.y-b.y) &&
               ISZERO(a.z-b.z));
    }
    INLN  friend BOOL operator!=(const V3 &a, const V3 &b) {
        return((a.x != b.x) || (a.y != b.y) || (a.z != b.z));
    }
    INLN  friend V3 operator-(const V3 &a){
        return V3(-a.x, -a.y, -a.z);
    }
    INLN  friend V3 operator+(const V3& a, const V3& b) {
        V3 ret(a);ret += b;return ret;
    }
    INLN  friend V3 operator-(const V3 &a, const V3 &b) {
        V3 ret(a); ret -= b; return ret;
    }
    INLN  friend V3 operator*(const V3 &v, REAL f) {
        return V3(f * v.x, f * v.y, f * v.z);
    }

    INLN  friend V3 operator*(REAL f, const V3 &v) {
        return V3(f * v.x, f * v.y, f * v.z);
    }
    INLN  friend V3 operator/(const V3 &v, REAL f) {
        return V3(v.x / f, v.y / f, v.z / f);
    }
    INLN  friend V3  operator/(const V3 &v, const V3 &f) {
        return V3(v.x / f.x, v.y / f.y, v.z / f.z);
    }
    INLN  friend V3  operator*(const V3 &v, const V3 &f) {
        return V3(v.x * f.x, v.y * f.y, v.z * f.z);
    }
    INLN  friend REAL vdist(V3& p1, V3& p2){
        V3 tmp(p1);
        tmp-=p2;
        return tmp.len();
    }
    INLN  friend REAL vdist2(V3& p1, V3& p2){
        V3 tmp(p1);
        tmp-=p2;
        return tmp.len2();
    }
    INLN  REAL vdp(V3& b){
        return x*b.x + y*b.y + z*b.z;
    }
    INLN  void setval(REAL xx, REAL yy, REAL zz) {
        x = xx; y = yy; z = zz;
    }
    INLN  void setval(REAL xx[3]) {
        x = xx[0]; y = xx[1]; z = xx[2];
    }
    INLN void squeeze(REAL val){
        if (x > -val && x < val)
            x = 0.00;
        if (y > -val && y < val)
            y = 0.00;
        if (z > -val && z < val)
            z = 0.00;
    }
    INLN  void setval(REAL xx) {
        x = xx; y = xx; z = xx;
    }
    INLN  BOOL Valid(){
        return fabs(x) != INFINIT;
    }
    INLN  void setval(V3& v) {
        x = v.x; y = v.y; z = v.z;
    }
    INLN void interpolate(V3& v1, V3& v2, REAL ammnt=.5f){
        x = v1.x+ (v2.x-v1.x)*ammnt;
        y = v1.y+ (v2.y-v1.y)*ammnt;
        z = v1.z+ (v2.z-v1.z)*ammnt;
    }
    INLN  REAL len() const {
        return(REAL) sqrt(x*x + y*y + z*z);
    }
    INLN  REAL len2() const{
        return(x*x + y*y + z*z);
    }
    BOOL is0(REAL eps=EPSILON) const{
        return((fabs(x) < eps) && (fabs(y) < eps) && (fabs(z) < eps));
    }
    BOOL	isnot0(){
        return (x!=0 || y!=0 || z!=0);
    }
    BOOL	isnot1(){
        return (x!=1 || y!=1 || z!=1);
    }
    INLN  V3 &norm(){
        REAL m = len();
        if (m > 0.0)
            m = 1.0 / m;
        else m = 0.0;
        x *= m;
        y *= m;
        z *= m;
        return *this;
    }
};

// most used values
extern V3 VX;
extern V3 VZ;
extern V3 VY;
extern V3 V0;
extern V3 V1;
extern V3 V22;
extern V3 V44;
extern V3 V88;
extern V3 VXY;
extern V3 VXZ;
extern V3 VYZ;
extern V3 VFALL;

//----------------------------------------------------------------------------V4
// Vertex of 4 coordinates (x,y,z,w)
class NO_VT V4
{
public:
    REAL x, y, z, w;
public:
    V4() {};
    V4(REAL inX, REAL inY, REAL inZ, REAL inW): x(inX), y(inY), z(inZ), w(inW) {};
    V4(const V4 &v): x(v.x), y(v.y), z(v.z), w(v.w) {};
    explicit V4(const V3 &v): x(v.x), y(v.y), z(v.z), w(1.00) {};
    V4(const REAL f): x(f), y(f), z(f),w(f) {}
    V4(const REAL *f): x(*f), y(*(f+1)), z(*(f+2)),w(*(f+3)) {}
    INLN  REAL  &operator[](unsigned int i) {
        return(i == 0) ? x : (i == 1) ? y : (i == 2) ? z : w;
    }
    INLN  const REAL& operator[](unsigned int i) const {
        return(i == 0) ? x : (i == 1) ? y : (i == 2) ? z : w;
    }
    INLN  V4& operator =  (const V3 &v) {
        x = v.x;  y = v.y;  z = v.z;  w = 0.00;
        return *this;
    }
    INLN  V4& operator += (const V4 &v) {
        x += v.x;  y += v.y;  z += v.z;  w += v.w;
        return *this;
    }
    INLN  V4& operator -= (const V4 &v) {
        x -= v.x;  y -= v.y;  z -= v.z;  w -= v.w;
        return *this;
    }
    INLN  V4& operator *= (REAL f) {
        x *= f;  y *= f;  z *= f;  w *= f;
        return *this;
    }
    INLN  V4& operator /= (REAL f) {
        x /= f;  y /= f;  z /= f;  w /= f;
        return *this;
    }
    INLN  friend BOOL operator == (const V4 &a, const V4 &b) {
        return((a.x == b.x) && (a.y == b.y) &&
               (a.z == b.z) && (a.w == b.w));
    }
    INLN  friend BOOL operator != (const V4 &a, const V4 &b) {
        return((a.x != b.x) || (a.y != b.y) ||
               (a.z != b.z) || (a.w != b.w));
    }
    INLN  friend V4 operator - (const V4 &a) {
        return V4(-a.x, -a.y, -a.z, -a.w);
    }
    INLN  friend V4 operator + (const V4 &a, const V4 &b) {
        V4 ret(a);  ret += b;  return ret;
    }
    INLN  friend V4 operator - (const V4 &a, const V4 &b) {
        V4 ret(a);        ret -= b;        return ret;
    }
    INLN  friend V4 operator * (const V4 &v, REAL f) {
        return V4(f * v.x, f * v.y, f * v.z, f * v.w);
    }
    INLN  friend V4 operator * (REAL f, const V4 &v) {
        return V4(f * v.x, f * v.y, f * v.z, f * v.w);
    }
    INLN  friend V4 operator / (const V4 &v, REAL f) {
        return V4(v.x / f, v.y / f, v.z / f, v.w / f);
    }
    INLN  void setval(REAL xIn, REAL yIn, REAL zIn, REAL wIn) {
        x = xIn; y = yIn; z = zIn; w = wIn;
    }
    INLN  REAL len() const {
        return(REAL) sqrt(x*x + y*y + z*z + w*w);
    }
    INLN  REAL    len2() const {
        return(x*x + y*y + z*z + w*w);
    }
    INLN  BOOL     is0() const {
        return((x == 0.00) && (y == 0.00) && (z == 0.00) && (w == 0.00));
    }
    INLN  V4  &norm() {
        REAL m = len();
        if (m > 0.00)m = 1.00 / m;
        else m = 0.00;
        x *= m; y *= m; z *= m;
        w *= m;
        return *this;
    }
};

//------------------------------------------------------------------------------------
// Dot product of V4's
INLN  REAL V4Dp(const V4 &a, const V4 &b){
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}
//------------------------------------------------------------------------------------
// Normalises the input vector and returns the vector normalised
INLN  V3& VNorm(const V3 &a)
{
    static V3 ret;
    ret.setval(a.x,a.y,a.z);
    return ret.norm();
}

// Returns the dot product between a and b
INLN  REAL VDp(const V3 &a, const V3 &b)
{
    return (a.x*b.x + a.y*b.y + a.z*b.z);
}

// Swaps 2 vectors
INLN  void VSwp(V3 &a, V3 &b)
{
    V3 tmp(a);    a = b;    b = tmp;
}

// Cross product
INLN  V3& VCp(const V3 &a, const V3 &b)
{
    static V3 v;
    v.setval(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
    return v;
}

// Are the vectors close to each other. 
INLN  BOOL VNear(const V3 &a, const V3 &b, REAL dist)
{
    static V3 diff;
    diff.setval(a.x,a.y,a.z);
    diff-=b;
    return(VDp(diff, diff) < dist*dist);
}

// Test if the vector is nearby 0,0,0
INLN  BOOL IsZero(const V3 &a,REAL e = EPSILONBIG)
{
    if(!ISZERO(a.x),e)return FALSE;
    if(!ISZERO(a.y),e)return FALSE;
    if(!ISZERO(a.y),e)return FALSE;
    return TRUE;
}

//------------------------------------------------------------------------------------
// Matrix implementation
class NO_VT M4
{
public:
    V4   m_cl[4];
    M4() {
        m_cl[0].setval(1.0, 0.0, 0.0, 0.0); m_cl[1].setval(0.0, 1.0, 0.0, 0.0);
        m_cl[2].setval(0.0, 0.0, 1.0, 0.0); m_cl[3].setval(0.0, 0.0, 0.0, 1.0);
    };
    M4(REAL v) {
        m_cl[0].setval(v, v, v, v); m_cl[1].setval(v, v, v, v);
        m_cl[2].setval(v, v, v, v); m_cl[3].setval(v, v, v, v);
    }
    INLN  V4& operator [] (unsigned int i) {
        return m_cl[i];
    }
    INLN  const V4& operator [] (unsigned int i) const {
        return m_cl[i];
    }
    INLN  M4(const M4 &m) {
        m_cl[0] = m[0]; m_cl[1] = m[1]; m_cl[2] = m[2]; m_cl[3] = m[3];
    }
    INLN  M4& operator=(const M4 &m) {
        m_cl[0] = m[0]; m_cl[1] = m[1]; m_cl[2] = m[2];m_cl[3] = m[3];
        return *this;
    }
    INLN  M4& operator+=(const M4 &m) {
        m_cl[0] += m[0]; m_cl[1] += m[1]; m_cl[2] += m[2]; m_cl[3] += m[3];
        return *this;
    }
    INLN  M4& operator-=(const M4 &m) {
        m_cl[0] -= m[0]; m_cl[1] -= m[1]; m_cl[2] -= m[2]; m_cl[3] -= m[3];
        return *this;
    }
    INLN  friend V4 operator * (const M4 &m, const V4 &v) {
        V4 ret;
        ret.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
        ret.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
        ret.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
        ret.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
        return ret;
    }
    INLN  friend V4 operator * (const V4 &v, const M4 &m) {
        V4 ret;
        ret.x = V4Dp(m[0], v); ret.y = V4Dp(m[1], v);
        ret.z = V4Dp(m[2], v); ret.w = V4Dp(m[3], v);
        return ret;
    }
    INLN void transposeTransform(V3& i){
        //// ERROR
        i.x -= m_cl[3][0];
        i.y -= m_cl[3][1];
        i.z -= m_cl[3][2];
        V3 r;
        r.x = i.x * m_cl[0][0] + i.y * m_cl[0][1] + i.z * m_cl[0][2] ;
        r.y = i.x * m_cl[1][0] + i.y * m_cl[1][1] + i.z * m_cl[1][2] ;
        r.z = i.x * m_cl[2][0] + i.y * m_cl[2][1] + i.z * m_cl[2][2] ;
        i=r;
    }
    INLN void transform(V3& v){
        V3 r;
        r.x = v.x * m_cl[0][0] + v.y * m_cl[1][0] + v.z * m_cl[2][0] + m_cl[3][0];
        r.y = v.x * m_cl[0][1] + v.y * m_cl[1][1] + v.z * m_cl[2][1] + m_cl[3][1];
        r.z = v.x * m_cl[0][2] + v.y * m_cl[1][2] + v.z * m_cl[2][2] + m_cl[3][2];
        v=r;
    }
    INLN void rotateonly(V3& v){
        V3 r;
        r.x = v.x * m_cl[0][0] + v.y * m_cl[1][0] + v.z * m_cl[2][0] ;
        r.y = v.x * m_cl[0][1] + v.y * m_cl[1][1] + v.z * m_cl[2][1] ;
        r.z = v.x * m_cl[0][2] + v.y * m_cl[1][2] + v.z * m_cl[2][2] ;
        v=r;
    }
    INLN void translateonly(V3& ret){
        ret.x = ret.x + m_cl[3][0];
        ret.y = ret.y + m_cl[3][1];
        ret.z = ret.z + m_cl[3][2];
    }
    INLN void scaleeonly(V3& ret){
        ret.x = ret.x * m_cl[0][0];
        ret.y = ret.y * m_cl[1][1];
        ret.z = ret.z * m_cl[2][2];
    }
    INLN  M4& operator*=(const M4 &m){
        M4 t;
        for (unsigned int l = 0; l < 4; l++){
            for (unsigned int c = 0; c < 4; c++){
                REAL f = 0;
                f += (m_cl[0][l] * m[c][0]); f += (m_cl[1][l] * m[c][1]);
                f += (m_cl[2][l] * m[c][2]); f += (m_cl[3][l] * m[c][3]);
                t[c][l] = f;
            }
        }
        *this = t;
        return *this;
    }
    INLN  M4 &operator*=(REAL f) {
        m_cl[0] *= f; m_cl[1] *= f; m_cl[2] *= f; m_cl[3] *= f;
        return *this;
    }
    INLN  friend BOOL operator==(const M4 &a, const M4 &b) {
        return((a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]) && (a[3] == b[3]));
    }
    INLN  friend BOOL operator!=(const M4 &a, const M4 &b) {
        return((a[0] != b[0]) || (a[1] != b[1]) || (a[2] != b[2]) || (a[3] != b[3]));
    }
    INLN  friend M4 operator+(const M4 &a, const M4 &b) {
        M4 ret(a); ret+=b; return ret;
    }
    INLN  friend M4   operator - (const M4 &a, const M4 &b) {
        M4 ret(a); ret -= b; return ret;
    }
    INLN  friend M4   operator * (const M4 &a, const M4 &b) {
        M4 ret(a); ret *= b; return ret;
    }
    INLN  friend V3    operator * (const M4 &m, const V3 &v) {
        V4 ret(v);
        ret = m * ret;
        return V3(ret.x, ret.y, ret.z);
    }
    INLN  friend V3    operator * (const V3 &v, const M4 &m) {
        V4 ret(v);
        ret = ret * m;
        return V3(ret.x, ret.y, ret.z);
    }
    INLN  friend M4   operator * (const M4 &m, REAL f) {
        M4 ret(m); ret *= f; return ret;
    }
    INLN  friend M4   operator * (REAL f, const M4 &m) {
        M4 ret(m);ret *= f; return ret;
    }
    INLN  M4& identity() {
        m_cl[0].setval(1.0, 0.0, 0.0, 0.0);
        m_cl[1].setval(0.0, 1.0, 0.0, 0.0);
        m_cl[2].setval(0.0, 0.0, 1.0, 0.0);
        m_cl[3].setval(0.0, 0.0, 0.0, 1.0);
        return *this;
    }
    INLN  M4& transp(){
        REAL t;
        for (unsigned int c = 0; c < 4; c++){
            for (unsigned int r = c + 1; r < 4; r++){
                t = m_cl[c][r];
                m_cl[c][r] = m_cl[r][c];
                m_cl[r][c] = t;
            }
        }
        return *this;
    }
    M4& infval();
    M4& invert();
};


// Returns identity matrix
INLN  M4 MTo1()
{
    M4 ret;
    return ret.identity();
}

// Returns transposed 'm' matrix
INLN  M4 MTranspose(const M4 &m)
{
    M4 ret(m);
    return ret.transp();
}
// Returns inverse of 'm' matrix
INLN  M4 MInvf(const M4 &m)
{
    M4 ret(m); return ret.invert();
}

// Matrix tranformations (Rotation, Rotation around a point and Sclation)
M4 MRotate(REAL radx, REAL rady, REAL radz);    // y is first most use
M4 MgetRotMx(const V3 &axis, REAL rad);
INLN  M4 MTranslate(REAL x, REAL y, REAL z){
    M4 mret;
    mret[3][0] = x; mret[3][1] = y; mret[3][2] = z;
    return mret;
}
INLN  M4 MScale(REAL x, REAL y, REAL z, REAL w = 1.0)
{
    M4 mret;
    mret[0][0] = x; mret[1][1] = y; mret[2][2] = z; mret[3][3] = w;
    return mret;
}
M4    MLook(V3& a, V3& b, V3& c,V3& d);

// Calculates a normal from 3 points. Returns the normal
INLN  V3& normal(const V3& p0,const  V3& p1,const  V3& p2){
    static V3 v;
    v = VCp(p0-p1, p2-p1);
    v.norm();
    return v;
}

//------------------------------------------------------------------------------------
// Color structure. 
class NO_VT CLR
{
public:
    BYTE r,g,b,a;
    CLR():r(0),g(0),b(0),a(255){};
    CLR(BYTE rgb):r(rgb),g(rgb),b(rgb),a(rgb){};
    CLR(const CLR& rgb):r(rgb.r),g(rgb.g),b(rgb.b),a(rgb.a){
    }
    CLR(BYTE rr, BYTE gg, BYTE bb, BYTE aa){
        r = rr;    g = gg;    b = bb;    a = aa;
    }
    CLR(BYTE rr, BYTE gg, BYTE bb){
        r = rr;    g = gg;    b = bb;    a = 255;
    }
    INLN  operator BYTE*(){
        return (BYTE*)&r;
    }
    INLN CLR& operator =(const CLR& rr){
        r = rr.r;    g = rr.g;    b = rr.b;    a = rr.a;
        return *this;
    }
    INLN  CLR& operator -=(CLR& rgb){
        r-=rgb.r;
        g-=rgb.g;
        b-=rgb.b;
        return *this;
    }
    BOOL  isnot0(){
        if(r!=0)
            return TRUE;
        if(g!=0)
            return TRUE;
        if(b!=0)
            return TRUE;
        return FALSE;
    }
    INLN  CLR& operator +=(CLR& rgb){
        r+=rgb.r;
        g+=rgb.g;
        b+=rgb.b;
        return *this;
    }
    INLN  CLR& operator +=(BYTE rgb){
        r+=rgb;
        g+=rgb;
        b+=rgb;
        return *this;
    }
    INLN  CLR& operator *=(CLR& rgb){
        r*=rgb.r;
        g*=rgb.g;
        b*=rgb.b;
        return *this;
    }
    INLN  void setval(BYTE rr, BYTE gg, BYTE bb){
        r=rr;g=gg;b=bb;a=255;
    }
    INLN  int Sum(){return (r+g+b);}
    INLN  CLR& operator *=(BYTE f) { // can overflow
        r *= f;
        g *= f;
        b *= f;
        return *this;
    }
    INLN  CLR friend operator-(CLR& a, CLR& b ){
        CLR vr(a);
        vr-=b;
        return vr;
    }
    INLN  CLR friend operator+(CLR& a, CLR& b ){
        CLR vr(a);
        vr+=b;
        return vr;
    }
    INLN  friend CLR operator / (const CLR& v, BYTE f) {
        return CLR(v.r / f, v.g / f, v.b / f);
    }
    INLN void operator =(BYTE f){
        r = f; g = f;  b = f;  a = f;
    }
    INLN BOOL operator !=(CLR& vrgb){
        return (r!=vrgb.r || g!=vrgb.g || b!=vrgb.b || a!=vrgb.a);
    }
    INLN  void interpolate(CLR& c1, CLR& c2, REAL ammnt=.5f)
    {
        r=c1.r;
        g=c1.g;
        b=c1.b;
        a=c1.a;
        return;
        REAL ir = c1.r + (c2.r-c1.r) * ammnt;
        REAL ig = c1.g + (c2.g-c1.g) * ammnt;
        REAL ib = c1.b + (c2.b-c1.b) * ammnt;
        if (ir>255.f)ir=255.f;
        if (ig>255.f)ig=255.f;
        if (ib>255.f)ib=255.f;
        r=(BYTE)ir;
        g=(BYTE)ig;
        b=(BYTE)ib;
        a = c1.a;
    }
    INLN  void ClampUp(CLR& cc){
        if (r>cc.r)r=cc.r;
        if (g>cc.g)g=cc.g;
        if (b>cc.b)b=cc.b;
    }
    INLN  void Scale(CLR& clr){
        r/=clr.r;
        g/=clr.g;
        b/=clr.b;
    }
public:
};

//------------------------------------------------------------------------------------
// Predefind color values
extern CLR ZZBLACK;
extern CLR ZDARKZZRED;
extern CLR ZDARKZZGREEN;
extern CLR ZDARKZZYELLOW;
extern CLR DARKZZBLUE;
extern CLR ZDARKZZMAGENTA;
extern CLR DARKZZCYAN;
extern CLR ZZGREY;
extern CLR ZZLIGHTGRAY;
extern CLR MONEYZZGREEN;
extern CLR SKYZZBLUE;
extern CLR ZZCREAM;
extern CLR ZZMEDIUMGRAY;
extern CLR ZZRED;
extern CLR ZZGREEN;
extern CLR ZLIGHTZZYELLOW;
extern CLR ZZYELLOW;
extern CLR ZZBLUE;
extern CLR ZZMAGENTA;
extern CLR ZZCYAN;
extern CLR ZZWHITE;
extern CLR ZZNONE;

//------------------------------------------------------------------------------------
// 2d Coordinates
struct UV
{
    REAL u,v;
    UV():u(0.00),v(0.00) {};
    UV(REAL inX, REAL inY): u(inX), v(inY){}
    UV(const UV &v): u(v.u), v(v.v) {}
    UV(const REAL f): u(f), v(f) {}
    UV(const REAL *f): u(*f), v(*(f+1)) {}
    INLN  operator REAL*(){return (REAL*)&u;}
    INLN  UV& operator += (const UV &ufval) {
        u += ufval.u; v += ufval.v;
        return *this;
    }
    INLN  UV& operator -= (const UV &ufval) {
        u -= ufval.u; v -= ufval.v;
        return *this;
    }
    INLN  UV& operator *= (REAL f) {
        u *= f; v *= f;
        return *this;
    }
    INLN  UV& operator *= (UV f) {
        u *= f.u; v *= f.v;
        return *this;
    }
    INLN  UV& operator /= (REAL f) {
        u /= f; v /= f;
        return *this;
    }
    INLN  UV& operator /= (UV& uv) {
        u /= uv.u; v /= uv.v;
        return *this;
    }
    INLN  void setval(REAL xx) {
        u = xx; v = xx;
    }
    INLN  void setval(REAL uu, REAL vv) {
        u = uu; v = vv;
    }
    INLN  void setval(UV& xx) {
        u = xx.u; v = xx.v;
    }
    INLN  void oneover() {
        u = 1.00/u; v = 1.00/v;
    }
    INLN  void domin(UV& t){
        u = min(u,t.u);
        v = min(v,t.v);
    }
    INLN void domax(UV& t){
        u = max(u,t.u);
        v = max(v,t.v);
    }
    INLN void interpolate(UV& t1, UV& t2, REAL ammnt=.5f){
        u = t1.u + (t2.u-t1.u)*ammnt;
        v = t1.v + (t2.v-t1.v)*ammnt;
    }
    INLN  friend BOOL operator == (const UV &a, const UV &b) {
        return((a.u == b.u) && (a.v == b.v));
    }
    INLN  friend BOOL operator != (const UV &a, const UV &b) {
        return((a.u != b.u) || (a.v != b.v));
    }
    INLN  friend UV  operator - (const UV &a) {
        return UV(-a.u, -a.v);
    }
    INLN  friend UV  operator + (const UV &a, const UV &b) {
        UV ret(a);
        ret += b;
        return ret;
    }
    INLN  friend UV  operator - (const UV &a, const UV &b) {
        UV ret(a);
        ret -= b;
        return ret;
    }
    INLN  friend UV operator * (const UV &v, REAL f) {
        return UV(f * v.u, f * v.v);
    }
    INLN  friend UV operator * (REAL f, const UV &v) {
        return UV(f * v.u, f * v.v);
    }
    INLN  friend UV operator / (const UV &v, REAL f) {
        return UV(v.u / f, v.v / f);
    }
    INLN  friend UV  operator / (const UV &v, const UV &f) {
        return UV(v.u / f.u, v.v / f.v);
    }
    INLN  friend UV  operator * (const UV &v, const UV &f) {
        return UV(v.u * f.u, v.v * f.v);
    }
};

// Predefined Values
extern UV	UV00;
extern UV	UV10;
extern UV	UV11;
extern UV	UV01;

//------------------------------------------------------------------------------------
// Minimal vertex data. holds a 3d point, 3 textures coordinates and a color
struct VTCI	// V-vertex T-texture C-color I-texture2(lum map usually)
{
    VTCI():_cmd(0){}
    VTCI(const V3& pct,const UV& ufval){
        _xyz	= pct;
        _uv[0]	= ufval;
        _rgb	= ZZNONE;
        _cmd	= 0;
    }
    VTCI(const V3& pct, const UV& ufval,const  CLR& rgb, BYTE c ){
        _xyz	= pct;
        _uv[0]	= ufval;
        _rgb	= rgb;
        _cmd	= c;
    }
    VTCI(const VTCI& r){
        _xyz = r._xyz; _uv[0] = r._uv[0]; _uv[1] = r._uv[1]; _rgb = r._rgb;_cmd = r._cmd;
    }
    INLN  VTCI& operator = (const VTCI& r){
        _xyz = r._xyz; _uv[0] = r._uv[0]; _uv[1] = r._uv[1]; _rgb = r._rgb;
        _cmd = r._cmd;
        return *this;
    }
    void interpolate(VTCI& v1, VTCI& v2, REAL ammnt){
        _cmd	= v1._cmd;
        _xyz.interpolate(v1._xyz, v2._xyz, ammnt);
        _uv[0].interpolate(v1._uv[0],v2._uv[0],ammnt);
        _uv[1].interpolate(v1._uv[1],v2._uv[1],ammnt);
        _rgb = v1._rgb;
    }
    BOOL operator ==(const VTCI& r){
        return (_xyz	== r._xyz &&
                _uv[0]	== r._uv[0] &&
                _rgb.r	== _rgb.r &&
                _rgb.g	== _rgb.g &&
                _rgb.b	== _rgb.b);
    }
    int StrSize(){return sizeof(*this);}

    BYTE _cmd;	 // 1
    V3  _xyz;    // 24
    UV  _uv[2];	 // 32
    CLR _rgb;	 // 32
};

INLN  CLR AnyColor(){
    CLR clr;
    return clr;
}
// for gcc compiler

#ifdef MS_WIN32
    #define _V3   V3
    #define _UV   UV
    #define _CLR  CLR    
    #define _VTCI VTCI
#else
    INLN V3&   _V3(REAL x, REAL y, REAL z)   {static V3  v;  v.setval(x,y,z);  return v;}
    INLN UV&   _UV(REAL u, REAL v)           {static UV  t;  t.setval(u,v);    return t;}
    INLN CLR&  _CLR(BYTE r, BYTE g, BYTE b)  {static CLR c;  c.setval(r,g,b); return c;}
#endif //MG_WIN32

// Wrappers from the time for_each was not supported by MS
#define FOREACH(_xtype, _coll, it_begin)    for(_xtype::iterator it_begin = _coll.begin();it_begin!=_coll.end();it_begin++)
#define _FOREACH(_xtype, _coll, it_begin)    for(it_begin = _coll.begin();it_begin!=_coll.end();it_begin++)

#endif //__BASEVMATH_H__



