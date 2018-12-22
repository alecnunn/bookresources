/******************************************************************************

Coral Tree ew/wba_ew.h
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
class	GPL_INIT_CLASSNAME(EW) : public GPL_INIT_CLASSNAME(GPL)

	macro'd definition for WBA/GPL logging

******************************************************************************/
class GPL_INIT_CLASSNAME(EW) : public GPL_INIT_CLASSNAME(WBA)
	{
		GPL_INIT_CONSTRUCT(EW);

	public:

		void	Initialize(void)
					{
					gplLoggerAddModule("EW");

					// override default behavior to exit on error
					GPL_LOGGER.SetType("EW ERROR",1,"BUILTIN_OSD_OUTPUT_TERSE");
					};

		void	Finalize(void)
					{
					};
	};
static GPL_INIT_CLASSNAME(EW) WBA_MODULE_INIT_INSTANCE(EW);

class WBA_AWARECLASS(EW)
	{
	public:
			WBA_AWARECLASS(EW)(void)
				{
				wbaPreEnvLoad("EW");

#if TRUE
				// empty default, can use CORAL_LOG_LEVEL
			    WBA_ARGS.SetQuiet("EW_LOG_LVL","","EW logging level",0);
#else
			    WBA_ARGS.SetQuiet("EW_LOG_LVL",		"1",
										"ew log debug level (0-3)",0);
			    WBA_ARGS.SetQuiet("EW_WARNING_LVL",	"1",
										"ew warning debug level (0-3)",0);
			    WBA_ARGS.SetQuiet("EW_ERROR_LVL",	"1",
											"ew error debug level (0-3)",0);
#endif

			    WBA_ARGS.SetQuiet("EW_COLOR_TEXT",	"1",
											"allow log printing in color",0);

				}

virtual		~WBA_AWARECLASS(EW)(void)
				{
				wbaPostEnvLoad("EW");
				}
	};



/******************************************************************************
class	EW_Application : public WBA_Application

******************************************************************************/
class EW_Application : public WBA_Application, public EW_WorkFunction
	{
	private:
		EW_EventContext *context;

	public:
		EW_Application(EW_Application *current) : WBA_Application(current) {};

		void			SetEventContext(EW_EventContext *set)
							{ context=set; };
		EW_EventContext *GetEventContext(void)
							{ return context; };

virtual	int				Main(void);

virtual	void			CallWorkFunction(void)
							{
							Process();
							};
	};
