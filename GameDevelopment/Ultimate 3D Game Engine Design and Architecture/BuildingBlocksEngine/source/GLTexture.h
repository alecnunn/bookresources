/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_GL_TEXTURE_H_
#define _BB_GL_TEXTURE_H_


#include<string>


DECLARE_ENGINE_NAMESPACE


class GLTexture
{
   public:
      GLTexture(int id, BB_TEXTURE_TYPE type)
      {
         m_id = id;
         m_type = type;
      }

      ~GLTexture()
      {
         FreeMemory();
      }

      unsigned int GetID() { return m_id; }
      BB_TEXTURE_TYPE GetTextureType() { return m_type; }

   private:
      void FreeMemory()
      {
         if(m_id > 0)
            glDeleteTextures(1, &m_id);

         m_id = 0;
      }

   private:
      GLuint m_id;
      BB_TEXTURE_TYPE m_type;
};


END_ENGINE_NAMESPACE

#endif