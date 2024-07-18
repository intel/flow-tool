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

#include <nlohmann/json.hpp>
#include "pkgrules.h"

class CAnalysis  {

private:
   
    /*! class pointer variable to handle config info  */     
    pkgrules *pRules = NULL;
    bool validateFlag = false;
    
    /*! function pointer */
    typedef std::string (CAnalysis::*fPtr)(std::string);
    /*! map of functional pointers and http_request string */  
    std::map<std::string, fPtr> map_string_fPtr;
    

    /** Load from config file if present*/
    bool readPkgRules(std::filesystem::path cfgPath);
    /*! handle API call */ 
    std::string handleValidate(std::string message);
    /*! handle API call */ 
    std::string handleExecute(std::string message);
    /*! load the snap files */ 
    std::string loadSnapfiles(const std::string& filename = "");

    /*! initialization  */ 
    void init();
    /*! handle cleanup */ 
    void deInit() ;
    /*! handle cancel request */ 
    int cancelRequest(int requestID);


public:
  //  Canalysis() = default;
     /*! handle API info for status endpoint */ 
    std::string handleInfo(std::string sReq);
     /*! handle API for entry endpoint */ 
    std::string handleEntry(std::string sReq);
     /** handleCancel*/
    std::string handleCancel(std::string sReq);
   
};

