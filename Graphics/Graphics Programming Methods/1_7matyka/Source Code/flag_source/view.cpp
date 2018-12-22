


#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <GL/glut.h>
#include <math.h>

#include "materialpoint.h"
#include "view.h"
#include "newton.h"
#include "cloth.h"
#include "SoftBody.h"

#ifndef M_PI
#define M_PI 3.1415
#endif

extern unsigned int texture1;
extern unsigned int texture2;
extern Vector3 Graw;
extern Vector3 GrawBAK;
extern Vector3 Wind;

// SoftBody draw options
int SoftBodydrawmode=4;		// 4 env
int lighting=1;			// lighting
int texturing=1;		// texture mapping


extern Obszar *KorzenObszarow;

float light_pos0[]={.0003,.0001,.001};
float light_pos1[]={-.0003,.0001,.001};


double aspect,sx=1.454,sy=1.454,sz=1.454,anglex=0,angley=0,anglez=0;
double tr_x=-0.2,tr_y=0,tr_z=0;			
			 
void GLWypisz(float x, float y, char *string);
int WindowDump(int wwidth, int wheight);

int saveanim=0;
int frame=0;

#ifndef PI
#define PI 3.141592654
#endif


extern Cloth cloth1;





void RysujVector3(Vector3 w1,Vector3 w2, double scale)
{
		double kx,ky,kprimx,kprimy,vsize,ksize;
		double x1,x2,y1,y2,z1,z2;

		x1=w1.x;
		y1=w1.y;
		z1=w1.z;


		x2=w2.x;
		y2=w2.y;
		z2=w2.z;



		if(x1==x2 && y1==y2)
			return;

		

// Wylicz d¦ugoŠ Vector3a v i ustal d¦ugoŠ Vector3a k

	vsize=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1));
	ksize=vsize*scale/4.f;


// Rysuj podstawà strza¦ki


    glBegin(GL_LINE_STRIP);

//	glVertex2f(x1,y1);
//	glVertex2f(((x2-x1)/vecsize),((y2-y1)/vecsize));

 	glVertex3f(x1,y1,z1);
 	glVertex3f(x1+(x2-x1)*scale,y1+(y2-y1)*scale,z1+(z2-z1)*scale);

    glEnd();


// wyznacz Vector3 jednostkowy skierowany naprzeciwko v

	kx=-(x2-x1)/vsize;
	ky=-(y2-y1)/vsize;

// obr¡Š k o 30 stopni

   	kprimx=kx*cos(35*PI/180.f)+ky*sin(35*PI/180.f);
   	kprimy=-kx*sin(35*PI/180.f)+ky*cos(35*PI/180.f);

// wymn¡+ przez jego d¦ugoŠ

	kprimx*=ksize;
	kprimy*=ksize;

// rysuj kprim

    glBegin(GL_LINE_STRIP);

	glVertex3f(x1+(x2-x1)*scale,y1+(y2-y1)*scale,z1+(z2-z1)*scale);
	glVertex3f(x1+(x2-x1)*scale+kprimx,y1+(y2-y1)*scale+kprimy,z1+(z2-z1)*scale);

    glEnd();

// obr¡Š k o -30 stopni

   	kprimx=kx*cos(-35*PI/180.f)+ky*sin(-35*PI/180.f);
   	kprimy=-kx*sin(-35*PI/180.f)+ky*cos(-35*PI/180.f);

// wymn¡+ przez jego d¦ugoŠ

	kprimx*=ksize;
	kprimy*=ksize;

// rysuj kprim

    glBegin(GL_LINE_STRIP);

	glVertex3f(x1+(x2-x1)*scale,y1+(y2-y1)*scale,z1+(z2-z1)*scale);
	glVertex3f(x1+(x2-x1)*scale+kprimx,y1+(y2-y1)*scale+kprimy,z1+(z2-z1)*scale);

    glEnd();

}




#define rysuj_Vector3y

#define rysuj_MaterialPointy
#define rysuj_sprezyny



void *font = GLUT_BITMAP_TIMES_ROMAN_24;
void *fonts[] =
{
  GLUT_BITMAP_9_BY_15,
  GLUT_BITMAP_TIMES_ROMAN_10,
  GLUT_BITMAP_TIMES_ROMAN_24
};


void DrawAreas(void)
{
	Obszar *roboczyO;

    glShadeModel(GL_SMOOTH); 

float mat[3];      


mat[0] = 0.01745; 
mat[1] = 0.01175; 
mat[2] = 0.01175;    
     glMaterialfv (GL_FRONT, GL_AMBIENT, mat);     
  glMaterialfv (GL_FRONT, GL_SPECULAR, mat);     
glMaterialf (GL_FRONT, GL_SHININESS, 0.6*128.0);     


	for(roboczyO=KorzenObszarow;roboczyO != NULL; roboczyO=roboczyO->prawy)
	if(roboczyO->flaga & O_AKT)
	{


//	mat[0] = roboczyO->r/100;    
//	mat[1] = roboczyO->g/100;    
//	mat[2] = roboczyO->b;    

    glMaterialfv (GL_FRONT, GL_AMBIENT, mat);     
  glMaterialfv (GL_FRONT, GL_SPECULAR, mat);     
glMaterialf (GL_FRONT, GL_SHININESS, 0.2*128.0);     


    glTranslatef(roboczyO->r1.x,roboczyO->r1.y,roboczyO->r1.z);
    glPushMatrix();

	if(roboczyO->MaterialPoint)
	{

			if(roboczyO->flaga & O_RYS) 
			{
				glColor3f(0,0,0);
			 	glutWireSphere(0.03,8,8);
//			glutSolidCube(0.07);
//				glutSolidSphere(roboczyO->R,16,16);
			}
	}
	else
	{
	 	glColor3f(roboczyO->r,roboczyO->g,roboczyO->b);
	 	glutWireSphere(roboczyO->R,24,24);
//		glutSolidSphere(roboczyO->R,24,24);
	}


    glTranslatef(-roboczyO->r1.x,-roboczyO->r1.y,-roboczyO->r1.z);
    glPushMatrix();


//	glPopMatrix();      

	}

    mat[0] = 0.1745; 
mat[1] = 0.01175; 
mat[2] = 0.01175;    
     glMaterialfv (GL_FRONT, GL_AMBIENT, mat);     
  glMaterialfv (GL_FRONT, GL_SPECULAR, mat);     
glMaterialf (GL_FRONT, GL_SHININESS, 0.6*128.0);     


}


void CartesianVectors(void)
{

#define pointersize 0.4
#define pointerwingsize 0.2*pointersize

	glLineWidth(4);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

		// x		

		glColor3f(1.0,0.0,0.0);
		glVertex3f(0.0,0,0.0);
		glVertex3f(pointersize,0,0.0);

		glVertex3f(pointersize,0,0.0);
		glVertex3f(pointersize-pointerwingsize,0+pointerwingsize,0.0);

		glVertex3f(pointersize,0,0.0);
		glVertex3f(pointersize-pointerwingsize,0-pointerwingsize,0.0);


		// y		

		glColor3f(0.0,1.0,0.0);
		glVertex3f(0,0,0.0);
		glVertex3f(0,pointersize,0.0);

		glVertex3f(0,pointersize,0.0);
		glVertex3f(0-pointerwingsize,pointersize-pointerwingsize,0.0);

		glVertex3f(0,pointersize,0.0);
		glVertex3f(0+pointerwingsize,pointersize-pointerwingsize,0.0);

		// z		

		glColor3f(0.0,0.0,1.0);
		glVertex3f(0,0,0.0);
		glVertex3f(0,0,pointersize);

		glVertex3f(0,0,pointersize);
		glVertex3f(0-pointerwingsize,0+pointerwingsize,pointersize-pointerwingsize);

		glVertex3f(0,0,pointersize);
		glVertex3f(0+pointerwingsize,0-pointerwingsize,pointersize-pointerwingsize);

	glEnd();

	glEnable(GL_LIGHTING);

}



Vector3 v1=Vector3(xmin,ymax,zmin);
Vector3 v2=Vector3(xmin,ymax,zmax);
Vector3 v3=Vector3(xmax,ymax,zmax);
Vector3 v4=Vector3(xmax,ymax,zmin);

Vector3 box[]={v1,v2,v3,v4};

void MyBox(void)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
//	glColor3f(0.44,0.53,0.62);


	glTexCoord2f(0,0);
   	glVertex3f(box[0].x,box[0].y,box[0].z);
	glTexCoord2f(0,1);
   	glVertex3f(box[1].x,box[1].y,box[1].z);
	glTexCoord2f(1,1);
   	glVertex3f(box[2].x,box[2].y,box[2].z);
	glTexCoord2f(1,0);
   	glVertex3f(box[3].x,box[3].y,box[3].z);

	glNormal3f(0,1,0);
	glTexCoord2f(0,0);
	glVertex3f(xmin,ymin,zmin);
	glTexCoord2f(1,0);
	glVertex3f(xmax,ymin,zmin);
	glTexCoord2f(1,1);
	glVertex3f(xmax,ymin,zmax);
	glTexCoord2f(0,1);
	glVertex3f(xmin,ymin,zmax);

	glNormal3f(0,1,0);
	glTexCoord2f(0,0);
	glVertex3f(xmin,ymin,zmin);
	glTexCoord2f(1,0);
	glVertex3f(xmin,ymin,zmax);
	glTexCoord2f(1,1);
	glVertex3f(xmin,ymax,zmax);
	glTexCoord2f(0,1);
	glVertex3f(xmin,ymax,zmin);

	glNormal3f(0,1,0);
	glTexCoord2f(0,0);
	glVertex3f(xmax,ymin,zmin);
	glTexCoord2f(1,0);
	glVertex3f(xmax,ymax,zmin);
	glTexCoord2f(1,1);
	glVertex3f(xmax,ymax,zmax);
	glTexCoord2f(0,1);
	glVertex3f(xmax,ymin,zmax);



	glNormal3f(0,1,0);
	glTexCoord2f(0,0);
	glVertex3f(xmin,ymin,zmin);
	glTexCoord2f(1,0);
	glVertex3f(xmin,ymax,zmin);
	glTexCoord2f(1,1);
	glVertex3f(xmax,ymax,zmin);
	glTexCoord2f(0,1);
	glVertex3f(xmax,ymin,zmin);


	glNormal3f(0,1,0);
	glTexCoord2f(0,0);
	glVertex3f(xmin,ymin,zmax);
	glTexCoord2f(1,0);
	glVertex3f(xmax,ymin,zmax);
	glTexCoord2f(1,1);
	glVertex3f(xmax,ymax,zmax);
	glTexCoord2f(0,1);
	glVertex3f(xmin,ymax,zmax);

	glEnd();

	glEnable(GL_LIGHTING);

}









void GLWypisz(float x, float y, char *string)
{
  int len, i;

  glRasterPos3f(x, y,0);
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(font, string[i]);
  }
}





float move=0;
void Draw(void)
{
	aspect=(double)szer/(double)wys;


//	CreateTexture();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND); 
	glClearColor (0.3, 0.31, 0.32, 1.0);


  

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);   
	glLoadIdentity();  



	glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	

	gluPerspective(50.,aspect, .1, 100.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.,0.,1.,0.,0.,0.,.0,.1,.0);


	glPushMatrix();
	glScalef(sx,sy,sz);
    glTranslatef(tr_x,tr_y,tr_z);
	glRotated(anglex,1,0,0);
	glRotated(angley,0,1,0);
	glRotated(anglez,0,0,1);
	


/*	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D,Reflection);
	MyBox();
*/ 
	glColor4f(1,1,1,1);
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
//	glDisable(GL_DEPTH_TEST); 
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture1);
//	MyBox();



/*	glFrontFace(GL_CW); 
    glEnable(GL_AUTO_NORMAL); 
    glEnable(GL_NORMALIZE);   
	glEnable(GL_FOG);   
    GLfloat fogColor[4] = {0.7, 0.7, 0.7, 0.9}; 
    glFogi (GL_FOG_MODE, GL_EXP);
    glFogfv (GL_FOG_COLOR, fogColor); 
    glFogf (GL_FOG_DENSITY, 0.45); 
    glHint (GL_FOG_HINT, GL_DONT_CARE); 
*/

// 2 - oswietlenie deLamberta
// 3 - teksturowanie

	cloth1.DisplayGL(3);	
//	cloth2.DisplayGL(3);	
//	cloth3.DisplayGL(3);	


    glutSwapBuffers();

	if(saveanim)
	{
		frame+=1;
		if(!(frame%5))
		WindowDump(szer,wys);
	}


}


//
// rotate backwards - gravity vector & model
// 
//

void update_angle(void)
{
	double x,z;

//	return;
	
	Graw = GrawBAK;

	// x
	Graw.y=GrawBAK.y*cos(-(M_PI/180.0f)*anglex)-GrawBAK.z*sin(-(M_PI/180.0f)*anglex);
	Graw.z=GrawBAK.y*sin(-(M_PI/180.0f)*anglex)+GrawBAK.z*cos(-(M_PI/180.0f)*anglex);

	z=Graw.z;	// potrzebne skladowanie tego
	// y
	Graw.z= z*cos(-(M_PI/180.0f)*angley)-GrawBAK.x*sin(-(M_PI/180.0f)*angley);
	Graw.x= z*sin(-(M_PI/180.0f)*angley)+GrawBAK.x*cos(-(M_PI/180.0f)*angley);

	x=Graw.x;

	// z
	Graw.x=x*cos(-(M_PI/180.0f)*anglez)-Graw.y*sin(-(M_PI/180.0f)*anglez);
	Graw.y=x*sin(-(M_PI/180.0f)*anglez)+Graw.y*cos(-(M_PI/180.0f)*anglez);

} 

void Key(unsigned char key, int x, int y)
{
	int i,j;
    switch (key) {


	case 'p':
		if(SoftBodydrawmode==4)
			SoftBodydrawmode=2;
		else
			SoftBodydrawmode=4;
		break;
	case 'o':
		if(lighting==0)
			lighting=1;
		else
			lighting=0;
		break;

	case 'i':
		if(texturing==0)
			texturing=1;
		else
			texturing=0;
		break;


	case ';':
		tr_y += 0.02;
		break;
	case '.':
		tr_y -= 0.02;
		break;
	case '/':
		tr_x += 0.02;
		break;
	case ',':
		tr_x -= 0.2;
		break;

	case 'k':
		tr_z += 0.02;
		break;
	case 'l':
		tr_z -= 0.02;
		break;


	case '+':
		sx+=0.003;
		sy+=0.003;
		sz+=0.003;

		break;
	case '-':
		sx-=0.003;
		sy-=0.003;
		sz-=0.003;

		break;

	case '8':
		anglex+=1;
		update_angle();
		break;
	case '2':	
		anglex-=1;
		update_angle();
		break;
	case '6':
		angley+=1;
		update_angle();
		break;
	case '4':
		angley-=1;
		update_angle();
		break;
	case '7':
		anglez+=1;
		update_angle();
		break;
	case '9':
		anglez-=1;
		update_angle();
		break;
	case '5':
		anglex=angley=anglez=0;
		sx=sy=sz=0.008;
		update_angle();
		break;



	case 'd':

	light_pos0[2]+=0.01;

	break;


	case 'f':

	light_pos0[2]-=0.01;

	break;

	case ' ':




	break;
	case 's':
		if(!saveanim)
			saveanim=1;
		else
			saveanim=0;
		break;
	case 'a':
		saveanim=0;
		break;

	case 'z':

//	for(i=0;i<cloth1.nx;i++)
//	{
//		cloth1.Points[i][0]->flaga &= ~P_ZAW;
//		cloth1.Points[i][cloth1.ny-1]->flaga &= ~P_ZAW;
//	}
//	for(j=0;j<cloth1.ny;j+=1)
//	{
//		cloth1.Points[0][j]->flaga &= ~P_ZAW;
//		cloth1.Points[cloth1.nx-1][j]->flaga &= ~P_ZAW;
//	}

		break;
	case 'g':

		if(Graw.length())
			Graw = Vector3(0,0,0);
		else
			Graw = GrawBAK;
		break;

	case 'h':

		if(Graw.length())
			Graw = Vector3(0,0,0);
		else
			Graw = Vector3(50,-50,0);
		break;

	
	case 'w':
//Vector3 Wind(0.0,0,8);

		if(!Wind.length())
			Wind = Vector3(0,0,3);
		else
			Wind = Vector3(0,0,0);
	break;


	case 27:
		exit(0);	
	
	break;
	
      default:
	return;
        }

    glutPostRedisplay();
}



void Init(void)
{
    glClearColor(0.2, 0.1, 0.0, 1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

	glViewport(0, 0, szer, wys);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluOrtho2D(-xmin, xmax, -ymin, ymax);
    glMatrixMode(GL_MODELVIEW);

}



void Reshape(int width, int height)
{

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    gluOrtho2D(-xmin, xmax, -ymin, ymax);
//    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}





#define FALSE 0
#define TRUE 1

int WindowDump(int wwidth, int wheight)
{
   int i,j;
   FILE *fptr;
   static int counter = 0; /* This supports animation sequences */
   char fname[32];
   unsigned char *image;

   /* Allocate our buffer for the image */
   if ((image = (unsigned char *) malloc(3*wwidth*wheight*sizeof(char))) == NULL ) {
      fprintf(stderr,"Failed to allocate memory for image\n");
      return(FALSE);
   }

   glPixelStorei(GL_PACK_ALIGNMENT,1);
   sprintf(fname,"frames/frame%04d.ppm",counter);

   if ((fptr = fopen(fname,"w")) == NULL) {
      fprintf(stderr,"Failed to open file for window dump\n");
      return(FALSE);
   }

   /* Copy the image into our buffer */
	glReadBuffer(GL_BACK_LEFT);
	glReadPixels(0,0,wwidth,wheight,GL_RGB,GL_UNSIGNED_BYTE,image);

   fprintf(fptr,"P6\n%d %d\n255\n",wwidth,wheight); /*for ppm */
   for (j=wheight-1;j>=0;j--) {
      for (i=0;i<wwidth;i++) {
         fputc(image[3*j*wwidth+3*i+1],fptr);
         fputc(image[3*j*wwidth+3*i+2],fptr);
         fputc(image[3*j*wwidth+3*i+3],fptr);
      }
   }

   fclose(fptr);

   /* Clean up */
   counter++;
   free(image);
   return(TRUE);
}



void idle(void)
{
/*
	anglex+=PI/80;
	angley+=PI/72;
	anglez+=PI/61;
	update_angle();
*/
	Timestep();

    glutPostRedisplay();
}



    
	








    
	
