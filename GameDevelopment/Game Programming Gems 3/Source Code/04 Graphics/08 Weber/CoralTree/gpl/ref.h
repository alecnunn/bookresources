#ifndef GPL_REF_H
#define GPL_REF_H

/******************************************************************************

Coral Tree gpl/ref.h
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

// This macro simplifies the syntax of up-casting from pointers
// to base classes.
#define	GPL_UPCAST(t, d, b)	register t *d = (t *) b;

// This macro simplifies ensuring that representation exists
// for a given object.
#define	GPL_ENSURE(t, d)	if (!GPL_Reference::IsValid()) { rep = new t(); } register t *d = (t *) rep;

/***********************************************************************
class GPL_ReferenceRep

GPL_Reference::Rep has been replaced by GPL_ReferenceRep.  Nested classes
are not lowest common denominator enough for portable code.
***********************************************************************/
class GPL_ReferenceRep
{
	public:
		unsigned long count;
		GPL_ReferenceRep(const unsigned long c = 1) : count(c) {}
		virtual ~GPL_ReferenceRep(void) {}
};

/***********************************************************************
class GPL_Reference

Base class for providing reference-counting.  Sub-classes are
responsible for extending GPL_Reference::Rep, particularly the
default and copy constructors and the destructor.
***********************************************************************/
class GPL_Reference
{
	public:
		GPL_Reference(void) : rep((GPL_ReferenceRep *) GPL_BAD_ADDRESS) {}
		GPL_Reference(const GPL_Reference & ref) { Attach(ref); }
		virtual ~GPL_Reference(void) { Detach(); }

		GPL_Reference & operator=(const GPL_Reference & ref);

		virtual GPL_Boolean IsValid(void) const;
		virtual GPL_Boolean IsShared(void) const;

	protected:
		GPL_ReferenceRep * rep;

		virtual void Attach(const GPL_Reference & ref);
		virtual void Detach(void);
		virtual void Privatize(void);
};

#endif /* GPL_REF_H */
