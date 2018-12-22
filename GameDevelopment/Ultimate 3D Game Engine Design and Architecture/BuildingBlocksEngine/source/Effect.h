/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_EFFECT_H_
#define _BB_EFFECT_H_

#include<ResourceManager.h>


DECLARE_ENGINE_NAMESPACE


class Effect
{
   public:
      Effect()
      {

      }

      virtual ~Effect()
      {
         Release();
      }

      virtual void ApplyEffect() = 0;

      virtual void Release()
      {
      }
};


class EffectResourceTag
{

};


class EffectResource
{
   public:
      EffectResource(Effect *effect)
      {
         m_effect = effect;
      }

      virtual ~EffectResource()
      {
         FreeMemory();
      }

      Effect *GetEffect()
      {
         return m_effect;
      }

   private:
      void FreeMemory()
      {
         if(m_effect != NULL)
            delete m_effect;

         m_effect = NULL;
      }

      Effect *m_effect;
};


END_ENGINE_NAMESPACE


#define BB_EFFECT_HANDLE bbe::Handle<bbe::EffectResourceTag>
#define BB_EFFECT_RESOURCE_MANAGER bbe::ResourceManager<bbe::EffectResource, BB_EFFECT_HANDLE >


#endif