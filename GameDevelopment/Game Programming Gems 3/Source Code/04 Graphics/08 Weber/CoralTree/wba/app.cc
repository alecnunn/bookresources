/******************************************************************************

Coral Tree wba/app.cc
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

WBA_Application * WBA_Application::singleton = (WBA_Application *) 0;

char *WBA__FILE__ = (char *) GPL_BAD_ADDRESS;
long WBA__LINE__ = 0;


WBA_Application::WBA_Application(WBA_Application *current)
	: exitCode(0), inputEchoMode(TRUE),
	standardOutput ((OSD_Output *) GPL_BAD_ADDRESS)
{
	if ((WBA_Application *) 0 == singleton)
	{
		singleton = current;
	}

	logger = &GPL_LOGGER;
	environment = &WBA_ENV;
}

WBA_Application::~WBA_Application(void)
{
	if ((OSD_Output *) GPL_BAD_ADDRESS != standardOutput)
	{
		osdOutputClose(standardOutput);
		delete standardOutput;
	}
}

void WBA_Application::Printf(char *format, ...)
{
	va_list args;
	va_start(args, format);
	GPL_String s;
	s.VPrintf(format, args);
	osdOutputPrintLine(GetStandardOutput(), (char *) 0, s.GetData());
	va_end(args);
}

int WBA_Application::Main(void)
{
	if (Initialize())
	{
		Process();
		Finalize();
	}
	return exitCode;
}

void WBA_Application::Exit(void)
{
	exit(exitCode);
}

const GPL_Version & WBA_Application::GetVersion(void) const
{
	return version;
}

const char * WBA_Application::GetBuildString(void) const
{
	//return version.ToString().GetData();
	return OSD_BUILDID;
}

const char *WBA_Application::GetUsage(GPL_Boolean all) const
{
	GPL_String	tmp, t, maybe;

	// Trick to avoid contravariance.
	GPL_String *u = &(((WBA_Application *) this)->usage);
	u->Append("NAME: ");
	u->Append((const char *)(GetName()));
	u->Append("\nPROGRAM: ");
	u->Append((const char *)(((WBA_Application *) this)->environment->GetProgramName()));
	u->Append("\nVERSION: ");
	u->Append((const char *)(GetVersion().ToString().GetData()));
	u->Append("\nPARAMETER USAGE: ");
	u->Append((const char *)(GetParameterUsage()));
	u->Append("\nPARAMETERS:");
	unsigned long i, count;
	count = ((WBA_Application *) this)->environment->GetParameters()->GetCount();
	for(i = 0; i < count; i++)
	{
		u->Append(" ");
		u->Append((const char *)(((WBA_Application *) this)->environment->GetParameters()->GetElement(i)));
	}
	u->Append("\nENVIRONMENT:\n");

	GPL_DoubleList<WBA_Argument> oversize_list;
	GPL_HashTableIterator iterator;
	WBA_Argument *arg;
	iterator.Reset(&WBA_ARGS);
	/* go through arguments */
	while((arg = (WBA_Argument *)iterator.Next()))
	{
		GPL_Boolean do_long_style = FALSE;
		if(!all && (arg->GetQuiet() == TRUE))
		{
			continue;
		}
		if(!(arg->GetDescription().IsNull()))
		{
			maybe.Clear();
			if(arg->GetKey().GetLength() <= 20)
			{
				t.Clear();
				t.Append((const char *)(arg->GetKey()));
				tmp.Printf(" %-20s",(char *)t); 
				t.Clear();
				maybe.Append((char *)tmp);
				/* go through aliases */
				const GPL_List<GPL_String> & aliases = arg->GetAliases();
				register const unsigned long n = aliases.GetCount();
				for (i = 0; n > i; ++i)
				{
					t.Join(",", aliases[i]);
				}
				if(t.GetLength() <= 10)
				{
					if ((char *) 0 != (char *) t)
					{
						tmp.Printf(" %-10s", (char *) t); 
					}
					else 
					{
						tmp.Printf(" %-10s", " "); 
					}
					t.Clear();
					maybe.Append((char *)tmp);
					/* current value */
					if(!(arg->GetValue().IsNull()))
					{
						t.Append("[");
						t.Append((const char *)arg->GetValue());
						t.Append("]");
					}
					else
					{
						t.Append("[");
						t.Append("NULL");
						t.Append("]");
					}
					if(t.GetLength() <= 10)
					{
						if ((char *) 0 != (char *) t)
						{
							tmp.Printf(" %-10s", (char *) t); 
						}
						t.Clear();
						if(arg->GetDescription().GetLength() <= 35)
						{
							maybe.Append((char *)tmp);
							maybe.Append(" ");
							maybe.Append((const char *)arg->GetDescription());
							maybe.Append("\n");
						} else { do_long_style = TRUE; }
					} else { do_long_style = TRUE; }
				} else { do_long_style = TRUE; }
			} else { do_long_style = TRUE; }
			if(do_long_style)
			{
				oversize_list.AppendNode(arg);
			}
			else
			{
				u->Append(maybe);
			}
		}
	}

	oversize_list.ToHead();
	while((arg = oversize_list.PostIncrement()))
	{
		u->Append("\n");
		u->Append(" ");
		u->Append(arg->GetKey());
		u->Append("\n");
		const GPL_List<GPL_String> & aliases = arg->GetAliases();
		register const unsigned long n = aliases.GetCount();
		t.Clear();
		for (i = 0; n > i; ++i)
		{
			t.Join(",", aliases[i]);
		}
		if(!t.IsNull())
		{
			u->Append("    ");
			u->Append(t);
			u->Append("\n");
		}
		u->Append("    ");
		u->Append("[");
		if(arg->GetValue().IsNull())
		{
			u->Append("NULL");
		}
		else
		{
			u->Append(arg->GetValue());
		}
		u->Append("]");
		u->Append("\n");
		if(!arg->GetDescription().IsNull())
		{
			u->Append("    ");
			u->Append(arg->GetDescription());
			u->Append("\n");
		}
	}

	return u->GetData();
}

const char *WBA_Application::DumpEnvironment(void) const
{
	GPL_String *s;
	s = &(((WBA_Application *) this)->dump);

	GPL_HashTableIterator iterator;
	WBA_Argument *arg;
	iterator.Reset(&WBA_ARGS);
	/* go through arguments */
	s->Clear();
	while((arg = (WBA_Argument *)iterator.Next()))
	{
		s->Append((const char *)(arg->GetKey()));
		s->Append("=");
		s->Append((const char *)arg->GetValue());
		s->Append("\n");
	}

	return s->GetData();
}

void WBA_Application::SetInputEchoMode(const GPL_Boolean mode)
{
	if(mode != inputEchoMode)
	{
		inputEchoMode = mode;
		osdStdinSetEchoMode(mode);
	}
}

GPL_Boolean WBA_Application::Prompt(const GPL_String & prompt,
									const GPL_String & initial,
									GPL_String & response,
									const GPL_Boolean hidden)
{
	// Print the prompt.
	if (0 < prompt.GetLength())
	{
		osdOutputRaw(FALSE, prompt.GetData(), prompt.GetLength());
		if (0 < initial.GetLength())
		{
			osdOutputRaw(FALSE, "[", 1);
			osdOutputRaw(FALSE, initial.GetData(), initial.GetLength());
			osdOutputRaw(FALSE, "]: ", 3);
		}
	}

	// Get some input.
	register char *buffer = (char *) GPL_BAD_ADDRESS;
	long n = 0;
	const GPL_Boolean oldInputEchoMode = GetInputEchoMode ();
	SetInputEchoMode(!hidden);
	buffer = osdGetStdin(&n);
	SetInputEchoMode(oldInputEchoMode);

	// Massage the input into a response.
	if ((char *) GPL_BAD_ADDRESS != buffer)
	{
		if (0 != n)
		{
			// Strip trailing white-space.
			while (0 < n && isspace(buffer[n - 1]))
			{
				--n;
			}
			buffer[n] = 0;
			response = buffer;
		}
		osdFree(buffer);
	}

	// Use the initial value, if necessary.
	if (0 == response.GetLength())
	{
		response = initial;
	}

	// Is there some way to determine a non-constant value for this?
	return TRUE;
}

GPL_Boolean WBA_Application::Prompt(const GPL_String & prompt,
									const GPL_String & initial,
									const GPL_String & delimiter,
									GPL_List<GPL_String> & responses,
									const GPL_Boolean hidden)
{
	// Get the response.
	GPL_String response;
	if (!Prompt(prompt, initial, response, hidden))
	{
		return FALSE;
	}

	// Parse the response string.
	responses.Clear();
	GPL_StringTokenizer tokens;
	tokens.Reset(response, delimiter);
	GPL_String token;
	while (0 != (token = tokens.Next()).GetLength())
	{
		// Strip preceding white-space.
		unsigned long i = 0;
		while (token.GetLength() > i && isspace(token[i]))
		{
			++i;
		}

		// Strip postceding white-space.
		long j = token.GetLength() - 1;
		while (0 <= j && isspace(token[(unsigned long) j]))
		{
			--j;
		}

		// If there is anything left...
		if ((long) i <= j)
		{
			// Add the response.
			responses.Append(token.GetSubString(i, (unsigned long) j));
		}
	}

	return TRUE;
}

OSD_Output * WBA_Application::GetStandardOutput(void)
{
	if ((OSD_Output *) GPL_BAD_ADDRESS == standardOutput)
	{
		standardOutput = new OSD_Output;
		if ((OSD_Output *) GPL_BAD_ADDRESS != standardOutput)
		{
			// Specifically ignoring the success of the following two
			// calls since: a) they should not fail; and b) if they
			// fail we have not good way to report their failure.;)
			osdOutputOpen(standardOutput);
			osdOutputSetFlags(standardOutput, OSD_OUT_STDOUT);
		}
	}
	return standardOutput;
}
