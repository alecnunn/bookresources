/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_VERTEX_DESCRIPTOR_H_
#define _BB_VERTEX_DESCRIPTOR_H_

#include<NameSpace.h>
#include<Defines.h>
#include<vector>


DECLARE_ENGINE_NAMESPACE


class VertexElement
{
   public:
      VertexElement()
      {
         m_type = BB_ELEMENT_TYPE_NULL;
         m_bytes = 0;
         m_offset = 0;
      }

      bool operator==(VertexElement &e)
      {
         return (m_type == e.m_type &&
                 m_bytes == e.m_bytes &&
                 m_offset == e.m_offset);
      }

      BB_ELEMENT_TYPE m_type;
      int m_bytes;
      int m_offset;
};


class VertexDescriptor
{
   public:
      VertexDescriptor()
      {
         m_currentStride = 0;
      }

      void Clear()
      {
         m_elements.clear();
         m_currentStride = 0;
      }

      BRESULT AddElement(BB_ELEMENT_TYPE type)
      {
         VertexElement element;
         element.m_type = type;
         
         switch(type)
         {
            case BB_ELEMENT_TYPE_IGNORE_3F:
            case BB_ELEMENT_TYPE_VERTEX_3F:
            case BB_ELEMENT_TYPE_NORMAL_3F:
            case BB_ELEMENT_TYPE_COLOR_3F:
            case BB_ELEMENT_TYPE_TEX1_3F:
            case BB_ELEMENT_TYPE_TEX2_3F:
            case BB_ELEMENT_TYPE_TEX3_3F:
            case BB_ELEMENT_TYPE_TEX4_3F:
            case BB_ELEMENT_TYPE_TEX5_3F:
            case BB_ELEMENT_TYPE_TEX6_3F:
            case BB_ELEMENT_TYPE_TEX7_3F:
            case BB_ELEMENT_TYPE_TEX8_3F:
               element.m_bytes = sizeof(float) * 3;
            break;
            
            case BB_ELEMENT_TYPE_IGNORE_2F:
            case BB_ELEMENT_TYPE_TEX1_2F:
            case BB_ELEMENT_TYPE_TEX2_2F:
            case BB_ELEMENT_TYPE_TEX3_2F:
            case BB_ELEMENT_TYPE_TEX4_2F:
            case BB_ELEMENT_TYPE_TEX5_2F:
            case BB_ELEMENT_TYPE_TEX6_2F:
            case BB_ELEMENT_TYPE_TEX7_2F:
            case BB_ELEMENT_TYPE_TEX8_2F:
               element.m_bytes = sizeof(float) * 2;
            break;

            default: break;
         };
         
         if(type == BB_ELEMENT_TYPE_NULL || element.m_bytes <= 0)
            {
               return BB_FAIL;
            }

         element.m_offset = m_currentStride;
         m_currentStride += element.m_bytes;

         m_elements.push_back(element);

         return BB_SUCCESS;
      }


      VertexElement GetElement(int index)
      {
         VertexElement element;

         if(index >= 0 && index < (int)m_elements.size())
            {
               return m_elements[index];
            }

         return element;
      }

      int GetTotalElements()
      {
         return (int)m_elements.size();
      }

      int GetStride()
      {
         return m_currentStride;
      }

   private:
      std::vector<VertexElement> m_elements;
      int m_currentStride;
};


END_ENGINE_NAMESPACE

#endif