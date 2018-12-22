/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Triangle Strip Creation, Optimizations, and Rendering Gem

Copyright (c) 2002, Intel Corporation.
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

//  MeshUtils.cpp
//		This functions includes the member functions form MeshUtils.h

#include "MeshUtils.h"
#include <string.h>


/**************************************************************
	MeshToTSMesh:
	 Places the Mesh data into the TSMesh.
	 The TSMesh is the triangle strip mesh which is used to
	 create the triangle strips.  Then use InsertTriStripsIntoMesh
	 to retrieve the tri-strips after they are created.
**************************************************************/
BOOL MeshUtils::MeshToTSMesh(Mesh *pMesh, TSMesh *pTSMesh){
	unsigned long uNumVertices, uNumFaces, uNumTexCoords;
	TSV3D   *pTSIndex;
	TSFace  *pTSFace;
	Face	*pFacePtr;
	char    buf[256];

	if((pMesh->GetNumVertices() < 3) || (pMesh->GetNumFaces() <= 0))
		return false;

	// Get the name of the mesh
	sprintf(buf, "%s","TriangleStripMesh");
	pTSMesh->pMeshName = (char *) malloc(strlen(buf)+1);
	sprintf(pTSMesh->pMeshName,"%s", buf);
		
	//Get Number of Vertices and Faces
	uNumVertices = pTSMesh->uNumVertices = pMesh->GetNumVertices();
	uNumFaces    = pTSMesh->uNumFaces    = pMesh->GetNumFaces();
	uNumTexCoords= pMesh->GetNumVertices();
	

	// copy the vertex array
	pTSIndex = pTSMesh->pVertex = (TSV3D *) malloc(sizeof(TSV3D) * uNumVertices);
	for (unsigned int i=0; i<uNumVertices; i++, pTSIndex++) {
		vector3 *tsVert = pMesh->GetVertexPosition(i);
		pTSIndex->x = tsVert->x;
		pTSIndex->y = tsVert->y;
		pTSIndex->z = tsVert->z;
	}

	// copy texture coordinates - this is for texture seam checks
	if(pMesh->GetNumTextures() <= 0){
		pTSMesh->pTexCoord = NULL;
	}
	else{
		pTSMesh->pTexCoord = (TSUV *) malloc(sizeof(TSUV) * uNumTexCoords);
		TSUV *tvpool = pTSMesh->pTexCoord;
		for(unsigned long tv = 0; tv < uNumTexCoords; tv++){
			vector2 *tsUV = pMesh->GetVertexTexCoord(tv);
			tvpool[tv].u = tsUV->x;
			tvpool[tv].v = tsUV->y;
		}
	}
	
	// copy the face array
	pTSFace = pTSMesh->pFace = (TSFace *) malloc(sizeof(TSFace) * uNumFaces);
	pFacePtr= pMesh->GetFacePtr();
		
	for (i=0; i<uNumFaces; i++, pTSFace++, pFacePtr++) {
		pTSFace->a = pFacePtr->Get(0);
		pTSFace->b = pFacePtr->Get(1);
		pTSFace->c = pFacePtr->Get(2);

		/*
		// copy texture indices
		if(maxTVFace)
		{
			pTSFACE->ta = maxTVFace[i].t[0];
			pTSFACE->tb = maxTVFace[i].t[1];
			pTSFACE->tc = maxTVFace[i].t[2];
		}
		else
		{
			pTSFACE->ta = NO_TEX_COORDS;
			pTSFACE->tb = NO_TEX_COORDS;
			pTSFACE->tc = NO_TEX_COORDS;
		}
		*/

		// Get the Smoothing Group ID and Material ID
		pTSFace->uSmoothId = 1;
		pTSFace->uMatId = 1;
	}

	return true;
}


/**************************************************************
	InsertTriStripsIntoMesh:
	 Places the tri-strip data into an Mesh.
	 The tri-strips access the Mesh's faces and vertices to 
	 retrieve the proper attributes. 
**************************************************************/
void MeshUtils::InsertTriStripsIntoMesh(TSMesh *pTSMesh, Mesh *pMesh){
	TSTriStrip *tsnode = pTSMesh->tshead->next;
	TSIndex *tsIndices;
	int numTS   = 0;		// # of Triangle Strips
	int tsIndex = 0;		// Index into the tri-strips for an Mesh
	TriStrip *pTriStrips;
	// Get total number of tri-strips that represent tsmesh
	while(tsnode != NULL){
		numTS++;
		tsnode = tsnode->next;
	}

	pMesh->SetNumTriStrips(numTS);
	pTriStrips = new TriStrip[numTS];
	 
	// Walk the tri-strips to convert into Mesh format
	tsnode = pTSMesh->tshead->next;
	while(tsnode != NULL){
		TriStrip *tstmp = &pTriStrips[tsIndex];
		
		tsIndex++;			// Increment to index the next tri-strip
		tstmp->uNumIndices = tsnode->numIndices;

		tsIndices = tsnode->indices;

		// Get the material ID for the first face of the tri-strip
		//tsattrib->matID = imesh->pFaceAttrib[tsIndices[0].fIndex].matID;

		// Allocate the vertices and normals needed in the Mesh
		tstmp->pIndices = (unsigned int *) malloc(tsnode->numIndices * sizeof(unsigned int));
		
		// If there is no material ID then set it to zero, else check the textureflag
		//if(tsattrib->matID  < 0)
		//	texTempFlag = 0;
		//else
		//	texTempFlag = iMat[tsattrib->matID].textureFlag;

		int tsfaceIndex;
		// Walk the indices of a single strip and store in the IMESH data structure
		for(unsigned int i=0; i<tsnode->numIndices; i++){
			tsfaceIndex = tsIndices[i].fIndex;
			Face *pFace;

			pFace = pMesh->GetFacePtr();
			// Which vertex in the Face
			switch(tsIndices[i].fVert){
	 			case VertexA:
					tstmp->pIndices[i] = pFace[tsfaceIndex].Get(0);
					break;
				case VertexB:
					tstmp->pIndices[i] = pFace[tsfaceIndex].Get(1);
					break;
				case VertexC:
					tstmp->pIndices[i] = pFace[tsfaceIndex].Get(2);
					break;
			};
		}	

		// Walk along the link list of tri-strips
		tsnode = tsnode->next;
	}

	// Remove TriStrip Mesh
	FreeTSMesh(pTSMesh);

	pMesh->SetTriStripPtr(&pTriStrips);
}

/**************************************************************
	FreeTSMesh:
	 Frees up the member variable for tri-strip mesh : pTSMesh
**************************************************************/
void MeshUtils::FreeTSMesh(TSMesh *pTSMesh)
{
	struct TSTriStrip *tsnode, *tstmpnode;

	if(pTSMesh->pFace)
		free(pTSMesh->pFace);
	if(pTSMesh->pEdge)
		free(pTSMesh->pEdge);
	if(pTSMesh->pMeshName)
		free(pTSMesh->pMeshName);
	if(pTSMesh->pTexCoord)
		free(pTSMesh->pTexCoord);
	if(pTSMesh->pVertex)
		free(pTSMesh->pVertex);

	tsnode = pTSMesh->tshead;
	while(tsnode != NULL){
		tstmpnode = tsnode->next;
		free(tsnode);
		tsnode = tstmpnode;
		if(tsnode != NULL)
			if(tsnode->indices)
				free(tsnode->indices);
	}
}