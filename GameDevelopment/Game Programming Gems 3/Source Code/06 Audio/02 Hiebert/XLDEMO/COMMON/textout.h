/****************************************************************************

   FILE: textout.h

   DESCRIPTION: header for OpenGL text output class

****************************************************************************/

#if !defined (GLTEXTOUT_CLASS)
#define GLTEXTOUT_CLASS

#include "switches.h"
#include "xlheader.h"

class GLTextOut
{
private: 
   GLuint fontOffset;
   void MakeRasterFont(void);
public:
   GLTextOut ();
   ~GLTextOut ();
   void Init (void);
   void PrintString(char *s);
};

#endif // !defined (TEXTOUT_CLASS)	

