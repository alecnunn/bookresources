/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Triangle Strip Creation, Optimizations, and Rendering Gem

Copyright (c) 1998-2002, Intel Corporation.
All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 
* Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer. 
* Redistributions in binary form must reproduce the above copyright notice, this 
list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution. 
* Neither the name of Intel Corporation nor the names of its contributors may be 
used to endorse or promote products derived from this software without specific 
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS'' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR 
JURISDICTION. It is each licensee's responsibility to comply with any export 
regulations applicable in each licensee's jurisdiction. 
*/
/*
*  TSInternal.h  : Triangle Strip Internal Header File
*
*  PURPOSE:
*   This file contains the prototypes for many of the functions 
*   used in the tri-strip generation algorithm.  It also contains
*   many useful vector operation funtions along with the hashing and 
*   reconnect data structures.
*    
*/

// Include files
#include <math.h>
#include "TS.h"

// Setup definitions
#ifndef FALSE
#define FALSE 0
#define TRUE 1
#define NULL -1
typedef int BOOL;
#endif // !FALSE

#define END_STRIP  -1  // End of a tri-strip
#define NO_FACE    -1  // No face found: a face is a triangle
#define NO_EDGE    -1  // No edge found

// A valid triangle is a triangle not included in a triangle strip
#define NO_VALID_TRIANGLES -99  // No valid triangles remain
#define EMPTY_SLOT  -9999		// Empty slot in the hash tables
#define INIT_MIN   999999999999.0; //Initialize minimum triangle area size  
#define SWAP_EDGE   0		    // Swap edge requires one repeated vertex
#define ACTIVE_EDGE 1			// Active edge requires zero repeated vertices
#define INVALID_EDGE -1			// Not a valid edge
#define D180_PI 57.29577951472  // 180 / pi
#define MAX_TRI_STRIP_LENGTH 5000 //Max # of Vertices per Tri-Strip
#define EPS 0.000001
#define NO_TEX_COORDS -9999

// Hash Table structures
struct TSHashEdges{
	int edgeNum;
	struct TSHashEdges *next;
};

struct TSHashFaces{
	int faceNum;
	struct TSHashFaces *next;
};
//  End of Hash Table structures


// Edge types for a triangle strip node
typedef enum {Start_Edge, End_Edge, Orphan_Edge} TSEdgeType;

class TSConnectList
{
public:
	struct TSNode *head;
	int connect;
};

struct TSNode{
	struct TSTriStrip *tstrip;
	int nfaces[3];   //Neighboring Faces
	TSEdgeType edgetype;
	int invalid;
	int smoothID, matID;
	unsigned long faceIndex;
	struct TSNode *next;
};

// Prototypes

//TSmetric
void	 TSCalculateTriangleCentroid(TSMesh *pTSMesh);
void	 TSDistancePtToCentroid(TSV3D *vinit, TSMesh *pTSMesh);
void	 TSInitializeTSMetric(long int *iFace, TSMesh *pTSMesh);
long int TSFindClusterCenter(TSMesh *pTSMesh);
float	 TSVectorLength(TSV3D* pV3d);
float	 TSTriangleArea(TSV3D* pV1, TSV3D *pV2, TSV3D *pV3);

//EdgetoFace Functions
void	 TSEdgeFaceTraversal(TSMesh *pTSMesh, TSParam *pTSParam);
void	 TSStoreFaceInfo(TSMesh *pTSMesh);
int		 TSPrevEdge(TSMesh *pTSMesh, int iNumEdges, TSEdge *pEdge);
int		 TSCompareEdge(TSEdge *pEdge1, TSEdge *pEdge2);
long int TSCreateEdgeFromFace(TSMesh *pTSMesh, struct TSHashEdges *pHashEdgeTable);
TSFace*  TSGetFacesFromEdge(TSMesh *pTSMesh, TSFace *pCurrentFace, TSEdge *pEdge);


//Hash Table Functions
void	 TSHashAddEdge(TSEdge *pEdge, long int iEdgeNum, struct TSHashEdges *pHashTable);
void	 TSFreeHash(struct TSHashEdges *pTmp);
long int TSHashEdgeFunc(TSEdge *pEdge);
long int TSHashFindEdge(TSMesh *pTSMesh, TSEdge *pEdge, struct TSHashEdges *pHashTable);

//Hash Duplicate Functions
void	 TSFreeHashDup(struct TSHashFaces *pTmp);
void	 TSHashAddDupFace(TSFace *pFace, long int iFaceIndex, struct TSHashFaces *pHashTable);
int		 TSFindDuplicateFaces(TSMesh *pTSMesh, struct TSHashFaces *pHashDupTable);
long int TSHashDupFunc(TSFace *pFace);
long int TSHashFindDupFace(TSMesh *pTSMesh, TSFace *pFace, struct TSHashFaces *pHashDupTable);

//Tri-Strip Algorithm Functions
void	 TSStoreIndices(TSMesh *pTSMesh, TSIndex *pIndexList, int *piLocation, long int iVertIndex, long int iFaceNum);
void	 TSCopyTSData(struct TSTriStrip *pTSList, TSIndex *pTSIndices, int iRunLength);
int		 TSGetFaceOrientation(TSEdge *pEdgeList, TSEdge *pEdge, TSFace *pFace);
int		 TSCheckDuplicateFace(TSFace *pFace1, TSFace *pFace2);
int		 TSMatchEdge(TSFace *pFace, long int iEdgeIndex1, long int iEdgeIndex2, TSMesh *pTSMesh);
int		 TSCompareUVCoords(TSUV *pV2d1, TSUV *pV2d2);
int		 TSCompareTexCoords(TSMesh *pTSMesh, TSFace *pValidFace, TSFace *pNextFace, short sTexFlag);
int		 TSCheckVertexInFace(TSFace *pFace, long int iVertexIndex);
long int TSFindNearestTriangle(long int iFaceIndex, TSMesh *pTSMesh, unsigned long uSmoothId, unsigned long uMatId, short sTexFlag);
long int TSFindNextValidTriangle(TSMesh *pTSMesh);
long int TSFindVertex3(long int iVertex1, long int iVertex2, TSFace *pFace);
long int TSFindNextFace(long int iFaceIndex, TSEdge *pEdge, TSMesh *pTSMesh, unsigned long uSmoothId, unsigned long uMatId, short sTexFlag);
struct TSTriStrip *TSCreateTriStrip();
TSRESULT TSTriStripMesh(long int iInitFace, TSMesh *pTSMesh, TSParam *pTSParam);


TSRESULT TSErrorChecking(TSMesh *pTSMesh, TSParam *pTSParameters, TSParam *pTSParam);

//Connect Prototypes
void	 TSRemoveInvalidStrips(struct TSTriStrip *pTSTmp);
void	 TSConnectTriStrips(TSMesh *pTSMesh, struct TSTriStrip *pLastStrip, TSConnectList *pTSList, TSParam *pTSParam);
void	 TSDeleteConnectStructures(TSMesh *pTSMesh, TSConnectList *pTSList);
void	 TSCreateConnectStructures(TSMesh *pTSMesh, struct TSTriStrip *pTSNode, TSConnectList *pTSList);
void	 TSConnectStrips(TSMesh *pTSMesh, TSParam *pTSParam);
void	 TSGetRNeighboringFaces(TSMesh *pTSMesh, TSFace *pCurrentFace, TSFace *pNextFace, int iFaces[3]);
int		 TSCombineTriStrips(TSMesh *pTSMesh, struct TSNode *pStrip1, struct TSNode *pStrip2, struct TSTriStrip **ppLastStrip, TSConnectList *pTSList, TSParam *pTSParam);
struct   TSNode *TSCreateTSNode();


// Vector Operation Functions
inline float TSDistance3D (TSV3D *v1, TSV3D *v2)
{
	float xDiff, yDiff, zDiff;

	xDiff = v1->x - v2->x;
	xDiff *= xDiff;
	yDiff = v1->y - v2->y;
	yDiff *= yDiff;
	zDiff = v1->z - v2->z;
	zDiff *= zDiff;

	return ( (float) sqrt (xDiff + yDiff + zDiff));
} 

inline void TSCrossprod(TSV3D* u, TSV3D* v, TSV3D *c){

	TSV3D n;

	n.x = u->y*v->z - u->z*v->y;
	n.y = u->z*v->x - v->z*u->x;
	n.z = u->x*v->y - u->y*v->x;

	*c = n;

} /* crossprod */

inline void TSSubtract3D (TSV3D *v1, TSV3D *v2, TSV3D *v3)
{
	v3->x = v1->x - v2->x;
	v3->y = v1->y - v2->y;
	v3->z = v1->z - v2->z;
}

inline void TSNormalize(TSV3D *pVector){
	float length = 1.0f / (float) sqrt(pVector->x * pVector->x + pVector->y * pVector->y + pVector->z * pVector->z);
	pVector->x *= length;
	pVector->y *= length;
	pVector->z *= length;
}