/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_DYNAMIC_MODEL_H_
#define _BB_DYNAMIC_MODEL_H_

#include<VertexDescriptor.h>
#include<ModelData.h>
#include<Effect.h>
#include<ResourceManager.h>
#include<Matrix.h>


DECLARE_ENGINE_NAMESPACE


class DynamicModel
{
   public:
      DynamicModel()
      {
         for(int i = 0; i < BB_MAX_LOD; i++)
            m_modelHandle[i].Nullify();

         m_effectHandle.Nullify();

         m_isVisible = true;
         m_currentLOD = 0;
      }

      virtual ~DynamicModel()
      {
         for(int i = 0; i < BB_MAX_LOD; i++)
            m_modelHandle[i].Nullify();

         m_effectHandle.Nullify();
      }

      float *GetTransformation()
      {
         return m_transformationMatrix.matrix;
      }

      void SetTransformation(Matrix4x4 m)
      {
         m_transformationMatrix = m;
      }

      Handle<ModelResourceTag> GetModelHandle()
      {
         return m_modelHandle[m_currentLOD];
      }

      Handle<EffectResourceTag> GetEffectHandle()
      {
         return m_effectHandle;
      }

      void SetModelHandle(Handle<ModelResourceTag> &handle)
      {
         m_modelHandle[m_currentLOD].Initialize(handle.GetIndex());
      }

      void SetEffectHandle(Handle<EffectResourceTag> &handle)
      {
         m_effectHandle.Initialize(handle.GetIndex());
      }

      void SetCurrentLOD(int index)
      {
         if(index >= 0 && index < BB_MAX_LOD)
            m_currentLOD = index;
      }
      
      int GetCurrentLOD()
      {
         return m_currentLOD;
      }

      void SetIsVisible(bool val)
      {
         m_isVisible = val;
      }

      bool GetIsVisible()
      {
         return m_isVisible;
      }

   private:
      Handle<ModelResourceTag> m_modelHandle[BB_MAX_LOD];
      Handle<EffectResourceTag> m_effectHandle;
      Matrix4x4 m_transformationMatrix;

      int m_currentLOD;
      bool m_isVisible;
};


END_ENGINE_NAMESPACE

#endif