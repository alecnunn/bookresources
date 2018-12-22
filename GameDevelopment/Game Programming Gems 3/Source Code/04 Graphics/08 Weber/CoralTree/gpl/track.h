
/******************************************************************************

Coral Tree gpl/track.h
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

class GPL_Track
{
	private:
		static GPL_DoubleList<GPL_Track>		gplTrack;
	public:
		GPL_Track();
		virtual ~GPL_Track();
};


