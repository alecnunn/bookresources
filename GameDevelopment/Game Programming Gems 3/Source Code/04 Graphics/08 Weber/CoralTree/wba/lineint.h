#ifndef WBA_LINEINT_H
#define WBA_LINEINT_H

/******************************************************************************

Coral Tree wba/lineint.h
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
#include <wba/env.h>
#include <wba/executor.h>

/***********************************************************************
class WBA_LineInterpreter
***********************************************************************/
class WBA_LineInterpreter
{
	public:
		WBA_LineInterpreter(void);
		virtual ~WBA_LineInterpreter(void) {}

		WBA_ValueSource *GetSource(void) { return source; }
		WBA_Executor *GetExecutor(void) { return executor; }

		void SetSource(WBA_ValueSource *s) { source = s; }
		void SetExecutor(WBA_Executor *e) { executor = e; }

		virtual GPL_Boolean Interpret(const GPL_String & line,
										GPL_Array<GPL_String> & arguments);

	private:
		WBA_ValueSource *source;
		WBA_Executor *executor;
};

#define	WBA_LINEINTERPRETER_NONE		0x00
#define	WBA_LINEINTERPRETER_NOTEMPTY	0x01
#define	WBA_LINEINTERPRETER_SINGLE		0x02
#define	WBA_LINEINTERPRETER_DOUBLE		0x04
#define	WBA_LINEINTERPRETER_ESCAPED		0x08
#define	WBA_LINEINTERPRETER_VARIABLE	0x10
#define	WBA_LINEINTERPRETER_EXPLICIT	0x20
#define	WBA_LINEINTERPRETER_EXECUTABLE	0x40

/***********************************************************************
class WBA_BourneShellLineInterpreter
***********************************************************************/
class WBA_BourneShellLineInterpreter : public WBA_LineInterpreter
{
	public:
		WBA_BourneShellLineInterpreter(void) {}
		virtual ~WBA_BourneShellLineInterpreter(void) {}

		virtual GPL_Boolean Interpret(const GPL_String & line,
										GPL_Array<GPL_String> & arguments);

	protected:
		GPL_Boolean InterpretVariable(const GPL_String & line,
										const unsigned long i,
										unsigned long & flags,
										unsigned long & start,
										GPL_String & string);
		GPL_String InterpretCommand(const GPL_String & string,
									unsigned long & flags);
		void AppendString(unsigned long & flags, GPL_String & string,
							GPL_Array<GPL_String> & arguments);
};

#endif /* WBA_LINEINT_H */
