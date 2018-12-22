
/******************************************************************************

Coral Tree gpl/dbug.cc
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

#include "gpl.h"

/******************************************************************************
This object is useful for tracing execution in a pretty print format.

keywords: c++, debug
******************************************************************************/
int GPL_Debug::Depth = 0;

int GPL_Timer::nextid = 0;

/******************************************************************************
******************************************************************************/
GPL_Debug::GPL_Debug(char *function_name, char *filename, int line_number)
{
	int i;
	Name = function_name;
	fprintf(stderr,"%10.10s %5.5d ",filename,line_number);
	for(i = 0; i < Depth; i++)
		fprintf(stderr," | ");

	fprintf(stderr, " > entering %s\n", Name);

	Depth++;
}

/******************************************************************************
******************************************************************************/
GPL_Debug::~GPL_Debug()
{
	int i;

	Depth--;

	fprintf(stderr,"                 ");
	for(i = 0; i < Depth; i++)
		fprintf(stderr," | ");

	fprintf(stderr, " < leaving %s\n", Name);
}

/******************************************************************************
******************************************************************************/
void GPL_Debug::print(char *st, char *filename, int line_number)
{
	int i;

	fprintf(stderr,"%10.10s %5.5d ",filename,line_number);
	for(i = 0; i < Depth; i++)
		fprintf(stderr," | ");

	fprintf(stderr, " %s\n", st);
}

/******************************************************************************
******************************************************************************/
void GPL_Debug::header(char *filename, int line_number)
{
	int i;
	fprintf(stderr,"%10.10s %5.5d ",filename,line_number);
	for(i = 0; i < Depth; i++)
		fprintf(stderr," | ");

	fprintf(stderr," ");
}


