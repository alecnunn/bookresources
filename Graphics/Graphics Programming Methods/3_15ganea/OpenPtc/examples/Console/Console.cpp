//
// Console example for OpenPTC 1.0 C++ Implementation
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

        // open the console with one page
        console.open("Console example",1);

        // create palette
        Palette palette;

        // generate palette
        int32 data[256];
        for (int i=0; i<256; i++) data[i] = i;

        // load palette data
        palette.load(data);

        // set console palette
        console.palette(palette);

        // loop until a key is pressed
        while (!console.key())
        {
            // lock console pixels
            char8 *pixels = (char8*) console.lock();

            // get console dimensions
            const int width  = console.width();
            const int height = console.height();
            const int pitch  = console.pitch();
            
            // get console format
            const Format format = console.format();

            // get format information
            const int bits  = format.bits();
            const int bytes = format.bytes();

            // draw random pixels
            for (int i=0; i<100; i++)
            {
                // get random position
                const int x = random(width);
                const int y = random(height);

                // generate random color integer
                const int32 color = ( random(255) <<  0 ) | 
                                    ( random(255) <<  8 ) |
                                    ( random(255) << 16 ) |
                                    ( random(255) << 24 );

                // calculate pointer to pixel [x,y]
                char8 *pixel = pixels + y*pitch + x*bytes;

                // check bits
                switch (bits)
                {
                    case 32:
                    {
                        // 32 bits per pixel
                        *(int32*)pixel = color;
                    }
                    break;

                    case 24:
                    {
                        // 24 bits per pixel
                        char8 *data = (char8*) pixel;
                        data[0] = (char8) ( (color & 0x000000FF) >> 0  );
                        data[1] = (char8) ( (color & 0x0000FF00) >> 8  );
                        data[2] = (char8) ( (color & 0x00FF0000) >> 16 );
                    }
                    break;

                    case 16:
                    {
                        // 16 bits per pixel
                        *(short16*)pixel = (short16) color;
                    }
                    break;

                    case 8:
                    {
                        // 8 bits per pixel
                        *(char8*)pixel = (char8) color;
                    }
                    break;

                }
            }
        
            // unlock console
            console.unlock();

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
