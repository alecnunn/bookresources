/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//  pofInternal.h
//
//    Evan Hart - ATI Research Inc. - 5/23/00
//
//
//
/////////////////////////////////////////////////////////////

#ifndef POF_INTERNAL_H
#define POF_INTERNAL_H

#include <stdio.h>

#include "pofTypes.h"
#include "pofDB.hpp"

struct POFMaterialRecord{
  POFChunkHeader *head;
  POFMaterialChunk *mtl;
  void *data;
};

struct POFVertexRecord{
  POFChunkHeader *head;
  POFVertexChunk *vert;
  void *data;
};

struct POFNormalRecord{
  POFChunkHeader *head;
  POFNormalChunk *norm;
  void *data;
};

struct POFColorRecord{
  POFChunkHeader *head;
  POFColorChunk *color;
  void *data;
};

struct POFTexCoordRecord{
  POFChunkHeader *head;
  POFTexCoordChunk *tex;
  void *data;
};

struct POFFaceRecord{
  POFChunkHeader *head;
  POFFaceChunk *face;
  void *data;
};

typedef pofMap1D<POFMaterialRecord> POFMaterialDB;
typedef pofMap2D<POFVertexRecord> POFVertexDB;
typedef pofMap2D<POFNormalRecord> POFNormalDB;
typedef pofMap2D<POFColorRecord> POFColorDB;
typedef pofMap2D<POFTexCoordRecord> POFTexCoordDB;
typedef pofMap1D<POFFaceRecord> POFFaceDB;

//explicit instantiations
template class pofMap1D<POFMaterialRecord>;
template class pofMap2D<POFVertexRecord>;
template class pofMap2D<POFNormalRecord>;
template class pofMap2D<POFColorRecord>;
template class pofMap2D<POFTexCoordRecord>;
template class pofMap1D<POFFaceRecord>;

typedef struct _POF_Block{
  POFHeaderChunk head;
  FILE *fp;
  int mode;

  POFMaterialDB matDB;
  POFVertexDB   vertDB;
  POFNormalDB   normDB;
  POFColorDB    colorDB;
  POFTexCoordDB texCoordDB;
  POFFaceDB faceDB;
} POF_Block;

#endif