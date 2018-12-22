/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <glh/glh_genext.h>
#include <glh/glh_extensions.h>
#include <glh/glh_obs.h>
#include <glh/glh_nveb.h>

#include <stdio.h>
#include <stdlib.h>

#include "glinterface.h"
#include "mathlib.h"
#include "vector.h"
#include "texture.h"
#include "pbuffer.h"
#include "loadpgm.h"

#include "dyn_particle_map.h"

#define REQUIRED_EXTENSIONS "GL_ARB_texture_compression " \
                            "GL_ARB_vertex_program " \
                            "GL_NV_texture_rectangle " \
                            "GL_NV_vertex_program " \
                            "GL_NV_fragment_program " \
                            "GLX_NV_float_buffer " \
                            "GLX_SGIX_fbconfig " \
                            "GLX_SGIX_pbuffer "

#define WIDTH	720
#define HEIGHT	480

#define NPARTICLES 500
                                                                                
DYN_PARTICLE_MAP *DPMap;

/*
 */
class GLFireDemo : public OpenGLInterface {
public:
	
	int init();
	int idle();
	int render_blur();
	int render();
        void ScreenToTGA();
	
	float CameraRotx,CameraRoty;
	vec3 dir;
	mat4 modelview;
	
	int pause;
        int record;
	
	float time;
	
	PBuffer *pbuffer;
	
	GLuint final_fp;
	GLuint point_fp;
	
        /* Textures to index into to compute texture coordinate
           displacement */
	Texture *DispTex1;
	Texture *DispTex2;

       /* Texture for final rendering of particle streaks with blur */
	Texture *ReactionProgress;
     
       /* Texture for streaks of moving fire particles*/
	Texture *StreakTex;
	Texture *wood;
	
	GLuint src_id;
	GLuint comp_id;

};

void GLFireDemo::ScreenToTGA()
{	
	
//printf("Came Here to dump Screen\n");
    int i, x, y, w, h;
    static int num =0;
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    x = viewport[0]; y = viewport[1]; 
    w = viewport[2]; h = viewport[3];
    w = w - x;
    h = h - y;

     // Buffer
	unsigned char *buffer;	
	
	// Name of output file
	char OutFileName[50];

	// Size of buffer
	int buf_size = 18 + (w * h * 3);	// HEADER_SIZE	==> 18
	
	// Temp Variable 
	unsigned char temp;	
	
	// pointer to output file
	FILE *OutFile;	
	
	// File name
	sprintf(OutFileName,"Images/Fire_%04d.tga",num);

	// Open the file
	if (!(OutFile = fopen(OutFileName, "wb")))
		return;
	
	// Inclement the number of the screen shot
	num++;

	// Allocate memory for buffer
	if (!(buffer = (unsigned char *) calloc(1, buf_size)))
		return;
	
	// Informationfor header
	buffer[2] = 2;		
	buffer[12] = w & 255;	
	buffer[13] = w >> 8;	
	buffer[14] = h & 255;	
	buffer[15] = h >> 8;	
	buffer[16] = 24;
	
	// Read from Frame buffer
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer + 18);
	
	// Converte from RGB to BGR	
	for (i = 18; i < buf_size; i += 3)
		{
			temp = buffer[i];
			buffer[i] = buffer[i + 2];
			buffer[i + 2] = temp;
		}	
	
	// Write data into file
	fwrite(buffer, sizeof(unsigned char), buf_size, OutFile);	
	
	//close file
	fclose(OutFile);	
	
	//Free buffer
	free(buffer);

}

/*
 */
int GLFireDemo::init() {

        bool isLoaded;
	CameraRotx = 0;
	CameraRoty = 0;
	
	pause = 0;
	
	time = 0;
      if (!glh_init_extensions(REQUIRED_EXTENSIONS))
       isLoaded = false;
      else
       isLoaded = true;

	// shaders
	final_fp = load_program("data/post.fp",GL_FRAGMENT_PROGRAM_ARB);
	if(!final_fp) return -1;
	
	point_fp = load_program("data/line.fp",GL_FRAGMENT_PROGRAM_ARB);
	if(!point_fp) return -1;
	
	DispTex1 = new Texture("data/DispTex1.jpg");
	DispTex2 = new Texture("data/DispTex2.jpg");
	
	ReactionProgress = new Texture("data/ReactionProgress.png",Texture::TRILINEAR | Texture::MIPMAP_SGIS | Texture::CLAMP_TO_EDGE,Texture::TEXTURE_1D);
	StreakTex = new Texture("data/streak.png",Texture::LINEAR | Texture::MIPMAP_SGIS | Texture::CLAMP_TO_EDGE,Texture::TEXTURE_2D);
	wood = new Texture("data/wood.png",Texture::LINEAR | Texture::MIPMAP_SGIS | Texture::CLAMP_TO_EDGE,Texture::TEXTURE_2D);
	
	// pixel buffer
	pbuffer = new PBuffer(512,512);
	pbuffer->enable();
	glClearDepth(1);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	pbuffer->disable();
	
	// textures
	glGenTextures(1,&src_id);
	glBindTexture(GL_TEXTURE_2D,src_id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,512,512,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
	
	glGenTextures(1,&comp_id);
	glBindTexture(GL_TEXTURE_2D,comp_id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,512,512,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);

	srand(::time(NULL));
	
        DPMap= new DYN_PARTICLE_MAP(NPARTICLES);
        DPMap->Build_Particle_Map(3);
                                                                                
	return 0;
}

/*
 */
int GLFireDemo::render_blur() {

	pbuffer->enable();
	
	// source
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
	gluPerspective(45,(float)windowWidth / (float)windowHeight,0.1,200);
        glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelview);

        glLineWidth(2);
        glEnable(GL_LINE_SMOOTH);

	glActiveTexture(GL_TEXTURE0);
        StreakTex->bind();
        glEnable(GL_TEXTURE_2D);

        glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_SRC_ALPHA);

        for(int i = 0; i < NPARTICLES; i++) 
	{
         float col= DPMap->Composition[i];
         glColor4f(col,col/2.0,0,0.0);
         glBegin(GL_LINES);
	glTexCoord2f(col,col);
         glVertex3d(DPMap->Particles[i][0],DPMap->Particles[i][1],DPMap->Particles[i][2]);
	 glTexCoord2f(col*1.03, col*1.03);
         glVertex3d(DPMap->old_Particles[i][0],DPMap->old_Particles[i][1],DPMap->old_Particles[i][2]);
         glEnd();
        }
        glDisable(GL_BLEND);

	error();
	
	glBindTexture(GL_TEXTURE_2D,src_id);
	glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,512,512,0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,point_fp);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,src_id);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,DispTex1->id);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D,DispTex2->id);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D,comp_id);
	
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0,0);
	glVertex2f(-1,-1);
	glTexCoord2f(1,0);
	glVertex2f(1,-1);
	glTexCoord2f(0,1);
	glVertex2f(-1,1);
	glTexCoord2f(1,1);
	glVertex2f(1,1);
	glEnd();
	
	glActiveTexture(GL_TEXTURE4);

	glDisable(GL_FRAGMENT_PROGRAM_ARB);

	glBindTexture(GL_TEXTURE_2D,comp_id);
	glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,512,512,0);
	
	pbuffer->disable();
	
	return 0;
}

/*
 */
int GLFireDemo::render() {
	
	render_blur();
	
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// add blured texture
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,final_fp);
	
	glBindTexture(GL_TEXTURE_2D,comp_id);
	glActiveTexture(GL_TEXTURE1);
	ReactionProgress->bind();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
	
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0,0);
	glVertex2f(-1,-1);
	glTexCoord2f(1,0);
	glVertex2f(1,-1);
	glTexCoord2f(0,1);
	glVertex2f(-1,1);
	glTexCoord2f(1,1);
	glVertex2f(1,1);
	glEnd();
	
	glDisable(GL_BLEND);
	
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
	
	error();

//Wood
        static float dark=1.0;
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE);
	glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
	gluPerspective(45,(float)windowWidth / (float)windowHeight,0.1,200);
        glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelview);
	glActiveTexture(GL_TEXTURE0);
        wood->bind();
        glEnable(GL_TEXTURE_2D);

        glColor3f(0.4*dark,0.35*dark,0.1*dark);

        GLUquadric *q = gluNewQuadric();
        gluQuadricTexture(q,GL_TRUE);

        glRotatef(90.0,0.0,1.0,0.0);
        glScalef(5.0,0.1,1.0);
        glTranslatef(0.0,1.0,-2.5);
        gluCylinder(q,0.5,0.5,5.0,10,2);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        dark*=0.999;

        glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
        glDisable(GL_BLEND);

// Print the frame rate

       	glColor3f(1,0,0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

        glRasterPos3f(-0.95,0.9,0);
        char c[512];
	sprintf(c,"Frames per second = %.0f \0",fps);
        char *tc;
        for(tc=c; *tc!='\0';tc++) 
	{
       	 glColor3f(1,0,0);
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*tc);
	}
        glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
       	glColor3f(0,0,0);

        if(record)
	  ScreenToTGA();	

	return 0;
}

/*
 */
int GLFireDemo::idle() {
	
	time += ifps;
        static int fn=0;
	
	if(!pause) {
                float *loc;
                loc= (float*)malloc(3*sizeof(float));
                if(loc==NULL)printf("we have memory problem for loc\n");
                                                                                
                /* Dynamics update of the paticles in the paticle map*/
                DPMap->Create_Memory_for_New_Particles(NPARTICLES);
                VECTOR tvec;
                                                                                
		for(int idx=0;idx<NPARTICLES;idx++)
                {
                  loc[0]=DPMap->Particles[idx][0];
                  loc[1]=DPMap->Particles[idx][1];
                  loc[2]=DPMap->Particles[idx][2];
                  tvec.x=loc[0]; tvec.y=loc[1]; tvec.z=loc[2];
                                                                                
                  int *num = DPMap->kdOptNNQuery(3, loc, 40, 5.5, idx);

                  if(num[0]!=0)
                    {
                      float dt=DPMap->Composition[idx]*0.7;
                      DPMap->Update_Dynamics(num,dt,idx);
                    }
                }
                 DPMap->Exchange_Updated_Particles();
	}

	fn++;
	// keyboard events
	if(keys[KEY_ESC]) exit();
	
	if(keys[KEY_PAUSE]) {
		pause = !pause;
		keys[KEY_PAUSE] = 0;
	}

	if(keys[KEY_THRESH]) {
		DPMap->theta -=0.05;
		if(DPMap->theta <0.1)
                 {
                  DPMap->theta = 0.1; 
                  DPMap->chi=1.0;
                 }
		keys[KEY_THRESH] = 0;
	}
	
	if(keys[KEY_RESET]) {
		DPMap->theta =0.4;
	}
	
	if(keys[KEY_HELP]) {
           printf(" This program demostrates the working of the algorithm described \n in chapter Grid-less Controllable Fire of Games Programming Gems 5 \n");
           printf(" \n Left mouse button allows turning the fire\n");
           printf(" Middle mouse button allows zooming in and out \n");
           printf(" \n Space bar pauses the animation\n");
           printf(" \n t decreases frequency of flicker and increases flame height\n");
           printf(" \n x resets threshold for flicker \n");
           printf(" h key results in this message being displayed\n");
           printf(" \n r key starts dumping frames as tga files\n in the directory Images\n Hit r again to stop dumping files \n\n");
           printf(" Esc key ends program\n");
           keys[KEY_HELP]=0;
	}
	
	if(keys[KEY_REC]) {
		record = !record;
		ScreenToTGA();	
		keys[KEY_REC] = 0;
	}
	
	// mouse events
	static int look = 0;
	
	if(!look && mouseButton & BUTTON_LEFT) {
		setPointer(windowWidth / 2,windowHeight / 2);
		mouseX = windowWidth / 2;
		mouseY = windowHeight / 2;
		look = 1;
	}
	if(mouseButton & BUTTON_RIGHT) look = 0;
	
	if(look) {
		showPointer(0);
		CameraRotx -= (mouseX - windowWidth / 2) * 0.1;
		CameraRoty += (mouseY - windowHeight / 2) * 0.1;
		if(CameraRoty < -20) CameraRoty = -20;
		if(CameraRoty > 20) CameraRoty = 20;
		setPointer(windowWidth / 2,windowHeight / 2);
	} else showPointer(1);
	
	
        static float cameradist=20.0f;
        if(mouseButton & BUTTON_UP) cameradist -= 5;
        if(mouseButton & BUTTON_DOWN) cameradist += 5;
        if(cameradist > 100) cameradist = 100;
        if(cameradist < 1) cameradist = 1;

	quat qx,qy,qfinal;

	qx.set(vec3(0,1,0),CameraRotx);
	qy.set(vec3(0,1,0),CameraRoty);

	qfinal = qy * qx;
        
	mat4 m0 = qfinal.to_matrix();
	vec3 camera = m0 * vec3(0,0,cameradist)+vec3(0,5,0);
	modelview.look_at(camera,vec3(0,5,0),vec3(0,1,0));
	
	return 0;
}

/*
 */
int main(int argc,char **argv) 
{
    GLFireDemo *FireDemo = new GLFireDemo;
	
    int fullscreen = 0;
    if(argc > 1) 
    {
	if(!strcmp(argv[1],"-fs")) fullscreen = 1;
	else if(!strcmp(argv[1],"-h")) 
        {
         printf("Gridless Fire Demo - use esc key to terminate program\n");
         return 0;
	}
     }
	
     FireDemo->setVideoMode(WIDTH,HEIGHT,fullscreen);
     FireDemo->setTitle("Realtime Gridless Fire Demostration Program!");
	
     if(!FireDemo->init()) FireDemo->main();
	
     return 0;
}
