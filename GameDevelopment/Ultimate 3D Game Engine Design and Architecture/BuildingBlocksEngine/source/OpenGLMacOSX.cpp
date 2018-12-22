/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<OpenGL.h>


AGLContext g_aglContext;


DECLARE_ENGINE_NAMESPACE


BRESULT OpenGLRenderer::InitializeOS(const RenderParams *params)
{
   if(params == NULL)
      return BB_FAIL;

   if(RenderInterface::Initialize(params) != BB_SUCCESS)
      return BB_PARAMETERS_FAIL;

   int aglAttri[8];
	aglAttri[0] = AGL_RGBA;
	aglAttri[1] = AGL_DEPTH_SIZE;
	aglAttri[2] =  params->m_depthBits;
	aglAttri[3] = AGL_STENCIL_SIZE;
	aglAttri[4] =  params->m_stencilBits;
	aglAttri[5] = AGL_DOUBLEBUFFER;
	aglAttri[6] = AGL_ACCELERATED;
	aglAttri[7] =  AGL_NONE;
	
	AGLPixelFormat pixelFormat;
	pixelFormat = aglChoosePixelFormat(0, 0, (const GLint*)aglAttri);
	g_aglContext = aglCreateContext(pixelFormat, 0);
	aglDestroyPixelFormat(pixelFormat);
	
	SetPortWindowPort(m_params.m_handle.m_window);
	aglSetDrawable(g_aglContext, GetWindowPort(m_params.m_handle.m_window));
	aglSetCurrentContext(g_aglContext);

   return BB_SUCCESS;
}


void OpenGLRenderer::ShutdownOS()
{
   aglDestroyContext(g_aglContext);
	DisposeWindow(m_params.m_handle.m_window);
}


void OpenGLRenderer::EndRendering()
{
   aglSwapBuffers(g_aglContext);
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
         m_vboSupport = true;
      }

   if(strstr(extension, "GL_ARB_multitexture") != 0)
      {
         m_multiTexSupport = true;
      }

   if(strstr(extension, "GL_ARB_texture_cube_map") != 0)
      {
         m_cubeMapSupport = true;
      }

   if(strstr(extension, "GL_EXT_framebuffer_object") != 0)
      {
         m_fboSupport = true;
      }
   
   if(strstr(extension, "GL_ARB_shading_language_100") != 0)
      {
         if(strstr(extension, "GL_ARB_shader_objects") != 0)
            {
               m_glslSupport = true;
            }
      }
}


END_ENGINE_NAMESPACE