//
// Buffer example for OpenPTC 1.0 C++ Implementation
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
 
        // open the console
        console.open("Buffer example",format);

        // get console dimensions
        const int width  = console.width();
        const int height = console.height();

        // allocate a buffer of pixels
        int32 *pixels = new int32[width*height];
                
        // loop until a key is pressed
        while (!console.key())
        {
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
        
            // load pixels to console
            console.load(pixels,width,height,width*4,format,Palette());
            
            // update console
            console.update();
        }
        
        // free pixels buffer
        delete[] pixels;

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
