/*
   Walk Through Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"walkThroughLinux.h"
#include<LinuxInput.h>


WalkThroughLinux *g_walkthrough = NULL;


void InputHandler(KeySym key)
{
   switch(key)
      {
         case XK_Escape:
            bbe::SetButtonFlag(BB_BUTTON_ESCAPE, true);
            break;

         case XK_Down:
            bbe::SetButtonFlag(BB_BUTTON_ARROW_DOWN, true);
            break;

         case XK_Up:
            bbe::SetButtonFlag(BB_BUTTON_ARROW_UP, true);
            break;

         case XK_Left:
            bbe::SetButtonFlag(BB_BUTTON_ARROW_LEFT, true);
            break;

         case XK_Right:
            bbe::SetButtonFlag(BB_BUTTON_ARROW_RIGHT, true);
            break;

         case XK_s:
            bbe::SetButtonFlag(BB_BUTTON_S, true);
            break;

         case XK_h:
            bbe::SetButtonFlag(BB_BUTTON_H, true);
            break;
      }
}


int main(int argc, char **argv)
{
   XEvent event;
   KeySym key;

   Display *disp = XOpenDisplay(0);
   g_walkthrough = new WalkThroughLinux(disp);

   if(g_walkthrough == NULL)
      {
         printf("Error creating g_walkthrough!\n");
         return 0;
      }

   if(g_walkthrough->GameInitialize() == false)
      {
         printf("Error in initialize!\n");
         delete g_walkthrough;
         return 0;
      }

   while(!g_walkthrough->isQuitDemo())
      {
         while(XPending(disp) > 0)
            {
               XNextEvent(disp, &event);

               switch(event.type)
                  {
                     case Expose:
                        if(event.xexpose.count != 0)
                           break;

                        if(g_walkthrough != NULL)
                           g_walkthrough->EnterGameLoop();
                        break;


                     case ConfigureNotify:
                        if(g_walkthrough != NULL)
                           g_walkthrough->Resize(event.xconfigure.width,
                                                   event.xconfigure.height);
                        break;


                     case KeyPress:
                        key = XLookupKeysym(&event.xkey, 0);
                        InputHandler(key);
                        break;


                     case ClientMessage:
                        if(*XGetAtomName(disp,
                           event.xclient.message_type) == *"WM_PROTOCOLS")
                           {
                              g_walkthrough->SetQuitDemo(true);
                           }
                        break;


                     default:
                        break;
                  }
               }

            if(g_walkthrough != NULL)
               g_walkthrough->EnterGameLoop();

            bbe::ClearButtonFlags();
         }

   g_walkthrough->GameShutdown();
   delete g_walkthrough;
   g_walkthrough = NULL;

   XCloseDisplay(disp);

   return 0;
}
