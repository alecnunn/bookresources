/*
   Billboards - Chapter 4
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"../main.h"


void InputHandler(KeySym key)
{
   switch(key)
      {
         case XK_Escape:
            g_quitDemo = True;
            break;
      }
}


int main(int argc, char **argv)
{
   XEvent event;
   KeySym key;
    
   g_quitDemo = False;

   bbe::RenderParams params;
   params.m_handle.m_disp = XOpenDisplay(0);

   if(!DemoInitialize(params))
      {
         printf("Error in initialize!\n");
         g_quitDemo = true;
      }

   while(!g_quitDemo)
      {
         while(XPending(params.m_handle.m_disp) > 0)
            {
               XNextEvent(params.m_handle.m_disp, &event);

               switch(event.type)
                  {
                     case Expose:
                        if(event.xexpose.count != 0)
                           break;

                        DemoUpdate();
                        DemoRender();
                        break;


                     case ConfigureNotify:
                        DemoResize(event.xconfigure.width,
                                   event.xconfigure.height);
                        break;


                     case KeyPress:
                        key = XLookupKeysym(&event.xkey, 0);
                        InputHandler(key);
                        break;


                     case ClientMessage:
                        if(*XGetAtomName(params.m_handle.m_disp,
                           event.xclient.message_type) == *"WM_PROTOCOLS")
                           {
                              g_quitDemo = True;
                           }
                        break;


                     default:
                        break;
                  }
               }

            DemoUpdate();
            DemoRender();
         }

   DemoShutdown();

   XCloseDisplay(params.m_handle.m_disp);

   return 0;
}
