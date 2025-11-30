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

#include <stdio.h> //tmpnam

#include <unistd.h> //mkstemp
//#include <sys/syslimits.h>
#include <fcntl.h>

#include "include/version.h"
#include <filesystem>
#include <cstdlib> //system
#include <fstream>
#include "statusInfo.h"
#include "helper.h"
#include "apihandlers.h"
#include "metricsinfo.h"
#include <sstream> 
#include <unistd.h>
#include "Config.h"

using namespace std::chrono;

CAPIHandlers::CAPIHandlers(int loglevel) {
    _log_level = loglevel; 
    init();
}

CAPIHandlers::~CAPIHandlers() {
    deinit();
}
   
int CAPIHandlers::init() {
    _bCancel = false;     
              
    return 0;
}

int CAPIHandlers::deinit() {             
    return 0;
}

/**
 * @brief Collects platform information and saves it as a JSON file.
 * 
 * This function collects platform information such as OS details, software components, and hardware details.
 * It saves the collected information as a JSON file with metadata and platform-specific information.
 * 
 * @param bNameOnly A boolean flag indicating whether to return only the filename or the entire JSON content. 
 *                  If true, it returns only the filename. If false, it returns the entire JSON content.
 * @return If bNameOnly is true, it returns the filename of the JSON file. 
 *         If bNameOnly is false, it returns the entire JSON content as a string.
 * @throws std::runtime_error if there is an error in collecting or processing the platform information.
 */
std::string CAPIHandlers::collect(bool bNameOnly) {
  
    Config* pConfig = NULL; 
    try {
  
        CStatusInfo* pStatusInfo = CStatusInfo::getInstance();
        pStatusInfo->clearStatusDetails(); 
        pStatusInfo->setStatus("collect", "running"); 

        std::string str_staticValues;
        SoftwareInfo sfinfo;         
        sfinfo.get_details(str_staticValues); //get the os info
        if (sfinfo._os.empty()){
            std::string errorMessage = std::string("Unable to get the OS information");
            throw std::runtime_error(errorMessage);
        }
        std::string strOS = Helper::to_lower_copy(sfinfo._os);
            
        //this json object will be saved as file 
        json output;

        //read config file
        std::filesystem::path fs_config = std::filesystem::current_path() / "config";
        std::string str_configpath = fs_config.generic_string();    
        std::string filename = str_configpath + "/" + CONFIG_FILE;
        
        json osNode;

        if(std::filesystem::exists(filename)){
            std::cout << "config file path: " << filename << std::endl;        
            std::ifstream ifs(filename); 
            json jr = json::parse(ifs);        
            if (jr.is_null()){
                std::string errorMessage = std::string("Unable to read config file");
                throw std::runtime_error(errorMessage);            
            }
            osNode = jr.at(strOS); 
        } else {  
            pConfig = new Config(); 
            if (pConfig){
                osNode = pConfig->LoadDefaulConfig();
            }                
        }      	        	
       	          
                       	    	    
        if (osNode.is_null()){
            std::string errorMessage = std::string("Unable to get OS specific config info");
            throw std::runtime_error(errorMessage);
        }	    
        
        output[("metadata")][("name")] = "platform-snapshot.json";
        output[("metadata")][("schema")] = "/usr/share/kit/schema/platform-snapshot-schema.json";
        output[("metadata")][("identifier")] = "9743895863498";
        output[("metadata")][("version")] = MICRO_SERVICE_VERSION;        
        output[("metadata")][("desc")] = DESC;
        output[("metadata")][("build")][("date")] = BUILDDATE;
                    	    	       
        //auto nodeobj = osNode;            //nodeobj is node under os name such as: ubuntu: {...}

        for (auto Iter = osNode.begin(); Iter != osNode.end(); Iter++) {
            //node "kernel": [...], "software": [...]
            
            auto itemName = Iter.key(); //kernel, software, hardware     
                  
            auto itemArray = Iter.value();

            if (itemArray.is_array()){ //has to be array contents         
                std::string newkey;
                std::string newvalue; 
                std::string items; 
            
                                 
                for (auto& IterArray : itemArray.items()){
                
                    auto sfcomponents = json::array();                
                    std::string strResult("");

                    json property = IterArray.value();
                    newkey = property["name"];
                    if (!newkey.empty()){
                        pStatusInfo->setStatusDetails("running", newkey);                                        
             
                    newvalue = property["command"];
                    if (!newvalue.empty()){                                          
                        //call popen to execute the command
                        strResult = Helper::runCmd(newvalue, 0);                             
                           
                        //this one needs special handling
                        int i = 0;                        
                        if (itemName == "software"){                        
                            if (!strResult.empty()){ 
                                //std::cout << "parsing software result contents: " << std::endl;       
                        	    std::istringstream iss(strResult);
                        	    std::string line;
                        	    json sfdata; 
                        	    
                                while (std::getline(iss, line)){	                            
                                    size_t pos = std::string::npos; 
                                    if (!line.empty()){		
                                        std::string msg = "each line: " + line; 
    		                            applog::Log((int)applog::_log_type::info, msg, _log_level);     			
	                                    pos = line.find(" ");
					                    if (pos != std::string::npos){

						                    std::string sfname = line.substr(0, pos); 
						                    //std::cout << "sf name: " << sfname << std::endl;      
						                    std::string sfversion = line.substr(pos+1);
						                    msg = "sf version: " + sfversion;      
						                    applog::Log((int)applog::_log_type::info, msg, _log_level);
						                    if (!sfname.empty()){
						                        sfdata[("name")] = sfname;
						                        sfdata[("version")] = sfversion;
						                        sfcomponents[i] = sfdata; 
						                        i++; 
						                    }
					                    }
					                }
					            }

                                    std::string msg = "total software components for this command is " + std::to_string(i); 
                            		applog::Log((int)applog::_log_type::info, msg, _log_level);
                                }
                            }else {
                                std::string str = Helper::erase_all(strResult, "\"");                
                                strResult = Helper::erase_all(str, "\n");   
                                //std::cout << "command result: " << strResult << std::endl;         
                            }                        
                        }
                        
                   if (itemName == "software"){                               
                        output["platform info"][itemName][newkey] = sfcomponents;
                        //to include localInstalls
                        if(property.contains("items")){
                             json itemsList =  property["items"];
                          if (checkLocalInstalls(itemsList)) {
                              output["platform info"][itemName][newkey] = property["items"] ;
                              
                            }
                        }
                    } else {
                         if( newkey == "platform") {
                           strResult = getPltName(strResult);
                        }   
                        output["platform info"][itemName][newkey] = strResult;
                    }                  
                    }  
                }

            } else {
                //for built in config support: have difficulties to nest the loalinstall inside software node
                //localinstall is listed as a seperate node but still output in the software category as it belongs to software list
                //localinstall is not an array
                if (itemName == "localInstall"){
                    if (checkLocalInstalls(itemArray)) {
                        output["platform info"]["software"][itemName] = itemArray; 
                        
                    } else {
                        output["platform info"]["software"][itemName] = {}; 
                    }
                }                             
            }                         
        } 	    
                                            
        std::filesystem::path fspath = std::filesystem::current_path() / OUTPUT_FILE;
        std::string str_fspath = fspath.generic_string();         
        //std::cout << "output path: " << str_fspath << std::endl;                               
        std::ofstream f(str_fspath,std::ios_base::trunc |std::ios_base::out);
        f << std::setw(4) << output.dump();
        f.close(); 
               
        if (pConfig){               
            delete pConfig;
        }
            
        if (bNameOnly){
            return OUTPUT_FILE;
        } else {
            return output.dump();
        }

    } catch (const std::exception &e) {
        if (pConfig){               
            delete pConfig;
        }
        std::string msg = "error executing actions: ";
        msg += e.what(); 
   		applog::Log((int)applog::_log_type::error, msg, _log_level);           
    }
    
    return "";
}

/**
 * @brief Retrieves the platform name from the given result string.
 * @param sResult The result string .
 * @return The platform name extracted from the result string, or "Unknown" if not found.
 */
std::string CAPIHandlers::getPltName(std::string sResult) {             
   std::istringstream f(sResult);
   std::string sVersion("Unknown"); 
   std::string s;
   std::size_t pos = sResult.find("Ultra");

    if (pos != std::string::npos) {
        
        std::size_t index = sResult.find_last_of(" ");
        if (index != std::string::npos && index + 1 < sResult.size()) {
            s = sResult.substr(index + 1);
            sVersion= s[0];
        }
    }
     
  return sVersion;      
}

bool CAPIHandlers::checkLocalInstalls(nlohmann::json& input) {             

  std::string Pname = input["name"];
  char* userName = nullptr;
  bool result = false;

  if( Pname.find("openvino") != std::string::npos)
  {
    std::size_t index = Pname.find_last_of("/");  
    if (index != std::string::npos) {
        std::string name = Pname.substr(index + 1);  
        input[name] = name;
        input.erase("name");
        result = true;
   }
    userName = getenv("USER");
    
    if(userName != nullptr )
    {
     std::string directory = "/home/" + std::string(userName) + "/"+ "gimp";
     if (checkDirExist(directory)) {
         std::size_t index = directory.find_last_of("/"); 
         if (index != std::string::npos) {
            std::string name = directory.substr(index + 1);  
            input[name] = name;
         }
         result = true;
       } 
    } 
  }

  return result;      
}

// Checks if a given directory exists and is a directory
bool CAPIHandlers::checkDirExist(const std::string& directory) {             
   return std::filesystem::exists(directory) && std::filesystem::is_directory(directory); 
}

//for future use
void CAPIHandlers::cancelPackage(){
    _bCancel = true; 
}
