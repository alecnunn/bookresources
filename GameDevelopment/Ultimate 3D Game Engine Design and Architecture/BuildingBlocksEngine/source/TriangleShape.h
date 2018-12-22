/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_TRIANGLE_SHAPE_H_
#define _BB_TRIANGLE_SHAPE_H_

#include<ModelData.h>


struct stUnitTriVertex
{
   float x, y, z;
   float tu1, tv1;
   float nx, ny, nz;
};


#define UNIT_TRI_VERTEX_TOTAL 3
#define UNIT_TRI_INDEX_TOTAL  3
#define UNIT_TRI_STRIDE       sizeof(stUnitTriVertex)


DECLARE_ENGINE_NAMESPACE


bool CreateTriangleMesh(float size, bbe::ModelData *model)
{
   if(model == NULL)
      return false;

   stUnitTriVertex UnitTriData[UNIT_TRI_VERTEX_TOTAL] =
   {
       1 * size, -1 * size, 0,     1, 0,  0, 0, 1,
      -1 * size, -1 * size, 0,     0, 0,  0, 0, 1,
              0,  1 * size, 0,  0.5f, 1,  0, 0, 1
   };

   unsigned int UnitTriIndices[UNIT_TRI_INDEX_TOTAL] =
   {
      0, 1, 2
   };


   model->Clear();

   model->AddDescriptorElement(BB_ELEMENT_TYPE_VERTEX_3F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_TEX1_2F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_NORMAL_3F);

   if(model->SetVertices(BB_PRIMITIVE_TRI_LIST, UNIT_TRI_VERTEX_TOTAL,
      UNIT_TRI_STRIDE, (char*)UnitTriData) == false)
      {
         return false;
      }

   if(model->SetIndices(UNIT_TRI_INDEX_TOTAL, UnitTriIndices) == false)
      {
         return false;
      }

   return true;
}


bool CreateUnitTriangleMesh(bbe::ModelData *model)
{
   return CreateTriangleMesh(1, model);
}


END_ENGINE_NAMESPACE


#endif