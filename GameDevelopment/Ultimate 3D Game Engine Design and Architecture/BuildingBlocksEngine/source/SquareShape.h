/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_SQUARE_SHAPE_H_
#define _BB_SQUARE_SHAPE_H_

#include<ModelData.h>


struct stUnitSquareVertex
{
   float x, y, z;
   float tu1, tv1;
   float nx, ny, nz;
};


#define UNIT_SQUARE_VERTEX_TOTAL 4
#define UNIT_SQUARE_INDEX_TOTAL  6
#define UNIT_SQUARE_STRIDE       sizeof(stUnitSquareVertex)


DECLARE_ENGINE_NAMESPACE


bool CreateSquareMesh(float width, float height, bbe::ModelData *model)
{
   if(model == NULL)
      return false;

   stUnitSquareVertex UnitSquareData[UNIT_SQUARE_VERTEX_TOTAL] =
   {
       1 * width, -1 * height, 0,  1, 0,  0, 0, 1,
      -1 * width, -1 * height, 0,  0, 0,  0, 0, 1,
      -1 * width,  1 * height, 0,  0, 1,  0, 0, 1,
       1 * width,  1 * height, 0,  1, 1,  0, 0, 1
   };

   unsigned int UnitSquareIndices[UNIT_SQUARE_INDEX_TOTAL] =
   {
      0, 1, 2, 2, 3, 0
   };


   model->Clear();

   model->AddDescriptorElement(BB_ELEMENT_TYPE_VERTEX_3F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_TEX1_2F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_NORMAL_3F);

   if(model->SetVertices(BB_PRIMITIVE_TRI_LIST, UNIT_SQUARE_VERTEX_TOTAL,
      UNIT_SQUARE_STRIDE, (char*)UnitSquareData) == false)
      {
         return false;
      }

   if(model->SetIndices(UNIT_SQUARE_INDEX_TOTAL, UnitSquareIndices) == false)
      {
         return false;
      }

   return true;
}


bool CreateUnitSquareMesh(bbe::ModelData *model)
{
   return CreateSquareMesh(1, 1, model);
}


END_ENGINE_NAMESPACE


#endif