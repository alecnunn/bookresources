/*
   Pixel format conversion interface for the HERMES library
   Copyright (c) 1998 Christian Nentwich (brn@eleet.mcb.at)
   This source code is licensed under the GNU LGPL
  
   Please refer to the file COPYING contained in the distribution for
   licensing conditions
*/

#ifndef __HERMES_CONVERTER_H__
#define __HERMES_CONVERTER_H__

#include "H_Types.h"
#include "H_Handle.h"
#include "H_Format.h"
#include "H_Config.h"


/* Flags for the general public */

#define HERMES_CONVERT_NORMAL 0
#define HERMES_CONVERT_DITHER 1 


/*
   Get a converter to work with, specifying a combination of the flags
   above. Returns 0 if unsuccessful.
*/
HERMES_C HermesHandle HERMES_API Hermes_ConverterInstance(unsigned long flags);


/*
   Return a converter if it is not needed anymore, thus releasing some 
   memory.
*/
HERMES_C void HERMES_API Hermes_ConverterReturn(HermesHandle handle);


/*
   Request a format conversion between two formats. This function returns 0
   if the conversion cannot be provided (which should not occur too often :)
   Repeated calls to this function will be cached an terminate almost 
   immediately, so don't be ashamed of calling it often.
*/
HERMES_C int HERMES_API Hermes_ConverterRequest(HermesHandle handle,HermesFormat *source,
			    HermesFormat *dest);


/*
   Set the palette of the source surface / destination surface for a 
   subsequent conversion. At the moment, only sourcepal is used.
   Returns 0 if unsuccessful (invalid handle!).
*/
HERMES_C int HERMES_API Hermes_ConverterPalette(HermesHandle handle,
			    HermesHandle sourcepal,
			    HermesHandle destpal);


/*
   Do a format conversion after calling the setup routines above. This will
   convert (or copy) the pixel data from s_pixels to the data in d_pixels.
   Both source and destination areas/origins can be specified as well as
   the scanline width in bytes of the source/destination.
   Returns 0 if unsuccessful (invalid handle or request not called before).
*/
HERMES_C int HERMES_API Hermes_ConverterCopy(HermesHandle handle,void *s_pixels,
			 int s_x,int s_y,int s_width,int s_height,int s_pitch,
			 void *d_pixels,int d_x,int d_y,int d_width,
			 int d_height,int d_pitch);


#endif
