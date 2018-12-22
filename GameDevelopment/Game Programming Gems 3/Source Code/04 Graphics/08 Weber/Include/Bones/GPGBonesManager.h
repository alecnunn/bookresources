
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

#ifndef __GPGBonesManager_h__
#define __GPGBonesManager_h__


class GPGMeshGroup;
class GPGMeshGroup_Character;
class GPGMeshFactory;
class GPGVertexMapGroup;
class GPGMotionMixer;
class GPGInfoRender;
class GPGCharacter;
class GPGTransform;


/// accepts text string describing progress
typedef void (*GPGProgressCB)(const char *text);

/// should return precise relative timer value in miliseconds
typedef float (*GPGGetTimerCB)(void);



/**************************************************************************//**
class GPGBonesManagerInterface

	This is an (almost) headerless interface to GPGBonesManager_Impl,
	representing a small subset of the bones-based animation.

	You cannot instantiate a GPGBonesManagerInterface directly.
	You must use ifxCreateBonesManager() to create an GPGBonesManager_Impl
	and return it as a GPGBonesManagerInterface.

	You should not delete a GPGBonesManagerInterface directly.
	It may seem like it works, but destructors will not be called
	and you will leak memory.  You must use ifxDestroyBonesManager().

	WARNING Although you can send a new mesh by just using UpdateMesh()
		the old vertexmap must still be valid for that mesh.

	LoadMesh() is an alternative to InitMesh() for stand-alone tests.

*//***************************************************************************/
class GPGBonesManagerInterface
	{
	public:
					/// `standard interface'

		enum BooleanProperty
				{
				// effective during run time
				ShowBones,			// display bones as wireframe
				ShowBoneNames,		// display bone name text
				ShowImplants,		// display bone implants
				ShowGrid,			// display world grid
				ShowWorldAxes,		// display axes at world origin
				ShowAxes,			// display axes on every bone
				ShowJoints,			// display joint cross-sections
				ShowCylinders,		// display cross-sections connected
				ShowConstraints,	// display IK constraint arcs
				ShowWeights,		// display weights on selected bone
				ShowAllWeights,		// display all weights with RGB blend
				ShowEffectors,		// display IK effectors
				ShowBoneLinks,		// display bone links
				ShowVertices,		// display vertex text (slow)
				ShowNormals,		// display normals as red lines with yellow
									// showing diff from length of 1.0
				AntiAliasLine,		// toggle line anti-aliasing
				ApplyIK,			// toggle Inverse Kinematics
				LimitAngVelocity,	// limit IK changes
				ReferenceLock,		// lock bones to reference position
				RootLock,			// lock root bone(s) to reference location
				RootClearTranslate,	// remove root bone(s) translation
				RootClearRotate,	// remove root bone(s) rotation
				NoDeform,			// only compute bones, do not deform
				Renormalize,		// renormalize normals after deformation
				IKIncremental,		// apply IK incrementally
				TimerUpdate,		// sets itself when new timer data,
									// clears itself when read (read-only)
				MultiResLimited,	// only deform vertices under MRM limit
				AutoTranslationX,	// chained anims attach head to tail
				AutoTranslationY,
				AutoTranslationZ,
				AutoRotationX,
				AutoRotationY,
				AutoRotationZ,
				AutoScaleX,
				AutoScaleY,
				AutoScaleZ,
				AutoBlend,			// use auto blendtime vs. BlendFraction

				// only effective at load time
				LoadJustWeights,	// does not reload bones reference data
				CreateBoneLinks,	// use bone links
				RegenerateWeights,	// create new weights algorithmically
				RemoveRogueWeights,	// remove spurious patches of weights
				SmoothWeights,		// smooth transitions in weights
				SmoothWeld,			// smooth close vertices collectively
				PrepareForSave,		// store info allowing bone save

				// changed per bone using IntegerProperty SelectBone
				AutoJoint,			// automatically compute joint section
				AutoLink,			// automatically compute link params
				ForTip,				// apply joint edits to bone tip vs base
				IsInfluential,		// bone is used in weight regeneration
				MaxBoolean };

		enum IntegerProperty
				{
				// effective during run time
				SelectBone,			// pick specific bone by id
				NumberOfBones,		// number of bones to pick from (read-only)
				DefaultBoneLinks,	// default number of links for AutoLink
				ShowSingleMesh,		// display data for a single mesh (meshid+1)
				VertexCycle,		// only display vertex data where
									// vertexid%VertexCycles=VertexCycle
				VertexCycles,
				IKIterations,		// number of iteration per frame for IK

				// changed per bone using IntegerProperty SelectBone
				NumBoneLinks,		// number of links if AutoLink not set
				MaxInteger };

		enum FloatProperty
				{
				// effective during run time
				Time,				// current animation time
				MinTime,			// min time of current motion (read-only)
				MaxTime,			// max time of current motion (read-only)
				BoneWidthScale,		// scaling ratio for bones display
				BoneMaxWidth,		// scaling limit for bones
				GridSize,			// size of world grid
				AxisLength,			// length of bone axis for display
				WorldAxisLength,	// length of world axis for display
				MaxAngVelocity,		// max angular velocity for IK
				BlendFraction,		// fraction from old to new mixer
				BlendStartTime,		// time which blend begins
									// reset by BlendToMixer()
				ReachingLimit,		// max distance of any vertex from root
									// (with fixed bone length and displacement)

				// only effective at load time
				ModelSize,			// approximate size of model
				SmoothThreshold,	// max delta for smoothing
				WeldMax,			// max distance to smooth collectively

				// changed per bone using IntegerProperty SelectBone
				LinkLength,			// total length of bone links
				JointCenterY,		// joint cross-section center and radials
				JointCenterZ,		// (dependent on BooleanProperty ForTip)
				JointScaleY,
				JointScaleZ,
				MaxFloat };

		// for built-in run-time profiling, subject to change
		enum TimedItem {
				TimeFrame,			// frame to next frame
				TimeRenderInfo,		// lines/points/text etc.
				TimeOutside,		// everything but animation
				TimeAllAnim,		// everything in animation
				TimeMotion,			// applying motion to bones
				TimeLastMotion,		// applying last motion to bones
				TimeBlend,			// motion blending
				TimeIK,				// applying IK to bones
				TimeLinkRecalc,		// updating links per frame
				TimeBoneCache,		// building bone transform cache
				TimeDeformation,	// apply core vertex blending
				TimeRenormalize,	// renormalizing normals
				NumTimedItems };

					/// Set/Get various properties as listed above
virtual	GPGRESULT	SetBool(BooleanProperty prop,bool set)					=0;
virtual	GPGRESULT	GetBool(BooleanProperty prop,bool *get)					=0;
virtual	GPGRESULT	SetInteger(IntegerProperty prop,I32 set)				=0;
virtual	GPGRESULT	GetInteger(IntegerProperty prop,I32 *get)				=0;
virtual	GPGRESULT	SetFloat(FloatProperty prop,F32 set)					=0;
virtual	GPGRESULT	GetFloat(FloatProperty prop,F32 *get)					=0;

					/// setup mesh (usually call only once)
virtual	GPGRESULT	InitMesh(GPGMeshGroup *pMeshGrp,
								GPGVertexMapGroup *pVertexMapGroup)			=0;

					/// apply animation to a mesh (for every frame)
					/// (same pVertexMapGroup assumed)
virtual	GPGRESULT	UpdateMesh(GPGMeshGroup *in,GPGMeshGroup **out)			=0;

					/// `extended interface'

					/** Just deform the mesh.
						Useful when DeformMesh bool is turned off
						to skip deformation in UpdateMesh(). */
virtual	GPGRESULT	DeformMesh(GPGMeshGroup *in,GPGMeshGroup **out)			=0;
					/// reset manager as though just instantiated
virtual	GPGRESULT	Reset(void)												=0;
					/// reset mixer assignment and autotranslation
virtual	GPGRESULT	ResetUnshared(void)										=0;
					/// tell bones manager how to create a mesh
virtual	GPGRESULT	SetMeshFactory(GPGMeshFactory *set)						=0;
					/// load reference bones data from a .bon file
virtual	GPGRESULT	LoadBones(const char *bonefilename)						=0;
					/// prepare existing bones data
virtual	GPGRESULT	PrepareBones(void)										=0;
					/// save reference bones data to a .bon file
virtual	GPGRESULT	SaveBones(const char *bonefilename,bool binary=false)	=0;
					/// lookup bone index by name
virtual	GPGRESULT	GetBoneIndex(GPGString bonename,long *boneid)			=0;
					/// lookup bone name by index
virtual	GPGRESULT	GetBoneName(long boneid,GPGString *bonename)			=0;
					/// quickly switch to an already setup motion mixer
virtual	GPGRESULT	SelectMotionMixer(GPGMotionMixerInterface *select)		=0;
					/// blend from one mixer to the next
virtual	GPGRESULT	BlendToMotionMixer(GPGMotionMixerInterface *next,
											const GPGBlendParam *blendparam)=0;
					/// setup Inverse Kinematics chain between two bones
virtual	GPGRESULT	CreateIKChain(long baseboneid,long endboneid)			=0;
					/** get data removed by RootClearTranslate and/or
						RootClearRotate */
virtual	GPGRESULT	GetRootClearance(GPGTransform *transform)				=0;
					/// Set current vector for auto-translation
virtual	GPGRESULT	SetAutoTranslate(const GPGVector3 &vector,bool last)	=0;
					/// Set current quaternion for auto-rotation
virtual	GPGRESULT	SetAutoRotate(const GPGQuaternion &quaternion,bool last)=0;
					/// Set current vector for auto-scale
virtual	GPGRESULT	SetAutoScale(const GPGVector3 &vector,bool last)		=0;
					/// Alter current vector for auto-translation
virtual	GPGRESULT	AddToAutoTranslate(const GPGVector3 &addVector,
																bool last)	=0;
					/// Alter current quaternion for auto-rotation
virtual	GPGRESULT	AddToAutoRotate(const GPGQuaternion &addQuaternion,
																bool last)	=0;
					/// Alter current vector for auto-scale
virtual	GPGRESULT	AddToAutoScale(const GPGVector3 &addVector,bool last)	=0;

virtual GPGRESULT	GetMotionMixer(U32 index,
								GPGMotionMixerInterface **ppMotionMixer)	=0;

					/// `test interface'

					/// Get hidden instance of (GPGCharacter *)
					// you must be able to declare a (GPGCharacter *) to use
					// this requires lots of extra headers
virtual	GPGRESULT	GetCharacter(GPGCharacter **character)					=0;

					/// load mesh manually instead of using InitMesh()
virtual	GPGRESULT	LoadMesh(const char *meshfilename,GPGMeshGroup **mesh)	=0;
					/// draw mesh using a specific GPGRender
virtual	GPGRESULT	DrawMesh(GPGRender *render,U32 meshid)					=0;

					/// tell bones manager how to draw line/point/text data
virtual	GPGRESULT	SetInfoRender(GPGInfoRender *set)						=0;
					/// draw line/point/text data
virtual	GPGRESULT	RenderInfo(void)										=0;
					/// provide a function that is to be given (char *)
					/// messages explaining what the bones manager is doing
virtual	GPGRESULT	SetProgressCallback(GPGProgressCB progressCB)			=0;
					/// provide a function that returns a float providing
					/// a precise incrementing fractional millisecond counter
					/// (required for timing operations)
virtual	GPGRESULT	SetGetTimerCallback(GPGGetTimerCB getTimerCB)			=0;
					/// find time spent in specific timed area
virtual	GPGRESULT	GetTimedPeriod(TimedItem prop,F32 *get)					=0;
					/// replace auto-allocated character with another
virtual	void		ReplaceCharacter(GPGMeshGroup_Character *set)			=0;
	};


			/// short text name of timed areas
const char	GPGBonesManagerTimedItemName
								[GPGBonesManagerInterface::NumTimedItems][16]=
			{ "Frame", "Info", "Out",
				"Anim", "Mot", "Last", "Blend",
				"IK", "Link",
				"Cache", "Deform", "Norm" };


extern "C"
{
#ifdef GPGBONESDLL

// Class factory definitions for run-time linking of the
// technology though a DLL interface.
#define FN_GPGCreateBonesManager		"GPGCreateBonesManager"
typedef GPGBonesManagerInterface*
		(*PF_GPGCreateBonesManager)(GPGBonesManagerInterface *pCloneOf);

#define FN_GPGDeleteBonesManager		"GPGDeleteBonesManager"
typedef void (*PF_GPGDeleteBonesManager)(GPGBonesManagerInterface* pManager);

#else

// Class factory methods for static linking of the
// technology through a library.
GPGBonesManagerInterface* GPGCreateBonesManager(
									GPGBonesManagerInterface *pCloneOf=NULL);
void GPGDeleteBonesManager(GPGBonesManagerInterface* pManager);

#endif
}


#endif /* __GPGBonesManager_h__ */
