/*
 * Log.hpp
 *
 *  Created on: Oct 22, 2018
 *      Author: ipaev
 */

#ifndef COMMON_LOG_HPP_
#define COMMON_LOG_HPP_

void addToLog(byte loglevel, const String& string);
void addToLog(byte logLevel, const char ch);
void addToLog(byte logLevel, const char *format...);

void addToLogEx(byte loglevel, const String& string);
void addToLogEx(byte logLevel, const char ch);
void addToLogEx(byte logLevel, const char *format, ...);
void getLogAll(String& str);

#endif /* COMMON_LOG_HPP_ */
