/*
   Bump Map Generator
   Created by Allen Sherrod
   Ultimate Game Engine Design and Architecture
*/


#include<iostream>
#include<TGA.h>
#include<Vector3D.h>

using namespace std;


int main(int args, char *arg[])
{
   cout << "Bump Map Creator 1.0" << endl;
   cout << "Created by Allen Sherrod aka The Programming Ace" << endl;
   cout << "www.UltimateGameProgramming.com" << endl;
   cout << "Supported file formats: TGA" << endl << endl;

   if(args < 2)
      {
         cout << "Not enough arguments defined." << endl;
         return 0;
      }

   int scale = 1;
   scale = (arg[3] != NULL) ? atoi(arg[3]) : 1;
   scale = (!scale || scale == 0L) ? 1 : scale;

   if(arg[1] == NULL || arg[2] == NULL)
      {
         cout << "No source or destination file defined." << endl;
         return 0;
      }

   int srcWidth = 0, srcHeight = 0, srcComp = 0, component = 3;
   unsigned char *src = NULL, *ptr = NULL;

   src = LoadTGA(arg[1], srcWidth, srcHeight, srcComp);
   ptr = src;

   if(src == NULL)
      {
         cout << "Error loading image." << endl;
         return 0;
      }

   bbe::Vector3D e1, e2, normal;

   // Convert each pixel to a normal vector.
   for(int y = 0; y < srcHeight; y++)
      {
         for(int x = 0; x < srcWidth; x++)
            {
               int width  = (x == srcWidth - 1) ? 0  : srcWidth;
               int offset = (x == srcWidth - 1) ? 0  : component;
               int height = (y == srcHeight - 1) ? 0 : width *
                            offset;

               // Get triangle of pixels (Red since greyscale).
               float height1 = ((float)ptr[0] / 255) * scale;
               float height2 = ((float)ptr[0 + offset] / 255) * scale;
               float height3 = ((float)ptr[0 + height] / 255) * scale;

               bbe::Vector3D p1((float)x, (float)y, height1);
               bbe::Vector3D p2((float)x + 1, (float)y, height2);
               bbe::Vector3D p3((float)x, (float)y + 1, height3);
               
               // Normalize the triangle of pixels.
               e1 = p2 - p1;
               e2 = p3 - p1;
               e1.Normalize();
               e2.Normalize();

               normal = e1.CrossProduct(e2);
               normal.Normalize();
               
               // Save in 0 to 255 range.
               ptr[0] = (unsigned char)(127.0f * normal.x + 128.0f);
               ptr[1] = (unsigned char)(127.0f * normal.y + 128.0f);
               ptr[2] = (unsigned char)(127.0f * normal.z + 128.0f);
               
               // Move to the start of the next pixel.
               ptr += component; 
            }
      }


   // Save image.
   if(!WriteTGA(arg[2], srcWidth, srcHeight, component, src))
      {
         cout << "Error saving normal image." << endl;
         return 0;
      }

   delete[] src;

   cout << "Bump map creation successful." << endl;

   return 0;
}