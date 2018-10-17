
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <pgmspace.h>
#include "FS.h"

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

// Do this in a template to prevent casting to String when not needed.
#define addLog(L,S) { addToLog(L,S); }


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

/*********************************************************************************************\
   Wrap a string with given pre- and postfix string.
  \*********************************************************************************************/
String wrap_String(const String& string, const String& wrap) {
    String result;
    result.reserve(string.length() + 2* wrap.length());
    result = wrap;
    result += string;
    result += wrap;
    return result;
}

/*********************************************************************************************\
   Format an object value pair for use in JSON.
  \*********************************************************************************************/
String to_json_object_value(const String& object, const String& value) {
    String result;
    result.reserve(object.length() + value.length() + 6);
    result = wrap_String(object, F("\""));
    result += F(":");
    if (value.indexOf('\n') == -1 && value.indexOf('"') == -1 && value.indexOf(F("Pragma")) == -1) {
        result += wrap_String(value, F("\""));
    } else {
        String tmpValue(value);
        tmpValue.replace('\n', '^');
        tmpValue.replace('"', '\'');
        tmpValue.replace(F("Pragma"), F("Bugje!"));
        result += wrap_String(tmpValue, F("\""));
    }
    return result;
}

/*********************************************************************************************\
 * LogStruct
\*********************************************************************************************/
#define LOG_STRUCT_MESSAGE_SIZE 128
#ifdef ESP32
#define LOG_STRUCT_MESSAGE_LINES 30
#else
#if defined(PLUGIN_BUILD_TESTING) || defined(PLUGIN_BUILD_DEV)
#define LOG_STRUCT_MESSAGE_LINES 10
#else
#define LOG_STRUCT_MESSAGE_LINES 15
#endif
#endif

struct LogStruct {
    LogStruct() : write_idx(0), read_idx(0) {
        for (int i = 0; i < LOG_STRUCT_MESSAGE_LINES; ++i) {
            Message[i].reserve(LOG_STRUCT_MESSAGE_SIZE);
            timeStamp[i] = 0;
            log_level[i] = 0;
        }
    }

    void add(const byte loglevel, const char *line) {
        write_idx = (write_idx + 1) % LOG_STRUCT_MESSAGE_LINES;
        if (write_idx == read_idx) {
            // Buffer full, move read_idx to overwrite oldest entry.
            read_idx = (read_idx + 1) % LOG_STRUCT_MESSAGE_LINES;
        }
        timeStamp[write_idx] = millis();
        log_level[write_idx] = loglevel;
        unsigned linelength = strlen(line);
        if (linelength > LOG_STRUCT_MESSAGE_SIZE-1)
            linelength = LOG_STRUCT_MESSAGE_SIZE-1;
        Message[write_idx] = "";
        for (unsigned i = 0; i < linelength; ++i) {
            Message[write_idx] += *(line + i);
        }
    }

    // Read the next item and append it to the given string.
    // Returns whether new lines are available.
    bool get(String& output, const String& lineEnd) {
        if (!isEmpty()) {
            read_idx = (read_idx + 1) % LOG_STRUCT_MESSAGE_LINES;
            output += formatLine(read_idx, lineEnd);
        }
        return !isEmpty();
    }

    String get_logjson_formatted(bool& logLinesAvailable, unsigned long& timestamp) {
        logLinesAvailable = false;
        if (isEmpty()) {
            return "";
        }
        read_idx = (read_idx + 1) % LOG_STRUCT_MESSAGE_LINES;
        timestamp = timeStamp[read_idx];
        String output = logjson_formatLine(read_idx);
        if (isEmpty()) return output;
        output += ",\n";
        logLinesAvailable = true;
        return output;
    }

    bool get(String& output, const String& lineEnd, int line) {
        int tmpread((write_idx + 1+line) % LOG_STRUCT_MESSAGE_LINES);
        if (timeStamp[tmpread] != 0) {
            output += formatLine(tmpread, lineEnd);
        }
        return !isEmpty();
    }

    bool getAll(String& output, const String& lineEnd) {
        int tmpread((write_idx + 1) % LOG_STRUCT_MESSAGE_LINES);
        bool someAdded = false;
        while (tmpread != write_idx) {
            if (timeStamp[tmpread] != 0) {
                output += formatLine(tmpread, lineEnd);
                someAdded = true;
            }
            tmpread = (tmpread + 1)% LOG_STRUCT_MESSAGE_LINES;
        }
        return someAdded;
    }

    bool isEmpty() {
        return (write_idx == read_idx);
    }

private:
    String formatLine(int index, const String& lineEnd) {
        String output;
        output += timeStamp[index];
        output += " : ";
        output += Message[index];
        output += lineEnd;
        return output;
    }

    String logjson_formatLine(int index) {
        String output;
        output.reserve(LOG_STRUCT_MESSAGE_SIZE + 64);
        output = "{";
        output += to_json_object_value("timestamp", String(timeStamp[index]));
        output += ",\n";
        output += to_json_object_value("text",  Message[index]);
        output += ",\n";
        output += to_json_object_value("level", String(log_level[index]));
        output += "}";
        return output;
    }


    int write_idx;
    int read_idx;
    unsigned long timeStamp[LOG_STRUCT_MESSAGE_LINES];
    byte log_level[LOG_STRUCT_MESSAGE_LINES];
    String Message[LOG_STRUCT_MESSAGE_LINES];

} Logging;

#include <deque>
std::deque<char> serialLogBuffer;
unsigned long last_serial_log_emptied = 0;

void addToLog(byte loglevel, const String& string)
{
    addToLog(loglevel, string.c_str());
}

void addToLog(byte logLevel, const __FlashStringHelper* flashString)
{
    String s(flashString);
    addToLog(logLevel, s.c_str());
}

void addToLog(byte logLevel, const char *line)
{
    const size_t line_length = strlen(line);
    {
        //Serial log
        int roomLeft = ESP.getFreeHeap() - 5000;
        if (roomLeft > 0) {
            String timestamp_log(millis());
            timestamp_log += F(" : ");
            for (size_t i = 0; i < timestamp_log.length(); ++i) {
                serialLogBuffer.push_back(timestamp_log[i]);
            }
            size_t pos = 0;
            while (pos < line_length && pos < static_cast<size_t>(roomLeft)) {
                serialLogBuffer.push_back(line[pos]);
                ++pos;
            }
            serialLogBuffer.push_back('\r');
            serialLogBuffer.push_back('\n');

        }
        {
            //weblog
            Logging.add(logLevel, line);
        }

#ifdef FEATURE_SD
        if (loglevelActiveFor(LOG_TO_SDCARD, logLevel)) {
            File logFile = SD.open("log.dat", FILE_WRITE);
            if (logFile)
                logFile.println(line);
            logFile.close();
        }
#endif
    }
}
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
        addLog(LOG_LEVEL_INFO, log);

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
        Serial.println(log);
        addLog(LOG_LEVEL_ERROR, log);
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
            addLog(LOG_LEVEL_ERROR,"write config file error");
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
            addLog(LOG_LEVEL_ERROR,"read config file error");
        }
    }
    f.close();

    Settings.validate();
    return(res);
}

/********************************************************************************************\
  Unsigned long Timer timeOut check
\*********************************************************************************************/

// Return the time difference as a signed value, taking into account the timers may overflow.
// Returned timediff is between -24.9 days and +24.9 days.
// Returned value is positive when "next" is after "prev"
long timeDiff(const unsigned long prev, const unsigned long next)
{
    unsigned long start = ESP.getCycleCount();
    long signed_diff = 0;
    // To cast a value to a signed long, the difference may not exceed half the ULONG_MAX
    const unsigned long half_max_unsigned_long = 2147483647u; // = 2^31 -1
    if (next >= prev) {
        const unsigned long diff = next - prev;
        if (diff <= half_max_unsigned_long) {
            // Normal situation, just return the difference.
            // Difference is a positive value.
            signed_diff = static_cast<long>(diff);
        } else {
            // prev has overflow, return a negative difference value
            signed_diff = static_cast<long>((ULONG_MAX - next) + prev + 1u);
            signed_diff = -1 * signed_diff;
        }
    } else {
        // next < prev
        const unsigned long diff = prev - next;
        if (diff <= half_max_unsigned_long) {
            // Normal situation, return a negative difference value
            signed_diff = static_cast<long>(diff);
            signed_diff = -1 * signed_diff;
        } else {
            // next has overflow, return a positive difference value
            signed_diff = static_cast<long>((ULONG_MAX - prev) + next + 1u);
        }
    }
    unsigned long end = ESP.getCycleCount();
    if (end > start) {
        //      ++timediff_calls;
        //      timediff_cpu_cycles_total += (end - start);
    }
    return signed_diff;
}

// Compute the number of milliSeconds passed since timestamp given.
// N.B. value can be negative if the timestamp has not yet been reached.
long timePassedSince(unsigned long timestamp) {
    return timeDiff(timestamp, millis());
}

void process_serialLogBuffer() {
    if (serialLogBuffer.size() == 0) return;
    if (timePassedSince(last_serial_log_emptied) > 10000) {
        last_serial_log_emptied = millis();
        serialLogBuffer.clear();
        return;
    }
    size_t snip = 128; // Some default, ESP32 doesn't have the availableForWrite function yet.

    snip = Serial.availableForWrite();

    if (snip > 0) last_serial_log_emptied = millis();
    size_t bytes_to_write = serialLogBuffer.size();
    if (snip < bytes_to_write) bytes_to_write = snip;
    for (size_t i = 0; i < bytes_to_write; ++i) {
        Serial.write(serialLogBuffer.front());
        serialLogBuffer.pop_front();
    }
}

void settings_setup() {
    addLog(LOG_LEVEL_ERROR,"settings loop");
}

void settings_loop() {

    addLog(LOG_LEVEL_ERROR,"settings loop");
    process_serialLogBuffer();
}
