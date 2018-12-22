
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


#include "GPGBackBones.h"

#define GPGSKIN_STRICT				FALSE

#define GPGSKIN_NEIGHBOR_DEBUG		FALSE
#define GPGSKIN_SMOOTH_DEBUG		FALSE
#define GPGSKIN_REGENERATE_DEBUG	FALSE
#define GPGSKIN_ROGUEPATCH_DEBUG	FALSE
#define GPGSKIN_RELINK_DEBUG		FALSE

#define GPGSKIN_FILTER_DEBUG		FALSE
#define GPGSKIN_FILTER_MIN			0.001f

#define GPGJOINT_USEMOREWEIGHTS		TRUE	// include close relatives
#define GPGFIXED_JOINTS				FALSE	// counteracts joint generation
#define GPGFIXED_JOINTSIZE			2.0f
#define GPGELLIPTICAL_JOINTS		(!GPGFIXED_JOINTS)

#define GPGJOINT_MAXRAD				5.0f	// joint max x,y over bonelength
#define GPGJOINT_MAXASPECT			5.0f	// joint max x/y or y/x
#define GPGJOINT_MAXDISPLACE		0.3f	// joint max cx/rx or cy/ry
#define GPGJOINT_MAXCHILDASPECT		1.0f	// joint max child base/parent tip
#define GPGJOINT_MAXTIPCHANGE		0.1f	// joint max (tip-base)/length
#define GPGJOINT_MINTIPCHANGE		-0.5f	// joint min (tip-base)/length

#define GPGDEFORM_VERTEXORIENTED	TRUE
#define GPGDEFORM_GLOBALORIENTED	TRUE
#define GPGSKIN_FASTNORMALIZE		TRUE
#define GPGSKIN_USE_SQRT_TABLE		TRUE
#define GPGSKIN_SQRT_TABLESIZE		200
#define GPGSKIN_SQRT_SCALESIZE		190


#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGSkin &skin)
	{
	return skin->Print(out,0);
	}


std::ostream &GPGSkin::Print(std::ostream &out,long indention) const
	{
	long m,number=GetOutputMeshConst()->GetMaxNumberVertices();

	out<<"Skin 0x"<<std::hex<<this<<'\n';

	out<<" vertices="<<std::dec<<number<<'\n';
	GPGVector3 vector;
	for(m=0;m<number;m++)
		{
		out<<"Element "<<std::dec<<m<<':';
		vector=GetOutputMeshConst()->GetVertexConst(m);
		out<<vector<<'\n';
		}

	number=m_vertexweights.GetNumberElements();
	out<<"m_vertexweights="<<std::dec<<number<<'\n';
	for(m=0;m<number;m++)
		{
		out<<"Element "<<std::dec<<m<<':';
		out<<m_vertexweights.GetElementConst(m)<<'\n';
		}

	return out;
	}
#endif


GPGString GPGSkin::Out(bool verbose) const
	{
	GPGString result("GPGSkin::Out() not implemented");
	return result;
	}


void GPGSkin::CreateInvSqrtTable(void)
	{
	if(m_invsqrt)
		return;

	m_invsqrt=new float[GPGSKIN_SQRT_TABLESIZE];
	long m;
	for(m=1;m<GPGSKIN_SQRT_TABLESIZE;m++)
		m_invsqrt[m]=1.0f/(float)sqrt(m/(float)(GPGSKIN_SQRT_SCALESIZE-1));

	m_invsqrt[0]=0.0f;
	}



#if FALSE
/******************************************************************************
void GPGSkin::CacheInfluences(void)

******************************************************************************/
void GPGSkin::CacheInfluences(void)
	{
	ComputeVertexOffsets();
//	GetVertexWeights().Dump();

	m_character->CalculateImplantReferences();

//	CacheWeightsToBones();
//	CacheVerticesToBones();
//	AssociateNormalsWithVertexWeights();
//	AssociateNormalsWithWeightedVertices();
	}
#endif


/******************************************************************************
	NOTE should check bones, too
******************************************************************************/
void GPGSkin::CalcBounds(GPGVector3 *min,GPGVector3 *max)
	{
	if(!m_inmesh)
		{
		min->Reset();
		max->Reset();
		return;
		}

	long meshid,meshes=m_inmesh->GetNumberMeshes();

	if(!meshes)
		{
		min->Set(100.0f,100.0f,100.0f);
		max->Set(-100.0f,-100.0f,-100.0f);
		return;
		}

	min->Set(1e9,1e9,1e9);
	max->Set(-1e9,-1e9,-1e9);
	for(meshid=0;meshid<meshes;meshid++)
		{
		m_inmesh->ChooseMeshIndex(meshid);

		long m,vertex,vertices=m_inmesh->GetMaxNumberVertices();
		for(vertex=0;vertex<vertices;vertex++)
			{
			const float *verts=m_inmesh->GetVertexConst(vertex);
			for(m=0;m<3;m++)
				{
				if((*min)[m]>verts[m])
					(*min)[m]=verts[m];
				if((*max)[m]<verts[m])
					(*max)[m]=verts[m];
				}
			}
		}
	}


/******************************************************************************
	find all verts sharing a face or opposing the same edge

	NOTE only manifolds are considered
******************************************************************************/
void GPGSkin::FindNeighbors(GPGMeshInterface *mesh,
										GPGArray<GPGMeshVertexMap> &neighbors)
	{
	GPGArray<GPGMeshVertexMap> &m_replicants=GetReplicants();
	GPGArray<GPGLong3List> edges;	// verts that oppose an edge
	GPGMeshVertex mv1,mv2;
	long vertex2,vertex3;

	neighbors.Clear();

	long meshid,meshes=m_inmesh->GetNumberMeshes();
	neighbors.ResizeToAtLeast(meshes);

	for(meshid=0;meshid<meshes;meshid++)
		{
		m_inmesh->ChooseMeshIndex(meshid);
		edges.Clear();

		//* face mates
		mv1.GetMeshIndex()=meshid;
		mv2.GetMeshIndex()=meshid;
		long vertex,vertices=m_inmesh->GetMaxNumberVertices();

		neighbors[meshid].ResizeToAtLeast(vertices);

		for(vertex=0;vertex<vertices;vertex++)
			neighbors[meshid][vertex].SetAutoDestruct(true);

		GPGListContext context;
		long m,face,faces=mesh->GetMaxNumberFaces();
		for(face=0;face<faces;face++)
			{
			const U32 *verts=mesh->GetFaceVerticesConst(face);
			for(m=0;m<3;m++)
				{
				vertex=verts[m];
				vertex2=verts[(m+1)%3];
				vertex3=verts[(m+2)%3];

				mv1.GetVertexIndex()=vertex;
				mv2.GetVertexIndex()=vertex2;

				// mirror vertices on each other
				if(!neighbors[meshid][vertex].SearchForContent(context,&mv2))
					**(neighbors[meshid][vertex]
											.Append(new GPGMeshVertex))=mv2;
				if(!neighbors[meshid][vertex2].SearchForContent(context,&mv1))
					**(neighbors[meshid][vertex2]
											.Append(new GPGMeshVertex))=mv1;

				//* edge - make sure vertex2 is the larger index
				if(vertex2<vertex)
					{
					long tmp=vertex2;
					vertex2=vertex;
					vertex=tmp;
					}

				edges.ResizeToAtLeast(vertex+1);

				// find any existing edge
				bool found=false;
				GPGLong3 *long3;
				GPGListContext edgecontext;
				edges[vertex].ToHead(edgecontext);
				while((long3=edges[vertex].PostIncrement(edgecontext))!=NULL)
					{
					if((*long3)[0]==vertex2)
						{
						found=true;
						long vertex4=(*long3)[1];

#if GPGSKIN_NEIGHBOR_DEBUG
						// tag as negative to enhance printout
						mv1.GetVertexIndex()= -vertex3;
						mv2.GetVertexIndex()= -vertex4;
#else
						mv1.GetVertexIndex()=vertex3;
						mv2.GetVertexIndex()=vertex4;
#endif

						// mirror across edge
						if(!neighbors[meshid][vertex3]
												.SearchForContent(context,&mv2))
							**(neighbors[meshid][vertex3]
												.Append(new GPGMeshVertex))=mv2;
						if(!neighbors[meshid][vertex4]
												.SearchForContent(context,&mv1))
							**(neighbors[meshid][vertex4]
												.Append(new GPGMeshVertex))=mv1;
						break;
						}
					}
				if(!found)
					(**edges[vertex].Append(new GPGLong3))
													.Set(vertex2,vertex3,0);
				}
			}

		// NOTE verts nearby are potential replicants,
		// but don't get used if they are already associated by face structure

		//* replicants
		for(vertex=0;vertex<vertices;vertex++)
			{
			GPGMeshVertexList &replist=m_replicants[meshid][vertex];
			GPGMeshVertex *mv;
			GPGListContext context,context2;

			replist.ToHead(context);
			while((mv=replist.PostIncrement(context))!=NULL)
				{
				if(!neighbors[meshid][vertex]
										.SearchForContent(context2,mv))
					**(neighbors[meshid][vertex].Append(new GPGMeshVertex))=*mv;
				}
			}
		}

#if GPGSKIN_NEIGHBOR_DEBUG
	for(meshid=0;meshid<meshes;meshid++)
		{
		m_inmesh->ChooseMeshIndex(meshid);
		long vertex,vertices=m_inmesh->GetMaxNumberVertices();

		GPGListContext context;
		GPGMeshVertex *mv;
		for(vertex=0;vertex<vertices;vertex++)
			{
			GPGLOG("%d:%d",meshid,vertex);
			neighbors[meshid][vertex].ToHead(context);
			while((mv=neighbors[meshid][vertex].PostIncrement(context))!=NULL)
				{
				GPGLOG(" %d:%d",mv->GetMeshIndex(),mv->GetVertexIndex());

				if(mv->GetVertexIndex()<0)
					mv->GetVertexIndex()*= -1;
				}
			GPGLOG("\n");
			}
		}
#endif
	}


/******************************************************************************
	find all weights for each vertex
******************************************************************************/
void GPGSkin::MapWeights(GPGMeshInterface *mesh,GPGArray<GPGLongListArray> &map)
	{
	map.Clear();

	long meshid,meshes=m_inmesh->GetNumberMeshes();
	map.ResizeToAtLeast(meshes);
	for(meshid=0;meshid<meshes;meshid++)
		{
		m_inmesh->ChooseMeshIndex(meshid);
		long vertex,vertices=m_inmesh->GetMaxNumberVertices();

		map[meshid].ResizeToAtLeast(vertices);
		for(vertex=0;vertex<vertices;vertex++)
			map[meshid][vertex].SetAutoDestruct(true);

		long m,length=m_vertexweights.GetNumberElements();
		long faces=m_inmesh->GetMaxNumberFaces();

		// create mapping
		for(m=0;m<length;m++)
			{
			GPGVertexWeight &vertexweight=m_vertexweights[m];
/*
			GPGLOG("weight %d %d",m,mesh->GetMeshIndex());
			GPGLOG("/%d",vertexweight.GetMeshIndex());
			GPGLOG(" %d",vertexweight.GetVertexIndex());
			GPGLOG(" %.6G\n",vertexweight.GetBoneWeight());
*/

			if((U32)vertexweight.GetMeshIndex()==mesh->GetMeshIndex())
				{
				vertex=vertexweight.GetVertexIndex();
//				GPGASSERT(vertex<vertices);

				if(vertex<vertices)
					**(map[meshid][vertex].Append(new long))=m;
				}
			}
		}

/*
	for(vertex=0;vertex<vertices;vertex++)
		{
		long *mapping;
		GPGListContext context;

		GPGLOG("for %d/%d",mesh->GetMeshIndex(),vertex);

		map[vertex].ToHead(context);
		while((mapping=map[vertex].PostIncrement(context))!=NULL)
			{
			long mid=m_vertexweights[*mapping].GetMeshIndex();
			long vert=m_vertexweights[*mapping].GetVertexIndex();

			GPGLOG(" %d:%d/%d",*mapping,mid,vert);
			}

		GPGLOG("\n");;
		}
*/
	}


/******************************************************************************
	merges split weights (same vertex, same bone, multiple entries)
	eliminates zero weights
	eliminates weights with meshid or vertid out of bounds
******************************************************************************/
void GPGSkin::FilterWeights(void)
	{
//	GPGArray<GPGLongList> map;		// map each vert to n weight entries
	// map each mesh/vert to n vert weights
	GPGArray<GPGLongListArray> map;

	MapWeights(m_inmesh,map);

	long meshid,meshes=m_inmesh->GetNumberMeshes();
#if TRUE
	for(meshid=0;meshid<meshes;meshid++)
		{
		m_inmesh->ChooseMeshIndex(meshid);
		long vertex,vertices=m_inmesh->GetMaxNumberVertices();

		// merge splits
		for(vertex=0;vertex<vertices;vertex++)
			{
			long *mapping1,*mapping2;
			GPGListContext context1,context2;

			map[meshid][vertex].ToHead(context1);
			while((mapping1=map[meshid][vertex].PostIncrement(context1))!=NULL)
				{
				long bone1=m_vertexweights[*mapping1].GetBoneIndex();

				context2=context1;

				while((mapping2=map[meshid][vertex].PostIncrement(context2))
																	!= NULL)
					{
					float weight1=m_vertexweights[*mapping1]
														.GetBoneWeight();
					long bone2=m_vertexweights[*mapping2].GetBoneIndex();

#if GPGSKIN_FILTER_DEBUG
					GPGLOG("vs %d/%d %d,%d\n",meshid,vertex,bone1,bone2);
#endif

					if(bone1==bone2)
						{
						float weight2=m_vertexweights[*mapping2]
														.GetBoneWeight();

						m_vertexweights[*mapping1]
											.SetBoneWeight(weight1+weight2);
						m_vertexweights[*mapping2].SetBoneWeight(0.0f);

#if GPGSKIN_FILTER_DEBUG
						GPGLOG("merge %d,%d %d/",*mapping1,*mapping2,meshid);
						GPGLOG("%d ",m_vertexweights[*mapping1]
														.GetVertexIndex());
						GPGLOG("%d",m_vertexweights[*mapping2]
														.GetVertexIndex());
						GPGLOG(" %.6G %.6G\n",weight1,weight2);
#endif
						}
					}
				}
			}
		}
#endif

#if TRUE
	long m,length=m_vertexweights.GetNumberElements();
	// remove near zero weights and any bonenodes out-of-bounds
	for(m=0;m<length;m++)
		{
		bool remove=false;
		meshid=m_vertexweights[m].GetMeshIndex();
		m_inmesh->ChooseMeshIndex(meshid);
		long vertices=m_inmesh->GetMaxNumberVertices();

		if(meshid<0 || meshid>=meshes)
			remove=true;
		else
			{
			long vertex=m_vertexweights[m].GetVertexIndex();

			remove=(vertex<0 || vertex>=vertices);
			}

		if(remove || m_vertexweights[m].GetBoneWeight()<GPGSKIN_FILTER_MIN ||
				m_vertexweights[m].GetBoneIndex()<0 ||
				m_vertexweights[m].GetBoneIndex() >=
											m_character->GetBoneTableSize() )
			{
#if GPGSKIN_FILTER_DEBUG
	GPGLOG("remove %d %d:%d ",m,meshid,m_vertexweights[m].GetVertexIndex());
	GPGLOG("of %d ",vertices);
	GPGLOG("bone %d/%d weight %.6G\n",m_vertexweights[m].GetBoneIndex(),
			m_character->GetBoneTableSize(),m_vertexweights[m].GetBoneWeight());
#endif

			// deep copy
			m_vertexweights[m].CopyFrom(m_vertexweights[length-1]);
			length--;
			m--;
			}
		}
	m_vertexweights.ResizeToExactly(length);
#endif
	}


/******************************************************************************
	clear existing weights
	set single weighting to closest bone

	first pass: use_joints=false
	second pass: use_joints=true

	if readonly, use existing vertex weights and just create proximity data
******************************************************************************/
void GPGSkin::RegenerateWeights(bool use_joints,bool readonly)
	{
	const GPGVector3 zero(0.0f,0.0f,0.0f);

	// map each mesh/vert to n vert weights
	GPGArray<GPGLongListArray> map;
	GPGListContext context;
	if(readonly)
		MapWeights(m_inmesh,map);
	else
		GetVertexWeights().Clear();

	long boneid,bones=m_character->GetBoneTableSize();
	GPGVector3 result;

	for(boneid=0;boneid<bones;boneid++)
		{
		GPGBoneNode *bonenode=m_character->LookupBoneIndex(boneid);
		if(!bonenode)
			continue;
		bonenode->BestVertexScore()= -1.0f;
		}

	long meshid,meshes=m_inmesh->GetNumberMeshes();
	for(meshid=0;meshid<meshes;meshid++)
		{
		m_inmesh->ChooseMeshIndex(meshid);
		long vertex,vertices=m_inmesh->GetMaxNumberVertices();

		for(vertex=0;vertex<vertices;vertex++)
			{
			long picked= -1;
			float dist,pickdist= -1.0f;
			boneid=0;

			long *mapping;
			GPGLongList *maplist=NULL;
			if(readonly)
				{
				maplist=&map[meshid][vertex];
				maplist->ToHead(context);
				}

//			for(boneid=0;boneid<bones;boneid++)
			bool done=false;
			while(!done)
				{
				if(readonly)
					{
					mapping=maplist->PostIncrement(context);

					if(!mapping)
						break;

					boneid=m_vertexweights[*mapping].GetBoneIndex();
					}

				GPGBoneNode *bonenode=m_character->LookupBoneIndex(boneid);
				if(bonenode && (!use_joints || bonenode->IsInfluential()))
					{
					float bonelength=bonenode->GetLength();
					long thisbone=boneid;
					bool has_children=(bonenode->Children()
													.GetNumberElements()>0);

					float basejointsize;
					float tipjointsize;

					long pass;
					for(pass=0;pass<2;pass++)
						{
						if(!pass)
							{
							//* check bone
							bonenode->StoredTransform()
									.ReverseTransformVector(
									m_inmesh->GetVertexConst(vertex),result);

							GPGCylinder &bounds=bonenode
													->CylindricalBounds();
							GPGVector3 &basescale=bounds.GetScale(0);

							basejointsize=0.5f*(basescale[1]+basescale[2]);

							// use real tip data
							GPGVector3 &tipscale=bounds.GetScale(1);
							tipjointsize=0.5f*(tipscale[1]+tipscale[2]);
							}
						else
							{
							//* check implant (and give credit to parent)
							if(!(bonenode->Displacement()==zero) &&
											bonenode->Parent()->IsBone() )
								{
								GPGBoneNode *parent=
										(GPGBoneNode *)bonenode->Parent();

								GPGTransform implant=parent
														->StoredTransform();
								implant.Translate(parent->GetLength(),
																	0.0f,0.0f);
								implant.Rotate(
									bonenode->ImplantReferenceRotation());
								implant.ReverseTransformVector(
									m_inmesh->GetVertexConst(vertex),result);

								bonelength=bonenode->ImplantLength();
								thisbone=parent->GetBoneIndex();

								// implant tip uses this bone's base
								GPGCylinder &bounds=
											bonenode->CylindricalBounds();
								GPGVector3 &jointscale=bounds.GetScale(0);
								tipjointsize=0.5f*(jointscale[1]+jointscale[2]);

								// implant base uses parent's tip
								GPGCylinder &parentbounds=
												parent->CylindricalBounds();
								GPGVector3 &parentscale=parentbounds
																.GetScale(1);
								basejointsize=0.5f*
											(parentscale[1]+parentscale[2]);

#if TRUE
								// HACK reduce influence when an implant
								basejointsize*=0.9f;
								tipjointsize*=0.9f;
#endif
								}
							else
								continue;
							}

						float jointsize;
						if(result[0]>0.0f)
							{
							if(result[0]>bonelength)
								{
								// after tip
								result[0]-=bonelength;
								jointsize=1.0f;

#if FALSE
								// no adjust on end bone
								// in case length is not representative
								if(has_children)
#endif
									jointsize-=result[0]/bonelength;
								if(jointsize<0.1f)
									jointsize=0.1f;
								jointsize*=tipjointsize;
								}
							else
								{
								// adjacent to bone
								if(bonelength>0.0f)
									jointsize=basejointsize+
												(tipjointsize-basejointsize)
													*result[0]/bonelength;
								else
									jointsize=basejointsize;

								result[0]=0.0f;
								}
							}
						else
							{
							// before base
							jointsize=(1.0f+result[0]/bonelength);
							if(jointsize<0.1f)
								jointsize=0.1f;
							jointsize*=basejointsize;
							}

						// reduce effect of maverick cross-sections
						const float maxaspect=2.0f;
						if(jointsize>bonelength*maxaspect)
							jointsize=bonelength*maxaspect;

						if(!use_joints)
							jointsize=1.0f;

						if(jointsize<=0.0f)
							continue;
#if TRUE
						result.Scale(1.0f/jointsize);
#else
						result[1]/=jointsize;
						result[2]/=jointsize;
#endif
						dist=result.CalcMagnitude();
						if(picked<0 || dist<pickdist)
							{
							picked=thisbone;
							pickdist=dist;
							}

#if GPGSKIN_REGENERATE_DEBUG
						if(meshid==1 && vertex==447)
							{
							GPGLOG("vert %d:%d bone %d len %.6G %.6G,%.6G=%.6G",
								meshid,vertex,boneid,bonelength,
								basejointsize,tipjointsize,jointsize);
							GPGLOG(" to %s dist %.6G picked=%d\n",
								result.Out().Raw(),dist,picked);
							}
#endif
						}
					}

				if(readonly)
//					boneid=m_vertexweights[*mapping].Offset()[0]=dist; what?
					m_vertexweights[*mapping].Offset()[0]=dist;		// ok?
				else
					done=(++boneid>=bones);
				}

#if GPGSKIN_STRICT
			GPGASSERT(picked>=0);
#endif
			if(picked>=0)
				{
				// remember which vertex scored best for each bone
				GPGBoneNode *bonenode=m_character->LookupBoneIndex(picked);
				GPGASSERT(bonenode);
				float &bestscore=bonenode->BestVertexScore();
				if(bestscore<0.0f || pickdist<bestscore)
					{
					bestscore=pickdist;
					bonenode->BestVertex().SetMeshVertex(meshid,vertex);
					}

				if(!readonly)
					{
					GPGVertexWeight &weight=GetVertexWeights()
														.CreateNewElement();
					weight.SetBoneIndex(picked);
					weight.SetMeshIndex(meshid);
					weight.SetVertexIndex(vertex);
					weight.SetBoneWeight(1.0f);
					weight.Offset()[0]=pickdist;	// temporarily save score
					}
				}
			}
		}
	}


/******************************************************************************
	expects offset[0] to contain proximity score
	uses offset[1] as a validity flag

	finds and replaces disjoint patches of weights
******************************************************************************/
void GPGSkin::RemoveRogueWeights(void)
	{
	GPGLOG("RemoveRogueWeights()\n");

	// map each mesh/vert to n mesh/vert's
	GPGArray<GPGMeshVertexMap> neighbors;
	// map each mesh/vert to n vert weights
	GPGArray<GPGLongListArray> map;
	GPGListContext longcontext;
	long *mapping;

	FindNeighbors(m_inmesh,neighbors);
	MapWeights(m_inmesh,map);

	// reset validity flag
	long m,length=m_vertexweights.GetNumberElements();
	for(m=0;m<length;m++)
		{
		GPGVertexWeight &vertexweight=m_vertexweights[m];
		vertexweight.Offset()[1]=0.0f;		// clear validity flag
		}

#if GPGSKIN_ROGUEPATCH_DEBUG
	GPGLOG("Rogue: fill valid patches <<<<<<<<<<<<<<\n");
#endif
	//* propagate valid patches from best vertex of each bone
	long boneid,bones=m_character->GetBoneTableSize();
	for(boneid=0;boneid<bones;boneid++)
		{
		GPGBoneNode *bonenode=m_character->LookupBoneIndex(boneid);
		if(!bonenode)
			continue;
		if(bonenode->BestVertexScore()>=0.0f)
			{
			U32 meshid=0xffffffff,vertid=0xffffffff;
			bonenode->BestVertex().GetMeshVertex(&meshid,&vertid);

			if(meshid!=0xffffffff && vertid!=0xffffffff)
				{
				map[meshid][vertid].ToHead(longcontext);
				while((mapping=map[meshid][vertid]
										.PostIncrement(longcontext)) != NULL)
					{
					if(mapping && m_vertexweights[*mapping].GetBoneIndex()
																	== boneid)
						{
						FloodFill(neighbors,map,false,*mapping,boneid);
						break;
						}
					}
				}
			}
		}

#if GPGSKIN_ROGUEPATCH_DEBUG
	GPGLOG("Rogue: fill invalid patches <<<<<<<<<<<<<<\n");
#endif
	//* fill in invalid patches with valid data
	for(m=0;m<length;m++)
		{
		GPGVertexWeight &vertexweight=m_vertexweights[m];
		long meshid=vertexweight.GetMeshIndex();
		long vertid=vertexweight.GetVertexIndex();

		if(vertexweight.Offset()[1]==0.0f)
			{
			// search neighbors
			GPGListContext context;
			GPGMeshVertex *neighborvertex;
			neighbors[meshid][vertid].ToHead(context);
			while((neighborvertex=
					neighbors[meshid][vertid].PostIncrement(context)) != NULL)
				{
				long neighmesh=neighborvertex->GetMeshIndex();
				long neighvert=neighborvertex->GetVertexIndex();

				map[neighmesh][neighvert].ToHead(longcontext);
				while((mapping=map[neighmesh][neighvert]
										.PostIncrement(longcontext)) != NULL)
					{
					// if valid neighbor
					if(m_vertexweights[*mapping].Offset()[1]>0.0f)
						{
						FloodFill(neighbors,map,false,m,
									m_vertexweights[*mapping].GetBoneIndex());
						break;
						}
					}

				// if flooded
				if(mapping)
					break;
				}
			}
		}

#if FALSE
	// remap incorrectable vertices to bone 0 for test
	for(m=0;m<length;m++)
		{
		GPGVertexWeight &vertexweight=m_vertexweights[m];
		long meshid=vertexweight.GetMeshIndex();
		long vertid=vertexweight.GetVertexIndex();

		if(vertexweight.Offset()[1]==0.0f)
			vertexweight.GetBoneIndex()=0;
		}
#endif

#if GPGSKIN_ROGUEPATCH_DEBUG
	GPGLOG("Rogue: assign remaining orphans <<<<<<<<<<<<<<\n");
#endif
#if FALSE
	// fill in orphaned verts in meshes with bone used by vert in local mesh
	// with best score
	long meshid,meshes=m_inmesh->GetNumberMeshes();
	for(meshid=0;meshid<meshes;meshid++)
		{
		m_inmesh->ChooseMeshIndex(meshid);
		long vertid,vertices=m_inmesh->GetMaxNumberVertices();

		if(vertices<1)
			continue;

		// scan for best id and fill in
		for(vertid=0;vertid<vertices;vertid++)
			{
			map[meshid][vertid].ToHead(longcontext);
			while((mapping=map[meshid][vertid].PostIncrement(longcontext))
																	!= NULL)
				{
				GPGVertexWeight &vertexweight=m_vertexweights[*mapping];

				if(vertexweight.Offset()[1]==0.0f)
					FloodFill(neighbors,map,true,*mapping,-1);
				}
			}
		}
#endif
	}


class GPGFloodLevel
	{
	public:
		long			weightindex;
		GPGListContext	context;
	};
/******************************************************************************
	validity stored temporarily as non-zero weight
******************************************************************************/
void GPGSkin::FloodFill(const GPGArray<GPGMeshVertexMap> &neighbors,
						const GPGArray<GPGLongListArray> &map,
						bool scanfirst,long weightid,long newboneid)
	{
	GPGArray<GPGFloodLevel> floodstack;
	GPGListContext longcontext;
	long *mapping;

	const long oldboneid=m_vertexweights[weightid].GetBoneIndex();
	U32 meshid=m_vertexweights[weightid].GetMeshIndex();
	U32 vertid=m_vertexweights[weightid].GetVertexIndex();
	long boneid=newboneid;

#if GPGSKIN_ROGUEPATCH_DEBUG
	GPGLOG("FloodFill() vert %d:%d to boneid %d\n",meshid,vertid,boneid);
#endif

	float bestscore=0.0f;
	long pass;
	for(pass=scanfirst;pass>=0;pass--)
		{
#if GPGSKIN_ROGUEPATCH_DEBUG
		GPGLOG(" PASS %d\n",pass);
#endif
		if(pass)
			boneid= -1;
		else if(boneid<0)
			break;

		floodstack.CreateNewElement().weightindex=weightid;

		GPGASSERT(meshid<neighbors.GetNumberElements());
		GPGASSERT(vertid<neighbors[meshid].GetNumberElements());
		neighbors[meshid][vertid].ToHead(floodstack[0].context);

		while(floodstack.GetNumberElements()>0)
			{
			long level=floodstack.GetNumberElements()-1;
			GPGVertexWeight &vertexweight=
								m_vertexweights[floodstack[level].weightindex];

			if(pass)
				{
				if(boneid<0 || bestscore>vertexweight.Offset()[0])
					{
					boneid=vertexweight.GetBoneIndex();
					bestscore=vertexweight.Offset()[0];
					}
				vertexweight.SetBoneIndex(-1);
				}
			else
				vertexweight.SetBoneIndex(boneid);

			vertexweight.Offset()[1]=1.0f;

			meshid=vertexweight.GetMeshIndex();
			vertid=vertexweight.GetVertexIndex();
#if GPGSKIN_ROGUEPATCH_DEBUG
			GPGLOG(" on %d:%d\n",meshid,vertid);
#endif

			// search neighbors
			GPGMeshVertex *neighborvertex;
			GPGASSERT(meshid<neighbors.GetNumberElements());
			GPGASSERT(vertid<neighbors[meshid].GetNumberElements());
			if((neighborvertex=neighbors[meshid][vertid]
							.PostIncrement(floodstack[level].context)) != NULL)
				{
				long neighmesh=neighborvertex->GetMeshIndex();
				long neighvert=neighborvertex->GetVertexIndex();
#if GPGSKIN_ROGUEPATCH_DEBUG
				GPGLOG(" neighbor %d:%d\n",neighmesh,neighvert);
#endif

				GPGASSERT((U32)neighmesh<map.GetNumberElements());
				GPGASSERT((U32)neighvert<map[neighmesh].GetNumberElements());

				// find weight with matching boneid, if any
				map[neighmesh][neighvert].ToHead(longcontext);
				while((mapping=map[neighmesh][neighvert]
										.PostIncrement(longcontext)) != NULL)
					{
					long neighboneid=m_vertexweights[*mapping]
														.GetBoneIndex();

					// if invalid neighbor of oldboneid
					if((scanfirst || neighboneid==oldboneid) &&
							(m_vertexweights[*mapping].Offset()[1]==0.0f ||
							(!pass && neighboneid<0)))
						{
#if GPGSKIN_ROGUEPATCH_DEBUG
						GPGLOG(" push\n");
#endif
						level++;
						floodstack.ResizeToExactly(level+1);
						floodstack[level].weightindex= *mapping;

						GPGASSERT((U32)neighmesh<neighbors.GetNumberElements());
						GPGASSERT((U32)neighvert<neighbors[neighmesh]
													.GetNumberElements());
						neighbors[neighmesh][neighvert].ToHead(
													floodstack[level].context);
						break;
						}
					}
				}
			else
				{
#if GPGSKIN_ROGUEPATCH_DEBUG
				GPGLOG(" pop\n");
#endif

				// pop
				floodstack.ResizeToExactly(level);
				}
			}
		}
	}


void GPGSkin::SmoothWeights(long iterations,float threshhold,
												float weldmax,float modelsize)
	{
#if GPGSKIN_SMOOTH_DEBUG
	GetVertexWeights().Dump();
#endif

//	float maxratio=threshhold/modelsize;
	float maxratio=threshhold;
	weldmax*=modelsize;

	// map each mesh/vert to n mesh/vert's
	GPGArray<GPGMeshVertexMap> neighbors;
	// map each mesh/vert to n vert weights
	GPGArray<GPGLongListArray> map;

	FindNeighbors(m_inmesh,neighbors);
	MapWeights(m_inmesh,map);

	char progresstext[128];

	// smooth
	long pass;
	for(pass=0;pass<iterations;pass++)
		{
		sprintf(progresstext,"Smooth Weights %d%%",100*pass/iterations);
		GPGCharacter::StateProgress(progresstext);

		bool lastpass=(pass==iterations-1);

		long meshid,meshes=m_inmesh->GetNumberMeshes();
		for(meshid=0;meshid<meshes;meshid++)
			{
			m_inmesh->ChooseMeshIndex(meshid);
			long vertex,vertices=m_inmesh->GetMaxNumberVertices();

			for(vertex=0;vertex<vertices;vertex++)
				{
				long *mapping;
				GPGListContext context1,context3;
				GPGListContext context2;
				map[meshid][vertex].ToHead(context1);
				while((mapping=map[meshid][vertex].PostIncrement(context1))
																	!= NULL)
					{
					long boneid=m_vertexweights[*mapping].GetBoneIndex();
					GPGBoneNode *bonenode=m_character->LookupBoneIndex(boneid);
					GPGASSERT(bonenode);

					GPGCylinder &bounds=bonenode->CylindricalBounds();
					GPGVector3 &jointscale=bounds.GetScale(0);
					float jointsize=0.5f*(jointscale[1]+jointscale[2]);

					GPGMeshVertex *neighborvertex;
					neighbors[meshid][vertex].ToHead(context2);
					while((neighborvertex=
							neighbors[meshid][vertex].PostIncrement(context2))
																	!= NULL)
						{
						long neighmesh=neighborvertex->GetMeshIndex();
						long neighvert=neighborvertex->GetVertexIndex();
						float weight=m_vertexweights[*mapping]
														.GetBoneWeight();
						long found= -1;
						float neighweight=0.0f;

						long *neighmapping;
						map[neighmesh][neighvert].ToHead(context3);
						while((neighmapping=map[neighmesh][neighvert]
											.PostIncrement(context3))!=NULL)
							{
							long neighboneid=m_vertexweights[*neighmapping]
														.GetBoneIndex();

							if(neighboneid==boneid)
								{
								found= *neighmapping;
								break;
								}
							}

						if(found>=0)
							neighweight=m_vertexweights[found].GetBoneWeight();

						float delta=weight-neighweight;

						m_inmesh->ChooseMeshIndex(neighmesh);
						const GPGVector3 &neighvector=m_inmesh
												->GetVertexConst(neighvert);
						m_inmesh->ChooseMeshIndex(meshid);

						GPGVector3 difference;
						difference.Subtract(m_inmesh->GetVertexConst(vertex),
																neighvector);
						float distance=difference.CalcMagnitude();
						if(distance<weldmax)
							distance=0.0f;
						else if(lastpass)
							continue;

//						float maxdiff=distance*maxratio;
						float maxdiff=distance*maxratio/jointsize;

						float adjust=(float)fabs(delta)-maxdiff;
						//* if difference is too abrupt
						if(adjust>0.0f)
							{
#if GPGSKIN_SMOOTH_DEBUG
//	bool show=true;
/*
*/
	bool show= (meshid==0 && vertex==485) ||
				(meshid==9 && vertex==1) ||
				(neighmesh==0 && neighvert==485) ||
				(neighmesh==9 && neighvert==1) ;

	if(show)
		{
		GPGLOG("vert %d:%d bone %d to %d:%d ",
								meshid,vertex,boneid,neighmesh,neighvert);
		GPGLOG("%.6G-%.6G=%.6G vs %.6G ",weight,neighweight,delta,maxdiff);
		GPGLOG("dist %.6G\n",distance);
		}
#endif

							//* create new influence if neighbor not already
							if(found<0)
								{
								found=m_vertexweights.GetNumberElements();

#if GPGSKIN_SMOOTH_DEBUG
	if(show)
								GPGLOG(" create %d ",found);
#endif

								m_vertexweights.ResizeToAtLeast(found+1);

								m_vertexweights[found]
											.CopyFrom(m_vertexweights[*mapping]);
								m_vertexweights[found]
											.SetMeshIndex(neighmesh);
								m_vertexweights[found]
											.SetVertexIndex(neighvert);
								m_vertexweights[found].SetBoneWeight(0.0f);

								(**neighbors[neighmesh][neighvert]
										.Append(new GPGMeshVertex))
										.SetMeshVertex(meshid,vertex);
								**(map[neighmesh][neighvert])
										.Append(new long)=found;
								}

							//* smooth difference
							adjust*=0.5;
							if(weight<neighweight)
								adjust= -adjust;

							if(lastpass)
								{
								//* on last pass, only copy weights to
								//* make sure they are equal
								if(meshid<=neighmesh && vertex<neighvert)
									m_vertexweights[found].BoneWeight()=
									m_vertexweights[*mapping].GetBoneWeight();
								else
									m_vertexweights[*mapping].BoneWeight()=
										m_vertexweights[found].GetBoneWeight();
								}
							else
								{
								m_vertexweights[*mapping].BoneWeight()-=adjust;
								m_vertexweights[found].BoneWeight()+=adjust;
								}

#if GPGSKIN_SMOOTH_DEBUG
	if(show)
		{
		GPGLOG(" adjust %.6G",adjust);
		GPGLOG(" %.6G",m_vertexweights[*mapping].GetBoneWeight());
		GPGLOG(" %.6G\n",m_vertexweights[found].GetBoneWeight());
		}
#endif
							}
						}
					}
				}
			}

			//* normalize
#if TRUE
		for(meshid=0;meshid<meshes;meshid++)
			{
			m_inmesh->ChooseMeshIndex(meshid);
			long vertex,vertices=m_inmesh->GetMaxNumberVertices();
			for(vertex=0;vertex<vertices;vertex++)
				{
				float sum=0.0f;
				long *mapping;
				GPGListContext context;

				map[meshid][vertex].ToHead(context);
				while((mapping=map[meshid][vertex]
												.PostIncrement(context))!=NULL)
					sum+=m_vertexweights[*mapping].GetBoneWeight();

#if GPGSKIN_STRICT
				GPGASSERT(sum>0.0f);
#endif
				if(sum>0.0f)
					{
					map[meshid][vertex].ToHead(context);
					while((mapping=map[meshid][vertex]
												.PostIncrement(context))!=NULL)
						m_vertexweights[*mapping].BoneWeight()/=sum;
					}
#if GPGSKIN_SMOOTH_DEBUG
				else
					GPGLOG("empty %d/%d\n",meshid,vertex);
#endif

// HACK
if(sum<0.1f)
	sum=1.0f;
				}
#endif
			}

//		neighbors.Clear();
//		map.Clear();
		}
	}


/******************************************************************************
	WARNING overwrites offset caching; use before ComputeVertexOffsets()

	stores weight ordered boneid in vertex and normal offsets
	then does the bonecache-optimised sort

	if GetVertexOrdered()==true, does not optimise bones,
		so that verts are in ascending order
******************************************************************************/
void GPGSkin::SortVertexWeightsForCache(void)
	{
	m_vertexweights.SortOnMeshThenVertex();
	if(GetVertexOrdered())
		return;

	const long maxweights=6;
	float weights[maxweights];
	long boneids[maxweights];

	long j,m,length=m_vertexweights.GetNumberElements();
	for(m=0;m<length;m++)
		{
		GPGVertexWeight &vertexweight=m_vertexweights[m];
		long vertid=vertexweight.GetVertexIndex();

		// find bounds of like verts
		long n=m;
		long start=n;
		while(n<length && m_vertexweights[n].GetVertexIndex()==vertid)
			n++;
		long end=n;

		// reset
		for(n=0;n<maxweights;n++)
			{
			weights[n]=0.0f;
			boneids[n]= -1;
			}

		// get ordered weights
		for(n=start;n<end;n++)
			{
			float weight=m_vertexweights[n].GetBoneWeight();
			long boneid=m_vertexweights[n].GetBoneIndex();

			j=maxweights-1;
//			while(j>=0 && weights[j]<weight)
			while(j>=0 && boneids[j]<boneid)
				{
				if(j<maxweights-1)
					{
					weights[j+1]=weights[j];
					boneids[j+1]=boneids[j];
					}
				weights[j]=weight;
				boneids[j]=boneid;
				j--;
				}
			}

		// fill in ordered weights
		for(n=0;n<maxweights;n++)
			{
			if(boneids[n]>=0)
				weights[n]=(float)boneids[n];
			}
		// pack
		for(n=start;n<end;n++)
			{
			m_vertexweights[n].Offset()=&weights[0];
			m_vertexweights[n].NormalOffset()=&weights[3];
			}

		m=end-1;
		}

	GetVertexWeights().SortOnMeshThenBonesThenVertex();
//	GetVertexWeights().Dump();
	}


bool GPGSkin::GetMasterMeshVertex(U32 meshid,U32 vertex,
												U32 *mmeshid,U32 *mvertex)
	{
	if(meshid>=m_mastervertices.GetNumberElements() ||
			vertex>=m_mastervertices[meshid].GetNumberElements())
		return false;

	m_mastervertices[meshid][vertex].GetMeshVertex(mmeshid,mvertex);
	return true;
	}


void GPGSkin::RelinkWeights(void)
	{
//	GetVertexWeights().Dump();

	long ancestors=0;

	long m,length=m_vertexweights.GetNumberElements();
	long newlength=length;
	for(m=0;m<length;m++)
		{
		GPGVertexWeight &vertexweight=m_vertexweights[m];
		long meshid=vertexweight.GetMeshIndex();
		long vertid=vertexweight.GetVertexIndex();
		long boneid=vertexweight.GetBoneIndex();
		float childweight=vertexweight.GetBoneWeight();

#if GPGSKIN_RELINK_DEBUG
//		bool show=( (vertexweight.GetMeshIndex()==0 && vertid==2644) ||
//				(vertexweight.GetMeshIndex()==0 && vertid==4095) );
		bool show=( boneid==51 && childweight>0.99);

		if(show)
			{
			GPGLOG("\nvertex=%d:%d childweight=%.6G\n",
						vertexweight.GetMeshIndex(),vertid,childweight);
			}
#endif

//		if(childweight<0.0001 || childweight>0.9999f)
		if(childweight<0.0001)
			continue;

		GPGBoneNode *bonenode=m_character->LookupBoneIndex(boneid);
		GPGASSERT(bonenode);
		if(!bonenode)
			continue;

#if GPGSKIN_RELINK_DEBUG
		if(show)
			{
			GPGLOG("bone %d %s\n",bonenode->GetBoneIndex(),
									bonenode->NameConst().Raw8());
			}
#endif

		GPGCoreNode *parent=bonenode->Parent();
		if(!parent->IsBone())
			continue;

		long parentid=((GPGBoneNode *)parent)->GetBoneIndex();

		GPGBoneLinks &bonelinks=bonenode->BoneLinks();
		long numlinks=bonelinks.GetNumberLinks();
		if(numlinks<1)
			continue;


		float parentweight=0.0f;
		float parcompeteweight=0.0f; // sibling/nephew/etc
		bool has_descendent=false;

		//* find weights for the same vertex (assumes they're sorted)
		long n=m;
		GPGVertexWeight *parententry=NULL;
		while(n>0 && m_vertexweights[n-1].GetVertexIndex()==vertid)
			n--;
		long start=n;
		while(n<length && m_vertexweights[n].GetVertexIndex()==vertid)
			n++;
		long end=n;

		ancestors=0;

		// find all ancestors first
		for(n=start;n<end;n++)
			{
			if(m_vertexweights[n].GetBoneIndex()==parentid)
				{
				parententry=&m_vertexweights[n];
				parentweight=parententry->GetBoneWeight();
				}
			}

		// then, deal with leaves
		for(n=start;n<end;n++)
			{
			if(m_vertexweights[n].GetBoneIndex()==parentid)
				{
				//* other is parent
				continue;
				}
			else
				{
				GPGBoneNode *otherbone=m_character->LookupBoneIndex(
										m_vertexweights[n].GetBoneIndex());
				GPGASSERT(otherbone);
				if(bonenode->HasAncestor(otherbone))
					{
					//* other is ancestor
					continue;
					}
				else if(otherbone->HasAncestor(bonenode))
					{
					//* other is descendent
					has_descendent=true;
					break;
					}
				else
					{
					//* neither is descendent of the other

					//* competitor for parent
					if(n>m && otherbone->HasAncestor(parent))
						parcompeteweight+=
										m_vertexweights[n].GetBoneWeight();
					}
				}
			}

		//* if a descendent will do the relinking, we don't have to
		if(has_descendent)
			continue;

		//* if no parent bone or ancestors, there's nothing to relink with
//		if(!parententry && ancestors==0)
//			continue;

		GPGASSERT(childweight>=0.0f);
		GPGASSERT(parentweight>=0.0f);		// redundant?

		//* how much of parent weight we are allowed to reassign
		float parcompetefraction=parcompeteweight/
											(parcompeteweight+childweight);

#if GPGSKIN_RELINK_DEBUG
		if(show)
			{
			GPGLOG("vertex=%d:%d ancestors=%d ",
							vertexweight.GetMeshIndex(),vertid,ancestors);
			GPGLOG("pre parentweight=%.6G\n",parentweight);
			GPGLOG("parcompeteweight=%.6G parcompetefraction=%.6G\n",
							parcompeteweight,parcompetefraction);
			}
#endif

		parentweight*=(1.0f-parcompetefraction);

		float partialweight=parentweight+childweight;

		float linklength=bonelinks.GetLinkLength()/(float)numlinks;
		GPGVector3 offset;
		CalcLocalOffset(boneid,
						m_vertexweights[m].OffsetConst(),&offset);

		float flinkid=offset[0]/linklength+(numlinks+1)*0.5f;

#if GPGSKIN_RELINK_DEBUG
		if(show)
			{
			GPGLOG("offset=%s linklength=%.6G flinkid=%.6G\n",
								offset.Out().Raw(),linklength,flinkid);
			}
#endif

		long linkid2=(long)flinkid;

		//* if not weighted to parent and beyond chain, leave as is
		if(!parententry && linkid2>numlinks)
			continue;

		if(linkid2<0)
			linkid2=0;
		if(linkid2>numlinks)
			linkid2=numlinks;
		if(linkid2==numlinks+1)		// redundant?
			linkid2=numlinks;
		long linkid1=linkid2-1;

		float substart=(linkid1+1)/(float)(numlinks+1);

		float subfraction=flinkid-substart*(float)(numlinks+1);
		if(subfraction<0.0f)
			subfraction=0.0f;
		if(subfraction>1.0f)
			subfraction=1.0f;

		//* reuse weight to original bone for weight to first link
		//* make new entry for weight to second link
		GPGVertexWeight &newentry=m_vertexweights.CreateNewElement();
		newentry.CopyFrom(vertexweight);

		if(linkid1>=0)
			newentry.SetBoneIndex(
								bonelinks.GetLink(linkid1)->GetBoneIndex());
		else
			newentry.SetBoneIndex(parentid);

		if(linkid2<numlinks)
			vertexweight.SetBoneIndex(
								bonelinks.GetLink(linkid2)->GetBoneIndex());

		if(fabs(subfraction)<0.001f)
			subfraction=0.0f;

		//* normal-derived influence fade
		float jointfade=0.0f;
		if(parententry)
			{
			// jointfade 0.0=inside radius,no loss  1.0=double+ radius,full loss
			GPGCylinder &bounds=bonenode->CylindricalBounds();
			GPGVector3 &jointscale=bounds.GetScale(0);
			// average of x and y radius
			float average=GPGSQRT(jointscale[1]*jointscale[1]+
									jointscale[2]*jointscale[2]);
			float jsx=(offset[0]/average);
			if(jsx<0.0f)
				jsx=0.0f;
/*
			float jsy=(offset[1]/jointscale[1])-1.0f-jsx;
			float jsz=(offset[2]/jointscale[2])-1.0f-jsx;
			if(jsy<0.0f)
				jsy=0.0f;
			if(jsz<0.0f)
				jsz=0.0f;
			jointfade=GPGSQRT(jsy*jsy+jsz*jsz);
			if(jointfade>1.0f)
				jointfade=1.0f;
*/

			// get normal from 'master' pre-replication post-weld vertex
			float cosa=1.0f;
			U32 mesh0=meshid,vertex0=vertid;
//			GetMasterMeshVertex(meshid,vertid,&mesh0,&vertex0);

			GPGMeshVertexList &replist=m_replicants[meshid][vertid];
			GPGListContext context;
			GPGMeshVertex *mv;
			long numreps=replist.GetNumberElements();
			replist.ToHead(context);
			do	{
				mv=replist.PostIncrement(context);
				if(mv)
					mv->GetMeshVertex(&mesh0,&vertex0);
				else
					{
					mesh0=meshid;
					vertex0=vertid;
					}

				m_inmesh->ChooseMeshIndex(mesh0);
				GPGVector3 normal=m_inmesh->GetNormalConst(vertex0);
				GPGVector3 xaxis(1.0f,0.0f,0.0f),boneaxis;
				bonenode->StoredTransform().RotateVector(xaxis,boneaxis);

				float dotproduct=normal.DotProduct(boneaxis);
				if(cosa>dotproduct)
					cosa=dotproduct;
				} while(mv);

			cosa-=jsx;
			if(cosa<0.0f)
				cosa=0.0f;

//			jointfade+=fabs(cosa);
			jointfade=(float)pow(fabs(cosa),2.0f);
			}

		GPGASSERT(jointfade>=0.0f);
		GPGASSERT(jointfade<=1.0f);

// jointfade=0.0f;

		float jointloss=partialweight*jointfade;
		partialweight-=jointloss;

		float subweight=partialweight*subfraction;
		GPGASSERT(subweight>=0.0f);
		GPGASSERT(subfraction>=0.0f);
		GPGASSERT(subfraction<=1.0f);

#if GPGSKIN_RELINK_DEBUG
		if(show)
			{
			GPGLOG("substart=%.6G subfraction=%.6G ",substart,subfraction);
			GPGLOG("parentweight=%.6G childweight=%.6G\n",
													parentweight,childweight);
			GPGLOG("jointfade=%.6G jointloss=%.6G ",jointfade,jointloss);
			GPGLOG("link1=%d/%d subweight=%.6G\n",linkid1,numlinks,subweight);
			}
#endif

		newentry.SetBoneWeight(partialweight-subweight);
		vertexweight.SetBoneWeight(subweight);

		if(parententry)
			{
			parententry->BoneWeight()*=parcompetefraction;

			// WARNING shouldn't just leave for other children to consume
			parententry->BoneWeight()+=jointloss;
			}

/*
		GPGLOG("numlinks=%d weight %.6G+%.6G=%.6G link %d %d\n",
				numlinks,parentweight,childweight,totalweight,
				linkid1,linkid2);

		GPGLOG(" substart=%.6G subfraction=%.6G subweight=%.6G\n",
				substart,subfraction,subweight);
*/
		}
	}


bool GPGSkin::CalcLocalOffset(long boneid,
								const GPGVector3 &world,GPGVector3 *offset)
	{
	GPGBoneNode *bonenode=m_character->LookupBoneIndex(boneid);
	if(!bonenode)
		return false;

	bonenode->StoredTransform().ReverseTransformVector(world,*offset);
	return true;
	}


/******************************************************************************
	if computetips=false, do not compute tip cross-sections in addition
		to base cross-sections
******************************************************************************/
void GPGSkin::CalculateJointCrossSections(bool updatecache,bool computetips)
	{
	if(updatecache)
		{
		//CacheInfluences();
		ComputeVertexOffsets();
		}

	m_character->ForEachNodeTransformed(GPGPARENTFIRST,
								&GPGSkin::CalculateJointsForBone,&computetips);
	}


BOOL GPGSkin::CalculateJointsForBone(GPGCoreNode &node,
									GPGTransform &transform,GPGVoidStar state)
	{
	if(!node.IsBone())
		return false;

	bool *computetips=NULL;
	GPGRESULT result=state.CopyPointerTo(&computetips);
	GPGASSERT(result==GPG_OK);
	GPGASSERT(computetips);

	GPGBoneNode &bonenode=(GPGBoneNode &)node;
	long index=bonenode.GetBoneIndex();
	if(!(index%5))
		{
		char progresstext[128];
		sprintf(progresstext,"Calc Joints %d%%",100*index/
								bonenode.RootCharacter()->GetBoneTableSize());
		GPGCharacter::StateProgress(progresstext);
		}

	CalculateJointForBone(node,transform,0,*computetips);
	if(*computetips)
		CalculateJointForBone(node,transform,1,*computetips);

	return false;
	}


void GPGSkin::CalculateJointForBone(GPGCoreNode &node,
							GPGTransform &transform,bool tip,bool computetips)
	{
	GPGBoneNode &bnode=(GPGBoneNode &)node;

	if(!bnode.IsAutoJoint() || bnode.IsBoneLink())
		return;

	long boneid=bnode.GetBoneIndex();
	float bonelength=bnode.GetLength();

	GPGSkin *skin=bnode.RootCharacter()->GetSkin();

	GPGList<long> family;
	family.SetAutoDestruct(true);
	GPGListContext longcontext;
	**(family.Append(new long))=boneid;
	if(bnode.Parent()->IsBone())
		**(family.Append(new long))=((GPGBoneNode *)bnode.Parent())
															->GetBoneIndex();

	GPGBoneNodeList &children=bnode.Children();
	bool has_children= (bnode.Children().GetNumberElements()>0);
	GPGListContext context;
	GPGBoneNode *child;
	children.ToHead(context);
	while((child=children.PostIncrement(context)) != NULL)
		**(family.Append(new long))=child->GetBoneIndex();

	GPGCylinder &bounds=bnode.CylindricalBounds();
	GPGVector3 &center=bounds.GetCenter(tip);
	GPGVector3 &scale=bounds.GetScale(tip);
	GPGVector3 basescale=bounds.GetScale(0);

	GPGVector3 parentscale;
	bool check_parent=false;
	if(computetips && bnode.Parent()->IsBone())
		{
		parentscale=((GPGBoneNode *)bnode.Parent())
										->CylindricalBounds().GetScale(1);

		if(parentscale.CalcMagnitude()>bonelength*0.01)
			check_parent=true;
		}

	const long passes=16;
	const float expander=1.4f;
	const float reducescale=1.0f;	// 0.95f;
	const float unitdeltamax=0.25f;
	const float deltainfluence=0.5f;
	const float xfade=0.5f;			// 0.25f;

	center.Reset();
	scale.Reset();

	scale[1]=0.1f;
	scale[2]=0.1f;

#if	GPGFIXED_JOINTS
	scale[1]=GPGFIXED_JOINTSIZE;
	scale[2]=GPGFIXED_JOINTSIZE;
	return false;
#endif

	bool has_weights=false;
	GPGArray<GPGVector3> offsets;
	GPGVertexWeights &m_vertexweights=skin->GetVertexWeights();
	long m,length=m_vertexweights.GetNumberElements();
	long index;
	if(length>0)
		{
		for(m=0;m<length;m++)
			{
#if GPGJOINT_USEMOREWEIGHTS
			index=m_vertexweights[m].GetBoneIndex();
			if(family.SearchForContent(longcontext,&index) != NULL)
#else
			if(m_vertexweights[m].GetBoneIndex()==boneid)
#endif
				{
#if GPGDEFORM_GLOBALORIENTED
				GPGVector3 &offset=offsets.CreateNewElement();
				skin->CalcLocalOffset(boneid,
								m_vertexweights[m].OffsetConst(),&offset);
#else
				GPGVector3 &offset=offsets.CreateNewElement();
				offset=m_vertexweights[m].OffsetConst();
#endif
				if(m_vertexweights[m].GetBoneIndex()==boneid)
					has_weights=true;
				if(tip)
					offset[0]-=bonelength;
				}
			}
		}
	else
		{
		const GPGPackVertex *packvertex=NULL;
		const GPGPackBoneWeight *packweight=NULL;

		long weights,n,vertex;
		long meshid,meshes=skin->GetInputMeshConst()->GetNumberMeshes();
		for(meshid=0;meshid<meshes;meshid++)
			{
			skin->GetInputMesh()->ChooseMeshIndex(meshid);

			GPGPackWeights &packweights=skin->GetPackVertexWeights()[meshid];
			packweights.RewindForRead();

			length=packweights.GetNumberVerticesConst();
			for(m=0;m<length;m++)
				{
				packvertex=packweights.NextPackVertexForRead();
				weights=packvertex->GetNumWeights();

				vertex=packvertex->GetVertexIndex();

				for(n=0;n<weights;n++)
					{
					packweight=packweights.NextPackBoneWeightForRead();

					index=packweight->GetBoneIndex();
					if(family.SearchForContent(longcontext,&index) != NULL)
						{
						GPGVector3 &offset=offsets.CreateNewElement();
						skin->CalcLocalOffset(boneid,
										packvertex->OffsetConst(),&offset);

						if(packweight->GetBoneIndex()==boneid)
							has_weights=true;
						if(tip)
							offset[0]-=bonelength;
						}
					}
				}
			}
		}

	if(!has_weights)
		{
		scale.Reset();
		return;
		}

	long numberoffsets=offsets.GetNumberElements();

	float crudescale=1e6f;
//	GPGWeightedVertexArray &weights=bnode.GetWeights();
	GPGVector3 originaloffset,radial;
	float x,angle,axis,delta[2],f[2],change;
//	long pass,n,m,cycle,numberweights=weights.GetNumberElements();
	long pass,n,cycle;

	for(m=0;m<numberoffsets;m++)
		{
//		GPGWeightedVertex &weighted_vertex=weights[m];
//		radial=weighted_vertex.Offset();
		radial=offsets[m];

		x=(float)fabs(radial[0]);
		radial[0]=0.0f;

		float radius=radial.CalcMagnitude()+x;
		if(crudescale>radius)
			crudescale=radius;
		}
	float deltamax=unitdeltamax*crudescale;

	if(numberoffsets<3)
		return;

	scale[1]=crudescale;
	scale[2]=crudescale;

	for(pass=0;pass<passes;pass++)
		{
		float crude2=deltainfluence*crudescale*((passes-pass)/(float)passes);

		scale[1]*=expander;
		scale[2]*=expander;

		delta[0]=0.0f;
		delta[1]=0.0f;

		// move, then resize
		for(cycle=0;cycle<2;cycle++)
			{
			for(m=0;m<numberoffsets;m++)
				{
//				GPGWeightedVertex &weighted_vertex=weights[m];
//				originaloffset=weighted_vertex.Offset();
				originaloffset=offsets[m];

				radial.Subtract(originaloffset,center);
				x=(float)fabs(radial[0])*xfade;
				radial[0]=0.0f;

				angle=GPGATAN2(radial[2],radial[1]);
				f[0]=(float)fabs(GPGCOS(angle))+0.001f;
				f[1]=(float)fabs(GPGSIN(angle))+0.001f;

				for(n=0;n<2;n++)
					{
					axis=((float)fabs(radial[n+1])+x)/f[n];
//						axis=(float)fabs(radial[n+1])/f[n]+x;

					if(scale[n+1]>axis)
						{
						if(cycle)
							scale[n+1]=axis;
						else
							{
							change=crude2*(1.0f-axis/scale[n+1]);

							if(change>0.0f)
								delta[n]+=(originaloffset[n+1]>0.0)?
														-change: change;
							}
						}
					}
#if FALSE
				if(m<4 && !bnode.NameConst().Compare("Bone08"))
					{
					GPGLOG("----------\n");
					GPGLOG("crude2 %.6G pass %d ",crude2,pass);
					GPGLOG("cycle %d m %d ",cycle,m);
					GPGLOG("angle %.6G x %.6G\n",angle,x);
					GPGLOG("%s\n",weighted_vertex.Offset().Out().Raw());
					GPGLOG("scale %.6G %.6G\n",scale[1],scale[2]);
					GPGLOG("center %.6G %.6G\n",center[1],center[2]);
					}
#endif
				}

			if(!cycle)
				{
				for(n=0;n<2;n++)
					{
					if((float)fabs(delta[n])>deltamax)
						delta[n]= (delta[n]>0.0)? deltamax: -deltamax;

					center[n+1]+=delta[n];
					}
				}

			long j;
			for(j=1;j<3;j++)
				{
				if(scale[j]>bonelength*GPGJOINT_MAXRAD)
					scale[j]=bonelength*GPGJOINT_MAXRAD;

				if(scale[j]>scale[3-j]*GPGJOINT_MAXASPECT)
					scale[j]=scale[3-j]*GPGJOINT_MAXASPECT;

				if((float)fabs(center[j])>scale[j]*GPGJOINT_MAXDISPLACE)
					center[j]*=scale[j]*GPGJOINT_MAXDISPLACE/
													(float)fabs(center[j]);

				if(tip)
					{
					float maxdiff=GPGJOINT_MAXTIPCHANGE*bonelength;
					float mindiff=GPGJOINT_MINTIPCHANGE*bonelength;

					float diff=scale[j]-basescale[j];
					if(diff<mindiff)
						diff=mindiff;
					else if(!has_children && diff>0.0f)
						diff=0.0f;
					else if(diff>maxdiff)
						diff=maxdiff;

					scale[j]=basescale[j]+diff;
					}
				else if(check_parent)
					{
					if(scale[j]>parentscale[j]*GPGJOINT_MAXCHILDASPECT)
					scale[j]=parentscale[j]*GPGJOINT_MAXCHILDASPECT;
					}
				}
			}
		}

#if FALSE
	if(!bnode.NameConst().Compare("Bone08"))
		{
		GPGLOG("++++++++++\n");
		GPGLOG("scale %.6G %.6G\n",scale[1],scale[2]);
		GPGLOG("center %.6G %.6G\n",center[1],center[2]);
		GPGLOG("++++++++++\n");
		}
#endif

	scale[1]*=reducescale;
	scale[2]*=reducescale;

	if(tip)
		center[0]=bonelength;
	}


void GPGSkin::DeformMesh(bool renormalize,bool multires)
	{
	if(!m_outmesh || !m_outmesh->GetNumberMeshes())

		{
//		GPGLOG("GPGSkin::DeformMesh() no meshes\n");
		return;
		}

	if(!m_vertexweights.GetNumberElements() &&
								!m_packweightarray.GetNumberElements())
		{
//		GPGLOG("GPGSkin::DeformMesh() no vertexweights or packweights\n");
		return;
		}


#if !GPGDEFORM_VERTEXORIENTED
	long meshid,meshes=m_outmesh->GetNumberMeshes();
	for(meshid=0;meshid<meshes;meshid++)
		{
		m_outmesh->ChooseMeshIndex(meshid);

		m_outmesh->ZeroAllVertices();
		m_outmesh->ZeroAllNormals();
		}
#endif

#if GPGDEFORM_VERTEXORIENTED
	TIME_PROGRESS(m_character,GPGBonesManagerInterface::TimeBoneCache,true);
	long lastsize=m_bonecachearray.GetNumberElements();
	m_bonecachearray.ResizeToAtLeast(m_character->GetBoneTableSize());
	PrepareBoneCacheArray();
	if((U32)lastsize!=m_bonecachearray.GetNumberElements())
		m_bonecachearray.Collect();
	TIME_PROGRESS(m_character,GPGBonesManagerInterface::TimeBoneCache,false);

	if (m_bUseVectorUnit)
		ComputeDeformedVerticesPackedSSE(multires);	// note: on Mac this is really Altivec not SSE!
	else
		ComputeDeformedVerticesPacked(multires);

#else
	// also stores transforms for AccumulateWeightedVertices()
	m_character->ForEachNodeTransformed(GPGPARENTFIRST|GPGSTORE_XFORM,
										&GPGSkin::ComputeStretchNormals);

	m_character->ForEachNodeTransformed(GPGCHILDFIRST,
										&GPGSkin::AccumulateWeightedVertices);

	m_character->ForEachNodeTransformed(GPGPARENTFIRST,
										&GPGSkin::TweakWeightedVertices);
#endif


	TIME_PROGRESS(m_character,GPGBonesManagerInterface::TimeRenormalize,true);

	if(renormalize)
		NormalizeOutputNormals();

	TIME_PROGRESS(m_character,GPGBonesManagerInterface::TimeRenormalize,false);
	}


void GPGSkin::ComputeVertexOffsets(void)
	{
	SortVertexWeightsForCache();

	// store up all bone transforms
	m_character->ResetToReference();
	m_character->ForEachNodeTransformed(GPGPARENTFIRST|GPGSTORE_XFORM,NULL);

	long m,length=m_vertexweights.GetNumberElements();
	for(m=0;m<length;m++)
		{
		GPGVertexWeight &vertexweight=m_vertexweights[m];

		long boneid=vertexweight.GetBoneIndex();
		long meshid=vertexweight.GetMeshIndex();
		long vertexid=vertexweight.GetVertexIndex();

		GPGBoneNode *bonenode=m_character->LookupBoneIndex(boneid);
		if(!bonenode)
			continue;

		m_inmesh->ChooseMeshIndex(meshid);

#if GPGDEFORM_GLOBALORIENTED
		vertexweight.Offset()=m_inmesh->GetVertexConst(vertexid);
		vertexweight.NormalOffset()=m_inmesh->GetNormalConst(vertexid);
#else
		GPGVector3 &offset=vertexweight.Offset();
		GPGVector3 global;
		global=m_inmesh->GetVertexConst(vertexid);
		bonenode->StoredTransform().ReverseTransformVector(global,offset);

#if GPGBONECACHE_NORMALS
		GPGVector3 &normaloffset=vertexweight.NormalOffset();
		global=m_inmesh->GetNormalConst(vertexid);

		bone->StoredTransform().ReverseRotateVector(global,normaloffset);
#endif

#endif
		}
	}


void GPGSkin::ComputePackVertexOffset(U32 meshid,GPGPackVertex *packvertex)
	{
	long vertexid=packvertex->GetVertexIndex();

	m_inmesh->ChooseMeshIndex(meshid);

	F32 *pvert=packvertex->Offset();
	const F32 *mvert=m_inmesh->GetVertexConst(vertexid);

	pvert[0]=mvert[0];
	pvert[1]=mvert[1];
	pvert[2]=mvert[2];

	pvert=packvertex->NormalOffset();
	mvert=m_inmesh->GetNormalConst(vertexid);

	pvert[0]=mvert[0];
	pvert[1]=mvert[1];
	pvert[2]=mvert[2];
	}


void GPGSkin::PrepareBoneCacheArray(void)
	{
	GPGMatrix4x4 result;
	GPGBoneNode *bnode;

	result.Reset();

	long m,length=m_character->GetBoneTableSize();
	for(m=0;m<length;m++)
		{
		if( !(bnode=m_character->LookupBoneIndex(m)) )
			continue;

		result.Multiply3x4(
					bnode->StoredTransform().GetMatrixDataConst(),
					bnode->ReferenceTransform().GetMatrixInverseDataConst());

		// copy 4x4 into 3x4
		m_bonecachearray[m].GetGlobalMatrix()=result;
		}
	}


/******************************************************************************
	assumes m_vertexweight is meshid/vertexid sorted

******************************************************************************/
void GPGSkin::PackVertexWeights(void)
	{
	const long meshes=m_inmesh->GetNumberMeshes();

	m_packweightarray.Clear();
	m_packweightarray.ResizeToAtLeast(meshes);

	long meshid;
	for(meshid=0;meshid<meshes;meshid++)
		{
		m_inmesh->ChooseMeshIndex(meshid);
		const long numverts=m_inmesh->GetMaxNumberVertices();

		const long start=m_vertexweights.GetMeshStart(meshid);
		GPGASSERT((U32)start<m_vertexweights.GetNumberElements());

		const long numweights=m_vertexweights.GetMeshStart(meshid+1)-start;

		GPGPackWeights &packweights=m_packweightarray[meshid];
		packweights.Allocate(numverts,numweights);

		GPGPackVertex *packvert=NULL;
		GPGPackBoneWeight *packweight=NULL;
		long m,n,vertid= -2;
		for(m=0;m<numweights;m++)
			{
			GPGVertexWeight &vertexweight=m_vertexweights[start+m];

			if(vertid!=vertexweight.GetVertexIndex())
				{
				vertid=vertexweight.GetVertexIndex();

				packvert=packweights.NextPackVertexForWrite();
				packvert->SetVertexIndex((U32)vertid);
				packvert->SetNumWeights(1);

				for(n=0;n<3;n++)
					{
					packvert->Offset()[n]=vertexweight.Offset()[n];
					packvert->NormalOffset()[n]=vertexweight.NormalOffset()[n];
					}
//				packvert->Offset()[n]=1.0f;
//				packvert->NormalOffset()[n]=0.0f;
				}
			else
				packvert->IncNumWeights();

			packweight=packweights.NextPackBoneWeightForWrite();
			packweight->SetBoneIndex(vertexweight.GetBoneIndex());
			packweight->SetBoneWeight(vertexweight.GetBoneWeight());
			}
		}
	}


void GPGSkin::UnpackVertexWeights(void)
	{
	const long meshes=m_packweightarray.GetNumberElements();

	m_vertexweights.Clear();

	U32 weightcount=0;
	U32 totalweights=0;
	U32 m,n;

	long meshid;
	for(meshid=0;meshid<meshes;meshid++)
		{
		GPGPackWeights &packweights=m_packweightarray[meshid];

		m_inmesh->ChooseMeshIndex(meshid);
		if(m_inmesh->GetNumberVertices()<m_inmesh->GetMaxNumberVertices())
			{
			GPGLOG("WARNING: %d of %d vertices valid on mesh %d\n",
									m_inmesh->GetNumberVertices(),
									m_inmesh->GetMaxNumberVertices(),meshid);
			}
		const U32 numverts=packweights.GetNumberVerticesConst();
		const U32 numweights=packweights.GetNumberWeightsAllocated();
		if(numverts!=m_inmesh->GetMaxNumberVertices())
			{
			GPGLOG("UnpackVertexWeights() only %d/%d vertices available"
															" for mesh %d\n",
							numverts,m_inmesh->GetMaxNumberVertices(),meshid);
			}

		totalweights+=numweights;
		m_vertexweights.ResizeToAtLeast(totalweights);

		packweights.RewindForRead();
		const GPGPackVertex *packvertex=NULL;
		const GPGPackBoneWeight *packweight=NULL;
		U32 weights;

		for(m=0;m<numverts;m++)
			{
			packvertex=packweights.NextPackVertexForRead();
			weights=packvertex->GetNumWeights();
			GPGASSERT(weights>0);
			for(n=0;n<weights;n++)
				{
				packweight=packweights.NextPackBoneWeightForRead();

				GPGVertexWeight &vertexweight=m_vertexweights[weightcount++];
				vertexweight.SetMeshIndex(meshid);
				vertexweight.SetVertexIndex(packvertex->GetVertexIndex());
				vertexweight.SetBoneIndex(packweight->GetBoneIndex());
				vertexweight.SetBoneWeight(packweight->GetBoneWeight());
				vertexweight.Offset().Set(	packvertex->OffsetConst()[0],
											packvertex->OffsetConst()[1],
											packvertex->OffsetConst()[2]);
				vertexweight.NormalOffset().Set(
											packvertex->NormalOffsetConst()[0],
											packvertex->NormalOffsetConst()[1],
											packvertex->NormalOffsetConst()[2]);
				}
			}
		}
	}


/******************************************************************************
	NOTE					`speed critical'

	WARNING If there is any scaling applied to the matrices,
			you must activate the compile-time flag GPGM34_UNSCALE
			in GPGMatrix3x4.h.
******************************************************************************/
void GPGSkin::ComputeDeformedVerticesPacked(bool multires)
	{
	TIME_PROGRESS(m_character,GPGBonesManagerInterface::TimeDeformation,true);

	// 38 is number of DSTs dispatched
//	XPPCStartTiming( 38, kPMC2_ProcessorCycles )

	F32 accumulator[4];
	F32 normaccum[4];
	GPGVertexIndexer *indexer=m_outmesh->GetVertexIndexer();

	GPGASSERT(m_inmesh->GetNumberMeshes()==m_outmesh->GetNumberMeshes());

	const GPGBoneCacheArray &cbonecachearray=m_bonecachearray;

	U32 m,n,meshid;
	const U32 meshes=(U32)m_outmesh->GetNumberMeshes();
	for(meshid=0;meshid<meshes;meshid++)
		{
		m_outmesh->ChooseMeshIndex(meshid);

		const long numverts= (multires)?
			m_outmesh->GetNumberVertices(): m_outmesh->GetMaxNumberVertices();

		GPGPackWeights &packweights=m_packweightarray[meshid];
		packweights.RewindForRead();
		const U32 length=packweights.GetNumberVerticesConst();
		const GPGPackVertex *packvertex=NULL;
		const GPGPackBoneWeight *packweight=NULL;
		U32 weights;
		U32 length1=length-1;

		if(length)
			packvertex=packweights.NextPackVertexForRead();
		for(m=0;m<length;m++)
			{
			packweight=packweights.NextPackBoneWeightForRead();
			weights=packvertex->GetNumWeights();
			GPGASSERT(weights>0);

			cbonecachearray.GetElementConst(packweight->GetBoneIndex())
						.GetGlobalMatrixConst()
						.TransformVertexAndNormal(
								packweight->GetBoneWeight(),
								packvertex->OffsetConst(),accumulator,
								packvertex->NormalOffsetConst(),normaccum);
			GPGASSERT(((GPGVector3)normaccum).CalcMagnitude()<=1.1f);
			for(n=1;n<weights;n++)
				{
				packweight=packweights.NextPackBoneWeightForRead();

				cbonecachearray.GetElementConst(packweight->GetBoneIndex())
						.GetGlobalMatrixConst()
						.CumulativeTransformVertexAndNormal(
								packweight->GetBoneWeight(),
								packvertex->OffsetConst(),accumulator,
								packvertex->NormalOffsetConst(),normaccum);
				GPGASSERT(((GPGVector3)normaccum).CalcMagnitude()<=1.1f);
				}

			indexer->SetVertexAndNormal(packvertex->GetVertexIndex(),
														accumulator,normaccum);

			if(m<length1)
				{
				packvertex=packweights.NextPackVertexForRead();
				if(packvertex->GetVertexIndex()>=numverts)
					{
					//* move on to next mesh
					m=length;	// faster than break
					}
				}
			}
		}

//	XPPCStopAndReportTimingAndAverage()
	TIME_PROGRESS(m_character,GPGBonesManagerInterface::TimeDeformation,false);
	}


/******************************************************************************
	WARNING speed critical
******************************************************************************/
void GPGSkin::ComputeDeformedVertices(bool multires)
	{
	GPGASSERT(FALSE);
#if FALSE
	TIME_PROGRESS(m_character,GPGBonesManagerInterface::TimeDeformation,true);

	F32 accumulator[3];
	F32 normaccum[3];
	GPGVertexIndexer *indexer=m_outmesh->GetVertexIndexer();

	GPGASSERT(m_inmesh->GetNumberMeshes()==m_outmesh->GetNumberMeshes());

	const GPGVertexWeights &cvertexweights=m_vertexweights;
	const GPGBoneCacheArray &cbonecachearray=m_bonecachearray;

	long meshid;
	const long meshes=m_inmesh->GetNumberMeshes();
	for(meshid=0;meshid<meshes;meshid++)
		{
		accumulator[0]=0.0f;
		accumulator[1]=0.0f;
		accumulator[2]=0.0f;
		normaccum[0]=0.0f;
		normaccum[1]=0.0f;
		normaccum[2]=0.0f;

		m_inmesh->ChooseMeshIndex(meshid);
		m_outmesh->ChooseMeshIndex(meshid);

		GPGASSERT(cvertexweights.GetMeshStart(meshid+1)
									<= cvertexweights.GetNumberElements());

		const long numverts= (multires)?
				m_inmesh->GetNumberVertices(): m_inmesh->GetMaxNumberVertices();
		long m=cvertexweights.GetMeshStart(meshid);
		if(m>=cvertexweights.GetNumberElements())
			break;
		const long length=cvertexweights.GetMeshStart(meshid+1);
		long vertid=cvertexweights[m].GetVertexIndex();
		for(;m<length;m++)
			{
			const GPGVertexWeight &vertexweight=
										cvertexweights.GetElementConst(m);

			GPGASSERT(vertexweight.GetBoneIndex()>=0);
			GPGASSERT(vertexweight.GetBoneIndex()
											< m_character->GetBoneTableSize());

			if(vertid==vertexweight.GetVertexIndex())
				{
				//* accumulating like vertex

				cbonecachearray.GetElementConst(
											vertexweight.GetBoneIndex())
						.GetGlobalMatrixConst()
						.CumulativeTransformVertexAndNormal(
								vertexweight.GetBoneWeight(),
								vertexweight.OffsetConst(),accumulator,
								vertexweight.NormalOffsetConst(),normaccum);
				GPGASSERT(((GPGVector3)normaccum).CalcMagnitude()<=1.1f);
				}
			else
				{
				//* found next vertex

				// previous vertex
				indexer->SetVertexAndNormal(vertid,accumulator,normaccum);

				// next vertex
				vertid=vertexweight.GetVertexIndex();
				if((vertid=vertexweight.GetVertexIndex())>numverts)
					{
					// skip ahead to next mesh (using sorted weights)
					m=length;	// faster than break
					}

				cbonecachearray.GetElementConst(
											vertexweight.GetBoneIndex())
						.GetGlobalMatrixConst()
						.TransformVertexAndNormal(
								vertexweight.GetBoneWeight(),
								vertexweight.OffsetConst(),accumulator,
								vertexweight.NormalOffsetConst(),normaccum);
				GPGASSERT(((GPGVector3)normaccum).CalcMagnitude()<=1.1f);

				}
			}

		if(vertid<numverts)
			indexer->SetVertexAndNormal(vertid,accumulator,normaccum);
		}

	TIME_PROGRESS(m_character,GPGBonesManagerInterface::TimeDeformation,false);
#endif
	}



/******************************************************************************
void GPGSkin::NormalizeOutputNormals(void)

******************************************************************************/
void GPGSkin::NormalizeOutputNormals(void)
	{
	GPGASSERT(m_outmesh);

	float mag2,scale;

	GPGVertexIndexer *indexer=m_outmesh->GetVertexIndexer();
	long meshid,meshes=m_outmesh->GetNumberMeshes();
	for(meshid=0;meshid<meshes;meshid++)
		{
		m_outmesh->ChooseMeshIndex(meshid);

		long index,normals=m_outmesh->GetNumberNormals();
		for(index=0;index<normals;index++)
			{
#if GPGSKIN_FASTNORMALIZE
			indexer->MoveToIndex(index);
			GPGVector3 *normal=indexer->GetNormal();

			mag2=(*normal)[0]*(*normal)[0]+(*normal)[1]*(*normal)[1]+
													(*normal)[2]*(*normal)[2];

#if GPGSKIN_USE_SQRT_TABLE
			GPGASSERT((long)(mag2*(GPGSKIN_SQRT_SCALESIZE-1))
													< GPGSKIN_SQRT_TABLESIZE);
			scale=m_invsqrt[(long)(mag2*(GPGSKIN_SQRT_SCALESIZE-1))];
#else
			scale=1.0f/(float)GPGSQRT(mag2);
#endif
			normal->Scale(scale);
#else
			// copy
			GPGVector3 vector=m_outmesh->GetNormalConst(index);

			vector.Normalize();		//* sqrt()!

			// copy
			m_outmesh->SetNormal(index,vector.Raw());
#endif
			}
		}
	}
