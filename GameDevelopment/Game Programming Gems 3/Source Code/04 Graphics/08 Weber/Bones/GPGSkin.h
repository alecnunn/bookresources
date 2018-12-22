
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

#ifndef __GPGSkin_h__
#define __GPGSkin_h__

#define GPG_GLOBALS	FALSE	// HACK


#ifndef	GPG_GLOBALS
#define GPG_GLOBALS	TRUE
#endif

#ifdef S3D_MAX_PLUGIN
#undef GPG_GLOBALS
#define GPG_GLOBALS FALSE
#endif

#if GPG_GLOBALS
#include "globals.h"
#endif


class GPGLongList: public GPGList<long>
	{
	public:
			GPGLongList(void)	{ SetAutoDestruct(true); };
	};
class GPGLongArray: public GPGArray<long> {};
class GPGLongListArray: public GPGArray<GPGLongList> {};

/******************************************************************************
class GPGSkin

	weights map into the mesh by index

	switching meshes without reseting weights is probably a bad idea

	input mesh holds original data with potential changing face structure
	output mesh holds deformed vertices and altered normals
	these may be the same mesh

	NOTE both meshes must be of the same implementation

******************************************************************************/
class GPGSkin
	{
	public:
						GPGSkin(void)
							{
							m_character=NULL;
							m_vertexordered=true;
							m_inmesh=NULL;
							m_outmesh=NULL;

							m_invsqrt=NULL;
							CreateInvSqrtTable();

#if GPG_GLOBALS // Don't do this when the exporter builds against it.
xxx
							m_bUseVectorUnit = HardwareHasVectorUnit();
#else
							m_bUseVectorUnit = false;		// no sse on the Exporter
#endif
							};

virtual					~GPGSkin(void)
							{
							if(m_invsqrt)
								delete[] m_invsqrt;
							};

//	external:
		bool				GetVertexOrdered(void)	{ return m_vertexordered; };
		void				SetVertexOrdered(bool set)
													{ m_vertexordered=set; };

							//* Meshes
		GPGMeshInterface	*GetInputMesh(void)			{ return m_inmesh; };
const	GPGMeshInterface	*GetInputMeshConst(void) const
														{ return m_inmesh; };
		void				SetInputMesh(GPGMeshInterface *set)
														{ m_inmesh=set; };

		GPGMeshInterface	*GetOutputMesh(void)		{ return m_outmesh;};
const	GPGMeshInterface	*GetOutputMeshConst(void) const
														{ return m_outmesh;};
		void				SetOutputMesh(GPGMeshInterface *set)
														{ m_outmesh=set; };

		void				RegenerateWeights(bool use_joints,bool readonly);
		void				FilterWeights(void);
		void				SmoothWeights(long iterations,float threshhold,
												float weldmax,float modelsize);
		void				RelinkWeights(void);
//		void				CacheInfluences(void);
		void				CalculateJointCrossSections(bool updatecache,
															bool computetips);
		void				DeformMesh(bool renormalize,bool multires);

		void				CalcBounds(GPGVector3 *min,GPGVector3 *max);

//	internal:
		void				SetCharacter(GPGCharacter *set){ m_character=set; };
		void				ComputeVertexOffsets(void);
		void				ComputePackVertexOffset(U32 meshid,
													GPGPackVertex *packvertex);

		void				RemoveRogueWeights(void);


		GPGVertexWeights	&GetVertexWeights(void)
								{ return m_vertexweights; };

		GPGArray<GPGMeshVertexMap>		&GetReplicants(void)
											{ return m_replicants; };
		GPGArray<GPGMeshVertexArray>	&GetMasterVertices(void)
											{ return m_mastervertices; };
		bool				GetMasterMeshVertex(U32 meshid,U32 vertex,
												U32 *mmeshid,U32 *mvertex);

		void				ClearWeights(void)
								{
								m_vertexweights.Clear();
								m_replicants.Clear();
								};

		void				CalculateInfluences(float limit);

		void						PackVertexWeights(void);
		void						UnpackVertexWeights(void);
		GPGArray<GPGPackWeights>	&GetPackVertexWeights(void)
										{ return m_packweightarray; };

#if GPGALLOW_STREAMS
		std::ostream		&Print(std::ostream &out,long indention) const;
#endif

		GPGString			Out(bool verbose=false) const;

	private:
		bool				CalcLocalOffset(long boneid,
								const GPGVector3 &world,GPGVector3 *offset);
		void				SortVertexWeightsForCache(void);
		void				ComputeDeformedVertices(bool multires);

		void				ComputeDeformedVerticesPacked(bool multires);
		void				ComputeDeformedVerticesPackedSSE(bool multires);

		void				FindNeighbors(GPGMeshInterface *mesh,
										GPGArray<GPGMeshVertexMap> &neighbors);
		void				MapWeights(GPGMeshInterface *mesh,
										GPGArray<GPGLongListArray> &map);

		void				FloodFill(
								const GPGArray<GPGMeshVertexMap> &neighbors,
								const GPGArray<GPGLongListArray> &map,
								bool scanfirst,long weightid,long newboneid);

		void				CreateInvSqrtTable(void);
		void				NormalizeOutputNormals(void);

		void				PrepareBoneCacheArray(void);
static	BOOL				CalculateJointsForBone(GPGCoreNode &node,
									GPGTransform &transform,GPGVoidStar state);
static	void				CalculateJointForBone(GPGCoreNode &node,
									GPGTransform &transform,
									bool tip,bool computetips);

		GPGArray<GPGPackWeights>		m_packweightarray;
		GPGVertexWeights				m_vertexweights;
		GPGBoneCacheArray				m_bonecachearray;

										// replicated or colocated verts
		GPGArray<GPGMeshVertexMap>		m_replicants;
										// chosen master replicant/colocatee
		GPGArray<GPGMeshVertexArray>	m_mastervertices;

		GPGCharacter					*m_character;
		GPGMeshInterface				*m_inmesh,*m_outmesh;
		float							*m_invsqrt;
		bool							m_vertexordered;
		bool							m_bUseVectorUnit;
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGSkin &skin);
#endif

#endif /* __GPGSkin_h__ */
