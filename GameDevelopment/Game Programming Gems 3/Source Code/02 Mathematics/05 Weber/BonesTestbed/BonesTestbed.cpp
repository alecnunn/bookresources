
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


#include "../Bones/GPGBackBones.h"
#include "GPGBonePortal.h"


enum DrawType { SOLID, HIDDENWIRE, OUTLINE, WIREFRAME, POINTCLOUD, AUTO };



#define	TEST_IK				FALSE	//* FALSE=load motion TRUE=use IK

#define SHOW_IMPLANTS		TRUE

#define FIXED_FRAMERATE		TRUE
#define TARGET_FRAMERATE	30.0f

							// if none specified, try loading a default set
#define	MESH_FILE			"harley_roll2.geo"
#define	BONE_FILE			"harley_roll2.bon"
#define	MOTION_FILE			"harley_roll2.mot"
#define	MOTION2_FILE		"harley_roll2_twist.mot"
#define	BLEND_AT			0.0f	// 1.2f
#define	BLENDTIME			0.533f	// 0.8f


#define	EFFECTOR_TESTED		"Bip01 R Hand"
#define	EFFECTOR_BASE		"Bip01 R Clavicle"

#define	FILENAME_LENGTH		256

#define	CAMERA_FOVY			30.0f
#define	CAMERA_PHI			0.0f		// 0.0f
#define	CAMERA_AZIMUTH		0.0f		// 270.0f

		// manager inputs
#define	QUALITY					TRUE
#define	BONE_LINKS				QUALITY
#define	NUM_BONE_LINKS			4
#define	REGENERATE_WEIGHTS		QUALITY
#define	SMOOTH_WEIGHTS			QUALITY
#define	SMOOTH_THRESHHOLD		(0.3f)	// 0.3f
#define	SMOOTH_WELD				TRUE
#define	WELDMAX					(0.001f)
#define	VERTEX_CYCLES			10
#define	REMOVE_ROGUEWEIGHTS		QUALITY
#define	LIMIT_ANGVELOLCITY		TRUE
#define	MAX_ANGVELOLCITY		0.1f
#define	IK_INCREMENTAL			TRUE
#define	IK_ITERATIONS			1

#define	TIMER_UPDATE		1000		// in ms

#define	AXIS_LENGTH			(0.02f)
#define	WORLDAXIS_LENGTH	(0.2f)

#define	BONE_WIDTH			(0.04f)
#define	BONE_ASPECT			(1.0f/12.0f)	// (1.0f/8.0f)

#define	KEY_AXES			'a'
#define	KEY_BONES			'b'
#define	KEY_CULLING			'c'
#define	KEY_DUAL_COLOR		'd'
#define	KEY_RENORMALIZE		'e'
#define	KEY_HELP			'h'
#define	KEY_INPUTMESH		'i'
#define	KEY_JOINTS			'j'
#define	KEY_CONSTRAINTS		'k'
#define	KEY_LINES			'l'
#define	KEY_NORMALS			'm'
#define	KEY_BONENAMES		'n'
#define	KEY_ORIGIN			'o'
#define	KEY_PARTIALMESH		'p'
#define	KEY_RENDERING		'r'
#define	KEY_SKIN			's'
#define	KEY_TEXTURE			't'
#define	KEY_VERTNUMBERS		'v'
#define	KEY_WEIGHTS			'w'
#define	KEY_BONELINKS		'x'
#define	KEY_CYLINDERS		'y'
#define	KEY_VERTEXCYCLE		'z'
#define	KEY_ROOTLOCK		','
#define	KEY_ANTIALIAS		'/'
#define	KEY_TESTFLAG		'.'
#define	KEY_TIMERS			'-'
#define	KEY_SPECIALEVENT	'`'
#define	KEY_RESET			27	// ESC

#define	GPGKEY_USED			(1<<14)

#define MOUSES	10
		//							rot1,rot2	panx,panz		time,transy
static	float MouseScale[MOUSES]={ 0.5f,0.5f, 0.004f,0.004f, 0.0033f,0.004f,
//							zoom-wheel,transx	auxillary
							0.06f,0.004f,		1.0f,1.0f };

extern long	ewColorCodeMode;
extern void wbaInstallModuleDeliveries(const GPL_String & component,
                                        const GPL_String & value);

float		tickscale;
bool		show_lines;
bool		ctrled=false;
bool		shifted=false;
bool		leftright=false;
bool		antialias=false;

GPGInfoRenderOGL	inforenderOGL;

float	GetTickTimer(void);
void	StateProgress(const char *text);
void	DrawLine(long color,const GPGVector3 &first,const GPGVector3 &second);


class BonesTestbed: public GPGBonePortal
	{
	public:
				BonesTestbed(void)
					{
					bonesmanager=GPGCreateBonesManager();
					motionmanager=GPGCreateMotionManager();
					motionmixer=GPGCreateMotionMixer();
					pastmixer=GPGCreateMotionMixer();

					meshfactory=new GPGMeshInterleavedFactory(OGL);
					bonesmanager->SetMeshFactory(meshfactory);

					tickscale=(float)ifxGetTickFrequency();
					if(tickscale>0.0f)
						tickscale=1.0f/tickscale;
					else
						tickscale=1e6;

					motionmixer->AssociateWithBonesManager(bonesmanager);
					motionmixer->AssociateWithMotionManager(motionmanager);
					pastmixer->AssociateWithBonesManager(bonesmanager);
					pastmixer->AssociateWithMotionManager(motionmanager);

					selectbone=NULL;
					creature=NULL;

					long m;
					for(m=0;m<GPGKEY_USED;m++)
						{
						keycount[m]=0;
						keymax[m]=1;
						}
					for(m=0;m<MOUSES;m++)
						fmouse[m]=0.0f;
					for(m=0;m<MOUSES;m++)
						dfmouse[m]=0.0f;

					fmouse[0]=CAMERA_AZIMUTH;
					fmouse[1]=CAMERA_PHI;
					fmouse[3]=0.1f;
					fmouse[6]=1.5f;

					keymax[KEY_INPUTMESH]=2;
					keymax[KEY_WEIGHTS]=2;
					keymax[KEY_RENDERING]=4;
					keymax[KEY_VERTEXCYCLE]=VERTEX_CYCLES;

					keycount[KEY_INPUTMESH]=0;
					keycount[KEY_TESTFLAG]=0;
					keycount[KEY_RENORMALIZE]=1;
					keycount[KEY_ROOTLOCK]=1;
					keycount[KEY_SKIN]=1;
					keycount[KEY_ORIGIN]=1;
					keycount[KEY_ANTIALIAS]=0;

					keycount[KEY_CULLING]=1;

					keycount[KEY_RENDERING]=1;

					bend=0.0f;
					inc=2.0f;
					};

				~BonesTestbed(void)
					{
					GPGDeleteBonesManager(bonesmanager);
					GPGDeleteMotionManager(motionmanager);
					GPGDeleteMotionMixer(motionmixer);
					GPGDeleteMotionMixer(pastmixer);
					}

		void	Initialize(void)
					{
					DrawBanner("Initializing");

					CreateTexture();

					SetFOVY(CAMERA_FOVY);

					mesh_filename[0]=0;
					bone_filename[0]=0;
					motion_filename[0]=0;

					//* read command line
					const char *line=GetCommandLine();
//					const char *line="abc.bon\ndef.mot\nghi.geo";
					long m=0,linelen=strlen(line);
					bool found=false;
					while(m<linelen)
						{
						long n=m;
						while(n<linelen && line[n]!=' ')
							n++;

						if( !strncmp(&line[n-3],"geo",3) ||
							!strncmp(&line[n-3],"GEO",3) )
							{
							found=true;
							strncpy(mesh_filename,&line[m],n-m);
							mesh_filename[n-m]=0;
//					glDepthFunc(GL_LEQUAL);
							}
						if( !strncmp(&line[n-3],"bon",3) ||
							!strncmp(&line[n-3],"BON",3) )
							{
							found=true;
							strncpy(bone_filename,&line[m],n-m);
							bone_filename[n-m]=0;
							}
						if( !strncmp(&line[n-3],"mot",3) ||
							!strncmp(&line[n-3],"MOT",3) )
							{
							found=true;
							strncpy(motion_filename,&line[m],n-m);
							motion_filename[n-m]=0;
							}

						m=n+1;
						}

					if(!found)
						{
						strncpy(mesh_filename,MESH_FILE,FILENAME_LENGTH);
						mesh_filename[FILENAME_LENGTH-1]=0;
						strncpy(bone_filename,BONE_FILE,FILENAME_LENGTH);
						bone_filename[FILENAME_LENGTH-1]=0;
						strncpy(motion_filename,MOTION_FILE,FILENAME_LENGTH);
						motion_filename[FILENAME_LENGTH-1]=0;
						}

					creature=NULL;
					bonesmanager->GetCharacter(&creature);

					if(creature)
						creature->SetDrawLineCB(&DrawLine);

					bonesmanager->SetProgressCallback(&StateProgress);
					bonesmanager->SetGetTimerCallback(&GetTickTimer);
					bonesmanager->SetInfoRender(&inforenderOGL);
					bonesmanager->SetBool(GPGBonesManagerInterface::ApplyIK,
								TEST_IK!=0);

					keymax[KEY_RENDERING]++;

					GPGMeshGroup *resultmesh;
					bonesmanager->LoadMesh(mesh_filename,&resultmesh);
					bonesmanager->SetFloat(GPGBonesManagerInterface::ModelSize,
								GetModelScale());
					bonesmanager->SetBool(
								GPGBonesManagerInterface::CreateBoneLinks,
								BONE_LINKS);
					bonesmanager->SetInteger(
								GPGBonesManagerInterface::DefaultBoneLinks,
								NUM_BONE_LINKS);
					bonesmanager->SetBool(
								GPGBonesManagerInterface::RegenerateWeights,
								REGENERATE_WEIGHTS);
					bonesmanager->SetBool(
								GPGBonesManagerInterface::RemoveRogueWeights,
								REMOVE_ROGUEWEIGHTS);
					bonesmanager->SetBool(
								GPGBonesManagerInterface::SmoothWeights,
								SMOOTH_WEIGHTS);
					bonesmanager->SetFloat(
								GPGBonesManagerInterface::SmoothThreshold,
								SMOOTH_THRESHHOLD);
					bonesmanager->SetBool(GPGBonesManagerInterface::SmoothWeld,
								SMOOTH_WELD);
					bonesmanager->SetFloat(GPGBonesManagerInterface::WeldMax,
								WELDMAX);
					bonesmanager->SetBool(
								GPGBonesManagerInterface::PrepareForSave,
								FALSE);
					bonesmanager->LoadBones(bone_filename);

					bonesmanager->SetBool(
								GPGBonesManagerInterface::LimitAngVelocity,
								LIMIT_ANGVELOLCITY);
					bonesmanager->SetFloat(
								GPGBonesManagerInterface::MaxAngVelocity,
								MAX_ANGVELOLCITY);
					bonesmanager->SetBool(
								GPGBonesManagerInterface::IKIncremental,
								IK_INCREMENTAL);
					bonesmanager->SetInteger(
								GPGBonesManagerInterface::IKIterations,
								IK_ITERATIONS);

					ikbase= -1;
					ikend= -1;
#if TEST_IK
					bonesmanager->GetBoneIndex(EFFECTOR_BASE,&ikbase);
					bonesmanager->GetBoneIndex(EFFECTOR_TESTED,&ikend);
					bonesmanager->CreateIKChain(ikbase,ikend);
#endif	// TEST_IK

					if(creature->GetSkin() &&
										creature->GetSkin()->GetInputMesh())
						keymax[KEY_PARTIALMESH]=
								creature->GetSkin()->GetInputMesh()
													->GetNumberMeshes();

					//* load motion
					DrawBanner("Loading Motion");

					bool replace=false;
					long requestid=0,resultid= -1;
					motionmanager->LoadMotion(motion_filename,replace,
														requestid,&resultid);
					if(resultid>=0)
						{
#if !TEST_IK
						motionmixer->MapFullMotionToCharacter(resultid);
#endif

						if(ikend>=0)
							motionmixer->ActivateIK(ikend,true);
						}

					bonesmanager->SelectMotionMixer(motionmixer);

					DrawBanner("Initialized");
					};


		float	GetModelScale(void)
					{
					float scale=1.0f;

					if(!creature)
						return scale;

					creature->ForEachNodeTransformed(GPGPARENTFIRST,
									&BonesTestbed::FindBoneBounds,&scale);

					GPGLOG("creature scale %.6G\n",scale);

					if(!creature->GetSkin())
						return scale;

					GPGVector3 min,max;
					creature->GetSkin()->CalcBounds(&min,&max);

					long m;
					for(m=0;m<3;m++)
						{
						if(scale<(float)fabs(max[m]-min[m]))
							scale=(float)fabs(max[m]-min[m]);
						if(scale<(float)fabs(max[m]))
							scale=(float)fabs(max[m]);
						if(scale<(float)fabs(min[m]))
							scale=(float)fabs(min[m]);
						}

					if(scale<1.0f)
						scale=1.0f;

					return scale;
					};

		void	ScaleViewer(void)
					{
					float scale=GetModelScale();

					MouseScale[2]*=scale;
					MouseScale[3]*=scale;
					MouseScale[5]*=scale;
					MouseScale[6]*=scale;
					MouseScale[7]*=scale;
					fmouse[3]*=scale;
					fmouse[6]*=scale;

					SetNearFar(0.05f*scale,50.0f*scale);

					bonesmanager->SetFloat(
									GPGBonesManagerInterface::AxisLength,
									scale*AXIS_LENGTH);
					bonesmanager->SetFloat(
									GPGBonesManagerInterface::WorldAxisLength,
									scale*WORLDAXIS_LENGTH);
					bonesmanager->SetFloat(
									GPGBonesManagerInterface::BoneWidthScale,
									BONE_ASPECT);
					bonesmanager->SetFloat(
									GPGBonesManagerInterface::BoneMaxWidth,
									scale*BONE_WIDTH);

					viewerscale=scale;
					};

static	BOOL	FindBoneBounds(GPGCoreNode &node,GPGTransform &transform,
															GPGVoidStar state)
					{
					if(node.IsBone())
						{
						float *scale=NULL;
						GPGRESULT result=state.CopyPointerTo(&scale);
						GPGASSERT(result==GPG_OK);
						GPGASSERT(scale);

						GPGBoneNode &bone=(GPGBoneNode &)node;

						GPGVector3 vert1,vert2;
						vert1.Set(bone.GetLength(),0.0f,0.0f);
						transform.TransformVector(vert1,vert2);
						transform.CopyTranslation(vert1);

						long m;
						for(m=0;m<3;m++)
							{
							if(*scale<(float)fabs(vert1[m]))
								*scale=(float)fabs(vert1[m]);
							if(*scale<(float)fabs(vert2[m]))
								*scale=(float)fabs(vert2[m]);
							}
						}
					return false;
					};

virtual void	IdleFunction(void)
					{
					long update=false;

					// select parent bone
					if(keycount[EW_KEY_CURSOR_UP])
						{
						keycount[EW_KEY_CURSOR_UP]=0;
						if(selectbone)
							{
							GPGCoreNode *parent=selectbone->Parent();
							if(parent->IsBone())
								selectbone=(GPGBoneNode *)parent;
							else
								selectbone=NULL;
							}

						update=true;
						}

					// select child bone
					if(keycount[EW_KEY_CURSOR_DOWN])
						{
						keycount[EW_KEY_CURSOR_DOWN]=0;
						if(selectbone)
							{
							if(selectbone->Children().GetNumberElements())
								selectbone=selectbone->Children().GetHead();
							}
						else if(creature)
							if(creature->Children().GetNumberElements())
								selectbone=creature->Children().GetHead();

						update=true;
						}

					// select prior/next sibling
					long left;
					if( (left=keycount[EW_KEY_CURSOR_LEFT]) ||
												keycount[EW_KEY_CURSOR_RIGHT])
						{
						keycount[EW_KEY_CURSOR_LEFT]=0;
						keycount[EW_KEY_CURSOR_RIGHT]=0;
						if(selectbone)
							{
							GPGListContext context;
							GPGBoneNodeList &siblings=selectbone->Parent()
															->Children();
							if(siblings.SearchForElement(context,selectbone))
								{
								if(left && siblings.PreDecrement(context))
									selectbone=siblings.GetCurrent(context);
								else if(!left&&siblings.PreIncrement(context))
									selectbone=siblings.GetCurrent(context);
								}
							}

						update=true;
						}

					if(update)
						UpdateManager();

					float modelscale;
					bonesmanager->GetFloat(GPGBonesManagerInterface::ModelSize,
																&modelscale);
					if(modelscale==0.0f)
						modelscale=1.0f;
					float zoomscalar=fmouse[6]/(10.0f*modelscale);

					long m;
					if(leftright || shifted)
						{
						float radians=fmouse[0]*GPGTO_RAD;
						float cosa=GPGCOS(radians);
						float sina=GPGSIN(radians);
						dfmouse[7]=dfmouse[4]*cosa-dfmouse[5]*sina;
						dfmouse[5]=dfmouse[5]*cosa+dfmouse[4]*sina;

						dfmouse[2]=dfmouse[0];
						dfmouse[3]=dfmouse[1];
						dfmouse[0]=0;
						dfmouse[1]=0;
						dfmouse[4]=0;
						}
					else if(ctrled)
						{
						/*** this leftmouse-dragup=closer seems backwards
								to me, but it seems to be the convention */
						fmouse[6]+=dfmouse[1]*MouseScale[6]*zoomscalar;
						for(m=0;m<6;m++)
							dfmouse[m]=0;

						dfmouse[5]=0;
						}
					else
						dfmouse[5]=0;

					for(m=0;m<MOUSES;m++)
						if(m!=6)
							{
							if(m>1 && m!=4)
								fmouse[m]+=dfmouse[m]*MouseScale[m]*zoomscalar;
							else
								fmouse[m]+=dfmouse[m]*MouseScale[m];

							if(m<2)
								{
								if(fmouse[m]<0.0f)
									fmouse[m]+=360.0f;
								if(fmouse[m]>360.0f)
									fmouse[m]-=360.0f;
								}
							}

					// can't "throw" a pan
					dfmouse[2]=0;
					dfmouse[3]=0;
					dfmouse[5]=0;
					dfmouse[7]=0;
					dfmouse[8]=0;
					dfmouse[9]=0;

					show_lines=(keycount[KEY_LINES]!=0);

					bend+=inc;
					if(fabs(bend)>160.0f)
						inc= -inc;

					antialias=(bool)keycount[KEY_ANTIALIAS];
					};

virtual void	PlaceCamera(void)
					{
					glRotatef(-90.0f,1.0f,0.0f,0.0f);
//					glTranslatef(0.0f,0.0f,-CAMERA_HEIGHT);
					};

virtual void	Draw3D(void)
					{
					ReportErrors("BonesTestbed::Draw3D() pre");

					// clear screen
					glClearColor(0.2f,0.2f,0.2f,0.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					ReportErrors("BonesTestbed::Draw3D() 1");

//					glDepthFunc(GL_LEQUAL);
					glEnable(GL_DEPTH_TEST);

					ReportErrors("BonesTestbed::Draw3D() 2");

					DrawCharacter();

					ReportErrors("BonesTestbed::Draw3D() post");

					glDisable(GL_DEPTH_TEST);
					};

		void	DrawCenter(void)
					{
					float vert[4][3];
					float radius=viewerscale*0.005f;

					vert[0][0]=radius;
					vert[0][1]=0.0f;
					vert[0][2]=0.0f;

					vert[1][0]= -0.5f*radius;
					vert[1][1]=0.87f*radius;
					vert[1][2]=0.0f;

					vert[2][0]=  vert[1][0];
					vert[2][1]= -vert[1][1];
					vert[2][2]=0.0f;

					vert[3][0]=0.0f;
					vert[3][1]=0.0f;
					vert[3][2]=radius;

					glBegin(GL_LINE_STRIP);
					glVertex3fv(vert[0]);
					glVertex3fv(vert[1]);
					glVertex3fv(vert[2]);
					glVertex3fv(vert[0]);
					glVertex3fv(vert[3]);
					glVertex3fv(vert[1]);
					glVertex3fv(vert[2]);
					glVertex3fv(vert[3]);
					glEnd();
					};

		void	DrawCharacter(void)
					{
					glPushMatrix();
					// camera-space pan
					glTranslatef(fmouse[2],fmouse[6],0.0f);

					// phi
					glRotatef(fmouse[1],1.0f,0.0f,0.0f);

					// theta
					glRotatef(fmouse[0],0.0f,0.0f,1.0f);

					glColor3f(1.0f,0.7f,1.0f);
					DrawCenter();

					//* auxilliary meter
					glColor3f(0.6f,1.0f,0.6f);
					glPushMatrix();
					glTranslatef(fmouse[8],0.0f,0.0f);
					DrawCenter();
					glPopMatrix();

					// world-space translation
					glTranslatef(fmouse[7],-fmouse[5],-fmouse[3]);

					if(keycount[KEY_RESET])
						{
						keycount[KEY_RESET]=false;
						selectbone=NULL;
						}

					if(keycount[KEY_SPECIALEVENT])
						{
						keycount[KEY_SPECIALEVENT]=false;

						}

					// draw skin
					if(creature && keycount[KEY_SKIN])
						{
						GPGSkin *pSkin=creature->GetSkin();
						if(pSkin && keycount[KEY_INPUTMESH])
							{
							glColor3f(0.0f,1.0f,1.0f);
							DrawMesh(pSkin->GetInputMesh(),
											(DrawType)keycount[KEY_RENDERING]);
							}

						if(pSkin && keycount[KEY_INPUTMESH]<2)
							{
							glColor3f(0.0f,1.0f,0.0f);
							DrawMesh(pSkin->GetOutputMesh(),
											(DrawType)keycount[KEY_RENDERING]);
//							DrawMesh(pSkin->GetOutputMesh(),POINTCLOUD);
							}
						}

					ReportErrors("BonesTestbed::DrawCharacter()");

					if(creature)
					{
						creature->SetTestFlag(keycount[KEY_TESTFLAG]);

						GPGMeshGroup *img=((GPGMeshGroup_Character *)creature)
														->GetInputMeshGroup();
						GPGMeshGroup *omg=NULL;
						bonesmanager->UpdateMesh(img,&omg);
					}

//					glDisable(GL_CULL_FACE);
//					glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
					SetupOpenGL(FALSE,POINTCLOUD);

					if(keycount[KEY_INPUTMESH]<2)
						bonesmanager->RenderInfo();

					if(keycount[KEY_INPUTMESH])
						{
						if(creature)
							creature->ResetToReference();
						bonesmanager->RenderInfo();
						}


#if TEST_IK
					GPGCoreNode *node;
					if( (node=creature->FindDescendentByName(EFFECTOR_TESTED)) )
						{
						float angle1=fmouse[4]*0.1f;
						float angle2=fmouse[4];
						GPGVector3 effector;
						effector.Set(20.0f*GPGSIN(angle1),
										20.0f*GPGCOS(angle1),
											55.0f+25.0f*GPGSIN(angle2));

//						((GPGBoneNode *)node)->GetEffector()=effector;
						motionmixer->SetPositionExplicitly(ikend,
									&effector,&GPGQuaternion::Identity());
						}
//					creature->ApplyIK();
					TIME_PROGRESS(creature,GPGBonesManagerImpl::TimeIK,true);
					motionmixer->ApplyIK();
					TIME_PROGRESS(creature,GPGBonesManagerImpl::TimeIK,false);
//					Sleep(300);
#endif

					creature->GetSkin()->DeformMesh(
										keycount[KEY_RENORMALIZE]!=0,false);
					TIME_PROGRESS(creature,
									GPGBonesManagerImpl::TimeAllAnim,false);

					glPopMatrix();
					};

		void	SetupOpenGL(bool lit,DrawType type)
					{
					if(type==OUTLINE)
						{
						glDisable(GL_LIGHTING);
						glPolygonMode(GL_FRONT,GL_LINE);
						}
					else if(lit)
						{
						if(keycount[KEY_TEXTURE])
							{
							glPixelStorei(GL_UNPACK_ALIGNMENT,1);
							glTexImage2D(GL_TEXTURE_2D,0,3,64,64,0,
											GL_RGB,GL_UNSIGNED_BYTE,
											texture);
							glTexParameterf(GL_TEXTURE_2D,
											GL_TEXTURE_WRAP_S,GL_REPEAT);
							glTexParameterf(GL_TEXTURE_2D,
											GL_TEXTURE_WRAP_T,GL_REPEAT);
							glTexParameterf(GL_TEXTURE_2D,
											GL_TEXTURE_MAG_FILTER,GL_NEAREST);
							glTexParameterf(GL_TEXTURE_2D,
											GL_TEXTURE_MIN_FILTER,GL_NEAREST);
//							glTexEnvf(GL_TEXTURE_ENV,
//											GL_TEXTURE_ENV_MODE,GL_DECAL);
							glTexEnvf(GL_TEXTURE_ENV,
											GL_TEXTURE_ENV_MODE,GL_MODULATE);
							glEnable(GL_TEXTURE_2D);
//							glShadeModel(GL_FLAT);
							}

						GLfloat black[]={ 0.0f, 0.0f, 0.0f, 1.0f };
						GLfloat diffuse0[]={ 0.4f, 0.4f, 0.4f, 1.0f };
						GLfloat ambient1[]={ 0.0f, 0.4f, 0.0f, 1.0f };
						GLfloat diffuse1[]={ 0.0f, 0.6f, 0.0f, 1.0f };
						GLfloat ambient2[]={ 0.0f, 0.0f, 0.4f, 1.0f };
						GLfloat diffuse2[]={ 0.0f, 0.0f, 0.6f, 1.0f };
						GLfloat specular[]={ 0.0f, 0.0f, 0.0f, 1.0f };
						GLfloat shininess[]={ 10.0f };

						GLfloat colormap[]={ 11.0f, 159.0f, 159.0f };
						glMaterialfv(GL_FRONT,GL_COLOR_INDEXES,colormap);

						if(keycount[KEY_DUAL_COLOR])
							{
							glMaterialfv(GL_BACK,GL_AMBIENT,ambient2);
							glMaterialfv(GL_BACK,GL_DIFFUSE,diffuse2);
							}
						else
							{
							glMaterialfv(GL_BACK,GL_AMBIENT,ambient1);
							glMaterialfv(GL_BACK,GL_DIFFUSE,diffuse1);
							}

						if(type==HIDDENWIRE)
							{
							glMaterialfv(GL_FRONT,GL_AMBIENT,black);
							glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse0);
							}
						else
							{
							glMaterialfv(GL_FRONT,GL_AMBIENT,ambient1);
							glMaterialfv(GL_FRONT,GL_DIFFUSE,diffuse1);
							}

						glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
						glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shininess);

						GLfloat light[]={ 1.0f, 1.0f, 1.0f, 1.0f };
						GLfloat position[]={ 1.0f, -1.0f, 1.0f, 0.0f };

						glLightfv(GL_LIGHT0,GL_POSITION,position);
						glLightfv(GL_LIGHT0,GL_AMBIENT,light);
						glLightfv(GL_LIGHT0,GL_DIFFUSE,light);
						glLightfv(GL_LIGHT0,GL_SPECULAR,light);

						glEnable(GL_LIGHTING);
						glEnable(GL_LIGHT0);
						glPolygonMode(GL_FRONT,GL_FILL);
						}
					else
						{
						glDisable(GL_LIGHTING);
						glDisable(GL_TEXTURE_2D);
						}

					// backface culling
					if(keycount[KEY_CULLING] || type==OUTLINE)
						{
						glEnable(GL_CULL_FACE);
						glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
						}
					else
						{
						glDisable(GL_CULL_FACE);
						glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
						}
					};

		void	DrawMesh(GPGMeshInterface *mesh,DrawType type=POINTCLOUD)
					{
					if(!mesh)
						return;

					long meshid,meshes=mesh->GetNumberMeshes();
					if(keycount[KEY_PARTIALMESH])
						meshes=1;
					for(meshid=0;meshid<meshes;meshid++)
						{
						if(keycount[KEY_PARTIALMESH])
							{
							meshid=keycount[KEY_PARTIALMESH]-1;
							mesh->ChooseMeshIndex(meshid);
							}
						else
							mesh->ChooseMeshIndex(meshid);

						bool usetexture=(keycount[KEY_TEXTURE]!=0);
						if(usetexture &&
									mesh->GetFaceTexCoordsConst(0)[0]>100000)
							usetexture=false;

						if(type==AUTO)
							{
							// NOTE only do when GPGMeshGroup
							SetupOpenGL(true,type);

/*
							GPGMeshGroup_Character *character=
										(GPGMeshGroup_Character *)creature;
							character->Draw(mesh);
							bonesmanager->DrawMesh(&renderOGL,meshid);
*/

							SetupOpenGL(false,type);
							}

						if(type==SOLID && usetexture)
							{
							SetupOpenGL(true,type);

							long face;
							long faces=mesh->GetNumberFaces();

							glBegin(GL_TRIANGLES);
							for(face=0;face<faces;face++)
								{
								const U32 *verts=
											mesh->GetFaceVerticesConst(face);
								const U32 *norms=
											mesh->GetFaceNormalsConst(face);
								const U32 *coords=
											mesh->GetFaceTexCoordsConst(face);

								glTexCoord2fv(
											mesh->GetTexCoordConst(coords[0]));
								glNormal3fv(mesh->GetNormalConst(norms[0]));
								glVertex3fv(mesh->GetVertexConst(verts[0]));
								glTexCoord2fv(
											mesh->GetTexCoordConst(coords[1]));
								glNormal3fv(mesh->GetNormalConst(norms[1]));
								glVertex3fv(mesh->GetVertexConst(verts[1]));
								glTexCoord2fv(
											mesh->GetTexCoordConst(coords[2]));
								glNormal3fv(mesh->GetNormalConst(norms[2]));
								glVertex3fv(mesh->GetVertexConst(verts[2]));
								}
							glEnd();

							SetupOpenGL(false,type);
							}
						else if(type==SOLID || type==HIDDENWIRE)
							{
							SetupOpenGL(true,type);

							long face;
							long faces=mesh->GetNumberFaces();

							glBegin(GL_TRIANGLES);
							for(face=0;face<faces;face++)
								{
								const U32 *verts=
											mesh->GetFaceVerticesConst(face);
								const U32 *norms=
											mesh->GetFaceNormalsConst(face);

								glNormal3fv(mesh->GetNormalConst(norms[0]));
								glVertex3fv(mesh->GetVertexConst(verts[0]));
								glNormal3fv(mesh->GetNormalConst(norms[1]));
								glVertex3fv(mesh->GetVertexConst(verts[1]));
								glNormal3fv(mesh->GetNormalConst(norms[2]));
								glVertex3fv(mesh->GetVertexConst(verts[2]));
								}
							glEnd();

							SetupOpenGL(false,type);
							}

						if(type==OUTLINE || type==HIDDENWIRE)
							{
							SetupOpenGL(false,OUTLINE);

							glEnable(GL_POLYGON_OFFSET_LINE);
							glPolygonOffset(-1.0f, 0.0f);

							long face;
							long faces=mesh->GetNumberFaces();

							glBegin(GL_TRIANGLES);
							for(face=0;face<faces;face++)
								{
								const U32 *verts=
											mesh->GetFaceVerticesConst(face);

								glVertex3fv(mesh->GetVertexConst(verts[0]));
								glVertex3fv(mesh->GetVertexConst(verts[1]));
								glVertex3fv(mesh->GetVertexConst(verts[2]));
								}
							glEnd();

							glDisable(GL_POLYGON_OFFSET_LINE);

							SetupOpenGL(false,OUTLINE);
							}

						if(type==WIREFRAME)
							{
							long face;
							long faces=mesh->GetNumberFaces();

							for(face=0;face<faces;face++)
								{
								// TODO less lines necessary? (redundancy)

								const U32 *verts=
											mesh->GetFaceVerticesConst(face);

//GPGLOG("face %ld/%ld vert %ld %ld %ld\n",meshid,face,verts[0],verts[1],verts[2]);

								glBegin(GL_LINE_LOOP);
								glVertex3fv(mesh->GetVertexConst(verts[0]));
								glVertex3fv(mesh->GetVertexConst(verts[1]));
								glVertex3fv(mesh->GetVertexConst(verts[2]));
								glEnd();
								}
							}

						if(type==POINTCLOUD)
							{
							long vertex;
							long vertices=mesh->GetNumberVertices();

//glPointSize(5);
							glBegin(GL_POINTS);

							for(vertex=0;vertex<vertices;vertex++)
								glVertex3fv(mesh->GetVertexConst(vertex));

							glEnd();
//glPointSize(1);
							}
						}
					}

		void	CreateTexture(void)
					{
					const long bit=0x2;
					unsigned char c;
					long m,n;
					for(m=0;m<64;m++)
						for(n=0;n<64;n++)
							{
							c=128+127*(((m&bit)==0)^(((n&bit))==0));
							texture[m][n][0]=c;
							texture[m][n][1]=c;
							texture[m][n][2]=c;
							}
					};

virtual void	React(EW_Event *event)
					{
					long mx,my;
					event->GetMousePosition(&mx,&my);
					long buttons=event->GetMouseButtons();

					leftright=(buttons==
								(EW_MOUSEBUTTONS_LEFT|EW_MOUSEBUTTONS_RIGHT));
					ctrled=false;
					shifted=false;
					if(event->GetItem()&EW_KEY_CONTROL)
						{
						event->SetItem(event->GetItem()^EW_KEY_CONTROL);
						ctrled=true;
						}
					if(event->GetItem()&EW_KEY_SHIFT)
						{
						event->SetItem(event->GetItem()^EW_KEY_SHIFT);
						shifted=true;
						}

					if(event->IsWork())
						{
						}
					else if(event->Is(EW_EVENT_KEYBOARD,
												EW_ITEM_ALL,EW_STATE_PRESS))
						{
//						long index= (event->GetItem()&255);
						long index=event->GetItem();

						// convert to lower case
						if(index>='A' && index<='Z')
							index+='a'-'A';

						if(index<GPGKEY_USED)
							{
							if(shifted)
								{
								keycount[index]--;
								if(keycount[index]<0)
									keycount[index]=keymax[index];
								}
							else
								{
								keycount[index]++;
								if(keycount[index]>keymax[index])
									keycount[index]=0;
								}
							}
						}
					else if(event->Is(EW_EVENT_MOUSEBUTTON,
											EW_ITEM_WHEEL,EW_STATE_ANY))
						{
						fmouse[6]+=event->GetState()*MouseScale[6]/30.0f;
						}
					else if(event->IsMousePress())
						{
						if(event->GetItem()==EW_ITEM_LEFT)
							event->GetMousePosition(&mousestate[0],
															&mousestate[1]);
						else if(event->GetItem()==EW_ITEM_MIDDLE)
							event->GetMousePosition(&mousestate[2],
															&mousestate[3]);
						else if(ctrled)
							event->GetMousePosition(&mousestate[8],
															&mousestate[5]);
						else if(leftright || shifted)
							event->GetMousePosition(&mousestate[4],
															&mousestate[5]);
						else
							event->GetMousePosition(&mousestate[4],
															&mousestate[9]);
						}
					else if(event->IsMouseMotion())
						{
						long offset=0;
						if(buttons&EW_MOUSEBUTTONS_MIDDLE)
							offset=2;
						else if(buttons&EW_MOUSEBUTTONS_RIGHT)
							offset=(ctrled)? 8: 4;
						else if(!(buttons&EW_MOUSEBUTTONS_LEFT))
							return;

						dfmouse[offset]=(float)(mx-mousestate[offset]);
						dfmouse[offset+1]= -(float)(my-mousestate[offset+1]);
						mousestate[offset]=mx;
						mousestate[offset+1]=my;
						}
					else if(event->IsIdleMouse() && buttons)
						{
						long offset=0;
						if(buttons&EW_MOUSEBUTTONS_MIDDLE)
							offset=2;
						else if(buttons&EW_MOUSEBUTTONS_RIGHT)
							offset=4;
						else if(!(buttons&EW_MOUSEBUTTONS_LEFT))
							return;

						dfmouse[offset]=0.0f;
						dfmouse[offset+1]=0.0f;
						}

					IdleFunction();
					SetDirty(EW_DIRTY_COMPLETE);

					UpdateManager();
					};

		void	UpdateManager(void)
					{
					bonesmanager->SetInteger(
							GPGBonesManagerInterface::SelectBone,
							selectbone? selectbone->GetBoneIndex(): -1);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowGrid,
									keycount[KEY_ORIGIN]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowWorldAxes,
									keycount[KEY_ORIGIN]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowBones,
									keycount[KEY_BONES]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowImplants,
									SHOW_IMPLANTS&&keycount[KEY_BONES]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowAxes,
									keycount[KEY_AXES]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowBoneNames,
									keycount[KEY_BONENAMES]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowJoints,
									keycount[KEY_JOINTS]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowCylinders,
									keycount[KEY_CYLINDERS]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowConstraints,
									keycount[KEY_CONSTRAINTS]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowBoneLinks,
									keycount[KEY_BONELINKS]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowWeights,
									keycount[KEY_WEIGHTS]==1);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowAllWeights,
									keycount[KEY_WEIGHTS]==2);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowNormals,
									keycount[KEY_NORMALS]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowVertices,
									keycount[KEY_VERTNUMBERS]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ReferenceLock,
									keycount[KEY_INPUTMESH]==2);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::ShowEffectors,
									true);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::Renormalize,
									keycount[KEY_RENORMALIZE]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::RootLock,
									keycount[KEY_ROOTLOCK]!=0);
					bonesmanager->SetBool(
							GPGBonesManagerInterface::AntiAliasLine,
									keycount[KEY_ANTIALIAS]!=0);
					bonesmanager->SetInteger(
							GPGBonesManagerInterface::VertexCycle,
									keycount[KEY_VERTEXCYCLE]);
					bonesmanager->SetInteger(
							GPGBonesManagerInterface::VertexCycles,
									VERTEX_CYCLES);
					bonesmanager->SetInteger(
							GPGBonesManagerInterface::ShowSingleMesh,
									keycount[KEY_PARTIALMESH]);
					bonesmanager->SetFloat(GPGBonesManagerInterface::Time,
									fmouse[4]);
					};

		void	DrawMessage(float dy,const char *buffer)
					{
					GetRootWindow()->Text(0,0,(char*)buffer,strlen(buffer),
															FALSE,EW_WHITE);
					glTranslatef(0.0f,dy,0.0f);
					};

virtual void	Draw2D(void)
					{
					SetupOpenGL(FALSE,POINTCLOUD);

					float fps_excl;
					float fps_incl;
					long sx,sy;
					GetSize(&sx,&sy);

					float x=5.0f;
					float y=sy-20.0f;
					float dy=-15.0f;

					char buffer[64];

					GetFPS(&fps_excl,&fps_incl);

#if FIXED_FRAMERATE
					static F32 error=0.0f;
					static F32 currentDelay=0.0f;
					static F32 lasttick=0.0f;
					F32 tick=GetTickTimer();
					F32 used=tick-lasttick;
					if(used>0.0f)
					{
						F32 delay=1e3f/TARGET_FRAMERATE-used-error;
						currentDelay=(delay>0.0f)? delay: 0.0f;
					}

//					GPGLOG("error=%.6G used=%.6G currentDelay=%.6G\n",
//							error,used,currentDelay);
					if(currentDelay>0.0f)
					{
//						usleep((U32)(1e3f*currentDelay));
						osdMilliSleep((U32)(currentDelay));

						F32 real=GetTickTimer()-tick;
						if(real>0.0f)
							error=real-currentDelay;
					}
					else
						error=0.0f;

					lasttick=GetTickTimer();
#endif

					glColor3f(1.0f,1.0f,1.0f);
					glPushMatrix();
					glTranslatef(x,y,0.0f);

					sprintf(buffer,"FPS %.1f %.1f",fps_excl,fps_incl);
					DrawMessage(dy,buffer);

					if(keycount[KEY_HELP])
						{
						glTranslatef(0.0f,dy,0.0f);

						DrawMessage(dy,"Mouse Buttons:");

						sprintf(buffer,"Left=Angle %.3G %.3G",
														fmouse[0],fmouse[1]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"Mid=Pan %.3G %.3G",
														fmouse[2],fmouse[3]);
						DrawMessage(dy,buffer);
						DrawMessage(dy," or shift-Left");

						sprintf(buffer,"Wheel=Zoom %.3G",fmouse[6]);
						DrawMessage(dy,buffer);
						DrawMessage(dy," or ctrl-Left vertical");

						sprintf(buffer,"Right=Time %.3G",fmouse[4]);
						DrawMessage(dy,buffer);
						DrawMessage(dy," (horizontal)");

						sprintf(buffer,"shift-Right=Trans %.3G %.3G",
														fmouse[7],fmouse[5]);
						DrawMessage(dy,buffer);
						DrawMessage(dy," or Left and Right");

						sprintf(buffer,"ctrl-Right=Aux %.3G %.3G",
														fmouse[8],fmouse[9]);
						DrawMessage(dy,buffer);

						glTranslatef(0.0f,dy,0.0f);

						DrawMessage(dy,"Keys:");

						sprintf(buffer,"%c Axes %ld",KEY_AXES,
													keycount[KEY_AXES]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Bones %ld",KEY_BONES,
													keycount[KEY_BONES]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Culling %ld",KEY_CULLING,
													keycount[KEY_CULLING]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Dual Color %ld",KEY_DUAL_COLOR,
													keycount[KEY_DUAL_COLOR]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Renormalize %ld",KEY_RENORMALIZE,
													keycount[KEY_RENORMALIZE]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Help %ld",KEY_HELP,
													keycount[KEY_HELP]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Input Mesh %ld",KEY_INPUTMESH,
													keycount[KEY_INPUTMESH]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Joints %ld",KEY_JOINTS,
													keycount[KEY_JOINTS]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Constraints %ld",KEY_CONSTRAINTS,
													keycount[KEY_CONSTRAINTS]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Lines (debug) %ld",KEY_LINES,
													keycount[KEY_LINES]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Normals %ld",KEY_NORMALS,
													keycount[KEY_NORMALS]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c BoneNames %ld",KEY_BONENAMES,
													keycount[KEY_BONENAMES]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Origin %ld",KEY_ORIGIN,
													keycount[KEY_ORIGIN]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Partial Mesh %ld",KEY_PARTIALMESH,
													keycount[KEY_PARTIALMESH]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Rendering %ld",KEY_RENDERING,
													keycount[KEY_RENDERING]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Skin %ld",KEY_SKIN,
													keycount[KEY_SKIN]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Texture %ld",KEY_TEXTURE,
													keycount[KEY_TEXTURE]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c VertNumbers %ld",KEY_VERTNUMBERS,
													keycount[KEY_VERTNUMBERS]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c BoneLinks %ld",KEY_BONELINKS,
													keycount[KEY_BONELINKS]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Weights %ld",KEY_WEIGHTS,
													keycount[KEY_WEIGHTS]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Bound Cylinders %ld",KEY_CYLINDERS,
													keycount[KEY_CYLINDERS]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Cycle Labels %ld",KEY_VERTEXCYCLE,
													keycount[KEY_VERTEXCYCLE]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Root Lock %ld",KEY_ROOTLOCK,
													keycount[KEY_ROOTLOCK]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c AntiAlias %ld",KEY_ANTIALIAS,
													keycount[KEY_ANTIALIAS]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Test Flag %ld",KEY_TESTFLAG,
													keycount[KEY_TESTFLAG]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Special Event %ld",KEY_SPECIALEVENT,
													keycount[KEY_SPECIALEVENT]);
						DrawMessage(dy,buffer);

						sprintf(buffer,"%c Timers %ld",KEY_TIMERS,
													keycount[KEY_TIMERS]);
						DrawMessage(dy,buffer);

						glTranslatef(0.0f,dy,0.0f);

						if(creature)
							{
							sprintf(buffer,"Bones: %ld",
									creature->GetBoneTableSize());
							DrawMessage(dy,buffer);

							GPGSkin *pSkin=creature->GetSkin();
							GPGMeshInterface *inmesh=
									pSkin? pSkin->GetInputMesh(): NULL;

							if(selectbone)
								{
								sprintf(buffer,"Bone: %d '%s'",
												selectbone->GetBoneIndex(),
												selectbone->NameConst().Raw8());
								DrawMessage(dy,buffer);

								sprintf(buffer," len: %.6G",
												selectbone->Length());
								DrawMessage(dy,buffer);

								sprintf(buffer," disp: %s",
									selectbone->Displacement().Out().Raw8());
								DrawMessage(dy,buffer);

								sprintf(buffer," rot: %s",
										selectbone->Rotation().Out().Raw8());
								DrawMessage(dy,buffer);

								sprintf(buffer," ref disp: %s",
									selectbone->ReferenceDisplacement()
												.Out().Raw8());
								DrawMessage(dy,buffer);

								sprintf(buffer," ref rot: %s",
										selectbone->LocalReferenceRotation()
												.Out().Raw8());
								DrawMessage(dy,buffer);
								}
							else
								{
								sprintf(buffer,"Bone: none selected");
								DrawMessage(dy,buffer);
								}

							long verts=0,norms=0,faces=0;
							if(!inmesh)
								DrawMessage(dy,"No Mesh");
							else
								{
								DrawMessage(dy,"Verts/Norms/Faces");

								long meshid,meshes=inmesh->GetNumberMeshes();
								for(meshid=0;meshid<meshes;meshid++)
									{
									inmesh->ChooseMeshIndex(meshid);

									sprintf(buffer,"%ld %d %d %d %s",meshid,
											inmesh->GetNumberVertices(),
											inmesh->GetNumberNormals(),
											inmesh->GetNumberFaces(),
											inmesh->GetMeshName().Raw());
									verts+=inmesh->GetNumberVertices();
									norms+=inmesh->GetNumberNormals();
									faces+=inmesh->GetNumberFaces();
									DrawMessage(dy,buffer);
									}
								if(meshes>1)
									{
									sprintf(buffer,"TOTAL %ld %ld %ld",
															verts,norms,faces);
									DrawMessage(dy,buffer);
									}
								}

							glTranslatef(0.0f,dy,0.0f);

							if(mesh_filename[0])
								DrawMessage(dy,mesh_filename);
							else
								DrawMessage(dy,"NO MESH");

							if(bone_filename[0])
								DrawMessage(dy,bone_filename);
							else
								DrawMessage(dy,"NO BONES");

							if(motion_filename[0])
								DrawMessage(dy,motion_filename);
							else
								DrawMessage(dy,"NO MOTION");
							}
						}
					else
						DrawMessage(dy,"h for help");

					if(keycount[KEY_TIMERS])
						{
						glTranslatef(0.0f,dy,0.0f);

						long m;
						long timeditems=GPGBonesManagerInterface::NumTimedItems;
						for(m=0;m<timeditems;m++)
							{
							float ftimer;
							bonesmanager->GetTimedPeriod(
									(GPGBonesManagerInterface::TimedItem)m,
									&ftimer);
							sprintf(buffer,"%s:  %.2f",
									GPGBonesManagerTimedItemName[m],
									ftimer);
							DrawMessage(dy,buffer);
							}

						glTranslatef(0.0f,dy,0.0f);

						sprintf(buffer,"Clock:  %.0fMHz",1e-3/tickscale);
						DrawMessage(dy,buffer);
						}

					glPopMatrix();
					};

		void	SetFontBase(GLint fontBase)
				{	inforenderOGL.SetFontBase(fontBase); }

	private:
		char				mesh_filename[FILENAME_LENGTH];
		char				bone_filename[FILENAME_LENGTH];
		char				motion_filename[FILENAME_LENGTH];

		GLubyte				texture[64][64][3];
		long				keycount[GPGKEY_USED];
		long				keymax[GPGKEY_USED];
		long				mousestate[MOUSES];
		GLfloat				fmouse[MOUSES],dfmouse[MOUSES];
		GLfloat				bend,inc;
		float				viewerscale;

		GPGBoneNode			*selectbone;

		GPGCharacter		*creature;
		GPGMotion			movement;

		GPGMeshFactory				*meshfactory;
		GPGBonesManagerInterface	*bonesmanager;
		GPGMotionManagerInterface	*motionmanager;
		GPGMotionMixerInterface		*motionmixer;
		GPGMotionMixerInterface		*pastmixer;
		long						ikbase,ikend;
	};


int main(int argc,char **argv)
{
	GPGTypeMaster *pTypeMaster=new GPGTypeMaster;

	GPGASSERT(pTypeMaster);
	GPGRegisterNative(pTypeMaster);
	GPL_INIT_CLASSNAME(GPL) *m_GPL_INIT=new GPL_INIT_CLASSNAME(GPL);
	GPL_INIT_CLASSNAME(WBA) *m_WBA_INIT=new GPL_INIT_CLASSNAME(WBA);
	GPL_INIT_CLASSNAME(EW)  *m_EW_INIT=new GPL_INIT_CLASSNAME(EW);
	GPL_INIT_CLASSNAME(WDS) *m_WDS_INIT=new GPL_INIT_CLASSNAME(WDS);

	GPL_Init::cnt=0;
	WBA_Init::cnt=0;
	EW_Init::cnt=0;
	WDS_Init::cnt=0;

	// terse logging
	char logLevel[8]="ttt";
	wbaInstallModuleDeliveries("WDS",logLevel);
	wbaInstallModuleDeliveries("EW",logLevel);
	wbaInstallModuleDeliveries("WBA",logLevel);
	wbaInstallModuleDeliveries("GPL",logLevel);

	ewColorCodeMode=1;
	EW_Window::SetDefaultRGBMode(TRUE);

	EW_EventContext *pEventContext=new EW_EventContext();
	EW_Window *pViewerWindow=new EW_Window();
	WDS_Scope *pViewerScope=new WDS_Scope();
	BonesTestbed *pBonesTestbed=new BonesTestbed();

	pBonesTestbed->Initialize();
	pBonesTestbed->ScaleViewer();

	pEventContext->AddWindow(pViewerWindow);
	pViewerWindow->SetGeometry(100,100,800,700);
	pViewerWindow->SetDoubleBuffer(TRUE);
	pViewerWindow->SetDoWork(TRUE);
	pViewerWindow->Open("Bones Testbed");

	EW_Font fontData;
	EW_FontRequest request;
	request.SetHeight(12);
	request.SetStyle(EW_FONT_PROPORTIONAL);
	if( !pViewerWindow->LoadFont(&fontData,&request) )
	{
		pViewerWindow->SetFont(&fontData);
		pViewerWindow->CopyDefaultFontFrom(&fontData);
	}

	pBonesTestbed->SetFontBase(fontData.fonthandle->FontBase);

	pViewerWindow->AddWidget(pViewerScope);
	pViewerScope->SetBounds(EW_BOUNDS_FILL_PARENT);
	pViewerScope->AddWidget(pBonesTestbed);
	pBonesTestbed->GetLocalLook()->SetElement(WDS_STATE_ALL,WDS_ELEMENT_BORDER,
													WDS_BEVELBOX_BORDERLESS);
	pBonesTestbed->SetBounds(EW_BOUNDS_FILL_PARENT);

	pEventContext->BeginLoop();

	delete pEventContext;

	delete m_GPL_INIT;
	delete m_WBA_INIT;
	delete m_EW_INIT;
	delete m_WDS_INIT;

	delete pTypeMaster;

	GPGLOG("\n> Terminating\n");
	return 0;
}


float GetTickTimer(void)
	{
	U32 time=ifxGetSystemTick();
	return time*tickscale;
	}


void StateProgress(const char *text)
	{
//	if(globalWindow)
//		globalWindow->DrawBanner(text);
	}


/******************************************************************************
void DrawLine(long color,const GPGVector3 &first,const GPGVector3 &second)

	since ICA proper can't use graphics,
	this is provided to give visual feedback

******************************************************************************/
void DrawLine(long color,const GPGVector3 &first,const GPGVector3 &second)
	{
	if(!show_lines)
		return;

	F32 colors[]={
			1.0f,0.0f,1.0f,
			0.0f,1.0f,1.0f,

			0.0f,0.0f,1.0f,
			1.0f,1.0f,0.0f,

			0.0f,1.0f,0.0f,
			1.0f,0.0f,0.0f,

			0.2f,0.2f,0.2f,
			1.0f,1.0f,1.0f
			};
	GPGVector3 vertex[2];
	vertex[0]=first;
	vertex[1]=second;
	inforenderOGL.DrawLineArray(vertex,2,false,true,
							(GPGVector3 *)&colors[(color&3)*6],1.0f,antialias);
	}
