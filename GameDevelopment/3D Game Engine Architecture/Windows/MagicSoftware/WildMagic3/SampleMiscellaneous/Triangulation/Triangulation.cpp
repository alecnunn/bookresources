// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Triangulation.h"
#include "TriangulateInt.h"

WM3_WINDOW_APPLICATION(Triangulation);

const int g_iSize = 256;

//----------------------------------------------------------------------------
Triangulation::Triangulation ()
    :
    WindowApplication2("Triangulation",0,0,g_iSize,g_iSize,ColorRGB::WHITE)
{
    m_aaiVertex = 0;
    m_aiIndex = 0;
}
//----------------------------------------------------------------------------
bool Triangulation::OnInitialize ()
{
    if ( !WindowApplication2::OnInitialize() )
        return false;

    // select a polygon
    m_iVQuantity = 10;
    System::Allocate(2,m_iVQuantity,m_aaiVertex);
    m_aaiVertex[0][0] =  29;  m_aaiVertex[0][1] = 139;
    m_aaiVertex[1][0] =  78;  m_aaiVertex[1][1] =  99;
    m_aaiVertex[2][0] = 125;  m_aaiVertex[2][1] = 141;
    m_aaiVertex[3][0] = 164;  m_aaiVertex[3][1] = 116;
    m_aaiVertex[4][0] = 201;  m_aaiVertex[4][1] = 168;
    m_aaiVertex[5][0] = 157;  m_aaiVertex[5][1] = 163;
    m_aaiVertex[6][0] = 137;  m_aaiVertex[6][1] = 200;
    m_aaiVertex[7][0] =  98;  m_aaiVertex[7][1] = 134;
    m_aaiVertex[8][0] =  52;  m_aaiVertex[8][1] = 146;
    m_aaiVertex[9][0] =  55;  m_aaiVertex[9][1] = 191;

    // construct the triangulation
    m_iTQuantity = m_iVQuantity-2;
    m_aiIndex = new int[3*m_iTQuantity];
    TriangulateInt(m_iVQuantity,m_aaiVertex,m_aiIndex);

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void Triangulation::OnTerminate ()
{
    System::Deallocate(m_aaiVertex);
    delete[] m_aiIndex;
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void Triangulation::OnDisplay ()
{
    ClearScreen();

    Color kBlue(0,0,255), kBlack(0,0,0), kGray(128,128,128);
    int i, iX0, iY0, iX1, iY1;

    // draw the triangulation edges
    const int* piIndex = m_aiIndex;
    for (i = 0; i < m_iTQuantity; i++)
    {
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;

        iX0 = m_aaiVertex[iV0][0];
        iY0 = m_aaiVertex[iV0][1];
        iX1 = m_aaiVertex[iV1][0];
        iY1 = m_aaiVertex[iV1][1];
        DrawLine(iX0,iY0,iX1,iY1,kGray);

        iX0 = m_aaiVertex[iV1][0];
        iY0 = m_aaiVertex[iV1][1];
        iX1 = m_aaiVertex[iV2][0];
        iY1 = m_aaiVertex[iV2][1];
        DrawLine(iX0,iY0,iX1,iY1,kGray);

        iX0 = m_aaiVertex[iV2][0];
        iY0 = m_aaiVertex[iV2][1];
        iX1 = m_aaiVertex[iV0][0];
        iY1 = m_aaiVertex[iV0][1];
        DrawLine(iX0,iY0,iX1,iY1,kGray);
    }

    // draw the polygon edges
    for (int i0 = m_iVQuantity-1, i1 = 0; i1 < m_iVQuantity; i0 = i1++)
    {
        iX0 = m_aaiVertex[i0][0];
        iY0 = m_aaiVertex[i0][1];
        iX1 = m_aaiVertex[i1][0];
        iY1 = m_aaiVertex[i1][1];
        DrawLine(iX0,iY0,iX1,iY1,kBlue);
    }

    // draw the polygon vertices
    for (i = 0; i < m_iVQuantity; i++)
    {
        iX0 = m_aaiVertex[i][0];
        iY0 = m_aaiVertex[i][1];
        for (int iDY = -1; iDY <= 1; iDY++)
        {
            for (int iDX = -1; iDX <= 1; iDX++)
                SetPixel(iX0+iDX,iY0+iDY,kBlack);
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
