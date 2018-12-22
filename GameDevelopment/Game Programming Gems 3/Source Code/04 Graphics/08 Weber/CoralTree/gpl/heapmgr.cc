
/******************************************************************************

Coral Tree gpl/heapmgr.cc
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

#include <gpl/gpl.h>

/***********************************************************************
class GPL_HeapManager
***********************************************************************/
GPL_HeapManager::GPL_HeapManager(void) : useLocking(FALSE), sizes(HashFunction)
{
}

GPL_HeapManager::~GPL_HeapManager(void)
{
}

GPL_Boolean GPL_HeapManager::Reallocate(const unsigned long size,
										GPL_Address & address)
{
	sizes.Remove(address);
	address = (GPL_Address) osdRealloc(address, size);
	sizes.Insert(address, size);
	return GPL_BAD_ADDRESS != address;
}

GPL_Boolean GPL_HeapManager::Deallocate(GPL_Address address)
{
	if (GPL_BAD_ADDRESS != address)
	{
		osdFree(address);
	}
	sizes.Remove(address);
	return TRUE;
}

unsigned long GPL_HeapManager::GetSize(GPL_Address address)
{
	unsigned long size = 0;
	sizes.LookUp(address, size);
	return size;
}

// GPL_Object
GPL_OBJECT_IMPLEMENTATION(GPL_HeapManager, 0x40000003, "GPL_HeapManager");

