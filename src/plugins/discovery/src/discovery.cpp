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
 *
 * discovery.cpp
 *
 * Implementation for handling system requests and generate response.
 *
 */

#include "discovery.h"
#include "apihandlers.h"


/**
*  Intialize system extender/metric provider/capability provider/ microservice
*/
int CDiscovery::init(std::string query) {            
 
	map_string_fPtr["collect"] = &CDiscovery::handleCollect; //this is the API for discovery 
	map_string_fPtr[MSG_STATUS] = &CDiscovery::handleStatus;		

	return 0;
}

/**
*  de-intialize system extender/metric provider/capability provider/ microservice
*/
void CDiscovery::deInit() {

    applog::Log((int)applog::_log_type::info, "CDiscovery deinit", _log_level);

	return;
}


std::string CDiscovery::handleInfo(std::string sReq) {
   
    return "discovery";
}

std::string CDiscovery::handleCancel(std::string sReq) {
    
    auto response = json::object();
    try{
        response[MSG_STATUS] = MSG_SUCCESS;
	} catch (const std::exception& e) {
        std::string msg = "Exception : ";
		msg.append(e.what());
        response[MSG_STATUS] = MSG_FAILURE;
        response[MSG_DESC] = msg;
	}
    
	return response.dump();
}


//! private function 
/*! retrive the information of the microservice itself such as version, name, description and what os it intends for*/
std::string CDiscovery::handleStatus(std::string message) {

	auto response = json::object();

	try {
	    CStatusInfo* pStatus = CStatusInfo::getInstance();    
	    //statusinfo.getStatus(); 
		response[MSG_STATUS] = MSG_SUCCESS;
		
		std::string strStatus = pStatus->getStatus("collect");
				
        if (strStatus == "running"){
            std::string strDetails = pStatus->getStatusDetails("running");
            json statusObj;
            statusObj["running"] = strDetails; 
            response["progress"] = statusObj;
        } else {
		    response["progress"] = strStatus;
		}

	}
	catch (const std::exception& e) {
		std::string msg = "discovery_error: error getting operation status";
		msg.append(e.what());

    	applog::Log((int)applog::_log_type::error, msg,  _log_level);
		response[MSG_STATUS] = MSG_FAILURE;
	}

	return response.dump();
}


//! private function 
/*! advertise the capabilties, reserved for future*/
std::string CDiscovery::handleCollect(std::string message) {  
  
    auto response = json::object(); 
    response[MSG_STATUS] = MSG_FAILURE;       
    CAPIHandlers *pAPIhandler = NULL;      
    CStatusInfo* pStatusInfo = CStatusInfo::getInstance();    
	
    try{ 	
        bool bNameOnly = true; 
        json jReq = json::parse(message);
        if(jReq.contains("nameonly")) { bNameOnly = jReq.at("nameonly").get<bool>(); }      
            
        pAPIhandler = new CAPIHandlers();
        if(pAPIhandler) {
            response[MSG_DATA] = pAPIhandler->collect(bNameOnly);            
            pStatusInfo->setStatus(COLLECT_STATUS, "100");                
            response[MSG_STATUS] = MSG_SUCCESS;                  
        }                                             

    } catch (const std::exception &exc) {
        std::cout << "Error: collecting target platform data: " << exc.what() << std::endl;
        pStatusInfo->setStatus(COLLECT_STATUS, "0");                
        response[MSG_STATUS] = MSG_FAILURE;
    }

    //cleanup
    if(pAPIhandler) {
        delete pAPIhandler;
        pAPIhandler = NULL;
        CStatusInfo::deleteInstance();
    }    
    return response.dump();
}


/**
*  the entry point of this libaray
*/
std::string CDiscovery::handleEntry(std::string sReq) {

    std::cout << "discovery handleEntry: " << sReq << std::endl;
    auto response = json::object();
   
    //todo: publish schema
    try {
        //parse request
        std::string api = "";
        json jReq = json::parse(sReq);
        if(jReq.contains("api")) { api = jReq.at("api").get<std::string>(); }
        
        if(api.compare("collect") ==0) {
            return handleCollect(sReq);
        } else if(api.compare("status") ==0) {
            return handleStatus(sReq);
        } else {
            response[MSG_STATUS] = MSG_FAILURE;
            response[MSG_DESC] = MSG_INVALID_REQUEST;
        }
	} catch (const std::exception& e) {
        std::string msg = "Exception : ";
		msg.append(e.what());
        response[MSG_STATUS] = MSG_FAILURE;
        response[MSG_DESC] = msg;
	}        
        
	return response.dump();
}        



