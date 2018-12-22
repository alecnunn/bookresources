/*

*/


#ifndef _BB_SKIN_H_
#define _BB_SKIN_H_

#include<defines.h>
#include<ResourceManager.h>


DECLARE_ENGINE_NAMESPACE


class CSkin
{
   public:
      CSkin() : m_shader(-1)
      {
      }

      void SetShader(int shader)
      {
         m_shader = shader;
      }

      int *GetShaderPtr()
      {
         return &m_shader;
      }

      int GetShader()
      {
         return m_shader;
      }

      void SetTexture(int index, BB_TEXTURE texture)
      {
         if(index >= 0 && index < BB_MAX_TEXTURES)
            m_texList[index] = texture;
      }

      BB_TEXTURE *GetTexturePtr(int index)
      {
         if(index >= 0 && index < BB_MAX_TEXTURES)
            return &m_texList[index];

         return NULL;
      }

      BB_TEXTURE GetTexture(int index)
      {
         if(index >= 0 && index < BB_MAX_TEXTURES)
            return m_texList[index];

         BB_TEXTURE null;
         return null;
      }

   private:
      int m_shader;
      BB_TEXTURE m_texList[BB_MAX_TEXTURES];
};


class CSkinResourceTag
{

};


class CSkinResource : public bbe::CBaseResource
{
   public:
      CSkinResource(bbe::CSkin *skin)
      {
         m_skin = skin;
      }

      virtual ~CSkinResource()
      {
         FreeMemory();
      }

      bbe::CSkin *GetSkin()
      {
         return m_skin;
      }

   private:
      void FreeMemory()
      {
         if(m_skin != NULL)
            delete m_skin;

         m_skin = NULL;
      }

      bbe::CSkin *m_skin;
};


END_ENGINE_NAMESPACE


#define BB_SKIN_RESOURCE_MANAGER bbe::CResourceManager<bbe::CSkinResource, bbe::CHandle<bbe::CSkinResourceTag> >

#endif