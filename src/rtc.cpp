/*
 * rtc.cpp
 *
 *  Created on: Oct 25, 2018
 *      Author: ipaev
 */

/*********************************************************************************************\
 * Real Time Clock
 *
 * Sources: Time by Michael Margolis and Paul Stoffregen (https://github.com/PaulStoffregen/Time)
 *          Timezone by Jack Christensen (https://github.com/JChristensen/Timezone)
\*********************************************************************************************/

#include <Arduino.h>
#include <Ticker.h>
#include "WiFiManager.h"
#include <sntp.h>

#include <settings.hpp>

#include <common.hpp>

struct TIME_T {
  uint8_t       second;
  uint8_t       minute;
  uint8_t       hour;
  uint8_t       day_of_week;               // sunday is day 1
  uint8_t       day_of_month;
  uint8_t       month;
  char          name_of_month[4];
  uint16_t      day_of_year;
  uint16_t      year;
  unsigned long days;
  unsigned long valid;
} RtcTime;

void BreakTime(uint32_t time_input, TIME_T &tm);

// "2017-03-07T11:08:02" - ISO8601:2004
#define D_YEAR_MONTH_SEPARATOR "-"
#define D_MONTH_DAY_SEPARATOR "-"
#define D_DATE_TIME_SEPARATOR "T"
#define D_HOUR_MINUTE_SEPARATOR ":"
#define D_MINUTE_SECOND_SEPARATOR ":"

#define D_DAY3LIST "SunMonTueWedThuFriSat"
#define D_MONTH3LIST "JanFebMarAprMayJunJulAugSepOctNovDec"
#define D_UTC_TIME "UTC"

static const char kMonthNames[] = D_MONTH3LIST;

#define SECS_PER_MIN  ((uint32_t)(60UL))
#define SECS_PER_HOUR ((uint32_t)(3600UL))
#define SECS_PER_DAY  ((uint32_t)(SECS_PER_HOUR * 24UL))
#define LEAP_YEAR(Y)  (((1970+Y)>0) && !((1970+Y)%4) && (((1970+Y)%100) || !((1970+Y)%400)))

Ticker TickerRtc;
extern uint32_t uptime;                        // Counting every second until 4294967295 = 130 year
static const uint8_t kDaysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // API starts months from 1, this array starts from 0
static const char kMonthNamesEnglish[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

uint32_t utc_time = 0;
uint32_t local_time = 0;
uint32_t daylight_saving_time = 0;
uint32_t standard_time = 0;
uint32_t ntp_time = 0;
uint32_t midnight = 1451602800;
uint32_t restart_time = 0;
int16_t  time_timezone = 0;  // Timezone * 10
uint8_t  midnight_now = 0;
uint8_t  ntp_sync_minute = 0;

extern Settings_t Settings;

String GetBuildDateAndTime()
{
  // "2017-03-07T11:08:02" - ISO8601:2004
  char bdt[21];
  char *p;
  char mdate[] = __DATE__;  // "Mar  7 2017"
  char *smonth = mdate;
  int day = 0;
  int year = 0;

  // sscanf(mdate, "%s %d %d", bdt, &day, &year);  // Not implemented in 2.3.0 and probably too much code
  byte i = 0;
  for (char *str = strtok_r(mdate, " ", &p); str && i < 3; str = strtok_r(NULL, " ", &p)) {
    switch (i++) {
    case 0:  // Month
      smonth = str;
      break;
    case 1:  // Day
      day = atoi(str);
      break;
    case 2:  // Year
      year = atoi(str);
    }
  }
  int month = (strstr(kMonthNamesEnglish, smonth) -kMonthNamesEnglish) /3 +1;
  snprintf_P(bdt, sizeof(bdt), PSTR("%d" D_YEAR_MONTH_SEPARATOR "%02d" D_MONTH_DAY_SEPARATOR "%02d" D_DATE_TIME_SEPARATOR "%s"), year, month, day, __TIME__);
  return String(bdt);
}

/*
 * timestamps in https://en.wikipedia.org/wiki/ISO_8601 format
 *
 *  DT_UTC - current data and time in Greenwich, England (aka GMT)
 *  DT_LOCAL - current date and time taking timezone into account
 *  DT_RESTART - the date and time this device last started, in local timezone
 *
 * Format:
 *  "2017-03-07T11:08:02-07:00" - if DT_LOCAL and SetOption52 = 1
 *  "2017-03-07T11:08:02"       - otherwise
 */
String GetDateAndTime(int mode)
{
  // "2017-03-07T11:08:02-07:00" - ISO8601:2004
  char dt[27];
  TIME_T tmpTime;

  uint32_t time_input;
  if(mode == 0) time_input = utc_time;
  else time_input = local_time;

  BreakTime(time_input, tmpTime);
  tmpTime.year += 1970;

  snprintf_P(dt, sizeof(dt), PSTR("%04d-%02d-%02dT%02d:%02d:%02d"),
    tmpTime.year, tmpTime.month, tmpTime.day_of_month, tmpTime.hour, tmpTime.minute, tmpTime.second);

  return String(dt);
}

String GetUptime()
{
  char dt[16];

  TIME_T ut;

  if (restart_time) {
    BreakTime(utc_time - restart_time, ut);
  } else {
    BreakTime(uptime, ut);
  }

  // "P128DT14H35M44S" - ISO8601:2004 - https://en.wikipedia.org/wiki/ISO_8601 Durations
//  snprintf_P(dt, sizeof(dt), PSTR("P%dDT%02dH%02dM%02dS"), ut.days, ut.hour, ut.minute, ut.second);

  // "128 14:35:44" - OpenVMS
  // "128T14:35:44" - Tasmota
  snprintf_P(dt, sizeof(dt), PSTR("%dT%02d:%02d:%02d"),
    ut.days, ut.hour, ut.minute, ut.second);
  return String(dt);
}

uint32_t GetMinutesUptime()
{
  TIME_T ut;

  if (restart_time) {
    BreakTime(utc_time - restart_time, ut);
  } else {
    BreakTime(uptime, ut);
  }

  return (ut.days *1440) + (ut.hour *60) + ut.minute;
}

uint32_t GetMinutesPastMidnight()
{
  uint32_t minutes = 0;

  if (RtcTime.valid) {
    minutes = (RtcTime.hour *60) + RtcTime.minute;
  }
  return minutes;
}

void BreakTime(uint32_t time_input, TIME_T &tm)
{
// break the given time_input into time components
// this is a more compact version of the C library localtime function
// note that year is offset from 1970 !!!

  uint8_t year;
  uint8_t month;
  uint8_t month_length;
  uint32_t time;
  unsigned long days;

  time = time_input;
  tm.second = time % 60;
  time /= 60;                // now it is minutes
  tm.minute = time % 60;
  time /= 60;                // now it is hours
  tm.hour = time % 24;
  time /= 24;                // now it is days
  tm.days = time;
  tm.day_of_week = ((time + 4) % 7) + 1;  // Sunday is day 1

  year = 0;
  days = 0;
  while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  tm.year = year;            // year is offset from 1970

  days -= LEAP_YEAR(year) ? 366 : 365;
  time -= days;              // now it is days in this year, starting at 0
  tm.day_of_year = time;

  days = 0;
  month = 0;
  month_length = 0;
  for (month = 0; month < 12; month++) {
    if (1 == month) { // february
      if (LEAP_YEAR(year)) {
        month_length = 29;
      } else {
        month_length = 28;
      }
    } else {
      month_length = kDaysInMonth[month];
    }

    if (time >= month_length) {
      time -= month_length;
    } else {
      break;
    }
  }
  strlcpy(tm.name_of_month, kMonthNames + (month *3), 4);
  tm.month = month + 1;      // jan is month 1
  tm.day_of_month = time + 1;         // day of month
  tm.valid = (time_input > 1451602800);  // 2016-01-01
}

uint32_t MakeTime(TIME_T &tm)
{
// assemble time elements into time_t
// note year argument is offset from 1970

  int i;
  uint32_t seconds;

  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds = tm.year * (SECS_PER_DAY * 365);
  for (i = 0; i < tm.year; i++) {
    if (LEAP_YEAR(i)) {
      seconds +=  SECS_PER_DAY;   // add extra days for leap years
    }
  }

  // add days for this year, months start from 1
  for (i = 1; i < tm.month; i++) {
    if ((2 == i) && LEAP_YEAR(tm.year)) {
      seconds += SECS_PER_DAY * 29;
    } else {
      seconds += SECS_PER_DAY * kDaysInMonth[i-1];  // monthDay array starts from 0
    }
  }
  seconds+= (tm.day_of_month - 1) * SECS_PER_DAY;
  seconds+= tm.hour * SECS_PER_HOUR;
  seconds+= tm.minute * SECS_PER_MIN;
  seconds+= tm.second;
  return seconds;
}

String GetTime()
{
  char stime[25];   // Skip newline
  snprintf_P(stime, sizeof(stime), sntp_get_real_time(local_time));
  return String(stime);
}

uint32_t LocalTime()
{
  return local_time;
}

uint32_t Midnight()
{
  return midnight;
}

boolean MidnightNow()
{
  boolean mnflg = midnight_now;
  if (mnflg) midnight_now = 0;
  return mnflg;
}

void RtcSecond()
{
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);

  TIME_T tmpTime;

  if ((ntp_sync_minute > 59) && (RtcTime.minute > 2)) ntp_sync_minute = 1;                 // If sync prepare for a new cycle
  uint8_t offset = (uptime < 30) ? RtcTime.second : (((ESP.getChipId() & 0xF) * 3) + 3) ;  // First try ASAP to sync. If fails try once every 60 seconds based on chip id
  if ((WL_CONNECTED == WiFi.status()) && (offset == RtcTime.second) && ((RtcTime.year < 2016) || (ntp_sync_minute == RtcTime.minute))) {
    ntp_time = sntp_get_current_timestamp();
    if (ntp_time > 1451602800) {  // Fix NTP bug in core 2.4.1/SDK 2.2.1 (returns Thu Jan 01 08:00:10 1970 after power on)
      utc_time = ntp_time;
      ntp_sync_minute = 60;  // Sync so block further requests
      if (restart_time == 0) {
        restart_time = utc_time - uptime;  // save first ntp time as restart time
      }
      BreakTime(utc_time, tmpTime);
      RtcTime.year = tmpTime.year + 1970;
      addToLog(LOG_LEVEL_DEBUG, "(" D_UTC_TIME ") %s", GetTime().c_str());
    } else {
      ntp_sync_minute++;  // Try again in next minute
    }
  }
  utc_time++;
  local_time = utc_time;
  if (local_time > 1451602800) {  // 2016-01-01
    int32_t time_offset = Settings.TimeZone * SECS_PER_HOUR;
    local_time += time_offset;
    time_timezone = time_offset / 360;  // (SECS_PER_HOUR / 10) fails as it is defined as UL
  }
  BreakTime(local_time, RtcTime);
  if (!RtcTime.hour && !RtcTime.minute && !RtcTime.second && RtcTime.valid) {
    midnight = local_time;
    midnight_now = 1;
  }
  RtcTime.year += 1970;
}

void RtcInit()
{
  sntp_setservername(0, Settings.ntp_server[0]);
  sntp_setservername(1, Settings.ntp_server[1]);
  sntp_setservername(2, Settings.ntp_server[2]);
  sntp_stop();
  sntp_set_timezone(0);      // UTC time
  sntp_init();
  utc_time = 0;
  BreakTime(utc_time, RtcTime);
  TickerRtc.attach(1, RtcSecond);
}


