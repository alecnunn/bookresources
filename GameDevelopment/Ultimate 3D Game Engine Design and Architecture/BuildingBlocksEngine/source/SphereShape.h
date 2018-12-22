/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_SPHERE_SHAPE_H_
#define _BB_SPHERE_SHAPE_H_

#include<ModelData.h>
#include<math.h>


struct stUnitSphereVertex
{
   float x, y, z;
   float tu1, tv1;
   float nx, ny, nz;
};


#define UNIT_SPHERE_STRIDE       sizeof(stUnitSphereVertex)


DECLARE_ENGINE_NAMESPACE


bool CreateSphereMesh(float radius, int hRes, int vRes, bbe::ModelData *model)
{
   if(model == NULL)
      return false;

   model->Clear();

   int numFaces = hRes * (vRes * 2) * 2;
   int numIndices = numFaces * 3;
   int numVerts = (hRes + 1) * (vRes + 1);
   float fPI = 3.141592654f;

   stUnitSphereVertex *verts = new stUnitSphereVertex[numVerts];
   stUnitSphereVertex *vertPtr = verts;

   if(verts == NULL)
      return false;

   unsigned int *indices = new unsigned int[numFaces * 3];
   unsigned int *indexPtr = indices;

   if(indices == NULL)
      return false;

   for(int i = 0; i <= hRes; i++)
      {
         for(int j = 0; j <= vRes; j++)
            {
               float ii = i / (float)hRes;
               float jj = j / (float)vRes;
               float theta = ii * 2 * fPI;
               float phi = (jj - 0.5f) * fPI;
               float phi2 = phi + fPI * 0.5f;
               float nx = (float)(cos(theta) * cos(phi));
               float ny = (float)(sin(theta) * cos(phi));
               float nz = (float)(sin(phi));

               vertPtr->x = radius * nx;
               vertPtr->y = radius * ny;
               vertPtr->z = radius * nz;

               vertPtr->tu1 = ii;
               vertPtr->tv1 = jj;

               vertPtr->nx = -nx;
               vertPtr->ny = -ny;
               vertPtr->nz = -nz;

               vertPtr++;
            }
      }

   for(int i = 0; i < hRes; i++)
      {
         for(int j = 0; j < vRes; j++)
            {
               *indexPtr++ = ((i + 0) * (vRes + 1) + j + 0);
               *indexPtr++ = ((i + 1) * (vRes + 1) + j + 1);
               *indexPtr++ = ((i + 0) * (vRes + 1) + j + 1);

               *indexPtr++ = ((i + 0) * (vRes + 1) + j + 0);
               *indexPtr++ = ((i + 1) * (vRes + 1) + j + 0);
               *indexPtr++ = ((i + 1) * (vRes + 1) + j + 1);
            }
      }

   model->AddDescriptorElement(BB_ELEMENT_TYPE_VERTEX_3F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_TEX1_2F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_NORMAL_3F);

   if(model->SetVertices(BB_PRIMITIVE_TRI_LIST, numVerts,
      UNIT_SPHERE_STRIDE, (char*)verts) == false)
      {
         return false;
      }

   if(model->SetIndices(numIndices, indices) == false)
      {
         return false;
      }

   if(verts != NULL)
      {
         delete[] verts;
      }

   if(indices != NULL)
      {
         delete[] indices;
      }

   return true;
}


bool CreateUnitSphereMesh(bbe::ModelData *model)
{
   return CreateSphereMesh(1, 10, 10, model);
}


END_ENGINE_NAMESPACE


#endif