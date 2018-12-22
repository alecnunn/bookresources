/******************************************************************************

Coral Tree ew/util.h
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

#define EW_CODE_NORMAL		0
#define EW_CODE_RED			1
#define EW_CODE_GREEN		2
#define EW_CODE_YELLOW		3
#define EW_CODE_BLUE		4
#define EW_CODE_MAGENTA		5
#define EW_CODE_CYAN		6
#define EW_CODE_WHITE		7
#define EW_CODE_BLACK		8

const char ewColorCodes=9;
const char ewColorCodeArray[2][ewColorCodes][16]=
		{{
		"", 
		"", 
		"", 
		"",
		"", 
		"", 
		"", 
		"",
		""},
		{"[40m[0m",	// default (white?) (and set to black background)
		"[31m",		// red
		"[32m",		// green
		"[33m",		// yellow
		"[34m",		// blue
		"[35m",		// magenta
		"[36m",		// cyan
		"[37m",		// white
		"[38m"		// black
		}};
