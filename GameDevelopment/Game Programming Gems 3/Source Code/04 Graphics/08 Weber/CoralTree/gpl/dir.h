#ifndef GPL_DIR_H
#define GPL_DIR_H

/******************************************************************************

Coral Tree gpl/dir.h
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

#include <gpl/file.h>

/***********************************************************************
class GPL_Directory

***********************************************************************/
class GPL_Directory {

public:
	GPL_Directory (void) {}
	GPL_Directory (const GPL_String & n) : name (n) {}
	virtual ~GPL_Directory (void) {}

	const GPL_String & GetName (void) const { return name; }

	void SetName (const GPL_String & n) { name = n; }

	virtual GPL_Boolean GetNames (GPL_Array<GPL_String> & names,
									const GPL_Boolean fullpath = TRUE);
	virtual GPL_Boolean GetFiles (GPL_Array<GPL_File> & files);

protected:
	GPL_String name;
};

#endif /* GPL_DIR_H */
