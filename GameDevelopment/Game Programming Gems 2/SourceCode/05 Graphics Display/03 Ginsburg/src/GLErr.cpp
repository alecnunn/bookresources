// *********************************************************************************
// *                                                                               *
// *                                                                               *
// *    (C) Copyright 1997, 1998  ATI Research, Inc.    All rights reserved        *
// *                                                                               *
// *                                                                               *
// *********************************************************************************

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <string.h>
#include "GLErr.h"
char                gszErrMsg[100];          // Global error message text

char				glErrCodeString[6][25] = {

						"GL_INVALID_ENUM",     
						"GL_INVALID_VALUE",    
						"GL_INVALID_OPERATION",
						"GL_STACK_OVERFLOW",   
						"GL_STACK_UNDERFLOW",  
						"GL_OUT_OF_MEMORY"
					};


void ReportGLError(char *functionName, GLenum errCode)
{

	char	errString[100];
	GLenum  localErrCode;

	sprintf(errString, "\n%s returned ", functionName);

	strcat(errString, &glErrCodeString[errCode - GL_INVALID_ENUM][0]);

	strcat(errString, "\n");

	//OutputDebugString(errString);

	while(GL_NO_ERROR != (localErrCode = glGetError()))
	{

		sprintf(errString, "%s returned ", functionName);

		strcat(errString, &glErrCodeString[localErrCode - GL_INVALID_ENUM][0]);

		strcat(errString, "\n");

		//OutputDebugString(errString);
	}

	//OutputDebugString("\n");
}