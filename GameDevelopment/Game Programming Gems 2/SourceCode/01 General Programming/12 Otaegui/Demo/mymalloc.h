/****************************************************************************
 * mymalloc.h - malloc wrapping functions con logging
 *
 * Funciones de uso general
 * 
 * (c) Copyright 2000 Sabarasa Entertainment
 * For internal use only.
 ****************************************************************************/

#ifndef MYMALLOC_H_
#define MYMALLOC_H_

#include "malloc.h"

#define		free(x)		myfree(x)

void * mymalloc( size_t size, char *string = NULL );

void myfree( void *a, char *string = NULL );

#endif