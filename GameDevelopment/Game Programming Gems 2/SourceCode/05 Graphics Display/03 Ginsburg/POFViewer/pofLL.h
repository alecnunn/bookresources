/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//  pofLL.h  - Version 0.1
//
//    Evan Hart - ATI Research - 5/15/2000
//
//   This is the header file used to define the low-level
//  information necessary for POF access.
//
///////////////////////////////////////////////////////////////

#ifndef POFLL_H
#define POFLL_H

/* File Signature */
const unsigned char pofSignature[] =
{0x89, 0x41, 0x54, 0x49, 0x0d, 0x0a, 0x1a, 0x0a};

/* Chunk Macro */
//#define pofChunkType(a) ( (a[0] & 0xff) | ((a[1] << 8) & 0xff00) | ((a[2] << 16) & 0xff0000) | ((a[3] << 24) & 0xff000000) )


/* Standard Chunk Types */
const char POF_HEADER_TXT[] =      "OHDR";
const char POF_END_TXT[] =         "OEND";
const char POF_MATERIAL_TXT[] =    "MATL";
const char POF_VERTEX_TXT[] =      "VRTX";
const char POF_NORMAL_TXT[] =      "NRML";
const char POF_TEX_COORD_TXT[] =   "TEXC";
const char POF_COLOR_TXT[] =       "COLR";
const char POF_TEXT_TXT[] =        "TEXT";
const char POF_FACE_SET_TXT[] =    "FACE";

enum POFChunkType {
  POF_HEADER    = ('O' & 0xff) | (('H' << 8) & 0xff00) | (('D' << 16) & 0xff0000) | (('R' << 24) & 0xff000000),
  POF_END       = ('O' & 0xff) | (('E' << 8) & 0xff00) | (('N' << 16) & 0xff0000) | (('D' << 24) & 0xff000000),
  POF_MATERIAL  = ('M' & 0xff) | (('A' << 8) & 0xff00) | (('T' << 16) & 0xff0000) | (('L' << 24) & 0xff000000),
  POF_VERTEX    = ('V' & 0xff) | (('R' << 8) & 0xff00) | (('T' << 16) & 0xff0000) | (('X' << 24) & 0xff000000),
  POF_NORMAL    = ('N' & 0xff) | (('R' << 8) & 0xff00) | (('M' << 16) & 0xff0000) | (('L' << 24) & 0xff000000),
  POF_TEX_COORD = ('T' & 0xff) | (('E' << 8) & 0xff00) | (('X' << 16) & 0xff0000) | (('C' << 24) & 0xff000000),
  POF_COLOR     = ('C' & 0xff) | (('O' << 8) & 0xff00) | (('L' << 16) & 0xff0000) | (('R' << 24) & 0xff000000),
  POF_FACE_SET  = ('F' & 0xff) | (('A' << 8) & 0xff00) | (('C' << 16) & 0xff0000) | (('E' << 24) & 0xff000000),
  POF_TEXT      = ('T' & 0xff) | (('E' << 8) & 0xff00) | (('X' << 16) & 0xff0000) | (('T' << 24) & 0xff000000)
};


#endif