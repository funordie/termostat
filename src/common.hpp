/*
 * common.hpp
 *
 *  Created on: Oct 13, 2018
 *      Author: ipaev
 */

#ifndef COMMON_COMMON_HPP_
#define COMMON_COMMON_HPP_

#include <Arduino.h>
#include <Log.hpp>

#include "oled.hpp"
#include "gpio.hpp"

enum LoggingLevels {
    LOG_LEVEL_NONE,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_DEBUG_MORE,
    LOG_LEVEL_ALL
};

// -- Time - Up to three NTP servers in your region
#define NTP_SERVER0            "pool.ntp.org"       /* [NtpServer1] Select first NTP server by name or IP address (129.250.35.250)*/
#define NTP_SERVER1            "nl.pool.ntp.org"    /* [NtpServer2] Select second NTP server by name or IP address (5.39.184.5)*/
#define NTP_SERVER2            "0.nl.pool.ntp.org"  /* [NtpServer3] Select third NTP server by name or IP address (93.94.224.67)*/

#define VERSION     1
#define TELE_PERIOD 60          /*seconds*/
#define TIMEZONE +3
#define CHECK_PERIOD 60          /*seconds*/
/*********************************************************************************************\
 * SettingsStruct
\*********************************************************************************************/
typedef struct SettingsStruct
{
    void validate() {
        //TODO:
    }

    SettingsStruct() {
        clearAll();
    }
    void clearAll() {
        Version = VERSION;
        tele_period = TELE_PERIOD;
        strlcpy(ntp_server[0], NTP_SERVER0, strlen(NTP_SERVER0));
        strlcpy(ntp_server[1], NTP_SERVER1, strlen(NTP_SERVER1));
        strlcpy(ntp_server[2], NTP_SERVER2, strlen(NTP_SERVER2));
        TimeZone = TIMEZONE;
        StructSize = sizeof(SettingsStruct);
        logLevel = LOG_LEVEL_INFO;
        check_period = CHECK_PERIOD;
    }

    int           Version;
    uint16_t      tele_period;
    char          ntp_server[3][33];
    int32_t       TimeZone;
    uint32_t      StructSize;  // Forced to be 32 bit, to make sure alignment is clear.
    int           logLevel;
    uint16_t      check_period;

} Settings_t;

#endif /* COMMON_COMMON_HPP_ */
