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

    /** any communication from framework to plugins */
class CProgramContext {
public:
      std::string version;/*framework*/
      std::string configPath;
      int log_level; // 0[emergency] - 7[debug]
      
      /*std::string tojsonString() {
        return "";
      }*/
};

class CPluginInterface {
public:
#ifdef __linux__
    //Out-of-line virtual desctructor for proper dynamic cast
    // support with older versions of gcc.
    virtual ~CPluginInterface() = default;
#endif
    std::string name;//todo: handle name conflicts
   
#if 0   
  /** any communication from plugins to framework */
  struct stCapabilities {
      std::string name;
      std::string version;
      
      std::string tojsonString() {
        return "";
      }
  };
#endif

  /** called once when plug in loading */
  virtual int init(void* pIn_Context) = 0;  
  virtual void deinit(void* pIn_Context) = 0;
  
  /** called everytime user requests*/
  virtual std::string entry(void* pIn_Context, std::string sRequest) = 0;
  
  /** returns capabilities structure in json format */
  virtual std::string getInfo(void* pIn_Context) = 0;
  
  virtual void cancel(void* pIn_Context, std::string sRequest) = 0;
};

// class factories
typedef CPluginInterface* create_t();
typedef void destroy_t(CPluginInterface*);
