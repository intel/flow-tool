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
#include <map>
#include <filesystem>
#include "pluginInterface.h"
#include <memory>

class CPluginManager {
    
private:

  CProgramContext *mpCtx;

  struct stPluginInfo {
      std::string name;
      void* pHandleDlopen;
      CPluginInterface* pPlugin;

  };
  //coverity 
  CPluginManager(CPluginManager const&) = delete;
  void operator=(CPluginManager const&) = delete;

  std::map<std::string, std::shared_ptr<stPluginInfo>> mapPluginInfo;
  
  /** helper function for group loading libraries. */
  int loadPlugins(std::string sdirPath);
  
  /** load plugin library */
 int loadPlugin(std::filesystem::path p, std::shared_ptr<stPluginInfo> pstPluginInfo);

  /** unload all loaded plugin libraries */
  int unloadPlugins();
  
  /** Creates plugin instance and add to the map */
  std::string registerPlugin(std::shared_ptr<stPluginInfo> pstPluginInfo);
  
  /** Destroy plugin instance and remove from the map */
 void unregisterPlugin(std::shared_ptr<stPluginInfo> pstPluginInfo);
  

public:
    CPluginManager(CProgramContext *pCtx);
    
    ~CPluginManager();
    
    CPluginInterface* getPluginHandle(std::string pluginName);
    
    //fn to retrun all supported plugins
};
