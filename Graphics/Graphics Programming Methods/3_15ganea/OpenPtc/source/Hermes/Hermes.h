/*
   Main include file for the HERMES library
   Copyright (c) 1998 Christian Nentwich (brn@eleet.mcb.at)
   This source code is licensed under the GNU LGPL
  
   Please refer to the file COPYING contained in the distribution for
   licensing conditions
*/

#ifndef __HERMES_H__
#define __HERMES_H__


#include "H_Conv.h"
#include "H_Pal.h"
#include "H_Clear.h"
#include "H_Config.h"

/* Initialise Hermes, returns 0 if failed */
HERMES_C int HERMES_API Hermes_Init();


/* Deinitialise Hermes, returns 0 if failed */
HERMES_C int HERMES_API Hermes_Done();


#endif


