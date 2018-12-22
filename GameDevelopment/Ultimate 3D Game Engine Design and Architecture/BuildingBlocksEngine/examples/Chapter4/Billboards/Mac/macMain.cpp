/*
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<Carbon/Carbon.h>
#include"../main.h"

void UpdateTimer(EventLoopTimerRef timer, void *userData);


int main(int argc, char* argv[])
{
   IBNibRef nibRef;
   OSStatus err;
   WindowRef window;
   bbe::RenderParams params;

   // Create a Nib reference passing the name of the nib file.
   err = CreateNibReference(CFSTR("main"), &nibRef);
   require_noerr( err, CantGetNibRef );

   // Once the nib reference is created, set the menu bar.
   err = SetMenuBarFromNib(nibRef, CFSTR("MenuBar"));
   require_noerr( err, CantSetMenuBar );

   // Then create a window.
   err = CreateWindowFromNib(nibRef, CFSTR("MainWindow"), &window);
   require_noerr( err, CantCreateWindow );

   // We don't need the nib reference anymore.
   DisposeNibReference(nibRef);
	
   Rect windowRect;
	
   // Get the window rectangle.
	GetWindowBounds(window, kWindowContentRgn, &windowRect);
    
   // The window was created hidden so show it.
   ShowWindow(window);
	
   params.m_handle.m_window = window;
   
   if(DemoInitialize(params) == 0)
      {
         printf("Error in initialize!/n");
         return 0;
      }
   
   DemoResize(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
   
   // Set up a timer to refresh the scene asap.
	InstallEventLoopTimer(GetMainEventLoop(), 0, kEventDurationSecond / 60,
                         NewEventLoopTimerUPP(UpdateTimer), NULL, 0);
    
   // Loop through the application until the user closes through app menu.
   RunApplicationEventLoop();
	
   DemoShutdown();
   
   // Destroy the window.
	DisposeWindow(window);

   CantCreateWindow:
   CantSetMenuBar:
   CantGetNibRef:
	return err;
}


void UpdateTimer(EventLoopTimerRef timer, void *userData)
{
	DemoRender();
}