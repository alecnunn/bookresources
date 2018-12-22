/*
   Simple Black Jack Game Demo - Chapter 9
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include"blackJackLinux.h"
#include<LinuxInput.h>


BlackJackGameLinux *g_blackJackGame = NULL;


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
   g_blackJackGame = new BlackJackGameLinux(disp);

   if(g_blackJackGame == NULL)
      {
         printf("Error creating g_blackJackGame!\n");
         return 0;
      }

   if(g_blackJackGame->GameInitialize() == false)
      {
         printf("Error in initialize!\n");
         delete g_blackJackGame;
         return 0;
      }

   while(!g_blackJackGame->isQuitDemo())
      {
         while(XPending(disp) > 0)
            {
               XNextEvent(disp, &event);

               switch(event.type)
                  {
                     case Expose:
                        if(event.xexpose.count != 0)
                           break;

                        if(g_blackJackGame != NULL)
                           g_blackJackGame->EnterGameLoop();
                        break;


                     case ConfigureNotify:
                        if(g_blackJackGame != NULL)
                           g_blackJackGame->Resize(event.xconfigure.width,
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
                              g_blackJackGame->SetQuitDemo(true);
                           }
                        break;


                     default:
                        break;
                  }
               }

            if(g_blackJackGame != NULL)
               g_blackJackGame->EnterGameLoop();

            bbe::ClearButtonFlags();
         }

   g_blackJackGame->GameShutdown();
   delete g_blackJackGame;
   g_blackJackGame = NULL;

   XCloseDisplay(disp);

   return 0;
}
