
/******************************************************************************

Coral Tree wba/shell.cc
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

OSD_SignalHandler(wbaControlCHandler)
{
	WBA_APP->Printf("  ");
}

WBA_Shell::WBA_Shell(WBA_Shell *current)
	: WBA_Application(current), continueProcessing(TRUE), echoMode(FALSE)
{
	lineInterpreter = &defaultLineInterpreter;
	promptMode = WBA_PROMPTMODE_NORMAL;
}

WBA_Shell::~WBA_Shell(void)
{
	WBA_CommandHandler *handler;
	commands.ToHead();
	while ((handler = commands.PostIncrement()))
	{
		commands.RemoveNode(handler);
		delete handler;
	}
	GPL_String *s;
	inputCommands.ToHead();
	while((s = inputCommands.PostIncrement()))
	{
		inputCommands.RemoveNode(s);
		delete s;
	}
}

void WBA_Shell::AddCommandInput(const GPL_String &string)
{
	GPL_String *s;
	s = new GPL_String();
	*s = string;
	inputCommands.ToHead();
	inputCommands.InsertBefore(s);
}

GPL_Boolean WBA_Shell::ParseBoolean(const GPL_String & string,
									GPL_Boolean & boolean)
{
	static GPL_RegularExpression booleanExpression("^[ 	]*"
		"((\\+|ON|TRUE|YES|T|Y|on|true|yes|t|y)|"
		"(\\-|OFF|FALSE|NO|F|N|off|false|no|f|n))"
		"[ 	]*$");
	if (booleanExpression.Matches(string))
	{
		if (0 != booleanExpression.GetMatch(2).GetLength())
		{
			boolean = TRUE;
		}
		else
		{
			boolean = FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

WBA_CommandHandler *WBA_Shell::FindHandler(const GPL_String &command_name)
{
	WBA_CommandHandler *handler;
	commands.ToHead();
	while ((handler = commands.PostIncrement()))
	{
		unsigned long j;
		for (j = 0; j < handler->names.GetCount(); ++j)
		{
			if (handler->names.GetElement(j) == command_name)
			{
				return handler;
			}
		}
	}
	return NULL;
}

int WBA_Shell::Main(void)
{
	prompt.Clear();
	prompt.Append(GetName());
	prompt.Append("> ");
	if (Initialize())
	{
		// The Control-C signal handler is installed here so that
		// shells which require a login still allow the user to
		// Control-C out of the login.
		//OSD_SetSignalHandler(wbaControlCHandler, SIGINT);

		while (continueProcessing)
		{
			GPL_String *cline;
			inputCommands.ToHead();
			if ((cline = inputCommands.PostIncrement()))
			{
				if (!lineInterpreter->Interpret(*cline, inputLine))
				{
					continue;
				}
				inputCommands.RemoveNode(cline);
				delete cline;
			}
			else
			{
				WBA_CommandHandler *handler=FindHandler(WBA_PREPROMPT_COMMAND);
				if(handler)
					{
					handler->arguments.Clear();
					if (!handler->HandleCommand())
						PrintHandlerHelp(handler);
					}

				// Send directly to stdout which assumes
				// that this is a valid console.
				// This should be a valid assumption for a `shell'.
				switch(GetPromptMode())
				{
					case WBA_PROMPTMODE_NORMAL:
						osdPrintf(GetStandardOutput(), (char *) 0,
									"%s", prompt.GetData());
						break;
					case WBA_PROMPTMODE_EXPECT:
						osdPrintf(GetStandardOutput(), (char *) 0,
									"%s\n", prompt.GetData());
						break;
					default:
						wbaLog("WBA WARNING", "Invalid prompt mode");
						break;
				}
				char *cp = osdGetStdin((long *) 0);
				const GPL_String s = cp;
				osdFree(cp);
				if (!lineInterpreter->Interpret(s, inputLine))
				{
					continue;
				}
			}

			if (inputLine.GetCount() >= 1)
			{
				if (echoMode)
				{
					GPL_String st = GPL_String::Join(" ", inputLine);
					// Just in case the input command contains something
					// that looks like a printf() format (e.g., '%s'),
					// print it with a format.
					Printf("%s", st.GetData());
				}
				WBA_CommandHandler *handler;
				handler = FindHandler(inputLine.GetElement(0));

				if(!handler)
					handler = FindHandler(WBA_DEFAULT_COMMAND);

				if(handler)
				{
					handler->arguments.Clear();
					handler->arguments = inputLine;
					if (!handler->HandleCommand())
					{
						PrintHandlerHelp(handler);
					}
				}
				else
				{
					Printf("'%s' not handled\n", 
						(const char *) inputLine.GetElement(0));
				}

#if 0
				WBA_CommandHandler *handler;
				commands.ToHead();
				GPL_Boolean handled = FALSE;
				while (!handled && (handler = commands.PostIncrement()))
				{
					unsigned long j;
					for (j = 0; j < handler->names.GetCount(); ++j)
					{
						if (handler->names.GetElement(j) ==
							inputLine.GetElement(0))
						{
							handler->arguments.Clear();
							handler->arguments = inputLine;
							if (!handler->HandleCommand())
							{
								PrintHandlerHelp(handler);
							}
							handled = TRUE;
							break;
						}
					}
				}
				if (!handled)
				{
					Printf("'%s' not handled\n",
							(const char *) inputLine.GetElement(0));
				}
#endif
			}
			// moved out one so that blank lines can process, too - Jason
			Process();
		}

		Finalize();
	}

	return exitCode;
}

void WBA_Shell::PrintHandlerHelp(WBA_CommandHandler *handler)
{
	if (handler->names.GetCount() > 0)
	{
		if ((const char *) 0 == handler->GetUsage())
		{
			Printf("USAGE: %s", (const char *) handler->names.GetElement(0));
		}
		else
		{
			Printf("USAGE: %s %s", (const char *) handler->names.GetElement(0),
					(const char *) (handler->GetUsage()));
		}
		GPL_String s = GPL_String::Join(" ",
					handler->names.GetRange(1, handler->names.GetCount() - 1));
		if (0 < s.GetLength())
		{
			Printf("ALIASES: %s", (const char *) s);
		}
		if ((const char *) 0 != handler->GetDescription())
		{
			Printf("DESCRIPTION: %s\n",
					(const char *) (handler->GetDescription()));
		}
	}
}

void WBA_Shell::PrintCommands(void)
{
	WBA_CommandHandler *handler;
	GPL_String s;
	commands.ToHead();
	while ((handler = commands.PostIncrement()))
	{
		if (!(handler->IsHidden()))
		{
			Printf("%-15s %s", (const char *) handler->names.GetElement(0),
					(const char *) (handler->GetDescription()));
		}
	}
}
