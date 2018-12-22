// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3StringTree.h"
#include "Wm3BoundingVolume.h"
#include "Wm3ColorRGBA.h"
#include "Wm3ColorRGB.h"
#include "Wm3Rtti.h"
using namespace Wm3;

//----------------------------------------------------------------------------
StringTree::StringTree (int iStringQuantity, int iChildQuantity)
    :
    m_kStrings(iStringQuantity),
    m_kChildren(iChildQuantity)
{
}
//----------------------------------------------------------------------------
StringTree::~StringTree ()
{
    int i;
    for (i = 0; i < m_kStrings.GetQuantity(); i++)
        delete[] m_kStrings[i];

    for (i = 0; i < m_kChildren.GetQuantity(); i++)
        delete m_kChildren[i];
}
//----------------------------------------------------------------------------
void StringTree::SetString (int i, char* acString)
{
    assert( 0 <= i && i < m_kStrings.GetMaxQuantity() );
    m_kStrings.Append(acString);
}
//----------------------------------------------------------------------------
void StringTree::SetChild (int i, StringTree* pkChild)
{
    assert( 0 <= i && i < m_kChildren.GetMaxQuantity() );
    m_kChildren.Append(pkChild);
}
//----------------------------------------------------------------------------
bool StringTree::Save (const char* acFilename, int iTabSize)
{
    FILE* pkFile = fopen(acFilename,"wt");
    if ( !pkFile )
        return false;

    Save(pkFile,0,iTabSize);

    return fclose(pkFile) == 0;
}
//----------------------------------------------------------------------------
void StringTree::Save (FILE* pkFile, int iLevel, int iTabSize)
{
    // indent to proper location
    int i, iIndent = iLevel*iTabSize;
    if ( iIndent > 0 )
    {
        for (i = 0; i < iIndent; i++)
            fprintf(pkFile,"%c",' ');
    }

    // label with level
    if ( iLevel < 10 )
        fprintf(pkFile,"%d:  ",iLevel);
    else
        fprintf(pkFile,"%d: ",iLevel);

    // header string
    if ( m_kStrings.GetQuantity() > 0 )
        fprintf(pkFile,"%s\n",m_kStrings[0]);
    else
        fprintf(pkFile,"<no header>\n");

    // body strings
    iIndent += 4;

    int j;
    for (j = 1; j < m_kStrings.GetQuantity(); j++)
    {
        for (i = 0; i < iIndent; i++)
            fprintf(pkFile,"%c",' ');
        fprintf(pkFile,"%s\n",m_kStrings[j]);
    }

    iLevel++;
    for (j = 0; j < m_kChildren.GetQuantity(); j++)
        m_kChildren[j]->Save(pkFile,iLevel,iTabSize);
}
//----------------------------------------------------------------------------
void StringTree::FormatFloat (float fValue, char* acString)
{
    if ( fValue > -Mathf::MAX_REAL )
    {
        if ( fValue < Mathf::MAX_REAL )
            sprintf(acString,"%f",fValue);
        else
            strcpy(acString,"INFINITY");
    }
    else
    {
        strcpy(acString,"-INFINITY");
    }
}
//----------------------------------------------------------------------------
void StringTree::FormatDouble (double dValue, char* acString)
{
    if ( dValue > -Mathd::MAX_REAL )
    {
        if ( dValue < Mathd::MAX_REAL )
            sprintf(acString,"%lf",dValue);
        else
            strcpy(acString,"INFINITY");
    }
    else
    {
        strcpy(acString,"-INFINITY");
    }
}
//----------------------------------------------------------------------------
char* StringTree::Format (const Rtti* pkRTTI, const char* acName)
{
    assert( pkRTTI );
    const char* acRTTIName = pkRTTI->GetName();

    char* acString;
    if ( acName )
    {
        acString = new char[strlen(acRTTIName)+strlen(acName)+4];
        sprintf(acString,"%s <%s>",acRTTIName,acName);
    }
    else
    {
        acString = new char[strlen(acRTTIName)+2];
        sprintf(acString,"%s",acRTTIName);
    }
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acString)
{
    char* acDuplicate;
    if ( acString )
    {
        acDuplicate = new char[strlen(acString)+1];
        strcpy(acDuplicate,acString);
    }
    else
    {
        acDuplicate = new char[strlen("<no title>")+1];
        strcpy(acDuplicate,"<no title>");
    }
    return acDuplicate;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// formatting of native data
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, bool bValue)
{
    assert( acPrefix );
    int iLength = (int)strlen(acPrefix)+2 + ( bValue ? 4 : 5 );
    char* acString = new char[iLength];
    if ( bValue )
        sprintf(acString,"%s true",acPrefix);
    else
        sprintf(acString,"%s false",acPrefix);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, char cValue)
{
    assert( acPrefix );
    int iLength = (int)strlen(acPrefix)+2 + 1;
    char* acString = new char[iLength];
    sprintf(acString,"%s %c",acPrefix,cValue);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, unsigned char ucValue)
{
    assert( acPrefix );
    char acDummy[32];
    sprintf(acDummy,"%u",(unsigned int)ucValue);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, short sValue)
{
    assert( acPrefix );
    char acDummy[32];
    sprintf(acDummy,"%hd",sValue);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, unsigned short usValue)
{
    assert( acPrefix );
    char acDummy[32];
    sprintf(acDummy,"%hu",usValue);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, int iValue)
{
    assert( acPrefix );
    char acDummy[32];
    sprintf(acDummy,"%d",iValue);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, unsigned int uiValue)
{
    assert( acPrefix );
    char acDummy[32];
    sprintf(acDummy,"%u",uiValue);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, long lValue)
{
    assert( acPrefix );
    char acDummy[32];
    sprintf(acDummy,"%ld",lValue);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, unsigned long ulValue)
{
    assert( acPrefix );
    char acDummy[32];
    sprintf(acDummy,"%lu",ulValue);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, float fValue)
{
    assert( acPrefix );
    char acDummy[256];
    FormatFloat(fValue,acDummy);

    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, double dValue)
{
    assert( acPrefix );
    char acDummy[256];
    FormatDouble(dValue,acDummy);

    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, void* pvValue)
{
    assert( acPrefix );
    char acDummy[32];
    sprintf(acDummy,"%p",pvValue);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const char* acValue)
{
    assert( acPrefix && acValue );
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acValue);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acValue);
    return acString;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// formatting of objects
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const BoundingVolume* pkValue)
{
    assert( acPrefix );
    char acX[256], acY[256], acZ[256], acR[256];
    FormatFloat(pkValue->GetCenter().X(),acX);
    FormatFloat(pkValue->GetCenter().Y(),acY);
    FormatFloat(pkValue->GetCenter().Z(),acZ);
    FormatFloat(pkValue->GetRadius(),acR);

    char acDummy[256];
    sprintf(acDummy,"(x: %s, y: %s, z: %s, r: %s)",acX,acY,acZ,acR);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const ColorRGBA& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"(r: %f, g: %f, b: %f, a: %f)",rkValue.R(),rkValue.G(),
        rkValue.B(),rkValue.A());
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const ColorRGB& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"(r: %f, g: %f, b: %f)",rkValue.R(),rkValue.G(),
        rkValue.B());
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Line3f& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"(px: %f, py: %f, pz: %f, dx: %f, dy: %f, dz: %f)",
        rkValue.Origin.X(),rkValue.Origin.Y(),rkValue.Origin.Z(),
        rkValue.Direction.X(),rkValue.Direction.Y(),rkValue.Direction.Z());
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Matrix2f& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"[c0:(%f,%f),c1:(%f,%f)]",
        rkValue[0][0],rkValue[1][0],
        rkValue[0][1],rkValue[1][1]);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Matrix3f& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"[c0:(%f,%f,%f),c1:(%f,%f,%f),c2:(%f,%f,%f)]",
        rkValue[0][0],rkValue[1][0],rkValue[2][0],
        rkValue[0][1],rkValue[1][1],rkValue[2][1],
        rkValue[0][2],rkValue[1][2],rkValue[2][2]);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Matrix4f& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,
    "[c0:(%f,%f,%f,%f),c1:(%f,%f,%f,%f),c2:(%f,%f,%f,%f),c3(%f,%f,%f,%f)]",
        rkValue[0][0],rkValue[1][0],rkValue[2][0],rkValue[3][0],
        rkValue[0][1],rkValue[1][1],rkValue[2][1],rkValue[3][1],
        rkValue[0][2],rkValue[1][2],rkValue[2][2],rkValue[3][2],
        rkValue[0][3],rkValue[1][3],rkValue[2][3],rkValue[3][3]);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Plane3f& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"(nx: %f, ny: %f, nz: %f, c: %f)",rkValue.Normal.X(),
        rkValue.Normal.Y(),rkValue.Normal.Z(),rkValue.Constant);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Quaternionf& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"(w: %.3f, x: %.3f, y: %.3f, z: %.3f)",rkValue.W(),
        rkValue.X(),rkValue.Y(),rkValue.Z());
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Vector2f& rkValue)
{
    assert( acPrefix );
    char acX[256], acY[256];
    FormatFloat(rkValue.X(),acX);
    FormatFloat(rkValue.Y(),acY);

    char acDummy[256];
    sprintf(acDummy,"(x: %s, y: %s)",acX,acY);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Vector3f& rkValue)
{
    assert( acPrefix );
    char acX[256], acY[256], acZ[256];
    FormatFloat(rkValue.X(),acX);
    FormatFloat(rkValue.Y(),acY);
    FormatFloat(rkValue.Z(),acZ);

    char acDummy[256];
    sprintf(acDummy,"(x: %s, y: %s, z: %s)",acX,acY,acZ);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Vector4f& rkValue)
{
    assert( acPrefix );
    char acX[256], acY[256], acZ[256], acW[256];
    FormatFloat(rkValue.X(),acX);
    FormatFloat(rkValue.Y(),acY);
    FormatFloat(rkValue.Z(),acZ);
    FormatFloat(rkValue.Z(),acW);

    char acDummy[256];
    sprintf(acDummy,"(x: %s, y: %s, z: %s, w: %s)",acX,acY,acZ,acW);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------

char* StringTree::Format (const char* acPrefix, const Line3d& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"(px: %lf, py: %lf, pz: %lf, dx: %lf, dy: %lf, dz: %lf)",
        rkValue.Origin.X(),rkValue.Origin.Y(),rkValue.Origin.Z(),
        rkValue.Direction.X(),rkValue.Direction.Y(),rkValue.Direction.Z());
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Matrix2d& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"[c0:(%lf,%lf),c1:(%lf,%lf)]",
        rkValue[0][0],rkValue[1][0],
        rkValue[0][1],rkValue[1][1]);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Matrix3d& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"[c0:(%lf,%lf,%lf),c1:(%lf,%lf,%lf),c2:(%lf,%lf,%lf)]",
        rkValue[0][0],rkValue[1][0],rkValue[2][0],
        rkValue[0][1],rkValue[1][1],rkValue[2][1],
        rkValue[0][2],rkValue[1][2],rkValue[2][2]);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Matrix4d& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,
    "[c0:(%lf,%lf,%lf,%lf),c1:(%lf,%lf,%lf,%lf),c2:(%lf,%lf,%lf,%lf),"
    "c3(%lf,%lf,%lf,%lf)]",
        rkValue[0][0],rkValue[1][0],rkValue[2][0],rkValue[3][0],
        rkValue[0][1],rkValue[1][1],rkValue[2][1],rkValue[3][1],
        rkValue[0][2],rkValue[1][2],rkValue[2][2],rkValue[3][2],
        rkValue[0][3],rkValue[1][3],rkValue[2][3],rkValue[3][3]);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Plane3d& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"(nx: %lf, ny: %lf, nz: %lf, c: %lf)",rkValue.Normal.X(),
        rkValue.Normal.Y(),rkValue.Normal.Z(),rkValue.Constant);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Quaterniond& rkValue)
{
    assert( acPrefix );
    char acDummy[256];
    sprintf(acDummy,"(w: %.3lf, x: %.3lf, y: %.3lf, z: %.3lf)",rkValue.W(),
        rkValue.X(),rkValue.Y(),rkValue.Z());
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Vector2d& rkValue)
{
    assert( acPrefix );
    char acX[256], acY[256];
    FormatDouble(rkValue.X(),acX);
    FormatDouble(rkValue.Y(),acY);

    char acDummy[256];
    sprintf(acDummy,"(x: %s, y: %s)",acX,acY);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Vector3d& rkValue)
{
    assert( acPrefix );
    char acX[256], acY[256], acZ[256];
    FormatDouble(rkValue.X(),acX);
    FormatDouble(rkValue.Y(),acY);
    FormatDouble(rkValue.Z(),acZ);

    char acDummy[256];
    sprintf(acDummy,"(x: %s, y: %s, z: %s)",acX,acY,acZ);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
char* StringTree::Format (const char* acPrefix, const Vector4d& rkValue)
{
    assert( acPrefix );
    char acX[256], acY[256], acZ[256], acW[256];
    FormatDouble(rkValue.X(),acX);
    FormatDouble(rkValue.Y(),acY);
    FormatDouble(rkValue.Z(),acZ);
    FormatDouble(rkValue.Z(),acW);

    char acDummy[256];
    sprintf(acDummy,"(x: %s, y: %s, z: %s, w: %s)",acX,acY,acZ,acW);
    int iLength = (int)strlen(acPrefix)+2 + (int)strlen(acDummy);
    char* acString = new char[iLength];
    sprintf(acString,"%s %s",acPrefix,acDummy);
    return acString;
}
//----------------------------------------------------------------------------
