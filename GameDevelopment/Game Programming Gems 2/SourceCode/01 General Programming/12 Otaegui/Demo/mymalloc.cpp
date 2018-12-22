/****************************************************************************
 * mymalloc.cpp - malloc wrapping functions with logging
 *
 * (c) Copyright 2000 Sabarasa Entertainment
 * For internal use only.
 ****************************************************************************/

#include <malloc.h>
#include "fplog.h"

void *
mymalloc( size_t size, char * string )
{
	void * lpRet;

	lpRet = malloc( size );

	fplog( "+ %s %d %d\n", string, lpRet, size );

	return lpRet;
}

void
myfree( void *a, char * string )
{
	free( a );

	fplog( "- %s %d\n", string, a );
}