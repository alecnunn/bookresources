/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//  pofRead.cpp
//
//    Evan Hart - ATI Research Inc. - 5/23/00
//
//
//
/////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>

#include "pof.h"

#include "pofLL.h"
#include "pofDB.hpp"
#include "pofInternal.h"



void pofReadFile(POF_Block* pof){
  POFChunkHeader *head;
  POFMaterialChunk* mtl;
  POFVertexChunk* vtx;
  POFNormalChunk* nrm;
  POFColorChunk* clr;
  POFTexCoordChunk* tex;
  POFFaceChunk* face;
  POFMaterialRecord* mrec;
  POFVertexRecord* vrec;
  POFNormalRecord* nrec;
  POFTexCoordRecord* tcrec;
  POFColorRecord* crec;
  POFFaceRecord* frec;
  void *data;

  do{
    head=new POFChunkHeader;
    fread(head,sizeof(char),sizeof(POFChunkHeader),pof->fp);
    switch (head->type){
      case POF_VERTEX:
        vrec=new POFVertexRecord;
        vtx=new POFVertexChunk;
        fread(vtx,sizeof(char),sizeof(POFVertexChunk),pof->fp);
        data=(void *)new char[head->length-sizeof(POFChunkHeader)-sizeof(POFVertexChunk)];
        fread(data,sizeof(char),head->length-sizeof(POFChunkHeader)-sizeof(POFVertexChunk),pof->fp);
        vrec->data=data;
        vrec->head=head;
        vrec->vert=vtx;
        (pof->vertDB).AddObject(head->name&0xffff,(head->name&0xffff0000)>>16,vrec);
        break;
      case POF_NORMAL:
        nrec=new POFNormalRecord;
        nrm=new POFNormalChunk;
        fread(nrm,sizeof(char),sizeof(POFNormalChunk),pof->fp);
        data=(void *)new char[head->length-sizeof(POFChunkHeader)-sizeof(POFNormalChunk)];
        fread(data,sizeof(char),head->length-sizeof(POFChunkHeader)-sizeof(POFNormalChunk),pof->fp);
        nrec->data=data;
        nrec->head=head;
        nrec->norm=nrm;
        (pof->normDB).AddObject(head->name&0xffff,(head->name&0xffff0000)>>16,nrec);
        break;
      case POF_TEX_COORD:
        tcrec=new POFTexCoordRecord;
        tex=new POFTexCoordChunk;
        fread(tex,sizeof(char),sizeof(POFTexCoordChunk),pof->fp);
        data=(void *)new char[head->length-sizeof(POFChunkHeader)-sizeof(POFTexCoordChunk)];
        fread(data,sizeof(char),head->length-sizeof(POFChunkHeader)-sizeof(POFTexCoordChunk),pof->fp);
        tcrec->data=data;
        tcrec->head=head;
        tcrec->tex=tex;
        (pof->texCoordDB).AddObject(head->name&0xffff,(head->name&0xffff0000)>>16,tcrec);
        break;
      case POF_COLOR:
        crec=new POFColorRecord;
        clr=new POFColorChunk;
        fread(clr,sizeof(char),sizeof(POFColorChunk),pof->fp);
        data=(void *)new char[head->length-sizeof(POFChunkHeader)-sizeof(POFColorChunk)];
        fread(data,sizeof(char),head->length-sizeof(POFChunkHeader)-sizeof(POFColorChunk),pof->fp);
        crec->data=data;
        crec->head=head;
        crec->color=clr;
        (pof->colorDB).AddObject(head->name&0xffff,(head->name&0xffff0000)>>16,crec);
        break;
      case POF_FACE_SET:
        frec=new POFFaceRecord;
        face=new POFFaceChunk;
        fread(face,sizeof(char),sizeof(POFFaceChunk),pof->fp);
        data=(void *)new char[head->length-sizeof(POFChunkHeader)-sizeof(POFFaceChunk)];
        fread(data,sizeof(char),head->length-sizeof(POFChunkHeader)-sizeof(POFFaceChunk),pof->fp);
        frec->data=data;
        frec->head=head;
        frec->face=face;
        (pof->faceDB).AddObject(head->name,frec);
        break;
      case POF_END:
        //skip the unknown stuff
        fseek(pof->fp,head->length-sizeof(POFChunkHeader),SEEK_CUR);
        break;
      case POF_MATERIAL:
      case POF_TEXT:
      
        //presently part of the default case
      default:
        //skip the unknown stuff
        fseek(pof->fp,head->length-sizeof(POFChunkHeader),SEEK_CUR);
        delete head;
        break;
    }
  } while (head->type!=POF_END);
  delete head;

  fclose(pof->fp);
}

int pofGetNumObjects(POF* pof, POFtype type){
  POF_Block *block=(POF_Block *)pof;

  switch (type){
    case POF_TYPE_MATERIAL:
      return (block->matDB).GetNumObjects();
      break;
    case POF_TYPE_VERTEX:
      return (block->vertDB).GetNumNames();
      break;
    case POF_TYPE_NORMAL:
      return (block->normDB).GetNumNames();
      break;
    case POF_TYPE_COLOR:
      return (block->colorDB).GetNumNames();
      break;
    case POF_TYPE_TEX_COORD:
      return (block->texCoordDB).GetNumNames();
      break;
    case POF_TYPE_FACE:
      return (block->faceDB).GetNumObjects();
      break;
  };
  return 0;
}

const POFuint* pofGetObjectNames(POF* pof, POFtype type){
  POF_Block *block=(POF_Block *)pof;

  switch (type){
    case POF_TYPE_MATERIAL:
      return (block->matDB).GetNames();
      break;
    case POF_TYPE_VERTEX:
      return (block->vertDB).GetNames();
      break;
    case POF_TYPE_NORMAL:
      return (block->normDB).GetNames();
      break;
    case POF_TYPE_COLOR:
      return (block->colorDB).GetNames();
      break;
    case POF_TYPE_TEX_COORD:
      return (block->texCoordDB).GetNames();
      break;
    case POF_TYPE_FACE:
      return (block->faceDB).GetNames();
      break;
  }

  return NULL;
}

int pofGetNumInstances(POF* pof, POFtype type, POFuint name){
  POF_Block *block=(POF_Block *)pof;

  switch (type){
    case POF_TYPE_MATERIAL:
      //no instances
      break;
    case POF_TYPE_VERTEX:
      return (block->vertDB).GetNumInstances(name);
      break;
    case POF_TYPE_NORMAL:
      return (block->normDB).GetNumInstances(name);
      break;
    case POF_TYPE_COLOR:
      return (block->colorDB).GetNumInstances(name);
      break;
    case POF_TYPE_TEX_COORD:
      return (block->texCoordDB).GetNumInstances(name);
      break;
  }

  return 0;
}

const POFuint* pofGetInstances(POF* pof, POFtype type, POFuint name){
  POF_Block *block=(POF_Block *)pof;

  switch (type){
    case POF_TYPE_MATERIAL:
      //no instances
      break;
    case POF_TYPE_VERTEX:
      return (block->vertDB).GetInstances(name);
      break;
    case POF_TYPE_NORMAL:
      return (block->normDB).GetInstances(name);
      break;
    case POF_TYPE_COLOR:
      return (block->colorDB).GetInstances(name);
      break;
    case POF_TYPE_TEX_COORD:
      return (block->texCoordDB).GetInstances(name);
      break;
  }

  return NULL;
}

const void* pofGetVertexList(POF* pof, POFuint name, POFuint instance, POFVertexChunk *head){
  POF_Block *block=(POF_Block *)pof;
  const POFVertexRecord *rec;

  rec=(block->vertDB).GetObject(name,instance);

  if (!rec)
    return NULL;

  memcpy(head,rec->vert,sizeof(POFVertexChunk));

  return rec->data;
}

const void* pofGetNormalList(POF* pof, POFuint name, POFuint instance, POFNormalChunk *head){
  POF_Block *block=(POF_Block *)pof;
  const POFNormalRecord *rec;

  rec=(block->normDB).GetObject(name,instance);

  if (!rec)
    return NULL;

  memcpy(head,rec->norm,sizeof(POFNormalChunk));

  return rec->data;
}

const void* pofGetColorList(POF* pof, POFuint name, POFuint instance, POFColorChunk *head){
  POF_Block *block=(POF_Block *)pof;
  const POFColorRecord *rec;

  rec=(block->colorDB).GetObject(name,instance);

  if (!rec)
    return NULL;

  memcpy(head,rec->color,sizeof(POFColorChunk));

  return rec->data;
}

const void* pofGetTexCoordList(POF* pof, POFuint name, POFuint instance, POFTexCoordChunk *head){
  POF_Block *block=(POF_Block *)pof;
  const POFTexCoordRecord *rec;

  rec=(block->texCoordDB).GetObject(name,instance);

  if (!rec)
    return NULL;

  memcpy(head,rec->tex,sizeof(POFTexCoordChunk));

  return rec->data;
}

const void* pofGetFaceList(POF* pof, POFuint name, POFFaceChunk *head){
  POF_Block *block=(POF_Block *)pof;
  const POFFaceRecord *rec;

  rec=(block->faceDB).GetObject(name);

  if (!rec)
    return NULL;

  memcpy(head,rec->face,sizeof(POFFaceChunk));

  return rec->data;
}