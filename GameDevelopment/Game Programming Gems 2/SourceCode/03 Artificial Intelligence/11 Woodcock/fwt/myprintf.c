/* Copyright (C) Steven Woodcock, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Woodcock, 2001"
 */
//*********************************************************************
// Name:     myprintf.c
// Purpose:  A simple debugging version of printf that dumps data
//           to a text file.
//*********************************************************************

//
// includes
//

#include <io.h>
#include <stdio.h>
#include <stdarg.h>

// globals

//extern char OutputFileName[];

int myprintf( char *str, ... ) {

   char buf[256];
   va_list vlist;

   static FILE *dbg;

   /* File section */

   if ( dbg == NULL ) {
      dbg = fopen("C:/windows/desktop/FWT.txt", "w" );
      if ( dbg == NULL ) return 0;
   }

   va_start( vlist, str );
   vsprintf( buf, str, vlist );
   va_end( vlist );

   fprintf(dbg, "%s", buf );
   fflush(dbg);

   return 0;
}