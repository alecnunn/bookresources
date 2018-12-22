/*
   Type definitions for the HERMES library
   Copyright (c) 1998 Christian Nentwich (brn@eleet.mcb.at)
   This source code is licensed under the GNU LGPL
  
   Please refer to the file COPYING contained in the distribution for
   licensing conditions
*/

#ifndef __HERMES_TYPES__
#define __HERMES_TYPES__

#if (_MSC_VER>=1000) || defined(__VISUALC__)

typedef unsigned __int32 hermes_int32;
typedef unsigned __int16 hermes_short16;
typedef unsigned __int8 hermes_char8;

#else

#if !defined (__BEOS__) || !defined(_SUPPORT_DEFS_H)
typedef unsigned int hermes_int32;
#endif /* BEOS */

typedef unsigned short hermes_short16;
typedef unsigned char hermes_char8;

#endif /* VISUALC */

#endif
