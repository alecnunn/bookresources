#ifndef WBA_ENV_H
#define WBA_ENV_H

/******************************************************************************

Coral Tree wba/env.h
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

#include <wba/argument.h>

/***********************************************************************
class WBA_ValueSource
***********************************************************************/
class WBA_ValueSource
{
	public:
		virtual GPL_Boolean GetValue(const GPL_String & name,
										GPL_String & value)
		{
			GPL_String n = name;n = n;
			value = value;
			return FALSE;
		}
};

#define WBA_PARAM	0
#define WBA_MINUS	1
#define WBA_PLUS	2

/***********************************************************************
class WBA_Environment
***********************************************************************/
class WBA_Environment : public WBA_ValueSource
{
	private:
		WBA_ArgumentTable				*argumentTable;
		GPL_Array<GPL_String>			parameters;
		GPL_String						programName;
		GPL_String						programBasename;
	public:
		WBA_Environment(void);
virtual	~WBA_Environment(void);

		WBA_ArgumentTable *GetArgumentTable(void){ return argumentTable; }
		GPL_Array<GPL_String> *GetParameters(void){ return &parameters; }
		const GPL_String &GetProgramName(void) const { return programName; }
		const GPL_String &GetProgramBasename(void) const { return programBasename; }
		virtual GPL_Boolean GetValue(const GPL_String & name,
										GPL_String & value);

		GPL_Boolean LoadCommandLine(const int argc, const char **argv,
									GPL_Boolean mode = TRUE);
		void LoadEnvVariables(const char **envp, GPL_Boolean mode = TRUE);
		GPL_Boolean LoadFile(const GPL_String & filename,
								GPL_Boolean mode = TRUE);

		void SubstituteVariables(GPL_String &string);

		void Print(void);

	protected:
		static GPL_String Strip(const GPL_String & s);
		
};

extern WBA_Environment *WBA_GLOBAL_ENV;

class GPL_INIT_CLASSNAME(WBA) : public GPL_INIT_CLASSNAME(GPL)
{
	GPL_INIT_CONSTRUCT(WBA);
	public:
		void Initialize(void)
		{
			gplLoggerAddModule("WBA");
			WBA_GLOBAL_ENV = new WBA_Environment();
		}
		void Finalize(void)
		{
			delete WBA_GLOBAL_ENV;
			WBA_GLOBAL_ENV = NULL;
		}
};
static GPL_INIT_CLASSNAME(WBA) WBA_INIT;
GPL_INIT_DECLARATION(WBA);

#endif /* WBA_ENV_H */
