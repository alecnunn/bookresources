//
// Fire demo for OpenPTC 1.0 C++ API
// Copyright (c) Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU GPL
//

// include files
#include "ptc.h"
#include <stdlib.h>


inline int random(int max)
{
    // random number
    return rand() % max;
}


int32 pack(int32 r,int32 g,int32 b)
{
    // pack color integer
    return (r<<16) | (g<<8) | b; 
}


void generate(Palette &palette)
{
    // lock palette data
    int32 *data = palette.lock();
    
    // black to red
    int i=0;
    int c=0;
    while (i<64)
    {
        data[i] = pack(c,0,0);
        c+=4;
        i++;
    }

    // red to yellow
    c=0;
    while (i<128)
    {
        data[i] = pack(255,c,0);
        c+=4;
        i++;
    }

    // yellow to white
    c=0;
    while (i<128)
    {
        data[i] = pack(255,255,c);
        c+=4;
        i++;
    }

    // white
    while (i<256) 
    {
        data[i] = pack(255,255,255);
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

        // open console
        console.open("Fire demo",320,200,format);

        // create surface
        Surface surface(320,208,format);

        // create palette
        Palette palette;

        // generate palette
        generate(palette);

        // set console palette
        console.palette(palette);

        // set surface palette
        surface.palette(palette);

        // flame data
        int state = 0;
        float intensity = 0;

        // main loop
        while (true)
        {
            // lower flame on keypress
            if (console.key()) state = 2;

            // state machine
            switch (state)
            {
                case 0:
                {
                    // raise flame
                    intensity += 0.007f;
                
                    // maximum flame height
                    if (intensity>0.8f) state = 1;
                }
                break;

                case 1:
                {
                    // constant flame
                }
                break;

                case 2:
                {
                    // lower flame
                    intensity -= 0.005f;

                    // exit program when flame is out
                    if (intensity<0.01f)
                    {
                        // close console
                        console.close();

                        // exit
                        return 0;
                    }
                }
                break;
            }

            // lock surface pixels
            char8 *pixels = (char8*) surface.lock();

            // get surface dimensions
            int width  = surface.width();
            int height = surface.height();

            // flame vertical loop
            for (int y=1; y<height-4; y+=2)
            {
                // current pixel pointer
                char8 *pixel = pixels + y*width;

                // flame horizontal loop
                for (int x=0; x<width; x++)
                {
                    // sum top pixels
                    char8 *p = pixel + (width<<1);
                    int32 top = *p;
                    top += *(p-1);
                    top += *(p+1);

                    // bottom pixel
                    int32 bottom = *(pixel + (width<<2));

                    // combine pixels
                    int32 c1 = (top + bottom) >> 2;
                    if (c1>1) c1--;

                    // interpolate
                    int32 c2 = (c1 + bottom) >> 1;

                    // store pixels
                    *pixel = (char8)c1;
                    *(pixel+width) = (char8)c2;

                    // next pixel
                    pixel++;
                }
            }

            // setup flame generator pointer
            char8 *generator = pixels + width*(height-4);

            // update flame generator bar
            for (int x=0; x<width; x+=4)
            {
                // random block color taking intensity into account
                int color = random((int)(255.0f*intensity));

                // write 4x4 color blocks
                *(generator+0)         = color;
                *(generator+1)         = color;
                *(generator+2)         = color;
                *(generator+3)         = color;
                *(generator+width+0)   = color;
                *(generator+width+1)   = color;
                *(generator+width+2)   = color;
                *(generator+width+3)   = color;
                *(generator+width*2+0) = color;
                *(generator+width*2+1) = color;
                *(generator+width*2+2) = color;
                *(generator+width*2+3) = color;
                *(generator+width*3+0) = color;
                *(generator+width*3+1) = color;
                *(generator+width*3+2) = color;
                *(generator+width*3+3) = color;

                // next block
                generator += 4;
            }

            // unlock surface
            surface.unlock();

            // setup copy area
            Area area(0,0,320,200);

            // copy surface to console
            surface.copy(console,area,area);

            // update console
            console.update();
        }
    }
    catch (Error &error)
    {
        // report error
        error.report();
    }

    // exit
    return 0;
}
