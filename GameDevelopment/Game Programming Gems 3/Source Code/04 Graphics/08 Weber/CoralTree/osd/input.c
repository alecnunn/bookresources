/******************************************************************************

Coral Tree osd/input.c
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

#include <osd/osd.h>

#if OSD_COMPILER==OSD_KR
char *osdGetStdin(n)
	long *n;
#else
char *osdGetStdin(long *n)
#endif
{
	long i;
	long allocated;
	int c;
	char *buffer;

	if (n)
	{
		*n = 0;
	}

	buffer = (char *) osdMalloc(OSD_GETSTDIN_BLOCKSIZE);
	if (!buffer)
	{
		return NULL;
	}
	allocated = OSD_GETSTDIN_BLOCKSIZE;

	i = 0;
	while (EOF != (c = getchar()) && '\n' != c)
	{
		buffer[i++] = c;
		if (i >= allocated) 
		{
			buffer = (char *) osdRealloc(buffer,
								allocated + OSD_GETSTDIN_BLOCKSIZE);
			allocated += OSD_GETSTDIN_BLOCKSIZE;
		}
	}
	if ('\n' == c)
	{
		buffer[i++] = c;
		if (i >= allocated) 
		{
			buffer = (char *) osdRealloc(buffer, allocated + 1);
			allocated += 1;
		}
	}
	buffer[i] = '\0';

	if (n)
	{
		*n = i;
	}
	return buffer;
}

#if OSD_COMPILER==OSD_KR
void osdStdinSetEchoMode()
#else
void osdStdinSetEchoMode(long m)
#endif
{
#if OSD_OSTYPE==OSD_WIN32
	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
	DWORD fdwMode = 0;
	if (GetConsoleMode(hConsole, &fdwMode))
	{
		if (!m)
		{
			fdwMode &= ~ENABLE_ECHO_INPUT;
		}
		else
		{
			fdwMode |= ENABLE_ECHO_INPUT;
		}
		if (!SetConsoleMode(hConsole, fdwMode))
		{
		}
	}
	else
	{
	}
#elif OSD_OSTYPE==OSD_UNIX
	struct termios io;
	if (0 == tcgetattr(STDIN_FILENO, &io))
	{
		if (!m)
		{
			io.c_lflag &= ~ECHO;
		}
		else
		{
			io.c_lflag |= ECHO;
		}
		if (0 != tcsetattr(STDIN_FILENO, TCSANOW, &io))
		{
		}
	}
	else
	{
	}
#else
#endif
}
