#ifndef GPL_HEAPMGR_H
#define GPL_HEAPMGR_H

/******************************************************************************

Coral Tree gpl/heapmgr.h
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

#include <gpl/map.h>
#include <gpl/memory.h>

/***********************************************************************
class GPL_HeapManager

Responsible for implementing a memory management policy for the heap.
Provides size information for each address.
***********************************************************************/
class GPL_HeapManager : public GPL_MemoryManager
{
	public:
		GPL_HeapManager(void);
		virtual ~GPL_HeapManager(void);

		virtual GPL_Boolean Reallocate(const unsigned long size,
										GPL_Address & address);
		virtual GPL_Boolean Deallocate(GPL_Address address);

		virtual unsigned long GetSize(GPL_Address address);

		// GPL_Object
		GPL_OBJECT_INTERFACE(GPL_HeapManager);

	protected:
		GPL_Boolean useLocking;
		GPL_Map<GPL_Address, unsigned long> sizes;

		static unsigned long HashFunction (const GPL_Address & a) { return (unsigned long) a; }
};

#endif /* GPL_HEAPMGR_H */
