#ifndef GPL_TUPLE_H
#define GPL_TUPLE_H

/******************************************************************************

Coral Tree gpl/tuple.h
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

#define GPL_HIT					1
#define GPL_TMISS				2
#define GPL_CMISS				3

/***********************************************************************
class GPL_Tuple
***********************************************************************/
class GPL_Tuple : public GPL_Description
{
	public:
		GPL_Tuple(void);
virtual ~GPL_Tuple(void);
		GPL_Boolean	CheckMatch(char *fmt, ...);
		GPL_Boolean	VCheckMatch(char *fmt, va_list *ap) const;
		GPL_Boolean	IsEmpty(void);
		GPL_Boolean	Get(char *fmt, ...);

	private:
};

#endif /* GPL_TUPLE_H */
