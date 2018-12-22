#ifndef WBA_SHELL_H
#define WBA_SHELL_H

/******************************************************************************

Coral Tree wba/shell.h
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

#include <gpl/gpl.h>
#include <wba/app.h>
#include <wba/executor.h>
#include <wba/lineint.h>

#define WBA_PROMPTMODE_NORMAL   1
#define WBA_PROMPTMODE_EXPECT   2



// NOTE:
// command named DEFAULT will be handled when no other command is recognized
// command named PREPROMPT will be handled will the command queue empties
#define WBA_DEFAULT_COMMAND		"DEFAULT"
#define WBA_PREPROMPT_COMMAND	"PREPROMPT"



// Forware declaration.
class WBA_Shell;
/***********************************************************************
class WBA_CommandHandler

Well-behaved shell command handler.
***********************************************************************/
class WBA_CommandHandler
{
	public:
		GPL_Array<GPL_String>	arguments;
		GPL_Array<GPL_String>	names;
	protected:
		WBA_Shell				*parent;
		GPL_Boolean				hidden;
	public:
		WBA_CommandHandler(void)
		{
			// HP's aCC complains because WBA_Shell has not been
			// completely defined yet so aCC does not know that
			// WBA_Shell is a sub-class of WBA_Application.
			// Instead of waiting twelve lines, the idiots issue
			// a warning.  Sigh.
			parent = (WBA_Shell *) ((void *) WBA_Application::GetCurrentApplication());
			hidden = FALSE;
		}
		WBA_CommandHandler(WBA_Shell *shell)
		{
			parent = shell;
			hidden = FALSE;
		}
virtual	~WBA_CommandHandler(void) {}
virtual const char		*GetUsage(void) const
							{ return (char *) GPL_BAD_ADDRESS; }
virtual const char		*GetDescription(void) const
							{ return (char *) GPL_BAD_ADDRESS; }
virtual GPL_Boolean		HandleCommand(void) { return FALSE; }
		void			Hide(void) { hidden = TRUE; }
		GPL_Boolean		IsHidden(void) { return hidden; }
};

/***********************************************************************
class WBA_Shell

Well-behaved shell.
***********************************************************************/
class WBA_Shell : public WBA_Application
{
	private:
		GPL_String									prompt;
		GPL_DoubleList<WBA_CommandHandler>			commands;
		WBA_LineInterpreter							defaultLineInterpreter;
		long										promptMode;
	protected:
		GPL_Boolean									continueProcessing;
		GPL_Boolean									echoMode;
		WBA_LineInterpreter							*lineInterpreter;
		GPL_Array<GPL_String>						inputLine;
		GPL_DoubleList<GPL_String>					inputCommands;
	public:
		WBA_Shell(WBA_Shell *current);
virtual	~WBA_Shell(void);

virtual	const char	*GetName(void) const { return (char *) GPL_BAD_ADDRESS; }
		GPL_Boolean	GetEchoMode(void) const { return echoMode; }
		WBA_LineInterpreter *GetLineInterpreter(void)
		{
			return lineInterpreter;
		}
GPL_DoubleList<WBA_CommandHandler> *GetCommands(void) { return &commands; }
		long GetPromptMode(void) const { return promptMode; }

virtual int		Main(void);
virtual GPL_Boolean	Initialize(void) { return WBA_Application::Initialize(); }
virtual void	Finalize(void) { WBA_Application::Finalize(); }
		void	SetPrompt(const GPL_String & p) { prompt = p; }

		void	AddCommand(WBA_CommandHandler *command)
				{
					commands.AppendNode(command);
				}
		void	AddCommand(const GPL_String &string, WBA_CommandHandler *command)
				{
					command->names.Clear();
					command->names.AppendElement(string);
					commands.AppendNode(command);
				}
		void	RemoveCommand(WBA_CommandHandler *command)
				{
					commands.RemoveNode(command);
				}

		WBA_CommandHandler *FindHandler(const GPL_String &command_name);

		void	DoneProcessing(void) { continueProcessing = FALSE; }
		void	PrintHandlerHelp(WBA_CommandHandler *handler);
		void	PrintCommands(void);
		void	AddCommandInput(const GPL_String &string);
		void	SetEchoMode(const GPL_Boolean mode) { echoMode = mode; }
		void	SetLineInterpreter(WBA_LineInterpreter *i)
				{
					lineInterpreter = i;
				}
		void SetPromptMode(const long m) { promptMode = m; }

static	GPL_Boolean	ParseBoolean(const GPL_String & string,
									GPL_Boolean & boolean);
};

class WBA_CommandQuit : public WBA_CommandHandler
{
	public:
		WBA_CommandQuit(void)
		{
			names.AppendElement("quit");
		}
		WBA_CommandQuit(WBA_Shell *shell) : WBA_CommandHandler(shell)
		{
			names.AppendElement("quit");
		}
		const char * GetUsage(void) const { return ""; }
		const char * GetDescription(void) const { return "quit shell"; }
		GPL_Boolean HandleCommand(void)
		{
			parent->DoneProcessing();
			return TRUE;
		}
};



/******************************************************************************
class WBA_CommandUnknown : public WBA_CommandHandler

	This handler has no real difference over the message in the main routine.
	It's purpose here is as an example.

******************************************************************************/
class WBA_CommandUnknown : public WBA_CommandHandler
	{
	public:
		WBA_CommandUnknown(void)
			{
			names.AppendElement(WBA_DEFAULT_COMMAND);
			}
		WBA_CommandUnknown(WBA_Shell *shell) : WBA_CommandHandler(shell)
			{
			names.AppendElement(WBA_DEFAULT_COMMAND);
			}

		const char * GetUsage(void) const { return ""; }
		const char * GetDescription(void) const
						{ return "for commands without a handler"; }

		GPL_Boolean HandleCommand(void)
			{
			GPL_String command=arguments.GetElement(0);

			parent->Printf("'%s' not recognized",command.GetData());
			return TRUE;
			}
	};



class WBA_CommandPrompt : public WBA_CommandHandler
{
	public:
		WBA_CommandPrompt(void)
		{
			names.AppendElement("prompt");
		}
		WBA_CommandPrompt(WBA_Shell *shell) : WBA_CommandHandler(shell)
		{
			names.AppendElement("prompt");
		}
		const char * GetUsage(void) const { return "<normal|expect|set [text]>"; }
		const char * GetDescription(void) const { return "set prompt mode or text"; }
		GPL_Boolean HandleCommand(void)
		{
			if(arguments.GetCount() < 2)
			{
				return FALSE;
			}
			if (arguments.GetElement(1) == "normal")
			{
				parent->SetPromptMode(WBA_PROMPTMODE_NORMAL);
			}
			if (arguments.GetElement(1) == "expect")
			{
				parent->SetPromptMode(WBA_PROMPTMODE_EXPECT);
			}
			if (arguments.GetElement(1) == "set")
			{
				const unsigned long n = arguments.GetCount();
				if (2 < n)
				{
					const GPL_Array<GPL_String> strings = arguments.GetRange(2,n-1);
					const GPL_String text = GPL_String::Join(" ", strings);
					parent->SetPrompt(text);
				}
			}
			return TRUE;
		}
};

class WBA_CommandEchoMode : public WBA_CommandHandler
{
	public:
		WBA_CommandEchoMode(void)
		{
			names.AppendElement("echomode");
		}
		WBA_CommandEchoMode(WBA_Shell *shell) : WBA_CommandHandler(shell)
		{
			names.AppendElement("echomode");
		}
		const char * GetUsage(void) const { return "<on|off|none>"; }
		const char * GetDescription(void) const { return "set echo mode"; }
		GPL_Boolean HandleCommand(void)
		{
			if (arguments.GetCount() != 2)
			{
				return FALSE;
			}
			if (arguments.GetElement(1) == "on")
			{
				parent->SetEchoMode(TRUE);
				parent->SetInputEchoMode(TRUE);
			}
			else if (arguments.GetElement(1) == "off")
			{
				parent->SetEchoMode(FALSE);
				parent->SetInputEchoMode(TRUE);
			}
			else if (arguments.GetElement(1) == "none")
			{
				parent->SetEchoMode(FALSE);
				parent->SetInputEchoMode(FALSE);
			}
			else
			{
				return FALSE;
			}
			return TRUE;
		}
};

class WBA_CommandInterpretMode : public WBA_CommandHandler
{
	public:
		WBA_CommandInterpretMode(void)
		{
			names.AppendElement("interpretmode");
			lineInterpreter.SetSource(WBA_APP->GetEnvironment());
			lineInterpreter.SetExecutor(&executor);
		}
		WBA_CommandInterpretMode(WBA_Shell *shell) : WBA_CommandHandler(shell)
		{
			names.AppendElement("interpretmode");
			lineInterpreter.SetSource(WBA_APP->GetEnvironment());
			lineInterpreter.SetExecutor(&executor);
		}
		const char * GetUsage(void) const { return "<on|off>"; }
		const char * GetDescription(void) const { return "set interpret mode"; }
		GPL_Boolean HandleCommand(void)
		{
			if (arguments.GetCount() != 2)
			{
				return FALSE;
			}
			if (arguments.GetElement(1) == "on")
			{
				parent->SetLineInterpreter(&lineInterpreter);
			}
			else if (arguments.GetElement(1) == "off")
			{
				parent->SetLineInterpreter((WBA_LineInterpreter *) 0);
			}
			else
			{
				return FALSE;
			}
			return TRUE;
		}

	protected:
		WBA_Executor executor;
		WBA_BourneShellLineInterpreter lineInterpreter;
};

class WBA_CommandEcho : public WBA_CommandHandler
{
	public:
		WBA_CommandEcho(void)
		{
			names.AppendElement("echo");
		}
		WBA_CommandEcho(WBA_Shell *shell) : WBA_CommandHandler(shell)
		{
			names.AppendElement("echo");
		}
		const char * GetUsage(void) const { return "[text ...]"; }
		const char * GetDescription(void) const { return "print text"; }
		GPL_Boolean HandleCommand(void)
		{
			register const unsigned long n = arguments.GetCount();
			if (1 < n)
			{
				const GPL_Array<GPL_String> strings = arguments.GetRange(1, n - 1);
				const GPL_String text = GPL_String::Join(" ", strings);
				parent->Printf("%s", text.GetData());
			}
			return TRUE;
		}
};

class WBA_CommandHelp : public WBA_CommandHandler
{
	public:
		WBA_CommandHelp(void)
		{
			names.AppendElement("help");
		}
		WBA_CommandHelp(WBA_Shell *shell) : WBA_CommandHandler(shell)
		{
			names.AppendElement("help");
		}
		const char * GetUsage(void) const { return "[command command ...]"; }
		const char * GetDescription(void) const { return "print help for command(s)"; }
		GPL_Boolean HandleCommand(void)
		{
			if(arguments.GetCount() == 1)
			{
				parent->PrintCommands();
			}
			else
			{
				unsigned long i;
				for(i = 1;i < arguments.GetCount(); i++)
				{
					WBA_CommandHandler *handler;
					parent->GetCommands()->ToHead();
					GPL_Boolean printed = FALSE;
					while(!printed &&(handler = parent->GetCommands()->PostIncrement()))
					{
						unsigned long j;
						for(j = 0; j < handler->names.GetCount();j++)
						{
							if(handler->names.GetElement(j) == arguments.GetElement(i))
							{
								parent->PrintHandlerHelp(handler);
								printed = TRUE;
								break;
							}
						}
					}
				}
			}
			return TRUE;
		}
};

class WBA_CommandMap : public WBA_CommandHandler
{
	private:
		GPL_String	map;
	public:
		WBA_CommandMap(const GPL_String &name, const GPL_String &map_txt)
		{
			names.AppendElement(name);
			map = map_txt;
		}
		const char * GetUsage(void) const { return "this is an alias\n  use $* for all args or $1 for first and so on"; }
		const char * GetDescription(void) const { return (const char *)map; }
		GPL_Boolean HandleCommand(void)
		{
			unsigned long i;
			unsigned long l;
			GPL_String input_line;
			for(i=0;i<map.GetLength();i++)
			{
				switch(map[i])
				{
					case '$':
						i++;
						if(i >= map.GetLength()) break;
						if(map[i] >= '0' && map[i] <= '9')
						{
							l = (long)(map[i] - '0');
							if(l < arguments.GetCount())
							{
								input_line.Append(arguments.GetElement(l));
								input_line.Append(" ");
							}
						}
						else if(map[i] == '*')
						{
							for(l=1;l<arguments.GetCount();l++)
							{
								input_line.Append(arguments.GetElement(l));
								input_line.Append(" ");
							}
						}
						break;
					default:
						input_line.Append((char)(map[i]));
						break;	
				}
			}
			GPL_List<GPL_String> stringlist;
			GPL_String s;
			GPL_StringTokenizer tokenizer;
			tokenizer.Reset(input_line, ";");
			for(s = tokenizer.Next(); !(s.IsNull()); s = tokenizer.Next())
			{
				stringlist.Append(s);
			}
			long a;
			for(a=(long)(stringlist.GetCount()-1);a>=0;a--)
			{
				parent->AddCommandInput(stringlist[a]);
			}
			return TRUE;
		}
};


class WBA_CommandSetMap : public WBA_CommandHandler
{
	public:
		WBA_CommandSetMap(void)
			{
			names.AppendElement("map");
			}
		WBA_CommandSetMap(WBA_Shell *shell) : WBA_CommandHandler(shell)
			{
			names.AppendElement("map");
			}

		const char * GetUsage(void) const { return "mapname text"; }
		const char * GetDescription(void) const { return "set a map"; }
		GPL_Boolean HandleCommand(void)
		{
			unsigned long i;
			GPL_String s;
			if(arguments.GetCount() < 2) return FALSE;
			WBA_CommandHandler *handler;
			handler = parent->FindHandler(arguments.GetElement(1));
			if(handler)
			{
				parent->RemoveCommand(handler);
				delete handler;
			}
			for(i=2;i<arguments.GetCount();i++)
			{
				s.Append(arguments.GetElement(i));
				s.Append(" ");
			}
			parent->AddCommand(new WBA_CommandMap(arguments.GetElement(1),s));
			return TRUE;
		}
};

class WBA_CommandUnmap : public WBA_CommandHandler
{
	public:
		WBA_CommandUnmap(void)
			{
			names.AppendElement("unmap");
			}
		WBA_CommandUnmap(WBA_Shell *shell) : WBA_CommandHandler(shell)
			{
			names.AppendElement("unmap");
			}

		const char * GetUsage(void) const { return "mapname"; }
		const char * GetDescription(void) const { return "unset a map"; }
		GPL_Boolean HandleCommand(void)
		{
			GPL_String s;
			if(arguments.GetCount() < 2) return FALSE;
			WBA_CommandHandler *handler;
			handler = parent->FindHandler(arguments.GetElement(1));
			if(handler)
			{
				parent->RemoveCommand(handler);
				delete handler;
			}
			return TRUE;
		}
};

class WBA_CommandInclude : public WBA_CommandHandler
{
#define WBA_INCLUDE_MAXLINELEN 256
	private:
		char line[WBA_INCLUDE_MAXLINELEN];
	public:
		WBA_CommandInclude(void)
			{
			names.AppendElement("include");
			}
		WBA_CommandInclude(WBA_Shell *shell) : WBA_CommandHandler(shell)
			{
			names.AppendElement("include");
			}

		const char * GetUsage(void) const { return "<filename>"; }
		const char * GetDescription(void) const { return "include commands from a file"; }
		long HandleCommand()
		{
			if(arguments.GetCount() != 2) return FALSE;
			GPL_File file;
			if(!file.Open((const char *)(arguments.GetElement(1)),OSD_FILE_READ))
			{
				// Commented this out due to default input file functionality
				//wbaLog("WBA WARNING", "could not open %s", (const char *)(arguments.GetElement(1)));
			}
			else
			{
				GPL_List<GPL_String> stringlist;
				while(0 <= (file.ReadLine(line, WBA_INCLUDE_MAXLINELEN)))
				{
					stringlist.Append(line);
				}
				long i;
				for(i=(long)(stringlist.GetCount()-1);i>=0;i--)
				{
					parent->AddCommandInput(stringlist[i]);
				}
				file.Close();
			}
			return TRUE;
		}
};

#endif /* WBA_SHELL_H */
