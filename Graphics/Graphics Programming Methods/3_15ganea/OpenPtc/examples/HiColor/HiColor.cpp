//
// HiColor example for OpenPTC 1.0 C++ Implementation
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
        Format format(16,0xF800,0x07E0,0x001F);
 
        // open the console
        console.open("HiColor example",format);

        // create surface matching console dimensions
        Surface surface(console.width(),console.height(),format);
        
        // loop until a key is pressed
        while (!console.key())
        {
            // lock surface pixels
            short16 *pixels = (short16*) surface.lock();

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
                pixels[x+y*width] = (short16) ( ( (r & 0x00F8) << 8 ) |
                                                ( (g & 0x00FC) << 3 ) |
                                                ( (b & 0x00F8) >> 3 ) );
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
