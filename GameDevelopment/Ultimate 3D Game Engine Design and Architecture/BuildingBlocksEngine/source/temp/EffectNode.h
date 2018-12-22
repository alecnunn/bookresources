/*

*/


#ifndef _BB_EFFECT_NODE_H_
#define _BB_EFFECT_NODE_H_

#include<node.h>
#include<Effect.h>
#include<ResourceManager.h>
#include<RenderInterface.h>


DECLARE_ENGINE_NAMESPACE


class EffectNode : public Node
{
   BB_RTTI_EXPOSE_TYPE;

   public:
      EffectNode(BB_EFFECT_HANDLE &handle,
                 BB_EFFECT_RESOURCE_MANAGER *effectManager,
                 RenderInterface *renderer)
      {
         m_renderer = renderer;
         m_effectManager = effectManager;
         m_handle.Initialize(handle.GetIndex());
      }

      virtual ~EffectNode()
      {
         m_renderer = NULL;
         m_effectManager = NULL;
      }


      void Process()
      {
         if(m_effectManager != NULL && m_renderer != NULL)
            {
               EffectResource *effectResPtr = NULL;
               Effect *effectPtr = NULL;

               if(m_effectManager != NULL)
                  effectResPtr = m_effectManager->GetResource(m_handle);

               if(effectResPtr != NULL)
                  effectPtr = effectResPtr->GetEffect();

               if(effectPtr != NULL)
                  effectPtr->ApplyEffect();
            }

         Node::Process();
      }

   protected:
      BB_EFFECT_HANDLE m_handle;
      BB_EFFECT_RESOURCE_MANAGER *m_effectManager;

      RenderInterface *m_renderer;
};

RTTI EffectNode::m_RTTIType("EffectNode", &Node::m_RTTIType);


END_ENGINE_NAMESPACE


#endif