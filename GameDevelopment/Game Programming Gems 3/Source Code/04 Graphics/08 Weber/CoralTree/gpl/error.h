
/******************************************************************************

Coral Tree gpl/error.h
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/

#define GPL_MAX_PRINT_TITLE_LEN		200
#define GPL_MAX_PRINT_STRING_LEN	1000

void gplPrintWithLocation(long source,long type,char *message,char *file,long line);

#define gplPrint(source,type,string)	gplPrintWithLocation(source,type,string,__FILE__,__LINE__)

/* GPL_PRINT sources */
#define GPL_SOFTWARE	0	/* software related condition */
#define GPL_SYSTEM		1	/* system related condition */
#define GPL_USER		2	/* improper usage by user of module */
#define GPL_GENERAL		3	/* non-specific GPL (specific is preferred) */

/* GPL_PRINT types */
#define GPL_VLOG		0	/* verbose log updates */
#define GPL_WARN		1	/* possible problem */
#define GPL_ERROR		2	/* definite problem */
#define GPL_FATAL		3	/* fatal problem */


