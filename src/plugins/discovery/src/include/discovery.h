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


#include "statusInfo.h"
#include "metricsinfo.h"
#include "log.h"


//! A public class
/*!
A public class that implements a system service;
see ICapabilityProvider for details of the service.
System implementation details.
*/    
class CDiscovery 
{
    public:
      
    std::string handleInfo(std::string sReq);
    std::string handleEntry(std::string sReq);
    std::string handleCancel(std::string sReq);        

    private:       
                     
        int _log_level {1};                      
               
        //! private variable 
        /*! function pointer */            
        typedef std::string (CDiscovery::*fPtr)(std::string);
        //! private variable 
        /*! map of functional pointers and http_request string */            
        std::map<std::string, fPtr> map_string_fPtr;            
    
                
        //! private function 
        /* function to collect the data using the snap config file     
         * @param[in] http_request message
         * @throws none
         * @return json std::string on success, empty std::string on failure.
         */         
        std::string handleCollect(std::string message);
        
        //! private function 
        /* function to get the status of collecting operation             
         * @param[in] http_request message
         * @throws none
         * @return json std::string on success, empty std::string on failure.
         */        
        std::string handleStatus(std::string message);      
               
        //! private function 
        /* initialization function
         *
         * @param[in] none
         * @throws none
         * @return 0 on success.
         */
        int init(std::string query);

          //! private function 
        /* cleanup function
         *
         * @param[in] none
         * @throws none
         * @return none.
         */
        void deInit();


       	//! private data structure 
        /*! these are the domains system currently supports
         * for enumerate API to quickly respond and mapped the data
         * infomration can help on fuzzy search   
         */ 
        const std::set<std::string> set_metrics {
            {("software")},
            {("memory")},
            {("cpu")},
            {("gpu")},
            {("storage")},
            {("battery")},
            {("network")},
            {("hardware")},
            {("fan")},
            {("sensor")}
        }; 

       	//! private data  
        /*! these are the fuzzy search items system currently supports
         * for quickly finding the items that are mapped to the domain
         * one domain can mapped to multiple search words
         */
        const std::map<std::string, std::string> metricsLookupValues {
            {"software", "software"},
            {"os", "software"},
            {"memory", "memory"},
            {"cpu", "cpu"},
            {"performance", "cpu"},
            {"gpu", "gpu"},
            {"storage", "storage"},
            {"hard drive", "storage"},
            {"drive", "storage"},
            {"battery", "battery"},
            {"power", "battery"},
            {"network", "network"},
            {"wifi", "network"},
            {"hardware", "hardware"},
            {"make", "hardware"},
            {"model", "hardware"},
            {"vendor", "hardware"},
            {"fan", "fan"},
            {"sensor", "sensor"}
        }; 


    };




