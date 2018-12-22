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
//	GPGMeshInterleaved.h
//
//	DESCRIPTION
//
//	NOTES
//

#ifndef GPGMeshInterleaved_h
#define GPGMeshInterleaved_h

#include "GPGMesh.h"

#define IID_GPGMeshInterleaved  7001

class GPGRenderD3D;
class GPGRenderOGL;
class GPGToonRenderOGL;


class GPGMeshInterleaved : public GPGMesh
{
	friend class GPGRenderD3D;
	friend class GPGRenderOGL;
	friend class GPGToonRenderOGL;
	friend class CGPGRenderLayerOpenGL;
	friend class CGPGRenderLayerDirectX52;
	friend class CGPGRenderLayerDirectX70;

public:
	GPGMeshInterleaved();
    U32 GetNumFaces();
    U32 SetNumFaces(U32 numberFaces);
   
    U32 GetNumVertices();
   	U32 SetNumVertices(U32 numberVertices);
    
	U32 GetMaxNumVertices();
    U32 GetMaxNumFaces();
	
    GPGRESULT GetVertexIter(GPGVertexIter* pVertexIter);

    GPGRESULT GetFaceIter(GPGFaceIter* pFaceIter);
    GPGRESULT GetPositionIter(GPGVector3Iter* pPositionIter);
    GPGRESULT GetNormalIter(GPGVector3Iter* pNormalIter);
    GPGRESULT GetColorIter(U32Iter* pColorIter);
    GPGRESULT GetTexCoordIter(GPGVector2Iter* pTexCoordIter, U32 layer = 0);
    
	void SetMeshFactory(GPGMeshFactory* pMeshFactory);
	GPGMeshFactory* GetMeshFactory();

    GPGRESULT Allocate(GPGVertexAttributes attributeFlags,
                     U32 maxNumVertices,
					 U32 maxNumFaces);

	GPGRESULT PartialCopy(GPGMesh *pSourceMesh, 
						 U32 uStartVert,
						 U32 uEndVert,
						 U32 uStartFace,
						 U32 uEndFace
						);
	GPGRESULT Copy(GPGMesh *pSourceMesh);

    GPGVertexAttributes GetAttributes();
    U32 GetMaxNumberVertices();
    U32 GetMaxNumberFaces();

    
    GPGRESULT GetInterface(U32 interfaceIdentifier, void** ppInterface);
	~GPGMeshInterleaved();

	// This is the rendering interface. It just returns a token or "code"
	// so that the rendering system knows which GPGMesh derived class 
	// to cast to and which rendering function to use.
	// The scene manager might want to sort meshes based on the "code"
	// so that state changes are minimized.
	GPGRenderCode GetRenderCode();

	BOOL IsDiffuseBGR();
	BOOL IsSpecularBGR();
	GPGRESULT SwizzleDiffuse();
	GPGRESULT SwizzleSpecular();

private:

	U32 numVerts;
	U8  *pVerts;
	U32 numFaces;
	GPGFace *pFaces;

	U32 maxVerts;
	U32 maxFaces;

	GPGMeshFactory* m_pMeshFactory;
	GPGVertexAttributes attribs;
	GPGVertexIter::AttributeOffsets offsets;
	GPGRenderCode renderCode;

	// Track the state of the vertex colors. -- could be integrated in to vertex attributes
	BOOL m_bBGRDiffuse;
	BOOL m_bBGRSpecular;

	// for use by the rendering object, which is a friend of this class.
	U8* GetVertices() { return pVerts; };
	GPGFace* GetFaces() { return pFaces; };

	// Used for reallocating the size of the mesh -- copies existing data.
	GPGRESULT IncreaseSize(U32 uNumFaces, U32 uNumVerts);
};



inline U32 GPGMeshInterleaved::GetNumFaces() { return numFaces; }
inline U32 GPGMeshInterleaved::SetNumFaces(U32 numberFaces) { return numFaces = numberFaces; }

inline U32 GPGMeshInterleaved::GetNumVertices() { return numVerts; }
inline U32 GPGMeshInterleaved::SetNumVertices(U32 numberVertices) { return numVerts = numberVertices; }

inline U32 GPGMeshInterleaved::GetMaxNumVertices() { return maxVerts; }
inline U32 GPGMeshInterleaved::GetMaxNumFaces()  { return maxFaces; }

inline void GPGMeshInterleaved::SetMeshFactory(GPGMeshFactory* pMeshFactory) { m_pMeshFactory = pMeshFactory; }
inline GPGMeshFactory* GPGMeshInterleaved::GetMeshFactory() { return m_pMeshFactory; }

inline GPGVertexAttributes GPGMeshInterleaved::GetAttributes() { return attribs; }
inline U32 GPGMeshInterleaved::GetMaxNumberVertices() { return maxVerts; }
inline U32 GPGMeshInterleaved::GetMaxNumberFaces()    { return maxFaces; }

inline GPGRenderCode GPGMeshInterleaved::GetRenderCode() { return renderCode; }

inline BOOL GPGMeshInterleaved::IsDiffuseBGR() { return m_bBGRDiffuse; };
inline BOOL GPGMeshInterleaved::IsSpecularBGR() { return m_bBGRSpecular;};

#endif
