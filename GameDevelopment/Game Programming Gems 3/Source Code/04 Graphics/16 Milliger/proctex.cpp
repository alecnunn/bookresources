/* 
 *  3D Procedural Texturing in OpenGL
 *
 *  This demo synthesizes Perlin-style fractal noise and turbulence using
 *  pre-filtered 3D textures. It uses the texture matrix to control spatial
 *  frequency, and the register combiners to sum together the octaves and
 *  convert the intensity value to a color.
 *
 *  This version uses 4 simultaneous lookups into a 64x64x64 3D luminance texture
 *  (only 256K memory)
 *
 *  v2.0 - uses dependent texture pass for color-table lookup
 * 
 *  Simon Green 3/2002 (sgreen@nvidia.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <GL/glut.h>
#include "GL/glext.h"

#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )

#define GLH_EXT_SINGLE_FILE
#include <glh_extensions.h>
#include <glh_linear.h>
#include <nvparse.h>

#include "noise.h"
#include "proctex.h"
#include "paramgl.h"

int winw = 512, winh = 512;

int ox, oy;
int buttonState = 0;

float tx = 0, ty = 0, tz = -100;
float rx = 0, ry = 0;
vec3f objPos, objRot;

int ntexunits = 0;

FilteredNoise *noise;
FilteredNoise::NoiseMode noiseMode = FilteredNoise::ABSNOISE;
GLuint noisetex, noisetex2D, lut_tex;

float time = 0.0;

bool eyeTexGen = false;

// noise parameters
int octaves = 4;
float ampi = 0.6;	// inital amplitude
float ampm = 0.5;	// amplitude multiplier
float freqm = 2.1;	// frequency multiplier
float texscale = 0.01;
float texscale_fine = 1.0;
float alpha = 0.35; // alpha reference value
float alpha_scale = 1.0f;
int layers = 50.0;
float depth = 20.0;

vec4f color1(0.6, 0.0, 0.0, 0.0);
vec4f color2(1.0, 1.0, 0.0, 0.0);

ParamListGL *parmlist;  // parameter list

bool toggle[256];

matrix4f viewMatrix, viewMatrixInv;

const float noiseScale = 4.0;

void (*drawFunc)() = drawTeapot;
void (*drawFunc2)() = drawSphere;
int csgMode = 0;

void (*texMatFunc)(int) = texMatDriftUp;

void (*drawFuncTab[])() = {
	drawSquare,
	drawSphere,
	drawCube,
	drawTeapot,
	drawParticles,
	drawCSG,
	drawSphereVol,
};

void (*texMatFuncTab[])(int i) = {
	texMatNoop,
	texMatRandomRot,
	texMatDrift,
	texMatCircle,
	texMatSpin,
	texMatDriftUp,
	texMatZanimate,
	texMatScale,
};

// material and light parameters
GLfloat matAmb[] =    {0.2, 0.2, 0.2, 1.0};
GLfloat matDiff[] =   {0.9, 0.9, 0.9, 1.0};
GLfloat matSpec[] =   {1.0, 1.0, 1.0, 1.0};
GLfloat black[] =     {0.0, 0.0, 0.0, 1.0};
GLfloat matShine =    60.0;

GLfloat lightPos[] =  {1.0, 1.0, 1.0, 0.0};
GLfloat lightAmb[] =  {1.0, 1.0, 1.0, 1.0};
GLfloat lightDiff[] = {1.0, 1.0, 1.0, 1.0};
GLfloat lightSpec[] = {1.0, 1.0, 1.0, 1.0};


// Rendering functions

// simple particle system for explosion effect
typedef struct {
    float size, dsize;
    vec3f pos, v;
} Particle;

#define NPARTICLES  10
Particle part[NPARTICLES];
GLuint spheredl;

void createSphere()
{
  spheredl = glGenLists(1);
  glNewList(spheredl, GL_COMPILE);
  glutSolidSphere(1.0, 40, 20);
  glEndList();
}

float frand()
{
  return (2.0 * rand() / (float) RAND_MAX) - 1.0;
}

void initParticles()
{
  int i;
  for(i=0; i<NPARTICLES; i++) {
    Particle *p = &part[i];
    p->size = 1.0 + (frand() * 0.1);
    p->pos[0] = frand() * 4.0;
    p->pos[1] = frand() * 4.0;
    p->pos[2] = frand() * 4.0;
    p->v[0] = frand() * 0.1;
    p->v[1] = frand() * 0.1;
    p->v[2] = frand() * 0.1;
    p->dsize = 0.1 + frand() * 0.05;
  }
}

void moveParticles(void)
{
  int i;

  for(i=0; i<NPARTICLES; i++) {
    Particle *p = &part[i];
    p->pos += p->v;
    p->size += p->dsize;
    if (p->size > 50.0) {
      initParticles();
    }
  }
}

void drawParticles()
{
  int i;

  moveParticles();

//  glDisable(GL_CULL_FACE);

  for(i=0; i<NPARTICLES; i++) {
    Particle *p = &part[i];
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(p->pos[0], p->pos[1], p->pos[2]);
    glScalef(p->size, p->size, p->size);
//    glutSolidSphere(1.0, 20, 10);
    glCallList(spheredl);
    glPopMatrix();
  }
}

void drawSquare()
{
  const float s = 40.0;
  
  glBegin(GL_QUADS);
  glVertex2f(-s, -s);
  glVertex2f(s, -s);
  glVertex2f(s, s);
  glVertex2f(-s, s);
  glEnd();
}

void drawSphere()
{
  glutSolidSphere(40.0, 60, 30);
}

void drawSphere2()
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(objPos[0], objPos[1], objPos[2]);
  glRotatef(objRot[0], 1.0, 0.0, 0.0);
  glRotatef(objRot[1], 0.0, 1.0, 0.0);
  glutSolidSphere(40.0, 60, 30);
  glPopMatrix();
}


void drawCube()
{
  glutSolidCube(60.0);
}

void drawTeapot()
{
  glDisable(GL_CULL_FACE);
  glMatrixMode(GL_MODELVIEW);
  glutSolidTeapot(30.0);
}

void drawSphereVolume(int layers, float r, float depth)
{
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  for(int i=0; i<layers; i++) {
    float s = r + (i/(float) (layers-1))*depth;
    float c = i/(float)(layers-1);
    glColor3f(c, c, c);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glScalef(s, s, s);
    glCallList(spheredl);
    glPopMatrix();
  }

  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
}

void drawSphereVol()
{
  glCullFace(GL_BACK);
  glDisable(GL_CULL_FACE);
  drawSphereVolume(layers, 0.0, depth);
}


/* CSG code lifted from Advanced OpenGL Rendering Course, thanks to Tom McReynolds */

/* Set stencil buffer to show the part of a (front or back face) that's
   inside b's volume. Requirements: GL_CULL_FACE enabled, depth func GL_LESS
   Side effects: depth test, stencil func, stencil op */
void
firstInsideSecond(void (*a) (void), void (*b) (void), GLenum face, GLenum test)
{
  glEnable(GL_DEPTH_TEST);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glCullFace(face);     /* controls which face of a to use */
  a();                  /* draw a face of a into depth buffer */

  /* use stencil plane to find parts of a in b */
  glDepthMask(GL_FALSE);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0, 0);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
  glCullFace(GL_BACK);
  b();                  /* increment the stencil where the front face of b is 
                           drawn */
  glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
  glCullFace(GL_FRONT);
  b();                  /* decrement the stencil buffer where the back face
                           of b is drawn */
  glDepthMask(GL_TRUE);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  glStencilFunc(test, 0, 1);
  glDisable(GL_DEPTH_TEST);

  glCullFace(face);
  a();                  /* draw the part of a that's in b */
}

void
fixDepth(void (*a) (void))
{
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_STENCIL_TEST);
  glDepthFunc(GL_ALWAYS);
  a();                  /* draw the front face of a, fixing the depth buffer */
  glDepthFunc(GL_LESS);
}

/* subtract a from b */
void
sub(void (*a)(), void (*b)())
{
  firstInsideSecond(a, b, GL_FRONT, GL_NOTEQUAL);

  fixDepth(b);

  firstInsideSecond(b, a, GL_BACK, GL_EQUAL);

  glDisable(GL_STENCIL_TEST);  /* reset things */
}

/* "and" two objects together */
void
and(void (*a) (void), void (*b) (void))
{
  firstInsideSecond(a, b, GL_BACK, GL_NOTEQUAL);

  fixDepth(b);

  firstInsideSecond(b, a, GL_BACK, GL_NOTEQUAL);

  glDisable(GL_STENCIL_TEST);  /* reset things */
}

// draw object using constructive solid geometry
void drawCSG()
{
  glClear(GL_STENCIL_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glDepthFunc(GL_LESS);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  switch(csgMode) {
  case 0:
    sub(drawSphere2, drawCube);
    break;
  case 1:
    sub(drawCube, drawSphere2);
    break;
  case 2:
    and(drawSphere2, drawCube);
    break;
  }
  glEnable(GL_DEPTH_TEST);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
}

// Texture matrix animation functions
// i is octave number

void texMatNoop(int i)
{
}

void texMatRandomRot(int i)
{
  glRotatef(noise->noise2D(i, 0) * 180.0, 0.0, 0.0, 1.0);
  glRotatef(noise->noise2D(i, 1) * 180.0, 1.0, 0.0, 0.0);
}

void texMatDrift(int i)
{
  glTranslatef(time*(i+1), time*(i+1), time*(i+1));
}

void texMatCircle(int i)
{
  glRotatef(noise->noise2D(i, 0) * 180.0, 0.0, 0.0, 1.0);
  glTranslatef(cos(time*(i+1)*0.1) * 2.0, sin(time*(i+1)*0.1) * 2.0, sin(time*(i+1.7)*0.1) * 2.0);
}


void texMatSpin(int i)
{
  glTranslatef(-0.5, -0.5, 0.0);
  glRotatef(-time * 360 * ((i+1)/4.0), 0.0, 0.0, 1.0);
  glTranslatef(0.5, 0.5, 0.0);
}

void texMatDriftUp(int i)
{
  glTranslatef(0.0, -time*(i+1), 0.0);
}

void texMatZanimate(int i)
{
  glTranslatef(0.0, 0.0, -time);
}

void texMatScale(int i)
{
  glScalef(1.0-time, 1.0-time, 1.0-time);
}

void initTexGen(GLenum mode)
{
  GLfloat planeS[] = { 1.0, 0.0, 0.0, 0.0 };
  GLfloat planeT[] = { 0.0, 1.0, 0.0, 0.0 };
  GLfloat planeR[] = { 0.0, 0.0, 1.0, 0.0 };

  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, mode);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTexGenfv(GL_S, (mode==GL_OBJECT_LINEAR) ? GL_OBJECT_PLANE : GL_EYE_PLANE, planeS);
  glTexGenfv(GL_T, (mode==GL_OBJECT_LINEAR) ? GL_OBJECT_PLANE : GL_EYE_PLANE, planeT);
  glTexGenfv(GL_R, (mode==GL_OBJECT_LINEAR) ? GL_OBJECT_PLANE : GL_EYE_PLANE, planeR);
  glPopMatrix();
}

void enableTexGen(void)
{
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
}

void disableTexGen(void)
{
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
}

void setTexGen(bool eye)
{
  for(int i=0; i<ntexunits; i++) {
    glActiveTextureARB(GL_TEXTURE0_ARB + i);
    if (eye)
      initTexGen(GL_EYE_LINEAR);
    else
//      initTexGen(GL_OBJECT_LINEAR);
      initTexGen(GL_EYE_LINEAR);
  }
}

void disableTexturing()
{
  for(int i=0; i<ntexunits; i++) {
    glActiveTextureARB(GL_TEXTURE0_ARB + i);
    glDisable(GL_TEXTURE_RECTANGLE_NV);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_3D);
  }
}

GLuint window_tex = 0;

void copyWindowToTexture()
{  
  if (window_tex) {
    // copy to texture
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, window_tex);
	glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, winw, winh);
  } else {
    // create texture
	glGenTextures(1, &window_tex);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, window_tex);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glCopyTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGB, 0, 0, winw, winh, 0);
  }
}

// set modelview and projection matrix to give window coordinates
void winCoordsBegin(void)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT), -1, 1);

  glMatrixMode(GL_MODELVIEW);
}

void winCoordsEnd(void)
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

// display the color look-up table
void drawLUT()
{  
  disableTexturing();
  
  glActiveTextureARB(GL_TEXTURE0_ARB);
  glBindTexture(GL_TEXTURE_2D, lut_tex);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glDisable(GL_REGISTER_COMBINERS_NV);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  winCoordsBegin();

  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex2i(0, 0);
  glTexCoord2f(1.0, 0.0); glVertex2i(winw, 0);
  glTexCoord2f(1.0, 1.0); glVertex2i(winw, winh);
  glTexCoord2f(0.0, 1.0); glVertex2i(0, winh);
  glEnd();

  winCoordsEnd();

  disableTexturing();
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_DEPTH_TEST);
}


void drawTexturedQuad()
{  
  disableTexturing();
  
  glActiveTextureARB(GL_TEXTURE0_ARB);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, window_tex);
  glEnable(GL_TEXTURE_RECTANGLE_NV);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  
  glActiveTextureARB(GL_TEXTURE1_ARB);
  glBindTexture(GL_TEXTURE_2D, lut_tex);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glEnable(GL_TEXTURE_SHADER_NV);
  glDisable(GL_REGISTER_COMBINERS_NV);

  glDisable(GL_CULL_FACE);
//  glDisable(GL_DEPTH_TEST);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_GREATER);
  glDepthMask(GL_FALSE);

  winCoordsBegin();

  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(0, 0, -0.999);
  glTexCoord2f(winw, 0.0); glVertex3f(winw, 0, -0.999);
  glTexCoord2f(winw, winh); glVertex3f(winw, winh, -0.999);
  glTexCoord2f(0.0, winh); glVertex3f(0, winh, -0.999);
  glEnd();

  winCoordsEnd();

  glDisable(GL_TEXTURE_SHADER_NV);
  disableTexturing();
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);
}

// check for OpenGL errors
void checkErrors(char *s)
{
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    fprintf(stderr, "%s: error - %s\n", s, (char *) gluErrorString(error));
  }
}


// render fractal sum of noise
// params: no. of octaves, initial amplitude, amplitude multiplier, initial frequency, frequency multiplier

void renderNoise3D(int octaves, float ampi, float ampm, float freqi, float freqm)
{
  float freq = freqi;
  float amp = ampi;
  int octave = 0;
  int passes = ((octaves-1)/ntexunits) + 1;

  for(int pass=0; pass<passes; pass++) {
    // setup texture units

    int ntex = octaves - octave;
    if (ntex > ntexunits) ntex = ntexunits;

    setCombinerConstants(ntex, amp, ampm);
    glEnable(GL_REGISTER_COMBINERS_NV);

    for(int tex=0; tex<ntex; tex++) {
      glActiveTextureARB(GL_TEXTURE0_ARB+tex);
      glBindTexture(GL_TEXTURE_3D, noisetex);
      glEnable(GL_TEXTURE_3D);

      enableTexGen();

      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glScalef(freq, freq, freq);
      texMatFunc(octave);
      if (!eyeTexGen)
        // get world linear texgen by using eye linear and applying inverse view matrix
        glMultMatrixf(viewMatrixInv.get_value());

      freq *= freqm;
      amp *= ampm;

      octave++;
    }

    // draw
    drawFunc();

    for(tex=0; tex<ntex; tex++) {
	  glActiveTextureARB(GL_TEXTURE0_ARB+tex);
      glDisable(GL_TEXTURE_3D);
      disableTexGen();
	  glMatrixMode(GL_TEXTURE);
	  glPopMatrix();
    }

    if (pass==0) {
	  glDepthMask(GL_FALSE);			// no need to write Z for subsequent passes
	  glDepthFunc(GL_LEQUAL);
	  glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);	    // additive blend
    }

    checkErrors("rendernoise");
  }

  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);
  glDisable(GL_BLEND);
  glDisable(GL_REGISTER_COMBINERS_NV);

  disableTexturing();
}


// GLUT callback functions
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // view transform
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(tx, ty, tz);
  glRotatef(rx, 1.0, 0.0, 0.0);
  glRotatef(ry, 0.0, 1.0, 0.0);

  glGetFloatv(GL_MODELVIEW_MATRIX, (float *) viewMatrix.get_value());
  viewMatrixInv = viewMatrix.inverse();

  if (toggle['a']) {
    if (toggle['A'])
      glAlphaFunc(GL_GREATER, alpha);
    else
      glAlphaFunc(GL_LESS, alpha);
    glEnable(GL_ALPHA_TEST);
  }

  if (toggle['l'])
    glEnable(GL_LIGHTING);

  setCombinerColors();
  glColor3f(1.0, 1.0, 1.0);
  renderNoise3D(octaves, ampi, ampm, texscale * texscale_fine, freqm);

  glDisable(GL_ALPHA_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_REGISTER_COMBINERS_NV);

  if (toggle['d']) {
    // second pass - dependent texture
    copyWindowToTexture();
    drawTexturedQuad();
  }

  if (toggle['L']) {
    // third pass - diffuse lighting
    glDepthFunc(GL_EQUAL);
    glBlendFunc(GL_ZERO, GL_SRC_COLOR);
    glEnable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, black);
    drawFunc();

    // 4th pass - specular lighting
    glBlendFunc(GL_ONE, GL_ONE);
    glLightfv(GL_LIGHT0, GL_AMBIENT, black);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, black);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);
    drawFunc();

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);

    glDepthFunc(GL_LESS);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
  }

  if (toggle['u']) {
    drawLUT();
  }

  if (toggle['h']) {
    parmlist->Render(0, 0);
  }

  time += 0.05;

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glutSwapBuffers();
}

void motion(int x, int y)
{
  float dx, dy;
  dx = x - ox;
  dy = y - oy;

  if ((toggle['h']) && (buttonState & 1)) {
    // call parameter list motion function
    parmlist->Motion(x, y);
    return;
  }

  if (toggle['m']) {
    // move object
    if (buttonState == 3) {
	  // left+middle = zoom
	  objPos[2] += (dy - dx) / 10.0;
    } 
    else if (buttonState & 2) {
	  // middle = translate
	  objPos[0] += dx / 10.0;
	  objPos[1] -= dy / 10.0;
    }
    else if (buttonState & 1) {
	  // left = rotate
	  objRot[0] += dy / 5.0;
	  objRot[1] += dx / 5.0;
    }


  } else {
    if (buttonState == 3) {
	  // left+middle = zoom
	  tz += (dy - dx) / 10.0;
    } 
    else if (buttonState & 2) {
	  // middle = translate
	  tx += dx / 10.0;
	  ty -= dy / 10.0;
    }
    else if (buttonState & 1) {
	  // left = rotate
	  rx += dy / 5.0;
	  ry += dx / 5.0;
    }
  }

  ox = x; oy = y;
  glutPostRedisplay();
}


void mouse(int button, int state, int x, int y)
{
  int mods;

  if (state == GLUT_DOWN)
	buttonState |= 1<<button;
  else if (state == GLUT_UP)
	buttonState = 0;

  mods = glutGetModifiers();
  if (mods & GLUT_ACTIVE_SHIFT)
	buttonState = 2;

  if (mods & GLUT_ACTIVE_CTRL)
	buttonState = 3;

  if ((toggle['h']) && (buttonState & 1)) {
    // call list mouse function
    parmlist->Mouse(x, y);
  }

  ox = x; oy = y;
  glutPostRedisplay();
}

void key(unsigned char key, int x, int y)
{
  toggle[key] ^= 1;

  switch (key) {
  case '\033':
  case 'q':
	exit(0);
	break;
  case '=':
  case '+':
	octaves++;
	break;
  case '-':
	if (octaves > 1) octaves--;
	break;
  case ']':
	ampm += 0.1;
    break;
  case '[':
	ampm -= 0.1;
	break;

  case '\'':
	ampi += 0.01;
	break;
  case ';':
	ampi -= 0.01;
	break;

  case '.':
	freqm += 0.1;
	break;
  case ',':
	freqm -= 0.1;
	break;

  case '*':
	alpha += 0.01;
	break;
  case '/':
	alpha -= 0.01;
	break;

  case 'c':
    drawFunc = drawFuncTab[5];
    csgMode = (csgMode+1) % 3;
    break;

  case 'r':
	tx = 0.0; ty = 0.0; tz = -100.0;
	rx = ry = 0.0;
	ampi = 0.5; ampm = 0.5;
	freqm = 2.0;
    time = 0.0;
	break;

  case 'w':
	glPolygonMode(GL_FRONT_AND_BACK, toggle['w'] ? GL_LINE : GL_FILL);
    break;

  case 'f':
    glBindTexture(GL_TEXTURE_3D, noisetex);
    if (toggle['f']) {
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    }
    break;

  case ' ':
    initParticles();
    break;

  case 'H':
    printf("\
'+'   increment no. of octaves\n\
'-'   decrement no. of octaves\n\
']'   increase amplitude multiplier\n\
'['   decrease amplitude multiplier\n\
'''   increase initial amplitude\n\
';'   decrease initial amplitude\n\
'>'   increase frequency multiplier\n\
'<'   decrease frequency multiplier\n\
'f'   toggle filtering\n\
'w'   toggle wireframe\n\
'l'   toggle lighting\n\
'd'   toggle dependent texture\n\
'r'   reset\n\
    ");
  }
	
  glutPostRedisplay();
}

// "special" key callback for manipulating sliders
void special(int key, int x, int y)
{
  switch(key) {
  case GLUT_KEY_DOWN:
    parmlist->Increment();
    break;
  case GLUT_KEY_UP:
    parmlist->Decrement();
    break;
  case GLUT_KEY_RIGHT:
    parmlist->GetCurrent()->Increment();
    break;
  case GLUT_KEY_LEFT:
    parmlist->GetCurrent()->Decrement();
    break;
  case GLUT_KEY_HOME:
    parmlist->GetCurrent()->Reset();
    break;
  case GLUT_KEY_END:
    parmlist->GetCurrent()->SetPercentage(1.0);
    break;
  }
  glutPostRedisplay();
}

void idle(void)
{
  glutPostRedisplay();
}

void reshape(int w, int h)
{
  winw = w;
  winh = h;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (float) w / (float) h, 1.0, 500.0);

  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, w, h);

  glDeleteTextures(1, &window_tex);
  window_tex = 0;
}

// register combiner setup
void initCombinersSum()
{
  nvparse(   
    "!!RC1.0\n"
    "// combiners to weight and sum textures for procedural noise\n"
    "{\n"
    "   // first octave\n"
    "   const0 = ( 0.5, 0.5, 0.5, 1 );\n"
    "	rgb {\n"
    "		spare0 = tex0 * const0;\n"
    "	}\n"
    "}\n"
    "{\n"
    "   // sum second octave\n"
    "   const0 = ( 0.25, 0.25, 0.25, 1 );\n"
    "	rgb {\n"
    "		discard = tex1 * const0;\n"
    "       discard = spare0;\n"
    "       spare0 = sum();\n"
    "	}\n"
    "}\n"
    "{\n"
    "   // sum third octave\n"
    "   const0 = ( 0.125, 0.125, 0.125, 1 );\n"
    "	rgb {\n"
    "		discard = tex2 * const0;\n"
    "       discard = spare0;\n"
    "       spare0 = sum();\n"
    "	}\n"
    "}\n"
    "{\n"
    "   // sum fourth octave\n"
    "   const0 = ( 0.0625, 0.0625, 0.0625, 1 );\n"
    "	rgb {\n"
    "		discard = tex3 * const0;\n"
    "       discard = spare0;\n"
    "       spare0 = sum();\n"
    "	}\n"
    "}\n"

    "// diffuse lighting\n"
    "out.rgb = spare0 * col0;\n"
    "out.a = spare0.b;\n"
  );

  nvparse_print_errors(stderr);

}

// sum and then mux between two colors
void initCombinersSumMux()
{
  nvparse(   
    "!!RC1.0\n"
    "// combiners to weight and sum textures for procedural noise\n"
    "{\n"
    "   // first octave\n"
    "   const0 = ( 0.5, 0.5, 0.5, 1 );\n"
    "	rgb {\n"
    "		spare0 = tex0 * const0;\n"
    "	}\n"
    "}\n"
    "{\n"
    "   // sum second octave\n"
    "   const0 = ( 0.25, 0.25, 0.25, 1 );\n"
    "	rgb {\n"
    "		discard = tex1 * const0;\n"
    "       discard = spare0;\n"
    "       spare0 = sum();\n"
    "	}\n"
    "}\n"
    "{\n"
    "   // sum third octave\n"
    "   const0 = ( 0.125, 0.125, 0.125, 1 );\n"
    "	rgb {\n"
    "		discard = tex2 * const0;\n"
    "       discard = spare0;\n"
    "       spare0 = sum();\n"
    "	}\n"
    "}\n"
    "{\n"
    "   // sum fourth octave\n"
    "   const0 = ( 0.0625, 0.0625, 0.0625, 1 );\n"
    "	rgb {\n"
    "		discard = tex3 * const0;\n"
    "       discard = spare0;\n"
    "       spare0 = sum();\n"
    "	}\n"
    "}\n"

    "{\n"
    "   // move luminance to alpha for later mux\n"
    "	alpha {\n"
    "       spare0 = spare0.b;\n"
    "	}\n"
    "}\n"

    "// choose between two colors\n"
    "{\n"
    "	rgb {\n"
    "		tex0 = spare0 * const0;\n"
    "       scale_by_two();\n"
    "	}\n"
    "}\n"

    "{\n"
    "	rgb {\n"
    "		tex1 = expand(spare0) * const1;\n"
    "	}\n"
    "}\n"

    "{\n"
    "	rgb {\n"
    "		discard = tex0;\n"
    "       discard = tex1;\n"
    "       spare0 = mux();\n"
    "	}\n"
    "}\n"
    "// diffuse lighting\n"
    "out.rgb = spare0 * col0;\n"
    "out.a = spare0;\n"

  );

  nvparse_print_errors(stderr);

}


// sum and then interpolate between two colors
void initCombinersSumColor()
{
  nvparse(   
    "!!RC1.0\n"
    "// combiners to weight and sum textures for procedural noise\n"
    "{\n"
    "   // first octave\n"
    "   const0 = ( 0.5, 0.5, 0.5, 1 );\n"
    "	rgb {\n"
    "		spare0 = tex0 * const0;\n"
    "	}\n"
    "}\n"
    "{\n"
    "   // sum second octave\n"
    "   const0 = ( 0.25, 0.25, 0.25, 1 );\n"
    "	rgb {\n"
    "		discard = tex1 * const0;\n"
    "       discard = spare0;\n"
    "       spare0 = sum();\n"
    "	}\n"
    "}\n"
    "{\n"
    "   // sum third octave\n"
    "   const0 = ( 0.125, 0.125, 0.125, 1 );\n"
    "	rgb {\n"
    "		discard = tex2 * const0;\n"
    "       discard = spare0;\n"
    "       spare0 = sum();\n"
    "	}\n"
    "}\n"
    "{\n"
    "   // sum fourth octave\n"
    "   const0 = ( 0.0625, 0.0625, 0.0625, 1 );\n"
    "	rgb {\n"
    "		discard = tex3 * const0;\n"
    "       discard = spare0;\n"
    "       spare0 = sum();\n"
    "	}\n"
    "}\n"

    "{\n"
    "   // move luminance to alpha for later mux\n"
    "	alpha {\n"
    "       spare0 = spare0.b;\n"
    "	}\n"
    "}\n"

    "// interpolate between three colors (first is black)\n"
    "// if (sum < 0.5)\n"
    "//   out = sum*col1*2\n"
    "// else\n"
    "//   out = col1 + (sum-0.5)*2*col2\n"
    "{\n"
    "   const0 = ( 1, 0, 0, 0 ); // col1\n"
    "	rgb {\n"
    "		tex0 = spare0 * const0;\n"
    "       scale_by_two();\n"
    "	}\n"
    "}\n"

    "{\n"
    "   const0 = ( 1, 0, 0, 1 ); // col1\n"
    "   const1 = ( 1, 1, 0, 1 ); // col2\n"
    "	rgb {\n"
    "       discard = const0;\n"
    "       discard = expand(spare0) * const1; // (2*spare0)-1\n"
    "       tex1 = sum();\n"
    "	}\n"
    "}\n"

    "{\n"
    "	rgb {\n"
    "		discard = tex0;\n"
    "       discard = tex1;\n"
    "       spare0 = mux();\n"
    "	}\n"
    "   alpha {\n"
    "       spare0 = spare0 * const0;\n"
    "   }\n"
    "}\n"

    "final_product = spare0 * col0;\n"
    "out.rgb = col1 + final_product;\n"
    "out.a = spare0;\n"
  );

  nvparse_print_errors(stderr);

}

// calculate minimum of 4 values for Voronoi patterns
void initCombinersMin()
{
  nvparse(   
    "!!RC1.0\n"
    "// combiners to calculate minimum value of 4 textures\n"
    "// for Worley cellular noise (Voronoi) \n"
    "{\n"
    "   // minimum(tex0, tex1)\n"
    "	alpha {\n"
    "		discard = tex0.b;\n"
    "       discard = unsigned_invert(tex1.b);\n"
    "       spare0 = sum();\n"
    "       bias_by_negative_one_half();\n"
    "	}\n"
    "}\n"
    "{\n"
    "   // tex0+(1-tex1)-0.5 < 0.5 ? tex0 : tex1\n"
    "	rgb {\n"
    "		discard = tex0;\n"
    "       discard = tex1;\n"
    "       spare0 = mux();\n"
    "	}\n"
    "}\n"

    "{\n"
    "   // minimum(spare0, tex2)\n"
    "	alpha {\n"
    "		discard = spare0.b;\n"
    "       discard = unsigned_invert(tex2.b);\n"
    "       spare0 = sum();\n"
    "       bias_by_negative_one_half();\n"
    "	}\n"
    "}\n"
    "{\n"
    "	rgb {\n"
    "		discard = spare0;\n"
    "       discard = tex2;\n"
    "       spare0 = mux();\n"
    "	}\n"
    "}\n"

    "{\n"
    "   // minimum(spare0, tex3)\n"
    "	alpha {\n"
    "		discard = spare0.b;\n"
    "       discard = unsigned_invert(tex3.b);\n"
    "       spare0 = sum();\n"
    "       bias_by_negative_one_half();\n"
    "	}\n"
    "}\n"
    "{\n"
    "   rgb {\n"
    "		discard = spare0;\n"
    "       discard = tex3;\n"
    "       spare0 = mux();\n"
    "       scale_by_two();\n"
    "	}\n"
    "}\n"

    "out.rgb = spare0 * col0;\n"
    "out.a = spare0;\n"
    );

  nvparse_print_errors(stderr);
}

void initCombinersTex0()
{
  nvparse(   
    "!!RC1.0\n"
    "out.rgb = tex0;\n"
    "out.a = spare0;\n"
  );

  nvparse_print_errors(stderr);
}

void initCombinersTex1()
{
  nvparse(   
    "!!RC1.0\n"
    "out.rgb = tex1;\n"
    "out.a = spare0;\n"
  );

  nvparse_print_errors(stderr);
}

void initCombinersTex2()
{
  nvparse(   
    "!!RC1.0\n"
    "out.rgb = tex2;\n"
    "out.a = spare0;\n"
  );

  nvparse_print_errors(stderr);
}

void initCombinersTex3()
{
  nvparse(   
    "!!RC1.0\n"
    "out.rgb = tex3;\n"
    "out.a = spare0;\n"
  );

  nvparse_print_errors(stderr);
}


void setCombinerConstants(int ntex, float ampi, float ampm)
{
  GLfloat v[4];

  int i;
  float amp = ampi;
  for(i=0; i<ntexunits; i++) {
    if (i>=ntex)
      amp = 0;
    v[0] = v[1] = v[2] = amp; v[3] = 1.0f;
    glCombinerStageParameterfvNV(GL_COMBINER0_NV + i, GL_CONSTANT_COLOR0_NV, v);
    amp *= ampm;
  }
}

void setCombinerColors()
{
  glCombinerParameterfvNV(GL_CONSTANT_COLOR0_NV, (GLfloat *) &color1);
  glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, (GLfloat *) &color2);
  
  glCombinerStageParameterfvNV(GL_COMBINER5_NV, GL_CONSTANT_COLOR0_NV, (GLfloat *) &color1);
  glCombinerStageParameterfvNV(GL_COMBINER6_NV, GL_CONSTANT_COLOR0_NV, (GLfloat *) &color1);
  glCombinerStageParameterfvNV(GL_COMBINER6_NV, GL_CONSTANT_COLOR1_NV, (GLfloat *) &color2);

  GLfloat col[4];
  col[3] = alpha_scale;
  glCombinerStageParameterfvNV(GL_COMBINER7_NV, GL_CONSTANT_COLOR0_NV, (GLfloat *) &col);
}


void initTextureShaders()
{
  nvparse(
	  "!!TS1.0              \n"
	  "texture_rectangle(); \n"
      "dependent_gb(tex0);  \n"
	  );
  nvparse_print_errors(stderr);
}

void checkShaderConsistency()
{
  for(int i=0; i<4; i++) {
    glActiveTextureARB(GL_TEXTURE0_ARB + i);
    int consistent;
    glGetTexEnviv(GL_TEXTURE_SHADER_NV, GL_SHADER_CONSISTENT_NV, &consistent);
    if(consistent == GL_FALSE)
	    fprintf(stderr, "Texture shader stage %d is inconsistent!\n", i);
  }
}


float smoothstep(float a, float b, float x)
{
  if (x < a)
    return 0;
  if (x >= b)
    return 1;
  x = (x - a) / (b - a);    /* normalize to [0:1] */
  return (x*x * (3 - 2*x));	// 3x^2 - 2x^3
}

/* Coefficients of basis matrix. */
#define CR00     -0.5
#define CR01      1.5
#define CR02     -1.5
#define CR03      0.5
#define CR10      1.0
#define CR11     -2.5
#define CR12      2.0
#define CR13     -0.5
#define CR20     -0.5
#define CR21      0.0
#define CR22      0.5
#define CR23      0.0
#define CR30      0.0
#define CR31      1.0
#define CR32      0.0
#define CR33      0.0

// Catmull-Rom Bicubic spline interpolation code
// from "Texturing & Modelling, A Procedural Approach"
// Interpolates middle control points, first and last determine tangents
// (only used for color splining here)
vec3f spline(float x, int nknots, const vec3f *knot)
{
    int span;
    int nspans = nknots - 3;
    vec3f c0, c1, c2, c3;	/* coefficients of the cubic.*/

    if (nspans < 1) {  /* illegal */
        fprintf(stderr, "Spline has too few knots.\n");
        return vec3f();
    }

    /* Find the appropriate 4-point span of the spline. */
    x = clamp(x, 0, 1) * nspans;
    span = (int) x;
    if (span >= nknots - 3)
        span = nknots - 3;
    x -= span;
    knot += span;

    /* Evaluate the span cubic at x using Horner's rule. */
    c3 = knot[0]*CR00 + knot[1]*CR01 + knot[2]*CR02 + knot[3]*CR03;
    c2 = knot[0]*CR10 + knot[1]*CR11 + knot[2]*CR12 + knot[3]*CR13;
    c1 = knot[0]*CR20 + knot[1]*CR21 + knot[2]*CR22 + knot[3]*CR23;
    c0 = knot[0]*CR30 + knot[1]*CR31 + knot[2]*CR32 + knot[3]*CR33;

    return ((c3*x + c2)*x + c1)*x + c0;
}

// Darwyn Peachy blue marble
vec3f spline_color(float x)
{
  const vec3f medium_blue(0.10, 0.10, 0.45);
  const vec3f dark_blue  (0.05, 0.05, 0.30);
  const vec3f darker_blue(0.03, 0.03, 0.20);
  const vec3f white      (1.0, 1.0, 1.0);

  const vec3f marble_colors[] = {
    white,
    white,
    medium_blue,
    medium_blue,
    medium_blue,
    white,
    white,
    dark_blue,
    dark_blue,
    darker_blue,
    darker_blue,
    white,
    darker_blue
  };

  const vec3f black     (0.0, 0.0, 0.0);
  const vec3f orange    (1.0, 0.5, 0.0);
  const vec3f red       (1.0, 0.0, 0.0);
  const vec3f yellow    (1.0, 1.0, 0.0);
  const vec3f blue      (0.0, 0.2, 0.5);
  const vec3f grey      (0.5, 0.5, 0.5);

  const vec3f fire_colors[] = {
    black,
    black,
    red,
    orange,
    yellow,
    white,
    white,
  };


  x = (x*2.0) - 1.0;
  vec3f c = spline(clamp(2*x + .75, 0, 1), sizeof(marble_colors) / sizeof(vec3f), &marble_colors[0]);
//  vec3f c = spline(x, sizeof(fire_colors) / sizeof(vec3f), &fire_colors[0]);
  c[0] = clamp(c[0], 0.0, 1.0);
  c[1] = clamp(c[1], 0.0, 1.0);
  c[2] = clamp(c[2], 0.0, 1.0);
  return c;
}

GLuint createLookupTexture(int w, int h)
{
  GLuint texid;
  glGenTextures(1, &texid);
  glBindTexture(GL_TEXTURE_2D, texid);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  GLubyte *img = new GLubyte[w * h * 3];

  GLubyte *ptr = img;
  for(int y=0; y<h; y++) {
    for(int x=0; x<w; x++) {
      float s = x / (float) (w-1);
      float t = y / (float) (h-1);

//      float i = smoothstep(0.5, 0.6, s);
      vec3f c = spline_color(s);
//      vec3f c = vec3f(0.7, 0.4, 0.1) * (s*5 - floor(s*5));
      *ptr++ = (GLubyte) (c[0] * 0xff);
      *ptr++ = (GLubyte) (c[1] * 0xff);
      *ptr++ = (GLubyte) (c[2] * 0xff);
    }
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		       w, h, 0,
		       GL_RGB, GL_UNSIGNED_BYTE,
		       img);

  delete [] img;

  return texid;
}


GLuint loadProgram(GLenum target, const GLubyte *code)
{
    GLuint prog_id;
    glGenProgramsNV(1, &prog_id);
    glBindProgramNV(target, prog_id);
    int size = strlen((const char *) code);
	glLoadProgramNV(target, prog_id, size, code);
	GLint errpos;
	glGetIntegerv(GL_PROGRAM_ERROR_POSITION_NV, &errpos);
	if(errpos != -1) {
		fprintf(stderr, "program error:\n");
		int bgn = errpos - 10;
		bgn = bgn < 0 ? 0 : bgn;
		const char * c = (const char *)(code + bgn);
		for(int i = 0; i < 30; i++)
		{
			if(bgn+i >= int(size-1))
				break;
			fprintf(stderr, "%c", *c++);
		}
		fprintf(stderr, "\n");
	}
    return prog_id;
}



// slider functions
void addColorParam(char *name, ParamList *list, float *c)
{
  char pname[100];
  sprintf(pname, "%s_r", name);
  list->AddParam(new Param<float>(pname, c[0], 0.0, 1.0, 1.0 / 256, &c[0]));
  sprintf(pname, "%s_g", name);
  list->AddParam(new Param<float>(pname, c[1], 0.0, 1.0, 1.0 / 256, &c[1]));
  sprintf(pname, "%s_b", name);
  list->AddParam(new Param<float>(pname, c[2], 0.0, 1.0, 1.0 / 256, &c[2]));
}

void initParameters()
{
  // create a new parameter list
  parmlist = new ParamListGL("misc");

  // add some parameters to the list
  // function parameters are: name, default value, minimum, maximum, step, pointer to variable (optional)
  parmlist->AddParam(new Param<int>("octaves", octaves, 0, 4, 1, &octaves));
  parmlist->AddParam(new Param<float>("initial amplitude", ampi, 0.0, 1.0, 0.1, &ampi));
  parmlist->AddParam(new Param<float>("amplitude multiplier", ampm, 0.0, 1.0, 0.1, &ampm));
  parmlist->AddParam(new Param<float>("texture scale", texscale_fine, 0.0, 2.0, 0.01, &texscale_fine));
  parmlist->AddParam(new Param<float>("frequency multiplier", freqm, 0.0, 3.0, 1.0, &freqm));
  addColorParam("color1", parmlist, (float *) &color1);
  addColorParam("color2", parmlist, (float *) &color2);
  parmlist->AddParam(new Param<float>("alpha ref", alpha, 0.0, 1.0, 0.1, &alpha));
  parmlist->AddParam(new Param<float>("alpha scale", alpha_scale, 0.0, 1.0, 0.1, &alpha_scale));
  parmlist->AddParam(new Param<int>("layers", layers, 0, 100, 1, &layers));
  parmlist->AddParam(new Param<float>("depth", depth, 0.0, 50.0, 0.1, &depth));

}


// menu functions
void objMenu(int i)
{
  drawFunc = drawFuncTab[i];
  initParticles();
}

void obj2Menu(int i)
{
  drawFunc2 = drawFuncTab[i];
}


void menu(int i)
{
  key((unsigned char) i, 0, 0);
}

void noiseMenu(int i)
{
  noiseMode = (FilteredNoise::NoiseMode) i;

  glDeleteTextures(1, &noisetex);

  switch(noiseMode) {
  case FilteredNoise::CHECKER:
  case FilteredNoise::RAMP:
    octaves = 1;
    noisetex = noise->createNoiseTexture3D(noiseScale, 1.0, noiseMode, true);
    break;
  
  case FilteredNoise::VEINS:
    color1 = vec4f(0.0, 0.0, 1.0, 1.0);
    color2 = vec4f(1.0, 1.0, 1.0, 1.0);
    noisetex = noise->createNoiseTexture3D(noiseScale, 1.0, noiseMode, true);
    break;
  case FilteredNoise::DISTANCE:
    noisetex = noise->createNoiseTexture3D(noiseScale, 1.0, noiseMode, true);
    initCombinersMin();
    freqm = 1.0;
    texscale = 1.0 / 16.0;
	texMatFunc = texMatCircle;
    break;
  case FilteredNoise::RAW:
    noisetex = noise->createNoiseTexture3D(noiseScale, 1.0, noiseMode, true);
    break;
  default:
    noisetex = noise->createNoiseTexture3D(noiseScale, 1.0, noiseMode, true);
    break;
  }
}

void texMatMenu(int i)
{
  texMatFunc = texMatFuncTab[i];
}

void octavesMenu(int i)
{
  octaves = i;
}

void scaleMenu(int i)
{
  texscale = (float) i / 10000.0;
}

void texGenMenu(int i)
{
  if (i)
    eyeTexGen = true;
  else
    eyeTexGen = false;
  setTexGen(eyeTexGen);
}

void combinerMenu(int i)
{
  switch(i) {
  case 0:
    initCombinersSum();
    break;
  case 1:
    initCombinersSumColor();
    break;
  case 2:
    initCombinersSumMux();
    break;
  case 3:
    initCombinersMin();
    break;
  case 4:
    initCombinersTex0();
    break;
  case 5:
    initCombinersTex1();
    break;
  case 6:
    initCombinersTex2();
    break;
  case 7:
    initCombinersTex3();
    break;  
  }

}


void initMenus(void)
{
  int objsub = glutCreateMenu(objMenu);
  glutAddMenuEntry("Square", 0);
  glutAddMenuEntry("Sphere", 1);
  glutAddMenuEntry("Cube", 2);
  glutAddMenuEntry("Teapot", 3);
  glutAddMenuEntry("Particles", 4);
  glutAddMenuEntry("CSG", 5);
  glutAddMenuEntry("Sphere volume", 6);

  int noisesub = glutCreateMenu(noiseMenu);
  glutAddMenuEntry("Raw noise", FilteredNoise::RAW);
  glutAddMenuEntry("Bicubic filtered noise", FilteredNoise::NOISE);
  glutAddMenuEntry("Abs filtered noise (turbulence)", FilteredNoise::ABSNOISE);
  glutAddMenuEntry("Veins (scaled noise)", FilteredNoise::VEINS);
  glutAddMenuEntry("Distance (Voronoi)", FilteredNoise::DISTANCE);
  glutAddMenuEntry("Checkerboard", FilteredNoise::CHECKER);
  glutAddMenuEntry("Ramp", FilteredNoise::RAMP);

  int combsub = glutCreateMenu(combinerMenu);
  glutAddMenuEntry("Fractal sum", 0);
  glutAddMenuEntry("Sum and color", 1);
  glutAddMenuEntry("Sum and mux", 2);
  glutAddMenuEntry("Minimum (Voronoi)", 3);
  glutAddMenuEntry("Texture 0", 4);
  glutAddMenuEntry("Texture 1", 5);
  glutAddMenuEntry("Texture 2", 6);
  glutAddMenuEntry("Texture 3", 7);

  int texgensub = glutCreateMenu(texGenMenu);
  glutAddMenuEntry("Object linear", 0);
  glutAddMenuEntry("World linear", 1);

  int matsub = glutCreateMenu(texMatMenu);
  glutAddMenuEntry("Static", 0);
  glutAddMenuEntry("Random rotation", 1);
  glutAddMenuEntry("Drift", 2);
  glutAddMenuEntry("Drift up", 5);
  glutAddMenuEntry("Circular translate", 3);
  glutAddMenuEntry("Rotate", 4);
  glutAddMenuEntry("Z translate", 6);
  glutAddMenuEntry("Scale", 7);

  int octsub = glutCreateMenu(octavesMenu);
  glutAddMenuEntry("1", 1);
  glutAddMenuEntry("2", 2);
  glutAddMenuEntry("3", 3);
  glutAddMenuEntry("4", 4);

  int scalesub = glutCreateMenu(scaleMenu);
  glutAddMenuEntry("0.1", 10);
  glutAddMenuEntry("0.25", 25);
  glutAddMenuEntry("0.5", 50);
  glutAddMenuEntry("1.0", 100);
  glutAddMenuEntry("2.0", 200);
  glutAddMenuEntry("4.0", 400);
  glutAddMenuEntry("8.0", 800);
  glutAddMenuEntry("16.0", 1600);
  glutAddMenuEntry("32.0", 3200);
  glutAddMenuEntry("64.0", 6400);

  glutCreateMenu(menu);
  glutAddSubMenu  ("Object", objsub);
  glutAddSubMenu  ("Texture", noisesub);
  glutAddSubMenu  ("Combiners", combsub);
  glutAddSubMenu  ("Octaves", octsub);
  glutAddSubMenu  ("Tex gen mode", texgensub);
  glutAddSubMenu  ("Texture matrix", matsub);
  glutAddSubMenu  ("Texture scale", scalesub);
  glutAddMenuEntry("Toggle sliders [h]", 'h');
  glutAddMenuEntry("Toggle lighting [l]", 'l');
  glutAddMenuEntry("Toggle alpha test [a]", 'a');
  glutAddMenuEntry("Toggle alpha test sense [A]", 'A');
  glutAddMenuEntry("Toggle filtering [f]", 'f');
  glutAddMenuEntry("Toggle wireframe [w]", 'w');
  glutAddMenuEntry("Toggle CSG mode [c]", 'c');
  glutAddMenuEntry("Toggle move mode [m]", 'm');
  glutAddMenuEntry("Toggle dependent texture pass [d]", 'd');
  glutAddMenuEntry("Toggle lighting pass [L]", 'L');
  glutAddMenuEntry("Toggle display LUT [u]", 'u');
  glutAddMenuEntry("Quit (esc)", '\033');
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}


// get extension pointers
void getExts(void)
{
  if(! glh_init_extensions(
						   "GL_ARB_multitexture "
						   "GL_NV_register_combiners "
                           "GL_VERSION_1_2 "  // for 3D textures
						   "GL_NV_vertex_program "
						   ))	{
    fprintf(stderr, "Error - required extensions were not supported: %s", glh_get_unsupported_extensions());
  }

  glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &ntexunits);
  printf("Texture units = %d\n", ntexunits);
}

void initGL()
{
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShine);

  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);

  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glDisable(GL_DITHER);
  glEnable(GL_CULL_FACE);

  glClearColor(0.5, 0.5, 0.5, 1.0);

  setTexGen(true);
  initCombinersSum();
  initTextureShaders();

  lut_tex = createLookupTexture(256, 1);

  createSphere();
  initParticles();

  toggle['d'] = true; // dependent texture
  toggle['L'] = true; // lighting
}

void dumpDriverInfo(void)
{
  int i;
  printf("Vendor:   %s\n", glGetString(GL_VENDOR));
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Version:  %s\n", glGetString(GL_VERSION));
  printf("Extensions: %s\n", glGetString(GL_EXTENSIONS));

  glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &i);
  printf("Max 3D texture size = %d\n", i);

  glGetIntegerv(GL_STENCIL_BITS, &i);
  printf("stencil bits = %d\n", i);
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);

  glutInitDisplayString("rgb double stencil=8 depth>=16");
  glutInitWindowSize(winw, winh);
  (void) glutCreateWindow("3D procedural texturing");

  glutDisplayFunc(display);
  glutKeyboardFunc(key);
  glutSpecialFunc(special);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutIdleFunc(idle);
  initMenus();
  initParameters();

  getExts();
  dumpDriverInfo();
  initGL();

  srand(250673);
  printf("Building 3D texture, please wait...");
  noise = new FilteredNoise(16, 16, 16);
  noisetex = noise->createNoiseTexture3D(noiseScale, 1.0, FilteredNoise::NOISE, true);

  printf("Done.\n");

  glutMainLoop();
  return 0;
}
