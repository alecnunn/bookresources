/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_MODEL_DATA_H_
#define _BB_MODEL_DATA_H_

#include<NameSpace.h>
#include<Defines.h>
#include<VertexDescriptor.h>


#define BB_RENDER_MODEL_DATA_PARAMS(model) model.GetPrimitiveType(), model.GetDescriptor(),\
                                           model.GetVertices(), model.GetIndices(),\
                                           model.GetVertexSizeInBytes(),\
                                           model.GetTotalIndices(),\
                                           model.GetVertexStride()


DECLARE_ENGINE_NAMESPACE


class ModelResourceTag
{

};


class ModelData
{
   public:
      ModelData()
      {
         m_indices = 0;
         m_totalIndices = 0;
         m_vertices = 0;
         m_totalVertices = 0;
         m_primitiveType = BB_PRIMITIVE_NULL;
      }

      ~ModelData()
      {
         Clear();
      }

      void Clear()
      {
         if(m_indices)
            delete[] m_indices;

         if(m_vertices)
            delete[] m_vertices;

         m_indices = 0;
         m_totalIndices = 0;
         m_vertices = 0;
         m_totalVertices = 0;
         m_primitiveType = BB_PRIMITIVE_NULL;
      }


      void ClearDescriptor()
      {
         m_descriptor.Clear();
      }

      BRESULT AddDescriptorElement(BB_ELEMENT_TYPE type)
      {
         return m_descriptor.AddElement(type);
      }

      VertexDescriptor *GetDescriptor()
      {
         return &m_descriptor;
      }


      bool SetIndices(int totalIndices, unsigned int *indices)
      {
         if(totalIndices <= 0 || indices == NULL)
            return false;

         if(m_indices)
            delete[] m_indices;

         m_totalIndices = totalIndices;

         m_indices = new unsigned int[totalIndices];

         if(m_indices == NULL)
            return false;

         memcpy(m_indices, indices, sizeof(unsigned int) * totalIndices);

         return true;
      }

      int GetTotalIndices()
      {
         return m_totalIndices;
      }

      unsigned int *GetIndices()
      {
         return m_indices;
      }



      bool SetVertices(BB_PRIMITIVE_TYPE type, int totalVertices,
                       int stride, char *vertices)
      {
         if(totalVertices <= 0 || vertices == NULL || stride <= 0)
            return false;

         m_primitiveType = type;

         if(m_vertices)
            delete[] m_vertices;

         m_totalVertices = totalVertices;

         m_vertices = new char[stride * totalVertices];

         if(m_vertices == NULL)
            return false;

         memcpy(m_vertices, vertices, stride * totalVertices);

         return true;
      }

      int GetTotalVertices()
      {
         return m_totalVertices;
      }

      int GetVertexSizeInBytes()
      {
         return m_descriptor.GetStride() * m_totalVertices;
      }

      int GetVertexStride()
      {
         return m_descriptor.GetStride();
      }

      char *GetVertices()
      {
         return m_vertices;
      }

      BB_PRIMITIVE_TYPE GetPrimitiveType()
      {
         return m_primitiveType;
      }


   private:
      unsigned int *m_indices;
      int m_totalIndices;

      char *m_vertices;
      int m_totalVertices;

      VertexDescriptor m_descriptor;

      BB_PRIMITIVE_TYPE m_primitiveType;
};


class ModelResource
{
   public:
      ModelResource(ModelData *model)
      {
         m_model = model;
      }

      ~ModelResource()
      {
         FreeMemory();
      }

      ModelData *GetModel()
      {
         return m_model;
      }

   private:
      void FreeMemory()
      {
         if(m_model != NULL)
            delete m_model;

         m_model = NULL;
      }

      ModelData *m_model;
};


END_ENGINE_NAMESPACE


#define BB_MODEL_RESOURCE_MANAGER bbe::ResourceManager<bbe::ModelResource, bbe::Handle<bbe::ModelResourceTag> >

#endif
