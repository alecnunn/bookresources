/*
 * C example for OpenPTC 1.0 C++ Implementation
 * Programmed by Glenn Fiedler (ptc@gaffer.org)
 * This source code is in the public domain
 */

/* include files */
#include "ptc.h"
#include <stdlib.h>


int random(int max)
{
    /* random number */
    return rand() % max;
}


/* ptc objects */
PTC_FORMAT format = 0;
PTC_SURFACE surface = 0;
PTC_CONSOLE console = 0;


void cleanup()
{
    /* destroy objects */
    ptc_format_destroy(format);
    ptc_surface_destroy(surface);
    ptc_console_destroy(console);
}


void PTC_CALLBACK handler(PTC_ERROR error)
{
    /* cleanup */
    cleanup();

    /* report error */
    ptc_error_report(error);
}


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    /* set error handler */
    ptc_error_handler(handler);

    /* create console */
    console = ptc_console_create();

    /* create format */
    format = ptc_format_create_direct(32,0x00FF0000,0x0000FF00,0x000000FF,0);

    /* open the console */
    ptc_console_open_format(console,"C example",format,0);

    /* create surface matching console dimensions */
    surface = ptc_surface_create(ptc_console_width(console),ptc_console_height(console),format);
    
    /* loop until a key is pressed */
    while (!ptc_console_key(console))
    {
        /* data */
        int i;
        int x,y;
        int r,g,b;
        int width;
        int height;
        ptc_int32 *pixels;

        /* lock surface pixels */
        pixels = (ptc_int32*) ptc_surface_lock(surface);

        /* get surface dimensions */
        width  = ptc_surface_width(surface);
        height = ptc_surface_height(surface);

        /* draw random pixels */
        for (i=0; i<100; i++)
        {
            // get random position
            x = random(width);
            y = random(height);

            // get random color
            r = random(256);
            g = random(256);
            b = random(256);

            // draw color [r,g,b] at position [x,y]
            pixels[x+y*width] = (r<<16) + (g<<8) + b;
        }
    
        /* unlock surface */
        ptc_surface_unlock(surface);

        /* copy to console */
        ptc_surface_copy(surface,console);
        
        /* update console */
        ptc_console_update(console);
    }

    /* close console */
    ptc_console_close(console);

    /* cleanup */
    cleanup();

    /* exit */
    return 0;
}
