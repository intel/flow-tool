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
#include <vector>
#include "helper.h"

 struct sysinfoData {
    std::string kversion;
    std::string distro;
    std::vector<std::string> distroVersion;
    std::vector<std::string> platform; //not used now
    std::string pkgsets;
    //std::vector<std::string> hardware;
 };
 

/**  class */
class CSysInfo  {

private:
    CSysInfo() = default;
    std::string osVersion; 
    sysinfoData pltinfo;
    std::string available;
    std::string pltName;
    
    bool getAvailableStorage(std::string& diskspace);
    bool getKernelVersion(std::string version);
    bool getDistroInfo(std::string distroName,std::vector<std::string> osdistroversions,std::string pkgset );
    bool getPltInfo(std::vector<std::string> platversions);
    //bool getHWInfo(std::vector<std::string> platHW);
    bool readSnapfile(sysinfoData& osInfo);
    std::string removeSpace(std::string& input);
    std::string extractString(std::string& input);

public:    


  /** info struct initialize*/
  //typedef sysinfoData systeminfoData ;
  
  /** get the info **/
  static bool getSysInfo(const sysinfoData& sysdata  );
};

