/*
 * Copyright (c) 2024, Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 * http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * 
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <set>
#include <iostream>
#include "applog.h"

applog* applog::instance = NULL;

void applog::setLogInfo(bool bLogDebug) {
	_blogInfo = bLogDebug; 
}

//logType: error, info, warning
//if the log level is release, only error and warning shows
//if the log level is debug, show everything, error, info, warning
void applog::Log(std::string logmessage, int logType) {
	if (_blogInfo){
		std::cout << logmessage << std::endl; 	
	} else {
		if (logType == (int)_log_type::error || logType == (int)_log_type::warning) {
			std::cout << logmessage << std::endl; 
		}
	}
}
