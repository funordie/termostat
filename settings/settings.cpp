
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <pgmspace.h>
#include "FS.h"

#include <common.hpp>

#define FILE_CONFIG       "config.dat"

#define LOG_LEVEL_ERROR                     1
#define LOG_LEVEL_INFO                      2
#define LOG_LEVEL_DEBUG                     3
#define LOG_LEVEL_DEBUG_MORE                4
#define LOG_LEVEL_DEBUG_DEV                 9 // use for testing/debugging only, not for regular use
#define LOG_LEVEL_NRELEMENTS                5 // Update this and getLogLevelDisplayString() when new log levels are added

// an abstract class used as a means to proide a unique pointer type
// but really has no body
class __FlashStringHelper;
#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#define F(string_literal) (FPSTR(PSTR(string_literal)))


/*********************************************************************************************\
 * SettingsStruct
\*********************************************************************************************/
struct SettingsStruct
{
  SettingsStruct() {
    clearAll();
  }

  void validate() {
    if (UDPPort > 65535) UDPPort = 0;

    if (Latitude  < -90.0  || Latitude > 90.0) Latitude = 0.0;
    if (Longitude < -180.0 || Longitude > 180.0) Longitude = 0.0;
    if (VariousBits1 > (1 << 30)) VariousBits1 = 0;
  }

  void clearNetworkSettings() {
    for (byte i = 0; i < 4; ++i) {
      IP[i] = 0;
      Gateway[i] = 0;
      Subnet[i] = 0;
      DNS[i] = 0;
    }
  }

  void clearAll() {
    PID = 0;
    Version = 0;
    Build = 0;
    IP_Octet = 0;
    Unit = 0;
    Name[0] = 0;
    NTPHost[0] = 0;
    Delay = 0;
    Pin_i2c_sda = -1;
    Pin_i2c_scl = -1;
    Pin_status_led = -1;
    Pin_sd_cs = -1;
    for (byte i = 0; i < 17; ++i) { PinBootStates[i] = 0; }
    for (byte i = 0; i < 4; ++i) {  Syslog_IP[i] = 0; }
    UDPPort = 0;
    SyslogLevel = 0;
    SerialLogLevel = 0;
    WebLogLevel = 0;
    SDLogLevel = 0;
    BaudRate = 0;
    MessageDelay = 0;
    deepSleep = 0;
    CustomCSS = false;
    DST = false;
    WDI2CAddress = 0;
    UseRules = false;
    UseSerial = false;
    UseSSDP = false;
    UseNTP = false;
    WireClockStretchLimit = 0;
    GlobalSync = false;
    ConnectionFailuresThreshold = 0;
    TimeZone = 0;
    MQTTRetainFlag = false;
    InitSPI = false;
    Pin_status_led_Inversed = false;
    deepSleepOnFail = false;
    UseValueLogger = false;
    ArduinoOTAEnable = false;
    DST_Start = 0;
    DST_End = 0;
    UseRTOSMultitasking = false;
    Pin_Reset = -1;
    StructSize = sizeof(SettingsStruct);
    MQTTUseUnitNameAsClientId = 0;
    Latitude = 0.0;
    Longitude = 0.0;
    VariousBits1 = 0;
    clearNetworkSettings();
  }


  unsigned long PID;
  int           Version;
  int16_t       Build;
  byte          IP[4];
  byte          Gateway[4];
  byte          Subnet[4];
  byte          DNS[4];
  byte          IP_Octet;
  byte          Unit;
  char          Name[26];
  char          NTPHost[64];
  unsigned long Delay;
  int8_t        Pin_i2c_sda;
  int8_t        Pin_i2c_scl;
  int8_t        Pin_status_led;
  int8_t        Pin_sd_cs;
  int8_t        PinBootStates[17];
  byte          Syslog_IP[4];
  unsigned int  UDPPort;
  byte          SyslogLevel;
  byte          SerialLogLevel;
  byte          WebLogLevel;
  byte          SDLogLevel;
  unsigned long BaudRate;
  unsigned long MessageDelay;
  byte          deepSleep;
  boolean       CustomCSS;
  boolean       DST;
  byte          WDI2CAddress;
  boolean       UseRules;
  boolean       UseSerial;
  boolean       UseSSDP;
  boolean       UseNTP;
  unsigned long WireClockStretchLimit;
  boolean       GlobalSync;
  unsigned long ConnectionFailuresThreshold;
  int16_t       TimeZone;
  boolean       MQTTRetainFlag;
  boolean       InitSPI;
//  byte          Protocol[CONTROLLER_MAX];
//  byte          Notification[NOTIFICATION_MAX]; //notifications, point to a NPLUGIN id
//  byte          TaskDeviceNumber[TASKS_MAX];
//  unsigned int  OLD_TaskDeviceID[TASKS_MAX];
//  union {
//    struct {
//      int8_t        TaskDevicePin1[TASKS_MAX];
//      int8_t        TaskDevicePin2[TASKS_MAX];
//      int8_t        TaskDevicePin3[TASKS_MAX];
//      byte          TaskDevicePort[TASKS_MAX];
//    };
//    int8_t        TaskDevicePin[4][TASKS_MAX];
//  };
//  boolean       TaskDevicePin1PullUp[TASKS_MAX];
//  int16_t       TaskDevicePluginConfig[TASKS_MAX][PLUGIN_CONFIGVAR_MAX];
//  boolean       TaskDevicePin1Inversed[TASKS_MAX];
//  float         TaskDevicePluginConfigFloat[TASKS_MAX][PLUGIN_CONFIGFLOATVAR_MAX];
//  long          TaskDevicePluginConfigLong[TASKS_MAX][PLUGIN_CONFIGLONGVAR_MAX];
//  boolean       OLD_TaskDeviceSendData[TASKS_MAX];
//  boolean       TaskDeviceGlobalSync[TASKS_MAX];
//  byte          TaskDeviceDataFeed[TASKS_MAX];    // When set to 0, only read local connected sensorsfeeds
//  unsigned long TaskDeviceTimer[TASKS_MAX];
//  boolean       TaskDeviceEnabled[TASKS_MAX];
//  boolean       ControllerEnabled[CONTROLLER_MAX];
//  boolean       NotificationEnabled[NOTIFICATION_MAX];
//  unsigned int  TaskDeviceID[CONTROLLER_MAX][TASKS_MAX];        // IDX number (mainly used by Domoticz)
//  boolean       TaskDeviceSendData[CONTROLLER_MAX][TASKS_MAX];
  boolean       Pin_status_led_Inversed;
  boolean       deepSleepOnFail;
  boolean       UseValueLogger;
  boolean       ArduinoOTAEnable;
  uint16_t      DST_Start;
  uint16_t      DST_End;
  boolean       UseRTOSMultitasking;
  int8_t        Pin_Reset;
  byte          SyslogFacility;
  uint32_t      StructSize;  // Forced to be 32 bit, to make sure alignment is clear.
  boolean       MQTTUseUnitNameAsClientId;

  //its safe to extend this struct, up to several bytes, default values in config are 0
  //look in misc.ino how config.dat is used because also other stuff is stored in it at different offsets.
  //TODO: document config.dat somewhere here
  float         Latitude;
  float         Longitude;
  uint32_t      VariousBits1;

  // FIXME @TD-er: As discussed in #1292, the CRC for the settings is now disabled.
  // make sure crc is the last value in the struct
  // Try to extend settings to make the checksum 4-byte aligned.
//  uint8_t       ProgmemMd5[16]; // crc of the binary that last saved the struct to file.
//  uint8_t       md5[16];
} Settings;

/********************************************************************************************\
Mount FS and check config.dat
\*********************************************************************************************/
void fileSystemCheck()
{
    //  addLog(LOG_LEVEL_INFO, F("FS   : Mounting..."));
    if (SPIFFS.begin())
    {
        fs::FSInfo fs_info;
        SPIFFS.info(fs_info);
        String log = F("FS   : Mount successful, used ");
        log=log+fs_info.usedBytes;
        log=log+F(" bytes of ");
        log=log+fs_info.totalBytes;
        addToLog(LOG_LEVEL_INFO, log);

        fs::File f = SPIFFS.open(FILE_CONFIG, "r");
        if (!f)
        {
            //				ResetFactory();
        }
        f.close();
    }
    else
    {
        String log = F("FS   : Mount failed");
        addToLog(LOG_LEVEL_ERROR, "log:%s", log.c_str());
        //			ResetFactory();
    }
}

/********************************************************************************************\
  Save settings to SPIFFS
  \*********************************************************************************************/
size_t SaveSettings(void)
{
    size_t res = -1;

    Settings.StructSize = sizeof(struct SettingsStruct);

    Settings.validate();

    fs::File f = SPIFFS.open(FILE_CONFIG, "r");
    if (!f)
    {
        res = f.write((const uint8_t *)&Settings, sizeof(struct SettingsStruct));
        if(!res) {
            addToLog(LOG_LEVEL_ERROR,"write config file error");
        }
    }
    f.close();
    return (res);
}

/********************************************************************************************\
  Load settings from SPIFFS
  \*********************************************************************************************/
int LoadSettings()
{
    size_t res = -1;

    fs::File f = SPIFFS.open(FILE_CONFIG, "r");
    if (!f)
    {
        res = f.read((uint8_t *)&Settings, sizeof(struct SettingsStruct));
        if(!res) {
            addToLog(LOG_LEVEL_ERROR,"read config file error");
        }
    }
    f.close();

    Settings.validate();
    return(res);
}

void settings_setup() {
    addToLog(LOG_LEVEL_ERROR,"settings loop");
}

void settings_loop() {

    addToLog(LOG_LEVEL_ERROR,"settings loop");
}
