#ifndef SGX_CORE_TIME_SYSTEM_CLOCK_HPP
#define SGX_CORE_TIME_SYSTEM_CLOCK_HPP	1

#include "sgx/core/types/types.hpp"

class CSystemClock {
public:

	class Time {
	public:
		tUINT32	hours, minutes, seconds;
		tUINT32	milliseconds;
	};

	typedef enum {
		Sunday = 0,
		Monday,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday,
	} Day;

	typedef enum {
		January = 0,
		February,
		March,
		April,
		May,
		June,
		July,
		August,
		September,
		October,
		November,
		December,
	} Month;

	class Date {
	public:
		tUINT32		day;
		Month		month;
		tUINT32		year;
		Day			dow;
		Time		time;
	};

	static void					GetDate(CSystemClock::Date &date);
	static CSystemClock::Date	GetDate();

	static void					GetTime(CSystemClock::Time &tm);
	static CSystemClock::Time	GetTime();
	static tUINT32				GetDay();
	static CSystemClock::Month	GetMonth();
	static tUINT32				GetYear();
	static CSystemClock::Day	GetDayOfWeek();
};

#endif /* SGX_CORE_TIME_SYSTEM_CLOCK_HPP */
