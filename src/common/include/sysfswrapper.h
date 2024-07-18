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
#include <fstream>
#include <mutex>
#include <stdio.h>
#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <string.h>
#include <filesystem>
#include "helper.h"
using namespace std;
using namespace std::filesystem;

/**
 * Implementation for wrapper function for access/update sysfs information
 */
class sysfswrapper{

public:
        //! public function 
        /* function to read a long/int value from sysfs
         *
         * @param[in] std::string file path
         * @param[in/out] long the value
         * @return int error code: 0 success, others failure.
         */  
        int readfrompath(std::string sysfspath, long& result);
        //int readfrompath(std::string sysfspath, std::string& result);
        //! public function 
        /* function to read a string value from sysfs
         *
         * @param[in] std::string file path
         * @param[in/out] int error code
         * @return std::string.
         */ 
        std::string readfrompath(std::string sysfspath, int& rescode );   
        //! public function 
        /* function to read a double value from sysfs
         *
         * @param[in] std::string file path
         * @param[in/out] long the value
         * @return int error code: 0 success, others failure.
         */       
        int readfrompath(std::string sysfspath, double& result);

        int filesoffolder(std::string s_Dir, std::vector<std::string>& v_matchingFiles);
        int matchingfilesoffolder(std::string s_Dir, std::string s_filename, std::vector<std::string>& v_matchingFiles);

};




