/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_GEOMETRY_NODE_H_
#define _BB_GEOMETRY_NODE_H_

#include<node.h>
#include<DynamicModel.h>


DECLARE_ENGINE_NAMESPACE


class GeometryNode : public Node
{
   public:
      GeometryNode(DynamicModel *model,
                   BB_MODEL_RESOURCE_MANAGER *modelManager,
                   BB_EFFECT_RESOURCE_MANAGER *effectManager,
                   RenderInterface *renderer)
      {
         m_renderer = renderer;
         m_model = model;
         m_modelManager = modelManager;
         m_effectManager = effectManager;
      }

      ~GeometryNode()
      {
         m_renderer = NULL;
         m_model = NULL;
         m_modelManager = NULL;
         m_effectManager = NULL;
      }


      void Process()
      {
         if(m_model != NULL && m_modelManager != NULL && m_renderer != NULL)
            {
               ModelResource *modelResPtr = NULL;
               ModelData *mPtr = NULL;
               bbe::EffectResource *effectResPtr = NULL;
               bbe::Effect *effectPtr = NULL;

               modelResPtr = m_modelManager->GetResource(
                  m_model->GetModelHandle());

               if(m_effectManager != NULL)
                  {
                     effectResPtr = m_effectManager->GetResource(
                        m_model->GetEffectHandle());
                  }

               if(effectResPtr != NULL)
                  {
                     effectPtr = effectResPtr->GetEffect();
                  }

               if(modelResPtr != NULL)
                  {
                     mPtr = modelResPtr->GetModel();

                     if(mPtr != NULL)
                        {
                           m_renderer->PushMatrix();

                              m_renderer->MultiplyToCurrentMatrix(
                                 m_model->GetTransformation());

                              if(effectPtr != NULL)
                                 {
                                    effectPtr->ApplyEffect();
                                 }

                              m_renderer->Render(
                                 BB_RENDER_MODEL_DATA_PARAMS((*mPtr)));

                           m_renderer->PopMatrix();
                        }
                  }
            }


         Node::Process();
      }

   protected:
      DynamicModel *m_model;

      BB_MODEL_RESOURCE_MANAGER *m_modelManager;
      BB_EFFECT_RESOURCE_MANAGER *m_effectManager;

      RenderInterface *m_renderer;
};


END_ENGINE_NAMESPACE


#endif