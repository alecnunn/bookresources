/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#ifndef OPENGLINTERFACE_H
#define OPENGLINTERFACE_H

class OpenGLInterface {
public:
	OpenGLInterface();
	virtual ~OpenGLInterface() { }
	
	int setVideoMode(int w,int h,int fs);
	int setTitle(const char *title);
	int setPointer(int x,int y);
	int showPointer(int show);
	int error();
	int main();
	int exit() { done = 1; return 0; }
	
	virtual int idle() { return 0; }
	virtual int render() { return 0; }
	
	virtual int buttonPress(int) { return 0; }
	virtual int buttonRelease(int) { return 0; }
	virtual int keyPress(int) { return 0; }
	virtual int keyRelease(int) { return 0; }
	
	enum {
		KEY_ESC = 256,
		KEY_TAB,
		KEY_RETURN,
		KEY_BACKSPACE,
		KEY_DELETE,
		KEY_HOME,
		KEY_END,
		KEY_PGUP,
		KEY_PGDOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_UP,
		KEY_DOWN,
		KEY_SHIFT,
		KEY_CTRL,
		KEY_RESET=120,
		KEY_THRESH=116,
		KEY_HELP=104,
		KEY_REC=114,
		KEY_PAUSE=32
	};
	
	enum {
		BUTTON_LEFT = 1 << 0,
		BUTTON_MIDDLE = 1 << 1,
		BUTTON_RIGHT = 1 << 2,
		BUTTON_UP = 1 << 3,
		BUTTON_DOWN = 1 << 4
	};
	
	int done;
	int keys[512];
	
	int windowWidth;
	int windowHeight;
	int fullScreen;
	
	int mouseX;
	int mouseY;
	int mouseButton;
	
	float fps;
	float ifps;
};

#endif 
