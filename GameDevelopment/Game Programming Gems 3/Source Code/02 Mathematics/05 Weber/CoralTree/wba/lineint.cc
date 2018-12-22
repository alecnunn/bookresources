/******************************************************************************

Coral Tree wba/lineint.cc
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

#include <wba/wba.h>
#include <wba/lineint.h>

/***********************************************************************
class WBA_LineInterpreter
***********************************************************************/
/***********************************************************************
WBA_LineInterpreter::WBA_LineInterpreter(void)
***********************************************************************/
WBA_LineInterpreter::WBA_LineInterpreter(void)
	: source((WBA_ValueSource *) GPL_BAD_ADDRESS),
	executor((WBA_Executor *) GPL_BAD_ADDRESS)
{
}

/***********************************************************************
GPL_Boolean WBA_LineInterpreter::Interpret(const GPL_String & line,
											GPL_Array<GPL_String> & arguments)
***********************************************************************/
GPL_Boolean WBA_LineInterpreter::Interpret(const GPL_String & line,
											GPL_Array<GPL_String> & arguments)
{
	arguments.Clear();

	GPL_String string = "";
	register const unsigned long n = line.GetLength();
	for (register unsigned long i = 0; n > i; ++i)
	{
		if (isspace(line[i]))
		{
			if (0 < string.GetLength())
			{
				arguments.AppendElement(string);
				string = "";
			}
		}
		else
		{
			string.Append(line[i]);
		}
	}

	if (0 < string.GetLength())
	{
		arguments.AppendElement(string);
	}

	return TRUE;
}

/***********************************************************************
class WBA_BourneShellLineInterpreter
***********************************************************************/
#define	ISVARCHAR(x)	(isalnum((x)) || '_' == (x))

/***********************************************************************
GPL_Boolean WBA_BourneShellLineInterpreter::InterpretVariable
	(const GPL_String & line, const unsigned long i, unsigned long & flags,
		unsigned long & start, GPL_String & string)
***********************************************************************/
GPL_Boolean WBA_BourneShellLineInterpreter::InterpretVariable
	(const GPL_String & line, const unsigned long i, unsigned long & flags,
		unsigned long & start, GPL_String & string)
{
	if (!ISVARCHAR(line[i]) &&
			(flags & WBA_LINEINTERPRETER_VARIABLE) &&
		('{' != line[i] || start != i))
	{
		if ('}' == line[i] && (flags & WBA_LINEINTERPRETER_EXPLICIT))
		{
			if (start + 1 < i)
			{
				GPL_String value = "";
				if ((WBA_ValueSource *) GPL_BAD_ADDRESS != GetSource())
				{
					GetSource()->GetValue
						(line.GetSubString(start + 1, i - 1), value);
				}
				string.Append(value);
			}
			else
			{
				string.Append('$');
			}
			start = line.GetLength();
			flags |= WBA_LINEINTERPRETER_NOTEMPTY;
			flags &= ~WBA_LINEINTERPRETER_VARIABLE;
			flags &= ~WBA_LINEINTERPRETER_EXPLICIT;

			// Skip the '}'.
			return TRUE;
		}
		else
		{
			if (start < i)
			{
				GPL_String value = "";
				if ((WBA_ValueSource *) GPL_BAD_ADDRESS != GetSource())
				{
					GetSource()->GetValue
							(line.GetSubString(start, i - 1), value);
				}
				string.Append(value);
			}
			else
			{
				string.Append('$');
			}
			start = line.GetLength();
			flags |= WBA_LINEINTERPRETER_NOTEMPTY;
			flags &= ~WBA_LINEINTERPRETER_VARIABLE;
			flags &= ~WBA_LINEINTERPRETER_EXPLICIT;
		}
	}
	return FALSE;
}

/***********************************************************************
GPL_String WBA_BourneShellLineInterpreter::InterpretCommand
	(const GPL_String & string, unsigned long & flags)
***********************************************************************/
GPL_String WBA_BourneShellLineInterpreter::InterpretCommand
	(const GPL_String & string, unsigned long & flags)
{
	flags &= ~WBA_LINEINTERPRETER_EXECUTABLE;
	GPL_String output;
	if ((WBA_Executor *) GPL_BAD_ADDRESS != GetExecutor() &&
		0 < string.GetLength())
	{
		if (GetExecutor()->Execute(string, output))
		{
			flags |= WBA_LINEINTERPRETER_NOTEMPTY;
		}
	}
	if (0 < output.GetLength())
	{
		return output;
	}
	else
	{
		return "";
	}
}

/***********************************************************************
void WBA_BourneShellLineInterpreter::AppendString
	(unsigned long & flags, GPL_String & string,
		GPL_Array<GPL_String> & arguments)
***********************************************************************/
void WBA_BourneShellLineInterpreter::AppendString
	(unsigned long & flags, GPL_String & string,
		GPL_Array<GPL_String> & arguments)
{
	// If the string is not empty...
	if (flags & WBA_LINEINTERPRETER_NOTEMPTY)
	{
		arguments.AppendElement(string);
		string = "";
		flags &= ~WBA_LINEINTERPRETER_NOTEMPTY;
	}
}

/***********************************************************************
GPL_Boolean WBA_BourneShellLineInterpreter::Interpret(const GPL_String & line,
											GPL_Array<GPL_String> & arguments)
***********************************************************************/
GPL_Boolean WBA_BourneShellLineInterpreter::Interpret(const GPL_String & line,
											GPL_Array<GPL_String> & arguments)
{
	arguments.Clear();

	unsigned long varStart = line.GetLength();
	unsigned long execStart = line.GetLength();
	unsigned long flags = WBA_LINEINTERPRETER_NONE;
	GPL_String string = "";
	register const unsigned long n = line.GetLength();
	register unsigned long i = 0;
	for (i = 0; n > i; ++i)
	{
		// If inside a single-tick...
		if (flags & WBA_LINEINTERPRETER_SINGLE)
		{
			// If this is the closing single-tick...
			if ('\'' == line[i])
			{
				flags &= ~WBA_LINEINTERPRETER_SINGLE;
			}
			else
			{
				string.Append(line[i]);
			}
		}

		// If escaped...
		else if (flags & WBA_LINEINTERPRETER_ESCAPED)
		{
			// If inside a double-tick...
			if (flags & WBA_LINEINTERPRETER_DOUBLE)
			{
				if ('"' == line[i] || '`' == line[i] ||
					'$' == line[i] || '\\' == line[i])
				{
					flags &= ~WBA_LINEINTERPRETER_ESCAPED;
					flags |= WBA_LINEINTERPRETER_NOTEMPTY;
					string.Append(line[i]);
				}
				else if ('\n' != line[i])
				{
					flags &= ~WBA_LINEINTERPRETER_ESCAPED;
					flags |= WBA_LINEINTERPRETER_NOTEMPTY;
					string.Append('\\');
					string.Append(line[i]);
				}
			}
			else
			{
				flags &= ~WBA_LINEINTERPRETER_ESCAPED;
				flags |= WBA_LINEINTERPRETER_NOTEMPTY;
				string.Append(line[i]);
			}
		}

		// If inside a back-tick...
		else if (flags & WBA_LINEINTERPRETER_EXECUTABLE)
		{
			if (InterpretVariable(line, i, flags, varStart, string))
			{
				continue;
			}

			// If this is the closing back-tick...
			if ('`' == line[i])
			{
				const GPL_String output =
					InterpretCommand(string.GetSubString(execStart,
												string.GetLength() - 1), flags);
				string.Remove(execStart, string.GetLength() - execStart);
				string.Append(output);
				execStart = string.GetLength();
			}
			else if ('\\' == line[i])
			{
				flags |= WBA_LINEINTERPRETER_ESCAPED;
			}
			else if ('$' == line[i])
			{
				flags |= WBA_LINEINTERPRETER_VARIABLE;
				varStart = i + 1;
			}
			else if ('{' == line[i])
			{
				if ((flags & WBA_LINEINTERPRETER_VARIABLE) &&
					!(flags & WBA_LINEINTERPRETER_EXPLICIT))
				{
					flags |= WBA_LINEINTERPRETER_EXPLICIT;
				}
				else
				{
					string.Append(line[i]);
				}
			}
			else if (!ISVARCHAR(line[i]) ||
						!(flags & WBA_LINEINTERPRETER_VARIABLE))
			{
				string.Append(line[i]);
			}
		}

		// If inside a double-tick...
		else if (flags & WBA_LINEINTERPRETER_DOUBLE)
		{
			if (InterpretVariable(line, i, flags, varStart, string))
			{
				continue;
			}

			// If this is the closing double-tick...
			if ('"' == line[i])
			{
				flags &= ~WBA_LINEINTERPRETER_DOUBLE;
			}
			else if ('\\' == line[i])
			{
				flags |= WBA_LINEINTERPRETER_ESCAPED;
			}
			else if ('`' == line[i])
			{
				flags |= WBA_LINEINTERPRETER_EXECUTABLE;
				execStart = string.GetLength();
			}
			else if ('$' == line[i])
			{
				flags |= WBA_LINEINTERPRETER_VARIABLE;
				varStart = i + 1;
			}
			else if ('{' == line[i])
			{
				if ((flags & WBA_LINEINTERPRETER_VARIABLE) &&
					!(flags & WBA_LINEINTERPRETER_EXPLICIT))
				{
					flags |= WBA_LINEINTERPRETER_EXPLICIT;
				}
				else
				{
					string.Append(line[i]);
				}
			}
			else if (!ISVARCHAR(line[i]) ||
						!(flags & WBA_LINEINTERPRETER_VARIABLE))
			{
				string.Append(line[i]);
			}
		}

		// If white-space...
		else if (isspace(line[i]))
		{
			if (InterpretVariable(line, i, flags, varStart, string))
			{
				continue;
			}

			AppendString(flags, string, arguments);
		}

		else
		{
			if (InterpretVariable(line, i, flags, varStart, string))
			{
				continue;
			}

			if ('\'' == line[i])
			{
				flags |= WBA_LINEINTERPRETER_NOTEMPTY;
				flags |= WBA_LINEINTERPRETER_SINGLE;
			}
			else if ('`' == line[i])
			{
				flags |= WBA_LINEINTERPRETER_EXECUTABLE;
				execStart = string.GetLength();
			}
			else if ('"' == line[i])
			{
				flags |= WBA_LINEINTERPRETER_NOTEMPTY;
				flags |= WBA_LINEINTERPRETER_DOUBLE;
			}
			else if ('\\' == line[i])
			{
				flags |= WBA_LINEINTERPRETER_ESCAPED;
			}
			else if ('#' == line[i])
			{
				break;
			}
			else if ('$' == line[i])
			{
				flags |= WBA_LINEINTERPRETER_VARIABLE;
				varStart = i + 1;
			}
			else if ('{' == line[i])
			{
				if ((flags & WBA_LINEINTERPRETER_VARIABLE) &&
					!(flags & WBA_LINEINTERPRETER_EXPLICIT))
				{
					flags |= WBA_LINEINTERPRETER_EXPLICIT;
				}
				else
				{
					string.Append(line[i]);
				}
			}
			else if (!ISVARCHAR(line[i]) ||
						!(flags & WBA_LINEINTERPRETER_VARIABLE))
			{
				flags |= WBA_LINEINTERPRETER_NOTEMPTY;
				string.Append(line[i]);
			}
		}
	}

	InterpretVariable(line, i, flags, varStart, string);

	AppendString(flags, string, arguments);

	return TRUE;
}
