
/******************************************************************************

Coral Tree gpl/ref.cc
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

GPL_Reference & GPL_Reference::operator=(const GPL_Reference & ref)
{
	Detach();
	Attach(ref);
	return *this;
}

GPL_Boolean GPL_Reference::IsValid(void) const
{
	if ((GPL_ReferenceRep *) GPL_BAD_ADDRESS == rep)
	{
		return FALSE;
	}
	return TRUE;
}

GPL_Boolean GPL_Reference::IsShared(void) const
{
	if ((GPL_ReferenceRep *) GPL_BAD_ADDRESS == rep)
	{
		return FALSE;
	}
	if (2 > rep->count)
	{
		return FALSE;
	}
	return TRUE;
}

void GPL_Reference::Attach(const GPL_Reference & ref)
{
	rep = ref.rep;
	if ((GPL_ReferenceRep *) 0 != ref.rep)
	{
		++rep->count;
	}
}

void GPL_Reference::Detach(void)
{
	if ((GPL_ReferenceRep *) 0 != rep)
	{
		if (0 == --rep->count)
		{
			delete rep;
		}
		rep = (GPL_ReferenceRep *) 0;
	}
}

void GPL_Reference::Privatize(void)
{
	// If this reference is sharing a representation...
	if ((GPL_ReferenceRep *) 0 != rep && 1 < rep->count)
	{
		register GPL_ReferenceRep * newRep = new GPL_ReferenceRep(*rep);
		--rep->count;
		rep = newRep;
	}
}

