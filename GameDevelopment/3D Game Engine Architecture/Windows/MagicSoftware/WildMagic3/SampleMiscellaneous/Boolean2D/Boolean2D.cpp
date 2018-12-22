// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Boolean2D.h"

WM3_WINDOW_APPLICATION(Boolean2D);

const int g_iSize = 256;

//----------------------------------------------------------------------------
Boolean2D::Boolean2D ()
    :
    WindowApplication2("Boolean2D",0,0,g_iSize,g_iSize,ColorRGB::WHITE)
{
    m_pkActive = 0;
    m_pkPoly0 = 0;
    m_pkPoly1 = 0;
    m_bPointPolygonTest = false;
}
//----------------------------------------------------------------------------
bool Boolean2D::OnInitialize ()
{
    if ( !WindowApplication2::OnInitialize() )
        return false;

    m_iChoice = 0;
    m_pkPoly0 = ConstructInvertedEll();
    m_pkPoly1 = ConstructPentagon();
    DoBoolean();

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void Boolean2D::OnDisplay ()
{
    ClearScreen();

    if ( m_bPointPolygonTest )
    {
        DrawRegions(*m_pkPoly0);
    }
    else
    {
        DrawPolySolid(*m_pkPoly0,Color(255,0,0));
        DrawPolySolid(*m_pkPoly1,Color(0,255,0));
        if ( m_pkActive )
            DrawPolySolid(*m_pkActive,Color(0,0,255));
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool Boolean2D::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if ( WindowApplication2::OnKeyDown(ucKey,iX,iY) )
        return true;

    switch ( ucKey )
    {
    case 'n':
    case 'N':
        delete m_pkPoly0;
        delete m_pkPoly1;
        m_pkActive = 0;

        m_iChoice = (m_iChoice+1) % 3;
        switch ( m_iChoice )
        {
        case 0:
            m_pkPoly0 = ConstructInvertedEll();
            m_pkPoly1 = ConstructPentagon();
            break;
        case 1:
            m_pkPoly0 = ConstructSquare();
            m_pkPoly1 = ConstructSShape();
            break;
        case 2:
            m_pkPoly0 = ConstructPolyWithHoles();
            m_pkPoly1 = ConstructPentagon();
            break;
        }
        DoBoolean();
        break;

    case 'p':
    case 'P':
        m_pkActive = 0;
        break;
    case 'u':
    case 'U':
        m_pkActive = &m_kUnion;
        break;
    case 'i':
    case 'I':
        m_pkActive = &m_kIntersection;
        break;
    case 'd':
    case 'D':
        m_pkActive = &m_kDiff01;
        break;
    case 'e':
    case 'E':
        m_pkActive = &m_kDiff10;
        break;
    case 'x':
    case 'X':
        m_pkActive = &m_kXor;
        break;
    case 't':
    case 'T':
        m_bPointPolygonTest = !m_bPointPolygonTest;
        break;
    }

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
Polygon2* Boolean2D::ConstructInvertedEll ()
{
    double dW = (double)GetWidth();
    double d1d8 = 0.125*dW;
    double d2d8 = 0.250*dW;
    double d3d8 = 0.375*dW;
    double d5d8 = 0.625*dW;
    double d6d8 = 0.750*dW;
    double d7d8 = 0.875*dW;

    const int iVQuantity = 10;
    Vector2d akV[iVQuantity] =
    {
        Vector2d(d1d8,d1d8),
        Vector2d(d3d8,d1d8),
        Vector2d(d3d8,d3d8),
        Vector2d(d2d8,d3d8),
        Vector2d(d2d8,d6d8),
        Vector2d(d5d8,d6d8),
        Vector2d(d5d8,d5d8),
        Vector2d(d7d8,d5d8),
        Vector2d(d7d8,d7d8),
        Vector2d(d1d8,d7d8)
    };

    Polygon2* pkPoly = new Polygon2;
    for (int i0 = iVQuantity-1, i1 = 0; i1 < iVQuantity; i0 = i1, i1++)
    {
        pkPoly->InsertVertex(akV[i1]);
        pkPoly->InsertEdge(Edge2(i0,i1));
    }
    pkPoly->Finalize();
    return pkPoly;
}
//----------------------------------------------------------------------------
Polygon2* Boolean2D::ConstructPentagon ()
{
    const int iVQuantity = 5;

    double dPrimitiveAngle = Mathd::TWO_PI/iVQuantity;
    double dRadius = 0.35*GetWidth();
    double dCx = 0.5*GetWidth(), dCy = 0.5*GetWidth();

    Vector2d akV[iVQuantity];
    for (int i = 0; i < iVQuantity; i++)
    {
        double dAngle = i*dPrimitiveAngle;
        akV[i].X() = dCx+dRadius*Mathd::Cos(dAngle);
        akV[i].Y() = dCy+dRadius*Mathd::Sin(dAngle);
    }

    Polygon2* pkPoly = new Polygon2;
    for (int i0 = iVQuantity-1, i1 = 0; i1 < iVQuantity; i0 = i1, i1++)
    {
        pkPoly->InsertVertex(akV[i1]);
        pkPoly->InsertEdge(Edge2(i0,i1));
    }
    pkPoly->Finalize();
    return pkPoly;
}
//----------------------------------------------------------------------------
Polygon2* Boolean2D::ConstructSquare ()
{
    double dW = (double)GetWidth();
    double d2d8 = 0.250*dW;
    double d6d8 = 0.750*dW;

    const int iVQuantity = 4;
    Vector2d akV[iVQuantity] =
    {
        Vector2d(d2d8,d2d8),
        Vector2d(d6d8,d2d8),
        Vector2d(d6d8,d6d8),
        Vector2d(d2d8,d6d8)
    };

    Polygon2* pkPoly = new Polygon2;
    for (int i0 = iVQuantity-1, i1 = 0; i1 < iVQuantity; i0 = i1, i1++)
    {
        pkPoly->InsertVertex(akV[i1]);
        pkPoly->InsertEdge(Edge2(i0,i1));
    }
    pkPoly->Finalize();
    return pkPoly;
}
//----------------------------------------------------------------------------
Polygon2* Boolean2D::ConstructSShape ()
{
    double dW = (double)GetWidth();
    double d10d32 = 10.0*dW/32.0;
    double d12d32 = 12.0*dW/32.0;
    double d13d32 = 13.0*dW/32.0;
    double d16d32 = 16.0*dW/32.0;
    double d19d32 = 19.0*dW/32.0;
    double d20d32 = 20.0*dW/32.0;
    double d22d32 = 22.0*dW/32.0;
    double d24d32 = 24.0*dW/32.0;
    double d26d32 = 26.0*dW/32.0;
    double d28d32 = 28.0*dW/32.0;

    const int iVQuantity = 12;
    Vector2d akV[iVQuantity] =
    {
        Vector2d(d24d32,d10d32),
        Vector2d(d28d32,d10d32),
        Vector2d(d28d32,d16d32),
        Vector2d(d22d32,d16d32),
        Vector2d(d22d32,d19d32),
        Vector2d(d24d32,d19d32),
        Vector2d(d24d32,d22d32),
        Vector2d(d20d32,d22d32),
        Vector2d(d20d32,d13d32),
        Vector2d(d26d32,d13d32),
        Vector2d(d26d32,d12d32),
        Vector2d(d24d32,d12d32)
    };

    Polygon2* pkPoly = new Polygon2;
    for (int i0 = iVQuantity-1, i1 = 0; i1 < iVQuantity; i0 = i1, i1++)
    {
        pkPoly->InsertVertex(akV[i1]);
        pkPoly->InsertEdge(Edge2(i0,i1));
    }
    pkPoly->Finalize();
    return pkPoly;
}
//----------------------------------------------------------------------------
Polygon2* Boolean2D::ConstructPolyWithHoles ()
{
    double dW = (double)GetWidth();
    double d2d16 = 2.0*dW/16.0;
    double d3d16 = 3.0*dW/16.0;
    double d4d16 = 4.0*dW/16.0;
    double d6d16 = 6.0*dW/16.0;
    double d14d16 = 14.0*dW/16.0;

    const int iVQuantity = 6;
    Vector2d akV[iVQuantity] =
    {
        // outer boundary
        Vector2d(d2d16,d2d16),
        Vector2d(d14d16,d2d16),
        Vector2d(d2d16,d14d16),

        // inner boundary
        Vector2d(d4d16,d3d16),
        Vector2d(d6d16,d6d16),
        Vector2d(d6d16,d3d16)
    };

    Polygon2* pkPoly = new Polygon2;
    for (int i = 0; i < iVQuantity; i++)
        pkPoly->InsertVertex(akV[i]);

    pkPoly->InsertEdge(Edge2(0,1));
    pkPoly->InsertEdge(Edge2(1,2));
    pkPoly->InsertEdge(Edge2(2,0));
    pkPoly->InsertEdge(Edge2(3,4));
    pkPoly->InsertEdge(Edge2(4,5));
    pkPoly->InsertEdge(Edge2(5,3));

    pkPoly->Finalize();
    return pkPoly;
}
//----------------------------------------------------------------------------
void Boolean2D::DrawPolySolid (Polygon2& rkP, Color kColor)
{
    Vector2d kV0, kV1;
    Edge2 kE;
    int i, iX0, iY0, iX1, iY1;

    // draw edges
    for (i = 0; i < rkP.GetEdgeQuantity(); i++)
    {
        rkP.GetEdge(i,kE);
        rkP.GetVertex(kE.i0,kV0);
        rkP.GetVertex(kE.i1,kV1);
        
        iX0 = int(kV0.X());
        iY0 = GetWidth()-1-int(kV0.Y());
        iX1 = int(kV1.X());
        iY1 = GetWidth()-1-int(kV1.Y());

        DrawLine(iX0,iY0,iX1,iY1,kColor);
    }

    // draw vertices
    Color kBlack(0,0,0);
    for (i = 0; i < rkP.GetVertexQuantity(); i++)
    {
        rkP.GetVertex(i,kV0);
        iX0 = int(kV0.X());
        iY0 = GetWidth()-1-int(kV0.Y());
        for (int iDX = -1; iDX <= 1; iDX++)
        {
            for (int iDY = -1; iDY <= 1; iDY++)
                SetPixel(iX0+iDX,iY0+iDY,kBlack);
        }
    }
}
//----------------------------------------------------------------------------
void Boolean2D::DrawRegions (const Polygon2& rkPoly)
{
    Color kInside(0,0,255), kOutside(255,255,0), kOn(0,0,0);
    for (int iY = 0; iY < GetHeight(); iY++)
    {
        for (int iX = 0; iX < GetWidth(); iX++)
        {
            Vector2d kP((double)iX,(double)(GetWidth()-1-iY));
            switch ( rkPoly.PointLocation(kP) )
            {
            case -1:  SetPixel(iX,iY,kInside);  break;
            case  0:  SetPixel(iX,iY,kOn); break;
            case +1:  SetPixel(iX,iY,kOutside);  break;
            }
        }
    }
}
//----------------------------------------------------------------------------
void Boolean2D::DoBoolean ()
{
    Polygon2& P = *m_pkPoly0;
    Polygon2& Q = *m_pkPoly1;

    m_kIntersection = P & Q;
    m_kUnion        = P | Q;
    m_kDiff01       = P - Q;
    m_kDiff10       = Q - P;
    m_kXor          = P ^ Q;
}
//----------------------------------------------------------------------------
