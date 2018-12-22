// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Lattice.h"
using namespace Wm3;

const char* Lattice::ms_acHeader = "Magic Image";

//----------------------------------------------------------------------------
Lattice::Lattice (int iDimensions, int* aiBound)
{
#ifdef _DEBUG
    assert( iDimensions > 0 && aiBound );
    for (int i = 0; i < iDimensions; i++)
    {
        assert( aiBound[i] > 0 );
    }
#endif

    m_iDimensions = iDimensions;
    m_aiBound = aiBound;
    m_aiOffset = new int[iDimensions];

    ComputeQuantityAndOffsets();
}
//----------------------------------------------------------------------------
Lattice::Lattice (const Lattice& rkLattice)
{
    m_iDimensions = rkLattice.m_iDimensions;
    m_iQuantity = rkLattice.m_iQuantity;
    m_aiBound = new int[m_iDimensions];
    m_aiOffset = new int[m_iDimensions];

    int iCopySize = m_iDimensions*sizeof(int);
    memcpy(m_aiBound,rkLattice.m_aiBound,iCopySize);
    memcpy(m_aiOffset,rkLattice.m_aiOffset,iCopySize);
}
//----------------------------------------------------------------------------
Lattice::Lattice (int iDimensions)
{
    assert( iDimensions > 0 );

    m_iDimensions = iDimensions;
    m_aiBound = 0;
    m_aiOffset = new int[iDimensions];
    memset(m_aiOffset,0,iDimensions*sizeof(int));
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
Lattice::Lattice ()
{
    m_iDimensions = 0;
    m_aiBound = 0;
    m_aiOffset = 0;
    m_iQuantity = 0;
}
//----------------------------------------------------------------------------
Lattice::~Lattice ()
{
    delete[] m_aiBound;
    delete[] m_aiOffset;
}
//----------------------------------------------------------------------------
void Lattice::SetBounds (int* aiBound)
{
#ifdef _DEBUG
    assert( aiBound );
    for (int i = 0; i < m_iDimensions; i++)
    {
        assert( aiBound[i] > 0 );
    }
#endif

    m_aiBound = aiBound;
    ComputeQuantityAndOffsets();
}
//----------------------------------------------------------------------------
void Lattice::ComputeQuantityAndOffsets ()
{
    int i;

    // calculate number of lattice points
    m_iQuantity = 1;
    for (i = 0; i < m_iDimensions; i++)
        m_iQuantity *= m_aiBound[i];

    // calculate offset indices of neighboring lattice points
    m_aiOffset[0] = 1;
    for (i = 1; i < m_iDimensions; i++)
        m_aiOffset[i] = m_aiBound[i-1]*m_aiOffset[i-1];
}
//----------------------------------------------------------------------------
Lattice& Lattice::operator= (const Lattice& rkLattice)
{
    if ( m_iDimensions != rkLattice.m_iDimensions )
    {
        delete[] m_aiBound;
        delete[] m_aiOffset;
        m_iDimensions = rkLattice.m_iDimensions;
        m_iQuantity = rkLattice.m_iQuantity;
        m_aiBound = new int[m_iDimensions];
        m_aiOffset = new int[m_iDimensions];
    }

    int iCopySize = m_iDimensions*sizeof(int);
    memcpy(m_aiBound,rkLattice.m_aiBound,iCopySize);
    memcpy(m_aiOffset,rkLattice.m_aiOffset,iCopySize);

    return *this;
}
//----------------------------------------------------------------------------
bool Lattice::operator== (const Lattice& rkLattice) const
{
    if ( m_iDimensions != rkLattice.m_iDimensions )
        return false;

    int iCompareSize = m_iDimensions*sizeof(int);
    return memcmp(m_aiBound,rkLattice.m_aiBound,iCompareSize) == 0;
}
//----------------------------------------------------------------------------
bool Lattice::operator!= (const Lattice& rkLattice) const
{
    return !operator==(rkLattice);
}
//----------------------------------------------------------------------------
int Lattice::GetIndex (const int* aiCoord) const
{
    // assert:  auiCoord is array of m_iDimensions elements
    int iIndex = aiCoord[0];
    for (int i = 1; i < m_iDimensions; i++)
        iIndex += m_aiOffset[i]*aiCoord[i];
    return iIndex;
}
//----------------------------------------------------------------------------
void Lattice::GetCoordinates (int iIndex, int* aiCoord) const
{
    // assert:  aiCoord is array of m_iDimensions elements
    for (int i = 0; i < m_iDimensions; i++)
    {
        aiCoord[i] = iIndex % m_aiBound[i];
        iIndex /= m_aiBound[i];
    }
}
//----------------------------------------------------------------------------
bool Lattice::Load (FILE* pkIFile)
{
    // Header is an ASCII string of 'char'.  No need to swap bytes on big
    // endian platforms.
    int iSize = (int)strlen(ms_acHeader) + 1;
    char* acBuffer = new char[iSize];
    System::Read1(pkIFile,iSize,acBuffer);
    acBuffer[iSize-1] = 0;
    if ( strncmp(acBuffer,ms_acHeader,iSize) != 0 )
    {
        delete[] acBuffer;
        m_iDimensions = 0;
        m_iQuantity = 0;
        m_aiBound = 0;
        m_aiOffset = 0;
        return false;
    }
    delete[] acBuffer;

    System::Read4(pkIFile,1,&m_iDimensions);

    delete[] m_aiBound;
    m_aiBound = new int[m_iDimensions];
    System::Read4(pkIFile,m_iDimensions,m_aiBound);

    delete[] m_aiOffset;
    m_aiOffset = new int[m_iDimensions];

    ComputeQuantityAndOffsets();

    return true;
}
//----------------------------------------------------------------------------
bool Lattice::Save (FILE* pkOFile) const
{
    System::Write1(pkOFile,(int)strlen(ms_acHeader)+1,ms_acHeader);
    System::Write4(pkOFile,1,&m_iDimensions);
    System::Write4(pkOFile,m_iDimensions,m_aiBound);
    return true;
}
//----------------------------------------------------------------------------
bool Lattice::LoadRaw (const char* acFilename, int& riDimensions,
    int*& raiBound, int& riQuantity, int& riRTTI, int& riSizeOf,
    char*& racData)
{
    FILE* pkIFile = fopen(acFilename,"rb");
    if ( !pkIFile )
        return false;

    // Header is an ASCII string of 'char'.  No need to swap bytes on big
    // endian platforms.
    int iSize = (int)strlen(ms_acHeader) + 1;
    char* acBuffer = new char[iSize];
    System::Read1(pkIFile,iSize,acBuffer);
    acBuffer[iSize-1] = 0;
    if ( strncmp(acBuffer,ms_acHeader,iSize) != 0 )
    {
        delete[] acBuffer;
        fclose(pkIFile);
        return false;
    }
    delete[] acBuffer;

    System::Read4(pkIFile,1,&riDimensions);

    raiBound = new int[riDimensions];
    System::Read4(pkIFile,riDimensions,raiBound);

    riQuantity = 1;
    for (int i = 0; i < riDimensions; i++)
        riQuantity *= raiBound[i];

    System::Read4(pkIFile,1,&riRTTI);
    System::Read4(pkIFile,1,&riSizeOf);

    racData = new char[riQuantity*riSizeOf];
    switch ( riSizeOf )
    {
    case 1:
        System::Read1(pkIFile,riQuantity,racData);
        break;
    case 2:
        System::Read2(pkIFile,riQuantity,racData);
        break;
    case 4:
        System::Read4(pkIFile,riQuantity,racData);
        break;
    case 8:
        System::Read8(pkIFile,riQuantity,racData);
        break;
    default:
        assert( false );
    }

    return true;
}
//----------------------------------------------------------------------------
