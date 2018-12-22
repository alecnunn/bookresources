/******************************************************************************

Coral Tree gpl/time.cc
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

/******************************************************************************
class GPL_Time
******************************************************************************/
/***********************************************************************
GPL_Time::GPL_Time(void)
***********************************************************************/
GPL_Time::GPL_Time(void)
{
	Zero();
}

/***********************************************************************
GPL_Time::GPL_Time(const real64 seconds)
***********************************************************************/
GPL_Time::GPL_Time(const real64 seconds)
{
	Zero();
	ConvertFromSeconds(seconds);
}

/***********************************************************************
GPL_Time::~GPL_Time(void)
***********************************************************************/
GPL_Time::~GPL_Time(void)
{
}

/***********************************************************************
GPL_Boolean GPL_Time::operator==(const GPL_Time & t) const
***********************************************************************/
GPL_Boolean GPL_Time::operator==(const GPL_Time & t) const
{
	if (osdtime.year == t.osdtime.year)
	{
		if (osdtime.month == t.osdtime.month)
		{
			if (osdtime.day == t.osdtime.day)
			{
				if (osdtime.hour == t.osdtime.hour)
				{
					if (osdtime.minute == t.osdtime.minute)
					{
						if (osdtime.second == t.osdtime.second)
						{
							if (osdtime.millisecond == t.osdtime.millisecond)
							{
								return TRUE;
							}
						}
					}
				}
			}
		}
	}
	return FALSE;
}

/***********************************************************************
GPL_Boolean GPL_Time::operator!=(const GPL_Time & t) const
***********************************************************************/
GPL_Boolean GPL_Time::operator!=(const GPL_Time & t) const
{
	if (operator==(t))
	{
		return FALSE;
	}
	return TRUE;
}

/***********************************************************************
GPL_Boolean GPL_Time::operator<(const GPL_Time & t) const
***********************************************************************/
GPL_Boolean GPL_Time::operator<(const GPL_Time & t) const
{
	if (osdtime.year == t.osdtime.year)
	{
		if (osdtime.month == t.osdtime.month)
		{
			if (osdtime.day == t.osdtime.day)
			{
				if (osdtime.hour == t.osdtime.hour)
				{
					if (osdtime.minute == t.osdtime.minute)
					{
						if (osdtime.second == t.osdtime.second)
						{
							return osdtime.millisecond < t.osdtime.millisecond;
						}
						return osdtime.second < t.osdtime.second;
					}
					return osdtime.minute < t.osdtime.minute;
				}
				return osdtime.hour < t.osdtime.hour;
			}
			return osdtime.day < t.osdtime.day;
		}
		return osdtime.month < t.osdtime.month;
	}
	return osdtime.year < t.osdtime.year;
}

/***********************************************************************
GPL_Boolean GPL_Time::operator<=(const GPL_Time & t) const
***********************************************************************/
GPL_Boolean GPL_Time::operator<=(const GPL_Time & t) const
{
	if (operator>(t))
	{
		return FALSE;
	}
	return TRUE;
}

/***********************************************************************
GPL_Boolean GPL_Time::operator>(const GPL_Time & t) const
***********************************************************************/
GPL_Boolean GPL_Time::operator>(const GPL_Time & t) const
{
	if (osdtime.year == t.osdtime.year)
	{
		if (osdtime.month == t.osdtime.month)
		{
			if (osdtime.day == t.osdtime.day)
			{
				if (osdtime.hour == t.osdtime.hour)
				{
					if (osdtime.minute == t.osdtime.minute)
					{
						if (osdtime.second == t.osdtime.second)
						{
							return osdtime.millisecond > t.osdtime.millisecond;
						}
						return osdtime.second > t.osdtime.second;
					}
					return osdtime.minute > t.osdtime.minute;
				}
				return osdtime.hour > t.osdtime.hour;
			}
			return osdtime.day > t.osdtime.day;
		}
		return osdtime.month > t.osdtime.month;
	}
	return osdtime.year > t.osdtime.year;
}

/***********************************************************************
GPL_Boolean GPL_Time::operator>=(const GPL_Time & t) const
***********************************************************************/
GPL_Boolean GPL_Time::operator>=(const GPL_Time & t) const
{
	if (operator<(t))
	{
		return FALSE;
	}
	return TRUE;
}

/***********************************************************************
GPL_Time GPL_Time::operator+(const real64 seconds) const
***********************************************************************/
GPL_Time GPL_Time::operator+(const real64 seconds) const
{
	GPL_Time t(*this);
	t += seconds;
	return t;
}

/***********************************************************************
GPL_Time GPL_Time::operator+(const GPL_Time & time) const
***********************************************************************/
GPL_Time GPL_Time::operator+(const GPL_Time & time) const
{
	GPL_Time t(*this);
	t += time;
	return t;
}

/***********************************************************************
GPL_Time & GPL_Time::operator+=(const real64 seconds)
***********************************************************************/
GPL_Time & GPL_Time::operator+=(const real64 seconds)
{
	AddSeconds(seconds);
	return *this;
}

/***********************************************************************
GPL_Time & GPL_Time::operator+=(const GPL_Time & time)
***********************************************************************/
GPL_Time & GPL_Time::operator+=(const GPL_Time & time)
{
	AddTime(time);
	Normalize();
	return *this;
}

/***********************************************************************
GPL_Time GPL_Time::operator-(const real64 seconds) const
***********************************************************************/
GPL_Time GPL_Time::operator-(const real64 seconds) const
{
	GPL_Time t(*this);
	t -= seconds;
	return t;
}

/***********************************************************************
GPL_Time GPL_Time::operator-(const GPL_Time & time) const
***********************************************************************/
GPL_Time GPL_Time::operator-(const GPL_Time & time) const
{
	GPL_Time t(*this);
	t -= time;
	return t;
}

/***********************************************************************
GPL_Time & GPL_Time::operator-=(const real64 seconds)
***********************************************************************/
GPL_Time & GPL_Time::operator-=(const real64 seconds)
{
	SubtractSeconds(seconds);
	return *this;
}

/***********************************************************************
GPL_Time & GPL_Time::operator-=(const GPL_Time & time)
***********************************************************************/
GPL_Time & GPL_Time::operator-=(const GPL_Time & time)
{
	SubtractTime(time);
	Normalize();
	return *this;
}

GPL_Time & GPL_Time::operator=(const GPL_Time & t)
{
	osdtime = t.osdtime;
	return *this;
}

/***********************************************************************
void GPL_Time::Zero(void)
***********************************************************************/
void GPL_Time::Zero(void)
{
	osdtime.year = 0;
	osdtime.month = 0;
	osdtime.day = 0;
	osdtime.hour = 0;
	osdtime.minute = 0;
	osdtime.second = 0;
	osdtime.millisecond = 0;
}

/***********************************************************************
void GPL_Time::Set(const int32 year, const int8 month,
					const int8 day, const int8 hour,
					const int8 minute, const int8 second,
					const int32 millisecond)
***********************************************************************/
void GPL_Time::Set(const int32 year, const int8 month,
					const int8 day, const int8 hour,
					const int8 minute, const int8 second,
					const int32 millisecond)
{
	SetYear(year);
	SetMonth(month);
	SetDay(day);
	SetHour(hour);
	SetMinute(minute);
	SetSecond(second);
	SetMillisecond(millisecond);
}

/***********************************************************************
GPL_Boolean GPL_Time::GetFromClock(void)
***********************************************************************/
GPL_Boolean GPL_Time::GetFromClock(void)
{
	// Get the system time.
	if (!osdTimeGet(&osdtime))
	{
		return FALSE;
	}

	// Adjust the month and day to count from zero.
	osdtime.month--;
	osdtime.day--;

	return TRUE;
}

/***********************************************************************
void GPL_Time::Normalize(void)
***********************************************************************/
void GPL_Time::Normalize(void)
{
	// The default normalization is to the Julian calendar.
	real64 seconds = 0.0;
	ConvertToSeconds(seconds);
	ConvertFromSeconds(seconds);
}

/***********************************************************************
void GPL_Time::ConvertFromMilliseconds(const int32 milliseconds)
***********************************************************************/
void GPL_Time::ConvertFromMilliseconds(const int32 milliseconds)
{
	real64 s = 0.0;
	s = (real64) milliseconds;
	s /= 1000.0;
	ConvertFromSeconds(s);
}

/***********************************************************************
void GPL_Time::ConvertFromSeconds(const real64 seconds)
***********************************************************************/
void GPL_Time::ConvertFromSeconds(const real64 seconds)
{
	// Years.
	real64 s = seconds;
	osdtime.year = YearsInDays(floor(s / 86400.0));
	s -= ((real64) DaysInYears(osdtime.year) * 86400.0);
//printf("#9 %f\n", s);

	// Months.
	real64 d = floor(s / 86400.0);
//printf("days == %+f\n", d);
	s -= (d * 86400.0);
//printf("#11 %f\n", s);
	int32 k = (int32) d;
	if (0 < k)
	{
		osdtime.month = 0;
//printf("k == %d\n", k);
		while (k >= DaysPerMonth(osdtime.month, osdtime.year))
		{
			k -= DaysPerMonth(osdtime.month, osdtime.year);
//printf("k == %d\n", k);
			if (11 == (osdtime.month % 12))
			{
				++osdtime.year;
			}
			osdtime.month = (osdtime.month + 1) % 12;
		}
	}
	else if (0 > k)
	{
		osdtime.month = -1;
//printf("k == %d\n", k);
		while (k <= (-1 * DaysPerMonth(osdtime.month, osdtime.year)))
		{
			k += DaysPerMonth(osdtime.month, osdtime.year);
//printf("k == %d\n", k);
			if (0 == (osdtime.month % 12))
			{
				--osdtime.year;
			}
			osdtime.month = (osdtime.month - 1) % 12;
		}
	}
	osdtime.day = (int8) k;

	// Hours.
	d = floor(s / 3600.0);
	osdtime.hour = (int8) d;
	s -= (d * 3600.0);
//printf("#12 %f\n", s);

	// Minutes.
	d = floor(s / 60.0);
	osdtime.minute = (int8) d;
	s -= (d * 60.0);
//printf("#13 %f\n", s);

	// Seconds.
	d = floor(s);
	osdtime.second = (int8) d;
	s -= d;
//printf("#14 %f\n", s);

	// Milli-seconds.
	osdtime.millisecond = (int32) floor(s * 1000.0);
//printf("%d,%d,%d,%d,%d,%d,%d\n", osdtime.year, osdtime.month, osdtime.day, osdtime.hour, osdtime.minute, osdtime.second, osdtime.millisecond);
}

/***********************************************************************
void GPL_Time::ConvertToMilliseconds(int32 & milliseconds)
***********************************************************************/
void GPL_Time::ConvertToMilliseconds(int32 & milliseconds)
{
	real64 s = 0.0;
	ConvertToSeconds(s);
	s *= 1000.0;
	milliseconds = (int32) floor(fmod(s, (real64) 0x7FFFFFFF));
}

/***********************************************************************
void GPL_Time::ConvertToSeconds(real64 & seconds)
***********************************************************************/
void GPL_Time::ConvertToSeconds(real64 & seconds)
{
	// Years.
//printf("%d,%d,%d,%d,%d,%d,%d\n", osdtime.year, osdtime.month, osdtime.day, osdtime.hour, osdtime.minute, osdtime.second, osdtime.millisecond);
	seconds += ((real64) DaysInYears(osdtime.year) * 86400.0);
//printf("#1 %f\n", seconds);

	// Days.
	seconds += ((real64) osdtime.day * 86400.0);
//printf("#2 %f\n", seconds);

	// Hours.
	seconds += ((real64) osdtime.hour * 3600.0);
//printf("#3 %f\n", seconds);

	// Minutes.
	seconds += ((real64) osdtime.minute * 60.0);
//printf("#4 %f\n", seconds);

	// Seconds.
	seconds += ((real64) osdtime.second * 1.0);
//printf("#5 %f\n", seconds);

	// Milli-seconds.
	seconds += ((real64) osdtime.millisecond * 0.001);
//printf("#6 %f\n", seconds);

	// Since the number of days per month depends
	// on the year, all of the other calculations
	// are performed first.
	// Months.
	if (0 < osdtime.month)
	{
		int8 i = 0;
		int32 days = 0;
		int32 years = YearsInDays(floor(seconds / 86400.0));
		while (osdtime.month > i)
		{
			days += DaysPerMonth(i, years);
			if (11 == (i % 12))
			{
				++years;
			}
			i = (i + 1) % 12;
		}
		seconds += ((real64) days * 86400.0);
	}
	else if (0 > osdtime.month)
	{
		int8 i = -1;
		int32 days = 0;
		int32 years = YearsInDays(floor(seconds / 86400.0));
		while (osdtime.month <= i)
		{
			days -= DaysPerMonth(i, years);
			if (0 == (i % 12))
			{
				--years;
			}
			i = (i - 1) % 12;
		}
		seconds += ((real64) days * 86400.0);
	}
//printf("#7 %f\n", seconds);
}

/***********************************************************************
void GPL_Time::AddTime(const GPL_Time & t)
***********************************************************************/
void GPL_Time::AddTime(const GPL_Time & t)
{
	osdtime.year += t.osdtime.year;
	osdtime.month += t.osdtime.month;
	osdtime.day += t.osdtime.day;
	osdtime.hour += t.osdtime.hour;
	osdtime.minute += t.osdtime.minute;
	osdtime.second += t.osdtime.second;
	osdtime.millisecond += t.osdtime.millisecond;
}

/***********************************************************************
void GPL_Time::AddSeconds(const real64 seconds)
***********************************************************************/
void GPL_Time::AddSeconds(const real64 seconds)
{
	real64 s = 0.0;
	ConvertToSeconds(s);
	s += seconds;
	ConvertFromSeconds(s);
}

/***********************************************************************
void GPL_Time::SubtractTime(const GPL_Time & t)
***********************************************************************/
void GPL_Time::SubtractTime(const GPL_Time & t)
{
	osdtime.year -= t.osdtime.year;
	osdtime.month -= t.osdtime.month;
	osdtime.day -= t.osdtime.day;
	osdtime.hour -= t.osdtime.hour;
	osdtime.minute -= t.osdtime.minute;
	osdtime.second -= t.osdtime.second;
	osdtime.millisecond -= t.osdtime.millisecond;
}

/***********************************************************************
void GPL_Time::SubtractSeconds(const real64 seconds)
***********************************************************************/
void GPL_Time::SubtractSeconds(const real64 seconds)
{
	real64 s = 0.0;
	ConvertToSeconds(s);
	s -= seconds;
	ConvertFromSeconds(s);
}

/***********************************************************************
int8 GPL_Time::DaysPerMonth(const int8 month,
								const int32 year)
***********************************************************************/
int8 GPL_Time::DaysPerMonth(const int8 month,
								const int32 year)
{
	int8 m = month;
	if (0 > m)
	{
		m *= -1;
	}

	switch (m)
	{
	case 0:
		return 31;

	case 1:
		return 28 + (IsLeapYear(year) ? 1 : 0);

	case 2:
		return 31;

	case 3:
		return 30;

	case 4:
		return 31;

	case 5:
		return 30;

	case 6:
		return 31;

	case 7:
		return 31;

	case 8:
		return 30;

	case 9:
		return 31;

	case 10:
		return 30;

	case 11:
		return 31;

	default:
		return 0;
	}
}

/***********************************************************************
int32 GPL_Time::DaysInYears(const int32 years)
***********************************************************************/
int32 GPL_Time::DaysInYears(const int32 years)
{
	// Base.
	int32 d = 365 * years;

	// Leap.
	d += (years / 4);

	// Not leap;
	d -= (years / 100);

	// Leap.
	d += (years / 400);

	return d;
}

/***********************************************************************
int32 GPL_Time::YearsInDays(const int32 days)
***********************************************************************/
int32 GPL_Time::YearsInDays(const int32 days)
{
	int32 d = days;
	int32 y = 0;
	while (365 <= d)
	{
		++y;
		d -= 365;
		if (0 == (y % 400))
		{
			--d;
		}
		else if (0 != (y % 100) && 0 == (y % 4))
		{
			--d;
		}
	}

	return y;
}

/***********************************************************************
GPL_Boolean GPL_Time::IsLeapYear(const int32 year)
***********************************************************************/
GPL_Boolean GPL_Time::IsLeapYear(const int32 year)
{

	if (0 == (year % 400))
	{
		return TRUE;
	}
	if (0 == (year % 100))
	{
		return FALSE;
	}
	if (0 == (year % 4))
	{
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************
GPL_Boolean GPL_Time::Print(GPL_String & string,
							const GPL_Verbosity verbosity) const
***********************************************************************/
GPL_Boolean GPL_Time::Print(GPL_String & string,
							const GPL_Verbosity verbosity) const
{
	GPL_String image;
	switch (verbosity)
	{
	case GPL_VERBOSITY_CONCISE:
		image.Printf("%02u%02u%02u%02u%02u%02u", 
			osdtime.year, osdtime.month + 1, osdtime.day + 1,
			osdtime.hour, osdtime.minute, osdtime.second);
		break;

	default:
		image.Printf("%02u-%02u-%02u %02u:%02u:%02u.%03u", 
			osdtime.year, osdtime.month + 1, osdtime.day + 1, osdtime.hour,
			osdtime.minute, osdtime.second, osdtime.millisecond);
		break;
	}
	string.Append(image);
	return TRUE;
}

/***********************************************************************
GPL_Boolean GPL_Time::Parse(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_Time::Parse(const GPL_String & string)
{
	if (ParseMMDD(string))
	{
		return TRUE;
	}
	if (ParseYYMMDD(string))
	{
		return TRUE;
	}
	if (ParseHHMM(string))
	{
		return TRUE;
	}
	if (ParseHHMMSS(string))
	{
		return TRUE;
	}
	if (ParseYYMMDDHH(string))
	{
		return TRUE;
	}
	if (ParseYYMMDDHHMM(string))
	{
		return TRUE;
	}
	if (ParseLong(string))
	{
		return TRUE;
	}
	if (ParseFull(string))
	{
		return TRUE;
	}
	if (ParseConcise(string))
	{
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************
int32 GPL_Time::ParseYear(const GPL_String & string)
***********************************************************************/
int32 GPL_Time::ParseYear(const GPL_String & string)
{
	int32 year = (int32) atoi(string.GetData());
	if (3 > string.GetLength())
	{
		if (70 > year)
		{
			year += 2000;
		}
		else
		{
			year += 1900;
		}
	}
	return year;
}

/***********************************************************************
GPL_Boolean GPL_Time::ParseMMDD(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_Time::ParseMMDD(const GPL_String & string)
{
	Zero();
	static GPL_RegularExpression expression
		("^([0-1][0-9])-([0-3][0-9])$");
	if (expression.Matches(string))
	{
		GPL_String s;
		if (!(s = expression.GetMatch(1)).IsNull())
		{
			osdtime.month = (int8) atoi(s.GetData());
			if (0 == osdtime.month)
			{
				return FALSE;
			}
			--osdtime.month;
		}
		if (!(s = expression.GetMatch(2)).IsNull())
		{
			osdtime.day = (int8) atoi(s.GetData());
			if (0 == osdtime.day)
			{
				return FALSE;
			}
			--osdtime.day;
		}
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************
GPL_Boolean GPL_Time::ParseYYMMDD(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_Time::ParseYYMMDD(const GPL_String & string)
{
	Zero();
	static GPL_RegularExpression expression
		("^([0-9][0-9]+)-([0-1][0-9])-([0-3][0-9])$");
	if (expression.Matches(string))
	{
		GPL_String s;
		if (!(s = expression.GetMatch(1)).IsNull())
		{
			osdtime.year = ParseYear(s);
		}
		if (!(s = expression.GetMatch(2)).IsNull())
		{
			osdtime.month = (int8) atoi(s.GetData());
			if (0 == osdtime.month)
			{
				return FALSE;
			}
			--osdtime.month;
		}
		if (!(s = expression.GetMatch(3)).IsNull())
		{
			osdtime.day = (int8) atoi(s.GetData());
			if (0 == osdtime.day)
			{
				return FALSE;
			}
			--osdtime.day;
		}
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************
GPL_Boolean GPL_Time::ParseHHMM(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_Time::ParseHHMM(const GPL_String & string)
{
	Zero();
	static GPL_RegularExpression expression
		("^([0-9][0-9]):([0-9][0-9])$");
	if (expression.Matches(string))
	{
		GPL_String s;
		if (!(s = expression.GetMatch(1)).IsNull())
		{
			osdtime.hour = (int8) atoi(s.GetData());
		}
		if (!(s = expression.GetMatch(2)).IsNull())
		{
			osdtime.minute = (int8) atoi(s.GetData());
		}
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************
GPL_Boolean GPL_Time::ParseHHMMSS(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_Time::ParseHHMMSS(const GPL_String & string)
{
	Zero();
	static GPL_RegularExpression expression
		("^([0-9][0-9]):([0-9][0-9]):([0-9][0-9])$");
	if (expression.Matches(string))
	{
		GPL_String s;
		if (!(s = expression.GetMatch(1)).IsNull())
		{
			osdtime.hour = (int8) atoi(s.GetData());
		}
		if (!(s = expression.GetMatch(2)).IsNull())
		{
			osdtime.minute = (int8) atoi(s.GetData());
		}
		if (!(s = expression.GetMatch(3)).IsNull())
		{
			osdtime.second = (int8) atoi(s.GetData());
		}
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************
GPL_Boolean GPL_Time::ParseYYMMDDHH(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_Time::ParseYYMMDDHH(const GPL_String & string)
{
	Zero();
	static GPL_RegularExpression expression
		("^([0-9][0-9]+)-([0-1][0-9])-([0-3][0-9]) ([0-9][0-9])$");
	if (expression.Matches(string))
	{
		GPL_String s;
		if (!(s = expression.GetMatch(1)).IsNull())
		{
			osdtime.year = ParseYear(s);
		}
		if (!(s = expression.GetMatch(2)).IsNull())
		{
			osdtime.month = (int8) atoi(s.GetData());
			if (0 == osdtime.month)
			{
				return FALSE;
			}
			--osdtime.month;
		}
		if (!(s = expression.GetMatch(3)).IsNull())
		{
			osdtime.day = (int8) atoi(s.GetData());
			if (0 == osdtime.day)
			{
				return FALSE;
			}
			--osdtime.day;
		}
		if (!(s = expression.GetMatch(4)).IsNull())
		{
			osdtime.hour = (int8) atoi(s.GetData());
		}
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************
GPL_Boolean GPL_Time::ParseYYMMDDHHMM(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_Time::ParseYYMMDDHHMM(const GPL_String & string)
{
	Zero();
	static GPL_RegularExpression expression
		("^([0-9][0-9]+)-([0-1][0-9])-([0-3][0-9]) ([0-9][0-9]):([0-9][0-9])$");
	if (expression.Matches(string))
	{
		GPL_String s;
		if (!(s = expression.GetMatch(1)).IsNull())
		{
			osdtime.year = ParseYear(s);
		}
		if (!(s = expression.GetMatch(2)).IsNull())
		{
			osdtime.month = (int8) atoi(s.GetData());
			if (0 == osdtime.month)
			{
				return FALSE;
			}
			--osdtime.month;
		}
		if (!(s = expression.GetMatch(3)).IsNull())
		{
			osdtime.day = (int8) atoi(s.GetData());
			if (0 == osdtime.day)
			{
				return FALSE;
			}
			--osdtime.day;
		}
		if (!(s = expression.GetMatch(4)).IsNull())
		{
			osdtime.hour = (int8) atoi(s.GetData());
		}
		if (!(s = expression.GetMatch(5)).IsNull())
		{
			osdtime.minute = (int8) atoi(s.GetData());
		}
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************
GPL_Boolean GPL_Time::ParseLong(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_Time::ParseLong(const GPL_String & string)
{
	Zero();
	static GPL_RegularExpression expression
		("^([0-9][0-9]+)-([0-1][0-9])-([0-3][0-9]) ([0-9][0-9]):([0-9][0-9]):([0-9][0-9])$");
	if (expression.Matches(string))
	{
		GPL_String s;
		if (!(s = expression.GetMatch(1)).IsNull())
		{
			osdtime.year = ParseYear(s);
		}
		if (!(s = expression.GetMatch(2)).IsNull())
		{
			osdtime.month = (int8) atoi(s.GetData());
			if (0 == osdtime.month)
			{
				return FALSE;
			}
			--osdtime.month;
		}
		if (!(s = expression.GetMatch(3)).IsNull())
		{
			osdtime.day = (int8) atoi(s.GetData());
			if (0 == osdtime.day)
			{
				return FALSE;
			}
			--osdtime.day;
		}
		if (!(s = expression.GetMatch(4)).IsNull())
		{
			osdtime.hour = (int8) atoi(s.GetData());
		}
		if (!(s = expression.GetMatch(5)).IsNull())
		{
			osdtime.minute = (int8) atoi(s.GetData());
		}
		if (!(s = expression.GetMatch(6)).IsNull())
		{
			osdtime.second = (int8) atoi(s.GetData());
		}
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************
GPL_Boolean GPL_Time::ParseFull(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_Time::ParseFull(const GPL_String & string)
{
	Zero();
	static GPL_RegularExpression expression
		("^([0-9][0-9]+)-([0-1][0-9])-([0-3][0-9]) ([0-9][0-9]):([0-9][0-9]):([0-9][0-9]).([0-9][0-9][0-9])$");
	if (expression.Matches(string))
	{
		GPL_String s;
		if (!(s = expression.GetMatch(1)).IsNull())
		{
			osdtime.year = ParseYear(s);
		}
		if (!(s = expression.GetMatch(2)).IsNull())
		{
			osdtime.month = (int8) atoi(s.GetData());
			if (0 == osdtime.month)
			{
				return FALSE;
			}
			--osdtime.month;
		}
		if (!(s = expression.GetMatch(3)).IsNull())
		{
			osdtime.day = (int8) atoi(s.GetData());
			if (0 == osdtime.day)
			{
				return FALSE;
			}
			--osdtime.day;
		}
		if (!(s = expression.GetMatch(4)).IsNull())
		{
			osdtime.hour = (int8) atoi(s.GetData());
		}
		if (!(s = expression.GetMatch(5)).IsNull())
		{
			osdtime.minute = (int8) atoi(s.GetData());
		}
		if (!(s = expression.GetMatch(6)).IsNull())
		{
			osdtime.second = (int8) atoi(s.GetData());
		}
		if (!(s = expression.GetMatch(7)).IsNull())
		{
			osdtime.millisecond = (int32) atoi(s.GetData());
		}
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************
GPL_Boolean GPL_Time::ParseConcise(const GPL_String & string)
***********************************************************************/
GPL_Boolean GPL_Time::ParseConcise(const GPL_String & string)
{
	Zero();
	static GPL_RegularExpression expression
		("^([0-9][0-9])([0-1][0-9])([0-3][0-9])([0-9][0-9])([0-9][0-9])([0-9][0-9])$");
	if (expression.Matches(string))
	{
		GPL_String s;
		if (!(s = expression.GetMatch(1)).IsNull())
		{
			osdtime.year = ParseYear(s);
		}
		if (!(s = expression.GetMatch(2)).IsNull())
		{
			osdtime.month = (int8) atoi(s.GetData());
			if (0 == osdtime.month)
			{
				return FALSE;
			}
			--osdtime.month;
		}
		if (!(s = expression.GetMatch(3)).IsNull())
		{
			osdtime.day = (int8) atoi(s.GetData());
			if (0 == osdtime.day)
			{
				return FALSE;
			}
			--osdtime.day;
		}
		if (!(s = expression.GetMatch(4)).IsNull())
		{
			osdtime.hour = (int8) atoi(s.GetData());
		}
		if (!(s = expression.GetMatch(5)).IsNull())
		{
			osdtime.minute = (int8) atoi(s.GetData());
		}
		if (!(s = expression.GetMatch(6)).IsNull())
		{
			osdtime.second = (int8) atoi(s.GetData());
		}
		return TRUE;
	}
	return FALSE;
}

/***********************************************************************
GPL_Integer GPL_Time::Compare(const GPL_Time & t,
								const GPL_Time & tolerance) const
***********************************************************************/
GPL_Integer GPL_Time::Compare(const GPL_Time & t,
								const GPL_Time & tolerance) const
{
	// Determine the difference between the two times.
	GPL_Time delta = Delta(t);

	// Apply the tolerance values.
	if (labs(tolerance.osdtime.year) >= labs(delta.osdtime.year))
	{
		delta.osdtime.year = 0;
	}
	if (abs(tolerance.osdtime.month) >= abs(delta.osdtime.month))
	{
		delta.osdtime.month = 0;
	}
	if (abs(tolerance.osdtime.day) >= abs(delta.osdtime.day))
	{
		delta.osdtime.day = 0;
	}
	if (abs(tolerance.osdtime.hour) >= abs(delta.osdtime.hour))
	{
		delta.osdtime.hour = 0;
	}
	if (abs(tolerance.osdtime.minute) >= abs(delta.osdtime.minute))
	{
		delta.osdtime.minute = 0;
	}
	if (abs(tolerance.osdtime.second) >= abs(delta.osdtime.second))
	{
		delta.osdtime.second = 0;
	}
	if (labs(tolerance.osdtime.millisecond) >= labs(delta.osdtime.millisecond))
	{
		delta.osdtime.millisecond = 0;
	}

	// Calculate the result.
	GPL_Integer result = 0;
	if (0 != delta.osdtime.year)
	{
		result = delta.osdtime.year;
	}
	if (0 != delta.osdtime.month)
	{
		result = (0 == result) ? delta.osdtime.month : (result * abs(delta.osdtime.month));
	}
	if (0 != delta.osdtime.day)
	{
		result = (0 == result) ? delta.osdtime.day : (result * abs(delta.osdtime.day));
	}
	if (0 != delta.osdtime.hour)
	{
		result = (0 == result) ? delta.osdtime.hour : (result * abs(delta.osdtime.hour));
	}
	if (0 != delta.osdtime.minute)
	{
		result = (0 == result) ? delta.osdtime.minute : (result * abs(delta.osdtime.minute));
	}
	if (0 != delta.osdtime.second)
	{
		result = (0 == result) ? delta.osdtime.second : (result * abs(delta.osdtime.second));
	}
	if (0 != delta.osdtime.millisecond)
	{
		result = (0 == result) ? delta.osdtime.millisecond : (result * labs(delta.osdtime.millisecond));
	}
	return result;
}

/***********************************************************************
GPL_Time GPL_Time::Delta(const GPL_Time & t) const
***********************************************************************/
GPL_Time GPL_Time::Delta(const GPL_Time & t) const
{
	GPL_Time delta;
	delta.osdtime.year = osdtime.year - t.osdtime.year;
	delta.osdtime.month = osdtime.month - t.osdtime.month;
	delta.osdtime.day = osdtime.day - t.osdtime.day;
	delta.osdtime.hour = osdtime.hour - t.osdtime.hour;
	delta.osdtime.minute = osdtime.minute - t.osdtime.minute;
	delta.osdtime.second = osdtime.second - t.osdtime.second;
	delta.osdtime.millisecond = osdtime.millisecond - t.osdtime.millisecond;
	return delta;
}

/***********************************************************************
GPL_Boolean GPL_Time::Pack (GPL_Message & msg) const
***********************************************************************/
GPL_Boolean GPL_Time::Pack (GPL_Message & msg) const
{
	msg.AppendLong(osdtime.year);
	msg.AppendChar(osdtime.month);
	msg.AppendChar(osdtime.day);
	msg.AppendChar(osdtime.hour);
	msg.AppendChar(osdtime.minute);
	msg.AppendChar(osdtime.second);
	msg.AppendLong(osdtime.millisecond);
	return TRUE;
}

/***********************************************************************
GPL_Boolean GPL_Time::Unpack (GPL_Message & msg)
***********************************************************************/
GPL_Boolean GPL_Time::Unpack (GPL_Message & msg)
{
	osdtime.year = msg.ReadLong();
	osdtime.month = msg.ReadChar();
	osdtime.day = msg.ReadChar();
	osdtime.hour = msg.ReadChar();
	osdtime.minute = msg.ReadChar();
	osdtime.second = msg.ReadChar();
	osdtime.millisecond = msg.ReadLong();
	return TRUE;
}

//GPL_OBJECT_IMPLEMENTATION(GPL_Time, 0x40000002, "GPL_Time");

const GPL_Time & gplCurrentTime(void)
{
	static GPL_Time rightNow;
	rightNow.GetFromClock();
	return rightNow;
}

