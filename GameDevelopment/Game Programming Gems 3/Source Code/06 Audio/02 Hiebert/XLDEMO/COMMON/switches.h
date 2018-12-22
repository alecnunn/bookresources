/****************************************************************************

   FILE: switches.h

	DESCRIPTION: header for xldemo switches

****************************************************************************/

// OS-selection switches
#ifdef _WIN32
# define WINDOWS
#endif
#ifdef __MACOS__
# define MACOS
#endif
#ifdef unix
# define LINUX
#endif

// general switches
#define BLENDING                   // turn on GL blending?
#define LIGHTING
//#define FOG                        // turn on GL fog?
