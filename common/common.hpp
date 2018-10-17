/*
 * common.hpp
 *
 *  Created on: Oct 13, 2018
 *      Author: ipaev
 */

#ifndef COMMON_COMMON_HPP_
#define COMMON_COMMON_HPP_

#include "gpio_define.hpp"
int limit_execution_time_sec(long * pValue, long time_sec);

enum LoggingLevels {LOG_LEVEL_NONE, LOG_LEVEL_ERROR, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG_MORE, LOG_LEVEL_ALL};
#define D_LOG_APPLICATION 	"APP: "  // Application
#define D_UPLOAD_ERR_2 		"Not enough space"

#endif /* COMMON_COMMON_HPP_ */
