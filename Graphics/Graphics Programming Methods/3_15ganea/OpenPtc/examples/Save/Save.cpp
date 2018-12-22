//
// Save example for OpenPTC 1.0 C++ Implementation
// Programmed by Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include "ptc.h"
#include <math.h>
#include <fstream.h>


void save(Surface &surface,char filename[])
{
    // open image file for writing
    ofstream os(filename,ios::out|ios::binary);

    // check image file stream
    if (!os) throw Error("could not open image file for writing");

    // get surface dimensions
    const int width  = surface.width();
    const int height = surface.height();
    
    // generate the header for a true color targa image
    char8 header[18] = { 0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    // set targa image width
    header[12] = width & 0xFF;
    header[13] = width >> 8;
    
    // set targa image height
    header[14] = height & 0xFF;
    header[15] = height >> 8;

    // set bits per pixel
    header[16] = 24;

    // write tga header
    os.write(header,18);

    // calculate size of image pixels
    const int size = width * height * 3;
    
    // allocate image pixels
    char8 *pixels = new char8[size];

    // save surface to image pixels
    surface.save(pixels,width,height,width*3,Format(24,0x00FF0000,0x0000FF00,0x000000FF),Palette());

    // write image pixels one line at a time
    for (int y=height-1; y>=0; y--) os.write((char*)&pixels[width*y*3],width*3);

    // free image pixels
    delete[] pixels;
}


inline int calculate(float real,float imaginary,int maximum)
{
    // complex number 'c'
    const float c_r = real;
    const float c_i = imaginary;

    // complex 'z'
    float z_r = 0;
    float z_i = 0;
    
    // complex 'z' squares
    float z_r_squared = 0;
    float z_i_squared = 0;

    // mandelbrot function iteration loop
    for (int count=0; count<maximum; count++)
    {
        // square 'z' and add 'c'
        z_i = 2 * z_r * z_i + c_i;
        z_r = z_r_squared - z_i_squared + c_r;

        // update 'z' squares
        z_r_squared = z_r * z_r;
        z_i_squared = z_i * z_i;

        // calculate squared magnitude of complex 'z'
        const float z_squared_magnitude = z_r_squared + z_i_squared;

        // stop iterating if the magnitude of 'z' is greater than two
        if (z_squared_magnitude>=4) return count;
    }

    // maximum
    return 0;
}


void mandelbrot(Console &console,Surface &surface,float x1,float y1,float x2,float y2)
{
    // constant values
    const int entries = 1024;
    const int maximum = 1024;

    // fractal color table
    int32 table[entries];

    // generate fractal color table
    for (int i=0; i<entries; i++)
    {
        // useful pi constant
        const float pi = 3.1415926f;

        // calculate normalized index
        const float index = i / (float) entries;

        // calculate sine curve time value
        const float time = index * pi - pi/2;

        // lookup sine curve intensity at time and scale to [0,1]
        float intensity = (float) ( sin(time) + 1.0f ) / 2.0f;

        // raise the intensity to a power
        intensity = (float) pow(intensity,0.1);

        // store intensity as a shade of blue
        table[i] = (int32) ( 255 * intensity );
    }

    // lock surface pixels
    int32 *pixels = (int32*) surface.lock();

    // get surface dimensions
    const int width  = surface.width();
    const int height = surface.height();

    // current pixel pointer
    int32 *pixel = pixels;

    // calculate real x,y deltas
    const float dx = (x2 - x1) / width;
    const float dy = (y2 - y1) / height;

    // imaginary axis
    float imaginary = y1;

    // iterate down surface y
    for (int y=0; y<height; y++)
    {
        // real axis
        float real = x1;

        // iterate across surface x
        for (int x=0; x<width; x++)
        {
            // calculate the mandelbrot interation count
            const int count = calculate(real,imaginary,maximum);

            // calculate color table index
            const int index = count % entries;

            // lookup color from iteration
            const int32 color = table[index];

            // store color
            *pixel = color;

            // next pixel
            pixel ++;

            // update real
            real += dx;
        }

        // update imaginary
        imaginary += dy;

        // setup line area
        Area area(0,y,width,y+1);

        // copy surface area to console
        surface.copy(console,area,area);

        // update console area
        console.update(area);
    }

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
 
        // open the console with a single page
        console.open("Save example",format,1);

        // create surface matching console dimensions
        Surface surface(console.width(),console.height(),format);

        // setup viewing area
        const float x1 = -2.00f;
        const float y1 = -1.25f;
        const float x2 = +1.00f;
        const float y2 = +1.25f;

        // render the mandelbrot fractal
        mandelbrot(console,surface,x1,y1,x2,y2);

        // save mandelbrot image
        save(surface,"Save.tga");

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
