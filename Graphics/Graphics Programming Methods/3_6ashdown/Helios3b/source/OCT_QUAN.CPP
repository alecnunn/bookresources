////////////////////////////////////////////////////////////
//
//  OCT_QUAN.CPP - Octree Color Quantization Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/11/10 - Modified InsertNode to properly
//                         mark nodes as reducible.
//              94/11/11 - Version 1.00B release.
//              94/11/27 - Converted to MS-Windows
//                         environment.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
//              96/11/23 - Added BYTE typecast to
//                         QuantizeColors return value in
//                         MapColors.
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

#include "oct_quan.h"

// Write device-independent bitmap (DIB) to file
// Quantize 24-bit RGB color bitmap
BOOL OctQuant::Quantize()
{
  if (BuildTree() == FALSE)     // Build octree
    return FALSE;

  // Initialize number of quantized colors
  num_colors = 0;

  // Set color palette entries
  FillPalette(proot, &num_colors);

  // Map 24-bit RGB colors to 8-bit color palette entries
  MapColors();

  DeleteTree();     // Delete octree

  return TRUE;
}

void OctQuant::DeleteTree()     // Delete octree
{
  int i;        // Loop index

  // Clear the reducible node list pointers
  for (i = 0; i < leaf_level; i++)
    prnl[i] = NULL;

  if (proot != NULL)    // Release octree nodes
  {
    DeleteNode(proot);
    proot = NULL;
  }

  // Reset octree leaf level
  leaf_level = O_MaxDepth + 1;

}

BOOL OctQuant::BuildTree()      // Build octree
{
  int row;              // Row counter
  int col;              // Column counter
  BOOL status = TRUE;   // Return status
  ColorRGB color;       // Pixel color

  // Allocate octree root node
  if ((proot = MakeNode(0)) == NULL)
    status = FALSE;

  // Build the octree
  if (status == TRUE)
  {
    for (row = 0; row < height; row++)
    {
      for (col = 0; col < width; col++)
      {
        GetPixel(col, row, &color);

        // Insert pixel color into octree
        if (InsertNode(proot, color) == FALSE)
        {
          DeleteNode(proot);      // Delete the octree
          status =  FALSE;
          break;
        }

        // Reduce octree if too many colors
        if (num_leaf > max_colors)
          ReduceTree();
      }

      if (status == FALSE)
        break;
    }
  }

  return status;
}

// Recursively delete octree nodes
void OctQuant::DeleteNode( OctNode *pn )
{
  int i;        // Loop index
  OctNode *pc;  // Child node pointer

  if (pn == NULL)
    return;

  if (pn->IsLeaf() == FALSE)
  {
    // Delete child nodes
    for (i = 0; i < 8; i++)
    {
      if ((pc = pn->GetChild(i)) != NULL)
      {
        DeleteNode(pc);
        pn->SetChild(i, NULL);
        pn->DecNumChild();
      }
    }
  }
  else
    num_leaf--;

  delete pn;
}

// Set color palette entries
void OctQuant::FillPalette( OctNode *pn, int *pindex )
{
  int i;    // Loop index

  // Perform recursive depth-first traversal of octree
  if (pn != NULL)
  {
    if ((pn->IsLeaf() == TRUE) || pn->GetLevel() ==
        leaf_level)
    {
      // Set color palette entry
      palette[*pindex].SetRed(pn->GetColor().GetRed());
      palette[*pindex].SetGreen(pn->GetColor().GetGreen());
      palette[*pindex].SetBlue(pn->GetColor().GetBlue());

      // Set node color palette index
      pn->SetIndex(*pindex);

      // Advance to next color palette entry
      *pindex = *pindex + 1;
    }
    else
    {
      // Visit child nodes
      for (i = 0; i < 8; i++)
        FillPalette(pn->GetChild(i), pindex);
    }
  }
}

// Get next reducible node pointer
OctNode *OctQuant::GetReducible()
{
  int new_level;        // New reducible node level
  OctNode *prn;         // Reducible node pointer
  OctNode *pscn = NULL; // Smallest pixel count node
  OctNode *pnext;       // Next node
  OctNode *pprev;       // Previous node

  new_level = leaf_level - 1;

  // Find lowest reducible node level 
  while (prnl[new_level] == NULL)
    new_level--;

  // Find node with smallest pixel count
  prn = prnl[new_level];
  while (prn != NULL)
  {
    if (pscn == NULL)
      pscn = prn;
    else if (prn->GetCount() < pscn->GetCount())
      pscn = prn;
    prn = prn->GetNext();
  }
  
  // Remove node from reducible list
  pnext = pscn->GetNext();
  pprev = pscn->GetPrev();
  if (pprev == NULL)
  {
    prnl[new_level] = pnext;
    if (pnext != NULL)
      pnext->SetPrev(NULL);
  }
  else
  {
    pprev->SetNext(pnext);
    if (pnext != NULL)
      pnext->SetPrev(pprev);
  }

  pscn->SetNext(NULL);
  pscn->SetPrev(NULL);
  pscn->SetMark(FALSE);
  
  return pscn;
}

void OctQuant::ReduceTree()     // Reduce octree
{
  int i;        // Loop index
  OctNode *pn;  // Node pointer
  OctNode *pc;  // Child node pointer

  pn = GetReducible();  // Get next reducible node

  // Delete children
  for (i = 0; i < 8; i++)
  {
    if ((pc = pn->GetChild(i)) != NULL)
    {
      DeleteNode(pc);
      pn->SetChild(i, NULL);
      pn->DecNumChild();
    }
  }

  pn->SetLeaf(TRUE);    // Mark node as leaf
  num_leaf++;           // Increment leaf count

  // Update reduction and leaf levels
  if (pn->GetLevel() < (leaf_level - 1))
    leaf_level = pn->GetLevel() + 1;
}

// Insert node into octree
BOOL OctQuant::InsertNode( OctNode *pn, ColorRGB &c )
{
  int c_index;          // Child index
  int level;            // Node level
  BOOL status = TRUE;   // Return status
  OctNode *pc;          // Child node pointer

  level = pn->GetLevel();       // Get node level
  pn->AddColor(c);              // Add RGB color to node

  if (pn->IsLeaf() == FALSE && level < leaf_level)
  {
    // Find child node
    c_index = pn->FindChild(c);

    if ((pc = pn->GetChild(c_index)) == NULL)
    {
      // Allocate child node
      if ((pc = MakeNode(level + 1)) == NULL)
        return FALSE;

      // Set child node pointer
      pn->SetChild(c_index, pc);

      pn->IncNumChild();      // Increment child count
    }

    if ((pn->GetNumChild() > 1) && (pn->IsMark() == FALSE))
    {
      // Mark node as candidate for reduction
      MakeReducible(pn);
    }

    // Insert child node into octree
    status = InsertNode(pc, c);
  }

  return status;
}

// Map 24-bit RGB colors to 8-bit color palette entries
void OctQuant::MapColors()
{
  int row;              // Row counter
  int col;              // Column counter
  BYTE qcolor;          // Quantized pixel color
  ColorRGB color;       // 24-bit RGB color

  // Quantize the bitmap colors
  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      // Get 24-bit RGB pixel color
      GetPixel(col, row, &color);
      
      // Get color palette index
      qcolor = (BYTE) QuantizeColor(proot, color);

      // Set 8-bit palette pixel color
      SetPalPixel(col, row, qcolor);
    }
  }
}

