/* Copyright (C) Torgeir Hagland, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Torgeir Hagland, 2000"
 */
/********************************************/
/* Programming By Torgeir Hagland			*/
/* Artwork and Animation By Stuart Black	*/
/********************************************/

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <strings.h>
#else
#include <unistd.h>
#include <string.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <assert.h>
#include <math.h>
#include "3dsftk.h"

#ifndef _WIN32
typedef unsigned int BOOL;
#define TRUE 1
#define FALSE 0
#endif

typedef	float	MATRIX[4][3];

struct Bone {
	struct Bone	*NextPtr;			/* Pointer to next bone in list								*/
	char		Name[12];			/* Name of this bone										*/
	long		NrVerts;			/* How many vertices this bone influences					*/
	MATRIX		Matrix;				/* Initial matrix of the bone								*/
	MATRIX		*AnimPtr;			/* Pointer to the animation	as described in the VUE file	*/
};

struct Skin {
	long		NrFrames;			/* The length of the VUE animation	*/
	point3ds	*PointPtr;			/* Vertex buffer to translate into	*/
	Bone		*BonePtr;			/* Linked list to the bones			*/
	mesh3ds		*MeshPtr;			/* Pointer to the 3ds mesh structure*/
};

struct BonePoint {
	point3ds	Point;				/* location of this point in 3D space				*/
	int			Index;				/* original index in the facearray of the mesh		*/
	Bone		*BonePtr;			/* pointer to the bone that influences this point	*/
};

#define	WORLD_NEAR	1000
#define WORLD_FAR	25000
#define	ONEDEGREE ((1.0f/180.0f)*3.1415926)

long	CurFrame	= 0;						/* Current frame in animation								*/
float	AngleY		= -25*ONEDEGREE;			/* Current angle of the camera								*/
float	Distance	= -12000;					/* Current camera distance from the world center (0,0,0)	*/
float	Height		= 0;						/* Current camera height from the world center (0,0,0)		*/
BOOL	Paused		= FALSE;					/* Is the animation playback paused or not					*/

Skin *SkinPtr;									/* pointer to the character/skin displayed					*/

void glInit()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glDepthRange(0.0, 1.0);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glPolygonMode(GL_FRONT,GL_LINE);
}

void Transform(MATRIX m, float *src, float *dst)
{
	float v[3];
	v[0]=src[0];v[1]=src[1];v[2]=src[2];

	dst[0]=(v[0]*m[0][0])+(v[1]*m[1][0])+(v[2]*m[2][0])+m[3][0];
	dst[1]=(v[0]*m[0][1])+(v[1]*m[1][1])+(v[2]*m[2][1])+m[3][1];
	dst[2]=(v[0]*m[0][2])+(v[1]*m[1][2])+(v[2]*m[2][2])+m[3][2];
}

void glDrawChar()
{
	mesh3ds		*meshptr	= SkinPtr->MeshPtr;
	Bone		*boneptr	= SkinPtr->BonePtr;
	point3ds	*vertptr	= meshptr->vertexarray;
	face3ds		*faceptr	= meshptr->facearray;

	/*
		For Each bone in the skin, transform X amount of vertices with the bone's
		Current animation matrix
	*/
	point3ds *skinptr=SkinPtr->PointPtr;
	while(boneptr)
	{
		MATRIX mat;
		memcpy(&mat,&boneptr->AnimPtr[CurFrame],sizeof(MATRIX));
		for (int i=0;i<boneptr->NrVerts;i++)
			Transform(mat,(float*)vertptr++,(float*)skinptr++);

		boneptr=boneptr->NextPtr;
	}

	/*
		Then Simply draw the object using the facelist
	*/
	skinptr=SkinPtr->PointPtr;
	glBegin(GL_TRIANGLES);
		glColor3f(1,1,1);
		for(int i=0;i<meshptr->nfaces;i++)
		{
			point3ds *v1=&skinptr[faceptr->v1];
			point3ds *v2=&skinptr[faceptr->v2];
			point3ds *v3=&skinptr[faceptr->v3];

			glVertex3f(v1->x,v1->y,v1->z);
			glVertex3f(v2->x,v2->y,v2->z);
			glVertex3f(v3->x,v3->y,v3->z);

			faceptr++;
		}
	glEnd();
}

void glDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDrawChar();
	glutSwapBuffers();
}

void glSetCameraPos()
{
	float CameraPos[3];
	CameraPos[0]=Distance*-sin(AngleY);
	CameraPos[1]=Height;
	CameraPos[2]=Distance*cos(AngleY);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraPos[0],CameraPos[1],CameraPos[2],0,0,0,0,1,0);
}

void glResize(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0,(GLfloat)w/(GLfloat)h,WORLD_NEAR,WORLD_FAR);

	glSetCameraPos();
}


  
void KeyHandler(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		Paused=!Paused;
		break;
	case 'a':
		AngleY+=5*ONEDEGREE;
		glSetCameraPos();
		if(Paused)
			glutPostRedisplay();
		break;
	case 'd':
		AngleY-=5*ONEDEGREE;
		glSetCameraPos();
		if(Paused)
			glutPostRedisplay();
		break;
	case 'w':
		Distance+=200;
		glSetCameraPos();
		if(Paused)
			glutPostRedisplay();
		break;
	case 's':
		Distance-=200;
		glSetCameraPos();
		if(Paused)
			glutPostRedisplay();
		break;
	case 'q':
		Height-=200;
		glSetCameraPos();
		if(Paused)
			glutPostRedisplay();
		break;
	case 'e':
		Height+=200;
		glSetCameraPos();
		if(Paused)
			glutPostRedisplay();
		break;
	case 27:
		exit(0);
	}
}

database3ds *LoadFiles(const char *FileName)
{
	file3ds *FileLoaded=OpenFile3ds(FileName,"r");		//was rw
	if (!FileLoaded) 
		return NULL;

	printf("Creating database for %s\r\n",FileName);

	database3ds *db=NULL;
	InitDatabase3ds(&db);
	CreateDatabase3ds(FileLoaded,db);
	ON_ERROR_RETURNR(NULL);
	DisconnectDatabase3ds(db);
	CloseFile3ds(FileLoaded);

	return db;
}

/*
	Returns the number of objects in this list that have *objname* in their name

	i.e.
	a call to this routine with this list: inf,inf01,inf03,inf04,inf05 with objname
	and the objname==inf, would return 5, while a call with objname==inf01, would return 1
*/
int GetNumbSubStr(char *objname, namelist3ds *meshlist)
{
	long nr=0;

	for(int i=0;i<meshlist->count;i++)
	{
		if(strstr(meshlist->list[i].name,objname))
			nr++;
	}

	return nr;
}

/*
	Using the same method as above, but with an extra check to prevent the skin from
	ending up in the bone list.
*/
void FindBones(database3ds *db, char *objname, namelist3ds *meshlist, Skin *skinptr)
{
	for(int i=0;i<meshlist->count;i++)
	{
		char *bonename=meshlist->list[i].name;
		if(strstr(bonename,objname) && strcmp(bonename,objname))
		{
			Bone *boneptr=(Bone*)calloc(1,sizeof(Bone));
			strcpy(boneptr->Name,bonename);
			boneptr->NextPtr=skinptr->BonePtr;
			skinptr->BonePtr=boneptr;
		}
	}
}

float CalcDistNotSquared(point3ds *p1, point3ds *p2)
{
	float dx=p1->x - p2->x;
	float dy=p1->y - p2->y;
	float dz=p1->z - p2->z;

	return (dx*dx)+(dy*dy)+(dz*dz);
}

void Copy3dsMatrix(MATRIX matbone, float *mat3ds)
{
	matbone[0][0]=mat3ds[0];
	matbone[0][1]=mat3ds[1];
	matbone[0][2]=mat3ds[2];

	matbone[1][0]=mat3ds[3];
	matbone[1][1]=mat3ds[4];
	matbone[1][2]=mat3ds[5];

	matbone[2][0]=mat3ds[6];
	matbone[2][1]=mat3ds[7];
	matbone[2][2]=mat3ds[8];

	matbone[3][0]=mat3ds[9];
	matbone[3][1]=mat3ds[10];
	matbone[3][2]=mat3ds[11];
}

void InverseMatrix(MATRIX a, MATRIX b)
{
	float det;
	int i, j;

	b[0][0]=(a[1][1]*a[2][2]-a[1][2]*a[2][1]);
	b[1][0]=(a[1][2]*a[2][0]-a[1][0]*a[2][2]);
	b[2][0]=(a[1][0]*a[2][1]-a[1][1]*a[2][0]);
	b[0][1]=(a[2][1]*a[0][2]-a[2][2]*a[0][1]);
	b[1][1]=(a[2][2]*a[0][0]-a[2][0]*a[0][2]);
	b[2][1]=(a[2][0]*a[0][1]-a[2][1]*a[0][0]);
	b[0][2]=(a[0][1]*a[1][2]-a[0][2]*a[1][1]);
	b[1][2]=(a[0][2]*a[1][0]-a[0][0]*a[1][2]);
	b[2][2]=(a[0][0]*a[1][1]-a[0][1]*a[1][0]);

	det=0;
	for (i=0;i<3;i++)
    	det+=a[i][0]*b[0][i];

	if (det==0) det=.0000001;
	for (i=0;i<3;i++)
	{
	    for (j=0;j<3;j++) b[i][j]/=det;
     	b[3][i]=-a[3][i];
	}
}

/*	
	remap's thew face index, and adds 32000 to prevent faces to be remapped on subsequent calls
*/
void RemapFaceList(mesh3ds *mesh, long oldindex, long newindex)
{
	face3ds *faceptr=mesh->facearray;
	for (int i=0;i<mesh->nfaces;i++)
	{
		if(faceptr->v1==oldindex)
			faceptr->v1=newindex+32000;
		if(faceptr->v2==oldindex)
			faceptr->v2=newindex+32000;
		if(faceptr->v3==oldindex)
			faceptr->v3=newindex+32000;
		faceptr++;
	}
}

/*
	Simply removes all the +32000 values that was added in RemapFaceList
*/
void CleanUpFaceList(mesh3ds *mesh)
{
	face3ds *faceptr=mesh->facearray;
	for (int i=0;i<mesh->nfaces;i++)
	{
		faceptr->v1-=32000;
		faceptr->v2-=32000;
		faceptr->v3-=32000;
		faceptr++;
	}
}

void SolveBoneInfluences(database3ds *db, Skin *skinptr)
{
	/* Allocate a big workbuffer */
	BonePoint *bonepointptr=(BonePoint*)malloc(30000*sizeof(BonePoint));
	BonePoint *curbonepoint=bonepointptr;

	long NrBoneVerts=0;

	/*	Make all the bones' vertices into one big vertex list with information on what bone
		each point came from */

	MATRIX tmpmat;

	Bone *boneptr=skinptr->BonePtr;
	while(boneptr)
	{
		mesh3ds *bonemesh=NULL;
		GetMeshByName3ds(db,boneptr->Name,&bonemesh);
		assert(bonemesh);

		Copy3dsMatrix(tmpmat,bonemesh->locmatrix);
		InverseMatrix(tmpmat,boneptr->Matrix);

		point3ds *bonemeshpoints=bonemesh->vertexarray;

		NrBoneVerts+=bonemesh->nvertices;
		assert(NrBoneVerts<30000);

		for(int i=0;i<bonemesh->nvertices;i++)
		{
			curbonepoint->Point.x=bonemeshpoints->x;
			curbonepoint->Point.y=bonemeshpoints->y;
			curbonepoint->Point.z=bonemeshpoints->z;
			curbonepoint->BonePtr=boneptr;
			bonemeshpoints++;
			curbonepoint++;
		}

		RelMeshObj3ds(&bonemesh);
		boneptr=boneptr->NextPtr;
	}

	mesh3ds		*skinmesh		= skinptr->MeshPtr;
	point3ds	*skinmeshpoints	= skinmesh->vertexarray;
	BonePoint	*skinpointptr	= (BonePoint*)malloc(skinmesh->nvertices*sizeof(BonePoint));
	BonePoint	*curskinpoint	= skinpointptr;

	/*	find the closest bone vertex to each skin vertex */
	int i;
	for (i=0;i<skinmesh->nvertices;i++)
	{
		curskinpoint->Point.x	= skinmeshpoints->x;
		curskinpoint->Point.y	= skinmeshpoints->y;
		curskinpoint->Point.z	= skinmeshpoints->z;
		curskinpoint->Index		= i;							/* need to store original vertex index, for face remapping */
		curskinpoint->BonePtr	= NULL;							/* no bone is influencing this bone yet */

		curbonepoint=bonepointptr;
		float mindist=1e6;

		for(int j=0;j<NrBoneVerts;j++)
		{
			float dist=CalcDistNotSquared(skinmeshpoints,&curbonepoint->Point);
			if(dist<mindist)
			{
				mindist=dist;
				curskinpoint->BonePtr=curbonepoint->BonePtr;
			}
			curbonepoint++;
		}
		curskinpoint++;
		skinmeshpoints++;
	}

	/*	Sort all the vertices of the skin by bone, and remap the faces accordingly */
	skinmeshpoints		= skinmesh->vertexarray;
	face3ds *skinfaces	= skinmesh->facearray;
	long CurIndex=0;
	boneptr=skinptr->BonePtr;
	while(boneptr)
	{
		curskinpoint=skinpointptr;
		for (i=0;i<skinmesh->nvertices;i++)
		{
			if(curskinpoint->BonePtr==boneptr)
			{
				Transform(boneptr->Matrix,(float*)&curskinpoint->Point,(float*)skinmeshpoints);
				RemapFaceList(skinmesh,curskinpoint->Index,CurIndex++);
				boneptr->NrVerts++;
				skinmeshpoints++;
			}
			curskinpoint++;
		}
		boneptr=boneptr->NextPtr;
	}

	/* Clean up after the remapping */
	CleanUpFaceList(skinmesh);

	free(skinpointptr);
	free(bonepointptr);
}


Skin *InitSkin(database3ds *db)
{
	Skin *skinptr=NULL;

	namelist3ds *meshlist=NULL;
	GetMeshNameList3ds(db,&meshlist);
	assert(meshlist);

	for(int i=0;i<meshlist->count;i++)
	{
		char *objname=meshlist->list[i].name;
		int NrSubStrObj=GetNumbSubStr(objname,meshlist);

		if(NrSubStrObj>1)
		{
			skinptr=(Skin*)calloc(1,sizeof(Skin));
			GetMeshByName3ds(db,objname,&skinptr->MeshPtr);
			assert(skinptr->MeshPtr);

			skinptr->PointPtr=(point3ds*)malloc(skinptr->MeshPtr->nvertices*sizeof(point3ds));

			FindBones(db,objname,meshlist,skinptr);
			SolveBoneInfluences(db,skinptr);
			break;
		}
	}

	return skinptr;
}

void AddKeyFrame(long frame, char *objname, float *matrix)
{
	Bone *boneptr=SkinPtr->BonePtr;
	while(boneptr)
	{
		if(!strcmp(objname,boneptr->Name))
		{
			boneptr->AnimPtr=(MATRIX*)realloc(boneptr->AnimPtr,SkinPtr->NrFrames*sizeof(MATRIX));
			assert(boneptr->AnimPtr);
			Copy3dsMatrix(boneptr->AnimPtr[SkinPtr->NrFrames-1],matrix);
			break;
		}
		boneptr=boneptr->NextPtr;
	}
}

bool VUEParse(char *FileName)
{
	FILE *f=fopen(FileName,"rb");
	if(!f)
		return false;

	printf("Parsing %s\r\n",FileName);

	char command[256],param[256],objname[16];

	fscanf(f,"%s %s",command,param);

	if(strcmp(command,"VERSION") || strcmp(param,"201"))
		return false;

	float	Matrix[12];

	while(!feof(f))
	{
		fscanf(f,"%s",command);
		if(!strcmp(command,"frame"))
		{
			if(fscanf(f,"%d",&CurFrame)!=1)
				return false;

			SkinPtr->NrFrames=CurFrame+1;
		}
		else if(!strcmp(command,"transform"))
		{
			if(fscanf(f,"%s",&param)!=1)
				return false;
			
			int len=strlen(param)-2;
			strncpy(objname,param+1,len);
			objname[len]=0;

			for(int i=0;i<12;i++)
			{
				if(fscanf(f,"%f",&Matrix[i])!=1)
					return false;
			}

			AddKeyFrame(CurFrame,objname,Matrix);
		}
	}
	fclose(f);

	CurFrame=0;

	return true;
}

void NextFrame()
{
	if(!Paused)
	{
		CurFrame++;
		if(CurFrame>SkinPtr->NrFrames)
		{
			CurFrame=0;
		}

		glutPostRedisplay();
#ifdef _WIN32
		Sleep(50);
#else
		usleep(50);
#endif
	}
}

void DisplayKeyControls()
{
	printf("\npause		- spacebar\n");
	printf("rotate left	- a\n");
	printf("rotate right	- d\n");
	printf("zoom in		- w\n");
	printf("zoom out	- s\n");
	printf("move up		- e\n");
	printf("move down	- q\n");
}


int	main(int argc, char **argv)
{
	if(argc<3)
	{
		printf("\nSyntax: xbone <3ds-file> <vue file>\n");
#ifdef _WIN32
		getch();
#endif
		exit(-1);
	}

	database3ds *db=LoadFiles(argv[1]);
	if(!db)
	{
		printf("\nerror opening %s\n",argv[1]);
#ifdef _WIN32
		getch();
#endif
		exit(-1);
	}

	SkinPtr=InitSkin(db);
	if(!SkinPtr)
	{
		printf("\nerror initializing skin %s\n",argv[1]);
#ifdef _WIN32
		getch();
#endif
		exit(-1);
	}

	if(!VUEParse(argv[2]))
	{
		printf("\nerror parsing vue file %s\n",argv[2]);
#ifdef _WIN32
		getch();
#endif
		exit(-1);
	}

	DisplayKeyControls();

	glutInitWindowPosition(150,150);
	glutInitWindowSize(400,400);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInit(&argc, argv);
	glutCreateWindow(argv[0]);
	glInit();
	glutReshapeFunc(glResize);
	glutDisplayFunc(glDisplay);
	glutKeyboardFunc(KeyHandler);
	glutIdleFunc(NextFrame);
	glutMainLoop();
	return FALSE;
}
