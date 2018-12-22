/*
   Building Blocks Engine
   Ultimate Game Engine Design and Architecture (2006)
   Created by Allen Sherrod
*/


#ifndef _BB_WINDOW_HANDLE_H_
#define _BB_WINDOW_HANDLE_H_

#ifdef _WIN32
   #include<windows.h>
#else
#ifdef _MAC
   #include<Carbon/Carbon.h>
#else
   #include<stdio.h>
   #include<GL/glx.h>
   #include<GL/gl.h>
   #include<GL/glu.h>
   #include<GL/glext.h>
   #include<GL/glxext.h>
   #include<X11/extensions/xf86vmode.h>
   #include<X11/keysym.h>
#endif
#endif

#include<NameSpace.h>
#include<string.h>


DECLARE_ENGINE_NAMESPACE


class WindowHandle
{
   public:
      WindowHandle()
      {
         memset(this, 0, sizeof(WindowHandle));
      }

#ifdef _WIN32
      HWND m_hwnd;
      HINSTANCE m_hInstance;
#else
#ifdef _MAC
      WindowRef m_window;
#else
      Display *m_disp;
#endif
#endif
};


END_ENGINE_NAMESPACE

#endif
