/*
 Copyright (C) Giuliano Catrambone (giulianocatrambone@gmail.com)

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 Commercial use other than under the terms of the GNU General Public
 License is allowed only after express negotiation of conditions
 with the authors.
*/

#include "Datetime.h"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

int main()

{

#ifdef WIN32
	__int64 ullNowUTCInMilliSecs;
	__int64 ullUTCInSecs;
#else
	unsigned long long ullNowUTCInMilliSecs;
	unsigned long long ullUTCInSecs;
#endif
	tm tmDateTime;
	unsigned long ulMilliSecs;
	long lTimeZoneDifferenceInHours;
	unsigned long ulYear;
	unsigned long ulMonth;
	unsigned long ulDay;
	unsigned long ulHour;
	unsigned long ulMinutes;
	unsigned long ulSeconds;
	bool bDaylightSavingTime;

#ifdef WIN32
	Datetime::nowUTCInMilliSecs(&ullNowUTCInMilliSecs, &lTimeZoneDifferenceInHours);
#else
	Datetime::nowUTCInMilliSecs(&ullNowUTCInMilliSecs, &lTimeZoneDifferenceInHours);
#endif

	cout << "  NowUTCInMilliSecs: " << ullNowUTCInMilliSecs << ", TimeZoneDifferenceInHours: " << lTimeZoneDifferenceInHours << endl;

	Datetime::get_tm_LocalTime(&tmDateTime, &ulMilliSecs);

	Datetime::convertFromLocalDateTimeToLocalInSecs(
		tmDateTime.tm_year + 1900, tmDateTime.tm_mon + 1, tmDateTime.tm_mday, tmDateTime.tm_hour - lTimeZoneDifferenceInHours, tmDateTime.tm_min,
		tmDateTime.tm_sec, -1, &ullUTCInSecs
	);

	cout << "Calculated UTC time: " << ullUTCInSecs * 1000 << endl;

	ulYear = tmDateTime.tm_year + 1900;
	ulMonth = tmDateTime.tm_mon + 1;
	ulDay = tmDateTime.tm_mday;
	ulHour = tmDateTime.tm_hour;
	ulMinutes = tmDateTime.tm_min;
	ulSeconds = tmDateTime.tm_sec;

	cout << "today: " << tmDateTime.tm_year + 1900 << "/" << tmDateTime.tm_mon + 1 << "/" << tmDateTime.tm_mday << " " << tmDateTime.tm_hour << ":"
		 << tmDateTime.tm_min << ":" << tmDateTime.tm_sec << endl;

	Datetime::addSeconds(
		tmDateTime.tm_year + 1900, tmDateTime.tm_mon + 1, tmDateTime.tm_mday, tmDateTime.tm_hour, tmDateTime.tm_min, tmDateTime.tm_sec, -1,
		365 * 24 * 3600, &ulYear, &ulMonth, &ulDay, &ulHour, &ulMinutes, &ulSeconds, &bDaylightSavingTime
	);

	cout << "today + 365 days: " << ulYear << "/" << ulMonth << "/" << ulDay << " " << ulHour << ":" << ulMinutes << ":" << ulSeconds
		 << ", daylight saving time: " << bDaylightSavingTime << endl;

	return 0;
}
