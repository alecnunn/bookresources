/* Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg ang Dave Gosselin, 2001"
 */
//  pof.h  - Version 0.1
//
//    Evan Hart - ATI Research - 5/2/2000
//
//   This is the header file used to define constants,
//  macros, and functions used with the Portable Object Format.
//
///////////////////////////////////////////////////////////////



#ifndef POF_H
#define POF_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "pofDefines.h"
#include "pofTypes.h"

#define POF_VERSION 10





/* File access modes */
#define POF_MODE_WRITE 1
#define POF_MODE_READ  2

/* library types */
typedef void POF;




/*********************************************/
/**************** Functions  *****************/
/*********************************************/



/*********************************************/


//general purpose functions
POF* pofOpen(const POFchar *filename, int mode);
void pofClose(POF *pof);

//read functions
int pofGetNumObjects(POF* pof, POFtype type);
const POFuint* pofGetObjectNames(POF* pof, POFtype type);
int pofGetNumInstances(POF* pof, POFtype type, POFuint name);
const POFuint* pofGetInstances(POF* pof, POFtype type, POFuint name);

const void* pofGetVertexList(POF* pof, POFuint name, POFuint instance, POFVertexChunk *head);
const void* pofGetNormalList(POF* pof, POFuint name, POFuint instance, POFNormalChunk *head);
const void* pofGetColorList(POF* pof, POFuint name, POFuint instance, POFColorChunk *head);
const void* pofGetTexCoordList(POF* pof, POFuint name, POFuint instance, POFTexCoordChunk *head);
const void* pofGetFaceList(POF* pof, POFuint name, POFFaceChunk *head);


//write functions
void pofPutVertexList(POF* pof, POFuint name, POFuint instance, const POFVertexChunk *head, const void *data);
void pofPutNormalList(POF* pof, POFuint name, POFuint instance, const POFNormalChunk *head, const void *data);
void pofPutColorList(POF* pof, POFuint name, POFuint instance, const POFColorChunk *head, const void *data);
void pofPutTexCoordList(POF* pof, POFuint name, POFuint instance, const POFTexCoordChunk *head, const void *data);
void pofPutFaceSet(POF* pof, POFuint name, const POFFaceChunk *head, const void *data);

#ifdef  __cplusplus
} // extern C
#endif

#endif