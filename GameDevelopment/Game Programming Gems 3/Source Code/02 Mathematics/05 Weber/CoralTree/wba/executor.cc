/******************************************************************************

Coral Tree wba/executor.cc
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

#include <wba/executor.h>

/***********************************************************************
class WBA_Executor
***********************************************************************/
/***********************************************************************
GPL_Boolean WBA_Executor::Execute(const GPL_String & command,
									GPL_String & output)
***********************************************************************/
GPL_Boolean WBA_Executor::Execute(const GPL_String & command,
									GPL_String & output)
{
	// If there is a command...
	GPL_Boolean success = TRUE;
	output.Clear();
	char *str = (char *) 0;
	if (0 < command.GetLength())
	{
		// Execute the command.
		int status = 0;
		success = osdExecute(command.GetData(), &status, &str, FALSE) && 0 == status;

		// If the command produced output...
		if ((char *) 0 != str)
		{
			// If the output has contents...
			register const unsigned long n = strlen(str);
			if (0 < n)
			{
				// Strip trailing new-lines.
				register unsigned long i = n;
				while (0 < i && '\n' == str[i - 1])
				{
					--i;
				}
				if (0 != i)
				{
					output.SetData(str, i);
				}
			}
			osdFree(str);
		}
	}
	return success;
}
