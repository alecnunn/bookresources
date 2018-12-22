/******************************************************************************

Coral Tree wba/env.cc
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

WBA_Environment *WBA_GLOBAL_ENV = 0;
//long WBA_Init::cnt = 0;
GPL_INIT_DEFINITION(WBA);
//GPL_Init WBA_Init::gplinit;

WBA_Environment::WBA_Environment(void)
{
	argumentTable = new WBA_ArgumentTable(9);
}

WBA_Environment::~WBA_Environment(void)
{
	delete argumentTable;
}

/***********************************************************************
GPL_Boolean WBA_Environment::GetValue(const GPL_String & name,
										GPL_String & value)
***********************************************************************/
GPL_Boolean WBA_Environment::GetValue(const GPL_String & name,
										GPL_String & value)
{
	register WBA_Argument *argument = argumentTable->Find(name);
	if ((WBA_Argument *) GPL_BAD_ADDRESS != argument)
	{
		value = argument->GetValue();
		return TRUE;
	}
	return FALSE;
}

GPL_Boolean WBA_Environment::LoadCommandLine(const int argc, const char **argv,
												GPL_Boolean mode)
{
	GPL_Boolean return_val = TRUE;
	// Save the program name.
	programName = argv[0];
	// Since osdBasename edits data we Duplicate.
	programBasename = programName.Duplicate();
	char *tmp;
	tmp = osdStrdup((char *)(programBasename));
	programBasename = (char *)(osdBasename(tmp));
	osdFree(tmp);

	// Clear the parameters in case LoadCommandLine() is called more than once.
	parameters.Clear();

	// Ignore the program name.
	long hasArguments = 1;
	register unsigned long i = 1;
	GPL_String arg;
	unsigned long length;
	GPL_String flag;
	while (argc > (int) i)
	{
		// If there are more arguments...
		arg = argv[i];
		length = arg.GetLength();
		if (hasArguments)
		{
			// If there are no more arguments...
			if (arg == "--")
			{
				hasArguments = 0;
				++i;
				continue;
			}

			// If this is an argument...
			long argtype;
			if (1 < length)
			{
				if('-' == arg[OSD_HELP_CAST(const unsigned long) 0])
				{
					argtype = WBA_MINUS;
				}
				else if ('+' == arg[OSD_HELP_CAST(const unsigned long) 0])
				{
					argtype = WBA_PLUS;
				}
				else
				{
					argtype = WBA_PARAM;
				}

				if(argtype != WBA_PARAM)
				{
					// Get rid of the leading '-' or '+'.
					flag = arg.GetSubString(1, length - 1);

					// see if argument exists in table
					WBA_Argument *argument;
					argument = argumentTable->Find(flag);
					if(!argument)
					{
						return_val = FALSE;
						if(mode)
						{
							argument = new WBA_Argument(flag);
							argumentTable->Set(argument);
							//argument->SetDescription("set from command line");
						}
					}

					// are there more arguments
					if (argc > (int) (1 + i))
					{
						// Default depending on flag
						if(argument)
						{
							if(argtype == WBA_PLUS) argument->Decrement();
							else argument->Increment();
						}

						// If the next argument is not a flag...
						arg = argv[1 + i];
						if (0 < arg.GetLength() && ('-' != arg[OSD_HELP_CAST(const unsigned long) 0] && '+' != arg[OSD_HELP_CAST(const unsigned long) 0]))
						{
							// It is the value.
							++i;
							if(argument) argument->SetValue(arg);
						}
					}
					else // set to the default
					{
						if(argument)
						{
							if(argtype == WBA_PLUS) argument->Decrement();
							else argument->Increment();
						}
					}

					++i;
					continue;
				}
			}
		}

		// If execution falls through to here, this is a parameter.
		parameters.AppendElement(arg);
		++i;
	}
	return return_val;
}

void WBA_Environment::LoadEnvVariables(const char **envp, GPL_Boolean mode)
{
	register char **env = (char **)envp;
	GPL_String key;
	GPL_String value;
	while ((char **) 0 != env && (char *) 0 != *env)
	{
		WBA_Argument *argument;
		long i;
		for(i=0;;i++)
		{
			if((*env)[i] == '\0')
			{
				if(0 == i)
				{
					wbaLog("WBA LOG", "environment variable load hit blank entry");
				}
				else
				{
					wbaLog("WBA WARNING", "environment variable load [%s] possibly corrupt", *env);
				}
				break;
			}
			if((*env)[i] == '=')
			{
				key.SetData(*env, i);
				value.SetData(&((*env)[i+1]));
				break;
			}
		}
		argument = argumentTable->Find(key);
		if(!argument)
		{
			if(mode)
			{
				argument = new WBA_Argument(key);
				argumentTable->Set(argument);
				//argument->SetDescription("set from environment variable");
			}
		}
		if(argument) argument->SetValue(value);
		env++;
	}
}

GPL_Boolean WBA_Environment::LoadFile(const GPL_String & filename,
										GPL_Boolean mode)
{
	GPL_File file;

	file.Open(filename, OSD_FILE_READ);
	if (!file.IsValid())
	{
		return FALSE;
	}

	GPL_FileIterator iterator;
	iterator.Reset(&file);
	GPL_StringTokenizer tokenizer;
	GPL_String line;
	GPL_String key;
	GPL_String value;

	while ((char *) 0 != (line = iterator.Next()).GetData())
	{
		static GPL_RegularExpression regexp("^[ \t]*([_A-Za-z][_A-Za-z0-9]*)[ \t]*=([ !\"\\$-~]*)");
		if (!regexp.Matches(line))
		{
			continue;
		}

		// If the key has data...
		key = regexp.GetMatch(1);
		value = regexp.GetMatch(2);
		if (0 < key.GetLength())
		{
			WBA_Argument *argument = argumentTable->Find(key);
			if ((WBA_Argument *) GPL_BAD_ADDRESS == argument)
			{
				if (mode)
				{
					argument = new WBA_Argument(key);
					argumentTable->Set(argument);
				}
			}
			if ((WBA_Argument *) GPL_BAD_ADDRESS != argument)
			{
				SubstituteVariables(value);
				argument->SetValue(value);
			}
		}
	}

	file.Close();
	return TRUE;
}

void WBA_Environment::SubstituteVariables(GPL_String &string)
{
	unsigned long i,j;
	for(i=0;i<string.GetLength();i++)
	{
		if('$' == string[i])
		{
			GPL_String var,newstr;
			if(i >= string.GetLength())
			{
				continue;
			}
			if(string[i+1] != '{')
			{
				continue;
			}
			for(j = i+2;j < string.GetLength();j++)
			{
				if('}' == string[j])
				{
					if(j > i+2)
					{
						WBA_Argument *arg;
						var = string.GetSubString(i+2,j-1);
						string.Remove(i,j-i+1);
						if((arg = argumentTable->Find(var)))
						{
							string.Insert(i,arg->GetValue());
						}
					}
					else
					{
						string.Remove(i,j-i+1);
					}
					break;
				}
			}
		}
	}
}

void WBA_Environment::Print(void)
{
	argumentTable->Print();
	unsigned long i;
	unsigned long count;
	count = parameters.GetCount();
	for(i = 0; i < count; i++)
	{
		fprintf(stderr,"(%s)\n", (char *)parameters[i]);
	}
}

GPL_String WBA_Environment::Strip(const GPL_String & s)
{
	const unsigned long l = s.GetLength();
	if (0 == l)
	{
		return s;
	}

	register char *d = s.GetData();
	register unsigned long f = 0;
	while (l > f && isspace(d[f]))
	{
		++f;
	}

	register unsigned long b = l - 1;
	while (f < b && isspace(d[b]))
	{
		--b;
	}

	return s.GetSubString(f, b);
}

