


#ifndef _BB_HEIGHT_MAP_H_
#define _BB_HEIGHT_MAP_H_

#include<ModelData.h>
#include<TGA.h>


struct stHeightMapVertex
{
   float x, y, z;
   float tu1, tv1;
};


#define HEIGHT_MAP_STRIDE  sizeof(stHeightMapVertex)


DECLARE_ENGINE_NAMESPACE


bool CreateHeightMapFromImageMesh(char *imageFile, float scale, bbe::CModelData *model)
{
   unsigned char *image = NULL, *heightMap = NULL;
   int halfWidth = 0, halfHeight = 0, width = 0, height = 0, components = 0;
   int size = 0, index = 0, totalVertices = 0, totalFaces = 0;
   stHeightMapVertex *data = NULL;

   if(model == NULL || imageFile == NULL)
      return false;

   image = LoadTGA(imageFile, width, height, components);

   if(image == NULL || (width != height))
      return false;

   size = width * height;
   totalVertices = (width - 1) * (height - 1) * 6;
   totalFaces = (width - 1) * (height - 1) * 2;

   halfWidth = (int)(width * 0.5f);

   heightMap = new unsigned char[size];
   data = new stHeightMapVertex[totalVertices];

   for(int i = 0; i < width * height * components; i += components)
      {
         heightMap[index++] = image[i];

         if(image[i] > halfHeight)
            halfHeight = image[i];
      }

   halfHeight /= 2;

   delete[] image;
   index = 0;

   for(int z = 0; z < height - 1; z++)
	   {
			for(int x = 0; x < width - 1; x++)
			   {
			      float left = (float)x / width;
			      float right = ((float)x + 1) / width;
				   float bottom = (float)z / height;
				   float top = (float)(z + 1) / height;

               // V1.
               data[index].tu1 = left;
               data[index].tv1 = bottom;
               data[index].x = (float)(x - halfWidth);
               data[index].y = (float)(heightMap[x + width * z] * scale) - halfHeight;
               data[index].z = (float)(z - halfWidth);
			      index++;

               // V2.
               data[index].tu1 = left;
               data[index].tv1 = top;
               data[index].x = (float)(x - halfWidth);
               data[index].y = (float)(heightMap[x + width * (z + 1)] * scale) - halfHeight;
               data[index].z = (float)((z + 1) - halfWidth);
			      index++;

               // V3.
               data[index].tu1 = right;
               data[index].tv1 = bottom;
               data[index].x = (float)((x + 1) - halfWidth);
               data[index].y = (float)(heightMap[(x + 1) + width * z] * scale) - halfHeight;
               data[index].z = (float)(z - halfWidth);
			      index++;

               // V4.
               data[index].tu1 = right;
               data[index].tv1 = bottom;
               data[index].x = (float)((x + 1) - halfWidth);
               data[index].y = (float)(heightMap[(x + 1) + width * z] * scale) - halfHeight;
               data[index].z = (float)(z - halfWidth);
			      index++;

               // V5.
               data[index].tu1 = right;
               data[index].tv1 = top;
               data[index].x = (float)((x + 1) - halfWidth);
               data[index].y = (float)(heightMap[(x + 1) + width * (z + 1)] * scale) - halfHeight;
               data[index].z = (float)((z + 1) - halfWidth);
			      index++;

               // V6.
               data[index].tu1 = left;
               data[index].tv1 = top;
               data[index].x = (float)(x - halfWidth);
               data[index].y = (float)(heightMap[x + width * (z + 1)] * scale) - halfHeight;
               data[index].z = (float)((z + 1) - halfWidth);
			      index++;
			   }
	   }

   delete[] heightMap;


   model->Clear();

   model->AddDescriptorElement(BB_ELEMENT_TYPE_VERTEX_3F);
   model->AddDescriptorElement(BB_ELEMENT_TYPE_TEX1_2F);

   if(model->SetVertices(BB_PRIMITIVE_TRI_LIST, totalVertices,
      HEIGHT_MAP_STRIDE, (char*)data) == false)
      {
         return false;
      }

   /*if(model->SetIndices(totalIndices, heighMapIndices) == false)
      {
         return false;
      }*/

   return true;
}


END_ENGINE_NAMESPACE


#endif