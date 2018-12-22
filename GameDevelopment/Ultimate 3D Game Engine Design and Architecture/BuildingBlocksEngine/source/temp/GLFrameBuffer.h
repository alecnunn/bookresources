/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _GL_FBO_H_
#define _GL_FBO_H_


#include<NameSpace.h>
#include<Defines.h>


DECLARE_ENGINE_NAMESPACE


class FrameBufferObject
{
   public:
      FrameBufferObject()
      {
         m_frameBuffer = 0;
         m_renderBuffer = 0;
         m_maxColorAttachments = 0;
      }

      ~FrameBufferObject()
      {
      }


      bool CreateFBO(int width, int height);

      void BindFrameBuffer();
      void unBindFrameBuffer();
      bool AttachColorTexture(unsigned int textureID, int unit);
      
      void Shutdown();

      bool operator==(FrameBufferObject &fb)
      {
         return (m_frameBuffer == fb.m_frameBuffer &&
                 m_renderBuffer == fb.m_renderBuffer);
      }

      unsigned int GetFrameBuffer()
      {
         return m_frameBuffer;
      }
      
      unsigned int GetRenderBuffer()
      {
         return m_renderBuffer;
      }

      int GetMaxColorAttachments()
      {
         return m_maxColorAttachments;
      }

   private:
      GLuint m_frameBuffer;
      GLuint m_renderBuffer;

      GLint m_maxColorAttachments;
};


END_ENGINE_NAMESPACE


#endif