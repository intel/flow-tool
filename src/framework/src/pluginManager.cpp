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

#include <dlfcn.h>          //load library dynamically
//#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX

#include <filesystem> //current exe folder
#include <system_error> //filesystem exceptions
#include <iostream>

#include "logger.h"
#include "pluginManager.h"

CPluginManager::CPluginManager(CProgramContext *pCtx) {
    mpCtx = pCtx;
    try {
        std::filesystem::path lib_fspath = std::filesystem::current_path() / "lib";
        std::string path_lib = lib_fspath.generic_string();
        PROGRAM_DEBUG(path_lib);
        if(std::filesystem::exists(lib_fspath)) {
            loadPlugins(path_lib);
        } else {
            CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, "lib folder not found in current path");
        }
    } catch (std::filesystem::filesystem_error const& ex) {
        std::cout << "Exception loading from current path " << std::endl;
        std::cout << "what():  " << ex.what() << '\n'
          << "path1(): " << ex.path1() << '\n'
          << "path2(): " << ex.path2() << '\n'
          << "code().value():    " << ex.code().value() << '\n'
          << "code().message():  " << ex.code().message() << '\n'
          << "code().category(): " << ex.code().category().name() << '\n';
    }
}

CPluginManager::~CPluginManager() {
    unloadPlugins();
}

int CPluginManager::loadPlugins(std::string sdirPath) {
    //std::cout << "loadPlugins: " << sdirPath << std::endl;
    PROGRAM_INFO("loadPlugins");
    PROGRAM_INFO(sdirPath);
    
    try {
        for (auto const& p : std::filesystem::directory_iterator(sdirPath)) {
            //std::cout << p.path() << std::endl;
            PROGRAM_INFO(p.path());
          if (p.is_regular_file() && (/*p.path().extension() == ".dll" ||*/ p.path().extension() == ".so")) {
              std::shared_ptr<stPluginInfo> pstPluginInfo = std::make_shared<stPluginInfo>();
            //std::cout <<"load**" << std::endl;
            if(loadPlugin(p.path(), pstPluginInfo) == 0) {
                if(mapPluginInfo.find(pstPluginInfo->name) != mapPluginInfo.end()) {
                    mapPluginInfo[pstPluginInfo->name] = pstPluginInfo;
                } 
            } 
            
          }
        }
    } catch (std::filesystem::filesystem_error const& ex) {
        std::cout << "what():  " << ex.what() << '\n'
                  << "path1(): " << ex.path1() << '\n'
                  << "path2(): " << ex.path2() << '\n'
                  << "code().value():    " << ex.code().value() << '\n'
                  << "code().message():  " << ex.code().message() << '\n'
                  << "code().category(): " << ex.code().category().name() << '\n';
    }catch (...) {
        std::cout << "Exception loadplugins : " << sdirPath << std::endl;
        PROGRAM_ERROR("Exception loadplugins");
    }
    return 0;
}

int CPluginManager::loadPlugin(std::filesystem::path p, std::shared_ptr<stPluginInfo> pstPluginInfo)  {
  
    try {
        CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, "loading PLUGIN");
        PROGRAM_DEBUG(p.string().c_str());
        dlerror();    /* Clear any existing error */

        pstPluginInfo->pHandleDlopen = dlopen (p.string().c_str(), RTLD_LAZY);
        if (pstPluginInfo->pHandleDlopen) {
            CLogger::getInstance().log(PROGRAM_DEBUG_LEVEL, "PLUGIN: Load SUCCESS");
            std::string sInfo = registerPlugin(pstPluginInfo);
            if(!sInfo.empty()) {
                //std::cout << "PLUGIN: Registering plugin " << p << "\n";
                return 0;
            }
            //PROGRAM_DEBUG(sInfo);
        } else {
           const char *dlopn_error = dlerror();
           if (dlopn_error) {
                fprintf(stderr, "Error: %s\n", dlopn_error);
           }
            PROGRAM_ERROR("invalid handle");
        }
    } catch (...) {
        PROGRAM_ERROR("Exception loading plugin");
    }   
   
    PROGRAM_ERROR("Failure loading plugin");

    return 1;
}

int CPluginManager::unloadPlugins() {
    PROGRAM_DEBUG("unloadPlugins");
    try {
        for (auto & item : mapPluginInfo) {
            std::shared_ptr<stPluginInfo> pstPluginInfo = item.second;
            if(pstPluginInfo) {
                unregisterPlugin(pstPluginInfo);
            }
        }
        mapPluginInfo.clear();
    } catch (...) {
        PROGRAM_ERROR("Exception unloading plugin");
    }   
    return 0;
}

/** Creates plugin instance and add to the map */
std::string CPluginManager::registerPlugin(std::shared_ptr<stPluginInfo> pstPluginInfo) {
    PROGRAM_DEBUG("registering plugin");
    std::string sInfo = "";
    try {
        dlerror();    /* Clear any existing error */
       
        create_t* create_plugin_factory = (create_t*) dlsym(pstPluginInfo->pHandleDlopen, "create");
        const char* dlsym_error = dlerror();
        if (dlsym_error) {
            PROGRAM_ERROR("Cannot load symbol create:");
            PROGRAM_ERROR(dlsym_error);
        } else {
            pstPluginInfo->pPlugin = create_plugin_factory();
            if(pstPluginInfo->pPlugin) {
                sInfo = pstPluginInfo->pPlugin->getInfo((void*)mpCtx);
                if(mapPluginInfo.find(sInfo) == mapPluginInfo.end()) {
                    mapPluginInfo[sInfo] = pstPluginInfo;
                    PROGRAM_DEBUG("plugin registration successful");
                    PROGRAM_DEBUG(sInfo);
                } else {
                    //plugin already registered
                    sInfo = "";
                    unregisterPlugin(pstPluginInfo);
                }
            }
        }
    } catch (...) {
        PROGRAM_ERROR("Exception registering plugin");
    }
    
    return sInfo;
}

void CPluginManager::unregisterPlugin(std::shared_ptr<stPluginInfo> pstPluginInfo) {
    PROGRAM_DEBUG("unregistering plugin");
    try {
        dlerror();    /* Clear any existing error */
                  
        destroy_t* destroy_plugin_factory = (destroy_t*) dlsym(pstPluginInfo->pHandleDlopen, "destroy");
        const char* dlsym_error = dlerror();
        if (dlsym_error) {
            std::cout << "Cannot load symbol destroy: " << dlsym_error << '\n';
        } else {
            destroy_plugin_factory(pstPluginInfo->pPlugin);
            dlclose(pstPluginInfo->pHandleDlopen);
        }
        
    } catch (...) {
        //std::cout << "exception unregistering plugin " << std::endl;
        PROGRAM_ERROR("Exception unregistering plugin");
    }
    
    return;
}

/** return handle to plugin class instance */
CPluginInterface* CPluginManager::getPluginHandle(std::string sPluginName) {
    for (auto & item : mapPluginInfo) {
        std::string sName = item.first;
        if(sPluginName.compare(sName) ==0 ) {
          std::shared_ptr<stPluginInfo> pstPluginInfo = item.second;
            if(pstPluginInfo) {
                return pstPluginInfo->pPlugin;
            }
        }
    }
    return NULL;
}
