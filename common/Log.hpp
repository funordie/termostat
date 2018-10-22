/*
 * Log.hpp
 *
 *  Created on: Oct 22, 2018
 *      Author: ipaev
 */

#ifndef COMMON_LOG_HPP_
#define COMMON_LOG_HPP_

void addLog(byte loglevel, const String& string);
void addLog(byte logLevel, const __FlashStringHelper* flashString);
void addLog(byte logLevel, const char *line);

#endif /* COMMON_LOG_HPP_ */
