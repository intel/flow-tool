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
#include <map>
#include <stdio.h>
#include <string>

/*!
A private inner class that implements a status info
*/
class CStatusInfo {
        
private:
    
    /*! single instance */
	static CStatusInfo* instance;
    	
    /*! map of API and it's status */
    std::map<std::string, std::string> map_status; 	
     
    /*! map of API and it's status details */
    std::map<std::string, std::string> map_statusDetails;
    
    /*! constructor */
    CStatusInfo() {
    }
    
public:
    ~CStatusInfo() { 
     
    }
   
   //coverity 
  CStatusInfo(CStatusInfo const&) = delete;
  void operator=(CStatusInfo const&) = delete;

	static CStatusInfo* getInstance() {
        if (!instance) {
            instance = new CStatusInfo();
        }
        
        return(instance);
    }
    
    static void deleteInstance() {
      if(instance) {
        
         delete instance; 
         instance = nullptr;
      } 
    }
	                 
    //! Public function
    /*! get the status of the operation such as running, completed, none */    
    std::string getStatus(std::string strAPI);
    
    /*! get the status details e.g. when it's running, the details can specify which stage it is running on 
      currently only running has details */    
    std::string getStatusDetails(std::string strStatus);
    
    //! Public function
    /*! set the status of the API*/    
    int setStatus(std::string strAPI, std::string strStatus);    
    
    //! Public function
    /*! set the status of the API*/    
    int setStatusDetails(std::string strStatus, std::string strDetails);      
    
    void clearAllStatus(); 
    
    void clearStatus(); 
    
    void clearStatusDetails();
};
