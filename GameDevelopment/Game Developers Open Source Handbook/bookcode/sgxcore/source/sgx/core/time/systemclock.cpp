#include "sgx/core/types/types.hpp"
#include "sgx/core/time/systemclock.hpp"

//
// Dates, with months, years
// 
// NOTE: Every function is here relies on the uber-date
// function, GetDate, which must be present in the
// platform-specific code.
// No cross-platform null driver exists for this

CSystemClock::Date
CSystemClock::GetDate()
{
	CSystemClock::Date date;

	GetDate(date);
	return date;
}

tUINT32
CSystemClock::GetDay()
{
	CSystemClock::Date date;

	GetDate(date);
	return date.day;
}

CSystemClock::Month
CSystemClock::GetMonth()
{
	CSystemClock::Date date;

	GetDate(date);
	return date.month;
}

tUINT32
CSystemClock::GetYear()
{
	CSystemClock::Date date;

	GetDate(date);
	return date.year;
}

CSystemClock::Day
CSystemClock::GetDayOfWeek()
{
	CSystemClock::Date date;

	GetDate(date);
	return date.dow;
}

//
// Basic time handler
//
void					
CSystemClock::GetTime(CSystemClock::Time &tm)
{
	CSystemClock::Date date;

	GetDate(date);
	tm = date.time;
}

CSystemClock::Time	
CSystemClock::GetTime()
{
	CSystemClock::Date date;

	GetDate(date);
	return date.time;
}

