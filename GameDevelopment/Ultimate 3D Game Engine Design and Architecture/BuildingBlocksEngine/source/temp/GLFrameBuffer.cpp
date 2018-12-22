/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<GLHeader.h>
#include<GLFrameBuffer.h>


DECLARE_ENGINE_NAMESPACE


bool FrameBufferObject::CreateFBO(int width, int height)
{
   glGenFramebuffersEXT(1, &m_frameBuffer);
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_frameBuffer);

   if(width > 0 && height > 0)
      {
         glGenRenderbuffersEXT(1, &m_renderBuffer);
         glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_renderBuffer);

         glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24,
                                    width, height);

         glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                       GL_RENDERBUFFER_EXT, m_renderBuffer);
      }

   glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &m_maxColorAttachments);

   GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

   if(status != GL_FRAMEBUFFER_COMPLETE_EXT ||
      status != GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)
      {
         return false;
      }

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

   return true;
}


void FrameBufferObject::BindFrameBuffer()
{
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_frameBuffer);
}


void FrameBufferObject::unBindFrameBuffer()
{
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


bool FrameBufferObject::AttachColorTexture(unsigned int textureID, int unit)
{
   if(unit >= m_maxColorAttachments)
      return false;

   int glAttachment = GL_COLOR_ATTACHMENT0_EXT + unit;

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_frameBuffer);

   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D, textureID, 0);

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

   return true;
}


void FrameBufferObject::Shutdown()
{
   if(m_frameBuffer > 0)
      glDeleteRenderbuffersEXT(1, &m_frameBuffer);

   if(m_renderBuffer > 0)
      glDeleteFramebuffersEXT(1, &m_renderBuffer);

   m_frameBuffer = 0;
   m_renderBuffer = 0;
}


END_ENGINE_NAMESPACE