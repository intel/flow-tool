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
#include <string>
#include<iostream> 
#include<algorithm>
#include <regex>
#include <nlohmann/json.hpp>

/** Data structure to store Item details. */
class Item {
public:
    std::string type;
    std::string name;
    std::string cmd;
    std::string version;
};

/** Data structure to store Capability details. */
class CPDetails {
public:
    std::string strName;
    std::string strVersion;
    
    std::string tojson();
    //int parse(std::string jsonstd::string, CPDetails* pCPDetails);
};


/** Helper class - Exposes common APIs used across framework and micro services. */
class Helper
{
public:

    static std::string runCmd(const std::string cmd, int dummy);
    static void trim( std::string& strTotrim, std::string trimChars = ""); 
    static void trimHead( std::string& strTotrim, std::string trimChars = "");  
    static void trimEnd( std::string& strTotrim, std::string trimChars = "");       
    static std::string erase_all( std::string& strOrigin, std::string strToErase);  
    static std::string to_lower_copy(std::string str);
    
};


