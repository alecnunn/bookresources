/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<OpenGL.h>
#include<OpenGLExtensions.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

HDC g_hdc = 0;
HGLRC g_hrc = 0;

PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB = NULL;

PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;

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


BRESULT OpenGLRenderer::InitializeOS(const RenderParams *params)
{
   int nPixelFormat = 0;

   if(params == NULL)
      return BB_FAIL;

   if(RenderInterface::Initialize(params) != BB_SUCCESS)
      return BB_PARAMETERS_FAIL;

   g_hdc = GetDC(params->m_handle.m_hwnd);

   if(!g_hdc)
      return BB_DEVICE_FAIL;

   PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW |
      PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
      params->m_colorBits, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      params->m_depthBits, params->m_stencilBits, 0, PFD_MAIN_PLANE,
      0, 0, 0, 0 };

   nPixelFormat = ChoosePixelFormat(g_hdc, &pfd);
   SetPixelFormat(g_hdc, nPixelFormat, &pfd);

   g_hrc = wglCreateContext(g_hdc);
   wglMakeCurrent(g_hdc, g_hrc);

   return BB_SUCCESS;
}


void OpenGLRenderer::ShutdownOS()
{
   if(g_hdc != 0)
      wglMakeCurrent(g_hdc, NULL);

   if(g_hrc != 0)
      wglDeleteContext(g_hrc);

   g_hdc = 0;
   g_hrc = 0;
}


void OpenGLRenderer::EndRendering()
{
   SwapBuffers(g_hdc);
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
         glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
         glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
         glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
         glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
         glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");

         if(glBindBufferARB && glDeleteBuffersARB && glGenBuffersARB &&
            glBufferDataARB && glGetBufferParameterivARB)
            {
               m_vboSupport = true;
            }
      }

   if(strstr(extension, "GL_ARB_multitexture") != 0)
      {
         glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
         glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");

         if(glClientActiveTextureARB && glActiveTextureARB)
            {
               m_multiTexSupport = true;
            }
      }

   if(strstr(extension, "GL_ARB_texture_cube_map") != 0)
      {
         m_cubeMapSupport = true;
      }

   if(strstr(extension, "GL_ARB_shading_language_100") != 0)
      {
         if(strstr(extension, "GL_ARB_shader_objects") != 0)
            {
               glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
               glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
               glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
               glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
               glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
               glDeleteObjectARB  = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
               glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
               glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
               glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
               glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
               glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
               glUniform4fARB = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
               glUniform3fARB = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress("glUniform3fARB");
               glUniform2fARB = (PFNGLUNIFORM2FARBPROC)wglGetProcAddress("glUniform2fARB");
               glUniform1fARB = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
               glUniform1iARB = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");

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
