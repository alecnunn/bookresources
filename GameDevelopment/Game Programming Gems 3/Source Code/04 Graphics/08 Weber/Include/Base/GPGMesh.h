/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/
//
//	GPGMesh interface
//
//	DESCRIPTION
//    The Toolkit achieves its render performance by partitioning 
//    3D meshes into smaller face sets (or simply meshes) with same 
//    material and texture properties.  This is done to minimize 
//    video card state changes during rendering.  GPGMesh is the 
//    interface to a single material mesh (a triangle mesh where all
//    the faces have the same material and thus all vertices have 
//    same format).  To specify an entire model, meshes are grouped 
//    together inside a MeshGroup which also specifies a material 
//    for each mesh.
//
//	NOTES
//    The interface is not specific to a particular data layout.
//    This allows the layout to be changed to improve performance 
//    while minimizing changes to the rest of the code base.
//
//    Rendering code uses the token returned by GetRenderCode() and 
//    GetInterface() to obtain, in a safe manner, a pointer to the 
//    class implementation.  The GPGMesh implementations include 
//    methods for directly accessing the mesh data for rendering
//    purposes.
//

#ifndef GPGMesh_h
#define GPGMesh_h

#include "GPGDataTypes.h"
#include "GPGResult.h"
#include "GPGIterators.h"
#include "GPGRender.h"
#include "GPGMeshFactory.h"


class GPGMesh
{
public:
	// Get the number of faces in the mesh
    virtual U32  GetNumFaces() = 0;

	// Set the number of faces in the mesh
    virtual U32  SetNumFaces(U32 numFaces) = 0;

	// Get the number of vertices in the mesh
    virtual U32  GetNumVertices() = 0;

	// Set the number of vertices in the mesh
	virtual U32  SetNumVertices(U32 numVertices) = 0;

	// Get the maximum (allocated) number of vertices
	virtual U32  GetMaxNumVertices() = 0;

	// Get the maximum (allocated) number of faces
    virtual U32  GetMaxNumFaces() = 0;

	// Get the mesh attributes
	virtual GPGVertexAttributes GetAttributes() = 0;

	// Initialize a face iterator for accessing faces in this mesh.
	// Create a new GPGFaceIter and pass its pointer into this
	// method to be initialized.
    virtual GPGRESULT GetFaceIter(GPGFaceIter* pFaceIter) = 0;

	// Initialize a vertex iterator for accessing the attribute
	// information for vertices in this mesh.
	//
	// NOTE: If you only need to access one or two vertex 
	//   attributes it is recommended that you use a more 
	//   specific iterator.  See GetPositionIter(), 
	//   GetNormalIter(), GetColorIter(), and GetTexCoordIter().
	//   This may result in a performance gain.
    virtual GPGRESULT GetVertexIter(GPGVertexIter* pVertexIter) = 0;

	// Initialize a vector iterator for accessing 
	// the position attribute of vertices in this mesh.
    virtual GPGRESULT GetPositionIter(GPGVector3Iter* pPositionIter) = 0;

	// Initialize a vector iterator for accessing 
	// the normal attribute of vertices in this mesh.
    virtual GPGRESULT GetNormalIter(GPGVector3Iter* pNormalIter) = 0;

	// Initialize a U32 iterator for accessing 
	// the diffuse color attribute of vertices in this mesh.
    virtual GPGRESULT GetColorIter(U32Iter* pColorIter) = 0;

	// Initialize a vector iterator for accessing the 
	// texture coordinate attribute of vertices in this mesh.
	// Specify the texture layer using the "layer" argument.
    virtual GPGRESULT GetTexCoordIter(GPGVector2Iter* pTextureIter, U32 layer = 0) = 0;
    

	// Store the pointer to the mesh factory that created this
	// instance.
	virtual void SetMeshFactory(GPGMeshFactory* pMeshFactory) = 0;

	// Obtain a pointer to the mesh factory that created this mesh.
	// The mesh factory can be used to create a new (empty) mesh 
	// with the same class implementation (vertex data layout) and 
	// same attributes.
	//
	// A high-performance copy can be achieved using the Copy() method 
	// on two meshes with the same class implementation and attributes.
	virtual GPGMeshFactory* GetMeshFactory() = 0;

	// Specify the mesh attributes and allocate memory for the
	// vertex and face data.  The mesh data is not initialized
	// to known values.  Use the iterators to set the mesh data.
	// Only call Allocate() once per mesh. Allocate() should be called before
	// any other methods, except Copy() and SetMeshFactory().
    virtual GPGRESULT Allocate(GPGVertexAttributes attributeFlags,
                              U32 maxNumVertices,
                              U32 maxNumFaces) = 0;

	// IncreaseSize() will create a new mesh of the larger dimensions (uNumFaces, uNumVerts),
	// and copy the old mesh data into it, then delete the previously existing mesh. 
	// A successful call to Allocate() must occur prior to a call to IncreaseSize().
	virtual GPGRESULT IncreaseSize(U32 uNumFaces, U32 uNumVerts) = 0;

	// Allocate() must be called on pSourceMesh before it can
	// be used as a source mesh for copying.
	//
	// PartialCopy() can be called on the destination (this) mesh before or after 
	// Allocate() is called.  If PartialCopy() is called before Allocate(), the 
	// destination mesh will have the exact same attributes as the source mesh.  
	// If Allocate() is called first, the copy will expand the destination mesh 
	// enough for the copy to complete.
	// 
	// uEndVert and uEndFace are included in the copy.
	virtual GPGRESULT PartialCopy(GPGMesh *pSourceMesh, 
								 U32 uStartVert,
								 U32 uEndVert,
								 U32 uStartFace,
								 U32 uEndFace
								) = 0;

	// Allocate() must be called on pSourceMesh before it can
	// be used as a source mesh for copying.
	//
	// Copy() can be called on the destination (this) mesh before or after 
	// Allocate() is called.  If Copy() is called before Allocate(), the 
	// destination mesh will have the exact same number of vertices, faces,
	// and attributes as the source mesh.  If Allocate() is called first,
	// the copy will fill-in as much common information as possible.  This
	// method allows you to add or remove attributes, vertices, and faces.
	virtual GPGRESULT Copy(GPGMesh *pSourceMesh) = 0;

	// The virtual destructor allows us to delete GPGMesh pointers and
	// the destructor in the implementation will be called.
	virtual ~GPGMesh() {};

	
    // GetInterface can be used to obtain the class implementation in a
	// runtime type-safe manner.  For example:
	//   GPGMeshInterleaved* pMeshInterleaved;
	//   res = GetInterface(IID_GPGMeshInterleaved, (void**)(&pMeshInterleaved));
	//   if (result == GPG_OK)
	//	   // safe to use pMeshInterleaved
	//
	// This method also allows for runtime discovery of extended interfaces.
    virtual GPGRESULT GetInterface(U32 interfaceIdentifier, void** ppInterface) = 0;

	// Interface for rendering system.  Each class implementation of GPGMesh 
	// has a unique render code associated with it.  This render code is
	// used in conjuction with GetInterface to obtain methods that directly
	// access the mesh data for rendering.
	virtual GPGRenderCode GetRenderCode() = 0;


	// Reports the format of the diffuse color channel 
	virtual BOOL IsDiffuseBGR() = 0;

	// Reports the format of the specular color channel 
	virtual BOOL IsSpecularBGR() = 0;

	// Flips the format for the diffuse color channel from BGR to RGB or the other way
	virtual GPGRESULT SwizzleDiffuse() = 0;

	// Flips the format for the specular color channel from BGR to RGB or the other way
	virtual GPGRESULT SwizzleSpecular() = 0;
};


#endif
