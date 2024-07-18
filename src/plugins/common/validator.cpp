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

#include "validator.h"
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

bool CValidator::validateSchema(const std::string schemaFile, const std::string jsonDataFile) {

    try {
        //open data file
        nlohmann::json jsonDataFileContents;
        std::ifstream dFile(jsonDataFile);
        if (dFile.is_open()) {
            jsonDataFileContents = nlohmann::json::parse(dFile);
            dFile.close();
        } else {
            std::cout << "Error: Invalid data"  << std::endl;
            return false;
        }
       
        //todo:return if file is empty or data is null/not in json format.
        
        //open schema file
        nlohmann::json schemaFileContents;
        std::ifstream sFile(schemaFile);
        if (sFile.is_open()) {
            schemaFileContents = nlohmann::json::parse(sFile);
            sFile.close();
        } else {
            std::cout << "Error: Invalid schema"  << std::endl;
            return false;
        }
               
        //todo:return if file is empty or data is null/not in json format.
        
        nlohmann::json_schema::json_validator validator;
        validator.set_root_schema(schemaFileContents);
        validator.validate(jsonDataFileContents);
        
        //todo: handle validator return value.
        
    } catch (const std::exception& e) {
        std::string msg = "schema - data mismatch";
        msg.append(e.what());
        return false;
    }

    return true;
}
