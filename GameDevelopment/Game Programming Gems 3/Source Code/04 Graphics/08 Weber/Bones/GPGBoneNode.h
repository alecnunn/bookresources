
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

#ifndef __GPGBoneNode_h__
#define __GPGBoneNode_h__



/******************************************************************************
class GPGBoneNode: public GPGCoreNode

******************************************************************************/
class GPGBoneNode: public GPGCoreNode
	{
	class GPGBoneNodeShare
		{
		public:
							GPGBoneNodeShare(void)	{ Reset(); };
			void			Reset(void);
			// rotation, length, constraints are relative to end of parent

			I32				m_boneid;
			I32				m_linkboneid;
			F32				m_length;
			GPGConstraints	m_positional;
			GPGConstraints	m_rotational;

			F32				m_implant_len;
			GPGQuaternion	m_implant_rot;
			F32				m_implant_lenref;
			GPGQuaternion	m_implant_rotref;

			GPGMeshVertex	m_bestvert;	// vert with best generate score
			F32				m_bestscore;

			BOOL			m_influential;

			BOOL			m_autojoint;
			GPGCylinder		m_bounds;

			BOOL			m_autolink;
			GPGBoneLinks	m_bonelinks;
			BOOL			m_isbonelink;
			U32				m_count;
		};

	public:
							GPGBoneNode(void)
								{
								// only here for GPGList<GPGBoneNode>
								GPGLOG("don't use "
											"GPGBoneNode::GPGBoneNode(void)");

								GPGBoneNode(NULL);
								};
							GPGBoneNode(GPGCharacter *set,
										GPGBoneNode *pCloneOf=NULL):
									GPGCoreNode(pCloneOf)
								{
								ResetBone();
								if(pCloneOf)
									{
									m_share=pCloneOf->m_share;
									m_share->m_count++;
									}
								else
									{
									m_share=new GPGBoneNodeShare();
									m_share->m_count=1;
									}
								SetRootCharacter(set);
								};
virtual						~GPGBoneNode(void)
								{
								if(!(--m_share->m_count))
									delete m_share;
								};

		void				Reset(void)
								{
								GPGCoreNode::Reset();
								ResetBone();
								};
		void				ResetBone(void);

		void				SetBoneIndex(I32 set)
										{ m_share->m_boneid=set; };
		I32					GetBoneIndex(void) const
										{ return m_share->m_boneid; };

		void				SetLength(F32 set)
										{ m_share->m_length=set; };
		F32					GetLength(void) const
										{ return m_share->m_length; };

		void				SetInfluential(BOOL set)
										{ m_share->m_influential=set; };
		BOOL				IsInfluential(void) const
										{ return m_share->m_influential; };

		void				SetEffected(BOOL set)	{ m_effected=set; };
		BOOL				IsEffected(void) const	{ return m_effected; };


		GPGConstraints		&PositionalConstraints(void)
										{ return m_share->m_positional; };
const	GPGConstraints		&PositionalConstraintsConst(void) const
										{ return m_share->m_positional; };
		GPGConstraints		&RotationalConstraints(void)
										{ return m_share->m_rotational; };
const	GPGConstraints		&RotationalConstraintsConst(void) const
										{ return m_share->m_rotational; };

		void				SetAutoJoint(BOOL set)
										{ m_share->m_autojoint=set; };
		BOOL				IsAutoJoint(void) const
										{ return m_share->m_autojoint; };

		void				SetAutoLink(BOOL set)
										{ m_share->m_autolink=set; };
		BOOL				IsAutoLink(void) const
										{ return m_share->m_autolink; };

//*	internal:

		I32					&BoneIndex(void)
										{ return m_share->m_boneid; };
		F32					&Length(void)		{ return m_share->m_length; };
		BOOL				&Influential(void)
										{ return m_share->m_influential; };

		BOOL				&Effected(void)			{ return m_effected; };
		GPGVector3			&Effector(void)			{ return m_effector; };
const	GPGVector3			&EffectorConst(void) const
										{ return m_effector; };

		BOOL				&AutoJoint(void)
										{ return m_share->m_autojoint; };
		BOOL				&AutoLink(void)
										{ return m_share->m_autolink; };

		GPGBoneLinks		&BoneLinks(void)
										{ return m_share->m_bonelinks; };
		I32					&LinkBoneIndex(void)
										{ return m_share->m_linkboneid; };

		BOOL				IsBoneLink(void)
										{ return m_share->m_isbonelink; };
		void				SetBoneLink(BOOL set)
										{ m_share->m_isbonelink=set; };

		GPGCylinder			&CylindricalBounds(void)
										{ return m_share->m_bounds; };

		BOOL				&DebugPicked(void)		{ return m_picked; };

		void				CalcImplantReference(void);
		GPGQuaternion		&ImplantReferenceRotation(void)
										{ return m_share->m_implant_rotref;};
		F32					&ImplantReferenceLength(void)
										{ return m_share->m_implant_lenref;};
		void				CalcImplant(void);
		GPGQuaternion		&ImplantRotation(void)
										{ return m_share->m_implant_rot; };
		F32					&ImplantLength(void)
										{ return m_share->m_implant_len; };

		void				CreateEffections(GPGCoreNode *base);

		BOOL				HasAncestor(GPGCoreNode *node);

		void				CreateBoneLinks(long defaultnumber);
		void				CalcBoneLinks(void);
		void				CalcBoneLinkReferences(void);

		GPGMeshVertex		&BestVertex(void)
										{ return m_share->m_bestvert; };
		F32					&BestVertexScore(void)
										{ return m_share->m_bestscore; };

#if GPGALLOW_STREAMS
		std::ostream		&Print(std::ostream &out,long indention) const;
#endif

			GPGString		Out(BOOL verbose=false) const;

	private:

		F32					CalcLinkLength(void);

		GPGBoneNodeShare	*m_share;

		//* NOTE non-sharable data

		//* effector is the desired position of this node (for IK)
		BOOL				m_effected;
		GPGVector3			m_effector;
		BOOL				m_picked;
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGBoneNode &character);
#endif

#endif /* __GPGBoneNode_h__ */

