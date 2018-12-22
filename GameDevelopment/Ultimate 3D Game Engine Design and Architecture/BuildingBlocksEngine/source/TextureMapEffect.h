/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_TEXTURE_MAP_EFFECT_H_
#define _BB_TEXTURE_MAP_EFFECT_H_

#include<RenderInterface.h>
#include<Effect.h>
#include<vector>


DECLARE_ENGINE_NAMESPACE


class TextureMapEffect : public Effect
{
   public:
      TextureMapEffect(char *decalFile, std::vector<BB_FILTER_TYPE> *filters,
                       RenderInterface *renderer)
      {
         m_renderer = renderer;

         if(m_renderer != NULL)
            {
               m_renderer->LoadTexFromFile(decalFile, BB_TEX2D_TYPE,
                                           &m_decalTexture);

               m_renderer->ApplyFilters(m_decalTexture, filters);
            }
      }

      ~TextureMapEffect()
      {

      }

      void ApplyEffect()
      {
         if(m_renderer != NULL)
            m_renderer->ApplyTexture(0, m_decalTexture);
      }


   protected:
      BB_TEXTURE m_decalTexture;
      RenderInterface *m_renderer;
};


class TextureShader : public Effect
{
   public:
      TextureShader(char *vs, char *ps, char *decalFile,
                    std::vector<BB_FILTER_TYPE> *filters,
                    RenderInterface *renderer)
      {
         m_renderer = renderer;

         if(m_renderer != NULL)
            {
               m_renderer->LoadTexFromFile(decalFile, BB_TEX2D_TYPE,
                                           &m_decalTexture);

               m_renderer->ApplyFilters(m_decalTexture, filters);

               m_renderer->CreateShaderFromFile(vs, ps, &m_shader);

               m_renderer->SetupShaderParameter("mvp", m_shader,
                                                &m_mvpParam);

               m_renderer->SetupShaderParameter("decal", m_shader,
                                                &m_decalParam);
            }
      }

      ~TextureShader()
      {

      }

      void ApplyEffect()
      {
         if(m_renderer != NULL)
            {
               m_renderer->ApplyShader(m_shader);

               m_renderer->SetShaderParameter1f(m_decalParam, 0);
               m_renderer->ApplyTexture(0, m_decalTexture);
            }
      }

      void Release()
      {
         if(m_renderer != NULL)
            m_renderer->DeleteTexture(m_decalTexture);
      }


   protected:
      ShaderHandle m_shader;
      ParameterHandle m_mvpParam;
      ParameterHandle m_decalParam;

      BB_TEXTURE m_decalTexture;

      RenderInterface *m_renderer;
};


// *** MULTI_TEXTURING ***


class MultiTexEffect : public Effect
{
   public:
      MultiTexEffect(char *decalFile1, char *decalFile2,
                     std::vector<BB_FILTER_TYPE> *filters,
                     RenderInterface *renderer)
      {
         m_renderer = renderer;

         if(m_renderer != NULL)
            {
               m_renderer->LoadTexFromFile(decalFile1, BB_TEX2D_TYPE,
                                           &m_decalTexture1);

               m_renderer->ApplyFilters(m_decalTexture1, filters);

               m_renderer->LoadTexFromFile(decalFile2, BB_TEX2D_TYPE,
                                           &m_decalTexture2);

               m_renderer->ApplyFilters(m_decalTexture2, filters);
            }
      }

      ~MultiTexEffect()
      {

      }

      void ApplyEffect()
      {
         if(m_renderer != NULL)
            {
               m_renderer->ApplyTexture(0, m_decalTexture1);
               m_renderer->ApplyTexture(1, m_decalTexture2);
            }
      }


   protected:
      BB_TEXTURE m_decalTexture1, m_decalTexture2;
      RenderInterface *m_renderer;
};


class MultiTexShader : public Effect
{
   public:
      MultiTexShader(char *vs, char *ps, char *decalFile1,
                     char *decalFile2,
                     std::vector<BB_FILTER_TYPE> *filters,
                     RenderInterface *renderer)
      {
         m_renderer = renderer;

         if(m_renderer != NULL)
            {
               m_renderer->LoadTexFromFile(decalFile1, BB_TEX2D_TYPE,
                                           &m_decalTexture1);

               m_renderer->ApplyFilters(m_decalTexture1, filters);

               m_renderer->LoadTexFromFile(decalFile2, BB_TEX2D_TYPE,
                                           &m_decalTexture2);

               m_renderer->ApplyFilters(m_decalTexture2, filters);

               m_renderer->CreateShaderFromFile(vs, ps, &m_shader);

               m_renderer->SetupShaderParameter("mvp", m_shader,
                                                &m_mvpParam);

               m_renderer->SetupShaderParameter("decal1", m_shader,
                                                &m_decalParam1);

               m_renderer->SetupShaderParameter("decal2", m_shader,
                                                &m_decalParam2);
            }
      }

      ~MultiTexShader()
      {

      }

      void ApplyEffect()
      {
         if(m_renderer != NULL)
            {
               m_renderer->ApplyShader(m_shader);

               m_renderer->SetShaderParameter1f(m_decalParam1, 0);
               m_renderer->ApplyTexture(0, m_decalTexture1);

               m_renderer->SetShaderParameter1f(m_decalParam2, 1);
               m_renderer->ApplyTexture(1, m_decalTexture2);
            }
      }

      void Release()
      {
         if(m_renderer != NULL)
            {
               m_renderer->DeleteTexture(m_decalTexture1);
               m_renderer->DeleteTexture(m_decalTexture2);
            }
      }


   protected:
      ShaderHandle m_shader;
      ParameterHandle m_mvpParam;
      ParameterHandle m_decalParam1, m_decalParam2;

      BB_TEXTURE m_decalTexture1, m_decalTexture2;

      RenderInterface *m_renderer;
};


END_ENGINE_NAMESPACE


#endif