/*
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. 

By downloading, copying, installing or using the software you agree to this 
license. If you do not agree to this license, do not download, install, 
copy or use the software. 

Intel Open Source License for Cartoon Rendering: Real-time Silhouette Edge Detection and Rendering sample code

Copyright (c) 2001, Intel Corporation.
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

// SurfOfRevolution.cpp : Defines the entry point for the console application.
//

#include "stdlib.h"
#include "math.h"
#include "mesh.h"
#include "SurfOfRevolution.h"

SurfOfRevolution *surfOfRev;
vector3 *faceNormals;
vector3 *vertexNormals;

void BuildSurfaceOfRevolution(Mesh *mesh);
void BuildMeshFromSurfData(Mesh *mesh);
void CalculateNormals();
void LoadMeshWithSurfData(Mesh *mesh);

void BuildSurfaceOfRevolution(Mesh *mesh)
{
	surfOfRev = new SurfOfRevolution();

	//must make vertex array before face array
	surfOfRev->MakeVertexArray();
	surfOfRev->MakeFaceArray();

	BuildMeshFromSurfData(mesh);

	delete surfOfRev;
}

void BuildMeshFromSurfData(Mesh *mesh)
{
	CalculateNormals();

	LoadMeshWithSurfData(mesh);
}

void LoadMeshWithSurfData(Mesh *mesh)
{
	//loop over the vertices, stick them into the mesh
	//loop over the facelist
	//loop over the normals

	int nRows = surfOfRev->GetNumPoints();
	int nCols = surfOfRev->GetNumSteps();

	mesh->SetNumFaces((((nCols-1)*(nRows-1))*2)+(nRows-1)*2);
	mesh->SetNumTextures(0);
	mesh->SetNumVertices(nRows*nCols);

	// AllocateMesh Parameters:  (does this contain texture coordinates?)
	mesh->AllocateMesh(0);

	Vert *vertPtr = surfOfRev->GetVertexArray();

	for(int i=0;i<nRows*nCols;i++)
	{
		vector3 *vPos;
		vector3 *vNormal;

		vPos = mesh->GetVertexPosition(i);	
		vNormal = mesh->GetVertexNormal(i);

		vPos->x = (F32) vertPtr[i].x;
		vPos->y = (F32) vertPtr[i].y;
		vPos->z = (F32) vertPtr[i].z;

		vNormal->x = (F32) vertexNormals[i].x;
		vNormal->y = (F32) vertexNormals[i].y;
		vNormal->z = (F32) vertexNormals[i].z;

	}

	// Store Face Information
	int *FA =  surfOfRev->GetFaceArray();
	int numFaces=0,  j;
	Face *pFace;
	for(i=0,j=0;i<((nCols-1)*(nRows-1))*2+(nRows-1)*2;i++,j+=3)
	{
		pFace = mesh->GetFacePtr(numFaces++);
		pFace->Set((U32)FA[j], (U32)FA[j+1], (U32)FA[j+2]);
	}

	// Free vertex normals & face normals
	if(vertexNormals)
	{
		free(vertexNormals);
		vertexNormals = NULL;
	}

	if(faceNormals)
	{
		delete [] faceNormals;
		faceNormals = NULL;
	}
}

void CalculateNormals()
{
	int nRows = surfOfRev->GetNumPoints();
	int nCols = surfOfRev->GetNumSteps();
	Vert *VA = surfOfRev->GetVertexArray();
	int *FA =  surfOfRev->GetFaceArray();
	vector3 v1,v2,v3,v4,v5,v6;
	vector3 t1,t2;
	vector3 vNormal;

	faceNormals = new vector3[(nRows*nCols+nRows)*2];
	
	int i,j;
	
	//loops over the faceArray and calculates faceNormals
	for(i=0,j=0;i<((nCols-1)*(nRows-1))*2+(nRows-1)*2;i++,j+=3)
	{
		int v1index,v2index,v3index;
		v1index = FA[j];
		v2index = FA[j+1];
		v3index = FA[j+2];

		//compute a vector from these
		//retrieve the vertex assocated with them
		vector3 v1(VA[v1index].x, VA[v1index].y, VA[v1index].z);
		vector3 v2(VA[v2index].x, VA[v2index].y, VA[v2index].z);
		vector3 v3(VA[v3index].x, VA[v3index].y, VA[v3index].z);
		//v2-v1,v3-v2, normalize result to be safe
		vector3 t1 = v2 - v1;
		vector3 t2 = v3 - v1;
		t1.normalize();
		t2.normalize();
		//do cross product, this is your face normal
		faceNormals[i] = -CrossProduct(t1,t2);
		//normalize just to be safe
		faceNormals[i].normalize();
	}

	//now that i have per face normals,
	//average them for per-vertex normals
	//this is n^2, bad, but only run at load time

	vertexNormals = (vector3 *)malloc(sizeof(vector3)*nRows*nCols);

	//for each vertex, walk the faces to see if any of the vertices match
	//if so, accumulate the face normal
	//loops over all of the vertices
	for(int vert=0;vert<nRows*nCols;vert++)
	{
		vector3 totalV;
		int count;
		count = 0;
		totalV.set(0.0,0.0,0.0);
		//loops over all of the faces, if the faceArray entry has the same index as the vert
		//add it to the total
		for(int iFace=0;iFace<((nCols-1)*(nRows-1))*2*3+(nRows-1)*2*3;iFace++)
		{
			if(FA[iFace] == vert)
			{
				totalV += faceNormals[iFace/3];
				count++;
			}

		}
		totalV /= (float)count;
		totalV.normalize();
		vertexNormals[vert] = totalV;	
	}
	
}

