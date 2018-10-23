/*
 * Log.hpp
 *
 *  Created on: Oct 22, 2018
 *      Author: ipaev
 */

#ifndef COMMON_LOG_HPP_
#define COMMON_LOG_HPP_

void addToLog(byte loglevel, const String& string);
void addToLog(byte logLevel, const __FlashStringHelper* flashString);
void addToLog(byte logLevel, const char ch);
size_t addToLog(byte logLevel, const char *format, ...);
void getLogAll(String& str);

#endif /* COMMON_LOG_HPP_ */
