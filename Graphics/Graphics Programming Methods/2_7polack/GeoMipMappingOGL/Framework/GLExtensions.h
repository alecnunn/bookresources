//==============================================================
//= GLExtensions.h
//= Original coder: Trent Polack (trent@codershq.com)
//==============================================================
//= An OpenGL extension managing system
//==============================================================
#ifndef __GLExtensions_GLExtensions_H__
#define __GLExtensions_GLExtensions_H__

#include <SDL_opengl.h>

#ifndef __GL_TEX_MODULATORS__
#define __GL_TEX_MODULATORS__
	#define GL_RGB_SCALE_ARB 0x8573
	#define GL_COMBINE_ARB	 0x8570
#endif

namespace GLExtensions {
	extern const char *supportedExtensions;

	//compiled vertex array members
	extern PFNGLLOCKARRAYSEXTPROC   glLockArraysEXT;
	extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;
	extern bool canCVA;

	//multitexturing members
	extern PFNGLACTIVETEXTUREARBPROC	   glActiveTextureARB;
	extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
	extern PFNGLMULTITEXCOORD1FARBPROC	   glMultiTexCoord1fARB;
	extern PFNGLMULTITEXCOORD1DARBPROC     glMultiTexCoord1dARB;
	extern PFNGLMULTITEXCOORD2FARBPROC	   glMultiTexCoord2fARB;
	extern PFNGLMULTITEXCOORD2DARBPROC     glMultiTexCoord2dARB;
	extern PFNGLMULTITEXCOORD3FARBPROC	   glMultiTexCoord3fARB;
	extern PFNGLMULTITEXCOORD3DARBPROC     glMultiTexCoord3dARB;
	extern int numTexUnits;

	bool SetupCVA( void );
	bool SetupMultitexturing( void );

	void SetupExtensionSystem( void );
	bool CheckExtension( const char *extName );
}


#endif	//__GLExtensions_GLExtensions_H__