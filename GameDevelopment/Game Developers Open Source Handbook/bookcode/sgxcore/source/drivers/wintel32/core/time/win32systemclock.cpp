#include "drivers/wintel32/wintel32core.hpp"
#include "sgx/core/core.hpp"
#include "sgx/system/system.hpp"

void
CSystemClock::GetDate(CSystemClock::Date &date)
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	date.time.hours = systime.wHour;
	date.time.minutes = systime.wMinute;
	date.time.seconds = systime.wSecond;
	date.time.milliseconds = systime.wMilliseconds;
	
	date.dow = SGX_STATIC_CAST(Day, systime.wDayOfWeek);

	date.day = systime.wDay;
	date.month = SGX_STATIC_CAST(Month, systime.wMonth-1);
	date.year = systime.wYear;
}
