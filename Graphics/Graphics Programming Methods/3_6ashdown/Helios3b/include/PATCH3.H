////////////////////////////////////////////////////////////
//
//  PATCH3.H - 3-D Patch Classes
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              96/11/17 - Added float typecast to area
//                         initialization.
//              97/04/01 - Version 1.03B release.
//
//  Compilers:  Microsoft Visual C/C++ Professional V5.0
//              Borland C++ Version 5.0
//
//  Author:     Ian Ashdown, P.Eng.
//              byHeart Consultants Limited
//              620 Ballantree Road
//              West Vancouver, B.C.
//              Canada V7S 1W3
//              Tel. (604) 922-6148
//              Fax. (604) 987-7621
//
//  Copyright 1994-2003 byHeart Consultants Limited
//
//  The following source code has been derived from:
//
//    Ashdown, I. 1994. Radiosity: A Programmer's
//    Perspective. New York, NY: John Wiley & Sons.
//
//    (Available on CD-ROM from www.helios32.com.)
//
//  It may be freely copied, redistributed, and/or modified
//  for personal use ONLY, as long as the copyright notice
//  is included with all source code files.
//
////////////////////////////////////////////////////////////

#ifndef _PATCH3_H
#define _PATCH3_H

#include "vector3.h"
#include "color.h"

#define QUAD_FLAG   0x01    // Quadrilateral flag

class Surface3;             // External reference

class ElemList              // Element list
{
  private:
    class Element3 *pelem;  // Element pointer
    ElemList *pnext;        // Next element list pointer

  public:
    ElemList( Element3 *pe, ElemList *pel )
    { pelem = pe; pnext = pel; }

    Element3 *GetElemPtr() { return pelem; }
    ElemList *GetNext() { return pnext; }
};

class PatchList             // Patch list
{
  private:
    class Patch3 *ppatch;   // Patch pointer
    PatchList *pnext;       // Next patch list pointer

  public:
    PatchList( Patch3 *pp, PatchList *ppl )
    { ppatch = pp; pnext = ppl; }

    Patch3 *GetPatchPtr() { return ppatch; }
    PatchList *GetNext() { return pnext; }
};

class Vertex3           // 3-D vertex
{
  private:
    Point3 posn;        // Vertex co-ordinates
    Vector3 normal;     // Vertex normal
    Spectra exitance;   // Vertex exitance
    ElemList *pelhd;    // Element list head pointer
    Vertex3 *pnext;     // Next vertex pointer

  public:
    Vertex3( Point3 &coord )
    {
      posn = coord;
      normal = 0.0;
      pelhd = NULL;
      pnext = NULL;
      exitance.Reset();
    }

    ~Vertex3()
    {
      ElemList *pel = pelhd;
      ElemList *pelnext;

      // Delete element list
      while (pel != NULL)
      {
        pelnext = pel->GetNext();
        delete pel;
        pel = pelnext;
      }
    }

    ElemList *GetElemListPtr() { return pelhd; }
    Point3 &GetPosn() { return posn; }
    Point3 *GetPosnPtr() { return &posn; }
    Spectra &GetExitance() { return exitance; }
    Vector3 &GetNormal() { return normal; }
    Vertex3 *GetNext() { return pnext; }
    void CalcNormal();
    void SetExitance( Spectra &e ) { exitance = e; }
    void SetElemListPtr( ElemList *ppl) { pelhd = ppl; }
    void SetNext( Vertex3 *pn ) { pnext = pn; }
    void SetPosn( Point3 &p ) { posn = p; }
};

class Element3                  // 3-D element
{
  protected:
    BYTE flags;                 // Flags bitmap
    float area;                 // Element area
    Patch3 *ppatch;             // Parent patch pointer
    Spectra exitance;           // Spectral exitance
    Vector3 normal;             // Normal vector
    Vertex3 *pvertex[4];        // Vertex pointer array
    Element3 *pnext;            // Next element pointer

  public:
    Element3( Vertex3 *pvtx[4], Patch3 *pp )
    {
      int index;        // Array index

      ppatch = pp;
      area = (float) 0.0;
      flags = (BYTE) 0;
      pnext = NULL;
      exitance.Reset();

      for (index = 0; index < 4; index++)
        pvertex[index] = pvtx[index];
    }

    BOOL IsQuad() { return (flags & QUAD_FLAG); }
    double GetArea() { return area; }
    int GetNumVert()
    { return (flags & QUAD_FLAG) ? 4 : 3; }
    Element3 *GetNext() { return pnext; }
    Patch3 *GetParentPtr() { return ppatch; }
    Spectra &GetExitance() { return exitance; }
    Vector3 &GetNormal() { return normal; }
    Vertex3 *GetVertexPtr( int i ) { return pvertex[i]; }
    void CalcArea();
    void CalcNormal();
    void SetExitance( Spectra &e ) { exitance = e; }
    void SetNext( Element3 *pn ) { pnext = pn; }
    void SetQuad() { flags |= QUAD_FLAG; }
};

class Patch3 : public Element3  // 3-D patch
{
  private:
    Point3 center;      // Patch center
    Element3 *pelhd;    // Element list head ptr
    Surface3 *psurf;    // Parent surface pointer

  public:
    Patch3( Vertex3 *pvtx[4], Surface3 *ps ) :
        Element3( pvtx, NULL )
    {
      pelhd = NULL;
      psurf = ps;
    }

    ~Patch3()
    {
      Element3 *pe = pelhd;
      Element3 *penext;

      while (pe != NULL)        // Delete elements
      {
        penext = pe->GetNext();
        delete pe;
        pe = penext;
      }
    }

    double GetUnsentFlux()
    {
      return ((exitance.GetRedBand() +
          exitance.GetGreenBand() + exitance.GetBlueBand())
          * (double) area);
    }

    Element3 *GetElementPtr() { return pelhd; }
    Patch3 *GetNext() { return (Patch3 *) pnext; }
    Point3 &GetCenter() { return center; }
    Surface3 *GetParentPtr() { return psurf; }
    void CalcCenter();
    void SetElementPtr( Element3 *pe ) { pelhd = pe; }
};

#endif

