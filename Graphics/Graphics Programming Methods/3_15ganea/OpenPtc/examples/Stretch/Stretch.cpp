//
// Stretch example for OpenPTC 1.0 C++ Implementation
// Programmed by Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include "ptc.h"
#include <math.h>
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
 
        // open the console
        console.open("Stretch example",format);

        // create surface matching console dimensions
        Surface surface(console.width(),console.height(),format);

        // create image surface
        Surface image(320,140,format);

        // load image to surface
        load(image,"Stretch.tga");

        // setup stretching parameters
        const int x  = surface.width()  / 2;
        const int y  = surface.height() / 2;
        const int dx = surface.width()  / 2;
        const int dy = surface.height() / 3;

        // create timer
        Timer timer;

        // start timer
        timer.start();

        // loop until a key is pressed
        while (!console.key())
        {
            // get current time from timer
            const double time = timer.time();

            // clear surface to white background
            surface.clear(Color(1.0f,1.0f,1.0f));

            // calculate zoom factor at current time
            const float zoom = 2.5f * (float) ( 1.0 - cos(time) );
            
            // calculate zoomed image coordinates
            const int x1 = (int) ( x - zoom * dx );
            const int y1 = (int) ( y - zoom * dy );
            const int x2 = (int) ( x + zoom * dx );
            const int y2 = (int) ( y + zoom * dy );

            // setup image copy area
            Area area(x1,y1,x2,y2);

            // copy and stretch image to surface
            image.copy(surface,image.area(),area);
    
            // copy surface to console
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
