/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#include<TGA.h>
#include<string.h>


unsigned char* LoadTGAFromMemory(char *data, int length, int &width, int &height, int &components)
{
   unsigned char tempColor;
   unsigned char unCompressHeader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   char *ptr = data;
   long tgaSize = 0;
   unsigned char *image = NULL;

   // We only want to read uncompressed tga's at this time.
   if(memcmp(unCompressHeader, ptr, sizeof(unCompressHeader)) != 0)
	   {
		   return NULL;
	   }

   ptr += sizeof(unCompressHeader);

   // Calculate image stats.
   width = ptr[0] + ptr[1] * 256;
   height = ptr[2] + ptr[3] * 256;
   components = ptr[4] / 8;
   tgaSize = width * height * components;

   ptr += 6;

   // We only care for RGB and RGBA images.
   if(components != 3 && components != 4)
      return NULL;

   // Allocate data for the image and load it.
   image = new unsigned char[sizeof(unsigned char) * tgaSize];
   memcpy(image, ptr, sizeof(char) * tgaSize);

   // Convert from BGR(A) to RGB(A) format.
   for(long index = 0; index < tgaSize; index += components)
	   {
		   tempColor = image[index];
		   image[index] = image[index + 2];
		   image[index + 2] = tempColor;
	   }

   return image;
}


unsigned char* LoadTGA(char* file, int &width, int &height, int &components)
{
   FILE *pfile;
   int length = 0;
   unsigned char *image = NULL;

   if(!file) return NULL;
   pfile = fopen(file, "rb");
   if(!pfile) return NULL;

   fseek(pfile, 0, SEEK_END);
   length = ftell(pfile);
   fseek(pfile, 0, SEEK_SET);

   // Read in all data from the file.
   char *data = new char[(length + 1) * sizeof(char)];
   if(!data) return NULL;

   fread(data, 1, length, pfile);

   data[length] = '\0';

   fclose(pfile);

   image = LoadTGAFromMemory(data, length, width, height, components);

   delete[] data;

   return image;
}


bool WriteTGA(char *file, int width, int height, int comp,
              unsigned char *outImage)
{
   FILE *pFile = 0;
   unsigned char tgaHeader[12] = {0, 0, 2, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0};
   unsigned char header[6];
   unsigned char bits = 0;
   unsigned char tempColors = 0;

   // Open file for output.
   pFile = fopen(file, "wb");

   // Check if the file opened or not.
   if(pFile == NULL)
      return false;

   // Set the color mode, and the bit depth.
   bits = comp * 8;

   // Save the width and height.
 	header[0] = width % 256;
 	header[1] = width / 256;
 	header[2] = height % 256;
	header[3] = height / 256;
	header[4] = bits;
	header[5] = 0;

   // Write the headers to the top of the file.
   fwrite(tgaHeader, sizeof(tgaHeader), 1, pFile);
   fwrite(header, sizeof(header), 1, pFile);

   // Now switch image from RGB to BGR.
   for(int i = 0; i < width * height * comp; i += comp)
      {
         tempColors = outImage[i];
         outImage[i] = outImage[i + 2];
         outImage[i + 2] = tempColors;
      }

   // Finally write the image.
   fwrite(outImage, width * height * comp, 1, pFile);

   // close the file.
   fclose(pFile);
   return true;
}