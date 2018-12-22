#ifndef META_PLATFORM_H 
#define META_PLATFORM_H 
#include <meta/options.h>

#ifdef WIN32

#ifndef OSD_OSTYPE 
#define OSD_OSTYPE    OSD_WIN32 
#endif 
#ifndef OSD_OS 
#define OSD_OS        OSD_WINNT 
#endif 
#ifndef OSD_OSVER 
#define OSD_OSVER     4 
#endif 
#ifndef OSD_COMPILER 
#define OSD_COMPILER  OSD_VISUAL 
#endif 
#ifndef OSD_HARDWARE 
#define OSD_HARDWARE  OSD_X86 
#endif 
#ifndef OSD_THREADS 
#define OSD_THREADS   OSD_NO_THREADS 
#endif 
#ifndef EW_WL 
#define EW_WL         EW_WIN32 
#endif 
#ifndef EW_GL 
#define EW_GL         EW_OPENGL 
#endif 
#ifndef CORAL_MAJOR 
#define CORAL_MAJOR           1 
#endif 
#ifndef CORAL_MINOR 
#define CORAL_MINOR           4 
#endif 
#ifndef CORAL_PATCH 
#define CORAL_PATCH           1 
#endif 
#ifndef CORAL_AVAILABILITY 
#define CORAL_AVAILABILITY    0 
#endif 
#ifndef CORAL_CODENAME 
#define CORAL_CODENAME        "jpweber@pestilence" 
#endif 

#else

#ifndef OSD_OSTYPE
#define OSD_OSTYPE    OSD_UNIX
#endif
#ifndef OSD_OS
#define OSD_OS        OSD_LINUX
#endif
#ifndef OSD_OSVER
#define OSD_OSVER     2
#endif
#ifndef OSD_COMPILER
#define OSD_COMPILER  OSD_GNU
#endif
#ifndef OSD_HARDWARE
#define OSD_HARDWARE  OSD_X86
#endif
#ifndef OSD_THREADS
#define OSD_THREADS   OSD_NO_THREADS
#endif
#ifndef EW_WL
#define EW_WL         EW_XWIN
#endif
#ifndef EW_GL
#define EW_GL         EW_OPENGL
#endif
#ifndef CORAL_MAJOR
#define CORAL_MAJOR           1
#endif
#ifndef CORAL_MINOR
#define CORAL_MINOR           4
#endif
#ifndef CORAL_PATCH
#define CORAL_PATCH           1
#endif
#ifndef CORAL_AVAILABILITY
#define CORAL_AVAILABILITY    0
#endif
#ifndef CORAL_CODENAME
#define CORAL_CODENAME        "jpweber@debacle"
#endif
#endif /* META_PLATFORM_H */

#endif
