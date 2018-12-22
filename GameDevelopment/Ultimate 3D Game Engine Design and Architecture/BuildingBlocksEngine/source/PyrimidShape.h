/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_PYRIMID_SHAPE_H_
#define _BB_PYRIMID_SHAPE_H_

#include<ModelData.h>


struct stUnitPyrimidVertex
{
   float x, y, z;
   float tu1, tv1;
   float nx, ny, nz;
};


#define UNIT_PYRIMID_VERTEX_TOTAL   15
#define UNIT_PYRIMID_INDEX_TOTAL    18
#define UNIT_PYRIMID_STRIDE         sizeof(stUnitPyrimidVertex)


DECLARE_ENGINE_NAMESPACE


bool CreatePyrimidMesh(float base, float height, bbe::ModelData *model)
{
   if(model == NULL)
      return false;

   stUnitPyrimidVertex UnitPyrimidData[UNIT_PYRIMID_VERTEX_TOTAL] =
   {
       1 * base, -1 * height, 1 * base,     1, 0,  0, 0, 1,
      -1 * base, -1 * height, 1 * base,     0, 0,  0, 0, 1,
              0,  1 * height,        0,  0.5f, 1,  0, 1, 0,

       1 * base, -1 * height, -1 * base,    1, 0,  0, 0, -1,
      -1 * base, -1 * height, -1 * base,    0, 0,  0, 0, -1,

       1 * base, -1 * height,  1 * base,    1, 0,  1, 0, 0,
       1 * base, -1 * height, -1 * base,    0, 0,  1, 0, 0,

      -1 * base, -1 * height,  1 * base,    1, 0,  -1, 0, 0,
      -1 * base, -1 * height, -1 * base,    0, 0,  -1, 0, 0,

       1 * base, -1 * height,  1 * base,    1, 0,  0, -1, 0,
      -1 * base, -1 * height,  1 * base,    0, 0,  0, -1, 0,
       1 * base, -1 * height, -1 * base,    1, 1,  0, -1, 0,
       
       1 * base, -1 * height, -1 * base,    1, 1,  0, -1, 0,
      -1 * base, -1 * height, -1 * base,    0, 1,  0, -1, 0,
      -1 * base, -1 * height,  1 * base,    0, 0,  0, -1, 0
   };

   unsigned int UnitPyrimidIndices[UNIT_PYRIMID_INDEX_TOTAL] =
   {
      0, 1, 2,
      3, 4, 2,
      5, 6, 2,
      7, 8, 2,
      9, 10, 11,
      12, 13, 14
   };


   model->Clear();

   model->AddDescriptorElement(BB_ELEMENT_TYPE_VERTEX_3F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_TEX1_2F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_NORMAL_3F);

   if(model->SetVertices(BB_PRIMITIVE_TRI_LIST, UNIT_PYRIMID_VERTEX_TOTAL,
      UNIT_PYRIMID_STRIDE, (char*)UnitPyrimidData) == false)
      {
         return false;
      }

   if(model->SetIndices(UNIT_PYRIMID_INDEX_TOTAL, UnitPyrimidIndices) == false)
      {
         return false;
      }

   return true;
}


bool CreateUnitPyrimidMesh(bbe::ModelData *model)
{
   return CreatePyrimidMesh(1, 1, model);
}


END_ENGINE_NAMESPACE


#endif