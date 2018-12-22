/*********************************************************************
 * main.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Interfaces with glut, reads in some parameters.  Nothing special.
 * Copyright 2003, Regents of the University of California 
 *
 *********************************************************************/

/**************************************************************************
 * Copyright (c) 2003, University of California at Berkeley
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the University of California nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>

#include "UI.h"

#define ARRAYSIZE(x) sizeof(x)/sizeof(x[0])

//#define TIMER_DELAY 12
#define TIMER_DELAY 0

void init(void);
void timer(int);
void update();
void display(void);
void reshape(int w, int h);
void key(unsigned char k, int x, int y);
void mouse(int button, int state, int x, int y);
void mousemove(int x, int y);
void special(int key, int x, int y);

DeformMain* deform;

static int old_x = 320, old_y = 240;
int button_clicked = -1;

const char* OPTIONS_STRING = "Options:\n\
\t-scene [scene]: sets the scene file.  Default is \"scene.setup\".\n\
\t-v: verbose.  v[n] (where [n] is a digit) or vv...v (n times) increases the verbosity.\n\
\t-drawconst: toggle draw constraints (default false).\n\
\t-drawpen: toggle draw penetration depths (default false).\n\
\t-drawanim: toggle draw animations (default true). \n\
\t-help: show help.\n\
";

void showhelp() {
  printf("------Arguments-----\n");
  printf(OPTIONS_STRING);
  printf("\n");
  printf("------Runtime commands-----\n");
  printf("Navigation:\n");
  printf("Right mouse button rotates the camera.\n");
  printf("Left mouse button translates the camera.\n");
  printf("Middle mouse button zooms the camera.\n");
  printf("\n");
  printf("Space key starts/stops the animation\n");

}

int verbose = 0;
const char* scene_file = "scene.setup";
bool draw_penetration = false;
bool draw_animation = true;
bool draw_constraints = false;

//#include "ps2vector.h"
//#include "ps2matrix.h"


int
main(int argc, char **argv)
{
  int i;
  for(i=1; i<argc; i++) {
    if(argv[i][0] == '-') {
      if(0 == strcmp(argv[i], "-scene")) {
	scene_file = argv[i+1];
	i++;
      }
      else if(0 == strcmp(argv[i], "-drawpen")) {
	draw_penetration = true;
      }
      else if(0 == strcmp(argv[i], "-drawconst")) {
	draw_constraints = true;
      }
      else if(0 == strcmp(argv[i], "-drawanim")) {
	draw_animation = true;
      }
      else if(0 == strcmp(argv[i], "-help")) {
	showhelp();
	exit(0);
      }
      else if(argv[i][1] == 'v') {
	if(isdigit(argv[i][2]))
	  verbose = argv[i][2] - '0';
	else {
	  for(int j=1; j<strlen(argv[i]); j++)
	    if(argv[i][j] == 'v')
	      verbose++;
	}
      }
      else {
	printf("Invalid option: %s\n", argv[i]);
	exit(-1);
      }
    }
    else
      break;
  }

   glutInit(&argc, argv);
   glutInitWindowSize(640,480);
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
   glutCreateWindow("Deform");

  
   //ps2vector::self_test();
   //ps2matrix::self_test();

   // need to set up the gl context with glutInit before calling init()
   init();

   glutReshapeFunc(reshape);
#if TIMER_DELAY
   glutTimerFunc(TIMER_DELAY, timer, 0);
#else
   glutIdleFunc(update);
#endif
   glutDisplayFunc(display);
   glutKeyboardFunc(key);
   glutMouseFunc(mouse);
   glutMotionFunc(mousemove);
   glutSpecialFunc(special);

   glutMainLoop();

   return 0;
}

static GLfloat light0_pos[] = {-0.7, 0.9f, -0.7, 0};
static GLfloat light1_pos[] = {-100, 200, 600, 1.0};
static GLfloat light2_pos[] = {100, 500, -100, 1.0};

void init_lights() {
  // lights
  GLfloat ambient[] = {0.09, 0.09, 0.09, 1.0};

  GLfloat light0_diffuse[] = {0.7f, 0.7f, 0.7f, 0 };
  GLfloat light0_specular[] = {0.5f, 0.5f, 0.5f, 0 };
  GLfloat light1_diffuse[] = {0.5f, 0.5f, 0.5f, 0.0f };
  GLfloat light1_specular[] = {0.2f, 0.2f, 0.2f, 0.0f };
  GLfloat light2_diffuse[] = {0.2f, 0.2f, 0.2f, 0.0f };

  GLfloat black[] = { 0, 0, 0, 0 };

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  //glEnable(GL_LIGHT2);

  // a directional light

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

  // a point light

  glLightfv(GL_LIGHT1, GL_AMBIENT, black);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0f);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0005f);
  //glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0f);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);

  glLightfv(GL_LIGHT2, GL_AMBIENT, black);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
  glLightfv(GL_LIGHT2, GL_SPECULAR, black);

  glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.0f);
  glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.0005f);
  glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0f);
}

void set_light_positions() {
  glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
  glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
}

#define MODELPREFIX "models/"

const char* models  [] = {
  "cube3x3",
  /*"cube3x3h",
  "tube",
  "torus",
  "tetra",
  "sphere",
  "mushroom",
  "mushroom2",
  "dodo",
  //"bimbojubblies",
  "sheet",
  //"bat3",
  //"sheep",*/
};


void
init(void)
{
  deform = new DeformMain;

  init_lights();

   glClearColor(0.3f, 0.3f, 0.4f, 0.0f);

   glEnable(GL_DEPTH_TEST);
   glEnable( GL_RESCALE_NORMAL );

   glEnable(GL_LINE_SMOOTH);

/*
   char buf[256];
   for(int i=0; i<ARRAYSIZE(models); i++) {
     sprintf(buf, "%s%s", MODELPREFIX, models[i]);
     if(!deform->LoadObject(buf))
       exit(1);
   }
*/   
   printf("Load successful\n");
}


void draw_axes() {
  glDisable(GL_LIGHTING);

  glColor3f(1,0,0);
  glBegin(GL_LINES);
  glVertex3f(0,0,0);
  glVertex3f(1,0,0);
  glEnd();

  glColor3f(0,1,0);
  glBegin(GL_LINES);
  glVertex3f(0,0,0);
  glVertex3f(0,1,0);
  glEnd();

  glColor3f(0,0,1);
  glBegin(GL_LINES);
  glVertex3f(0,0,0);
  glVertex3f(0,0,1);
  glEnd();
}

void
display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   set_light_positions();

   glEnable(GL_LIGHTING);

   deform->Draw();

   glDisable(GL_LIGHTING);

   glDisable( GL_TEXTURE_2D );

   //if(!save_movie)
   //draw_axes();

   glLoadIdentity();

   glFlush();

   glutSwapBuffers();
}

bool nav [4] = { 0,0,0,0 };

void timer(int)
{
  update();
  glutTimerFunc(TIMER_DELAY, timer, 0);
}

void update()
{
  //ps2matrix::speed_test();
  deform->Update();
  deform->Nav(nav[0], nav[1], nav[2], nav[3]);
  nav[0] = nav[1] = nav[2] = nav[3] = false;
  glutPostRedisplay();
}

void
perspective( float fov, float aspect, float nearClip, float farClip )
{
   float w, h;
   fov *= 3.141592654f/180.0f;
   h = 2.0f * nearClip * (float)tanf( fov/2.0f );
   w = h * aspect;

   glFrustum( -w/2.0f, w/2.0f, -h/2.0f, h/2.0f, nearClip, farClip );
}

void
reshape(int w, int h)
{
   glViewport(0, 0, w, h);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   perspective( 40.0f, (float)w/(float)h, 1.0f, 4000.0f );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{
	old_x = x;
	old_y = y;
	deform->MoveTo(x,480-y);

	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		if(state == GLUT_UP) deform->Click(0, 0);
		else deform->Click(0, 1);
		break;
	case GLUT_RIGHT_BUTTON:
		if(state == GLUT_UP) deform->Click(1, 0);
		else deform->Click(1, 1);
		break;
	case GLUT_MIDDLE_BUTTON:
		if(state == GLUT_UP) deform->Click(2, 0);
		else deform->Click(2, 1);
		break;
	}
	if(state == GLUT_DOWN) button_clicked = button;
	else button_clicked = -1;
}

void
key(unsigned char k, int x, int y)
{
  deform->Key(k);
}

void mousemove(int x, int y)
{
	int dx,dy;
	dx = x-old_x;
	dy = -(y-old_y);
	old_x = x;
	old_y = y;

	int button = -1;
	switch (button_clicked) {
	case GLUT_LEFT_BUTTON:
	  button = 0;
	  break;
	case GLUT_MIDDLE_BUTTON:
	  button = 1;
	  break;
	case GLUT_RIGHT_BUTTON:
	  button = 2;
	  break;
	}

	deform->Drag(button, dx,dy);
}

void
special(int key, int x, int y)
{
  switch (key) {
  case GLUT_KEY_UP:
	  nav[0] = true;
	  break;
  case GLUT_KEY_DOWN:
	  nav[1] = true;
	  break;
  case GLUT_KEY_LEFT:
	  nav[2] = true;
	  break;
  case GLUT_KEY_RIGHT:
	  nav[3] = true;
	  break;
    // left shoulder
  case GLUT_KEY_HOME:
    deform->PrevMenu();
    break;
  case GLUT_KEY_END:
    deform->NextMenu();
    break;
    
    // right shoulder
  case GLUT_KEY_PAGE_UP:
    deform->PrevTool();
    break;
  case GLUT_KEY_PAGE_DOWN:
    deform->NextTool();
    break;
  }
}



