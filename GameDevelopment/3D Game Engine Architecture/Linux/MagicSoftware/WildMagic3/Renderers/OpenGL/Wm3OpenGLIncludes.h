// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3OPENGLINCLUDES_H
#define WM3OPENGLINCLUDES_H

// GLUT for Linux, Microsoft Windows, or other platforms supporting GLUT
#if defined(WM3_USE_GLUT)

#ifdef __APPLE__
#include <OpenGL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

// Microsoft Windows
#elif defined(WM3_USE_WGL)

#include <GL/glew.h>
#include <windows.h>

#else
#error WM3_USE_GLUT or WM3_USE_WGL must be defined.
#endif

#endif
