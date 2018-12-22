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
*  TS.cpp  : Triangle Strip Implementation File
*
*  PURPOSE:
*	Core logic behind stripifying a given 3D model
*    
*/

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#define NULL 0
typedef int BOOL;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "TSInternal.h"

// Globals
static long int HASHSIZE;			// Size of the Hash Table
TSIndex *tIndices;					// Temporary Storage for Each Tri-Strip
unsigned long progressNumFaces = 0; // # of Faces processed for progress bar


/*---------------------------------------------------------------*
TSGenerate:

	Checks parameters for errors. Creates an edge list from the
	faces by using a hash table. Generates the tri-strips from 
	the 3D-model and then tries to reconnect any possible
	tri-strips. Then frees up memory.

	Returns: 
	TSRESULT - success or failure(what type)

	Parameters:
	TSMesh *pTSMesh - Where tri-strips are stored after creation.
	TSParam *pTSParameters - tri-strip algo. options
*----------------------------------------------------------------*/
TSRESULT TSGenerate(TSMesh *pTSMesh, TSParam *pTSParameters)
{
	TSRESULT tsFlag;
	TSParam *pTSParam;
	long int iFace;		// Initial Face Index

	pTSParam = (TSParam *) malloc(sizeof(TSParam));
	//---------------------------------------------------
	// Error Checking
	if((tsFlag = TSErrorChecking(pTSMesh, pTSParameters, pTSParam)) != TS_SUCCESS){
		return tsFlag;
	}

	TSEdgeFaceTraversal(pTSMesh, pTSParam);

	TSInitializeTSMetric(&iFace, pTSMesh);

	tIndices = (TSIndex *) malloc(MAX_TRI_STRIP_LENGTH * sizeof(TSIndex));

	// Create Triangle Strips
	if((tsFlag = TSTriStripMesh(iFace, pTSMesh, pTSParam)) != TS_SUCCESS){
		free(pTSParam);
		free(pTSMesh->pEdge);
		free(tIndices);
		pTSParam = NULL;
		pTSMesh->pEdge = NULL;
		tIndices = NULL;
		return tsFlag;
	}

	if(pTSParam->bConnectFlag)
		TSConnectStrips(pTSMesh, pTSParam);

	if(pTSMesh->pEdge){
		free(pTSMesh->pEdge);
		pTSMesh->pEdge = NULL;
	}

	free(pTSParam);
	free(tIndices);
	pTSParam = NULL;
	tIndices = NULL;

	return TS_SUCCESS;
}


/*---------------------------------------------------------------*
TSTriStripMesh:

	This is the core function for taking the input faces of a 
	3D mesh and creating tri-strips from those faces.
	
	Returns: 
	TSRESULT - success or failure(what type)

	Parameters:
	long int iInitFace - beginning face to start the tri-strip algo.
	TSMesh *pTSMesh - contains tri-strip info.
	TSParam *pTSParam - tri-strip algo. options
*----------------------------------------------------------------*/
TSRESULT TSTriStripMesh(long int iInitFace, TSMesh *pTSMesh, TSParam *pTSParam){
	int endstrip=0;					// Continue Tri-Strip?
	int location=0;					// Location within tri-strip array
	long int validFace=iInitFace;	// current face
	long int nextFace;				// next face to add
	long int fVertex;				// First Vertex of a Face
	long int midVertex;				// Middle Vertex of a Face
	long int lVertex;				// Last Vertex of a Face
	long int tmpVertex;				// Temporary Vertex of a Face
	unsigned long smID;				// Smoothing Group ID
	unsigned long matID;			// Material ID
	TSFace *ftslist = pTSMesh->pFace;
	
	// Create Linked List of Triangle Strips
	pTSMesh->tslist = TSCreateTriStrip();
	pTSMesh->tshead = pTSMesh->tslist;

	// Loop Until No Valid Triangles Remain
	do{
		// Create Tri-Strip
		pTSMesh->tslist->next = TSCreateTriStrip();
		pTSMesh->tslist = pTSMesh->tslist->next;
		pTSMesh->tslist->next = NULL;
		pTSMesh->tslist->indices = NULL;
		pTSMesh->tslist->invalid = 0;

		// Store Initial Face
		ftslist[validFace].iInvalid = 1;
		fVertex = ftslist[validFace].a;
		midVertex = ftslist[validFace].b;
		lVertex = ftslist[validFace].c;

		// Get Smoothing Group and Material IDs
		if((smID = ftslist[validFace].uSmoothId) < 0)
			return TS_INVALID_SMGROUP;

		if((matID = ftslist[validFace].uMatId) < 0)
			return TS_INVALID_MATID;

		pTSMesh->tslist->matID = matID;

		if((nextFace = TSFindNearestTriangle(validFace, pTSMesh, smID, matID, pTSParam->texcoordFlag)) == END_STRIP){
			//Independent Triangle
			TSStoreIndices(pTSMesh, tIndices, &location, ftslist[validFace].a, validFace);
			TSStoreIndices(pTSMesh, tIndices, &location, ftslist[validFace].b, validFace);
			TSStoreIndices(pTSMesh, tIndices, &location, ftslist[validFace].c, validFace);
		
			pTSMesh->tslist->numIndices = location;
			progressNumFaces++;
			endstrip = 1;
		}
		else{
			// Get Orientation and Rearrange the Inital Storing of Vertices
			// Determine first and last vertex
			if(!TSMatchEdge(&ftslist[nextFace], midVertex, lVertex, pTSMesh)){
				if(TSCheckVertexInFace(&ftslist[nextFace], lVertex)){
					tmpVertex = fVertex;
					fVertex = midVertex;
					midVertex = lVertex;
					lVertex = tmpVertex;
				}
				else{
					tmpVertex = fVertex;
					fVertex = lVertex;
					lVertex = midVertex;
					midVertex = tmpVertex;
				}
			}
			// Store Vertices of Inital Face
			TSStoreIndices(pTSMesh, tIndices, &location, fVertex, validFace);
			TSStoreIndices(pTSMesh, tIndices, &location, midVertex, validFace);
			TSStoreIndices(pTSMesh, tIndices, &location, lVertex, validFace);

			// Store Vertex for next face
			tmpVertex = TSFindVertex3(midVertex, lVertex, &ftslist[nextFace]);
			TSStoreIndices(pTSMesh, tIndices, &location, tmpVertex, nextFace);
			midVertex = lVertex;
			lVertex = tmpVertex;

			// Store Vertex of NextFace
			validFace = nextFace;
			ftslist[validFace].iInvalid = 1;

			progressNumFaces +=2;
		}

		// Loop Until End of a Strip
		while(!endstrip){
			// Algorithm for Next Face of current or next strip
			if((nextFace = TSFindNearestTriangle(validFace, pTSMesh, smID, matID, pTSParam->texcoordFlag)) == END_STRIP){
				pTSMesh->tslist->numIndices = location;
				endstrip = 1;
			}//end-if
			else{
				// Store Indices of Triangle
				// Add Vertex of NextFace if edges match
				if(TSMatchEdge(&ftslist[nextFace], midVertex, lVertex, pTSMesh)){
					if(location >= (MAX_TRI_STRIP_LENGTH - 1)){
						pTSMesh->tslist->numIndices = location;
						break;
					}
					tmpVertex = TSFindVertex3(midVertex, lVertex, &ftslist[nextFace]);
					midVertex = lVertex;
					lVertex = tmpVertex;
					TSStoreIndices(pTSMesh, tIndices, &location, tmpVertex, nextFace);
				}
				else{
					// Swap- Edges do not match
					if(TSCheckVertexInFace(&ftslist[nextFace], lVertex)){
						if(location >= (MAX_TRI_STRIP_LENGTH - 3)){
							pTSMesh->tslist->numIndices = location;
							break;
						}
						midVertex  = TSFindVertex3(midVertex, lVertex, &ftslist[validFace]);
						lVertex    = TSFindVertex3(midVertex, lVertex, &ftslist[nextFace]);
						tmpVertex = tIndices[location-1].vert;
						--location;
						
						TSStoreIndices(pTSMesh, tIndices, &location, midVertex, validFace);
						TSStoreIndices(pTSMesh, tIndices, &location, tmpVertex, validFace);
						TSStoreIndices(pTSMesh, tIndices, &location, lVertex, nextFace);

						midVertex = tmpVertex;
					}
					// Flip - End Strip(would have to repeat two vertices)
					else{
						pTSMesh->tslist->numIndices = location;
						endstrip = 1;
						continue;
					}
				}

				progressNumFaces++;

				if(pTSParam->progressCallback && (progressNumFaces >= (pTSParam->progressFrequency))){
					if(!(pTSParam->progressCallback(pTSParam->progressFrequency))){
						return TS_CANCEL;
					}
					pTSParam->progressFrequency += pTSParam->progressFrequency;
				}

				validFace = nextFace;
				ftslist[validFace].iInvalid = TRUE;
			}//end-else
		} //end-while

		validFace = TSFindNextValidTriangle(pTSMesh);

		// Reset Flags
		endstrip  = 0;

		if(pTSParam->progressCallback && (progressNumFaces >= (pTSParam->progressFrequency))){
			if(!(pTSParam->progressCallback(pTSParam->progressFrequency))){
				return TS_CANCEL;
			}
			progressNumFaces = 0;
		}

		// Allocate space and copy data from tmp to strip data
		TSCopyTSData(pTSMesh->tslist, tIndices, location);

		location = 0;
	}while(validFace != NO_VALID_TRIANGLES);

	return TS_SUCCESS;
}

/*---------------------------------------------------------------*
TSCompareTexCoords:

	Compares the physical texture coordinates between two faces.
	The check is to determine if there is a texture seam between the
	two faces.
	
	Returns: 
	int - TRUE: if there are fewer equal tex. coords between faces 
				than texFlag correction value.
		 FALSE: if there are more or equal tex. coords between faces
				than texFlag correction value.

	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	TSFace *pValidFace  - valid face or current face
	TSFace *pNextFace  - next face
	short sTexFlag   - Texture Seam correction
*----------------------------------------------------------------*/
int TSCompareTexCoords(TSMesh *pTSMesh, TSFace *pValidFace, TSFace *pNextFace, short sTexFlag){
	int count = 0;
	TSUV *tvtmp = pTSMesh->pTexCoord;

	if(sTexFlag == 0)
		return FALSE;

	if((pValidFace->ta == NO_TEX_COORDS) || (pValidFace->tb == NO_TEX_COORDS) ||
		(pValidFace->tc == NO_TEX_COORDS) || (pNextFace->ta == NO_TEX_COORDS) ||
		(pNextFace->tb == NO_TEX_COORDS) || (pNextFace->tc == NO_TEX_COORDS))
		return FALSE;

	if(pValidFace->a == pNextFace->a)
		if(TSCompareUVCoords(&tvtmp[pValidFace->ta], &tvtmp[pNextFace->ta]))
			count++;
	
	if(pValidFace->a == pNextFace->b)
		if(TSCompareUVCoords(&tvtmp[pValidFace->ta], &tvtmp[pNextFace->tb]))
			count++;
	
	if(pValidFace->a == pNextFace->c)
		if(TSCompareUVCoords(&tvtmp[pValidFace->ta], &tvtmp[pNextFace->tc]))
			count++;

	if(pValidFace->b == pNextFace->a)
		if(TSCompareUVCoords(&tvtmp[pValidFace->tb], &tvtmp[pNextFace->ta]))
			count++;

	if(pValidFace->b == pNextFace->b)
		if(TSCompareUVCoords(&tvtmp[pValidFace->tb], &tvtmp[pNextFace->tb]))
			count++;

	if(pValidFace->b == pNextFace->c)
		if(TSCompareUVCoords(&tvtmp[pValidFace->tb], &tvtmp[pNextFace->tc]))
			count++;

	if(pValidFace->c == pNextFace->a)
		if(TSCompareUVCoords(&tvtmp[pValidFace->tc], &tvtmp[pNextFace->ta]))
			count++;

	if(pValidFace->c == pNextFace->b)
		if(TSCompareUVCoords(&tvtmp[pValidFace->tc], &tvtmp[pNextFace->tb]))
			count++;

	if(pValidFace->c == pNextFace->c)
		if(TSCompareUVCoords(&tvtmp[pValidFace->tc], &tvtmp[pNextFace->tc]))
			count++;

	if(count < sTexFlag)
		return TRUE;
	else
		return FALSE;
}


/*---------------------------------------------------------------*
TSCompareUVCoords:

	Compares two texture coordinates to see if they are equal.
	
	Returns: 
	int - TRUE: if t1 is equal to t2
		 FALSE: if t1 not equal to t2

	Parameters:
	TSUV *pV2d1 - texture coord. one (u,v)
	TSUV *pV2d2 - texture coord. two (u,v)
*----------------------------------------------------------------*/
int TSCompareUVCoords(TSUV *pV2d1, TSUV *pV2d2){
	if((fabs(pV2d1->u - pV2d2->u) <= EPS) && (fabs(pV2d1->v - pV2d2->v) <= EPS))
		return TRUE;
	return FALSE;
}


/*---------------------------------------------------------------*
TSGetFacesFromEdge:

	Takes an edge and gets the two faces that contain that edge.
	Then it returns the face that is not the currentFace.
	
	Returns: 
	TSFace * - returns a TSFace

	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	TSFace *pCurrentFace - the current active face of a tri-strip
	TSEdge *pEdge - a single side of a triangle
*----------------------------------------------------------------*/
TSFace *TSGetFacesFromEdge(TSMesh *pTSMesh, TSFace *pCurrentFace, TSEdge *pEdge){
	if(pEdge->faces[0] != NO_FACE)
		if(&pTSMesh->pFace[pEdge->faces[0]] != pCurrentFace)
			return &pTSMesh->pFace[pEdge->faces[0]];
		
	if(pEdge->faces[1] != NO_FACE)
		if(&pTSMesh->pFace[pEdge->faces[1]] != pCurrentFace)
			return &pTSMesh->pFace[pEdge->faces[1]];

	return NULL;
}


/*---------------------------------------------------------------*
TSGetFaceOrientation:

	Takes an edge and gets the two faces that contain that edge.
	Then it returns the face that is not the currentFace.
	
	Returns: 
	int - returns the type of edge that the current face is 
		  trying to connect. ACTIVE_EDGE, SWAP_EDGE, and INVALID_EDGE

	Parameters:
	TSEdge *pEdgeList - Edge list
	TSEdge *pEdge - the current edge
	TSFace *pFace - the current face
*----------------------------------------------------------------*/
int TSGetFaceOrientation(TSEdge *pEdgeList, TSEdge *pEdge, TSFace *pFace){
	// Match Edge means Active Edge
	if( (TSCompareEdge(pEdge, &pEdgeList[pFace->iEdges[0]])) ||
		(TSCompareEdge(pEdge, &pEdgeList[pFace->iEdges[1]])) ||
		(TSCompareEdge(pEdge, &pEdgeList[pFace->iEdges[2]])))
		return ACTIVE_EDGE;
	else
		if((pFace->a == pEdge->b) || (pFace->b == pEdge->b) || (pFace->c == pEdge->b))
			return SWAP_EDGE;

	return INVALID_EDGE;
}


/*---------------------------------------------------------------*
TSStoreIndices:

	When a new face is added to a tri-strip, then the following 
	indices must be added to the current slot in the temporary 
	tri-strip array tIndices.  The indices that are added are 
	the vertex and face indices into the vertex and face pools, 
	respectively.  Also, the position(0,1,2) of the face is stored.
	
	Returns: void
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	TSIndex *pIndexList - temp. storage for one tri-strip
	int *piLocation - current position within a single tri-strip
	long int iVertIndex - Vertex index into vertex pool
	long int iFaceNum - Face index into face pool
*----------------------------------------------------------------*/
void TSStoreIndices(TSMesh *pTSMesh, TSIndex *pIndexList, int *piLocation, long int iVertIndex, long int iFaceNum)
{
	TSFace *ftmp = pTSMesh->pFace;
	
	// Store Vertex Index
	pIndexList[*piLocation].vert = iVertIndex;

	pIndexList[*piLocation].fIndex = iFaceNum;

	if(ftmp[iFaceNum].a == iVertIndex){
		pIndexList[*piLocation].fVert = VertexA;
	}
	else
		if(ftmp[iFaceNum].b == iVertIndex){
			pIndexList[*piLocation].fVert = VertexB;
		}
		else
			if(ftmp[iFaceNum].c == iVertIndex){
				pIndexList[*piLocation].fVert = VertexC;
			}
	*piLocation += 1;
}

/*---------------------------------------------------------------*
TSCopyTSData:

	Copy the tri-strip data in tIndices(temp. storage) into the
	linked list of tri-strips of TSMesh.
		
	Returns: void
	
	Parameters:
	struct TSTriStrip *pTSList - Linked list of tri-strips
	TSIndex *pTSIndices - temp. storage for one tri-strip	
	int iRunLength -	the number of vertices in the current tri-strip
*----------------------------------------------------------------*/
void TSCopyTSData(struct TSTriStrip *pTSList, TSIndex *pTSIndices, int iRunLength){
	
	pTSList->indices = (TSIndex *) malloc(iRunLength * sizeof(TSIndex));

	for(int i=0; i < iRunLength; i++){
		pTSList->indices[i].vert = pTSIndices[i].vert;
		pTSList->indices[i].fIndex = pTSIndices[i].fIndex;
		pTSList->indices[i].fVert = pTSIndices[i].fVert;
	}
}


/*---------------------------------------------------------------*
TSCheckVertexInFace:

	Is the vertex in the face?
		
	Returns:
	int - TRUE: Yes the face contains the vertex.
		 FALSE: No the face contains the vertex.
	
	Parameters:
	TSFace *pFace   - current face
	long int iVertexIndex - vertex index

*----------------------------------------------------------------*/
int TSCheckVertexInFace(TSFace *pFace, long int iVertexIndex){
	if((pFace->a == iVertexIndex) || (pFace->b == iVertexIndex) || (pFace->c == iVertexIndex))
		return TRUE;
	else 
		return FALSE;
}


/*---------------------------------------------------------------*
TSMatchEdge:

	Match current edge of triangle strip with an edge
    of the next triangle
		
	Returns:
	int - TRUE: Yes the face contains the vertex.
		 FALSE: No the face contains the vertex.
	
	Parameters:
	TSFace *pFace - current face
	long int iEdgeIndex1 - Edge vertex index 1
	long int iEdgeIndex2 - Edge vertex index 2
	TSMesh *pTSMesh - tri-strip mesh
*----------------------------------------------------------------*/
int TSMatchEdge(TSFace *pFace, long int iEdgeIndex1, long int iEdgeIndex2, TSMesh *pTSMesh){
	TSEdge edgeTmp;
	TSEdge *pEdge = pTSMesh->pEdge;

	edgeTmp.a = iEdgeIndex1;
	edgeTmp.b = iEdgeIndex2;

	if( (TSCompareEdge(&edgeTmp, &pEdge[pFace->iEdges[0]])) ||
		(TSCompareEdge(&edgeTmp, &pEdge[pFace->iEdges[1]])) ||
		(TSCompareEdge(&edgeTmp, &pEdge[pFace->iEdges[2]])))
		return TRUE;

	return FALSE;
}


/*---------------------------------------------------------------*
TSCompareEdge:

	Compare two edges to see if they are equal
		
	Returns:
	int - TRUE: Edges are equal
		 FALSE: Edges are not equal
	
	Parameters:
	TSEdge *pEdge1 - Edge one
	TSEdge *pEdge2 - Edge two
*----------------------------------------------------------------*/
int TSCompareEdge(TSEdge *pEdge1, TSEdge *pEdge2){
	if( ((pEdge1->a == pEdge2->a) && (pEdge1->b == pEdge2->b)) ||
		((pEdge1->a == pEdge2->b) && (pEdge1->b == pEdge2->a)))
		return TRUE;

	return FALSE;
}

/*---------------------------------------------------------------*
TSCheckDuplicateFace:

	Check to see if the face1 equal to face2.  The faces could be
	equal but have different clockwise or counter-clockwise rotations.
		
	Returns:
	int - TRUE: Faces are equal
		 FALSE: Faces are not equal
	
	Parameters:
	TSFace *pFace1 - input face one
	TSFace *pFace2 - input face two
*----------------------------------------------------------------*/
int TSCheckDuplicateFace(TSFace *pFace1, TSFace *pFace2){
	if((pFace1->a == pFace2->a) || (pFace1->a == pFace2->b) || (pFace1->a == pFace2->c))
		if((pFace1->b == pFace2->a) || (pFace1->b == pFace2->b) || (pFace1->b == pFace2->c))
			if((pFace1->c == pFace2->a) || (pFace1->c == pFace2->b) || (pFace1->c == pFace2->c))
				return TRUE;

	return FALSE;
}


/*---------------------------------------------------------------*
TSFindNextValidTriangle:

	Find a Triangle that has not been used to start next tri-strip
		
	Returns:
	long int - Index into the face pool for the next triangle to be
			   added to the tri-strip
	
	Parameters:
	TSMesh *pTSMesh - tri-stripmesh
*----------------------------------------------------------------*/
long int TSFindNextValidTriangle(TSMesh *pTSMesh){
	TSFace *pTmpFace = pTSMesh->pFace;
	unsigned long uFaceIndex = NO_VALID_TRIANGLES;

	for(long int f=(pTSMesh->uNumFaces - 1); f >=0 ; f--){
		if(!(pTmpFace[f].iInvalid))
			return f;
	}

	if(uFaceIndex != NO_VALID_TRIANGLES)
		return uFaceIndex;

	// No valid Triangles left so DONE
	return NO_VALID_TRIANGLES;
}


/*---------------------------------------------------------------*
TSFindNearestTriangle:

	Find the nearest triangle to the current triangle.
    The nearest triangle is determined by a triangle's distance
	from the centroid.
		
	Returns:
	long int - Index into the face pool for the nearest triangle 
			   to the current triangle.
	
	Parameters:
	long int iFaceIndex - current face
	TSMesh *pTSMesh - tri-stripmesh
	unsigned long uSmoothId - smoothing group ID
	unsinged long uMatId - material ID
	short sTexFlag - texture seam correction flag
*----------------------------------------------------------------*/
long int TSFindNearestTriangle(long int iFaceIndex, TSMesh *pTSMesh, unsigned long uSmoothId, unsigned long uMatId, short sTexFlag){
	long int nface[] = {-1, -1, -1};
	long int final = NO_FACE;
	double min = INIT_MIN;
	TSFace *ftstmp = pTSMesh->pFace;

	// Get the Three Edges
	TSEdge *e1 = &pTSMesh->pEdge[ (ftstmp[iFaceIndex].iEdges[0]) ];
	TSEdge *e2 = &pTSMesh->pEdge[ (ftstmp[iFaceIndex].iEdges[1]) ];
	TSEdge *e3 = &pTSMesh->pEdge[ (ftstmp[iFaceIndex].iEdges[2]) ];

	nface[0] = TSFindNextFace(iFaceIndex, e1, pTSMesh, uSmoothId, uMatId, sTexFlag);
	nface[1] = TSFindNextFace(iFaceIndex, e2, pTSMesh, uSmoothId, uMatId, sTexFlag);
	nface[2] = TSFindNextFace(iFaceIndex, e3, pTSMesh, uSmoothId, uMatId, sTexFlag);

	for(int loop = 0; loop < 3; loop++){
		if(nface[loop] != NO_FACE)
			if(ftstmp[nface[loop]].fDistance < min){
				final = nface[loop];
				min = ftstmp[nface[loop]].fDistance;
			}//end-if
	}// end-for
	
	if(final != NO_FACE)
		return final;
	
	return END_STRIP;
}



/*---------------------------------------------------------------*
TSFindNextFace:

	Given an Edge and a Face:  Find another face that uses that edge
    Check for validity of face via smoothID, material ID, duplicate,
	or texture seam break.  
		
	Returns:
	long int - Index into the face pool for the face that connects to
			   the same edge and is valid.  If there is not another face
			   or no valid faces return NO_FACE. 
	
	Parameters:
	long int iface - current face
	TSMesh *pEdge - an edge of the current face
	TSMesh *pTSMesh - tri-stripmesh
	unsigned long uSmoothId - smoothing group ID
	unsinged long uMatId - material ID
	short sTexFlag - texture seam correction flag
*----------------------------------------------------------------*/
long int TSFindNextFace(long int iface, TSEdge *pEdge, TSMesh *pTSMesh, unsigned long uSmoothId, unsigned long uMatId, short sTexFlag){
	TSFace *ftstmp = pTSMesh->pFace;

	long int etmp1 = pEdge->faces[0];
	long int etmp2 = pEdge->faces[1];

	if(etmp1 != NO_FACE)
		if((etmp1 != iface) && (!(ftstmp[etmp1].iInvalid)) && 
			((ftstmp[etmp1].uSmoothId == uSmoothId)) && (ftstmp[etmp1].uMatId == uMatId))
			if((!ftstmp[iface].iDuplicate) && (!ftstmp[etmp1].iDuplicate))
				if(!TSCompareTexCoords(pTSMesh, &pTSMesh->pFace[iface], &pTSMesh->pFace[etmp1], sTexFlag))
					return etmp1;
	
	if(etmp2 != NO_FACE)
		if((etmp2 != iface) && (!(ftstmp[etmp2].iInvalid)) &&
			(ftstmp[etmp2].uSmoothId == uSmoothId) && (ftstmp[etmp2].uMatId == uMatId))
			if(!(ftstmp[iface].iDuplicate) && !(ftstmp[etmp2].iDuplicate))
				if(!TSCompareTexCoords(pTSMesh, &pTSMesh->pFace[iface], &pTSMesh->pFace[etmp2], sTexFlag))
					return etmp2;
			
	return NO_FACE;
}


/*---------------------------------------------------------------*
TSFindVertex3:

	Given an edge of a triangle find the vertex not included in the edge. 
		
	Returns:
	long int - Index into the vertex pool for the 3rd vertex of a
			   face. Return -1 if no vertex found.
	
	Parameters:
	long int iVertex1 - a vertex of an edge
	long int iVertex2 - another vertex of the same edge
	TSFace *pFace - next face to add and get the proper vertex.
*----------------------------------------------------------------*/
long int TSFindVertex3(long int iVertex1, long int iVertex2, TSFace *pFace){
	if((pFace->a != iVertex1) && (pFace->a != iVertex2))
		return pFace->a;
	if((pFace->b != iVertex1) && (pFace->b != iVertex2))
		return pFace->b;
	if((pFace->c != iVertex1) && (pFace->c != iVertex2))
		return pFace->c;

	// 3rd Vertex Not found properly
	return -1;
}


/*---------------------------------------------------------------*
TSCreateTriStrip:

	Create a new node for the next tri-strip and return the node.
		
	Returns:
	struct TSTriStrip * - Pointer to a newly created TSTriStrip node.
	
	Parameters: NONE
*----------------------------------------------------------------*/
struct TSTriStrip *TSCreateTriStrip(){
	struct TSTriStrip *pTemp;

	pTemp = (struct TSTriStrip *) malloc(sizeof(struct TSTriStrip));
	pTemp->next = NULL;

	return pTemp;
}

/********************************************************************
   BEGIN SECTION: TSMetric used to evaluate faces
********************************************************************/

/*---------------------------------------------------------------*
TSInitializeTSMetric:
	
	Get the triangle's area and distance from iFace. This
	function intializes the distance and area metrics for 
	the tri-strip algorithm.
		
	Returns: Void
	
	Parameters:
	long int *iFace - beginning face for TSMetric
	TSMesh *pTSMesh - tri-strip mesh
*----------------------------------------------------------------*/
void TSInitializeTSMetric(long int *iFace, TSMesh *pTSMesh){
	TSV3D *vtmp;
	long int a, b, c;
	
	//Find Centroid for all Faces
	TSCalculateTriangleCentroid(pTSMesh);

	//  Start With the Triangle with the Smallest Area
	for(unsigned long i=0; i< pTSMesh->uNumFaces; i++){
		a = pTSMesh->pFace[i].a;
		b = pTSMesh->pFace[i].b;
		c = pTSMesh->pFace[i].c;

		// Compute the area of each triangle face:
		pTSMesh->pFace[i].fArea = TSTriangleArea (&pTSMesh->pVertex[a],&pTSMesh->pVertex[b],
												&pTSMesh->pVertex[c]);

		// Face Index into Face Array
		pTSMesh->pFace[i].uFaceIndex = i;
	}
	
	// Find Initial Face Based on Triangle Area
	*iFace = TSFindClusterCenter(pTSMesh);
	vtmp = &pTSMesh->pVertex[pTSMesh->pFace[*iFace].a];
	
	// Store Distance of every Triangle from the centroid
	TSDistancePtToCentroid(vtmp, pTSMesh);
}


/*---------------------------------------------------------------*
TSFindClusterCenter:
	
	Finds the smallest area triangle.  
		
	Returns:
	long int - the smallest area triangle found
	
	Parameters:
	TSMesh *iTSMesh - tri-strip mesh
*----------------------------------------------------------------*/
long int TSFindClusterCenter(TSMesh *pTSMesh){
	double  min = INIT_MIN;
	long int iface = NO_VALID_TRIANGLES;
	TSFace *ftmp = pTSMesh->pFace;

	for(unsigned long i=0; i<pTSMesh->uNumFaces; i++){
		if(ftmp[i].iInvalid)
			continue;
		if(min > ftmp[i].fArea){
			min = ftmp[i].fArea;
			iface = i;
		}
	}
  
	return iface;
}


/*---------------------------------------------------------------*
TSCalculateTriangleCentroid:
	
	Finds the centroid for each triangle of the mesh.
		
	Returns: Void
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
*----------------------------------------------------------------*/
void TSCalculateTriangleCentroid(TSMesh *pTSMesh)
{	
	TSFace *ftstmp = pTSMesh->pFace;
	TSV3D *v1, *v2, *v3;

	for(unsigned long loop = 0; loop < pTSMesh->uNumFaces; loop++){			
		if(ftstmp[loop].iInvalid)
			continue;
		v1 = &pTSMesh->pVertex[ftstmp[loop].a];
		v2 = &pTSMesh->pVertex[ftstmp[loop].b];
		v3 = &pTSMesh->pVertex[ftstmp[loop].c];

		ftstmp[loop].centroid.x = (float)((v1->x + v2->x + v3->x)/3.0);
		ftstmp[loop].centroid.y = (float)((v1->y + v2->y + v3->y)/3.0);
		ftstmp[loop].centroid.z = (float)((v1->z + v2->z + v3->z)/3.0);
	}
}

/*---------------------------------------------------------------*
TSDistancePtToCentroid:
	
	Assigns a distance to each face from the initial vertex(vinit)
	to the centroid of the face.
		
	Returns: Void
	
	Parameters:
	TSV3D *vinit - intial vertex to get distances from.
	TSMesh *pTSMesh - tri-strip mesh
*----------------------------------------------------------------*/
void TSDistancePtToCentroid(TSV3D *vinit, TSMesh *pTSMesh){
	TSFace *ftmp = pTSMesh->pFace;
	
	for(unsigned long loop = 0; loop < pTSMesh->uNumFaces; loop++){
		if(ftmp[loop].iInvalid)
			continue;
		ftmp[loop].fDistance = TSDistance3D(vinit, &ftmp[loop].centroid); 
	}
}


/*---------------------------------------------------------------*
TSVectorLength:
	
	Length of a vector.
		
	Returns: 
	float - vector length
	
	Parameters:
	TSV3D *v - vector
*----------------------------------------------------------------*/
float TSVectorLength(TSV3D* pV3d)
{
	return (float )sqrt(pV3d->x*pV3d->x + pV3d->y*pV3d->y + pV3d->z*pV3d->z);
} 


/*---------------------------------------------------------------*
TSTriangleArea:
	
	Triangle area. According to LaGrange:
	area of a triangle = 1/2 ||a x b||
		
	Returns: 
	float - area of a triangle
	
	Parameters:
	TSV3D *pV1 - vertex one of the triangle
	TSV3D *pV2 - vertex two of the triangle
	TSV3D *pV3 - vertex three of the triangle
*----------------------------------------------------------------*/
float TSTriangleArea (TSV3D *pV1, TSV3D *pV2, TSV3D *pV3)
{
	TSV3D tV1, tV2;
	TSV3D vCross;

	// Translate to the origin:
	TSSubtract3D (pV3, pV1, &tV1);
	TSSubtract3D (pV3, pV2, &tV2);

	// Solve for area...yeah, this actually works:
	TSCrossprod (&tV1, &tV2, &vCross);
	float area = 1.0f / 2.0f * TSVectorLength (&vCross); 

	return area;
}
/********************************************************************
   END SECTION: TSMetric used to evaluate faces 
********************************************************************/


/********************************************************************
   BEGIN SECTION: Edge-to-Face Traversal && Face-to-Edge Traversal 
********************************************************************/

/*---------------------------------------------------------------*
TSEdgeFaceTraversal:
	
	Create an edge list for all of the edges in the mesh.
	Initialize hash table for edges.

	Returns: Void
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	TSParam *pTSParam - tri-strip export options
*----------------------------------------------------------------*/
void TSEdgeFaceTraversal(TSMesh *pTSMesh, TSParam *pTSParam){
	struct TSHashEdges *pHashEdgeTable;
	struct TSHashFaces *hashDupTable;
	// Parse Edges for Triangle Strips
	// Init. Edges with no face association
	// TStrip
	pTSMesh->pEdge = (TSEdge *) malloc(pTSMesh->uNumFaces * 3 * sizeof(TSEdge));
	
	/* Hash Edges */
	HASHSIZE = pTSMesh->uNumVertices;
	pHashEdgeTable = (struct TSHashEdges *) malloc(HASHSIZE * sizeof(struct TSHashEdges));
	hashDupTable = (struct TSHashFaces *) malloc(HASHSIZE * sizeof(struct TSHashFaces));

	for(long int h=0; h < HASHSIZE; h++){
		pHashEdgeTable[h].edgeNum = EMPTY_SLOT;
		hashDupTable[h].faceNum = EMPTY_SLOT;
		pHashEdgeTable[h].next =  NULL;
		hashDupTable[h].next = NULL;
	}

	//Set faces that each edge is associated with to zero
	for(unsigned long edgeloop = 0; edgeloop < (pTSMesh->uNumFaces * 3); edgeloop++){
		pTSMesh->pEdge[edgeloop].faces[0] = NO_FACE;
		pTSMesh->pEdge[edgeloop].faces[1] = NO_FACE;
	}

	pTSMesh->uNumEdges = TSCreateEdgeFromFace(pTSMesh, pHashEdgeTable);

	TSStoreFaceInfo(pTSMesh);

	for(unsigned long faceloop = 0; faceloop < pTSMesh->uNumFaces; faceloop++)
		pTSMesh->pFace[faceloop].iDuplicate = 0;		

	TSFindDuplicateFaces(pTSMesh, hashDupTable);

	// Free HashTables
	for(h=0; h < HASHSIZE; h++){
		if(hashDupTable[h].next != NULL)
			TSFreeHashDup(hashDupTable[h].next);
		if(pHashEdgeTable[h].next != NULL)
			TSFreeHash(pHashEdgeTable[h].next);
	}
	free(hashDupTable);
	free(pHashEdgeTable);
}


/*---------------------------------------------------------------*
TSCreateEdgeFromFace:
	
	Creates the minimum list of edges for the 3D model.
		
	Returns: 
	long int - total edges created
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	struct TSHashEdges *pHashEdgeTable - Hashtable for the edges
*----------------------------------------------------------------*/
long int TSCreateEdgeFromFace(TSMesh *pTSMesh, struct TSHashEdges *pHashEdgeTable){
	TSEdge tEdge;
	TSEdge *tsEdge = pTSMesh->pEdge;
	TSFace *tsface;
	long int  totEdges = 0;
	long int  prevEdge;

	for(unsigned long currentFace=0; currentFace < pTSMesh->uNumFaces; currentFace++){
		tsface = &pTSMesh->pFace[currentFace];
		if((tsface->a == tsface->b) ||
			(tsface->a == tsface->c) ||
			(tsface->b == tsface->c)){
			tsface->iInvalid = TRUE;
			continue;
		}
		tEdge.a = tsface->a;
		tEdge.b = tsface->b;
		if((prevEdge = TSHashFindEdge(pTSMesh, &tEdge, pHashEdgeTable)) != -1){
			tsface->iEdges[0] = prevEdge;
		}
		else{
			TSHashAddEdge(&tEdge, totEdges, pHashEdgeTable);
			tsface->iEdges[0] = totEdges;
			tsEdge[totEdges].a = tEdge.a;
			tsEdge[totEdges].b = tEdge.b;
			totEdges++;
		}
		

		tEdge.a = tsface->b;
		tEdge.b = tsface->c;
		if((prevEdge = TSHashFindEdge(pTSMesh, &tEdge, pHashEdgeTable)) != -1){
			tsface->iEdges[1] = prevEdge;
		}
		else{
			TSHashAddEdge(&tEdge, totEdges, pHashEdgeTable);
			tsface->iEdges[1] = totEdges;
			tsEdge[totEdges].a = tEdge.a;
			tsEdge[totEdges].b = tEdge.b;
			totEdges++;
		}

		tEdge.a = tsface->c;
		tEdge.b = tsface->a;
		if((prevEdge = TSHashFindEdge(pTSMesh, &tEdge, pHashEdgeTable)) != -1){
			tsface->iEdges[2] = prevEdge;
		}
		else{
			TSHashAddEdge(&tEdge, totEdges, pHashEdgeTable);
			tsface->iEdges[2] = totEdges;
			tsEdge[totEdges].a = tEdge.a;
			tsEdge[totEdges].b = tEdge.b;
			totEdges++;
		}
		tsface->iInvalid = 0;	
	}
	
	return (totEdges);
}


/*---------------------------------------------------------------*
TSPrevEdge:
	
	Check to see if the edge was previously created.
		
	Returns: 
	int - The edge exist so return the edge number, else return -1.
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	int iNumEdges - number of edges created so far
	TSEdge *pEdge - Edge of a face
*----------------------------------------------------------------*/
int TSPrevEdge(TSMesh *pTSMesh, int iNumEdges, TSEdge *pEdge){
	TSEdge *pTmpEdge = pTSMesh->pEdge;

	for(int z=0; z < iNumEdges; z++){
		if ( ((pTmpEdge[z].a == pEdge->a) && (pTmpEdge[z].b == pEdge->b)) ||
			 ((pTmpEdge[z].a == pEdge->b) && (pTmpEdge[z].b == pEdge->a)) )
			 return z;
	}
	return -1;  // Edge Not Found
}


/*---------------------------------------------------------------*
TSStoreFaceInfo:
	
	Store the face #'s that are associated with each edge.
		
	Returns: Void
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
*----------------------------------------------------------------*/
void TSStoreFaceInfo(TSMesh *pTSMesh){
	long int edgept;
	TSEdge *edgeNum = pTSMesh->pEdge;
	TSFace *tsFace  = pTSMesh->pFace;

	for(unsigned long j=0; j<pTSMesh->uNumFaces; j++){
		if(tsFace[j].iInvalid)
			continue;
		for(int i=0; i<3; i++){
			edgept = tsFace[j].iEdges[i];
			if(edgeNum[edgept].faces[0] == -1)
				edgeNum[edgept].faces[0] = j;
			else
				edgeNum[edgept].faces[1] = j;
		}
	}
}

// HASH functions
/*---------------------------------------------------------------*
TSHashEdgeFunc:
	
	Hash function for an edge.
		
	Returns: 
	long int - hash value for the edge
	
	Parameters:
	TSEGDE *pEdge - Edge of a face
*----------------------------------------------------------------*/
long int TSHashEdgeFunc(TSEdge *pEdge){
	unsigned long uTemp = (pEdge->a + pEdge->b) % HASHSIZE;
	return((long int) uTemp);
}

/*---------------------------------------------------------------*
TSHashFindEdge:
	
	Look up an edge in the hash table. 
		
	Returns: 
	long int - If edge is found, return the index number of the 
			   edge.
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	TSEdge *pEdge - Edge of a face
	struct TSHashEdges *pHashTable - hash table for the edges
*----------------------------------------------------------------*/
long int TSHashFindEdge(TSMesh *pTSMesh, TSEdge *pEdge, struct TSHashEdges *pHashTable){
	TSEdge *pTmpEdge = pTSMesh->pEdge;
	long int index;
	struct TSHashEdges *tmp;
	long int edgeIndex = 0;

	index = TSHashEdgeFunc(pEdge);
	if((edgeIndex = pHashTable[index].edgeNum) == EMPTY_SLOT)
		return -1;

	if (((pTmpEdge[edgeIndex].a == pEdge->a) && (pTmpEdge[edgeIndex].b == pEdge->b)) ||
		((pTmpEdge[edgeIndex].a == pEdge->b) && (pTmpEdge[edgeIndex].b == pEdge->a)) ){
		return edgeIndex;
	}
	else{
		// Check linked list
		tmp = pHashTable[index].next;
		while(tmp != NULL){
			edgeIndex = tmp->edgeNum;
			if (((pTmpEdge[edgeIndex].a == pEdge->a) && (pTmpEdge[edgeIndex].b == pEdge->b)) ||
				((pTmpEdge[edgeIndex].a == pEdge->b) && (pTmpEdge[edgeIndex].b == pEdge->a)) )
				return edgeIndex;
			tmp = tmp->next;
		}
	}
	return -1;
}

/*---------------------------------------------------------------*
TSHashAddEdge:
	
	Add an edge to the hash table.
		
	Returns: Void
	
	Parameters:
	TSEdge *pEdge - Edge of a face
	long int iEdgeIndex - Index of the edge to add to the hash table
	struct TSHashEdges *pHashTable - hash table for the edges
*----------------------------------------------------------------*/
void TSHashAddEdge(TSEdge *pEdge, long int iEdgeIndex, struct TSHashEdges *pHashTable){
	long int index;
	struct TSHashEdges *pTmpNode, *pTmp;
	int hflag = 0;

	index = TSHashEdgeFunc(pEdge);
	if(pHashTable[index].edgeNum != EMPTY_SLOT){
		pTmp = pHashTable[index].next;
		if(pTmp == NULL)
			hflag = 1;
		else{
			while(pTmp->next != NULL)
				pTmp = pTmp->next;
		}

		// Add new node to linked list
		pTmpNode = (struct TSHashEdges *) malloc(sizeof(struct TSHashEdges));
		pTmpNode->edgeNum = iEdgeIndex;
		if(hflag)
			pHashTable[index].next = pTmpNode;
		else
			pTmp->next = pTmpNode;
		pTmpNode->next = NULL;
	}
	else
		pHashTable[index].edgeNum = iEdgeIndex;
}


/*---------------------------------------------------------------*
TSFreeHash:
	
	Free the hash table.
		
	Returns: Void
	
	Parameters:
	struct TSHashEdges *tmp - hash table to be freed
*----------------------------------------------------------------*/
void TSFreeHash(struct TSHashEdges *pTmp){
	struct TSHashEdges *pTmp2;

	while(pTmp != NULL){
		pTmp2 = pTmp->next;
		free(pTmp);
		pTmp = pTmp2;
	}
}


//----------------------------------------------------------
// Hash functions for Duplicate Faces


/*---------------------------------------------------------------*
TSFindDuplicateFaces:
	
	Checks to see if there are any duplicate faces with the mesh.
	A duplicate face is any two faces that contain three vertices
	that are all equal to the other face's three vertices respectively.
		
	Returns: 
	int - TRUE: a duplicate face exist.
		 FALSE: no duplicate faces.
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	struct TSHashFaces *pHashDupTable - hash table for duplicate faces
*----------------------------------------------------------------*/
int TSFindDuplicateFaces(TSMesh *pTSMesh, struct TSHashFaces *pHashDupTable){
	TSFace *pTmpFace = pTSMesh->pFace;
	long int index = 0;
	int duplicateFlag = 0;

	for(unsigned long i=0; i<pTSMesh->uNumFaces; i++){
		if((index = TSHashFindDupFace(pTSMesh, &pTmpFace[i], pHashDupTable)) != NO_FACE){
			pTmpFace[i].iDuplicate = 1;
			pTmpFace[index].iDuplicate = 1;			
			duplicateFlag = 1;
		}
		else{
			TSHashAddDupFace(&pTmpFace[i], i, pHashDupTable);

		}
	}

	return duplicateFlag;
}


/*---------------------------------------------------------------*
TSHashDupFunc:
	
	Hash function for a duplicate face.
		
	Returns: 
	long int - hash value for a face
	
	Parameters:
	TSFace *pFace - triangle of the mesh
*----------------------------------------------------------------*/
long int TSHashDupFunc(TSFace *pFace){
	unsigned long temp = (pFace->a + pFace->b + pFace->c) % HASHSIZE;
	return((long int) temp);
}


/*---------------------------------------------------------------*
TSHashFindDupFace:
	
	Check to see if the duplicated face is in the hash table.
		
	Returns: 
	long int - if a hashed duplicated face is found return the
			   index of the duplicated face, else return -1.
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	TSFace *pFace - face of the mesh
	struct TSHashFaces *pHashDupTable - hash table for duplicate faces
*----------------------------------------------------------------*/
long int TSHashFindDupFace(TSMesh *pTSMesh, TSFace *pFace, struct TSHashFaces *pHashDupTable){
	TSFace *pTmpFace = pTSMesh->pFace;
	long int index;
	long int faceIndex = 0;
	struct TSHashFaces *tmp;

	index = TSHashDupFunc(pFace);
	if((faceIndex = pHashDupTable[index].faceNum) == EMPTY_SLOT)
		return -1;

	if(TSCheckDuplicateFace(&pTmpFace[faceIndex], pFace)){
		return faceIndex;
	}
	else{
		// Check linked list
		tmp = pHashDupTable[index].next;
		while(tmp != NULL){
			faceIndex = tmp->faceNum;
			if(TSCheckDuplicateFace(&pTmpFace[faceIndex], pFace))
				return faceIndex;
			tmp = tmp->next;
		}
	}
	return -1;
}

/*---------------------------------------------------------------*
TSHashAddDupFace:
	
	Check to see if the duplicated face is in the hash table.
		
	Returns: Void
	
	Parameters:
	TSFace *pFace - face of the mesh
	long int iFaceIndex - face to add to the hash table
	struct TSHashFaces *pHashDupTable - hash table for duplicate faces
*----------------------------------------------------------------*/
void TSHashAddDupFace(TSFace *pFace, long int iFaceIndex, struct TSHashFaces *pHashTable){
	long int index;
	struct TSHashFaces *tmpnode, *tmp;
	int hflag = 0;

	index = TSHashDupFunc(pFace);
	if(pHashTable[index].faceNum != EMPTY_SLOT){
		tmp = pHashTable[index].next;
		if(tmp == NULL)
			hflag = 1;
		else{
			while(tmp->next != NULL)
				tmp = tmp->next;
		}

		// Add new node to linked list
		tmpnode = (struct TSHashFaces *) malloc(sizeof(struct TSHashFaces));
		tmpnode->faceNum = iFaceIndex;
		if(hflag)
			pHashTable[index].next = tmpnode;
		else
			tmp->next = tmpnode;
		tmpnode->next = NULL;
	}
	else
		pHashTable[index].faceNum = iFaceIndex;
}

/*---------------------------------------------------------------*
TSFreeHashDup:
	
	Free the hash table for the duplicated faces
		
	Returns: Void
	
	Parameters:
	struct TSHashFaces *pTmp - hash table for duplicate faces
*----------------------------------------------------------------*/
void TSFreeHashDup(struct TSHashFaces *pTmp){
	struct TSHashFaces *pTmp2;

	while(pTmp != NULL){
		pTmp2 = pTmp->next;
		free(pTmp);
		pTmp = pTmp2;
	}
}
//----------------------------------------------------------

/********************************************************************
   END SECTION: Edge-to-Face Traversal && Face-to-Edge Traversal 
********************************************************************/



/*************************************************************************
   BEGIN SECTION: Reconnect Triangle Strips
**************************************************************************/

/*---------------------------------------------------------------*
TSConnectStrips:
	
	Second pass merge process of tri-strips.  This function will
	try to analyze the first pass tri-strips created to see if any
	of the tri-strips can be reconnected. There is a linked list
	associated to each entry of the TSConnectList *itslist array
	which is the size of the number of faces.
		
	Returns: Void
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	TSParam *pTSParam - tri-strip export options
*----------------------------------------------------------------*/
void TSConnectStrips(TSMesh *pTSMesh, TSParam *pTSParam){
	struct TSTriStrip *pTSNode = pTSMesh->tshead;
	struct TSTriStrip *pLastStrip;
	TSConnectList *pTSList;


	// Add Starting and Ending Faces of a Strip into Face List
	// TSEdgeType:
	//   START_EDGE, END_EDGE, ORPHAN_EDGE
	pTSList = (TSConnectList *) malloc(pTSMesh->uNumFaces * sizeof(TSConnectList));

	// Get last strip to add new strips
	while(pTSNode->next != NULL){
		pTSNode = pTSNode->next;
	}
	pLastStrip = pTSNode;
	pTSNode = pTSMesh->tshead;
		// Initialize the structure
	for(unsigned long i=0; i < pTSMesh->uNumFaces; i++){
		pTSList[i].head = NULL;
		pTSList[i].connect = 0;
	}
	
	TSCreateConnectStructures(pTSMesh, pTSNode, pTSList);

	//--------------------------------------------------------------------
	// Check if two tri-strips can be added
	TSConnectTriStrips(pTSMesh, pLastStrip, pTSList, pTSParam);

	//--------------------------------------------------------------------
	// Remove invalid Strips
	TSRemoveInvalidStrips(pTSMesh->tshead);
	
	//--------------------------------------------------------------------
	// Free the Reconnect Structures
	TSDeleteConnectStructures(pTSMesh, pTSList);
}

/*---------------------------------------------------------------*
TSCreateConnectStructures:
	
	Creates the reconnection data structures.  An entry is made 
	for the starting and ending for each tri-strip.  If a tri-strip
	is an orphan triangle, then there is only one node created.
	  
	Returns: Void
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	struct TSTriStrip *pTSNode - linked list of tri-strips
	TSConnectList *pTSList - pointer to the array of linked lists
*----------------------------------------------------------------*/
void TSCreateConnectStructures(TSMesh *pTSMesh, struct TSTriStrip *pTSNode, TSConnectList *pTSList){
	struct TSNode *pStartNode, *pEndNode;
	int iStartFaceIndex, iEndFaceIndex;  //Start and Ending Face Index
	int iNumTriStrips = 0;

	while(pTSNode->next != NULL){
		// Orphan Triangle Strip
		if(pTSNode->next->numIndices == 3){
			iStartFaceIndex = pTSNode->next->indices[0].fIndex;
			pStartNode = TSCreateTSNode();
			
			pStartNode->tstrip = pTSNode;
			pStartNode->invalid = 0;
			pStartNode->edgetype = Orphan_Edge;
			pStartNode->smoothID = pTSMesh->pFace[iStartFaceIndex].uSmoothId;
			pStartNode->matID = pTSMesh->pFace[iStartFaceIndex].uMatId;
			pStartNode->faceIndex = iStartFaceIndex;

			TSGetRNeighboringFaces(pTSMesh, &pTSMesh->pFace[iStartFaceIndex], NULL, pStartNode->nfaces);

			pStartNode->next = pTSList[iStartFaceIndex].head;
			pTSList[iStartFaceIndex].head = pStartNode;
		}
		else{
			iStartFaceIndex = pTSNode->next->indices[0].fIndex;
			iEndFaceIndex = pTSNode->next->indices[(pTSNode->next->numIndices - 1)].fIndex;

			pStartNode = TSCreateTSNode();
			pEndNode   = TSCreateTSNode();

			pStartNode->tstrip = pEndNode->tstrip = pTSNode;
			pStartNode->invalid = pEndNode->invalid = 0;
			pStartNode->edgetype = Start_Edge;
			pEndNode->edgetype = End_Edge;

			pStartNode->smoothID = pEndNode->smoothID = pTSMesh->pFace[iStartFaceIndex].uSmoothId;
			pStartNode->matID = pEndNode->matID = pTSMesh->pFace[iEndFaceIndex].uMatId;
			pStartNode->faceIndex = iStartFaceIndex;
			pEndNode->faceIndex = iEndFaceIndex;

			// Watch for duplicate face when getting neigboring faces
			TSGetRNeighboringFaces(pTSMesh, &pTSMesh->pFace[iStartFaceIndex], &pTSMesh->pFace[pTSNode->next->indices[3].fIndex], pStartNode->nfaces);
			TSGetRNeighboringFaces(pTSMesh, &pTSMesh->pFace[iEndFaceIndex], &pTSMesh->pFace[pTSNode->next->indices[(pTSNode->next->numIndices - 1)].fIndex], pEndNode->nfaces);

			pStartNode->next = pTSList[iStartFaceIndex].head;
			pEndNode->next = pTSList[iEndFaceIndex].head;
			pTSList[iStartFaceIndex].head = pStartNode;
			pTSList[iEndFaceIndex].head = pEndNode;
		}
		
		iNumTriStrips++;
		pTSNode = pTSNode->next;
	}
}

/*---------------------------------------------------------------*
TSDeleteConnectStructures:
	
	Delete the reconnect array of linked list.
	  
	Returns: Void
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	TSConnectList *pTSList - pointer to the array of linked lists
*----------------------------------------------------------------*/
void TSDeleteConnectStructures(TSMesh *pTSMesh, TSConnectList *pTSList){
	TSNode *pFreeNode, *pTmpFreeNode;
	for(unsigned int r=0; r<pTSMesh->uNumFaces; r++){
		if(pTSList[r].head != NULL){
			pFreeNode = pTSList[r].head;
			while(pFreeNode->next != NULL){
				pTmpFreeNode = pFreeNode;
				pFreeNode = pFreeNode->next;
				free(pTmpFreeNode);
			}
			free(pFreeNode);
		}
	}
	free(pTSList);
}

/*---------------------------------------------------------------*
TSConnectTriStrips:
	
	This is the core of the reconnect code.  It analyzes the 
	reconnect data structures to see if there are any matches between
	tri-strips beginnings and ending nodes that can be connected.
	  
	Returns: Void
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	struct TSTriStrip *pLastStrip - last tri-strip node in the linked list
	TSConnectList *pTSList - pointer to the array of linked lists
	TSParam *pTSParam - tri-strip export options
*----------------------------------------------------------------*/
void TSConnectTriStrips(TSMesh *pTSMesh, struct TSTriStrip *pLastStrip, TSConnectList *pTSList, TSParam *pTSParam){
	int connectionFlag = 1;
	int invalidFlag = 0;      // If TSNode is no longer valid
	int connectFlag = 0;	  // If the whole face list is not valid
	int counter=0;
	struct TSNode *pTSNode, *pTmpTSNode;
		
	while(connectionFlag){
		connectionFlag = 0;
		for(unsigned int f=0; f<pTSMesh->uNumFaces; f++){
			if(((pTSNode = pTSList[f].head) == NULL) || (pTSList[f].connect))
				continue;
			while(pTSNode != NULL){
				if(!pTSNode->invalid){
					for(int n=0; n<3; n++){
						if(pTSNode->nfaces[n] == NO_FACE)
							continue;
						pTmpTSNode = pTSList[pTSNode->nfaces[n]].head;
						while(pTmpTSNode != NULL){
							if(!pTmpTSNode->invalid){
								if((pTSNode->smoothID == pTmpTSNode->smoothID) && (pTSNode->matID == pTmpTSNode->matID)){
									if(TSCombineTriStrips(pTSMesh, pTSNode, pTmpTSNode, &pLastStrip, pTSList, pTSParam)){
										pTSNode->invalid = pTmpTSNode->invalid = 1;
										counter++;
										connectionFlag = 0;
										break;	// tsNode is invalid so break
									}
									connectFlag = 1;
								}
							}
							pTmpTSNode = pTmpTSNode->next;
						}
					}
				}
				pTSNode = pTSNode->next;
			}
		}
	}
}

/*---------------------------------------------------------------*
TSRemoveInvalidStrips:
	
	When two tri-strips are combined to form one tri-strip, the
	new tri-strip is added to the linked list.  The other tri-strips
	are marked as invalid.  This is where the invalid tri-strips
	are removed from the linked list of tri-strips.
	  
	Returns: Void
	
	Parameters:
	struct TSTriStrip *pTSTmp - linked list of tri-strips
*----------------------------------------------------------------*/
void TSRemoveInvalidStrips(struct TSTriStrip *pTSTmp){
	struct TSTriStrip *pTSTmp2, *pTSTmp3;

	while(pTSTmp != NULL){
		pTSTmp2 = pTSTmp->next;
		while(pTSTmp2 != NULL){
			if(pTSTmp2->invalid){
				pTSTmp3 = pTSTmp2;
				pTSTmp2 = pTSTmp2->next;	
				free(pTSTmp3->indices);
				free(pTSTmp3);
			}
			else
				break;
		}
		pTSTmp->next = pTSTmp2;
		pTSTmp = pTSTmp->next;
	}
}


/*---------------------------------------------------------------*
TSCombineTriStrips:
	
	Takes two TSNodes and checks all the various cases to see
	if the two nodes can be combined with a minimum number of 
	repeated vertices.  If the nodes can be combined, then the 
	rules for the two node types are followed and then the 
	new tri-strip is added to the linked list.
	  
	Returns: 
	int - TRUE: tri-strips can be combined
		 FALSE: tri-strips can not be combined together.

	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	struct TSNode *pStrip1 - pointer to node in front of first tri-strip
	struct TSNode *pStrip2 - pointer to node in front of second tri-strip
	struct TSTriStrip **ppLastStrip - node for the lastStrip in the linked list
	TSConnectList *pTSList - pointer to the reconnect tri-strip data structure
	TSParam *pTSParam - tri-strip export options
*----------------------------------------------------------------*/
int TSCombineTriStrips(TSMesh *pTSMesh, struct TSNode *pStrip1, struct TSNode *pStrip2, struct TSTriStrip **ppLastStrip, TSConnectList *pTSList, TSParam *pTSParam){
	struct TSTriStrip *pNewStrip;
	TSEdge edge, edge2;
	TSFace *pFace1, *pFace2;
	int addFlag = 0;
	int location = 0;
	unsigned long counter = 0;
	int numVerts;
	int edgeType;
	int size;		// Sizeof TSIndex
	unsigned long tmpVert, tmp2Vert;
	TSIndex *StripIndex1 = pStrip1->tstrip->next->indices; 
	TSIndex *StripIndex2 = pStrip2->tstrip->next->indices;

	if(pStrip1->tstrip->next->invalid || pStrip2->tstrip->next->invalid)
		return FALSE;

	pNewStrip = (struct TSTriStrip *) malloc(sizeof(struct TSTriStrip));
	pNewStrip->matID = pStrip1->matID;
	pNewStrip->invalid = 0;

	// Compare Edge types of each tri-strip node
	switch(pStrip1->edgetype){
	//-------------------------------------------------------
	case Orphan_Edge:
		switch(pStrip2->edgetype){
		//*****************************
//Orphan::Orphan
		case Orphan_Edge:
			addFlag = 0;
			break;
		//*****************************
// Orphan:Start_Edge
		case Start_Edge:
			addFlag = 1;
			numVerts = pStrip2->tstrip->next->numIndices;
			size = sizeof(TSIndex);
			pFace1 = &pTSMesh->pFace[pStrip1->faceIndex];
			pFace2 = &pTSMesh->pFace[pStrip2->faceIndex];

			if(TSCompareTexCoords(pTSMesh, &pTSMesh->pFace[pStrip1->faceIndex], &pTSMesh->pFace[pStrip2->faceIndex], pTSParam->texcoordFlag)){
				addFlag = 0;
				break;
			}

			edge.a = pStrip2->tstrip->next->indices[0].vert;
			edge.b = pStrip2->tstrip->next->indices[1].vert;
			edgeType = TSGetFaceOrientation(pTSMesh->pEdge, &edge, pFace1);
			switch(edgeType){
			case ACTIVE_EDGE:
				pNewStrip->numIndices = pStrip2->tstrip->next->numIndices + 2;
				pNewStrip->indices = (TSIndex *) malloc(pNewStrip->numIndices * size);
				tmpVert = TSFindVertex3(edge.a, edge.b, pFace1);
				TSStoreIndices(pTSMesh, pNewStrip->indices, &location, tmpVert,  pFace1->uFaceIndex);
				memcpy(&pNewStrip->indices[1], &pStrip2->tstrip->next->indices[1], size);
				memcpy(&pNewStrip->indices[2], &pStrip2->tstrip->next->indices[0], size);
				memcpy(&pNewStrip->indices[3], &pStrip2->tstrip->next->indices[1], size);
				memcpy(&pNewStrip->indices[4], &pStrip2->tstrip->next->indices[2], size);
				memcpy(&pNewStrip->indices[5], &pStrip2->tstrip->next->indices[3], (numVerts-3) * size);
				break;
			case SWAP_EDGE:
				pNewStrip->numIndices = pStrip2->tstrip->next->numIndices + 2;
				pNewStrip->indices = (TSIndex *) malloc(pNewStrip->numIndices * size);
				tmpVert = TSFindVertex3(edge.a, edge.b, pFace1);
				TSStoreIndices(pTSMesh, pNewStrip->indices, &location, tmpVert,  pFace1->uFaceIndex);
				TSStoreIndices(pTSMesh, pNewStrip->indices, &location, edge.b,  pFace1->uFaceIndex);
				memcpy(&pNewStrip->indices[2], &pStrip2->tstrip->next->indices[0], (pNewStrip->numIndices-1) * size);
				break;
			case INVALID_EDGE:
				pNewStrip->numIndices = pStrip2->tstrip->next->numIndices + 2;
				pNewStrip->indices = (TSIndex *) malloc(pNewStrip->numIndices * size);
				tmpVert = TSFindVertex3(edge.a, edge.b, pFace2);
				tmp2Vert = TSFindVertex3(tmpVert, edge.a, pFace1);
				TSStoreIndices(pTSMesh, pNewStrip->indices, &location, tmp2Vert,  pFace1->uFaceIndex);
				memcpy(&pNewStrip->indices[1], &pStrip2->tstrip->next->indices[0], size);
				memcpy(&pNewStrip->indices[2], &pStrip2->tstrip->next->indices[2], size);

				memcpy(&pNewStrip->indices[3], &pStrip2->tstrip->next->indices[1], size);
				memcpy(&pNewStrip->indices[4], &pStrip2->tstrip->next->indices[2], size);
				memcpy(&pNewStrip->indices[5], &pStrip2->tstrip->next->indices[3], (numVerts-3) * size);
				
				break;
			}
			break;
		//*****************************
// Orphan:End_Edge
		case End_Edge:
			numVerts = pStrip2->tstrip->next->numIndices;
			addFlag = 1;
			size = sizeof(TSIndex);
			pFace1 = &pTSMesh->pFace[pStrip1->faceIndex];
			pFace2 = &pTSMesh->pFace[pStrip2->faceIndex];
			
			if(TSCompareTexCoords(pTSMesh, &pTSMesh->pFace[pStrip1->faceIndex], &pTSMesh->pFace[pStrip2->faceIndex], pTSParam->texcoordFlag)){
				addFlag = 0;
				break;
			}

			edge.a = pStrip2->tstrip->next->indices[numVerts-2].vert;
			edge.b = pStrip2->tstrip->next->indices[numVerts-1].vert;
			edgeType = TSGetFaceOrientation(pTSMesh->pEdge, &edge, pFace1);
			switch(edgeType){
			case ACTIVE_EDGE:
				pNewStrip->numIndices = numVerts + 1;
				pNewStrip->indices = (TSIndex *) malloc(pNewStrip->numIndices * size);
				tmpVert = TSFindVertex3(edge.a, edge.b, pFace1);
				memcpy(&pNewStrip->indices[0], &pStrip2->tstrip->next->indices[0], numVerts * size);
				location = pNewStrip->numIndices - 1;
				TSStoreIndices(pTSMesh, pNewStrip->indices, &location, tmpVert,  pFace1->uFaceIndex);
				break;
			case SWAP_EDGE:
				pNewStrip->numIndices = numVerts + 2;
				pNewStrip->indices = (TSIndex *) malloc(pNewStrip->numIndices * size);
				tmpVert = TSFindVertex3(edge.a, edge.b, pFace2);

				memcpy(pNewStrip->indices, pStrip2->tstrip->next->indices, (numVerts-1) * size);
				location = numVerts-1;;
				TSStoreIndices(pTSMesh, pNewStrip->indices, &location, tmpVert, pFace2->uFaceIndex);
				TSStoreIndices(pTSMesh, pNewStrip->indices, &location, edge.b, pFace2->uFaceIndex);
				tmp2Vert = TSFindVertex3(tmpVert, edge.b, pFace1);
				TSStoreIndices(pTSMesh, pNewStrip->indices, &location, tmp2Vert, pFace1->uFaceIndex);
				break;
			case INVALID_EDGE:
				addFlag = 0;
				/*
				// debug this goodiron problem
				newStrip->numIndices = strip2->tstrip->next->numIndices + 2;
				newStrip->indices = (TSIndex *) malloc(newStrip->numIndices * size);
				tmpVert = findVertex3(edge.a, edge.b, face2);

				memcpy(&newStrip->indices[0], &strip2->tstrip->next->indices[0], (newStrip->numIndices-4) * size);
				location = newStrip->numIndices - 3;
				storeIndices(tsmesh, newStrip->indices, &location, tmpVert, face2->faceIndex);
				storeIndices(tsmesh, newStrip->indices, &location, edge.b, face2->faceIndex);
				tmp2Vert = findVertex3(tmpVert, edge.b, face1);
				storeIndices(tsmesh, newStrip->indices, &location, tmp2Vert, face1->faceIndex);
				break;
				*/
			}
			break;

		//*****************************
		}
		break;
	//-------------------------------------------------------
	case Start_Edge:
		unsigned long numVertsS1, numVertsS2;
		addFlag = 1;

		switch(pStrip2->edgetype){
		//*****************************
// Start_Edge:Orphan_Edge
		case Orphan_Edge:
			addFlag = 0;
			break;
		//*****************************
// Start_Edge:Start_Edge
		case Start_Edge:
			addFlag = 1;
			numVertsS1 = pStrip1->tstrip->next->numIndices;
			numVertsS2 = pStrip2->tstrip->next->numIndices;
			size = sizeof(TSIndex);
			pFace1 = &pTSMesh->pFace[pStrip1->faceIndex];
			pFace2 = &pTSMesh->pFace[pStrip2->faceIndex];

			if(TSCompareTexCoords(pTSMesh, &pTSMesh->pFace[pStrip1->faceIndex], &pTSMesh->pFace[pStrip2->faceIndex], pTSParam->texcoordFlag)){
				addFlag = 0;
				break;
			}

			edge.a = pStrip2->tstrip->next->indices[0].vert;
			edge.b = pStrip2->tstrip->next->indices[1].vert;
			edge2.a = pStrip1->tstrip->next->indices[0].vert;
			edge2.b = pStrip1->tstrip->next->indices[1].vert;
			
			if(TSCompareEdge(&edge, &edge2))
				edgeType = 1;
			else
				if((edge.b == pFace1->a) || (edge.b == pFace1->b) || (edge.b == pFace1->c))
					edgeType = 0;
				else
					edgeType = -1;

			switch(edgeType){
			case ACTIVE_EDGE:
				if(edge.a == edge2.a){
					addFlag = 0;
					/*
					newStrip->numIndices = numVertsS1 + numVertsS2 - 1;
					newStrip->indices = (TSIndex *) malloc(newStrip->numIndices * size);
					memcpy(&newStrip->indices[0], &strip2->tstrip->next->indices[0], (numVertsS2-1) * size);
					memcpy(&newStrip->indices[numVertsS2-1], &strip1->tstrip->next->indices[1], (numVertsS1-1) * size);
					*/
				}
				else{
					if((numVertsS1 % 2) == 0){
						pNewStrip->numIndices = numVertsS1 + numVertsS2 - 2;
						pNewStrip->indices = (TSIndex *) malloc(pNewStrip->numIndices * size);
						memcpy(&pNewStrip->indices[0], &pStrip1->tstrip->next->indices[numVertsS1-1], size);
						memcpy(&pNewStrip->indices[1], &pStrip1->tstrip->next->indices[numVertsS1-2], size);
						memcpy(&pNewStrip->indices[2], &pStrip1->tstrip->next->indices[numVertsS1-3], size);
						counter = 3;
					}
					else{
						pNewStrip->numIndices = numVertsS1 + numVertsS2 - 1;
						pNewStrip->indices = (TSIndex *) malloc(pNewStrip->numIndices * size);
						memcpy(&pNewStrip->indices[0], &pStrip1->tstrip->next->indices[numVertsS1-1], size);
						memcpy(&pNewStrip->indices[1], &pStrip1->tstrip->next->indices[numVertsS1-3], size);
						memcpy(&pNewStrip->indices[2], &pStrip1->tstrip->next->indices[numVertsS1-2], size);
						memcpy(&pNewStrip->indices[3], &pStrip1->tstrip->next->indices[numVertsS1-3], size);
						counter = 4;
					}
					for(int s1=numVertsS1-4; s1 >= 0 ; s1--){
						memcpy(&pNewStrip->indices[counter], &pStrip1->tstrip->next->indices[s1], size);
						counter++;
					}
					memcpy(&pNewStrip->indices[counter], &pStrip2->tstrip->next->indices[2], (numVertsS2 - 2) * size);
				}
				break;
			case SWAP_EDGE:
				addFlag = 0;
				/*
				newStrip->numIndices = numVertsS1 + numVertsS2 + 1;
				newStrip->indices = (TSIndex *) malloc(newStrip->numIndices * size);
				memcpy(&newStrip->indices[0], &strip2->tstrip->next->indices[0], numVertsS2 * size);
				memcpy(&newStrip->indices[numVertsS2], &strip2->tstrip->next->indices[numVertsS2-1], size);
				memcpy(&newStrip->indices[numVertsS2+1], &strip1->tstrip->next->indices[0], numVertsS1 * size);
				*/
				break;

			case INVALID_EDGE:
				// If the vertex zero of Strip1 and Strip2 are equal
				if(pStrip1->tstrip->next->indices[0].vert == pStrip2->tstrip->next->indices[0].vert){
					if((numVertsS1 % 2) == 0){
						pNewStrip->numIndices = numVertsS1 + numVertsS2;
						pNewStrip->indices = (TSIndex *) malloc(pNewStrip->numIndices * size);
						memcpy(&pNewStrip->indices[0], &pStrip1->tstrip->next->indices[numVertsS1-1], size);
						memcpy(&pNewStrip->indices[1], &pStrip1->tstrip->next->indices[numVertsS1-2], size);
						memcpy(&pNewStrip->indices[2], &pStrip1->tstrip->next->indices[numVertsS1-3], size);
						counter = 3;
					}
					else{
						pNewStrip->numIndices = numVertsS1 + numVertsS2 + 1 ;
						pNewStrip->indices = (TSIndex *) malloc(pNewStrip->numIndices * size);
						memcpy(&pNewStrip->indices[0], &pStrip1->tstrip->next->indices[numVertsS1-1], size);
						memcpy(&pNewStrip->indices[1], &pStrip1->tstrip->next->indices[numVertsS1-3], size);
						memcpy(&pNewStrip->indices[2], &pStrip1->tstrip->next->indices[numVertsS1-2], size);
						memcpy(&pNewStrip->indices[3], &pStrip1->tstrip->next->indices[numVertsS1-3], size);
						counter = 4;
					}

					for(int s1=numVertsS1-4; s1 >= 0 ; s1--){
						memcpy(&pNewStrip->indices[counter], &pStrip1->tstrip->next->indices[s1], size);
						counter++;
					}
					memcpy(&pNewStrip->indices[counter], &pStrip1->tstrip->next->indices[1], size);
					counter++;
					memcpy(&pNewStrip->indices[counter], &pStrip2->tstrip->next->indices[1], (numVertsS2 - 1) * size);
				}
				else
					addFlag = 0;
				break;
			}
			break;
		//*****************************
// Start_Edge:End_Edge
		case End_Edge:
		// Connect Two Strips
			numVertsS1 = pStrip1->tstrip->next->numIndices;
			numVertsS2 = pStrip2->tstrip->next->numIndices;
			size = sizeof(TSIndex);
			pFace1 = &pTSMesh->pFace[pStrip1->faceIndex];
			pFace2 = &pTSMesh->pFace[pStrip2->faceIndex];

			if(TSCompareTexCoords(pTSMesh, &pTSMesh->pFace[pStrip1->faceIndex], &pTSMesh->pFace[pStrip2->faceIndex], pTSParam->texcoordFlag)){
				addFlag = 0;
				break;
			}

			edge.a = pStrip2->tstrip->next->indices[numVertsS2-2].vert;
			edge.b = pStrip2->tstrip->next->indices[numVertsS2-1].vert;
			edge2.a = pStrip1->tstrip->next->indices[0].vert;
			edge2.b = pStrip1->tstrip->next->indices[1].vert;
			
			if(TSCompareEdge(&edge, &edge2))
				edgeType = 1;
			else
				if((edge.b == pFace1->a) || (edge.b == pFace1->b) || (edge.b == pFace1->c))
					edgeType = 0;
				else
					edgeType = -1;

			switch(edgeType){
			case ACTIVE_EDGE:
				if(edge.a == edge2.a){
					pNewStrip->numIndices = numVertsS1 + numVertsS2 - 2;
					pNewStrip->indices = (TSIndex *) malloc(pNewStrip->numIndices * size);
					memcpy(&pNewStrip->indices[0], &pStrip2->tstrip->next->indices[0], (numVertsS2-1) * size);
					memcpy(&pNewStrip->indices[numVertsS2-1], &pStrip1->tstrip->next->indices[1], (numVertsS1-1) * size);
				}
				else{
					pNewStrip->numIndices = numVertsS1 + numVertsS2 - 1;
					pNewStrip->indices = (TSIndex *) malloc(pNewStrip->numIndices * size);
					memcpy(&pNewStrip->indices[0], &pStrip2->tstrip->next->indices[0], (numVertsS2) * size);
					memcpy(&pNewStrip->indices[numVertsS2], &pStrip1->tstrip->next->indices[1], (numVertsS1-1) * size);
				}
				break;
			case SWAP_EDGE:
				if(pStrip1->tstrip->next->indices[0].vert == pStrip2->tstrip->next->indices[numVertsS2-1].vert){
					addFlag = 0;
				}
				else{
					addFlag = 0;
				}

				break;
			case INVALID_EDGE:
				addFlag = 0;
				break;
			}
			break;
		//*****************************
		}
		break;
	//-------------------------------------------------------
	case End_Edge:
		switch(pStrip2->edgetype){
		//*****************************
// End_Edge:Orphan_Edge
		case Orphan_Edge:
			addFlag = 0;
			break;
		//*****************************
// End_Edge:Start_Edge
		case Start_Edge:
			addFlag = 0;
			break;
		//*****************************
// End_Edge:End_Edge
		case End_Edge:
			addFlag = 0;
			
		//*****************************
		}
		break;
	//-------------------------------------------------------
	}

	if(addFlag){
		long int iStartFaceIndex, iEndFaceIndex;
		TSNode *pStartNode, *pEndNode;

		// Add new strip
		(*ppLastStrip)->next = pNewStrip;
		pNewStrip->next = NULL;

		pStrip1->tstrip->next->invalid = 1;
		pStrip2->tstrip->next->invalid = 1;

		// Add new strip to itslist
		iStartFaceIndex = pNewStrip->indices[0].fIndex;
		iEndFaceIndex = pNewStrip->indices[(pNewStrip->numIndices - 1)].fIndex;

		if(iStartFaceIndex != iEndFaceIndex){

			pStartNode = TSCreateTSNode();
			pEndNode   = TSCreateTSNode();

			pStartNode->tstrip = pEndNode->tstrip = *ppLastStrip;
			pStartNode->invalid = pEndNode->invalid = 0;
			pStartNode->edgetype = Start_Edge;
			pEndNode->edgetype = End_Edge;

			pStartNode->smoothID = pEndNode->smoothID = pTSMesh->pFace[iStartFaceIndex].uSmoothId;
			pStartNode->matID = pEndNode->matID = pTSMesh->pFace[iStartFaceIndex].uMatId;
			pStartNode->faceIndex = iStartFaceIndex;
			pEndNode->faceIndex = iEndFaceIndex;

			// Watch for duplicate face when getting neigboring faces
			TSGetRNeighboringFaces(pTSMesh, &pTSMesh->pFace[iStartFaceIndex], &pTSMesh->pFace[pNewStrip->indices[3].fIndex], pStartNode->nfaces);
			TSGetRNeighboringFaces(pTSMesh, &pTSMesh->pFace[iStartFaceIndex], &pTSMesh->pFace[pNewStrip->indices[(pNewStrip->numIndices - 2)].fIndex], pEndNode->nfaces);

			pStartNode->next = pTSList[iStartFaceIndex].head;
			pEndNode->next = pTSList[iEndFaceIndex].head;
			pTSList[iStartFaceIndex].head = pStartNode;
			pTSList[iEndFaceIndex].head = pEndNode;
		}

		*ppLastStrip = (*ppLastStrip)->next;
	}
	else
		free(pNewStrip);

	return addFlag;
};


/*---------------------------------------------------------------*
TSGetRNeighboringFaces:
	
	Gets the neighboring faces for the current face.
	  
	Returns: Void
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	TSFace *pCurrentFace - current active face
	TSFace *pNextFace -	next face to be checked
	int iFaces[3] - Neighboring faces to the current face
*----------------------------------------------------------------*/
void TSGetRNeighboringFaces(TSMesh *pTSMesh, TSFace *pCurrentFace, TSFace *pNextFace, int iFaces[3]){
	TSEdge *pEdgeTmp;
	TSFace *pFace;

	for(int i=0; i<3; i++)
		iFaces[i] = -1;

	for(i=0; i<3; i++){
		pEdgeTmp = &pTSMesh->pEdge[pCurrentFace->iEdges[i]];
		if((pFace = TSGetFacesFromEdge(pTSMesh, pCurrentFace, pEdgeTmp)) != NULL)
			if((pFace != pCurrentFace) && (pFace != pNextFace))
				iFaces[i] = pFace->uFaceIndex;
	}
}


/*---------------------------------------------------------------*
TSCreateTSNode:
	
	Creates a new TSNode and return a pointer to that node.
	  
	Returns: 
	struct TSNode * -  a new TSNode
	
	Parameters: None
*----------------------------------------------------------------*/
struct TSNode *TSCreateTSNode(){
	struct TSNode *pTmpNode;

	pTmpNode = (struct TSNode *) malloc(sizeof(struct TSNode));
	pTmpNode->next = NULL;

	return pTmpNode;
};
/*************************************************************************
   END SECTION: Reconnect Triangle Strips
**************************************************************************/


/*************************************************************************
   BEGIN SECTION: Error Checking
**************************************************************************/

/*---------------------------------------------------------------*
TSErrorChecking:
	
	Check to incoming TSMesh and TSParam to make sure that 
	they contain valid data.
	  
	Returns: 
	TSRESULT - result of checking the validity of tsmesh
	
	Parameters:
	TSMesh *pTSMesh - tri-strip mesh
	TSParam *pTSParameters - tri-strip export options
	TSPARMA *pTSParam - tri-strip export options (default)
*----------------------------------------------------------------*/
TSRESULT TSErrorChecking(TSMesh *pTSMesh, TSParam *pTSParameters, TSParam *pTSParam){

	if(pTSMesh->uNumVertices < 3){
		pTSMesh->pVertex = NULL;
		return TS_NO_VERTEX_DATA;
	}

	if(pTSMesh->uNumFaces <= 0){
		pTSMesh->pFace = NULL;
		return TS_NO_FACE_DATA;
	}

	if(pTSMesh->pFace == NULL)
		return TS_INVALID_FACE_DATA;

	if(pTSMesh->pVertex == NULL)
		return TS_INVALID_VERTEX_DATA;

	if(pTSParameters == NULL){
		pTSParam->bConnectFlag = 1;
		pTSParam->texcoordFlag = NONE;
		pTSParam->progressCallback = NULL;
	}
	else{
		if((pTSParameters->bConnectFlag != 0) && (pTSParameters->bConnectFlag != 1))
			pTSParam->bConnectFlag = 1;
		else
			pTSParam->bConnectFlag = pTSParameters->bConnectFlag;
		
		if((pTSParameters->texcoordFlag < 0) || (pTSParameters->texcoordFlag > 2))
			pTSParam->texcoordFlag = NONE;
		else
			if(pTSMesh->pTexCoord != NULL)
				pTSParam->texcoordFlag = pTSParameters->texcoordFlag;
			else
				pTSParam->texcoordFlag = NONE;

		if(pTSParameters->progressFrequency <= 0)		
			pTSParam->progressFrequency = (int)((float)pTSMesh->uNumFaces * 0.10f);
		else
			pTSParam->progressFrequency = pTSParameters->progressFrequency;

		pTSParam->progressCallback = pTSParameters->progressCallback;
	}

	return TS_SUCCESS;
}
/*************************************************************************
   END SECTION: Error Checking
**************************************************************************/