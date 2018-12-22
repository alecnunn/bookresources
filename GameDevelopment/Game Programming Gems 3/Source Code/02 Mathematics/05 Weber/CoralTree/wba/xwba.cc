
/******************************************************************************

Coral Tree wba/xwba.cc
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

/******************************************************************************
class WBA_Test
******************************************************************************/
class WBA_Test : public WBA_Shell
{
	public:
		WBA_Test() : WBA_Shell(this) {}
		virtual ~WBA_Test(void) {}

		virtual const char * GetName(void) const { return "WBA_Test"; }
		virtual const char * GetParameterUsage(void) const { return "parameters are not used"; }

		virtual long Initialize(void)
		{
			SetPrompt("xwba> ");
			SetLineInterpreter(new WBA_BourneShellLineInterpreter());
			WBA_LOG("WBA LOG", "WBA_Test::Initialize");
			return TRUE;
		}
		virtual void Finalize(void)
		{
			WBA_LOG("WBA LOG", "WBA_Test::Finalize");
		}

	protected:
		virtual void Process(void)
		{
		}
		virtual void Process1(void)
		{
			WBA_LOG("WBA TWICE", "this is our name [%s]", GetName());
			WBA_LOG("spurious type", "this is some random text");
			WBA_LOG("WBA LOG","WBA_Test::Process");
			WBA_LOG("WBA LOG","%s called as %s", GetName (),
				environment->GetProgramName().GetData());
			WBA_LOG("WBA LOG","%s has basename %s", GetName (),
				environment->GetProgramBasename ().GetData ());
			WBA_Argument *arg;
			if(arg = WBA_ARGS.Find("WBA_DEBUG"))
			{
				WBA_LOG("WBA LOG","WBA_DEBUG=%s", (const char *)arg->GetValue());
				if(arg->GetLong() > 0) 
					WBA_LOGGER.SetType("WBA DEBUG", 1, "Test Program Output");
				if(arg->GetLong() < 0)
				{
					WBA_LOG("WBA ERROR", "invalid debug level of %d", arg->GetLong());
				}
			}
			if(arg = WBA_ARGS.Find("WBA_PRINT_HELP"))
			{
				WBA_LOG("WBA LOG","WBA_PRINT_HELP=%s", (const char *)arg->GetValue());
				if(arg->GetLong() > 0) Printf("%s", GetUsage());
			}
			if(arg = WBA_ARGS.Find("WBA_PRINT_VERSION"))
			{
				WBA_LOG("WBA LOG","WBA_PRINT_VERSION=%s", (const char *)arg->GetValue());
				if(arg->GetLong() > 0) Printf("%s", GetVersion().ToString().GetData());
			}
			WBA_LOG("WBA DEBUG", "so far so good");
		}
};

/******************************************************************************
class WBA_DeliveryExit
******************************************************************************/
class WBA_DeliveryExit : public WBA_Delivery
{
	public:
		virtual GPL_Boolean Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text)
		{
			GPL_String s;s=filename;s=tname;s=text;long l;l=linenum;
			WBA_APP->Exit(); 
			return FALSE;
		}
};

/******************************************************************************
class WBA_CommandPrintStuff
******************************************************************************/
class WBA_CommandPrintStuff : public WBA_CommandHandler
{
	public:
		WBA_CommandPrintStuff(void)
		{
			names.AppendElement("printstuff");
			names.AppendElement("PRINT");
		}
		const char * GetUsage(void) const { return "[arguments]"; }
		const char * GetDescription(void) const { return "prints given arguments"; }
		GPL_Boolean HandleCommand(void)
		{
			long i;
			WBA_LOG("WBA LOG","argument count is %d", arguments.GetCount());
			if (arguments.GetCount() == 1)
			{
				return FALSE;
			}
			for (i = 0; i < arguments.GetCount(); i++)
			{
				WBA_LOG("WBA LOG","%s",(const char *)(arguments.GetElement(i)));
			}

			return TRUE;
		}
};

/******************************************************************************
class WBA_CommandPrintUser
******************************************************************************/
class WBA_CommandPrintUser : public WBA_CommandHandler
{
	public:
		WBA_CommandPrintUser(void)
		{
			names.AppendElement("whoami");
		}
		const char * GetUsage(void) const { return ""; }
		const char * GetDescription(void) const { return "prints current user"; }
		GPL_Boolean HandleCommand(void)
		{
			if (!osdUserGet(OSD_GLOBAL_BUFFER[7], OSD_GLOBAL_BUFFER_SIZE - 1))
			{
				WBA_APP->Printf("Unable to get user name");
			}
			else
			{
				WBA_APP->Printf("%s", OSD_GLOBAL_BUFFER[7]);
			}
			return TRUE;
		}
};

/******************************************************************************
class WBA_CommandPrintBoolean
******************************************************************************/
class WBA_CommandPrintBoolean : public WBA_CommandHandler
{
	public:
		WBA_CommandPrintBoolean(void)
		{
			names.AppendElement("toggle");
		}
		const char * GetUsage(void) const { return "<boolean>"; }
		const char * GetDescription(void) const { return "prints boolean value"; }
		GPL_Boolean HandleCommand(void)
		{
			GPL_Boolean boolean = FALSE;
			if (!WBA_Shell::ParseBoolean(arguments.GetElement(1), boolean))
			{
				WBA_APP->Printf("Unable to parse Boolean");
			}
			else
			{
				WBA_APP->Printf("%s", (!boolean ? "FALSE" : "TRUE"));
			}
			return TRUE;
		}
};

/******************************************************************************
class WBA_TestGetPassword 
******************************************************************************/
class WBA_TestGetPassword : public WBA_CommandHandler
{
	public:
		WBA_TestGetPassword(void)
		{
			names.AppendElement("su");
		}
		WBA_TestGetPassword(WBA_Shell *shell) : WBA_CommandHandler(shell)
		{
			names.AppendElement("su");
		}
		virtual ~WBA_TestGetPassword(void) {}
		const char *GetUsage(void) const { return ""; }
		const char *GetDescription(void) const { return "get the user's password"; }
		virtual GPL_Boolean HandleCommand(void)
		{
			GPL_String password;
			WBA_APP->Prompt("Password: ", password, password, TRUE);
			WBA_APP->Printf("Your password is '%s'", password.GetData());
			return TRUE;
		}
};

/******************************************************************************
int main(const int argc, const char **argv, const char **envp)
******************************************************************************/
int main(const int argc, const char **argv, const char **envp)
{
	/* APP CREATION */
	WBA_Test ta;
	WBA_Test *this_app;
	//this_app = new WBA_Test();
	this_app = &ta;

	/* PRE ENV LOAD */
	WBA_ARGS.Set("WBA_DEBUG",         (char *)NULL, "debugging level", 2, "d", "debug");
	WBA_ARGS.Set("WBA_PRINT_HELP",    "0", "print help",      1, "h");
	WBA_ARGS.Set("WBA_THIS_IS_AN_EXTRA_LONG_ARG", "lhsdkjhskdjfh", "blah blah sdjfhksjdh sjdkh fksdh fksfjksdh f",   5, "vs", "kdsj", "kjduf", "qqqqq", "s");
	WBA_ARGS.Set("WBA_BLAT", "lhsdkjhskdjfh", "blah blah sdjfhksjdh sjdkh fksdh fksfjksdh fjjjjjjjj jjjsssssss ddddddddd fffffffff gggggggg hhhhhhhhhh jjjjjjjjjj kkkkkkk",   5, "one", "two", "three", "four", "five");
	WBA_ARGS.Set("WBA_DDDDD", "djfh", "blah blah sdjfhksjdh sjdkh fksdh fksfjksdh fjjjjjjjj jjjsssssss ddddddddd fffffffff gggggggg hhhhhhhhhh jjjjjjjjjj kkkkkkk",   1, "hi");
	WBA_ARGS.Set("WBA_PRINT_VERSION", "0", "print version",   1, "v");

	{
		/* ENV LOAD */
		GPL_Aware gpl;
		WBA_Aware wba;

		/* now load the environment in ascending priority order */
		WBA_ENV.LoadFile("file.cfg", TRUE); // load everything defined in file
		WBA_ENV.LoadFile("/tmp", TRUE); // test bad filename
		WBA_ENV.LoadEnvVariables(envp, FALSE); // load only relevent env variables
		if(!WBA_ENV.LoadCommandLine(argc, argv, TRUE)) // load all arguments
		{
			this_app->Printf("%s", this_app->GetUsage());
			this_app->Exit();
		}
	}

	WBA_LOGGER.SetDelivery("Test Program Output",    new WBA_DeliveryPrintf() );
	WBA_LOGGER.SetDelivery("Other",                  new WBA_DeliveryPrintf() );
	WBA_LOGGER.SetDelivery("Exit",                   new WBA_DeliveryExit()   );
	WBA_LOGGER.SetDelivery("NoOp",                   new WBA_DeliveryNoop()   );
	//WBA_LOGGER.SetType("WBA LOG",    1, "Test Program Output"                 );
	WBA_LOGGER.SetType("WBA TWICE",  2, "Test Program Output",   "Other"      );
	WBA_LOGGER.SetType("WBA ERROR",  2, "Test Program Output",   "Exit"       );

	WBA_LOGGER.SetDelivery("Test Log File", new GPL_DeliveryFile(GPL_File::GetFullPathName("wba.log")));
	GPL_StringArray dnames = WBA_LOGGER.GetType("WBA LOG");
	dnames.AppendElement("Test Log File");
	WBA_LOGGER.SetType("WBA LOG", dnames);

	WBA_LOG("GPL LOG","using gpl's log channel");

	wbaLog("WBA","testing wba's internal logging");

	wbaLog("WBA LOG", "logger test");

	this_app->Printf("WBA_DEBUG => '%s'", WBA_ARGS.Find("WBA_DEBUG")->GetValue().GetData());
	if (WBA_ARGS.Find("WBA_DEBUG")->IsTrue())
	{
		this_app->Printf("Debugging enabled");
	}
	else
	{
		this_app->Printf("Debugging disabled");
	}

	this_app->Printf("-----------------------------------");
	this_app->Printf("%s", this_app->DumpEnvironment());
	this_app->Printf("-----------------------------------");

	char **env_ptr;
	env_ptr = WBA_ARGS.BuildEnvp();
	long a=0;
	while(env_ptr[a])
	{
		this_app->Printf("%s", env_ptr[a]);
		osdFree(env_ptr[a]);
		a++;
	}
	osdFree(env_ptr);

	/* SHELL SETUP */
	this_app->AddCommand(new WBA_CommandPrintStuff());
	this_app->AddCommand(new WBA_CommandPrintUser());
	this_app->AddCommand(new WBA_CommandPrintBoolean());
	this_app->AddCommand(new WBA_CommandQuit(this_app));
	this_app->AddCommand(new WBA_CommandHelp(this_app));
	this_app->AddCommand(new WBA_CommandEcho(this_app));
	this_app->AddCommand(new WBA_CommandEchoMode(this_app));
	this_app->AddCommand(new WBA_CommandInterpretMode(this_app));
	this_app->AddCommand(new WBA_CommandSetMap());
	this_app->AddCommand(new WBA_TestGetPassword());

	/* GO */
	return this_app->Main();
}
