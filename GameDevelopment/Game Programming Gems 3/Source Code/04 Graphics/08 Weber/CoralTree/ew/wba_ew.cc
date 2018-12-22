/******************************************************************************

Coral Tree ew/wba_ew.cc
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



#include "ew.h"



/******************************************************************************
int	EW_Application::Main(void)

	continueProcessing is currently ignored?

******************************************************************************/
int	EW_Application::Main(void)
	{
	// event context ??

	if (Initialize())
		{
		if(GetEventContext())
			{
			GetEventContext()->SetWorkFunction(this);
			GetEventContext()->BeginLoop();
			}
		else
			EW_PRINT(EW_APP,EW_ERROR,"EW_Application::Main() event context must be set");

		Finalize();
		}

	return exitCode;
	}
