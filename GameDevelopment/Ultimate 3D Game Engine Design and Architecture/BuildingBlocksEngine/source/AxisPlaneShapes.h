/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_PLANE_SHAPE_H_
#define _BB_PLANE_SHAPE_H_

#include<ModelData.h>


struct stAxisPlaneVertex
{
   float x, y, z;
   float tu1, tv1;
   float nx, ny, nz;
};


#define UNIT_AXIS_PLANE_VERTEX_TOTAL   4
#define UNIT_AXIS_PLANE_INDEX_TOTAL    6
#define UNIT_AXIS_PLANE_STRIDE         sizeof(stAxisPlaneVertex)


DECLARE_ENGINE_NAMESPACE


bool CreateXZPlaneMesh(float size, bbe::ModelData *model)
{
   if(model == NULL)
      return false;

   stAxisPlaneVertex PlaneData[UNIT_SQUARE_VERTEX_TOTAL] =
   {
       1 * size, 0, -1 * size,  1, 0,  0, 1, 0,
      -1 * size, 0, -1 * size,  0, 0,  0, 1, 0,
      -1 * size, 0,  1 * size,  0, 1,  0, 1, 0,
       1 * size, 0,  1 * size,  1, 1,  0, 1, 0
   };

   unsigned int PlaneIndices[UNIT_SQUARE_INDEX_TOTAL] =
   {
      0, 1, 2, 2, 3, 0
   };


   model->Clear();

   model->AddDescriptorElement(BB_ELEMENT_TYPE_VERTEX_3F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_TEX1_2F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_NORMAL_3F);

   if(model->SetVertices(BB_PRIMITIVE_TRI_LIST, UNIT_AXIS_PLANE_VERTEX_TOTAL,
      UNIT_AXIS_PLANE_STRIDE, (char*)PlaneData) == false)
      {
         return false;
      }

   if(model->SetIndices(UNIT_AXIS_PLANE_INDEX_TOTAL, PlaneIndices) == false)
      {
         return false;
      }

   return true;
}


bool CreateUnitXZPlaneMesh(bbe::ModelData *model)
{
   return CreateXZPlaneMesh(1, model);
}


END_ENGINE_NAMESPACE


#endif