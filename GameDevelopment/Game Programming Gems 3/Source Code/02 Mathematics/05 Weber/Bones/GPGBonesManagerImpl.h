
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



#define	GPG_BONECIRCLE_SIDES	64


/******************************************************************************
class GPGBonesManagerImpl: public GPGBonesManagerInterface

	NOTE You need to init mesh at full resolution

******************************************************************************/
class GPGBonesManagerImpl: public GPGBonesManagerInterface
	{
	public:

					GPGBonesManagerImpl(GPGBonesManagerInterface *pCloneOf);
virtual				~GPGBonesManagerImpl(void);

virtual	GPGRESULT	Reset(void);
virtual	GPGRESULT	ResetUnshared(void);

virtual	GPGRESULT	SetBool(BooleanProperty prop,bool set);
virtual	GPGRESULT	GetBool(BooleanProperty prop,bool *get);
virtual	GPGRESULT	SetInteger(IntegerProperty prop,I32 set);
virtual	GPGRESULT	GetInteger(IntegerProperty prop,I32 *get);
virtual	GPGRESULT	SetFloat(FloatProperty prop,F32 set);
virtual	GPGRESULT	GetFloat(FloatProperty prop,F32 *get);
virtual GPGRESULT	GetTimedPeriod(TimedItem prop,F32 *get);
virtual	GPGRESULT	SetMeshFactory(GPGMeshFactory *set)
						{
						m_meshfactory=set;
						return GPG_OK;
						}

virtual	GPGRESULT	InitMesh(GPGMeshGroup *pMeshGrp,
								GPGVertexMapGroup *pVertexMapGroup);
virtual GPGRESULT	UpdateMesh(GPGMeshGroup *in,GPGMeshGroup **out);
virtual GPGRESULT	DeformMesh(GPGMeshGroup *in,GPGMeshGroup **out);

virtual	GPGRESULT	LoadBones(const char *bonefilename);
virtual	GPGRESULT	PrepareBones(void);
virtual	GPGRESULT	SaveBones(const char *bonefilename,bool binary);
virtual	GPGRESULT	LoadMesh(const char *meshfilename,GPGMeshGroup **mesh);
virtual	GPGRESULT	DrawMesh(GPGRender *render,U32 meshid);

virtual	GPGRESULT	GetBoneIndex(GPGString bonename,long *boneid);
virtual	GPGRESULT	GetBoneName(long boneid,GPGString *bonename);

virtual GPGRESULT	SelectMotionMixer(GPGMotionMixerInterface *select);
virtual	GPGRESULT	BlendToMotionMixer(GPGMotionMixerInterface *next,
												const GPGBlendParam *param);

virtual GPGRESULT	GetRootClearance(GPGTransform *transform)
						{
						*transform=m_rootTransform;
						return GPG_OK;
						};

virtual	GPGRESULT	CreateIKChain(long baseboneid,long endboneid);

virtual GPGRESULT	GetCharacter(GPGCharacter **ppGet)
						{
						if(!ppGet)
							return GPG_E_INVALID_POINTER;

						*ppGet=(GPGCharacter *)m_character;
						return GPG_OK;
						};

virtual	GPGRESULT	SetInfoRender(GPGInfoRender *set)
						{
						m_inforender=set;
						return GPG_OK;
						};
virtual GPGRESULT	RenderInfo(void);
virtual	GPGRESULT	SetProgressCallback(GPGProgressCB progressCB)
						{
						m_character->SetProgressCB(progressCB);
						return GPG_OK;
						};
virtual	GPGRESULT	SetGetTimerCallback(GPGGetTimerCB set)
						{
						m_getTimerCB=set;
						return GPG_OK;
						};

virtual GPGRESULT	SetAutoTranslate(const GPGVector3 &vector,bool last);
virtual GPGRESULT	SetAutoRotate(const GPGQuaternion &quaternion,bool last);
virtual GPGRESULT	SetAutoScale(const GPGVector3 &vector,bool last);
virtual GPGRESULT	AddToAutoTranslate(const GPGVector3 &addVector,bool last);
virtual GPGRESULT	AddToAutoRotate(const GPGQuaternion &addQuaternion,
																	bool last);
virtual GPGRESULT	AddToAutoScale(const GPGVector3 &addVector,bool last);

virtual GPGRESULT	GetMotionMixer(U32 index,
										GPGMotionMixerInterface **ppMotionMixer)
						{
						if(!ppMotionMixer)
							return GPG_E_INVALID_POINTER;

						*ppMotionMixer=index? m_lastmixer: m_motionmixer;
						return GPG_OK;
						};

virtual	void		ReplaceCharacter(GPGMeshGroup_Character *set);

static	void		TimeProgress(GPGCharacter *character,U32 index,bool on);

		void		DrawCircle(GPGTransform &transform,
							const GPGVector3 *scale,const GPGVector3 &color);
	private:

		void		ApplyAutoTransition(	const GPGVector3 &location1,
											const GPGVector3 &location2,
											const GPGQuaternion &rotation1,
											const GPGQuaternion &rotation2);

		void		DrawTransformedPointArray(
						GPGTransform &transform,const GPGVector3 *scale,
						const GPGVector3 *vertex,long vertices,
						bool multicolor,const GPGVector3 *color);

		void		DrawTransformedTriangleArray(
						GPGTransform &transform,const GPGVector3 *scale,
						const GPGVector3 *vertex,const GPGVector3 *normal,
						long vertices,
						bool strip,bool multicolor,const GPGVector3 *color);

		void		DrawTransformedLineArray(
						GPGTransform &transform,const GPGVector3 *scale,
						const GPGVector3 *vertex,long vertices,
						bool strip,bool multicolor,const GPGVector3 *color);

		void		DrawBoneMesh(GPGTransform &transform,float radius,
										float length,const GPGVector3 &color);

		void		DrawGrid(const GPGVector3 &color);
		void		DrawAxes(GPGTransform &transform,float length,
													const GPGVector3 &colorX,
													const GPGVector3 &colorY,
													const GPGVector3 &colorZ);

		void		DrawConstraints(GPGTransform &transform,
										GPGBoneNode &bone,float axis_length);

		void		DrawUnitLineInX(GPGTransform &transform,
							const GPGVector3 *scale,const GPGVector3 &color);
		void		DrawUnitLineInY(GPGTransform &transform,
							const GPGVector3 *scale,
							bool reduced,const GPGVector3 &color);


		void		CreateCircle(void);
		void		DrawArc(GPGTransform &transform,
							const GPGVector3 *scale,float startangle,
							float endangle,const GPGVector3 &color);

		void		CalcJoints(void);

		GPGBoneNode	*GetBoneByIndex(long index);


		GPGMeshGroup_Character	*m_character,*m_local_character;
		GPGVertexWeights		m_unlinked_weights; // don't need if no save

		GPGMeshFactory			*m_meshfactory;

		GPGInfoRender			*m_inforender;
		bool					m_bools[MaxBoolean];
		I32						m_integers[MaxInteger];
		F32						m_floats[MaxFloat];
		bool					m_joints_calced;

		GPGVector3				m_circledata[GPG_BONECIRCLE_SIDES+1];

		GPGGetTimerCB			m_getTimerCB;
		U32						m_frames;
		F32						m_lasttimer[NumTimedItems];
		F32						m_sumtimer[NumTimedItems];
		F32						m_lastperiod[NumTimedItems];


		//* WARNING not sharable
		GPGMotionMixerInterface	*m_motionmixer;

								// blending params
		GPGMotionMixerInterface	*m_lastmixer;
		GPGBlendParam			m_blendparam;
		F32						m_lasttime;
		F32						m_blendscale;
		GPGTransform			m_rootTransform;
		GPGVector3				m_auto_translate;
		GPGVector3				m_last_translate;
		GPGQuaternion			m_auto_rotate;
		GPGQuaternion			m_last_rotate;
		GPGVector3				m_auto_scale;
		GPGVector3				m_last_scale;
	};
