//
// Modes example for OpenPTC 1.0 C++ Implementation
// Programmed by Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include "ptc.h"
#include <stdio.h>


void print(const Format &format)
{
    // check format type
    if (format.direct())
    {
        // check alpha
        if (!format.a())
        {
            // direct color format without alpha
            printf("Format(%d,0x%.8X,0x%.8X,0x%.8X)",format.bits(),format.r(),format.g(),format.b());
        }
        else
        {
            // direct color format with alpha
            printf("Format(%d,0x%.8X,0x%.8X,0x%.8X,0x%.8X)",format.bits(),format.r(),format.g(),format.b(),format.a());
        }
    }
    else
    {
        // indexed color format
        printf("Format(%d)",format.bits());
    }
}


void print(const Mode &mode)
{
    // print mode width and height
    printf(" %4d x %-4d x ",mode.width(),mode.height());

    // print mode format
    print(mode.format());

    // newline
    printf("\n");
}


void main()
{
    try
    {
        // create console
        Console console;
        
        // get list of console modes
        const Mode *modes = console.modes();

        // check for empty list
        if (!modes[0].valid())
        {
            // the console mode list was empty
            printf("[console mode list is not available]\n");
        }
        else
        {
            // print mode list header
            printf("[console modes]\n");

            // mode index
            int index = 0;

            // iterate through all modes
            while (modes[index].valid())
            {
                // print mode
                print(modes[index]);

                // next mode
                index ++;
            }
        }

        // close console
        console.close();
    }
    catch (Error &error)
    {
        // report error
        error.report();
    }
}
