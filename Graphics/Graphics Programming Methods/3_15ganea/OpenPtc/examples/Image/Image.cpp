//
// Image example for OpenPTC 1.0 C++ Implementation
// Programmed by Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include "ptc.h"
#include <fstream.h>


void load(Surface &surface,char filename[])
{
    // open image file
    ifstream is(filename,ios::in|ios::binary|ios::nocreate);

    // check image file stream
    if (!is) throw Error("could not load image");

    // skip header
    is.seekg(18);

    // get surface dimensions
    const int width  = surface.width();
    const int height = surface.height();
    
    // allocate image pixels
    char8 *pixels = new char8[width*height*3];

    // read image pixels one line at a time
    for (int y=height-1; y>=0; y--) is.read((char*)&pixels[width*y*3],width*3);

    // load pixels to surface
    surface.load(pixels,width,height,width*3,Format(24,0x00FF0000,0x0000FF00,0x000000FF),Palette());

    // free image pixels
    delete[] pixels;
}


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    try
    {
        // create console
        Console console;

        // create format
        Format format(32,0x00FF0000,0x0000FF00,0x000000FF);

        try
        {
            // try to open the console matching the image resolution
            console.open("Image example",320,200,format);
        }
        catch (Error&)
        {
            // fallback to the default resolution
            console.open("Image example",format);
        }

        // create surface
        Surface surface(320,200,format);

        // load image to surface
        load(surface,"Image.tga");

        // copy surface to console
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
