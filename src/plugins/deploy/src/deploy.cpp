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
 * deploy.cpp
 * Implementation for handling API requests and generate response.
 */

#include <fstream>

#include "deploy.h"
#include "apihandlers.h"
#include "deploy_definitions.h"
#include "validator.h"


/**
 * Converts the CRequest object to a JSON string representation.
 * 
 * @return The JSON string representation of the CRequest object.
 */
std::string CRequest::tojsonString() {
    
    json jData;
    if(!sId.empty()) {jData["id"] = sId;}
    std::cout << "deploy: tojsonString " << std::endl;
    return jData.dump();
}

/**
 * @brief Sets the values of the CRequest object based on the provided JSON tag.
 * 
 * @param jTag The JSON tag containing the values to be set.
 * @return 0 if the values are successfully set, otherwise an error code.
 */
int CRequest::setValues(json jTag) {
    
    if(jTag.contains("id")) { sId = jTag.at("id").get<std::string>(); }
    std::cout << "deploy setValues " << std::endl;
    return 0;
}

/***************************************************************/

CDeploy::CDeploy() {
    init();
}

CDeploy::~CDeploy() {
    deInit();
}

/**
 * @brief Initializes the CDeploy object.
 * 
 * @return 0 on success.
 */
int CDeploy::init() {

     //std::cout << "plugin init" << std::endl;

      	if (!pLog) {
            pLog = applog::getInstance();
        }

        std::string msg = "deploy log start";
        pLog->Log(msg, (int)pLog->_log_type::info);

        map_string_fPtr["info"] = &CDeploy::handleInfo;
        map_string_fPtr["deploy"] = &CDeploy::handleDeployment;
        map_string_fPtr["validate"] = &CDeploy::handleValidate;

	return 0;
}

/**
 * @brief Deinitializes the CDeploy plugin.
 * 
 * This function is responsible for cleaning up resources used by the CDeploy plugin.
 * It stops the deploy log and deletes the log object.
 */
void CDeploy::deInit() {
    //std::cout << "plugin deinit" << std::endl;

    if (pLog) {     
        std::string msg = "deploy log stop";
        pLog->Log(msg, (int)pLog->_log_type::info);
        delete pLog;
    }
	return;
}


/**
 * Handles the information request for the deploy operation.
 *
 * @param sReq The request string.
 * @return The response string.
 */
std::string CDeploy::handleInfo(std::string sReq) {
  
    return "deploy";
}

/**
 * Handles the cancellation of a deployment request.
 * 
 * @param sReq The request data in string format.
 * @return The response data in JSON format.
 */
std::string CDeploy::handleCancel(std::string sReq) {
    
    json jsonRequest = json::parse(sReq);
    std::cout << "deploy: handleCancel" << std::endl;
    //todo: publish schema
    CRequest reqInfo;
    if(jsonRequest.contains("id")) { reqInfo.sId = jsonRequest.at("id").get<std::string>(); }
    
    auto response = json::object();
    try{
        response[STATUS] = SUCCESS;
	} catch (const std::exception& e) {
        std::string msg = "Exception : ";
		msg.append(e.what());
        response[STATUS] = FAILURE;
        response[DESCRIPTION] = msg;
	}
    
	return response.dump();
}

/**
 * Handles the entry point for the deploy functionality.
 *
 * @param sReq The request string to be processed.
 * @return A JSON string containing the response.
 */
std::string CDeploy::handleEntry(std::string sReq) {

    std::cout << "handleEntry: " << sReq << std::endl;
    auto response = json::object();
    //todo: publish schema
    try {
        //parse request
        std::string api = "";//, manifestPath = "";
        //int continueCounter = 0;
        json jReq = json::parse(sReq);
        if(jReq.contains("api")) { api = jReq.at("api").get<std::string>(); }

        if(api.compare("deploy") ==0) {
            return handleDeployment(sReq);
        } else if(api.compare("validate") ==0) {
            return handleValidate(sReq);
        }
          else {
            response[STATUS] = FAILURE;
            response[DESCRIPTION] = INVALID_REQUEST;
        }
	} catch (const std::exception& e) {
        std::string msg = "Exception : ";
		msg.append(e.what());
        response[STATUS] = FAILURE;
        response[DESCRIPTION] = msg;
	}

	return response.dump();
    
}

/**
* Enforce actions
* @param[in] manifest file
* @throws none
* @return json std::string on success, empty std::string on failure.
* @return summary of what actions are enforced.         
*/
std::string CDeploy::handleDeployment(std::string sReq) {
    
    auto response = json::object();	
    std::string manifestPath = "";
    int continueCounter = 0;
    json jReq = json::parse(sReq);
    if(jReq.contains("manifest")) { manifestPath = jReq.at("manifest").get<std::string>(); }
    if(jReq.contains("continue")) { continueCounter = jReq.at("continue").get<int>(); }
    
    std::cout << "handleDeployment: " << manifestPath << std::endl;

	if (manifestPath.empty()) {//1 param
		response[STATUS] = FAILURE;
        response[DESCRIPTION] = INVALID_REQUEST;
        return response.dump();
	}
    
    CAPIHandlers *pAPIhandler = NULL; 
    try {
            //std::cout << "manifest path: " << manifestPath << std::endl;
        
            //load manifest
            pAPIhandler = new CAPIHandlers();
            if(pAPIhandler) {
                int retVal = pAPIhandler->startDeploy(manifestPath, continueCounter);
                if(retVal == 0) {
                    response[STATUS] = SUCCESS;
                    
                } else {
                    response[STATUS] = FAILURE;
                    response[DESCRIPTION] = "Internal error";
                    std::filesystem::path currentToolPath = std::filesystem::current_path();
                    std::filesystem::path temperrorfile = currentToolPath / "error.txt";
                    if(!std::filesystem::exists(temperrorfile))
                       std::ofstream output(temperrorfile);

                    //todo: handle error
                }
            } else {
                response[STATUS] = FAILURE;
                 std::filesystem::path currentToolPath = std::filesystem::current_path();
                 std::filesystem::path temperrorfile = currentToolPath / "error.txt";
                 if(!std::filesystem::exists(temperrorfile))
                    std::ofstream output(temperrorfile);
                    
                response[DESCRIPTION] = "Internal error";

            }//todo: handle error
            //std::cout << "update status" << std::endl;
            
	}
	catch (const std::exception& e) {
		std::string msg = "Error getting information ";
		msg.append(e.what());
		response[STATUS] = FAILURE;
        response[DESCRIPTION] = msg;
        
	}
    
    if (pAPIhandler){
        delete pAPIhandler; 
        pAPIhandler = NULL; 
    }
	return response.dump();
}


/**
* validate the schema
*/
std::string CDeploy::handleValidate(std::string message)
{
    auto response = json::object();  
   try {
       std::filesystem::path currentToolPath = std::filesystem::current_path();
       std::filesystem::path manifestFile = currentToolPath / "pkg-manifest.json";

       std::string folderPath = "config/schema/";
       std::string file = "deploy.manifest.schema_v1.json";
       std::filesystem::path schema = currentToolPath / folderPath / file;
       bool status = CValidator::validateSchema(schema, manifestFile);
       if(!status)
       {  
          response[STATUS] = FAILURE;
          response[DESCRIPTION] = INTERNAL_ERROR;
          std::cout << "internal error " << status << std::endl;
       } else
       response[STATUS] = SUCCESS;
   }  
     catch (const std::exception& e) {
	   std::string msg = "failed to validate schem ";
	   msg.append(e.what());
	   response[STATUS] = FAILURE;
           response[DESCRIPTION] = INTERNAL_ERROR;
 
	}
    return response.dump();

}
