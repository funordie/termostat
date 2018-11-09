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

#include <NtpClientLib.h>
#include <settings.hpp>

#include <common.hpp>


extern Settings_t Settings;

/*
 */
String GetDateAndTime()
{
    String str = NTP.getTimeDateString ();
//    addToLog(LOG_LEVEL_DEBUG, str);
  return str;
}

void RtcInit()
{
    // NTP begin with default parameters:
    //   NTP server: pool.ntp.org
    //   TimeZone: UTC
    //   Daylight saving: off
    NTP.begin (); // Only statement needed to start NTP sync.
    NTP.setTimeZone(Settings.TimeZone);
    NTP.setDayLight(Settings.Daylight);
}
