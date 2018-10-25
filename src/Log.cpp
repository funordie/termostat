/*
 * Log.cpp
 *
 *  Created on: Oct 22, 2018
 *      Author: ipaev
 */

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <pgmspace.h>
#include "FS.h"

#include <common.hpp>

extern Settings_t Settings;

/*********************************************************************************************\
 * LogStruct
\*********************************************************************************************/
#define LOG_STRUCT_MESSAGE_SIZE 128
#define LOG_STRUCT_MESSAGE_LINES 15

String getNumerical(const String& tBuf, bool mustBeInteger) {
  String result = "";
  const unsigned int bufLength = tBuf.length();
  if (bufLength == 0) return result;
  boolean decPt = false;
  int firstDec = 0;
  char c = tBuf.charAt(0);
  if(c == '+' || c == '-') {
    result += c;
    firstDec = 1;
  }
  for(unsigned int x=firstDec; x < bufLength; ++x) {
    c = tBuf.charAt(x);
    if(c == '.') {
      if (mustBeInteger) return result;
      // Only one decimal point allowed
      if(decPt) return result;
      else decPt = true;
    }
    else if(c < '0' || c > '9') return result;
    result += c;
  }
  return result;
}

boolean isNumerical(const String& tBuf, bool mustBeInteger) {
  const unsigned int bufLength = tBuf.length();
  if (bufLength == 0) return false;
  boolean decPt = false;
  int firstDec = 0;
  char c = tBuf.charAt(0);
  if(c == '+' || c == '-')
    firstDec = 1;
  for(unsigned int x=firstDec; x < bufLength; ++x) {
    c = tBuf.charAt(x);
    if(c == '.') {
      if (mustBeInteger) return false;
      // Only one decimal point allowed
      if(decPt) return false;
      else decPt = true;
    }
    else if(c < '0' || c > '9') return false;
  }
  return true;
}

/********************************************************************************************\
  Check if string is valid float
  \*********************************************************************************************/
boolean isFloat(const String& tBuf) {
  return isNumerical(tBuf, false);
}

boolean isValidFloat(float f) {
  if (f == NAN)      return false; //("NaN");
  if (f == INFINITY) return false; //("INFINITY");
  if (-f == INFINITY)return false; //("-INFINITY");
  if (isnan(f))      return false; //("isnan");
  if (isinf(f))      return false; //("isinf");
  return true;
}

boolean isInt(const String& tBuf) {
  return isNumerical(tBuf, true);
}

bool validIntFromString(const String& tBuf, int& result) {
  const String numerical = getNumerical(tBuf, true);
  const bool isvalid = isInt(numerical);
  result = numerical.toInt();
  return isvalid;
}

bool validFloatFromString(const String& tBuf, float& result) {
  const String numerical = getNumerical(tBuf, false);
  const bool isvalid = isFloat(numerical);
  result = numerical.toFloat();
  return isvalid;
}

// convert old and new time string to nr of seconds
float timeStringToSeconds(String tBuf) {
    float sec = 0;
    int split = tBuf.indexOf(':');
    if (split < 0) { // assume only hours
        sec += tBuf.toFloat() * 60 * 60;
    } else {
        sec += tBuf.substring(0, split).toFloat() * 60 * 60;
        tBuf = tBuf.substring(split +1);
        split = tBuf.indexOf(':');
        if (split < 0) { //old format
            sec += tBuf.toFloat() * 60;
        } else { //new format
            sec += tBuf.substring(0, split).toFloat() * 60;
            sec += tBuf.substring(split +1).toFloat();
        }
    }
    return sec;
}

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
  if (value.length() == 0 || !isFloat(value)) {
    if (value.indexOf('\n') == -1 && value.indexOf('"') == -1 && value.indexOf(F("Pragma")) == -1) {
      result += wrap_String(value, F("\""));
    } else {
      String tmpValue(value);
      tmpValue.replace('\n', '^');
      tmpValue.replace('"', '\'');
      tmpValue.replace(F("Pragma"), F("Bugje!"));
      result += wrap_String(tmpValue, F("\""));
    }
  } else {
    result += value;
  }
  return result;
}

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

void addToLog(byte loglevel, const String& string)
{
    addToLog(loglevel, string.c_str());
}

//void addToLog(byte logLevel, const __FlashStringHelper* flashString)
//{
//    String s(flashString);
//    addToLog(logLevel, s.c_str());
//}

void addToLog(byte logLevel, const char ch)
{
    //weblog
    Logging.add(logLevel, String(ch).c_str());
}

size_t addToLog(byte logLevel, const char *format, ...)
{
    if(logLevel <= Settings.logLevel) {
        va_list arg;
        va_start(arg, format);
        char temp[64];
        char* buffer = temp;
        size_t len = vsnprintf(temp, sizeof(temp), format, arg);
        va_end(arg);
        if (len > sizeof(temp) - 1) {
            buffer = new char[len + 1];
            if (!buffer) {
                return 0;
            }
            va_start(arg, format);
            vsnprintf(buffer, len + 1, format, arg);
            va_end(arg);
        }
        buffer[len + 1] = '\n';

        //serial log
        len = Serial.write((const uint8_t*) buffer, len);

        //web log
        Logging.add(logLevel, buffer);

        if (buffer != temp) {
            delete[] buffer;
        }
        return len;
    }
    return 0;
}

String getLog(bool& logLinesAvailable) {
    unsigned long timestamp;
    return Logging.get_logjson_formatted(logLinesAvailable, timestamp);
}

void getLogAll(String& str) {
    str = "";
    Serial.printf("%s: start\n", __FUNCTION__);
    bool logLinesAvailable;
    do{

        String tmp = getLog(logLinesAvailable);
        Serial.printf("%s: getLog return %s\n", __FUNCTION__, tmp.c_str());
        str += tmp + "\n";
    }while(logLinesAvailable);
}
