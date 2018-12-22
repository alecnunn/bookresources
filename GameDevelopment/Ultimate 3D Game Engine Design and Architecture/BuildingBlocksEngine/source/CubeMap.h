/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_CUBE_MAP_H_
#define _BB_CUBE_MAP_H_

#include<stdio.h>
#include<Endian.h>


unsigned char* LoadCubeMapFromMemory(char *data, int &width, int &height, int &components)
{
   char *ptr = &data[0];
   long size = 0;
   bbe::Endian endian;

   memcpy(&width, ptr, sizeof(int));
   endian.ResolveEndian((char*)&width,  sizeof(int), BB_ENDIAN_LITTLE);
   ptr += sizeof(int);

   memcpy(&height, ptr, sizeof(int));
   endian.ResolveEndian((char*)&height,  sizeof(int), BB_ENDIAN_LITTLE);
   ptr += sizeof(int);

   memcpy(&components, ptr, sizeof(int));
   endian.ResolveEndian((char*)&components,  sizeof(int), BB_ENDIAN_LITTLE);
   components /= 8;
   ptr += sizeof(int);

   size = (width * height * components) * 6;

   // We only care for RGB and RGBA images.
   if(components != 3 && components != 4)
      return NULL;

   if(width == 0 || height == 0)
      return NULL;

   // Allocate data for the image and load it.
   unsigned char *image = new unsigned char[sizeof(unsigned char) * size];
   memcpy(image, ptr, sizeof(char) * size);

   return image;
}


unsigned char* LoadCubeMap(char *file, int &width, int &height, int &components)
{
   if(!file)
      return NULL;

   FILE *pfile;
   pfile = fopen(file, "rb");

   if(!pfile)
      return NULL;

   int length = 0;

   fseek(pfile, 0, SEEK_END);
   length = ftell(pfile);
   fseek(pfile, 0, SEEK_SET);

   // Read in all data from the file.
   char *data = new char[(length + 1) * sizeof(char)];
   if(!data) return NULL;

   fread(data, 1, sizeof(char) * length, pfile);

   data[length] = '\0';

   fclose(pfile);

   unsigned char *image = NULL;

   image = LoadCubeMapFromMemory(data, width, height, components);

   delete[] data;

   return image;
}

#endif