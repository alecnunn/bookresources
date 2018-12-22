////////////////////////////////////////////////////////////
//
//  OCT_QUAN.H - Octree Color Quantization Class Include
//               File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/11/11 - Version 1.00B release.
//              94/11/27 - Converted to MS-Windows
//                         environment.
//              94/12/16 - Version 1.01A release.
//              94/12/19 - Added GetNumColors function.
//              95/02/05 - Version 1.02A release.
//              95/02/11 - Simplified TestBit function.
//                       - Modified O_MaxDepth to account
//                         for 6-bit color display adapters.
//                       - Modified FindChild to eliminate
//                         dependence on O_MaxDepth.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
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

#ifndef _OCT_QUAN_H
#define _OCT_QUAN_H

#include "color.h"

// Maximum octree reduction level
//
// NOTE: Most 256-color display adapters are only capable 
//       of generating 262,144 distinct colors (64 colors
//       per 6-bit color channel). Since this effectively
//       truncates an 8-bit color value to 6 bits, the
//       octree depth need not exceed five levels below
//       the root node.

static const int O_MaxDepth = 5;

// Maximum number of palette colors
static const int O_MaxColors = 256;

class OctNode           // Octree node
{                       
  private:
    int level;          // Node level
    BOOL leaf_flag;     // Leaf flag
    BOOL mark_flag;     // Marked flag
    DWORD count;        // Pixel count
    struct
    {
      DWORD red;
      DWORD green;
      DWORD blue;
    }
    sum;                // Summed color value
    int index;          // Color palette index
    int children;       // Number of child nodes
    OctNode *pchild[8]; // Children node pointers
    OctNode *pnext;     // Next reducible node pointer
    OctNode *pprev;     // Previous reducible node pointer

    int TestBit(BYTE val, int index)
    { return ((val >> index) & 0x01); }

  public:
    OctNode( int node_level, BOOL leaf )
    {
      int i;    // Loop index

      level = node_level;
      leaf_flag = leaf;
      mark_flag = FALSE;
      count = 0L;
      index = 0;
      children = 0;
      sum.red = sum.green = sum.blue = 0L;

      for (i = 0; i < 8; i++)
        pchild[i] = NULL;

      pnext = pprev = NULL;
    };

    BOOL IsLeaf() { return leaf_flag; }
    BOOL IsMark() { return mark_flag; }
    
    DWORD GetCount() { return count; }

    ColorRGB GetColor()
    {
      ColorRGB temp;    // Temporary color

      temp.SetRed((BYTE) (sum.red / count));
      temp.SetGreen((BYTE) (sum.green / count));
      temp.SetBlue((BYTE) (sum.blue / count));

      return temp;
    }

    int GetIndex() { return index; }
    int GetLevel() { return level; }

    // Determine child node according to color
    int FindChild( ColorRGB &c )
    {
      int index;    // Child node pointer index

      // Determine child node pointer index
      index = TestBit(c.GetRed(), (7 - level)) << 2 |
          TestBit(c.GetGreen(), (7 - level)) << 1 |
          TestBit(c.GetBlue(), (7 - level));

      return index;
    }

    OctNode *GetChild( int i ) { return pchild[i]; }
    OctNode *GetNext() { return pnext; }
    OctNode *GetPrev() { return pprev; }
    int GetNumChild() { return children; }

    // Add RGB color to node
    void AddColor( ColorRGB &c )
    {
      sum.red += (DWORD) c.GetRed();
      sum.green += (DWORD) c.GetGreen();
      sum.blue += (DWORD) c.GetBlue();

      count++;
    }

    void DecNumChild() { children--; }
    void IncNumChild() { children++; }
    void SetChild( int i, OctNode *pc ) { pchild[i] = pc; }
    void SetIndex( int i ) { index = i; }
    void SetLeaf( BOOL flag ) { leaf_flag = flag; }
    void SetMark( BOOL flag ) { mark_flag = flag; }
    void SetNext( OctNode *pn ) { pnext = pn; }
    void SetPrev( OctNode *pn ) { pprev = pn; }
};

class OctQuant          // Octree color quantization
{
  private:
    int leaf_level;     // Leaf level
    int num_leaf;       // Number of leaf nodes

    // Reducible node list pointers
    OctNode *prnl[O_MaxDepth + 1];

    OctNode *proot;     // Octree root node pointer

    BOOL BuildTree();
    BOOL InsertNode( OctNode *, ColorRGB & );

    OctNode *MakeNode( int level )
    {
      BOOL leaf;    // Leaf node flag

      leaf = (level >= leaf_level) ? TRUE : FALSE;

      if (leaf == TRUE)
        num_leaf++;

      return new OctNode(level, leaf );
    }

    OctNode *GetReducible();

    // Quantize color
    int QuantizeColor( OctNode *pn, ColorRGB &c )
    {
      int c_index;      // Child node pointer index

      if ((pn->IsLeaf() == TRUE) || pn->GetLevel() ==
          leaf_level)
        return pn->GetIndex();
      else
      {
        c_index = pn->FindChild(c);

        return QuantizeColor(pn->GetChild(c_index), c);
      }
    }

    void DeleteNode( OctNode * );
    void DeleteTree();
    void FillPalette( OctNode *, int * );

    // Add node to reducible list
    void MakeReducible( OctNode *pn )
    {
      int level;        // Node level
      OctNode *phead;   // List head node
      
      level = pn->GetLevel();
      phead = prnl[level];
      pn->SetNext(phead);
      if (phead != NULL)
        phead->SetPrev(pn);
      prnl[level] = pn;

      pn->SetMark(TRUE);
    }
    
    void MapColors();
    void ReduceTree();

  protected:
    int height;         // Bitmap height
    int width;          // Bitmap width
    int max_colors;     // Maximum number of colors
    int num_colors;     // Number of palette entries

    // Bitmap color palette
    ColorRGB palette[O_MaxColors];

    virtual void GetPixel( int, int, ColorRGB * ) = 0;
    virtual void SetPalPixel( int, int, BYTE & ) = 0;

  public:
    OctQuant()
    {
      int i;    // Loop index

      width = height = 0;
      num_leaf = 0;
      num_colors = 0;
      leaf_level = O_MaxDepth + 1;

      // Clear the reducible node list pointers
      for (i = 0; i < leaf_level; i++)
        prnl[i] = NULL;

      proot = NULL;
    }

    BOOL Quantize();
    int GetNumColors() { return num_colors; }
};

#endif

