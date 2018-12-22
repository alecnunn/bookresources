// SphereMap using libnoise

#include <iostream>
#include <time.h>
#include "../noise/noise.h"
#include "noiseutils.h"
#include <windows.h>

#pragma comment(lib,"libnoise.lib")
#pragma comment(lib,"winmm.lib")

using namespace noise;

int main (int argc, char** argv)
{
   srand(time(NULL));
   int planet_type = 2; //rand() % 6;

   module::Perlin perlin;
   perlin.SetSeed(timeGetTime()); //GetTickCount());
   perlin.SetFrequency(1.0);
   perlin.SetOctaveCount(6);
   perlin.SetPersistence (0.5);

   switch(planet_type){
      case 2: //frozen
         perlin.SetOctaveCount(5);
         perlin.SetPersistence (0.4);
         break;

      case 4: //gas giant
         perlin.SetOctaveCount(3);
         perlin.SetPersistence (0.3);
         break;

      case 5: //inferno
         perlin.SetOctaveCount(6);
         perlin.SetFrequency(1.4);
         perlin.SetPersistence (0.5);
         break;
   }

   utils::NoiseMap heightMap;
   //utils::NoiseMapBuilderPlane heightMapBuilder;
   utils::NoiseMapBuilderSphere heightMapBuilder;

   heightMapBuilder.SetSourceModule (perlin);
   heightMapBuilder.SetDestNoiseMap (heightMap);
   heightMapBuilder.SetDestSize (512, 512);
   heightMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
   heightMapBuilder.Build ();

   utils::RendererImage renderer;
   utils::Image image;
   renderer.SetSourceNoiseMap (heightMap);
   renderer.SetDestImage (image);
   renderer.ClearGradient ();

   switch(planet_type) {
      case 1: //oceanic
         renderer.AddGradientPoint (-1.0000, utils::Color (0, 0, 120, 255)); // deeps
         renderer.AddGradientPoint (-0.2500, utils::Color (0, 0, 255, 255)); // shallow
         renderer.AddGradientPoint ( 0.0000, utils::Color (0, 120, 250, 255)); // shore
         renderer.AddGradientPoint ( 0.0625, utils::Color (180, 180, 60, 255)); // sand
         renderer.AddGradientPoint ( 0.1250, utils::Color (50, 160, 0, 255)); // grass
         renderer.AddGradientPoint ( 0.3750, utils::Color (180, 180, 0, 255)); // dirt
         renderer.AddGradientPoint ( 0.7500, utils::Color (150, 150, 150, 255)); // rock
         renderer.AddGradientPoint ( 1.0000, utils::Color (255, 255, 255, 255)); // snow
         break;
      case 2: //frozen
         renderer.AddGradientPoint (-1.0000, utils::Color (130, 130, 150, 255)); // deeps
         renderer.AddGradientPoint (-0.2500, utils::Color (140, 140, 150, 255)); // shallow
         renderer.AddGradientPoint ( 0.0000, utils::Color (150, 150, 150, 255)); // shore
         renderer.AddGradientPoint ( 0.0625, utils::Color (160, 160, 160, 255)); // sand
         renderer.AddGradientPoint ( 0.1250, utils::Color (170, 170, 170, 255)); // grass
         renderer.AddGradientPoint ( 0.3750, utils::Color (200, 200, 200, 255)); // dirt
         renderer.AddGradientPoint ( 0.7500, utils::Color (230, 230, 230, 255)); // rock
         renderer.AddGradientPoint ( 1.0000, utils::Color (255, 255, 255, 255)); // snow
         break;
      case 3: //rocky
         renderer.AddGradientPoint (-1.0000, utils::Color (120, 100, 100, 255)); // deeps
         renderer.AddGradientPoint (-0.2500, utils::Color (120, 120, 120, 255)); // shallow
         renderer.AddGradientPoint ( 0.0000, utils::Color (160, 150, 160, 255)); // shore
         renderer.AddGradientPoint ( 0.0625, utils::Color (120, 120, 100, 255)); // sand
         renderer.AddGradientPoint ( 0.1250, utils::Color (120, 120, 120, 255)); // grass
         renderer.AddGradientPoint ( 0.3750, utils::Color (150, 160, 170, 255)); // dirt
         renderer.AddGradientPoint ( 0.7500, utils::Color (150, 150, 150, 255)); // rock
         renderer.AddGradientPoint ( 1.0000, utils::Color (160, 150, 160, 255)); // snow
         break;
      case 4: //gas giant
         renderer.AddGradientPoint (-1.0000, utils::Color (130, 100, 130, 255)); 
         renderer.AddGradientPoint (-0.5000, utils::Color (160, 140, 160, 255)); 
         renderer.AddGradientPoint ( 0.0000, utils::Color (100, 180, 200, 255)); 
         renderer.AddGradientPoint ( 0.5000, utils::Color (150, 150, 150, 255)); 
         renderer.AddGradientPoint ( 1.0000, utils::Color (160, 180, 120, 255)); 
         break;
      case 5: //inferno
         renderer.AddGradientPoint (-1.0000, utils::Color (200, 30, 30, 255)); 
         renderer.AddGradientPoint (-0.6000, utils::Color (235, 40, 40, 255)); 
         renderer.AddGradientPoint (-0.3000, utils::Color (255, 50, 50, 255)); 
         renderer.AddGradientPoint ( 0.0000, utils::Color (80, 70, 70, 255));
         renderer.AddGradientPoint ( 0.1250, utils::Color (100, 100, 100, 255)); 
         renderer.AddGradientPoint ( 0.5000, utils::Color (150, 120, 100, 255)); 
         renderer.AddGradientPoint ( 1.0000, utils::Color (130, 140, 140, 255)); 
         break;

   }

   renderer.EnableLight();
   renderer.SetLightContrast(3.0);
   renderer.SetLightBrightness(2.0);
   renderer.SetLightColor(utils::Color(255,255,255,255));
   renderer.Render();

   utils::WriterBMP writer;
   writer.SetSourceImage(image);
   writer.SetDestFilename("texture.bmp");
   writer.WriteDestFile();

   return 0;
}
