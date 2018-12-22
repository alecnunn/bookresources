#ifndef GPL_TOOLS_H
#define GPL_TOOLS_H 1

/******************************************************************************

Coral Tree gpl/tools.h
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

char *gplExtract(const char *string, char *target, const char *sep, int pos);

// Will modify the pointer passed to it.
void gplStripLeadingSpaces(char **);

// Will modify the pointer passed to it.
void gplStripTrailingSpaces(char *,int );

char *gplStrDup(const char *);

#endif // GPL_TOOLS_H
