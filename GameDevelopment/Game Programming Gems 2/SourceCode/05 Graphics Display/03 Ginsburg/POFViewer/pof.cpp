/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//  pof.c  - Version 0.1
//
//    Evan Hart - ATI Research - 5/2/2000
//
//   This is the source file used to define constants,
//  macros, and functions used with the Portable Object Format.
//
///////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pof.h"
#include "pofLL.h"
#include "pofDefines.h"
#include "pofInternal.h"


//
//
/////////////////////////////////////////////////////////////////////
void pofReadFile(POF_Block* pof);


//
//
/////////////////////////////////////////////////////////////////////
POF* pofOpen(const POFchar *filename, int mode){
  POF_Block *file;
  POFChunkHeader head;
  POFbyte sig[8];

  file=new POF_Block();

  

  file->mode=mode;

  switch (mode){
    case POF_MODE_WRITE:
      file->fp=fopen(filename,"wb");
      if (!file->fp)
        break;
      head.type=POF_HEADER;
      head.length=sizeof(POFChunkHeader)+sizeof(POFHeaderChunk);
      head.flags=0;
      head.encoding=0;
      file->head.version=POF_VERSION;
      fwrite(pofSignature,sizeof(POFbyte),8,file->fp);
      fwrite(&head,sizeof(POFbyte),sizeof(POFChunkHeader),file->fp);
      fwrite(&file->head,sizeof(POFbyte),sizeof(POFHeaderChunk),file->fp);
      return file;
      break;
    case POF_MODE_READ:
      //presently unsupported
      file->fp=fopen(filename,"rb");
      if (!file->fp)
        break;
      fread(sig,sizeof(POFbyte),8,file->fp);
      if (memcmp(sig,pofSignature,sizeof(POFbyte)*8))
        break; //sig didn't match
      fread(&head,sizeof(POFbyte),sizeof(POFChunkHeader),file->fp);
      if (head.type!=POF_HEADER)
        break;
      fread(&file->head,sizeof(POFbyte),sizeof(POFHeaderChunk),file->fp);
      if (file->head.version>POF_VERSION)
        break; //new version can't deal
      fseek(file->fp,head.length-sizeof(POFChunkHeader)-sizeof(POFHeaderChunk),SEEK_CUR);
      pofReadFile(file);
      return file;
      break;
  };


  return NULL;
}

//
//
/////////////////////////////////////////////////////////////////////
void pofClose(POF *pof){
  POFChunkHeader temp;
  POF_Block *block=(POF_Block *)pof;

  if (block->mode==POF_MODE_WRITE){
    temp.type=POF_END;
    temp.length=sizeof(POFChunkHeader);
    temp.flags=0;
    temp.encoding=0;

    fwrite(&temp,sizeof(POFbyte),sizeof(POFChunkHeader),block->fp);
    fclose(block->fp);
  }

  

  delete block;
}

//
//
/////////////////////////////////////////////////////////////////////
void pofPutVertexList(POF* pof, POFuint name, POFuint instance, const POFVertexChunk *vhead, const void *data){
  POF_Block *block=(POF_Block*)pof;
  POFChunkHeader head;
  POFuint type, config;

  //initialize the chunk header
  head.type=POF_VERTEX;
  head.encoding=0; //uncompressed
  head.flags=0;
  head.name=(name & 0xffff) | ((instance & 0xffff) << 16);
  head.length=sizeof(POFChunkHeader)+sizeof(POFVertexChunk);

  type=(vhead->format >> 16) & 0xffff;
  config= vhead->format & 0xffff;

  if ((type!=POF_FLOAT) && (config!=POF_VERTEX_3))
    return; //data is not correctly formatted

  //data block size
  head.length+=vhead->count*3*sizeof(float);

  fwrite(&head,sizeof(POFbyte),sizeof(POFChunkHeader),block->fp);
  fwrite(vhead,sizeof(POFbyte),sizeof(POFVertexChunk),block->fp);
  fwrite(data,sizeof(POFbyte),3*sizeof(float)*vhead->count,block->fp);
}

//
//
/////////////////////////////////////////////////////////////////////
void pofPutNormalList(POF* pof, POFuint name, POFuint instance, const POFNormalChunk *nhead, const void *data){
  POF_Block *block=(POF_Block*)pof;
  POFChunkHeader head;
  POFuint type, config;

  //initialize the chunk header
  head.type=POF_NORMAL;
  head.encoding=0; //uncompressed
  head.flags=0;
  head.name=(name & 0xffff) | ((instance & 0xffff) << 16);
  head.length=sizeof(POFChunkHeader)+sizeof(POFNormalChunk);

  type=(nhead->format >> 16) & 0xffff;
  config= nhead->format & 0xffff;

  if ((type!=POF_FLOAT) && (config!=POF_NORMAL_3))
    return; //data is not correctly formatted

  //data block size
  head.length+=nhead->count*3*sizeof(float);

  fwrite(&head,sizeof(POFbyte),sizeof(POFChunkHeader),block->fp);
  fwrite(nhead,sizeof(POFbyte),sizeof(POFVertexChunk),block->fp);
  fwrite(data,sizeof(POFbyte),3*sizeof(float)*nhead->count,block->fp);
}

//
//
/////////////////////////////////////////////////////////////////////
void pofPutColorList(POF* pof, POFuint name, POFuint instance, const POFColorChunk *chead, const void *data){
  POF_Block *block=(POF_Block*)pof;
  POFChunkHeader head;
  POFuint type, config;

  //initialize the chunk header
  head.type=POF_COLOR;
  head.encoding=0; //uncompressed
  head.flags=0;
  head.name=(name & 0xffff) | ((instance & 0xffff) << 16);
  head.length=sizeof(POFChunkHeader)+sizeof(POFColorChunk);

  type=(chead->format >> 16) & 0xffff;
  config= chead->format & 0xffff;

  if ((type!=POF_FLOAT) && (config!=POF_COLOR_RGB))
    return; //data is not correctly formatted

  //data block size
  head.length+=chead->count*3*sizeof(float);

  fwrite(&head,sizeof(POFbyte),sizeof(POFChunkHeader),block->fp);
  fwrite(chead,sizeof(POFbyte),sizeof(POFVertexChunk),block->fp);
  fwrite(data,sizeof(POFbyte),3*sizeof(float)*chead->count,block->fp);
}

//
//
/////////////////////////////////////////////////////////////////////
void pofPutTexCoordList(POF* pof, POFuint name, POFuint instance, const POFTexCoordChunk *tchead, const void *data){
  POF_Block *block=(POF_Block*)pof;
  POFChunkHeader head;
  POFuint type, config;

  //initialize the chunk header
  head.type=POF_TEX_COORD;
  head.encoding=0; //uncompressed
  head.flags=0;
  head.name=(name & 0xffff) | ((instance & 0xffff) << 16);
  head.length=sizeof(POFChunkHeader)+sizeof(POFTexCoordChunk);

  type=(tchead->format >> 16) & 0xffff;
  config= tchead->format & 0xffff;

  if ((type!=POF_FLOAT) && (config!=POF_TEXCOORD_ST))
    return; //data is not correctly formatted

  //data block size
  head.length+=tchead->count*2*sizeof(float);

  fwrite(&head,sizeof(POFbyte),sizeof(POFChunkHeader),block->fp);
  fwrite(tchead,sizeof(POFbyte),sizeof(POFVertexChunk),block->fp);
  fwrite(data,sizeof(POFbyte),2*sizeof(float)*tchead->count,block->fp);
}

//
//
/////////////////////////////////////////////////////////////////////
void pofPutFaceSet(POF* pof, POFuint name, const POFFaceChunk *fshead, const void *data){
  POF_Block *block=(POF_Block*)pof;
  POFChunkHeader head;

  //initialize the chunk header
  head.type=POF_FACE_SET;
  head.encoding=0; //uncompressed
  head.flags=0;
  head.name=name;
  head.length=sizeof(POFChunkHeader)+sizeof(POFFaceChunk);

  //data block size
  head.length+=fshead->count*3*sizeof(unsigned int);

  fwrite(&head,sizeof(POFbyte),sizeof(POFChunkHeader),block->fp);
  fwrite(fshead,sizeof(POFbyte),sizeof(POFFaceChunk),block->fp);
  fwrite(data,sizeof(POFbyte),fshead->count*sizeof(unsigned int)*3,block->fp);
}


