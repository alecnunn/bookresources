/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_BOX_SHAPE_H_
#define _BB_BOX_SHAPE_H_

#include<ModelData.h>
#include<Plane.h>
#include<vector>


struct stUnitBoxVertex
{
   float x, y, z;
   float tu1, tv1;
   float nx, ny, nz;
};


#define UNIT_BOX_VERTEX_TOTAL 24
#define UNIT_BOX_INDEX_TOTAL  36
#define UNIT_BOX_STRIDE       sizeof(stUnitBoxVertex)


DECLARE_ENGINE_NAMESPACE


inline bool CreateCubeMesh(float size, ModelData *model, std::vector<Plane> *planes)
{
   if(model == NULL)
      return false;

   stUnitBoxVertex UnitBoxData[UNIT_BOX_VERTEX_TOTAL] =
   {
      -1 * size, -1 * size, -1 * size,  1, 0,  0, 0, 1,
       1 * size, -1 * size, -1 * size,  0, 0,  0, 0, 1,
       1 * size,  1 * size, -1 * size,  0, 1,  0, 0, 1,
      -1 * size,  1 * size, -1 * size,  1, 1,  0, 0, 1,

       1 * size, -1 * size, 1 * size,  1, 0,  0, 0, -1,
      -1 * size, -1 * size, 1 * size,  0, 0,  0, 0, -1,
      -1 * size,  1 * size, 1 * size,  0, 1,  0, 0, -1,
       1 * size,  1 * size, 1 * size,  1, 1,  0, 0, -1,

      -1 * size, -1 * size, -1 * size,  0, 0,  1, 0, 0,
      -1 * size,  1 * size, -1 * size,  0, 1,  1, 0, 0,
      -1 * size,  1 * size,  1 * size,  1, 1,  1, 0, 0,
      -1 * size, -1 * size,  1 * size,  1, 0,  1, 0, 0,
       
      1 * size, -1 * size,  1 * size,  0, 0,  -1, 0, 0,
      1 * size,  1 * size,  1 * size,  0, 1,  -1, 0, 0,
      1 * size,  1 * size, -1 * size,  1, 1,  -1, 0, 0,
      1 * size, -1 * size, -1 * size,  1, 0,  -1, 0, 0,
       
      -1 * size, 1 * size,  1 * size,  0, 0,  0, -1, 0,
      -1 * size, 1 * size, -1 * size,  0, 1,  0, -1, 0,
       1 * size, 1 * size, -1 * size,  1, 1,  0, -1, 0,
       1 * size, 1 * size,  1 * size,  1, 0,  0, -1, 0,
       
       1 * size, -1 * size,  1 * size,  0, 0,  0, 1, 0,
       1 * size, -1 * size, -1 * size,  0, 1,  0, 1, 0,
      -1 * size, -1 * size, -1 * size,  1, 1,  0, 1, 0,
      -1 * size, -1 * size,  1 * size,  1, 0,  0, 1, 0
   };


   if(planes != NULL)
      {
         Plane plane;

         // Back.
         planes->push_back(Plane(0, 0, 1, size));

         // Front.
         planes->push_back(Plane(0, 0, -1, size));

         // Left
         planes->push_back(Plane(1, 0, 0, size));

         // Right.
         planes->push_back(Plane(-1, 0, 0, size));

         // Top.
         planes->push_back(Plane(0, -1, 0, size));

         // Bottom.
         planes->push_back(Plane(0, 1, 0, size));
      }

   unsigned int UnitBoxIndices[UNIT_BOX_INDEX_TOTAL] =
   {
      0, 1, 2, 2, 3, 0,
      4, 5, 6, 6, 7, 4,
      8, 9, 10, 10, 11, 8,
      12, 13, 14, 14, 15, 12,
      16, 17, 18, 18, 19, 16,
      20, 21, 22, 22, 23, 20
   };


   model->Clear();

   model->AddDescriptorElement(BB_ELEMENT_TYPE_VERTEX_3F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_TEX1_2F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_NORMAL_3F);

   if(model->SetVertices(BB_PRIMITIVE_TRI_LIST, UNIT_BOX_VERTEX_TOTAL,
      UNIT_BOX_STRIDE, (char*)UnitBoxData) == false)
      {
         return false;
      }

   if(model->SetIndices(UNIT_BOX_INDEX_TOTAL, UnitBoxIndices) == false)
      {
         return false;
      }

   return true;
}


inline bool CreateCubeMesh(float size, bbe::ModelData *model)
{
   return CreateCubeMesh(size, model, NULL);
}


inline bool CreateUnitCubeMesh(bbe::ModelData *model)
{
   return CreateCubeMesh(1, model);
}


END_ENGINE_NAMESPACE


#endif