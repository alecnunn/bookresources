//==============================================================
//= GLExtensions.cpp
//= Original coder: Trent Polack (trent@codershq.com)
//==============================================================
//= An OpenGL extension managing system
//==============================================================
#include <SDL_opengl.h>

#include "glext.h"

#include "GLExtensions.h"
#include "Log.h"

const char *GLExtensions::supportedExtensions;

PFNGLLOCKARRAYSEXTPROC GLExtensions::glLockArraysEXT;
PFNGLUNLOCKARRAYSEXTPROC GLExtensions::glUnlockArraysEXT;
bool GLExtensions::canCVA;

PFNGLACTIVETEXTUREARBPROC GLExtensions::glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC GLExtensions::glClientActiveTextureARB;
PFNGLMULTITEXCOORD1FARBPROC	GLExtensions::glMultiTexCoord1fARB;
PFNGLMULTITEXCOORD1DARBPROC GLExtensions::glMultiTexCoord1dARB;
PFNGLMULTITEXCOORD2FARBPROC	GLExtensions::glMultiTexCoord2fARB;
PFNGLMULTITEXCOORD2DARBPROC GLExtensions::glMultiTexCoord2dARB;
PFNGLMULTITEXCOORD3FARBPROC	GLExtensions::glMultiTexCoord3fARB;
PFNGLMULTITEXCOORD3DARBPROC GLExtensions::glMultiTexCoord3dARB;
int GLExtensions::numTexUnits;

//setup the OpenGL extension system
void GLExtensions::SetupExtensionSystem( void ) {
	supportedExtensions= ( const char * )glGetString( GL_EXTENSIONS );
}

//check support for a certain OpenGL extension
bool GLExtensions::CheckExtension( const char *extName ) {
	unsigned int nextExt;
	char *supExt= ( char * )supportedExtensions;
	char *endExt;

	//find the end of the extension list
	endExt= supExt + strlen( supportedExtensions );

	//search through the entire list
	while( supExt < endExt ) {
		//find the next extension in the list
		nextExt= strcspn( supExt, " " );

		//check the extension to the one given in the argument list
		if( ( strlen( extName ) == nextExt ) && 
			( strncmp( extName, supExt, nextExt ) == 0 ) ) {
			//the extension is supported
			g_log.Write( DemoFramework::LOG_RENDERER, "Your video card supports extension: %s", extName );
			return true;
		}

		//move to the nexte extension in the list
		supExt+= ( nextExt + 1 );
	}

	//the extension is not supported
	g_log.Write( DemoFramework::LOG_RENDERER, "Your video card does not support extension: %s", extName );
	return false;
}

//setup compiled vertex arrays
bool GLExtensions::SetupCVA( void ) {
	CheckExtension( "GL_EXT_compiled_vertex_array" ) ? canCVA= true : canCVA= false;

	if( !canCVA ) return false;

	glLockArraysEXT= ( PFNGLLOCKARRAYSEXTPROC )wglGetProcAddress( "glLockArraysEXT" );
	glUnlockArraysEXT= ( PFNGLUNLOCKARRAYSEXTPROC )wglGetProcAddress( "glUnlockArraysEXT" );

	return true;
}

//setup multitexturing
bool GLExtensions::SetupMultitexturing( void ) {
	if( CheckExtension( "GL_ARB_multitexture" ) ) {
		glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &numTexUnits );
		if( !numTexUnits )	return false;
	}

	glActiveTextureARB= ( PFNGLACTIVETEXTUREARBPROC )wglGetProcAddress( "glActiveTextureARB" );
	glClientActiveTextureARB= ( PFNGLCLIENTACTIVETEXTUREARBPROC )wglGetProcAddress( "glClientActiveTextureARB" );
	glMultiTexCoord1fARB= ( PFNGLMULTITEXCOORD1FARBPROC )wglGetProcAddress( "glMultiTexCoord1fARB" );
	glMultiTexCoord1dARB= ( PFNGLMULTITEXCOORD1DARBPROC )wglGetProcAddress( "glMultiTexCoord1dARB" );
	glMultiTexCoord2fARB= ( PFNGLMULTITEXCOORD2FARBPROC )wglGetProcAddress( "glMultiTexCoord2fARB" );
	glMultiTexCoord2dARB= ( PFNGLMULTITEXCOORD2DARBPROC )wglGetProcAddress( "glMultiTexCoord2dARB" );
	glMultiTexCoord3fARB= ( PFNGLMULTITEXCOORD3FARBPROC )wglGetProcAddress( "glMultiTexCoord3fARB" );
	glMultiTexCoord3dARB= ( PFNGLMULTITEXCOORD3DARBPROC )wglGetProcAddress( "glMultiTexCoord3dARB" );

	g_log.Write( DemoFramework::LOG_RENDERER, "Your video card supports %d texture units", numTexUnits );
	return true;
}
