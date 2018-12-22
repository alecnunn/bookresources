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

#ifndef __GPGMeshGroup_Impl_h__
#define __GPGMeshGroup_Impl_h__


#define GPGCHEAT_GPGMESH		TRUE	// assume layout for speed


/**************************************************************************//**
	Interface implementation to GPGMesh

*//***************************************************************************/
class GPGMeshGroup_Impl: public GPGMeshInterface
	{
	public:
				GPGMeshGroup_Impl(void)
					{
					m_pMeshFactory=NULL;
					m_meshgroup=NULL;
					m_validmeshgroup=NULL;
					m_allocated=false;
					};
virtual			~GPGMeshGroup_Impl(void)
				{
					if(m_allocated && m_meshgroup)
						delete m_meshgroup;
				}

		GPGMeshGroup	*GetMeshGroup(void)				{ return m_meshgroup; };

		void	SetMeshFactory(GPGMeshFactory *pMeshFactory)
				{	m_pMeshFactory=pMeshFactory; }

		void	SetMeshGroup(GPGMeshGroup *set)
					{
					if(m_allocated && m_meshgroup)
						delete m_meshgroup;
					m_allocated=false;
					m_meshgroup=set;
					};

virtual	U32		GetNumberMeshes(void) const
				{	return (m_meshgroup)? m_meshgroup->GetNumMeshes(): 0; }

				//* implementions of virtuals
virtual	void	ChooseMeshIndex(U32 set)
					{
					GPGMeshInterface::ChooseMeshIndex(set);

					GPGMesh *mesh=m_meshgroup->GetMesh(GetMeshID());
					GPGASSERT(mesh);

					GPGRESULT result;
					result=mesh->GetVertexIter(&m_vertexIndexer);
					GPGASSERT(result==GPG_OK);

					result=mesh->GetFaceIter(&m_faceIter);
					GPGASSERT(result==GPG_OK);

					m_validmeshgroup=m_meshgroup;
					};

virtual GPGString	GetMeshName(void)
						{ return m_meshgroup->GetMeshName(GetMeshID()); };
virtual GPGString	GetParentMeshName(void)
						{ return m_meshgroup->GetParentMeshName(GetMeshID()); };

virtual	void	SetNumberVertices(U32 set)
					{
					GPGASSERT(m_meshgroup);
					m_meshgroup->GetMesh(GetMeshID())->SetNumVertices(set);
					};
virtual	U32		GetNumberVertices(void) const
					{
					GPGASSERT(m_meshgroup);
					return m_meshgroup->GetMesh(GetMeshID())->GetNumVertices();
					};
virtual	U32		GetNumberNormals(void) const
					{
					// each vertex has it's own normal
					return GetNumberVertices();
					};
virtual	U32		GetNumberTexCoords(void) const
					{
					// each vertex has it's own tex coord
					return GetNumberVertices();
					};
virtual	U32		GetNumberFaces(void) const
					{
					GPGASSERT(m_meshgroup);
					return m_meshgroup->GetMesh(GetMeshID())->GetNumFaces();
					};

virtual	U32		GetMaxNumberVertices(void) const
					{
					GPGASSERT(m_meshgroup);
					return m_meshgroup->GetMesh(
											GetMeshID())->GetMaxNumVertices();
					};
virtual	U32		GetMaxNumberNormals(void) const
					{
					return GetMaxNumberVertices();
					};
virtual	U32		GetMaxNumberTexCoords(void) const
					{
					return GetMaxNumberVertices();
					};
virtual	U32		GetMaxNumberFaces(void) const
					{
					GPGASSERT(m_meshgroup);
					return m_meshgroup->GetMesh(GetMeshID())->GetMaxNumFaces();
					};

virtual GPGVertexIndexer *GetVertexIndexer(void)
					{
//					GPGASSERT(m_meshgroup==m_validmeshgroup);
					return &m_vertexIndexer;
					};

virtual
const	float	*GetVertexConst(U32 index) const
					{
					GPGVector3 *vector=GetVertexVector(index);

#if GPGCHEAT_GPGMESH
					return vector->RawConst();
#else
					static float floats[3];

					floats[0]=vector->X();
					floats[1]=vector->Y();
					floats[2]=vector->Z();

					return floats;
#endif
					};

virtual	void	SetVertex(U32 index,const float vert[3])
					{
					GPGVector3 *vector=GetVertexVector(index);
					vector->Raw()[0]=vert[0];
					vector->Raw()[1]=vert[1];
					vector->Raw()[2]=vert[2];
					};

virtual
const	float	*GetNormalConst(U32 index) const
					{
					GPGVector3 *normal=GetNormalVector(index);

#if GPGCHEAT_GPGMESH
					return normal->RawConst();
#else
					static float floats[3];

					floats[0]=normal->X();
					floats[1]=normal->Y();
					floats[2]=normal->Z();

					return floats;
#endif
					};
virtual	void	SetNormal(U32 index,const float vert[3])
					{
					GPGVector3 *normal=GetNormalVector(index);
					normal->Raw()[0]=vert[0];
					normal->Raw()[1]=vert[1];
					normal->Raw()[2]=vert[2];
					};

virtual
const	float	*GetTexCoordConst(U32 index) const
					{
					GPGVector2 *texcoord=GetTexCoordVector(index);
					GPGASSERT(texcoord);

#if GPGCHEAT_GPGMESH
					return &(texcoord->Value(0));
#else
					static float floats[2];

					floats[0]=texcoord->X();
					floats[1]=texcoord->Y();

					return floats;
#endif
					};
virtual	void	SetTexCoord(U32 index,const float vert[2])
					{
					GPGVector2 *texcoord=GetTexCoordVector(index);
					texcoord->X()=vert[0];
					texcoord->Y()=vert[1];
					};

virtual
const	U32		*GetFaceVerticesConst(U32 index) const
					{
					GPGFace *face=GetFace(index);

#if GPGCHEAT_GPGMESH
					return (U32 *)(&face->Vertex(0));
#else
					static U32 array[3];

					array[0]=face->VertexA();
					array[1]=face->VertexB();
					array[2]=face->VertexC();

					return longs;
#endif
					};

virtual
const	U32		*GetFaceNormalsConst(U32 index) const
					{
					return GetFaceVerticesConst(index);
					};

virtual
const	U32		*GetFaceTexCoordsConst(U32 index) const
					{
					return GetFaceVerticesConst(index);
					};

virtual void	SetFaceVertices(U32 index,const U32 vertID[3])
					{
					GPGASSERT(vertID[0]>=0);
					GPGASSERT(vertID[1]>=0);
					GPGASSERT(vertID[2]>=0);

					GPGASSERT(vertID[0]<GetMaxNumberVertices());
					GPGASSERT(vertID[1]<GetMaxNumberVertices());
					GPGASSERT(vertID[2]<GetMaxNumberVertices());

					GPGFace *face=GetFace(index);

					face->VertexA()=vertID[0];
					face->VertexB()=vertID[1];
					face->VertexC()=vertID[2];
					};
virtual void	SetFaceNormals(U32 index,const U32 normID[3])
					{
					// let SetFaceVertices() do the work
					};
virtual void	SetFaceTexCoords(U32 index,const U32 coordID[3])
					{
					// let SetFaceVertices() do the work
					};

virtual BOOL	CreateMeshes(U32 meshes)
				{
					GPGRESULT result=m_meshgroup->Allocate(meshes);
					m_allocated=GPGSUCCESS(result);
					if(m_allocated)
					{
						U32 m;
						for(m=0;m<meshes;m++)
							m_meshgroup->SetMesh(m,m_pMeshFactory->Create());
					}
					return m_allocated;
				}
virtual BOOL	CreateMesh(U32 vertices,U32 faces)
				{
					const GPGVertexAttributes attributeFlags=
					{	1,	// hasPosition
						1,	// hasNormal
						0,	// hasDiffuseColor
						0,	// hasSpecularColor
						1	// numTextureLayers
					};

					GPGASSERT(m_meshgroup);
					GPGRESULT result=m_meshgroup->GetMesh(GetMeshID())
							->Allocate(attributeFlags,vertices,faces);
					return GPGSUCCESS(result);
				}
virtual BOOL	ResizeMesh(U32 vertices,U32 faces)
				{
					GPGASSERT(m_meshgroup);
					GPGRESULT result=m_meshgroup->GetMesh(GetMeshID())
							->IncreaseSize(vertices,faces);
					return GPGSUCCESS(result);
				}

	private:

		GPGVector3	*GetVertexVector(U32 index) const
					{
					GPGASSERT(m_meshgroup);
					GPGASSERT(index>=0);
					GPGASSERT(index<GetMaxNumberVertices());
					GPGASSERT(m_meshgroup==m_validmeshgroup);

					((GPGVertexIndexer &)m_vertexIndexer).MoveToIndex(index);
					GPGASSERT(((GPGVertexIndexer &)m_vertexIndexer)
															.GetPosition());
					return ((GPGVertexIndexer &)m_vertexIndexer).GetPosition();
					};
		GPGVector3	*GetNormalVector(U32 index) const
					{
					GPGASSERT(m_meshgroup);
					GPGASSERT(index>=0);
					GPGASSERT(index<GetMaxNumberVertices());
					GPGASSERT(m_meshgroup==m_validmeshgroup);

					((GPGVertexIndexer &)m_vertexIndexer).MoveToIndex(index);
					GPGASSERT(((GPGVertexIndexer &)m_vertexIndexer)
																.GetNormal());
					return ((GPGVertexIndexer &)m_vertexIndexer).GetNormal();
					};
		GPGVector2	*GetTexCoordVector(U32 index) const
					{
					GPGASSERT(m_meshgroup);
					GPGASSERT(index>=0);
					GPGASSERT(index<GetMaxNumberVertices());
					GPGASSERT(m_meshgroup==m_validmeshgroup);

					((GPGVertexIndexer &)m_vertexIndexer).MoveToIndex(index);
					GPGASSERT(((GPGVertexIndexer &)m_vertexIndexer)
															.GetTexCoord());
					return ((GPGVertexIndexer &)m_vertexIndexer).GetTexCoord();
					};
		GPGFace		*GetFace(U32 index) const
					{
					GPGASSERT(m_meshgroup);
					GPGASSERT(index>=0);
					GPGASSERT(index<GetMaxNumberFaces());
					GPGASSERT(m_meshgroup==m_validmeshgroup);

					GPGASSERT(((GPGFaceIter &)m_faceIter).Index(index));
					return ((GPGFaceIter &)m_faceIter).Index(index);
					};

		GPGVertexIndexer	m_vertexIndexer;
		GPGFaceIter			m_faceIter;
		GPGMeshFactory		*m_pMeshFactory;
		GPGMeshGroup		*m_meshgroup;
		GPGMeshGroup		*m_validmeshgroup;
		BOOL				m_allocated;
	};



#endif /* __GPGMeshGroup_Impl_h__ */
