//
// Clear example for OpenPTC 1.0 C++ Implementation
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


inline float random()
{
    // random float
    return rand() / (float)RAND_MAX;
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
        console.open("Clear example",format);

        // create surface matching console dimensions
        Surface surface(console.width(),console.height(),format);
        
        // loop until a key is pressed
        while (!console.key())
        {
            // get surface dimensions
            const int width  = surface.width();
            const int height = surface.height();

            // get random position
            const int x = random(width);
            const int y = random(height);
            
            // get random area size
            const int size = random(width/8);

            // setup clear area
            Area area(x-size,y-size,x+size,y+size);

            // create random color
            Color color(random(),random(),random());
            
            // clear surface area with color
            surface.clear(color,area);
            
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
