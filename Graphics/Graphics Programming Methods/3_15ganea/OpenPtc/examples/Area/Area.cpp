//
// Area example for OpenPTC 1.0 C++ Implementation
// Programmed by Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include "ptc.h"
#include <stdlib.h>


inline int random(int max)
{
    // random number
    return rand() % max;
}


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    try
    {
        // create console
        Console console;

        // create format
        Format format(32,0x00FF0000,0x0000FF00,0x000000FF);
 
        // create console
        console.open("Area example",format);

        // create surface half the size of the console
        Surface surface(console.width()/2,console.height()/2,format);
        
        // loop until a key is pressed
        while (!console.key())
        {
            // lock surface pixels
            int32 *pixels = (int32*) surface.lock();

            // get surface dimensions
            const int width  = surface.width();
            const int height = surface.height();

            // draw random pixels
            for (int i=0; i<100; i++)
            {
                // get random position
                const int x = random(width);
                const int y = random(height);

                // get random color
                const int r = random(256);
                const int g = random(256);
                const int b = random(256);

                // draw color [r,g,b] at position [x,y]
                pixels[x+y*width] = (r<<16) + (g<<8) + b;
            }
        
            // unlock surface
            surface.unlock();

            // setup destination area
            const int x = console.width() / 4;
            const int y = console.height() / 4;
            Area area = Area(x,y,x+surface.width(),y+surface.height());

            // copy surface to console destination area
            surface.copy(console,surface.area(),area);
            
            // update console area
            console.update(area);
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
