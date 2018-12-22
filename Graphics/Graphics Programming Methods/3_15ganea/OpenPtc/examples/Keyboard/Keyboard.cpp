//
// Keyboard example for OpenPTC 1.0 C++ Implementation
// Programmed by Glenn Fiedler (ptc@gaffer.org)
// This source code is in the public domain
//

// include files
#include "ptc.h"


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    try
    {
        // create console
        Console console;
        
        // create format
        Format format(32,0x00FF0000,0x0000FF00,0x000000FF);
 
        // open the console
        console.open("Keyboard example",format);

        // create surface matching console dimensions
        Surface surface(console.width(),console.height(),format);
        
        // setup cursor data
        int x = surface.width() / 2;
        int y = surface.height() / 2;
        const int size = surface.width() / 10;
        const Color color(1.0f,1.0f,1.0f);

        // main loop
        while (true)
        {
            // check for key press
            while (console.key())
            {
                // read console key press
                const Key key = console.read();

                // move slow
                int delta = 1;

                // shift modifier
                if (key.shift())
                {
                    // move fast
                    delta = 10;
                }

                // handle cursor keys
                switch (key.code())
                {
                    case Key::LEFT:  x-=delta; break;
                    case Key::RIGHT: x+=delta; break;
                    case Key::UP:    y-=delta; break;
                    case Key::DOWN:  y+=delta; break;
                }

                // exit when escape is pressed
                if (key.code()==Key::ESCAPE) return 0;
            }

            // clear surface
            surface.clear();

            // setup cursor area
            Area area(x-size,y-size,x+size,y+size);

            // draw cursor as a quad
            surface.clear(color,area);

            // copy to console
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
