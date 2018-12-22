/*
*/
#include <math.h>
#include "Obstruct.h"



#define LEFT   0x20
#define RIGHT  0x10
#define BOTTOM 0x08
#define TOP    0x04
#define FRONT  0x02
#define BACK   0x01




/*
*/
void NormalizeVector(SVector &svVec)
{
    float fLength = (float)sqrt(svVec.fX * svVec.fX + svVec.fY * svVec.fY + svVec.fZ * svVec.fZ);
    if ( fLength )
        fLength = 1 / fLength;
    svVec.fX *= fLength;
    svVec.fY *= fLength;
    svVec.fZ *= fLength;
}




/*
*/
void FindPlaneIntersectionPoint(SVector &svStart, 
                                SVector &svEnd, 
                                SVector &svPlanePoint, 
                                SVector &svPlaneNormal,
                                SVector *psvIntersectionPoint)
{
    SVector svNumerator, svDenominator;
    svNumerator.fX = svStart.fX - svPlanePoint.fX;
    svNumerator.fY = svStart.fY - svPlanePoint.fY;
    svNumerator.fZ = svStart.fZ - svPlanePoint.fZ;

    svDenominator.fX = svEnd.fX - svStart.fX;
    svDenominator.fY = svEnd.fY - svStart.fY;
    svDenominator.fZ = svEnd.fZ - svStart.fZ;

    float fNumerator = svNumerator.fX * svPlaneNormal.fX +
                       svNumerator.fY * svPlaneNormal.fY +
                       svNumerator.fZ * svPlaneNormal.fZ;
    float fDenominator = svDenominator.fX * svPlaneNormal.fX +
                         svDenominator.fY * svPlaneNormal.fY +
                         svDenominator.fZ * svPlaneNormal.fZ;

    float fT = (fNumerator / -fDenominator);
    psvIntersectionPoint->fX = fT * svEnd.fX + (1 - fT) * svStart.fX;
    psvIntersectionPoint->fY = fT * svEnd.fY + (1 - fT) * svStart.fY;
    psvIntersectionPoint->fZ = fT * svEnd.fZ + (1 - fT) * svStart.fZ;
}




/*
*/
bool PointInCubePlane(SVector &svPoint, 
                      long lPlaneType, 
                      SVector &svMin, 
                      SVector &svMax)
{
    float fPointX, fPointY, fMinX, fMaxX, fMinY, fMaxY;
    switch ( lPlaneType )
    {
    case FRONT:
    case BACK:
        fMinX = svMin.fX;
        fMaxX = svMax.fX;
        fMinY = svMin.fY;
        fMaxY = svMax.fY;
        fPointX = svPoint.fX;
        fPointY = svPoint.fY;
        break;

    case LEFT:
    case RIGHT:
        fMinX = svMin.fZ;
        fMaxX = svMax.fZ;
        fMinY = svMin.fY;
        fMaxY = svMax.fY;
        fPointX = svPoint.fZ;
        fPointY = svPoint.fY;
        break;

    case TOP:
    case BOTTOM:
        fMinX = svMin.fX;
        fMaxX = svMax.fX;
        fMinY = svMin.fZ;
        fMaxY = svMax.fZ;
        fPointX = svPoint.fX;
        fPointY = svPoint.fZ;
        break;

    default:
        return false;
    }

    if ( fPointX > fMaxX || fPointX < fMinX ||
         fPointY > fMaxY || fPointY < fMinY )
         return false;

    return true;
}




/*
*/
float CalcDiffractionLevel(SVector &svListener,
                           SVector &svSource,
                           SVector *psvPoint, 
                           long lPlaneType, 
                           SVector &svMin, 
                           SVector &svMax)
{
    SVector svPoint = *psvPoint;
    float fHeight, fWidth;
    switch ( lPlaneType )
    {
    case FRONT:
    case BACK:
        fHeight = svMax.fY - svPoint.fY;
        fWidth = svMax.fX - svPoint.fX;
        if ( fHeight < fWidth )
        {
            fWidth = svPoint.fX - svMin.fX;
            if ( fHeight < fWidth )
            {
                if ( fHeight < (svPoint.fY - svMin.fY) )
                    svPoint.fY = svMax.fY;
                else
                    svPoint.fY = svMin.fY;
            }
            else if ( fWidth < (svPoint.fY - svMin.fY) )
                svPoint.fX = svMin.fX;
            else
                svPoint.fY = svMin.fY;
        }
        else
        {
            fHeight = svPoint.fY - svMin.fY;
            if ( fWidth < fHeight )
            {
                if ( fWidth < (svPoint.fX - svMin.fX) )
                    svPoint.fX = svMax.fX;
                else
                    svPoint.fX = svMin.fX;
            }
            else if ( fHeight < (svPoint.fX - svMin.fX) )
                svPoint.fY = svMin.fY;
            else
                svPoint.fX = svMin.fX;
        }
        break;

    case LEFT:
    case RIGHT:
        fHeight = svMax.fY - svPoint.fY;
        fWidth = svMax.fZ - svPoint.fZ;
        if ( fHeight < fWidth )
        {
            fWidth = svPoint.fZ - svMin.fZ;
            if ( fHeight < fWidth )
            {
                if ( fHeight < (svPoint.fY - svMin.fY) )
                    svPoint.fY = svMax.fY;
                else
                    svPoint.fY = svMin.fY;
            }
            else if ( fWidth < (svPoint.fY - svMin.fY) )
                svPoint.fZ = svMin.fZ;
            else
                svPoint.fY = svMin.fY;
        }
        else
        {
            fHeight = svPoint.fY - svMin.fY;
            if ( fWidth < fHeight )
            {
                if ( fWidth < (svPoint.fZ - svMin.fZ) )
                    svPoint.fZ = svMax.fZ;
                else
                    svPoint.fZ = svMin.fZ;
            }
            else if ( fHeight < (svPoint.fZ - svMin.fZ) )
                svPoint.fY = svMin.fY;
            else
                svPoint.fZ = svMin.fZ;
        }
        break;

    case TOP:
    case BOTTOM:
        fHeight = svMax.fZ - svPoint.fZ;
        fWidth = svMax.fX - svPoint.fX;
        if ( fHeight < fWidth )
        {
            fWidth = svPoint.fX - svMin.fX;
            if ( fHeight < fWidth )
            {
                if ( fHeight < (svPoint.fZ - svMin.fZ) )
                    svPoint.fZ = svMax.fZ;
                else
                    svPoint.fZ = svMin.fZ;
            }
            else if ( fWidth < (svPoint.fZ - svMin.fZ) )
                svPoint.fX = svMin.fX;
            else
                svPoint.fZ = svMin.fZ;
        }
        else
        {
            fHeight = svPoint.fZ - svMin.fZ;
            if ( fWidth < fHeight )
            {
                if ( fWidth < (svPoint.fX - svMin.fX) )
                    svPoint.fX = svMax.fX;
                else
                    svPoint.fX = svMin.fX;
            }
            else if ( fHeight < (svPoint.fX - svMin.fX) )
                svPoint.fZ = svMin.fZ;
            else
                svPoint.fX = svMin.fX;
        }
        break;

    default:
        return 0;
    }

    SVector svVec1;
    svVec1.fX = svListener.fX - svPoint.fX;
    svVec1.fY = svListener.fY - svPoint.fY;
    svVec1.fZ = svListener.fZ - svPoint.fZ;
    NormalizeVector(svVec1);
    
    SVector svVec2;
    svVec2.fX = svSource.fX - svPoint.fX;
    svVec2.fY = svSource.fY - svPoint.fY;
    svVec2.fZ = svSource.fZ - svPoint.fZ;
    NormalizeVector(svVec2);
    
    float fCos = svVec1.fX * svVec2.fX + svVec1.fY * svVec2.fY + svVec1.fZ * svVec2.fZ;
    return 1 - ((fCos + 1) / 2);
}




/*
*/
float GetObstructionLevel(SVector &svListener, 
                          SVector &svSource, 
                          SVector &svMin, 
                          SVector &svMax)
{
    long lListenerFlags = 0;
    if ( svListener.fX < svMin.fX )
        lListenerFlags |= LEFT;
    else if ( svListener.fX > svMax.fX )
        lListenerFlags |= RIGHT;
    if ( svListener.fY < svMin.fY )
        lListenerFlags |= BOTTOM;
    else if ( svListener.fY > svMax.fY )
        lListenerFlags |= TOP;
    if ( svListener.fZ < svMin.fZ )
        lListenerFlags |= FRONT;
    else if ( svListener.fZ > svMax.fZ )
        lListenerFlags |= BACK;
    
    long lSourceFlags = 0;
    if ( svSource.fX < svMin.fX )
        lSourceFlags |= LEFT;
    else if ( svSource.fX > svMax.fX )
        lSourceFlags |= RIGHT;
    if ( svSource.fY < svMin.fY )
        lSourceFlags |= BOTTOM;
    else if ( svSource.fY > svMax.fY )
        lSourceFlags |= TOP;
    if ( svSource.fZ < svMin.fZ )
        lSourceFlags |= FRONT;
    else if ( svSource.fZ > svMax.fZ )
        lSourceFlags |= BACK;
    
    if ( lListenerFlags & lSourceFlags )
        return 1;
    
    SVector svPlanePoint[3], 
            svPlaneNormal[3];
    
    long lPlaneType[3], 
         lNumPlanes = 0;

    switch ( lSourceFlags & 0x3f )
    {
    // Up cases
    case 4: // U (Top Plane)
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 0;
        svPlaneNormal[0].fY = 1;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = TOP;
        lNumPlanes = 1;
        break;

    case 6: //UF (Top & Front Planes)
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 0;
        svPlaneNormal[0].fY = 1;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = TOP;
        svPlanePoint[1] = svMin;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 0;
        svPlaneNormal[1].fZ = -1;
        lPlaneType[1] = FRONT;
        lNumPlanes = 2;
        break;

    case 5: // UB (Top & Back Planes)
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 0;
        svPlaneNormal[0].fY = 1;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = TOP;
        svPlanePoint[1] = svMax;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 0;
        svPlaneNormal[1].fZ = 1;
        lPlaneType[1] = BACK;
        lNumPlanes = 2;
        break;

    case 22: // RUF ()
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = RIGHT;
        svPlanePoint[1] = svMax;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = TOP;
        svPlanePoint[2] = svMin;
        svPlaneNormal[2].fX = 0;
        svPlaneNormal[2].fY = 0;
        svPlaneNormal[2].fZ = -1;
        lPlaneType[2] = FRONT;
        lNumPlanes = 3;
        break;

    case 20: // RU
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = RIGHT;
        svPlanePoint[1] = svMax;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = TOP;
        lNumPlanes = 2;
        break;

    case 36: // LU
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = -1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = LEFT;
        svPlanePoint[1] = svMax;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = TOP;
        lNumPlanes = 2;
        break;

    case 38: // LUF
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = -1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = LEFT;
        svPlanePoint[1] = svMax;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = TOP;
        svPlanePoint[2] = svMin;
        svPlaneNormal[2].fX = 0;
        svPlaneNormal[2].fY = 0;
        svPlaneNormal[2].fZ = -1;
        lPlaneType[2] = FRONT;
        lNumPlanes = 3;
        break;

    case 37: // LUB
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = -1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = LEFT;
        svPlanePoint[1] = svMax;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = TOP;
        svPlanePoint[2] = svMax;
        svPlaneNormal[2].fX = 0;
        svPlaneNormal[2].fY = 0;
        svPlaneNormal[2].fZ = 1;
        lPlaneType[2] = BACK;
        lNumPlanes = 3;
        break;

    case 21: // RUB
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = RIGHT;
        svPlanePoint[1] = svMax;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = TOP;
        svPlanePoint[2] = svMax;
        svPlaneNormal[2].fX = 0;
        svPlaneNormal[2].fY = 0;
        svPlaneNormal[2].fZ = 1;
        lPlaneType[2] = BACK;
        lNumPlanes = 3;
        break;


    
    
    // Down cases
    case 8: // D
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = 0;
        svPlaneNormal[0].fY = -1;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = BOTTOM;
        lNumPlanes = 1;
        break;

    case 10: //DF
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = 0;
        svPlaneNormal[0].fY = -1;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = BOTTOM;
        svPlanePoint[1] = svMin;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 0;
        svPlaneNormal[1].fZ = -1;
        lPlaneType[1] = FRONT;
        lNumPlanes = 2;
        break;

    case 9: // DB
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = 0;
        svPlaneNormal[0].fY = -1;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = BOTTOM;
        svPlanePoint[1] = svMax;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 0;
        svPlaneNormal[1].fZ = 1;
        lPlaneType[1] = BACK;
        lNumPlanes = 2;
        break;

    case 26: // RDF
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = RIGHT;
        svPlanePoint[1] = svMin;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = -1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = BOTTOM;
        svPlanePoint[2] = svMin;
        svPlaneNormal[2].fX = 0;
        svPlaneNormal[2].fY = 0;
        svPlaneNormal[2].fZ = -1;
        lPlaneType[2] = FRONT;
        lNumPlanes = 3;
        break;

    case 24: // RD
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = RIGHT;
        svPlanePoint[1] = svMin;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = -1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = BOTTOM;
        lNumPlanes = 2;
        break;

    case 40: // LD
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = -1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = LEFT;
        svPlanePoint[1] = svMin;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = -1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = BOTTOM;
        lNumPlanes = 2;
        break;

    case 42: // LDF
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = -1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = LEFT;
        svPlanePoint[1] = svMin;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = -1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = BOTTOM;
        svPlanePoint[2] = svMin;
        svPlaneNormal[2].fX = 0;
        svPlaneNormal[2].fY = 0;
        svPlaneNormal[2].fZ = -1;
        lPlaneType[2] = FRONT;
        lNumPlanes = 3;
        break;

    case 41: // LDB
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = -1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = LEFT;
        svPlanePoint[1] = svMin;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = -1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = BOTTOM;
        svPlanePoint[2] = svMax;
        svPlaneNormal[2].fX = 0;
        svPlaneNormal[2].fY = 0;
        svPlaneNormal[2].fZ = 1;
        lPlaneType[2] = BACK;
        lNumPlanes = 3;
        break;

    case 25: // RDB
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = RIGHT;
        svPlanePoint[1] = svMin;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = -1;
        svPlaneNormal[1].fZ = 0;
        lPlaneType[1] = BOTTOM;
        svPlanePoint[2] = svMax;
        svPlaneNormal[2].fX = 0;
        svPlaneNormal[2].fY = 0;
        svPlaneNormal[2].fZ = 1;
        lPlaneType[2] = BACK;
        lNumPlanes = 3;
        break;

    
    
    
    // Side cases
    case 2: // F
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = 0;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = -1;
        lPlaneType[0] = FRONT;
        lNumPlanes = 1;
        break;

    case 1: // B
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 0;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 1;
        lPlaneType[0] = BACK;
        lNumPlanes = 1;
        break;

    case 18: // RF
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = RIGHT;
        svPlanePoint[1] = svMin;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 0;
        svPlaneNormal[1].fZ = -1;
        lPlaneType[1] = FRONT;
        lNumPlanes = 2;
        break;

    case 16: // R
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = RIGHT;
        lNumPlanes = 1;
        break;

    case 32: // L
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = -1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = LEFT;
        lNumPlanes = 1;
        break;

    case 34: // LF
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = -1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = LEFT;
        svPlanePoint[1] = svMin;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 0;
        svPlaneNormal[1].fZ = -1;
        lPlaneType[1] = FRONT;
        lNumPlanes = 2;
        break;

    case 33: // LB
        svPlanePoint[0] = svMin;
        svPlaneNormal[0].fX = -1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = LEFT;
        svPlanePoint[1] = svMax;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 0;
        svPlaneNormal[1].fZ = 1;
        lPlaneType[1] = BACK;
        lNumPlanes = 2;
        break;

    case 17: // RB
        svPlanePoint[0] = svMax;
        svPlaneNormal[0].fX = 1;
        svPlaneNormal[0].fY = 0;
        svPlaneNormal[0].fZ = 0;
        lPlaneType[0] = RIGHT;
        svPlanePoint[1] = svMax;
        svPlaneNormal[1].fX = 0;
        svPlaneNormal[1].fY = 0;
        svPlaneNormal[1].fZ = 1;
        lPlaneType[1] = BACK;
        lNumPlanes = 2;
        break;


    default:
        lNumPlanes = 0;
        break;
    }

    for ( long m = 0; m < lNumPlanes; m++ )
    {
        SVector svIntersection;
        FindPlaneIntersectionPoint(svSource, 
                                   svListener, 
                                   svPlanePoint[m], 
                                   svPlaneNormal[m], 
                                   &svIntersection);

        if ( PointInCubePlane(svIntersection, 
                              lPlaneType[m], 
                              svMin, 
                              svMax) )
        {
            return CalcDiffractionLevel(svListener,
                                        svSource,
                                        &svIntersection,
                                        lPlaneType[m],
                                        svMin,
                                        svMax);
        }
    }

    return 1;
}
