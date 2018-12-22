/******************************************************************************

Coral Tree wba/log.cc
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

/* WARNING!! Be sure to edit gplLoggerAddModule to match WBA_LOG_LEVEL_DEFAULT*/
/*           if you change WBA_LOG_LEVEL_DEFAULT                              */
/*           This is so non 'aware' apps still have the default behavior      */
#define	WBA_LOG_LEVEL_DEFAULT	"nwe"

GPL_Delivery *wbaWarningDelivery = (GPL_Delivery *) GPL_BAD_ADDRESS;
GPL_Delivery *wbaErrorDelivery = (GPL_Delivery *) GPL_BAD_ADDRESS;
static GPL_Boolean wbaEnvInitialized = FALSE;

static GPL_String wbaGetModuleValue(const GPL_String & module);
// 20000606 jpweber: removed static for external access
void wbaInstallModuleDeliveries(const GPL_String & module,
										const GPL_String & value);
static void wbaInstallModeDeliveries(const GPL_String & type,
										const GPL_Character character);

/******************************************************************************
void	wbaPreEnvLoad(const char *module)

******************************************************************************/
void wbaPreEnvLoad(const char *module)
{
	if (!wbaEnvInitialized)
	{
		// Install the crash delivery.
		// TRUE activates core dump capability
		WBA_LOGGER.SetDelivery("CORAL_INTENTIONAL_CRASH",
								new GPL_DeliveryExit(TRUE));

		if ((GPL_Delivery *) GPL_BAD_ADDRESS == wbaWarningDelivery)
		{
			WBA_LOGGER.SetDelivery("BUILTIN_WARNING",
									new GPL_DeliveryOsdOutputTerse());
		}
		else
		{
			WBA_LOGGER.SetDelivery("BUILTIN_WARNING", wbaWarningDelivery);
		}

		if ((GPL_Delivery *) GPL_BAD_ADDRESS == wbaErrorDelivery)
		{
			WBA_LOGGER.SetDelivery("BUILTIN_ERROR",
									new GPL_DeliveryOsdOutput());
		}
		else
		{
			WBA_LOGGER.SetDelivery("BUILTIN_ERROR", wbaErrorDelivery);
		}

		WBA_ARGS.SetQuiet("CORAL_LOG_LVL", WBA_LOG_LEVEL_DEFAULT,
							"Coral general logging level", 0);
		WBA_ARGS.SetQuiet("CORAL_LOGFILE", "", "logging file", 0);

		wbaEnvInitialized = TRUE;
	}

	GPL_String s = module;
	s.Append("_LOG_LVL");

	GPL_String d = "Coral ";
	d.Append(module);
	d.Append(" logging level");
	WBA_ARGS.SetQuiet(s, "", d, 0);
}

/******************************************************************************
void	wbaPostEnvLoad(const char *module)

	for MOD of WBA,EW,etc

	by default,
		CORAL_LOG_LVL and MOD_LOG_LVL are ""

	CORAL_LOG_LVL used except when MOD_LOG_LVL is left as ""

	if >=3 characters, first 3 chars used as log, warning, and error settings
	if 1 or 2 characters, first char used for all three
	if both are "", the default is used for all three (WBA_LOG_LEVEL_DEFAULT)

	recognized values:
		n,0=noop
		t,1=terse
		p,2=print verbose
		x,3=exit
		C  =CRASH (abort() causing core dump or debugger break)

	note that exits and crashes automatically imply and verbose print

	for example, the arg
		-CORAL_LOG_LVL ntx
		causes all mods, by default, to ignore logs, print warnings tersely,
		and print and exit on errors

	another arg
		-EW_LOG_LVL tpC
		would only apply to EW, overriding any CORAL_LOG_LVL, and print logs
		tersely, warnings verbosely, and intentional cause a core dump
		on errors

	if CORAL_LOGFILE is defined, all output is sent to file as well

******************************************************************************/
void wbaPostEnvLoad(const char *module)
{
	// If a log file has been specified...
	GPL_DeliveryFile *deliveryFile = (GPL_DeliveryFile *) GPL_BAD_ADDRESS;
	WBA_Argument *arg = WBA_ARGS.Find("CORAL_LOGFILE");
	if ((WBA_Argument *) GPL_BAD_ADDRESS != arg)
	{
		if (0 < arg->GetValue().GetLength())
		{
			// Install the log file delivery.
			deliveryFile = new GPL_DeliveryFile(arg->GetValue().GetData());
			WBA_LOGGER.SetDelivery("CORAL_LOG_FILE", deliveryFile);
		}
	}

	// PRE-CONDITION: CORAL_LOG_LVL must have a value.
	// Currently, wbaPreEnvLoad() guarantees this.
	// Install the general deliveries.
	wbaInstallModuleDeliveries("CORAL", wbaGetModuleValue("CORAL"));

	// Get the value for the module, using CORAL_LOG_LVL if no
	// value is defined.  This *MUST* happen since gplLoggerAddModule()
	// has already installed deliveries for the module's LOG,
	// WARNING, and ERROR types.  Otherwise, modules will not
	// properly default to the value of CORAL_LOG_LVL.
	GPL_String value = wbaGetModuleValue(module);
	if (0 == value.GetLength())
	{
		value = wbaGetModuleValue("CORAL");
	}

	// Install the module deliveries.
	wbaInstallModuleDeliveries(module, value);
}

static GPL_String wbaGetModuleValue(const GPL_String & module)
{
	// Determine the variable name.
	GPL_String name = module;
	name.Append("_LOG_LVL");

	// If the variable is defined...
	GPL_String value;
	WBA_Argument *arg = WBA_ARGS.Find(name);
	if ((WBA_Argument *) GPL_BAD_ADDRESS != arg)
	{
		value = arg->GetValue();
	}
	return value;
}

void wbaInstallModuleDeliveries(const GPL_String & module,
										const GPL_String & value)
{
	// If there are no data to parse...
	if (0 == value.GetLength())
	{
		// No further processing required.
		return;
	}

	// If the value is too short...
	GPL_String data = value;
	if (3 > data.GetLength())
	{
		// Fill it with the first character.
		const GPL_Character character = data.operator[](0);
		data = GPL_String(3, character);
	}

	// Install the log deliveries.
	GPL_String name = module;
	name.Append(" LOG");
	wbaInstallModeDeliveries(name, data.operator[](0));

	// Install the warning deliveries.
	name = module;
	name.Append(" WARNING");
	wbaInstallModeDeliveries(name, data.operator[](1));

	// Install the error deliveries.
	name = module;
	name.Append(" ERROR");
	wbaInstallModeDeliveries(name, data.operator[](2));
}

static void wbaInstallModeDeliveries(const GPL_String & type,
										const GPL_Character character)
{
	GPL_StringArray deliveries;

	// value: n,0=noop t,1=terse p,2=print x,3=exit C=CRASH
	switch(character)
	{
		// verbose
		case 'p':
			deliveries.AppendElement("BUILTIN_OSD_OUTPUT");
			break;

		// terse
		case 't':
			deliveries.AppendElement("BUILTIN_OSD_OUTPUT_TERSE");
			break;

		// quiet
		case 'n':
			deliveries.AppendElement("BUILTIN_NOOP");
			break;

		// CRASH! (core dump)
		case 'C':
			deliveries.AppendElement("BUILTIN_ERROR");
			deliveries.AppendElement("CORAL_INTENTIONAL_CRASH");
			break;

		// verbose & exit
		case 'x':
			deliveries.AppendElement("BUILTIN_ERROR");
			deliveries.AppendElement("BUILTIN_EXIT");
			break;

		// error
		case 'e':
			deliveries.AppendElement("BUILTIN_ERROR");
			break;

		// warning
		case 'w':
		default:
			deliveries.AppendElement("BUILTIN_WARNING");
			break;
	}

	// If this is not the no-op type...
	if ('0' != character && 'n' != character && '.' != character)
	{
		// If there is a log file delivery...
		if ((GPL_Delivery *) GPL_BAD_ADDRESS !=
			WBA_LOGGER.GetDelivery("CORAL_LOG_FILE"))
		{
			// Put the log file delivery after everything
			// but the exit and crash deliveries.
			deliveries.InsertElement("CORAL_LOG_FILE", 1);
		}
	}

	// if not the pass through character
	if ('.' != character) 
	{
		// Install the deliveries.
		WBA_LOGGER.SetType(type, deliveries);
	}
}
