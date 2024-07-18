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

#include <nlohmann/json.hpp>
using json = nlohmann::json;


#include "applog.h"

class CRequest {
public:
    std::string sId;
    std::string reqtime;
    long lTimeExpired;

    std::string tojsonString();
    int setValues(json jTag);
};

class CDeployRequest {
public:
    std::string sManifestPath;

    std::string tojsonString();
    int setValues(json jTag);
};

/*!
A public class that implements a deployment plugin;
*/    
class CDeploy {
    
public:
    CDeploy();
    ~CDeploy();

    /*! function to get plugin information such as version, description
     *
     * @param[in] std::string message
     * @throws none
     * @return json std::string on success, empty std::string on failure.
     */        
    std::string handleInfo(std::string sReq);
    std::string handleEntry(std::string sReq);
    std::string handleCancel(std::string sReq);

private:
//coverity
   CDeploy(CDeploy const&) = delete;
   void operator=(CDeploy const&) = delete;

	/** log manager */
	applog *pLog = NULL;
        
    /*! function pointer definition */            
    typedef std::string (CDeploy::*fPtr)(std::string);
      
    /*! map of functional pointers and std::string API string */
    std::map<std::string, fPtr> map_string_fPtr;

         
    /*! function to enforce manifest action items.
     * @param[in] std::string message
     * @param[in] integer - continue after reboot
     * @throws none
     * @return json std::string on success, empty std::string on failure.
     */         
    //std::string handleDeployment(std::string message, int continueCounter);
    std::string handleDeployment(std::string sReq);


    /*! function to enforce manifest action items.
     * @param[in] std::string message
     * @throws none
     * @return json std::string on success, empty std::string on failure.
     */         
    //std::string handleDeployment(std::string message, int continueCounter);
    std::string handleValidate(std::string sReq);

    /*! initialization function
     * @return 0 on success.
     */
    int init();
       
    /*! cleanup function */
    void deInit();
   
};
