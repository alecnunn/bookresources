/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/


/********************************************************
File of standard bare minimum support of using X11
*********************************************************/
#include <iostream>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/xf86vmode.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include "glinterface.h"

static Display *display;
static int screen;
static Window window,rootWindow;
static Atom WM_DELETE_WINDOW;
static XF86VidModeModeInfo **modes;
static GLXContext glxContext;

OpenGLInterface::OpenGLInterface() {
	memset(this,0,sizeof(OpenGLInterface));
	display = XOpenDisplay(NULL);
	if(!display) {
		std::cerr << "couldn`t open display\n";
		done = 1;
	}
}

/*
 */
static int modescmp(const void *pa,const void *pb) {
	XF86VidModeModeInfo *a = *(XF86VidModeModeInfo**)pa;
	XF86VidModeModeInfo *b = *(XF86VidModeModeInfo**)pb;
	if(a->hdisplay > b->hdisplay) return -1;
	return b->vdisplay - a->vdisplay;
}

/*
 */
int OpenGLInterface::setVideoMode(int w,int h,int fs) {
	
	if(done || screen) return 0;
	
	windowWidth = w;
	windowHeight = h;
	fullScreen = fs;
	
	int attrib[] = {
		GLX_RGBA,
		GLX_RED_SIZE,8,
		GLX_GREEN_SIZE,8,
		GLX_BLUE_SIZE,8,
		GLX_ALPHA_SIZE,8,
		GLX_STENCIL_SIZE,8,
		GLX_DEPTH_SIZE,24,
		GLX_DOUBLEBUFFER,
		None
	};
	screen = DefaultScreen(display);
	rootWindow = RootWindow(display,screen);
	XVisualInfo *visualInfo = glXChooseVisual(display,screen,attrib);
	if(!visualInfo) {
		fprintf(stderr,"couldn`t get an RGBA, double-buffered visual\n");
		done = 1;
		return -1;
	}
	
	if(fullScreen) {
		int i,nmodes;
		XF86VidModeModeLine mode;
		if(XF86VidModeGetModeLine(display,screen,&nmodes,&mode) && XF86VidModeGetAllModeLines(display,screen,&nmodes,&modes)) {
			qsort(modes,nmodes,sizeof(XF86VidModeModeInfo*),modescmp);
			for(i = nmodes - 1; i > 0; i--) if (modes[i]->hdisplay >= windowWidth && modes[i]->vdisplay >= windowHeight) break;
			if(modes[i]->hdisplay != mode.hdisplay || modes[i]->vdisplay != mode.vdisplay) XF86VidModeSwitchToMode(display,screen,modes[i]);
			XF86VidModeSetViewPort(display,screen,0,0);
		} else fullScreen = 0;
	}
		
	XSetWindowAttributes attr;
	unsigned long mask;
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(display,rootWindow,visualInfo->visual,AllocNone);
	attr.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask;
	if(fullScreen) {
		mask = CWBackPixel | CWColormap | CWOverrideRedirect | CWSaveUnder | CWBackingStore | CWEventMask;
		attr.override_redirect = True;
		attr.backing_store = NotUseful;
		attr.save_under = False;
	} else mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
	
	window = XCreateWindow(display,rootWindow,0,0,windowWidth,windowHeight,0,visualInfo->depth,InputOutput,visualInfo->visual,mask,&attr);
	XMapWindow(display,window);
	
	if(fullScreen) {
		XMoveWindow(display,window,0,0);
		XRaiseWindow(display,window);
		XWarpPointer(display,None,window,0,0,0,0,windowWidth / 2,windowHeight / 2);
		XFlush(display);
		XF86VidModeSetViewPort(display,screen,0,0);
		XGrabPointer(display,window,True,0,GrabModeAsync,GrabModeAsync,window,None,CurrentTime);
		XGrabKeyboard(display,window,True,GrabModeAsync,GrabModeAsync,CurrentTime);
	} else {
		WM_DELETE_WINDOW = XInternAtom(display,"WM_DELETE_WINDOW",False);
		XSetWMProtocols(display,window,&WM_DELETE_WINDOW,1);
	}
	XFlush(display);
	
	glxContext = glXCreateContext(display,visualInfo,NULL,True);
	if(!glxContext) {
		fprintf(stderr,"glXCreateContext failed\n");
		done = 1;
		return -1;
	}
	glXMakeCurrent(display,window,glxContext);
	
	return 0;
}

/*
 */
int OpenGLInterface::setTitle(const char *title) {
	XStoreName(display,window,title);
	XSetIconName(display,window,title);
	return 0;
}

/*
 */
int OpenGLInterface::setPointer(int x,int y) {
	XWarpPointer(display,None,window,0,0,0,0,x,y);
	XFlush(display);
	return 0;
}

/*
 */
int OpenGLInterface::showPointer(int show) {
	if(show) XDefineCursor(display,window,0);
	else {
		char data[] = { 0 };
		Cursor cursor;
		Pixmap blank;
		XColor dummy;
		blank = XCreateBitmapFromData(display,window,data,1,1);
		cursor = XCreatePixmapCursor(display,blank,blank,&dummy,&dummy,0,0);
		XDefineCursor(display,window,cursor);
		XFreePixmap(display,blank);
		XFreeCursor(display,cursor);
	}
	return 0;
}

/*
 */
int OpenGLInterface::error() {
	GLenum error;
	while((error = glGetError()) != GL_NO_ERROR) {
		fprintf(stderr,"OpenGL error: %s\n",gluErrorString(error));
	}
	return 0;
}

/*
 */
static int getTime() {
	struct timeval tval;
	struct timezone tzone;
	gettimeofday(&tval,&tzone);
	return tval.tv_sec * 1000 + tval.tv_usec / 1000;
}

/*
 */

static int translateKey(int key) {
	int ret;
	if(key & 0xff00) {
		switch(key) {
			case XK_Escape: ret = OpenGLInterface::KEY_ESC; break;
			case XK_Tab: ret = OpenGLInterface::KEY_TAB; break;
			case XK_Return: ret = OpenGLInterface::KEY_RETURN; break;
			case XK_BackSpace: ret = OpenGLInterface::KEY_BACKSPACE; break;
			case XK_Delete: ret = OpenGLInterface::KEY_DELETE; break;
			case XK_Home: ret = OpenGLInterface::KEY_HOME; break;
			case XK_End: ret = OpenGLInterface::KEY_END; break;
			case XK_Page_Up: ret = OpenGLInterface::KEY_PGUP; break;
			case XK_Page_Down: ret = OpenGLInterface::KEY_PGDOWN; break;
			case XK_Left: ret = OpenGLInterface::KEY_LEFT; break;
			case XK_Right: ret = OpenGLInterface::KEY_RIGHT; break;
			case XK_Up: ret = OpenGLInterface::KEY_UP; break;
			case XK_Down: ret = OpenGLInterface::KEY_DOWN; break;
			case XK_Shift_L: case XK_Shift_R: ret = OpenGLInterface::KEY_SHIFT; break;
			case XK_Control_L: case XK_Control_R: ret = OpenGLInterface::KEY_CTRL; break;
			default: ret = (key < 0x06ff) ? (key & 0x00ff) : 0;
		}
	} else {
		ret = key;
	}
	return ret;
}

/*
 */
int OpenGLInterface::main() {
	KeySym key;
	int startTime = getTime(),endTime = 0,counter = 0;
    fps = 60;
	while(!done) {
		while(XPending(display) > 0) {
			XEvent event;
			XNextEvent(display,&event);
			switch(event.type) {
				case ClientMessage:
					if(event.xclient.format == 32 && event.xclient.data.l[0] == (long)WM_DELETE_WINDOW) done = 1;
					break;
				case ConfigureNotify:
					windowWidth = event.xconfigure.width;
					windowHeight = event.xconfigure.height;
					glViewport(0,0,windowWidth,windowHeight);
					break;
				case KeyPress:
					XLookupString(&event.xkey,NULL,0,&key,NULL);
					key = translateKey(key);
					keys[key] = 1;
					keyPress(key);
					break;
				case KeyRelease:
					XLookupString(&event.xkey,NULL,0,&key,NULL);
					key = translateKey(key);
					keys[key] = 0;
					keyRelease(key);
					break;
				case MotionNotify:
					mouseX = event.xmotion.x;
					mouseY = event.xmotion.y;
					break;
				case ButtonPress:
					mouseButton |= 1 << ((event.xbutton.button - 1));
					buttonPress(1 << (event.xbutton.button - 1));
					break;
				case ButtonRelease:
					if(event.xbutton.button < 4) mouseButton &= ~(1 << (event.xbutton.button - 1));
					buttonRelease(1 << (event.xbutton.button - 1));
					break;
			}
		}
		
		if(counter++ == 10) {
			endTime = startTime;
			startTime = getTime(); 
			fps = counter * 1000.0 / (float)(startTime - endTime);
			counter = 0;
		}
		ifps = 1.0 / fps;
		
		idle();
		render();
		
		glXSwapBuffers(display,window);
		mouseButton &= ~(BUTTON_UP | BUTTON_DOWN);
	}
	
	glXDestroyContext(display,glxContext);
	XDestroyWindow(display,window);
	if(fullScreen) XF86VidModeSwitchToMode(display,screen,modes[0]);
	XCloseDisplay(display);
	
	return 0;
}
