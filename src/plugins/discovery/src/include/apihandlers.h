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

#include <iostream>
#include <string>
#include <algorithm>
#include <list>
#include <vector>
#include <map>
#include <queue>
#include "discovery_definitions.h"
#include "log.h"

/*! Class to handle APIs */
class CAPIHandlers {
    
private:

   //coverity 
  CAPIHandlers(CAPIHandlers const&) = delete;
  void operator=(CAPIHandlers const&) = delete;
        
    /** initialize */
    int init();
    
    /** deinitialize */
    int deinit();
    
    /** Log message- inline function*/
    inline void logMsg(std::string msg){
        
        //todo fix inline fuction.
        std::cout << msg << std::endl;
    }
     
    bool _bCancel = false; 
    void cancelPackage();    
    
    int _log_level{1};
    
   /** getPltName: get the platform name 
    *   param: string data
    *   return either the platform name
    */
    std::string getPltName(std::string sResult);
	/** checkLocalInstalls: parse the local build list
    *   param: json 
    *   return bool
    */
    bool checkLocalInstalls(nlohmann::json& input);
   /** checkDirExist: check directory for local build list
    *   param: string 
    *   return bool
    */
    bool checkDirExist(const std::string& directory);
  
public:

    CAPIHandlers(int loglevel=1);
    ~CAPIHandlers();
    
    /** collect the platform information in a json file 
    *   param: bool
    *   return either a file name or file contents
    */
    std::string collect(bool bNameOnly); 
    
};

