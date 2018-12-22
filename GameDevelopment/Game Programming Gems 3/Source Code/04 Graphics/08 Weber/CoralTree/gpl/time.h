#ifndef GPL_TIME_H
#define GPL_TIME_H

/******************************************************************************

Coral Tree gpl/time.h
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

#include <gpl/persist.h>
#include <gpl/depict.h>

/******************************************************************************
class GPL_Time
******************************************************************************/
//class GPL_Time : public GPL_Persistent, public GPL_Depictable
class GPL_Time : public GPL_Depictable
{
	public:
		GPL_Time(void);
		GPL_Time(const real64 seconds);
		GPL_Time(const OSD_Time & t) : osdtime(t) {}
		virtual	~GPL_Time(void);

		GPL_Boolean operator==(const GPL_Time & t) const;
		GPL_Boolean operator!=(const GPL_Time & t) const;
		GPL_Boolean operator<(const GPL_Time & t) const;
		GPL_Boolean operator<=(const GPL_Time & t) const;
		GPL_Boolean operator>(const GPL_Time & t) const;
		GPL_Boolean operator>=(const GPL_Time & t) const;
		GPL_Time operator+(const real64 seconds) const;
		GPL_Time operator+(const GPL_Time & time) const;
		GPL_Time & operator+=(const real64 seconds);
		GPL_Time & operator+=(const GPL_Time & time);
		GPL_Time operator-(const real64 seconds) const;
		GPL_Time operator-(const GPL_Time & time) const;
		GPL_Time & operator-=(const real64 seconds);
		GPL_Time & operator-=(const GPL_Time & time);

		GPL_Time & operator=(const GPL_Time & t);

		int32 GetYear(void) const { return osdtime.year; }
		int8 GetMonth(void) const { return osdtime.month; }
		int8 GetDay(void) const { return osdtime.day; }
		int8 GetHour(void) const { return osdtime.hour; }
		int8 GetMinute(void) const { return osdtime.minute; }
		int8 GetSecond(void) const { return osdtime.second; }
		int32 GetMillisecond(void) const { return osdtime.millisecond; }

		void SetYear(const int32 year) { osdtime.year = year; }
		void SetMonth(const int8 month) { osdtime.month = month; }
		void SetDay(const int8 day) { osdtime.day = day; }
		void SetHour(const int8 hour) { osdtime.hour = hour; }
		void SetMinute(const int8 minute) { osdtime.minute = minute; }
		void SetSecond(const int8 second) { osdtime.second = second; }
		void SetMillisecond(const int32 millisecond)
		{
			osdtime.millisecond = millisecond;
		}
		void Set(const int32 year, const int8 month,
					const int8 day, const int8 hour,
					const int8 minute, const int8 second,
					const int32 millisecond);

		GPL_Integer Compare(const GPL_Time & t,
							const GPL_Time & tolerance) const;
		GPL_Time Delta(const GPL_Time & t) const;

		void Zero(void);
		GPL_Boolean GetFromClock(void);
		virtual void Normalize(void);
		virtual void ConvertFromMilliseconds(const int32 milliseconds);
		virtual void ConvertFromSeconds(const real64 seconds);
		virtual void ConvertToMilliseconds(int32 & milliseconds);
		virtual void ConvertToSeconds(real64 & seconds);

		void AddTime(const GPL_Time & t);
		void AddSeconds(const real64 seconds);
		void SubtractTime(const GPL_Time & t);
		void SubtractSeconds(const real64 seconds);

		virtual GPL_Boolean ParseMMDD(const GPL_String & string);
		virtual GPL_Boolean ParseYYMMDD(const GPL_String & string);
		virtual GPL_Boolean ParseHHMM(const GPL_String & string);
		virtual GPL_Boolean ParseHHMMSS(const GPL_String & string);
		virtual GPL_Boolean ParseYYMMDDHH(const GPL_String & string);
		virtual GPL_Boolean ParseYYMMDDHHMM(const GPL_String & string);
		virtual GPL_Boolean ParseLong(const GPL_String & string);
		virtual GPL_Boolean ParseFull(const GPL_String & string);
		virtual GPL_Boolean ParseConcise(const GPL_String & string);

		// GPL_Depictable
		virtual GPL_Boolean Print(GPL_String & string,
									const GPL_Verbosity verbosity) const;
		virtual GPL_Boolean Parse(const GPL_String & string);

		// GPL_Persistent
		virtual GPL_Boolean Pack (GPL_Message & msg) const;
		virtual GPL_Boolean Unpack (GPL_Message & msg);

		// GPL_Object
		//GPL_OBJECT_INTERFACE(GPL_Time);

	protected:
		OSD_Time osdtime;

		static int8 DaysPerMonth(const int8 month,
										const int32 year);
		static int32 DaysInYears(const int32 years);
		static int32 YearsInDays(const int32 days);
		static GPL_Boolean IsLeapYear(const int32 year);
		virtual int32 ParseYear(const GPL_String & string);
};

extern const GPL_Time & gplCurrentTime(void);

#endif /* GPL_TIME_H */
