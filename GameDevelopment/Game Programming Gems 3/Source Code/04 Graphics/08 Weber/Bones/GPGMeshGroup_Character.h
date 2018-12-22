
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

#ifndef __GPGMeshGroup_Character_h__
#define __GPGMeshGroup_Character_h__



/******************************************************************************
class GPGMeshGroup_Character: public GPGCharacter

	presumes a single mesh GEO file, multi-mesh may misbehave

	The mulitple materials of the single mesh geo are converted into
	multiple meshes of the GPGMeshGroup.
	The weights loaded in the bone file are mapped into these new vertices.

******************************************************************************/
class GPGMeshGroup_Character: public GPGCharacter
	{
	class GPGMG_CharShare
		{
		public:
								GPGMG_CharShare(void)	{ Reset(); }
			void				Reset(void)
									{
									m_lastinput=NULL;
									m_in_allocated=false;
									m_out_allocated=false;
									m_inmeshgroup=NULL;
									m_outmeshgroup=NULL;
									}

			U32					m_count;
			GPGMeshGroup		*m_lastinput;

			bool				m_in_allocated;
			GPGMeshGroup		*m_inmeshgroup;
			GPGMeshGroup_Impl	m_impl_in;

			bool				m_out_allocated;
			GPGMeshGroup		*m_outmeshgroup;
			GPGMeshGroup_Impl	m_impl_out;
		};

	public:
				GPGMeshGroup_Character(GPGMeshGroup_Character *pCloneOf=NULL):
						GPGCharacter(pCloneOf)
					{
					Initialize();
					if(pCloneOf)
						{
						m_share=pCloneOf->m_share;
						m_share->m_count++;
						}
					else
						{
						m_share=new GPGMG_CharShare();
						m_share->m_count=1;
						}
					}

		void	Initialize(void)
					{
					m_vertexmapgroup=NULL;
					m_meshfactory=NULL;
					m_manager=NULL;
					}

virtual			~GPGMeshGroup_Character(void)
					{
					if(!(--m_share->m_count))
						{
						FreeInMesh();
						FreeOutMesh();
						delete m_share;
						}
					}

		GPGRESULT	AdoptMeshGroup(GPGMeshGroup *mg,GPGVertexMapGroup *vmg);

		bool		LoadASHMesh(const char *meshfilename);
		bool		LoadASHBone(const char *bonefilename,bool justweights);
		bool		SaveASHBone(const char *bonefilename,bool binary);
		GPGRESULT	CleanupWeights(bool regenerate,bool removerogueweights,
					bool smooth,float threshhold,float weldmax,float modelsize);

		void		SetBonesManager(GPGBonesManagerInterface *set)
													{ m_manager=set;}
		GPGBonesManagerInterface *GetBonesManager(void)
													{ return m_manager; }

		void		SetMeshFactory(GPGMeshFactory *set)
													{ m_meshfactory=set; }

		GPGMeshGroup	*GetLastInput(void)	{ return m_share->m_lastinput; }
		void			SetLastInput(GPGMeshGroup *set)
											{ m_share->m_lastinput=set; }

		GPGMeshGroup	*GetInputMeshGroup(void)
											{ return m_share->m_inmeshgroup; }
		GPGMeshGroup	*GetOutputMeshGroup(void)
											{ return m_share->m_outmeshgroup; }
		GPGMeshInterface	*GetInputMeshInterface(void)
											{ return &(m_share->m_impl_in); }
		GPGMeshInterface	*GetOutputMeshInterface(void)
											{ return &(m_share->m_impl_out); }

		void		GetBackmapIndex(U32 mesh,U32 vertex,
												U32 *mesh0,U32 *vertex0)
						{
						GPGASSERT(mesh<m_backmap.GetNumberElements());
						GPGASSERT(vertex<m_backmap[mesh].GetNumberElements());
						m_backmap[mesh][vertex].GetMeshVertex(mesh0,vertex0);
						}

					// uses m_backmap and vertexmap to find 0th replication
virtual	GPGRESULT	GetMasterMeshIndex(U32 mesh,U32 vertex,
												U32 *mesh0,U32 *vertex0);

	private:

		void		SetAutoInfluence(void);
		void		AttachMeshes(void);
		void		AllocInMesh(void);
		void		FreeInMesh(void);
		void		AllocOutMesh(void);
		void		FreeOutMesh(void);
		GPGRESULT	AdoptVertexMap(void);

		void		FindOverlap(float weldmax);
		void		PrintGPGMeshGroup(GPGMeshGroup_Impl *img);

		GPGMG_CharShare					*m_share;
		GPGMeshFactory					*m_meshfactory;
		GPGVertexMapGroup				*m_vertexmapgroup;
		GPGArray<GPGMeshVertexArray>	m_backmap;

		GPGBonesManagerInterface		*m_manager;
	};

#endif /* __GPGMeshGroup_Character_h__ */
