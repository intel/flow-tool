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

/**
 * <Copyright & License Details>
 *
 * system.cpp
 *
 * Implementation for handling system requests and generate response.
 *
 */
#ifdef __linux__
#include <iostream>
#include <string>
#include <chrono>
#include "metricsinfo.h"


/**
 * get_details : function to get the software information
 * @param: std::string& not used, intended to be used to store detailed return information 
   @return: int status
*/
int SoftwareInfo::get_details(std::string& returnVal) {

    auto start = high_resolution_clock::now();
   	
    _os = "";  
    version = "";
    release = "";      	
    
	std::string swinfo = Helper::runCmd("cat /etc/os-release", 0); 	
	    
    if (!swinfo.empty()){
    	std::istringstream iss(swinfo);
    	std::string line;
    
		bool bOS = false; 
		bool bVersion = false; 
		bool bDesp = false;     
		while (std::getline(iss, line))
		{
			size_t pos = string::npos; 
			if (!line.empty()){					
				if (!bOS ) {
					pos = line.find("NAME=");
					if (pos != string::npos){		
						if (pos != 0)
							continue; 
							
						std::string str_os = line; 
						std::string str = Helper::erase_all(str_os, "NAME=");
						_os = Helper::erase_all(str, "\"");
						Helper::trim(_os, "");
						bOS = true; 
						continue; 
					}
				}
				
				if (!bVersion ){
					pos = line.find("VERSION_ID=");
					if (pos != string::npos){				
						std::string str_version = line; 
						std::string str = Helper::erase_all(str_version, "VERSION_ID=");
						version = Helper::erase_all(str, "\"");
						Helper::trim(version, "");					
						bVersion = true; 
						continue; 
					}
				}
				
				if (!bDesp){
					pos = line.find("VERSION=");
					if (pos != string::npos){				
						std::string str_release = line; 
						std::string str = Helper::erase_all(str_release, "VERSION=");
						release = Helper::erase_all(str, "\"");
						Helper::trim(release, "");
						bDesp = true; 
						continue; 
					}				
				}												   								
			}
			if (bOS && bVersion && bDesp) {
				break;  
			}
		}        
    }              
        
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    
	std::string msg = "discovery_warning: get softwareInfo details duration " + std::to_string(duration.count()) ;

	applog::Log((int)applog::_log_type::warning, msg, _log_level);	        
    
    return 0;
}

#endif
