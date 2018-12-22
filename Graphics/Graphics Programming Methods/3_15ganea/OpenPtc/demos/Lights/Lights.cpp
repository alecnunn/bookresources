//
// Lights demo for OpenPTC 1.0 C++ API
// Copyright (c) Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU GPL
//

// include files
#include "ptc.h"
#include <math.h>


// distance lookup table
unsigned distance_table[300][512];      // note: 16.16 fixed


// intensity calculation
inline unsigned CalcIntensity(int dx,int dy,int i)
{
    // lookup intensity at [dx,dy]
    return i*distance_table[dy][dx];
}


int APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nCmdShow)
{
    try
    {
        // create console
        Console console;

        // open console
        console.open("Lights demo",320,200,Format(32,0x00FF0000,0x0000FF00,0x000000FF));

        // create surface
        Surface surface(320,200,Format(8));
     
        // setup intensity table
        for (int dy=0; dy<200; dy++)
        {
            for (int dx=0; dx<512; dx++)
            {
                float divisor = (float) sqrt((dx*dx)+(dy*dy));
                if (divisor<0.3f) divisor = 0.3f;
                distance_table[dy][dx] = (unsigned)(65535.0f/divisor);      // float -> 16.16 fixed
            }
        }

        // create palette
        Palette palette;

        // generate greyscale palette
        int32 *data = palette.lock();
        for (int i=0; i<256; i++) data[i] = (i<<16) | (i<<8) | i;
        palette.unlock();

        // set console palette
        console.palette(palette);

        // set surface palette
        surface.palette(palette);

        // data
        float cx1 = 60;
        float cy1 = 110;
        float cx2 = 100;
        float cy2 = 80;
        float cx3 = 250;
        float cy3 = 110;
        float cx4 = 200;
        float cy4 = 90;
        float dx1 = 0;
        float dy1 = 0;
        float dx2 = 0;
        float dy2 = 0;
        float dx3 = 0;
        float dy3 = 0;
        float dx4 = 0;
        float dy4 = 0;
        unsigned i1 = 0;
        unsigned i2 = 0;
        unsigned i3 = 0;
        unsigned i4 = 0;

        // time data
        float move_t = 0.3f;
        float move_dt = 0.1f;
        float move_ddt = 0.0006f;
        float flash_t = 0.1f;
        float flash_dt = 0.0f;
        float flash_ddt = 0.0004f;

        // control data
        float max_intensity = 30.0f;
        float max_intensity_inc = 0.2f;

        // main loop
        while (!console.key())
        {
            // source positions
            int x1 = (int)(cx1+dx1);
            int y1 = (int)(cy1+dy1);
            int x2 = (int)(cx2+dx2);
            int y2 = (int)(cy2+dy2);
            int x3 = (int)(cy3+dx3);
            int y3 = (int)(cy3+dy3);
            int x4 = (int)(cx4+dx4);
            int y4 = (int)(cy4+dy4);

            // lock surface
            char8 *pixels = (char8*) surface.lock();

            // get surface dimensions
            int width  = surface.width();
            int height = surface.height();

            // line loop
            for (int y=0; y<200; y++)
            { 
                // calcalate pointer to start of line
                char8 *line = pixels + y*width;

                // get y deltas
                int dy1 = abs(y-y1);
                int dy2 = abs(y-y2);
                int dy3 = abs(y-y3);
                int dy4 = abs(y-y4);

                // setup x
                int x = 0;

                // line loop
                while (x<width)
                {
                    // get x deltas
                    int dx1 = abs(x1-x);
                    int dx2 = abs(x2-x);
                    int dx3 = abs(x3-x);
                    int dx4 = abs(x4-x);

                    // get increments
                    int ix1 = 1;
                    int ix2 = 1;
                    int ix3 = 1;
                    int ix4 = 1;
                    if (x1>x) ix1 = -1;
                    if (x2>x) ix2 = -1;
                    if (x3>x) ix3 = -1;
                    if (x4>x) ix4 = -1;

                    // set span length to min delta
                    int length = width - x;
                    if (x1>x && dx1<length) length = dx1;
                    if (x2>x && dx2<length) length = dx2;
                    if (x3>x && dx3<length) length = dx3;
                    if (x4>x && dx4<length) length = dx4;

                    // pixel loop
                    while (length--)
                    {
                        // calc intensities
                        unsigned intensity;
                        intensity =  CalcIntensity(dx1,dy1,i1);
                        intensity += CalcIntensity(dx2,dy2,i2);
                        intensity += CalcIntensity(dx3,dy3,i3);
                        intensity += CalcIntensity(dx4,dy4,i4);
                        intensity >>= 16;
                        if (intensity>255) intensity=255;

                        // update deltas
                        dx1 += ix1;
                        dx2 += ix2;
                        dx3 += ix3;
                        dx4 += ix4;

                        // store the pixel
                        line[x++] = (char8) intensity;
                   }
                }
            }

            // unlock surface
            surface.unlock();

            // move the lights around
            dx1 = 50  * (float)sin( (move_t+0.0f) * 0.10f );
            dy1 = 80  * (float)sin( (move_t+0.6f) * 0.14f );
            dx2 = 100 * (float)sin( (move_t+0.1f) * 0.10f );
            dy2 = 30  * (float)sin( (move_t-0.4f) * 0.30f );
            dx3 = 39  * (float)sin( (move_t+9.9f) * 0.20f );
            dy3 = 50  * (float)sin( (move_t+0.4f) * 0.30f );
            dx4 = 70  * (float)sin( (move_t-0.3f) * 0.25f );
            dy4 = 40  * (float)sin( (move_t-0.1f) * 0.31f );

            // flash intensity
            i1 = (unsigned) ( max_intensity * ( (float)sin( (flash_t+0.000f) * 1.000f ) + 1 ) );
            i2 = (unsigned) ( max_intensity * ( (float)sin( (flash_t+2.199f) * 0.781f ) + 1 ) );
            i3 = (unsigned) ( max_intensity * ( (float)sin( (flash_t-1.450f) * 1.123f ) + 1 ) );
            i4 = (unsigned) ( max_intensity * ( (float)sin( (flash_t+0.000f) * 0.500f ) + 1 ) );

            // update time
            move_t   += move_dt;
            move_dt  += move_ddt;
            flash_t  += flash_dt;
            flash_dt += flash_ddt;

            // reset on big flash...
            if (move_t>600 && i1>10000 && i2>10000 && i3>10000 && i4>10000)
            {
                move_t = 0.3f;
                move_dt = 0.1f;
                move_ddt = 0.0006f;
                flash_t = 0.1f;
                flash_dt = 0.0f;
                flash_ddt = 0.0004f;
                max_intensity = 0.0f;
                max_intensity_inc = 0.2f;
            }

            // update intensity
            max_intensity += max_intensity_inc;
            max_intensity_inc += 0.008f;

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
