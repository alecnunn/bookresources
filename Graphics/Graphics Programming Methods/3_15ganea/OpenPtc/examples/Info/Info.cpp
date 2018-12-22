//
// Info example for OpenPTC 1.0 C++ Implementation
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


void main()
{
    try
    {
        // print ptc version string define
        printf("[ptc version]\n%s\n\n",__PTC_VERSION__);

        // create console
        Console console;

        // open the console
        console.open("Info example");

        // print console data
        printf("[console data]\n");
        printf("name   = %s\n",console.name());
        printf("title  = %s\n",console.title());
        printf("width  = %d\n",console.width());
        printf("height = %d\n",console.height());
        printf("pages  = %d\n",console.pages());
        printf("pitch  = %d\n",console.pitch());
        printf("format = ");
        print(console.format());
        printf("\n\n");

        // print console information
        printf("[console information]\n");
        printf("%s\n",console.information());

        // close console
        console.close();
    }
    catch (Error &error)
    {
        // report error
        error.report();
    }
}
