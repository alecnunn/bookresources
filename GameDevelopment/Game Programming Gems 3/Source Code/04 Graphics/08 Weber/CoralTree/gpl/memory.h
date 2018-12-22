#ifndef GPL_MEMORY_H
#define GPL_MEMORY_H

/******************************************************************************

Coral Tree gpl/memory.h
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

#include <gpl/typedefs.h>
#include <gpl/object.h>

#define TEMP_DEWARN { 							\
						GPL_Opaque k;			\
						k=key;					\
						k = k;					\
						address=address;		\
						unsigned long s;		\
						s=size;					\
						s=s;					\
					}

/***********************************************************************
class GPL_Memory

Abstract representation of memory.
***********************************************************************/
class GPL_Memory : public GPL_Object
{
	public:
		GPL_Memory(void) {}
		virtual ~GPL_Memory(void) {}

		virtual unsigned long GetSize(void) const { return 0; }

		virtual GPL_Boolean Resize(const unsigned long size) 
		{
			unsigned long s = size;s = s;
			return FALSE;
		}

		virtual void Initialize(void) {}
		virtual void Finalize(void) {}

		virtual GPL_Boolean StartRead(const GPL_Opaque key,
										GPL_Address address,
										const unsigned long size)
		{
			TEMP_DEWARN;
			return TRUE;
		}
		virtual GPL_Boolean StopRead(const GPL_Opaque key)
		{
			GPL_Opaque k;
			k = key;
			k = k;
			return TRUE;
		}
		virtual GPL_Boolean StartWait(const GPL_Opaque key,
										GPL_Address address,
										const unsigned long size)
		{
			TEMP_DEWARN;
			return TRUE;
		}
		virtual GPL_Boolean StopWait(const GPL_Opaque key)
		{
			GPL_Opaque k;
			k = key;
			k = k;
			return TRUE;
		}
		virtual GPL_Boolean StartWrite(const GPL_Opaque key,
										GPL_Address address,
											const unsigned long size)
		{
			TEMP_DEWARN;
			return TRUE;
		}
		virtual GPL_Boolean StopWrite(const GPL_Opaque key,
										GPL_Address address,
											const unsigned long size)
		{
			TEMP_DEWARN;
			return TRUE;
		}

		// GPL_Object
		GPL_OBJECT_ABSTRACT_INTERFACE(GPL_Memory);
};
#undef TEMP_DEWARN

/***********************************************************************
class GPL_MemoryManager

Responsible for implementing a memory management policy.
***********************************************************************/
class GPL_MemoryManager : public GPL_Object
{
	public:
		GPL_MemoryManager(void) {}
		virtual ~GPL_MemoryManager(void) {}

		virtual unsigned long GetSize(GPL_Address address)
		{
			address = address;
			return 0;
		}

		virtual void Initialize(void) {}
		virtual void Finalize(void) {}
		virtual void Defragment(void) {}

		virtual GPL_Boolean Reallocate(const unsigned long size,
										GPL_Address & address)
		{
			unsigned long s = size;s = s;
			GPL_Address a = address;a = a;
			return FALSE;
		}
		virtual GPL_Boolean Deallocate(GPL_Address address)
		{
			address = address;
			return FALSE;
		}

		virtual GPL_Boolean StartRead(GPL_Address & address,
										const unsigned long size)
		{
			address = address;
			unsigned long s;
			s = size;
			s = s;
			return TRUE;
		}
		virtual GPL_Boolean StopRead(GPL_Address address)
		{
			address = address;
			return TRUE;
		}
		virtual GPL_Boolean StartWait(GPL_Address & address,
										const unsigned long size)
		{
			address = address;
			unsigned long s;
			s = size;
			s = s;
			return TRUE;
		}
		virtual GPL_Boolean StopWait(GPL_Address address)
		{
			address = address;
			return TRUE;
		}
		virtual GPL_Boolean StartWrite(GPL_Address & address,
										const unsigned long size)
		{
			address = address;
			unsigned long s;
			s = size;
			s = s;
			return TRUE;
		}
		virtual GPL_Boolean StopWrite(GPL_Address address,
										const unsigned long size)
		{
			address = address;
			unsigned long s;
			s = size;
			s = s;
			return TRUE;
		}

		// GPL_Object
		GPL_OBJECT_ABSTRACT_INTERFACE(GPL_MemoryManager);

	protected:
};

#endif /* GPL_MEMORY_H */
