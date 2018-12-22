//
// Pixel example for OpenPTC 1.0 C++ Implementation
// Programmed by Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include "ptc.h"


inline void putpixel(Surface &surface,int x,int y,char8 r,char8 g,char8 b)
{
    // lock surface pixels
    int32 *pixels = (int32*) surface.lock();

    // pack the color integer from r,g,b components
    const int32 color = (int32(r)<<16) | (int32(g)<<8) | int32(b);

    // plot the pixel on the surface
    pixels[x+y*surface.width()] = color;
    
    // unlock surface
    surface.unlock();
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
        console.open("Pixel example",format);

        // create surface matching console dimensions
        Surface surface(console.width(),console.height(),format);
        
        // plot a white pixel in the middle of the surface
        putpixel(surface,surface.width()/2,surface.height()/2,255,255,255);

        // copy to console
        surface.copy(console);
        
        // update console
        console.update();

        // read key
        console.read();

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
