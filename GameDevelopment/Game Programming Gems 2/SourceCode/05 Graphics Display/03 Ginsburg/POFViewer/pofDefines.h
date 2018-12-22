/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//  pofDefines.h  - Version 0.1
//
//    Evan Hart - ATI Research - 5/15/2000
//
//   This is the header file used to define constants,
//  macros, and functions used with the Portable Object Format.
//
///////////////////////////////////////////////////////////////


#ifndef POFDEFINES_H
#define POFDEFINES_H

//types
#define POF_FLOAT           0x0001
#define POF_UBYTE           0x0002
#define POF_BYTE            0x0003

//configurations
#define POF_VERTEX_2        0x0001
#define POF_VERTEX_3        0x0002
#define POF_VERTEX_4        0x0003

#define POF_NORMAL_3        0x0004

#define POF_TEXCOORD_S      0x0005
#define POF_TEXCOORD_ST     0x0006
#define POF_TEXCOORD_STR    0x0007
#define POF_TEXCOORD_STQ    0x0008
#define POF_TEXCOORD_STRQ   0x0009

#define POF_COLOR_RGB       0x000a
#define POF_COLOR_RGBA      0x000b

//tex coord instances
#define POF_TCI_BASE        0x0000
#define POF_TCI_LIGHT       0x0001
#define POF_TCI_DETAIL      0x0002
#define POF_TCI_BUMP        0x0003
#define POF_TCI_GLOSS       0x0004
#define POF_TCI_EMISSIVE    0x0005

#define POF_TCI_AUX0        0x0100
#define POF_TCI_AUX1        0x0101
#define POF_TCI_AUX2        0x0102
#define POF_TCI_AUX3        0x0103
#define POF_TCI_AUX4        0x0104
#define POF_TCI_AUX5        0x0105
#define POF_TCI_AUX6        0x0106
#define POF_TCI_AUX7        0x0107

#define POF_TCI_TAN0        0x0200
#define POF_TCI_TAN1        0x0201
#define POF_TCI_TAN2        0x0202
#define POF_TCI_TAN3        0x0203
#define POF_TCI_TAN4        0x0204
#define POF_TCI_TAN5        0x0205
#define POF_TCI_TAN6        0x0206
#define POF_TCI_TAN7        0x0207

#define POF_TCI_BINORM0     0x0300
#define POF_TCI_BINORM1     0x0301
#define POF_TCI_BINORM2     0x0302
#define POF_TCI_BINORM3     0x0303
#define POF_TCI_BINORM4     0x0304
#define POF_TCI_BINORM5     0x0305
#define POF_TCI_BINORM6     0x0306
#define POF_TCI_BINORM7     0x0307

//color instances
#define POF_CI_DIFFUSE      0x0000
#define POF_CI_SPECULAR     0x0001
#define POF_CI_AMBIENT      0x0002
#define POF_CI_EMISSIVE     0x0003

#define POF_CI_AUX0         0x0100
#define POF_CI_AUX1         0x0101
#define POF_CI_AUX2         0x0102
#define POF_CI_AUX3         0x0103
#define POF_CI_AUX4         0x0104
#define POF_CI_AUX5         0x0105
#define POF_CI_AUX6         0x0106
#define POF_CI_AUX7         0x0107


#endif

