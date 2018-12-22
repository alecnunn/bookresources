/*
   Palette handling interface for the HERMES library
   Copyright (c) 1998 Christian Nentwich (brn@eleet.mcb.at)
   This source code is licensed under the GNU LGPL
  
   Please refer to the file COPYING contained in the distribution for
   licensing conditions
*/

#ifndef __HERMES_PALETTE__
#define __HERMES_PALETTE__


#include "H_Types.h"
#include "H_Config.h"


/* Get a handle for a palette to work with. This allocates memory for an
   internal palette. Returns 0 if failed.
*/
HERMES_C HermesHandle HERMES_API Hermes_PaletteInstance();


/*
   Return a handle for a palette if the palette isn't used anymore. The 
   internal palette will be deallocated.
*/
HERMES_C void HERMES_API Hermes_PaletteReturn(HermesHandle handle);


/*
   Copy the contents of the palette parameter provided into the internal
   palette. The user palette has to be 256*4 bytes long.
*/
HERMES_C void HERMES_API Hermes_PaletteSet(HermesHandle handle,hermes_int32 *palette);


/*
   Return the pointer to the internal palette. The palette is 256*4 bytes
   long.
*/
HERMES_C hermes_int32* HERMES_API Hermes_PaletteGet(HermesHandle handle);


/*
   Force invalidation of the palette cache. This will force lookup tables to
   be regenerated and has to be done manually after PaletteGet has been used
   and the data has been modified without the knowledge of Hermes.
*/
HERMES_C void HERMES_API Hermes_PaletteInvalidateCache(HermesHandle handle);


#endif
