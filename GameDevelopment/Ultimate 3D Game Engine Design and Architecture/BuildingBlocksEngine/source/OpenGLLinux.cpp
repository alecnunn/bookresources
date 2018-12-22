/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<OpenGL.h>
#include<OpenGLExtensions.h>

PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB = NULL;

//PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;
//PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;

PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
PFNGLGETINFOLOGARBPROC glGetInfoLogARB = NULL;
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB = NULL;
PFNGLUNIFORM4FARBPROC glUniform4fARB = NULL;
PFNGLUNIFORM3FARBPROC glUniform3fARB = NULL;
PFNGLUNIFORM2FARBPROC glUniform2fARB = NULL;
PFNGLUNIFORM1FARBPROC glUniform1fARB = NULL;
PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;


DECLARE_ENGINE_NAMESPACE


typedef struct
{
   int screen;
   Window win;
   GLXContext context;

   XSetWindowAttributes attributes;
   XF86VidModeModeInfo deskMode;

   int x, y;
} stLinuxGLWindow;


stLinuxGLWindow g_opengl;


BRESULT OpenGLRenderer::InitializeOS(const RenderParams *params)
{
   if(params == NULL)
      return BB_FAIL;

   if(RenderInterface::Initialize(params) != BB_SUCCESS)
      return BB_PARAMETERS_FAIL;

   XVisualInfo *vi;
   XF86VidModeModeInfo **modes;
   Atom wmDelete;
   Window winDummy;
   unsigned int borderDummy;
   int dpyWidth, dpyHeight;
   int i;
   int glxMajorVersion, glxMinorVersion;
   int vidModeMajorVersion, vidModeMinorVersion;
   int modeNum, bestMode;

   bestMode = 0;

   g_opengl.screen = DefaultScreen(m_params.m_handle.m_disp);

   XF86VidModeQueryVersion(m_params.m_handle.m_disp, &vidModeMajorVersion,
                           &vidModeMinorVersion);

   XF86VidModeGetAllModeLines(m_params.m_handle.m_disp, g_opengl.screen,
                              &modeNum, &modes);

   g_opengl.deskMode = *modes[0];

   for(i = 0; i < modeNum; i++)
      {
         if((modes[i]->hdisplay == m_params.m_width) && (modes[i]->vdisplay == m_params.m_height))
            bestMode = i;
      }

   // TODO: Allow params color bits to manipulate the following...

   static int attrListSgl[] =
   {
      GLX_RGBA, 
      GLX_RED_SIZE, 8, 
      GLX_GREEN_SIZE, 8, 
      GLX_BLUE_SIZE, 8, 
      GLX_DEPTH_SIZE, m_params.m_depthBits,
      None
   };

   static int attrListDbl[] =
   {
      GLX_RGBA, GLX_DOUBLEBUFFER, 
      GLX_RED_SIZE, 8, 
      GLX_GREEN_SIZE, 8, 
      GLX_BLUE_SIZE, 8, 
      GLX_DEPTH_SIZE, m_params.m_depthBits,
      None
   };

   vi = glXChooseVisual(m_params.m_handle.m_disp, g_opengl.screen, attrListDbl);

   // Try single.
   if(vi == NULL)
      vi = glXChooseVisual(m_params.m_handle.m_disp, g_opengl.screen, attrListSgl);

   g_opengl.context = glXCreateContext(m_params.m_handle.m_disp, vi, 0, GL_TRUE);

   g_opengl.attributes.colormap = XCreateColormap(m_params.m_handle.m_disp,
      RootWindow(m_params.m_handle.m_disp, vi->screen), vi->visual, AllocNone);

   g_opengl.attributes.border_pixel = 0;

   if(m_params.m_fullscreen)
      {
         XF86VidModeSwitchToMode(m_params.m_handle.m_disp, g_opengl.screen,
                                 modes[bestMode]);

         XF86VidModeSetViewPort(m_params.m_handle.m_disp, g_opengl.screen,
                                0, 0);

         dpyWidth = modes[bestMode]->hdisplay;
         dpyHeight = modes[bestMode]->vdisplay;

         XFree(modes);
    
         g_opengl.attributes.override_redirect = True;
         g_opengl.attributes.event_mask = ExposureMask | KeyPressMask |
                                          ButtonPressMask |
                                          StructureNotifyMask;

         g_opengl.win = XCreateWindow(m_params.m_handle.m_disp,
            RootWindow(m_params.m_handle.m_disp, vi->screen),
            0, 0, dpyWidth, dpyHeight, 0, vi->depth,
            InputOutput, vi->visual, CWBorderPixel |
            CWColormap | CWEventMask |
            CWOverrideRedirect, &g_opengl.attributes);

         XWarpPointer(m_params.m_handle.m_disp, None, g_opengl.win,
                      0, 0, 0, 0, 0, 0);

         XMapRaised(m_params.m_handle.m_disp, g_opengl.win);

         XGrabKeyboard(m_params.m_handle.m_disp, g_opengl.win, True, GrabModeAsync,
                       GrabModeAsync, CurrentTime);

         XGrabPointer(m_params.m_handle.m_disp, g_opengl.win, True, ButtonPressMask,
                      GrabModeAsync, GrabModeAsync, g_opengl.win, None,
                      CurrentTime);
      }
   else
      {
         g_opengl.attributes.event_mask = ExposureMask | KeyPressMask |
                                 ButtonPressMask | StructureNotifyMask;

         g_opengl.win = XCreateWindow(m_params.m_handle.m_disp,
            RootWindow(m_params.m_handle.m_disp, vi->screen),
            0, 0, m_params.m_width, m_params.m_height, 0, vi->depth,
            InputOutput, vi->visual, CWBorderPixel |
            CWColormap | CWEventMask, &g_opengl.attributes);

         // TO DO: Look into possibly having the window name as a
         // RenderParams member.

         wmDelete = XInternAtom(m_params.m_handle.m_disp, "WM_DELETE_WINDOW", True);
         XSetWMProtocols(m_params.m_handle.m_disp, g_opengl.win, &wmDelete, 1);
         XSetStandardProperties(m_params.m_handle.m_disp, g_opengl.win, "",
                                "", None, NULL, 0, NULL);

         XMapRaised(m_params.m_handle.m_disp, g_opengl.win);
      }

   glXMakeCurrent(m_params.m_handle.m_disp, g_opengl.win, g_opengl.context);

   unsigned int depthBits = (unsigned int)m_params.m_depthBits;
   unsigned int width = (unsigned int)m_params.m_width;
   unsigned int height = (unsigned int)m_params.m_height;

   XGetGeometry(m_params.m_handle.m_disp, g_opengl.win, &winDummy,
                &g_opengl.x, &g_opengl.y, &width,
                &height, &borderDummy,
                &depthBits);

   return BB_SUCCESS;
}


void OpenGLRenderer::ShutdownOS()
{
   if(g_opengl.context)
      {
         if(!glXMakeCurrent(m_params.m_handle.m_disp, None, NULL))
            printf("Error getting context.\n");

         glXDestroyContext(m_params.m_handle.m_disp, g_opengl.context);
         g_opengl.context = NULL;
      }


   if(m_params.m_fullscreen)
      {
         XF86VidModeSwitchToMode(m_params.m_handle.m_disp, g_opengl.screen,
                                 &g_opengl.deskMode);

         XF86VidModeSetViewPort(m_params.m_handle.m_disp, g_opengl.screen,
                                0, 0);
      }
}


void OpenGLRenderer::EndRendering()
{
   glXSwapBuffers(m_params.m_handle.m_disp, g_opengl.win);
}


void OpenGLRenderer::InitializeExtensions()
{
   char *extension = (char*)glGetString(GL_EXTENSIONS);

   m_vboSupport = false;
   m_multiTexSupport = true;
   m_cubeMapSupport = false;
   m_fboSupport = false;
   m_glslSupport = false;

   if(strstr(extension, "ARB_vertex_buffer_object") != 0)
      {
         glBindBufferARB = (PFNGLBINDBUFFERARBPROC)glXGetProcAddress((const GLubyte*)"glBindBufferARB");
         glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)glXGetProcAddress((const GLubyte*)"glDeleteBuffersARB");
         glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)glXGetProcAddress((const GLubyte*)"glGenBuffersARB");
         glBufferDataARB = (PFNGLBUFFERDATAARBPROC)glXGetProcAddress((const GLubyte*)"glBufferDataARB");
         glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)glXGetProcAddress((const GLubyte*)"glGetBufferParameterivARB");

         if(glBindBufferARB && glDeleteBuffersARB && glGenBuffersARB &&
            glBufferDataARB && glGetBufferParameterivARB)
            {
               m_vboSupport = true;
            }
      }

   if(strstr(extension, "GL_ARB_multitexture") != 0)
      {
         /*glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)glXGetProcAddress((const GLubyte*)"glClientActiveTextureARB");
         glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)glXGetProcAddress((const GLubyte*)"glActiveTextureARB");

         if(glClientActiveTextureARB && glActiveTextureARB)
            {
               m_multiTexSupport = true;
            }*/
         m_multiTexSupport = true;
      }

   if(strstr(extension, "GL_ARB_texture_cube_map") != 0)
      {
         m_cubeMapSupport = true;
      }

   if(strstr(extension, "GL_ARB_shading_language_100") != 0)
      {
         if(strstr(extension, "GL_ARB_shader_objects") != 0)
            {
               m_glslSupport = true;

               glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)glXGetProcAddress((const GLubyte*)"glCreateProgramObjectARB");
               glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)glXGetProcAddress((const GLubyte*)"glCreateShaderObjectARB");
               glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)glXGetProcAddress((const GLubyte*)"glCompileShaderARB");
               glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)glXGetProcAddress((const GLubyte*)"glLinkProgramARB");
               glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)glXGetProcAddress((const GLubyte*)"glGetInfoLogARB");
               glDeleteObjectARB  = (PFNGLDELETEOBJECTARBPROC)glXGetProcAddress((const GLubyte*)"glDeleteObjectARB");
               glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)glXGetProcAddress((const GLubyte*)"glUseProgramObjectARB");
               glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)glXGetProcAddress((const GLubyte*)"glShaderSourceARB");
               glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)glXGetProcAddress((const GLubyte*)"glAttachObjectARB");
               glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)glXGetProcAddress((const GLubyte*)"glGetObjectParameterivARB");
               glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)glXGetProcAddress((const GLubyte*)"glGetUniformLocationARB");
               glUniform4fARB = (PFNGLUNIFORM4FARBPROC)glXGetProcAddress((const GLubyte*)"glUniform4fARB");
               glUniform3fARB = (PFNGLUNIFORM3FARBPROC)glXGetProcAddress((const GLubyte*)"glUniform3fARB");
               glUniform2fARB = (PFNGLUNIFORM2FARBPROC)glXGetProcAddress((const GLubyte*)"glUniform2fARB");
               glUniform1fARB = (PFNGLUNIFORM1FARBPROC)glXGetProcAddress((const GLubyte*)"glUniform1fARB");
               glUniform1iARB = (PFNGLUNIFORM1IARBPROC)glXGetProcAddress((const GLubyte*)"glUniform1iARB");

               if(glCreateProgramObjectARB || glCreateShaderObjectARB ||  glCompileShaderARB ||
                  glLinkProgramARB || glGetInfoLogARB || glDeleteObjectARB ||
                  glUseProgramObjectARB || glShaderSourceARB || glAttachObjectARB ||
                  glGetObjectParameterivARB || glGetUniformLocationARB || glUniform4fARB ||
                  glUniform2fARB || glUniform1fARB || glUniform1iARB || glUniform3fARB)
                  {
                     m_glslSupport = true;
                  }
            }
         }
}


END_ENGINE_NAMESPACE
