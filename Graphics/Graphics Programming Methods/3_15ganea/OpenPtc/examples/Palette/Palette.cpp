//
// Palette example for OpenPTC 1.0 C++ Implementation
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
        Format format(8);

        // open console
        console.open("Palette example",format);

        // create surface
        Surface surface(console.width(),console.height(),format);
        
        // create palette
        Palette palette;

        // generate palette
        int32 data[256];
        for (int i=0; i<256; i++) data[i] = i;

        // load palette data
        palette.load(data);

        // set console palette
        console.palette(palette);
        
        // set surface palette
        surface.palette(palette);

        // loop until a key is pressed
        while (!console.key())
        {
            // lock surface pixels
            char8 *pixels = (char8*) surface.lock();

            // get surface dimensions
            const int width  = surface.width();
            const int height = surface.height();

            // draw random pixels
            for (int i=0; i<100; i++)
            {
                // get random position
                const int x = random(width);
                const int y = random(height);

                // get random color index
                const char8 index = random(256);

                // draw color [index] at position [x,y]
                pixels[x+y*width] = index;
            }
        
            // unlock surface
            surface.unlock();

            // copy to console
            surface.copy(console);

            // update console
            console.update();
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
