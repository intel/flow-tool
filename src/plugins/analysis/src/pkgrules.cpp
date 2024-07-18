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

#include "iostream"
#include <fstream>

#include "pkgrules.h"


pkgrules::pkgrules() {
}

pkgrules::~pkgrules() {
}

/**
 * load_snapfile  : snap file instances
 */
nlohmann::json pkgrules::load_snapfile(std::string snapfile)
{
    nlohmann::json snapInst;
    try {

        std::ifstream fileHandler;
        fileHandler.open(snapfile);
        
      if (fileHandler.is_open()) {
          fileHandler >> snapInst;
          readMetadata(snapInst, snapfile);
          fileHandler.close();
      } 
   
    } catch (const std::exception& e) {
        std::cout << "analysis error:   "  << e.what() <<  std::endl;
      
    }

    return snapInst;
}

/**
 * readMetadata  : read the content into the vector
 */
nlohmann::json pkgrules::readMetadata(nlohmann::json snapjson, std::string file)
{
    nlohmann::json snapInst;

    try {
         if(file.find("platform") != std::string::npos){
          nlohmann::json meta_info = snapjson["metadata"]; 
          pltmetainfo.date = meta_info["build"]["date"]; 
          pltmetainfo.identifier = meta_info["identifier"]; 
          pltmetainfo.version = meta_info["version"];
         } 
        //load the applicablity data
	 if (snapjson.find("applicability") != snapjson.end()) {
	         nlohmann::json applicabilityInfo = snapjson["applicability"];
            applicability.push_back(applicabilityInfo);

        }

    } catch (const std::exception& e) {
        std::cout << "analysis error:   "  << e.what() <<  std::endl;
      
    }

    return snapInst;
}

/**
 * comparefiles  : compare the snap file instances
 */
bool pkgrules::comparefiles(nlohmann::json snapjson, nlohmann::json snapjson2, bool flag) {  

  refcontent.clear();
  pltcontent.clear(); 
  localpkg.clear();
  diffReportMap.clear();
  bVerbCheck = flag;
 
  try {
       nlohmann::json platform_info = snapjson["platform info"];
       for (const auto& item : platform_info) {
            for (auto it = item.begin(); it != item.end(); ++it) {
              // std::cout << "it.key()  " <<  it.key() << std::endl;
     	         if ((it.key() != "dpkglist") && (it.key() != "snaplist") && (it.key() != "localInstall") ){
                  //check for all the empty values
     	           bool isvalueEmpty = !(it.value().is_null() || 
                            (it.value().is_string() && it.value().get<std::string>().empty()) || 
                            (it.value().is_array() && it.value().empty()) ); /*appimage*/
      
                if (isvalueEmpty) {
                    nlohmann::json info = {
		                {it.key(), it.value()}
		               };
		               platformInfo.push_back(info);
                    }
                   }
	      } 
         } 
          nlohmann::json software_info = platform_info["software"];

          //iterate to read the name and version for dpkg 
          nlohmann::json dpkg_info = software_info["dpkglist"];
           for (const auto& content : dpkg_info) {
               packageContent_t pltPkginfo;
              
               pltPkginfo.name = content["name"];
               pltPkginfo.version = content["version"]; 
	           pltcontent.push_back(pltPkginfo);
            }
        
          //iterate to get the local build data
         nlohmann::json local_info = software_info["localInstall"];
         const auto& locals = software_info["localInstall"];
         for (auto it = locals.begin(); it != locals.end(); ++it) {
            packageContent_t pltPkginfo;
            pltPkginfo.name = it.key();
            pltcontent.push_back(pltPkginfo);
          }

      //read the golden snapshot file  
      for (const auto& package : snapjson2["software"]["localInstall"]) 
      {    
	     packageContent_t goldenRefInfo;
	     
	     goldenRefInfo.name = package["name"];
	     goldenRefInfo.version = package["version"]; 
	      for (const auto& path : package["path"]) {
		       //std::cout << "golden cmpare: " << path << std::endl;
	        	goldenRefInfo.path.push_back(path);
	      }	
        refcontent.push_back(goldenRefInfo);
	  
     }
     
     //load the preact data from golden manifest
     if (snapjson2.find("preact") != snapjson2.end()) {
     
        for (const auto& item : snapjson2["preact"]) {
             nlohmann::json preactInfo;
             preactInfo["desc"] = item["desc"];   
             preactInfo["action"] = item["action"]; 
             preactInfo["param"] = item["param"]; 
             preact.push_back(preactInfo);
        }
     }
     
     //read the snaplist 
     nlohmann::json snap_info = software_info["snaplist"];
     for (const auto& content : snap_info) {
          packageContent_t pltPkginfo;
              
          pltPkginfo.name = content["name"];
          pltPkginfo.version = content["version"]; 
	      pltcontent.push_back(pltPkginfo);
     }
     //read the appimage
     nlohmann::json appimage_info = software_info["appimage"];
     for (const auto& content : appimage_info) {
          packageContent_t pltPkginfo;
              
          pltPkginfo.name = content["name"];
          pltPkginfo.version = content["version"]; 
	      pltcontent.push_back(pltPkginfo);
     }
     //read the goldenconfig dpkglist
      for (const auto& package : snapjson2["software"]["dpkglist"]) 
      {  //  std::cout << "reached here dpkglist golden:" << std::endl;
	 packageContent_t goldenRefInfo;
	     
	 goldenRefInfo.name = package["name"];
	 goldenRefInfo.version = package["version"]; 
         refcontent.push_back(goldenRefInfo);
     }
        //get the snap2 
        nlohmann::json ref_info = snapjson2["platform info"]; 
        nlohmann::json softwareRef_info = ref_info["software"];
  
        //iterate to read the name and version for dpkg 
        nlohmann::json dpkgRef_info = softwareRef_info["dpkglist"];

        for (const auto& content : dpkgRef_info) {
             packageContent_t refPkginfo;
             refPkginfo.name = content["name"];
            //  std::cout << "ref new: " << refPkginfo.name << std::endl;
	           refPkginfo.version = content["version"]; 
	           refcontent.push_back(refPkginfo);
        }

       bool result = getDiffRef();

       if(!result) 
          return false;

    } catch (const std::exception& e) {
       std::cout << "analysis error:   "  << e.what() <<  std::endl;
       return false;
    }

    return true;
}

/**
 *  getDiffRef : Get the diff from the platform snapfile
 */
bool pkgrules::getDiffRef()
{

  try {
      // Iterate first to find the delta only
       for ( packageContent_t& pkgRef : refcontent) {
           bool foundInplt = false;
          //find the name match
       
            for (const packageContent_t& pkgPlt : pltcontent) {
                if (pkgPlt.name == pkgRef.name) {
                foundInplt = true;
                //version not support for goldenconfig
                if(bVerbCheck)
                {
                  pkgRef.action = "found";
                  diffReportMap[pkgRef.name] = { pkgRef.name,pkgRef.version, pkgRef.action };
                   break;
                }
                if (pkgPlt.version == pkgRef.version) {
                    pkgRef.action = "found";
                    diffReportMap[pkgRef.name] = { pkgRef.name,pkgRef.version, pkgRef.action };
                } else if (pkgPlt.version < pkgRef.version) {
                    pkgRef.action = "found:less";
                    diffReportMap[pkgRef.name] = { pkgRef.name,pkgRef.version, pkgRef.action };
                    
                } else {
                    pkgRef.action = "found:greater";
                    diffReportMap[pkgRef.name] = { pkgRef.name,pkgRef.version, pkgRef.action };
               }
                break;
             }
           } 
         // set install flag for all and populate with package info
         if(!foundInplt) {
            
            auto it = diffReportMap.find(pkgRef.name);
            //add it to the report Map
            if (it == diffReportMap.end()) { // 
               pkgRef.action = "not found";
               diffReportMap[pkgRef.name] = { pkgRef.name,pkgRef.version, pkgRef.action };

             }
             
         } 
     }
   }  catch (const std::exception& ex) {
      std::cout << "analysis error:   "  << ex.what() <<  std::endl;
       return false;
   }
   return true;	
}

/**
 * schemajsonOutput  : validate  the schema output
 */
const std::map<std::string, pkgrules::packageContent_t>& pkgrules::schemajsonOutput() const {
   
   if(diffReportMap.empty())
    std::cout << "pkgrules map is empty " << std::endl;
    
   return diffReportMap;

}

/**
 * generateReport  : generate the gap report file
 */
std::string pkgrules::generateReport()
{

    std::string result;
    std::filesystem::path currentToolPath = std::filesystem::current_path();
    std::filesystem::path report = currentToolPath / "gap_report.json";
    std::string reportFile = report.generic_string();    

    nlohmann::json jsonmanifest;
    
    try {
         if(bVerbCheck) {
          //append the applicablies
            for (size_t i = 0; i < applicability.size(); ++i) {
                jsonmanifest["applicability"] = applicability[i];
              //  std::cout << "applicable : " << std::endl;
            }
        } 
        nlohmann::json contentOutput = nlohmann::json::array();
  
       for (const auto &data : diffReportMap) {
              const packageContent_t& pkg = data.second;
              nlohmann::json delta; 
              delta["action"] = pkg.action;
              delta["name"] = pkg.name;
              if (!pkg.path.empty()) {
                // std::cout <<"add the path" << pkg.path << "name is " <<  pkg.name << std::endl;
                // delta["path"] = pkg.path;
              }
            contentOutput.push_back(delta);
       }

      jsonmanifest["desc"] = "Information on the packages that needs to be deployed ";
      jsonmanifest["title"] = "gap_analysis_report.json";
      jsonmanifest["build"]["date"] = pltmetainfo.date;
      jsonmanifest["id"] = pltmetainfo.identifier;
      jsonmanifest["version"] = pltmetainfo.version;
      jsonmanifest["act"] = contentOutput;
    
      //create the file
      std::ofstream outputmanifestFile(reportFile, std::ios::out);
      if (outputmanifestFile.is_open()) {
          outputmanifestFile<< std::setw(1) << jsonmanifest << std::endl;
          outputmanifestFile.close();
          result = "created path: " + reportFile;
      } else {
          result = "failed to create report file " ;
      }

    } catch (const std::exception& ex) {
       std::cout << "analysis error:   "  << ex.what() <<  std::endl;
       return ex.what();
   } 

   return result;

}

