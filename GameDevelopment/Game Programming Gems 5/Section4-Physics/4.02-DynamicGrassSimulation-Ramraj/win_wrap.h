#ifndef __WIN_WRAP_H__
#define __WIN_WRAP_H__
//----------------------------------------------------------------------------
//
//win_wrap.h
//
//Copyright © Rishi Ramraj, 2004
//Basic Windows GL wrapper
//----------------------------------------------------------------------------

//··········································································//
//    header :: Inclusions
//··········································································//

//··········································································//
//    header :: Definitions
//··········································································//

#define WM_TGL_FULL_SCR (WM_USER + 1)

//··········································································//
//    header :: Structures
//··········································································//

//··········································································//
//    header :: Class Defs
//··········································································//

//··········································································//
//    header :: Function Defs
//··········································································//

bool wnd_register_cls( const HINSTANCE & hInstance, const char * cls_name, const void * wnd_proc);
bool wnd_create_gl_wnd( const HINSTANCE & hInstance, const char * cls_name,
					    HWND & hWnd, HDC & hDC, HGLRC & hRC,
						const int width, const int height, const int bpp, bool & full_scr,
						const char * app_title, bool & is_visible);
void wnd_destroy_gl_wnd(HWND & hWnd, HDC & hDC, HGLRC & hRC,const bool & full_scr);

void wnd_post_quit(void);
void wnd_post_tgl_full_scr(void);

//UI data
bool wnd_set_key_press(int i, bool val);
bool wnd_get_key_press(int i);

void wnd_set_mouse_x(int x);
void wnd_set_mouse_y(int y);
int wnd_get_mouse_x(void);
int wnd_get_mouse_y(void);

//Accessors
void wnd_set_scr_width(int width);
void wnd_set_scr_height(int height);
int wnd_get_scr_width(void);
int wnd_get_scr_height(void);
int wnd_get_bpp(void);

//----------------------------------------------------------------------------
//win_wrap.h
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
#endif //__WIN_WRAP_H__