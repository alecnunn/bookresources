//
// Flower demo for OpenPTC 1.0 C++ API
// Copyright (c) Scott Buchanan (aka Goblin)
// This source code is licensed under the GNU GPL
//

// include files
#include "ptc.h"
#include <math.h>
#include <stdlib.h>


int32 pack(int32 r,int32 g,int32 b)
{
    // pack color integer
    return (r<<16) | (g<<8) | b; 
}


void generate_flower(Surface &flower)
{
    // lock surface
    char8 *data = (char8*) flower.lock();

    // surface width and height constants for cleaner code
    int fx = flower.width();
    int fy = flower.height();
    int fx2 = fx / 2;
    int fy2 = fy / 2;

    // useful 2*pi constant
    const float TWO_PI = 6.283185307f;

    // generate flower image
    for (int y=0; y < fy; y++)
      for (int x=0; x < fx; x++)
        data[x + y*fx] = (char8) ( 1.0 * cos(18*atan2((y-fy2),(x-fx2)))*255/TWO_PI +
                                   0.3 * sin(15*atan2((y-fy2),(x-fx2)))*255/TWO_PI +
                                   sqrt((y-fy2)*(y-fy2) + (x-fx2)*(x-fx2)) );

    // You might want to move the 1.0 and 0.3 and the 18 and the 15
    // to parameters passed to the generate function...
    // the 1.0 and the 0.3 define the 'height' of the flower, while the
    // 18 and 15 control the number of 'petals'
    flower.unlock();
}


void generate(Palette &palette)
{
    // lock palette data
    int32 *data = palette.lock();
    
    // black to yellow
    int i=0;
    int c=0;
    while (i<64)
    {
        data[i] = pack(c,c,0);
        c+=4;
        i++;
    }

    // yellow to red
    c=0;
    while (i<128)
    {
        data[i] = pack(255,255-c,0);
        c+=4;
        i++;
    }

    // red to white
    c=0;
    while (i<192)
    {
        data[i] = pack(255,c,c);
        c+=4;
        i++;
    }

    // white to black
    c=0;
    while (i<256) 
    {
        data[i] = pack(255-c,255-c,255-c);
        c+=4;
        i++;
    }

    // unlock palette
    palette.unlock();
}


int APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nCmdShow)
{
    try
    {
        // create format
        Format format(8);

        // create console
        Console console;

        // create flower surface
        Surface flower(640,400,format);

        // generate flower
        generate_flower(flower);

        // open console
        console.open("Flower demo",320,200,format);

        // create surface
        Surface surface(320,200,format);

        // create palette
        Palette palette;

        // generate palette
        generate(palette);

        // set console palette
        console.palette(palette);

        // set surface palette
        surface.palette(palette);

        // time data
        float time  = 0.0f;
        float delta = 0.04f;

        // main loop
        while (!console.key())
        {

            // lock surface pixels
            char8 *scr = (char8*) surface.lock();
            char8 *map = (char8*) flower.lock();

            // get surface dimensions
            int width  = surface.width();
            int height = surface.height();
            int mapWidth = flower.width();

            float xo = (float)(width/2) + 120 * (float)sin( time * 1.1f + 1.5f);
            float yo = (float)(height/2) + 90 * (float)cos( time * 0.8f + 1.1f);
            int offset1 = (int)xo + ((int)yo) * mapWidth;

            float xo2 = (float)(width/2) + 120 * (float)sin( time * 0.9f + 4.2f );
            float yo2 = (float)(height/2) + 90 * (float)cos( time * 0.7f + 6.9f );
            int offset2 = (int)xo2 + ((int)yo2) * mapWidth;

            float xo3 = (float)(width/2) + 120 * (float)sin( time * 0.9f + 3.1f );
            float yo3 = (float)(height/2) + 90 * (float)cos( time * 1.1f + 1.2f );
            int offset3 = (int)xo3 + ((int)yo3) * mapWidth;

            // vertical loop
            for (int y=0; y<height; y++)
            {
                // horizontal loop
                for (int x=0; x<width; x++)
                {
                        scr[x + y*width] = map[x + y*mapWidth + offset1] +
                                           map[x + y*mapWidth + offset2] +
                                           map[x + y*mapWidth + offset3];
                }
            }

            // unlock surface(s)
            surface.unlock();
            flower.unlock();

            // setup copy area
            Area area(0,0,320,200);

            // copy surface to console
            surface.copy(console,area,area);

            // update console
            console.update();

            // update time
            time += delta;
        }

        // close console
        console.close();
    }
    catch (Error &error)
    {
        // report error
        error.report();
    }

    // exit
    return 0;
}
