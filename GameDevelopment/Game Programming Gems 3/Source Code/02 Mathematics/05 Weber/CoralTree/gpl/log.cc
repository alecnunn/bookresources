/******************************************************************************

Coral Tree gpl/log.cc
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

const char *GPL__FILE__;
long GPL__LINE__;
GPL_Logger *GPL_GLOBAL_LOGGER = (GPL_Logger *) GPL_BAD_ADDRESS;
long GPL_DELIVERY_OUTPUT_INDENT = 0;
OSD_Output gplLogGlobalOutput;
GPL_Boolean gplLogInitialized=FALSE;

GPL_INIT_DEFINITION(GPL);


/***********************************************************************
class GPL_Delivery
***********************************************************************/
GPL_Boolean GPL_Delivery::Deliver(const GPL_String & filename,
									const long linenum,
									const GPL_String & tname,
									const GPL_String & text)
{
	return FALSE;
}

GPL_String GPL_Delivery::VerboseImage (const GPL_String & typeName,
										const GPL_String & text,
										const GPL_Time & time,
										const GPL_String & fileName,
										const long lineNumber,
										const char dateSeparator,
										const char timeSeparator)
{
	GPL_String image;
	image.Printf("%s: %s err=%-4d %02d%c%02d%c%02d %02d%c%02d%c%02d \"%s\":%d",
					typeName.GetData(),
					text.GetData(),
					osdGetLastError(),
					time.GetYear() - 1900, dateSeparator,
					time.GetMonth() + 1, dateSeparator, time.GetDay() + 1,
					time.GetHour(), timeSeparator,
					time.GetMinute(), timeSeparator, time.GetSecond(),
					fileName.GetData(), lineNumber);
	return image;
}

GPL_String GPL_Delivery::TerseImage (const GPL_String & typeName,
										const GPL_String & text)
{
	GPL_String image;
	image.Printf ("%s: %s", typeName.GetData(), text.GetData());
	return image;
}

/***********************************************************************
class GPL_DeliveryPrintf
***********************************************************************/
/***********************************************************************
GPL_Boolean GPL_DeliveryPrintf::Deliver(const GPL_String & filename,
										const long linenum,
										const GPL_String & tname,
										const GPL_String & text)
***********************************************************************/
GPL_Boolean GPL_DeliveryPrintf::Deliver(const GPL_String & filename,
										const long linenum,
										const GPL_String & tname,
										const GPL_String & text)
{
	now.GetFromClock();
	fprintf(stderr, "%s\n",
			VerboseImage(tname, text, now, filename, linenum).GetData());
	fflush(stderr);
	return TRUE;
}

/***********************************************************************
class GPL_DeliveryPrintfTerse
***********************************************************************/
/***********************************************************************
GPL_Boolean GPL_DeliveryPrintfTerse::Deliver(const GPL_String & filename,
												const long linenum,
												const GPL_String & tname,
												const GPL_String & text)
***********************************************************************/
GPL_Boolean GPL_DeliveryPrintfTerse::Deliver(const GPL_String & filename,
												const long linenum,
												const GPL_String & tname,
												const GPL_String & text)
{
	fprintf(stderr, "%s\n", TerseImage(tname, text).GetData());
	fflush(stderr);
	return TRUE;
}

/***********************************************************************
class GPL_DeliveryFile
***********************************************************************/
/***********************************************************************
GPL_Boolean GPL_DeliveryFile::Deliver(const GPL_String & filename,
										const long linenum,
										const GPL_String & tname,
										const GPL_String & text)
***********************************************************************/
GPL_Boolean GPL_DeliveryFile::Deliver(const GPL_String & filename,
										const long linenum,
										const GPL_String & tname,
										const GPL_String & text)
{
	now.GetFromClock();
	return WriteString(VerboseImage(tname, text, now, filename,
									linenum, dsep, tsep));
}

/***********************************************************************
GPL_Boolean GPL_DeliveryFile::WriteString(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_DeliveryFile::WriteString(const GPL_String & string)
{
	if (faulty)
	{
		return FALSE;
	}
	if (!file.IsOpen())
	{
		if (!file.Open((const char *) fname, OSD_FILE_WRITE | OSD_FILE_SYNCH))
		{
			faulty = TRUE;
			gplLog("GPL WARNING", "could not open \'%s\'\n",
					(const char *) fname);
			return FALSE;
		}
	}

	if (-1 == file.Write(string.GetData(), string.GetLength()) ||
		-1 == file.Write("\n", 1))
	{
		gplLog("GPL WARNING", "could not write to \'%s\'\n",
				(const char *) fname);
		file.Close();
		faulty = TRUE;
		return FALSE;
	}
	return TRUE;
}

/***********************************************************************
class GPL_DeliveryFileTerse
***********************************************************************/
/***********************************************************************
GPL_Boolean GPL_DeliveryFileTerse::Deliver(const GPL_String & filename,
											const long linenum,
											const GPL_String & tname,
											const GPL_String & text)
***********************************************************************/
GPL_Boolean GPL_DeliveryFileTerse::Deliver(const GPL_String & filename,
											const long linenum,
											const GPL_String & tname,
											const GPL_String & text)
{
	return WriteString(TerseImage(tname, text));
}

/***********************************************************************
class GPL_DeliveryOsdOutput
***********************************************************************/
/***********************************************************************
GPL_Boolean GPL_DeliveryOsdOutput::Deliver(const GPL_String & filename,
											const long linenum,
											const GPL_String & tname,
											const GPL_String & text)
***********************************************************************/
GPL_Boolean GPL_DeliveryOsdOutput::Deliver(const GPL_String & filename,
											const long linenum,
											const GPL_String & tname,
											const GPL_String & text)
{
	now.GetFromClock();
	return WriteString(VerboseImage(tname, text, now, filename, linenum));
}

/***********************************************************************
class GPL_DeliveryOsdOutputTerse
***********************************************************************/
GPL_DeliveryOsdOutputTerse::~GPL_DeliveryOsdOutputTerse()
{
}

/***********************************************************************
GPL_Boolean GPL_DeliveryOsdOutputTerse::Deliver(const GPL_String & filename,
												const long linenum,
												const GPL_String & tname,
												const GPL_String & text)
***********************************************************************/
GPL_Boolean GPL_DeliveryOsdOutputTerse::Deliver(const GPL_String & filename,
												const long linenum,
												const GPL_String & tname,
												const GPL_String & text)
{
	return WriteString(TerseImage(tname, text));
}



/******************************************************************************
GPL_Boolean GPL_DeliveryOsdOutputTerse::WriteString(const GPL_String & string)
******************************************************************************/
GPL_Boolean GPL_DeliveryOsdOutputTerse::WriteString(const GPL_String & string)
{
	if (!gplLogInitialized)
	{
		gplLogInitialized = TRUE;
		osdOutputOpen(&gplLogGlobalOutput);
		osdOutputSetFlags(&gplLogGlobalOutput, OSD_OUT_STDERR);

		if ((char *) 0 != fn)
		{
			// NOTE: This file is never explicitly closed;
			// process termination should do that.
			osdFileOpen(&file, fn, OSD_FILE_WRITE);
		}
	}

	if (0 < GPL_DELIVERY_OUTPUT_INDENT)
	{
		GPL_String text(GPL_DELIVERY_OUTPUT_INDENT, ' ');
		text.Append(string);
		osdOutputPrintLine(&gplLogGlobalOutput, (char *) 0, string.GetData());
		if ((char *) 0 != fn)
		{
			osdFileWrite(&file, text.GetData(), text.GetLength());
			osdFileWrite(&file, "\n", 1);
		}
	}
	else
	{
		osdOutputPrintLine(&gplLogGlobalOutput, (char *) 0, string.GetData());
		if ((char *) 0 != fn)
		{
			osdFileWrite(&file, string.GetData(), string.GetLength());
			osdFileWrite(&file, "\n", 1);
		}
	}

	return TRUE;
}

/***********************************************************************
class GPL_DeliverySystemLog
***********************************************************************/
/***********************************************************************
GPL_DeliverySystemLog::GPL_DeliverySystemLog(const int e, const char *i)
***********************************************************************/
GPL_DeliverySystemLog::GPL_DeliverySystemLog(const int e, const char *i)
{
	eventType = e;
	if (0 == numberOfInstances++)
	{
		osdSystemLogInitialize(i);
	}
}

/***********************************************************************
GPL_DeliverySystemLog::~GPL_DeliverySystemLog(void)
***********************************************************************/
GPL_DeliverySystemLog::~GPL_DeliverySystemLog(void)
{
	if (0 == --numberOfInstances)
	{
		osdSystemLogFinalize();
	}
}

/***********************************************************************
GPL_Boolean GPL_DeliverySystemLog::Deliver(const GPL_String & filename,
											const long linenum,
											const GPL_String & tname,
											const GPL_String & text)
***********************************************************************/
GPL_Boolean GPL_DeliverySystemLog::Deliver(const GPL_String & filename,
											const long linenum,
											const GPL_String & tname,
											const GPL_String & text)
{
	// Record the event in the system log.
	if (-1 == osdSystemLogRecordEvent(eventType,
										TerseImage(tname, text).GetData()))
	{
		return FALSE;
	}
	return TRUE;
}

unsigned long GPL_DeliverySystemLog::numberOfInstances = 0;

/***********************************************************************
class GPL_Logger
***********************************************************************/
/***********************************************************************
GPL_Logger::GPL_Logger(void)
***********************************************************************/
GPL_Logger::GPL_Logger(void)
	: typeToDeliveryNames(GPL_String::HashFunction),
		deliveryNameToDelivery(GPL_String::HashFunction)
{
	defaultDelivery = new GPL_DeliveryPrintf();

	SetDelivery("BUILTIN_NOOP", new GPL_DeliveryNoop());
	SetDelivery("BUILTIN_OSD_OUTPUT", new GPL_DeliveryOsdOutput());
	SetDelivery("BUILTIN_OSD_OUTPUT_TERSE", new GPL_DeliveryOsdOutputTerse());
	SetDelivery("BUILTIN_PRINTF_TERSE", new GPL_DeliveryPrintfTerse());
	SetDelivery("BUILTIN_PRINTF", new GPL_DeliveryPrintf());
	SetDelivery("BUILTIN_EXIT", new GPL_DeliveryExit());
}

/***********************************************************************
GPL_Logger::~GPL_Logger(void)
***********************************************************************/
GPL_Logger::~GPL_Logger(void)
{
}

/***********************************************************************
GPL_Delivery *GPL_Logger::GetDelivery(const GPL_String & name) const
***********************************************************************/
GPL_Delivery *GPL_Logger::GetDelivery(const GPL_String & name) const
{
	GPL_Pointer<GPL_Delivery> delivery;
	if (!deliveryNameToDelivery.LookUp(name, delivery))
	{
		return (GPL_Delivery *) GPL_BAD_ADDRESS;
	}
	return delivery.GetAddress();
}

/***********************************************************************
void GPL_Logger::SetDefaultDelivery(GPL_Delivery *delivery)
***********************************************************************/
void GPL_Logger::SetDefaultDelivery(GPL_Delivery *delivery)
{
	if ((GPL_Delivery *) GPL_BAD_ADDRESS == delivery)
	{
		return;
	}
	defaultDelivery = delivery;
}

/***********************************************************************
void GPL_Logger::SetDelivery(const GPL_String & name, GPL_Delivery *delivery)
***********************************************************************/
void GPL_Logger::SetDelivery(const GPL_String & name, GPL_Delivery *delivery)
{
	deliveryNameToDelivery.Replace(name, delivery);
}

/***********************************************************************
void GPL_Logger::UnsetDelivery(const GPL_String & name)
***********************************************************************/
void GPL_Logger::UnsetDelivery(const GPL_String & name)
{
	deliveryNameToDelivery.Remove(name);
}

/***********************************************************************
GPL_StringArray GPL_Logger::GetType(const GPL_String & tname) const
***********************************************************************/
GPL_StringArray GPL_Logger::GetType(const GPL_String & tname) const
{
	GPL_StringArray dnames;
	typeToDeliveryNames.LookUp(tname, dnames);
	return dnames;
}

/***********************************************************************
void GPL_Logger::SetType(const GPL_String & tname,
							const GPL_StringArray & deliveries)
***********************************************************************/
void GPL_Logger::SetType(const GPL_String & tname,
							const GPL_StringArray & dnames)
{
	typeToDeliveryNames.Replace(tname, dnames);
}

/***********************************************************************
void GPL_Logger::SetType(const GPL_String & tname, int deliveries, ...)
***********************************************************************/
void GPL_Logger::SetType(const GPL_String & tname, int deliveries, ...)
{
	GPL_Array<GPL_String> dnames;
	va_list ap;
	va_start(ap, deliveries);
	while (deliveries--)
	{
		dnames.AppendElement((char *) (va_arg(ap, char *)));
	}
	SetType(tname, dnames);
	va_end(ap);
}

/***********************************************************************
void GPL_Logger::Log(const GPL_String & filename, const long linenum,
						const GPL_String & tname, const char *format, ...)
***********************************************************************/
void GPL_Logger::Log(const GPL_String & filename, const long linenum,
						const GPL_String & tname, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	VLog(filename, linenum, tname, format, ap);
	va_end(ap);
}

/***********************************************************************
void GPL_Logger::VLog(const GPL_String & filename, const long linenum,
						const GPL_String & tname, const char *format,
						va_list ap)
***********************************************************************/
void GPL_Logger::VLog(const GPL_String & filename, const long linenum,
						const GPL_String & tname, const char *format,
						va_list ap)
{
	GPL_Array<GPL_String> dnames;
	typeToDeliveryNames.LookUp(tname, dnames);

	GPL_Boolean found = FALSE;
	char *string = osdVSPrintf((char *) format, ap);
	GPL_Pointer<GPL_Delivery> delivery;
	unsigned long i;
	for (i = 0; i < dnames.GetCount(); i++)
	{
		if (deliveryNameToDelivery.LookUp(dnames.GetElement(i), delivery))
		{
			found = TRUE;
			delivery->Deliver(filename, linenum, tname, string);
		}
	}

	if (!found)
	{
		defaultDelivery->Deliver(filename, linenum, tname, string);
	}

	osdFree(string);
}

/***********************************************************************
void GPL_Logger::DirectLog(GPL_Delivery *delivery, const GPL_String & filename,
							const long linenum, const GPL_String & tname,
							const char *format, ...)
***********************************************************************/
void GPL_Logger::DirectLog(GPL_Delivery *delivery, const GPL_String & filename,
							const long linenum, const GPL_String & tname,
							const char *format, ...)
{

	va_list ap;
	va_start(ap, format);
	DirectVLog(delivery, filename, linenum, tname, format, ap);
	va_end(ap);
}

/***********************************************************************
void GPL_Logger::DirectVLog(GPL_Delivery *delivery, const GPL_String & filename,
							const long linenum, const GPL_String & tname,
							const char *format, va_list ap)
***********************************************************************/
void GPL_Logger::DirectVLog(GPL_Delivery *delivery, const GPL_String & filename,
							const long linenum, const GPL_String & tname,
							const char *format, va_list ap)
{
	char *string = osdVSPrintf((char *) format, ap);
	if (delivery) 
	{
		delivery->Deliver(filename, linenum, tname, string);
	}
	else
	{
		defaultDelivery->Deliver(filename, linenum, tname, string);
	}

	osdFree(string);
}

/***********************************************************************
void GPL_Logger::Merge(GPL_Logger *logger)
***********************************************************************/
void GPL_Logger::Merge(GPL_Logger *logger)
{
	// If there is no default delivery...
	if (defaultDelivery.IsNull())
	{
		// Use the other logger's default delivery.
		defaultDelivery = logger->defaultDelivery;
	}

	// Copy any types that do not exist.
	GPL_MapIterator<GPL_String, GPL_StringArray> types;
	types.Reset(&(logger->typeToDeliveryNames));
	while (types.Next())
	{
		// This works because GPL_Map::Insert() does nothing if
		// a value already exists for the key.
		typeToDeliveryNames.Insert(types.Key(), types.Value());
	}

	// Copy any deliveries that do not already exist.
	GPL_MapIterator<GPL_String, GPL_Pointer<GPL_Delivery> > deliveries;
	deliveries.Reset(&(logger->deliveryNameToDelivery));
	while (deliveries.Next())
	{
		// This works because GPL_Map::Insert() does nothing if
		// a value already exists for the key.
		deliveryNameToDelivery.Insert(deliveries.Key(), deliveries.Value());
	}
}

/***********************************************************************
void gplLoggerAddModule(const char *module)
***********************************************************************/
void gplLoggerAddModule(const char *module)
{
	GPL_String s;
	s = module; s.Append((const char *)" LOG");
	GPL_LOGGER.SetType(s, 1, (const char *)"BUILTIN_NOOP");
	s.Clear();
	s = module; s.Append((const char *)" WARNING");
	GPL_LOGGER.SetType(s, 1, (const char *)"BUILTIN_OSD_OUTPUT_TERSE");
	s.Clear();
	s = module; s.Append((const char *)" ERROR");
	GPL_LOGGER.SetType(s, 1, (const char *)"BUILTIN_OSD_OUTPUT");
}

/***********************************************************************
void gplHackPrint(long lvl, char *fmt, ...)
***********************************************************************/
void gplHackPrint(long lvl, char *fmt, ...)
{
	static GPL_Boolean checkedEnvironment = 0;
	static long doPrint = 0;
	char value[64];
	if (!checkedEnvironment)
	{
		checkedEnvironment = TRUE;
		if (osdGetEnvironmentVariable("GPL_HACK_PRINT", value, 64))
		{
			doPrint = atol(value);
		}
	}
	if (lvl <= doPrint)
	{
		va_list args;
		char *s;
		va_start(args, fmt);
		s = osdVSPrintf(fmt, args);
		fprintf(stderr, "%s", s);
		va_end(args);
		osdFree(s);
	}
}
