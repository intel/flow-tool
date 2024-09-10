

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

#include "analysis.h"
#include "iostream"
#include "validator.h"
#include "definitions.h"
#include <sstream>
#include <nlohmann/json-schema.hpp>
#include <filesystem>

using nlohmann::json;

/** Provider Initialization */
void CAnalysis::init() {
    try{
    	if (NULL == pRules) {
            pRules = new pkgrules(); 
        }
    } catch  (const std::exception& e) {
       
    }

}
/** return plugin information */
std::string CAnalysis::handleInfo(std::string sReq) {
   
    return "analysis";
}


/**
* validate the schema
*/
std::string CAnalysis::handleValidate(std::string message)
{
   
   std::filesystem::path currentToolPath = std::filesystem::current_path();
   std::filesystem::path manifestFile = currentToolPath / "gap_report.json";
   
   std::string folderPath = "config/schema/";
   std::string file = "analysis.manifest.schema_v1.json";
   std::filesystem::path schema = currentToolPath / folderPath / file;

   auto response = json::object();
   std::string result = "success";

   json obj_capabilityList;
   json obj_list_item;
  
  try {
       if ( !std::filesystem::exists(schema) || !std::filesystem::is_regular_file(schema) || std::filesystem::is_symlink(schema) ) {
           result = "analysis: failed to open schema" ;
           response[MSG_STATUS] = MSG_FAILURE;
           response[MSG_DESC] = result;
           return response.dump();
       }
        bool status = CValidator::validateSchema(schema, manifestFile);
        if(status)
        {
          validateFlag = true;
            //since each call is running within it's own instance repopulate the map.
          loadSnapfiles(); 
            //get the diff for dpkg
            const std::map<std::string, pkgrules::packageContent_t>& output = pRules->schemajsonOutput();
           validateFlag = false;
            if(!output.empty()) {
                for (const auto& item : output) {
                    const pkgrules::packageContent_t output = item.second;
                    obj_list_item["name"] = output.name;
                    obj_list_item["version"] = output.version; 
                        //populate all the items
                     obj_capabilityList[obj_capabilityList.size()]= obj_list_item;
                } 
            } else {
                result = "package diff not available" ;
                response[MSG_DESC] = result;
                response[MSG_STATUS] = MSG_FAILURE;
            }
                response["data"] = obj_capabilityList;
                response[MSG_STATUS] = MSG_SUCCESS; 

        } else {
                result = "failed to validate schema" ;
                response[MSG_STATUS] = MSG_FAILURE;
                response[MSG_DESC] = result;
        }
  } catch (const std::exception& e) {
         std::string msg = "analysis Error:  ";
	 msg.append(e.what());
	 response[MSG_STATUS] = MSG_FAILURE;
         response[MSG_DESC] = msg;
    }
    
   deInit();
   return response.dump();
}

/**
* process the snap files and return status
*/
std::string CAnalysis::handleExecute(std::string message) {

 bool bverbCheck = false;
 std::string status = "success";
 auto response = json::object();
 std::string filename = "";

 try {
  
     //POST request
     json jsonObj = json::parse(message);
    //get the post data
    if (jsonObj.contains("post")) {
       filename = jsonObj["post"];
       bverbCheck = true;
       status = loadSnapfiles(filename); 
    }
    if(!bverbCheck)
      status = loadSnapfiles(); 

    if (status.find("failed") != std::string::npos) {
       response[MSG_DESC] = status;
       response[MSG_STATUS] = MSG_FAILURE;
    }
    else {  
      response[MSG_DESC] = status;
      response[MSG_STATUS] = MSG_SUCCESS;
     } 
 } catch (const std::exception& e) {
        std::string msg = "analysis Error:  ";
	msg.append(e.what());
	response[MSG_STATUS] = MSG_FAILURE;
        response[MSG_DESC] = msg;
    }

 return response.dump();
}

/**
 * cancel a current pending request
 */
int CAnalysis::cancelRequest(int requestID) {
    return 0;
}

/**
 * entry point. f
 */
std::string CAnalysis::handleEntry(std::string sReq) {

    auto response = json::object();
   
    try {
       // init();
        //parse request
        std::string api = "";
        json jReq = json::parse(sReq);
        if(jReq.contains("api")) { 
           api = jReq.at("api").get<std::string>();
        }
        if(api.compare("execute") ==0) {
            return handleExecute(sReq);
        }else if(api.compare("validate") ==0) {
            return handleValidate(sReq);
        } 
        else {
            response[MSG_STATUS] = MSG_FAILURE;
            response[MSG_DESC] = MSG_INVALID_REQUEST;
        }
	} catch (const std::exception& e) {
           std::string msg = "analysis Error:  ";
	   msg.append(e.what());
           response[MSG_STATUS] = MSG_FAILURE;
           response[MSG_DESC] = msg;
	}        
        
	return response.dump();
}   

/**
 * deInit: 
 */
void CAnalysis::deInit() {

    if (pRules && !validateFlag) {
         delete pRules;
         pRules = NULL;
    }
 
}

/**
 * loadSnapfiles load the snapfiles
 */
std::string CAnalysis::loadSnapfiles(const std::string& filename)
{
  std::string result = "success"; 
  bool bverbCheck = false;
  std::filesystem::path RefSnapFile;

  try { 
        
        std::filesystem::path currentToolPath = std::filesystem::current_path();

        //verify the platform snap shot was created
        std::filesystem::path PlatSnapFile = currentToolPath / "platform-snapshot.json";
        std::string folderPath = "pkg/";
        std::string file = "golden_snapshot.json";

        if( !filename.empty())
        {
           RefSnapFile = currentToolPath / folderPath / filename;
           bverbCheck = true;
        } else {
           RefSnapFile = currentToolPath / folderPath / file;
        }
       
       // Check if the file exists
       if (!std::filesystem::exists(PlatSnapFile)) {
          return "failed: platform snapshot file doesn't exist";
        }
       if (!std::filesystem::exists(RefSnapFile)) {
           return "failed: reference snapshot file doesn't exist";
        }
        
       init(); 
         // Load the first JSON file
       nlohmann::json snapPlt = pRules->load_snapfile(PlatSnapFile); 
       nlohmann::json snapRef = pRules->load_snapfile(RefSnapFile); 
       
	if(snapPlt.empty() ) {
	   deInit();
	   return "analysis: failed loading platform snapshot file";
	}
        else if(snapRef.empty()) {
           deInit();
           return "analysis: failed loading reference snapshot file";
        }   
        //compare the files
	if (snapPlt == snapRef) {
           result =  "failed: no diff found in the packages";
	} else {
           bool status = pRules->comparefiles(snapPlt,snapRef,bverbCheck);
		   
	   if(status) {
      	      result = pRules->generateReport();
	   }
        }
    } catch (const std::exception &e) {
        std::string msg = "analysis Error:  ";
	msg.append(e.what());
        return msg;
       
    }
    deInit();
     return result;
}

