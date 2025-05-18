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

// #include <string>

#include "Datetime.h"
#include <stdexcept>
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif
// #include "spdlog/spdlog.h"
#include <format>
/*
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
*/

// 2021-02-26 15:41:15
string Datetime::timePointAsLocalString(chrono::system_clock::time_point t)
{
	tm tmDateTime;
	// char strDateTime[64];
	time_t utcTime = chrono::system_clock::to_time_t(t);
#ifdef WIN32
	localtime_s(&tmDateTime, &utcTime);
#else
	localtime_r(&utcTime, &tmDateTime);
#endif

	return std::format(
		"{:0>4}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}", tmDateTime.tm_year + 1900, tmDateTime.tm_mon + 1, tmDateTime.tm_mday, tmDateTime.tm_hour,
		tmDateTime.tm_min, tmDateTime.tm_sec
	);
}

// 2021-02-26T15:41:15Z
string Datetime::timePointAsUtcString(chrono::system_clock::time_point t)
{
	tm tmDateTime;
	// char strDateTime[64];
	time_t utcTime = chrono::system_clock::to_time_t(t);
#ifdef _WIN32
	gmtime_s(&tmDateTime, &utcTime);
#else
	gmtime_r(&utcTime, &tmDateTime);
#endif

	return std::format(
		"{:0>4}-{:0>2}-{:0>2}T{:0>2}:{:0>2}:{:0>2}Z", tmDateTime.tm_year + 1900, tmDateTime.tm_mon + 1, tmDateTime.tm_mday, tmDateTime.tm_hour,
		tmDateTime.tm_min, tmDateTime.tm_sec
	);
}

string Datetime::localToUtcString(tm localTime)
{
#ifdef _WIN32
	time_t utcTime = _mkgmtime(&localTime);
#else
	time_t utcTime = timegm(&localTime);
#endif

	return utcToUtcString(utcTime);
}

tm Datetime::utcSecondsToLocalTime(time_t utcTime)
{
	tm tmDateTime;

#ifdef _WIN32
	localtime_s(&tmDateTime, &utcTime);
#else
	localtime_r(&utcTime, &tmDateTime);
#endif
	return tmDateTime;
}

void Datetime::nowUTCInMilliSecs(unsigned long long *pullNowUTCInSecs, unsigned long *pulAdditionalMilliSecs, long *plTimeZoneDifferenceInHours)
{
#ifdef _WIN32
	SYSTEMTIME stSystemTime;
#else
	struct timeval tvTimeval;
#endif

#ifdef _WIN32
	GetLocalTime(&stSystemTime);

	(*pullNowUTCInSecs) = time(NULL);
	(*pulAdditionalMilliSecs) = stSystemTime.wMilliseconds;

	if (plTimeZoneDifferenceInHours != (long *)NULL)
		Datetime::getTimeZoneInformation(plTimeZoneDifferenceInHours);
#else
	if (plTimeZoneDifferenceInHours != (long *)NULL)
	{
		struct timezone tz;

		gettimeofday(&tvTimeval, &tz);

		*plTimeZoneDifferenceInHours = ((tz.tz_minuteswest / 60) * -1);
	}
	else
	{
		gettimeofday(&tvTimeval, NULL);
	}

	(*pullNowUTCInSecs) = tvTimeval.tv_sec;
	(*pulAdditionalMilliSecs) = (tvTimeval.tv_usec / 1000);
#endif
}

void Datetime::nowUTCInMilliSecs(unsigned long long *pullNowUTCInMilliSecs, long *plTimeZoneDifferenceInHours)
{
	unsigned long long ullNowUTCInSecs;
	unsigned long ulAdditionalMilliSecs;

	Datetime::nowUTCInMilliSecs(&ullNowUTCInSecs, &ulAdditionalMilliSecs, plTimeZoneDifferenceInHours);

	(*pullNowUTCInMilliSecs) = ullNowUTCInSecs;
	(*pullNowUTCInMilliSecs) *= 1000;
	(*pullNowUTCInMilliSecs) += ulAdditionalMilliSecs;
}

void Datetime::nowLocalInMilliSecs(unsigned long long *pullNowLocalInMilliSecs)
{
	unsigned long long ullNowUTCInMilliSecs;
	long lTimeZoneDifferenceInHours;

	Datetime::nowUTCInMilliSecs(&ullNowUTCInMilliSecs, &lTimeZoneDifferenceInHours);

	*pullNowLocalInMilliSecs = ullNowUTCInMilliSecs + (lTimeZoneDifferenceInHours * 3600 * 1000);
}

string Datetime::nowLocalTime(unsigned long ulTextFormat)
{
	tm tmDateTime;
	unsigned long ulMilliSecs;

	if (ulTextFormat != 1 && ulTextFormat != 2)
	{
		string errorMessage = std::format(
			"Wrong input"
			", ulTextFormat: {}",
			ulTextFormat
		);
		// SPDLOG_ERROR(errorMessage);

		throw runtime_error(errorMessage);
	}

	Datetime::get_tm_LocalTime(&tmDateTime, &ulMilliSecs);

	string sDateTime;
	if (ulTextFormat == 1)
		sDateTime = std::format(
			"{:0>4}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}", tmDateTime.tm_year + 1900, tmDateTime.tm_mon + 1, tmDateTime.tm_mday, tmDateTime.tm_hour,
			tmDateTime.tm_min, tmDateTime.tm_sec
		);
	else if (ulTextFormat == 2)
		sDateTime = std::format("{:0>4}_{:0>2}_{:0>2}", tmDateTime.tm_year + 1900, tmDateTime.tm_mon + 1, tmDateTime.tm_mday);

	return sDateTime;
}

void Datetime::getTimeZoneInformation(long *plTimeZoneDifferenceInHours)
{
#ifdef _WIN32
	TIME_ZONE_INFORMATION tzInfo;

	::GetTimeZoneInformation(&tzInfo);

	*plTimeZoneDifferenceInHours = ((tzInfo.Bias / 60) * (-1));
#else
	struct timeval tv;
	struct timezone tz;

	if (::gettimeofday(&tv, &tz) != 0)
	{
		string errorMessage = std::format("gettimeofday failed");
		// SPDLOG_ERROR(errorMessage);
		throw runtime_error(errorMessage);
	}

	*plTimeZoneDifferenceInHours = ((tz.tz_minuteswest / 60) * (-1));
#endif
}

long Datetime::getTimeZoneInformation(void)
{
	long lTimeZoneDifferenceInHours;

	Datetime::getTimeZoneInformation(&lTimeZoneDifferenceInHours);

	return lTimeZoneDifferenceInHours;
}

void Datetime::get_tm_LocalTime(tm *ptmDateTime, unsigned long *pulMilliSecs)
{
#ifdef _WIN32
	time_t tTime;
	SYSTEMTIME stSystemTime;

	tTime = time(NULL);

	Datetime::convertFromUTCToLocal(tTime, ptmDateTime);

	GetLocalTime(&stSystemTime);

	*pulMilliSecs = stSystemTime.wMilliseconds;
#else
	struct timeval tvTimeval;

	if (gettimeofday(&tvTimeval, NULL) == -1)
	{
		string errorMessage = std::format("gettimeofday failed");
		// SPDLOG_ERROR(errorMessage);
		throw runtime_error(errorMessage);
	}

	Datetime::convertFromUTCToLocal(tvTimeval.tv_sec, ptmDateTime);

	*pulMilliSecs = tvTimeval.tv_usec / 1000;
#endif
}

void Datetime::convertFromLocalToUTC(tm *ptmDateTime, time_t *ptUTCTime) { *ptUTCTime = localToUTC(ptmDateTime); }

time_t Datetime::localToUTC(tm *ptmDateTime) { return mktime(ptmDateTime); }

void Datetime::convertFromLocalToUTC(tm *ptmLocalDateTime, tm *ptmUTCDateTime)
{
	time_t tUTCTime;

	convertFromLocalToUTC(ptmLocalDateTime, &tUTCTime);

	convertFromUTCInSecondsToBreakDownUTC(tUTCTime, ptmUTCDateTime);
}

void Datetime::convertFromUTCInSecondsToBreakDownUTC(time_t tUTCTime, tm *ptmUTCDateTime)
{
#ifdef _WIN32
	gmtime_s(ptmUTCDateTime, &tUTCTime);
#else
	gmtime_r(&tUTCTime, ptmUTCDateTime);
#endif
}

void Datetime::convertFromLocalDateTimeToLocalInSecs(
	unsigned long ulYear, unsigned long ulMon, unsigned long ulDay, unsigned long ulHour, unsigned long ulMin, unsigned long ulSec,
	long lDaylightSavingTime, unsigned long long *pullLocalInSecs
)
{
	long lTimeZoneDifferenceInHours;
	time_t tUTCTime;
	tm tmDateTime;

	tmDateTime.tm_year = ulYear - 1900;
	tmDateTime.tm_mon = ulMon - 1;
	tmDateTime.tm_mday = ulDay;
	tmDateTime.tm_hour = ulHour;
	tmDateTime.tm_min = ulMin;
	tmDateTime.tm_sec = ulSec;

	//	Negative value if status of daylight saving time is unknown.
	// (does it mean it attempt to determine whether Daylight Saving Time is in effect
	//	for the specified time?)
	tmDateTime.tm_isdst = lDaylightSavingTime;

	// Simulate as the date is a local date
	Datetime::convertFromLocalToUTC(&tmDateTime, &tUTCTime);

	Datetime::getTimeZoneInformation(&lTimeZoneDifferenceInHours);

	*pullLocalInSecs = ((unsigned long long)tUTCTime) + (((unsigned long long)lTimeZoneDifferenceInHours) * 3600);
}

void Datetime::convertFromUTCToLocal(time_t tUTCTime, tm *ptmLocalDateTime)
{
#ifdef _WIN32
	localtime_s(ptmLocalDateTime, &tUTCTime);
#else
	localtime_r(&tUTCTime, ptmLocalDateTime);
#endif
}

void Datetime::addSeconds(
	unsigned long ulSrcYear, unsigned long ulSrcMonth, unsigned long ulSrcDay, unsigned long ulSrcHour, unsigned long ulSrcMinutes,
	unsigned long ulSrcSeconds, long lSrcDaylightSavingTime, long long llSecondsToAdd, unsigned long *pulDestYear, unsigned long *pulDestMonth,
	unsigned long *pulDestDay, unsigned long *pulDestHour, unsigned long *pulDestMinutes, unsigned long *pulDestSeconds,
	bool *pbDestDaylightSavingTime
)
{
	// initialize the tm struct (the tm_wday field too)
	time_t tUtcTime;
	tm tmDateTime;

	if (llSecondsToAdd == 0)
	{
		*pulDestYear = ulSrcYear;
		*pulDestMonth = ulSrcMonth;
		*pulDestDay = ulSrcDay;
		*pulDestHour = ulSrcHour;
		*pulDestMinutes = ulSrcMinutes;
		*pulDestSeconds = ulSrcSeconds;
		*pbDestDaylightSavingTime = lSrcDaylightSavingTime;

		return;
	}

	tmDateTime.tm_year = ulSrcYear;
	tmDateTime.tm_mon = ulSrcMonth;
	tmDateTime.tm_mday = ulSrcDay;
	tmDateTime.tm_hour = ulSrcHour;
	tmDateTime.tm_min = ulSrcMinutes;
	tmDateTime.tm_sec = ulSrcSeconds;

	tmDateTime.tm_year -= 1900;
	tmDateTime.tm_mon -= 1;

	//	A negative value for tm_isdst causes mktime() to attempt
	//	to determine whether Daylight Saving Time is in effect
	//	for the specified time.
	tmDateTime.tm_isdst = lSrcDaylightSavingTime;

	tUtcTime = mktime(&tmDateTime);

	tUtcTime = tUtcTime + llSecondsToAdd;

	Datetime::convertFromUTCToLocal(tUtcTime, &tmDateTime);

	*pulDestYear = tmDateTime.tm_year + 1900;
	*pulDestMonth = tmDateTime.tm_mon + 1;
	*pulDestDay = tmDateTime.tm_mday;
	*pulDestHour = tmDateTime.tm_hour;
	*pulDestMinutes = tmDateTime.tm_min;
	*pulDestSeconds = tmDateTime.tm_sec;

	if (tmDateTime.tm_isdst == 1)
		*pbDestDaylightSavingTime = true;
	else
		*pbDestDaylightSavingTime = false;
}

void Datetime::isLeapYear(unsigned long ulYear, bool *pbIsLeapYear)
{
	if (ulYear % 4 != 0)
		*pbIsLeapYear = false;

	if (ulYear % 400 == 0)
		*pbIsLeapYear = true;

	if (ulYear % 100 == 0)
		*pbIsLeapYear = false;

	*pbIsLeapYear = true;
}

void Datetime::getLastDayOfMonth(unsigned long ulYear, unsigned long ulMonth, unsigned long *pulLastDayOfMonth)
{
	bool bIsLeapYear;
	int piDaysInMonths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (ulMonth <= 0 || ulMonth > 12)
	{
		string errorMessage = std::format(
			"Wrong input"
			", ulMonth: {}",
			ulMonth
		);
		// SPDLOG_ERROR(errorMessage);
		throw runtime_error(errorMessage);
	}

	Datetime::isLeapYear(ulYear, &bIsLeapYear);

	*pulLastDayOfMonth = piDaysInMonths[ulMonth - 1];

	if (ulMonth == 2 && bIsLeapYear) // february of a leap year
		(*pulLastDayOfMonth) += 1;
}

// 2021-02-26T15:41:15Z
time_t Datetime::sDateSecondsToUtc(string sDate)
{

	unsigned long ulUTCYear;
	unsigned long ulUTCMonth;
	unsigned long ulUTCDay;
	unsigned long ulUTCHour;
	unsigned long ulUTCMinutes;
	unsigned long ulUTCSeconds;
	tm tmDate;
	int sscanfReturn;

#ifdef _WIN32
	if ((sscanfReturn =
			 sscanf_s(sDate.c_str(), "%4lu-%2lu-%2luT%2lu:%2lu:%2luZ", &ulUTCYear, &ulUTCMonth, &ulUTCDay, &ulUTCHour, &ulUTCMinutes, &ulUTCSeconds)
		) != 6)
#else
	if ((sscanfReturn =
			 sscanf(sDate.c_str(), "%4lu-%2lu-%2luT%2lu:%2lu:%2luZ", &ulUTCYear, &ulUTCMonth, &ulUTCDay, &ulUTCHour, &ulUTCMinutes, &ulUTCSeconds)) !=
		6)
#endif
	{
		string errorMessage = std::format(
			"Field has a wrong format (sscanf failed)"
			", sDate: {}"
			", sscanfReturn: {}",
			sDate, sscanfReturn
		);
		// SPDLOG_ERROR(errorMessage);

		throw runtime_error(errorMessage);
	}

	time_t utcTime;

	time(&utcTime);
#ifdef _WIN32
	gmtime_s(&tmDate, &utcTime);
#else
	gmtime_r(&utcTime, &tmDate);
#endif

	tmDate.tm_year = ulUTCYear - 1900;
	tmDate.tm_mon = ulUTCMonth - 1;
	tmDate.tm_mday = ulUTCDay;
	tmDate.tm_hour = ulUTCHour;
	tmDate.tm_min = ulUTCMinutes;
	tmDate.tm_sec = ulUTCSeconds;

#ifdef _WIN32
	utcTime = _mkgmtime(&tmDate);
#else
	utcTime = timegm(&tmDate);
#endif

	return utcTime;
}

// 2021-02-26T15:41:15Z
time_t Datetime::sDateSecondsToUtcInMs(string sDate) { return Datetime::sDateSecondsToUtc(sDate) * 1000; }

// HH:MM
long Datetime::sTimeToMilliSecs(string sTime)
{
	int hours;
	int minutes;
	int sscanfReturn;

#ifdef _WIN32
	if ((sscanfReturn = sscanf_s(sTime.c_str(), "%2d:%2d", &hours, &minutes)) != 2)
#else
	if ((sscanfReturn = sscanf(sTime.c_str(), "%2d:%2d", &hours, &minutes)) != 2)
#endif
	{
		string errorMessage = std::format(
			"Field has a wrong format (sscanf failed)"
			", sTime: {}"
			", sscanfReturn: {}",
			sTime, sscanfReturn
		);

		return -1;
	}

	return (hours * 3600 + minutes * 60) * 1000;
}

// 2021-02-26T15:41:15Z
string Datetime::utcToUtcString(time_t utc)
{
	tm tmDateTime;

#ifdef _WIN32
	gmtime_s(&tmDateTime, &utc);
#else
	gmtime_r(&utc, &tmDateTime);
#endif

	return std::format(
		"{:0>4}-{:0>2}-{:0>2}T{:0>2}:{:0>2}:{:0>2}Z", tmDateTime.tm_year + 1900, tmDateTime.tm_mon + 1, tmDateTime.tm_mday, tmDateTime.tm_hour,
		tmDateTime.tm_min, tmDateTime.tm_sec
	);
}

// 2021-02-26T15:41:15
string Datetime::utcToLocalString(time_t utc)
{
	tm tmDateTime = utcSecondsToLocalTime(utc);

	return std::format(
		"{:0>4}-{:0>2}-{:0>2}T{:0>2}:{:0>2}:{:0>2}", tmDateTime.tm_year + 1900, tmDateTime.tm_mon + 1, tmDateTime.tm_mday, tmDateTime.tm_hour,
		tmDateTime.tm_min, tmDateTime.tm_sec
	);
}

// 2021-02-26T15:41:15.477+0100 (ISO8610)
// 2021-02-26T15:41:15.477Z
int64_t Datetime::sDateMilliSecondsToUtc(string sDate)
{

	unsigned long ulUTCYear;
	unsigned long ulUTCMonth;
	unsigned long ulUTCDay;
	unsigned long ulUTCHour;
	unsigned long ulUTCMinutes;
	unsigned long ulUTCSeconds;
	unsigned long ulUTCMilliSeconds;
	unsigned long ulHourTimeZone;
	unsigned long ulMinuteTimeZone;
	tm tmDate;
	int sscanfReturn;

	char signTimeZone = '+';
	string dateFormat;
	if (sDate.size() == 28)
	{
		// 2021-02-26T15:41:15.477+0100 (ISO8610)

		signTimeZone = sDate[23];

		dateFormat = std::format("%4lu-%2lu-%2luT%2lu:%2lu:%2lu.%3lu{}%2lu%2lu", signTimeZone);
	}
	else if (sDate.size() == 24)
	{
		// 2021-02-26T15:41:15.477Z
		dateFormat = "%4lu-%2lu-%2luT%2lu:%2lu:%2lu.%3luZ";

		ulHourTimeZone = 0;
		ulMinuteTimeZone = 0;
	}
	else
	{
		string errorMessage = std::format(
			"Wrong date format"
			", sDate: {}",
			sDate
		);
		// SPDLOG_ERROR(errorMessage);

		throw runtime_error(errorMessage);
	}

	if (sDate.size() == 28)
	{
#ifdef _WIN32
		if ((sscanfReturn = sscanf_s(
				 sDate.c_str(), dateFormat.c_str(), &ulUTCYear, &ulUTCMonth, &ulUTCDay, &ulUTCHour, &ulUTCMinutes, &ulUTCSeconds, &ulUTCMilliSeconds,
				 &ulHourTimeZone, &ulMinuteTimeZone
			 )) != 9)
#else
		if ((sscanfReturn = sscanf(
				 sDate.c_str(), dateFormat.c_str(), &ulUTCYear, &ulUTCMonth, &ulUTCDay, &ulUTCHour, &ulUTCMinutes, &ulUTCSeconds, &ulUTCMilliSeconds,
				 &ulHourTimeZone, &ulMinuteTimeZone
			 )) != 9)
#endif
		{
			string errorMessage = std::format(
				"Field has a wrong format (sscanf failed)"
				", sDate: {}"
				", sscanfReturn: {}",
				sDate, sscanfReturn
			);
			// SPDLOG_ERROR(errorMessage);

			throw runtime_error(errorMessage);
		}
	}
	else
	{
#ifdef _WIN32
		if ((sscanfReturn = sscanf_s(
				 sDate.c_str(), dateFormat.c_str(), &ulUTCYear, &ulUTCMonth, &ulUTCDay, &ulUTCHour, &ulUTCMinutes, &ulUTCSeconds, &ulUTCMilliSeconds
			 )) != 7)
#else
		if ((sscanfReturn = sscanf(
				 sDate.c_str(), dateFormat.c_str(), &ulUTCYear, &ulUTCMonth, &ulUTCDay, &ulUTCHour, &ulUTCMinutes, &ulUTCSeconds, &ulUTCMilliSeconds
			 )) != 7)
#endif
		{
			string errorMessage = std::format(
				"Field has a wrong format (sscanf failed)"
				", sDate: {}"
				", sscanfReturn: {}",
				sDate, sscanfReturn
			);
			// SPDLOG_ERROR(errorMessage);

			throw runtime_error(errorMessage);
		}
	}

	{
		time_t utcTime;

		time(&utcTime);
#ifdef _WIN32
		gmtime_s(&tmDate, &utcTime);
#else
		gmtime_r(&utcTime, &tmDate);
#endif

		tmDate.tm_year = ulUTCYear - 1900;
		tmDate.tm_mon = ulUTCMonth - 1;
		tmDate.tm_mday = ulUTCDay;
		tmDate.tm_hour = ulUTCHour;
		tmDate.tm_min = ulUTCMinutes;
		tmDate.tm_sec = ulUTCSeconds;
	}

#ifdef _WIN32
	int64_t utcTime = _mkgmtime(&tmDate) * 1000;
#else
	int64_t utcTime = timegm(&tmDate) * 1000;
#endif
	utcTime += ulUTCMilliSeconds;

	if (signTimeZone == '+')
		utcTime -= (((ulHourTimeZone * 3600) + (ulMinuteTimeZone * 60)) * 1000);
	else // if (signTimeZone == '-')
		utcTime += (((ulHourTimeZone * 3600) + (ulMinuteTimeZone * 60)) * 1000);

	return utcTime;
}
