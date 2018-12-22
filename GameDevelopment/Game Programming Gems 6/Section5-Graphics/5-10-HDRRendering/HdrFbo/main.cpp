/*
   Allen Sherrod
   HDR Rendering with Frame Buffer Objects
   September 1, 2005
*/


#include<windows.h>
#include<gl/gl.h>
#include<gl/glut.h>
#include<gl/glext.h>
#include<gl/wglext.h>
#include"ext.h"
#include"rgbeLoader.h"

void Resize(int width, int height);
void KeyDown(unsigned char key, int x, int y);
void SpecialKeys(int key, int x, int y);
bool Initialize();
void RenderScene();
void Shutdown();

// Used to rotate the object.
float g_angle = 0.0f;

// Shader data.
GLhandleARB toneShader;
unsigned int glslToneOffset;
unsigned int glslToneFullTexture;
unsigned int glslToneExposure;
unsigned int glslToneRes;

// FBO data.
unsigned int frameBufferIndex = 0;
unsigned int depthBufferIndex = 0;
unsigned int offScreenID = 0;

// Window width and height.
#define WINDOW_WIDTH    640
#define WINDOW_HEIGHT   480
float fullOffset[] = { 1 / WINDOW_WIDTH, 1 / WINDOW_HEIGHT };

// HDR Texture image.
unsigned int hdrID = 0;
int imageWidth = 0, imageHeight = 0;
float exposure = 2.0f;


int main(int arg, char **argc)
{
   glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
   glutInitWindowPosition(100, 100);

   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutInit(&arg, argc);

   glutCreateWindow("HDR using Frame Buffer Objects");

   glutDisplayFunc(RenderScene);
   glutReshapeFunc(Resize);
   glutKeyboardFunc(KeyDown);
   glutSpecialFunc(SpecialKeys);

   if(Initialize()) glutMainLoop();
   else printf("Error in the initialization!\n\n");

   Shutdown();
   return 1;
}


void Resize(int width, int height)
{
   glViewport(0, 0, width, height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 1000.0f);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


void KeyDown(unsigned char key, int x, int y)
{
   switch(key)
      {
         case 27:
            Shutdown();
            exit(0);
            break;
      }
}


void SpecialKeys(int key, int x, int y)
{
   switch(key)
      {
         case GLUT_KEY_UP: exposure += 0.1f; break;
         case GLUT_KEY_DOWN: exposure -= 0.1f; break;
      }

   if(exposure < 0.0f) exposure = 0.01f;
}


bool Initialize()
{
   // Setup OpenGL properties.
   glShadeModel(GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
   glEnable(GL_TEXTURE_RECTANGLE_NV);

   // Initialize extensions and load resources.
   if(!SetupGLSL()) return false;
   if(!SetupFBO()) return false;


   // Load shader files.
   if(!CreateShader("media/generalVS.glsl", "media/tonePS.glsl", &toneShader))
      return false;

   // Bind shader variables.
   glslToneOffset = glGetUniformLocationARB(toneShader, "offset");
   glslToneFullTexture = glGetUniformLocationARB(toneShader, "fullTexture");
   glslToneExposure = glGetUniformLocationARB(toneShader, "exposure");
   glslToneRes = glGetUniformLocationARB(toneShader, "res");


   // Load the HDR image file.
   float *hdrImage = ReadRGBEImage("media/ugp.hdr", &imageWidth, &imageHeight);
   if(!hdrImage) return false;

   // Generate and create floating point HDR texture.
   glGenTextures(1, &hdrID);
   glBindTexture(GL_TEXTURE_RECTANGLE_NV, hdrID);
   glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGB, imageWidth,
                imageHeight, 0, GL_RGB, GL_FLOAT, hdrImage);

   delete[] hdrImage;


   // Generate frame buffer object then bind it.
   glGenFramebuffersEXT(1, &frameBufferIndex);
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferIndex);
   glGenRenderbuffersEXT(1, &depthBufferIndex);

   // Create the texture we will be using to render to.
   glGenTextures(1, &offScreenID);
   glBindTexture(GL_TEXTURE_RECTANGLE_NV, offScreenID);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                GL_RGBA, GL_FLOAT, 0);

   // Bind the texture to the frame buffer.
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                             GL_TEXTURE_RECTANGLE_NV, offScreenID, 0);

   // Initialize the render buffer.
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBufferIndex);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24,
                            WINDOW_WIDTH, WINDOW_HEIGHT);
   glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                GL_RENDERBUFFER_EXT, depthBufferIndex);

   // Make sure we have not errors.
   if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
      return false;

   // Return out of the frame buffer.
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

   return true;
}


void FullScreenPass()
{
   // Bind the frame buffer so we can render to it.
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBufferIndex);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

   glTranslatef(0, 0, -12);
   glRotatef(g_angle, 1.0f, 0.0f, 0.0f);
	glRotatef(g_angle, 0.0f, 1.0f, 0.0f);
   g_angle >= 360 ? g_angle = 0 : g_angle += 0.5f;

   glUseProgramObjectARB(0);

   // Render cube.
   glBindTexture(GL_TEXTURE_RECTANGLE_NV, hdrID);

   glBegin(GL_QUADS);

      glTexCoord2f(imageWidth, imageHeight); glVertex3f(-2.0f, 2.0f, 2.0f);
      glTexCoord2f(0.0f, imageHeight); glVertex3f(2.0f, 2.0f, 2.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(2.0f, -2.0f, 2.0f);
      glTexCoord2f(imageWidth, 0.0f); glVertex3f(-2.0f, -2.0f, 2.0f);

      glTexCoord2f(0.0f, imageHeight); glVertex3f(2.0f, 2.0f, -2.0f);
      glTexCoord2f(imageWidth, imageHeight); glVertex3f(-2.0f, 2.0f, -2.0f);
      glTexCoord2f(imageWidth, 0.0f); glVertex3f(-2.0f, -2.0f, -2.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(2.0f, -2.0f, -2.0f);

      glTexCoord2f(0.0f, imageHeight); glVertex3f(2.0f, 2.0f, -2.0f);
      glTexCoord2f(imageWidth, imageHeight); glVertex3f(2.0f, 2.0f, 2.0f);
      glTexCoord2f(imageWidth, 0.0f); glVertex3f(2.0f, -2.0f, 2.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(2.0f, -2.0f, -2.0f);

      glTexCoord2f(0.0f, imageHeight); glVertex3f(-2.0f, 2.0f, 2.0f);
      glTexCoord2f(imageWidth, imageHeight); glVertex3f(-2.0f, 2.0f, -2.0f);
      glTexCoord2f(imageWidth, 0.0f); glVertex3f(-2.0f, -2.0f, -2.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f, -2.0f, 2.0f);

      glTexCoord2f(0.0f, imageHeight); glVertex3f(2.0f, -2.0f, -2.0f);
      glTexCoord2f(imageWidth, imageHeight); glVertex3f(-2.0f, -2.0f, -2.0f);
      glTexCoord2f(imageWidth, 0.0f); glVertex3f(-2.0f, -2.0f, 2.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(2.0f, -2.0f, 2.0f);

      glTexCoord2f(0.0f, imageHeight); glVertex3f(2.0f, 2.0f, -2.0f);
      glTexCoord2f(imageWidth, imageHeight); glVertex3f(-2.0f, 2.0f, -2.0f);
      glTexCoord2f(imageWidth, 0.0f); glVertex3f(-2.0f, 2.0f, 2.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(2.0f, 2.0f, 2.0f);

   glEnd();

   // Return out of fbo.
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


void TonePass()
{
   // Enter ortho.
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
   
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   // Bind tone map shader data.
   glUseProgramObjectARB(toneShader);
   glUniform2fARB(glslToneOffset, fullOffset[0], fullOffset[1]);
   glUniform1iARB(glslToneFullTexture, 0);
   glUniform1fARB(glslToneExposure, exposure);
   glUniform2fARB(glslToneRes, WINDOW_WIDTH, WINDOW_HEIGHT);

   // Bind the fullscreen surface image.
   glBindTexture(GL_TEXTURE_RECTANGLE_NV, offScreenID);

   // Draw out the fullscreen quad.   
   glBegin(GL_QUADS);

      glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
      glTexCoord2f(WINDOW_WIDTH, 0.0f); glVertex2f(WINDOW_WIDTH, 0);
      glTexCoord2f(WINDOW_WIDTH, WINDOW_HEIGHT); glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
      glTexCoord2f(0.0f, WINDOW_HEIGHT); glVertex2f(0, WINDOW_HEIGHT);

   glEnd();

   // Return out of ortho mode.
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}


void RenderScene()
{
   // Render passes
   FullScreenPass();
   TonePass();

   glutSwapBuffers();
   glutPostRedisplay();
}


void Shutdown()
{
   // Free all resources.
   if(toneShader) glDeleteObjectARB(toneShader);
   if(hdrID) glDeleteTextures(1, &hdrID);
   if(frameBufferIndex) glDeleteFramebuffersEXT(1, &frameBufferIndex);
   if(depthBufferIndex) glDeleteRenderbuffersEXT(1, &depthBufferIndex);
   if(offScreenID) glDeleteTextures(1, &offScreenID);
}