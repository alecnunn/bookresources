
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

#ifndef __GPGCoreNode_h__
#define __GPGCoreNode_h__



/******************************************************************************
class GPGCoreNode

	Defined here: Effections are an association to an effector through
	an GPGBoneNode

******************************************************************************/
class GPGCoreNode
	{
	class GPGCoreNodeShare
		{
		public:
							GPGCoreNodeShare(void)	{ Reset(); };
			void			Reset(void)
								{
								m_name="none";
								m_reference_displacement.Reset();
								m_local_reference_rotation.MakeIdentity();
								m_referenceTransform.Reset();
								m_is_bone=false;
								};

			bool			m_is_bone;
			GPGVector3		m_reference_displacement;
			GPGQuaternion	m_local_reference_rotation;
			GPGTransform	m_referenceTransform;
			GPGString		m_name;
			U32				m_count;
		};

	public:
							GPGCoreNode(GPGCoreNode *pCloneOf=NULL);
virtual						~GPGCoreNode(void);
		void				Reset(void);

		bool				IsBone(void) const	{ return m_share->m_is_bone; };

		GPGString			Name(void)				{ return m_share->m_name; };
const	GPGString			NameConst(void) const	{ return m_share->m_name; };
		void				SetName(GPGString set)	{ m_share->m_name=set; };

		void				AppendChild(GPGBoneNode *bonenode);
		void				RemoveChild(GPGBoneNode *bonenode);

		GPGVector3			&Displacement(void)		{ return m_displacement; };
const	GPGVector3			&DisplacementConst(void) const
													{ return m_displacement; };

		GPGQuaternion		&Rotation(void)			{ return m_rotation; };
const	GPGQuaternion		&RotationConst(void) const
													{ return m_rotation; };

		GPGVector3			&Scale(void)			{ return m_scale; };
const	GPGVector3			&ScaleConst(void) const { return m_scale; };

//*	internal:

		GPGCoreNode			*Parent(void)			{ return m_parent; };
const	GPGCoreNode			*ParentConst(void) const
													{ return m_parent; };
		void				SetParent(GPGCoreNode *set)
													{ m_parent=set; };

		GPGBoneNodeList		&Children(void)			{ return *m_pChildren; };
const	GPGBoneNodeList		&ChildrenConst(void) const
													{ return *m_pChildren; };

		GPGBoneNodeList		&Effections(void)		{ return *m_pEffections; };
const	GPGBoneNodeList		&EffectionsConst(void) const
													{ return *m_pEffections; };

		GPGVector3			&ReferenceDisplacement(void)
								{ return m_share->m_reference_displacement; };
		GPGQuaternion		&LocalReferenceRotation(void)
								{ return m_share->m_local_reference_rotation; };

const	GPGQuaternion		&GlobalReferenceRotationConst(void)
									{ return m_share->m_referenceTransform
													.QuaternionConst(); };

		GPGVector3			&BlendDisplacement(void)
											{ return m_blend_displacement; };
const	GPGVector3			&BlendDisplacementConst(void) const
											{ return m_blend_displacement; };

		GPGQuaternion		&BlendRotation(void)	{ return m_blend_rotation;};
const	GPGQuaternion		&BlendRotationConst(void) const
													{ return m_blend_rotation;};

		GPGVector3			&BlendScale(void)		{ return m_blend_scale; };
const	GPGVector3			&BlendScaleConst(void) const
													{ return m_blend_scale; };

		void				SetRootCharacter(GPGCharacter *set)
													{ m_character=set; };
		GPGCharacter		*RootCharacter(void)	{ return m_character; };


		void				StoreReferenceTransform(GPGTransform &transform)
									{
									transform.UpdateMatrix();
									m_share->m_referenceTransform=transform;
									};
		GPGTransform		&ReferenceTransform(void)
									{ return m_share->m_referenceTransform;};

		void				StoreTransform(GPGTransform &transform)
									{
									transform.UpdateMatrix();
									m_storedTransform=transform;
									};
		GPGTransform		&StoredTransform(void)
									{ return m_storedTransform; };

#if GPGALLOW_STREAMS
		std::ostream		&Print(std::ostream &out,long indention) const;
#endif

		GPGString			Out(bool verbose=false) const;

	protected:
		void				SetIsBone(bool set)	{ m_share->m_is_bone=set; };

	private:

		GPGCoreNodeShare	*m_share;

		//* WARNING not sharable
		GPGCharacter		*m_character;
		GPGCoreNode			*m_parent;
		GPGBoneNodeList		*m_pChildren;
		GPGBoneNodeList		*m_pEffections;
		GPGTransform		m_storedTransform;
		GPGVector3			m_displacement;
		GPGQuaternion		m_rotation;
		GPGVector3			m_scale;
		GPGVector3			m_blend_displacement;
		GPGQuaternion		m_blend_rotation;
		GPGVector3			m_blend_scale;
	};

#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGCoreNode &character);
#endif

#endif /* __GPGCoreNode_h__ */
