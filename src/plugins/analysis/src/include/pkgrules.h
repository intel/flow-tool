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


#include <vector>
#include <nlohmann/json.hpp>
#include <map>

class pkgrules
{
   
public:

    pkgrules();
    ~pkgrules();
    
      typedef struct packageContent {
      std::string name;
      std::string version;
      std::string action;
      std::vector<std::string> path;
    } packageContent_t;
    
   
    // preact package list
    std::vector<nlohmann::json> preact = {};
    
    // applicability package list
    std::vector<nlohmann::json> applicability = {};
    
    /*! load the snap files for parsing */ 
    nlohmann::json load_snapfile(std::string snapfile);

    /*! get the diff */
    bool comparefiles(nlohmann::json snapjson, nlohmann::json snapjson2, bool flag);
   /*! return the delta from snap files */
    const std::map<std::string, packageContent_t>& schemajsonOutput() const;
    std::string generateReport(); 

private:

    typedef struct metaData {
      std::string date;
      std::string identifier;
      std::string action;
      std::string version ;
    } packageMetaData_t;

    /*! verify the name match with config file */ 
    packageMetaData_t pltmetainfo;
    
    bool bVerbCheck = false;
    /*! vector to hold the config packages values */
    std::vector<packageContent_t> pltcontent; 
    /*! vector to hold the config packages values */
    std::vector<packageContent_t> refcontent; 

    /*! vector to hold the platform info  */ 
    std::vector<nlohmann::json> platformInfo;
    /*! vector to hold the platform info  */ 
    std::vector<std::string> localpkg;
    
     /*! vector to hold the config packages values */ 
    std::map<std::string, packageContent_t> diffReportMap;

    /*! retrieve the first delta with reference file */ 
    bool getDiffRef();

	/*! retrieve the metadata */
    nlohmann::json readMetadata(nlohmann::json snapjson, std::string file);


  
};

