/******************************************************************************

Coral Tree ew/util.cc
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



#include "ew.h"


long ewColorCodeMode= -1;


/******************************************************************************
const char	*ewColorCode(long index)

******************************************************************************/
/*PROTOTYPE*/
const char *ewColorCode(long index)
	{
	if(ewColorCodeMode<0)
		{
		WBA_Argument *argument;

		ewColorCodeMode=0;
		if((argument=WBA_ARGS.Find("EW_COLOR_TEXT")))
			ewColorCodeMode=argument->GetLong();
		}

	return ewColorCodeArray[ewColorCodeMode%2][index%ewColorCodes];
	}



/******************************************************************************
int		ewStringToArgv(char *string, char ***argv)

	This function takes a NULL terminated string and converts it to
	(int argc,char **argv) format.

	command name not given, but must fill in argv[0] with something

keywords: string, blackbox
******************************************************************************/
/*PROTOTYPE*/
int ewStringToArgv(char *string, char ***argv)
	{
	char current[1024];
	char *p=string;

	long backslash=FALSE;
	long quote=FALSE;
	long inside=FALSE;
	long argc=1;
	long done=FALSE;
	long m=0;

	*argv=(char **)osdMalloc(sizeof(char *));

	while(!done)
		{
		if(backslash)
			{
			current[m++]=*p;
			backslash=FALSE;
			}
		else
			{
			long next=FALSE;

			switch(*p)
				{
				case 0:
					next=TRUE;
					done=TRUE;
					break;

				case '\\':
					backslash=TRUE;
					break;

				case '"':
					if(quote)
						next=TRUE;
					quote=!quote;
					break;

				case ' ':
				case '\t':
					if(inside && !quote)
						{
						next=TRUE;
						break;
						}

					// drop through

				default:
					inside=TRUE;
					current[m++]=*p;
					break;
				}

			if(next && m)
				{
				*argv=(char **)osdRealloc(*argv,(argc+1)*sizeof(char *));

				current[m]=0;
				(*argv)[argc]=osdStrdup(current);

				argc++;
				inside=FALSE;
				m=0;
				}
			}
		p++;
		}

	// we don't know what the command name is, so make something up
	(*argv)[0]=osdStrdup("command");

	return argc;
	}



#if FALSE
	// old way

	int argc = 1;
	int i = 0, in = 0, j = 0;
	char *new_string;

	if(!string)
		EW_PRINT(EW_EW,EW_ERROR,"NULL handed to string converter");

	new_string = osdStrdup(string);
	if(!new_string)
		EW_PRINT(EW_EW,EW_ERROR,"could not duplicate string");

/* -NEED TO KNOW HOW MANY ARGS WE HAVE BEFORE THE MALLOC---------------------*/
	while( new_string[i] != '\0' )
	{
		if(new_string[i] == ' ' || new_string[i] == '\t')
		{
			in = 0;
		}
		else if(!in)
		{
			argc++;
			in = 1;
		}
		i++;
	}

	*argv = (char **)osdMalloc(argc * sizeof(char *));
	(*argv)[0] = osdStrdup("command");


/* -SET THE POINTERS---------------------------------------------------------*/
	in = 0;
	argc = 1;
	i = 0;
	while( new_string[i] != '\0' )
	{
		if(new_string[i] == ' ' || new_string[i] == '\t')
		{
			new_string[i] = '\0';
			if(in) 
			{
				j++;
				in = 0;
			}
		}
		else if(!in)
		{
			(*argv)[j] = &new_string[i];
			argc++;
			in = 1;
		}
		i++;
	}
	return argc;

#endif
