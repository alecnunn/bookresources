
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

#ifndef __GPGCharacter_h__
#define __GPGCharacter_h__



#define	GPGALLOW_TIMEPROGRESS	TRUE

#if GPGALLOW_TIMEPROGRESS
	#define	TIME_PROGRESS(x,y,z)	GPGCharacter::TimeProgress(x,y,z)
#else
	inline void GPGNoTimeProgress(GPGCharacter *character,U32 index,bool on) {}

	#define TIME_PROGRESS		1 ? (void)0 : ::GPGNoTimeProgress
#endif



#define GPGPARENTFIRST		0
#define GPGCHILDFIRST		1
#define GPGSTORE_XFORM		2



typedef void (*GPGDrawLineCB)(long color,const GPGVector3 &first,
													const GPGVector3 &second);
typedef void (*GPGTimeProgressCB)(GPGCharacter *character,U32 index,bool on);

typedef BOOL (*GPGForEachNodeCB)(GPGCoreNode &,GPGVoidStar state);
typedef BOOL (*GPGForEachNodeTransformedCB)(GPGCoreNode &,
											GPGTransform &,GPGVoidStar state);



/******************************************************************************
class GPGCharacter: public GPGCoreNode

	for children, probably will have just one root bone

******************************************************************************/
class GPGCharacter: public GPGCoreNode
	{
	public:
/*		enum				TimedItem {
								Frame, AllAnim, FaceCopy, Motion, IK,
								LinkRecalc, BoneCache, Deformation,
								Renormalize, NumTimedItems };
*/
							GPGCharacter(GPGCharacter *pCloneOf=NULL);
virtual						~GPGCharacter(void);

		void				Reset(void);
		void				GrabSkin(void);

		GPGSkin				*GetSkin(void)					{ return m_skin; };

		GPGCoreNode			*FindDescendentByName(GPGString name)
								{
								name.ForceUppercase();
								return FindDescendentByName2(*this,name);
								};

		void				ClearBoneTable(void)
								{
								m_bonetable.Clear();
								}
		void				RemoveBoneLinks(void);
		void				RebuildBoneTable(void);
		long				GetBoneTableSize(void)
								{
								return m_bonetable.GetNumberElements();
								}
		void				SetBoneAtIndex(long index,GPGBoneNode *set);
		GPGBoneNode			*LookupBoneIndex(long index)
								{
								if(index>=(long)m_bonetable.GetNumberElements() 
									|| index<0)
								{
									return NULL;
								}
								return m_bonetable[index];
								};

		GPGTransformStack	&TransformStack(void)	{ return m_transforms;};

	// not inline:

		void				CalcNodeRotationsFromStoredTranslations(
																bool lookback);

		void				ApplyIK(void);

		void				ResetToReference(void);
		void				ResetToFullReach(void);

		void				ForEachNode(long flags,GPGForEachNodeCB callback,
													GPGVoidStar state=NULL);
		void				ForEachNodeDual(GPGForEachNodeCB preCallback,
											GPGForEachNodeCB postCallback,
													GPGVoidStar state=NULL);
		void				ForEachNodeTransformed(long flags,
									GPGForEachNodeTransformedCB callback=NULL,
													GPGVoidStar state=NULL);

		long				CountRealBones(void);
		F32					CalcReachingLimit(void);
		void				StoreReferencePositions(void);
		void				GenerateBoneLinks(long defaultnumber);
		void				RecalcLinks(void);
		void				CalculateImplantReferences(void);
		void				BlendBones(F32 weight);

		void				RemoveDescendents(GPGCoreNode &parent);

#if GPGALLOW_STREAMS
		std::ostream		&Print(std::ostream &out,long indention) const;
#endif

		GPGString			Out(bool verbose=false) const;

static	void				DrawLine(long color,const GPGVector3 &first,
													const GPGVector3 &second)
								{
								if(m_drawlineCB)
									m_drawlineCB(color,first,second);
								};
static	void				StateProgress(const char *text)
								{
								if(m_progressCB)
									m_progressCB(text);
								};
static	void				TimeProgress(GPGCharacter *character,
														U32 index,bool on)
								{
								if(m_timeProgressCB)
									m_timeProgressCB(character,index,on);
								};
		void				SetDrawLineCB(GPGDrawLineCB set)
								{ m_drawlineCB=set; };
		void				SetProgressCB(GPGProgressCB set)
								{ m_progressCB=set; };
		void				SetTimeProgressCB(GPGTimeProgressCB set)
								{ m_timeProgressCB=set; };

		GPGVertexWeights	*GetVertexWeights(void)	{ return m_vertexweights; };
		void				SetVertexWeights(GPGVertexWeights *set)
													{ m_vertexweights=set; };

		GPGIKModes			&GetIKModes(void)		{ return m_ikmodes; };

		long				GetTestFlag(void)		{ return m_testflag; };
		void				SetTestFlag(long set)	{ m_testflag=set; };

//* internal:
		void				ForEachNode2(long flags,GPGCoreNode &parent,
										GPGForEachNodeCB callback,
										GPGVoidStar state);
		void				ForEachNodeDual2(GPGCoreNode &parent,
										GPGForEachNodeCB preCallback,
										GPGForEachNodeCB postCallback,
										GPGVoidStar state);
		void				ForEachNodeTransformed2(long flags,
										GPGCoreNode &parent,
										GPGForEachNodeTransformedCB callback,
										GPGVoidStar state);

	private:
		void				CopyBoneHierarchy(GPGCoreNode *pParent,
													GPGCoreNode *pNewParent);
		void				CalcNodeRotations(bool lookback,
														GPGCoreNode &parent);

		GPGCoreNode			*FindDescendentByName2(GPGCoreNode &parent,
															GPGString name);

static	BOOL				CreateLinksOnNode(GPGCoreNode &node,
															GPGVoidStar state);
static	BOOL				RecalcLinksOnNode(GPGCoreNode &node,
															GPGVoidStar state);
static	BOOL				CalcLinkReferencesOnNode(GPGCoreNode &node,
															GPGVoidStar state);
static	BOOL				RemoveBoneLinkNode(GPGCoreNode &node,
															GPGVoidStar state);
static	BOOL				AddBoneToTable(GPGCoreNode &node,GPGVoidStar state);
static	BOOL				BlendBoneNode(GPGCoreNode &node,GPGVoidStar state);


static	BOOL				ResetToReferenceOnNode(GPGCoreNode &node,
															GPGVoidStar state);
static	BOOL				ResetToFullReachOnNode(GPGCoreNode &node,
															GPGVoidStar state);
static	BOOL				CountRealBone(GPGCoreNode &node,GPGVoidStar state);
static	BOOL				StoreReferenceForBone(GPGCoreNode &node,
									GPGTransform &transform,GPGVoidStar state);
static	BOOL				KineBone(GPGCoreNode &node,GPGTransform &transform,
															GPGVoidStar state);
static	BOOL				StoreIKData(GPGCoreNode &node,GPGVoidStar state);
static	BOOL				CalculateImplantReferenceForBone(GPGCoreNode &node,
									GPGTransform &transform,GPGVoidStar state);

#if GPGALLOW_STREAMS
		void				PrintDescendents(const GPGCoreNode &parent,
									std::ostream &out,long indention) const;
#endif

		GPGBoneContext			*m_pBoneContext;
		GPGSkin					*m_skin;

		GPGTransformStack		m_transforms;
		GPGVertexWeights		*m_vertexweights;	// pointer into skin
		GPGArray<GPGBoneNode *>	m_bonetable;
		F32						m_timer;
		GPGIKModes				m_ikmodes;
static	GPGDrawLineCB			m_drawlineCB;
static	GPGProgressCB			m_progressCB;
static	GPGTimeProgressCB		m_timeProgressCB;
		long					m_testflag;
	};



#if GPGALLOW_STREAMS
std::ostream &operator<<(std::ostream &out,const GPGCharacter &character);
#endif

#endif /* __GPGCharacter_h__ */
