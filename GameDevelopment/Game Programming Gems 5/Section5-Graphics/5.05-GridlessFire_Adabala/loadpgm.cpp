/***********************************************
                                                                                
Demo for chapter "Gridless Controllable Fire"
in Games Programming Gems 5.
                                                                                
Author: Neeharika Adabala
                                                                                
Date: August 2004
                                                                                
************************************************/

/*************************************************
Function for loading vertex and fragment programs
**************************************************/

#include<glh/glh_genext.h>
#include "loadpgm.h"

GLuint load_program(const char *name,GLuint target) {
	FILE *file = fopen(name,"r");
	if(!file) {
		fprintf(stderr,"error open file \"%s\"",name);
		return 0;
	}
	fseek(file,0,SEEK_END);
	int size = ftell(file);
	fseek(file,0,SEEK_SET);
	char *data = new char[size + 1];
	fread(data,1,size,file);
	data[size] = '\0';
	fclose(file);
	GLuint program;
	glGenProgramsARB(1,&program);
	glBindProgramARB(target,program);
	glProgramStringARB(target,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(data),data);
	char *error = (char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
	if(error && *error) fprintf(stderr,"%s in \"%s\" file\n",error,name);
	delete data;
	return program;
}
