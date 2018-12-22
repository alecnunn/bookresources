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
/* Real-time glass								*/
/*										*/
/* - The Main									*/
/*										*/
/* AUTHOR:	Gabor Nagy							*/
/* DATE:	2000-May-02 14:49:29						*/
/*  v1.0									*/
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

E3dModel*	Scn_GlassModel=NULL;
E3dModel*	Scn_BulbGlassModel=NULL;
E3dModel*	Scn_BulbBlackModel=NULL;
E3dModel*	Scn_BulbCopperModel=NULL;
E3dModel*	Scn_ClothModel=NULL;
E3dModel*	Scn_TableModel=NULL;

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
unsigned long	Sh_Frame=0;
unsigned long	Sh_NumOfFrames=200;

// Shadow map
//
unsigned long*	Sh_ShadowMapImage=NULL;
int		Sh_ShadowMapXSize=128,Sh_ShadowMapYSize=128;


int		Sh_TexXSize, Sh_TexYSize;
GLubyte*	Sh_TexImage;
GLuint		Sh_EnvMapTexId, Sh_WoodTexId;

float		Scn_CameraX=0.0, Scn_CameraY=0.0, Scn_CameraZ=0.0;

E3dLight	Scn_Headlight;


// Viewing
//
float	Sh_Longitude=152.0, Sh_Latitude=-36.0;
float	Sh_ViewDistance = 20.0;
float	Sh_ZNear=0.1, Sh_ZFar=8192.0;
float	Sh_WindowAspect = 4.0/3.0;
long	Sh_WindowXSize, Sh_WindowYSize;
int	Sh_PrevMX, Sh_PrevMY;
EBool	Sh_LeftButton = FALSE, Sh_MiddleButton = FALSE;

int	Sh_MainMenu, Sh_DisplayMenu;

int	Sh_CurrentGlassObject=-1;


/*======================================*/
/* Setlect blocker object		*/
/*======================================*/
void ShCB_SetlectGlassObject(int LValue)
{
 E3dModel*	LModel;
 E3dMesh*	LMesh;
 E3dPolyGroup*	LPolyGroup;
 E3dMaterial*	LMaterial;

 if(Sh_CurrentGlassObject!=LValue)
 {
   switch(LValue) 
   {
     case 0: 
       if(Scn_GlassModel!=NULL) E3d_ModelFree(Scn_GlassModel);
       if((LModel=E3d_ReadOBJFile("objects/BulbGlass.obj")) != NULL)
       {
	 Scn_GlassModel = LModel;

	 if((LMesh=(E3dMesh*)(LModel->Geometry))!=NULL)
	 {
	   LPolyGroup=LMesh->PolyGroups[0];
	   LPolyGroup->Material=LMaterial=E3d_MaterialAllocate();
	   LMaterial->Ambient[0]=0.0;
	   LMaterial->Ambient[1]=0.0;
	   LMaterial->Ambient[2]=0.0;

	   LMaterial->Diffuse[0]=3.0;LMaterial->Diffuse[1]=3.0;LMaterial->Diffuse[2]=3.0;LMaterial->Diffuse[3]=0.36;
	 }
       }

       if((LModel=E3d_ReadOBJFile("objects/BulbBlack.obj")) != NULL)
       {
         Scn_BulbBlackModel = LModel;
// Plastic part of light bulb
//
	 if((LMesh=(E3dMesh*)(LModel->Geometry))!=NULL)
	 {
	   LPolyGroup=LMesh->PolyGroups[0];
	   LPolyGroup->Material=LMaterial=E3d_MaterialAllocate();
	   LMaterial->Ambient[0]=0.0;
	   LMaterial->Ambient[1]=0.0;
	   LMaterial->Ambient[2]=0.0;
	   LMaterial->Diffuse[0]=0.1;
	   LMaterial->Diffuse[1]=0.1;
	   LMaterial->Diffuse[2]=0.1;
	 }

	 E3d_ModelRefreshMatrices(LModel);
       }

       if((LModel=E3d_ReadOBJFile("objects/BulbCopper.obj")) != NULL)
       {
         Scn_BulbCopperModel = LModel;
// Copper part of light bulb
//
	 if((LMesh=(E3dMesh*)(LModel->Geometry))!=NULL)
	 {
	   LPolyGroup=LMesh->PolyGroups[0];
	   LPolyGroup->Material=LMaterial=E3d_MaterialAllocate();
	   LMaterial->Ambient[0]=2.0;
	   LMaterial->Ambient[1]=1.2;
	   LMaterial->Ambient[2]=0.4;
	   LMaterial->Diffuse[0]=1.0;
	   LMaterial->Diffuse[1]=0.6;
	   LMaterial->Diffuse[2]=0.2;
	 }

	 E3d_ModelRefreshMatrices(LModel);
       }
     break;
   }
   Sh_CurrentGlassObject=LValue;
 }

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


// Read and set up environment-map texture
//
 glGenTextures(1,&Sh_EnvMapTexId);
 glBindTexture(GL_TEXTURE_2D, Sh_EnvMapTexId);

 if((LTexData=(GLubyte*)E3d_LoadImage("Envmap.rgba"))!=NULL)
 {
  LTexXSize=128;LTexYSize=128;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, LTexXSize, LTexYSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, LTexData );
 }
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


 glEnable(GL_LIGHTING);
 glDisable(GL_TEXTURE_2D);

 E3d_GLUpdateLight(0, &Sh_Light);

// Draw cloth
//
 if((LModel=Scn_ClothModel)!=NULL)
 {
  glPushMatrix();

  glDisable(GL_TEXTURE_2D);

  E3d_DrawModel(LModel, FALSE, &Sh_Light, FALSE);
  glPopMatrix();
 }


// Draw table
//
 if((LModel=Scn_TableModel)!=NULL)
 {
  glPushMatrix();

// Set up wood texture
//
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, Sh_WoodTexId);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);

  E3d_DrawModel(LModel, TRUE, &Sh_Light, FALSE);
  glPopMatrix();
 }


// Draw light bulb copper part
//
 if((LModel=Scn_BulbCopperModel)!=NULL)
 {
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);

// Set up shpere-map texture
//
  glBindTexture(GL_TEXTURE_2D, Sh_EnvMapTexId);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_TEXTURE_2D);

// Texture-coordinate generation method
//
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);

  E3d_DrawModel(LModel, FALSE, &Sh_Light, FALSE);
  glPopMatrix();
 }


// Draw light bulb plastic part
//
 if((LModel=Scn_BulbBlackModel)!=NULL)
 {
  glDisable(GL_TEXTURE_2D);
  glPushMatrix();

  E3d_DrawModel(LModel, FALSE, &Sh_Light, FALSE);
  glPopMatrix();
 }


// Render glass model
//
 if(Scn_GlassModel!=NULL)
 {
  glEnable(GL_BLEND);

// Turn off Z update
//
  glDepthMask(GL_FALSE);

// Enable back-face culling
//
  glEnable(GL_CULL_FACE);


  LMesh=(E3dMesh*)(Scn_GlassModel->Geometry);
  LPolyGroup=LMesh->PolyGroups[0];

  LMaterial=LPolyGroup->Material;


  glPushMatrix();

// Define headlight
//
  Scn_Headlight.Position.X=Scn_CameraX;
  Scn_Headlight.Position.Y=Scn_CameraY;
  Scn_Headlight.Position.Z=Scn_CameraZ;
  E3d_GLUpdateLight(0, &Scn_Headlight);


// Set up shpere-map texture
//
  glBindTexture(GL_TEXTURE_2D, Sh_EnvMapTexId);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_TEXTURE_2D);

// Texture-coordinate generation method
//
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);


  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


/*
// Colored glass
//
  glBlendFunc(GL_ONE_MINUS_CONSTANT_COLOR_EXT, GL_CONSTANT_COLOR_EXT);

// "Transparent color" of the glass
//
  glBlendColorEXT(0.2, 0.4, 0.2, 1.0);
*/


  E3d_DrawModel(Scn_GlassModel, FALSE, NULL, FALSE);

  glPopMatrix();
  glBlendFunc(GL_ONE, GL_ZERO);

// Turn on Z-updating
//
  glDepthMask(GL_TRUE);

  glDisable(GL_BLEND);

// Restore light
//
  E3d_GLUpdateLight(0, &Sh_Light);

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
 E3dMatrix		LMatrix;
 float			Mx, My, Mz;

#ifdef _WIN32
 float newestTime;

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
 struct timeval		LTV;
 struct timezone	LTZ;

// For timing
//
 Sh_Frame+=1;

 if(gettimeofday(&LTV,&LTZ)!=-1)
 {
  if((LTV.tv_sec-Sh_StartTime)>=10)
  {

//printf("FrameRate: %3.2f fps\n",(float)Sh_Frame/(float)(LTV.tv_sec-Sh_StartTime));fflush(stdout);

   Sh_Frame=0;
   Sh_StartTime=LTV.tv_sec;
  }
 }
#endif
 glViewport(0, 0, Sh_WindowXSize, Sh_WindowYSize);
 
 glClearColor(0.0, 0.0, 0.0, 0.0);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluPerspective(64.0, Sh_WindowAspect, Sh_ZNear, Sh_ZFar);

 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity(); 

// Camera matrix for the head-light
//
 glRotatef(-Sh_Longitude, 0.0, 1.0, 0.0);
 glRotatef(Sh_Latitude, 1.0, 0.0, 0.0);
 glTranslatef(0.0, 0.0, Sh_ViewDistance);
 glGetFloatv(GL_MODELVIEW_MATRIX, LMatrix);

 Mx=0.0;My=0.0;Mz=0.0;
 E3dM_MatrixTransform3x4(LMatrix, Scn_CameraX, Scn_CameraY, Scn_CameraZ);

 glLoadIdentity(); 


// Camera transformation
//
 glTranslatef(0.0, 0.0, -Sh_ViewDistance);
 glRotatef(-Sh_Latitude, 1.0, 0.0, 0.0);
 glRotatef(Sh_Longitude, 0.0, 1.0, 0.0);



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

     E3d_GLUpdateLight(0, &Sh_Light);
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
 E3dModel*	LModel;
 E3dMesh*	LMesh;
 E3dPolyGroup*	LPolyGroup;

 glutInit(&LArgC, LArgV);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
 glutInitWindowSize(420, 320);
 glutCreateWindow("Glass");

 glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);

 E3d_MaterialDefault(&E3d_DefaultMaterial);

// Read a glass object
//
 ShCB_SetlectGlassObject(0);

 if((Scn_ClothModel=E3d_ReadOBJFile("objects/cloth.obj")) != NULL)
 {
  Scn_ClothModel->Translation.Y=-5.0;
  Scn_ClothModel->Translation.Z=-3.0;

  if((LMesh=(E3dMesh*)(Scn_ClothModel->Geometry))!=NULL)
  {
   LPolyGroup=LMesh->PolyGroups[0];

   LPolyGroup->ShadowPass=TRUE;

   LPolyGroup->Material=LMaterial=E3d_MaterialAllocate();
   LMaterial->Diffuse[0]=0.7;
   LMaterial->Diffuse[1]=0.5;
   LMaterial->Diffuse[2]=0.2;

// This object won't move, so we won't recompute it's local->world matrix at each frame
//
   E3d_ModelRefreshMatrices(Scn_ClothModel);
  }
 }

// Initialize table
//
 if((LModel=E3d_ReadOBJFile("objects/table.obj")) != NULL)
 {
  Scn_TableModel=LModel;

  LModel->Translation.Y=0.0;

  if((LMesh=(E3dMesh*)(LModel->Geometry))!=NULL)
  {
   LPolyGroup=LMesh->PolyGroups[0];

// Make material white so the wood texture will blend properly
//
   LPolyGroup->Material=LMaterial=E3d_MaterialAllocate();
   LMaterial->Diffuse[0]=1.0;
   LMaterial->Diffuse[1]=1.0;
   LMaterial->Diffuse[2]=1.0;


// This object won't move, so we won't recompute it's local->world matrix at each frame
//
   E3d_ModelRefreshMatrices(LModel);
  }
 }


// Initialize main light source
//
 E3d_LightDefault(&Sh_Light);
 Sh_Light.Position.X=-16.0;
 Sh_Light.Position.Y=16.0;
 Sh_Light.Position.Z=3.6;


// Initialize head-light
//
 E3d_LightDefault(&Scn_Headlight);
 Scn_Headlight.Position.X=0.0;
 Scn_Headlight.Position.Y=0.0;
 Scn_Headlight.Position.Z=0.0;



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

 E3d_GLUpdateLight(0, &Sh_Light);
 glEnable(GL_LIGHT0);
 Sh_InitTextures();

 glutReshapeFunc(Sh_Resize);
 glutDisplayFunc(Sh_Display);

 glutKeyboardFunc(ShCB_Keyboard);
 glutMouseFunc(ShCB_MouseButtons);
 glutMotionFunc(ShCB_MouseMotion);

 Sh_MainMenu = glutCreateMenu(ShCB_MainMenu);
 glutAddMenuEntry("Exit", 5);
 glutAttachMenu(GLUT_RIGHT_BUTTON);

 glutMainLoop();
}
