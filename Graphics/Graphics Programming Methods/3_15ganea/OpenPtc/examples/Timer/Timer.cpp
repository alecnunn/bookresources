//
// Timer example for OpenPTC 1.0 C++ Implementation
// Programmed by Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include "ptc.h"
#include <math.h>


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    try
    {
        // create console
        Console console;
        
        // create format
        Format format(32,0x00FF0000,0x0000FF00,0x000000FF);
 
        // open the console
        console.open("Timer example",format);

        // create surface matching console dimensions
        Surface surface(console.width(),console.height(),format);

        // create timer
        Timer timer;

        // start timer
        timer.start();

        // loop until a key is pressed
        while (!console.key())
        {
            // get current time from timer
            const double time = timer.time();
            
            // clear surface
            surface.clear();

            // lock surface pixels
            int32 *pixels = (int32*) surface.lock();

            // get surface dimensions
            const int width  = surface.width();
            const int height = surface.height();

            // sine curve parameters
            const float repeats   = 2;
            const float center    = height / 2.0f;
            const float magnitude = height / 3.0f;

            // render a sine curve
            for (int x=0; x<width; x++)
            {
                // useful pi constant
                const float pi = 3.1415926f;

                // rescale 'x' in the range [0,2*pi]
                const float sx = x / (float) width * 2 * pi;

                // calculate time at current position
                const double t = time + sx * repeats;

                // lookup sine intensity at time 't'
                const float intensity = (float) sin(t);

                // convert intensity to a y position on the surface
                const int y = (int) ( center + intensity * magnitude );

                // plot pixel on sine curve
                pixels[x+y*width] = 0x000000FF;
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
