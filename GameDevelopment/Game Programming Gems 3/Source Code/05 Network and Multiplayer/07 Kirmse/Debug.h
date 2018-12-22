/*
 * Debug
 *
 * Handles debug output
 *
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef DPRINTF
   #define dprintf app->debug->Printf
#else
   #define dprintf 1 ? 0 : app->debug->Printf
#endif

class Debug
{
public:

   Debug();
   ~Debug();

   void Printf(char *fmt, ...);

private:

#ifdef DPRINTF
   FILE *mpFile;
#endif   
};

// Cause a breakpoint
inline void Break() {_asm {int 3}; }
   

#endif
