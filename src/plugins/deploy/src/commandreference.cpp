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

#include <iostream>

#include "actions.h"
#include "commandreference.h"

#define LOG(msg) \
    std::cout << __FILE__ << "(" << __LINE__ << "): " << msg << std::endl

inline void CCommandReference::logMsg(std::string msg)  {
    std::cout << __FILE__ << "(" << __LINE__ << "): " << msg << std::endl;    
}

CCommandReference::CCommandReference() {
    init("ubuntu", "deb");
}

CCommandReference::~CCommandReference() {
}

CCommandReference::CCommandReference(std::string platform, std::string package_manager) {
    init(platform, package_manager);
}
   
/**
 * @brief Initializes the CCommandReference object.
 * @param platform The platform for which the commands are being initialized.
 * @param package_manager The package manager to be used for the commands.
 * @return 0 on success, non-zero on failure.
 */
int CCommandReference::init(std::string platform, std::string package_manager) {
    //logMsg("CCommandReference::init");

    //overwrite order matters.
    //1. generic linux commands
    //2. linux distro commands
    //3. distro version specific commands.
    //4. developer commands
    
    CPkgActions pkgAction;
    mergeToActiveDict(pkgAction.sDictionary_linux_json);
    mergeToActiveDict(pkgAction.sDictionary_ubuntu_deb_json); //default is ubuntu and deb

    return 0;
}

/**
 * Merges the provided dictionary into the active dictionary.
 *
 * @param sDict_src The JSON string representation of the dictionary to be merged.
 * @return 0 if the merge is successful, 1 otherwise.
 */
int CCommandReference::mergeToActiveDict(std::string sDict_src) {

    try {
        nlohmann::json jsonInDict = nlohmann::json::parse(sDict_src);
        jsonActiveDict.update(jsonInDict);
        jsonActiveDict.merge_patch(jsonInDict);

        //std::cout <<  __FILE__ << "(" << __LINE__ << "):" << jsonActiveDict.dump()  << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}


/**
 * Retrieves the commands associated with a given verb.
 *
 * @param verb The verb for which to retrieve the commands.
 * @return A vector of strings containing the commands associated with the verb.
 */
std::vector<std::string> CCommandReference::getCommandsForVerb(std::string verb) {
    std::vector<std::string> vCommands;
    vCommands.clear();//todo: do we need this?
    try {
        if(jsonActiveDict.contains(verb)) {
            nlohmann::json values = jsonActiveDict[verb];
            int sz = int(values.size());
            for (int i = 0; i < sz; ++i) {
                vCommands.push_back(values[i].get<std::string>());
            }
        } else {
            //std::cout <<  __FILE__ << "(" << __LINE__ << "):" << "Info: No action defined for verb : "  << verb << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return vCommands;
}
