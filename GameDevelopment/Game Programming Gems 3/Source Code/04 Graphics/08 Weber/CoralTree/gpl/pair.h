#ifndef GPL_PAIR_H
#define GPL_PAIR_H

/******************************************************************************

Coral Tree gpl/pair.h
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

/******************************************************************************
GPL_Pair

With credit to STL...
******************************************************************************/
template <class A, class B>
class GPL_Pair
{
	public:
		A first;
		B second;

		GPL_Pair(const A & a, const B & b) : first(a), second(b) {}
		GPL_Pair(const GPL_Pair<A, B> & p) : first(p.first), second(p.second) {}
		~GPL_Pair(void) {}

		GPL_Boolean operator==(const GPL_Pair<A, B> & pair) const
		{
			return first == pair.first && second == pair.second;
		}

	private:
		GPL_Pair(void);
};

#endif /* GPL_PAIR_H */

