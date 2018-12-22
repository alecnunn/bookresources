/*
   Surface clear interface for the HERMES library
   Copyright (c) 1998 Christian Nentwich (brn@eleet.mcb.at)
   This source code is licensed under the GNU LGPL
  
   Please refer to the file COPYING contained in the distribution for
   licensing conditions
*/

#ifndef __HERMES_CLEAR__
#define __HERMES_CLEAR__


#include "H_Types.h"
#include "H_Format.h"
#include "H_Config.h"


/*
   Get a handle for a new clearer instance to work with. Returns 0 if failed.
*/
HERMES_C HermesHandle HERMES_API Hermes_ClearerInstance();


/*
   Return the clearer instance if it is no longer needed.
*/
HERMES_C void HERMES_API Hermes_ClearerReturn(HermesHandle handle);


/*
   Request the clearing routines to be set up for clearning to a specific
   format later. Repeated calls to the routine will be cached and terminate
   after a short check.
*/
HERMES_C int HERMES_API Hermes_ClearerRequest(HermesHandle handle,HermesFormat *format);


/*
   Clear a surface. pixels points to the pixel data, x1, y1, width, height
   specify the area to clear, pitch is the width of a scanline in bytes,
   the rest are the colour components.
*/ 
HERMES_C int HERMES_API Hermes_ClearerClear(HermesHandle handle,void *pixels,
		        int x1,int y1,int width,int height,int pitch,
		        hermes_int32 r,hermes_int32 g,hermes_int32 b,hermes_char8 index);


#endif
