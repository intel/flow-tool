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

#pragma once

#include <string>
#include <stdio.h>

//todo: change CAppLog
class applog {
public:
    
    enum _log_type {
        error = 1,
        warning = 2,
        info = 3,
    };
    
    enum _log_level {
        release = 1,
        debug= 2        
    };    
    

    //logType: error, info, warning
    //if the log level is release, only error and warning shows
    //if the log level is debug, show everything, error, info, warning
    static inline void Log(int logType, std::string logmessage, int loglevel=1) {
	    if ((int)_log_level::debug == loglevel){
		    std::cout << logmessage << std::endl; 	
	    } else {
		    if (logType == (int)_log_type::error || logType == (int)_log_type::warning) {
			    std::cout << logmessage << std::endl; 
		    }
	    }
    }    
        
};
