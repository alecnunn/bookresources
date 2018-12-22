/*
   Allen Sherrod
   HDR Rendering with Frame Buffer Objects
   September 1, 2005
*/


#ifndef _RGBE_LOADER_H_
#define _RGBE_LOADER_H_

#define RGBE_VALID_PROGRAMTYPE   0x01
#define RGBE_VALID_GAMMA         0x02
#define RGBE_VALID_EXPOSURE      0x04


typedef struct
{
   int valid;
   char programType[16];
   float gamma;
   float exposure;
} rgbeHeader;


float *ReadRGBEImage(char *fileName, int *w, int *h);
bool WriteRGBEImage(char *fileName, float *img, int w, int h, rgbeHeader &header);

#endif