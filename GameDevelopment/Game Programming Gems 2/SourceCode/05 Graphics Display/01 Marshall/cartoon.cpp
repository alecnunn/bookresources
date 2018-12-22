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
/* This file includes all of the main files for painting and inking 
	a model.  The main functions are listed below:

	Inker:  InitializeInker  -  This functions should be performed at authoring/load-time
				DetectEdges -   Pass in the eyevector and it find list of appropriate edges
				RenderLines -	Renders the lines that were found in the detectedges function

    * These functions do not include error handling, since it doesn't add to the understanding
	   of the Painter or Inker gem. 
 */

#include "Cartoon.h"

/****************************************************************************************/
// ***  INKER FUNCTIONS ***
// This sections includes all of the necessary Inker functions
/****************************************************************************************/

// InitializeInker:
//		1) Allocates a block of memory for rendered edges to be placed in when detected.
//      2) Builds a unique edge list for the given mesh
//      3) Parses the edge list for boundary edge discontinuities
void Cartoon::InitializeInker(Mesh *pMesh)
{
	// Allocate a pool of edges for the rendered edges to be added to when detected
	m_pRenderEdges  = new U16[((U32)pMesh->GetNumFaces()) * 6];

	// Allocate space for the face normals
	AllocateFaceNormals(pMesh);

	// Find the unique edge list for the mesh
	GetUniqueEdgeList(pMesh);	

	// Detect all discontinuities for the edge list
	CheckEdgeListForDiscontinuities(pMesh);
}

// DetectEdges:
//		1)  Detect Boundary Edges
//		2)  Detect Crease Angle Edges
//		3)  Detect Silhouette Edges
void Cartoon::DetectEdges(Mesh *pMesh, vector3 *pEyePosition)
{
	Edge *pEdge;
	vector3 viewVector ;
	vector3 *pFaceNormalA, *pFaceNormalB;
	vector3 *pEdgeVertexPosition;
	U32 uFaceIndexA, uFaceIndexB;
	U32 i;
	F32 uDotProduct1, uDotProduct2;
	int uNumRenderVertices = 0;
	
	// Iterate over the unique edge list
	for(i = 0; i < m_uNumEdges; i++)
	{
		pEdge = &m_pEdges[i]; 

		// Store the faces indices that are associated with this edge
		uFaceIndexA = pEdge->GetFaceIndex(0);
		uFaceIndexB = pEdge->GetFaceIndex(1);
		
		//------------------------------------Boundary Edges Begin-----------------------------------------------------------
		// Boundary edges
		// Here is where you would check for the materials of both faces.  
		// This viewer only uses one material per model so there are no material border edges.
		//  A check for texture coordinate, normal discontinuities, or material borders would go here or in
		//  the function CheckEdgeListForDiscontinuities
		if( pEdge->GetEdgeFlag() & BOUNDARY_EDGE )
		{
			// This is a boundary edge
			AddEdgeToRenderEdges(pEdge, uNumRenderVertices);
			continue;
		}
		//------------------------------------Boundary Edges End--------------------------------------------------------------

		//------------------------------------Crease Angle Edges Begin------------------------------------------------------
		// Crease angle edges
		// Get the face normals for the faces associated with pEdge
		pFaceNormalA = &m_pFaceNormals[uFaceIndexA];
		pFaceNormalB = &m_pFaceNormals[uFaceIndexB];

		// If the dot product of the two face normals exceeds threshold mark it
		// as a crease angle edge
		uDotProduct1 = DotProduct(*pFaceNormalA, *pFaceNormalB);
		if(uDotProduct1 <= m_fCreaseAngle)
		{
			// This is a crease angle edge
			AddEdgeToRenderEdges(pEdge, uNumRenderVertices);
			continue;
		} 
		//------------------------------------Crease Angle Edges End---------------------------------------------------------
				
		//------------------------------------Silhouette Edges Begin------------------------------------------------------------
		// Calculate if it is a silhouette edge
		// Calculate vector from eye
		pEdgeVertexPosition = pMesh->GetVertexPosition(pEdge->GetVertexIndex(0));
		viewVector.x = pEyePosition->x - pEdgeVertexPosition->x;
		viewVector.y = pEyePosition->y - pEdgeVertexPosition->y;
		viewVector.z = pEyePosition->z - pEdgeVertexPosition->z;

		viewVector.normalize();
		
		// Caculate two dot products: Each is the dotproduct between the view vector and the face normal
		uDotProduct1 = DotProduct(viewVector, *pFaceNormalA);
		uDotProduct2 = DotProduct(viewVector, *pFaceNormalB);
		
		// If the sign of the dot product is negative, then it is a silhouette edge
		if( (uDotProduct1 * uDotProduct2) <= 0.00000001f)
		{
				// Silhouette Edge
				AddEdgeToRenderEdges(pEdge, uNumRenderVertices);
		} 
		//------------------------------------Silhouette Edges End--------------------------------------------------------------

	}	// for (all edges)

	// 2 vertices per edge
	m_uNumRenderEdges = uNumRenderVertices  >> 1;	
}

// RenderLines:
//		1)  Setup the Opengl state for line rendering
//		2)  Render the lines detected in the DetectEdges function
//	    3)  Render points between the lines
void Cartoon::RenderLines(Mesh *pMesh,  vector3 pvColor, F32 thickness) 
{
	static int baseStride = sizeof(vector3) * 2;
	static float curColor[4];
	static GLint depthFunc;

	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
	glDepthFunc(GL_LEQUAL);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_LIGHTING);

	char *bytePtr = (char*) pMesh->GetVertexPtr();
	glGetFloatv(GL_CURRENT_COLOR, (float*)curColor);
	glColor3f(pvColor.x, pvColor.y, pvColor.z);
	glVertexPointer(3, GL_FLOAT, baseStride, bytePtr);

	// Set the point size and line thickness in pixel size
	glPointSize(thickness);
	glLineWidth(thickness);

	// Use line and point smooth
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH); 
	glEnable(GL_POINT_SMOOTH);

	// Render the line segments and points between the lines
	glMatrixMode(GL_MODELVIEW);
	glDrawElements(GL_LINES, m_uNumRenderEdges*2, GL_UNSIGNED_SHORT, m_pRenderEdges);

	glDepthFunc(GL_LESS);
	glDrawElements(GL_POINTS, m_uNumRenderEdges*2, GL_UNSIGNED_SHORT, m_pRenderEdges);

	// Disable line and point smooth
	glDisable(GL_BLEND);
	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_LINE_SMOOTH);

	// Set Line width back to the original width of 1.0
	glLineWidth(1.0);

	// return color state to its original state before Inking
	glColor4fv((float*)curColor);

	//glEnable(GL_LIGHTING);
	glEnableClientState(GL_NORMAL_ARRAY);
	glDepthFunc(depthFunc);
}

// Allocate memory for the face normals
// Calculate the face normals - Provides highly accurate silhouette edge detection
//		- if you have animating geometry, then this routine will need to be optimized
//        since it will have to run every frame.  For rounded models, you may get by with 
//		  just using the vertex normals, but most likely you will have missed silhouettes
void Cartoon::AllocateFaceNormals(Mesh *pMesh)
{
	m_pFaceNormals = new vector3[pMesh->GetNumFaces()];

	m_MeshUtils.CalculateFaceNormals(pMesh, m_pFaceNormals);
}

// GetUniqueEdgeList:
//			Call the function CreateUniqueEdgeList from the MeshUtils class
void Cartoon::GetUniqueEdgeList(Mesh *pMesh)
{
	m_MeshUtils.CreateUniqueEdgeList(pMesh, &m_pEdges, &m_uNumEdges);
}

// Function: CheckEdgeListForDiscontinuities()
//		If the mesh exporter separates vertices based upon the following, then
//      this function will mark the edge as a boundary edge:
//		Check for Normal Discontinuities
//		Check for Texture Discontinuities
//		Check for Material Boundary
void Cartoon::CheckEdgeListForDiscontinuities(Mesh *pMesh)
{
	Edge *pEdge;
	U32 uFaceSlot1, uFaceSlot2;

	for(U32 i=0; i < m_uNumEdges; i++)
	{
		pEdge = &m_pEdges[i];
		uFaceSlot1 = pEdge->GetFaceIndex(0);
		uFaceSlot2 = pEdge->GetFaceIndex(1);

		if( (uFaceSlot1 == uFaceSlot2) || (uFaceSlot1 == EMPTY_FACE_SLOT) ||
			(uFaceSlot2 == EMPTY_FACE_SLOT) )
		{
			pEdge->SetEdgeFlag(BOUNDARY_EDGE);
		}
		else
			pEdge->SetEdgeFlag((U32) 0);
	}
}
/****************************************************************************************/
// End of the Inker Functions
/****************************************************************************************/

// Cartoon Constructor
Cartoon::Cartoon()
{
	m_pRenderEdges = NULL;
	m_pEdges = NULL;
	m_pFaceNormals = NULL;
}

// Cartoon Destructor
Cartoon::~Cartoon()
{
	// Clean-up the Inker member variables
	if(m_pEdges)
	{
		delete [] m_pEdges;
		m_pEdges = NULL;
	}

	if(m_pRenderEdges)
	{
		delete [] m_pRenderEdges;
		m_pRenderEdges = NULL;
	}

	if(m_pFaceNormals)
	{
		delete [] m_pFaceNormals;
		m_pFaceNormals = NULL;
	}
}
