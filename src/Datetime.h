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

#ifndef Datetime_h
#define Datetime_h

#include <chrono>
#include <string>

using namespace std;

class Datetime
{
  public:
	static string timePointAsLocalString(chrono::system_clock::time_point t);
	static string timePointAsUtcString(chrono::system_clock::time_point t);
	static string localToUtcString(tm localTime);
	static tm utcSecondsToLocalTime(time_t utcTime);

	/**
		The plTimeZoneDifferenceInHours parameter could be also NULL,
		in that case the variable is not initialized.
	*/
	static void nowUTCInMilliSecs(unsigned long long *pullNowUTCInSecs, unsigned long *pulAdditionalMilliSecs, long *plTimeZoneDifferenceInHours);

	/**
		The plTimeZoneDifferenceInHours parameter could be also NULL,
		in that case the variable is not initialized.
	*/
	static void nowUTCInMilliSecs(unsigned long long *pullNowUTCInMilliSecs, long *plTimeZoneDifferenceInHours);

	static void nowLocalInMilliSecs(unsigned long long *pullNowLocalInMilliSecs);

	// ulTextFormat:
	// 	1: "YYYY-MM-DD HH:MI:SS"
	// 	2: "YYYY_MM_DD"
	static string nowLocalTime(unsigned long ulTextFormat);

	static void getTimeZoneInformation(long *plTimeZoneDifferenceInHours);

	static long getTimeZoneInformation(void);

	static void get_tm_LocalTime(tm *ptmDateTime, unsigned long *pulMilliSecs);

	/**
		The mktime function converts the supplied time structure
		(possibly incomplete)
		pointed to by timeptr into a fully defined structure
		with normalized values
		and then converts it to a time_t calendar time value.
		Here is an example to fill is the tm structure
			tmDateTime. tm_year		= ...;
			tmDateTime. tm_mon		= ...;
			tmDateTime. tm_mday		= ...;
			tmDateTime. tm_hour		= ...;
			tmDateTime. tm_min		= ...;
			tmDateTime. tm_sec		= ...;

			tmDateTime. tm_year		-= 1900;
			tmDateTime. tm_mon		-= 1;

			tmDateTime. tm_isdst	 -1;
	*/
	static void convertFromLocalToUTC(tm *ptmDateTime, time_t *ptUTCTime);

	static void convertFromLocalToUTC(tm *ptmLocalDateTime, tm *ptmUTCDateTime);

	/**
		Convert a UTC time to local time
	*/
	static void convertFromUTCToLocal(time_t tUTCTime, tm *ptmLocalDateTime);

	static void convertFromUTCInSecondsToBreakDownUTC(time_t tUTCTime, tm *ptmUTCDateTime);

	/**
		Convert a date time (local) in seconds (local)
		ulYear: i.e. 2011
		lDaylightSavingTime could be:
			- 0: DaylightSavingTime NO present for the input time (ulSrcYear, ...)
			- 1: DaylightSavingTime present for the input time (ulSrcYear, ...)
			- -1: information about DaylightSavingTime not available
	*/
	static void convertFromLocalDateTimeToLocalInSecs(
		unsigned long ulYear, unsigned long ulMon, unsigned long ulDay, unsigned long ulHour, unsigned long ulMin, unsigned long ulSec,
		long lDaylightSavingTime, unsigned long long *pullLocalInSecs
	);

	/**
		Add or substruct a number of seconds to a
		specific date time.

		The llSecondsToAdd parameter represents the
		number of seconds to be added (substructed) and
		could be also e negative value

		lDaylightSavingTime could be:
			- 0: DaylightSavingTime NO present for the input time (ulSrcYear, ...)
			- 1: DaylightSavingTime present for the input time (ulSrcYear, ...)
			- -1: information about DaylightSavingTime not available
		The output pointers parameters (pulDestYear, pulDestMonth, ...)
		could be also the same addresses of the input parameters
		(ulSrcYear, ulSrcMonth, ...)
	*/
	static void addSeconds(
		unsigned long ulSrcYear, unsigned long ulSrcMonth, unsigned long ulSrcDay, unsigned long ulSrcHour, unsigned long ulSrcMinutes,
		unsigned long ulSrcSeconds, long lSrcDaylightSavingTime, long long llSecondsToAdd, unsigned long *pulYear, unsigned long *pulMonth,
		unsigned long *pulDay, unsigned long *pulHour, unsigned long *pulMinutes, unsigned long *pulSeconds, bool *pbDaylightSavingTime
	);

	static void isLeapYear(unsigned long ulYear, bool *pbIsLeapYear);

	static void getLastDayOfMonth(unsigned long ulYear, unsigned long ulMonth, unsigned long *pulLastDayOfMonth);

	static long sTimeToMilliSecs(string sTime);
	static time_t sDateSecondsToUtc(string sDate);
	static time_t sDateSecondsToUtcInMs(string sDate);
	static int64_t sDateMilliSecondsToUtc(string sDate);
	static string utcToUtcString(time_t utc);
	static string utcToLocalString(time_t utc);
};

#endif
