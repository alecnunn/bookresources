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

// Cartoon.h

#ifndef CARTOON_H
#define CARTOON_H

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Mesh.h"
#include "Edge.h"
#include "mtxlib.h"
#include "MeshUtils.h"

class Cartoon
{
private:

	// Inker member variables
	F32		m_fCreaseAngle;				 //  Artist's specified crease angle
	U32		m_uNumEdges;				//  Number of edges from mesh
	U32		m_uNumRenderEdges; //  Number of edges to be rendered
	Edge *m_pEdges;							// Edges from the mesh
	U16    *m_pRenderEdges;			//  Edges found via DetectEdges function
	vector3 *m_pFaceNormals;	   //  Face normals for each face
	// End Inker member variables

	MeshUtils m_MeshUtils;				//  Functions that operate on the mesh
public:
	Cartoon();
	~Cartoon();

	// Get Functions
	F32 GetCreaseAngle();

	// Set Functions
	void SetCreaseAngle(F32 fCreaseAngle);

	// Inker Functions
	void InitializeInker(Mesh *pMesh);						
	void AllocateFaceNormals(Mesh *pMesh);	 
	void GetUniqueEdgeList(Mesh *pMesh);
	void CheckEdgeListForDiscontinuities(Mesh *pMesh);
	void DetectEdges(Mesh *pMesh, vector3 *pEyeVector);
	void AddEdgeToRenderEdges(Edge *pEdge, int & uNumVerts);

	// OpenGL render functions
	// Render lines takes a mesh, color, and a line thickness as parameters
	void RenderLines(Mesh *pMesh,  vector3 pvColor, F32 thickness);

};

// AddEdgesToRenderEdges:
//		This functions takes an Edge as input and adds its vertex indices
//		to the edge list to be rendered.
inline void Cartoon::AddEdgeToRenderEdges(Edge *pEdge, int & uNumVerts)
{	
	m_pRenderEdges[uNumVerts++] = pEdge->GetVertexIndex(0);
	m_pRenderEdges[uNumVerts++] = pEdge->GetVertexIndex(1);
}

inline F32 Cartoon::GetCreaseAngle()
{
	return m_fCreaseAngle;
}

inline void Cartoon::SetCreaseAngle(F32 fCreaseAngle)
{
	m_fCreaseAngle = fCreaseAngle;
}

#endif // CARTOON_H