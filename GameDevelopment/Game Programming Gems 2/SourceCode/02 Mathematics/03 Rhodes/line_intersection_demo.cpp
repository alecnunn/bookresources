/* Copyright (C) Graham Rhodes, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Graham Rhodes, 2001"
 */
/**************************************************************************************
|
|           File: line_intersection_demo.cpp
|
|        Purpose: Test program to demonstrate use of line segment intersection utility functions
|
|     Book Title: Game Programming Gems II
|
|  Chapter Title: Fast, Robust Intersection of 3D Line Segments
|
|         Author: Graham Rhodes
|
|      Revisions: 05-Apr-2001 - GSR. Original.
|
**************************************************************************************/

#include <stdio.h>
#include <gl\glut.h>
#include <gl\glu.h>
#include <gl\gl.h>
#include <math.h>
#include "lineintersect_utils.h"

// pragma to get rid of GLUT inline and local function removal warnings.
#pragma warning(disable:4514)
#pragma warning(disable:4505)

void paintcallback();
void idlecallback();
void keycallback(unsigned char key, int x, int y);
void mousecallback(int button, int state, int x, int y);
void motioncallback(int x, int y);
void reshapecallback(int width, int height);

void PositionCamera();
void DrawUnitCube();
void DrawGroundPlane();
void PrepareShadow();

void InitParallelLines();
void InitNonparallelLines();
void InitLines();

void Help();

float A[6];
float B[6];
float IA[6] = {0.001f, -0.01f, 0.001f, -0.01f, 0.01f, -0.003f};
float IB[6] = {0.005f, 0.0075f, 0.002f, 0.002f, 0.005f, -0.01f};
float Intersection[] = {0.0, 0.0, 0.0};
float Vector[] = {0.0, 0.0, 0.0};
bool true_intersection = false;;
int count = 0, frame = 0;
float elevation = 0.f;
float azimuth = 0.f;
float g = 32.2;
float ftemp[6];

int mouse_x = 0, mouse_y = 0;
bool trackball = false;
bool paused = true;
bool trueintersect = false;
bool temp = false;
bool infinite_lines = false;

void DrawBox();

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL);

  glutInitWindowSize(800, 600);
  glutCreateWindow("Line Segment Intersection Demo");

  glutReshapeFunc(reshapecallback);
  glutDisplayFunc(paintcallback);
  glutKeyboardFunc(keycallback);
  glutIdleFunc(idlecallback);
  glutMouseFunc(mousecallback);
  glutMotionFunc(motioncallback);

  Help();

  InitLines();
  InitNonparallelLines();

  glutMainLoop();

	return 0;
}

void paintcallback()
{
  glDrawBuffer(GL_BACK);
  glMatrixMode(GL_MODELVIEW);
  glDisable(GL_LIGHTING);
  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);
  glDisable(GL_CLIP_PLANE3);
  glEnable(GL_CULL_FACE);
  glDisable(GL_STENCIL_TEST);
  glCullFace(GL_BACK);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glFrontFace(GL_CCW);

  glClearColor(1.0f,1.0f,1.0f,0.0f);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
 
  PositionCamera();

  glEnable(GL_DEPTH_TEST);
  DrawBox();

// draw the two line segments
  glLineWidth(4.0);
  glBegin(GL_LINES);
    glColor4f(0.75f,0.f,0.75f,1.0f);
    glVertex3f(A[0], A[1], A[2]);
    glVertex3f(A[3], A[4], A[5]);
    glColor4f(0.f,.75f,.75f,1.0f);
    glVertex3f(B[0], B[1], B[2]);
    glVertex3f(B[3], B[4], B[5]);
  glEnd();

  glColor4f(0.0f, 0.2f, 1.0f, 1.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glPointSize(10.0);
  IntersectLineSegments(A[0], A[1], A[2], A[3], A[4], A[5],
                        B[0], B[1], B[2], B[3], B[4], B[5],
                        infinite_lines, 1.e-6, ftemp[0], ftemp[1], ftemp[2],
                        ftemp[3], ftemp[4], ftemp[5],
                        Intersection[0], Intersection[1], Intersection[2],
                        Vector[0], Vector[1], Vector[2], true_intersection);

// draw vector between two nearest points
  glLineWidth(1.0);
  glBegin(GL_LINES);
    glColor4f(0.5f,0.5f,0.5f,1.0f);
    glVertex3f(ftemp[0], ftemp[1], ftemp[2]);
    glVertex3f(ftemp[3], ftemp[4], ftemp[5]);
  glEnd();

// draw midpoint between nearest point, aka "the intersection point"
  glBegin(GL_POINTS);
    if (true_intersection)
    {
      glColor4f(0.f,1.f,0.f,1.0f);
    }
    else
    {
      glColor4f(1.f,0.f,0.f,1.0f);
    }
    glVertex3d(Intersection[0], Intersection[1], Intersection[2]);
  glEnd();

  glFlush();

  glutSwapBuffers();
}

void PositionCamera()
{
  glLoadIdentity();
  gluLookAt(0.0, 0.0, -3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glRotatef(elevation, 1.0f, 0.0f, 0.0f);
  glRotatef(azimuth, 0.0f, 1.0f, 0.0f);
}

void idlecallback()
{
  if (paused == false)
  {
    for (int i=0; i<6;i++)
    {
      A[i] += IA[i];
      if (A[i] < -0.5 || A[i] > 0.5) IA[i] = -IA[i];
      B[i] += IB[i];
      if (B[i] < -0.5 || B[i] > 0.5) IB[i] = -IB[i];
    }
    if (trueintersect)
    {
      A[1] = A[4] = B[1] = B[4] = 0.0;
    }
  }
  glutPostRedisplay();
}

void keycallback(unsigned char key, int x, int y)
{
  x; y; // to get rid of unused formal parameter warning
  if (key == 'q')
  {
    exit(0);
  }
  else if (key == 'r' || key == 'r')
  {
    paused = paused ? false : true;
  }
  else if (key == 'p' || key == 'P')
  {
    InitParallelLines();
  }
  else if (key == 'n' || key == 'N')
  {
    InitNonparallelLines();
  }
  else if (key == 't' || key == 'T')
  {
    trueintersect = trueintersect ? false : true;
  }
  else if (key == 'i' || key == 'I')
  {
    infinite_lines = infinite_lines ? false : true;
  }
  else if (key == '?') Help();
}

void DrawBox()
{
  glLineWidth(1.0);
//  glColor4f(0.15f, 0.55f, 0.75f,1.0f);
  glColor4d(0.95f, 0.95f, 0.75f,1.0f);
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0, 1.0);
  DrawUnitCube();
  glColor4d(0.f, 0.f, 0.f,1.0f);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glDisable(GL_CULL_FACE);
  DrawUnitCube();
}

void mousecallback(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    trackball = true;
    mouse_x = x;
    mouse_y = y;
  }
  else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) trackball = false;
}

void motioncallback(int x, int y)
{
  if (false == trackball) return;
  float dazimuth = ((float)(x - mouse_x))/1.0f;
  float delevation = ((float)(y - mouse_y))/1.0f;
  azimuth += dazimuth;
  elevation -= delevation;
  mouse_x = x;
  mouse_y = y;
  glutPostRedisplay();
}

void reshapecallback(int width, int height)
{
  double gldAspect=(GLdouble)width/(GLdouble)height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glFrustum(-0.75, 0.75, -0.75/gldAspect, 0.75/gldAspect, 1.5, 25.0);

  glViewport(0, 0, width, height);
}

void DrawUnitCube()
{
  glBegin(GL_QUADS);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);

    glVertex3f(0.5f, -0.5f,  0.5f);
    glVertex3f(0.5f,  0.5f,  0.5f);
    glVertex3f(0.5f,  0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
  glEnd();

// turn on stencil buffer writing for floor----used during shadow drawing later
// now draw the floor
  glBegin(GL_QUADS);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
  glEnd();
  glColor4f(0.0, 0.0, 0.0, 1.0f);
  glDisable(GL_STENCIL_TEST);
}

void InitParallelLines()
{
  InitLines();
  for (int i = 0; i < 6; i++) IA[i] = IB[i] = 0.0f;
  IA[0] = .015f;
  IA[3] = -.003f;
  IB[0] = -.01f;
  IB[3] = .00125f;
}

void InitLines()
{
  A[0] = -0.5f;
  A[1] = -0.5f;
  A[2] = 0.f;
  A[3] = 0.f;
  A[4] = -0.5f;
  A[5] = 0.f;
  B[0] = 0.25f;
  B[1] = 0.f;
  B[2] = 0.f;
  B[3] = 0.5f;
  B[4] = 0.f;
  B[5] = 0.f;
}

void InitNonparallelLines()
{
  IA[0] = 0.001f;
  IA[1] = -0.01f;
  IA[2] = 0.001f;
  IA[3] = -0.01f;
  IA[4] = 0.01f;
  IA[5] = -0.003f;
  IB[0] = 0.005f;
  IB[1] = 0.0075f;
  IB[2] = 0.002f;
  IB[3] = 0.002f;
  IB[4] = 0.005f;
  IB[5] = -0.01f;
}

void Help()
{
  printf("\nLine Intersection Demo\nby Graham Rhodes\n\n");
  printf("Keyboard Commands\n-----------------");
  printf("Q = Quit\n");
  printf("R = Start/stop animation of line segments\n");
  printf("P = Start parallel line mode\n");
  printf("N = Start nonparallel line mode\n");
  printf("T = Start planar line mode\n");
  printf("I = Toggle infinite/finite lines\n");
  printf("? = Print this command list\n\n");
}