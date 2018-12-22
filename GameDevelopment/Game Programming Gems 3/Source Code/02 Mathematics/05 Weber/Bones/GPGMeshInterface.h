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

#ifndef __GPGMeshInterface_h__
#define __GPGMeshInterface_h__



/**************************************************************************//**
	The float buffers are assumed to contain 3 values (X,Y,Z).

*//***************************************************************************/
class GPGMeshInterface
{
	public:
					GPGMeshInterface(void)
					{
						m_meshid= 999999999L;
					}

					// for multi-mesh, choose which we refer to
virtual	void		ChooseMeshIndex(U32 set)			{ m_meshid=set; }
		U32			GetMeshIndex(void) const			{ return m_meshid; }
virtual	U32			GetNumberMeshes(void) const			{ return 0; }
virtual	GPGString	GetMeshName(void)					{ return ""; }
virtual	GPGString	GetParentMeshName(void)				{ return ""; }

					// reading
virtual	U32			GetNumberVertices(void) const							=0;
virtual	U32			GetNumberNormals(void) const							=0;
virtual	U32			GetNumberTexCoords(void) const		{ return 0; }
virtual	U32			GetNumberFaces(void) const								=0;

					// for MRM
					/// set current resolution, not total number of vertices
virtual	void		SetNumberVertices(U32 set)								{}

virtual	U32			GetMaxNumberVertices(void) const
											{ return GetNumberVertices(); }
virtual	U32			GetMaxNumberNormals(void) const
											{ return GetNumberNormals(); }
virtual	U32			GetMaxNumberTexCoords(void) const
											{ return GetNumberTexCoords(); }
virtual	U32			GetMaxNumberFaces(void) const
											{ return GetNumberFaces(); }

virtual	const float	*GetVertexConst(U32 index) const						=0;
virtual	const float	*GetNormalConst(U32 index) const						=0;
virtual	const float	*GetTexCoordConst(U32 index) const	{ return NULL; }

virtual	const U32	*GetFaceVerticesConst(U32 index) const					=0;
virtual	const U32	*GetFaceNormalsConst(U32 index) const					=0;
virtual	const U32	*GetFaceTexCoordsConst(U32 index) const	{ return NULL; }

					/// allocate a given number of empty meshes
virtual	BOOL		CreateMeshes(U32 meshes)			{ return false; }
					/// allocate space for mesh (use ChooseMeshIndex() to pick)
virtual	BOOL		CreateMesh(U32 vertices,U32 faces)	{ return false; }
					/// change space for mesh and move data as necessary
virtual	BOOL		ResizeMesh(U32 vertices,U32 faces)	{ return false; }

virtual	void		SetVertex(U32 index,const float vert[3])				=0;
virtual	void		SetNormal(U32 index,const float vert[3])				=0;
virtual	void		SetTexCoord(U32 index,const float vert[2])				{}

virtual GPGVertexIndexer *GetVertexIndexer(void)		{ return NULL; }

virtual	void		SetFaceVertices(U32 index,const U32 vertID[3])			{}
virtual	void		SetFaceNormals(U32 index,const U32 normID[3])			{}
virtual	void		SetFaceTexCoords(U32 index,const U32 coordID[3])		{}

	protected:
		U32			GetMeshID(void) const				{ return m_meshid; }

	private:
		U32			m_meshid;
};



#endif /* __GPGMeshInterface_h__ */

