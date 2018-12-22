#include<gl\glut.h>
#include<gl\glext.h>
#define CG_DEPRECATED_API
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include<fstream.h>
#include<iostream.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include <assert.h>
#include"matrix.h"
#include "bmp.h"
GLuint	texturest;

//Multi texture
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
bitmap_t TextureImage;

static cgContext *Context = NULL;
static cgBindIter *ModelViewProjBind = NULL;
#define B 32
static cgBindIter *GradPermTable[B+B+2] ;
GLuint ProgId = 1;
static cgProgramIter *ProgramIter = NULL;

void SetupCG()
{
	Context = cgCreateContext();
	assert(Context != NULL);
	cgError Ret;
	Ret=cgAddProgramFromFile(Context, "planet.cg", cgVertex30Profile, "main");
	assert(Ret == cgNoError);
	ProgramIter = cgProgramByName(Context, "main");
	cgGLLoadProgram(ProgramIter, ProgId);
	ModelViewProjBind = cgGetBindByName(ProgramIter, "main.ModelViewProj");
	
	fprintf(stderr, "---- PROGRAM BEGIN ----\n%s---- PROGRAM END ----\n",
        cgGetProgramObjectCode(ProgramIter));
	 ModelViewProjBind = cgGetBindByName(ProgramIter, "ModelViewProj");
	char name[10];
	 for(int i=0;i<B+B+2;i++)
	 {
	 sprintf(name, "pg[%d]", i);
	 GradPermTable[i] = cgGetBindByName(ProgramIter, name);
	 }
}


float pg[B+B+2][4];
#define DOT(a,b)(a[0]*b[0]+a[1]*b[1]+a[2]*b[2])

void SetupTable()
{
	int i,j,k;
	float v[3],s;
	for (i = 0 ; i < B ; i++) {
	
		do{
		for (int j = 0 ; j < 3 ; j++)
			v[j] = (float)((rand() % (B + B)) - B) / B;
		s=DOT(v,v);
		}while(s>1);
		s=sqrt(s);
		for(j=0;j<3;j++)
			pg[i][j]=v[j]/s;	
	}

	for(i=0;i<B;i++)
		pg[i][3]=i;
	for(i=B;i>0;i-=2){
	k=pg[i][3];
	pg[i][3]=pg[j=rand()%B][3];
	pg[j][3]=k;
	}
	for(i=0;i<B+2;i++){
		pg[B+i][3]=pg[i][3];
		for(j=0;j<3;j++)
			pg[B+i][j]=pg[i][j];
	}
}
void setuptext()
{
	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
        wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)
        wglGetProcAddress("glMultiTexCoord2fARB");
}



void setup()
{
	

	glActiveTextureARB(0);

	
	LoadBMP("land.bmp",&TextureImage);
	glGenTextures(1,&texturest);
	glBindTexture(GL_TEXTURE_2D, texturest);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage.width, TextureImage.height, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage.data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering*/
	
	
	//
	
	
	
}
struct vertex{
	double x,y,z,u,v;
};
vertex sphere[20000];
	int i3=0;
	int res=130;
	bool wire=false;
	bool LOD=false;
void generatesphere(int numbofpoints)
{
	/////////////
	double horizontal=0;
	if(numbofpoints>130)
	{
	res=numbofpoints=130;
	}
	if(numbofpoints<27)
	{
	res=numbofpoints=27;
	}
	float n, p, nStep = 6.283185307179586476925286766559/numbofpoints;
	i3=0;
	float fx=0;
	float fy=0;
	int num=0;
	for( n = 0; n  <= numbofpoints/2; n++ )	
	{
	
	
		for ( p = 0; p <= numbofpoints; p++ )	
		{		
			horizontal	=  sin(fy);	
			sphere[i3].x = (horizontal * sin(fx));	
			sphere[i3].y = (horizontal * cos(fx));
			sphere[i3].z = (cos(fy));
			sphere[i3].u=p/numbofpoints;
			sphere[i3].v=2*n/(numbofpoints);
						
		
			i3++;
			fy-=nStep;

			horizontal	=  sin(fy);	
			sphere[i3].x = (horizontal * sin(fx));	
			sphere[i3].y = (horizontal * cos(fx));
			sphere[i3].z = (cos(fy));
			sphere[i3].u=p/numbofpoints;
			sphere[i3].v=2*(n+1)/(numbofpoints);
			
			
			fx-=nStep;
			i3++;
			fy+=nStep;

		}
		fx=0;
		fy-=nStep;
	
		
	}
	cout<<"Number of Triangles "<<i3;
	cout<<endl;
}

void RenderScene(void) {
	
	setup();
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);
	glPushMatrix();
	//////
	cgError Ret;
	Ret=cgGLBindProgram(ProgramIter);
	assert(Ret == cgNoError);
	
	for(int i2=0;i2<B+B+2;i2++)
	{
		cgGLBindUniform4fv(ProgramIter,GradPermTable[i2],pg[i2]);
  	}

	cgGLEnableProgramType(cgVertexProfile);
	updatecamera();

	glTranslatef(0,0,-3);
	glRotatef(90,1,0,0);
	if(ModelViewProjBind != NULL)
		cgGLBindUniformStateMatrix(ProgramIter, 
		ModelViewProjBind,
		cgGLModelViewProjectionMatrix,
		cgGLMatrixIdentity);

	
	////////

	/////
	glEnable(GL_TEXTURE_2D);
	/////
	//glTranslatef(0,0,-3);

	///

	if(wire==true)
	{
		glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE );
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL );
	}
	glBegin(GL_TRIANGLE_STRIP);
		for(int i=0;i<i3;i++)
		{
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,sphere[i].u,sphere[i].v);
		glVertex3f(sphere[i].x,sphere[i].y,sphere[i].z);
	
		}
	glEnd();
	cgGLDisableProgramType(cgVertexProfile);
	glDisable(GL_TEXTURE_2D);
	glColor3f(0,0,1);

	glutSolidSphere(1.16,30,30);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1,1,1);
	glPopMatrix();
	glutSwapBuffers();
	
}
void setres()
{
float x=fabs(translationm[0][3])*20;
float y=fabs(translationm[1][3])*20;
float z=fabs(translationm[2][3])*20;
float ress=(x+y+z)/10;
res=130-ress;
}
void Key( unsigned char key, int x, int y )
{
	if(key=='l')
	{
		if(LOD==false)
		{
			LOD=true;
		}
		else
		{
			LOD=false;
		}
	}

	if(key=='p')
	{
		wire=true;
	}
	if(key=='o')
	{
		wire=false;
	}

		if(key=='d')
	{
		rotatey(.1);
		RenderScene();
		
		
	}
	if(key=='a')
	{	
		rotatey(-.1);
		RenderScene();
		
		
	}
	if(key=='r')
	{
		rotatex(.1);
		RenderScene();
		
		
	}
	if(key=='t')
	{	
		rotatex(-.1);
		RenderScene();
		
	}
	if(key=='w')
	{	
	
		moveforward(.1);
		if(LOD==true)
		{
			setres();//This is for the simple LOD system. You probably will not want to update the LOD every time you move forward, as done here.
			generatesphere(res);
		}
		RenderScene();
		
		
	}
	if(key=='s')
	{	
	
		moveforward(-.1);
		if(LOD==true)
		{
			setres();//This is for the simple LOD system. You probably will not want to update the LOD every time you move forward, as done here.
			generatesphere(res);
		}
		RenderScene();
		
	}
	if(key=='z')
	{
		moveup(-1);
		RenderScene();
	}
	if(key=='x')
	{
		moveup(1);
		RenderScene();
	}
	
}  

void main() {
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(512,512);
	glutCreateWindow("VertexProgram Planet");
	glClearColor(0,0,0,0);
	GLfloat fAspect=512/512;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,fAspect,0.1,400.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(Key);
	srand(12);
	SetupTable();
	setuptext();
	setres();
	SetupCG();
	generatesphere(res);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	FreeBMP(&TextureImage);
}
