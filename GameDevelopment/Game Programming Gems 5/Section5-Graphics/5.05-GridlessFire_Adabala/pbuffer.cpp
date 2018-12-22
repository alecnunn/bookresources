/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#define GLH_EXT_SINGLE_FILE
#include <glh/glh_extensions.h>
#include "pbuffer.h"

#define REQUIRED_EXTENSIONS "GLX_SGIX_pbuffer " \
                            "GLX_SGIX_fbconfig "


/*
 */
struct PBuffer_data {
	Display *display;
	GLXPbuffer pbuffer;
	GLXContext context;
	
	Display *old_display;
	GLXPbuffer old_pbuffer;
	GLXContext old_context;
};

/*
 */
PBuffer::PBuffer(int width,int height,int float_buffer) : width(width), height(height) {
	
	bool isLoaded;
	Display *display = glXGetCurrentDisplay();
	int screen = DefaultScreen(display);
	GLXContext old_context = glXGetCurrentContext();
	
       if(!glh_init_extensions(REQUIRED_EXTENSIONS))
          isLoaded = false;
        else
          isLoaded = true;

	int attrib[] = {
		GLX_RED_SIZE,8,
		GLX_GREEN_SIZE,8,
		GLX_BLUE_SIZE,8,
		GLX_ALPHA_SIZE,8,
		GLX_STENCIL_SIZE,8,
		GLX_DEPTH_SIZE,24,
		GLX_DRAWABLE_TYPE,GLX_PBUFFER_BIT,
		0
	};
	int attrib_float[] = {
		GLX_RED_SIZE,32,
		GLX_GREEN_SIZE,32,
		GLX_BLUE_SIZE,32,
		GLX_ALPHA_SIZE,32,
		GLX_STENCIL_SIZE,8,
		GLX_DEPTH_SIZE,24,
		GLX_FLOAT_COMPONENTS_NV,true,
		GLX_DRAWABLE_TYPE,GLX_PBUFFER_BIT,
		0
	};
	
	int count;
	GLXFBConfig *config;
	if(float_buffer) config = glXChooseFBConfigSGIX(display,screen,attrib_float,&count);
	else config = glXChooseFBConfigSGIX(display,screen,attrib,&count);	
	if(!config) {
		fprintf(stderr,"pbuffer: glXChooseFBConfigSGIX() failed\n");
		return;
	}
	
	int pattrib[] = {
		GLX_LARGEST_PBUFFER,true,
		GLX_PRESERVED_CONTENTS,true,
		0
	};

	GLXPbuffer pbuffer = glXCreateGLXPbufferSGIX(display,config[0],width,height,pattrib);
	if(!pbuffer) {
		fprintf(stderr,"pbuffer: glXCreateGLXPbufferSGIX() failed\n");
		return;
	}

	GLXContext context = glXCreateContextWithConfigSGIX(display,config[0],GLX_RGBA_TYPE,old_context,true);
	if(!context) {
		fprintf(stderr,"pbuffer: glXCreateContextWithConfigSGIX() failed\n");
		return;
	}
	
	data = new PBuffer_data;
	data->display = display;
	data->pbuffer = pbuffer;
	data->context = context;

	data->old_display = display;
	data->old_pbuffer = glXGetCurrentDrawable();
	data->old_context = old_context;
}

/*
 */
PBuffer::~PBuffer() {
	if(data->context) glXDestroyContext(data->display,data->context);
	if(data->pbuffer) glXDestroyGLXPbufferSGIX(data->display,data->pbuffer);
	delete data;
}

/*
 */
int PBuffer::enable() {
	if(!glXMakeCurrent(data->display,data->pbuffer,data->context)) {
		fprintf(stderr,"pbuffer: glXMakeCurrent() failed\n");
		return -1;
	}
	return 0;
}

/*
 */
int PBuffer::disable() {
	if(!glXMakeCurrent(data->old_display,data->old_pbuffer,data->old_context)) {
		fprintf(stderr,"pbuffer: glXMakeCurrent() failed\n");
		return -1;
	}
	return 0;
}
