/* Copyright (C) Gabor Nagy, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Gabor Nagy, 2000"
 */
/*==============================================================================*/
/* Real-time cast shadows							*/
/*										*/
/* - The Main									*/
/*										*/
/* AUTHOR:	Gabor Nagy							*/
/* DATE:	2000-May-02 14:28:33						*/
/*  v1.02									*/
/*										*/
/* For Game Programming Graphics Gems						*/
/*==============================================================================*/
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <time.h>
#else
#include <sys/time.h>
#include <sys/param.h>
#endif

#include "str.h"
#include "obj.h"


E3dModel*	Scn_BulbModel=NULL;
E3dModel*	Scn_BlockerModel=NULL;
E3dModel*	Scn_ReceiverModel0=NULL;
E3dModel*	Scn_ReceiverModel1=NULL;

E3dLight	Sh_Light;

enum
{
 ShDOLLY_CAMERA=0, ShMOVE_LIGHT
};


int	Sh_MiddleMouseMode=ShDOLLY_CAMERA;

int	Sh_DisplayMode=0;

#ifdef _WIN32
float			Sh_StartTime;
#else
unsigned long	Sh_StartTime;
#endif

unsigned long	Sh_NumOfFrames=200;
unsigned long	Sh_Frame=0;

// Shadow map
//
unsigned long*	Sh_ShadowMapImage=NULL;
int		Sh_ShadowMapXSize=128, Sh_ShadowMapYSize=128;


int		Sh_TexXSize, Sh_TexYSize;
GLubyte*	Sh_TexImage;
GLuint		Sh_ShadowMapTexId, Sh_WoodTexId;

EBool		Sh_BackFaceShadowElimination=TRUE;

// Viewing
//
float	Sh_Longitude=152.0, Sh_Latitude=-36.0;
float	Sh_ViewDistance = 20.0;
float	Sh_ZNear=0.1, Sh_ZFar=8192.0;
float	Sh_WindowAspect = 4.0/3.0;
long	Sh_WindowXSize, Sh_WindowYSize;
int	Sh_PrevMX, Sh_PrevMY;
EBool	Sh_LeftButton = FALSE, Sh_MiddleButton = FALSE;

int	Sh_MainMenu, Sh_DisplayMenu, Sh_BlockerObjectMenu, Sh_ShadowMapMenu, Sh_BackFaceShadowMenu;

int	Sh_CurrentBlockerObject=-1;

/* From shadow.c */
extern void E3d_ShadowMatrix(E3dLight* LLight, E3dModel *LModel, E3dMatrix LBlockerMatrix, E3dMatrix LReceiverUVMatrix);
extern void Sh_CreateShadowMap(GLuint LShadowMapTexId, int LMapXSize, int LMapYSize, E3dMesh *LBlockerMesh, E3dMatrix LBlockerMatrix);

/*======================================*/
/* Setlect blocker object		*/
/*======================================*/
void ShCB_SetlectBlockerObject(int LValue)
{
 E3dMesh*	LMesh;
 E3dPolyGroup*	LPolyGroup;
 E3dMaterial*	LMaterial;

 if(Sh_CurrentBlockerObject!=LValue)
 {
   switch(LValue) 
   {
     case 0: 
       if(Scn_BlockerModel!=NULL) E3d_ModelFree(Scn_BlockerModel);
       if((Scn_BlockerModel=E3d_ReadOBJFile("objects/torus.obj")) != NULL)
       {
	 Scn_BlockerModel->Translation.Y=5.0;
       }
     break;

     case 1: 
       if(Scn_BlockerModel!=NULL) E3d_ModelFree(Scn_BlockerModel);
       if((Scn_BlockerModel=E3d_ReadOBJFile("objects/HGlassShadow.obj")) != NULL)
       {
	 Scn_BlockerModel->Translation.Y=5.0;
       }
     break;
   }
   Sh_CurrentBlockerObject=LValue;
 }

 if(Scn_BlockerModel!=NULL)
 {
  LMesh=(E3dMesh*)(Scn_BlockerModel->Geometry);

  LPolyGroup=LMesh->PolyGroups[0];

  LPolyGroup->Material=LMaterial=E3d_MaterialAllocate();
  LMaterial->Diffuse[0]=0.0;
  LMaterial->Diffuse[1]=0.5;
  LMaterial->Diffuse[2]=0.9;
 }

 glutPostRedisplay();
}


/*======================================*/
/* Set shadow-map size			*/
/*======================================*/
void ShCB_SetShadowMapSize(int LValue)
{
 if(Sh_ShadowMapXSize!=LValue)
 {
  if(Sh_ShadowMapImage!=NULL) free(Sh_ShadowMapImage);

  Sh_ShadowMapImage=(unsigned long*)malloc(LValue*LValue*sizeof(unsigned long));
 }

 Sh_ShadowMapXSize=LValue;
 Sh_ShadowMapYSize=LValue;

 glutPostRedisplay();
}


/*======================================*/
/* Back-face shadow elimination On/Off	*/
/*======================================*/
void ShCB_BackFaceShadow(int LValue)
{
 Sh_BackFaceShadowElimination=LValue;

 glutPostRedisplay();
}


/*======================================*/
/* Main menu callback			*/
/*======================================*/
void ShCB_MainMenu(int value)
{
  switch(value) 
  {
    case 5:	exit(0);   break;
  }
}


/*==============================*/
/* Glut idle function		*/
/*==============================*/
void Sh_Run(void)
{
 if(Scn_BlockerModel!=NULL)
 {
  Scn_BlockerModel->Rotation.X+=1.0;
  Scn_BlockerModel->Rotation.Y+=2.0;
  Scn_BlockerModel->Rotation.Z+=4.0;
  glutPostRedisplay();
 }
}


// Texture "border color" for clamping
// (same as the shadow-map background color: white)
//
float	Sh_TxBorder[4]= { 1.0, 1.0, 1.0, 1.0};

/*==============================*/
/* Read raw RGBA file		*/
/*==============================*/
char* E3d_LoadImage(char* LFileName)
{
 FILE*		LInFile;
 char*		LBuffer=NULL;
 unsigned long	LFileSize;

 if((LInFile=fopen(LFileName,"r"))!=NULL)
 {

// Determine file length
//
  fseek(LInFile, 0, SEEK_END);

  LFileSize=ftell(LInFile);

  fseek(LInFile, 0, SEEK_SET);

// Allocate buffer and read file
//
  if((LBuffer=EMalloc(LFileSize))!=NULL)
  {
   fread(LBuffer, 1, LFileSize, LInFile);
  }

  fclose(LInFile);
 }

 return(LBuffer);
}


/*==============================*/
/* Initialize textures		*/
/*==============================*/
void Sh_InitTextures(void)
{
 int		LTexXSize, LTexYSize;
 GLubyte*	LTexData;

// Read and set up wood texture
//
 glGenTextures(1,&Sh_WoodTexId);
 glBindTexture(GL_TEXTURE_2D, Sh_WoodTexId);

 if((LTexData=(GLubyte*)E3d_LoadImage("Wood.rgba"))!=NULL)
 {
  LTexXSize=128;LTexYSize=128;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, LTexXSize, LTexYSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, LTexData );
 }

// Initialize shadow map
//
 glGenTextures(1,&Sh_ShadowMapTexId);
 glBindTexture(GL_TEXTURE_2D, Sh_ShadowMapTexId);

 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
 glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, Sh_TxBorder);

 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}


/*==============================*/
/* Render the scene		*/
/*==============================*/
void Sh_DrawScene()
{
 E3dModel*	LModel;
 E3dMesh*	LMesh;
 E3dPolyGroup*	LPolyGroup;
 E3dMaterial*	LMaterial;

 glDisable(GL_LIGHTING);

// Draw bulb model to show where the light source is
//
 if(Scn_BulbModel!=NULL)
 {
  glPushMatrix();

  Scn_BulbModel->Translation.X=Sh_Light.Position.X;
  Scn_BulbModel->Translation.Y=Sh_Light.Position.Y;
  Scn_BulbModel->Translation.Z=Sh_Light.Position.Z;
  E3d_ModelRefreshMatrices(Scn_BulbModel);

  E3d_DrawModel(Scn_BulbModel, FALSE, NULL, FALSE);
  glPopMatrix();
 }


 glEnable(GL_LIGHTING);
 glDisable(GL_TEXTURE_2D);

// Render blocker model
//
 if(Scn_BlockerModel!=NULL)
 {
  glPushMatrix();
  E3d_ModelRefreshMatrices(Scn_BlockerModel);

  E3d_DrawModel(Scn_BlockerModel, FALSE, NULL, FALSE);
  glPopMatrix();
 }

// Render receiver objects
//
 if((LModel=Scn_ReceiverModel0)!=NULL)
 {
  glPushMatrix();

  glBindTexture(GL_TEXTURE_2D, Sh_ShadowMapTexId);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_TEXTURE_2D);

  E3d_DrawModel(LModel, FALSE, &Sh_Light, Sh_BackFaceShadowElimination);
  glPopMatrix();
 }


// Draw table
//
 if((LModel=Scn_ReceiverModel1)!=NULL)
 {
  glPushMatrix();

  if((LMesh=(E3dMesh*)(LModel->Geometry))!=NULL)
  {
   LPolyGroup=LMesh->PolyGroups[0];
   LMaterial=LPolyGroup->Material;

// We will use lights in this pass
//
   LMaterial->Type=E3dMAT_PHONG;

// Set up wood texture
//
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_TEXTURE_2D);

   glBindTexture(GL_TEXTURE_2D, Sh_WoodTexId);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
   glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T);

// Just lighting and texture in this pass
//
   LPolyGroup->ShadowPass=FALSE;
   E3d_DrawModel(LModel, TRUE, &Sh_Light, Sh_BackFaceShadowElimination);

   glPopMatrix();


// Draw shadow pass of table
//
   glPushMatrix();

// No light calculations in this pass
//
   LMaterial->Type=E3dMAT_CONSTANT;

   glEnable(GL_BLEND);
   glDisable(GL_LIGHTING);

// We will use multiplicative blending here, so we can avoid
// creating a separate white on black shadow-map
// Result=SourceRGB*DestinationRGB
//
   glBlendFunc(GL_DST_COLOR, GL_ZERO);

   glBindTexture(GL_TEXTURE_2D, Sh_ShadowMapTexId);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glEnable(GL_TEXTURE_2D);

// This is the shadow-pass
//
   LPolyGroup->ShadowPass=TRUE;
   E3d_DrawModel(LModel, FALSE, &Sh_Light, Sh_BackFaceShadowElimination);

   glEnable(GL_LIGHTING);

   glDisable(GL_BLEND);
   glPopMatrix();
  }

 }


 glDisable(GL_TEXTURE_2D);

 glPopMatrix();
}


/*==============================================*/
/* GLUT window-resize callback function		*/
/*==============================================*/
void Sh_Resize(int LXSize, int LYSize)
{
 Sh_WindowXSize = LXSize; 
 Sh_WindowYSize = LYSize;
 Sh_WindowAspect = (float)Sh_WindowXSize/(float)Sh_WindowYSize;
 glViewport(0, 0, Sh_WindowXSize, Sh_WindowYSize);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glutPostRedisplay();
}


/*======================================*/
/* GLUT main Display function		*/
/*======================================*/
void Sh_Display(void) 
{
 E3dMatrix		LBlockerMatrix,LReceiverUVMatrix;
#ifdef _WIN32
 float newestTime;
#else
 struct timeval		LTV;
 struct timezone	LTZ;
#endif

 E3d_GLUpdateLight(0, &Sh_Light);

#ifdef _WIN32
// For timing
//
 Sh_Frame+=1;

 newestTime = (float)timeGetTime()/1000.0f;
 if((newestTime-Sh_StartTime)>=10.0f)
 {

  printf("FrameRate: %3.2f fps\n",(float)Sh_Frame/(float)(newestTime-Sh_StartTime));fflush(stdout);

  Sh_Frame=0;
  Sh_StartTime=newestTime;
 }

#else
// For timing
//
 Sh_Frame+=1;

 if(gettimeofday(&LTV,&LTZ)!=-1)
 {
  if((LTV.tv_sec-Sh_StartTime)>=10)
  {

   printf("FrameRate: %3.2f fps\n",(float)Sh_Frame/(float)(LTV.tv_sec-Sh_StartTime));fflush(stdout);

   Sh_Frame=0;
   Sh_StartTime=LTV.tv_sec;
  }
 }
#endif

 glViewport(0, 0, Sh_WindowXSize, Sh_WindowYSize);
 
 glClearColor(0.3, 0.0, 0.0, 0.0);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 if(Scn_BlockerModel!=NULL)
 {
  E3d_ShadowMatrix(&Sh_Light,Scn_BlockerModel,LBlockerMatrix,LReceiverUVMatrix);

  Sh_CreateShadowMap(Sh_ShadowMapTexId,Sh_ShadowMapXSize, Sh_ShadowMapYSize, (E3dMesh*)(Scn_BlockerModel->Geometry), LBlockerMatrix);

// Set the ShadowMapUV matrices of the receiver objects
//
  if(Scn_ReceiverModel0!=NULL)
  {
   E3d_MatrixCopy(LReceiverUVMatrix,Scn_ReceiverModel0->ShadowMapUVMatrix);
  }

  if(Scn_ReceiverModel1!=NULL)
  {
   E3d_MatrixCopy(LReceiverUVMatrix,Scn_ReceiverModel1->ShadowMapUVMatrix);
  }
 }

// The shadow map generator has set it's own viewport,
// so we have to (re)configure it here
//
  glViewport(0, 0, Sh_WindowXSize, Sh_WindowYSize);


// We must clear the BACK buffer again before starting to render the scene
//
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(64.0, Sh_WindowAspect, Sh_ZNear, Sh_ZFar);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); 

// Camera transformation
//
  glTranslatef(0.0,0.0,-Sh_ViewDistance);
  glRotatef(-Sh_Latitude, 1.0, 0.0, 0.0);
  glRotatef(Sh_Longitude, 0.0, 1.0, 0.0);

  E3d_GLUpdateLight(0, &Sh_Light);


  Sh_DrawScene();

  glutSwapBuffers();
}


/*======================================*/
/* Mouse motion callback handler	*/
/*======================================*/
void ShCB_MouseMotion(int LX, int LY)
{
  if (Sh_LeftButton)
  {
    Sh_Longitude -= (float)(Sh_PrevMX-LX) * 0.4;
    Sh_Latitude += (float)(Sh_PrevMY-LY) * 0.4;
  }

  if (Sh_MiddleButton)
  {
   switch(Sh_MiddleMouseMode)
   {
    case ShDOLLY_CAMERA:
     Sh_ViewDistance += (float)(Sh_PrevMY - LY) * 0.2;
    break;

    case ShMOVE_LIGHT:
     Sh_Light.Position.X -= (float)(Sh_PrevMX - LX)*0.2;
     if(Sh_Light.Position.X<-23.0) Sh_Light.Position.X=-23.0;
     else if(Sh_Light.Position.X>25.0) Sh_Light.Position.X=25.0;

     Sh_Light.Position.Z -= (float)(Sh_PrevMY - LY)*0.2;
     if(Sh_Light.Position.Z<-25.0) Sh_Light.Position.Z=-25.0;
     else if(Sh_Light.Position.Z>25.0) Sh_Light.Position.Z=25.0;

    break;
   }
  }
  Sh_PrevMX = LX;Sh_PrevMY = LY;
  glutPostRedisplay();
}


/*======================================*/
/* Mouse button callback handler	*/
/*======================================*/
void ShCB_MouseButtons(int LButton, int LState, int LX, int LY)
{
  Sh_PrevMX = LX;
  Sh_PrevMY = LY;
  Sh_LeftButton = ((LButton == GLUT_LEFT_BUTTON) && (LState == GLUT_DOWN));
  Sh_MiddleButton = ((LButton == GLUT_MIDDLE_BUTTON) && (LState == GLUT_DOWN));
}


/*======================================*/
/* Keyboard callback handler		*/
/*======================================*/
void ShCB_Keyboard(unsigned char LCh, int LX, int LY)
{
  switch (LCh) 
  {
    case 'c':	Sh_MiddleMouseMode=ShDOLLY_CAMERA; break;
    case 'l':	Sh_MiddleMouseMode=ShMOVE_LIGHT; break;
    case 27:	exit(0); break;	// "Esc"
  }
  glutPostRedisplay();
}


/*======================================*/
/* The main...				*/
/*======================================*/
void main(int LArgC, char** LArgV)
{
 E3dMaterial*	LMaterial;
 E3dMesh*	LMesh;
 E3dPolyGroup*	LPolyGroup;

 glutInit(&LArgC, LArgV);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
 glutInitWindowSize(420, 320);
 glutCreateWindow("Shadows");

 glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);

 E3d_MaterialDefault(&E3d_DefaultMaterial);

// Read light-bulb geometry
//
 if((Scn_BulbModel=E3d_ReadOBJFile("objects/bulb.obj")) != NULL)
 {
  if((LMesh=(E3dMesh*)(Scn_BulbModel->Geometry))!=NULL)
  {
// Glass part
//
   LPolyGroup=LMesh->PolyGroups[0];
   LPolyGroup->Material=LMaterial=E3d_MaterialAllocate();
   LMaterial->Type=E3dMAT_CONSTANT;
   LMaterial->Diffuse[0]=1.0;
   LMaterial->Diffuse[1]=1.0;
   LMaterial->Diffuse[2]=1.0;

// Copper part
//
   LPolyGroup=LMesh->PolyGroups[1];
   LPolyGroup->Material=LMaterial=E3d_MaterialAllocate();
   LMaterial->Type=E3dMAT_CONSTANT;
   LMaterial->Diffuse[0]=0.5;
   LMaterial->Diffuse[1]=0.3;
   LMaterial->Diffuse[2]=0.1;
  }
 }



// Read a blocker object
//
 ShCB_SetlectBlockerObject(0);

// Initialize cloth model
//
 if((Scn_ReceiverModel0=E3d_ReadOBJFile("objects/cloth.obj")) != NULL)
 {
  Scn_ReceiverModel0->Translation.Y=-5.0;
  Scn_ReceiverModel0->Translation.Z=-3.0;

  if((LMesh=(E3dMesh*)(Scn_ReceiverModel0->Geometry))!=NULL)
  {
   LPolyGroup=LMesh->PolyGroups[0];

   LPolyGroup->ShadowPass=TRUE;

   LPolyGroup->Material=LMaterial=E3d_MaterialAllocate();
   LMaterial->Diffuse[0]=0.7;
   LMaterial->Diffuse[1]=0.3;
   LMaterial->Diffuse[2]=0.1;

// This object won't move, so we won't recompute it's local->world matrix at each frame
//
   E3d_ModelRefreshMatrices(Scn_ReceiverModel0);
  }
 }

// Initialize table model
//
 if((Scn_ReceiverModel1=E3d_ReadOBJFile("objects/table.obj")) != NULL)
 {
  Scn_ReceiverModel1->Translation.Y=0.0;

  if((LMesh=(E3dMesh*)(Scn_ReceiverModel1->Geometry))!=NULL)
  {
   LPolyGroup=LMesh->PolyGroups[0];

// Make material white so the wood texture will blend properly
//
   LPolyGroup->Material=LMaterial=E3d_MaterialAllocate();
   LMaterial->Diffuse[0]=1.6;
   LMaterial->Diffuse[1]=1.6;
   LMaterial->Diffuse[2]=1.6;

// This object won't move, so we won't recompute it's local->world matrix at each frame
//
   E3d_ModelRefreshMatrices(Scn_ReceiverModel1);
  }
 }


// Initialize our light source
//
 E3d_LightDefault(&Sh_Light);
 Sh_Light.Position.X=-12.0;
 Sh_Light.Position.Y=16.0;
 Sh_Light.Position.Z=7.5;

// Allocate memory for the shadow map
//
 Sh_ShadowMapImage=(unsigned long*)malloc(Sh_ShadowMapXSize*Sh_ShadowMapYSize*sizeof(unsigned long));

 glEnable(GL_DEPTH_TEST);
 glDepthFunc(GL_LEQUAL);
 glPolygonOffset(1.0, 1.0);
 glEnable(GL_CULL_FACE);
 glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
 glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
 glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
// glEnable(GL_COLOR_MATERIAL);
// glColorMaterial(GL_FRONT, GL_DIFFUSE);

 E3d_GLUpdateLight(0, &Sh_Light);
 glEnable(GL_LIGHT0);
 Sh_InitTextures();

 glutIdleFunc(Sh_Run);

 glutReshapeFunc(Sh_Resize);
 glutDisplayFunc(Sh_Display);

 glutKeyboardFunc(ShCB_Keyboard);
 glutMouseFunc(ShCB_MouseButtons);
 glutMotionFunc(ShCB_MouseMotion);

// Select blocker object
//
 Sh_BlockerObjectMenu = glutCreateMenu(ShCB_SetlectBlockerObject);
 glutAddMenuEntry("Torus", 0);
 glutAddMenuEntry("Hourglass", 1);


// Shadow-map size selection
//
 Sh_ShadowMapMenu = glutCreateMenu(ShCB_SetShadowMapSize);
 glutAddMenuEntry("32x32", 32);
 glutAddMenuEntry("64x64", 64);
 glutAddMenuEntry("128x128", 128);
 glutAddMenuEntry("256x256", 256);

// Back-face shadow ON/OFF
//
 Sh_BackFaceShadowMenu = glutCreateMenu(ShCB_BackFaceShadow);
 glutAddMenuEntry("OFF", 0);
 glutAddMenuEntry("ON", 1);

 Sh_MainMenu = glutCreateMenu(ShCB_MainMenu);
 glutAddSubMenu("Blocker object", Sh_BlockerObjectMenu);
 glutAddSubMenu("Back-face shadow elimination", Sh_BackFaceShadowMenu);
 glutAddSubMenu("Shadow-map size", Sh_ShadowMapMenu);
 glutAddMenuEntry("Exit", 5);
 glutAttachMenu(GLUT_RIGHT_BUTTON);

#ifdef _WIN32
 Sh_StartTime = (float)timeGetTime()/1000.0f;
#else
 {
  struct timeval	LTV;
  struct timezone	LTZ;

  if(gettimeofday(&LTV,&LTZ)!=-1)
  {
   Sh_StartTime=LTV.tv_sec;
  }
 }
#endif

 glutMainLoop();
}
