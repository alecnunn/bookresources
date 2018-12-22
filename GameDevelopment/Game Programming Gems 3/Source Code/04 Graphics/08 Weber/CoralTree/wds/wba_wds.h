/******************************************************************************

Coral Tree wds/wba_wds.h
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


/******************************************************************************
class	GPL_INIT_CLASSNAME(WDS) : public GPL_INIT_CLASSNAME(GPL)

	macro'd definition for WBA/GPL logging

******************************************************************************/
class GPL_INIT_CLASSNAME(WDS) : public GPL_INIT_CLASSNAME(EW)
	{
		GPL_INIT_CONSTRUCT(WDS);

	public:

		void	Initialize(void)
					{
					gplLoggerAddModule("WDS");
					};

		void	Finalize(void)
					{
					};
	};

static GPL_INIT_CLASSNAME(WDS) WBA_MODULE_INIT_INSTANCE(WDS);

class WBA_AWARECLASS(WDS)
	{
	public:
			WBA_AWARECLASS(WDS)(void)
				{
				WBA_ARGS.SetQuiet("USER",		"nobody",	"user name",0);
				WBA_ARGS.SetQuiet("HOST",		"localhost","host name",0);
				WBA_ARGS.SetQuiet("HOSTTYPE",	"unknown",	"machine type",0);
				WBA_ARGS.SetQuiet("HOME",		"/",	"user's home directory",0);
//				WBA_ARGS.SetQuiet("PATH",		"/bin",		"command path",0);

				WBA_ARGS.Set("WDS_LOOK_PATH",".:~",
								"path for finding look files",0);


//				wbaPreEnvLoad("WDS");
				}

virtual		~WBA_AWARECLASS(WDS)(void)
				{
//				wbaPostEnvLoad("WDS");
				}
	};



/******************************************************************************
class	WDS_NullApplication: public WBA_Application

	provides empty operations for WBA_Application's pure virtuals

******************************************************************************/
class WDS_NullApplication: public WBA_Application
	{
	public:
						WDS_NullApplication(): WBA_Application(this) {};

virtual					~WDS_NullApplication(void)			{};

//virtual	const char	*GetVersion(void)			const	{ return "0.0"; };

virtual	const	char	*GetName(void)				const	{ return "name"; };
virtual	const	char	*GetParameterUsage(void)	const	{ return ""; };

				void	MakeAware(const int argc,const char **argv,
															const char **envp)
							{
							GPL_Aware gpl;
							WBA_Aware wba;
							EW_Aware ew;
							WDS_Aware wds;

							WBA_ENV.LoadEnvVariables(envp,FALSE);
							WBA_ENV.LoadCommandLine(argc,argv,TRUE);
							};

	protected:

virtual			void	Process(void)	{};
	};

