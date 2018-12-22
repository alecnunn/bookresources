/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//  pofTypes.h  - Version 0.1
//
//    Evan Hart - ATI Research - 5/11/2000
//
//   This is the header file used to define constants,
//  macros, and functions used with the Portable Object Format.
//
///////////////////////////////////////////////////////////////

#ifndef POF_TYPES_H
#define POF_TYPES_H


/*********************************************/
/*************** Simple Types ****************/
/*********************************************/
typedef unsigned char POFbyte;
typedef char POFchar;
typedef unsigned int POFuint;
typedef float POFfloat;

enum POFtype {POF_TYPE_MATERIAL, POF_TYPE_VERTEX, POF_TYPE_NORMAL,
              POF_TYPE_COLOR, POF_TYPE_TEX_COORD, POF_TYPE_FACE
};


/*********************************************/
/************** Compound Types ***************/
/*********************************************/

/* Chunk Header */
typedef struct _POFChunkHeader{
  POFuint type;
  POFuint length;
  POFuint flags;
  POFuint encoding;
  POFuint name;
} POFChunkHeader;

/* Header */
typedef struct _POFHeaderChunk{
  POFuint version;
}POFHeaderChunk;

/* Material */
typedef struct _POFMaterialChunk{
  POFuint  subtype;
  POFfloat ambient[4];
  POFfloat diffuse[4];
  POFfloat specular[4];
  POFfloat shininess;
  POFchar  texture[64];
} POFMaterialChunk;

/* Vertex List */
typedef struct _POFVertexChunk{
  POFuint count;
  POFuint format;
} POFVertexChunk;

/* Normal List */
typedef struct _POFNormalChunk{
  POFuint count;
  POFuint format;
} POFNormalChunk;

/* Texture Coordinates List */
typedef struct _POFTexCoordChunk{
  POFuint count;
  POFuint format;
} POFTexCoordChunk;

/* Color List */
typedef struct _POFColorChunk{
  POFuint count;
  POFuint format;
} POFColorChunk;


/* Indexed Face Chunk */
typedef struct _POFFaceChunk{
  POFuint count;
  POFuint coordSet;
  POFuint material;
} POFFaceChunk;

#endif