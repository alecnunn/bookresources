#ifndef GPL_TYPEDEFS_H
#define GPL_TYPEDEFS_H

/******************************************************************************

Coral Tree gpl/typedefs.h
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

#include <osd/osd.h>

typedef	long	GPL_Boolean;

typedef uint8	GPL_Byte;
typedef	char8	GPL_Character;
typedef	int32	GPL_Integer;
typedef	real32	GPL_Real;
typedef	uint32	GPL_Opaque;

typedef	void *	GPL_Address;
#define	GPL_BAD_ADDRESS	((void *) 0)

typedef	uint32	GPL_ClassId;

#endif /* GPL_TYPEDEFS_H */
