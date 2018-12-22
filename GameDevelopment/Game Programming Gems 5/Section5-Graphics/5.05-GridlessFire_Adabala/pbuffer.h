/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

#ifndef PBUFFER_H
#define PBUFFER_H

struct PBuffer_data;

class PBuffer {
public:
	PBuffer(int width,int height,int float_buffer = 0);
	~PBuffer();
	
	int enable();
	int disable();
	
	int width;
	int height;
	
protected:
	
	struct PBuffer_data *data;
	
};

#endif 
