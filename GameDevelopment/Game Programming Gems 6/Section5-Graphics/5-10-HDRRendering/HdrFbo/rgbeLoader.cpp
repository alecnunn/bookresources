/*
   Allen Sherrod
   HDR Rendering with Frame Buffer Objects
   September 1, 2005
*/


#include<stdio.h>
#include<math.h>
#include<ctype.h>
#include<string.h>
#include"rgbeLoader.h"


bool ReadRGBEHeader(FILE *fp, int *width, int *height, rgbeHeader *header);
bool ReadRGBEUncompressed(FILE *fp, float *img, int size);
bool ReadRGBERLE(FILE *fp, float *image, int width, int height, bool decode);


void EncodeRGBE(float *src, unsigned char *dst)
{
   float r = 0, g = 0, b = 0, maxComp = 0, d = 0;
   int expMin = 0, expMax = 256;
   
   r = *(src);
   g = *(src + 1);
   b = *(src + 2);
   
   maxComp = r;
   if(g > maxComp) maxComp = g;
   if(b > maxComp) maxComp = b;
   
   while(expMax - expMin > 1)
      {
         int k = (expMax + expMin) * 0.5;
         if(maxComp > pow(2, k)) expMin = k;
         else expMax = k;
      }
   
   d = 1 / (float)pow(2, expMax);
   r *= d;
   g *= d;
   b *= d;
   
   if(r < 0) r = 0;
   if(r > 255) r = 255;
   if(g < 0) g = 0;
   if(g > 255) g = 255;
   if(b < 0) b = 0;
   if(b > 255) b = 255;
   
   dst[0] = (unsigned char)(r * 255);
   dst[1] = (unsigned char)(g * 255);
   dst[2] = (unsigned char)(b * 255);
   dst[3] = expMax + 128;
}


void DecodeRGBE(unsigned char *src, float *dst)
{
   if(src[3])
      {
         float exp = (float)ldexp(1.0, src[3] - (128 + 8));
         dst[0] = src[0] * exp;
         dst[1] = src[1] * exp;
         dst[2] = src[2] * exp;
      }
   else
      {
         dst[0] = 0; dst[1] = 0; dst[2] = 0;
      }
}


float *ReadRGBEImage(char *fileName, int *w, int *h)
{
   rgbeHeader header;
   int width = 0, height = 0;
   FILE *fp = NULL;

   if(!fileName) return NULL;
   fp = fopen(fileName, "rb");
   if(!fp) return NULL;

   ReadRGBEHeader(fp, &width, &height, &header);

   float *hdrImage = new float[width * height * 3];
   ReadRGBERLE(fp, hdrImage, width, height, true);

   fclose(fp);

   if(w) *w = width;
   if(h) *h = height;

   return hdrImage;
}


bool ReadRGBEHeader(FILE *fp, int *width, int *height, rgbeHeader *header)
{
   char buffer[128];
   float temp;

   if(header)
      {
         header->valid = 0;
         header->programType[0] = 0;
         header->gamma = 1.0f;
         header->exposure = 1.0f;
      }

   if(fgets(buffer, sizeof(buffer) / sizeof(buffer[0]), fp) == NULL)
      return false;

   if(header)
      {
         header->valid |= RGBE_VALID_PROGRAMTYPE;

         for(int i = 0; i < sizeof(header->programType) - 1; i++)
            {
               if(isspace(buffer[i + 2]) || (buffer[i + 2] == 0)) break;
               header->programType[i] = buffer[i + 2];
            }

         header->programType[i] = 0;

         if(fgets(buffer, sizeof(buffer) / sizeof(buffer[0]), fp) == NULL)
            return false;
      }

   while(1)
      {
         if((buffer[0] == 0)) return false;
         else if(strcmp(buffer, "\n") == 0) break;
         else if(strcmp(buffer, "FORMAT=32-bit_rle_rgbe\n") == 0) { }
         else if(header && (sscanf(buffer, "GAMMA=%g", &temp) == 1))
            {
               header->valid |= RGBE_VALID_GAMMA;
               header->gamma = temp;
            }
         else if(header && (sscanf(buffer, "EXPOSURE=%g", &temp) == 1))
            {
               header->valid |= RGBE_VALID_EXPOSURE;
               header->exposure = temp;
            }

         if(fgets(buffer, sizeof(buffer) / sizeof(buffer[0]), fp) == 0)
            return false;
      }

   if(fgets(buffer, sizeof(buffer) / sizeof(buffer[0]), fp) == 0)
      return false;
   if(sscanf(buffer, "-Y %d +X %d", height, width) < 2)
      return false;

   return true;
}


bool ReadRGBEUncompressed(FILE *fp, float *img, int size)
{
   unsigned char rgbe[4];

   while(size > 0)
      {
         if(fread(rgbe, sizeof(rgbe), 1, fp) < 1) return false;
         DecodeRGBE(&rgbe[0], img);

         img += 3;
         size--;
      }

   return true;
}


bool ReadRGBERLE(FILE *fp, float *image, int width, int height, bool decode)
{
   unsigned char rgbe[4], *buffer = NULL, *ptr, *ptr_end;
   int count;
   unsigned char buf[2];

   if(width < 8 || width > 0x7fff)
      return ReadRGBEUncompressed(fp, image, width * height);

   while(height > 0)
      {
         if(fread(rgbe, sizeof(rgbe), 1, fp) < 1)
            {
               if(buffer) delete[] buffer;
               return false;
            }

         if(rgbe[0] != 2 || rgbe[1] != 2 || rgbe[2] & 0x80)
            {
               DecodeRGBE(&rgbe[0], image);
               image += 3;

               if(buffer) delete[] buffer;
               return ReadRGBEUncompressed(fp, image, width * height - 1);
            }

         if(((int)rgbe[2] << 8 | rgbe[3]) != width)
            {
               if(buffer) delete[] buffer;
               return false;
            }

         if(!buffer)
            buffer = new unsigned char[sizeof(unsigned char) * width * 4];

         if(!buffer) return false;
         ptr = &buffer[0];

         for(int i = 0; i < 4; i++)
            {
               ptr_end = &buffer[(i + 1) * width];

               while(ptr < ptr_end)
                  {
                     if(fread(buf, sizeof(buf[0]) * 2, 1, fp) < 1)
                        {
                           delete[] buffer;
                           return false;
                        }

                     if(buf[0] > 128)
                        {
                           count = buf[0] - 128;

                           if(count == 0 || count > ptr_end - ptr)
                              {
                                 delete[] buffer;
                                 return false;
                              }

                           while(count > 0)
                              {
                                 *ptr++ = buf[1];
                                 count--;
                              }
                        }
                     else
                        {
                           count = buf[0];

                           if(count == 0 || count > ptr_end - ptr)
                              {
                                 delete[] buffer;
                                 return false;
                              }

                           *ptr++ = buf[1];
                           count--;

                           if(count > 0)
                              {
                                 if(fread(ptr, sizeof(*ptr) * count, 1, fp) < 1)
                                    {
                                       delete[] buffer;
                                       return false;
                                    }

                                 ptr += count;
                              }
                        }
                  }
            }

         for(i = 0; i < width; i++)
            {
               rgbe[0] = buffer[i];
               rgbe[1] = buffer[i + width];
               rgbe[2] = buffer[i + 2 * width];
               rgbe[3] = buffer[i + 3 * width];

               if(decode) DecodeRGBE(rgbe, image);
               else
                  {
                     image[0] = rgbe[0];
                     image[1] = rgbe[1];
                     image[2] = rgbe[2];
                     image[3] = rgbe[3];
                  }
               image += 3;
            }

         height--;
      }

   delete[] buffer;
   return true;
}