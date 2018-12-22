/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/
#include "stdafx.h"
#include "basemath.h"
#include "basegeom.h"

//----------------------------------------------------------------------------------------
// Build a rotation matrix rotated around x, y and z by radx, rady and radz radians.
// Returns the matrix
M4 MRotate(REAL radx, REAL rady, REAL radz)
{
    REAL sinr, cosr;
    M4 mret;
    M4 mry, mrx, mrz;
    if(radz != 0.0){
        sinr = (REAL)SIN(radz);
        cosr = (REAL)COS(radz);
        mret[0][0] =  cosr; mret[1][0] = -sinr; mret[2][0] =  0.0;
        mret[0][1] =  sinr; mret[1][1] =  cosr; mret[2][1] =  0.0;
        mret*=mrz;
    }
    if(rady != 0.0){
        sinr = (REAL)SIN(rady);
        cosr = (REAL)COS(rady);
        mry[0][0] =  cosr;  mry[2][0] =  sinr;
        mry[0][2] = -sinr;  mry[2][2] =  cosr;
        mret*=mry;
    }
    if(radx != 0.0){
        sinr = (REAL)SIN(radx);
        cosr = (REAL)COS(radx);
        mrx[1][1] =  cosr;  mrx[2][1] = -sinr;
        mrx[1][2] =  sinr;  mrx[2][2] =  cosr;
        mret*=mrx;
    }
    return mret;
}

//----------------------------------------------------------------------------------------
// Builds a rotation matrix rotated over 'oxyz' by 'rad' angle
// Returns the matrix
M4 MgetRotMx(const V3 &oxyz, REAL rad)
{
    V3 nrm = oxyz;
    nrm.norm();
    REAL sinr = (REAL)SIN(rad);
    REAL cosr = (REAL)COS(rad);
    REAL invcosr = 1.0 - cosr;
    REAL x = nrm.x;
    REAL y = nrm.y;
    REAL z = nrm.z;
    REAL xSq = x * x;
    REAL ySq = y * y;
    REAL zSq = z * z;
    M4      mret;
    mret[0][0] = (invcosr * xSq) + (cosr);
    mret[1][0] = (invcosr * x * y) - (sinr * z );
    mret[2][0] = (invcosr * x * z) + (sinr * y );
    mret[0][1] = (invcosr * x * y) + (sinr * z);
    mret[1][1] = (invcosr * ySq) + (cosr);
    mret[2][1] = (invcosr * y * z) - (sinr * x);
    mret[0][2] = (invcosr * x * z) - (sinr * y);
    mret[1][2] = (invcosr * y * z) + (sinr * x);
    mret[2][2] = (invcosr * zSq) + (cosr);
    return mret;
}

//----------------------------------------------------------------------------------------
// Builds a look-at matrix from forward, side and up vectors
// Returns the matrix
M4 MLook(V3& eye, V3& fwd, V3& up, V3& side)
{
    M4 mret;
    mret[0][0] = side.x;    mret[1][0] = side.y;    mret[2][0] = side.z;   mret[3][0] = 0.0;
    mret[0][1] = up.x;      mret[1][1] = up.y;      mret[2][1] = up.z;     mret[3][1] = 0.0;
    mret[0][2] = -fwd.x;    mret[1][2] = -fwd.y;    mret[2][2] = -fwd.z;   mret[3][2] = 0.0;
    mret[0][3] = 0.0;      mret[1][3] = 0.0;      mret[2][3] = 0.0;     mret[3][3] = 1.0;
    mret *= MTranslate(-eye.x, -eye.y, -eye.z);
    return mret;
}

//---------------------------------------------------------------------------------------
// Rotates the OrtoPos object.
void	OrtoPos::Rotate(REAL a, REAL e, REAL r)
{
    _angles.y += a;
    _angles.x -= e;
    _angles.z += r;
    BLOCKRANGE(_angles.x);
    AnglesToDir();
}

//---------------------------------------------------------------------------------------
// Transforms the forward, side and up vectors in euler angles. 
// (a quad would be more appropriate)
void OrtoPos::DirToAngles(BOOL toDeg)
{
    REAL dy2 = (REAL)sqrt(1-_ldir.y*_ldir.y);
    _angles.x = (REAL)asin(_ldir.y);
    _angles.z = (REAL)acos(_lup.y  / dy2);
    _angles.y = (REAL)asin(_ldir.x / dy2);
    ROLLPI(_angles.y);
}

//---------------------------------------------------------------------------------------
// The other way around
// (a quad would be more appropriate)
void OrtoPos::AnglesToDir()
{
    REAL CosAz = COS(_angles.y);
    REAL CosEl = COS(_angles.x);
    REAL SinAz = SIN(_angles.y);
    REAL SinEl = SIN(_angles.x);
    REAL CosRl = COS(_angles.z);
    REAL SinRl = SIN(_angles.z);
    _ldir.x = SinAz * CosEl;
    _ldir.y = SinEl;
    _ldir.z = CosEl * -CosAz;
    _lup.x = -CosAz * SinRl - SinAz * SinEl * CosRl;
    _lup.y = CosEl * CosRl;
    _lup.z = -SinAz * SinRl - SinEl * CosRl * -CosAz;
    _lside = VCp(_ldir, _lup);
}

//----------------------------------------------------------------------------------------
// predefind color values
CLR ZZBLACK(0, 0, 0);
CLR ZDARKZZRED (128,0,0);
CLR ZDARKZZGREEN (0,128,0);
CLR ZDARKZZYELLOW(128,128,0);
CLR ZLIGHTZZYELLOW(157,157,0);
CLR DARKZZBLUE(0,0,128);
CLR ZDARKZZMAGENTA(128,128,0);
CLR DARKZZCYAN(0,128,128);
CLR ZZLIGHTGRAY(192,192,192);
CLR ZZGREY(128,128,128);
CLR MONEYZZGREEN(191,216,192);
CLR SKYZZBLUE(140,198,250);
CLR ZZCREAM(10,245,245);
CLR ZZMEDIUMGRAY(128,128,128);
CLR ZZRED(255,0,0);
CLR ZZGREEN(0,255,0);
CLR ZZYELLOW(255,255,0);
CLR ZZBLUE(0,0,255);
CLR ZZMAGENTA(255,0,255);
CLR ZZCYAN(0,255,255);
CLR ZZWHITE(255,255,255);
CLR ZZNONE(0, 0, 0,0);

// predefind vertex values
V3 VFALL(0.0,-1.0,0.0);
V3 VX(0.0,1.0,0.0);
V3 VZ(0.0,0.0,1.0);
V3 VY(0.0,1.0,0.0);
V3 V0(0.0,0.0,0.0);
V3 V1(1.0,1.0,1.0);
V3 V22(2.f,2.f,2.f);
V3 V44(4.f,4.f,4.f);
V3 V88(8.f,8.f,8.f);
V3 VXY(1.0,1.0,0.0);
V3 VXZ(1.0,0.0,1.0);
V3 VYZ(0.0,1.0,1.0);

// predefind texture coordinates values
UV	UV00(0.0,0.0);
UV	UV10(1.0,0.0);
UV	UV11(1.0,1.0);
UV	UV01(0.0,1.0);
