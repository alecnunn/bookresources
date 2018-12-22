/****************************************************************************

   FILE: xlheader.h

	DESCRIPTION: loads OS-specific xL headers

****************************************************************************/

#ifdef WINDOWS
#include <windows.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <alut.h>
#include <strstrea.h>
#endif

#ifdef MACOS
#include <Windows.h>
#include <ToolUtils.h>
# include "gl.h"
#include "glu.h"
#include "glut.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <strstream.h>
#include "alut.h"
#endif

#ifdef LINUX
#include "GL/glut.h"
#include <stdio.h>
#include <math.h>
#include <AL/al.h>
#include <AL/alut.h>
#include <strstream.h>
#endif
