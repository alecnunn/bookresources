//
// Tunnel demo for OpenPTC 1.0 C++ API
// Originally coded by Thomas Rizos (rizos@swipnet.se)
// Adapted for PTC by Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU GPL
//

// include files
#include "ptc.h"
#include <math.h>


// tunnel class
class Tunnel
{
    public:

        Tunnel()
        {
            // allocate tables
            tunnel  = new int32[320*200];
            texture = new char8[256*256*2];

            // setup
            setup();
        }

        ~Tunnel()
        {
            // free tables
            delete[] tunnel;
            delete[] texture;
        }

        void setup()
        {
            // useful pi constant
            const float pi = 3.1415926f;

            // tunnel index
            int index = 0;

            // generate tunnel table
            for (int y=100; y>-100; y--)
            {
                for (int x=-160; x<160; x++)
                {
                    // calculate angle from center
                    double angle = atan2(y,x) * 256 / pi / 2;
                
                    // calculate radius from center
                    double radius = sqrt(x*x + y*y);
                
                    // clamp radius to minimum
                    if (radius<1.0) radius = 1.0;

                    // texture coordinates
                    double u = angle;
                    double v = 6000/radius;
                
                    // calculate texture index for (u,v)
                    tunnel[index++] = ((int32) v & 0xFF) * 256 + ((int32) u & 0xFF);
                }
            }

            // generate blue plasma texture
            index = 0;
            float angle2 = pi * 2/256 * 230;
            for (y=0; y<256*2; y++)
            {
                float angle1 = pi * 2/256 * 100;
                for (int x=0; x<256; x++)
                {
                    texture[index++] = (char8)( (float)sin(angle1)*80 + (float)sin(angle2)*40 + 128);
                    angle1 += pi*2/256*3;
                }
                angle2 += pi * 2/256 *2;
            }
        }

        void draw(int32 *buffer,float t)
        {
            // tunnel control functions
            int x = (int) ( sin(t)*99.9 );
            int y = (int) ( t*200 );
                
            // calculate tunnel scroll offset
            int scroll = ((y&0xFF) << 8) + (x&0xFF);

            // loop through each pixel
            for (int i=0; i<64000; i++)
            {
                // lookup tunnel texture
                buffer[i] = texture[tunnel[i]+scroll];
            }
  }

    private:

        // tunnel data
        int32 *tunnel;
        char8 *texture;
};


int APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nCmdShow)
{
    try
    {
        // create format
        Format format(32,0x00FF0000,0x0000FF00,0x000000FF);

        // create console
        Console console;

        // open console
        console.open("Tunnel demo",320,200,format);

        // create surface
        Surface surface(320,200,format);

        // create tunnel
        Tunnel tunnel;

        // time data
        float time  = 0;
        float delta = 0.03f;

        // loop until a key is pressed
        while (!console.key())
        {
            // lock surface
            int32 *buffer = (int32*) surface.lock();

            // draw tunnel
            tunnel.draw(buffer,time);

            // unlock surface
            surface.unlock();

            // copy to console
            surface.copy(console);

            // update console
            console.update();

            // update time
            time += delta;
        }

        // close console
        console.close();
    }
    catch (Error error)
    {
        // report error
        error.report();
    }

    // exit
    return 0;
}
